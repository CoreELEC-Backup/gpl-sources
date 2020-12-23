#include "Bot.hpp"
#include "common.hpp"
#include "mrboom.h"
#include "GridFunctions.hpp"
#include <algorithm>

#define IFTRACES    ((debugTracesPlayer(_playerIndex)) && (traceMask & DEBUG_MASK_GRIDS))

Bot::Bot(int playerIndex)
{
   _playerIndex = playerIndex;
   initBot();
}

void Bot::initBot()
{
   calculatedBestCellToDropABomb   = 0;
   calculatedBestCellToPickUpBonus = 0;
   for (int j = 0; j < grid_size_y; j++)
   {
      for (int i = 0; i < grid_size_x; i++)
      {
         bestExplosionsGrid[i][j] = 0;
         noDangerGrid[i][j]       = false;
      }
   }
   _direction1FrameAgo  = button_error;
   _direction2FramesAgo = button_error;
   _shiveringCounter    = 0;
}

bool Bot::cellSafe(int cell)
{
   int cellX = CELLX(cell);
   int cellY = CELLY(cell);

   return(!dangerGrid[cellX][cellY] && !flameGrid[cellX][cellY] && !monsterIsComingGrid[cell]);
}

int Bot::bestBonusCell()
{
   if (!isAboutToWin() && travelGrid.cost(calculatedBestCellToPickUpBonus) != TRAVELCOST_CANTGO &&
       cellSafe(calculatedBestCellToPickUpBonus))
   {
      return(calculatedBestCellToPickUpBonus);
   }
   else
   {
      return(-1);
   }
}

int Bot::scoreForBonus(Bonus bonus, int x, int y)
{
   if (!bonusPlayerWouldLike(_playerIndex, bonus))
   {
      if (((bonus == bonus_remote) || (bonus == bonus_egg) || (bonus == bonus_roller)) && (isPlayerFastestToCell(_playerIndex, x, y)))
      {
         if (isSameTeamTwoFastestToCell(x, y))
         {
            if (tracesDecisions(_playerIndex))
            {
               log_debug("%d we are the fastest 2 to bonus %d (%d/%d) ignoring\n", _playerIndex, bonus, x, y);
            }
            return(0);
         }
         else
         {
            if (tracesDecisions(_playerIndex))
            {
               log_debug("%d should pick bonus %d (%d/%d) for safety reason\n", _playerIndex, bonus, x, y);
            }
         }
      }
   }
   int distance = travelSafeGrid.cost(x, y);
   if (distance == TRAVELCOST_CANTGO)
   {
      return(0);
   }
   switch (bonus)
   {
   case bonus_push:
   case bonus_remote:
   case bonus_bulletproofjacket:
      distance /= 4;
      break;

   case bonus_egg:
   case bonus_heart:
   case bonus_roller:
      distance /= 8;
      break;

   default:
      break;
   }
   if (distance < 100)
   {
      return(TRAVELCOST_CANTGO - distance);
   }
   else
   {
      if (isPlayerFastestToCell(_playerIndex, x, y))
      {
         return(TRAVELCOST_CANTGO - distance);
      }
   }
   return(0);
}

uint8_t Bot::calculateBestCellToPickUpBonus()
{
   int bestCell  = -1;
   int bestScore = 0;

   for (int j = 0; j < grid_size_y; j++)
   {
      for (int i = 0; i < grid_size_x; i++)
      {
         Bonus bonus = bonusInCell(i, j);
         if (bonus != no_bonus)
         {
            int score = scoreForBonus(bonus, i, j);
            if (score > bestScore)
            {
               int cellIndex = CELLINDEX(i, j);
               bestCell  = cellIndex;
               bestScore = score;
            }
         }
      }
   }
   if (tracesDecisions(_playerIndex))
   {
      log_debug("BOTTREEDECISIONS/calculateBestCellToPickUpBonus: %d/%d:bestCell=%d bestScore=%d\n", frameNumber(), _playerIndex, bestCell, bestScore);
   }
   return(bestCell);
}

int noise(int player, int x, int y)
{
   return((x + player + y) % nb_dyna);
}

int Bot::bestCellToDropABomb()
{
   int bestCell  = -1;
   int bestScore = 0;

   for (int j = 0; j < grid_size_y; j++)
   {
      for (int i = 0; i < grid_size_x; i++)
      {
         int score = bestExplosionsGrid[i][j] * 128;
         if (score < 0)
         {
            score = 0;
         }
         if (score)
         {
            score += noise(_playerIndex, i, j);
         }
         int travelCost = 1 + travelGrid.cost(i, j) / 16;
         if (score > travelCost)
         {
            score = score / travelCost;
         }

         if (score > bestScore)
         {
            bestCell  = CELLINDEX(i, j);
            bestScore = score;
         }
      }
   }
   return(bestCell);
}

