/*
 *  $Id: filter.c,v 1.14 2009/02/13 13:02:39 schmirl Exp $
 */

#include "client/filter.h"
#include "client/socket.h"
#include "tools/select.h"
#include "common.h"
#include <sys/ioctl.h>
#include <string.h>

#include <vdr/device.h>

#define PID_MASK_HI 0x1F
// --- cStreamdevFilter ------------------------------------------------------

static int FilterSockBufSize_warn = 0;

class cStreamdevFilter: public cListObject {
private:
	uchar              m_Buffer[8192];
	int                m_Used;
	int                m_Pipe[2];
	u_short            m_Pid;
	u_char             m_Tid;
	u_char             m_Mask;
#ifdef TIOCOUTQ
	unsigned long      m_maxq;
	unsigned long      m_flushed;
#endif

public:
	cStreamdevFilter(u_short Pid, u_char Tid, u_char Mask);
	virtual ~cStreamdevFilter();

	bool Matches(u_short Pid, u_char Tid);
	bool PutSection(const uchar *Data, int Length, bool Pusi);
	int  ReadPipe(void) const { return m_Pipe[0]; }

	void Reset(void);

	u_short Pid(void) const { return m_Pid; }
	u_char Tid(void) const { return m_Tid; }
	u_char Mask(void) const { return m_Mask; }
};

inline bool cStreamdevFilter::Matches(u_short Pid, u_char Tid) {
	return m_Pid == Pid && m_Tid == (Tid & m_Mask);
}

cStreamdevFilter::cStreamdevFilter(u_short Pid, u_char Tid, u_char Mask) {
	m_Used = 0;
	m_Pid  = Pid;
	m_Tid  = Tid;
	m_Mask = Mask;
	m_Pipe[0] = m_Pipe[1] = -1;
#ifdef TIOCOUTQ
	m_flushed = 0;
	m_maxq = 0;
#endif

#ifdef SOCK_SEQPACKET  
	// SOCK_SEQPACKET (since kernel 2.6.4)
	if (socketpair(AF_UNIX, SOCK_SEQPACKET, 0, m_Pipe) != 0) {
		esyslog("streamdev-client: socketpair(SOCK_SEQPACKET) failed: %m, trying SOCK_DGRAM");
	}
#endif
	if (m_Pipe[0] < 0 && socketpair(AF_UNIX, SOCK_DGRAM, 0, m_Pipe) != 0) {
		esyslog("streamdev-client: couldn't open section filter socket: %m");
	} 

	// Set buffer for socketpair. During certain situations, such as startup, channel/transponder
	// change, VDR may lag in reading data. Instead of discarding it, we can buffer it.
	// Buffer size required may be up to 4MByte.

	if(StreamdevClientSetup.FilterSockBufSize) {
		int sbs = StreamdevClientSetup.FilterSockBufSize;
		int sbs2;
		unsigned int sbss = sizeof(sbs);
		int r;

		r = setsockopt(m_Pipe[1], SOL_SOCKET, SO_SNDBUF, (char *)&sbs, sbss);

		if(r < 0) {
			isyslog("streamdev-client: setsockopt(SO_SNDBUF, %d) = %s", sbs, strerror(errno));
		}
		sbs2 = 0;
		r = getsockopt(m_Pipe[1], SOL_SOCKET, SO_SNDBUF, (char *)&sbs2, &sbss);
		if(r < 0 || !sbss || !sbs2) {
			isyslog("streamdev-client: getsockopt(SO_SNDBUF, &%d, &%d) = %s", sbs2, sbss, strerror(errno));
		} else {
			// Linux actually returns double the requested size
			// if everything works fine. And it actually buffers up to that double amount
			// as can be seen from observing TIOCOUTQ (kernel 3.7/2014).

			if(sbs2 > sbs)
				sbs2 /= 2;
			if(sbs2 < sbs) {
				if(FilterSockBufSize_warn != sbs2) {
					isyslog("streamdev-client: ******************************************************");
					isyslog("streamdev-client: getsockopt(SO_SNDBUF) = %d < %d (configured).", sbs2, sbs);
					isyslog("streamdev-client: Consider increasing system buffer size:");
					isyslog("streamdev-client: 'sysctl net.core.wmem_max=%d'", sbs);
					isyslog("streamdev-client: ******************************************************");
					FilterSockBufSize_warn = sbs2;
				}
			}
		}
	} 

	if(fcntl(m_Pipe[0], F_SETFL, O_NONBLOCK) != 0 ||
		fcntl(m_Pipe[1], F_SETFL, O_NONBLOCK) != 0) {
		esyslog("streamdev-client: couldn't set section filter socket to non-blocking mode: %m");
	}
}

