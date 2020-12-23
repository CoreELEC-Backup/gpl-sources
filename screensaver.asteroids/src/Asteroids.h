/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2005 Joakim Eriksson <je@plane9.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "ship.h"
#include "asteroid.h"
#include "bullet.h"

/***************************** D E F I N E S *******************************/

#define NUMASTEROIDFRAGMENTS  3
#define NUMASTEROIDS          (10*NUMASTEROIDFRAGMENTS)
#define NUMBULLETS            10
#define MAXLEVELTIME          (5*60)      // Max time before we reset the whole level

/****************************** M A C R O S ********************************/
/***************************** C L A S S E S *******************************/

////////////////////////////////////////////////////////////////////////////
//
class ATTRIBUTE_HIDDEN CAsteroids
{
public:
  CAsteroids(CMyAddon* addon);
  ~CAsteroids();
  bool RestoreDevice();
  void InvalidateDevice();
  void Update(f32 dt);
  bool Draw();

protected:
  CShip        m_Ship;
  CBullet      m_Bullets[NUMBULLETS];
  CAsteroid    m_Asteroids[NUMASTEROIDS];
  f32          m_LevelTime;

  CBullet* NewBullet();
  CAsteroid* NewAsteroid();
  void Init();
  void Warp();
  void ShipAI(f32 dt);
  void PerformCollisions();

private:
  CMyAddon* m_addon;
};

/***************************** I N L I N E S *******************************/
