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

#include "recordingcontroller.h"
#include "recordings/packetplayer.h"
#include "recordings/recordingscache.h"
#include "robotv/robotvclient.h"
#include "tools/recid2uid.h"

RecordingController::RecordingController(RoboTvClient* parent) : m_parent(parent), m_recPlayer(NULL) {
}

RecordingController::RecordingController(const RecordingController& orig) {
}

RecordingController::~RecordingController() {
    delete m_recPlayer;
}

MsgPacket* RecordingController::process(MsgPacket* request) {
    switch(request->getMsgID()) {
        case ROBOTV_RECSTREAM_OPEN:
            return processOpen(request);

        case ROBOTV_RECSTREAM_CLOSE:
            return processClose(request);

        case ROBOTV_RECSTREAM_REQUEST:
            return processRequest(request);

        case ROBOTV_RECSTREAM_SEEK:
            return processSeek(request);

        case ROBOTV_RECSTREAM_PAUSE:
            return processPause(request);
    }

    return nullptr;
}

MsgPacket* RecordingController::processOpen(MsgPacket* request) {
    const char* recid = request->get_String();
    unsigned int uid = recid2uid(recid);
    dsyslog("lookup recid: %s (uid: %u)", recid, uid);

    LOCK_RECORDINGS_READ;

    auto recording = RecordingsCache::instance().lookup(Recordings, uid);
    auto response = createResponse(request);

    if(recording && m_recPlayer == NULL) {
        m_recPlayer = new PacketPlayer(recording);

        delete m_recPlayer->requestPacket();
        m_recPlayer->reset();

        uint32_t length = (uint32_t)(m_recPlayer->endTime().count() - m_recPlayer->startTime().count()) / 1000;

        response->put_U32(ROBOTV_RET_OK);
        response->put_U32(0);
        response->put_U64(m_recPlayer->getLengthBytes());
        response->put_U8(recording->IsPesRecording());//added for TS
        response->put_U32(length);

    }
    else {
        response->put_U32(ROBOTV_RET_DATAUNKNOWN);
        esyslog("%s - unable to start recording !", __FUNCTION__);
    }

    return response;
}

MsgPacket* RecordingController::processClose(MsgPacket* request) {
    if(m_recPlayer) {
        delete m_recPlayer;
        m_recPlayer = NULL;
    }

    MsgPacket* response = createResponse(request);
    response->put_U32(ROBOTV_RET_OK);
    return response;
}

MsgPacket* RecordingController::processRequest(MsgPacket* request) {
    if(!m_recPlayer) {
        return nullptr;
    }

    MsgPacket* p = m_recPlayer->requestPacket();

    if(p == nullptr) {
        return createResponse(request);
    }

    return createResponse(request, p);
}

MsgPacket* RecordingController::processSeek(MsgPacket* request) {
    if(m_recPlayer == nullptr) {
        return nullptr;
    }

    int64_t position = request->get_S64();
    int64_t pts = m_recPlayer->seek(position);

    MsgPacket* response = createResponse(request);
    response->put_S64(pts);
    return response;
}

MsgPacket* RecordingController::processPause(MsgPacket* request) {
    if(m_recPlayer == nullptr) {
        return nullptr;
    }

    return createResponse(request);
}
