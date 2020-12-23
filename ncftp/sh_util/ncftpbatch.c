/* ncftpbatch.c
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
	INITCOMMONCONTROLSEX gComCtls;
	HINSTANCE ghInstance = 0;
	HWND gMainWnd = 0;
	HWND gStaticCtrl = 0;
#	include "..\ncftp\util.h"
#	include "..\ncftp\pref.h"
#	include "..\ncftp\spool.h"
#	include "resource.h"
#	include "gpshare.h"
#	define getpid _getpid
#else
#	define YieldUI(a)
#	include "../ncftp/util.h"
#	include "../ncftp/pref.h"
#	include "../ncftp/spool.h"
#	include "gpshare.h"
#endif

#ifdef HAVE_LONG_FILE_NAMES

#define kNcFTPBatchDefaultUseSendfile 1

#define kSpoolDir "spool"
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
#	define kSpoolLog "log.txt"
#else
#	define kSpoolLog "log"
#endif

#define kMinimumReconnectDelaySameHost 30

#define NEW_SLEEP_VAL(s) ((unsigned int) (((0.1 * (rand() % 15)) + 1.2) * (s))) 

int gQuitRequested = 0;
int gNeedOpen = 0;
int gGlobalSpooler = 0;
off_t gMaxLogSize = 10 * 1024 * 1024;
unsigned int gDelayBetweenPasses = 0;
int gGotSig = 0;
FTPLibraryInfo gLib;
FTPConnectionInfo gConn;
int gIsTTY;
int gSpooled = 0;
char gSpoolDir[512];
char gLogFileName[512];
char gXferLogFileName[512];
const char *gJobID = "?123";
struct dirent *gDirentBuf = NULL;
size_t gDirentBufSize = 0;
extern int gFirewallType;
extern char gFirewallHost[64];
extern char gFirewallUser[32];
extern char gFirewallPass[32];
extern char gFirewallExceptionList[256];
extern unsigned int gFirewallPort;
extern int gFwDataPortMode;
FILE *savefp;
int gCurSpoolFileLinesRead;
int gItemInUse = 0;
char gItemPath[1024];
char *gItemContents = NULL;
size_t gItemContentsAllocSize = 0;
size_t gItemContentsSize = 0;
char gMyItemPath[1024];
int gOperation;
char gOperationStr[16];
unsigned int gDelaySinceLastFailure;
time_t gTimeOfFirstAttempt;
time_t gTimeOfLastFailure;
time_t gTimeOfLastFailureAny;
char gHost[64];
char gLastHost[64];
char gHostIP[32];
unsigned int gPort;
char gRUser[128];
char gRPass[128];
char gRAcct[128];
char gManualOverrideFeatures[256];
char gPreFTPCommand[128];
char gPerFileFTPCommand[128];
char gPostFTPCommand[128];
char gPreShellCommand[256];
char gPostShellCommand[256];
char gProgressLog[512];
FILE *gProgLog = NULL;
int gXtype;
int gRecursive;
int gDelete;
int gPassive;
char gRDir[1024];
char gRNewDir[1024];
char gLDir[1024];
char gLNewDir[1024];
char gRFile[256];
char gRNewFile[256];
double gRFileSize;
time_t gRFileMtime;
char gLFile[256];
char gLNewFile[256];
double gLFileSize;
time_t gLFileMtime;
char gRStartDir[1024];
char gRPrevDir[1024];
char gSourceAddrStr[128];
char gLockFile[256];
int gLockFd = -1;
int gBatchMiscBuf[4096];

/* Writes logging data to a ~/.ncftp/spool/log file.
 * This is nice for me when I need to diagnose problems.
 */
FILE *gLogFile = NULL;
time_t gLogTime;
char gLogLBuf[256];
unsigned int gMyPID;
const char *gLogOpenMode = FOPEN_APPEND_TEXT;
int gUnused;
int gMayCancelJmp = 0;
int gMaySigExit = 1;

#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
char gStatusText[512];
#else
#ifdef HAVE_SIGSETJMP
sigjmp_buf gCancelJmp;
#else	/* HAVE_SIGSETJMP */
jmp_buf gCancelJmp;
#endif	/* HAVE_SIGSETJMP */
#endif

extern const char gOS[], gVersion[];

time_t AtoTime_t(const char *const tok, time_t *t);
extern struct dirent *Readdir(DIR *const dir, struct dirent *const dp, const size_t sz);
static void ErrBox(const char *const fmt, ...)
#if (defined(__GNUC__)) && (__GNUC__ >= 2)
__attribute__ ((format (printf, 1, 2)))
#endif
;

time_t
AtoTime_t(const char *const tok, time_t *t)
{
	/* This function may later be extended to handle non-timestamp formats like YYYY-mm-dd HH:MM:SS */
	time_t r = 0;
	unsigned long ul;

	if (tok != NULL) {
		ul = 0;
		if (sscanf(tok, "%lu", &ul) == 1) {
			r = (time_t) ul;
		}
	}
	if (t != NULL)
		*t = r;
	return (r);
}	/* AtoTime_t */




static void
Log(
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
		int uiShow,
#else
		int UNUSED(uiShow),
#endif
		const char *const fmt, ...)
#if (defined(__GNUC__)) && (__GNUC__ >= 2)
__attribute__ ((format (printf, 2, 3)))
#endif
;

static void LogPerror(const char *const fmt, ...)
#if (defined(__GNUC__)) && (__GNUC__ >= 2)
__attribute__ ((format (printf, 1, 2)))
#endif
;

static void LogEndItemResult(int uiShow, int errorOccurred, const char *const fmt, ...)
#if (defined(__GNUC__)) && (__GNUC__ >= 2)
__attribute__ ((format (printf, 3, 4)))
#endif
;



#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
static void YieldUI(int redraw)
{
	MSG msg;

	if (redraw)
		InvalidateRect(gMainWnd, NULL, (redraw > 1));

	while (PeekMessage (&msg, gMainWnd, 0, 0, PM_REMOVE)) {
		TranslateMessage (&msg);
		DispatchMessage (&msg);
	}
}	// YieldUI
#endif



static void ErrBox(const char *const fmt, ...)
{
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
	char buf[512];
	va_list ap;

	ZeroMemory(buf, sizeof(buf));
	va_start(ap, fmt);
	wvsprintf(buf, fmt, ap);
	va_end(ap);

	MessageBox((gMainWnd != NULL) ? gMainWnd : GetDesktopWindow(),
		buf, "Error", MB_OK | MB_ICONINFORMATION);
#else
	va_list ap;

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
#endif
}	/* ErrBox */




static void PerrorBox(const char *const whatFailed)
{
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
	char errMsg[256];

	(void) FormatMessage(
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		errMsg,
		sizeof(errMsg),
		NULL
	);

	(void) ErrBox("%s: %s\n", whatFailed, errMsg);
#else
	perror(whatFailed);
#endif
}	/* PerrorBox */



/*VARARGS*/
static void
Log(
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
		int uiShow,
#else
		int UNUSED(uiShow),
#endif
		const char *const fmt, ...)
{
	va_list ap;
	struct tm lt;
	char tstr[128];
	
	if (gLogFile != NULL) {
		strftime(tstr, sizeof(tstr), "%Y-%m-%d %H:%M:%S %Z", Localtime(time(&gLogTime), &lt));
		(void) fprintf(gLogFile,
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
			"%s [$%08x] | ",
#else
			"%s [%06u] | ",
#endif
			tstr,
			gMyPID
		);
		va_start(ap, fmt);
		(void) vfprintf(gLogFile, fmt, ap);
		va_end(ap);
	}
	if (gIsTTY != 0) {
		va_start(ap, fmt);
		(void) vfprintf(stdout, fmt, ap);
		va_end(ap);
	}
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
	if (uiShow) {
		char *cp;

		va_start(ap, fmt);
		(void) vsprintf(gStatusText, fmt, ap);
		va_end(ap);
		cp = gStatusText + strlen(gStatusText) - 1;
		while (iscntrl(*cp)) {
			*cp-- = '\0';
		}
		YieldUI(2);
	}
#else
	LIBNCFTP_USE_VAR(uiShow);
#endif
}	/* Log */



/*VARARGS*/
static void
LogPerror(const char *const fmt, ...)
{
	va_list ap;
	struct tm lt;
	int oerrno;
	char tstr[128];
	
	oerrno = errno;
	if (gLogFile != NULL) {
		strftime(tstr, sizeof(tstr), "%Y-%m-%d %H:%M:%S %Z", Localtime(time(&gLogTime), &lt));
		(void) fprintf(gLogFile,
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
			"%s [$%08x] | ",
#else
			"%s [%06u] | ",
#endif
			tstr,
			gMyPID
		);
		va_start(ap, fmt);
		(void) vfprintf(gLogFile, fmt, ap);
		va_end(ap);
#ifdef HAVE_STRERROR
		(void) fprintf(gLogFile, ": %s\n", strerror(oerrno));
#else
		(void) fprintf(gLogFile, ": errno=%d\n", (oerrno));
#endif
	}
	if (gIsTTY != 0) {
		va_start(ap, fmt);
		(void) vfprintf(stdout, fmt, ap);
		va_end(ap);
#ifdef HAVE_STRERROR
		(void) fprintf(stdout, ": %s\n", strerror(oerrno));
#else
		(void) fprintf(stdout, ": errno=%d\n", (oerrno));
#endif
	}
}	/* LogPerror */



#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
void
PrWinStatBar(const FTPCIPtr cip, int mode)
{
	switch (mode) {
		case kPrInitMsg:
			if (gLogFile != NULL)
				(void) fflush(gLogFile);
			YieldUI(2);
			break;

		case kPrUpdateMsg:
			YieldUI(1);
			break;

		case kPrEndMsg:
			if (gLogFile != NULL)
				(void) fflush(gLogFile);
			YieldUI(1);
			break;
	}
}	/* PrWinStatBar */
#endif



static void
DebugHook(const FTPCIPtr cipUnused, char *msg)
{
	gUnused = cipUnused != 0;			/* shut up gcc */
	Log(0, "  %s", msg);
}	/* DebugHook */




static void
CloseLog(void)
{
	if (gLogFile != NULL) {
		(void) fclose(gLogFile);
		gLogFile = NULL;
	}
}	/* CloseLog */




static int
OpenLog(void)
{
	FILE *fp;
	struct Stat st;
	const char *openMode;

	CloseLog();
	if ((gLogFileName[0] == '\0') || (strcasecmp(gLogFileName, "/dev/null") == 0))
		return (0);

	openMode = gLogOpenMode;
	if ((gMaxLogSize > 0) && (Stat(gLogFileName, &st) == 0) && ((longest_int) st.st_size > gMaxLogSize)) {
		/* Prevent the log file from growing forever. */
		openMode = FOPEN_WRITE_TEXT;
	}

#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
	fp = _fsopen(gLogFileName, openMode, _SH_DENYNO);
#else
	fp = fopen(gLogFileName, openMode);
#endif

	if (fp != NULL) {
#ifdef HAVE_SETVBUF
		/* Note: On Win32, _IOLBF is the same as _IOFBF.  Bleeeeh.  */
		(void) setvbuf(fp, gLogLBuf, _IOLBF, sizeof(gLogLBuf));
#endif	/* HAVE_SETVBUF */
		(void) time(&gLogTime);
		gLogFile = fp;
		gMyPID = (unsigned int) getpid();
		return (0);
	}
	return (-1);
}	/* OpenLog */




static int
DeleteFileWithRetries(const char *const pn, const int logIfNoEnt)
{
	/* Mostly for oddities under Windows/Cygwin */
	errno = 0;
	if (unlink(pn) == 0) { return 0; }
#ifdef ENOENT
	if (errno == ENOENT) {
		if (logIfNoEnt)
			Log(0, "Hmmm, could not delete %s, because it already does not exist? (%s)\n", pn, strerror(errno));
		return 0;
	}
#endif
	Log(0, "Hmmm, could not delete %s, will try again: %s\n", pn, strerror(errno));
	sleep((rand() % 3) + 1);
	if (unlink(pn) == 0) { return 0; }
	Log(0, "Hmmm, still could not delete %s, will one more time: %s\n", pn, strerror(errno));
	sleep((rand() % 11) + 4);
	if (unlink(pn) == 0) { return 0; }
	return (-1);
}	/* DeleteFileWithRetries */




