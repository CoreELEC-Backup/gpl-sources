/* io_sendfile.c
 *
 * Copyright (c) 2016 Mike Gleason, NcFTP Software.
 * All rights reserved.
 *
 */

#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
#	define WIN 1
#	include <windows.h>
#	define longest_int _int64
#	define PRINTF_LONG_LONG "%I64d"
#	define GETSOCKOPT_ARG4 (char *)
#	define NO_SENDFILE 1
#else
#	if defined(HAVE_CONFIG_H)
#		include <config.h>
#       define NO_DEFAULT_DEFS
#	endif
#endif

#ifdef __FreeBSD__
#	ifndef FREEBSD
#		define FREEBSD 1000
#	endif
#endif

#if defined( __APPLE__) && defined(__MACH__) && !defined(MACOSX)
#	define MACOSX 1
#endif

#ifdef FREEBSD
#	ifndef HAVE_SYS_SELECT_H
#		define HAVE_SYS_SELECT_H 1
#	endif
#endif

#if defined(__linux__) && !defined(LINUX)
#	define LINUX 26000
#endif

#ifdef LINUX
#	ifndef HAVE_SYS_SENDFILE_H
#		define HAVE_SYS_SENDFILE_H 1
#	endif
#endif

#ifdef WIN
#	define off_t __int64
#elif !defined(NO_DEFAULT_DEFS)
#	ifndef HAVE_UNISTD_H 
#		define HAVE_UNISTD_H 1
#	endif
#	ifndef _FILE_OFFSET_BITS
#		define _FILE_OFFSET_BITS 64
#	endif
#	ifndef HAVE_SNPRINTF
#		define HAVE_SNPRINTF 1
#	endif
#	ifndef HAVE_USLEEP
#		define HAVE_USLEEP 1
#	endif
#	ifndef HAVE_POSIX_MEMALIGN
#		define HAVE_POSIX_MEMALIGN 1
#	endif
#	ifndef Socklen_t
#		define Socklen_t socklen_t
#	endif
#	ifndef _GNU_SOURCE
#		define _GNU_SOURCE 1
#	endif
#endif

#ifdef HAVE_UNISTD_H
#	include <unistd.h>
#endif
#ifndef WIN
#	include <sys/time.h>
#	include <sys/stat.h>
#	include <sys/socket.h>
#	include <netinet/in.h>
#	include <arpa/inet.h>
#endif
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <time.h>
#include <signal.h>
#ifdef HAVE_SYS_SELECT_H
#	include <sys/select.h>
#endif
#ifdef HAVE_SYS_SENDFILE_H
#	include <sys/sendfile.h>
#endif

#ifndef Socklen_t
#	define Socklen_t int
#endif

#ifndef GETSOCKOPT_ARG4
#	define GETSOCKOPT_ARG4
#endif

#ifndef PRINTF_LONG_LONG
#	define PRINTF_LONG_LONG "%lld"
#endif

#ifndef longest_int
#	define longest_int long long int
#endif

#if defined(FREEBSD) && (FREEBSD >= 301)
#	define FREEBSD_SENDFILE 1
#endif

#ifdef MACOSX
#	define MACOSX_SENDFILE 1
#endif

#include "io_sendfile.h"

/* Global variables. */
int gSendfileInProgress = 0;




int 
DoSendfileAvailable(void)
{
#if defined(FREEBSD_SENDFILE) || defined(MACOSX_SENDFILE)
	return 1;
#elif defined(LINUX) && (LINUX >= 22000)
	return 1;
#else
    return 0;
#endif
}	/* DoSendfileAvailable */




void
DoSendfileParamsInit(DoSendfileParams *const dspp, const size_t siz)
{
	memset(dspp, 0, siz ? siz : sizeof(DoSendfileParams));
	dspp->size = siz ? siz : sizeof(DoSendfileParams);
	dspp->id = 0xD053F113;
	dspp->version = kDoSendfileVersion;
}	/* DoSendfileParamsInit */




