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

#include "timercontroller.h"
#include "robotv/robotvclient.h"
#include "tools/hash.h"
#include "vdr/menu.h"
#include "service/epgsearch/services.h"

#include <regex>
#include <vdr/plugin.h>

Epgsearch_services_v1_0* getEpgServiceData() {
    cPlugin* plugin = cPluginManager::GetPlugin("epgsearch");

    if(plugin == nullptr) {
        esyslog("unable to connect to 'epgsearch plugin !");
        return nullptr;
    }

    auto serviceData = new Epgsearch_services_v1_0;

    if(!plugin->Service("Epgsearch-services-v1.0", serviceData)) {
        esyslog("unable to get 'Epgsearch_services_v1_0' from plugin.");
        return nullptr;
    }

    return serviceData;
}

TimerController::TimerController(RoboTvClient* parent) : m_parent(parent) {
}

TimerController::TimerController(const TimerController& orig) {
}

MsgPacket* TimerController::process(MsgPacket* request) {
    switch(request->getMsgID()) {
        case ROBOTV_TIMER_GET:
            return processGet(request);

        case ROBOTV_TIMER_GETLIST:
            return processGetTimers(request);

        case ROBOTV_SEARCHTIMER_GETLIST:
            return processGetSearchTimers(request);

        case ROBOTV_TIMER_ADD:
            return processAdd(request);

        case ROBOTV_SEARCHTIMER_ADD:
            return processAddSearchTimer(request);

        case ROBOTV_TIMER_DELETE:
            return processDelete(request);

        case ROBOTV_SEARCHTIMER_DELETE:
            return processDeleteSearchTimer(request);

        case ROBOTV_TIMER_UPDATE:
            return processUpdate(request);

        default:
            break;
    }

    return nullptr;
}

void TimerController::timer2Packet(const cTimer* timer, MsgPacket* p) {
    Utf8Conv toUtf8;
    int flags = checkTimerConflicts(timer);
    const char* fileName = timer->File();
    const char* aux = timer->Aux();
    auto searchTimerId = (uint32_t)-1;

    // we have auxillary timer data ?
    if(aux != nullptr) {
        // search for epgsearch id marker "<s-id>"
        std::string auxString = std::string(aux);
        std::size_t p1 = auxString.find("<s-id>");
        std::size_t p2 = auxString.find("</s-id>");

        if(p1 != std::string::npos && p2 != std::string::npos && (p2 - p1) > 6) {
            p1 += 6; // "<s-id>" length
            std::string id = auxString.substr(p1, p2 - p1);
            searchTimerId = (uint32_t)strtol(id.c_str(), nullptr, 10);
        }
    }

    // get channel / logo
    auto channel = timer->Channel();
    std::string logoUrl = ChannelController::createLogoUrl(channel);

    cRecordControl* rc = cRecordControls::GetRecordControl(timer);
    if(rc != nullptr) {
        fileName = rc->FileName();
    }

    p->put_U32(roboTV::Hash::createTimerUid(timer));
    p->put_U32(timer->Flags() | flags);
    p->put_U32((uint32_t)timer->Priority());
    p->put_U32((uint32_t)timer->Lifetime());
    p->put_U32(roboTV::Hash::createChannelUid(channel));
    p->put_U32((uint32_t)timer->StartTime());
    p->put_U32((uint32_t)timer->StopTime());
    p->put_U32(searchTimerId); // !! day changed to searchTimerId
    p->put_U32(roboTV::Hash::createStringHash(fileName)); // !!! weekdays changed to recording id
    p->put_String(toUtf8.convert(logoUrl)); // !!! filename changed to logo url

    if(p->getProtocolVersion() >= 8) {
        p->put_String(MovieController::folderFromName(timer->File()));
    }

    // get timer event
    const cEvent* event = findEvent(timer);

    if(event == nullptr) {
        p->put_U8(0);
        return;
    }

    // add event
    p->put_U8(1);

    event2Packet(event, p);
}

void TimerController::event2Packet(const cEvent* event, MsgPacket* p) {
    Utf8Conv toUtf8;

    p->put_U32(event->EventID());
    p->put_U32((uint32_t)event->StartTime());
    p->put_U32((uint32_t)event->Duration());

    if(p->getProtocolVersion() >= 8) {
        p->put_U32((uint32_t)event->Vps());
    }

    int i = 0;

    for(;;) {
        uchar c = event->Contents(i++);
        p->put_U8(c);

        if(c == 0) {
            break;
        }
    }

    p->put_U32((uint32_t)event->ParentalRating());
    p->put_String(toUtf8.convert(!isempty(event->Title()) ? event->Title() : ""));
    p->put_String(toUtf8.convert(!isempty(event->ShortText()) ? event->ShortText() : ""));
    p->put_String(toUtf8.convert(!isempty(event->Description()) ? event->Description() : ""));
}

