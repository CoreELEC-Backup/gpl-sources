#!/bin/sh
# $Id: undoneepgsearch.sh,v 1.7 2008/01/24 15:29:10 cjac Exp $
#
# Created 2007 by Viking / vdr-portal
#
# This script does an undone of recrdings done with EPGsearch
# It compares Title and Subtitle with the entry in the done file.
# If there is no Subtitle, then the Title and Description is compared.
# Options :
# --TitleOnly  Only match title, Subtitle and Description are ignored
#              This can be dangerous ! With series you remove ALL entries !
# --CheckDone  Only tell if recording is done, not undone
# --CheckOnly  Only tell if recording is done, not undone
#
# The options can also be combined.
#
#
# Add this to your reccmds.conf :
#
# Remove from EPGsearch done : /usr/local/bin/undoneepgsearch.sh
# Remove from EPGsearch done - TitleOnly : /usr/local/bin/undoneepgsearch.sh --TitleOnly
# Is Recording done : /usr/local/bin/undoneepgsearch.sh --CheckDone



#------------------------------------------------------------------------------

# default recordingdone settings
EPGSEARCHDONE_FILE="/etc/vdr/plugins/epgsearch/epgsearchdone.data"
# EPGSEARCHDONE_FILE="/tmp/epgsearchdone.data"

# Backup epgsearchdone.data before changing it (only once a day)
BACKUP=yes

SVDRPSEND=svdrpsend

# For some debugging infos, set to yes
DEBUG=no
## DEBUG=yes

# do not edit below this line
#------------------------------------------------------------------------------


[ "$1" = "" ] && printf "\nERROR Parameter 1 is not set !\n" && exit 1

