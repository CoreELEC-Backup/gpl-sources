#ifndef TOOLBOX_SOCKET_H
#define TOOLBOX_SOCKET_H

#include "tools/tools.h"
#include "tools/source.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>

/* cTBSocket provides a cTBSource-derived interface for input and output on
   TCP/IPv4-sockets. */

class cTBSocket: public cTBSource {
private:
	struct sockaddr_in m_LocalAddr;
	struct sockaddr_in m_RemoteAddr;

	int m_Type;
	int m_Protocol;

public:
	cTBSocket(int Type = SOCK_STREAM, int Protocol = 0);
	virtual ~cTBSocket();
	
	/* See cTBSource::SysRead() 
	   Reimplemented for TCP/IPv4 sockets. */
	virtual ssize_t SysRead(void *Buffer, size_t Length) const;

	/* See cTBSource::SysWrite() 
	   Reimplemented for TCP/IPv4 sockets. */
	virtual ssize_t SysWrite(const void *Buffer, size_t Length) const;

	/* Connect() tries to connect an available local socket within TimeoutMs
           milliseconds to the port given by Port of the target host given by
           Host in numbers-and-dots notation (i.e. "212.43.45.21"). A TimeoutMs
           of 0 will disable non-blocking IO for the connect call. Returns true
           if the connection attempt was successful and false otherwise, setting
           errno appropriately. */
	virtual bool Connect(const std::string &Host, uint Port, uint TimeoutMs = 0);

	/* Shutdown() shuts down one or both ends of a socket. If called with How
	   set to SHUT_RD, further reads on this socket will be denied. If called
	   with SHUT_WR, all writes are denied. Called with SHUT_RDWR, all firther
	   action on this socket will be denied. Returns true on success and false
	   otherwise, setting errno appropriately. */
	virtual bool Shutdown(int How);

	/* Close() closes the associated socket and releases all structures. 
	   Returns true on success and false otherwise, setting errno 
	   appropriately. The object is in the closed state afterwards, regardless
	   of any errors. */
	virtual bool Close(void);

	/* Listen() listens on the local port Port for incoming connections. The 
	   BackLog parameter defines the maximum length the queue of pending 
	   connections may grow to. Returns true if the object is listening on
	   the specified port and false otherwise, setting errno appropriately. */
	virtual bool Listen(const std::string &Ip, uint Port, int BackLog);

	/* Accept() returns a newly created cTBSocket, which is connected to the 
	   first connection request on the queue of pending connections of a
	   listening socket. If no connection request was pending, or if any other
	   error occurred, the resulting cTBSocket is closed. */
	virtual cTBSocket Accept(void) const;

	/* Accept() extracts the first connection request on the queue of pending
	   connections of the listening socket Listener and connects it to this
	   object. Returns true on success and false otherwise, setting errno to
	   an appropriate value. */
	virtual bool Accept(const cTBSocket &Listener);

	/* Sets DSCP sockopt */
	bool SetDSCP(void);

	/* LocalPort() returns the port number this socket is connected to locally.
	   The result is undefined for a non-open socket. */
	int LocalPort(void) const { return ntohs(m_LocalAddr.sin_port); }

	/* RemotePort() returns the port number this socket is connected to on the
	   remote side. The result is undefined for a non-open socket. */
	int RemotePort(void) const { return ntohs(m_RemoteAddr.sin_port); }

	/* LocalIp() returns the internet address in numbers-and-dots notation of
	   the interface this socket is connected to locally. This can be 
	   "0.0.0.0" for a listening socket listening to all interfaces. If the
	   socket is in its closed state, the result is undefined. */
	std::string LocalIp(void) const { return inet_ntoa(m_LocalAddr.sin_addr); }

	/* RemoteIp() returns the internet address in numbers-and-dots notation of
	   the interface this socket is connected to on the remote side. If the
	   socket is in its closed state, the result is undefined. */
	std::string RemoteIp(void) const { return inet_ntoa(m_RemoteAddr.sin_addr); }

	in_addr_t LocalIpAddr(void) const { return m_LocalAddr.sin_addr.s_addr; }
	in_addr_t RemoteIpAddr(void) const { return m_RemoteAddr.sin_addr.s_addr; }

	int Type(void) const { return m_Type; }
};

inline ssize_t cTBSocket::SysRead(void *Buffer, size_t Length) const {
	if (m_Type == SOCK_STREAM)
		return ::recv(*this, Buffer, Length, 0);
	else {
		socklen_t len = sizeof(m_RemoteAddr);
		return ::recvfrom(*this, Buffer, Length, 0, (sockaddr*)&m_RemoteAddr, &len);
	}
}

inline ssize_t cTBSocket::SysWrite(const void *Buffer, size_t Length) const {
	return ::send(*this, Buffer, Length, 0);
	if (m_Type == SOCK_STREAM)
		return ::send(*this, Buffer, Length, 0);
	else {
		socklen_t len = sizeof(m_RemoteAddr);
		return ::sendto(*this, Buffer, Length, 0, (sockaddr*)&m_RemoteAddr, len);
	}
}

#endif // TOOLBOX_SOCKET_H
