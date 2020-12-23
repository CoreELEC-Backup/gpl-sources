#!/bin/sh
# Originally written by :
# Viking / vdrportal.de
# cjac AT ich-habe-fertig.com
#
#----------------------------------------------------------------
# Version 1.3
#
# Mike Constabel <vejoun @ vdrportal>
#
# HISTORY:
#
# 2007-03-29: Version 1.3
#
# - fixes for use with epgsearch >= 0.9.21
#
# 2006-09-01: Version 1.2
#
# - fixed setsid calling
#
# 2006-07-17: Version 1.1
#
# - added setsid for calling UPDD
#
# 2006-??-?? update for vdr >= 1.3.44 and epgsearch >= 0.9.13a
#
#----------------------------------------------------------------
#
# Call with one of these parameters
# a. Recording Directory as parameter 1
# b. --recursive "Start_Dir"
# c. --recursive
#
# If called with "--recursive" either "Start_Dir" or "VIDEO_ROOT"
# from below is searched for recordings to put to the done-file
#
#----------------------------------------------------------------

# Should wo only test what is done ?
TEST=yes

# should we add recordings that have a S-ID in info.vdr ?
# That is recordings already recorded with epgsearch-autotimer
# and they are probably already in the done file!
# yes = add recodrings with S-ID
# no = don't add recordings with S-ID
ADD_SID_RECORDINGS=no

# should the script ask for S-ID for each recording ?
# The script shows a list of possible S-ID's at the beginning
ASK_SID=no

# What S-ID should be used if no other selected
DEFAULT_SID=-1

# Use the recording-dir's ctime as recording time?
CTIME_FROM_RECORDING=yes

# adjust the following line to your path to svdrpsend
SVDRPSEND=svdrpsend

# Home of EPGsearch
EPGSEARCH_HOME="/etc/vdr/plugins"

# Video root
VIDEO_ROOT="/video"

# do not edit below this line
#------------------------------------------------------------------------------

EPGSEARCHDONE_FILE="$EPGSEARCH_HOME/epgsearchdone.data"
EPGSEARCH_FILE="$EPGSEARCH_HOME/epgsearch.conf"
PrevTitle=""

function ShowUsableSIDs()
{
  printf "\n"
  grep -v "^#" $EPGSEARCH_FILE | sort -t':' -k2 | awk -F':' '{ print $1"\t"$2 }'
  printf "\n"
}

function AddRecToDone()
{
  Rec=$1
  if [ -e "$Rec/info.vdr" ]; then
    # Get ctime from recordingdir
    if [ "$CTIME_FROM_RECORDING" = "yes" ]; then
       CTIME="$(echo "$Rec" | sed 's#......\.rec/##;s#.*/##')"
       CTIME="$(date +%s -d"${CTIME:0:10} ${CTIME:11:2}:${CTIME:14:2}")"
    fi
    # Find S-ID in info.vdr
    S_IDAlt=`grep -s "^D .*s-id:" $Rec/info.vdr | sed -re 's/^D .*s-id: ([0-9]*).*/\1/'`
    S_IDNeu=`grep -s "^@ .*<epgsearch>.*<s-id>.*<\/s-id>.*<\/epgsearch>" $Rec/info.vdr | sed -re 's/^@ .*<epgsearch>.*<s-id>([0-9]*)<\/s-id>.*<\/epgsearch>.*/\1/'`
    [ "$S_IDAlt" != "" ] && S_ID="$S_IDAlt"
    [ "$S_IDNeu" != "" ] && S_ID="$S_IDNeu"
    Title=$(grep "^T " $Rec/info.vdr| cut -f2- -d' '|head -1)
    Subtitle=$(grep "^S " $Rec/info.vdr| cut -f2- -d' '|head -1)
    if [ "$S_ID" = "" -o "$S_ID" != "" -a "$ADD_SID_RECORDINGS" = "yes" ]; then
      [ $(grep "^T " $Rec/info.vdr| wc -l) -gt 1 ] && printf "\n\nERROR: DUAL T Line %s\n\n" "$Rec"

      printf "Adding \"%s, %s\".\n" "$Title" "$Subtitle"

      if [ "$ASK_SID" = "yes" -a "$S_ID" = "" ]; then
	if [ "$Title" != "$PrevTitle" ]; then
	  printf "Enter S-ID (s=skip, ENTER=$DEFAULT_SID): "
	  read NEW_SID
	  if [ "$NEW_SID" != "s" ]; then
            [ -z "$NEW_SID" ] && NEW_SID=$DEFAULT_SID
            printf "S-ID is set to $NEW_SID for \"$Title\"\n\n"
	  fi
	else
	  printf "Title matches, using same S-ID as before : $NEW_SID\n\n"
	fi
	PrevTitle=$Title
      else
	[ "$S_ID" = "" ] && NEW_SID=$DEFAULT_SID || NEW_SID=$S_ID
      fi

      if [ "$NEW_SID" != "s" ]; then
        echo "R $CTIME 0 $NEW_SID"   >> $EPGSEARCHDONE_FILE
        grep -v "^[EVX] " $Rec/info.vdr  >> $EPGSEARCHDONE_FILE
        echo "r"                     >> $EPGSEARCHDONE_FILE
      else
	printf "SKIP   \"%s, %s\"\n\n" "$Title" "$Subtitle"
      fi
    else
      printf "SKIP   \"%s, %s\" - it has S-ID: $S_ID\n\n" "$Title" "$Subtitle"
    fi
  else
    printf "No Info.vdr found : %s\n" "$Rec"
  fi

}

if [ -z "$1" ]; then
  printf "\nERROR : Parameter 1 should be either \"--recursive\" with start directory or a recording directory.\n"
  exit 1
fi

[ "$TEST" = "yes" ] && EPGSEARCHDONE_FILE=$EPGSEARCHDONE_FILE.test || cp $EPGSEARCHDONE_FILE $EPGSEARCHDONE_FILE.bak

if [ "$1" = "--recursive" ]; then
  shift
  [ "$ASK_SID" = "yes" ] && ShowUsableSIDs
  printf "Default S-ID: $DEFAULT_SID\n\n"
  [ ! -z "$1" ] && START_DIR=$1 || START_DIR=$VIDEO_ROOT
  for i in $(find $START_DIR/ -type d -name "*.rec" -print); do AddRecToDone $i ; done
else
  AddRecToDone $1
fi

if [ "$TEST" = "yes" ]; then
 printf "\n\nTEST run !!!!\nNow take a look at the File $EPGSEARCHDONE_FILE to see if everything is OK.\n\n"
else
  # Tell epgsearch that done-file was changed
  if ! echo "$SVDRPSEND PLUG epgsearch UPDD >/dev/null 2>&1" | at now >/dev/null 2>&1; then
    if ! setsid "$SVDRPSEND" PLUG epgsearch UPDD >/dev/null 2>&1; then
      echo "ERROR calling $SVDRPSEND PLUG epgsearch UPDD"
    fi
  fi
fi

#EOF
