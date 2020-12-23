#!/bin/sh
#------------------------------------------------------------------------------
# this script allows searching for a repeat of timer using epgsearch (>=0.9.3).
# it requires the timercmd patch from Gerhard Steiner, that extends the timers
# menu of VDR with commands like in recordings menu
#
# add the following lines to your timercmds.conf
#
# Search for repeat : /path_of_this_script/timerrep.sh 0
# Search for repeat (with subtitle): /path_of_this_script/timerrep.sh 1
#
# Author: Christian Wieninger (cwieninger@gmx.de)
#------------------------------------------------------------------------------

# adjust the following lines to your config
# your plugins config dir
PLUGINCONFDIR=/etc/vdr/plugins/epgsearch
# path to svdrpsend
SVDRPSEND=svdrpsend
# if you are using special subfolders for some recordings, please add them here
FOLDERS="Comedy,Wissen,Serien,Magazine"
# the key used to call epgsearch
EPGSEARCHKEY=green

# do not edit below this line
#------------------------------------------------------------------------------

cat << EOM >/tmp/cmd.sh

SEARCHTERM="$6"~"$7";
#event info not yet present? then extract it from the file name
if test "\$SEARCHTERM" == "~"; then
SEARCHTERM='$8'
#cut leading special folders
i=0;
FOLDERS=$FOLDERS;
while [ "\$LASTWORD" != "\$FOLDERS" ];
do
    LASTWORD=\${FOLDERS%%,*};
    SEARCHTERM=\${SEARCHTERM#*\$LASTWORD~};
    i=\$i+1;
    FOLDERS=\${FOLDERS#*,};
done
#cut trailing dummy subtitle created by epgsearch
SEARCHTERM=\${SEARCHTERM%~???_??.??.????-??:??}
if [ "$1" -eq "0" ]; then
    SEARCHTERM=\${SEARCHTERM%~*};
fi
fi

RCFILE=$PLUGINCONFDIR/.epgsearchrc;
echo Search=\$SEARCHTERM > \$RCFILE;
echo SearchMode=0 >> \$RCFILE;
echo UseDescr=0 >> \$RCFILE;
$SVDRPSEND HITK $EPGSEARCHKEY;
EOM

echo ". /tmp/cmd.sh; rm /tmp/cmd.sh" | at now



