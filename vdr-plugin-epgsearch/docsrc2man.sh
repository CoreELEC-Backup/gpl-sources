#!/bin/bash
#
# Creates the man pages
#
# Needs: pod2man and nroff
#
# Mike Constabel
#
# Version 0.1 - 31.07.2006
# jasmin.j 24. May 2017:
#   Added dependency generation by "--depend <tag>"
#

DOCSRC="doc-src"

if [ ! -s "epgsearch.c" ]; then
	echo "Cannot find epgsearch.c. Call this script from epgsearch source directory."
	exit
fi

PRINT_DEPS=0
if [ "$1" = "--depend" ]; then
	PRINT_DEPS=1
	PRINT_DEPS_STMP=$2
	echo "$PRINT_DEPS_STMP: \\"
fi

VERSION="$(awk -F\" '/VERSION/ {print $2; exit;}' epgsearch.c)"

function man_dir () {
    if [ $PRINT_DEPS -eq 0 ]; then
		mkdir -p man/$1
		rm man/$1/* 2>/dev/null
    fi
}

function man_gen () {
	for i in "$DOCSRC"/$1/*.txt; do
		if [ $PRINT_DEPS -eq 0 ]; then
			echo -ne "create man page: ($1) $(basename "$i" ".txt")..."
			name=$(echo "$(basename "$i")" | sed -e 's/\.[0-9]\..*$//')
			sect=$(echo "$i" | sed -e 's/.*\.\([0-9]\)\.txt/\1/')
			pod2man -u -c "Epgsearch Version $VERSION" -n "$name" --section="$sect" "$i" >"man/$1/$(basename "$i" ".txt")"
			if [ $? -eq 0 ]; then
				echo " done."
			else
				echo " failed."
			fi
		else
			echo -e "\t$i \\"
		fi
	done
}

function man_gz () {
	if [ $PRINT_DEPS -eq 0 ]; then
		rm "$DOCSRC"/$1/*~ 2>/dev/null
		gzip -f man/$1/*.[0-9]
	fi
}

for LANGUAGE in $(ls "$DOCSRC"/); do

	[ ! -d ""$DOCSRC"/$LANGUAGE" ] && continue

	man_dir $LANGUAGE
	man_gen $LANGUAGE
	man_gz  $LANGUAGE

done

echo

if [ $PRINT_DEPS -eq 0 ]; then
	for LANGUAGE in $(ls "$DOCSRC"/); do

		[ ! -d "$DOCSRC/$LANGUAGE" ] && continue
		mkdir -p doc/$LANGUAGE
		rm doc/$LANGUAGE/* 2>/dev/null

		for i in man/$LANGUAGE/*.gz; do
			echo -ne "create doc file from man page: ($LANGUAGE) $(basename "$i")..."
			zcat "$i" | preconv | nroff -man - | col -xbp > "doc/$LANGUAGE/$(basename "$i" ".gz").txt"
			echo " done"
		done

	done

	echo
fi

#EOF
