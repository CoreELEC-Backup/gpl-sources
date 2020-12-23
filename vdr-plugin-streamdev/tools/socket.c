#include "tools/socket.h"
#include "tools/select.h"

#include <vdr/tools.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

// default class: best effort
#define DSCP_BE    0
// gold class (video): assured forwarding 4 with lowest drop precedence
#define DSCP_AF41 34 << 2
// premium class (voip): expedited forwarding
#define DSCP_EF   46 << 2
// actual DSCP value used
#define STREAMDEV_DSCP DSCP_AF41

cTBSocket::cTBSocket(int Type, int Protocol) {
	memset(&m_LocalAddr, 0, sizeof(m_LocalAddr));
	memset(&m_RemoteAddr, 0, sizeof(m_RemoteAddr));
	m_Type = Type;
	m_Protocol = Protocol;
}

cTBSocket::~cTBSocket() {
	if (IsOpen()) Close();
}

bool cTBSocket::Connect(const std::string &Host, unsigned int Port, unsigned int TimeoutMs) {
	socklen_t len;
	int socket;

	if (IsOpen()) Close();
		
	if ((socket = ::socket(PF_INET, m_Type, m_Protocol)) == -1)
		return false;

	m_LocalAddr.sin_family = AF_INET;
	m_LocalAddr.sin_port   = 0;
	m_LocalAddr.sin_addr.s_addr = INADDR_ANY;
	if (::bind(socket, (struct sockaddr*)&m_LocalAddr, sizeof(m_LocalAddr)) 
			== -1) {
		::close(socket);
		return false;
	}

	if (TimeoutMs > 0 && ::fcntl(socket, F_SETFL, O_NONBLOCK) == -1) {
		::close(socket);
		return false;
	}
	
	m_RemoteAddr.sin_family = AF_INET;
	m_RemoteAddr.sin_port   = htons(Port);
	m_RemoteAddr.sin_addr.s_addr = inet_addr(Host.c_str());
	if (::connect(socket, (struct sockaddr*)&m_RemoteAddr, sizeof(m_RemoteAddr)) == -1) {
		if (TimeoutMs > 0 && errno == EINPROGRESS) {
			int so_error;
			socklen_t len = sizeof(so_error);
			cTBSelect select;
			select.Add(socket);
			if (select.Select(TimeoutMs) == -1 ||
					::getsockopt(socket, SOL_SOCKET, SO_ERROR, &so_error, &len) == -1) {
				::close(socket);
				return false;
			}
			if (so_error) {
				errno = so_error;
				::close(socket);
				return false;
			}

		}
		else {
			::close(socket);
			return false;
		}
	}

	if (m_Type == SOCK_STREAM) {
		len = sizeof(struct sockaddr_in);
		if (::getpeername(socket, (struct sockaddr*)&m_RemoteAddr, &len) == -1) {
			::close(socket);
			return false;
		}
	}
	
	len = sizeof(struct sockaddr_in);
	if (::getsockname(socket, (struct sockaddr*)&m_LocalAddr, &len) == -1) {
		::close(socket);
		return false;
	}

	if (!cTBSource::Open(socket)) {
		::close(socket);
		return false;
	}
	return true;
}

bool cTBSocket::Listen(const std::string &Ip, unsigned int Port, int BackLog) {
	int val;
	socklen_t len;
	int socket;

	if (IsOpen()) Close();
	
	if ((socket = ::socket(PF_INET, m_Type, m_Protocol)) == -1)
		return false;

	val = 1;
	if (::setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == -1)
		return false;

	m_LocalAddr.sin_family = AF_INET;
	m_LocalAddr.sin_port   = htons(Port);
	m_LocalAddr.sin_addr.s_addr = inet_addr(Ip.c_str());
	if (::bind(socket, (struct sockaddr*)&m_LocalAddr, sizeof(m_LocalAddr)) 
			== -1)
		return false;

	len = sizeof(struct sockaddr_in);
	if (::getsockname(socket, (struct sockaddr*)&m_LocalAddr, &len) == -1) 
		return false;
	
	if (m_Type == SOCK_STREAM && ::listen(socket, BackLog) == -1)
		return false;
	
	if (!cTBSource::Open(socket))
		return false;

	return true;
}

bool cTBSocket::Accept(const cTBSocket &Listener) {
	socklen_t addrlen;
	int socket;

	if (IsOpen()) Close();

	addrlen = sizeof(struct sockaddr_in);
	if ((socket = ::accept(Listener, (struct sockaddr*)&m_RemoteAddr,
			&addrlen)) == -1)
		return false;

	addrlen = sizeof(struct sockaddr_in);
	if (::getsockname(socket, (struct sockaddr*)&m_LocalAddr, &addrlen) == -1)
		return false;
	
	int sol=1;
	// Ignore possible errors here, proceed as usual
	::setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, &sol, sizeof(sol)); 

	if (!cTBSource::Open(socket))
		return false;
	
	return true;
}

RETURNS(cTBSocket, cTBSocket::Accept(void) const, ret)
	ret.Accept(*this);
RETURN(ret)

bool cTBSocket::Close(void) {
	bool ret = true;
	
	if (!IsOpen())
		ERRNUL(EBADF);

	if (::close(*this) == -1)
		ret = false;

	if (!cTBSource::Close())
		ret = false;

	memset(&m_LocalAddr, 0, sizeof(m_LocalAddr));
	memset(&m_RemoteAddr, 0, sizeof(m_RemoteAddr));

	return ret;
}

bool cTBSocket::Shutdown(int how) {
	if (!IsOpen())
		ERRNUL(EBADF);

	return ::shutdown(*this, how) != -1;
}

bool cTBSocket::SetDSCP(void) {
	int dscp = STREAMDEV_DSCP;
	return ::setsockopt(*this, IPPROTO_IP, IP_TOS, &dscp, sizeof(dscp)) != -1;
}