int Bot::bestSafeCell()
{
   int bestCell  = cellPlayer(_playerIndex);
   int bestScore = 0;

   for (int j = 0; j < grid_size_y; j++)
   {
      for (int i = 0; i < grid_size_x; i++)
      {
         if (!somethingThatIsNoTABombAndThatWouldStopPlayer(i, j))
         {
            int score = TRAVELCOST_CANTGO - travelGrid.cost(i, j);
            if (bestExplosionsGrid[i][j])
            {
               score += TRAVELCOST_CANTGO;
            }
            if ((score > bestScore) && cellSafe(CELLINDEX(i, j)))
            {
               int cellIndex = CELLINDEX(i, j);
               bestCell  = cellIndex;
               bestScore = score;
            }
         }
      }
   }
   return(bestCell);
}

#define MAX_PIXELS_PER_FRAME    8



bool Bot::isSomewhatInTheMiddleOfCell()
{
   int x = GETXPIXELSTOCENTEROFCELL(_playerIndex);
   int y = GETYPIXELSTOCENTEROFCELL(_playerIndex);

   return((x > -MAX_PIXELS_PER_FRAME / 2) && (x < MAX_PIXELS_PER_FRAME / 2) && (y < MAX_PIXELS_PER_FRAME / 2) && (y > -MAX_PIXELS_PER_FRAME / 2));
}

bool Bot::amISafe()
{
   return(cellSafe(cellPlayer(_playerIndex)));
}

bool Bot::isThereABombUnderMe()
{
   int x = xPlayer(_playerIndex);
   int y = yPlayer(_playerIndex);

   return(bombInCell(x, y));
}

int Bot::howManyBombsLeft()
{
   return(nbBombsLeft(_playerIndex));
}

void Bot::printGrid()
{
   if (IFTRACES)
   {
      for (int j = 0; j < grid_size_y; j++)
      {
         for (int i = 0; i < grid_size_x; i++)
         {
            db brickKind = m.truc[i + j * grid_size_x_with_padding];
            if (monsterInCell(i, j) || playerInCell(i, j))
            {
               if (monsterInCell(i, j))
               {
                  log_debug("   8(   ");
               }
               else
               {
                  log_debug("   8)   ");
               }
            }
            else
            {
               switch (brickKind)
               {
               case 1:
                  log_debug("[======]");
                  break;

               case 2:
                  log_debug("(******)");
                  break;

               default:
                  if (no_bonus != bonusInCell(i, j))
                  {
                     log_debug("   (%d)  ", bonusInCell(i, j));
                  }
                  else
                  {
                     log_debug("        ");
                  }
                  break;
               }
            }
         }
         log_debug("\n");
      }
      log_debug("bestExplosionsGrid player %d\n", _playerIndex);
      for (int j = 0; j < grid_size_y; j++)
      {
         for (int i = 0; i < grid_size_x; i++)
         {
            log_debug("%04d", bestExplosionsGrid[i][j]);
            if (dangerGrid[i][j])
            {
               log_debug("x");
            }
            else
            {
               log_debug("_");
            }
         }
         log_debug("\n");
      }
      log_debug("travelCostGrid %d/%d cell:%d x:%d y:%d adderX=%d adderY=%d\n", frameNumber(), _playerIndex, cellPlayer(_playerIndex), xPlayer(_playerIndex), yPlayer(_playerIndex), GETXPIXELSTOCENTEROFCELL(_playerIndex) * framesToCrossACell(_playerIndex) / CELLPIXELSSIZE,
                GETYPIXELSTOCENTEROFCELL(_playerIndex) * framesToCrossACell(_playerIndex) / CELLPIXELSSIZE);
      travelGrid.print();

      log_debug("travelCostSafeGrid\n");
      travelSafeGrid.print();

      log_debug("%d flameGrid player %d\n", m.changement, _playerIndex);
      for (int j = 0; j < grid_size_y; j++)
      {
         for (int i = 0; i < grid_size_x; i++)
         {
            log_debug("%04d ", flameGrid[i][j]);
         }
         log_debug("\n");
      }
      log_debug("hasRemote=%d isCellCulDeSac=%d flamesize:%d lapipipino:%d lapipipino5:%d amISafe=%d\n", hasRemote(_playerIndex), isCellCulDeSac(xPlayer(_playerIndex), yPlayer(_playerIndex)), flameSize(_playerIndex), m.lapipipino[_playerIndex], m.lapipipino5[_playerIndex], amISafe());
   }
}

void Bot::stopWalking()
{
   mrboom_update_input(button_up, _playerIndex, 0, true);
   mrboom_update_input(button_down, _playerIndex, 0, true);
   mrboom_update_input(button_left, _playerIndex, 0, true);
   mrboom_update_input(button_right, _playerIndex, 0, true);
}

