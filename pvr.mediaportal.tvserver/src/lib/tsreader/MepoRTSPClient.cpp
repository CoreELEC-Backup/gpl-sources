/*
 *      Copyright (C) 2005-2010 Team Kodi
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
 */

#if defined LIVE555

#include "p8-platform/util/timeutils.h"
#include "MepoRTSPClient.h"
#include "MemorySink.h"
#include <kodi/General.h> //for kodi::Log
#include "utils.h"
#include "os-dependent.h"

CRTSPClient::CRTSPClient()
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::CRTSPClient()");
  allowProxyServers = false;
  controlConnectionUsesTCP = true;
  supportCodecSelection = false;
  clientProtocolName = "RTSP";
  tunnelOverHTTPPortNum = 0;
  statusCode = 0;
  singleMedium = NULL;
  desiredPortNum = 0;
  createReceivers = true;
  simpleRTPoffsetArg = -1;
  socketInputBufferSize = 0;
  streamUsingTCP = false;
  fileSinkBufferSize = 20000;
  oneFilePerFrame = false;
  m_BufferThreadActive = false;
  m_duration = 7200*1000;
  m_fStart = 0.0f;
  m_session = NULL;
  m_ourClient = NULL;
  m_bPaused = false;
  m_outFileName[0] = '\0';
  m_buffer = NULL;
  m_env = NULL;
  m_fDuration = 0.0f;
  m_url[0] = '\0';
  m_bRunning = false;
}

CRTSPClient::~CRTSPClient()
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::~CRTSPClient()");
  Medium::close(m_ourClient);
  m_ourClient = NULL;

  if (m_env)
  {
    TaskScheduler *scheduler = &m_env->taskScheduler();
    m_env->reclaim();
    m_env = NULL;
    delete scheduler;
  }
}


Medium* CRTSPClient::createClient(UsageEnvironment& env,int verbosityLevel, char const* applicationName)
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::createClient()");
  return RTSPClient::createNew(env, verbosityLevel, applicationName,tunnelOverHTTPPortNum);
}

char* CRTSPClient::getOptionsResponse(Medium* client, char const* url,char* username, char* password)
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::getOptionsResponse()");
  RTSPClient* rtspClient = (RTSPClient*)client;
  char* optionsResponse = rtspClient->sendOptionsCmd(url, username, password);

  if (optionsResponse == NULL)
  {
    kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::getOptionsResponse(): \"OPTIONS\" request failed: %s", m_env->getResultMsg());
  } else {
    kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::getOptionsResponse(): \"OPTIONS\" request returned: %s", optionsResponse);
  }

  return optionsResponse;
}

char* CRTSPClient::getSDPDescriptionFromURL(Medium* client, char const* url,
             char const* username, char const* password,
             char const* /*proxyServerName*/,
             unsigned short /*proxyServerPortNum*/,
             unsigned short /*clientStartPort*/)
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::getSDPDescriptionFromURL()");
  RTSPClient* rtspClient = (RTSPClient*)client;
  char* result;
  if (username != NULL && password != NULL)
  {
    result = rtspClient->describeWithPassword(url, username, password);
  }
  else
  {
    result = rtspClient->describeURL(url);
  }

  statusCode = rtspClient->describeStatus();
  return result;
}

char* CRTSPClient::getSDPDescription()
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::getSDPDescription()");
  RTSPClient *client = (RTSPClient*)m_ourClient;
  RTSPClient *rtspClient = RTSPClient::createNew(client->envir(), 0, "TSFileSource", tunnelOverHTTPPortNum);
  char* result;
  result = rtspClient->describeURL(m_url);

  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::getSDPDescription() statusCode = %d", rtspClient->describeStatus());
  Medium::close(rtspClient);

  return result;
}