MsgPacket* TimerController::processGet(MsgPacket* request) { /* OPCODE 81 */
    uint32_t number = request->get_U32();
    MsgPacket* response = createResponse(request);

    LOCK_TIMERS_READ;

    if(Timers->Count() == 0) {
        response->put_U32(ROBOTV_RET_DATAUNKNOWN);
        return response;
    }

    const cTimer* timer = Timers->Get(number - 1);

    if(timer == nullptr) {
        response->put_U32(ROBOTV_RET_DATAUNKNOWN);
        return response;
    }

    response->put_U32(ROBOTV_RET_OK);
    timer2Packet(timer, response);

    return response;
}

MsgPacket* TimerController::processGetTimers(MsgPacket* request) {
    MsgPacket* response = createResponse(request);

    LOCK_TIMERS_READ;

    int numTimers = Timers->Count();

    response->put_U32((uint32_t)numTimers);

    for(int i = 0; i < numTimers; i++) {
        auto timer = Timers->Get(i);

        if(!timer) {
            continue;
        }

        timer2Packet(timer, response);
    }

    return response;
}

MsgPacket* TimerController::processGetSearchTimers(MsgPacket* request) {
    auto toInt = [](const std::string& s) {
        return strtol(s.c_str(), nullptr, 10);
    };

    auto service = getEpgServiceData();
    MsgPacket* response = createResponse(request);

    if(service == nullptr) {
        response->put_U32(ROBOTV_RET_ERROR);
        return response;
    }

    response->put_U32(ROBOTV_RET_OK);
    std::list<std::string> list = service->handler->SearchTimerList();

    std::regex r(":");

    LOCK_CHANNELS_READ;

    for(auto t: list) {
        // split the timer line into chunks delimited by ":"
        std::sregex_token_iterator first{t.begin(), t.end(), r, -1}, last;
        std::vector<std::string> timer = {first, last};

        // EXAMPLE
        // 39:Star Wars| The Clone Wars:0:::1:S19.2E-133-12-126:0:1:1:1:1:0:::1:0:0:1:Serien:50:99:2:10:1:0:0::1:0:1:1:0:0:0:0:0:1:0:0::1:0:0:0:0:0:0:0:0:0:90::0

        // the following items are interesting for us
        //  0 - unique search timer id
        //  1 - the search term (will be the tv-show name)
        //  5 - use channel (1 - interval);
        //  6 - channel id
        //  8 - search mode (0/1/2/3/4/5, 0 - search the whole term)
        // 15 - use as search timer (0/1)
        // 18 - use series recordings (0/1)
        // 19 - directory for recordings
        // 22 - time margin for start in minutes
        // 23 - time margin for stop in minutes
        // 24 - use VPS (0/1)
        // 25 - action (0 - create a timer / 2 - announce only))
        // 28 - avoid repeats (0/1)
        // 30 - compare title
        // 31 - compare subtitle

        // for the complete documentation refer to:
        // http://winni.vdr-developer.org/epgsearch/en/epgsearch.4.html#2__the_format_of_epgsearch_conf

        // skip non-search timer entries
        bool isSearchTimer = (toInt(timer[15]) > 0);
        if(!isSearchTimer) {
            continue;
        }

        // get channel / logo
        const cChannel* channel = Channels->GetByChannelID(tChannelID::FromString(timer[6].c_str()));
        std::string logoUrl = ChannelController::createLogoUrl(channel);

        // replace '|' with ':'
        std::string name = timer[1];
        std::replace(name.begin(), name.end(), '|', ':');

        response->put_U32((uint32_t)toInt(timer[0]));   // timer id
        response->put_String(name.c_str());             // search term (tv show name)
        response->put_U32(roboTV::Hash::createChannelUid(channel));   // channel uid
        response->put_String(channel->Name());          // channel name
        response->put_U32((uint32_t)toInt(timer[18]));  // series recording
        response->put_String(timer[19].c_str());        // folder
        response->put_String(logoUrl.c_str());          // channel logo
        response->put_String(t.c_str());                // timer definition
    }

    delete service;

    response->compress(9);
    return response;
}

