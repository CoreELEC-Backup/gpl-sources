/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2005 Joakim Eriksson <je@plane9.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/addon-instance/Screensaver.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifndef WIN32
#include <kodi/gui/gl/GL.h>
#include <kodi/gui/gl/Shader.h>
#else
#include <d3d11.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "types.h"

/***************************** D E F I N E S *******************************/

#define NUMLINES    100

/****************************** M A C R O S ********************************/
/***************************** C L A S S E S *******************************/

typedef struct TRenderVertex
{
  glm::vec3 pos;
  glm::vec4 col;
} TRenderVertex;

class CAsteroids;
class CTimer;

class ATTRIBUTE_HIDDEN CMyAddon
  : public kodi::addon::CAddonBase,
    public kodi::addon::CInstanceScreensaver
#ifndef WIN32
  , public kodi::gui::gl::CShaderProgram
#endif
{
public:
  CMyAddon();

  bool Start() override;
  void Stop() override;
  void Render() override;

#ifndef WIN32
  void OnCompiledAndLinked() override;
  bool OnEnabled() override;
#endif

  bool Begin();
  bool Draw();
  void DrawLine(const CVector2& pos1, const CVector2& pos2, const glm::vec4& col1, const glm::vec4& col2);

private:
  s32 m_NumLines;
  int m_Width;
  int m_Height;

  TRenderVertex* m_Verts;
#ifndef WIN32
  TRenderVertex* m_VertBuf;

  glm::mat4 m_projMat;
  GLuint m_vertexVBO = 0;

  GLint m_uProjMatrix = -1;
  GLint m_aPosition = -1;
  GLint m_aColor = -1;
#else
  ID3D11DeviceContext* m_pContext;
  ID3D11Buffer*        m_pVBuffer;
  ID3D11PixelShader*   m_pPShader;
#endif

  CAsteroids* m_asteroids;
  CTimer* m_timer;
};

/***************************** I N L I N E S *******************************/
