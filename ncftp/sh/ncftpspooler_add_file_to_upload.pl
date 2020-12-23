#!/usr/bin/perl -w
#
# $Id: ncftpspooler_add_file_to_upload.pl 1124 2016-09-15 21:42:06Z mgleason $
#
use strict;
use Getopt::Long;
use File::Basename;
use Date::Parse;	# http://search.cpan.org/~gbarr/TimeDate-2.30/lib/Date/Parse.pm
use POSIX;
use Cwd 'abs_path';

my ($gDebug) 					= 0;

my ($gProgress)					= -1;
my ($gStartTime)				= time();
my ($gLastProgressReportTime)			= 0;
my (@gIncludePatterns)				= ();
my (@gExcludePatterns)				= ();
my ($gScreenColumns) 				= $ENV{"COLUMNS"} || 80;
my ($gMayExcludeDirectories)			= -1;
my ($gMaxDepth)					= -1;
my ($gOneDev) 					= 0;
my ($newer_t)					= 0;
my ($older_t)					= 0;
my ($gMinSize)					= -1;
my ($gMaxSize)					= -1;
my ($gSort)					= "";

my ($dryrun)					= 0;
my ($nprocessed)				= 0;
my ($njobsqueued)				= 0;
my ($tbytesqueued)				= 0;
my ($jobname) 					= "";
my ($jobpath) 					= "";
my ($jobpathtmp) 				= "";
my ($jobdata) 					= "";
my ($jobdate) 					= "";
my ($jobtime) 					= "";
my ($njobfiles)					= 0;
my ($njobsperjobfile)				= 0;
my ($nbytesperjobfile)				= 0;
my ($maxjobsperjobfile)				= undef;
my ($maxbytesperjobfile)			= undef;
my ($create_new_job_file)			= 1;
my ($Mbps_Est) 					= 20;
my ($pid) 					= POSIX::getpid();
my ($seqnum) 					= 0;
my ($lroot) 					= "";
my ($rroot) 					= "";
my ($job_scheduled_time)			= time();
my ($queue_directory)				= "/var/spool/ncftp";
my (%vars)					= (
	"rhost" => "",
	"ruser" => "",
	"rpassword" => "",
	"test" => 1
);




sub AbbrevStr
{
	my ($s) = $_[0];
	my ($maxlen) = $_[1];
	my ($len, $p);

	if (($len = length($s)) > $maxlen) {
		$p = int($maxlen * 1 / 3);
		return (
			substr($s, 0, $p) .
			"..." .
			substr($s, $len - ($maxlen - $p) + 3)
		);
	}
	return ($s);
}	# AbbrevStr




sub ByteUnits
{
	my ($bytes) = $_[0];
	my ($doprint) = 0;
	$doprint = $_[1] if (scalar(@_) >= 2);

	my (@ulist) = ("k", "M", "G", "T", "P", "E", "Z", "Y");

	if ($bytes =~ /^\s*(\d+(\.\d+)?)\s*$/i) {
		$bytes = $1;
	} elsif ($bytes =~ /^\s*(\d+(\.\d+)?)\s*(ki)/i) {
		$bytes = $1 * 1024;
	} elsif ($bytes =~ /^\s*(\d+(\.\d+)?)\s*(Mi)/i) {
		$bytes = $1 * 1024 * 1024;
	} elsif ($bytes =~ /^\s*(\d+(\.\d+)?)\s*(Gi)/i) {
		$bytes = $1 * 1024 * 1024 * 1024;
	} elsif ($bytes =~ /^\s*(\d+(\.\d+)?)\s*(Ti)/i) {
		$bytes = $1 * 1024 * 1024 * 1024 * 1024;
	} elsif ($bytes =~ /^\s*(\d+(\.\d+)?)\s*(Pi)/i) {
		$bytes = $1 * 1024 * 1024 * 1024 * 1024 * 1024;
	} elsif ($bytes =~ /^\s*(\d+(\.\d+)?)\s*(Ei)/i) {
		$bytes = $1 * 1024 * 1024 * 1024 * 1024 * 1024 * 1024;
	} elsif ($bytes =~ /^\s*(\d+(\.\d+)?)\s*(Zi)/i) {
		$bytes = $1 * 1024 * 1024 * 1024 * 1024 * 1024 * 1024 * 1024;
	} elsif ($bytes =~ /^\s*(\d+(\.\d+)?)\s*(k)/i) {
		$bytes = $1 * 1000;
	} elsif ($bytes =~ /^\s*(\d+(\.\d+)?)\s*(M)/i) {
		$bytes = $1 * 1000 * 1000;
	} elsif ($bytes =~ /^\s*(\d+(\.\d+)?)\s*(G)/i) {
		$bytes = $1 * 1000 * 1000 * 1000;
	} elsif ($bytes =~ /^\s*(\d+(\.\d+)?)\s*(T)/i) {
		$bytes = $1 * 1000 * 1000 * 1000 * 1000;
	} elsif ($bytes =~ /^\s*(\d+(\.\d+)?)\s*(P)/i) {
		$bytes = $1 * 1000 * 1000 * 1000 * 1000 * 1000;
	} elsif ($bytes =~ /^\s*(\d+(\.\d+)?)\s*(E)/i) {
		$bytes = $1 * 1000 * 1000 * 1000 * 1000 * 1000 * 1000;
	} elsif ($bytes =~ /^\s*(\d+(\.\d+)?)\s*(Z)/i) {
		$bytes = $1 * 1000 * 1000 * 1000 * 1000 * 1000 * 1000 * 1000;
	} elsif ($bytes =~ /^\s*(\d+(\.\d+)?)\s*(\S+)/i) {
		die "Unrecognized unit, $3\n";
	}

	my ($ni) = $bytes;
	my ($n) = $bytes;
	my ($u) = "";

	my ($rni) = $ni;
	my ($rn) = $n;
	my ($ru) = $u;

	for (my $i = 0; $i < scalar(@ulist); $i++) {
		last if ($n < 1000);
		printf ("%.0f bytes = %.3f %sB = %.3f %siB\n", $bytes, $n, $u, $ni, $u) if ($doprint);
		$n *= 0.001;
		$ni /= 1024.0;
		$u = $ulist[$i];
		$rni = $ni;
		$rn = $n;
		$ru = $u;
	}

	return ($bytes, $rni, $rn, $ru);
}	# ByteUnits