MsgPacket* TimerController::processAdd(MsgPacket* request) {
    MsgPacket* response = createResponse(request);

    request->get_U32(); // index unused
    uint32_t flags      = request->get_U32();
    uint32_t priority   = request->get_U32();
    uint32_t lifetime   = request->get_U32();
    uint32_t channelid  = request->get_U32();
    time_t startTime    = request->get_U32();
    time_t stopTime     = request->get_U32();
    time_t day          = request->get_U32();
    uint32_t weekdays   = request->get_U32();
    const char* file    = request->get_String();
    const char* aux     = request->get_String();

    // handle instant timers
    if(startTime == -1 || startTime == 0) {
        startTime = time(nullptr);
    }

    struct tm tm_r{};

    struct tm* time = localtime_r(&startTime, &tm_r);

    if(day <= 0) {
        day = cTimer::SetTime(startTime, 0);
    }

    int start = time->tm_hour * 100 + time->tm_min;
    time = localtime_r(&stopTime, &tm_r);
    int stop = time->tm_hour * 100 + time->tm_min;

    cString buffer;

    LOCK_TIMERS_WRITE;
    LOCK_CHANNELS_READ;
    LOCK_SCHEDULES_READ;

    const cChannel *channel = roboTV::Hash::findChannelByUid(Channels, channelid);

    if (channel == nullptr) {
        esyslog("channel with id '%i' not found - unable to add timer !", channelid);
        response->put_U32(ROBOTV_RET_DATAINVALID);
        return response;
    }

    buffer = cString::sprintf("%u:%s:%s:%04d:%04d:%d:%d:%s:%s\n", flags,
                              (const char *) channel->GetChannelID().ToString(),
                              *cTimer::PrintDay(day, weekdays, true), start, stop, priority, lifetime, file, aux);

    // replace invalid characters in file
    auto* p = (char*)file;

    while(*p) {
        if(*p == ' ' || *p == ':') {
            *p = '_';
        }

        p++;
    }

    auto timer = new cTimer;

    if(timer->Parse(buffer)) {
        const cTimer* t = Timers->GetTimer(timer);

        if(t == nullptr) {

            // check for conflicts
            int timerFlags = checkTimerConflicts(timer);
            if(timerFlags > 2048) {
                isyslog("Timer %s has conflicts - unable to add", *timer->ToDescr());
                response->put_U32(ROBOTV_RET_TIMER_CONFLICT);
                delete timer;
                return response;
            }

            timer->SetEventFromSchedule(Schedules);

            Timers->Add(timer);
            Timers->SetModified();

            isyslog("Timer %s added", *timer->ToDescr());
            response->put_U32(ROBOTV_RET_OK);
            return response;
        }
        else {
            esyslog("Timer already defined: %d %s", t->Index() + 1, *t->ToText());
            response->put_U32(ROBOTV_RET_DATALOCKED);
        }
    }
    else {
        esyslog("Error in timer settings");
        response->put_U32(ROBOTV_RET_DATAINVALID);
    }

    delete timer;

    return response;
}

MsgPacket* TimerController::processAddSearchTimer(MsgPacket* request) {

    // get channelUid and search term from message

    uint32_t uid = request->get_U32();
    std::string term = request->get_String();

    MsgPacket* response = createResponse(request);

    LOCK_CHANNELS_READ;

    // check if channel is valid

    const cChannel *channel = roboTV::Hash::findChannelByUid(Channels, uid);

    if(channel == nullptr) {
        response->put_U32(ROBOTV_RET_DATAUNKNOWN);
        return response;
    }

    // translate term
    std::string searchTerm;
    for(auto& c: term) {
        searchTerm += (c == ':') ? '|' : c;
    }

    // search term must not be empty

    if(searchTerm.empty()) {
        response->put_U32(ROBOTV_RET_DATAINVALID);
        return response;
    }

    // check for 'epgsearch' service

    auto service = getEpgServiceData();

    if(service == nullptr) {
        response->put_U32(ROBOTV_RET_ERROR);
        return response;
    }

    RoboTVServerConfig& config = RoboTVServerConfig::instance();

    // assemble search timer string
    // http://winni.vdr-developer.org/epgsearch/en/epgsearch.4.html#2__the_format_of_epgsearch_conf

    // the following items are interesting for us
    //  0 - unique search timer id
    //  1 - the search term (will be the tv-show name)
    //  5 - use channel (1 - interval);
    //  6 - channel id
    //  8 - search mode (0/1/2/3/4/5, 0 - search the whole term)
    // 15 - use as search timer (0/1)
    // 18 - use series recordings (0/1)
    // 19 - directory for recordings
    // 20 - priority
    // 22 - time margin for start in minutes
    // 23 - time margin for stop in minutes
    // 24 - use VPS (0/1)
    // 25 - action (0 - create a timer / 2 - announce only))
    // 28 - avoid repeats (0/1)
    // 30 - compare title
    // 31 - compare subtitle
    // 32 - compare description

    std::string searchTimer = *cString::sprintf(
        "%i:%s:0:::%i:%s:0:%i:1:0:0:0:::%i:0:0:%i:%s:%i:99:%i:%i:%i:%i:0::%i:0:%i:%i:%i:0:0:0:0:1:0:0::1:0:0:0:0:0:0:0:%i:0:%i",
        0,                                   // search timer id (0 -> new timer)
        searchTerm.c_str(),                  // the search term
        1,                                   // use channel (1 - interval)
        *channel->GetChannelID().ToString(), // channel id
        3,                                   // search mode (0 - search the whole term / 3 - matches exactly)
        1,                                   // use as search timer
        1,                                   // use series recordings
        config.seriesFolder.c_str(),         // directory for recordings
        80,                                  // priority
        2,                                   // time margin for start in minutes
        10,                                  // time margin for stop in minutes
        0,                                   // use VPS
        0,                                   // action create timer
        1,                                   // avoid repeats
        1,                                   // compare title
        1,                                   // compare subtitle
        1,                                   // compare description
        1,                                   // ignore missing epg categories
        100                                  // the minimum required match in percent when descriptions are compared to avoid repeats
        );

    if(searchTimer.empty()) {
        esyslog("processAddSearchTimer - timer definiton is empty !");
        response->put_U32(ROBOTV_RET_ERROR);
        return response;
    }

    dsyslog("add search timer: '%s'", searchTimer.c_str());

    int searchTimerId = service->handler->AddSearchTimer(searchTimer);

    if(searchTimerId == -1) {
        response->put_U32(ROBOTV_RET_NOTSUPPORTED);
        return response;
    }

    response->put_U32(ROBOTV_RET_OK);
    response->put_U32((uint32_t)searchTimerId);

    return response;
}

