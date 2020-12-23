/*
 *  $Id: socket.c,v 1.15 2010/08/18 10:26:55 schmirl Exp $
 */
 
#include <tools/select.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define MINLOGREPEAT 10	//don't log connect failures too often (seconds)

// timeout for writing to command socket
#define WRITE_TIMEOUT_MS 200
#define QUIT_TIMEOUT_MS 500

#include "client/socket.h"
#include "common.h"

cClientSocket::cClientSocket(void) 
{
	memset(m_DataSockets, 0, sizeof(cTBSocket*) * si_Count);
	m_ServerVersion = 0;
	m_Priority = -100;
	m_Prio = false;
	m_Abort = false;
	m_LastSignalUpdate = 0;
	m_LastSignalStrength = -1;
	m_LastSignalQuality = -1;
	m_LastDev = -1;
	Reset();
}

cClientSocket::~cClientSocket() 
{
	Reset();
	if (IsOpen()) Quit();
}

void cClientSocket::Reset(void) 
{
	for (int it = 0; it < si_Count; ++it)
		DELETENULL(m_DataSockets[it]);
	m_Priority = -100;
}

cTBSocket *cClientSocket::DataSocket(eSocketId Id) const {	
	return m_DataSockets[Id];
}

bool cClientSocket::Command(const std::string &Command, uint Expected) 
{
	uint code = 0;
	std::string buffer;
	if (Send(Command) && Receive(Command, &code, &buffer)) {
		if (code == Expected)
			return true;

		dsyslog("streamdev-client: Command '%s' rejected by %s:%d: %s",
			Command.c_str(), RemoteIp().c_str(), RemotePort(), buffer.c_str());
	}
	return false;
}
bool cClientSocket::Send(const std::string &Command)
{
	std::string pkt = Command + "\015\012";
	Dprintf("OUT: |%s|\n", Command.c_str());

	errno = 0;
	if (!TimedWrite(pkt.c_str(), pkt.size(), WRITE_TIMEOUT_MS)) {
		esyslog("ERROR: streamdev-client: Failed sending command '%s' to %s:%d: %s",
			Command.c_str(), RemoteIp().c_str(), RemotePort(), strerror(errno));
		Close();
		return false;
	}
	return true;
}

#define TIMEOUT_MS 1000
bool cClientSocket::Receive(const std::string &Command, uint *Code, std::string *Result, uint TimeoutMs) {
	int bufcount;
	do
	{
		errno = 0;
		bufcount = ReadUntil(m_Buffer, sizeof(m_Buffer) - 1, "\012", TimeoutMs < TIMEOUT_MS ? TimeoutMs : TIMEOUT_MS);
		if (bufcount == -1) {
			if (m_Abort)
				return false;
			if (errno != ETIMEDOUT || TimeoutMs <= TIMEOUT_MS) {
				esyslog("ERROR: streamdev-client: Failed reading reply to '%s' from %s:%d: %s",
					Command.c_str(), RemoteIp().c_str(), RemotePort(), strerror(errno));
				Close();
				return false;
			}
			TimeoutMs -= TIMEOUT_MS;
		}
	} while (bufcount == -1);
	if (m_Buffer[bufcount - 1] == '\015')
		--bufcount;
	m_Buffer[bufcount] = '\0';
	Dprintf("IN: |%s|\n", m_Buffer);

	if (Result != NULL)
		*Result = m_Buffer;
	if (Code != NULL)
		*Code = strtoul(m_Buffer, NULL, 10);
	return true;
}

