/* u_localtime.c
 *
 * Copyright (c) 1996-2005 Mike Gleason, NcFTP Software.
 * All rights reserved.
 *
 */

#include "syshdrs.h"
#ifdef PRAGMA_HDRSTOP
#	pragma hdrstop
#endif

#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
#define _CRT_SECURE_NO_WARNINGS 1
#endif

struct tm *
Localtime(time_t t, struct tm *const tp)
{
#if defined(HAVE_LOCALTIME_R) && (defined(HPUX)) && (HPUX < 1100)
	if (t == 0)
		time(&t);
	if (tp != NULL)
		memset(tp, 0, sizeof(struct tm));
	if (localtime_r(&t, tp) == 0)
		return (tp);
#elif defined(HAVE_LOCALTIME_R) && !defined(MACOSX)
	if (t == 0)
		time(&t);
	if (tp != NULL)
		memset(tp, 0, sizeof(struct tm));
	if (localtime_r(&t, tp) != NULL)
		return (tp);
#else
	struct tm *tmp;

	if (t == 0)
		time(&t);
	tmp = localtime(&t);
	if (tmp != NULL) {
		memcpy(tp, tmp, sizeof(struct tm));
		return (tp);
	}
#endif
	if (tp != NULL)
		memset(tp, 0, sizeof(struct tm));
	return NULL;
}	/* Localtime */
