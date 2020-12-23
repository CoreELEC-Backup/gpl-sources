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

#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include <map>

#include <vdr/recording.h>
#include <vdr/plugin.h>
#include <vdr/menu.h>
#include <recordings/recordingscache.h>

#include "robotvcommand.h"
#include "robotvclient.h"
#include "robotvserver.h"

RoboTvClient::RoboTvClient(int fd, unsigned int id) : m_id(id), m_socket(fd),
    m_streamController(this),
    m_recordingController(this),
    m_timerController(this) {

    m_controllers = {
        &m_streamController,
        &m_recordingController,
        &m_channelController,
        &m_timerController,
        &m_movieController,
        &m_loginController,
        &m_epgController,
        &m_artworkController
    };

    m_loginController.setSocket(m_socket);
    Start();
}

RoboTvClient::~RoboTvClient() {
    // shutdown connection
    shutdown(m_socket, SHUT_RDWR);
    Cancel(10);

    // close connection
    close(m_socket);

    // delete messagequeue
    {
        std::lock_guard<std::mutex> lock(m_queueLock);

        while(!m_queue.empty()) {
            MsgPacket* p = m_queue.front();
            m_queue.pop_front();
            delete p;
        }
    }

    dsyslog("done");
}

void RoboTvClient::Action(void) {
    bool bClosed(false);

    while(Running()) {

        // send pending messages
        {
            std::lock_guard<std::mutex> lock(m_queueLock);

            while(!m_queue.empty()) {
                MsgPacket* p = m_queue.front();

                if(!p->write(m_socket, m_timeout)) {
                    break;
                }

                m_queue.pop_front();
                delete p;
            }
        }

        m_request = MsgPacket::read(m_socket, bClosed, 10);

        if(bClosed) {
            delete m_request;
            m_request = NULL;
            break;
        }

        if(m_request != NULL) {
            processRequest();
            delete m_request;
        }
    }
}

void RoboTvClient::Recording(const cDevice* Device, const char* Name, const char* FileName, bool On) {
    // check if we should ignore this notification
    if(!m_loginController.statusEnabled()) {
        return;
    }

    std::string fileName = (FileName == nullptr) ? "": FileName;
    std::string name = (Name == nullptr) ? "" : Name;

    isyslog("----------------------------------");
    isyslog("RECORDINGEVENT:");
    isyslog("Client ID: %i", getId());
    isyslog("Filename:  %s", fileName.c_str());
    isyslog("Name:      %s", name.c_str());
    isyslog("Recording: %s", On ? "Yes" : "No");
    isyslog("----------------------------------");

    // execute in thread to prevent invalid locking
    std::thread t([this, fileName, On]() {
        LOCK_RECORDINGS_READ;

        auto r = Recordings->GetByName(fileName.c_str());

        if(r != nullptr) {
            const cEvent* e = r->Info()->GetEvent();
            onRecording(e, On);
        }

        // also request timers update on recording change (the status of the
        // timer changes)

        MsgPacket* resp = new MsgPacket(ROBOTV_STATUS_TIMERCHANGE, ROBOTV_CHANNEL_STATUS);
        resp->setProtocolVersion(m_loginController.protocolVersion());

        queueMessage(resp);
    });
    t.detach();
}

void RoboTvClient::TimerChange(const cTimer* Timer, eTimerChange Change) {
    // ignore invalid timers
    if(Timer == NULL || !m_loginController.statusEnabled()) {
        return;
    }

    isyslog("Sending timer change request to client #%i ...", m_id);
    MsgPacket* resp = new MsgPacket(ROBOTV_STATUS_TIMERCHANGE, ROBOTV_CHANNEL_STATUS);
    resp->setProtocolVersion(m_loginController.protocolVersion());

    if(Change == tcAdd) {
        TimerController::timer2Packet(Timer, resp);
    }

    queueMessage(resp);
}

void RoboTvClient::ChannelChange(const cChannel* Channel) {
    if(!Running()) {
        return;
    }

    m_streamController.processChannelChange(Channel);

    if(!m_loginController.statusEnabled()) {
        return;
    }

    MsgPacket* resp = new MsgPacket(ROBOTV_STATUS_CHANNELCHANGED, ROBOTV_CHANNEL_STATUS);
    m_channelController.addChannelToPacket(Channel, resp);

    queueMessage(resp);
}

void RoboTvClient::UpdateRecordings() {
    if(!m_loginController.statusEnabled()) {
        return;
    }

    MsgPacket* resp = new MsgPacket(ROBOTV_STATUS_RECORDINGSCHANGE, ROBOTV_CHANNEL_STATUS);
    queueMessage(resp);
}

void RoboTvClient::onRecording(const cEvent* event, bool on) {
    if(event == nullptr) {
        return;
    }

    MsgPacket* resp = new MsgPacket(ROBOTV_STATUS_RECORDING, ROBOTV_CHANNEL_STATUS);
    resp->setProtocolVersion(m_loginController.protocolVersion());

    resp->put_U32((uint32_t)event->Index());
    resp->put_U32(on ? 1 : 0);
    resp->put_String(m_toUtf8.convert(event->Title()));
    resp->put_String(m_toUtf8.convert(event->Description()));

    TimerController::event2Packet(event, resp);
    queueMessage(resp);
}

void RoboTvClient::sendStatusMessage(const char* Message) {
    MsgPacket* resp = new MsgPacket(ROBOTV_STATUS_MESSAGE, ROBOTV_CHANNEL_STATUS);

    resp->put_U32(0);
    resp->put_String(Message);

    queueMessage(resp);
}

bool RoboTvClient::processRequest() {

    // set protocol version for all messages
    // except login, because login defines the
    // protocol version

    if(m_request->getMsgID() != ROBOTV_LOGIN) {
        m_request->setProtocolVersion(m_loginController.protocolVersion());
    }

    for(auto i : m_controllers) {
        MsgPacket* response = i->process(m_request);
        if(response != nullptr){
            queueMessage(response);
            return true;
        }
    }

    return false;
}

void RoboTvClient::queueMessage(MsgPacket* p) {
    std::lock_guard<std::mutex> lock(m_queueLock);
    m_queue.push_back(p);
}
