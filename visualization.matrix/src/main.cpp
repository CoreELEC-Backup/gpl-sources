/*
 *      Copyright (C) 2005-2019 Team Kodi
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "main.h"

#include <regex>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_JPEG
#define STBI_ONLY_PNG
#define STBI_ONLY_BMP
#include "stb_image.h"
#include "kodi/Filesystem.h"
#include "kodi/General.h"



#define _USE_MATH_DEFINES
#include <algorithm>
#include <chrono>
#include <math.h>

#define SMOOTHING_TIME_CONSTANT (0.5) // default 0.8
#define MIN_DECIBELS (-100.0)
#define MAX_DECIBELS (-30.0)

#define AUDIO_BUFFER (1024)
#define NUM_BANDS (AUDIO_BUFFER / 2)

// Override GL_RED if not present with GL_LUMINANCE, e.g. on Android GLES
#ifndef GL_RED
#define GL_RED GL_LUMINANCE
#endif

struct Preset
{
  std::string name;
  uint32_t labelId;
  std::string file;
  int channel[4];
};

// NOTE: With "#if defined(HAS_GL)" the use of some shaders is avoided
//       as they can cause problems on weaker systems.
const std::vector<Preset> g_presets =
{
   {"Kodi",                         30100, "kodi.frag.glsl",        99,  0,  1, -1},
   {"Album",                        30101, "album.frag.glsl",       99, -1,  1,  2},
   {"Rain only",                    30102, "nologo.frag.glsl",      99, -1,  1, -1},
   {"Rain with waveform",           30103, "nologowf.frag.glsl",    99, -1,  1, -1},
   {"Rain with waveform envelope",  30104, "nologowfenv.frag.glsl", 99, -1,  1, -1},
   {"Clean",                        30105, "clean.frag.glsl",       99, -1, -1, -1},
   {"Clean with waveform",          30106, "cleanwf.frag.glsl",     99, -1, -1, -1},
   {"Clean with waveform envelope", 30107, "cleanwfenv.frag.glsl",  99, -1, -1, -1},
};

const std::vector<std::string> g_fileTextures =
{
  "logo.png",
  "noise.png",
};

std::string fsCommonFunctionsLowPower = 
R"functions(float h11(float p)
{
  return fract(fract(p * .1031) * (p + 33.33));
}

float waveform(vec2 uv)
{
  float wave = texture(iChannel0,vec2(uv.x*.15+.5,0.75)).x - .5;
  return min(abs(uv.y*20.+wave*10.),0.5);
}

#ifdef dNoise
float noise(vec2 gv)
{
	//return texture(iChannel2, vec2(gl_FragCoord.xy/(256.*iDotSize) +iTime*cNoiseFluctuation)).x;
	return texture(iChannel2, vec2(gl_FragCoord.xy/(256.*iDotSize))).x;
}
#endif

vec3 bw2col(float bw, vec2 uv)
{
  float d = length(fract(uv*cColumns)-.5);
  float peakcolor = .6-d;
  float basecolor = .8-d;
  return (basecolor*cColor+peakcolor)*bw;
}

)functions";

std::string fsCommonFunctionsNormal = 
R"functions(float h11(float p)
{
  return fract(20.12345+sin(p*RNDSEED1)*RNDSEED2);
}

float waveform(vec2 uv)
{
  float wave = texture(iChannel0,vec2(uv.x*.15+.5,0.75)).x*.5 + uv.y;
  return abs(smoothstep(.225,.275,wave) -.5);
}

#ifdef dNoise
float noise(vec2 gv)
{
	//return texture(iChannel2, (gv/cResolution*iDotSize*400.33) + iTime*cNoiseFluctuation).x;
  return texture(iChannel2, (gv*.035431) + iTime*cNoiseFluctuation).x;
}
#endif

vec3 bw2col(float bw, vec2 uv)
{
  float d = length(fract(uv*cColumns)-.5);
  float peakcolor = smoothstep(.35,.0,d)*bw;
  float basecolor = smoothstep(.85,.0,d)*bw;
  return basecolor*cColor+peakcolor;
}

)functions";

CVisualizationMatrix::CVisualizationMatrix()
  : m_kissCfg(kiss_fft_alloc(AUDIO_BUFFER, 0, nullptr, nullptr)),
    m_audioData(new GLubyte[AUDIO_BUFFER]()),
    m_magnitudeBuffer(new float[NUM_BANDS]()),
    m_pcm(new float[AUDIO_BUFFER]())
{
  m_currentPreset = kodi::GetSettingInt("lastpresetidx");
  m_dotSize = static_cast<float>(kodi::GetSettingInt("dotsize"));
  m_fallSpeed = static_cast<float>(kodi::GetSettingInt("fallspeed")) * .01;
  m_distortThreshold = static_cast<float>(kodi::GetSettingInt("distortthreshold")) * .005;
  m_dotColor.red = static_cast<float>(kodi::GetSettingInt("red")) / 255.f;
  m_dotColor.green = static_cast<float>(kodi::GetSettingInt("green")) / 255.f;
  m_dotColor.blue = static_cast<float>(kodi::GetSettingInt("blue")) / 255.f;
  m_lowpower = kodi::GetSettingBoolean("lowpower");
  m_noiseFluctuation = m_lowpower ? (static_cast<float>(kodi::GetSettingInt("noisefluctuation")) * 0.0002f)/m_fallSpeed * 0.25f : (static_cast<float>(kodi::GetSettingInt("noisefluctuation")) * 0.0004f)/m_fallSpeed * 0.25f;
  m_lastAlbumChange = 0.0;
}

CVisualizationMatrix::~CVisualizationMatrix()
{
  delete [] m_audioData;
  delete [] m_magnitudeBuffer;
  delete [] m_pcm;
  free(m_kissCfg);
}

//-- Render -------------------------------------------------------------------
// Called once per frame. Do all rendering here.
//-----------------------------------------------------------------------------
void CVisualizationMatrix::Render()
{
  if (m_initialized)
  {
    RenderTo(m_matrixShader.ProgramHandle(), 0);
  }
}

bool CVisualizationMatrix::Start(int iChannels, int iSamplesPerSec, int iBitsPerSample, std::string szSongName)
{
  kodi::Log(ADDON_LOG_DEBUG, "Start %i %i %i %s\n", iChannels, iSamplesPerSec, iBitsPerSample, szSongName.c_str());

  //background vertex
  static const GLfloat vertex_data[] =
  {
    -1.0, 1.0, 1.0, 1.0,
     1.0, 1.0, 1.0, 1.0,
     1.0,-1.0, 1.0, 1.0,
    -1.0,-1.0, 1.0, 1.0,
  };

  // Upload vertex data to a buffer
  glGenBuffers(1, &m_state.vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, m_state.vertex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);

  m_samplesPerSec = iSamplesPerSec;
  Launch(m_currentPreset);
  m_initialized = true;

  return true;
}

void CVisualizationMatrix::Stop()
{
  m_initialized = false;
  kodi::Log(ADDON_LOG_DEBUG, "Stop");

  UnloadPreset();
  UnloadTextures();

  glDeleteBuffers(1, &m_state.vertex_buffer);
}


void CVisualizationMatrix::AudioData(const float* pAudioData, int iAudioDataLength, float* pFreqData, int iFreqDataLength)
{
  WriteToBuffer(pAudioData, iAudioDataLength, 2);

  kiss_fft_cpx in[AUDIO_BUFFER], out[AUDIO_BUFFER];
  for (unsigned int i = 0; i < AUDIO_BUFFER; i++)
  {
    in[i].r = BlackmanWindow(m_pcm[i], i, AUDIO_BUFFER);
    in[i].i = 0;
  }

  kiss_fft(m_kissCfg, in, out);

  out[0].i = 0;

  SmoothingOverTime(m_magnitudeBuffer, m_magnitudeBuffer, out, NUM_BANDS, SMOOTHING_TIME_CONSTANT, AUDIO_BUFFER);

  const double rangeScaleFactor = MAX_DECIBELS == MIN_DECIBELS ? 1 : (1.0 / (MAX_DECIBELS - MIN_DECIBELS));
  for (unsigned int i = 0; i < NUM_BANDS; i++)
  {
    float linearValue = m_magnitudeBuffer[i];
    double dbMag = !linearValue ? MIN_DECIBELS : LinearToDecibels(linearValue);
    double scaledValue = UCHAR_MAX * (dbMag - MIN_DECIBELS) * rangeScaleFactor;

    m_audioData[i] = std::max(std::min((int)scaledValue, UCHAR_MAX), 0);
  }

  for (unsigned int i = 0; i < NUM_BANDS; i++)
  {
    float v = (m_pcm[i] + 1.0f) * 128.0f;
    m_audioData[i + NUM_BANDS] = std::max(std::min((int)v, UCHAR_MAX), 0);
  }

  m_needsUpload = true;
}

//-- OnAction -----------------------------------------------------------------
// Handle Kodi actions such as next preset, lock preset, album art changed etc
//-----------------------------------------------------------------------------
bool CVisualizationMatrix::NextPreset()
{
  m_currentPreset = (m_currentPreset + 1) % g_presets.size();
  Launch(m_currentPreset);
  UpdateAlbumart();
  kodi::SetSettingInt("lastpresetidx", m_currentPreset);
  return true;
}

bool CVisualizationMatrix::PrevPreset()
{
  m_currentPreset = (m_currentPreset - 1) % g_presets.size();
  Launch(m_currentPreset);
  UpdateAlbumart();
  kodi::SetSettingInt("lastpresetidx", m_currentPreset);
  return true;
}

bool CVisualizationMatrix::LoadPreset(int select)
{
  kodi::Log(ADDON_LOG_DEBUG, "Loading preset %i\n",select);
  m_currentPreset = select % g_presets.size();
  Launch(m_currentPreset);
  UpdateAlbumart();
  kodi::SetSettingInt("lastpresetidx", m_currentPreset);
  return true;
}

bool CVisualizationMatrix::RandomPreset()
{
  m_currentPreset = (int)((std::rand() / (float)RAND_MAX) * g_presets.size());
  Launch(m_currentPreset);
  UpdateAlbumart();
  kodi::SetSettingInt("lastpresetidx", m_currentPreset);
  return true;
}

//-- GetPresets ---------------------------------------------------------------
// Return a list of presets to Kodi for display
//-----------------------------------------------------------------------------
bool CVisualizationMatrix::GetPresets(std::vector<std::string>& presets)
{
  std::string name;
  for (auto preset : g_presets)
  {
    name = kodi::GetLocalizedString(preset.labelId, preset.name);
    presets.push_back(name);
  }
  return true;
}

//-- GetPreset ----------------------------------------------------------------
// Return the index of the current playing preset
//-----------------------------------------------------------------------------
int CVisualizationMatrix::GetActivePreset()
{
  return m_currentPreset;
}

bool CVisualizationMatrix::UpdateAlbumart()
{
  return CVisualizationMatrix::UpdateAlbumart(m_albumArt);
}

bool CVisualizationMatrix::UpdateAlbumart(std::string albumart)
{
  m_albumArt = albumart;

  kodi::Log(ADDON_LOG_DEBUG, "Updating album art %s\n",albumart.c_str());
  if (g_presets[m_currentPreset].channel[3] != 2)
  {
    return false;
  }

  std::string thumb = kodi::vfs::GetCacheThumbName(albumart.c_str());
  thumb = thumb.substr(0,8);
  std::string special = std::string("special://thumbnails/") + thumb.c_str()[0] + std::string("/") + thumb.c_str();

  if (kodi::vfs::FileExists(special + std::string(".png")))
  {
    m_channelTextures[3] = CreateTexture(kodi::vfs::TranslateSpecialProtocol(special + std::string(".png")), GL_RGBA, GL_LINEAR, GL_CLAMP_TO_EDGE);
    return true;
  }
  else if (kodi::vfs::FileExists(special + std::string(".jpg")))
  {
    m_channelTextures[3] = CreateTexture(kodi::vfs::TranslateSpecialProtocol(special + std::string(".jpg")), GL_RGBA, GL_LINEAR, GL_CLAMP_TO_EDGE);
    return true;
  }

  m_channelTextures[3] = CreateTexture(kodi::GetAddonPath("resources/textures/logo.png"), GL_RGBA, GL_LINEAR, GL_CLAMP_TO_EDGE);
  
  return false;
}

void CVisualizationMatrix::RenderTo(GLuint shader, GLuint effect_fb)
{
  glUseProgram(shader);

  if (shader == m_matrixShader.ProgramHandle())
  {
    GLuint w = Width();
    GLuint h = Height();
    if (m_state.fbwidth && m_state.fbheight)
      w = m_state.fbwidth, h = m_state.fbheight;
    int64_t intt = static_cast<int64_t>(std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() * 1000.0 * m_fallSpeed) - m_initialTime;
    if (m_bitsPrecision)
      intt &= (1<<m_bitsPrecision)-1;

    if (m_needsUpload)
    {
      for (int i = 0; i < 4; i++)
      {
        if (m_shaderTextures[i].audio)
        {
          glActiveTexture(GL_TEXTURE0 + i);
          glBindTexture(GL_TEXTURE_2D, m_channelTextures[i]);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, NUM_BANDS, 2, 0, GL_RED, GL_UNSIGNED_BYTE, m_audioData);
        }
      }
      m_needsUpload = false;


      if (g_presets[m_currentPreset].channel[3] == 2)
      {
        double logotimer = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
        float delta = static_cast<float>(logotimer - m_lastAlbumChange)*0.6f;
        GLfloat r = std::max(sin(delta),0.0f)*0.7f;
        GLfloat g = std::max(sin(delta - 1.0f),0.0f)*0.7f;
        GLfloat b = std::max(sin(delta - 2.0f),0.0f)*0.7f;
        glUniform3f(m_attrAlbumRGBLoc, r, g, b);
        if (m_lastAlbumChange == 0.0)
        {
          glUniform3f(m_attrAlbumPositionLoc, 0.f, 0.f, 2.0f);
        }
        if (logotimer - m_lastAlbumChange >= 10.)
        {
          m_albumX = static_cast<GLfloat>(std::fmod(logotimer * 1234., 1.) * (static_cast<double>(Width())/static_cast<double>(Height()) + 1.) - 1.);
          m_albumY = static_cast<GLfloat>(std::fmod(logotimer * 7654., 1.));
          m_lastAlbumChange = logotimer;
          m_AlbumNeedsUpload = true;
        }
        if (m_AlbumNeedsUpload)
        {
          glUniform3f(m_attrAlbumPositionLoc, m_albumX, m_albumY, 2.0f);//FIXME: proper framing, the album can reach over the edge of the screen
          m_AlbumNeedsUpload = true;//FIXME: limit upload to the actual album shader
        }
      }
    }

    float t = intt / 1000.0f;

    glUniform1f(m_attrGlobalTimeLoc, t);

    for (int i = 0; i < 4; i++)
    {
      glActiveTexture(GL_TEXTURE0 + i);
      glUniform1i(m_attrChannelLoc[i], i);
      glBindTexture(GL_TEXTURE_2D, m_channelTextures[i]);
    }
  }
  else
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_state.framebuffer_texture);
    glUniform1i(m_state.uTexture, 0); // first currently bound texture "GL_TEXTURE0"
  }

  // Draw the effect to a texture or direct to framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, effect_fb);

  glBindBuffer(GL_ARRAY_BUFFER, m_state.vertex_buffer);
  glVertexAttribPointer(m_state.attr_vertex_e, 4, GL_FLOAT, 0, 16, 0);
  glEnableVertexAttribArray(m_state.attr_vertex_e);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  glDisableVertexAttribArray(m_state.attr_vertex_e);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  for (int i = 0; i < 4; i++)
  {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

  glUseProgram(0);
}

void CVisualizationMatrix::Mix(float* destination, const float* source, size_t frames, size_t channels)
{
  size_t length = frames * channels;
  for (unsigned int i = 0; i < length; i += channels)
  {
    float v = 0.0f;
    for (size_t j = 0; j < channels; j++)
    {
      v += source[i + j];
    }

    destination[(i / 2)] = v / (float)channels;
  }
}

void CVisualizationMatrix::WriteToBuffer(const float* input, size_t length, size_t channels)
{
  size_t frames = length / channels;

  if (frames >= AUDIO_BUFFER)
  {
    size_t offset = frames - AUDIO_BUFFER;

    Mix(m_pcm, input + offset, AUDIO_BUFFER, channels);
  }
  else
  {
    size_t keep = AUDIO_BUFFER - frames;
    memmove(m_pcm, m_pcm + frames, keep * sizeof(float));

    Mix(m_pcm + keep, input, frames, channels);
  }
}

void CVisualizationMatrix::Launch(int preset)
{
  m_bitsPrecision = DetermineBitsPrecision();
  // mali-400 has only 10 bits which means milliseond timer wraps after ~1 second.
  // we'll fudge that up a bit as having a larger range is more important than ms accuracy
  m_bitsPrecision = std::max(m_bitsPrecision, 13);
  kodi::Log(ADDON_LOG_DEBUG, "bits of precision: %d", m_bitsPrecision);

  UnloadTextures();

  m_usedShaderFile = kodi::GetAddonPath("resources/shaders/" + g_presets[preset].file);
  for (int i = 0; i < 4; i++)
  {
    if (g_presets[preset].channel[i] >= 0 && g_presets[preset].channel[i] < static_cast< int > (g_fileTextures.size()))
    {
      m_shaderTextures[i].texture = kodi::GetAddonPath("resources/textures/" + g_fileTextures[g_presets[preset].channel[i]]);
    }
    else if (g_presets[preset].channel[i] == 99) // framebuffer
    {
      m_shaderTextures[i].audio = true;
    }
    else
    {
      m_shaderTextures[i].texture = "";
      m_shaderTextures[i].audio = false;
    }
  }
  // Audio
  m_channelTextures[0] = CreateTexture(GL_RED, NUM_BANDS, 2, m_audioData);
  // Logo
  if (!m_shaderTextures[1].texture.empty())
  {
    m_channelTextures[1] = CreateTexture(m_shaderTextures[1].texture, GL_RGBA, GL_LINEAR, GL_CLAMP_TO_EDGE);
  }
  // Noise
  if (!m_shaderTextures[2].texture.empty())
  {
    m_channelTextures[2] = CreateTexture(m_shaderTextures[2].texture, GL_RGBA, GL_LINEAR, GL_REPEAT);
  }
  // Album
  if (!m_shaderTextures[3].texture.empty())
  {
    m_channelTextures[3] = CreateTexture(m_shaderTextures[3].texture, GL_RGBA, GL_LINEAR, GL_CLAMP_TO_EDGE);
  }

  m_state.fbwidth = Width();
  m_state.fbheight = Height();
  LoadPreset(m_usedShaderFile);
}

void CVisualizationMatrix::UnloadTextures()
{
  for (int i = 0; i < 4; i++)
  {
    if (m_channelTextures[i])
    {
      glDeleteTextures(1, &m_channelTextures[i]);
      m_channelTextures[i] = 0;
    }
  }
}

void CVisualizationMatrix::LoadPreset(const std::string& shaderPath)
{
  UnloadPreset();
  GatherDefines();
  std::string vertMatrixShader = kodi::GetAddonPath("resources/shaders/main_matrix_" GL_TYPE_STRING ".vert.glsl");
  if (!m_matrixShader.LoadShaderFiles(vertMatrixShader, shaderPath) ||
      !m_matrixShader.CompileAndLink("", "", m_defines, ""))
  {
    kodi::Log(ADDON_LOG_ERROR, "Failed to compile matrix shaders (current file '%s')", shaderPath.c_str());
    return;
  }

  GLuint matrixShader = m_matrixShader.ProgramHandle();

  m_attrGlobalTimeLoc = glGetUniformLocation(matrixShader, "iTime");
  m_attrAlbumPositionLoc = glGetUniformLocation(matrixShader, "iAlbumPosition");
  m_attrAlbumRGBLoc = glGetUniformLocation(matrixShader, "iAlbumRGB");
  m_attrChannelLoc[0] = glGetUniformLocation(matrixShader, "iChannel0");
  m_attrChannelLoc[1] = glGetUniformLocation(matrixShader, "iChannel1");
  m_attrChannelLoc[2] = glGetUniformLocation(matrixShader, "iChannel2");
  m_attrChannelLoc[3] = glGetUniformLocation(matrixShader, "iChannel3");

  m_state.attr_vertex_e = glGetAttribLocation(matrixShader,  "vertex");

  // Prepare a texture to render to
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &m_state.framebuffer_texture);
  glBindTexture(GL_TEXTURE_2D, m_state.framebuffer_texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_state.fbwidth, m_state.fbheight, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // Prepare a framebuffer for rendering
  glGenFramebuffers(1, &m_state.effect_fb);
  glBindFramebuffer(GL_FRAMEBUFFER, m_state.effect_fb);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_state.framebuffer_texture, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  m_initialTime = static_cast<int64_t>(std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count() * 1000.0);
  m_initialTime += (m_initialTime % 100000);
}

void CVisualizationMatrix::UnloadPreset()
{
  if (m_state.framebuffer_texture)
  {
    glDeleteTextures(1, &m_state.framebuffer_texture);
    m_state.framebuffer_texture = 0;
  }
  if (m_state.effect_fb)
  {
    glDeleteFramebuffers(1, &m_state.effect_fb);
    m_state.effect_fb = 0;
  }
}

GLuint CVisualizationMatrix::CreateTexture(GLint format, unsigned int w, unsigned int h, const GLvoid* data)
{
  GLuint texture = 0;
  glActiveTexture(GL_TEXTURE0);
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D,  GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
  return texture;
}

GLuint CVisualizationMatrix::CreateTexture(const GLvoid* data, GLint format, unsigned int w, unsigned int h, GLint internalFormat, GLint scaling, GLint repeat)
{
  GLuint texture = 0;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, scaling);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, scaling);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat);

  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_UNSIGNED_BYTE, data);
  glBindTexture(GL_TEXTURE_2D, 0);

  return texture;
}

GLuint CVisualizationMatrix::CreateTexture(const std::string& file, GLint internalFormat, GLint scaling, GLint repeat)
{
  kodi::Log(ADDON_LOG_DEBUG, "creating texture %s\n", file.c_str());

  int width,height,n;
  unsigned char* image;
  stbi_set_flip_vertically_on_load(true);
  
  image = stbi_load(file.c_str(), &height, &width, &n, STBI_rgb_alpha);
  if (image == nullptr)
  {
    kodi::Log(ADDON_LOG_ERROR, "couldn't load image");
    return 0;
  }  

  GLuint texture = CreateTexture(image, GL_RGBA, width, height, internalFormat, scaling, repeat);
  stbi_image_free(image);
  image = nullptr;

  return texture;
}

float CVisualizationMatrix::BlackmanWindow(float in, size_t i, size_t length)
{
  double alpha = 0.16;
  double a0 = 0.5 * (1.0 - alpha);
  double a1 = 0.5;
  double a2 = 0.5 * alpha;

  float x = (float)i / (float)length;
  return in * (a0 - a1 * cos(2.0 * M_PI * x) + a2 * cos(4.0 * M_PI * x));
}

void CVisualizationMatrix::SmoothingOverTime(float* outputBuffer, float* lastOutputBuffer, kiss_fft_cpx* inputBuffer, size_t length, float smoothingTimeConstant, unsigned int fftSize)
{
  for (size_t i = 0; i < length; i++)
  {
    kiss_fft_cpx c = inputBuffer[i];
    float magnitude = sqrt(c.r * c.r + c.i * c.i) / (float)fftSize;
    outputBuffer[i] = smoothingTimeConstant * lastOutputBuffer[i] + (1.0 - smoothingTimeConstant) * magnitude;
  }
}

float CVisualizationMatrix::LinearToDecibels(float linear)
{
  if (!linear)
    return -1000;
  return 20 * log10f(linear);
}

int CVisualizationMatrix::DetermineBitsPrecision()
{
  m_state.fbwidth = 32, m_state.fbheight = 26*10;
  LoadPreset(kodi::GetAddonPath("resources/shaders/main_test.frag.glsl"));
  RenderTo(m_matrixShader.ProgramHandle(), m_state.effect_fb);
  glFinish();

  unsigned char* buffer = new unsigned char[m_state.fbwidth * m_state.fbheight * 4];
  if (buffer)
    glReadPixels(0, 0, m_state.fbwidth, m_state.fbheight, GL_RGBA, GL_UNSIGNED_BYTE, buffer);

  int bits = 0;
  unsigned char b = 0;
  for (int j=0; j<m_state.fbheight; j++)
  {
    unsigned char c = buffer[4*(j*m_state.fbwidth+(m_state.fbwidth>>1))];
    if (c && !b)
      bits++;
    b = c;
  }
  delete buffer;
  UnloadPreset();
  return bits;
}

void CVisualizationMatrix::GatherDefines()
{
  m_defines = "";
#if defined(HAS_GL)
  m_defines += "#version 150\n";
  m_defines += "#extension GL_OES_standard_derivatives : enable\n";
  m_defines += "out vec4 FragColor;\n";
  m_defines += "#ifndef texture2D\n#define texture2D texture\n#endif\n\n";
#else
  m_defines += "#version 100\n\n";
  m_defines += "#extension GL_OES_standard_derivatives : enable\n\n";
  m_defines += "precision mediump float;\n";
  m_defines += "precision mediump int;\n\n";
  m_defines += "#define FragColor gl_FragColor\n";
  m_defines += "#ifndef texture\n#define texture texture2D\n#endif\n\n";
#endif

  m_defines += "const float iDotSize = " + std::to_string(m_dotSize) + ";\n";//TODO remove from shaders
  m_defines += "const float cDotSize = " + std::to_string(m_dotSize) + ";\n";
  m_defines += "const float cColumns = " + std::to_string(static_cast<float>(Width())/(m_dotSize*2.0)) + ";\n";
  m_defines += "const float cNoiseFluctuation = " + std::to_string(m_noiseFluctuation) + ";\n";
  m_defines += "const float cDistortThreshold = " + std::to_string(m_distortThreshold) + ";\n";
  m_defines += "const vec3 cColor = vec3(" + std::to_string(m_dotColor.red) + "," + std::to_string(m_dotColor.green) + "," + std::to_string(m_dotColor.blue) + ");\n";

  if (m_state.fbwidth && m_state.fbheight)
  {
    m_defines += "const vec2 cResolution = vec2(" + std::to_string(m_state.fbwidth) + "," + std::to_string(m_state.fbheight) + ");\n";
    m_defines += "const vec2 iResolution = vec2(" + std::to_string(m_state.fbwidth) + "," + std::to_string(m_state.fbheight) + ");\n";//TODO remove from shaders
  }
  else
  {
    m_defines += "const vec2 cResolution = vec2(" + std::to_string(Width()) + ".," + std::to_string(Height()) + ".);\n";
    m_defines += "const vec2 iResolution = vec2(" + std::to_string(Width()) + ".," + std::to_string(Height()) + ".);\n";//TODO remove from shaders
  }

  m_defines += "uniform sampler2D iChannel0;\n";

  if (g_presets[m_currentPreset].channel[1] != -1)
  {
    m_defines += "uniform sampler2D iChannel1;\n";
  }

  if (g_presets[m_currentPreset].channel[2] != -1)
  {
    m_defines += "uniform sampler2D iChannel2;\n";
    m_defines += "#define dNoise\n";
  }
  
  if (g_presets[m_currentPreset].channel[3] != -1)
  {
    m_defines += "uniform sampler2D iChannel3;\n";
  }

  if (g_presets[m_currentPreset].channel[3] == 2)
  {
    m_defines += "uniform vec3 iAlbumPosition;\n";
    m_defines += "uniform vec3 iAlbumRGB;\n";
  }

  m_defines += "uniform float iTime;\n";

  //TODO: make pretty
  m_defines += "#define RNDSEED1 170.12\n";
  m_defines += "#define RNDSEED2 7572.1\n";

  m_defines += "#define INTENSITY 1.0\n";
  m_defines += "#define MININTENSITY 0.075\n";

  m_defines += "#define DISTORTFACTORX 0.6\n";
  m_defines += "#define DISTORTFACTORY 0.4\n";

  m_defines += "#define VIGNETTEINTENSITY 0.05\n";

  if (m_lowpower)
  {
    m_defines += fsCommonFunctionsLowPower;
  }
  else
  {
    m_defines += fsCommonFunctionsNormal;
  }

  kodi::Log(ADDON_LOG_DEBUG, "Fragment shader header\n%s",m_defines.c_str());
}

ADDONCREATOR(CVisualizationMatrix) // Don't touch this!
