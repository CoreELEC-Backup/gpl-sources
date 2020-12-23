#!/bin/bash
#
# Creates the html pages
#
# Needs: pod2html
#
# Mike Constabel
#
# Version 0.1 - 24.09.2006
#

DOCSRC="doc-src"

if [ ! -s "epgsearch.c" ]; then
	echo "Cannot find epgsearch.c. Call this script from epgsearch source directory."
	exit
fi

VERSION="$(awk -F\" '/VERSION/ {print $2; exit;}' epgsearch.c)"

for LANGUAGE in $(ls "$DOCSRC"/); do

	[ ! -d ""$DOCSRC"/$LANGUAGE" ] && continue
	mkdir -p html/$LANGUAGE
	rm html/$LANGUAGE/* 2>/dev/null

	for i in "$DOCSRC"/$LANGUAGE/*.txt; do
		echo -ne "create html page: ($LANGUAGE) $(basename "$i" ".txt")..."
		pod2html --infile="$i" --outfile="html/$LANGUAGE/$(basename "$i" ".txt").html" --norecurse --title="Epgsearch Version $VERSION"
		if [ $? -eq 0 ]; then
			echo " done."
		else
			echo " failed."
		fi
	done

	rm "$DOCSRC"/$LANGUAGE/*~ 2>/dev/null
done

echo

#EOF
