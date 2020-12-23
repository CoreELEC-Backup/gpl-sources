/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2005 Joakim Eriksson <je@plane9.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "main.h"
#include "bullet.h"

////////////////////////////////////////////////////////////////////////////
//
CBullet::CBullet()
{
  m_State = BS_NONE;
  m_Pos.Zero();
  m_Vel.Zero();
  m_Size = 2.0f;
}

////////////////////////////////////////////////////////////////////////////
//
CBullet::~CBullet()
{
}

////////////////////////////////////////////////////////////////////////////
//
void CBullet::Fire(const CVector2& pos, const CVector2& vel)
{
  m_State = BS_ACTIVE;
  m_Pos = pos;
  m_Vel = vel;
}

////////////////////////////////////////////////////////////////////////////
//
 void CBullet::Update(f32 dt, int width, int height)
{
  if (m_State != BS_ACTIVE)
    return;
  m_Pos += m_Vel*dt;

  // Removed if moved outside screen
  if ((m_Pos.x < 0.0f) || (m_Pos.x > width)  || (m_Pos.y < 0.0f) || (m_Pos.y > height))
    m_State = BS_NONE;
}

////////////////////////////////////////////////////////////////////////////
//
void CBullet::Draw(CMyAddon* render)
{
  if (m_State != BS_ACTIVE)
    return;

  glm::vec4 col(1.0f, 1.0f, 1.0f, 1.0f);
  render->DrawLine(m_Pos, m_Pos+Normalized(m_Vel)*m_Size, col, col);
}