static void
Respool(const int logIfNoEnt)
{
	char tstr[64];
	time_t tnext;
	struct tm tnext_tm;
	char sname[64];
	char initialpath[256];
	char finalpath[256];
	FILE *fp = NULL;
	size_t nread, nwrote;

	initialpath[0] = '\0';
	finalpath[0] = '\0';

	if (gDelaySinceLastFailure == 0)
		gDelaySinceLastFailure = 5;
	else {
		gDelaySinceLastFailure = NEW_SLEEP_VAL(gDelaySinceLastFailure);
		if (gDelaySinceLastFailure > 900) {
			/* If sleep duration got so large it got past 15 minutes,
			 * start over again.
			 */
			gDelaySinceLastFailure = 60;
		}
	}
	tnext = time(NULL) + (time_t) gDelaySinceLastFailure;
	strftime(tstr, sizeof(tstr), "%Y-%m-%d %H:%M:%S %Z", Localtime(tnext, &tnext_tm));

	gMaySigExit = 0;

	fp = OpenSpoolFile(NULL, initialpath, sizeof(initialpath), finalpath, sizeof(finalpath), sname, sizeof(sname), gSpoolDir, gOperationStr, tnext);
	if (fp == NULL) {
		Log(0, "Could not open spool file for writing: %s\n", strerror(errno));
		return;
	}

	if (WriteSpoolEntry(
		fp,
		gOperationStr,
		sname,
		gRFile,
		gRDir,
		gRFileSize,
		gRFileMtime,
		gLFile,
		gLDir,
		gLFileSize,
		gLFileMtime,
		gRNewFile,
		gRNewDir,
		gLNewFile,
		gLNewDir,
		gHost,
		gHostIP,
		gPort,
		gRUser,
		gRPass,
		gRAcct,
		gXtype,
		gRecursive,
		gDelete,
		gPassive,
		gPreFTPCommand,
		gPerFileFTPCommand,
		gPostFTPCommand,
		gPreShellCommand,
		gPostShellCommand,
		gDelaySinceLastFailure,
		gManualOverrideFeatures,
		gSourceAddrStr,
		gTimeOfFirstAttempt,
		gTimeOfLastFailure
	) < 0) {
		Log(0, "write to spoolfile (%s) failed: %s\n", initialpath, strerror(errno));
		goto err;
	}

	if (savefp != NULL) {
		/* If we hadn't finished reading all of the entries from the
		 * transaction file, then append the remaining entries to the
		 * newly created transaction file we just made.
		 */
		if (fprintf(fp, "\n\n\n%s\n", kSpoolEntryBeginLine) < 0)
			goto err;
		while ((nread = fread(gBatchMiscBuf, sizeof(char), sizeof(gBatchMiscBuf), savefp)) > 0) {
			if ((nwrote = fwrite(gBatchMiscBuf, sizeof(char), nread, fp)) != nread) {
				Log(0, "write to spoolfile (%s) failed: %s\n", initialpath, strerror(errno));
				goto err;
			}
		}
		(void) fclose(savefp);
		savefp = NULL;
	}

	if (CloseSpoolFileAndRename(fp, (gLogFile != NULL) ? gLogFile : stderr, initialpath, finalpath) < 0) {
		Log(0, "close/rename to spoolfile (%s) failed: %s\n", initialpath, strerror(errno));
		goto err;
	}

	if (DeleteFileWithRetries(gMyItemPath, logIfNoEnt) != 0) {
		/* quit now */
		Log(0, "Could not delete old copy of job %s!\n", gMyItemPath);
		return;
	}
	Log(0, "Rescheduled %s for %s as %s.\n", gItemPath, tstr, finalpath);

	gMaySigExit = 1;
	return;

err:
	if (savefp != NULL)
		(void) fclose(savefp);
	savefp = NULL;
	if (fp != NULL) {
		(void) fclose(fp);
	}
	if (finalpath[0] != '\0')
		(void) unlink(finalpath);
}	/* Respool */




static void
ExitStuff(void)
{
	if (gItemInUse > 0) {
		(void) rename(gMyItemPath, gItemPath);
		gItemInUse = 0;
	}
}	/* ExitStuff */



#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
#elif 0
static void
SigAlrm(int sigNum)
{
	if (gMayCancelJmp != 0) {
		gUnused = sigNum;
		if (gItemInUse > 0) {
			(void) rename(gMyItemPath, gItemPath);
			gItemInUse = 0;
		}
#ifdef HAVE_SIGSETJMP
		siglongjmp(gCancelJmp, 1);
#else	/* HAVE_SIGSETJMP */
		longjmp(gCancelJmp, 1);
#endif	/* HAVE_SIGSETJMP */
	}
}	/* SigAlrm */
#endif




