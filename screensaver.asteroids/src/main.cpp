/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2005 Joakim Eriksson <je@plane9.com>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "main.h"
#include "Asteroids.h"
#include "timer.h"

#include <time.h>

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
const BYTE PixelShader[] =
{
     68,  88,  66,  67,  18, 124,
    182,  35,  30, 142, 196, 211,
     95, 130,  91, 204,  99,  13,
    249,   8,   1,   0,   0,   0,
    124,   1,   0,   0,   4,   0,
      0,   0,  48,   0,   0,   0,
    124,   0,   0,   0, 188,   0,
      0,   0,  72,   1,   0,   0,
     65, 111, 110,  57,  68,   0,
      0,   0,  68,   0,   0,   0,
      0,   2, 255, 255,  32,   0,
      0,   0,  36,   0,   0,   0,
      0,   0,  36,   0,   0,   0,
     36,   0,   0,   0,  36,   0,
      0,   0,  36,   0,   0,   0,
     36,   0,   0,   2, 255, 255,
     31,   0,   0,   2,   0,   0,
      0, 128,   0,   0,  15, 176,
      1,   0,   0,   2,   0,   8,
     15, 128,   0,   0, 228, 176,
    255, 255,   0,   0,  83,  72,
     68,  82,  56,   0,   0,   0,
     64,   0,   0,   0,  14,   0,
      0,   0,  98,  16,   0,   3,
    242,  16,  16,   0,   1,   0,
      0,   0, 101,   0,   0,   3,
    242,  32,  16,   0,   0,   0,
      0,   0,  54,   0,   0,   5,
    242,  32,  16,   0,   0,   0,
      0,   0,  70,  30,  16,   0,
      1,   0,   0,   0,  62,   0,
      0,   1,  73,  83,  71,  78,
    132,   0,   0,   0,   4,   0,
      0,   0,   8,   0,   0,   0,
    104,   0,   0,   0,   0,   0,
      0,   0,   1,   0,   0,   0,
      3,   0,   0,   0,   0,   0,
      0,   0,  15,   0,   0,   0,
    116,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      3,   0,   0,   0,   1,   0,
      0,   0,  15,  15,   0,   0,
    122,   0,   0,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      3,   0,   0,   0,   2,   0,
      0,   0,   3,   0,   0,   0,
    122,   0,   0,   0,   1,   0,
      0,   0,   0,   0,   0,   0,
      3,   0,   0,   0,   2,   0,
      0,   0,  12,   0,   0,   0,
     83,  86,  95,  80,  79,  83,
     73,  84,  73,  79,  78,   0,
     67,  79,  76,  79,  82,   0,
     84,  69,  88,  67,  79,  79,
     82,  68,   0, 171,  79,  83,
     71,  78,  44,   0,   0,   0,
      1,   0,   0,   0,   8,   0,
      0,   0,  32,   0,   0,   0,
      0,   0,   0,   0,   0,   0,
      0,   0,   3,   0,   0,   0,
      0,   0,   0,   0,  15,   0,
      0,   0,  83,  86,  95,  84,
     65,  82,  71,  69,  84,   0,
    171, 171
};
#endif

