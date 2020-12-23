/* spool.h
 *
 * Copyright (c) 1992-2016 by Mike Gleason.
 * All rights reserved.
 * 
 */

#define kSpoolDir "spool"
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
#	define kSpoolLog "log.txt"
#else
#	define kSpoolLog "log"
#endif

#define kSpoolEntryBegin	"#BEGIN#"
#define kSpoolEntryBeginLine	kSpoolEntryBegin " This is a NcFTP spool file entry."

/* spool.c */
void TruncBatchLog(void);
int MkSpoolDir(char *, size_t);
void SpoolName(char *const sp, const size_t size, const int flag, const int serial, time_t when);
int CanSpool(void);
int HaveSpool(void);
char *SpoolFilePath(char *const dst, size_t dsize, const char *sdir, const char *const sname);
FILE *OpenSpoolFile(
	FILE *const ofp,
	char *const initialpath,
	const size_t ipsize,
	char *const finalpath,
	const size_t fpsize,
	char *const sname,
	const size_t snsize,
	const char *sdir,
	const char *const op,
	const time_t when
	);
int WriteSpoolEntry(
	FILE *const fp,
	const char *const op,
	const char *const jobname,
	const char *const rfile,
	const char *const rdir,
	double rfilesize,
	time_t rmtime,
	const char *const lfile,
	const char *const ldir,
	double lfilesize,
	time_t lmtime,
	const char *const rnewfile,
	const char *const rnewdir,
	const char *const lnewfile,
	const char *const lnewdir,
	const char *const host,
	const char *const ip,
	const unsigned int port,
	const char *const user,
	const char *const passclear,
	const char *const xacct,
	const int xtype,
	const int recursive,
	const int deleteflag,
	const int passive,
	const char *const preftpcmd,
	const char *const perfileftpcmd,
	const char *const postftpcmd,
	const char *const preshellcmd,
	const char *const postshellcmd,
	const unsigned int delaySinceLastFailure,
	const char *const manualOverrideFeatures,
	const char *const preferredLocalAddrStr,
	const time_t timeOfFirstAttempt,
	const time_t timeOfLastFailure
	);
int CloseSpoolFileAndRename(FILE *const fp, FILE *const efp, char *const initialpath, char *const finalpath);
int SpoolX(
	FILE *const ofp,
	FILE *const efp,
	const char *sdir,
	const char *const op,
	const char *const rfile,
	const char *const rdir,
	double rfilesize,
	time_t rmtime,
	const char *const lfile,
	const char *const ldir,
	double lfilesize,
	time_t lmtime,
	const char *const rnewfile,
	const char *const rnewdir,
	const char *const lnewfile,
	const char *const lnewdir,
	const char *const host,
	const char *const ip,
	const unsigned int port,
	const char *const user,
	const char *const passclear,
	const char *const xacct,
	const int xtype,
	const int recursive,
	const int deleteflag,
	const int passive,
	const char *const preftpcmd,
	const char *const perfileftpcmd,
	const char *const postftpcmd,
	const char *const preshellcmd,
	const char *const postshellcmd,
	const time_t when,
	const unsigned int delaySinceLastFailure,
	const char *const manualOverrideFeatures,
	const char *const preferredLocalAddrStr,
	const time_t timeOfFirstAttempt,
	const time_t timeOfLastFailure,
	const int reserved);
void RunBatch(void);
void RunBatchWithCore(const FTPCIPtr);
void Jobs(void);
void RunBatchIfNeeded(const FTPCIPtr);
