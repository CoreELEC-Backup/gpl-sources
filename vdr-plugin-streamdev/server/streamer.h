/*
 *  $Id: streamer.h,v 1.12 2010/07/19 13:49:32 schmirl Exp $
 */
 
#ifndef VDR_STREAMDEV_STREAMER_H
#define VDR_STREAMDEV_STREAMER_H

#include <vdr/thread.h>
#include <vdr/ringbuffer.h>
#include <vdr/tools.h>

#include "remux/tsremux.h"

class cTBSocket;
class cStreamdevStreamer;
class cServerConnection;

#ifndef TS_SIZE
#define TS_SIZE 188
#endif

#define WRITERBUFSIZE (20000 * TS_SIZE)

// --- cStreamdevBuffer -------------------------------------------------------

class cStreamdevBuffer: public cRingBufferLinear {
public:
	// make public
	void WaitForPut(void) { cRingBuffer::WaitForPut(); }
	// Always write complete TS packets
	// (assumes Count is a multiple of TS_SIZE)
	int PutTS(const uchar *Data, int Count);
	cStreamdevBuffer(int Size, int Margin = 0, bool Statistics = false, const char *Description = NULL);
};

inline int cStreamdevBuffer::PutTS(const uchar *Data, int Count)
{
	int free = Free();
	if (free < Count)
		Count = free;

	Count -= Count % TS_SIZE;
	if (Count)
		Count = Put(Data, Count);
	else
		WaitForPut();
	return Count;
}

// --- cStreamdevWriter -------------------------------------------------------

class cStreamdevWriter: public cThread {
private:
	cStreamdevStreamer *m_Streamer;
	cTBSocket          *m_Socket;

protected:
	virtual void Action(void);

public:
	cStreamdevWriter(cTBSocket *Socket, cStreamdevStreamer *Streamer);
	virtual ~cStreamdevWriter();
};

// --- cStreamdevStreamer -----------------------------------------------------

class cStreamdevStreamer: public cThread {
private:
	const cServerConnection *m_Connection;
	Streamdev::cTSRemux     *m_Remux;
	cStreamdevWriter        *m_Writer;
	cStreamdevBuffer        *m_SendBuffer;

protected:
	virtual uchar* GetFromReceiver(int &Count) = 0;
	virtual void DelFromReceiver(int Count) = 0;
	virtual int Put(const uchar *Data, int Count);
	virtual void Action(void);

	bool IsRunning(void) const { return m_Writer; }
	void SetRemux(Streamdev::cTSRemux *Remux) { delete m_Remux; m_Remux = Remux; }

public:
	cStreamdevStreamer(const char *Name, const cServerConnection *Connection = NULL);
	virtual ~cStreamdevStreamer();

	const cServerConnection* Connection(void) const { return m_Connection; }

	virtual void Start(cTBSocket *Socket);
	virtual void Stop(void);
	virtual bool IsReceiving(void) const = 0;
	bool Abort(void);

	uchar *Get(int &Count) { return m_Remux->Get(Count); }
	void Del(int Count) { m_Remux->Del(Count); } 

	virtual void Detach(void) {}
	virtual void Attach(void) {}

	virtual cString ToText() const { return ""; };
};

inline bool cStreamdevStreamer::Abort(void)
{
	return Active() && !m_Writer->Active();
}

#endif // VDR_STREAMDEV_STREAMER_H