void Bot::startPushingRemoteButton()
{
   mrboom_update_input(button_a, _playerIndex, 1, true);
}

void Bot::stopPushingRemoteButton()
{
   mrboom_update_input(button_a, _playerIndex, 0, true);
}

void Bot::startPushingJumpButton()
{
   mrboom_update_input(button_x, _playerIndex, 1, true);
}

void Bot::stopPushingJumpButton()
{
   mrboom_update_input(button_x, _playerIndex, 0, true);
}

void Bot::startPushingBombDropButton()
{
   pushingDropBombButton = true;
   mrboom_update_input(button_b, _playerIndex, 1, true);
}

void Bot::stopPushingBombDropButton()
{
   pushingDropBombButton = false;
   mrboom_update_input(button_b, _playerIndex, 0, true);
}

#ifdef DEBUG
extern int howToGoDebug;
#endif
bool Bot::walkToCell(int cell)
{
        #ifdef DEBUG
   howToGoDebug = 0;
        #endif
   bool        shouldJump = false;
   enum Button direction  = howToGo(_playerIndex, CELLX(cell), CELLY(cell), travelSafeGrid, shouldJump);
   if (direction == button_error)
   {
      direction = howToGo(_playerIndex, CELLX(cell), CELLY(cell), travelGrid, shouldJump);
   }

        #ifdef DEBUG
   walkingToCell[_playerIndex] = cell;
   if (tracesDecisions(_playerIndex))
   {
      char *directionText = (char *)"?";
      switch (direction)
      {
      case button_up:
         directionText = (char *)"button_up";
         break;

      case button_down:
         directionText = (char *)"button_down";
         break;

      case button_left:
         directionText = (char *)"button_left";
         break;

      case button_right:
         directionText = (char *)"button_right";
         break;

      default:
         break;
      }
      log_debug("\nBOTTREEDECISIONS: %d/%d:howToGo to %d:%s %d/%d shouldjump=%d\n", frameNumber(), _playerIndex, cell, directionText, GETXPIXELSTOCENTEROFCELL(_playerIndex), GETYPIXELSTOCENTEROFCELL(_playerIndex), shouldJump);
   }
        #endif
   stopWalking();

   if (shouldJump)
   {
      startPushingJumpButton();
   }

   if (hasInvertedDisease(_playerIndex))
   {
      switch (direction)
      {
      case button_up:
         direction = button_down;
         break;

      case button_down:
         direction = button_up;
         break;

      case button_left:
         direction = button_right;
         break;

      case button_right:
         direction = button_left;
         break;

      default:
         break;
      }
   }

   mrboom_update_input(direction, _playerIndex, 1, true);

#define MAX_SHIVERING    3
   if ((_direction2FramesAgo == direction) && (_direction1FrameAgo != direction))
   {
      _shiveringCounter++;
      if (_shiveringCounter >= MAX_SHIVERING)
      {
         if (tracesDecisions(_playerIndex))
         {
            log_debug("BOTTREEDECISIONS/shivering on bot: %d/%d ->startPushingBombDropButton\n", frameNumber(), _playerIndex);
         }
         startPushingBombDropButton();
      }
      if (_shiveringCounter >= MAX_SHIVERING * 2)
      {
         if (tracesDecisions(_playerIndex))
         {
            log_debug("BOTTREEDECISIONS/shivering on bot: %d/%d ->startPushingJumpButton\n", frameNumber(), _playerIndex);
         }
         startPushingJumpButton();
      }
      if (_shiveringCounter >= MAX_SHIVERING * 3)
      {
         if (tracesDecisions(_playerIndex))
         {
            log_debug("BOTTREEDECISIONS/shivering on bot: %d/%d ->startPushingRemoteButton\n", frameNumber(), _playerIndex);
         }
         startPushingRemoteButton();
      }
   }
   else
   {
      _shiveringCounter = 0;
   }
   _direction2FramesAgo = _direction1FrameAgo;
   _direction1FrameAgo  = direction;

   return(direction != button_error);
}

int Bot::getCurrentCell()
{
   int x = xPlayer(_playerIndex);
   int y = yPlayer(_playerIndex);

   return(CELLINDEX(x, y));
}

void Bot::printCellInfo(int cell)
{
   log_debug("printCellInfoBot Cell:%d Bot:%d: travelCostGrid=%d bestExplosionsGrid=%d  flameGrid=%d dangerGrid=%d\n", cell, _playerIndex, travelGrid.cost(cell), bestExplosionsGrid[CELLX(cell)][CELLY(cell)], flameGrid[CELLX(cell)][CELLY(cell)], dangerGrid[CELLX(cell)][CELLY(cell)]);
}