bool CRTSPClient::clientSetupSubsession(Medium* client, MediaSubsession* subsession, bool streamUsingTCP)
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::clientSetupSubsession()");
  if (client == NULL || subsession == NULL)
    return false;
  RTSPClient* rtspClient = (RTSPClient*) client;
  return ( rtspClient->setupMediaSubsession(*subsession, False, (streamUsingTCP ? True : False)) ? true : false);
}

bool CRTSPClient::clientStartPlayingSession(Medium* client, MediaSession* session)
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::clientStartPlayingSession()");
  if (client == NULL || session == NULL)
    return false;
  RTSPClient* rtspClient = (RTSPClient*) client;

  long dur = m_duration/1000;
  double fStart = m_fStart;

  if (m_fDuration > 0.0)
  {
    double fStartToEnd = m_fDuration-m_fStart;
    if (fStartToEnd<0)
      fStartToEnd = 0;
    fStart = dur - fStartToEnd;
    if (fStart<0)
      fStart = 0;
  }

  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::clientStartPlayingSession() play from %.3f / %.3f", fStart, (float) m_duration/1000);
  return (rtspClient->playMediaSession(*session,fStart) ? true : false);
}

bool CRTSPClient::clientTearDownSession(Medium* client,MediaSession* session)
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::clientTearDownSession()");
  if (client == NULL || session == NULL)
    return false;
  RTSPClient* rtspClient = (RTSPClient*)client;
  return (rtspClient->teardownMediaSession(*session) ? true : false);
}

void my_subsessionAfterPlaying(void* UNUSED(clientData))
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::subsessionAfterPlaying()");
}

void my_subsessionByeHandler(void* UNUSED(clientData))
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::subsessionByeHandler()");
}

void CRTSPClient::closeMediaSinks()
{
  if (m_session == NULL)
    return;

  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::closeMediaSinks()");

  MediaSubsessionIterator iter(*m_session);
  MediaSubsession* subsession;

  while ((subsession = iter.next()) != NULL)
  {
    Medium::close(subsession->sink);
    subsession->sink = NULL;
  }
}

void CRTSPClient::tearDownStreams()
{
  if (m_session == NULL)
    return;

  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::tearDownStreams()");

  clientTearDownSession(m_ourClient, m_session);
}
bool CRTSPClient::setupStreams()
{
  // Setup streams
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::setupStreams()");

  bool madeProgress = false;
  MediaSubsessionIterator iter(*m_session);
  MediaSubsession *subsession;

  while ((subsession = iter.next()) != NULL)
  {
    if (subsession->clientPortNum() == 0)
      continue; // port # was not set

    if (!clientSetupSubsession(m_ourClient, subsession, streamUsingTCP))
    {
      kodi::Log(ADDON_LOG_ERROR, "Failed to setup %s %s %s", subsession->mediumName(), subsession->codecName(), m_env->getResultMsg() );
    }
    else
    {
      kodi::Log(ADDON_LOG_DEBUG, "Setup %s %s %d %d", subsession->mediumName(), subsession->codecName(), subsession->clientPortNum(), subsession->clientPortNum() + 1);
      madeProgress = true;
    }
  }

  if (!madeProgress)
  {
    shutdown();
    return false;
  }
  return true;
}

bool CRTSPClient::startPlayingStreams()
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::startPlayingStreams()");

  if (!clientStartPlayingSession(m_ourClient, m_session))
  {
    kodi::Log(ADDON_LOG_ERROR, "Failed to start playing session :%s", m_env ->getResultMsg() );
    shutdown();
    return false;
  }
  else
  {
    kodi::Log(ADDON_LOG_DEBUG, "Started playing session");
  }
  return true;
}

void CRTSPClient::shutdown()
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::shutdown()");

  // Close our output files:
  closeMediaSinks();

  // Teardown, then shutdown, any outstanding RTP/RTCP subsessions
  tearDownStreams();
  Medium::close(m_session);

  // Finally, shut down our client:
  Medium::close(m_ourClient);
  m_session = NULL;
  m_ourClient = NULL;
}


