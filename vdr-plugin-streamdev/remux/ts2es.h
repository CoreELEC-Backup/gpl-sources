#ifndef VDR_STREAMDEV_TS2ESREMUX_H
#define VDR_STREAMDEV_TS2ESREMUX_H

#include "remux/tsremux.h"
#include "server/streamer.h"

namespace Streamdev {

class cTS2ES;

class cTS2ESRemux: public cTSRemux {
private:
	int                m_Pid;
	cStreamdevBuffer  *m_ResultBuffer;
	cTS2ES            *m_Remux;

public:
	cTS2ESRemux(int Pid);
	virtual ~cTS2ESRemux();
	
	int Put(const uchar *Data, int Count);
	uchar *Get(int &Count) { return m_ResultBuffer->Get(Count); }
	void Del(int Count) { m_ResultBuffer->Del(Count); }
};

} // namespace Streamdev

#endif // VDR_STREAMDEV_TS2ESREMUX_H
