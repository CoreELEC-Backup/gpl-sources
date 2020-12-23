#ifndef VDR_STREAMDEV_EXTERNREMUX_H
#define VDR_STREAMDEV_EXTERNREMUX_H

#include "remux/tsremux.h"
#include <vdr/ringbuffer.h>
#include <string>

class cChannel;
class cPatPmtParser;
class cServerConnection;

namespace Streamdev {

class cTSExt;

class cExternRemux: public cTSRemux {
private:
	cRingBufferLinear *m_ResultBuffer;
	cTSExt            *m_Remux;

public:
	cExternRemux(const cServerConnection *Connection, const cChannel *Channel, const int *APids, const int *Dpids);
	cExternRemux(const cServerConnection *Connection, const cPatPmtParser *PatPmt, const int *APids, const int *Dpids);
	virtual ~cExternRemux();
	
	int Put(const uchar *Data, int Count);
	uchar *Get(int &Count) { return m_ResultBuffer->Get(Count); }
	void Del(int Count) { m_ResultBuffer->Del(Count); }
};

} // namespace Streamdev

#endif // VDR_STREAMDEV_EXTERNREMUX_H
