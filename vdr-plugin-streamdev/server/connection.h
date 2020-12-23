/*
 *  $Id: connection.h,v 1.10 2010/08/03 10:46:41 schmirl Exp $
 */
 
#ifndef VDR_STREAMDEV_SERVER_CONNECTION_H
#define VDR_STREAMDEV_SERVER_CONNECTION_H

#include "tools/socket.h"
#include "common.h"
#include "server/streamer.h"

#include <map>
#include <string>

typedef std::map<std::string,std::string> tStrStrMap;
typedef std::pair<std::string,std::string> tStrStr;

class cChannel;

/* Basic capabilities of a straight text-based protocol, most functions
   virtual to support more complicated protocols */

class cServerConnection: public cListObject, public cTBSocket 
{
private:
	const char *m_Protocol;
	bool        m_DeferClose;
	bool        m_Pending;

	char        m_ReadBuffer[MAXPARSEBUFFER];
	uint        m_ReadBytes;
	
	char        m_WriteBuffer[MAXPARSEBUFFER];
	uint        m_WriteBytes;
	uint        m_WriteIndex;

	cStreamdevStreamer *m_Streamer;

	tStrStrMap  m_Headers;

protected:
	/* Will be called when a command terminated by a newline has been 
	   received */
	virtual bool Command(char *Cmd) = 0;

	/* Will put Message into the response queue, which will be sent in the next
	   server cycle. Note that Message will be line-terminated by Respond. 
	   Only one line at a time may be sent. If there are lines to follow, set
	   Last to false. Command(NULL) will be called in the next cycle, so you can
	   post the next line. */
	virtual bool Respond(const char *Message, bool Last = true, ...);
			//__attribute__ ((format (printf, 2, 4)));

	/* Add a request header */
	void SetHeader(const char *Name, const char *Value, const char *Prefix = "") { m_Headers.insert(tStrStr(std::string(Prefix) + Name, Value)); }

	/* Set the streamer */
	void SetStreamer(cStreamdevStreamer* Streamer) { delete m_Streamer; m_Streamer = Streamer; }

	/* Return the streamer */
	cStreamdevStreamer *Streamer() const { return m_Streamer; }

	static const cChannel *ChannelFromString(const char *String, int *Apid = NULL, int *Dpid = NULL);

public:
	/* If you derive, specify a short string such as HTTP for Protocol, which
	   will be displayed in error messages */
	cServerConnection(const char *Protocol, int Type = SOCK_STREAM);
	virtual ~cServerConnection();

	/* If true, any client IP will be accepted */
	virtual bool CanAuthenticate(void) { return false; }

	/* Gets called if the client has been accepted by the core */
	virtual void Welcome(void) { }
	
	/* Gets called if the client has been rejected by the core */
	virtual void Reject(void) { DeferClose(); }

	/* Get the client socket's file number */
	virtual int Socket(void) const { return (int)*this; }

	/* Determine if there is data to send or any command pending */
	virtual bool HasData(void) const;

	/* Gets called by server when the socket can accept more data. Writes
	   the buffer filled up by Respond(). Calls Command(NULL) if there is a
	   command pending. Returns false in case of an error */
	virtual bool Write(void);

	/* Gets called by server when there is incoming data to read. Calls
	   Command() for each line. Returns false in case of an error, or if
	   the connection shall be closed and removed by the server */
	virtual bool Read(void);

	/* Is polled regularely by the server. Returns true if the connection
	   needs to be terminated. */
	virtual bool Abort(void) const = 0;

	/* Will make the socket close after sending all queued output data */
	void DeferClose(void) { m_DeferClose = true; }

	/* Close the socket */
	virtual bool Close(void);

	virtual void Flushed(void) {}

	/* This connections protocol name */
	virtual const char* Protocol(void) const { return m_Protocol; }

	/* Text description of stream */
	virtual cString ToText(char Delimiter = ' ') const;

	/* std::map with additional information */
	const tStrStrMap& Headers(void) const { return m_Headers; }
};

inline bool cServerConnection::HasData(void) const
{
	return m_WriteBytes > 0 || m_Pending || m_DeferClose;
}

#endif // VDR_STREAMDEV_SERVER_CONNECTION_H
