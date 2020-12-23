/* u_error.c
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
FTPLogError(const FTPCIPtr cip, const int pError, const char *const fmt, ...)
{
	va_list ap;
	int errnum;
	size_t len;
	char buf[256];
	int endsinperiod;
	int endsinnewline;
#ifndef HAVE_STRERROR
	char errnostr[16];
#endif
	size_t buflen = 0;
	int x;
	time_t t;
	struct tm lt, *ltp;

	buf[0] = '\0';
	if ((x = cip->debugTimestamping) != 0) {
		ltp = Localtime(time(&t), &lt);
		if (x == 1) {
			buflen = strftime(buf, sizeof(buf), "%H:%M:%S  ", ltp);
		} else {
			buflen = strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S  ", ltp);
		}
	}

	errnum = errno;
	va_start(ap, fmt);
#ifdef HAVE_VSNPRINTF
	vsnprintf(buf + buflen, sizeof(buf) - 1 - buflen, fmt, ap);
	buf[sizeof(buf) - 1] = '\0';
#else
	(void) vsprintf(buf + buflen, fmt, ap);
#endif
	va_end(ap);

	if (pError != 0) {
		len = strlen(buf);
		endsinperiod = 0;
		endsinnewline = 0;
		if (len > 2) {
			if (buf[len - 1] == '\n') {
				endsinnewline = 1;
				buf[len - 1] = '\0';
				if (buf[len - 2] == '.') {
					endsinperiod = 1;
					buf[len - 2] = '\0';
				}
			} else if (buf[len - 1] == '.') {
				endsinperiod = 1;
				buf[len - 1] = '\0';
			}
		}
#ifdef HAVE_STRERROR
		(void) STRNCAT(buf, ": ");
		(void) STRNCAT(buf, strerror(errnum));
#else
#	ifdef HAVE_SNPRINTF
		snprintf(errnostr, sizeof(errnostr) - 1, " (errno = %d)", errnum);
		errnostr[sizeof(errnostr) - 1] = '\0';
#	else
		sprintf(errnostr, " (errno = %d)", errnum);
#	endif
		STRNCAT(buf, errnostr);
#endif
		if (endsinperiod != 0)
			(void) STRNCAT(buf, ".");
		if (endsinnewline != 0)
			(void) STRNCAT(buf, "\n");
	}

	if (cip->errLog != NULL) {
		(void) fprintf(cip->errLog, "%s", buf);
		(void) fflush(cip->errLog);
	}
	if ((cip->debugLog != NULL) && (cip->debugLog != cip->errLog)) {
		if ((cip->errLog != stderr) || (cip->debugLog != stdout)) {
			(void) fprintf(cip->debugLog, "%s", buf);
			(void) fflush(cip->debugLog);
		}
	}
	if (cip->errLogProc != NULL) {
		(*cip->errLogProc)(cip, buf);
	}
	if ((cip->debugLogProc != NULL) && (cip->debugLogProc != cip->errLogProc)) {
		(*cip->debugLogProc)(cip, buf);
	}
}	/* FTPLogError */
