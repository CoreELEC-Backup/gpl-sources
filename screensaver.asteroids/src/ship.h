/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2005 Joakim Eriksson <je@plane9.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

/***************************** D E F I N E S *******************************/

#define SHIPLINES    4
#define BULLETDELAY  0.25
#define WARPDELAY    30.0    // If we dont hit anything in this time we autowarp

/****************************** M A C R O S ********************************/
/***************************** C L A S S E S *******************************/

////////////////////////////////////////////////////////////////////////////
//
class CShip
{
public:
  CShip();
  ~CShip();
  void Update(f32 dt);
  void Draw(CMyAddon* render);
  CVector2 GetDirVec();
  CVector2 GetTangDirVec();
  bool CanFire();

  CVector2    m_Pos;
  CVector2    m_Vel;

  f32 m_WarpDelay;
  f32 m_Rot;
  f32 m_Speed;

  f32 m_Size;
  f32 m_BulletDelay;      // Cooldown time for the gun

  CVector2 m_Lines[SHIPLINES][2];
};

/***************************** I N L I N E S *******************************/