static void
SigExit(int sigNum)
{
#ifdef SIGUSR2
	if (sigNum == SIGUSR2) {
		/* Only used to break out of sleep() */
		Log(0, "-----caught signal %d, continuing-----\n", sigNum);
		return;
	}
#endif
	gQuitRequested = sigNum;

	/* If it was SIGUSR1, we will exit when it is convenient for us,
	 * otherwise for any other signal we will exit if we may.
	 */
	if ((sigNum !=
#ifdef SIGUSR1
	SIGUSR1
#else
	-1
#endif
	  ) && (gMaySigExit != 0)) {
#ifdef SIGBUS
		if ((sigNum == SIGSEGV) || (sigNum == SIGBUS) || (sigNum == SIGILL)) {
#else
		if ((sigNum == SIGSEGV) || (sigNum == SIGILL)) {
#endif
			ExitStuff();
			Log(0, "-----caught signal %d, aborting-----\n", sigNum);
			DisposeWinsock();

			/* Need to do this, because we may have been
			 * in the root directory which we probably
			 * can't write the core file to.
			 */
			if ((chdir("/tmp") == 0) || (chdir("/") == 0))
				abort();
		} else {
			Log(0, "-----caught signal %d, exiting-----\n", sigNum);
			if (gItemInUse) {
				/* Regenerate an updated file rather than renaming the old one. */
				Respool(1);
				gItemInUse = 0;
			}
			DisposeWinsock();
			exit(0);
		}
	}
}	/* SigExit */




static void
FTPInit(void)
{
	int result;

	InitWinsock();
	result = FTPInitLibrary(&gLib);
	if (result < 0) {
		ErrBox("ncftpbatch: init library error %d (%s).\n", result, FTPStrError(result));
		DisposeWinsock();
		exit(1);
	}

	result = FTPInitConnectionInfo(&gLib, &gConn, kDefaultFTPBufSize);
	if (result < 0) {
		ErrBox("ncftpbatch: init connection info error %d (%s).\n", result, FTPStrError(result));
		DisposeWinsock();
		exit(1);
	}
}	/* FTPInit */




static void
InitHostVariables(void)
{
	memset(gRStartDir, 0, sizeof(gRStartDir));
	memset(gRPrevDir, 0, sizeof(gRPrevDir));
}	/* InitHostVariables(void) */




/* These things are done first, before we even parse the command-line
 * options.
 */
static void
PreInit(const char *const prog)
{
	const char *cp;

#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
	gIsTTY = 0;
	ZeroMemory(gStatusText, sizeof(gStatusText));
#else
	gIsTTY = ((isatty(2) != 0) && (getppid() > 1)) ? 1 : 0;
	umask(077);
#endif
#ifdef SIGPOLL
	NcSignal(SIGPOLL, (FTPSigProc) SIG_IGN);
#endif
	InitUserInfo();

	FTPInit();
	srand((unsigned int) getpid());

	memset(gItemPath, 0, sizeof(gItemPath));
	memset(gMyItemPath, 0, sizeof(gMyItemPath));
	memset(gSpoolDir, 0, sizeof(gSpoolDir));
	memset(gLogFileName, 0, sizeof(gLogFileName));
	memset(gXferLogFileName, 0, sizeof(gXferLogFileName));
	memset(gProgressLog, 0, sizeof(gProgressLog));
	memset(gRDir, 0, sizeof(gRDir));
	memset(gLDir, 0, sizeof(gLDir));
	memset(gLNewDir, 0, sizeof(gLNewDir));
	memset(gRNewDir, 0, sizeof(gRNewDir));
	memset(gRFile, 0, sizeof(gRFile));
	memset(gLFile, 0, sizeof(gLFile));
	memset(gLNewFile, 0, sizeof(gLNewFile));
	memset(gRNewFile, 0, sizeof(gRNewFile));
	memset(gManualOverrideFeatures, 0, sizeof(gManualOverrideFeatures));
	memset(gLockFile, 0, sizeof(gLockFile));
	memset(gBatchMiscBuf, 0, sizeof(gBatchMiscBuf));
	gTimeOfFirstAttempt = 0;
	gTimeOfLastFailure = 0;
	gTimeOfLastFailureAny = 0;
	savefp = NULL;
	gCurSpoolFileLinesRead = 0;
	InitHostVariables();

	LoadFirewallPrefs(0);

	cp = strrchr(prog, '/');
	if (cp == NULL)
		cp = strrchr(prog, '\\');
	if (cp == NULL)
		cp = prog;
	else
		cp++;
	if (strncasecmp(cp, "ncftpspool", 10) == 0)
		gGlobalSpooler = 1;

	cp = getenv("NCFTPBATCH_PROGRESS_LOG");
	if (cp != NULL)
		STRNCPY(gProgressLog, cp);

	(void) signal(SIGINT, SigExit);
	(void) signal(SIGTERM, SigExit);
#ifdef SIGUSR1
	(void) signal(SIGUSR1, SigExit);
#endif
#ifdef SIGUSR2
	(void) signal(SIGUSR2, SigExit);
#endif
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
#else
	(void) signal(SIGSEGV, SigExit);
	(void) signal(SIGBUS, SigExit);
	(void) signal(SIGFPE, SigExit);
	(void) signal(SIGILL, SigExit);
#if defined(SIGIOT) && (SIGIOT != SIGABRT)
	(void) signal(SIGIOT, SigExit);
#endif
#ifdef SIGEMT
	(void) signal(SIGEMT, SigExit);
#endif
#ifdef SIGSYS
	(void) signal(SIGSYS, SigExit);
#endif
#ifdef SIGSTKFLT
	(void) signal(SIGSTKFLT, SigExit);
#endif
#endif
}	/* PreInit */



static void
PostInit(void)
{
	struct dirent *direntbuf;
	size_t debufsize;
#ifdef HAVE_PATHCONF
	long nmx;
#endif
	/* These things are done after parsing the command-line options. */

	if (gGlobalSpooler != 0) {
#if defined(__CYGWIN__)
		if (gLockFile[0] == '\0')
			STRNCPY(gLockFile, "/var/lock/ncftpspooler.lck");
#endif
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
#else
		if (gSpoolDir[0] == '\0')
			STRNCPY(gSpoolDir, "/var/spool/ncftp");
		if (chdir(gSpoolDir) < 0) {
			/* Print a warning if we can't access it yet,
			 * but allow for it to be created later by
			 * an external process.
			 */
			perror(gSpoolDir);
			/* continue */
		}
		if (chdir("/") < 0)
			exit(1);
#endif
		if (gDelayBetweenPasses == 0)
			gDelayBetweenPasses = 120;
	} else {
		if (gSpoolDir[0] == '\0')
			(void) OurDirectoryPath(gSpoolDir, sizeof(gSpoolDir), kSpoolDir);
	}

#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
#else
	if (gLockFile[0] != '\0') {
		char pidbuf[32];

		gLockFd = open(gLockFile, O_RDWR|O_CREAT|O_TRUNC, 00666);
		if (gLockFd < 0) {
			perror(gLockFile);
			exit(1);
		}
		/* FYI only, not important, but write the most recent process' PID to the file. */
		sprintf(pidbuf, "%u\n", (unsigned int) getpid());
		if (write(gLockFd, pidbuf, strlen(pidbuf)) < 0) {
			perror(gLockFile);
			exit(1);
		}
	}
#endif

	if (gLogFileName[0] == '\0')
		(void) Path(gLogFileName, sizeof(gLogFileName), gSpoolDir, kSpoolLog);
	debufsize = 512;
#ifdef HAVE_PATHCONF
	nmx = pathconf(gLogFileName, _PC_NAME_MAX);
	if (nmx >= 512)
		debufsize = (size_t) nmx;
#endif
	debufsize += sizeof(struct dirent) + 8;
	direntbuf = (struct dirent *) calloc(debufsize, (size_t) 1);
	if (direntbuf == NULL) {
		PerrorBox("malloc failed for dirent buffer");
		exit(1);
	}
	gDirentBuf = direntbuf;
	gDirentBufSize = debufsize;
}	/* PostInit */




/* These things are just before the program exits. */
static void
PostShell(void)
{
	CloseLog();
}	/* PostShell */




static int
LoadCurrentSpoolFileContents(int logErrors)
{
	FILE *fp;
	char line[256];
	size_t hlen;
	char *tok1, *tok2;
	char *cp, *lim;
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
#else
	struct stat st;
#endif

	fp = savefp;	/* will be NULL if this is first entry. */
	if (fp == NULL) {
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
	/* gItemContents is not used on Win32 */
	if ((fp = _fsopen(gMyItemPath, FOPEN_READ_TEXT, _SH_DENYNO)) == NULL) {
		/* Could have been renamed already. */
		if (logErrors != 0)
			LogPerror("%s", gMyItemPath);
		return (-1);
	}
#else
	if ((stat(gMyItemPath, &st) < 0) || ((fp = fopen(gMyItemPath, FOPEN_READ_TEXT)) == NULL)) {
		/* Could have been renamed already. */
		if (logErrors != 0)
			LogPerror("%s", gMyItemPath);
		return (-1);
	}
#endif
	}

#if 0
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
#else
	/* Make sure our global buffer to contain the contents of
	 * the current spool file is ready to use.
	 */
	if ((size_t) st.st_size > gItemContentsAllocSize) {
		gItemContentsAllocSize = (size_t) (st.st_size + (4096 - (st.st_size % 4096)));
		if (gItemContentsAllocSize == 0) {
			cp = malloc(gItemContentsAllocSize);
		} else {
			cp = realloc(gItemContents, gItemContentsAllocSize);
		}
		if (cp == NULL) {
			if (logErrors != 0)
				LogPerror("malloc");
			return (-1);
		}
		memset(cp, 0, gItemContentsAllocSize);
		gItemContents = cp;
	} else {
		cp = gItemContents;
	}

	/* Load the entire image of the spool file. */
	gItemContentsSize = (size_t) st.st_size;
	if (fread(cp, (size_t) 1, gItemContentsSize, fp) != gItemContentsSize) {
		LogPerror("fread from %s", gMyItemPath);
		fclose(fp);
		return (-1);
	}
	memset(cp + gItemContentsSize, 0, gItemContentsAllocSize - gItemContentsSize);

	/* Rewind it since we have to re-read it for parsing. */
	if (fseek(fp, 0L, SEEK_SET) != 0L) {
		LogPerror("rewind %s", gMyItemPath);
		fclose(fp);
		return (-1);
	}
#endif /* UNIX */

#endif

	gOperation = '?';
	STRNCPY(gOperationStr, "?");
	gHost[0] = '\0';
	gHostIP[0] = '\0';
	gPort = kDefaultFTPPort;
	gRUser[0] = '\0';
	gRPass[0] = '\0';
	gRAcct[0] = '\0';
	gXtype = 'I';
	gRecursive = 0;
	gDelete = 0;
	gPassive = 2;
	if (gFwDataPortMode >= 0)
		gPassive = gFwDataPortMode;
	gRDir[0] = '\0';
	gLDir[0] = '\0';
	gLNewDir[0] = '\0';
	gRNewDir[0] = '\0';
	gRFile[0] = '\0';
	gLFile[0] = '\0';
	gLFileSize = -1;
	gRFileSize = -1;
	gRFileMtime = 0;
	gLFileMtime = 0;
	gLNewFile[0] = '\0';
	gRNewFile[0] = '\0';
	gManualOverrideFeatures[0] = '\0';
	gSourceAddrStr[0] = '\0';
	gPreFTPCommand[0] = '\0';
	gPerFileFTPCommand[0] = '\0';
	gPostFTPCommand[0] = '\0';
	gPreShellCommand[0] = '\0';
	gPostShellCommand[0] = '\0';
	gDelaySinceLastFailure = 0;
	gTimeOfFirstAttempt = 0;
	gTimeOfLastFailure = 0;

	if (savefp == NULL)
		gCurSpoolFileLinesRead = 0;
	line[sizeof(line) - 1] = '\0';
	hlen = strlen(kSpoolEntryBegin);
	for (;;) {
		if (fgets(line, sizeof(line) - 1, fp) == NULL) {
			/* Reached end of the transaction file. */
			savefp = NULL;
			break;
		}
		gCurSpoolFileLinesRead++;
		if (strncmp(line, kSpoolEntryBegin, hlen) == 0) {
			if (gCurSpoolFileLinesRead <= 1)
				continue;
			/* Found start of the next entry.
			 * This is a multiple-entry transaction file. 
			 */
			savefp = fp;
			break;
		}
		if (strncmp(line, "# local-", 8) == 0) {	/* Temporary hack, can be removed */
			tok1 = strtok(line + 2, " =\t\r\n");
		} else {
			tok1 = strtok(line, " =\t\r\n");
		}
		if (tok1 == NULL)
			continue;
		if (tok1[0] == '#')
			continue;
		tok2 = strtok(NULL, "\r\n");
		if (tok2 == NULL)
			continue;
		if (strcmp(tok1, "op") == 0) {
			gOperation = tok2[0];
			STRNCPY(gOperationStr, tok2);
		} else if (strcmp(tok1, "delay-since-last-failure") == 0) {
			gDelaySinceLastFailure = (unsigned int) atoi(tok2);
		} else if (strcmp(tok1, "time-of-first-attempt") == 0) {
			AtoTime_t(tok2, &gTimeOfFirstAttempt);
		} else if (strcmp(tok1, "time-of-last-failure") == 0) {
			AtoTime_t(tok2, &gTimeOfLastFailure);
		} else if (strcmp(tok1, "hostname") == 0) {
			(void) STRNCPY(gHost, tok2);
		} else if (strcmp(tok1, "host-ip") == 0) {
			/* Don't really use this anymore, it is
			 * only used if the host is not set.
			 */
			(void) STRNCPY(gHostIP, tok2);
		} else if (strcmp(tok1, "port") == 0) {
			gPort = (unsigned int) atoi(tok2);
		} else if (strcmp(tok1, "passive") == 0) {
			if (isdigit((int) tok2[0]))
				gPassive = atoi(tok2);
			else
				gPassive = StrToBool(tok2);
		} else if (strncmp(tok1, "user", 4) == 0) {
			(void) STRNCPY(gRUser, tok2);
		} else if (strncmp(tok1, "pass", 4) == 0) {
			(void) STRNCPY(gRPass, tok2);
		} else if (strcmp(tok1, "anon-pass") == 0) {
			(void) STRNCPY(gLib.defaultAnonPassword, tok2);
		} else if (strncmp(tok1, "acc", 3) == 0) {
			(void) STRNCPY(gRAcct, tok2);
		} else if (strcmp(tok1, "xtype") == 0) {
			gXtype = tok2[0];
		} else if (strcmp(tok1, "recursive") == 0) {
			gRecursive = StrToBool(tok2);
		} else if (strcmp(tok1, "delete") == 0) {
			gDelete = StrToBool(tok2);
		} else if (strcmp(tok1, "remote-dir") == 0) {
			(void) STRNCPY(gRDir, tok2);
		} else if (strcmp(tok1, "local-dir") == 0) {
			(void) STRNCPY(gLDir, tok2);
		} else if (strcmp(tok1, "remote-file") == 0) {
			(void) STRNCPY(gRFile, tok2);
		} else if (strcmp(tok1, "local-file") == 0) {
			(void) STRNCPY(gLFile, tok2);
		} else if (strcmp(tok1, "local-rename-dir") == 0) {
			(void) STRNCPY(gLNewDir, tok2);
		} else if (strcmp(tok1, "local-rename-file") == 0) {
			(void) STRNCPY(gLNewFile, tok2);
		} else if (strcmp(tok1, "remote-rename-dir") == 0) {
			(void) STRNCPY(gRNewDir, tok2);
		} else if (strcmp(tok1, "remote-rename-file") == 0) {
			(void) STRNCPY(gRNewFile, tok2);
		} else if ((strcmp(tok1, "local-file-size") == 0) || (strcmp(tok1, "local-size") == 0)) {
			gLFileSize = atof(tok2);
		} else if ((strcmp(tok1, "local-file-mtime") == 0) || (strcmp(tok1, "local-mtime") == 0)) {
			AtoTime_t(tok2, &gLFileMtime);
		} else if (strcmp(tok1, "remote-file-size") == 0) {
			gRFileSize = atof(tok2);
		} else if (strcmp(tok1, "remote-file-mtime") == 0) {
			AtoTime_t(tok2, &gRFileMtime);
		} else if (strcmp(tok1, "source-address") == 0) {
			(void) STRNCPY(gSourceAddrStr, tok2);
		} else if (strcmp(tok1, "manual-override-features") == 0) {
			(void) STRNCPY(gManualOverrideFeatures, tok2);
		} else if (strcmp(tok1, "pre-ftp-command") == 0) {
			(void) STRNCPY(gPreFTPCommand, tok2);
			cp = gPreFTPCommand;
			lim = cp + sizeof(gPreFTPCommand) - 1;
			goto multi;
		} else if (strcmp(tok1, "per-file-ftp-command") == 0) {
			(void) STRNCPY(gPerFileFTPCommand, tok2);
			cp = gPerFileFTPCommand;
			lim = cp + sizeof(gPerFileFTPCommand) - 1;
			goto multi;
		} else if (strcmp(tok1, "post-ftp-command") == 0) {
			(void) STRNCPY(gPostFTPCommand, tok2);
			cp = gPostFTPCommand;
			lim = cp + sizeof(gPostFTPCommand) - 1;
			goto multi;
		} else if (strcmp(tok1, "pre-shell-command") == 0) {
			(void) STRNCPY(gPreShellCommand, tok2);
			cp = gPreShellCommand;
			lim = cp + sizeof(gPreShellCommand) - 1;
			goto multi;
		} else if (strcmp(tok1, "post-shell-command") == 0) {
			(void) STRNCPY(gPostShellCommand, tok2);
			cp = gPostShellCommand;
			lim = cp + sizeof(gPostShellCommand) - 1;
		multi:
			cp += strlen(cp) - 1;
			while ((*cp == '\\') && (cp < lim)) {
				*cp++ = '\n';
				if (fgets(cp, (int) (lim - cp), fp) == NULL) {
					*cp = '\0';
					break;
				}
				cp += strlen(cp) - 1;
				if (*cp == '\n')
					*cp-- = '\0';
				if (*cp == '\r')
					*cp-- = '\0';
			}
		} else if (strcmp(tok1, "job-name") == 0) {
			/* ignore */
		} else {
			/* else, unknown option which is OK. */
			if (logErrors != 0) {
				Log(0, "Ignoring unknown parameter \"%s\" in %s.\n", tok1, gMyItemPath);
			}
		}
	}
	if (savefp == NULL) {
		(void) fclose(fp);
		fp = NULL;
	}

	if (islower(gOperation))
		gOperation = toupper(gOperation);

	if (gHost[0] == '\0') {
		if (gHostIP[0] != '\0') {
			(void) STRNCPY(gHost, gHostIP);
		} else {
			if (logErrors != 0)
				Log(0, "Spool file %s missing required parameter: %s.\n", gMyItemPath, "host");
			return (-1);
		}
	}

	if (gOperation == 'G') {
		if (gRecursive != 0) {
			if (gRFile[0] == '\0') {
				if (logErrors != 0)
					Log(0, "Spool file %s missing required parameter: %s.\n", gMyItemPath, "remote-file");
				return (-1);
			}
			if (gLDir[0] == '\0') {
				if (logErrors != 0)
					Log(0, "Spool file %s missing required parameter: %s.\n", gMyItemPath, "local-dir");
				return (-1);
			}
		} else {
			if (gRFile[0] == '\0') {
				if (logErrors != 0)
					Log(0, "Spool file %s missing required parameter: %s.\n", gMyItemPath, "remote-file");
				return (-1);
			}
			if (gLFile[0] == '\0') {
				if (logErrors != 0)
					Log(0, "Spool file %s missing required parameter: %s.\n", gMyItemPath, "local-file");
				return (-1);
			}
		}
	} else if (gOperation == 'P') {
		if (gRecursive != 0) {
			if (gLFile[0] == '\0') {
				if (logErrors != 0)
					Log(0, "Spool file %s missing required parameter: %s.\n", gMyItemPath, "local-file");
				return (-1);
			}
			if (gRDir[0] == '\0') {
				if (logErrors != 0)
					Log(0, "Spool file %s missing required parameter: %s.\n", gMyItemPath, "remote-dir");
				return (-1);
			}
		} else {
			if (gLFile[0] == '\0') {
				if (logErrors != 0)
					Log(0, "Spool file %s missing required parameter: %s.\n", gMyItemPath, "local-file");
				return (-1);
			}
			if (gRFile[0] == '\0') {
				if (logErrors != 0)
					Log(0, "Spool file %s missing required parameter: %s.\n", gMyItemPath, "remote-file");
				return (-1);
			}
		}
	} else {
		if (logErrors != 0)
			Log(0, "Invalid spool file operation: %c.\n", gOperation);
		return (-1);
	}

	if (gRUser[0] == '\0')
		(void) STRNCPY(gRUser, "anonymous");
	if ((gRPass[0] != '\0') && (strcmp(gRUser, "anonymous") == 0))
		(void) STRNCPY(gLib.defaultAnonPassword, gRPass);

	return (0);
}	/* LoadCurrentSpoolFileContents */




static int
RunShellCommandWithSpoolItemData(const char *const cmd, const char *const addstr)
{
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
	return (-1);
#else	/* UNIX */
	int pfd[2];
	char *argv[8];
	pid_t pid = 0;
	int oerrno;

	if ((cmd == NULL) || (cmd[0] == '\0'))
		return (-1);

	if (access(cmd, X_OK) < 0) {
		LogPerror("Cannot run program \"%s\"", cmd);
		return (-1);
	}

	if (pipe(pfd) < 0) {
		LogPerror("pipe");
	}

	pid = fork();
	if (pid < 0) {
		(void) close(pfd[0]);
		(void) close(pfd[1]);
		LogPerror("fork");
	} else if (pid == 0) {
		(void) close(pfd[1]);	/* Child closes write end. */
		if (pfd[0] != 0) {
			(void) dup2(pfd[0], 0);
			(void) close(pfd[0]);
		}
		argv[0] = strdup(cmd);
		argv[1] = NULL;

		/* Avoid sharing other resources with the shell
		 * command, such as our FTP session descriptors.
		 */
		FTPCloseControlConnection(&gConn);
		gMyPID = (unsigned int) getpid();
		CloseLog();

		(void) execv(cmd, argv);
		oerrno = errno;
		(void) OpenLog();
		errno = oerrno;
		LogPerror("Could not run program \"%s\"", cmd);
		exit(1);
	}
	(void) close(pfd[0]);	/* Parent closes read end. */
	if ((gItemContents != NULL) && (gItemContents[0] != '\0') && (gItemContentsSize > 0))
		(void) PWrite(pfd[1], (const char *) gItemContents, gItemContentsSize);
	if ((addstr != NULL) && (addstr[0] != '\0'))
		(void) PWrite(pfd[1], (const char *) addstr, strlen(addstr));
	(void) close(pfd[1]);	/* Parent closes write end. */

	if (pid > 1) {
#ifdef HAVE_WAITPID
		(void) waitpid(pid, NULL, 0);
#else
		(void) wait(NULL);
#endif	/* HAVE_WAITPID */
	}
	return (0);
#endif	/* UNIX */
}	/* RunShellCommandWithSpoolItemData */




/*VARARGS*/
static void
LogEndItemResult(int uiShow, int errorOccurred, const char *const fmt, ...)
{
	va_list ap;
	struct tm lt;
	FILE *xferLogFp;
	char *jobName, *cp;
	char buf[2048-128], tstr[128];

	time(&gLogTime);
	memset(tstr, 0, sizeof(tstr));
	memset(buf, 0, sizeof(buf));
	(void) strcpy(buf, "\nresult=");

	va_start(ap, fmt);
#ifdef HAVE_VSNPRINTF
	(void) vsnprintf(buf + 8, sizeof(buf) - 8, fmt, ap);
#else
	(void) vsprintf(buf + 8, fmt, ap);
#endif
	va_end(ap);

	Log(uiShow, "%s", buf + 8);
	(void) RunShellCommandWithSpoolItemData(gPostShellCommand, buf);

	if (gXferLogFileName[0] != '\0') {
		xferLogFp = fopen(gXferLogFileName, FOPEN_APPEND_TEXT);
		if (xferLogFp != NULL) {
			(void) strftime(tstr, sizeof(tstr) - 1, "%Y-%m-%d %H:%M:%S %Z", Localtime(gLogTime, &lt));
			jobName = StrRFindLocalPathDelim(gItemPath);
			if (jobName == NULL) {
				jobName = gItemPath;
			} else {
				jobName++;
			}
			cp = strrchr(jobName, '.');
			if (cp != NULL) {
				/* Don't need .txt extension */
				*cp = '\0';
			}
			fprintf(xferLogFp, "%s|%c|%s|%s|%s|%s|" PRINTF_LONG_LONG "|%.2f|%.3f|%s|%s|%s|%s|%c|%s|" PRINTF_LONG_LONG "|" PRINTF_LONG_LONG "|" PRINTF_LONG_LONG "|%s|%d|%d|%s",
				tstr,
				gOperation,
				gLDir, gLFile,
				gRDir, gRFile,
				gConn.bytesTransferred,
				gConn.sec,
				(gConn.kBytesPerSec >= 0) ? (gConn.kBytesPerSec * 1024.0 * 8.0 / 1000000.0) /* Mbps */ : -1.0,
				gConn.user,
				"",	/* Reserved for email or <gasp> password */
				gConn.host,
				"",	/* Reserved for filter type */
				gConn.curTransferType,
				(gConn.dataPortMode == kSendPortMode ? "Po" : "Ps"),
				(longest_int) gConn.t0.tv_sec,
				gConn.expectedSize,
				gConn.startPoint,
				jobName,
				(errorOccurred) ? gConn.errNo : 0,
				(errorOccurred) ? errno : 0,
				(errorOccurred) ? buf + 8 /* includes \n */ : "OK\n"
			);
			fclose(xferLogFp);
		}
	}
}	/* LogEndItemResult */



static void
PrLogStatBar(const FTPCIPtr cip, int mode)
{
	double rate, done;
	int secLeft, minLeft;
	const char *rStr;
	static const char *uStr;
	static double uTotal, uMult;
	const char *stall;

	switch (mode) {
		case kPrInitMsg:
			if (gProgLog != NULL)
				fclose(gProgLog);
			gProgLog = fopen(gProgressLog, FOPEN_APPEND_TEXT);
			if (gProgLog == NULL)
				return;
			if (cip->expectedSize == kSizeUnknown) {
				cip->progress = PrSizeAndRateMeter;
				PrSizeAndRateMeter(cip, mode);
				return;
			}
			uTotal = FileSize((double) cip->expectedSize, &uStr, &uMult);
			(void) fprintf(gProgLog, "%s:  ", (cip->lname == NULL) ? "" : cip->lname);
			(void) fflush(gProgLog);
			break;

		case kPrUpdateMsg:
			if (gProgLog == NULL)
				return;
			secLeft = (int) (cip->secLeft + 0.5);
			minLeft = secLeft / 60;
			secLeft = secLeft - (minLeft * 60);
			if (minLeft > 999) {
				minLeft = 999;
				secLeft = 59;
			}

			rate = FileSize(cip->kBytesPerSec * 1024.0, &rStr, NULL);
			done = (double) (cip->bytesTransferred + cip->startPoint) / uMult;

			if (cip->stalled < 2)
				stall = " ";
			else if (cip->stalled < 15)
				stall = "-";
			else
				stall = "=";

			/* Print the updated information. */
			(void) fprintf(gProgLog,
				"\r%s:  ETA: %3d:%02d  %6.2f/%6.2f %-2.2s  %6.2f %.2s/s %.1s",
				(cip->lname == NULL) ? "" : cip->lname,
				minLeft,
				secLeft,
				done,
				uTotal,
				uStr,
				rate,
				rStr,
				stall
			);
			(void) fflush(gProgLog);
			break;

		case kPrEndMsg:
			if (gProgLog == NULL)
				return;

			rate = FileSize(cip->kBytesPerSec * 1024.0, &rStr, NULL);
			done = (double) (cip->bytesTransferred + cip->startPoint) / uMult;

			if (cip->expectedSize >= (cip->bytesTransferred + cip->startPoint)) {
				(void) fprintf(gProgLog,
					"\r%s:  %6.2f %-2.2s  %6.2f %.2s/s %22s\n",
					(cip->lname == NULL) ? "" : cip->lname,
					uTotal,
					uStr,
					rate,
					rStr,
					""	/* space padding */
				);
			} else {
				(void) fprintf(gProgLog,
					"\r%s:  %6.2f/%6.2f %-2.2s  %6.2f %.2s/s %22s\n",
					(cip->lname == NULL) ? "" : cip->lname,
					done,
					uTotal,
					uStr,
					rate,
					rStr,
					""	/* space padding */
				);
			}
			(void) fflush(gProgLog);	/* redundant */
			(void) fclose(gProgLog);
			gProgLog = NULL;
			break;
	}
}	/* PrLogStatBar */




static int
RRename(void)
{
	char newPath[1024];
	int result;

	if ((gRNewDir[0] == '\0') && (gRNewFile[0] == '\0'))
		return (kNoErr);	/* Nothing to do. */

	if (TVFSPathBuild(newPath, sizeof(newPath), NULL, gRNewDir, gRNewFile) < 0)
		return (-1);
	if (FTPRename(&gConn, gRFile, newPath) == kNoErr) {
		Log(0, "Renamed remote file to %s.\n", newPath);
		return (kNoErr);
	}

	/* Rename failed, but we'll try to create the directory and try again. */
	if (gRNewDir[0] != '\0') {
		if (strcmp(gRNewDir, gRDir) != 0) {
			/* Specified a different directory.
			 * Make sure it exists.
			 * A relative path may be present.
			 */
			if (FTPMkdir2(&gConn, gRNewDir, kRecursiveYes, NULL) < 0) {
				if (gConn.errNo == kErrCannotGoToPrevDir)
					gNeedOpen = 1;
				Log(0, "Could not rename remote file because its directory %s could not be created.\n", gRNewDir);
				return (-1);
			}
		}
	}

	if ((result = FTPRename(&gConn, gRFile, newPath)) == kNoErr) {
		Log(0, "Renamed remote file to %s.\n", newPath);
		return (kNoErr);
	}
	Log(0, "Could not rename remote file to %s: %s.\n", newPath, FTPStrError(result));
	return (-1);
}	/* RRename */




static int
LRename(void)
{
	char newPath[1024];

	if ((gLNewDir[0] == '\0') && (gLNewFile[0] == '\0'))
		return (kNoErr);	/* Nothing to do. */

	if (gLNewDir[0] != '\0') {
		if (strcmp(gLNewDir, gLDir) != 0) {
			/* Specified a different directory.
			 * Make sure it exists.
			 * A relative path may be present.
			 */
			if (MkDirs(gLNewDir, 00777) < 0) {
				Log(0, "Could not rename local file because its directory %s could not be created: %s.\n", gLNewDir, strerror(errno));
				return (-1);
			}
		}
	}

	if (LocalPathBuild(newPath, sizeof(newPath), NULL, gLNewDir, gLNewFile) < 0)
		return (-1);
	if (rename(gLFile, newPath) != 0) {
		Log(0, "Could not rename local file to %s: %s.\n", newPath, strerror(errno));
		return (-1);
	}

	Log(0, "Renamed local file to %s.\n", newPath);
	return (kNoErr);
}	/* LRename */




static int
DoItem(void)
{
	char pwdec[256];
	char resolvedIPstr[64];
	int needOpen;
	int result;
	int cdflags;

	if (LoadCurrentSpoolFileContents(1) < 0)
		return (0);	/* remove invalid spool file */

	if (RunShellCommandWithSpoolItemData(gPreShellCommand, NULL) == 0) {
		/* Ran the pre-command, now reload the spool file in
		 * case they modified it.
		 */
		if (LoadCurrentSpoolFileContents(1) < 0)
			return (0);	/* remove invalid spool file */
	}

	cdflags = kChdirFullPath|kChdirOneSubdirAtATime;
	if (gOperation == 'P')
		cdflags = kChdirFullPath|kChdirOneSubdirAtATime|kChdirAndMkdir;

	if (gLDir[0] != '\0') {
		if (MkDirs(gLDir, 00777) < 0) {
#ifdef HAVE_STRERROR
			LogEndItemResult(1, 1, "Could not mkdir local-dir=%s: %s\n", gLDir, strerror(errno));
#else
			LogEndItemResult(1, 1, "Could not mkdir local-dir=%s: errno %d\n", gLDir, (errno));
#endif
			return (0);	/* remove spool file */
		} else if (chdir(gLDir) < 0) {
#ifdef HAVE_STRERROR
			LogEndItemResult(1, 1, "Could not cd to local-dir=%s: %s\n", gLDir, strerror(errno));
#else
			LogEndItemResult(1, 1, "Could not cd to local-dir=%s: errno %d\n", gLDir, (errno));
#endif
			return (0);	/* remove spool file */
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
#else
		} else if ((gOperation == 'G') && (access(gLDir, W_OK) < 0)) {
#ifdef HAVE_STRERROR
			LogEndItemResult(1, 1, "Could not write to local-dir=%s: %s\n", gLDir, strerror(errno));
#else
			LogEndItemResult(1, 1, "Could not write to local-dir=%s: errno %d\n", gLDir, (errno));
#endif
			return (0);	/* remove spool file */
#endif
		}
	}

	if (gRUser[0] == '\0')
		(void) STRNCPY(gRUser, "anonymous");

	/* Decode password, if it was base-64 encoded. */
	if (strncmp(gRPass, kPasswordMagic, kPasswordMagicLen) == 0) {
		FromBase64(pwdec, gRPass + kPasswordMagicLen, strlen(gRPass + kPasswordMagicLen), 1);
		(void) STRNCPY(gRPass, pwdec);
	}

	/* Now see if we need to open a new host.  We try to leave the
	 * host connected, so if they batch multiple files using the
	 * same remote host we don't need to re-open the remote host.
	 */
	needOpen = 0;
	if (gConn.connected == 0) {
		/* Not connected at all. */
		Log(0, "Was not connected originally.\n");
		needOpen = 1;
	} else if (ISTRCMP(gHost, gConn.host) != 0) {
		/* Host is different. */
		needOpen = 1;
		Log(0, "New host (%s), old host was (%s).\n", gHost, gConn.host);
	} else if (strcmp(gRUser, gConn.user) != 0) {
		/* Same host, but new user. */
		needOpen = 1;
		Log(0, "New user (%s), old user was (%s).\n", gRUser, gConn.user);
	} else if (gNeedOpen != 0) {
		needOpen = gNeedOpen;
		gNeedOpen = 0;
		if ((gConn.startingWorkingDirectory == NULL) || (gConn.startingWorkingDirectory[0] == '\0')) {
			if (FTPGetCWD(&gConn, gRStartDir, sizeof(gRStartDir)) < 0) {
				(void) FTPCloseHost(&gConn);
				needOpen = 1;
			}
		} else {
			(void) STRNCPY(gRStartDir, gConn.startingWorkingDirectory);
		}
	}

	if (needOpen != 0) {
		(void) AdditionalCmd(&gConn, gPostFTPCommand, NULL);
		(void) FTPCloseHost(&gConn);
		InitHostVariables();
		if (FTPInitConnectionInfo(&gLib, &gConn, kDefaultFTPBufSize) < 0) {
			/* Highly unlikely... */
			LogEndItemResult(1, 1, "init connection info failed!\n");
			ExitStuff();
			DisposeWinsock();
			exit(1);
		}

		gConn.debugLogProc = DebugHook;
		gConn.debugLog = NULL;
		gConn.errLogProc = NULL;
		gConn.errLog = NULL;
		(void) STRNCPY(gConn.host, gHost);
		gConn.port = gPort;
		(void) STRNCPY(gConn.user, gRUser);
		(void) STRNCPY(gConn.pass, gRPass);
		if ((gConn.pass[0] == '\0') && (strcmp(gConn.user, "anonymous")) && (strcmp(gConn.user, "ftp")) && (gConn.user[0] != '\0'))
			gConn.passIsEmpty = 1;
		(void) STRNCPY(gConn.acct, gRAcct);
		gConn.maxDials = 1;
		gConn.dataPortMode = gPassive;
		gConn.useSendfile = kNcFTPBatchDefaultUseSendfile;
		gConn.manualOverrideFeatures = gManualOverrideFeatures;
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
		gConn.progress = PrWinStatBar;
#endif
		if (gProgressLog[0] != '\0')
			gConn.progress = PrLogStatBar;

		if (MayUseFirewall(gConn.host, gFirewallType, gFirewallExceptionList) != 0) {
			gConn.firewallType = gFirewallType; 
			(void) STRNCPY(gConn.firewallHost, gFirewallHost);
			(void) STRNCPY(gConn.firewallUser, gFirewallUser);
			(void) STRNCPY(gConn.firewallPass, gFirewallPass);
			gConn.firewallPort = gFirewallPort;
		}

		if (gSourceAddrStr[0] != '\0')
			(void) AddrStrToAddr(gSourceAddrStr, &gConn.preferredLocalAddr, 21);
		
		gConn.connTimeout = 30;
		gConn.ctrlTimeout = 135;
		gConn.xferTimeout = 300;
		if (ISTRCMP(gHost, gLastHost) == 0) {
			if ((time(NULL) - gTimeOfLastFailureAny) < kMinimumReconnectDelaySameHost) {
				/* Same host, but last "recent" attempt to connect failed. */
				Log(1, "Skipping same failed host as recent attempt (%s).\n", gLastHost);
				return (-1);	/* Try again next time. */
			}
		}
		if (AddrStrToIPStr(resolvedIPstr, sizeof(resolvedIPstr), gHost, (int) gPort) == NULL) {
			LogEndItemResult(1, 1, "Couldn't resolve IP for %s, will try again next time.\n", gHost);
			(void) FTPCloseHost(&gConn);
			(void) STRNCPY(gLastHost, gHost);	/* save failed connection to gHost. */
			InitHostVariables();
			return (-1);	/* Try again next time. */
		}
		Log(1, "Opening %s:%u (%s) as user %s...\n", gHost, gPort, resolvedIPstr, gRUser);
		result = FTPOpenHost(&gConn);
		if (result < 0) {
			LogEndItemResult(1, 1, "Couldn't open %s, will try again next time.\n", gHost);
			(void) FTPCloseHost(&gConn);
			(void) STRNCPY(gLastHost, gHost);	/* save failed connection to gHost. */
			InitHostVariables();
			return (-1);	/* Try again next time. */
		}
		gLastHost[0] = '\0';	/* have connected - "clear" gLastHost. */
		if (FTPGetCWD(&gConn, gRStartDir, sizeof(gRStartDir)) < 0) {
			LogEndItemResult(1, 1, "Couldn't get start directory on %s, will try again next time.\n", gHost);
			(void) AdditionalCmd(&gConn, gPostFTPCommand, NULL);
			(void) FTPCloseHost(&gConn);
			InitHostVariables();
			return (-1);	/* Try again next time. */
		}
		if (gConn.hasCLNT != kCommandNotAvailable) {
			(void) FTPCmd(&gConn, "CLNT %s %.5s %s",
				(gGlobalSpooler != 0) ? "NcFTPSpooler" : "NcFTPBatch",
				gVersion + 11,
				gOS
			);
		}
		(void) AdditionalCmd(&gConn, gPreFTPCommand, NULL);

		if (FTPChdir3(&gConn, gRDir, NULL, 0, cdflags) < 0) {
			LogEndItemResult(1, 1, "Could not remote cd to %s.\n", gRDir);

			/* Leave open, but unspool.
			 *
			 * Odds are that the directory no longer exists,
			 * so it would be pointless to retry.
			 */
			return (0);
		}
	} else {
		/* Same host, but go back to root.
		 * The remote directory path is relative
		 * to root, so go back to it.
		 */
		if ((gRDir[0] == '\0') || (gRPrevDir[0] == '\0') || (strcmp(gRPrevDir, gRDir) != 0)) {
			/* This item's remote directory was different from the previous one. */

			if ((gRDir[0] == '\0') && (gRStartDir[0] != '\0')) {
				(void) STRNCPY(gRDir, gRStartDir);
			}

			if (gRDir[0] != '/') {
				if (gRStartDir[0] == '\0') {
					Log(1, "Hmmm, I forgot the original starting directory...\n");
					return (-1);	/* Try again next time, in case conn dropped. */
				} else if (FTPChdir(&gConn, gRStartDir) < 0) {
					LogEndItemResult(1, 1, "Could not remote cd back to %s.\n", gRStartDir);
					return (-1);	/* Try again next time, in case conn dropped. */
				}
			}

			if (FTPChdir3(&gConn, gRDir, NULL, 0, cdflags) < 0) {
				LogEndItemResult(1, 1, "Could not remote cd to %s.\n", gRDir);
				return (-1);	/* Try again next time, in case conn dropped. */
			}
		}
	}

	/* We have now changed to the directory. Save this value to be checked by the next item. */
	STRNCPY(gRPrevDir, gRDir);

	if (gTimeOfFirstAttempt == 0)
		(void) time(&gTimeOfFirstAttempt);

	if (gOperation == 'G') {
		if (gRecursive != 0) {
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
			sprintf(gStatusText, "Downloading %.200s", gRFile);
#endif
			result = FTPGetFiles3(&gConn, gRFile, gLDir, gRecursive, kGlobNo, gXtype, kResumeYes, kAppendNo, gDelete, kTarNo, kNoFTPConfirmResumeDownloadProc, 0);
		} else {
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
			sprintf(gStatusText, "[0%%] - Downloading %.200s", gRFile);
#endif
			result = FTPGetOneFile3(&gConn, gRFile, gLFile, gXtype, (-1), kResumeYes, kAppendNo, gDelete, kNoFTPConfirmResumeDownloadProc, 0);
		}
		if (result == kErrCouldNotStartDataTransfer) {
			LogEndItemResult(1, 1, "Remote item %s is no longer retrievable.\n", gRFile);
			result = 0;	/* file no longer on host */
		} else if ((result == kErrRemoteSameAsLocal) || (result == kErrLocalSameAsRemote)) {
			LogEndItemResult(1, 1, "Remote item %s is already present locally.\n", gRFile);
			result = 0;
		} else if (result == kErrLocalFileNewer) {
			LogEndItemResult(1, 1, "Remote item %s is already present on remote host and is newer.\n", gRFile);
			result = 0;
		} else if (result == kNoErr) {
			if (LRename() < 0) {
				LogEndItemResult(1, 1, "Succeeded downloading to %s, but a post-processing error occurred: %s\n", gLFile, strerror(errno));
			} else if (RRename() < 0) {
				LogEndItemResult(1, 1, "Succeeded downloading %s, but a post-processing error occurred: %s\n", gRFile, strerror(errno));
			} else {
				(void) AdditionalCmd(&gConn, gPerFileFTPCommand, gRFile);
				if (gConn.kBytesPerSec >= 0) {
					LogEndItemResult(1, 0, "Succeeded downloading %s (%.0f bytes, %.1f sec, %.2f Mbps).\n", gRFile, (double) gConn.bytesTransferred, gConn.sec, gConn.kBytesPerSec * 1024.0 * 8.0 / 1000000.0);
				} else {
					LogEndItemResult(1, 0, "Succeeded downloading %s.\n", gRFile);
				}
			}
		} else if (gConn.bytesTransferred >= 1) {
			LogEndItemResult(1, 1, "Error (%d) occurred on %s after %.0f bytes transferred: %s\n", result, gRFile, (double) gConn.bytesTransferred, FTPStrError(result));
		} else {
			LogEndItemResult(1, 1, "Error (%d) occurred on %s: %s\n", result, gRFile, FTPStrError(result));
		}
	} else /* if (gOperation == 'P') */ {
		if (gRecursive != 0) {
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
			sprintf(gStatusText, "Uploading %.200s", gLFile);
#endif
			result = FTPPutFiles3(&gConn, gLFile, gRDir, gRecursive, kGlobNo, gXtype, kAppendNo, NULL, NULL, kResumeYes, gDelete, kNoFTPConfirmResumeUploadProc, 0);
		} else {
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
			sprintf(gStatusText, "[0%%] - Uploading %.200s", gLFile);
#endif
			result = FTPPutOneFile4(&gConn, gLFile, gRFile, gXtype, (-1), kAppendNo, NULL, NULL, kResumeYes, gDelete, kNoFTPConfirmResumeUploadProc, gTimeOfFirstAttempt, gLFileMtime);
		}
		if (result == kErrCouldNotStartDataTransfer) {
			LogEndItemResult(1, 1, "Remote item %s is no longer sendable.  Perhaps permission denied on destination?\n", gRFile);
			result = 0;	/* file no longer on host */
		} else if ((result == kErrRemoteSameAsLocal) || (result == kErrLocalSameAsRemote)) {
			LogEndItemResult(1, 1, "Local item %s is already present on remote host.\n", gLFile);
			result = 0;
		} else if (result == kErrRemoteFileNewer) {
			LogEndItemResult(1, 1, "Local item %s is already present on remote host and is newer.\n", gLFile);
			result = 0;
		} else if (result == kNoErr) {
			if (LRename() < 0) {
				LogEndItemResult(1, 1, "Succeeded uploading %s, but a post-processing error occurred: %s\n", gLFile, strerror(errno));
			} else if (RRename() < 0) {
				LogEndItemResult(1, 1, "Succeeded uploading to %s, but a post-processing error occurred: %s\n", gRFile, strerror(errno));
			} else {
				(void) AdditionalCmd(&gConn, gPerFileFTPCommand, gLFile);
				if (gConn.kBytesPerSec >= 0) {
					LogEndItemResult(1, 0, "Succeeded uploading %s (%.0f bytes, %.1f sec, %.2f Mbps).\n", gLFile, (double) gConn.bytesTransferred, gConn.sec, gConn.kBytesPerSec * 1024.0 * 8.0 / 1000000.0);
				} else {
					LogEndItemResult(1, 0, "Succeeded uploading %s.\n", gLFile);
				}
			}
		} else if (gConn.bytesTransferred >= 1) {
			LogEndItemResult(1, 1, "Error (%d) occurred on %s after %.0f bytes transferred: %s\n", result, gLFile, (double) gConn.bytesTransferred, FTPStrError(result));
		} else {
			LogEndItemResult(1, 1, "Error (%d) occurred on %s: %s\n", result, gLFile, FTPStrError(result));
		}
	}
	
	if (result != kNoErr) {
		gTimeOfLastFailureAny = time(&gTimeOfLastFailure);
	}

	switch (result) {
		case kErrSYMLINKFailed:
		case kErrSYMLINKNotAvailable:
		case kErrLocalDeleteFailed:
		case kErrDELEFailed:
		case kErrMKDFailed:
		case kErrCWDFailed:
		case kErrRMDFailed:
		case kErrRenameFailed:
		case kErrOpenFailed:
			/* We logged the error, but do not attempt to
			 * retry this spool entry.
			 */
			result = 0;
			break;
	}

	/* Return (result < 0) if the item should be re-spooled,
	 * Return (result == 0) if the spool item is finished and should be deleted from the queue.
	 */
	return (result);
}	/* DoItem */




static int
DecodeName(const char *const src, int *yyyymmdd, int *hhmmss, int *jobType)
{
	char itemName[64];
	char *tok, *ps;
	int t;
	int valid = -1;

	/* Format is:
	 *
	 * X-YYYYMMDD-hhmmss
	 *
	 * Where X is the entry type (G or P, for Get or Put)
	 *       YYYYMMMDD is the 4-digit year, month, month day
	 *       hhmmss is the hour minute second
	 *
	 * The names are also allowed to have additional fields
	 * appended after the hhmmss field, to make it easier
	 * to create unique spool file names.  For example, NcFTP Client
	 * uses X-YYYYMMDD-hhmmss-PPPPPPPPPP-JJJJ, with the PID
	 * and job sequence number appended.
	 */
	(void) STRNCPY(itemName, src);
	for (t = 0, ps = itemName; ((tok = strtok(ps, "-")) != NULL); ps = NULL) {
		t++;
		switch (t) {
			case 1:
				/* Verify that entry is a G or P */
				if (strchr("GgPpXx", (int) tok[0]) == NULL)
					goto fail;
				if (jobType != NULL) {
					*jobType = ((int) tok[0]);
					if (islower(*jobType))
						*jobType = toupper(*jobType);
				}
				break;
			case 2:
				/* Quick sanity check */
				if (isdigit((int) tok[0]) == 0)
					goto fail;
				if (yyyymmdd != NULL) {
					*yyyymmdd = atoi(tok);
				}
				break;
			case 3:
				if (isdigit((int) tok[0]) == 0)
					goto fail;
				if (hhmmss != NULL) {
					*hhmmss = atoi(tok);
				}
				valid = 0;
				break;
		}
	}
	if (valid < 0) {
fail:
		if (jobType != NULL) {
			*jobType = 0;
		}
		if (yyyymmdd != NULL) {
			*yyyymmdd = 0;
		}
		if (hhmmss != NULL) {
			*hhmmss = 0;
		}
		return (-1);
	}
	return (valid);
}	/* DecodeName */




static void
Now(int *yyyymmdd, int *hhmmss)
{
	struct tm lt;

	if (Gmtime(0, &lt) == NULL) {
		*yyyymmdd = 0;
		*hhmmss = 0;
	} else {
		*yyyymmdd = ((lt.tm_year + 1900) * 10000)
			+ ((lt.tm_mon + 1) * 100)
			+ (lt.tm_mday);
		*hhmmss = (lt.tm_hour * 10000)
			+ (lt.tm_min * 100)
			+ (lt.tm_sec);
	}
}	/* Now */




#ifdef F_SETLK
static int
LockFile(const int fd, const int lock)
{
#ifdef F_SETLK
	struct flock l;
	int lk = F_SETLK;

	memset(&l, 0, sizeof(l));
	if (lock == 'r') {
		l.l_type = F_RDLCK;
	} else if (lock == 'R') {
		l.l_type = F_RDLCK;
		lk = F_SETLKW;
	} else if (lock == 'w') {
		l.l_type = F_WRLCK;
	} else if (lock == 'W') {
		l.l_type = F_WRLCK;
		lk = F_SETLKW;
	} else {
		l.l_type = F_UNLCK;
	}
	/* l.l_start = (off_t) 0; */
	l.l_whence = SEEK_SET;
	/* l.l_len = (off_t) 0;	everything == 0 */

	return (fcntl(fd, lk, &l));
#else
	return (0);
#endif
}	/* LockFile */
#endif




static void
EventShell(volatile unsigned int sleepval)
{
	volatile int nItems;
	int nProcessed, nFinished;
	unsigned int minDSLF;
	struct dirent *dent;
	struct Stat st;
	char *cp;
	int renamerc = 0;
	int iType;
	int iyyyymmdd, ihhmmss, nyyyymmdd, nhhmmss;
	DIR *volatile DIRp;
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
	int passes;
#else
	int sj;
	volatile int passes;
#endif

	DIRp = NULL;
	dent = gDirentBuf;
	(void) OpenLog();
	Log(0, "-----started-----\n");

#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
#else
#ifdef HAVE_SIGSETJMP
	sj = sigsetjmp(gCancelJmp, 1);
#else	/* HAVE_SIGSETJMP */
	sj = setjmp(gCancelJmp);
#endif	/* HAVE_SIGSETJMP */

	if (sj != 0) {
		gMayCancelJmp = 0;
		if (DIRp != NULL) {
			(void) closedir(DIRp);
			DIRp = NULL;
		}
		FTPShutdownHost(&gConn);
		Log(0, "Timed-out, starting over.\n");
	}
	gMayCancelJmp = 1;
#endif

	passes = 0;
	nItems = 0;
	nProcessed = 0;
	nFinished = 0;
	minDSLF = 0;

	for ( ; ; ) {
		passes++;
		if ((passes > 1) || ((passes == 1) && (sleepval != 0))) {
			/* Don't wait between passes if we just
			 * processed an item.  We only wait between
			 * passes if we didn't do anything on the
			 * previous pass.
			 */
			if ((nProcessed == 0) || (nItems == nFinished)) {
				if (minDSLF != 0) {
					sleepval = minDSLF + 1;
					if ((gDelayBetweenPasses != 0) && (gDelayBetweenPasses < minDSLF))
						sleepval = gDelayBetweenPasses;
					minDSLF = 0;
				} else if (gDelayBetweenPasses != 0) {
					sleepval = gDelayBetweenPasses;
				} else if (sleepval == 0) {
					sleepval = 3;
				} else if (sleepval > 900) {
					/* If sleep duration got so large it got past 15 minutes,
					 * start over again.
					 */
					sleepval = 60;
				} else {
					sleepval = NEW_SLEEP_VAL(sleepval);
				}

				(void) FTPCloseHost(&gConn);
				InitHostVariables();
				Log(0, "Sleeping %u seconds before starting pass %d.\n", sleepval, passes);
				if ((sleepval == 0) || (sleepval > 30000)) {
					Log(0, "Panic: invalid sleep amount %u.\n", sleepval);
					exit(1);
				}
				(void) sleep(sleepval);
			}
			YieldUI(1);

			/* Re-open it, in case they deleted the log
			 * while this process was running.  This also
			 * gives us an opportunity to create a new log
			 * if the existing log grew too large.
			 */
			(void) OpenLog();
		}

		if ((DIRp = opendir(gSpoolDir)) == NULL) {
			/* Do not require the spool directory be present.
			 * Another process could create it later and
			 * then add jobs for us to process.
			 */
			Log(0, "Skipping pass %d; queue directory \"%s\" not accessible.\n", passes, gSpoolDir);
			sleep(60);
			continue;
		}

		gLastHost[0] = '\0';	/* clear [failed]LastHost before starting a pass */
		Log(0, "Starting pass %d.\n", passes);
		if (passes >= 1000000) {
			/* This "panic" and the one a few lines above
			 * are here temporarily; I think the bug this
			 * was checking for has been fixed finally.
			 */
			Log(0, "Panic: invalid pass number %d.\n", passes);
			exit(1);
		}

		for (nItems = 0, nProcessed = 0, nFinished = 0; ; ) {
			if (Readdir(DIRp, dent, gDirentBufSize) == NULL)
				break;

			YieldUI(0);

			(void) STRNCPY(gItemPath, gSpoolDir);
			(void) STRNCAT(gItemPath, LOCAL_PATH_DELIM_STR);
			(void) STRNCAT(gItemPath, dent->d_name);
			if ((Stat(gItemPath, &st) < 0) || (S_ISREG(st.st_mode) == 0)) {
				/* Item may have been
				 * deleted by another
				 * process.
				 */
				continue;
			}

			if (DecodeName(dent->d_name, &iyyyymmdd, &ihhmmss, &iType) < 0) {
				/* Junk file in the spool directory. */
				continue;
			}

			cp = StrRFindLocalPathDelim(gItemPath);
			if (cp == NULL) {
				/* Impossible */
				continue;
			}
			cp++;

			if ((iType != 'G') && (iType != 'P')) {
				/* We only handle available GET and PUT jobs (unavailable are 'X'). */
				continue;
			}

			/* Count items waiting for processing. */
			nItems++;

			Now(&nyyyymmdd, &nhhmmss);
			if ((nyyyymmdd < iyyyymmdd) || ((nyyyymmdd == iyyyymmdd) && (nhhmmss < ihhmmss))) {
				/* Process only if the specified start
				 * time has passed.
				 */
				continue;
			}

			(void) STRNCPY(gMyItemPath, gItemPath);
			gMyItemPath[(int) (cp - gItemPath)] = 'x';

#ifdef F_SETLK
			errno = 0;
			if ((gLockFd >= 0) && (LockFile(gLockFd, 'W') < 0)) {
				Log(0, "Lock %s failed: %s\n", gLockFile, strerror(errno));
				/* quit now */
				return;
			}
#endif

			/* Race condition between other ncftpbatches,
			 * but only one of them will rename it
			 * successfully if rename() is atomic like it
			 * is supposed to be (cough, cough, Cygwin).
			 */
			renamerc = rename(gItemPath, gMyItemPath);

#ifdef F_SETLK
			errno = 0;
			if ((gLockFd >= 0) && (LockFile(gLockFd, 'u') < 0)) {
				Log(0, "Unlock %s failed: %s\n", gLockFile, strerror(errno));
				/* quit now */
				return;
			}
#endif

			if (renamerc == 0) {
				gItemInUse = 1;
				Log(0, "Processing path: %s\n", gItemPath);
				nProcessed++;

				for (;;) {
					if (DoItem() < 0) {
						Respool(1);
						gItemInUse = 0;
						if ((minDSLF == 0) || (minDSLF > gDelaySinceLastFailure))
							minDSLF = gDelaySinceLastFailure;
						if (gQuitRequested == 0)
							sleep(2);
						break;
					} else if (savefp != NULL) {
						/* More entries remain. */
						if (chdir(LOCAL_PATH_DELIM_STR) < 0)
							exit(1);
						if (gQuitRequested != 0) {
							Respool(1);
							gItemInUse = 0;
							break;
						}
						continue;
					} else {
						/* Finished with transaction file. */
						gItemInUse = 0;
						nFinished++;
						Log(0, "Done with %s.\n", gItemPath);
						if (DeleteFileWithRetries(gMyItemPath, 1) != 0) {
							/* quit now */
							Log(0, "Could not delete finished job %s!\n", gMyItemPath);
							return;
						}
						break;
					}
				}

				if (chdir(LOCAL_PATH_DELIM_STR) < 0)
					exit(1);
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
				/* Allow time for message to be seen */
				sleep(1);
#endif
			}	/* rename */

			if (gQuitRequested != 0) {
				if (DIRp != NULL) {
					(void) closedir(DIRp);
					DIRp = NULL;
				}
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
				Log(0, "User requested close.\n");
#else
				if (gQuitRequested == 1)
					Log(0, "User requested close.\n");
				else
					Log(0, "-----processing delayed signal %d, exiting-----\n", gQuitRequested);
#endif
				(void) AdditionalCmd(&gConn, gPostFTPCommand, NULL);
				(void) FTPCloseHost(&gConn);
				InitHostVariables();
				gMayCancelJmp = 0;
				Log(0, "-----done-----\n");
				return;
			}
		}
		if (DIRp != NULL) {
			(void) closedir(DIRp);
			DIRp = NULL;
		}
		if ((nItems == 0) && (nFinished == 0)) {
			Log(0, "The spool directory %s is now empty.\n", gSpoolDir);

			/* Spool directory is empty, done. */
			if (gGlobalSpooler == 0)
				break;
		}
	}
	(void) AdditionalCmd(&gConn, gPostFTPCommand, NULL);
	(void) FTPCloseHost(&gConn);
	InitHostVariables();
	gMayCancelJmp = 0;
	Log(0, "-----done-----\n");
}	/* EventShell */




#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
#else

static void
ListQueue(int reportMode, int activeOnly)
{
	int nItems;
	struct Stat st;
	struct dirent *dent;
	DIR *DIRp;
	char *cp;
	int iyyyymmdd, ihhmmss;
	char dstr[64], zstr[64], jfstr[64];
	char yyyy[8], mm[4], dd[4];
	char HH[4], MM[4], SS[4];
	struct tm lt;
	time_t t;
	int iType = 0, nP = 0, nG = 0, nJobFiles = 0;
	double tP = 0.0, tG = 0.0, tX;
	const char *uStr;
	int printedHeader = 0;
	int nJobsInThisJobFile;

	if ((DIRp = opendir(gSpoolDir)) == NULL) {
		PerrorBox(gSpoolDir);
		(void) fprintf(stderr, "This directory is created automatically the first time you do a background\noperation from NcFTP.\n");
		DisposeWinsock();
		exit(1);
	}

	dent = gDirentBuf;
	for (nItems = 0; ; ) {
		if (Readdir(DIRp, dent, gDirentBufSize) == NULL)
			break;
		
		(void) STRNCPY(gItemPath, gSpoolDir);
		(void) STRNCAT(gItemPath, LOCAL_PATH_DELIM_STR);
		(void) STRNCAT(gItemPath, dent->d_name);

		if (DecodeName(dent->d_name, &iyyyymmdd, &ihhmmss, &iType) < 0) {
			/* Junk file in the spool directory. */
			continue;
		}

		cp = StrRFindLocalPathDelim(gItemPath);
		if (cp == NULL) {
			/* Impossible */
			continue;
		}
		cp++;

		if (iType == 'P') { nP++; } else if (iType == 'G') { nG++; }

		nJobFiles++;
		nItems++;
		if (reportMode >= 4) {
			continue;
		}

		memset(dstr, 0, sizeof(dstr));
		(void) sprintf(dstr, "%08d%06d", iyyyymmdd, ihhmmss);
		(void) memcpy(yyyy, dstr, 4); yyyy[4] = '\0';
		(void) memcpy(mm, dstr + 4, 2); mm[2] = '\0';
		(void) memcpy(dd, dstr + 6, 2); dd[2] = '\0';
		(void) memcpy(HH, dstr + 8, 2); HH[2] = '\0';
		(void) memcpy(MM, dstr + 10, 2); MM[2] = '\0';
		(void) memcpy(MM, dstr + 12, 2); SS[2] = '\0';
		t = UnMDTMDate(dstr);

		if ((t == 0) || (t == (time_t) -1) || (Localtime(t, &lt) == NULL)) {
			memset(zstr, 0, sizeof(zstr));
		} else {
			memset(dstr, 0, sizeof(dstr));
			memset(zstr, 0, sizeof(zstr));
			(void) strftime(zstr, sizeof(zstr) - 1, "%Z", &lt);
			(void) strftime(dstr, sizeof(dstr) - 1, "%Y-%m-%d %H:%M:%S", &lt);
		}

		if (reportMode == 3) {
			/* Only report what can be found from the spool filename itself. */
			if (++printedHeader == 1) {
				(void) printf("---Scheduled For------------Spool File-----------------------------------------\n");
			}
			if ((activeOnly == 0) || (gItemPath[0] == 'x')) {
				if ((t == 0) || (t == (time_t) -1) || (Localtime(t, &lt) == NULL)) {
					(void) printf("%c  %s-%s-%s %s:%s:%s UTC  %s\n",
						(gItemPath[0] == 'x') ? '*' : ' ',
						yyyy, mm, dd, HH, MM, SS,
						gItemPath
					);
				} else {
					(void) printf("%c  %s %.3s  %s\n",
						(gItemPath[0] == 'x') ? '*' : ' ',
						dstr,
						zstr,
						gItemPath
					);
				}
			}
			continue;
		}

		/* reportMode == 1 or 2 */

		if ((Stat(gItemPath, &st) < 0) || (S_ISREG(st.st_mode) == 0)) {
			/* Item may have been
			 * deleted by another
			 * process.
			 */
			continue;
		}

		--nItems;	/* We haven't actually loaded the first item from the spool file yet. */
		if (iType == 'P') { nP--; } else if (iType == 'G') { nG--; }
		nJobsInThisJobFile = 0;

		(void) STRNCPY(gMyItemPath, gItemPath);
		do {
			if (LoadCurrentSpoolFileContents(0) < 0) {
				if (savefp != NULL) {
					fclose(savefp);
					savefp = NULL;
				}
				break;
			}

			nJobsInThisJobFile++;
			nItems++;
			iType = gOperation;
			if (iType == 'P') { nP++; } else if (iType == 'G') { nG++; }

			if ((iType != 'P') && (gRFileSize >= 0))
				tG += gRFileSize;
			else if ((iType == 'P') && (gLFileSize > 0))
				tP += gLFileSize;

			if (reportMode == 2)
				continue;

			if (++printedHeader == 1) {
				(void) printf("---Scheduled For------------Host------------------------------Command------------------------------\n");
			}
			if ((activeOnly == 0) || (gItemPath[0] == 'x')) {
				if ((t == 0) || (t == (time_t) -1) || (Localtime(t, &lt) == NULL)) {
					(void) printf("%c  %s-%s-%s %s:%s:%s UTC  %-32s  ",
						(gItemPath[0] == 'x') ? '*' : ' ',
						yyyy, mm, dd, HH, MM, SS,
						gHost
					);
				} else {
					(void) printf("%c  %s %.3s  %-32s  ",
						(gItemPath[0] == 'x') ? '*' : ' ',
						dstr,
						zstr,
						gHost
					);
				}
				if (iType != 'P') {
					(void) printf("%s", (nJobsInThisJobFile < 2) ? "GET" : "+ G");
					if (gRecursive != 0) {
						(void) printf(" -R \"%s\"", gRFile);
					} else {
						(void) printf(" \"%s\"", gRFile);
					}
				} else {
					(void) printf("%s", (nJobsInThisJobFile < 2) ? "PUT" : "+ P");
					if (gRecursive != 0) {
						(void) printf(" -R \"%s\"", gLFile);
					} else {
						(void) printf(" \"%s\"", gLFile);
					}
				}
				(void) printf("\n");
			}
		} while (savefp != NULL);
	}
	(void) closedir(DIRp);
	if (nItems == 0) {
		/* Spool directory is empty, done. */
		(void) printf("%s \"%s\" directory is empty.\n",
			(gGlobalSpooler != 0) ? "The" : "Your",
			gSpoolDir);
	} else {
		if ((reportMode == 1) || (reportMode == 3))
			printf("\n");

		jfstr[0] = '\0';
		if ((nG > 0) && (nP > 0)) {
		} else if (nItems > nJobFiles) {
#ifdef HAVE_SNPRINTF
			(void) snprintf(jfstr, sizeof(jfstr), " in %d spool file%s", nJobFiles, (nJobFiles == 1) ? "" : "s");
#else
			(void) sprintf(jfstr, " in %d spool file%s", nJobFiles, (nJobFiles == 1) ? "" : "s");
#endif
		}
		if (nG > 0) {
			if (tG > 0.0) {
				tX = FileSize(tG, &uStr, NULL);
				printf("%d GET job%s queued%s for %.1f %s.\n", nG, ((nG == 1) ? "" : "s"), jfstr, tX, uStr);
			} else {
				printf("%d GET job%s queued%s.\n", nG, ((nG == 1) ? "" : "s"), jfstr);
			}
		}
		if (nP > 0) {
			if (tP > 0.0) {
				tX = FileSize(tP, &uStr, NULL);
				printf("%d PUT job%s queued%s for %.1f %s.\n", nP, ((nP == 1) ? "" : "s"), jfstr, tX, uStr);
			} else {
				printf("%d PUT job%s queued%s.\n", nP, ((nP == 1) ? "" : "s"), jfstr);
			}
		}
		if ((nItems - (nG + nP)) > 0) {
			printf("%d job%s in progress.\n", (nItems - (nG + nP)), (((nItems - (nG + nP)) == 1) ? "" : "s"));
		}
		if (nItems > nJobFiles) {
#ifdef HAVE_SNPRINTF
			(void) snprintf(jfstr, sizeof(jfstr), " in %d spool file%s", nJobFiles, (nJobFiles == 1) ? "" : "s");
#else
			(void) sprintf(jfstr, " in %d spool file%s", nJobFiles, (nJobFiles == 1) ? "" : "s");
#endif
		}
		if ((nG > 0) && (nP > 0)) {
			printf("%d total job%s queued%s.\n", nItems, ((nItems == 1) ? "" : "s"), jfstr);
		}
	}
}	/* ListQueue */

#endif





#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)

static void OnDraw(HWND hwnd, HDC hdc)
{
	RECT clientRect, rect, r;
	char str[128];
	struct tm lt;
	BOOL sizeIsUnknown, inProgress;
	int secLeft, minLeft;
	double rate, per;
	const char *rStr;
	int oldBkMode;
	int iper;
	HBRUSH redBrush, bkgndBrush;
	TEXTMETRIC textMetrics;
	static int lineHeight = 0;
	static int lastUpdate = 0;
	COLORREF oldBkColor;
	static HFONT statusTextFont;
	LOGFONT lf;
	char *cp;

	strftime(str, sizeof(str), "%Y-%m-%d %H:%M:%S %Z", Localtime(0, &lt));

	sizeIsUnknown = (gConn.expectedSize == kSizeUnknown);
	inProgress = (gConn.bytesTransferred > 0);

	GetClientRect(hwnd, &clientRect);

	if (lineHeight == 0) {
		// First time through.
		//
		ZeroMemory(&lf, (DWORD) sizeof(lf));
		lf.lfHeight = -MulDiv(8, GetDeviceCaps(hdc, LOGPIXELSY), 72);
		strcpy(lf.lfFaceName, "MS Sans Serif");
		statusTextFont = CreateFontIndirect(&lf);
		if (statusTextFont != NULL)
			SendMessage(gStaticCtrl, WM_SETFONT, (WPARAM) statusTextFont, (LPARAM) 1);

		GetTextMetrics(hdc, &textMetrics);
		lineHeight = textMetrics.tmAscent + textMetrics.tmDescent + textMetrics.tmExternalLeading;

		GetWindowRect(gMainWnd, &r);
		r.bottom = r.top + 30 + lineHeight + lineHeight + lineHeight + 20 - 4;
		MoveWindow(gMainWnd, r.left, r.top, r.right - r.left, r.bottom - r.top, TRUE);
	}

	if (gConn.dataSocket < 0) {
		// Transfer not in progress, show the status text.
		//
		SetWindowText(gStaticCtrl, gStatusText);
		if (lastUpdate == 0) {
			ShowWindow(gStaticCtrl, SW_SHOW);
			SetWindowText(gMainWnd, "NcFTPBatch");
		}
		lastUpdate = 1;
	} else {
		if (lastUpdate == 1)
			ShowWindow(gStaticCtrl, SW_HIDE);
		lastUpdate = 0;

		rect.left = 10;
		rect.top = 10;
		rect.right = clientRect.right - 10;
		rect.bottom = rect.top + lineHeight + 10;
		
		if (!sizeIsUnknown) {
			FrameRect(hdc, &rect, GetStockObject(BLACK_BRUSH));
			
			r.left = rect.left + 1;
			per = gConn.percentCompleted / 100.0;
			if (per < 0.0)
				per = 0.0;
			r.right = r.left + (int) ((double) (rect.right - 1 - r.left) * per);
			r.top = rect.top + 1;
			r.bottom = rect.bottom - 1;
			
			redBrush = CreateSolidBrush(RGB(255,0,0));
			FillRect(hdc, &r, redBrush);
			DeleteObject(redBrush);
			
			r.left = r.right;
			r.right = rect.right - 1;
			if ((r.left + 2) < r.right)
				FillRect(hdc, &r, GetStockObject(WHITE_BRUSH));

			r.left = rect.left + 10;
			r.right = rect.right - 10;
			r.top = rect.top + 2;
			r.bottom = rect.bottom - 2;
			
			oldBkMode = SetBkMode(hdc, TRANSPARENT);
			if (gConn.lname != NULL)
				DrawText(hdc, gConn.lname, -1, &r, DT_SINGLELINE | DT_WORD_ELLIPSIS | DT_VCENTER | DT_CENTER);
			(void) SetBkMode(hdc, oldBkMode);

			cp = strchr(gStatusText, '[');
			if (cp != NULL) {
				iper = (int) (per * 100.0 + 0.5);

				if ((iper > 99) && (cp[2] == '%')) {
					memmove(cp + 2, cp, strlen(cp) + 2);
				} else if ((iper > 99) && (cp[3] == '%')) {
					memmove(cp + 1, cp, strlen(cp) + 1);
				} else if ((iper > 9) && (cp[2] == '%')) {
					memmove(cp + 1, cp, strlen(cp) + 1);
				}
				sprintf(cp, "[%d", iper);
				if (iper > 99)
					cp[4] = '%';
				else if (iper > 9)
					cp[3] = '%';
				else
					cp[2] = '%';
			}
		} else {
			FillRect(hdc, &rect, GetStockObject(WHITE_BRUSH));
			FrameRect(hdc, &rect, GetStockObject(BLACK_BRUSH));
			if (gConn.lname != NULL)
				DrawText(hdc, gConn.lname, -1, &r, DT_SINGLELINE | DT_WORD_ELLIPSIS | DT_VCENTER | DT_CENTER);

			cp = strchr(gStatusText, '[');
			if (cp != NULL) {
				// Get rid of the prefix from [0%] - Down...
				//
				memmove(gStatusText, gStatusText + 7, strlen(gStatusText) + 7);
			}
		}
		SetWindowText(gMainWnd, gStatusText);

		oldBkColor = SetBkColor(hdc, RGB(192,192,192));

		rect.left = 10;
		rect.top = 30 + lineHeight;
		rect.right = clientRect.right - 10;
		rect.bottom = rect.top + lineHeight;
		bkgndBrush = CreateSolidBrush(RGB(192,192,192));
		FillRect(hdc, &rect, bkgndBrush);
		DeleteObject(bkgndBrush);

		rect.right = (clientRect.right / 2) - 10;
		if (sizeIsUnknown) {
			sprintf(str, PRINTF_LONG_LONG " bytes", (gConn.startPoint + gConn.bytesTransferred));
		} else {
			sprintf(str, PRINTF_LONG_LONG " of " PRINTF_LONG_LONG " bytes",
				inProgress ? (gConn.startPoint + gConn.bytesTransferred) : (longest_int) 0,
				gConn.expectedSize
				);
		}
		DrawText(hdc, str, -1, &rect, DT_SINGLELINE | DT_NOCLIP);
		
		if ((!sizeIsUnknown) && (inProgress)) {
			rect.left = (clientRect.right / 2);
			rect.right = (3 * clientRect.right / 4);
			secLeft = (int) (gConn.secLeft + 0.5);
			minLeft = secLeft / 60;
			secLeft = secLeft - (minLeft * 60);
			if (minLeft > 999) {
				minLeft = 999;
				secLeft = 59;
			}
			sprintf(str, "ETA: %d:%02d", minLeft, secLeft);
			DrawText(hdc, str, -1, &rect, DT_SINGLELINE | DT_CENTER);
		}
		
		if (inProgress) {
			rate = FileSize(gConn.kBytesPerSec * 1024.0, &rStr, NULL);
			rect.left = (3 * clientRect.right / 4);
			rect.right = clientRect.right - 10;
			sprintf(str, "%.1f %s/sec", rate, rStr);
			DrawText(hdc, str, -1, &rect, DT_SINGLELINE | DT_RIGHT);
		}

		SetBkColor(hdc, oldBkColor);
	}
}	/* OnDraw */





LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc;
	PAINTSTRUCT ps;

	switch (iMsg) {
	case WM_USER:
		return 0;

	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		OnDraw(hwnd, hdc);
		EndPaint(hwnd, &ps);
		return 0;

	case WM_DESTROY:
		gQuitRequested = 1;
		gConn.cancelXfer = 1;
		return 0;
	}
	
	return DefWindowProc(hwnd, iMsg, wParam, lParam);
}	// WndProc




