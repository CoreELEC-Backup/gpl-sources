/* rglobr.c
 *
 * Copyright (c) 1996-2005 Mike Gleason, NcFTP Software.
 * All rights reserved.
 *
 */

#include "syshdrs.h"
#ifdef PRAGMA_HDRSTOP
#	pragma hdrstop
#endif

#if 0
static void
printls(LineListPtr list)
{
	LinePtr fip;
	int i;

	for (i = 1, fip = list->first; fip != NULL; fip = fip->next, i++)
		printf("%4d: %s\n", i, fip->line == NULL ? "NULL" : fip->line);
}

static void
print1(FTPFileInfoListPtr list)
{
	FTPFileInfoPtr fip;
	int i;

	for (i = 1, fip = list->first; fip != NULL; fip = fip->next, i++)
		printf("%4d: %s\n", i, fip->relname == NULL ? "NULL" : fip->relname);
}



static void
print2(FTPFileInfoListPtr list)
{
	FTPFileInfoPtr fip;
	int i, n;

	n = list->nFileInfos;
	for (i=0; i<n; i++) {
		fip = list->vec[i];
		printf("%4d: %s\n", i + 1, fip->relname == NULL ? "NULL" : fip->relname);
	}
}




static void
SortRecursiveFileList(FTPFileInfoListPtr files)
{
	VectorizeFileInfoList(files);
	SortFileInfoList(files, 'b', '?');
	UnvectorizeFileInfoList(files);
}	/* SortRecursiveFileList */
#endif

struct MyFtwExtraInfo {
	size_t limitMaxDirs;
	size_t limitMaxFiles;
	size_t limitMaxDepth;
	FTPFileInfoListPtr fileList;
};

int
FTPRemoteFtwProc(const FtwInfoPtr ftwip)
{
	struct MyFtwExtraInfo *xinfop = (struct MyFtwExtraInfo *) ftwip->userdata;
	FTPCIPtr cip = (FTPCIPtr) ftwip->cip;
	FTPFileInfo fi;

	if ((xinfop->limitMaxDepth != 0) && (ftwip->depth >= xinfop->limitMaxDepth)) {
		FTPLogError(cip, kDontPerror, "Depth limit reached (%u dirs deep).\n", (unsigned int) ftwip->depth);
		return (-1);
	}

	InitFileInfo(&fi);
	if (strncmp(ftwip->curPath, "./", 2) == 0) {
		fi.relnameLen = ftwip->curPathLen - 2;
		fi.relname = StrDup(ftwip->curPath + 2);
	} else if (strcmp(ftwip->curPath, ".") == 0) {
		/* Don't need starting directory */
		return (0);
	} else {
		fi.relnameLen = ftwip->curPathLen;
		fi.relname = StrDup(ftwip->curPath);
	}
	fi.rname = NULL;
	fi.lname = NULL;
	fi.rlinkto = (ftwip->rlinkto == NULL) ? NULL : StrDup(ftwip->rlinkto);
	fi.mdtm = ftwip->curStat.st_mtime;
	fi.size = (longest_int) ftwip->curStat.st_size;
	fi.type = ftwip->curType;
	fi.mode = (int) ftwip->curStat.st_mode;
	AddFileInfo(xinfop->fileList, &fi);
	
	/*
fprintf(stderr, "(%c) relname [%s], rlinkto [%s], mdtm %lld, size %lld, mode %08o\n", fi.type, fi.relname, fi.rlinkto == NULL ? "" : fi.rlinkto, fi.mdtm, fi.size, fi.mode);
	*/

	if ((xinfop->limitMaxDirs != 0) && (ftwip->numDirs >= xinfop->limitMaxDirs)) {
		FTPLogError(cip, kDontPerror, "Max subdirs limit (%u) reached.\n", (unsigned int) ftwip->numDirs);
		return (-1);
	}
	if ((xinfop->limitMaxFiles != 0) && (ftwip->numFiles >= xinfop->limitMaxFiles)) {
		FTPLogError(cip, kDontPerror, "Max files limit (%u) reached.\n", (unsigned int) ftwip->numFiles);
		return (-1);
	}
	return (0);
}	/* FTPRemoteFtwProc */




