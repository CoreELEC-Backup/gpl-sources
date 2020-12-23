#!/bin/sh
#
# epg2taste.sh - v.0.1
#
# add this line to your epgsearchcmds.conf:
# folgende zeile in die epgsearchcmds.conf eintragen:
#
# epg2taste : /usr/local/bin/epg2taste.sh

# CONFIG START
  TASTE_FILE="/etc/vdr/plugins/taste.conf"
  SVDRPSEND=svdrpsend

# default taste settings
  REGULAR_EXPRESSION=0 # Regular Expression / Regulärer Ausdruck
  IGNORE_CASE=0        # Ignore Case / Groß/Kleinschreibung ignorieren
# CONFIG END

# add taste
echo "${REGULAR_EXPRESSION}:${IGNORE_CASE}:${1}" >> "${TASTE_FILE}"
echo "Done..."

# jump back
at now <<EOF
perl -l -e "printf \"\n$SVDRPSEND HITK BACK\" x 2" | sh
EOF

