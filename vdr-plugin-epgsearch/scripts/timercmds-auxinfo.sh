#!/bin/bash
#
# Displays the AUX-Info from timers
#
# Use it as command in timercmds.conf
#
# Example:
#
# Display AUX info :  /usr/local/bin/timercmds-auxinfo.sh
#
# 2006-04-24 vejoun @ vdrportal
# Version 0.4
#

#<Configuration>

# Your timers.conf
TIMERS="/video/timers.conf"

#</Configuration>

CHANNELID="$2"
START="$3"
TITLE="$5"
SUBTITLE="$6"
DIR="$7"

TIME="$(awk 'BEGIN{print strftime("%Y-%m-%d:%H%M",'$START')}')" || exit $?

SEARCH="[0-9]*:${CHANNELID}:${TIME}:[0-9]*:[0-9]*:[0-9]*:${DIR//:/|/}:"

AUX="$(egrep -m 1 "$SEARCH" "$TIMERS" | cut -d":" -f9-)"

if [ -n "$TITLE" ]; then
	echo -e "TITLE:\n$TITLE\n"
else
	echo -e "TITLE:\nTimer off, no title available\n"
fi

if [ -n "$SUBTITLE" ]; then
	echo -e "SUBTITLE:\n$SUBTITLE\n"
else
	if [ -n "$TITLE" ]; then
	echo -e "SUBTITLE:\nNo subtitle available\n"
	else
		echo -e "SUBTITLE:\nTimer off, no subtitle available\n"
	fi
fi

if [ -n "$DIR" ]; then
	echo -e "PATH:\n$DIR\n"
fi

if [ -z "$AUX" ]; then
   echo -e "AUX:\nNo AUX data found\n"
else
   echo -e "AUX:\n${AUX//></>\\n<}\n"
fi

#EOF
