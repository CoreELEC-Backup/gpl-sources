#pragma once
/*
 *      Copyright (C) 2005-2013 Team Kodi
 *      https://kodi.tv
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *************************************************************************
 *  This file is a modified version from Team MediaPortal's
 *  TsReader DirectShow filter
 *  MediaPortal is a GPL'ed HTPC-Application
 *  Copyright (C) 2005-2012 Team MediaPortal
 *  http://www.team-mediaportal.com
 *
 * Changes compared to Team MediaPortal's version:
 * - Code cleanup for PVR addon usage
 * - Code refactoring for cross platform usage
 *************************************************************************
 */

#ifdef LIVE555

#include "p8-platform/threads/threads.h"
#include "lib/tsreader/MemoryBuffer.h"

#include "liveMedia.hh"
#include "BasicUsageEnvironment.hh"
#include "GroupsockHelper.hh"


#define RTSP_URL_BUFFERSIZE 2048

class CRTSPClient: public P8PLATFORM::CThread
{
public:
  CRTSPClient();
  virtual ~CRTSPClient(void);
  bool Initialize(CMemoryBuffer* buffer);
  bool OpenStream(const char* url);
  bool Play(double fStart,double fDuration);
  void Stop();
  bool IsRunning();
  long Duration();
  bool Pause();
  bool IsPaused();
  void Continue();
  void FillBuffer(unsigned long byteCount);

  char* getSDPDescription();
  bool UpdateDuration();

protected:
  CMemoryBuffer* m_buffer;
  Medium* createClient(UsageEnvironment& env,int verbosityLevel, char const* applicationName);
  char* getSDPDescriptionFromURL(Medium* client, char const* url,
               char const* username, char const* password,
               char const* /*proxyServerName*/,
               unsigned short /*proxyServerPortNum*/,
               unsigned short /*clientStartPort*/);
  bool clientSetupSubsession(Medium* client, MediaSubsession* subsession, bool streamUsingTCP);
  bool clientStartPlayingSession(Medium* client, MediaSession* session);
  bool clientTearDownSession(Medium* client, MediaSession* session);
  void closeMediaSinks();
  void tearDownStreams();
  bool setupStreams();
  //void checkForPacketArrival(void* /*clientData*/);
  MediaSession* m_session;

  bool allowProxyServers;
  bool controlConnectionUsesTCP;
  bool supportCodecSelection;
  char const* clientProtocolName;
  portNumBits tunnelOverHTTPPortNum;
  unsigned statusCode;
  char const* singleMedium;
  unsigned short desiredPortNum;
  bool createReceivers;
  int simpleRTPoffsetArg;
  unsigned socketInputBufferSize;
  bool streamUsingTCP;
	size_t fileSinkBufferSize;
  bool oneFilePerFrame;

public:
  UsageEnvironment* m_env;
  Medium* m_ourClient;
  char* getOptionsResponse(Medium* client, char const* url,char* username, char* password);
  void shutdown();
  bool startPlayingStreams();

  // Thread
private:
  virtual void *Process(void);
  void StartBufferThread();
  void StopBufferThread();
  bool m_BufferThreadActive;

  long m_duration;
  double m_fStart;
  double m_fDuration;
  char m_url[RTSP_URL_BUFFERSIZE];
  bool m_bRunning;
  bool m_bPaused;
  char m_outFileName[1000];
};
#endif //LIVE555
