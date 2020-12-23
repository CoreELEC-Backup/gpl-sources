/*
 *      vdr-plugin-vnsi - KODI server plugin for VDR
 *
 *      Copyright (C) 2005-2014 Team XBMC
 *      Copyright (C) 2015 Team KODI
 *
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
 *  along with KODI; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "status.h"
#include "vnsi.h"

#include <vdr/tools.h>
#include <vdr/recording.h>
#include <vdr/videodir.h>
#include <vdr/shutdown.h>

#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

cVNSIStatus::cVNSIStatus() : cThread("VNSIStatus")
{
}

cVNSIStatus::~cVNSIStatus()
{
  Shutdown();
}

void cVNSIStatus::Init(CVNSITimers *timers)
{
  m_vnsiTimers = timers;
  Start();
}

void cVNSIStatus::Shutdown()
{
  Cancel(5);
  cMutexLock lock(&m_mutex);
  m_clients.clear();
}

static bool CheckFileSuffix(const char *name,
                            const char *suffix, size_t suffix_length)
{
  size_t name_length = strlen(name);
  return name_length > suffix_length &&
    memcmp(name + name_length - suffix_length, suffix, suffix_length) == 0;
}

static void DeleteFiles(const char *directory_path, const char *suffix)
{
  const size_t suffix_length = strlen(suffix);

  DIR *dir = opendir(directory_path);
  if (dir == nullptr)
    return;

  std::string path(directory_path);
  path.push_back('/');
  const size_t start = path.size();

  while (auto *e = readdir(dir))
  {
    if (CheckFileSuffix(e->d_name, suffix, suffix_length))
    {
      path.replace(start, path.size(), e->d_name);

      if (unlink(path.c_str()) < 0)
      {
        ERRORLOG("Failed to delete %s: %s", path.c_str(), strerror(errno));
      }
    }
  }

  closedir(dir);
}

void cVNSIStatus::AddClient(int fd, unsigned int id, const char *ClientAdr, CVNSITimers &timers)
{
  cMutexLock lock(&m_mutex);
  std::shared_ptr<cVNSIClient> client = std::make_shared<cVNSIClient>(fd, id, ClientAdr, timers);
  m_clients.push_back(client);
}

void cVNSIStatus::Action(void)
{
  cTimeMs chanTimer(0);
  cTimeMs epgTimer(0);
  cTimeMs recTimer(0);
  int recCnt = 0;

  // get initial state of the recordings
#if VDRVERSNUM >= 20301
  cStateKey chanState;
  const cChannels *channels = cChannels::GetChannelsRead(chanState);
  chanState.Remove(false);
#endif

  // get initial state of the recordings
#if VDRVERSNUM >= 20301
  cStateKey recState;
  const cRecordings *recordings = cRecordings::GetRecordingsRead(recState);
  recState.Remove(false);
#else
  int recState = -1;
  Recordings.StateChanged(recState);
#endif

  // get initial state of the timers
#if VDRVERSNUM >= 20301
  cStateKey timerState;
  const cTimers *timers = cTimers::GetTimersRead(timerState);
  timerState.Remove(false);
#else
  int timerState = -1;
  Timers.Modified(timerState);
#endif

  // vnsitimer
  int vnsitimerState;
  m_vnsiTimers->StateChange(vnsitimerState);

  // last update of epg
#if VDRVERSNUM >= 20301
  cStateKey epgState;
  const cSchedules *epg = cSchedules::GetSchedulesRead(epgState);
  epgState.Remove(false);
#else
  time_t epgUpdate = cSchedules::Modified();
#endif

  // delete old timeshift file
  struct stat sb;
  if ((*TimeshiftBufferDir) && stat(TimeshiftBufferDir, &sb) == 0 && S_ISDIR(sb.st_mode))
  {
    DeleteFiles(TimeshiftBufferDir, ".vnsi");
  }
  else
  {
#if VDRVERSNUM >= 20102
    DeleteFiles(cVideoDirectory::Name(), ".vnsi");
#else
    DeleteFiles(VideoDirectory, ".vnsi");
#endif
  }

  // set thread priority
  SetPriority(1);

  while (Running())
  {
    m_mutex.Lock();

    // remove disconnected clients
    for (auto i = m_clients.begin(); i != m_clients.end();)
    {
      if (!(*i)->Active())
      {
        INFOLOG("removing client with ID %u from client list", (*i)->GetID());
        i = m_clients.erase(i);
      }
      else
      {
        ++i;
      }
    }

    // Don't to updates during running channel scan, KODI's PVR manager becomes
    //restarted of finished scan.
    if (!cVNSIClient::InhibidDataUpdates())
    {
      // reset inactivity timeout as long as there are clients connected
      if (!m_clients.empty())
      {
        ShutdownHandler.SetUserInactiveTimeout();
      }

      // trigger clients to reload the modified channel list
      if (chanTimer.TimedOut())
      {
#if VDRVERSNUM >= 20301
        if (channels->Lock(chanState))
        {
          chanState.Remove(false);
          INFOLOG("Requesting clients to reload channel list");
          for (auto client : m_clients)
            client->ChannelsChange();
          chanTimer.Set(5000);
        }
#else
        int modified = Channels.Modified();
        if (modified)
        {
          Channels.SetModified((modified == CHANNELSMOD_USER) ? true : false);
          INFOLOG("Requesting clients to reload channel list");
          for (auto client : m_clients)
            client->ChannelsChange();
        }
        chanTimer.Set(5000);
#endif
      }

#if VDRVERSNUM >= 20301
      if (recordings->Lock(recState))
      {
        recState.Remove();
        recTimer.Set(2500);
        ++recCnt;
      }
      else if (recCnt && recTimer.TimedOut())
      {
        INFOLOG("Requesting clients to reload recordings list (%d)", recCnt);
        recCnt = 0;
        for (auto client : m_clients)
        {
          client->RecordingsChange();
        }
      }

      if (timers->Lock(timerState))
      {
        timerState.Remove(false);
        INFOLOG("Requesting clients to reload timers");
        for (auto client : m_clients)
        {
          client->SignalTimerChange();
        }
      }

      if (m_vnsiTimers->StateChange(vnsitimerState))
      {
        INFOLOG("Requesting clients to reload vnsi-timers");
        for (auto client : m_clients)
        {
          client->SignalTimerChange();
        }
      }

      if (epgTimer.TimedOut())
      {
        if (epg->Lock(epgState))
        {
          epgState.Remove(false);
          DEBUGLOG("Requesting clients to load epg");
          int callAgain = 0;
          for (auto client : m_clients)
          {
            callAgain |= client->EpgChange();
          }
          if (callAgain & VNSI_EPG_AGAIN)
          {
            epgTimer.Set(100);
            epgState.Reset();
          }
          else
          {
            if (callAgain & VNSI_EPG_PAUSE)
            {
              epgState.Reset();
            }
            epgTimer.Set(5000);
            m_vnsiTimers->Scan();
          }
        }
      }
#else
      // update recordings
      if (Recordings.StateChanged(recState))
      {
        INFOLOG("Recordings state changed (%i)", recState);
        recTimer.Set(2500);
        ++recCnt;
      }
      else if (recCnt && recTimer.TimedOut())
      {
        INFOLOG("Requesting clients to reload recordings list");
        recCnt = 0;
        for (auto client : m_clients)
          client->RecordingsChange();
      }

      // update timers
      if (Timers.Modified(timerState))
      {
        INFOLOG("Timers state changed (%i)", timerState);
        INFOLOG("Requesting clients to reload timers");
        for (auto client : m_clients)
        {
          client->SignalTimerChange();
        }
      }

      // update epg
      if ((cSchedules::Modified() > epgUpdate + 10) || time(NULL) > epgUpdate + 300)
      {
        for (auto client : m_clients)
        {
          client->EpgChange();
        }
        epgUpdate = time(NULL);
      }
#endif
    }

    m_mutex.Unlock();

    usleep(250*1000);
  }
}