void
DoSendfileProgressInfoInit(DoSendfileProgressInfo *const dspip, const size_t siz)
{
	memset(dspip, 0, siz ? siz : sizeof(DoSendfileProgressInfo));
	dspip->size = siz ? siz : sizeof(DoSendfileProgressInfo);
	dspip->id = 0xD053F113;
	dspip->version = kDoSendfileVersion;
}	/* DoSendfileProgressInfoInit */




/*VARARGS*/
void
DoSendfileErrPrintF(DoSendfileParamsPtr dspp, const char *const fmt, ...)
{
	va_list ap;
	FILE *x = stderr;

	va_start(ap, fmt);
	if ((x != NULL) && (dspp != NULL))
		(void) vfprintf(x, fmt, ap);
	va_end(ap);
}	/* DoSendfileErrPrintF */




static void
DoSendfileTimeout(int sigNum)
{
	if (gSendfileInProgress > 0) {
		gSendfileInProgress = -sigNum;
		/* fprintf(stderr, "Canceler caught sendfile timeout, returning.\n"); */
		errno = EINTR;
	} else {
		/* fprintf(stderr, "Caught spurious SIGALRM, ignoring.\n"); */
	}
	return;		/* Break out of sendfile with EINTR or (nwrote < ntoread) */
}	/* DoSendfileTimeout */




static void
DoSendfileCancel(int sigNum)
{
	/* Can be SIGPIPE or SIGURG */
	if (gSendfileInProgress > 0) {
		gSendfileInProgress = -sigNum;
		/* fprintf(stderr, "Canceler caught signal %d, returning.\n", sigNum); */
		errno = EPIPE;
	} else {
		/* fprintf(stderr, "Caught spurious SIGPIPE, ignoring.\n"); */
	}
	return;		/* Break out of sendfile with EPIPE */
}	/* DoSendfileCancel */



#ifdef NO_SENDFILE
int
DoSendfile(const int ifd, const longest_int isize, const int ofd, longest_int *const osize, DoSendfileParams *const dspp)
{
	errno = ENOSYS;
	if ((ifd == -666) && (isize == 0) && (ofd == 0) && (osize == 0) && (dspp == 0)) errno = 1; /* ignore unused */
	return -1;
}
#else

