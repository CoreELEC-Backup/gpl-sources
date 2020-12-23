/* spoolutil.c
 *
 * Copyright (c) 1992-2016 by Mike Gleason.
 * All rights reserved.
 * 
 */

#include "syshdrs.h"
#ifdef PRAGMA_HDRSTOP
#	pragma hdrstop
#endif

#ifdef HAVE_LONG_FILE_NAMES

#include "spool.h"
#include "util.h"

int gSpoolSerial = 0;

extern FTPLibraryInfo gLib;
extern char gOurDirectoryPath[];
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
extern char gOurInstallationPath[];
#endif
#ifdef ncftp
extern int gUnprocessedJobs;
#endif

int
MkSpoolDir(char *sdir, size_t size)
{
	struct stat st;
	*sdir = '\0';

	/* Don't create in root directory. */
	if (gOurDirectoryPath[0] != '\0') { 
		(void) OurDirectoryPath(sdir, size, kSpoolDir);
		if ((stat(sdir, &st) < 0) && (MkDirs(sdir, 00700) < 0)) {
			perror(sdir);
			return (-1);
		} else {
			return (0);
		}
	}
	return (-1);
}	/* MkSpoolDir */




void
SpoolName(char *const sp, const size_t size, const int flag, const int serial, time_t when)
{
	char dstr[32];
	struct tm lt;

	if ((when == (time_t) 0) || (when == (time_t) -1))
		(void) time(&when);
	if (Gmtime(when, &lt) == NULL) {
		/* impossible */
		(void) Strncpy(dstr, "20010101-000000", size);
	} else {
		(void) strftime(dstr, sizeof(dstr), "%Y%m%d-%H%M%S", &lt);
	}
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
	(void) sprintf(sp, "%c-%s-%08X-%d",
		flag,
		dstr,
		(unsigned int) getpid(),
		serial
	);
#else
#	ifdef HAVE_SNPRINTF
	(void) snprintf(sp, size - 1,
                    "%c-%s-%06d-%d",
                    flag,
                    dstr,
                    (unsigned int) getpid(),
                    serial
                    );
#	else
	(void) sprintf(sp,
                   "%c-%s-%06d-%d",
                   flag,
                   dstr,
                   (unsigned int) getpid(),
                   serial
                   );
#	endif
#endif
}	/* SpoolName */




static int
WriteSpoolLine(FILE *const ofp, const char *const line)
{
	int c;
	const char *cp;

	c = 0;
	for (cp = line; *cp; cp++) {
		c = (int) *cp;
		if (c == '\r')
			continue;
		if ((c == '\n') && (cp[1] != '\0')) {
			if (putc('\\', ofp) == EOF)
				return (-1);
		}
		if (putc(c, ofp) == EOF)
			return (-1);
	}
	if (c != '\n') {
		c = '\n';
		if (putc(c, ofp) == EOF)
			return (-1);
	}
	return (0);
}	/* WriteSpoolLine */



char *
SpoolFilePath(
	char *const dst,
	size_t dsize,
	const char *sdir,
	const char *const sname
)
{
	char sdir2[256];

	if (dst == NULL)
		return NULL;

	memset(dst, 0, dsize);
	if ((sname == NULL) || (sname[0] == '\0'))
		return NULL;

	if (sdir == NULL) {
		if (MkSpoolDir(sdir2, sizeof(sdir2)) < 0)
			return NULL;
		sdir = sdir2;
	}

	Path(dst, dsize, sdir, sname);
	return (dst);
}	/* SpoolFilePath */