Title=""
Subtitle=""
TempFile=/tmp/${0##*/}.$$
EPGSEARCHDONE_WORK=$EPGSEARCHDONE_FILE.work
Today=$(date +%Y%m%d)
Undone=false


function CleanExit() {
  [ -e $TempFile ] && rm -f $TempFile
  [ -e $EPGSEARCHDONE_WORK ] && rm -f $EPGSEARCHDONE_WORK
  [ -e $EPGSEARCHDONE_WORK.undone ] && rm -f $EPGSEARCHDONE_WORK.undone
  exit 1
}


# Get "--" options
while [ "${1:0:2}" = "--" ]; do
  eval ${1:2}=yes
  shift
done

Rec=$1

[ "$CheckOnly" = "yes" ] && CheckDone=$CheckOnly

if [ ! -e "$Rec/info.vdr" ]; then
  printf "\nNo Info file found in recording\n"
  exit 0
fi

# Find Tite, Subtitle and Description
Title=$(grep "^T " $Rec/info.vdr| cut -f2- -d' ')
Subtitle=$(grep "^S " $Rec/info.vdr| cut -f2- -d' ')
Description=$(grep "^D " $Rec/info.vdr | sed -e 's/\[/\./g' | sed -e 's/\]/\./g' | sed -e 's/\*/\./g')

if [ "$TitleOnly" = "yes" ]; then
  Description=""
  Subtitle=""
fi

if [ -z "$Title" -a -z "$Subtitle" ]; then
  printf "Title and Subtitle not found, doing nothing\n"
  exit 0
else

  printf "Title: $Title\n"

  if [ "$TitleOnly" = "yes" ]; then
    echo "- Only using title"
  else
    [ ! -z "$Subtitle" ] && printf "Subtitle: $Subtitle\n" || printf "Subtitle: No Subtitle, using Description\n"
  fi

  # How many times does title match
  TitleCnt=$(grep -c "^T $Title$" $EPGSEARCHDONE_FILE)
  printf "\nFound $TitleCnt matching title lines\n"

  if [ "$CheckDone" = "yes" ]; then
    printf "\nDone matching all criteria ?\n\n"

    if [ -z "$Subtitle" ]; then
      grep -A1 "^T $Title$" $EPGSEARCHDONE_FILE | grep -q "$Description"
    else
      grep -A1 "^T $Title$" $EPGSEARCHDONE_FILE | grep -q -B1 "^S $Subtitle$"
    fi
    if [ $? -eq 0 ]; then
      printf "YES, DONE\n"
    else
      printf "NO, NOT done\n"
    fi
    exit 0
  fi

  if [ $TitleCnt -gt 0 ]; then
    # Backup done file, but only one backup per day
    [ ! -e $EPGSEARCHDONE_FILE.$Today -a "$BACKUP" = "yes" ] && cp $EPGSEARCHDONE_FILE $EPGSEARCHDONE_FILE.$Today

    # Create Workfile
    cp -f $EPGSEARCHDONE_FILE $EPGSEARCHDONE_WORK
  else
    printf "\nNo matching entry found in done-list.\n"
    exit 0
  fi

  # Try one match after each other
  let Try=1
  let Match=1
  while [ $Try -le $TitleCnt ]; do
    printf "\nDoes $Try. entry match all criteria : "

    [ $DEBUG = yes ] && printf "\nMatch=$Match\n"

    [ $Match -eq 1 ] && grep -m$Match -A4 "^T $Title$" $EPGSEARCHDONE_WORK >$TempFile || grep -m$Match -A4 "^T $Title$" $EPGSEARCHDONE_WORK | grep -A5 "^--$" >$TempFile

    if [ -z "$Subtitle" ]; then
      grep -q "$Description" $TempFile
    else
      grep -q -B1 "^S $Subtitle$" $TempFile
    fi
    if [ $? -eq 0 ]; then
      printf "YES, "
      let MatchLine=$(grep -m$Match -n "^T $Title$" $EPGSEARCHDONE_WORK |tail -n 1| cut -f1 -d ':')
      [ $DEBUG = yes ] && printf "\n\nMatching line : $MatchLine\n"
      if [ $MatchLine -gt 3 ]; then
	let FirstLine=MatchLine-3
	[ $DEBUG = yes ] && printf "First line of Recording : $FirstLine\n"
	# First line OK ?
	nice -n 19 head -n $FirstLine $EPGSEARCHDONE_WORK | tail -n 1 | grep -q "^r"
	if [ $? -ne 0 ]; then
	  printf "\nERROR: something went wrong finding the First line of recording, quitting\n"
	  CleanExit
	fi
        let MatchRLine=$(grep -m$Match -n "^r$" $TempFile |head -n 1| cut -f1 -d ':')
	let LastMatchLine=MatchLine+MatchRLine
	# Bugfix - if more than one result then results are separated by a "--" line
	grep -q "^--$" $TempFile && let LastMatchLine--
        [ $DEBUG = yes ] && printf "Last Matching line : $LastMatchLine\n"

	let TailLines=$(wc -l $EPGSEARCHDONE_WORK | cut -f1 -d' ')
	nice -n 19 head -n $LastMatchLine $EPGSEARCHDONE_WORK | tail -n 1 | grep -q "^R "
	if [ $? -ne 0 -a $LastMatchLine -lt $TailLines ]; then
	  printf "\nERROR: something went wrong finding the Last line of recording, quitting\n"
	  CleanExit
	fi

	let TailLines=TailLines-LastMatchLine+1
	[ $DEBUG = yes ] && printf "TailLines = $TailLines\n"

	# Sanity check
	if [ $LastMatchLine -gt $FirstLine ]; then
	  nice -n 19 head -n $FirstLine $EPGSEARCHDONE_WORK >$EPGSEARCHDONE_WORK.undone
	  STATUS=$?
	  nice -n 19 tail -n $TailLines $EPGSEARCHDONE_WORK >>$EPGSEARCHDONE_WORK.undone

	  if [ $STATUS -eq 0 -a $? -eq 0 ]; then
	    cp $EPGSEARCHDONE_WORK.undone $EPGSEARCHDONE_WORK
	    Undone=true
	    printf "Undone\n"
	  fi
	  rm -f $EPGSEARCHDONE_WORK.undone
	fi
      fi
    else
      printf "NO, not undone\n"
      let Match++
      if [ -z "$Subtitle" ]; then
	printf "\nEPG DESCRIPTION from done (maybe it helps) :\n\n"
        grep "^D " $TempFile | cut -c3- | tr '|' '\n'
      fi
    fi
    let Try++
  done

  if [ "$Undone" = "true" ]; then
    let WorkLines=$(wc -l $EPGSEARCHDONE_WORK | cut -f1 -d' ')
    let EpgsLines=$(wc -l $EPGSEARCHDONE_FILE | cut -f1 -d' ')
    [ $DEBUG = yes ] && printf "\nOld number of lines $EpgsLines, new $WorkLines\n"
    if [ $EpgsLines -gt $WorkLines ]; then
      cp -f $EPGSEARCHDONE_WORK $EPGSEARCHDONE_FILE
      [ $? -eq 0 ] && printf "\nUndone successfull\n" || printf "\nSomething went wrong with undone\n"
      # Reload done-file
      echo "$SVDRPSEND PLUG epgsearch UPDD" | at now 2>/dev/null
    else
      printf "\nSomething went wrong with undone\n"
    fi
  else
    [ -z "$Subtitle" ] && printf "\n\nYou could try using the option --TitleOnly\n"
  fi

  [ -e $TempFile ] && rm -f $TempFile
  [ -e $EPGSEARCHDONE_WORK ] && rm -f $EPGSEARCHDONE_WORK
fi


