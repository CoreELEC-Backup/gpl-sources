/* gpshare.h
 *
 * Shared routines for ncftpget and ncftpput.
 */

typedef enum ExitStatus {
	kExitSuccess = 0,
	kExitOpenFailed = 1,
	kExitOpenTimedOut,
	kExitXferFailed,
	kExitXferTimedOut,
	kExitChdirFailed,
	kExitChdirTimedOut,
	kExitMalformedURL,
	kExitUsage,
	kExitBadConfigFile,
	kExitInitLibraryFailed,
	kExitInitConnInfoFailed,
	kExitSpoolFailed,
	kExitNoMemory,
	kExitMkdirFailed,
	kExitGlobNoMatch
} ExitStatus;

#define kKilobyte 1024
#define kMegabyte (kKilobyte * 1024)
#define kGigabyte ((long) kMegabyte * 1024L)
#define kTerabyte ((double) kGigabyte * 1024.0)

#define kErrorLoadingConfig (-1)
#define kConfigNotLoaded 0
#define kLoadedFromConfigFile 1
#define kLoadedFromBookmark 2

#ifndef STDIN_FILENO
#	define STDIN_FILENO 0
#	define STDOUT_FILENO 1
#	define STDERR_FILENO 2
#endif

/* gpshare.c */
double FileSize(double size, const char **uStr0, double *uMult0);
void PrSizeAndRateMeter(const FTPCIPtr, int);
void PrStatBar(const FTPCIPtr, int);
int ReadConfigFromBookmark(const char *const bmname, FTPCIPtr cip);
int ReadConfigFile(const char *, FTPCIPtr);
void SetRedial(const FTPCIPtr, const char *const);
void SetTimeouts(const FTPCIPtr, const char *const);
char *GetPass2(const char *const prompt);
int GetDefaultProgressMeterSetting(void);
FILE *OpenPager(void);
void ClosePager(FILE *fp);
int AdditionalCmd(FTPCIPtr const cip, const char *const spec, const char *const arg);
