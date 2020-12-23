/* u_printf.c
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

/*VARARGS*/
void
PrintF(const FTPCIPtr cip, const char *const fmt, ...)
{
	va_list ap;
	char buf[1024];
	char tbuf[40];
	size_t tbuflen = 0;
	int x;
	time_t t;
	struct tm lt, *ltp;

	tbuf[0] = '\0';
	if ((x = cip->debugTimestamping) != 0) {
		ltp = Localtime(time(&t), &lt);
		if (x == 1) {
			tbuflen = strftime(tbuf, sizeof(tbuf), "%H:%M:%S", ltp);
		} else {
			tbuflen = strftime(tbuf, sizeof(tbuf), "%Y-%m-%d %H:%M:%S", ltp);
		}
		if (cip->debugLog != NULL)
			(void) fprintf(cip->debugLog, "%s  ", tbuf);
	}

	va_start(ap, fmt);

	if (cip->debugLog != NULL) {
		(void) vfprintf(cip->debugLog, fmt, ap);
		(void) fflush(cip->debugLog);
	}

	if (cip->debugLogProc != NULL) {
		if (tbuflen != 0)
			memcpy(buf, tbuf, tbuflen + 1);
#ifdef HAVE_VSNPRINTF
		(void) vsnprintf(buf + tbuflen, sizeof(buf) - tbuflen - 1, fmt, ap);
		buf[sizeof(buf) - 1] = '\0';
#else
		(void) vsprintf(buf + tbuflen, fmt, ap);
#endif
		(*cip->debugLogProc)(cip, buf);
	}
	va_end(ap);
}	/* PrintF */
