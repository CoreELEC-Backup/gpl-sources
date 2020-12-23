#!/bin/sh
#
# epg2master-timer.sh
#
# source: epgsearch-plugin, taken from vdr-wiki.de
# author: Christian Jacobsen / Viking (vdrportal)
#
# add this line to your epgsearchcmds.conf:
# folgende zeile in die epgsearchcmds.conf eintragen:
#
# epg2master-timer               : /path_to_this_script/epg2master-timer.sh
# epg2master-timer (no subtitle) : /path_to_this_script/epg2master-timer.sh -nosub

# CONFIG START
  MASTERTIMER_FILE=/etc/master-timer/torecord
# CONFIG END

if touch $MASTERTIMER_FILE >/dev/null 2>&1 ; then
    USESUB=yes
    if [ "$1" = -nosub ] ; then
        USESUB=no
    fi

    # add timer
    printf "\nAdding Master-Timer :\n"
    printf "\n[$1]\nTitle = $1\n" | tee -a $MASTERTIMER_FILE
    if [ "$6" != "" -a $USESUB = yes ] ; then
        printf "Subtitle = $6\n" | tee -a $MASTERTIMER_FILE
    fi

    # with "^" and "$" so that the exact channel name is used.
    printf "Channel = ^$5\$\n" | tee -a $MASTERTIMER_FILE
    printf "\n\nLast 10 lines of torecord : \n\n"
    tail -10 $MASTERTIMER_FILE
else
    echo "Error, cannot open file ($MASTERTIMER_FILE)..."
fi