MsgPacket* TimerController::processDelete(MsgPacket* request) {
    uint32_t uid = request->get_U32();

    LOCK_TIMERS_WRITE;

    cTimer* timer = roboTV::Hash::findTimerByUid(Timers, uid);
    MsgPacket* response = createResponse(request);

    if(timer == nullptr) {
        esyslog("Unable to delete timer - invalid timer identifier");
        response->put_U32(ROBOTV_RET_DATAINVALID);
        return response;
    }

    timer->Skip();

    cRecordControls::Process(Timers, time(nullptr));

    isyslog("Deleting timer %s", *timer->ToDescr());
    Timers->Del(timer);
    Timers->SetModified();
    response->put_U32(ROBOTV_RET_OK);

    return response;
}

MsgPacket* TimerController::processDeleteSearchTimer(MsgPacket* request) {

    // get search timer id

    int searchTimerId = request->get_U32();

    // create response

    auto response = createResponse(request);

    // check for 'epgsearch' service

    auto service = getEpgServiceData();

    if(service == nullptr) {
        response->put_U32(ROBOTV_RET_ERROR);
        return response;
    }

    // let 'epgsearch' remove the search timer

    if(!service->handler->DelSearchTimer(searchTimerId)) {
        response->put_U32(ROBOTV_RET_DATAUNKNOWN);
    }

    response->put_U32(ROBOTV_RET_OK);

    return response;
}

MsgPacket* TimerController::processUpdate(MsgPacket* request) {
    MsgPacket* response = createResponse(request);

    uint32_t uid        = request->get_U32();
    uint32_t flags      = request->get_U32();
    uint32_t priority   = request->get_U32();
    uint32_t lifetime   = request->get_U32();
    uint32_t channelid  = request->get_U32();
    time_t startTime    = request->get_U32();
    time_t stopTime     = request->get_U32();
    time_t day          = request->get_U32();
    uint32_t weekdays   = request->get_U32();
    const char* file    = request->get_String();
    const char* aux     = request->get_String();

    struct tm tm_r{};
    struct tm* time = localtime_r(&startTime, &tm_r);

    if(day <= 0) {
        day = cTimer::SetTime(startTime, 0);
    }

    int start = time->tm_hour * 100 + time->tm_min;
    time = localtime_r(&stopTime, &tm_r);
    int stop = time->tm_hour * 100 + time->tm_min;

    cString buffer;

    LOCK_TIMERS_WRITE;
    LOCK_CHANNELS_READ;

    const cChannel *channel = roboTV::Hash::findChannelByUid(Channels, channelid);

    if (channel == nullptr) {
        esyslog("channel with id '%i' not found - unable to update timer !", channelid);
        response->put_U32(ROBOTV_RET_DATAINVALID);
        return response;
    }

    buffer = cString::sprintf(
            "%u:%s:%s:%04d:%04d:%d:%d:%s:%s\n",
            flags,
            (const char*)channel->GetChannelID().ToString(),
            *cTimer::PrintDay(day, weekdays, true),
            start,
            stop,
            priority,
            lifetime,
            file,
            aux);

    cTimer* timer = roboTV::Hash::findTimerByUid(Timers, uid);
    if(timer == nullptr) {
        esyslog("Timer not defined");
        response->put_U32(ROBOTV_RET_DATAUNKNOWN);
        return response;
    }

    if(timer->Recording()) {
        isyslog("Will not update timer - currently recording");
        response->put_U32(ROBOTV_RET_OK);
        return response;
    }

    cTimer t = *timer;

    if(!t.Parse(buffer)) {
        esyslog("Error in timer settings");
        response->put_U32(ROBOTV_RET_DATAINVALID);
        return response;
    }

    // check for conflicts
    int timerFlags = checkTimerConflicts(&t);
    if(timerFlags > 2048) {
        isyslog("Timer %s has conflicts - unable to update", *timer->ToDescr());
        response->put_U32(ROBOTV_RET_TIMER_CONFLICT);
        return response;
    }

    *timer = t;

    Timers->SetModified();

    response->put_U32(ROBOTV_RET_OK);

    return response;
}