bool CRTSPClient::Initialize(CMemoryBuffer* buffer)
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::Initialize()");

  m_buffer = buffer;
  m_duration = 7200*1000;

  TaskScheduler* scheduler = BasicTaskScheduler::createNew();
  m_env = BasicUsageEnvironment::createNew(*scheduler);

  m_ourClient = createClient(*m_env, 0/*verbosityLevel*/, "TSFileSource");

  if (m_ourClient == NULL)
  {
    kodi::Log(ADDON_LOG_ERROR, "Failed to create %s %s", clientProtocolName, m_env->getResultMsg() );
    shutdown();
    return false;
  }
  return true;
}

bool CRTSPClient::OpenStream(const char* url)
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::OpenStream()");
  m_session = NULL;

  strncpy(m_url, url, RTSP_URL_BUFFERSIZE - 1);
  m_url[RTSP_URL_BUFFERSIZE - 1] = '\0';

  // Open the URL, to get a SDP description:
  char* sdpDescription = getSDPDescriptionFromURL(m_ourClient, url, ""/*username*/, ""/*password*/,""/*proxyServerName*/, 0/*proxyServerPortNum*/,1234/*desiredPortNum*/);

  if (sdpDescription == NULL)
  {
    kodi::Log(ADDON_LOG_ERROR, "Failed to get a SDP description from URL %s %s", url, m_env->getResultMsg() );
    shutdown();
    return false;
  }
  kodi::Log(ADDON_LOG_DEBUG, "Opened URL %s %s", url, sdpDescription);

  char* range = strstr(sdpDescription, "a=range:npt=");
  if (range != NULL)
  {
    char *pStart = range + strlen("a=range:npt=");
    char *pEnd = strstr(range, "-");
    if (pEnd != NULL)
    {
      pEnd++;
      double Start = atof(pStart);
      double End = atof(pEnd);

      kodi::Log(ADDON_LOG_DEBUG, "rangestart:%f rangeend:%f", Start, End);
      m_duration = (long) ((End-Start)*1000.0);
    }
  }
  // Create a media session object from this SDP description:
  m_session = MediaSession::createNew(*m_env, sdpDescription);
  delete[] sdpDescription;

  if (m_session == NULL)
  {
    kodi::Log(ADDON_LOG_ERROR, "Failed to create a MediaSession object from the SDP description:%s ", m_env->getResultMsg());
    shutdown();
    return false;
  }
  else if (!m_session->hasSubsessions())
  {
    kodi::Log(ADDON_LOG_DEBUG, "This session has no media subsessions");
    shutdown();
    return false;
  }

  // Then, setup the "RTPSource"s for the session:
  MediaSubsessionIterator iter(*m_session);
  MediaSubsession *subsession;
  bool madeProgress = false;
  char const* singleMediumToTest = singleMedium;

  while ((subsession = iter.next()) != NULL)
  {
    // If we've asked to receive only a single medium, then check this now:
    if (singleMediumToTest != NULL)
    {
      if (strcmp(subsession->mediumName(), singleMediumToTest) != 0)
      {
        kodi::Log(ADDON_LOG_DEBUG, "Ignoring %s %s %s", subsession->mediumName(), subsession->codecName(), singleMedium);
        continue;
      }
      else
      {
        // Receive this subsession only
        singleMediumToTest = "xxxxx";
        // this hack ensures that we get only 1 subsession of this type
      }
    }

    if (desiredPortNum != 0)
    {
      subsession->setClientPortNum(desiredPortNum);
      desiredPortNum += 2;
    }

    if (createReceivers)
    {
      if (!subsession->initiate(simpleRTPoffsetArg))
      {
        kodi::Log(ADDON_LOG_ERROR, "Unable to create receiver for %s %s %s", subsession->mediumName(), subsession->codecName(), m_env->getResultMsg());
      }
      else
      {
        kodi::Log(ADDON_LOG_DEBUG, "Created receiver for type=%s codec=%s ports: %d %d ", subsession->mediumName(), subsession->codecName(), subsession->clientPortNum(), subsession->clientPortNum() + 1 );
        madeProgress = true;

        if (subsession->rtpSource() != NULL)
        {
          // Because we're saving the incoming data, rather than playing
          // it in real time, allow an especially large time threshold
          // (1 second) for reordering misordered incoming packets:
          int socketNum = subsession->rtpSource()->RTPgs()->socketNum();

          kodi::Log(ADDON_LOG_DEBUG, "rtsp:increaseReceiveBufferTo to 2000000 for s:%d", socketNum);
          increaseReceiveBufferTo( *m_env, socketNum, 2000000 );

          unsigned const thresh = 1000000; // 1 second
          subsession->rtpSource()->setPacketReorderingThresholdTime(thresh);

          if (socketInputBufferSize > 0)
          {
            // Set the RTP source's input buffer size as specified:
            unsigned int curBufferSize = getReceiveBufferSize(*m_env, socketNum);
            unsigned int newBufferSize = setReceiveBufferTo(*m_env, socketNum, socketInputBufferSize);

            kodi::Log(ADDON_LOG_DEBUG, "Changed socket receive buffer size for the %s %s %d %d", subsession->mediumName(), subsession->codecName(), curBufferSize, newBufferSize);
          }
        }
      }
    }
    else
    {
      if (subsession->clientPortNum() == 0)
      {
        kodi::Log(ADDON_LOG_DEBUG, "No client port was specified for the %s %s", subsession->mediumName(), subsession->codecName());
      }
      else
      {
        madeProgress = true;
      }
    }
  }

  if (!madeProgress)
  {
    shutdown();
    return false;
  }

  // Perform additional 'setup' on each subsession, before playing them:
  if (!setupStreams())
  {
    return false;
  }

  // Create output files:
  // Create and start "FileSink"s for each subsession:
  madeProgress = false;
  iter.reset();

  while ((subsession = iter.next()) != NULL)
  {
    if (subsession->readSource() == NULL) continue; // was not initiated

    // Mediaportal:
    CMemorySink* fileSink = CMemorySink::createNew(*m_env, *m_buffer, fileSinkBufferSize);
    // XBMC test via file:
    //FileSink* fileSink = FileSink::createNew(*m_env, m_outFileName, fileSinkBufferSize, false); //oneFilePerFrame

    subsession->sink = fileSink;
    if (subsession->sink == NULL)
    {
      kodi::Log(ADDON_LOG_DEBUG, "Failed to create FileSink %s", m_env->getResultMsg());
      shutdown();
      return false;
    }
    kodi::Log(ADDON_LOG_DEBUG, "Created output sink: %s", m_outFileName);
    subsession->sink->startPlaying(*(subsession->readSource()), my_subsessionAfterPlaying, subsession);

    // Also set a handler to be called if a RTCP "BYE" arrives
    // for this subsession:
    if (subsession->rtcpInstance() != NULL)
    {
      subsession->rtcpInstance()->setByeHandler(my_subsessionByeHandler, subsession);
    }
    madeProgress = true;
  }

  return true;
}


