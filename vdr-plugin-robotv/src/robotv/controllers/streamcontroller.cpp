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

#include <chrono>
#include <tools/time.h>
#include "streamcontroller.h"
#include "robotv/robotvclient.h"
#include "tools/hash.h"

StreamController::StreamController(RoboTvClient* parent) :
    m_langStreamType(StreamInfo::Type::AC3),
    m_parent(parent) {
}

StreamController::StreamController(const StreamController& orig) {
}

StreamController::~StreamController() {
    stopStreaming();
}

MsgPacket* StreamController::process(MsgPacket* request) {
    /** OPCODE 20 - 39: RoboTV network functions for live streaming */

    switch(request->getMsgID()) {
        case ROBOTV_CHANNELSTREAM_OPEN:
            return processOpen(request);

        case ROBOTV_CHANNELSTREAM_CLOSE:
            return processClose(request);

        case ROBOTV_CHANNELSTREAM_REQUEST:
            return processRequest(request);

        case ROBOTV_CHANNELSTREAM_PAUSE:
            return processPause(request);

        case ROBOTV_CHANNELSTREAM_SIGNAL:
            return processSignal(request);

        case ROBOTV_CHANNELSTREAM_SEEK:
            return processSeek(request);
    }

    return nullptr;
}

MsgPacket* StreamController::processOpen(MsgPacket* request) {
    uint32_t uid = request->get_U32();
    int32_t priority = 50;

    if(!request->eop()) {
        priority = request->get_S32();
    }

    if(!request->eop()) {
        request->get_U8(); // BINARY COMPATIBILITY (was waitForKeyFrame)
    }

    // get preferred language
    if(!request->eop()) {
        m_language = request->get_String();
        m_langStreamType = (StreamInfo::Type)request->get_U8();
    }

    if(m_langStreamType == StreamInfo::Type::NONE) {
        m_langStreamType = StreamInfo::Type::AC3;
    }

    isyslog("======================================");
    isyslog("CHANNEL STREAM REQUEST");
    isyslog("======================================");


    LOCK_TIMERS_READ;
    LOCK_CHANNELS_READ;

    MsgPacket *response = createResponse(request);
    const cChannel *channel = roboTV::Hash::findChannelByUid(Channels, uid);

    if(channel == nullptr) {
        esyslog("INVALID CHANNELUID: %08x - ABORTING", uid);
        response->put_U32(ROBOTV_RET_DATAINVALID);
        return response;
    }
    else {
        isyslog("%i - %s (%s)", channel->Number(), channel->Name(), *channel->GetChannelID().ToString());

        if (!m_language.empty()) {
            isyslog("Preferred audio track: %s - %s", StreamInfo::typeName(m_langStreamType), m_language.c_str());
        }
    }

    isyslog("--------------------------------------");

    stopStreaming();

    int status = startStreaming(channel, priority);

    if(status == ROBOTV_RET_OK) {
        isyslog("--------------------------------------");
        isyslog("Started streaming of channel %s (priority %i)", channel->Name(), priority);
    }
    else {
        time_t now = time(nullptr);

        for (const cTimer *ti = Timers->First(); ti; ti = Timers->Next(ti)) {
            if (ti->Recording() && ti->Matches(now)) {
                esyslog("Recording running !");
                status = ROBOTV_RET_RECRUNNING;
            }
        }
        esyslog("Can't stream channel %s (status: %i)", channel->Name(), status);
    }

    response->put_U32((uint32_t)status);
    return response;
}

MsgPacket* StreamController::processClose(MsgPacket* request) {
    stopStreaming();
    return createResponse(request);
}

MsgPacket* StreamController::processRequest(MsgPacket* request) {
    if(m_streamer == nullptr) {
        return nullptr;
    }

    MsgPacket* p = nullptr;

    int64_t start = roboTV::currentTimeMillis().count();

    while(p == nullptr && (roboTV::currentTimeMillis().count() - start) < 500) {
        p = m_streamer->requestPacket();

        if(p == nullptr) {
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
    }

    if(p == nullptr) {
        return createResponse(request);
    }

    return createResponse(request, p);
}

MsgPacket* StreamController::processPause(MsgPacket* request) {
    if(m_streamer == nullptr) {
        return nullptr;
    }

    bool on = request->get_U32();
    isyslog("LIVESTREAM: %s", on ? "PAUSED" : "TIMESHIFT");

    m_streamer->pause(on);

    return createResponse(request);
}

MsgPacket* StreamController::processSignal(MsgPacket* request) {
    if(m_streamer == nullptr) {
        return nullptr;
    }

    m_streamer->requestSignalInfo();
    return nullptr;
}

void StreamController::processChannelChange(const cChannel* Channel) {
    if(m_streamer != NULL) {
        m_streamer->processChannelChange(Channel);
    }
}

int StreamController::startStreaming(const cChannel* channel, int32_t priority) {
    std::lock_guard<std::mutex> lock(m_lock);

    m_streamer = new LiveStreamer(m_parent, priority);
    m_streamer->setLanguage(m_language.c_str(), m_langStreamType);

    return m_streamer->switchChannel(channel);
}

void StreamController::stopStreaming() {
    std::lock_guard<std::mutex> lock(m_lock);

    delete m_streamer;
    m_streamer = NULL;
}

MsgPacket* StreamController::processSeek(MsgPacket* request) {
    std::lock_guard<std::mutex> lock(m_lock);

    if(m_streamer == nullptr) {
        return nullptr;
    }

    int64_t position = request->get_S64();
    int64_t pts = m_streamer->seek(position);

    MsgPacket* response = createResponse(request);
    response->put_S64(pts);
    return response;
}
