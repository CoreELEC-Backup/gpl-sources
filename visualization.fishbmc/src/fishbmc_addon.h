/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *  Copyright (C) 2012 Marcel Ebmer
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "fische.h"

#include <kodi/addon-instance/Visualization.h>
#include <kodi/gui/gl/Shader.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/type_ptr.hpp>

struct sPosition
{
  sPosition() : x(0.0f), y(0.0f), z(1.0f), u(1.0f) {}
  sPosition(float* d) : x(d[0]), y(d[1]), z(d[2]), u(1.0f) {}
  sPosition(float x, float y, float z = 0.0f) : x(x), y(y), z(z), u(1.0f) {}
  float x,y,z,u;
};

struct sCoord
{
  sCoord() : s(0.0f), t(0.0f) {}
  sCoord(float s, float t) : s(s), t(t) {}
  float s,t;
};

class ATTRIBUTE_HIDDEN CVisualizationFishBMC
  : public kodi::addon::CAddonBase,
    public kodi::addon::CInstanceVisualization,
    public kodi::gui::gl::CShaderProgram
{
public:
  CVisualizationFishBMC();
  ~CVisualizationFishBMC() override;

  ADDON_STATUS GetStatus() override;
  bool Start(int channels, int samplesPerSec, int bitsPerSample, std::string songName) override;
  void Stop() override;
  void Render() override;
  void AudioData(const float* audioData, int audioDataLength, float *freqData, int freqDataLength) override;
  ADDON_STATUS SetSetting(const std::string& settingName, const kodi::CSettingValue& settingValue) override;

  void OnCompiledAndLinked() override;
  bool OnEnabled() override;

private:
  void start_render();
  void finish_render();
  void textured_quad (float center_x,
                      float center_y,
                      float angle,
                      float axis,
                      float width,
                      float height,
                      float tex_left,
                      float tex_right,
                      float tex_top,
                      float tex_bottom);
  static void on_beat(void* handler, double frames_per_beat);
  static void write_vectors(void* handler, const void* data, size_t bytes);
  static size_t read_vectors(void* handler, void** data);
  void delete_vectors();

  bool m_startOK = false;
  bool m_shaderLoaded = false;

  glm::mat4 m_projMatrix;
  glm::mat4 m_modelMatrix;

  sPosition m_vertex[4];
  sCoord m_coord[4];
  GLuint m_indexer[4] = {0, 1, 3, 2};

  GLint m_uProjMatrixLoc = -1;
  GLint m_uModelViewMatrixLoc = -1;
  GLint m_aVertexLoc = -1;
  GLint m_aCoordLoc = -1;

  GLuint m_vertexVBO[2] = {0};
  GLuint m_indexVBO = 0;
  GLuint m_texture = 0;

  FISCHE* m_fische = nullptr;
  float m_aspect;
  bool m_isrotating;
  float m_angle;
  float m_lastangle;
  bool m_errorstate;
  int m_framedivisor;
  float m_angleincrement;
  float m_texright;
  float m_texleft;
  bool m_filemode;
  int m_size;
  uint8_t* m_axis = nullptr;
};

