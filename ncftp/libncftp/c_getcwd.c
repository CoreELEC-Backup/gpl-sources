/* c_getcwd.c
 *
 * Copyright (c) 1996-2005 Mike Gleason, NcFTP Software.
 * All rights reserved.
 *
 */

#include "syshdrs.h"
#ifdef PRAGMA_HDRSTOP
#	pragma hdrstop
#endif

int
FTPGetCWD(const FTPCIPtr cip, char *const newCwd, const size_t newCwdSize)
{
	ResponsePtr rp;
	char *l, *r;
	int result;

	if (cip == NULL)
		return (kErrBadParameter);
	if (strcmp(cip->magic, kLibraryMagic))
		return (kErrBadMagic);

	if ((newCwd == NULL) || (newCwdSize == 0)) {
		result = kErrInvalidDirParam;
		cip->errNo = kErrInvalidDirParam;
	} else if ((cip->currentWorkingDirectory != NULL) && (cip->currentWorkingDirectory[0] != '\0')) {
		/* Already have it cached. */
		if (newCwd != cip->currentWorkingDirectory)
			(void) Strncpy(newCwd, cip->currentWorkingDirectory, newCwdSize);
		result = kNoErr;
	} else {
		rp = InitResponse();
		if (rp == NULL) {
			result = kErrMallocFailed;
			cip->errNo = kErrMallocFailed;
			FTPLogError(cip, kDontPerror, "Malloc failed.\n");
		} else {
			result = RCmd(cip, rp, "PWD");
			if (cip->currentWorkingDirectory != NULL) {
				cip->currentWorkingDirectory[0] = '\0';
				cip->currentWorkingDirectory[cip->currentWorkingDirectorySize - 2] = '\0';
			}
			if (result == 2) {
				if ((r = strrchr(rp->msg.first->line, '"')) != NULL) {
					/* "xxxx" is current directory.
					 * Strip out just the xxxx to copy into the remote cwd.
					 */
					l = strchr(rp->msg.first->line, '"');
					if ((l != NULL) && (l != r)) {
						*r = '\0';
						++l;
						if (cip->currentWorkingDirectory != NULL)
							(void) Strncpy(cip->currentWorkingDirectory, l, cip->currentWorkingDirectorySize);
						if (newCwd != cip->currentWorkingDirectory)
							(void) Strncpy(newCwd, l, newCwdSize);
						*r = '"';	/* Restore, so response prints correctly. */
					}
				} else {
					/* xxxx is current directory.
					 * Mostly for VMS.
					 */
					if ((r = strchr(rp->msg.first->line, ' ')) != NULL) {
						*r = '\0';
						if (cip->currentWorkingDirectory != NULL)
							(void) Strncpy(cip->currentWorkingDirectory, (rp->msg.first->line), cip->currentWorkingDirectorySize);
						if (newCwd != cip->currentWorkingDirectory)
							(void) Strncpy(newCwd, (rp->msg.first->line), newCwdSize);
						*r = ' ';	/* Restore, so response prints correctly. */
					}
				}
				result = kNoErr;
			} else if (result > 0) {
				result = kErrPWDFailed;
				cip->errNo = kErrPWDFailed;
			}
			DoneWithResponse(cip, rp);
		}
	}
	if (result == kNoErr) {
		if (cip->currentWorkingDirectory[cip->currentWorkingDirectorySize - 2] != '\0') {
			cip->currentWorkingDirectory[0] = '\0';
			result = kErrPathTooLong;
		}
		if (newCwd[newCwdSize - 2] != '\0')
			result = kErrPathTooLong;
	}
	return (result);
}	/* FTPGetCWD */




int
FTPChdirAndGetCWD(const FTPCIPtr cip, const char *const cdCwd, char *const newCwd, const size_t newCwdSize)
{
	ResponsePtr rp;
	char *l, *r;
	int result;

	if (cip == NULL)
		return (kErrBadParameter);
	if (strcmp(cip->magic, kLibraryMagic))
		return (kErrBadMagic);

	if ((newCwd == NULL) || (cdCwd == NULL)) {
		result = kErrInvalidDirParam;
		cip->errNo = kErrInvalidDirParam;
	} else {
		if (cdCwd[0] == '\0') {	/* But allow FTPChdir(cip, ".") to go through. */
			result = FTPGetCWD(cip, newCwd, newCwdSize);
			return (result);
		}
		rp = InitResponse();
		if (rp == NULL) {
			result = kErrMallocFailed;
			cip->errNo = kErrMallocFailed;
			FTPLogError(cip, kDontPerror, "Malloc failed.\n");
		} else {
			if (strcmp(cdCwd, "..") == 0)
				result = RCmd(cip, rp, "CDUP"); 	
			else
				result = RCmd(cip, rp, "CWD %s", cdCwd);
			if (result == 2) {
				if (cip->currentWorkingDirectory != NULL)
					cip->currentWorkingDirectory[0] = '\0';		/* Invalidate pwd cache. */
				l = strchr(rp->msg.first->line, '"');
				if ((l == rp->msg.first->line) && ((r = strrchr(rp->msg.first->line, '"')) != NULL) && (l != r)) {
					/* "xxxx" is current directory.
					 * Strip out just the xxxx to copy into the remote cwd.
					 *
					 * This is nice because we didn't have to do a PWD.
					 */
					*r = '\0';
					++l;
					if (cip->currentWorkingDirectory != NULL)
						(void) Strncpy(cip->currentWorkingDirectory, l, cip->currentWorkingDirectorySize);
					if (newCwd != cip->currentWorkingDirectory)
						(void) Strncpy(newCwd, l, newCwdSize);
					*r = '"';	/* Restore, so response prints correctly. */
					DoneWithResponse(cip, rp);
					result = kNoErr;
				} else {
					DoneWithResponse(cip, rp);
					result = FTPGetCWD(cip, newCwd, newCwdSize);
				}
			} else if (result > 0) {
				result = kErrCWDFailed;
				cip->errNo = kErrCWDFailed;
				DoneWithResponse(cip, rp);
			} else {
				DoneWithResponse(cip, rp);
			}
		}
	}
	return (result);
}	/* FTPChdirAndGetCWD */