bool cClientSocket::CheckConnection(void) {
	CMD_LOCK;

	if (IsOpen()) {
		cTBSelect select;

		// XXX+ check if connection is still alive (is there a better way?)
		// There REALLY shouldn't be anything readable according to PROTOCOL here
		// If there is, assume it's an eof signal (subseq. read would return 0)
		select.Add(*this, false);
		int res;
		if ((res = select.Select(0)) == 0) {
			return true;
		}
		Dprintf("closing connection (res was %d)\n", res);
		Close();
	}

	if (!Connect(StreamdevClientSetup.RemoteIp, StreamdevClientSetup.RemotePort, StreamdevClientSetup.Timeout * 1000)){
		static time_t lastTime = 0;
		if (time(NULL) - lastTime > MINLOGREPEAT) {
			esyslog("ERROR: streamdev-client: Couldn't connect to %s:%d: %s", 
				(const char*)StreamdevClientSetup.RemoteIp,
				StreamdevClientSetup.RemotePort, strerror(errno));
			lastTime = time(NULL);
		}
		return false;
	}

	uint code = 0;
	std::string buffer;
	if (!Receive("<connect>", &code, &buffer)) {
		Close();
		return false;
	}
	if (code != 220) {
		esyslog("ERROR: streamdev-client: Didn't receive greeting from %s:%d: %s",
		        RemoteIp().c_str(), RemotePort(), buffer.c_str());
		Close();
		return false;
	}

	unsigned int major, minor;
	if (sscanf(buffer.c_str(), "%*u VTP/%u.%u", &major, &minor) == 2)
		m_ServerVersion = major * 100 + minor;

	if (m_ServerVersion == 0) {
		if (!Command("CAPS TSPIDS", 220)) {
			Close();
			return false;
		}

		const char *Filters = "";
		if(Command("CAPS FILTERS", 220))
			Filters = ",FILTERS";

		const char *Prio = "";
		if(Command("CAPS PRIO", 220)) {
			Prio = ",PRIO";
			m_Prio = true;
		}
		isyslog("streamdev-client: Connected to server %s:%d using capabilities TSPIDS%s%s",
				RemoteIp().c_str(), RemotePort(), Filters, Prio);
	}
	else {
		if(!Command("VERS 1.0", 220)) {
			Close();
			return false;
		}
		m_Prio = true;
		isyslog("streamdev-client: Connected to server %s:%d using protocol version %u.%u",
				RemoteIp().c_str(), RemotePort(), major, minor);
	}

	return true;
}

bool cClientSocket::ProvidesChannel(const cChannel *Channel, int Priority) {
	if (!CheckConnection()) return false;

	CMD_LOCK;

	std::string command = (std::string)"PROV " + (const char*)itoa(Priority) + " " 
	                    + (const char*)Channel->GetChannelID().ToString();
	if (!Send(command))
		return false;

	uint code;
	std::string buffer;
	if (!Receive(command, &code, &buffer))
		return false;
	if (code != 220 && code != 560) {
		esyslog("streamdev-client: Unexpected reply to '%s' from %s:%d: %s",
			command.c_str(), RemoteIp().c_str(), RemotePort(), buffer.c_str());
		return false;
	}
	return code == 220;
}

bool cClientSocket::CreateDataConnection(eSocketId Id) {
	cTBSocket listen(SOCK_STREAM);

	if (!CheckConnection()) return false;

	if (m_DataSockets[Id] != NULL)
		DELETENULL(m_DataSockets[Id]);

	if (!listen.Listen(LocalIp(), 0, 1)) {
		esyslog("ERROR: streamdev-client: Couldn't create data connection: %s", 
				strerror(errno));
		return false;
	}

	std::string command = (std::string)"PORT " + (const char*)itoa(Id) + " " 
	                    + LocalIp().c_str() + "," 
	                    + (const char*)itoa((listen.LocalPort() >> 8) & 0xff) + ","
	                    + (const char*)itoa(listen.LocalPort() & 0xff);
	size_t idx = 4;
	while ((idx = command.find('.', idx + 1)) != (size_t)-1)
		command[idx] = ',';

	CMD_LOCK;

	if (!Command(command, 220))
		return false;

	/* The server SHOULD do the following:
	 * - get PORT command
	 * - connect to socket
	 * - return 220
	 */

	m_DataSockets[Id] = new cTBSocket;
	if (!m_DataSockets[Id]->Accept(listen)) {
		esyslog("ERROR: streamdev-client: Couldn't establish data connection to %s:%d%s%s",
				RemoteIp().c_str(), RemotePort(), errno == 0 ? "" : ": ",
				errno == 0 ? "" : strerror(errno));
		DELETENULL(m_DataSockets[Id]);
		return false;
	}

	return true;
}