int TimerController::checkTimerConflicts(const cTimer* timer) {
    // check for timer conflicts
    dsyslog("Checking conflicts for: %s", (const char*)timer->ToText(true));

    LOCK_TIMERS_READ;

    // order active timers by starttime
    std::map<time_t, const cTimer*> timeline;
    int numTimers = Timers->Count();

    for(int i = 0; i < numTimers; i++) {
        auto t = Timers->Get(i);

        // same timer -> skip
        if(!t || timer->Index() == i) {
            continue;
        }

        // timer not active -> skip
        if(!(t->Flags() & tfActive)) {
            continue;
        }

        // this one is earlier -> no match
        if(t->StopTime() <= timer->StartTime()) {
            continue;
        }

        // this one is later -> no match
        if(t->StartTime() >= timer->StopTime()) {
            continue;
        }

        timeline[t->StartTime()] = t;
    }

    std::set<int> transponders;
    transponders.insert(timer->Channel()->Transponder()); // we also count ourself
    const cTimer* to_check = timer;

    for(auto& i : timeline) {
        auto t = i.second;

        // this one is earlier -> no match
        if(t->StopTime() <= to_check->StartTime()) {
            continue;
        }

        // this one is later -> no match
        if(t->StartTime() >= to_check->StopTime()) {
            continue;
        }

        // same transponder -> no conflict
        if(t->Channel()->Transponder() == to_check->Channel()->Transponder()) {
            continue;
        }

        // different source -> no conflict
        if(t->Channel()->Source() != to_check->Channel()->Source()) {
            continue;
        }

        dsyslog("Possible conflict: %s", (const char*)t->ToText(true));
        transponders.insert(t->Channel()->Transponder());

        // now check conflicting timer
        to_check = t;
    }

    uint32_t number_of_devices_for_this_channel = 0;

    for(int i = 0; i < cDevice::NumDevices(); i++) {
        cDevice* device = cDevice::GetDevice(i);

        if(device != nullptr && device->ProvidesTransponder(timer->Channel())) {
            number_of_devices_for_this_channel++;
        }
    }

    int cflags = 0;

    if(transponders.size() > number_of_devices_for_this_channel) {
        dsyslog("ERROR - Not enough devices");
        cflags += 2048;
    }
    else if(transponders.size() > 1) {
        dsyslog("Overlapping timers - Will record");
        cflags += 1024;
    }
    else {
        dsyslog("No conflicts");
    }

    return cflags;
}

const cEvent *TimerController::findEvent(const cTimer *timer) {
    auto event = timer->Event();

    if(event != nullptr) {
        return event;
    }

    LOCK_SCHEDULES_READ;

    if(Schedules == nullptr) {
        return nullptr;
    }

    const cSchedule* schedule = Schedules->GetSchedule(timer->Channel()->GetChannelID());

    for(event = schedule->Events()->First(); event; event = schedule->Events()->Next(event)) {
        // VPS timer
        if((timer->Flags() & tfVps) && event->Vps() == timer->StartTime()) {
            return event;
        }

        // regular timer
        time_t eventStopTime = event->StartTime() + event->Duration();

        if(event->StartTime() >= timer->StartTime() && eventStopTime <= timer->StopTime()) {
            return event;
        }
    }

    return nullptr;
}
