#include "syshdrs.h"
#ifdef PRAGMA_HDRSTOP
#	pragma hdrstop
#endif

int
SBind(int sockfd, const int port, const int nTries, const int reuseFlag)
{
	struct sockaddr_in localAddr;

	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr.sin_port = htons((unsigned short) port);

	return (SBindAddr(sockfd, &localAddr, nTries, reuseFlag));
}	/* SBind */




int
SBindAddr(int sockfd, struct sockaddr_in *localAddr, const int nTries, const int reuseFlag)
{
	unsigned int i;
	int on;
	sockopt_size_t onsize;

	if ((sockfd < 0) || (localAddr == NULL) || (localAddr->sin_family != AF_INET)) {
		errno = EINVAL;
		return (kSioErrInvalidArg);
	}

	if (reuseFlag != kReUseAddrNo) {
		/* This is mostly so you can quit the server and re-run it
		 * again right away.  If you don't do this, the OS may complain
		 * that the address is still in use.
		 */
		on = 1;
		onsize = (sockopt_size_t) sizeof(on);
		(void) setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
			SETSOCKOPT_ARG4 &on, onsize);

#ifdef SO_REUSEPORT
		/* Tells kernel that it's okay to have more
		 * than one process originating from this
		 * local port.
		 */
		on = 1;
		onsize = (sockopt_size_t) sizeof(on);
		(void) setsockopt(sockfd, SOL_SOCKET, SO_REUSEPORT,
			SETSOCKOPT_ARG4 &on, onsize);
#endif	/* SO_REUSEPORT */
	}

	for (i=1; ; i++) {
		/* Try binding a few times, in case we get Address in Use
		 * errors.
		 */
		if (bind(sockfd, (struct sockaddr *) localAddr, (sockaddr_size_t) sizeof(struct sockaddr_in)) == 0) {
			break;
		}
		if ((int) i == nTries) {
			return (kSBindFailed);
		}
		/* Give the OS time to clean up the old socket,
		 * and then try again.
		 */
		sleep(i * 3);
	}

	return (0);
}	/* SBindAddr */




int
SListen(int sfd, int backlog)
{
	if (sfd < 0) {
		errno = EINVAL;
		return (kSioErrInvalidArg);
	}
	if (listen(sfd, (listen_backlog_t) backlog) < 0)
		return (kSListenFailed);
	return (0);
}	/* SListen */
