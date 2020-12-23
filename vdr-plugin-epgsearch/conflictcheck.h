/*                                                                  -*- c++ -*-
Copyright (C) 2004-2013 Christian Wieninger

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html

The author can be reached at cwieninger@gmx.de

The project's page is at http://winni.vdr-developer.org/epgsearch
*/

#ifndef __EPGSEARCHCONFLCH_H
#define __EPGSEARCHCONFLCH_H

#include "epgsearchtools.h"
#include <vdr/diseqc.h>
#include <vector>
#include <set>
#include <list>

#define DO_MULTIPLE_RECORDINGS 1
#define DO_REC_AND_PLAY_ON_PRIMARY_DEVICE 1

class cConflictCheckTime;
class TimerObjSort;

// --- cConflictCheckTimerObj --------------------------------------------------------
class cConflictCheckTimerObj : public cTimerObj
{
    const cEvent* event;
public:
    time_t start;
    time_t stop;
    int device;
    int origIndex;
    int recDuration;
    time_t lastRecStart;
    time_t lastRecStop;
    cConflictCheckTime* conflCheckTime;
    std::set<cConflictCheckTimerObj*, TimerObjSort>* concurrentTimers;
    bool ignore;


    cConflictCheckTimerObj(cTimer* Timer, time_t Start, time_t Stop, int Device = -1, int OrigIndex = -1);
    ~cConflictCheckTimerObj();
    int Compare(const cListObject &ListObject) const;
    const cEvent* Event();
    const cEvent* SetEventFromSchedule();
    int Matches(const cEvent *Event, int *Overlap) const;
    const cTimer* OrigTimer(const cTimers* timers) const {
        return timers->GetById(timer->Id(), timer->Remote());
    }
    cTimer* OrigTimer(cTimers* timers) {
        return timers->GetById(timer->Id(), timer->Remote());
    }
};

class TimerObjSort
{
public:
    bool operator()(cConflictCheckTimerObj* a, cConflictCheckTimerObj* b) {
        return (a->Compare(*b) < 0);
    }
};


// --- cConflictCheckTime --------------------------------------------------------
class cConflictCheckTime : public cListObject
{
public:
    time_t evaltime;
    std::set<cConflictCheckTimerObj*, TimerObjSort> startingTimers;
    std::set<cConflictCheckTimerObj*, TimerObjSort> stoppingTimers;
    std::set<cConflictCheckTimerObj*, TimerObjSort> failedTimers;
    std::set<cConflictCheckTimerObj*, TimerObjSort> concurrentRecs;
    bool ignore;

    cConflictCheckTime(time_t EvalTime) : evaltime(EvalTime), ignore(false) {}
    int Compare(const cListObject &ListObject) const {
        cConflictCheckTime *p = (cConflictCheckTime *)&ListObject;
        return evaltime - p->evaltime;
    }
};

// --- cConflictCheckDevice --------------------------------------------------------
// This class tries to emulate the behaviour of a DVB device
// NOTE: The case device == NULL is only for debugging purposes
class cConflictCheckDevice
{
public:
    std::set<cConflictCheckTimerObj*, TimerObjSort> recTimers;
    cDevice* device;
    int devicenr;
    std::vector<cConflictCheckDevice*> bondedDevices;

    cConflictCheckDevice() {
        device = NULL;
        devicenr = 0;
    }
    int Priority() const {
        int prio = -1;
        for (std::set<cConflictCheckTimerObj*, TimerObjSort>::iterator it = recTimers.begin(); it != recTimers.end(); ++it)
            prio = std::max(prio, (*it)->timer->Priority());
        return prio;
    };
    int CardIndex(void) const {
        if (device) return device->CardIndex();
        else return devicenr;
    }
    bool Receiving() const {
        return !recTimers.empty();
    }
    bool IsTunedTo(const cChannel* Channel) const {
        for (std::set<cConflictCheckTimerObj*, TimerObjSort>::iterator it = recTimers.begin(); it != recTimers.end(); ++it)
            if ((*it)->timer->Channel()->Source() == Channel->Source() &&
                (*it)->timer->Channel()->Transponder() == Channel->Transponder())
                return true;
        return false;
    }
    bool HasDecoder() const {
        if (device) return device->HasDecoder();
        else return (devicenr == 3);
    }
    bool HasCi() const {
        if (device) return device->HasCi();
        else return (devicenr == 3);
    }
    bool IsPrimaryDevice() const {
        if (device) return device->IsPrimaryDevice();
        else return (devicenr == 3);
    }
    bool ProvidesSource(int Source) const {
        if (device) return device->ProvidesSource(Source);
        else {
//      int type = Source & cSource::st_Mask;
//      if (devicenr == 0) return type == cSource::stCable;
//      if (devicenr > 0) return type == cSource::stTerr;
//      return false;
            return true;
        }
    }
    cCamSlot *CamSlot(void) const {
        if (device) return device->CamSlot();
        else return NULL;
    }
    int Ca() const {
        for (std::set<cConflictCheckTimerObj*, TimerObjSort>::iterator it = recTimers.begin(); it != recTimers.end(); ++it)
            return (*it)->timer->Channel()->Ca();
        return 0;
    }
    bool HasPid(int Pid) const {
        return true;
    }
    bool ProvidesChannel(const cChannel *Channel, int Priority = -1, bool *NeedsDetachReceivers = NULL) const {
        bool result = false;
        bool hasPriority = Priority < 0 || Priority > this->Priority();
        bool needsDetachReceivers = false;

        if (ProvidesSource(Channel->Source())) {
            result = hasPriority;
            if (Priority >= 0 && Receiving()) {
                if (IsTunedTo(Channel)) {
                    if ((Channel->Vpid() && !HasPid(Channel->Vpid())) || (Channel->Apid(0) && !HasPid(Channel->Apid(0)))) {
#ifdef DO_MULTIPLE_RECORDINGS
                        if (CamSlot() && Channel->Ca() >= CA_ENCRYPTED_MIN) {
                            if (CamSlot()->CanDecrypt(Channel))
                                result = true;
                            else
                                needsDetachReceivers = true;
                        } else if (!IsPrimaryDevice())
                            result = true;
#ifdef DO_REC_AND_PLAY_ON_PRIMARY_DEVICE
                        else
                            result = Priority >= 0;
#endif
#endif
                    } else
                        result = !IsPrimaryDevice() || Priority >= 0;
                } else
                    needsDetachReceivers = true;
            }
        }
        if (result) {
            if (!BondingOk(Channel)) {
                // This device is bonded, so we need to check the priorities of the others:
                for (size_t i = 0; i < bondedDevices.size(); i++) {
                    if (bondedDevices[i]->Priority() >= Priority) {
                        LogFile.Log(3, "Attached receiver to bonded device %i has higher priority.", bondedDevices[i]->CardIndex() + 1);
                        result = false;
                        break;
                    }
                }
                if (result)
                    LogFile.Log(3, "Bonding ok, but detaches receiver on device %i.", CardIndex());
                else
                    LogFile.Log(3, "Bonding not okay on device %i.", CardIndex());
                needsDetachReceivers = Receiving();
            } else {
                LogFile.Log(3, "Bonding ok on device %i.", CardIndex());
            }
        }
        if (NeedsDetachReceivers)
            *NeedsDetachReceivers = needsDetachReceivers;
        return result;
    }

