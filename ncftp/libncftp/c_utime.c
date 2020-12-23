/* c_utime.c
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

static void
GmTimeStr(char *const dst, const size_t dstsize, time_t t)
{
	char buf[64];
	struct tm gt;

	if (Gmtime(t, &gt) == NULL) {
		dst[0] = '\0';
	} else {
#ifdef HAVE_SNPRINTF
		buf[sizeof(buf) - 1] = '\0';
		(void) snprintf(buf, sizeof(buf) - 1, "%04d%02d%02d%02d%02d%02d",
                        /* YYYYMMDDhhmmss */
                        gt.tm_year + 1900,
                        gt.tm_mon + 1,
                        gt.tm_mday,
                        gt.tm_hour,
                        gt.tm_min,
                        gt.tm_sec
                        );
#else
		(void) sprintf(buf, "%04d%02d%02d%02d%02d%02d",
                       /* YYYYMMDDhhmmss */
                       gt.tm_year + 1900,
                       gt.tm_mon + 1,
                       gt.tm_mday,
                       gt.tm_hour,
                       gt.tm_min,
                       gt.tm_sec
                       );
#endif
		(void) Strncpy(dst, buf, dstsize);
	}
}	/* GmTimeStr */




int
FTPUtime(const FTPCIPtr cip, const char *const file, time_t actime, time_t modtime, time_t crtime)
{
	char mstr[64], astr[64], cstr[64];
	time_t now;
	int result = kNoErr;
	int wantToSetCrTime = 1;
	ResponsePtr rp;

	if (cip == NULL)
		return (kErrBadParameter);
	if (strcmp(cip->magic, kLibraryMagic))
		return (kErrBadMagic);

	now = (time_t) 0;
	if ((modtime == (time_t) 0) || (modtime == (time_t) -1))
		modtime = time(&now);
	(void) GmTimeStr(mstr, sizeof(mstr), modtime);

	if ((actime == (time_t) 0) || (actime == (time_t) -1)) {
		if (now != (time_t) 0) {
			actime = now;
		} else {
			actime = time(&now);
		}
	}
	if ((crtime == (time_t) 0) || (crtime == (time_t) -1)) {
		wantToSetCrTime = 0;
		if (now != (time_t) 0) {
			crtime = now;
		} else {
			crtime = time(&now);
		}
	}
	(void) GmTimeStr(astr, sizeof(astr), actime);
	(void) GmTimeStr(cstr, sizeof(cstr), crtime);

	if ((cip->hasMFF == kCommandAvailable) && (wantToSetCrTime != 0)) {
		result = FTPCmd(cip, "MFF Modify=%s;Create=%s %s", mstr, cstr, file); 	
		if ((result == 2) || (result == 0)) {
			result = kNoErr;
			return (result);
		} else {
			cip->errNo = kErrUTIMEFailed;
			result = kErrUTIMEFailed;
			/* It may not have liked us trying to set the creation time.
			 * Fallthrough and try just setting the modification time.
			 */
		}
	}

	if (cip->hasMFMT == kCommandAvailable) {
		result = FTPCmd(cip, "MFMT %s %s", mstr, file); 	
		if ((result == 2) || (result == 0)) {
			result = kNoErr;
		} else {
			cip->errNo = kErrUTIMEFailed;
			result = kErrUTIMEFailed;
		}
		return (result);
	}

	if (result == kNoErr)
		result = kErrUTIMENotAvailable;
	if (cip->hasSITE_UTIME != kCommandNotAvailable) {
		rp = InitResponse();
		if (rp == NULL) {
			result = kErrMallocFailed;
			cip->errNo = kErrMallocFailed;
			FTPLogError(cip, kDontPerror, "Malloc failed.\n");
		} else {
			result = RCmd(cip, rp, "SITE UTIME %s %s %s %s UTC", file, astr, mstr, cstr); 	
			if (result < 0) {
				DoneWithResponse(cip, rp);
				return (result);
			} else if (result == 2) {
				cip->hasSITE_UTIME = kCommandAvailable;
				result = kNoErr;
				DoneWithResponse(cip, rp);
			} else if ((FTP_UNIMPLEMENTED_CMD(rp->code)) || (FTP_SYNTAX_ERROR_IN_PARAMETERS(rp->code))) {
				cip->hasSITE_UTIME = kCommandNotAvailable;
				cip->errNo = kErrUTIMENotAvailable;
				result = kErrUTIMENotAvailable;
				DoneWithResponse(cip, rp);
			} else if ((cip->serverType == kServerTypeNcFTPd) && (strchr(file, ' ') != NULL)) {
				/* Workaround bug with filenames containing
				 * spaces.
				 */
				DoneWithResponse(cip, rp);
				result = FTPCmd(cip, "MDTM %s %s", mstr, file); 	
				if ((result == 2) || (result == 0)) {
					result = kNoErr;
				} else {
					cip->errNo = kErrUTIMEFailed;
					result = kErrUTIMEFailed;
				}
			} else {
				cip->errNo = kErrUTIMEFailed;
				result = kErrUTIMEFailed;
				DoneWithResponse(cip, rp);
			}
		}
	}
	if (result == kErrUTIMENotAvailable) {
		if ((cip->hasMDTM == kCommandNotAvailable) || (cip->hasMDTM_set == kCommandNotAvailable)) {
			cip->errNo = kErrUTIMENotAvailable;
			result = kErrUTIMENotAvailable;
		} else {
			rp = InitResponse();
			if (rp == NULL) {
				result = kErrMallocFailed;
				cip->errNo = kErrMallocFailed;
				FTPLogError(cip, kDontPerror, "Malloc failed.\n");
			} else {
				result = RCmd(cip, rp, "MDTM %s %s", mstr, file); 	
				if (result < 0) {
					DoneWithResponse(cip, rp);
					return (result);
				} else if (result == 2) {
					cip->hasMDTM_set = kCommandAvailable;
					result = kNoErr;
				} else {
					/* Ideally, we would only disable
					 * the MDTM_set feature if we
					 * received a code that corresponds
					 * to an unimplemented command.
					 * Unfortunately, since the regular
					 * syntax of MDTM uses a pathname
					 * parameter, we'll often get back
					 * a 550 response when we try to
					 * set the timestamp, because a
					 * server that doesn't support this
					 * feature reads the timestamp
					 * as a pathname which doesn't
					 * exist.  As a result, this feature
					 * could get disabled if a server
					 * which does support the feature
					 * returns a 550 when it turns
					 * out that the file exists but
					 * we didn't have permission to
					 * change the timestamp.
					 */
					if ((FTP_UNIMPLEMENTED_CMD(rp->code)) || (FTP_SYNTAX_ERROR_IN_PARAMETERS(rp->code)) || (rp->code == 550))
						cip->hasMDTM_set = kCommandNotAvailable;
					cip->errNo = kErrUTIMENotAvailable;
					result = kErrUTIMENotAvailable;
				}
				DoneWithResponse(cip, rp);
			}
		}
	}
	return (result);
}	/* FTPUtime */
