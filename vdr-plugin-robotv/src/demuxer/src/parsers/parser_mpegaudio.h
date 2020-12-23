/*
 *      vdr-plugin-robotv - roboTV server plugin for VDR
 *
 *      Copyright (C) 2016 Alexander Pipelka
 *
 *      https://github.com/pipelka/vdr-plugin-robotv
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
 *  along with this program; if not, write to the Free Software
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#ifndef ROBOTV_DEMUXER_MPEGAUDIO_H
#define ROBOTV_DEMUXER_MPEGAUDIO_H

#include "parser.h"

// --- ParserMpeg2Audio -------------------------------------------------

class ParserMpeg2Audio : public Parser {
public:

    ParserMpeg2Audio(TsDemuxer* demuxer);

protected:

    bool checkAlignmentHeader(unsigned char* buffer, int& framesize, bool parse);

private:

    bool parseAudioHeader(uint8_t* buffer, int& channels, int& samplerate, int& bitrate, int& framesize);

};

#endif // ROBOTV_DEMUXER_MPEGAUDIO_H
