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

#include <cstring>
#include <unordered_map>

#include "robotvdmx/demuxerbundle.h"
#include "robotvdmx/pes.h"

DemuxerBundle::DemuxerBundle(TsDemuxer::Listener* listener) : m_listener(listener) {
    m_pendingError = false;
}

DemuxerBundle::~DemuxerBundle() {
    clear();
}


void DemuxerBundle::clear() {
    for (auto &i : m_list) {
        delete i;
    }

    m_list.clear();
}

TsDemuxer* DemuxerBundle::findDemuxer(int Pid) const {
    for (auto i : m_list) {
        if(i != nullptr && i->getPid() == Pid) {
            return i;
        }
    }

    return nullptr;
}

void DemuxerBundle::reorderStreams(const char* lang, StreamInfo::Type type) {
    std::map<uint32_t, TsDemuxer*> weight;

    // compute weights

    for (auto stream : m_list) {
        if(stream == nullptr) {
            continue;
        }

        // 32bit weight:
        // V0000000ASLTXXXXPPPPPPPPPPPPPPPP
        //
        // VIDEO (V):      0x80000000
        // AUDIO (A):      0x00800000
        // SUBTITLE (S):   0x00400000
        // LANGUAGE (L):   0x00200000
        // STREAMTYPE (T): 0x00100000 (only audio)
        // CHANNELS (X):   0x000F0000 (only audio)
        // PID (P):        0x0000FFFF

#define VIDEO_MASK      0x80000000
#define AUDIO_MASK      0x00800000
#define SUBTITLE_MASK   0x00400000
#define LANGUAGE_MASK   0x00200000
#define STREAMTYPE_MASK 0x00100000
#define CHANNEL_MASK    0x000F0000
#define PID_MASK        0x0000FFFF

        // last resort ordering, the PID
        uint32_t w = 0xFFFF - ((uint32_t)stream->getPid() & PID_MASK);

        uint8_t channels = 0;

        if(stream->getChannels() > 6) {
            channels = 3;
        }
        else if(stream->getChannels() == 6) {
            channels = 2;
        }
        else if(stream->getChannels() == 2) {
            channels = 1;
        }

        // stream type weights
        switch(stream->getContent()) {
            case StreamInfo::Content::VIDEO:
                w |= VIDEO_MASK;
                break;

            case StreamInfo::Content::AUDIO:
                w |= AUDIO_MASK;

                // weight of audio stream type
                w |= (stream->getType() == type) ? STREAMTYPE_MASK : 0;

                // weight of channels
                w |= (channels << 16) & CHANNEL_MASK;
                break;

            case StreamInfo::Content::SUBTITLE:
                w |= SUBTITLE_MASK;
                break;

            default:
                break;
        }

        // weight of language
        w |= (strcmp(stream->getLanguage(), lang) == 0) ? LANGUAGE_MASK : 0;

        // summed weight
        weight[w] = stream;
    }

    // reorder streams on weight
    int idx = 0;
    m_list.clear();

    for(auto i = weight.rbegin(); i != weight.rend(); i++, idx++) {
        TsDemuxer* stream = i->second;
        m_list.push_back(stream);
    }
}

bool DemuxerBundle::isReady() const {
    if(m_list.empty()) {
        return false;
    }

    for (auto i : m_list) {
        if(!i->isParsed()) {
            return false;
        }
    }

    return true;
}

void DemuxerBundle::updateFrom(StreamBundle* bundle) {
    // remove old demuxers
    clear();

    // create new stream demuxers
    for (auto &i : *bundle) {
        StreamInfo& info = i.second;
        auto dmx = new TsDemuxer(m_listener, info);

        m_list.push_back(dmx);
    }
}

bool DemuxerBundle::processTsPacket(uint8_t* packet, int64_t streamPosition) {
    if(*packet != 0x47) {
        m_pendingError = true;
        return false;
    }

    if(TsError(packet) || TsIsScrambled(packet)) {
        m_pendingError = true;
        return false;
    }

    if(!TsHasPayload(packet)) {
        return false;
    }

    int pid = TsPid(packet);
    TsDemuxer* demuxer = findDemuxer(pid);

    if(demuxer == nullptr) {
        return false;
    }

    int offset = TsPayloadOffset(packet);

    if(offset < 0 || offset >= TS_SIZE) {
        return false;
    }

    bool pusi = TsPayloadStart(packet);

    // valid packet ?
    if(pusi && !PesIsHeader(&packet[offset])) {
        m_pendingError = true;
        return false;
    }

    if(m_pendingError && pusi) {
        reset();
        m_pendingError = false;
    }

    if(m_pendingError) {
        return false;
    }

    demuxer->setStreamPosition(streamPosition);
    return demuxer->processTsPacket(packet);
}

void DemuxerBundle::reset() {
    for(auto i: m_list) {
        i->reset();
    }
}
