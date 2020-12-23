/*
*      Copyright (C) 2017 Team Kodi
*      http://www.kodi.tv
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
*  along with XBMC; see the file COPYING.  If not, write to
*  the Free Software Foundation, 51 Franklin Street, Fifth Floor, Boston,
*  MA 02110-1301 USA
*  http://www.gnu.org/copyleft/gpl.html
*
*/

#include "pvrclient-launcher.h"
#include "private/os/threads/event.h"
#include "private/os/threads/thread.h"

#include <kodi/General.h>

class ATTRIBUTE_HIDDEN PVRClientLauncherPrivate : private Myth::OS::CThread
{
public:
  PVRClientLauncherPrivate(PVRClientMythTV* client);
  virtual ~PVRClientLauncherPrivate();

  bool Start();
  bool WaitForCompletion(unsigned timeout);

protected:
  void *Process();

private:
  PVRClientMythTV* m_client;
  Myth::OS::CEvent m_alarm;
};

PVRClientLauncher::PVRClientLauncher(PVRClientMythTV* client)
: m_p(new PVRClientLauncherPrivate(client))
{
}

PVRClientLauncher::~PVRClientLauncher()
{
  delete m_p;
}

bool PVRClientLauncher::Start()
{
  return m_p->Start();
}

bool PVRClientLauncher::WaitForCompletion(unsigned timeout)
{
  return m_p->WaitForCompletion(timeout);
}

PVRClientLauncherPrivate::PVRClientLauncherPrivate(PVRClientMythTV* client)
: Myth::OS::CThread()
, m_client(client)
{
  std::string name;
  std::string version;
  m_client->GetBackendName(name);
  m_client->GetBackendVersion(version);
  m_client->ConnectionStateChange(name, PVR_CONNECTION_STATE_CONNECTING, version);
}

PVRClientLauncherPrivate::~PVRClientLauncherPrivate()
{
  StopThread(false); // Set stopping. don't wait as we need to signal the thread first
  m_alarm.Signal();
  StopThread(true); // Wait for thread to stop
}

bool PVRClientLauncherPrivate::Start()
{
  return StartThread(true);
}

bool PVRClientLauncherPrivate::WaitForCompletion(unsigned timeout)
{
  return m_alarm.Wait(timeout);
}

void* PVRClientLauncherPrivate::Process()
{
  bool notifyAddonFailure = true;
  // By default this launcher will retry for ever until the user cancel it by a dialog.
  bool retry = true;
  while (!IsStopped() && retry)
  {
    if (m_client->Connect())
    {
      std::string name;
      std::string version;
      m_client->GetBackendName(name);
      m_client->GetBackendVersion(version);
      m_client->ConnectionStateChange(name, PVR_CONNECTION_STATE_CONNECTED, version);
      /* Read setting "LiveTV Priority" from backend database */
      bool savedLiveTVPriority = kodi::GetSettingBoolean("livetv_priority", DEFAULT_LIVETV_PRIORITY);
      CMythSettings::SetLiveTVPriority(m_client->GetLiveTVPriority());
      if (CMythSettings::GetLiveTVPriority() != savedLiveTVPriority)
        m_client->SetLiveTVPriority(savedLiveTVPriority);
      /* End of process */

      // Connected.
      std::string msg = kodi::GetLocalizedString(30114);
      kodi::QueueNotification(QUEUE_INFO, "", msg);

      break;
    }

    if (notifyAddonFailure)
    {
      PVRClientMythTV::CONN_ERROR error = m_client->GetConnectionError();
      if (error == PVRClientMythTV::CONN_ERROR_UNKNOWN_VERSION)
      {
        // Failed to connect the MythTV backend with the known protocol versions.
        std::string msg = kodi::GetLocalizedString(30300);
        kodi::QueueNotification(QUEUE_ERROR, "", msg);
      }
      else if (error == PVRClientMythTV::CONN_ERROR_API_UNAVAILABLE)
      {
        // Failed to connect the API services of MythTV backend. Please check your PIN code or backend setup.
        std::string msg = kodi::GetLocalizedString(30301);
        kodi::QueueNotification(QUEUE_ERROR, "", msg);
      }
      else
      {
        // No response from MythTV backend.
        std::string msg = kodi::GetLocalizedString(30304);
        kodi::QueueNotification(QUEUE_WARNING, "", msg);
      }
      // No longer notify the failure
      notifyAddonFailure = false;
    }
    else
    {
      m_alarm.Wait(PVRCLIENT_LAUNCHER_RETRY * 1000);
    }
  }
  kodi::Log(ADDON_LOG_INFO, "Launcher stopped");
  // Signal the launcher has finished
  m_alarm.Broadcast();
  return 0;
}