FILE *
OpenSpoolFile(
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
)
{
	FILE *fp = NULL;
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
#else
	mode_t um;
#endif

	if (op == NULL)
		return NULL;

	for (;;) {
		memset(initialpath, 0, ipsize);
		memset(finalpath, 0, fpsize);
		memset(sname, 0, snsize);

		++gSpoolSerial;

		SpoolName(sname, snsize, 'z', gSpoolSerial, when);
		if (SpoolFilePath(initialpath, ipsize, sdir, sname) == NULL) return NULL;
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
		if ((fp = _fsopen(finalpath, FOPEN_READ_TEXT, _SH_DENYNO)) != NULL)
#else
		if ((fp = fopen(finalpath, "r")) != NULL)
#endif
		{
			/* File existed already! Choose a different one. */
			(void) fclose(fp);
			fp = NULL;
			continue;
		}

		SpoolName(sname, snsize, op[0], gSpoolSerial, when);
		if (SpoolFilePath(finalpath, fpsize, sdir, sname) == NULL) return NULL;
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
		if ((fp = _fsopen(finalpath, FOPEN_READ_TEXT, _SH_DENYNO)) != NULL)
#else
		if ((fp = fopen(finalpath, "r")) != NULL)
#endif
		{
			/* File existed already! Choose a different one. */
			(void) fclose(fp);
			fp = NULL;
			continue;
		}

		/* Neither file existed. Done. */
		break;
	}

	if (ofp != NULL)
		return (ofp);

#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
	fp = fopen(initialpath, FOPEN_WRITE_TEXT);
#else
	/* Make sure file is private */
	um = umask(077);
	fp = fopen(initialpath, FOPEN_WRITE_TEXT);
	(void) umask(um);
#endif

	return (fp);
}	/* OpenSpoolFile */



