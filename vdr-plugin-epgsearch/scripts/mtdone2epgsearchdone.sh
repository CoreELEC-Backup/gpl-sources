#!/bin/sh
# Written by :
# Viking / vdrportal.de
# cjac AT ich-habe-fertig.com
#
# Call with one of these parameters
# a. Master-Timer Done-file
#
# Parameter :
# --sid X	= use S-ID X instead of default (see below)


#------------------------------------------------------------------------------

# Should wo only test what is done ?
TEST=yes

# should the script ask for S-ID for each recording ?
# The script shows a list of possible S-ID's at the beginning
ASK_SID=no

# What S-ID should be used if no other selected
DEFAULT_SID=0

# adjust the following line to your path to svdrpsend
SVDRPSEND=svdrpsend

# Home of EPGsearch
EPGSEARCH_HOME="/etc/vdr/plugins"

# do not edit below this line
#------------------------------------------------------------------------------

EPGSEARCHDONE_FILE="$EPGSEARCH_HOME/epgsearchdone.data"
EPGSEARCH_FILE="$EPGSEARCH_HOME/epgsearch.conf"
PrevTitle=""
Count=0

function ShowUsableSIDs()
{
  printf "\n"
  grep -v "^#" $EPGSEARCH_FILE | sort -t':' -k2 | awk -F':' '{ print $1"\t"$2 }'
  printf "\n"
}

function AddRecToDone()
{
  Rec=$1
  Title=$(echo $Rec|cut -f1 -d'|')
  Subtitle=$(echo $Rec|cut -f2 -d'|')
  [ "$Subtitle" = "NoSub" ] && Subtitle=""


      if [ "$ASK_SID" = "yes" ]; then
	if [ "$Title" != "$PrevTitle" ]; then
	  [ $Count -gt 10 ] && Count=1 || let Count++
	  [ $Count -eq 1 ] && ShowUsableSIDs
  	  printf "Adding \"%s, %s\".\n" "$Title" "$Subtitle"
	  printf "Enter S-ID (s=skip, ENTER=$DEFAULT_SID): "
	  read NEW_SID </dev/tty
	  if [ "$NEW_SID" != "s" ]; then
            [ -z "$NEW_SID" ] && NEW_SID=$DEFAULT_SID
            printf "S-ID is set to $NEW_SID for \"$Title\"\n\n"
	  fi
	else
	  let Count++
          printf "Adding \"%s, %s\".\n" "$Title" "$Subtitle"
	  printf "Title matches, using same S-ID as before : $NEW_SID\n\n"
	fi
	PrevTitle=$Title
      else
        printf "Adding \"%s, %s\".\n" "$Title" "$Subtitle"
	NEW_SID=$DEFAULT_SID
      fi

      if [ "$NEW_SID" != "s" ]; then
        echo "R 0 0 $NEW_SID"        >> $EPGSEARCHDONE_FILE
        echo "T $Title"              >> $EPGSEARCHDONE_FILE
        [ ! -z "$Subtitle" ] && echo "S $Subtitle" >> $EPGSEARCHDONE_FILE
        echo "D "                    >> $EPGSEARCHDONE_FILE
        echo "r"                     >> $EPGSEARCHDONE_FILE
      else
	printf "SKIP   \"%s, %s\"\n\n" "$Title" "$Subtitle"
      fi

}

if [ "$1" = "--sid" ]; then
  shift
  if [ -z "$1" ]; then
    printf "\nS-ID as parameter expected\n\n"
  else
    DEFAULT_SID=$1
    shift
    printf "\nFound parameter \"--sid\", Default S-ID is now set to $DEFAULT_SID\n\n"
  fi
fi


if [ -z "$1" ]; then
  printf "\nERROR : Parameter 1 should be a Master-Timer done-file\n\n"
  exit 1
fi

[ "$TEST" = "yes" ] && EPGSEARCHDONE_FILE=$EPGSEARCHDONE_FILE.test || cp $EPGSEARCHDONE_FILE $EPGSEARCHDONE_FILE.bak

[ "$ASK_SID" = "yes" ] && ShowUsableSIDs
printf "Default S-ID: $DEFAULT_SID\n\n"
while read i; do AddRecToDone "$i" ; done <$1

if [ "$TEST" = "yes" ]; then
 printf "\n\nTEST run !!!!\nNow take a look at the File $EPGSEARCHDONE_FILE to see if everything is OK.\n\n"
else
  # Tell epgsearch that done-file was changed
  echo "$SVDRPSEND PLUG epgsearch UPDD >/dev/null 2>&1" | at now >/dev/null 2>&1
fi