    bool BondingOk(const cChannel *Channel) const {
        if (bondedDevices.empty())
            return true;

        LogFile.Log(3, "Checking for bonding constraints on device %i", CardIndex() + 1);

        cString BondingParams = GetBondingParams(Channel);
        for (size_t i = 0; i < bondedDevices.size(); i++) {
            // bonding not okay, if a bonded devices records on another polarization or freq. band
            if (!bondedDevices[i]->recTimers.empty()) {
                if (strcmp(BondingParams, GetBondingParams((*bondedDevices[i]->recTimers.begin())->timer->Channel())) != 0) {
                    LogFile.Log(3, "Bonded device %i has receiver attached. Not safe to use device.", bondedDevices[i]->CardIndex() + 1);
                    return false;
                } else {
                    LogFile.Log(3, "Bonded device %i has receiver attached but its safe.", bondedDevices[i]->CardIndex() + 1);
                }
            } else {
                LogFile.Log(3, "Bonded device %i has no receivers attached - ok.", bondedDevices[i]->CardIndex() + 1);
            }
        }
        return true;
    }

    cString GetBondingParams(const cChannel *Channel) const { //copied from cDVBTuner
        cDvbTransponderParameters dtp(Channel->Parameters());
        if (Setup.DiSEqC) {
            if (const cDiseqc *diseqc = Diseqcs.Get(device->CardIndex() + 1, Channel->Source(), Channel->Frequency(), dtp.Polarization(), NULL))
                return diseqc->Commands();
        } else {
            bool ToneOff = Channel->Frequency() < Setup.LnbSLOF;
            bool VoltOff = dtp.Polarization() == 'V' || dtp.Polarization() == 'R';
            return cString::sprintf("%c %c", ToneOff ? 't' : 'T', VoltOff ? 'v' : 'V');
        }
        return "";
    }
};

// --- cConflictCheck --------------------------------------------------------
class cConflictCheck
{
    cList<cConflictCheckTimerObj>* timerList;
    cList<cConflictCheckTime>* evaltimeList;
    cList<cConflictCheckTime>* failedList;
    std::set<cConflictCheckTimerObj*> pendingTimers;
    cConflictCheckDevice *devices;

    int numDevices;
    time_t maxCheck;
    std::vector<eModuleStatus> camSlotStatusArray;
public:
    int relevantConflicts;
    int numConflicts;
    bool localConflicts;
    time_t nextRelevantConflictDate;

    cConflictCheck();
    ~cConflictCheck();
    void InitDevicesInfo();
    void Check();
    void BondDevices(const char* bondings);
    void SetLocal() {
        localConflicts = true;
    }
    bool GetLocal() {
        return localConflicts;
    }
    cList<cConflictCheckTimerObj>* CreateCurrentTimerList();
    cList<cConflictCheckTime>* CreateEvaluationTimeList(cList<cConflictCheckTimerObj>*);
    cList<cConflictCheckTime>* CreateConflictList(cList<cConflictCheckTime>*, cList<cConflictCheckTimerObj>* timerList);
    cList<cConflictCheckTime>* CreateRemoteConflictList(cList<cConflictCheckTimerObj>* timerList, cList<cConflictCheckTime>* failedList);
    int GetDevice(cConflictCheckTimerObj* TimerObj, bool *NeedsDetachReceivers);
    cList<cConflictCheckTime>* GetFailed() {
        return failedList;
    }
    cList<cConflictCheckTimerObj>* GetTimers() {
        return timerList;
    }
    void AddConflict(cConflictCheckTimerObj* TimerObj, cConflictCheckTime* Checktime, std::set<cConflictCheckTimerObj*>& pendingTimers);
    int ProcessCheckTime(cConflictCheckTime* checkTime);
    bool TimerInConflict(const cTimer*);
    void EvaluateConflCheckCmd();
    eModuleStatus CamSlotModuleStatus(cCamSlot *CamSlot);
};

#endif
