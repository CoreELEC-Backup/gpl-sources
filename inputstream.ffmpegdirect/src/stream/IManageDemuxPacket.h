/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <kodi/addon-instance/Inputstream.h>

namespace ffmpegdirect
{

class IManageDemuxPacket
{
public:
  virtual ~IManageDemuxPacket() = default;

  virtual DEMUX_PACKET* AllocateDemuxPacketFromInputStreamAPI(int dataSize) = 0;
  virtual DEMUX_PACKET* AllocateEncryptedDemuxPacketFromInputStreamAPI(int dataSize, unsigned int encryptedSubsampleCount) = 0;
  virtual void FreeDemuxPacketFromInputStreamAPI(DEMUX_PACKET* packet) = 0;
};

} //namespace ffmpegdirect
