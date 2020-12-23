#!/bin/sh
#
# epg2autotimer.sh - v.0.1
#
# source: epgsearch-plugin
#
# add this line to your epgsearchcmds.conf:
# folgende zeile in die epgsearchcmds.conf eintragen:
#
# epg2autotimer : /usr/local/bin/epg2autotimer.sh

# CONFIG START
  AUTOTIMER_FILE="$SOURCEDIR/vdradmin/vdradmind.at"
  SVDRPSEND=svdrpsend

# default autotimer settings
  STATUS=1       # 0 = inactive (by default) / 1 = active
  SERIE=1        # Serienaufnahme
  PRIO=0         # Priority / Priorität
  LIFE_TIME=0    # Lifetime / Lebensdauer
  TARGET_DIR=    # Folder / Verzeichnis
  SEARCH_WHERE=1 # Where to search / Wo soll gesucht werden? 1: Title 3: Title+Subtitle 7: All
# CONFIG END

# add autotimer
echo "${STATUS}:${1}:${SEARCH_WHERE}:::${SERIE}:${PRIO}:${LIFE_TIME}:${4}:${TARGET_DIR}" >> "${AUTOTIMER_FILE}"
echo "Done..."

# jump back
at now <<EOF
perl -l -e "printf \"\n$SVDRPSEND HITK BACK\" x 2" | sh
EOF