#pragma warning(disable : 4100)		// warning C4100: unreferenced formal parameter
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance_unused, PSTR szCmdLine_unused, int iCmdShow)
{
	WNDCLASSEX wndclass;
	HWND hWnd;
	RECT r;

	ghInstance = hInstance;

	ZeroMemory(&gComCtls, sizeof(gComCtls));
	gComCtls.dwSize = sizeof(gComCtls);
	gComCtls.dwICC = ICC_PROGRESS_CLASS;
	if (! InitCommonControlsEx(&gComCtls)) {
		PerrorBox("InitCommonControlsEx");
		return 0;
	}

	ZeroMemory(&wndclass, sizeof(wndclass));
	wndclass.cbSize        = sizeof (wndclass) ;
	wndclass.style         = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = WndProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;
	wndclass.hInstance     = hInstance;
	wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = __T("ncftpbatch");
	wndclass.hIcon         = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MAINFRAME));

	if (RegisterClassEx(&wndclass) == (ATOM) 0) {
		PerrorBox("RegisterClassEx");
		return 0;
	}

	// Create the main window, which is
	// never intended to be seen.
	//
	hWnd = CreateWindow (
		wndclass.lpszClassName,		// window class name
		__T("NcFTPBatch"),			// window caption
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,		// window style
		100,						// initial x position
		100,						// initial y position
		450,						// initial x size
		100,						// initial y size
		NULL,						// parent window handle
		NULL,						// window menu handle
		hInstance,					// program instance handle
		NULL);						// creation parameters

	if (hWnd == NULL) {
		PerrorBox("CreateWindow(main window)");
		return 0;
	}
	gMainWnd = hWnd;

	GetClientRect(gMainWnd, &r);
	r.top = r.left = 10;
	r.right -= 10;
	r.bottom -= 10;

	ZeroMemory(gStatusText, (DWORD) sizeof(gStatusText));
	hWnd = CreateWindow (
		"STATIC",					// window class name
		gStatusText,				// window caption
		SS_LEFT | WS_CHILD | WS_VISIBLE,			// window style
		r.left,						// initial x position
		r.top,						// initial y position
		r.right - r.left,			// initial x size
		r.bottom - r.top,			// initial y size
		gMainWnd,					// parent window handle
		NULL,						// window menu handle
		hInstance,					// program instance handle
		NULL);						// creation parameters

	if (hWnd == NULL) {
		PerrorBox("CreateWindow(static control)");
		return 0;
	}
	gStaticCtrl = hWnd;

	SendMessage(gMainWnd, WM_USER, (WPARAM) 0, (LPARAM) 0);
	ShowWindow(gMainWnd, SW_SHOWNORMAL);
	// Here we go!
	//
	PreInit("ncftpbatch.exe");
	PostInit();
	EventShell(0);
	PostShell();

	return 0;
}	// WinMain
#pragma warning(default : 4100)		// warning C4100: unreferenced formal parameter


