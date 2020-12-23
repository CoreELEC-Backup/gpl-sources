#include "syshdrs.h"
#ifdef PRAGMA_HDRSTOP
#	pragma hdrstop
#endif

int
SNewStreamClient(void)
{
	int sfd;

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd < 0)
		return kSNewFailed;

	return (sfd);
}	/* SNewStreamClient */




int
SNewDatagramClient(void)
{
	int sfd;

	sfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sfd < 0)
		return kSNewFailed;

	return (sfd);
}	/* SNewDatagramClient */




int
SNewStreamServerByAddr(struct sockaddr_in *const saddr, const int nTries, const int reuseFlag, const int listenQueueSize)
{
	int oerrno;
	int sfd;

	if (saddr == NULL) {
		errno = EINVAL;
		return (kSioErrInvalidArg);
	}

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd < 0)
		return kSNewFailed;

	if (SBindAddr(sfd, saddr, nTries, reuseFlag) < 0) {
		oerrno = errno;
		(void) closesocket(sfd);
		errno = oerrno;
		return kSBindFailed;
	}

	if (SListen(sfd, listenQueueSize) < 0) {
		oerrno = errno;
		(void) closesocket(sfd);
		errno = oerrno;
		return kSListenFailed;
	}

	return (sfd);
}	/* SNewStreamServerByAddr */




int
SNewStreamServerByName(const char *const addrStr, const int nTries, const int reuseFlag, const int listenQueueSize)
{
	struct sockaddr_in saddr;

	if (addrStr == NULL) {
		errno = EINVAL;
		return (kSioErrInvalidArg);
	}

	if ((strchr(addrStr, ':') == NULL) && (isdigit((int) addrStr[0]))) {
		/* Just a port number specified */
		return (SNewStreamServer(atoi(addrStr), nTries, reuseFlag, listenQueueSize));
	}

	/* ip:port */
	if (AddrStrToAddr(addrStr, &saddr, -1) < 0)
		return (kSioErrBadAddrStr);

	return (SNewStreamServerByAddr(&saddr, nTries, reuseFlag, listenQueueSize));
}	/* SNewStreamServerByName */




int
SNewStreamServer(const int port, const int nTries, const int reuseFlag, const int listenQueueSize)
{
	int oerrno;
	int sfd;

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd < 0)
		return kSNewFailed;

	if (SBind(sfd, port, nTries, reuseFlag) < 0) {
		oerrno = errno;
		(void) closesocket(sfd);
		errno = oerrno;
		return kSBindFailed;
	}

	if (SListen(sfd, listenQueueSize) < 0) {
		oerrno = errno;
		(void) closesocket(sfd);
		errno = oerrno;
		return kSListenFailed;
	}

	return (sfd);
}	/* SNewStreamServer */




int
SNewDatagramServerByAddr(struct sockaddr_in *const saddr, const int nTries, const int reuseFlag)
{
	int oerrno;
	int sfd;

	if (saddr == NULL) {
		errno = EINVAL;
		return (kSioErrInvalidArg);
	}

	sfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sfd < 0)
		return kSNewFailed;

	if (SBindAddr(sfd, saddr, nTries, reuseFlag) < 0) {
		oerrno = errno;
		(void) closesocket(sfd);
		errno = oerrno;
		return kSBindFailed;
	}

	return (sfd);
}	/* SNewDatagramServer */




int
SNewDatagramServerByName(const char *const addrStr, const int nTries, const int reuseFlag)
{
	struct sockaddr_in saddr;

	if (addrStr == NULL) {
		errno = EINVAL;
		return (kSioErrInvalidArg);
	}

	if ((strchr(addrStr, ':') == NULL) && (isdigit((int) addrStr[0]))) {
		/* Just a port number specified */
		return (SNewDatagramServer(atoi(addrStr), nTries, reuseFlag));
	}

	/* ip:port */
	if (AddrStrToAddr(addrStr, &saddr, -1) < 0)
		return (kSioErrBadAddrStr);

	return (SNewDatagramServerByAddr(&saddr, nTries, reuseFlag));
}	/* SNewDatagramServer */




int
SNewDatagramServer(const int port, const int nTries, const int reuseFlag)
{
	int oerrno;
	int sfd;

	sfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sfd < 0)
		return kSNewFailed;

	if (SBind(sfd, port, nTries, reuseFlag) < 0) {
		oerrno = errno;
		(void) closesocket(sfd);
		errno = oerrno;
		return kSBindFailed;
	}

	return (sfd);
}	/* SNewDatagramServer */
