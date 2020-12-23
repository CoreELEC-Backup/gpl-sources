/*
    Copyright 2004-2005 Chris Tallon

    This file is part of VOMP.

    VOMP is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    VOMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VOMP; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#ifndef RECPLAYER_H
#define RECPLAYER_H

#include <stdio.h>
#include <vdr/recording.h>
#include <vdr/remux.h>

#include "server/streamer.h"

class Segment
{
  public:
    uint64_t start;
    uint64_t end;
};

class RecPlayer
{
  public:
    RecPlayer(const char* FileName);
    ~RecPlayer();
    uint64_t getLengthBytes();
    uint32_t getLengthFrames();
    unsigned long getBlock(unsigned char* buffer, uint64_t position, unsigned long amount);
    int openFile(int index);
    uint64_t getLastPosition();
    cRecording* getCurrentRecording();
    const cPatPmtParser* getPatPmtData() { return parser; }
    void scan();
    uint64_t positionFromResume(int ResumeID);
    uint64_t positionFromMark(int MarkIndex);
    uint64_t positionFromTime(int Seconds);
    uint64_t positionFromPercent(int Percent);
    uint64_t positionFromFrameNumber(uint32_t frameNumber);
    uint32_t frameNumberFromPosition(uint64_t position);
    bool getNextIFrame(uint32_t frameNumber, uint32_t direction, uint64_t* rfilePosition, uint32_t* rframeNumber, uint32_t* rframeLength);

  private:
    cRecording* recording;
    cIndexFile* indexFile;
    cPatPmtParser* parser;
    FILE* file;
    int fileOpen;
    Segment* segments[1000];
    uint64_t totalLength;
    uint64_t lastPosition;
    uint32_t totalFrames;
};

#endif