#else	/* UNIX */

static void
ReadCore(int fd)
{
	FTPLibraryInfo tLib;
	FTPConnectionInfo tConn;
	int rc;

	if ((PRead(fd, (char *) &tLib, sizeof(tLib), 1) == sizeof(tLib))
		&& (PRead(fd, (char *) &tConn, sizeof(tConn), 1) == sizeof(tConn))
		&& (strncmp(tConn.magic, gConn.magic, sizeof(tConn.magic)) == 0)
	) {
		(void) memcpy(&gConn, &tConn, sizeof(gConn));
		(void) memcpy(&gLib, &tLib, sizeof(gLib));
		rc = FTPRebuildConnectionInfo(&gLib, &gConn);
		if (rc < 0) {
			FTPInit();
		} else {
			gConn.debugLogProc = DebugHook;
		}
	}
}	/* ReadCore */




static void
Daemon(void)
{
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
	/* Change to root directory so filesystems
	 * can be unmounted, if they could in fact
	 * be unmounted.
	 */
	(void) chdir("\\");
#else
	int i, fd;
	int devnull;
	int pid;

	/* Redirect standard in, out, and err, if they were terminals. */
	devnull = open("/dev/null", O_RDWR, 00666);

	for (i=0; i<3; i++) {
		if (gConn.ctrlSocketR == i)
			continue;
		if (gConn.ctrlSocketW == i)
			continue;

		/* Close standard descriptors and replace
		 * with /dev/null.
		 */
		(void) close(i);
		if (devnull >= 0)
			(void) dup2(devnull, i);
	}

	if (devnull >= 0)
		(void) close(devnull);

	/* Close all unneeded descriptors. */
	for (fd = 3; fd < 256; fd++) {
		if (gConn.ctrlSocketR == fd)
			continue;
		if (gConn.ctrlSocketW == fd)
			continue;
		(void) close(fd);
	}

	pid = fork();
	if (pid < 0)
		exit(1);
	else if (pid > 0)
		exit(0);	/* parent. */

#ifdef HAVE_SETSID
	/* Become session leader for this "group." */
	(void) setsid();
#endif

	/* Run as "nohup."  Don't want to get hangup signals. */
	(void) NcSignal(SIGHUP, (FTPSigProc) SIG_IGN);

	/* Turn off TTY control signals, just to be sure. */
	(void) NcSignal(SIGINT, (FTPSigProc) SIG_IGN);
	(void) NcSignal(SIGQUIT, (FTPSigProc) SIG_IGN);
#ifdef SIGTSTP
	(void) NcSignal(SIGTSTP, (FTPSigProc) SIG_IGN);
#endif
	
	/* Become our own process group. */
#ifdef HAVE_SETPGID
	(void) setpgid(0, 0);
#elif defined(HAVE_SETPGRP) && defined(SETPGRP_VOID)
	(void) setpgrp();
#elif defined(HAVE_SETPGRP) && !defined(SETPGRP_VOID)
	(void) setpgrp(0, getpid());
#endif

#ifdef TIOCNOTTY
	/* Detach from controlling terminal, so this
	 * process is not associated with any particular
	 * tty.
	 */
	fd = open("/dev/tty", O_RDWR, 0);
	if (fd >= 0) {
		(void) ioctl(fd, TIOCNOTTY, 0);
		(void) close(fd);
	}
#endif

	/* Change to root directory so filesystems
	 * can be unmounted.
	 */
	if (chdir("/") < 0)
		exit(1);
#endif
}	/* Daemon */