cStreamdevFilter::~cStreamdevFilter() {
	Dprintf("~cStreamdevFilter %p\n", this);

	if (m_Pipe[0] >= 0) {
		close(m_Pipe[0]);
	}
	if (m_Pipe[1] >= 0) {
		close(m_Pipe[1]);
	}
}

bool cStreamdevFilter::PutSection(const uchar *Data, int Length, bool Pusi) {

	if (!m_Used && !Pusi) /* wait for payload unit start indicator */
		return true;
	if (m_Used && Pusi)   /* reset at payload unit start */
		Reset();

	if (m_Used + Length >= (int)sizeof(m_Buffer)) {
		esyslog("ERROR: Streamdev: Section handler buffer overflow (%d bytes lost)",
				Length);
		Reset();
		return true;
	}

	memcpy(m_Buffer + m_Used, Data, Length);
	m_Used += Length;
	if (m_Used > 3) {
		int length = (((m_Buffer[1] & 0x0F) << 8) | m_Buffer[2]) + 3;
		if (m_Used == length) {
			m_Used = 0;
#ifdef TIOCOUTQ
			// If we can determine the queue size of the socket,
			// we flush rather then let the socket drop random packets.
			// This ensures that we have more contiguous set of packets
			// on the receiver side.
			if(m_flushed) {
				unsigned long queue = 0;
				ioctl(m_Pipe[1], TIOCOUTQ, &queue);
				if(queue > m_maxq)
					m_maxq = queue;
				if(queue * 2 < m_maxq) {
					dsyslog("cStreamdevFilter::PutSection(Pid:%d Tid: %d): "
					        "Flushed %ld bytes, max queue: %ld",
					        m_Pid, m_Tid, m_flushed, m_maxq);
					m_flushed = m_maxq = 0;

				} else {
					m_flushed += length;
				}
			}
			if(!m_flushed)
#endif
			if(write(m_Pipe[1], m_Buffer, length) < 0) {
				if(errno != EAGAIN && errno != EWOULDBLOCK) {
					dsyslog("cStreamdevFilter::PutSection(Pid:%d Tid: %d): error: %s",
					         m_Pid, m_Tid, strerror(errno));
					return false;
				} else {
#ifdef TIOCOUTQ
					m_flushed += length;
#else
					dsyslog("cStreamdevFilter::PutSection(Pid:%d Tid: %d): "
					        "Dropping packet %ld bytes (queue overflow)",
					        m_Pid, m_Tid, length);
#endif
				}
			}
		}

		if (m_Used > length) {
			dsyslog("cStreamdevFilter::PutSection: m_Used > length !  Pid %2d, Tid%2d "
				"(len %3d, got %d/%d)", m_Pid, m_Tid, Length, m_Used, length);
			if(Length < TS_SIZE-5) {
				// TS packet not full -> this must be last TS packet of section data -> safe to reset now
				Reset();
			}
		}

	}
	return true;
}

void cStreamdevFilter::Reset(void) {
	if(m_Used)
		dsyslog("cStreamdevFilter::Reset skipping %d bytes", m_Used);
	m_Used = 0;
}

// --- cStreamdevFilters -----------------------------------------------------

cStreamdevFilters::cStreamdevFilters(cClientSocket *ClientSocket):
		cThread("streamdev-client: sections assembler") {
	m_ClientSocket = ClientSocket;
	m_TSBuffer = NULL;
}

cStreamdevFilters::~cStreamdevFilters() {
	SetConnection(-1);
}