sub ProgressMessage
{
	my ($s) = $_[0];
	my ($lastmsg) = $_[1];
	my ($spec) = sprintf("%%-%us", $gScreenColumns);

	if ($lastmsg) {
		printf STDERR ("\r%s\n", sprintf($spec, sprintf("%s", AbbrevStr($s, $gScreenColumns - 0))));
	} else {
		printf STDERR ("\r%s", sprintf($spec, sprintf("%s... ", AbbrevStr($s, $gScreenColumns- 4))));
	}
}	# ProgressMessage




sub CreateQueueDirectoryOrDie
{
	if ((! defined($queue_directory)) || ($queue_directory eq "") || ($queue_directory eq ".")) {
		die "Invalid queue directory.\n";
	}
	if ((-e $queue_directory) && (! -d $queue_directory)) {
		die "$queue_directory is not a directory.\n";
	}
	if (! -e $queue_directory) {
		if (! mkdir($queue_directory, 00777)) {
			die "Could not create queue directory, $queue_directory.\n";
		}
		if (! -d $queue_directory) {
			die "Missing queue directory, $queue_directory.\n";
		}
		warn "Created $queue_directory.\n";
	}
}	# CreateQueueDirectoryOrDie




sub CloseJobFile
{
	if ($jobpathtmp ne "") {
		$create_new_job_file = 1;
		if (! $dryrun) {
			if (! rename($jobpathtmp, $jobpath)) {
				die("Could not rename $jobpathtmp to $jobpath: $!\n");
			}
		}
		printf STDOUT ("# Closed %s (%d jobs).\n", $jobpathtmp, $njobsperjobfile) if (($njobsperjobfile > 1) && ($gDebug >= 2));
		$njobsperjobfile = 0;
		$nbytesperjobfile = 0;
		$njobfiles++;
	}
	$jobpathtmp = "";
}	# CloseJobFile




