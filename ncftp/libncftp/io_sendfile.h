/* dosendfile.h 
 *
 * Copyright (c) 2016 Mike Gleason, NcFTP Software.
 * All rights reserved.
 *
 */

#ifndef longest_int
#	define longest_int long long int
#	define longest_uint unsigned long long int
#endif

#define kDoSendfileID 0xD053F113
#define kDoSendfileVersion 1

typedef struct {
	unsigned int id;
	int version;
    size_t size;
	longest_int current;
} DoSendfileProgressInfo;

typedef struct DoSendfileParams *DoSendfileParamsPtr;

typedef int (*DoSendfileProgressProc)(DoSendfileProgressInfo *const dspip);
typedef void (*DoSendfileErrPrintFProc)(DoSendfileParamsPtr dspp, const char *const fmt, ...)
#if (defined(__GNUC__)) && (__GNUC__ >= 2)
__attribute__ ((format (printf, 2, 3)))
#endif
;

typedef struct DoSendfileParams {
	unsigned int id;
    int version;
	size_t size;
	int printError;
	int userSpaceModeOnly;
	int sendfileModeOnly;
	int nonBlockingOutput;
	int nonBlockingInput;
	int handleSIGINT;
	off_t initialOffset;
	double xferTimeout;
	char *buf;			/* Can be NULL */
	size_t bufSize;
	DoSendfileErrPrintFProc dseppp;	/* Can be NULL */
	DoSendfileProgressInfo *dspip;	/* Can be NULL */
	DoSendfileProgressProc dsppp;	/* Can be NULL */
} DoSendfileParams;

extern int gSendfileInProgress;

int DoSendfileAvailable(void);
void DoSendfileParamsInit(DoSendfileParams *const dspp, const size_t siz);
void DoSendfileProgressInfoInit(DoSendfileProgressInfo *const dspip, const size_t siz);
void DoSendfileErrPrintF(DoSendfileParamsPtr dspp, const char *const fmt, ...)
#if (defined(__GNUC__)) && (__GNUC__ >= 2)
__attribute__ ((format (printf, 2, 3)))
#endif
;
int DoSendfile(const int ifd, const longest_int isize, const int ofd, longest_int *const osize, DoSendfileParams *const dspp);
