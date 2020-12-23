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

#ifndef ROBOTV_TIMERCONTROLLER_H
#define ROBOTV_TIMERCONTROLLER_H

#include "vdr/timers.h"
#include "controller.h"

class RoboTvClient;
class MsgPacket;

class TimerController : public Controller {
public:

    explicit TimerController(RoboTvClient* parent);

    ~TimerController() override = default;

    MsgPacket* process(MsgPacket* request) override;

    static void event2Packet(const cEvent* event, MsgPacket* p);

    static void timer2Packet(const cTimer* timer, MsgPacket* p);

    static int checkTimerConflicts(const cTimer* timer);

private:

    MsgPacket* processGet(MsgPacket* request);

    MsgPacket* processGetTimers(MsgPacket* request);

    MsgPacket* processGetSearchTimers(MsgPacket* request);

    MsgPacket* processAdd(MsgPacket* request);

    MsgPacket* processAddSearchTimer(MsgPacket* request);

    MsgPacket* processDelete(MsgPacket* request);

    MsgPacket* processDeleteSearchTimer(MsgPacket* request);

    MsgPacket* processUpdate(MsgPacket* request);

    TimerController(const TimerController& orig);

    RoboTvClient* m_parent;

    static const cEvent *findEvent(const cTimer *pTimer);
};

#endif // ROBOTV_TIMERCONTROLLER_H

