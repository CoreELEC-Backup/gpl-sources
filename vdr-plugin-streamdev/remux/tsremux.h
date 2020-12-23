#ifndef VDR_STREAMDEV_TSREMUX_H
#define VDR_STREAMDEV_TSREMUX_H

#include "libdvbmpeg/transform.h"
#include <vdr/remux.h>

// Picture types:
#define NO_PICTURE 0

namespace Streamdev {

class cTSRemux {
public:
	virtual ~cTSRemux() {};

	virtual int Put(const uchar *Data, int Count) = 0;
	virtual uchar *Get(int &Count) = 0;
	virtual void Del(int Count) = 0;

	static void SetBrokenLink(uchar *Data, int Length);
	static int GetPid(const uchar *Data);
	static int GetPacketLength(const uchar *Data, int Count, int Offset);
	static int ScanVideoPacket(const uchar *Data, int Count, int Offset, uchar &PictureType);
};

} // namespace Streamdev

#endif // VDR_STREAMDEV_TSREMUX_H
