/*
 *  $Id: common.h,v 1.16 2010/07/19 13:49:24 schmirl Exp $
 */
 
#ifndef VDR_STREAMDEV_COMMON_H
#define VDR_STREAMDEV_COMMON_H

/* FreeBSD has it's own version of isnumber(),
   but VDR's version is incompatible */
#ifdef __FreeBSD__
#undef isnumber
#endif

#include <vdr/tools.h>
#include <vdr/plugin.h>

#include "tools/socket.h"

#ifdef DEBUG
#include <stdio.h>
#include <time.h>
#define Dprintf(fmt, x...) {\
	struct timespec ts;\
	clock_gettime(CLOCK_MONOTONIC, &ts);\
	fprintf(stderr, "%ld.%.3ld [%d] "fmt,\
		ts.tv_sec, ts.tv_nsec / 1000000, cThread::ThreadId(), ##x);\
}
#else
#define Dprintf(x...)
#endif

#define MAXPARSEBUFFER KILOBYTE(16)

/* Service ID for loop prevention */
#define LOOP_PREVENTION_SERVICE "StreamdevLoopPrevention"

/* Check if a channel is a radio station. */
#define ISRADIO(x) ((x)->Vpid()==0||(x)->Vpid()==1||(x)->Vpid()==0x1fff)

class cChannel;

enum eStreamType {
	stTS,
	stPES,
	stPS,
	stES,
	stEXT,
	stTSPIDS,
	st_Count
};

enum eSocketId {
	siLive,
	siReplay,
	siLiveFilter,
	siDataRespond,
	si_Count
};

extern const char *VERSION;

class cMenuEditIpItem: public cMenuEditItem {
private:
	static const char IpCharacters[];
	char *value;
	int curNum;
	int pos;
	bool step;

protected:
	virtual void Set(void);

public:
	cMenuEditIpItem(const char *Name, char *Value); // Value must be 16 bytes
	~cMenuEditIpItem();

	virtual eOSState ProcessKey(eKeys Key);
};

#endif // VDR_STREAMDEV_COMMON_H
