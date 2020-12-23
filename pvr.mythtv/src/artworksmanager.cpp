/*
 *      Copyright (C) 2018 Team XBMC
 *      http://www.xbmc.org
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

#include "artworksmanager.h"

#include "pvrclient-mythtv.h"
#include "private/os/os.h"

#define FILEOPS_CHANNEL_DUMMY_ICON    "channel.png"
#define FILEOPS_RECORDING_DUMMY_ICON  "recording.png"
#define CHANNEL_ICON_WIDTH            300

ArtworkManager::ArtworkManager(const std::string& server, unsigned wsapiport, const std::string& wsapiSecurityPin)
: m_wsapi(NULL)
{
  m_wsapi = new Myth::WSAPI(server, wsapiport, wsapiSecurityPin);
}

ArtworkManager::~ArtworkManager()
{
  delete m_wsapi;
}

std::string ArtworkManager::GetChannelIconPath(const MythChannel& channel)
{
  if (channel.IsNull() || channel.Icon().empty())
    return "";
  if (!CMythSettings::GetChannelIcons())
    return kodi::GetAddonPath() + PATH_SEPARATOR_STRING + "resources" + PATH_SEPARATOR_STRING + FILEOPS_CHANNEL_DUMMY_ICON;

  return m_wsapi->GetChannelIconUrl(channel.ID(), CHANNEL_ICON_WIDTH);
}

std::string ArtworkManager::GetPreviewIconPath(const MythProgramInfo& recording)
{
  if (recording.IsNull())
    return "";
  if (!CMythSettings::GetRecordingIcons())
    return kodi::GetAddonPath() + PATH_SEPARATOR_STRING + "resources" + PATH_SEPARATOR_STRING + FILEOPS_RECORDING_DUMMY_ICON;

  return m_wsapi->GetPreviewImageUrl(recording.ChannelID(), recording.RecordingStartTime());
}

std::string ArtworkManager::GetArtworkPath(const MythProgramInfo& recording, ArtworksType type)
{
  if (recording.IsNull())
    return "";
  if (!CMythSettings::GetRecordingIcons())
    switch (type)
    {
    case AWTypeCoverart:
      return kodi::GetAddonPath() + PATH_SEPARATOR_STRING + "resources" + PATH_SEPARATOR_STRING + FILEOPS_RECORDING_DUMMY_ICON;
    default:
      return "";
    }

  return m_wsapi->GetRecordingArtworkUrl(GetTypeNameByArtworksType(type), recording.Inetref(), recording.Season());
}
