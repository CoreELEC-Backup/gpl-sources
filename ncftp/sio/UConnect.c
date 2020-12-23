#include "syshdrs.h"
#ifdef PRAGMA_HDRSTOP
#	pragma hdrstop
#endif

#ifdef HAVE_SYS_UN_H

extern int _SConnect(const int sfd, const void *const addr, const size_t saddrsiz, const int tlen);

int
UConnect(int sfd, const struct sockaddr_un *const /* must be aligned to 4 byte boundary */ addr, int ualen, int tlen)
{
	int result;
	
	if ((addr == NULL) || (ualen == 0)) {
		errno = EINVAL;
		return (-1);
	}
	
	result = _SConnect(sfd, addr, (size_t) ualen, tlen);
	return (result);
}	/* UConnect */

#endif	/* HAVE_SYS_UN_H */
