/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 *
 *  PrBoom: a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2001 by
 *  Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze
 *  Copyright 2005, 2006 by
 *  Florian Schulze, Colin Phipps, Neil Stevens, Andrey Budko
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 *  02111-1307, USA.
 *
 * DESCRIPTION:
 *      Handles in-memory caching of WAD lumps
 *
 *-----------------------------------------------------------------------------
 */

// use config.h if autoconf made one -- josh
#include "config.h"

#include "doomstat.h"
#include "doomtype.h"

#include "w_wad.h"
#include "z_zone.h"
#include "lprintf.h"

static struct {
  void *cache;
  unsigned int locks;
} *cachelump;

/* W_InitCache
 *
 * cph 2001/07/07 - split from W_Init
 */
void W_InitCache(void)
{
  // set up caching
  cachelump = calloc(sizeof *cachelump, numlumps);
  if (!cachelump)
    I_Error ("W_Init: Couldn't allocate lumpcache");
}

void W_DoneCache(void)
{
   if (cachelump)
      free(cachelump);
}

/* W_CacheLumpNum
 * killough 4/25/98: simplified
 * CPhipps - modified for new lump locking scheme
 *           returns a const*
 */

const void *W_CacheLumpNum(int lump)
{
  const int locks = 1;

  if (!cachelump[lump].cache)      // read the lump in
    W_ReadLump(lump, Z_Malloc(W_LumpLength(lump), PU_CACHE, &cachelump[lump].cache));

  /* cph - if wasn't locked but now is, tell z_zone to hold it */
  if (!cachelump[lump].locks && locks) {
    Z_ChangeTag(cachelump[lump].cache,PU_STATIC);
  }
  cachelump[lump].locks += locks;

  return cachelump[lump].cache;
}

const void *W_LockLumpNum(int lump)
{
  return W_CacheLumpNum(lump);
}

/*
 * W_UnlockLumpNum
 *
 * CPhipps - this changes (should reduce) the number of locks on a lump
 */

void W_UnlockLumpNum(int lump)
{
  const int unlocks = 1;

  // invalid lump, ignore unlock
  if (lump < 0) return;

  cachelump[lump].locks -= unlocks;
  /* cph - Note: must only tell z_zone to make purgeable if currently locked,
   * else it might already have been purged
   */
  if (unlocks && !cachelump[lump].locks)
    Z_ChangeTag(cachelump[lump].cache, PU_CACHE);
}