int
WriteSpoolEntry(
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
	)
{
	char pass[160];
	char lpathname[1024];
	char ldir2[256];
	char *ldir3;
	struct Stat st;

	if ((op != NULL) && (op[0] == 'p') && ((lfilesize <= 0) || (lmtime == 0))) {
		/* Also include the size and time of the file when the
		 * item was first spooled, if not already supplied.
		 */
		Path(lpathname, sizeof(lpathname), ldir, lfile);
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
		if (WinStat64(lpathname, &st) == 0) {
#else
		if (lstat(lpathname, &st) == 0) {
#endif
			if (lfilesize <= 0)
				lfilesize = (double) st.st_size;
			if (lmtime == 0)
				lmtime = st.st_mtime;
		}
	}

	/* This starting comment line is now important.
	 * It will serve as the delineator between multiple entries in the same transaction file.
	 */
	if (fprintf(fp, "%s\n", kSpoolEntryBeginLine) < 0)
		goto err;

	if (fprintf(fp, "job-name=%s\n", jobname) < 0)
		goto err;
	if (fprintf(fp, "op=%s\n", op) < 0)
		goto err;
	if ((delaySinceLastFailure != 0) && (fprintf(fp, "delay-since-last-failure=%u\n", delaySinceLastFailure) < 0))
		goto err;
	if ((timeOfFirstAttempt != 0) && (fprintf(fp, "time-of-first-attempt=%lu\n", (unsigned long) timeOfFirstAttempt) < 0))
		goto err;
	if ((timeOfLastFailure != 0) && (fprintf(fp, "time-of-last-failure=%lu\n", (unsigned long) timeOfLastFailure) < 0))
		goto err;
	if (fprintf(fp, "hostname=%s\n", host) < 0)
		goto err;
	if ((ip != NULL) && (ip[0] != '\0') && (fprintf(fp, "host-ip=%s\n", ip) < 0))
		goto err;
	if ((port != 0) && (port != (unsigned int) kDefaultFTPPort) && (fprintf(fp, "port=%u\n", port) < 0))
		goto err;
	if ((user != NULL) && (user[0] != '\0') && (strcmp(user, "anonymous") != 0) && (fprintf(fp, "username=%s\n", user) < 0))
		goto err;
	if ((strcmp(user, "anonymous") != 0) && (passclear != NULL) && (passclear[0] != '\0')) {
		(void) memcpy(pass, kPasswordMagic, kPasswordMagicLen);
		ToBase64(pass + kPasswordMagicLen, passclear, strlen(passclear), 1);
		if (fprintf(fp, "password=%s\n", pass) < 0)
			goto err;
	} else if ((strcmp(user, "anonymous") == 0) && (gLib.defaultAnonPassword[0] != '\0')) {
		if (fprintf(fp, "password=%s\n", gLib.defaultAnonPassword) < 0)
			goto err;
	}
	if ((xacct != NULL) && (xacct[0] != '\0') && (fprintf(fp, "acct=%s\n", xacct) < 0))
		goto err;
	if (fprintf(fp, "xtype=%c\n", xtype) < 0)
		goto err;
	if ((recursive != 0) && (fprintf(fp, "recursive=%s\n", YESNO(recursive)) < 0))
		goto err;
	if ((deleteflag != 0) && (fprintf(fp, "delete=%s\n", YESNO(deleteflag)) < 0))
		goto err;
	if (fprintf(fp, "passive=%d\n", passive) < 0)
		goto err;
	if (fprintf(fp, "remote-dir=%s\n", rdir) < 0)
		goto err;
	if ((ldir == NULL) || (ldir[0] == '\0') || (strcmp(ldir, ".") == 0)) {
		/* Use current process' working directory. */
		FTPGetLocalCWD(ldir2, sizeof(ldir2));
		if (fprintf(fp, "local-dir=%s\n", ldir2) < 0)
			goto err;
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
	} else if ((ldir[0] != '/') && (ldir[0] != '\\')) {
		FTPGetLocalCWD(ldir2, sizeof(ldir2));
		if (DPathCat(&ldir3, ldir2, ldir, 1) == 0) {
			if (fprintf(fp, "local-dir=%s\n", ldir3) < 0)
				goto err;
			free(ldir3);
		}
#else
	} else if (ldir[0] != '/') {
		FTPGetLocalCWD(ldir2, sizeof(ldir2));
		if (DPathCat(&ldir3, ldir2, ldir, 0) == 0) {
			if (fprintf(fp, "local-dir=%s\n", ldir3) < 0)
				goto err;
			free(ldir3);
		}
#endif
	} else {
		if (fprintf(fp, "local-dir=%s\n", ldir) < 0)
			goto err;
	}
	if (fprintf(fp, "remote-file=%s\n", rfile) < 0)
		goto err;
	if ((rfilesize > 0) && (fprintf(fp, "remote-file-size=%.0f\n", rfilesize) < 0))
		goto err;
	if ((rmtime > 0) && (fprintf(fp, "remote-file-mtime=%lu\n", (unsigned long) rmtime) < 0))
		goto err;
	if (fprintf(fp, "local-file=%s\n", lfile) < 0)
		goto err;
	if ((lfilesize > 0) && (fprintf(fp, "local-file-size=%.0f\n", lfilesize) < 0))
		goto err;
	if ((lmtime > 0) && (fprintf(fp, "local-file-mtime=%lu\n", (unsigned long) lmtime) < 0))
		goto err;
	if ((lnewdir != NULL) && (lnewdir[0] != '\0')) {
		if (fprintf(fp, "local-rename-dir=%s\n", lnewdir) < 0)
			goto err;
	}
	if ((lnewfile != NULL) && (lnewfile[0] != '\0')) {
		if (fprintf(fp, "local-rename-file=%s\n", lnewfile) < 0)
			goto err;
	}
	if ((rnewdir != NULL) && (rnewdir[0] != '\0')) {
		if (fprintf(fp, "remote-rename-dir=%s\n", rnewdir) < 0)
			goto err;
	}
	if ((rnewfile != NULL) && (rnewfile[0] != '\0')) {
		if (fprintf(fp, "remote-rename-file=%s\n", rnewfile) < 0)
			goto err;
	}
	if ((preferredLocalAddrStr != NULL) && (preferredLocalAddrStr[0] != '\0')) {
		if (fprintf(fp, "source-address=%s\n", preferredLocalAddrStr) < 0)
			goto err;
	}
	if ((manualOverrideFeatures != NULL) && (manualOverrideFeatures[0] != '\0')) {
		if (fprintf(fp, "manual-override-features=") < 0)
			goto err;
		if (WriteSpoolLine(fp, manualOverrideFeatures) < 0)
			goto err;
	}
	if ((preftpcmd != NULL) && (preftpcmd[0] != '\0')) {
		if (fprintf(fp, "pre-ftp-command=") < 0)
			goto err;
		if (WriteSpoolLine(fp, preftpcmd) < 0)
			goto err;
	}
	if ((perfileftpcmd != NULL) && (perfileftpcmd[0] != '\0')) {
		if (fprintf(fp, "per-file-ftp-command=") < 0)
			goto err;
		if (WriteSpoolLine(fp, perfileftpcmd) < 0)
			goto err;
	}
	if ((postftpcmd != NULL) && (postftpcmd[0] != '\0')) {
		if (fprintf(fp, "post-ftp-command=") < 0)
			goto err;
		if (WriteSpoolLine(fp, postftpcmd) < 0)
			goto err;
	}
	if ((preshellcmd != NULL) && (preshellcmd[0] != '\0')) {
		if (fprintf(fp, "pre-shell-command=") < 0)
			goto err;
		if (WriteSpoolLine(fp, preshellcmd) < 0)
			goto err;
	}
	if ((postshellcmd != NULL) && (postshellcmd[0] != '\0')) {
		if (fprintf(fp, "post-shell-command=") < 0)
			goto err;
		if (WriteSpoolLine(fp, postshellcmd) < 0)
			goto err;
	}

	if (fflush(fp) < 0)
		goto err;
	return (0);

err:
	return (-1);
}	/* WriteSpoolEntry */





int
CloseSpoolFileAndRename(
	FILE *const fp,
	FILE *const efp,
	char *const initialpath,
	char *const finalpath
)
{
	int closed = 0, renamed = 0;

	if (fp != NULL) {
		if (fclose(fp) < 0) {
			if (efp != NULL)
				fprintf(efp, "write to spoolfile (%s) failed: %s\n", initialpath, strerror(errno));
		} else {
			closed = 1;
		}
	}

	/* Move the spool file into its "live" name. */
	if ((initialpath != NULL) && (initialpath[0] != '\0') && (finalpath != NULL) && (finalpath[0] != '\0')) {
		if (rename(initialpath, finalpath) < 0) {
			if (efp != NULL)
				fprintf(efp, "rename spoolfile failed (%s --> %s): %s\n", initialpath, finalpath, strerror(errno));
			(void) unlink(initialpath);
		} else {
#ifdef ncftp
			gUnprocessedJobs++;
#endif
			renamed = 1;
		}
	}
	return ((renamed && closed) ? 0 : -1);
}	/* CloseSpoolFileAndRename */




int
SpoolX(
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
	const int UNUSED(reserved)
	)
{
	char sname[64];
	char initialpath[256];
	char finalpath[256];
	FILE *fp;

	LIBNCFTP_USE_VAR(reserved);

	fp = OpenSpoolFile(ofp, initialpath, sizeof(initialpath), finalpath, sizeof(finalpath), sname, sizeof(sname), sdir, op, when);
	if (fp == NULL) {
		if (efp != NULL)
			fprintf(efp, "Could not open spool file for writing: %s\n", strerror(errno));
		return (-1);
	}

	if (WriteSpoolEntry(
		fp,
		op,
		sname,
		rfile,
		rdir,
		rfilesize,
		rmtime,
		lfile,
		ldir,
		lfilesize,
		lmtime,
		rnewfile,
		rnewdir,
		lnewfile,
		lnewdir,
		host,
		ip,
		port,
		user,
		passclear,
		xacct,
		xtype,
		recursive,
		deleteflag,
		passive,
		preftpcmd,
		perfileftpcmd,
		postftpcmd,
		preshellcmd,
		postshellcmd,
		delaySinceLastFailure,
		manualOverrideFeatures,
		preferredLocalAddrStr,
		timeOfFirstAttempt,
		timeOfLastFailure
	) < 0) {
		if (efp != NULL)
			fprintf(efp, "write to spoolfile (%s) failed: %s\n", initialpath, strerror(errno));
		return (-1);
	}

	if ((fp != ofp) && (CloseSpoolFileAndRename(fp, efp, initialpath, finalpath) < 0))
		return (-1);

	return (0);
}	/* SpoolX */




void
RunBatch(void)
{
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
	char ncftpbatch[260];
	const char *prog;
	int winExecResult;

	if (gOurInstallationPath[0] == '\0') {
		(void) fprintf(stderr, "Cannot find path to %s.  Please re-run Setup.\n", "ncftpbatch.exe");
		return;
	}
	prog = ncftpbatch;
	OurInstallationPath(ncftpbatch, sizeof(ncftpbatch), "ncftpbatch.exe");
	
	winExecResult = WinExec(prog, SW_SHOWNORMAL);
	if (winExecResult <= 31) switch (winExecResult) {
		case ERROR_BAD_FORMAT:
			fprintf(stderr, "Could not run %s: %s\n", prog, "The .EXE file is invalid");
			return;
		case ERROR_FILE_NOT_FOUND:
			fprintf(stderr, "Could not run %s: %s\n", prog, "The specified file was not found.");
			return;
		case ERROR_PATH_NOT_FOUND:
			fprintf(stderr, "Could not run %s: %s\n", prog, "The specified path was not found.");
			return;
		default:
			fprintf(stderr, "Could not run %s: Unknown error #%d.\n", prog, winExecResult);
			return;
	}
#else
	char *argv[8];
	pid_t pid = 0;
#if (defined(BINDIR) || defined(PREFIX_BINDIR))
	char ncftpbatch[256];

#ifdef BINDIR
	STRNCPY(ncftpbatch, BINDIR);
#else
	STRNCPY(ncftpbatch, PREFIX_BINDIR);
#endif
	STRNCAT(ncftpbatch, "/");
	STRNCAT(ncftpbatch, "ncftpbatch");
#endif	/* BINDIR */

	pid = fork();
	if (pid < 0) {
		perror("fork");
	} else if (pid == 0) {
		argv[0] = strdup("ncftpbatch");
		argv[1] = strdup("-d");
		argv[2] = NULL;
#if (defined(BINDIR) || defined(PREFIX_BINDIR))
		(void) execv(ncftpbatch, argv);
		(void) fprintf(stderr, "Could not run %s.  Is it in installed as %s?\n", argv[0], ncftpbatch);
#else	/* BINDIR */
		(void) execvp(argv[0], argv);
		(void) fprintf(stderr, "Could not run %s.  Is it in your $PATH?\n", argv[0]);
#endif	/* BINDIR */
		perror(argv[0]);
		exit(1);
	}

	if (pid > 1) {
#ifdef HAVE_WAITPID
		(void) waitpid(pid, NULL, 0);
#else
		(void) wait(NULL);
#endif	/* HAVE_WAITPID */
	}
#endif	/* UNIX */
}	/* RunBatch */

#else	/* ! HAVE_LONG_FILE_NAMES */

int
SpoolX(
	FILE *const ofp,
	const char *sdir,
	const char *const op,
	const char *const rfile,
	const char *const rdir,
	const char *const lfile,
	const char *const ldir,
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
	const unsigned int delaySinceLastFailure)
{
	return (-1);
}

void
RunBatch(void)
{
	fprintf(stderr, "Background processing not available on this platform.\n");
}	/* RunBatch */

#endif	/* HAVE_LONG_FILE_NAMES */
