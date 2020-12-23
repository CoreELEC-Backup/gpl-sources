/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2005 Joakim Eriksson <je@plane9.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "types.h"
#include <chrono>

/***************************** D E F I N E S *******************************/
/****************************** M A C R O S ********************************/
/************************** S T R U C T U R E S ****************************/

////////////////////////////////////////////////////////////////////////////
//
class CTimer
{
public:
  CTimer();
  void Init(void);
  void Update(void);
  f32 GetDeltaTime(void);

protected:
  double m_OldCount;
  f32 m_DeltaTime;

  static double WallTime ()
  {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(now).count() / 1.0e6;
  }
};

/***************************** G L O B A L S *******************************/
/***************************** I N L I N E S *******************************/

////////////////////////////////////////////////////////////////////////////
//
inline CTimer::CTimer()
{
  m_DeltaTime = 0.0f;
}

////////////////////////////////////////////////////////////////////////////
//
inline void CTimer::Init(void)
{
  m_OldCount = WallTime();
}

////////////////////////////////////////////////////////////////////////////
//
inline void CTimer::Update(void)
{
  m_DeltaTime = WallTime()-m_OldCount;
  m_OldCount = WallTime();
}

////////////////////////////////////////////////////////////////////////////
//
inline f32 CTimer::GetDeltaTime(void)
{
  return m_DeltaTime;
}