int
DoSendfile(const int ifd, const longest_int isize, const int ofd, longest_int *const osize, DoSendfileParams *const dspp)
{
	longest_int twrote;
	longest_int tntoread;
	struct sigaction sa;
	struct sigaction oldsa, oldsp, oldsi;
	int result;
	int oerrno;
	volatile int using_alarm;
	unsigned int amt;
	ssize_t nread;
	ssize_t nwrote;
	longest_int ntowrite;
	longest_int ntoread;
	fd_set rfds, wfds, xfds;
	int nready, maxfd;
	struct timeval sel_tmout;
	int do_perror;
	int xferTimeoutSec;
	int xferTimeoutUSec;
	DoSendfileProgressInfo *dspip;
	DoSendfileProgressProc dsppp;
	DoSendfileErrPrintFProc dseppp;

#ifdef SO_SNDTIMEO
	struct timeval tmout;
#endif	/* SO_SNDTIMEO */

#ifdef SIGURG
	struct sigaction oldsu;
#endif	/* SIGURG */

#if defined(FREEBSD_SENDFILE) || defined(MACOSX_SENDFILE)
	struct sf_hdtr sf;
	off_t off, off1;
#else
	struct stat st;
#endif	/* FREEBSD, MACOSX */

#if defined(LINUX) && (LINUX >= 22000)
	off_t off, off1;
#endif	/* LINUX */

	if (osize != NULL)
		*osize = (longest_int) 0;

	if ((ifd < 0) || (ofd < 0)) {
		errno = EBADF;
		return (-1);
	}

	if ((dspp == NULL) || (dspp->id != kDoSendfileID)) {
		errno = EINVAL;
		return (-1);
	}
	dspip = dspp->dspip;
	dsppp = dspp->dsppp;
	if (dspp->dseppp == NULL)
		dspp->dseppp = DoSendfileErrPrintF;
	dseppp = dspp->dseppp;
	do_perror = dspp->printError;

	maxfd = (ifd > ofd) ? ifd : ofd;
	tntoread = ntoread = isize;
	twrote = 0;
	using_alarm = 0;
	gSendfileInProgress = 0;

	(void) sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = DoSendfileTimeout;
	(void) sigaction(SIGALRM, &sa, (struct sigaction *) &oldsa);

	sa.sa_handler = DoSendfileCancel;
	(void) sigaction(SIGPIPE, &sa, (struct sigaction *) &oldsp);

	if (dspp->handleSIGINT) {
		sa.sa_handler = DoSendfileCancel;
		(void) sigaction(SIGINT, &sa, (struct sigaction *) &oldsi);
	}

#ifdef SIGURG
	(void) sigaction(SIGURG, &sa, (struct sigaction *) &oldsu);
#endif	/* SIGURG */

	result = -1;

#ifndef O_NONBLOCK
	dspp->nonBlockingOutput = dspp->nonBlockingInput = 0;
#endif

	xferTimeoutSec = (int) dspp->xferTimeout;
	xferTimeoutUSec = (int) ((dspp->xferTimeout - (double) xferTimeoutSec) * 1000000.0);

	if ((dspp->xferTimeout > 0) && (dspp->nonBlockingOutput == 0)) {
#ifdef SO_SNDTIMEO
		tmout.tv_sec = xferTimeoutSec;
		tmout.tv_usec = xferTimeoutUSec;
		if (setsockopt(ofd, SOL_SOCKET, SO_SNDTIMEO, GETSOCKOPT_ARG4 &tmout, (Socklen_t) sizeof(tmout)) != 0) {
#endif	/* SO_SNDTIMEO */

			/* Set the timer. */
			using_alarm = 1;
			if ((isize <= 0) || (isize > (longest_int) 1000000000)) {
				(void) alarm(65535);
			} else {
				amt = (unsigned int) (isize / 4096);
				if ((int) amt < xferTimeoutSec)
					amt = (unsigned int) xferTimeoutSec;
				if (amt > 65535)
					amt = 65535;
				(void) alarm(amt);
			}

#ifdef SO_SNDTIMEO
		}
#endif	/* SO_SNDTIMEO */
	}

	oerrno = 0;

	if (dspp->userSpaceModeOnly != 0) {
		if (dspp->buf == NULL) {
			if (do_perror) (*dseppp)(dspp, "DoSendfile: Buffer size was not set for regular userspace mode.\n");
			errno = EINVAL;
			return (-1);
		}
		result = 0;
		goto userspace_loop;
	}

#if defined(LINUX) && (LINUX >= 22000)
	if ((tntoread == 0) && (fstat(ifd, &st) == 0)) {
		tntoread = ntoread = st.st_size;
	}

	off = off1 = dspp->initialOffset;

	for (;;) {
		errno = 0;
		if (ntoread <= 0)
			break;
		if (gSendfileInProgress == 0)
			gSendfileInProgress = 1;

		if (gSendfileInProgress < 0) {
			nwrote = -1;
			errno = oerrno;
		} else {
			off1 = off;
			nwrote = (ssize_t) sendfile(ofd, ifd, &off, (size_t) ntoread);
			oerrno = errno;
		}

		if (nwrote <= 0) {
			/* Error; note EINTR is not handled in the
			 * usual fashion.  If we get it, it should
			 * mean that may have caught an alarm (so we would
			 * not want to do the usual retry).
			 */
			if ((errno == EINVAL) && (ntoread == tntoread))
				break;		/* Didn't work, but can still fall back to regular non-sendfile mode. */
			if ((nwrote == 0) && (tntoread > 0)) {
				/* EOF ??? */
				if (do_perror) (*dseppp)(dspp, "DoSendfile: Error occurred after "
					PRINTF_LONG_LONG
					" of "
					PRINTF_LONG_LONG
					" bytes were sent: %s\n",
					twrote,
					tntoread,
					"Unexpected EOF"
				);
				errno = EEXIST;
			} else if (gSendfileInProgress == -SIGURG) {
				if (do_perror) (*dseppp)(dspp, "DoSendfile: interrupted by SIGURG.\n");
				errno = EINTR;
			} else if (gSendfileInProgress == -SIGINT) {
				if (do_perror) (*dseppp)(dspp, "DoSendfile: interrupted by SIGINT.\n");
				errno = EINTR;
			} else if ((gSendfileInProgress == -SIGALRM) || ((oerrno == EINTR) && (using_alarm != 0))) {
				/* Timed-out */
				if (do_perror) (*dseppp)(dspp, "DoSendfile: interrupted by timeout alarm after "
					PRINTF_LONG_LONG
					" of "
					PRINTF_LONG_LONG
					" bytes were sent.\n",
					twrote,
					tntoread
				);
				errno = ETIMEDOUT;
			} else if (oerrno == EINTR) {
				/* Used to just retry (continue;)
				 * like you would with a regular
				 * system call;  It is unclear
				 * if we can really call sendfile again
				 * and have it resume where it left off.
				 */
				if (do_perror) (*dseppp)(dspp, "DoSendfile: EINTR, aborting.\n");
			} else if ((gSendfileInProgress == -SIGPIPE) || (oerrno == EPIPE)) {
				if (do_perror) (*dseppp)(dspp, "DoSendfile: broken pipe.\n");
				errno = EPIPE;
			} else if (oerrno == EAGAIN) {
				if ((dspp->nonBlockingOutput != 0) || (dspp->nonBlockingInput != 0)) {
					/* Retry; should only do this if the socket was set to non-blocking. */
					/* The ending offset was in "off". */
					twrote = off;
					ntoread -= (off - off1);
					if (dspip != NULL) {
						/* Progress meter update */
						dspip->current = twrote;
						if (dsppp != NULL)
							(*dsppp)(dspip);
					}

					/* fprintf(stderr, "DoSendfile: LINUX: EAGAIN started at " PRINTF_LONG_LONG " of " PRINTF_LONG_LONG " bytes; ended at " PRINTF_LONG_LONG " bytes.\n", (longest_int) off1, tntoread, (longest_int) off); */

					FD_ZERO(&wfds); FD_SET(ofd, &wfds);
					FD_ZERO(&xfds); FD_SET(ifd, &xfds); FD_SET(ofd, &xfds);
					sel_tmout.tv_sec = xferTimeoutSec;
					sel_tmout.tv_usec = xferTimeoutUSec;
					nready = select(maxfd + 1, NULL, &wfds, &xfds, &sel_tmout);
					if (nready > 0)
						continue;	/* Ready to write again. */
					if (nready == 0) {
						/* Timed out */
						if (do_perror) (*dseppp)(dspp, "DoSendfile: socket write timed out after "
							PRINTF_LONG_LONG
							" of "
							PRINTF_LONG_LONG
							" bytes were sent.\n",
							twrote,
							tntoread
						);
						errno = ETIMEDOUT;
						break;
					}
					if (do_perror) (*dseppp)(dspp, "DoSendfile: select error: %s.\n", strerror(errno));
				} else {
					/* Linux also returns this if the SO_SNDTIMEO
					 * is triggered.
					 */
					if (do_perror) (*dseppp)(dspp, "DoSendfile: socket send timeout after "
						PRINTF_LONG_LONG
						" of "
						PRINTF_LONG_LONG
						" bytes were sent.\n",
						twrote,
						tntoread
					);
					errno = ETIMEDOUT;
				}
			} else {
				if (do_perror) (*dseppp)(dspp, "DoSendfile: Error occurred after "
					PRINTF_LONG_LONG
					" of "
					PRINTF_LONG_LONG
					" bytes were sent: %s\n",
					twrote,
					tntoread,
					strerror(oerrno)
				);
			}
			break;
		}

		ntoread -= nwrote;
		twrote += nwrote;
	}
	if (ntoread <= 0) {
		/* twrote == tntoread */
		result = 0;
		gSendfileInProgress = 0;
	}
	/* end LINUX */

#elif defined(FREEBSD_SENDFILE) || defined(MACOSX_SENDFILE)
	off = off1 = dspp->initialOffset;
	for (;;) {
		if (off >= 0)
			off1 = off;
		off = (off_t) 0;
		sf.headers = NULL;
		sf.hdr_cnt = 0;
		sf.trailers = NULL;
		sf.trl_cnt = 0;

		errno = 0;
		gSendfileInProgress = 1;

#ifdef MACOSX_SENDFILE
		/* int sendfile(int fd, int s, off_t offset, off_t *len, struct sf_hdtr *hdtr, int flags); */
		nwrote = (ssize_t) sendfile(ifd, ofd, off1, &off, &sf, 0);

#else		/* FreeBSD */

		/* int sendfile(int fd, int s, off_t offset, size_t nbytes, struct sf_hdtr *hdtr, off_t *sbytes, int flags); */
		nwrote = (ssize_t) sendfile(ifd, ofd, off1, (size_t) 0 /* send to EOF */, &sf, &off, 0);
#endif
		oerrno = errno;

		if (nwrote >= 0) {
			twrote += off;
			if ((twrote < tntoread) && (tntoread > 0)) {
				/* short */
				if (do_perror) (*dseppp)(dspp, "DoSendfile: Short send: At most " PRINTF_LONG_LONG " of " PRINTF_LONG_LONG " bytes were received by client host.\n", twrote, tntoread);
				errno = EAGAIN;
				break;
			} else {
				/* complete */
				ntoread = 0;
				result = 0;
				gSendfileInProgress = 0;
			}
			break;
		} else {
			/* Error; note EINTR is not handled in the
			 * usual fashion.  If we get it, it should
			 * mean that may have caught an alarm (so we would
			 * not want to do the usual retry).
			 */
			if (gSendfileInProgress == -SIGURG) {
				if (do_perror) (*dseppp)(dspp, "DoSendfile: interrupted by SIGURG.\n");
				errno = EINTR;
			} else if (gSendfileInProgress == -SIGINT) {
				if (do_perror) (*dseppp)(dspp, "DoSendfile: interrupted by SIGINT.\n");
				errno = EINTR;
			} else if ((gSendfileInProgress == -SIGALRM) || ((oerrno == EINTR) && (using_alarm != 0))) {
				/* Timed-out */
				if (do_perror) (*dseppp)(dspp, "DoSendfile: interrupted by timeout alarm.\n");
				errno = ETIMEDOUT;
			} else if (oerrno == EINTR) {
				/* Retry */
				continue;
			} else if (oerrno == EAGAIN) {
				/* Retry; should only get here if the socket was set to non-blocking */
				/* The number of bytes that were written was set in "off" */
				twrote += off;
				off = twrote;
				if (dspip != NULL) {
					/* Progress meter update */
					dspip->current = twrote;
					if (dsppp != NULL)
						(*dsppp)(dspip);
				}

				/* fprintf(stderr, "FREEBSD: EAGAIN started at " PRINTF_LONG_LONG " of " PRINTF_LONG_LONG " bytes; ended at " PRINTF_LONG_LONG " bytes.\n", (longest_int) off1, tntoread, (longest_int) off); */

				FD_ZERO(&wfds); FD_SET(ofd, &wfds);
				FD_ZERO(&xfds); FD_SET(ifd, &xfds); FD_SET(ofd, &xfds);
				sel_tmout.tv_sec = xferTimeoutSec;
				sel_tmout.tv_usec = xferTimeoutUSec;
				nready = select(maxfd + 1, NULL, &wfds, &xfds, &sel_tmout);
				if (nready > 0)
					continue;	/* Ready to write again. */
				if (nready == 0) {
					/* Timed out */
					if (do_perror) (*dseppp)(dspp, "DoSendfile: socket write timed out after "
						PRINTF_LONG_LONG
						" of "
						PRINTF_LONG_LONG
						" bytes were sent.\n",
						twrote,
						tntoread
					);
					errno = ETIMEDOUT;
					break;
				}
				if (do_perror) (*dseppp)(dspp, "DoSendfile: select error: %s.\n", strerror(errno));
			} else if ((gSendfileInProgress == -SIGPIPE) || (oerrno == EPIPE)) {
				if (do_perror) (*dseppp)(dspp, "DoSendfile: broken pipe.\n");
				errno = EPIPE;
			} else if (oerrno == EWOULDBLOCK) {
				/* FreeBSD returns this if no data was sent
				 * through sendfile.
				 */
				gSendfileInProgress = -SIGALRM;
			} else {
				if (do_perror) (*dseppp)(dspp, "DoSendfile: Error occurred after " PRINTF_LONG_LONG " of " PRINTF_LONG_LONG " bytes were sent: %s.\n", twrote, tntoread, strerror(oerrno));
			}
			break;
		}
	}
	/* end FREEBSD, MACOSX */
#else
	if ((tntoread == 0) && (fstat(ifd, &st) == 0)) {
		tntoread = ntoread = st.st_size;
	}
	if (dspp->sendfileModeOnly != 0) {
		if (do_perror) (*dseppp)(dspp, "DoSendfile: Don't know how to sendfile() on this platform.\n");
		errno = ENOSYS;
		return (-1);
	} else if (dspp->buf == NULL) {
		if (do_perror) (*dseppp)(dspp, "DoSendfile: Don't know how to sendfile() on this platform and buffer size was not set for regular userspace mode.\n");
		errno = ENOSYS;
		return (-1);
	}
#endif	/* OTHER */

userspace_loop:
    ntowrite = 0;
	if ((ntoread == tntoread) && (tntoread > 0) && (dspp->sendfileModeOnly != 0)) {
		errno = ENOSYS;
		result = -1;
	} else if ((ntoread == tntoread) && (tntoread > 0) && (dspp->buf != NULL)) {
		memset(dspp->buf, 0, dspp->bufSize);
		gSendfileInProgress = 1;
		while (ntoread > 0) {
			ntowrite = 0;

			if (gSendfileInProgress == -SIGURG) {
				if (do_perror) (*dseppp)(dspp, "DoSendfile: interrupted by SIGURG.\n");
				errno = EINTR;
				goto umodebreak;
			} else if (gSendfileInProgress == -SIGINT) {
				if (do_perror) (*dseppp)(dspp, "DoSendfile: interrupted by SIGINT.\n");
				errno = EINTR;
				goto umodebreak;
			} else if ((gSendfileInProgress == -SIGPIPE) || (oerrno == EPIPE)) {
				if (do_perror) (*dseppp)(dspp, "DoSendfile: broken pipe.\n");
				errno = EPIPE;
				goto umodebreak;
			} else if ((gSendfileInProgress == -SIGALRM) || ((oerrno == EINTR) && (using_alarm != 0))) {
				/* Timed-out */
				if (do_perror) (*dseppp)(dspp, "DoSendfile: interrupted by timeout alarm.\n");
				errno = ETIMEDOUT;
				goto umodebreak;
			}

			nread = read(ifd, dspp->buf, dspp->bufSize);
			if (nread == 0)
				break;
			if (nread < 0) {
				if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
					FD_ZERO(&rfds); FD_SET(ifd, &rfds);
					FD_ZERO(&xfds); FD_SET(ifd, &xfds);
					sel_tmout.tv_sec = xferTimeoutSec;
					sel_tmout.tv_usec = xferTimeoutUSec;
					nready = select(maxfd + 1, NULL, &wfds, &xfds, &sel_tmout);
					if (nready > 0)
						continue;	/* Ready to read again. */
					if (nready == 0) {
						/* Timed out */
						if (do_perror) (*dseppp)(dspp, "DoSendfile: read timed out after "
							PRINTF_LONG_LONG
							" of "
							PRINTF_LONG_LONG
							" bytes were sent.\n",
							twrote,
							tntoread
						);
						errno = ETIMEDOUT;
						goto umodebreak;
					}
					/* select failed */
					if (errno == EINTR)
						continue;
					if (do_perror) (*dseppp)(dspp, "DoSendfile: select error: %s.\n", strerror(errno));
					goto umodebreak;
				}
				if (errno == EINTR)
					continue;
				if (do_perror) (*dseppp)(dspp, "DoSendfile: read error on fd=%d: %s\n", ifd, strerror(errno));
				return (-1);
			}
			ntoread -= nread;

			ntowrite = nread;
			while (ntowrite > 0) {
				if (gSendfileInProgress == -SIGURG) {
					if (do_perror) (*dseppp)(dspp, "DoSendfile: interrupted by SIGURG.\n");
					errno = EINTR;
					goto umodebreak;
				} else if (gSendfileInProgress == -SIGINT) {
					if (do_perror) (*dseppp)(dspp, "DoSendfile: interrupted by SIGINT.\n");
					errno = EINTR;
					goto umodebreak;
				} else if ((gSendfileInProgress == -SIGPIPE) || (oerrno == EPIPE)) {
					if (do_perror) (*dseppp)(dspp, "DoSendfile: broken pipe.\n");
					errno = EPIPE;
					goto umodebreak;
				} else if ((gSendfileInProgress == -SIGALRM) || ((oerrno == EINTR) && (using_alarm != 0))) {
					/* Timed-out */
					if (do_perror) (*dseppp)(dspp, "DoSendfile: interrupted by timeout alarm.\n");
					errno = ETIMEDOUT;
					goto umodebreak;
				}
				nwrote = (ssize_t) write(ofd, dspp->buf + (nread - ntowrite), (size_t) ntowrite);
				if (nwrote < 0) {
					if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
						FD_ZERO(&wfds); FD_SET(ofd, &wfds);
						FD_ZERO(&xfds); FD_SET(ifd, &xfds); FD_SET(ofd, &xfds);
						sel_tmout.tv_sec = xferTimeoutSec;
						sel_tmout.tv_usec = xferTimeoutUSec;
						nready = select(maxfd + 1, NULL, &wfds, &xfds, &sel_tmout);
						if (nready > 0)
							continue;	/* Ready to write again. */
						if (nready == 0) {
							/* Timed out */
							if (do_perror) (*dseppp)(dspp, "DoSendfile: socket write timed out after "
								PRINTF_LONG_LONG
								" of "
								PRINTF_LONG_LONG
								" bytes were sent.\n",
								twrote,
								tntoread
							);
							errno = ETIMEDOUT;
							goto umodebreak;
						}
						/* select failed */
						if (do_perror) (*dseppp)(dspp, "DoSendfile: select error: %s.\n", strerror(errno));
						goto umodebreak;
					}
					if (errno == EINTR)
						continue;
					if (do_perror) (*dseppp)(dspp, "DoSendfile: write error on fd=%d: %s\n", ofd, strerror(errno));
					goto umodebreak;
				}
				twrote += nwrote;
				ntowrite -= nwrote;
				if (dspip != NULL) {
					/* Progress meter update */
					dspip->current = twrote;
					if (dsppp != NULL)
						(*dsppp)(dspip);
				}
			}
		}
umodebreak:
		result = ((ntoread > 0) || (ntowrite > 0)) ? -1 : 0;
	}
	
	gSendfileInProgress = 0;
	if (using_alarm != 0)
		(void) alarm(0);
	(void) sigaction(SIGALRM, (struct sigaction *) &oldsa, NULL);
	(void) sigaction(SIGPIPE, (struct sigaction *) &oldsp, NULL);
	if (dspp->handleSIGINT) {
		(void) sigaction(SIGINT, (struct sigaction *) &oldsi, NULL);
	}
#ifdef SIGURG
	(void) sigaction(SIGURG, (struct sigaction *) &oldsu, NULL);
#endif	/* SIGURG */

	if (osize != NULL)
		*osize = twrote;
	return (result);
}	/* DoSendfile */

#endif
