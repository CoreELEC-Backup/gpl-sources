/*
 *  $Id: connection.c,v 1.16 2010/08/03 10:51:53 schmirl Exp $
 */
 
#include "server/connection.h"
#include "server/setup.h"
#include "server/suspend.h"
#include "common.h"

#include <vdr/tools.h>
#include <vdr/thread.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>

cServerConnection::cServerConnection(const char *Protocol, int Type):
		cTBSocket(Type),
		m_Protocol(Protocol),
		m_DeferClose(false),
		m_Pending(false),
		m_ReadBytes(0),
		m_WriteBytes(0),
		m_WriteIndex(0),
		m_Streamer(NULL)
{
}

cServerConnection::~cServerConnection() 
{
	delete(m_Streamer);
}

const cChannel* cServerConnection::ChannelFromString(const char *String, int *Apid, int *Dpid) {
	const cChannel *channel = NULL;
	char *string = strdup(String);
	char *ptr, *end;
	int apididx = 0;
	
	if ((ptr = strrchr(string, '+')) != NULL) {
		*(ptr++) = '\0';
		apididx = strtoul(ptr, &end, 10);
		Dprintf("found apididx: %d\n", apididx);
	}

	if (isnumber(string)) {
		int temp = strtol(String, NULL, 10);
		if (temp == 0)
			temp = cDevice::CurrentChannel();
#if APIVERSNUM >= 20300
		LOCK_CHANNELS_READ;
		if (temp >= 1 && temp <= Channels->MaxNumber())
			channel = Channels->GetByNumber(temp);
#else
		if (temp >= 1 && temp <= Channels.MaxNumber())
			channel = Channels.GetByNumber(temp);
#endif
	} else {
#if APIVERSNUM >= 20300
		LOCK_CHANNELS_READ;
		channel = Channels->GetByChannelID(tChannelID::FromString(string));
#else
		channel = Channels.GetByChannelID(tChannelID::FromString(string));
#endif

		if (channel == NULL) {
			int i = 1;
#if APIVERSNUM >= 20300
			while ((channel = Channels->GetByNumber(i, 1)) != NULL) {
#else
			while ((channel = Channels.GetByNumber(i, 1)) != NULL) {
#endif
				if (String == channel->Name())
					break;

				i = channel->Number() + 1;
			}
		}
	}

	if (channel != NULL && apididx > 0) {
		int apid = 0, dpid = 0;
		int index = 1;

		for (int i = 0; channel->Apid(i) != 0; ++i, ++index) {
			if (index == apididx) {
				apid = channel->Apid(i);
				break;
			}
		}

		if (apid == 0) {
			for (int i = 0; channel->Dpid(i) != 0; ++i, ++index) {
				if (index == apididx) {
					dpid = channel->Dpid(i);
					break;
				}
			}
		}

		if (Apid != NULL) 
			*Apid = apid;
		if (Dpid != NULL) 
			*Dpid = dpid;
	}

	free(string);
	return channel;
}

bool cServerConnection::Read(void) 
{
	int b;
	if ((b = cTBSocket::Read(m_ReadBuffer + m_ReadBytes,
	                         sizeof(m_ReadBuffer) - m_ReadBytes - 1)) < 0) {
		esyslog("ERROR: read from client (%s) %s:%d failed: %m",
		        m_Protocol, RemoteIp().c_str(), RemotePort());
		return false;
	}

	if (b == 0) {
		isyslog("client (%s) %s:%d has closed connection",
		        m_Protocol, RemoteIp().c_str(), RemotePort());
		return false;
	}

	m_ReadBytes += b;
	m_ReadBuffer[m_ReadBytes] = '\0';

	char *end;
	bool result = true;
	while ((end = strchr(m_ReadBuffer, '\012')) != NULL) {
		*end = '\0';
		if (end > m_ReadBuffer && *(end - 1) == '\015')
			*(end - 1) = '\0';

		if (!Command(m_ReadBuffer))
			return false;

		m_ReadBytes -= ++end - m_ReadBuffer;
		if (m_ReadBytes > 0)
			memmove(m_ReadBuffer, end, m_ReadBytes);
	}

	if (m_ReadBytes == sizeof(m_ReadBuffer) - 1) {
		esyslog("ERROR: streamdev: input buffer overflow (%s) for %s:%d",
		        m_Protocol, RemoteIp().c_str(), RemotePort());
		return false;
	}
	
	return result;
}

bool cServerConnection::Write(void) 
{
	int b;
	if ((b = cTBSocket::Write(m_WriteBuffer + m_WriteIndex, 
	                          m_WriteBytes - m_WriteIndex)) < 0) {
		esyslog("ERROR: streamdev: write to client (%s) %s:%d failed: %m",
		        m_Protocol, RemoteIp().c_str(), RemotePort());
		return false;
	}

	m_WriteIndex += b;
	if (m_WriteIndex == m_WriteBytes) {
		m_WriteIndex = 0;
		m_WriteBytes = 0;
		if (m_Pending)
			Command(NULL);
		if (m_DeferClose)
			return false;
		Flushed();
	}
	return true;
}

bool cServerConnection::Respond(const char *Message, bool Last, ...) 
{
	char *buffer;
	int length;
	va_list ap;
	va_start(ap, Last);
	length = vasprintf(&buffer, Message, ap);
	va_end(ap);

	if (length < 0) {
		esyslog("ERROR: streamdev: buffer allocation failed (%s) for %s:%d",
				m_Protocol, RemoteIp().c_str(), RemotePort());
		return false;
	}

	if (m_WriteBytes + length + 2 > sizeof(m_WriteBuffer)) {
		esyslog("ERROR: streamdev: output buffer overflow (%s) for %s:%d", 
		        m_Protocol, RemoteIp().c_str(), RemotePort());
		free(buffer);
		return false;
	}
	Dprintf("OUT: |%s|\n", buffer);
	memcpy(m_WriteBuffer + m_WriteBytes, buffer, length);
	free(buffer);

	m_WriteBytes += length;
	m_WriteBuffer[m_WriteBytes++] = '\015';
	m_WriteBuffer[m_WriteBytes++] = '\012';
	m_Pending = !Last;
	return true;
}

bool cServerConnection::Close()
{
	if (IsOpen())
		isyslog("streamdev-server: closing %s connection to %s:%d", Protocol(), RemoteIp().c_str(), RemotePort());
	return cTBSocket::Close();
}

cString cServerConnection::ToText(char Delimiter) const
{
	return cString::sprintf("%s%c%s:%d", Protocol(), Delimiter, RemoteIp().c_str(), RemotePort());
}