void CRTSPClient::Stop()
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient:Stop");

  if (m_BufferThreadActive)
  {
    StopBufferThread();
  }

  shutdown();
  m_buffer->Clear();
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient:Stop done");
}

void CRTSPClient::StartBufferThread()
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::StartBufferThread");

  if (!m_BufferThreadActive)
  {
    CreateThread();
    m_BufferThreadActive = true;
  }
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::StartBufferThread done");
}

void CRTSPClient::StopBufferThread()
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::StopBufferThread");
  m_bRunning = false;
  if (!m_BufferThreadActive)
    return;

  StopThread();

  m_BufferThreadActive = false;
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::StopBufferThread done");
}

bool CRTSPClient::IsRunning()
{
  return m_BufferThreadActive;
}

long CRTSPClient::Duration()
{
  return m_duration;
}

void CRTSPClient::FillBuffer(unsigned long byteCount)
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::Fillbuffer...%d\n", byteCount);
  unsigned long long tickCount = GetTickCount64();

  while ( IsRunning() && m_buffer->Size() < byteCount)
  {
    usleep(5000);
    if (GetTickCount64() - tickCount > 3000)
      break;
  }
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::Fillbuffer...%d/%d\n", byteCount, m_buffer->Size() );
}

void *CRTSPClient::Process()
{
  m_BufferThreadActive = true;
  m_bRunning = true;

  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient:: thread started");

  while (m_env != NULL && !IsStopped())
  {
    m_env->taskScheduler().doEventLoop();
    if (m_bRunning == false)
      break;
  }

  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient:: thread stopped");

  m_BufferThreadActive = false;

  return NULL;
}