CMyAddon::CMyAddon()
  : m_asteroids(nullptr),
    m_timer(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////
// Kodi tells us we should get ready to start rendering. This function
// is called once when the screensaver is activated by Kodi.
//
bool CMyAddon::Start()
{
  m_NumLines = 0;
  m_Verts = nullptr;

#ifndef WIN32
  m_projMat = glm::ortho(0.0f, float(Width()), float(Height()), 0.0f);
  
  std::string fraqShader = kodi::GetAddonPath("resources/shaders/" GL_TYPE_STRING "/frag.glsl");
  std::string vertShader = kodi::GetAddonPath("resources/shaders/" GL_TYPE_STRING "/vert.glsl");
  if (!LoadShaderFiles(vertShader, fraqShader) || !CompileAndLink())
    return false;

  glGenBuffers(1, &m_vertexVBO);

  m_VertBuf = new TRenderVertex[10000];
  m_Verts = m_VertBuf;
#else
  m_pContext = reinterpret_cast<ID3D11DeviceContext*>(Device());
  ID3D11Device* pDevice = nullptr;
  m_pContext->GetDevice(&pDevice);

  CD3D11_BUFFER_DESC vbDesc(sizeof(TRenderVertex) * (NUMLINES * 2 + 2 /*safe*/), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
  pDevice->CreateBuffer(&vbDesc, nullptr, &m_pVBuffer);
  pDevice->CreatePixelShader(PixelShader, sizeof(PixelShader), nullptr, &m_pPShader);
  SAFE_RELEASE(pDevice);
#endif

  m_Width = Width();
  m_Height = Height();

  srand((u32)time(nullptr));
  m_asteroids = new CAsteroids(this);
  if (!m_asteroids)
    return false;

  m_timer = new CTimer();
  m_timer->Init();
  if (!m_asteroids->RestoreDevice())
  {
    Stop();
    return false;
  }

  return true;
}

////////////////////////////////////////////////////////////////////////////
// Kodi tells us to render a frame of our screensaver. This is called on
// each frame render in Kodi, you should render a single frame only - the DX
// device will already have been cleared.
//
void CMyAddon::Render()
{
  if (!m_asteroids)
    return;

#ifndef WIN32
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
#endif

  Begin();
  m_timer->Update();
  m_asteroids->Update(m_timer->GetDeltaTime());
  m_asteroids->Draw();
  Draw();
}

////////////////////////////////////////////////////////////////////////////
// Kodi tells us to stop the screensaver we should free any memory and release
// any resources we have created.
//
void CMyAddon::Stop()
{
  if (!m_asteroids)
    return;

#ifdef WIN32
  SAFE_RELEASE(m_pPShader);
  SAFE_RELEASE(m_pVBuffer);
#endif

  m_asteroids->InvalidateDevice();
  SAFE_DELETE(m_asteroids);
  SAFE_DELETE(m_timer);

#ifndef WIN32
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glDeleteBuffers(1, &m_vertexVBO);
  m_vertexVBO = 0;
  
  delete m_VertBuf;
  m_VertBuf = nullptr;
#endif
}

////////////////////////////////////////////////////////////////////////////
//
bool CMyAddon::Begin()
{
#ifdef WIN32
  D3D11_MAPPED_SUBRESOURCE res = {};
  if (SUCCEEDED(m_pContext->Map(m_pVBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &res)))
    m_Verts = (TRenderVertex*)res.pData;
#endif
  return true;
}

////////////////////////////////////////////////////////////////////////////
//
bool CMyAddon::Draw()
{
  if (m_NumLines == 0)
    return true;

#ifndef WIN32
  glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(TRenderVertex)*m_NumLines * 2, m_VertBuf, GL_STATIC_DRAW);

  glVertexAttribPointer(m_aPosition, 3, GL_FLOAT, GL_FALSE, sizeof(TRenderVertex), BUFFER_OFFSET(offsetof(TRenderVertex, pos)));
  glEnableVertexAttribArray(m_aPosition);

  glVertexAttribPointer(m_aColor, 4, GL_FLOAT, GL_FALSE, sizeof(TRenderVertex), BUFFER_OFFSET(offsetof(TRenderVertex, col)));
  glEnableVertexAttribArray(m_aColor);

  // render
  EnableShader();
  glDrawArrays(GL_LINES, 0, m_NumLines * 2);
  DisableShader();

  m_Verts = m_VertBuf;
#else
  m_pContext->Unmap(m_pVBuffer, 0);
  m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
  UINT strides = sizeof(TRenderVertex), offsets = 0;
  m_pContext->IASetVertexBuffers(0, 1, &m_pVBuffer, &strides, &offsets);
  m_pContext->PSSetShader(m_pPShader, nullptr, 0);
  m_pContext->Draw(m_NumLines * 2, 0);
  Begin();
#endif
  m_NumLines = 0;
  return true;
}

////////////////////////////////////////////////////////////////////////////
//
void CMyAddon::DrawLine(const CVector2& pos1, const CVector2& pos2, const glm::vec4& col1, const glm::vec4& col2)
{
  if (m_NumLines >= NUMLINES)
  {
    Draw();
  }

  m_Verts->pos = glm::vec3(pos1.x, pos1.y, 0.0f);
  m_Verts->col = col1;
  m_Verts++;

  m_Verts->pos = glm::vec3(pos2.x, pos2.y, 0.0f);
  m_Verts->col = col2;
  m_Verts++;

  m_NumLines++;
}

#ifndef WIN32
void CMyAddon::OnCompiledAndLinked()
{
  m_uProjMatrix = glGetUniformLocation(ProgramHandle(), "u_modelViewProjectionMatrix");
  m_aPosition = glGetAttribLocation(ProgramHandle(), "a_position");
  m_aColor = glGetAttribLocation(ProgramHandle(), "a_color");
}

bool CMyAddon::OnEnabled()
{
  glUniformMatrix4fv(m_uProjMatrix, 1, GL_FALSE, glm::value_ptr(m_projMat));
  return true;
}
#endif

ADDONCREATOR(CMyAddon);
