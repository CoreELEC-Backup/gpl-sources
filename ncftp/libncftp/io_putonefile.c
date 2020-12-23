/* io_putonefile.c
 *
 * Copyright (c) 1996-2005 Mike Gleason, NcFTP Software.
 * All rights reserved.
 *
 */

#include "syshdrs.h"
#ifdef PRAGMA_HDRSTOP
#	pragma hdrstop
#endif

#ifndef NO_SIGNALS
#	define NO_SIGNALS 1
#endif

int
FTPPutOneFile4(
	const FTPCIPtr cip,
	const char *const file,
	const char *const dstfile,
	const int xtype,
	const int fdtouse,
	const int appendflag,
	const char *const tmppfx,
	const char *const tmpsfx,
	const int resumeflag,
	const int deleteflag,
	const FTPConfirmResumeUploadProc resumeProc,
	const time_t batchStartTime,
	const time_t origLmtime)
{
	int result;

	if (cip == NULL)
		return (kErrBadParameter);
	if (strcmp(cip->magic, kLibraryMagic))
		return (kErrBadMagic);
	
	if ((dstfile == NULL) || (dstfile[0] == '\0'))
		return (kErrBadParameter);
	if (fdtouse < 0) {
		if ((file == NULL) || (file[0] == '\0'))
			return (kErrBadParameter);
	}
	result = FTPPutOneF(cip, file, dstfile, xtype, fdtouse, appendflag, tmppfx, tmpsfx, resumeflag, deleteflag, resumeProc, batchStartTime, origLmtime);
	return (result);
}	/* FTPPutOneFile4 */




int
FTPPutOneFile(const FTPCIPtr cip, const char *const file, const char *const dstfile)
{
	return (FTPPutOneFile4(cip, file, dstfile, kTypeBinary, -1, 0, NULL, NULL, kResumeNo, kDeleteNo, kNoFTPConfirmResumeUploadProc, 0, 0));
}	/* FTPPutOneFile */




int
FTPPutOneFile2(const FTPCIPtr cip, const char *const file, const char *const dstfile, const int xtype, const int fdtouse, const int appendflag, const char *const tmppfx, const char *const tmpsfx)
{
	return (FTPPutOneFile4(cip, file, dstfile, xtype, fdtouse, appendflag, tmppfx, tmpsfx, kResumeNo, kDeleteNo, kNoFTPConfirmResumeUploadProc, 0, 0));
}	/* FTPPutOneFile2 */




int
FTPPutOneFile3(const FTPCIPtr cip, const char *const file, const char *const dstfile, const int xtype, const int fdtouse, const int appendflag, const char *const tmppfx, const char *const tmpsfx, const int resumeflag, const int deleteflag, const FTPConfirmResumeUploadProc resumeProc, int UNUSED(reserved))
{
	LIBNCFTP_USE_VAR(reserved);
	return (FTPPutOneFile4(cip, file, dstfile, xtype, fdtouse, appendflag, tmppfx, tmpsfx, resumeflag, deleteflag, resumeProc, 0, 0));
}	/* FTPPutOneFile3 */




int
FTPPutOneFileAscii(const FTPCIPtr cip, const char *const file, const char *const dstfile)
{
	return (FTPPutOneFile4(cip, file, dstfile, kTypeAscii, -1, 0, NULL, NULL, kResumeNo, kDeleteNo, kNoFTPConfirmResumeUploadProc, 0, 0));
}	/* FTPPutOneFileAscii */