int
FTPRemoteRecursiveFileList2(FTPCIPtr cip, char *const rdir, FTPFileInfoListPtr files)
{
	int result;
	char rcwd[512];
	FtwInfo ftwi;
	struct MyFtwExtraInfo xinfo;

	if ((result = FTPGetCWD(cip, rcwd, sizeof(rcwd))) < 0)
		return (result);

	InitFileInfoList(files);

	if (rdir == NULL)
		return (-1);

	if (FTPChdir(cip, rdir) < 0) {
		/* Probably not a directory.
		 * Just add it as a plain file
		 * to the list.
		 */
		(void) ConcatFileToFileInfoList(files, rdir);
		return (kNoErr);
	}

	FtwInit(&ftwi);

	xinfo.limitMaxDirs = 0;
	xinfo.limitMaxFiles = 0;
	xinfo.limitMaxDepth = 50;
	xinfo.fileList = files;
	ftwi.userdata = &xinfo;

	/* Paths collected must be relative. */
	if ((result = FTPFtw(cip, &ftwi, ".", FTPRemoteFtwProc)) != 0) {
		FTPPerror(cip, cip->errNo, kErrCWDFailed, "Could not traverse directory", NULL);
		if ((result = FTPChdir(cip, rcwd)) < 0) {
			rcwd[0] = '\0';
		}
		FtwDispose(&ftwi);
		return (result);
	}
	FtwDispose(&ftwi);

	/* print1(&fil); */
	/* Could sort it to breadth-first here. */
	/* (void) SortRecursiveFileList(&fil); */
	(void) ComputeRNames(files, rdir, 1, 1);

	if ((result = FTPChdir(cip, rcwd)) < 0) {
		rcwd[0] = '\0';
		return (result);
	}
	return (kNoErr);
}	/* FTPRemoteRecursiveFileList2 */