sub EnqueueItem
{
	my ($lpathname, $short_ftype, $lmtstr, $lsize) = @_;

	return if ((! defined($lpathname)) || ($lpathname eq ""));

	$nprocessed++;
	if (! -f $lpathname) {
		warn "Missing local file to send, $lpathname.\n";
		return;
	}

	my ($op) = "put";
	my ($o) = substr($op, 0, 1);

	my ($sep) = "";
	my ($local_dir) = dirname($lpathname);
	my ($lrelname) = $lpathname;
	if ($lroot ne "") {
		$lroot =~ s/\/+$//;
		if ($lroot ne "") {
			$lrelname =~ s/^$lroot\/*//;
		}
	}

	my ($subdir) = dirname($lrelname);
	my ($lfilename) = basename($lrelname);
	my ($rfilename) = $lfilename;

	return if ($lfilename eq ".DS_Store");

	if ($rroot eq "") {
		$rroot = $vars{"rroot"} || ".";
	}

	my ($remote_dir) = $rroot;
	if ($subdir ne "") {
		$subdir =~ s/^\.\///;
		if (($subdir ne ".") && ($subdir ne "")) {
			$remote_dir .= "/" . $subdir;
		}
	}
	$remote_dir =~ s/^\.\///;

	if ($jobdate eq "") {
		$jobdate = POSIX::strftime("%Y%m%d", gmtime($job_scheduled_time));
		$jobtime = POSIX::strftime("%H%M%S", gmtime($job_scheduled_time));
	}

	CreateQueueDirectoryOrDie();

	if ($create_new_job_file) {
		do {
			$seqnum++;
			$jobname = sprintf("%s-%s-%s-%s-%09d", $o, $jobdate, $jobtime, $pid, $seqnum);
			$jobpath = "$queue_directory/$jobname.txt";
			$jobpathtmp = sprintf("%s/%s-%s-%s-%s-%09d", $queue_directory, "x", $jobdate, $jobtime, $pid, $seqnum);
		} while ((-e $jobpath) || (-e $jobpathtmp));
		$njobsperjobfile = 0;
		$nbytesperjobfile = 0;
		$create_new_job_file = 0;
	} else {
		$sep = "\n\n\n";
		$seqnum++;
		$jobname = sprintf("%s-%s-%s-%s-%09d", $o, $jobdate, $jobtime, $pid, $seqnum);
	}

	my ($rpassword_comment) = "";
	$rpassword_comment = "# " if ($vars{"rpassword"} eq "");

	$jobdata = <<EOF;
$sep#BEGIN# This NcFTP spool file entry was generated by $0.
job-name=$jobname
op=$op
hostname=$vars{"rhost"}
username=$vars{"ruser"}
${rpassword_comment}password=$vars{"rpassword"}
xtype=I
remote-dir=$remote_dir
local-dir=$local_dir
remote-file=$rfilename
local-file=$lfilename
EOF
	if ((exists($vars{"passive"})) && (defined($vars{"passive"})) && ($vars{"passive"} ne "")) {
		$jobdata .= "passive=" . $vars{"passive"} . "\n";
	}
	if ((exists($vars{"recursive"})) && (defined($vars{"recursive"})) && ($vars{"recursive"} ne "")) {
		$jobdata .= "recursive=" . $vars{"recursive"} . "\n";
	}
	if ((exists($vars{"delete"})) && (defined($vars{"delete"})) && ($vars{"delete"} ne "")) {
		$jobdata .= "delete=" . $vars{"delete"} . "\n";
	}
	if ((exists($vars{"source-address"})) && (defined($vars{"source-address"})) && ($vars{"source-address"} ne "")) {
		$jobdata .= "source-address=" . $vars{"source-address"} . "\n";
	}
	if ((exists($vars{"pre-ftp-command"})) && (defined($vars{"pre-ftp-command"})) && ($vars{"pre-ftp-command"} ne "")) {
		$jobdata .= "pre-ftp-command=" . $vars{"pre-ftp-command"} . "\n";
	}
	if ((exists($vars{"per-file-ftp-command"})) && (defined($vars{"per-file-ftp-command"})) && ($vars{"per-file-ftp-command"} ne "")) {
		$jobdata .= "per-file-ftp-command=" . $vars{"per-file-ftp-command"} . "\n";
	}
	if ((exists($vars{"post-ftp-command"})) && (defined($vars{"post-ftp-command"})) && ($vars{"post-ftp-command"} ne "")) {
		$jobdata .= "post-ftp-command=" . $vars{"post-ftp-command"} . "\n";
	}
	if ((exists($vars{"pre-shell-command"})) && (defined($vars{"pre-shell-command"})) && ($vars{"pre-shell-command"} ne "")) {
		$jobdata .= "pre-shell-command=" . $vars{"pre-shell-command"} . "\n";
	}
	if ((exists($vars{"post-shell-command"})) && (defined($vars{"post-shell-command"})) && ($vars{"post-shell-command"} ne "")) {
		$jobdata .= "post-shell-command=" . $vars{"post-shell-command"} . "\n";
	}
	if ((exists($vars{"manual-override-features"})) && (defined($vars{"manual-override-features"} )) && ($vars{"manual-override-features"} ne "")) {
		$jobdata .= "manual-override-features=" . $vars{"manual-override-features"} . "\n";
	}
	if ((exists($vars{"remote-rename"})) && (defined($vars{"remote-rename"})) && ($vars{"remote-rename"} ne "")) {
		my ($newrelpath) = $vars{"remote-rename"};
		my ($d) = dirname($newrelpath);
		my ($b) = basename($newrelpath);
		if (($d eq ".") || ($d eq "")) {
			$jobdata .= "remote-rename-file=" . $b . "\n";
		} else {
			$jobdata .= "remote-rename-dir=" . $d . "\n";
			$jobdata .= "remote-rename-file=" . $b . "\n";
		}
	}
	if ((exists($vars{"local-rename"})) && (defined($vars{"local-rename"})) && ($vars{"local-rename"} ne "")) {
		my ($newrelpath) = $vars{"local-rename"};
		my ($d) = dirname($newrelpath);
		my ($b) = basename($newrelpath);
		if (($d eq ".") || ($d eq "")) {
			$jobdata .= "local-rename-file=" . $b . "\n";
		} else {
			$jobdata .= "local-rename-dir=" . $d . "\n";
			$jobdata .= "local-rename-file=" . $b . "\n";
		}
	}
	
	$jobdata .= "local-file-size=$lsize\n" if ($lsize ne "");
	$jobdata .= "local-mtime=$lmtstr\n" if ($lmtstr ne "");

	if (! $dryrun) {
		CreateQueueDirectoryOrDie();
		if (! open(JOBFILE, ">> $jobpathtmp")) {
			die("Could not open $jobpathtmp for writing: $!\n");
		}

		print JOBFILE $jobdata or die "Could not write jobdata to $jobpathtmp: $!\n";
		close(JOBFILE);
	}

	$njobsperjobfile++;
	$nbytesperjobfile += $lsize if ($lsize ne "");
	if (
		((! defined($maxjobsperjobfile)) && (! defined($maxbytesperjobfile))) ||
		((defined($maxjobsperjobfile)) && ($njobsperjobfile >= $maxjobsperjobfile)) || 
		((defined($maxbytesperjobfile)) && ($maxbytesperjobfile >= 0) && ($nbytesperjobfile >= $maxbytesperjobfile))
	) {
		CloseJobFile();
	}

	$njobsqueued++;
	if ($gDebug >= 2) {
		printf STDOUT ("# Queued job #%d to %s = {\n%s}\n\n", $njobsqueued, $jobpath, $jobdata);
	} elsif ($gDebug >= 1) {
		printf STDOUT ("> %s %-12d $jobname: %s\n", $lmtstr, $lsize, $lrelname);
	} else {
	}
}	# EnqueueItem




sub FtwProcessItem
{
	my ($pathname, $ftype, $dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = @_;

	if ($pathname =~ /^\.\/(.+)$/) {
		# Strip leading ./ from relative paths
		$pathname = $1;
	}

	if ($gProgress) {
		# Print a gProgress report every 2 seconds
		my ($now) = time();
		if ($now > $gLastProgressReportTime + 1) {
			ProgressMessage($pathname, 0);
			$gLastProgressReportTime = $now;
		}
	}

	my ($tstr, $short_ftype);
	if ($ftype eq "file") {
		$short_ftype = "-";
		my ($qit) = 1;
		if (($newer_t != 0) && ($mtime < $newer_t)) { $qit = 0; }
		if (($older_t != 0) && ($mtime >= $older_t)) { $qit = 0; }
		if (($gMaxSize > 0) && ($size > $gMaxSize)) {
			printf STDERR ("Warning: skipping \"%s\", which is too large (%.0f > %.0f).\n", $pathname, $size, $gMaxSize);
			$qit = 0;
		}
		if (($gMinSize > 0) && ($size < $gMinSize)) {
			printf STDERR ("Warning: skipping \"%s\", which is too small (%.0f < %.0f).\n", $pathname, $size, $gMinSize);
			$qit = 0;
		}
		if ($qit) {
			$tstr = strftime("%Y-%m-%d %H:%M:%S", localtime($mtime));
			EnqueueItem($pathname, $short_ftype, $mtime, $size);
			$tbytesqueued += $size;
		}
	} elsif ($ftype eq "symlink") {
		$short_ftype = "l";
	} elsif ($ftype eq "directory") {
		$short_ftype = "d";
	} else {
		$short_ftype = "?";
	}

	if ($gDebug >= 3) {
		$tstr = strftime("%Y-%m-%d  %H:%M:%S", localtime($mtime));
		printf(">>> %s  %s  %-12d %s\n", $short_ftype, $tstr, $size, $pathname);
	}
	return (0);	# no error
}	# FtwProcessItem




sub FtwIsExcluded
{
	my ($pathname) = $_[0];
	my ($pattern);
	my ($exclude) = 0;

	return 0 if ((scalar(@gExcludePatterns) == 0) && (scalar(@gIncludePatterns) == 0));

	$exclude = 1 if (scalar(@gExcludePatterns) == 0);
	for $pattern (@gExcludePatterns) {
		if ($pathname =~ /$pattern/i) {
			$exclude = 1;
			last;
		}
	}

	if ($exclude) {
		for $pattern (@gIncludePatterns) {
			if ($pathname =~ /$pattern/i) {
				$exclude = 0;
				last;
			}
		}
	}

	return ($exclude);
}	# FtwIsExcluded




sub FtwProcessDirectory
{
	my ($directory, $curdepth, $maxdepth, $onedev, $rootdev) = @_;
	my ($filename, $pathname, $subdir);
	my ($ftype);
	my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks);
	my ($d_dev,$d_ino,$d_mode,$d_nlink,$d_uid,$d_gid,$d_rdev,$d_size,$d_atime,$d_mtime,$d_ctime,$d_blksize,$d_blocks);
	my ($nitems);
	my ($rc);
	my (@subdirs) = ();
	my (@filenames) = ();
	my (@mtimes) = ();
	my (@mdata) = ();

	if (! opendir(DIR, $directory)) {
		# Maybe it was removed/renamed before we could get to it.
		return (0);
	}

	($d_dev,$d_ino,$d_mode,$d_nlink,$d_uid,$d_gid,$d_rdev,$d_size,$d_atime,$d_mtime,$d_ctime,$d_blksize,$d_blocks) = stat($directory);
	if (! defined($d_dev)) {
		# Race condition?
		closedir(DIR);
		return (0);
	}
	if (! defined($rootdev)) {
		$rootdev = $d_dev;
	}

	if (($rc = FtwProcessItem($directory, "directory", $d_dev,$d_ino,$d_mode,$d_nlink,$d_uid,$d_gid,$d_rdev,$d_size,$d_atime,$d_mtime,$d_ctime,$d_blksize,$d_blocks)) != 0) {
		# Proc said to abort.
		closedir(DIR);
		return ($rc);
	}

	if (($maxdepth >= 0) && ($curdepth > $maxdepth)) {
		closedir(DIR);
		return (0);
	}

	$directory = "" if ($directory eq "/");
	$nitems = 0;
	my ($f_i) = 0;
	my (@f_is) = ();
	while (defined($filename = readdir(DIR))) {
		next if ($filename eq ".");
		next if ($filename eq "..");
		$nitems++;
		$pathname = $directory . "/" . $filename;
		($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = lstat($pathname);
		if (! defined($dev)) {
			# Maybe it was removed/renamed
			next;
		}

		if (-l _) {
			$ftype = "symlink";
		} elsif (-f _) {
			$ftype = "file";
		} elsif (-d _) {
			$ftype = "directory";
			if ($onedev) {
				$dev = (stat(_))[0];
				push(@subdirs, $filename) if ($dev == $rootdev);
			} else {
				push(@subdirs, $filename);
			}
		} else {
			$ftype = "other";
		}

		if ($ftype ne "directory") {
			if (! FtwIsExcluded($pathname)) {
				$f_is[$f_i] = $f_i; $f_i++;
				push(@filenames, $filename);
				push(@mtimes, $mtime);
				push(@mdata, join('|', $ftype, $dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks));
			}
		}
	}
	closedir(DIR);

	if (scalar(@f_is) > 0) {
		my (@sorted_f_is) = ();
		if (uc($gSort) =~ /M?(TIME|DATE)* ASC/) {
			@sorted_f_is = sort {
				$mtimes[$a] <=> $mtimes[$b] or
				$filenames[$a] cmp $filenames[$b]
			} @f_is;
		} elsif (uc($gSort) =~ /M?(TIME|DATE)* DESC/) {
			@sorted_f_is = sort {
				$mtimes[$b] <=> $mtimes[$a] or
				$filenames[$a] cmp $filenames[$b]
			} @f_is;
		} elsif (uc($gSort) eq "NAME ASC") {
			@sorted_f_is = sort {
				$filenames[$a] cmp $filenames[$b] or
				$mtimes[$a] <=> $mtimes[$b]
			} @f_is;
		} elsif (uc($gSort) eq "NAME DESC") {
			@sorted_f_is = sort {
				$filenames[$b] cmp $filenames[$a] or
				$mtimes[$a] <=> $mtimes[$b]
			} @f_is;
		} else {
			@sorted_f_is = @f_is;
		}

		my ($n) = scalar(@f_is);
		for (my $i = 0; $i < scalar(@f_is); $i++) {
			$f_i = $sorted_f_is[$i];

			$filename = $filenames[$f_i];
			$pathname = $directory . "/" . $filename;
			($ftype, $dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = split(/\|/, $mdata[$f_i]);
			# print STDERR "  $i of $n: p=[$pathname] ", $filenames[$f_i], "\n";
			if (($rc = FtwProcessItem($pathname, $ftype, $dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks)) != 0) {
				# Proc said to abort.
				return ($rc);
			}
		}
	}

	#
	# Now do the rest of the subdirectories.
	# We have waited until we closed the parent directory
	# to conserve file descriptors.
	#
	if (scalar(@subdirs) > 0) {
		for $subdir (@subdirs) {
			$subdir = "$directory/$subdir";
			if ((! $gMayExcludeDirectories) || (! FtwIsExcluded($subdir))) {
				$rc = FtwProcessDirectory($subdir, $curdepth + 1, $maxdepth, $onedev, $rootdev);
				return ($rc) if ($rc < 0);
			}
		}
	}

	return (0);
}	# FtwProcessDirectory




sub Ftw
{
	my ($directory, $maxdepth, $onedev) = @_;
	if ($directory =~ /^(.+)\/+$/) {
		# Strip trailing slashes
		$directory = $1;
	}
	return (-1) if (! -d $directory);
	return (FtwProcessDirectory($directory, 1, $maxdepth, $onedev, undef));
}	# Ftw




sub Usage
{
	printf STDERR ("Usage: find ... -print | %s [--options] [more files to add]\n", $0);
	printf STDERR <<EOF;
Directory traversal options:
  --max-depth=X
  --xdev
  --[no]progress
  --exclude=REGEX
  --include=REGEX
  --newer=TIME|FILE
  --older=TIME|FILE
  --min-size=X
  --max-size=X
  --sort-ascending="NAME"|"MTIME"
  --sort-descending="NAME"|"MTIME"
Spool Options:
  --queue=/var/spool/ncftp
  --dry-run
  --lroot="/local/prefix/to/omit"
  --rroot="/remote/prefix/to/add"
  --login-config="/path/to/file.txt"
  --username="USER"
  --password="NOTRECOMMENDED"
  --host="ftp.remotehost.example.com"
  --passive=0|1
  --delete
  --source-address=IP
  --manual-override-features=FEATURE1[,FEATURE2[,...]]
  --pre-ftp-command=CMD
  --per-file-ftp-command=CMD
  --post-ftp-command=CMD
  --pre-shell-command=cmd.sh
  --post-shell-command=cmd.sh
  --remote-rename=NEWNAME
  --local-rename=NEWNAME
  --max-jobs-per-spool-file=X
  --max-bytes-per-spool-file=X
  --mbps-est=X
  -v          (Verbose mode)
  -d 1|2|3    (Debug level)
Login Config File example:
  host=ftp.remotehost.example.com
  username=USER
  password=Password1
  rroot=/remote/prefix/to/add
Features tokens (for complete list, see "man ncftp" for details):
  [!]hasREST
  [!]hasAPPE
  [!]hasCLNT
EOF
	exit(2);
}	# Usage



sub ReadLoginConfigFile
{
	my ($login_cfg) = $_[0];
	if (! open(LCFG, "< $login_cfg")) { return 0; }
	while (defined(my $line = <LCFG>)) {
		next if ($line =~ /^\s*#/);
		$line =~ s/[\r\n]+$//;
		my ($var, $val) = split(/=/, $line, 2);
		next unless (defined($var) && ($var ne ""));
		$val = 1 unless defined($val);
		$var =~ s/^\s*//; $var =~ s/\s*$//;
		$val =~ s/^\s*//; $val =~ s/\s*$//;
		$var = "rhost" if ($var =~ /host/i);
		$var = "ruser" if ($var =~ /user/i);
		$var = "rpassword" if ($var =~ /pass/i);
		$vars{$var} = $val;
		# printf ("VAR=[%s] VAL=[%s]\n", $var, $val);
	}
	close(LCFG);
}	# ReadLoginConfigFile



sub TimeFromStrOrFile
{
	my ($arg) = $_[0];
	my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = lstat($arg);
	if (defined($mtime)) { return $mtime; }

	return (str2time($arg));
}	# TimeFromStrOrFile




sub ProcessCommandLineArguments
{
	my ($sort_descending) = "";
	my ($sort_ascending) = "";
	my ($maxSize) = "";
	my ($minSize) = "";

	Getopt::Long::Configure ("bundling");
	Usage() unless GetOptions(
		"exclude=s" 		=> sub { push(@gExcludePatterns, $_[1]); },
		"x=s" 			=> sub { push(@gExcludePatterns, $_[1]); },
		"include=s" 		=> sub { push(@gIncludePatterns, $_[1]); },
		"i=s"	 		=> sub { push(@gIncludePatterns, $_[1]); },
		"xdev!"			=> \$gOneDev,
		"maxdepth|max-depth=i"	=> \$gMaxDepth,
		"maxsize|max-size=s" 	=> \$maxSize,
		"minsize|min-size=s"	=> \$minSize,
		"progress!"		=> \$gProgress,

		"d|debug=i" 		=> sub { $gDebug = $_[1]; },
		"v|verbose" 		=> sub { $gDebug++; },
		"n|dryrun|dry-run!"	=> \$dryrun,
		"queue=s"               => \$queue_directory,
		"sort=s"     		=> \$sort_ascending,
		"sort-ascending=s"      => \$sort_ascending,
		"sort-descending=s"     => \$sort_descending,
		"j|max-jobs-per-spool-file=i"	=> \$maxjobsperjobfile,
		"J|max-bytes-per-spool-file=s"	=> \$maxbytesperjobfile,
		"passive=s"		=> \$vars{"passive"},
		"delete"		=> sub { $vars{"delete"} = "1"; },
		"recursive=s"		=> \$vars{"recursive"},	# Don't use this option...
		"lroot=s"		=> \$lroot,
		"rroot=s"		=> \$rroot,
		"o=s"			=> \$vars{"manual-override-features"},
		"manual-override-features=s" => \$vars{"manual-override-features"},
		"source-address=s" 	=> \$vars{"source-address"},
		"pre-ftp-command=s" 	=> \$vars{"pre-ftp-command"},
		"per-file-ftp-command=s" => \$vars{"per-file-ftp-command"},
		"post-ftp-command=s" 	=> \$vars{"post-ftp-command"},
		"pre-shell-command=s" 	=> \$vars{"pre-shell-command"},
		"post-shell-command=s" 	=> \$vars{"post-shell-command"},
		"remote-rename=s"	=> \$vars{"remote-rename"},
		"local-rename=s"	=> \$vars{"local-rename"},
		"username=s"		=> \$vars{"ruser"},
		"password=s"		=> \$vars{"rpassword"},
		"hostname=s"		=> \$vars{"rhost"},
		"mbps-est=i"		=> \$Mbps_Est,
		"login-config=s",	=> sub {
			if (! ReadLoginConfigFile($_[1])) { die("Invalid login config file.\n"); }
		},
		"f=s",			=> sub {
			if (! ReadLoginConfigFile($_[1])) { die("Invalid login config file.\n"); }
		},
		"at=s",			=> sub {
			$job_scheduled_time = str2time($_[1]) or die ("Invalid time specified with --at\n");
		},
		"newer=s"		=> sub {
			$newer_t = TimeFromStrOrFile($_[1]) or die ("Invalid time specified with --newer\n");
		},
		"older=s"		=> sub {
			$older_t = TimeFromStrOrFile($_[1]) or die ("Invalid time specified with --older\n");
		},
	);

	if ($minSize ne "") {
		$gMinSize = (ByteUnits($minSize))[0];
		if (! defined($gMinSize)) { Usage(); }
		if ($gMinSize eq "") { Usage(); }
		if ($gMinSize < 0) { Usage(); }
	}

	if ($maxSize ne "") {
		$gMaxSize = (ByteUnits($maxSize))[0];
		if (! defined($gMaxSize)) { Usage(); }
		if ($gMaxSize eq "") { Usage(); }
		if ($gMaxSize < 0) { Usage(); }
	}

	if (defined($maxbytesperjobfile)) {
		$maxbytesperjobfile = (ByteUnits($maxbytesperjobfile))[0];
		if (! defined($maxbytesperjobfile)) { Usage(); }
		if ($maxbytesperjobfile eq "") { Usage(); }
		if ($maxbytesperjobfile < 0) { Usage(); }
	}

	if ($gMayExcludeDirectories < 0) {
		if ((scalar(@gIncludePatterns) == 0) && (scalar(@gExcludePatterns) == 0)) {
			$gMayExcludeDirectories = 0;
		} elsif ((scalar(@gIncludePatterns) > 0) && (scalar(@gExcludePatterns) == 0)) {
			$gMayExcludeDirectories = 0;
		} elsif ((scalar(@gIncludePatterns) == 0) && (scalar(@gExcludePatterns) > 0)) {
			$gMayExcludeDirectories = 1;
		}
	}
	$gProgress = 0 if ($gProgress < 0);
	CreateQueueDirectoryOrDie();

	if ((! exists($vars{"rhost"})) || ($vars{"rhost"} eq "")) {
		print STDERR "You must specify a remote host.\n";
		Usage();
	}

	if ((! exists($vars{"ruser"})) || ($vars{"ruser"} eq "")) {
		print STDERR "You must specify a remote username.\n";
		Usage();
	}
	
	if ($sort_descending ne "") {
		$gSort = "$sort_descending DESC";
	}
	if ($sort_ascending ne "") {
		$gSort = "$sort_ascending ASC";
	}
}	# ProcessCommandLineArguments




sub Main
{
	ProcessCommandLineArguments();

	my ($itempath);
	my ($rc);
	my ($nitems) = 0;
	my @items = @ARGV;

	for $itempath (@items) {
		$itempath = abs_path($itempath);
		if (-d $itempath) {
			$rc = Ftw($itempath, $gMaxDepth, $gOneDev);
			if ($rc != 0) {
				print STDERR "*** Result for $itempath was ($rc).\n";
			}
			$nitems++;
		} elsif (-f $itempath) {
			my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = lstat($itempath);
			FtwProcessItem ($itempath, "file", $dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks);
			$nitems++;
		} else {
			print STDERR "$itempath is not a valid directory (or file).\n";
		}
	}

	if ($nprocessed < 1) {
		my ($line) = "";
		while (defined($line = <STDIN>)) {
			$itempath = $line;
			$itempath =~ s/[\r\n]+//;
			next if ($itempath eq "");
			$itempath = abs_path($itempath);
			if (-d $itempath) {
				$rc = Ftw($itempath, $gMaxDepth, $gOneDev);
				if ($rc != 0) {
					print STDERR "*** Result for $itempath was ($rc).\n";
				}
				$nitems++;
			} elsif (-f $itempath) {
				my ($dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks) = lstat($itempath);
				FtwProcessItem ($itempath, "file", $dev,$ino,$mode,$nlink,$uid,$gid,$rdev,$size,$atime,$mtime,$ctime,$blksize,$blocks);
				$nitems++;
			} else {
				print STDERR "$itempath is not a valid directory (or file).\n";
			}
		}
	}

	CloseJobFile();

	my ($sec) = ($tbytesqueued / (1000000 * $Mbps_Est)) * 8;
	my ($days) = $sec / 86400;
	my ($hours) = $sec / 3600;
	my ($minutes) = $sec / 60;

	my ($tTiB) = $tbytesqueued / (1024 * 1024 * 1024 * 1024);
	my ($tGiB) = $tbytesqueued / (1024 * 1024 * 1024);
	my ($tMiB) = $tbytesqueued / (1024 * 1024);

	my ($txiB) = $tMiB;
	my ($txu) = "MiB";
	if ($tGiB > 0.7) { $txiB = $tGiB; $txu = "GiB"; }
	if ($tTiB > 0.7) { $txiB = $tTiB; $txu = "TiB"; }

	my ($tss) = $sec;
	my ($tsu) = "seconds";
	if ($minutes >= 1) { $tss = $minutes; $tsu = "minutes"; }
	if ($hours >= 1) { $tss = $hours; $tsu = "hours"; }
	if ($days > 1.5) { $tss = $days; $tsu = "days"; }

	printf("* %d job%s %squeued to %d spool file%s for a total of %.1f $txu; %.1f $tsu to upload at $Mbps_Est Mbps.\n",
		$njobsqueued,
		($njobsqueued == 1) ? "" : "s",
		($dryrun ? "(pretended to be) " : ""),
		$njobfiles,
		($njobfiles == 1) ? "" : "s",
		$txiB,
		$tss
	);

	if ($gProgress) {
		ProgressMessage(sprintf("Done. Elapsed Time = %d second%s.", time() - $gStartTime, (time() - $gStartTime) == 1 ? "" : "s"), 1);
	}

	Usage() if ($nitems == 0);
	exit(0);
}	# Main

Main();