void CRTSPClient::Continue()
{
  if (m_ourClient != NULL && m_session != NULL)
  {
    RTSPClient* rtspClient = (RTSPClient*) m_ourClient;
    rtspClient->playMediaSession(*m_session, -1.0);
    StartBufferThread();
    m_bPaused = false;
  }
}

bool CRTSPClient::IsPaused()
{
  return m_bPaused;
}

bool CRTSPClient::Pause()
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::Pause()");
  if (m_ourClient != NULL && m_session != NULL)
  {
    kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::Pause() stopthread");
    StopThread(10000);                    // Ambass : sometimes 100mS ( prev value ) is not enough and thread is not stopped.
                                                 //          now stopping takes around 5 secs ?!?! why ????
    kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::Pause() thread stopped");
    RTSPClient* rtspClient=(RTSPClient*)m_ourClient;
    rtspClient->pauseMediaSession(*m_session);
    m_bPaused = true;
  }
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::Pause() done");

  return true;
}
bool CRTSPClient::Play(double fStart,double fDuration)
{
  kodi::Log(ADDON_LOG_DEBUG, "CRTSPClient::Play from %f / %f", (float)fStart, (float)fDuration);

  m_bPaused = false;
  m_fStart = fStart;
  m_fDuration = fDuration;

  if (m_BufferThreadActive)
  {
    Stop();
    m_buffer->Clear();
    if (Initialize(m_buffer) == false)
    {
      shutdown();
      return false;
    }
    if (OpenStream(m_url) == false)
    {
      shutdown();
      return false;
    }
  }

  if (m_ourClient == NULL || m_session == NULL)
  {
    m_buffer->Clear();
    if (Initialize(m_buffer) == false)
    {
      shutdown();
      return false;
    }
    if (OpenStream(m_url) == false)
    {
      shutdown();
      return false;
    }
  }

  if (!startPlayingStreams())
  {
    shutdown();
    return false;
  }
  StartBufferThread();

  return true;
}

bool CRTSPClient::UpdateDuration()
{
  char* sdpDescription = getSDPDescription();
  if (sdpDescription == NULL)
  {
    kodi::Log(ADDON_LOG_ERROR, "UpdateStreamDuration: Failed to get a SDP description from URL %s %s", m_url, m_env->getResultMsg() );
    return false;
  }
  //kodi::Log(ADDON_LOG_DEBUG, "Opened URL %s %s",url,sdpDescription);

  char* range = strstr(sdpDescription, "a=range:npt=");
  if (range != NULL)
  {
    char *pStart = range + strlen("a=range:npt=");
    char *pEnd = strstr(range,"-");

    if (pEnd != NULL)
    {
      pEnd++;
      double Start = atof(pStart);
      double End = atof(pEnd);

      //kodi::Log(ADDON_LOG_DEBUG, "rangestart:%f rangeend:%f", Start,End);
      m_duration = (long) ((End-Start)*1000.0);
    }
  }

  return true;
}
#endif //LIVE555