static void
#if (defined(__GNUC__)) && (__GNUC__ >= 2)
	__attribute__ ((noreturn))
#endif
Usage(void)
{
	(void) fprintf(stderr, "Usages:\n");
	if (gGlobalSpooler == 0) {
		(void) fprintf(stderr, "\tncftpbatch -d | -D [-x xfer-log-file] [-U umask]  (start NcFTP batch processing)\n");
		(void) fprintf(stderr, "\tncftpbatch -l                                     (list spooled jobs)\n");
	} else {
#ifdef F_SETLK
		(void) fprintf(stderr, "\tncftpspooler -d [-q spool-dir] [-K lock-file] [-o log-file] [-O max-log-size] [-x xfer-log-file] [-s delay] [-U umask]\n");
#else
		(void) fprintf(stderr, "\tncftpspooler -d [-q spool-dir] [-o log-file] [-O max-log-size] [-x xfer-log-file] [-s delay] [-U umask]\n");
#endif
		(void) fprintf(stderr, "\tncftpspooler -l  (list spooled jobs)\n");
	}
	(void) fprintf(stderr, "\nLibrary version: %s.\n", gLibNcFTPVersion + 5);
	(void) fprintf(stderr, "This is a freeware program by Mike Gleason (http://www.NcFTP.com).\n");
	DisposeWinsock();
	exit(2);
}	/* Usage */




