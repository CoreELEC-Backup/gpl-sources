/*
 *      vdr-plugin-vnsi - KODI server plugin for VDR
 *
 *      Copyright (C) 2010 Alwin Esch (Team XBMC)
 *      Copyright (C) 2010, 2011 Alexander Pipelka
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

#pragma once

#include <linux/dvb/frontend.h>
#include <linux/videodev2.h>
#include <vdr/thread.h>

#include "parser.h"
#include "responsepacket.h"
#include "demuxer.h"
#include "videoinput.h"

#include <memory>

class cxSocket;
class cChannel;
class cTSParser;
class cResponsePacket;
class cVideoBuffer;
class cVideoInput;
class cDevice;

class cLiveStreamer : public cThread
{
  friend class cParser;

public:

  cLiveStreamer(int clientID, bool bAllowRDS, int protocol, uint8_t timeshift, uint32_t timeout);
  virtual ~cLiveStreamer();

  cLiveStreamer(const cLiveStreamer &) = delete;
  cLiveStreamer &operator=(const cLiveStreamer &) = delete;

  void Activate(bool On);

  bool StreamChannel(const cChannel *channel, int priority, cxSocket *Socket, cResponsePacket* resp);
  bool IsStarting() { return m_startup; }
  bool IsAudioOnly() { return m_IsAudioOnly; }
  bool IsMPEGPS() { return m_IsMPEGPS; }
  bool SeekTime(int64_t time, uint32_t &serial);
  void RetuneChannel(const cChannel *channel);
  void AddStatusSocket(int fd);
  void SendStatus();

protected:
  virtual void Action(void);
  bool Open(int serial = -1);
  void Close();

  void sendStreamPacket(sStreamPacket *pkt);
  void sendStreamChange();
  void sendSignalInfo();
  void sendStreamStatus();
  void sendBufferStatus();
  void sendRefTime(sStreamPacket &pkt);
  void sendStreamTimes();

  const int m_ClientID;
  const cChannel *m_Channel = nullptr;
  cDevice *m_Device;
  cxSocket *m_Socket = nullptr;             /*!> The socket class to communicate with client */
  std::unique_ptr<cxSocket> m_statusSocket;
  int m_Frontend = -1;                      /*!> File descriptor to access used receiving device  */
  dvb_frontend_info m_FrontendInfo;         /*!> DVB Information about the receiving device (DVB only) */
  v4l2_capability m_vcap;                   /*!> PVR Information about the receiving device (pvrinput only) */
  cString m_DeviceString;                   /*!> The name of the receiving device */
  bool m_startup = true;
  bool m_IsAudioOnly = false;               /*!> Set to true if streams contains only audio */
  bool m_IsMPEGPS = false;                  /*!> TS Stream contains MPEG PS data like from pvrinput */
  uint32_t m_scanTimeout;                   /*!> Channel scanning timeout (in seconds) */
  cTimeMs m_last_tick;
  bool m_SignalLost = false;
  bool m_IFrameSeen = false;
  cResponsePacket m_streamHeader;
  cVNSIDemuxer m_Demuxer;
  cVideoBuffer *m_VideoBuffer = nullptr;
  cVideoInput m_VideoInput;
  int m_Priority;
  uint8_t m_Timeshift;
  cCondWait m_Event;
  time_t m_refTime;
  int64_t m_refDTS;
  int64_t m_curDTS = 0;
  int m_protocolVersion = 0;
};