int cStreamdevFilters::OpenFilter(u_short Pid, u_char Tid, u_char Mask) {
	cStreamdevFilter *f = new cStreamdevFilter(Pid, Tid, Mask);
	int fh = f->ReadPipe();

	LOCK_THREAD;
	Add(f);

	return fh;
}

void cStreamdevFilters::CloseFilter(int Handle) {
	LOCK_THREAD;

	for (cStreamdevFilter *fi = First(); fi; fi = Next(fi)) {
		if(fi->ReadPipe() == Handle) {
			// isyslog("cStreamdevFilters::CloseFilter(%d): Pid %4d, Tid %3d, Mask %2x (%d filters left)\n",
			//		Handle, (int)fi->Pid(), (int)fi->Tid(), fi->Mask(), Count()-1);
			Del(fi);
			return;
		}
	}
	esyslog("cStreamdevFilters::CloseFilter(%d): failed (%d filters left)\n", Handle, Count()-1);
}

bool cStreamdevFilters::ReActivateFilters(void)
{
	LOCK_THREAD;

	bool res = true;
	for (cStreamdevFilter *fi = First(); fi; fi = Next(fi)) {
		res = m_ClientSocket->SetFilter(fi->Pid(), fi->Tid(), fi->Mask(), true) && res;
		Dprintf("ReActivateFilters(%d, %d, %d) -> %s", fi->Pid(), fi->Tid(), fi->Mask(), res ? "Ok" :"FAIL");
	}
	return res;
}

void cStreamdevFilters::SetConnection(int Handle) {

	Cancel(2);
	DELETENULL(m_TSBuffer);

	if (Handle >= 0) {
		m_TSBuffer = new cTSBuffer(Handle, MEGABYTE(1), 1);
		ReActivateFilters();
		Start();
	}
}

void cStreamdevFilters::Action(void) {
	int fails = 0;

	while (Running()) {
		const uchar *block = m_TSBuffer->Get();
		if (block) {
			u_short pid = (((u_short)block[1] & PID_MASK_HI) << 8) | block[2];
			u_char tid = block[3];
			bool Pusi = block[1] & 0x40;
			// proprietary extension
			int len = block[4];
#if 0
			if (block[1] == 0xff &&
					block[2] == 0xff &&
					block[3] == 0xff &&
					block[4] == 0x7f)
				isyslog("*********** TRANSPONDER -> %s **********", block+5);
#endif
			LOCK_THREAD;
			cStreamdevFilter *f = First();
			while (f) {
				cStreamdevFilter *next = Next(f);
				if (f->Matches(pid, tid)) {

					if (f->PutSection(block + 5, len, Pusi))
						break;

					if (errno != ECONNREFUSED && 
							errno != ECONNRESET &&
							errno != EPIPE) {
						Dprintf("FATAL ERROR: %m\n");
						esyslog("streamdev-client: couldn't send section packet: %m");
					}
					m_ClientSocket->SetFilter(f->Pid(), f->Tid(), f->Mask(), false);
					Del(f);
					// Filter was closed.
					//  - need to check remaining filters for another match
				}
				f = next;
			}
		} else {
#if 1 // TODO: this should be fixed in vdr cTSBuffer
			// Check disconnection
			int fd = *m_ClientSocket->DataSocket(siLiveFilter);
			if(fd < 0)
				break;
			cPoller Poller(fd);
			if (Poller.Poll()) {
				char tmp[1];
				errno = 0;
				Dprintf("cStreamdevFilters::Action(): checking connection");
				if (recv(fd, tmp, 1, MSG_PEEK) == 0 && errno != EAGAIN) {
					++fails;
					if (fails >= 10) {
						esyslog("cStreamdevFilters::Action(): stream disconnected ?");
						m_ClientSocket->CloseDataConnection(siLiveFilter);
						break;
					}
				} else {
					fails = 0;
				}
			} else {
				fails = 0;
			}
			cCondWait::SleepMs(10);
#endif
		}
	}

	DELETENULL(m_TSBuffer);
	dsyslog("StreamdevFilters::Action() ended");
}