main_void_return_t
main(int argc, char **const argv)
{
	int c;
	int runAsDaemon = -1;
	unsigned int sleepval = 0;
	int listonly = -1;
	int activeonly = 0;
	int readcore = -1;
	longest_int o;
	GetoptInfo opt;

	PreInit(argv[0]);
#if (defined(SOCKS)) && (SOCKS >= 5)
	SOCKSinit(argv[0]);
#endif	/* SOCKS */

	if (gGlobalSpooler != 0) {
		runAsDaemon = -1;
		GetoptReset(&opt);
		while ((c = Getopt(&opt, argc, argv, "a:DdlL:x:s:q:o:O:U:K:")) > 0) switch(c) {
			case 'a':
				activeonly++;
				break;
			case 'd':
				runAsDaemon = 1;
				break;
			case 'D':
				runAsDaemon = 0;
				break;
			case 'x':
				STRNCPY(gXferLogFileName, opt.arg);
				break;
			case 'K':
				STRNCPY(gLockFile, opt.arg);
				break;
			case 'L':
				/* Deprecated */
				STRNCPY(gProgressLog, opt.arg);
				break;
			case 'l':
				if (listonly < 0) listonly = 0;
				listonly++;
				break;
			case 's':
				if (atoi(opt.arg) > 0)
					gDelayBetweenPasses = (unsigned int) atoi(opt.arg);
				break;
			case 'q':
				STRNCPY(gSpoolDir, opt.arg);
				break;
			case 'o':
				STRNCPY(gLogFileName, opt.arg);
				break;
			case 'O':
				o = 0;
				(void) sscanf(opt.arg, PRINTF_LONG_LONG, &o);
				gMaxLogSize = (off_t) o;
				break;
			case 'U':
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
				{
					unsigned int um = 0xFFFFFFFF;
					if ((sscanf(opt.arg, "%o", &um) == 1) && (um != (0xFFFFFFFF))) {
						(void) umask(um);
					}
				}
				break;
#else
				Usage();
#endif
			default:
				Usage();
		}

		/* Require that they use the "-d" option,
		 * since otherwise a ncftpspooler process
		 * would be launched into the background
		 * unbeknownst to the user.  It is common
		 * for a user to run a program with no
		 * arguments in hopes of seeing a usage
		 * screen.
		 */
		if ((listonly < 0) && (runAsDaemon < 0)) {
			/* Must specify either -l or -d/-D */
			Usage();
		}
	} else {
		/* User Spooler */
		GetoptReset(&opt);
		while ((c = Getopt(&opt, argc, (char **) argv, "|:aXDdlLO::x:S:Z:s:wU:K:")) > 0) switch(c) {
			case 'a':
				activeonly++;
				break;
			case 'd':
				runAsDaemon = 1;
				break;
			case 'D':
				runAsDaemon = 0;
				break;
			case 'l':
				if (listonly < 0) listonly = 0;
				listonly++;
				break;
			case 'x':
				STRNCPY(gXferLogFileName, opt.arg);
				break;
			case 'K':
				STRNCPY(gLockFile, opt.arg);
				break;
			case 'L':
				STRNCPY(gProgressLog, opt.arg);
				break;
			case 'O':
				o = 0;
				(void) sscanf(opt.arg, PRINTF_LONG_LONG, &o);
				gMaxLogSize = (off_t) o;
				break;
			case 'Z':
				if (atoi(opt.arg) > 0)
					sleep((unsigned int) atoi(opt.arg));
				break;
			case 'S':
				if (atoi(opt.arg) > 0)
					sleepval = (unsigned int) atoi(opt.arg);
				break;
			case 's':
				if (atoi(opt.arg) > 0)
					gDelayBetweenPasses = (unsigned int) atoi(opt.arg);
				break;
			case 'w':
				gLogOpenMode = FOPEN_WRITE_TEXT;
				break;
			case 'X':
				/* Yes, I do exist. */
				DisposeWinsock();
				exit(0);
				/*NOTREACHED*/
			case '|':
				readcore = atoi(opt.arg);
				break;
			case 'U':
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
				{
					unsigned int um2 = 0xFFFFFFFF;
					if ((sscanf(opt.arg, "%o", &um2) == 1) && (um2 != (0xFFFFFFFF))) {
						(void) umask(um2);
					}
				}
				break;
#else
				Usage();
#endif
			default:
				Usage();
		}

		if ((listonly < 0) && (runAsDaemon < 0)) {
			/* Must specify either -l or -d/-D */
			Usage();
		}
	}

	PostInit();
	if (listonly > 0) {
		ListQueue(listonly, activeonly);
	} else {
		if (gGlobalSpooler == 0) {
			if (readcore >= 0) {
				/* Inherit current live FTP session
				 * from ncftp!
				 */
				ReadCore(readcore);
			}
		}
		if (runAsDaemon > 0) {
			if (OpenLog() < 0) {
				perror(gLogFileName);
				exit(1);
			}
			Daemon();
			gIsTTY = 0;
		}

		EventShell(sleepval);
		PostShell();
	}
	DisposeWinsock();
	exit(0);
}	/* main */

#endif	/* UNIX */


#else	/* HAVE_LONG_FILE_NAMES */
main()
{
	fprintf(stderr, "this program needs long filenames, sorry.\n");
	exit(1);
}	/* main */
#endif	/* HAVE_LONG_FILE_NAMES */
