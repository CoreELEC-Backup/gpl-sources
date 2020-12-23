/*
 *  $Id: filter.h,v 1.5 2008/04/07 14:27:28 schmirl Exp $
 */

#ifndef VDR_STREAMDEV_FILTER_H
#define VDR_STREAMDEV_FILTER_H

#include <vdr/config.h>
#include <vdr/tools.h>
#include <vdr/thread.h>

class cTSBuffer;
class cStreamdevFilter;
class cClientSocket;

class cStreamdevFilters: public cList<cStreamdevFilter>, public cThread {
private:
	cClientSocket     *m_ClientSocket;
	cTSBuffer         *m_TSBuffer;
	
protected:
	virtual void Action(void);
	bool ReActivateFilters(void);

public:
	cStreamdevFilters(cClientSocket *ClientSocket);
	virtual ~cStreamdevFilters();

	void SetConnection(int Handle);
	int OpenFilter(u_short Pid, u_char Tid, u_char Mask);
	void CloseFilter(int Handle);
};

#endif // VDR_STREAMDEV_FILTER_H