int
FTPRemoteRecursiveFileList1(FTPCIPtr cip, char *const rdir, FTPFileInfoListPtr files)
{
	FTPLineList dirContents;
	FTPFileInfoList fil;
	int result;
	char rcwd[512];

	if ((result = FTPGetCWD(cip, rcwd, sizeof(rcwd))) < 0)
		return (result);

	InitFileInfoList(files);

	if (rdir == NULL)
		return (-1);

	if (FTPChdir(cip, rdir) < 0) {
		/* Probably not a directory.
		 * Just add it as a plain file
		 * to the list.
		 */
		(void) ConcatFileToFileInfoList(files, rdir);
		return (kNoErr);
	}

	/* Paths collected must be relative. */
	if ((result = FTPListToMemory2(cip, "", &dirContents, "-lRa", 1, (int *) 0)) < 0) {
		if ((result = FTPChdir(cip, rcwd)) < 0) {
			rcwd[0] = '\0';
		}
		return (result);
	}

#if 0
DisposeLineListContents(&dirContents);
InitLineList(&dirContents);
AddLine(&dirContents, "drwx------   2 ftpuser  ftpuser       4096 Oct 13 02:12 in");
AddLine(&dirContents, "drwx------   2 ftpuser  ftpuser       4096 Oct 13 02:07 ../out");
AddLine(&dirContents, "drwx------   2 ftpuser  ftpuser       4096 Oct 13 02:11 out2");
AddLine(&dirContents, "drwx------   2 ftpuser  ftpuser       4096 Oct 13 02:11 /usr/tmp/zzzin");
AddLine(&dirContents, "");
AddLine(&dirContents, "./in:");
AddLine(&dirContents, "-rw-r--r--   1 ftpuser  ftpuser      18475 Oct 13 01:58 test_dos.txt");
AddLine(&dirContents, "-rw-r--r--   1 ftpuser  ftpuser      17959 Oct 13 01:57 ../test_mac.txt");
AddLine(&dirContents, "-rw-------   1 ftpuser  ftpuser      17959 Oct 13 01:56 test_unix.txt");
AddLine(&dirContents, "");
AddLine(&dirContents, "./../out:");
AddLine(&dirContents, "-rw-------   1 ftpuser  ftpuser      17969 Oct 13 01:58 test_dos.txt");
AddLine(&dirContents, "-rw-------   1 ftpuser  ftpuser      17959 Oct 13 01:57 test_mac.txt");
AddLine(&dirContents, "-rw-------   1 ftpuser  ftpuser      17959 Oct 13 01:56 test_unix.txt");
AddLine(&dirContents, "");
AddLine(&dirContents, "./out2/../foob:");
/* AddLine(&dirContents, "/tmp/out2:"); */
AddLine(&dirContents, "-rw-------   1 ftpuser  ftpuser      17969 Oct 13 02:08 test_dos.txt");
AddLine(&dirContents, "-rw-------   1 ftpuser  ftpuser      17959 Oct 13 02:08 test_mac.txt");
AddLine(&dirContents, "-rw-------   1 ftpuser  ftpuser      17959 Oct 13 02:08 test_unix.txt");
AddLine(&dirContents, "");
AddLine(&dirContents, "/usr/tmp/zzzin:");
AddLine(&dirContents, "-rw-r--r--   1 ftpuser  ftpuser      18475 Oct 13 01:58 test_dos.txt");
AddLine(&dirContents, "-rw-r--r--   1 ftpuser  ftpuser      17959 Oct 13 01:57 test_mac.txt");
AddLine(&dirContents, "-rw-------   1 ftpuser  ftpuser      17959 Oct 13 01:56 test_unix.txt");
#endif

	/* printls(&dirContents); */
	(void) UnLslR(cip, &fil, &dirContents, cip->serverType);
	DisposeLineListContents(&dirContents);
	/* print1(&fil); */
	/* Could sort it to breadth-first here. */
	/* (void) SortRecursiveFileList(&fil); */
	(void) ComputeRNames(&fil, rdir, 1, 1);
	(void) ConcatFileInfoList(files, &fil);
	DisposeFileInfoListContents(&fil);

	if ((result = FTPChdir(cip, rcwd)) < 0) {
		rcwd[0] = '\0';
		return (result);
	}
	return (kNoErr);
}	/* FTPRemoteRecursiveFileList1 */




int
FTPRemoteRecursiveFileList(FTPCIPtr cip, FTPLineListPtr fileList, FTPFileInfoListPtr files)
{
	FTPLinePtr filePtr, nextFilePtr;
	FTPLineList dirContents;
	FTPFileInfoList fil;
	int result;
	char *rdir;
	char rcwd[512];

	if ((result = FTPGetCWD(cip, rcwd, sizeof(rcwd))) < 0)
		return (result);

	InitFileInfoList(files);

	for (filePtr = fileList->first;
		filePtr != NULL;
		filePtr = nextFilePtr)
	{
		nextFilePtr = filePtr->next;

		rdir = filePtr->line;
		if (rdir == NULL)
			continue;

		if (FTPChdir(cip, rdir) < 0) {
			/* Probably not a directory.
			 * Just add it as a plain file
			 * to the list.
			 */
			(void) ConcatFileToFileInfoList(files, rdir);
			continue;
		}

		/* Paths collected must be relative. */
		if ((result = FTPListToMemory2(cip, "", &dirContents, "-lRa", 1, (int *) 0)) < 0) {
			goto goback;
		}

		(void) UnLslR(cip, &fil, &dirContents, cip->serverType);
		DisposeLineListContents(&dirContents);
		(void) ComputeRNames(&fil, rdir, 1, 1);
		(void) ConcatFileInfoList(files, &fil);
		DisposeFileInfoListContents(&fil);

goback:
		if ((result = FTPChdir(cip, rcwd)) < 0) {
			rcwd[0] = '\0';
			return (result);
		}
	}	
	return (kNoErr);
}	/* FTPRemoteRecursiveFileList */
