#!/bin/sh

FTPHOST="ftp.remotehost.example.com"
FTPUSER="username"
FTPPASS="password"
REMOTE_DIR="/remote/directory"
CSVFILE="/tmp/test.csv"
tmpfile="$HOME/.ncftp/spool/tmp/filelist.txt"

#
# Check for required programs.
#
for prog in perl sed cut ncftpput ncftpbatch
do
	progIsOptional="`echo "$prog" | cut -d, -f2`"
	prog="`echo "$prog" | cut -d, -f1`"
	pathprog=""
	oIFS="$IFS"
	IFS=":\
"
	for dir in $PATH
	do
		if [ -f "$dir/$prog" ] && [ -x "$dir/$prog" ] ; then
			pathprog="$dir/$prog"
			break
		fi
	done
	IFS="$oIFS"
	if [ "$pathprog" = "" ] && [ "$progIsOptional" = "optional" ] ; then
		echo "Warning: Program \"$prog\" is not installed (or not in \$PATH)"
	elif [ "$pathprog" = "" ] ; then
		echo "Error: Program \"$prog\" is not installed (or not in \$PATH)" 1>&2
		exit 1
	fi
done
unset prog pathprog progIsOptional oIFS

#
# Create the temporary directory used by this script.
#
umask 077
if [ ! -d "$HOME/.ncftp/spool/tmp" ] ; then
	mkdir "$HOME/.ncftp/spool/tmp"
	if [ ! -d "$HOME/.ncftp/spool/tmp" ] ; then
		exit 1
	fi
fi


#
# Convert the CSV file into an intermediate format 
# that can be easily parsed by the shell.
#
perl -e '{
    use strict;
    use warnings;
    my ($line);
    my (@fields) = ();

    while (defined($line = <>)) {
    	$line =~ s/[\r\n]+//;
	next if ($line =~ /^\s*$/);	# skip blank lines
	next if ($line =~ /^\s*#/);	# skip comments
	@fields = ();
    	while ($line =~ /((\"[^\"]*\")|([^,]*))(,|$)/ig) {
		my ($field) = $1;
		if ($field =~ /^\"(.*)\"$/) {
			$field = $1;
		}
		push(@fields, $field);
	}
	print join("|", @fields), "\n";
    }
}' < "$CSVFILE" > "$tmpfile"

nlines=`wc -l < "$tmpfile"`
i=1
while [ "$i" -le "$nlines" ] ; do
	# Field 1 in the CSV is the pathname to the local file.
	LOCAL_FILE=`sed -n "${i},${i}p" < "$tmpfile" | cut -d"|" -f1`

	# Field 2 and 3 are the date and time to start the transfer, in UTC (GMT).
	YYYYMMDD=`sed -n "${i},${i}p" < "$tmpfile" | cut -d"|" -f2 | sed 's/[^0-9]//g;'`
	HHMMSS=`sed -n "${i},${i}p" < "$tmpfile" | cut -d"|" -f3 | sed 's/[^0-9]//g;'`

	i=`expr "$i" + 1`

	if [ -d "$HOME/.ncftp/spool" ] ; then
		mkdir "$HOME/.ncftp/spool/tmp" 2>/dev/null
		mv "$HOME/.ncftp/spool/"p-* "$HOME/.ncftp/spool/tmp" 2>/dev/null
	fi

	ncftpput -bb -u "$FTPUSER" -p "$FTPPASS" "$FTPHOST" "$REMOTE_DIR" "$LOCAL_FILE"
	es="$?"

	if [ "$YYYYMMDD" != "" ] && [ "$HHMMSS" != "" ] ; then
		#
		# If a date and time was specified, reschedule the
		# transfer to occur at that date.  If no date
		# was specified, it will be transferred as soon
		# as possible.
		#
		if [ "$es" -ne 0 ] ; then
			echo "ERROR: ncftpput failed with exit status $es." 1>&2
			mv "$HOME/.ncftp/spool/tmp"/p-* "$HOME/.ncftp/spool/" 2>/dev/null
			exit "$es"
		fi

		if [ ! -d "$HOME/.ncftp/spool" ] ; then
			echo "ERROR: spool directory was not created." 1>&2
			echo "ERROR: ncftpput did not create a new spool file." 1>&2
			exit 1
		fi

		nfixed=0
		cd "$HOME/.ncftp/spool"
		for spoolfile in p-*
		do
			if [ ! -f "$spoolfile" ] ; then continue ; fi
			suffix=`echo "$spoolfile" | cut -d- -f4-`
			newtimestamp="p-${YYYYMMDD}-${HHMMSS}-${suffix}"
			echo mv "$spoolfile" "$newtimestamp" || exit 1
			sed 's/^job-name=.*/job-name='"${newtimestamp}"'/;' < "$spoolfile" > "$newtimestamp" || exit 1
			/bin/rm "$spoolfile" 
			nfixed=`expr "$nfixed" + 1`
		done

		mv "$HOME/.ncftp/spool/tmp"/p-* "$HOME/.ncftp/spool/" 2>/dev/null

		if [ "$nfixed" -eq 0 ] ; then
			echo "ERROR: no spool files were changed." 1>&2
			exit 1
		fi
	fi
done

# Start the spooler.
echo ncftpbatch -d
ncftpbatch -d
sleep 2
/bin/ls -l "$HOME/.ncftp/spool/log" 2>/dev/null