bool cClientSocket::CloseDataConnection(eSocketId Id) {
	CMD_LOCK;

	if(Id == siLive || Id == siLiveFilter)
		if (m_DataSockets[Id] != NULL) {
			std::string command = (std::string)"ABRT " + (const char*)itoa(Id);
			Command(command, 220);
			DELETENULL(m_DataSockets[Id]);
		}
	return true;
}

bool cClientSocket::SetChannelDevice(const cChannel *Channel) {
	if (!CheckConnection()) return false;

	CMD_LOCK;

	std::string command = (std::string)"TUNE " 
				+ (const char*)Channel->GetChannelID().ToString();
	if (!Command(command, 220))
		return false;

	m_LastSignalUpdate = 0;
	return true;
}

bool cClientSocket::SetPriority(int Priority) {
	if (Priority == m_Priority)
		return true;

	if (!CheckConnection()) return false;

	CMD_LOCK;

	std::string command = (std::string)"PRIO " + (const char*)itoa(Priority);
	if (!Command(command, 220))
		return false;

	m_Priority = Priority;
	return true;
}

bool cClientSocket::GetSignal(int *SignalStrength, int *SignalQuality, int *Dev) {
	if (!CheckConnection()) return -1;

	CMD_LOCK;

	if (m_LastSignalUpdate != time(NULL)) {
		uint code = 0;
		std::string buffer;
		std::string command("SGNL");
		if (!Send(command) || !Receive(command, &code, &buffer) || code != 220
				|| sscanf(buffer.c_str(), "%*d %d %d:%d", &m_LastDev, &m_LastSignalStrength, &m_LastSignalQuality) != 3) {
			m_LastDev = -1;
			m_LastSignalStrength = -1;
			m_LastSignalQuality = -1;
		}
		m_LastSignalUpdate = time(NULL);
	}
	if (SignalStrength)
		*SignalStrength = m_LastSignalStrength;
	if (SignalQuality)
		*SignalQuality = m_LastSignalQuality;
	if (Dev)
		*Dev = m_LastDev;
	return 0;
}

bool cClientSocket::SetPid(int Pid, bool On) {
	if (!CheckConnection()) return false;

	CMD_LOCK;

	std::string command = (std::string)(On ? "ADDP " : "DELP ") + (const char*)itoa(Pid);
	return Command(command, 220);
}

bool cClientSocket::SetFilter(ushort Pid, uchar Tid, uchar Mask, bool On) {
	if (!CheckConnection()) return false;

	CMD_LOCK;

	std::string command = (std::string)(On ? "ADDF " : "DELF ") + (const char*)itoa(Pid)
	                    + " " + (const char*)itoa(Tid) + " " + (const char*)itoa(Mask);
	return Command(command, 220);
}

bool cClientSocket::CloseDvr(void) {
	if (!CheckConnection()) return false;

	CMD_LOCK;

	if (m_DataSockets[siLive] != NULL) {
		std::string command = (std::string)"ABRT " + (const char*)itoa(siLive);
		if (!Command(command, 220))
			return false;
		DELETENULL(m_DataSockets[siLive]);
	}
	return true;
}

bool cClientSocket::Quit(void) {
	m_Abort = true;
	if (!IsOpen()) return false;

	CMD_LOCK;
	std::string command("QUIT");
	bool res = Send(command) && Receive(command, NULL, NULL, QUIT_TIMEOUT_MS);
	Close();
	return res;
}
	
bool cClientSocket::SuspendServer(void) {
	if (!CheckConnection()) return false;

	CMD_LOCK;

	return Command("SUSP", 220);
}
