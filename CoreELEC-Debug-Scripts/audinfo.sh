#!/bin/bash
#
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2018-present Team CoreELEC (https://coreelec.org)
#
# Collect CoreELEC audio information
#
#####################################################
#
# Comand Line Arguments
# -l = Show local only
# -r = Remove items that are redundant between debug scripts, and show local only
#
#####################################################

OUTPUTFILE="/storage/audinfo.txt"

fancycat()
{
# $1 = file $2 = message if file not found
    printf "------------ $1 ------------" >> $OUTPUTFILE
    if [ -f $1 ]; then
        printf "\n" >> $OUTPUTFILE
        cat $1 | tr '\000' '\n' >> $OUTPUTFILE
    else
        printf " $2\n" >> $OUTPUTFILE
    fi
}

fancychk()
{
   printf "------------ $1 ------------" >> $OUTPUTFILE
    if [ -f $1 ]; then
        printf " Set by user!\n" >> $OUTPUTFILE
    else
        printf " Unset by user!\n" >> $OUTPUTFILE
    fi
}

fancycatdir()
{
# $1 = directory $2 = filename pattern $3 = message if file not found
    printf "------------ $1 ------------" >> $OUTPUTFILE
    if [ -d $1 ]; then
        printf "\n" >> $OUTPUTFILE
        for filename in $1/$2
        do
            [ -e $filename ] || continue
            if [ -f $filename ]; then
                fancycat $filename $3
            fi
        done
    else
        printf " Directory Not Found!\n"
    fi
}

wildcat()
{
# $1 = filename pattern $2 = message if file not found
    printf "------------ $1 ------------" >> $OUTPUTFILE
    if [ -e $1 ]; then
        printf "\n" >> $OUTPUTFILE
        for filename in $1
        do
            [ -e $filename ] || continue
            if [ -f $filename ]; then
                fancycat $filename $2
            fi
        done
    else
        printf " $2\n" >> $OUTPUTFILE
    fi
}


printf "CoreELEC Audio Information...\n\n" > $OUTPUTFILE

if [ "$1" != "-r" ]; then
    fancycat "/etc/os-release" "Not Found!"
    fancycat "/proc/device-tree/coreelec-dt-id" "Not Found!"
    fancycat "/proc/device-tree/le-dt-id" "Not Found!"
    fancycat "/proc/cmdline" "Not Found!"
    fancycat "/sys/devices/virtual/amhdmitx/amhdmitx0/edid_parsing" "Not Found!"
    fancycat "/sys/devices/virtual/amhdmitx/amhdmitx0/rawedid" "Not Found!"
    fancycat "/sys/devices/virtual/amhdmitx/amhdmitx0/config" "Not Found!"
fi

fancycat "/sys/devices/virtual/amhdmitx/amhdmitx0/aud_cap" "Not Found!"
fancycat "/proc/device-tree/pinctrl@ff634480/spdifout/mux/groups" "Not Found!"

printf "------------ /sys/class/sound ------------" >> $OUTPUTFILE
if [ -d /sys/class/sound ]; then
    for soundcard in `ls -d /sys/class/sound/card*`
        do
            if [ -f $soundcard'/id' ]; then
                printf "\n" >> $OUTPUTFILE
                cat $soundcard'/id' >> $OUTPUTFILE
                cdnum=$(cat $soundcard'/number')
                printf "|-card$cdnum\n" >> $OUTPUTFILE
                for ccontrol in `ls -d /sys/class/sound/controlC$cdnum`
                    do
                        printf "$ccontrol" | awk -F'/' '{print "|-"$NF}' >> $OUTPUTFILE
                    done
                for cpcm in `ls -d /sys/class/sound/pcmC$cdnum*`
                    do
                        printf "$cpcm" | awk -F'/' '{print "|-"$NF}' >> $OUTPUTFILE
                    done
            fi
        done
else
    printf " Not Found!\n" >> $OUTPUTFILE
fi

fancycat "/proc/asound/cards" "Not Found!"
fancycat "/proc/asound/pcm" "Not Found!"

printf "------------ aplay ------------" >> $OUTPUTFILE
if [ -x `which aplay` ]; then
    printf "\n" >> $OUTPUTFILE
    printf "------------ aplay -l ------------\n" >> $OUTPUTFILE
    aplay -l >> $OUTPUTFILE
    printf "------------ aplay -L ------------\n" >> $OUTPUTFILE
    aplay -L >> $OUTPUTFILE
else
    printf " Not Found!\n" >> $OUTPUTFILE
fi

printf "------------ kodi audio settings ------------" >> $OUTPUTFILE
if [ -f /storage/.kodi/userdata/guisettings.xml ]; then
    printf "\n" >> $OUTPUTFILE
    for tag in "accessibility.audiohearing" \
               "accessibility.audiovisual" \
               "accessibility.subhearing" \
               "audiooutput.ac3passthrough" \
               "audiooutput.ac3transcode" \
               "audiooutput.atempothreshold" \
               "audiooutput.audiodevice" \
               "audiooutput.boostcenter" \
               "audiooutput.channels" \
               "audiooutput.config" \
               "audiooutput.dtshdpassthrough" \
               "audiooutput.dtspassthrough" \
               "audiooutput.eac3passthrough" \
               "audiooutput.guisoundmode" \
               "audiooutput.maintainoriginalvolume" \
               "audiooutput.passthrough" \
               "audiooutput.passthroughdevice" \
               "audiooutput.processquality" \
               "audiooutput.samplerate" \
               "audiooutput.audiooutput.stereoupmix" \
               "audiooutput.audiooutput.streamnoise" \
               "audiooutput.audiooutput.streamsilence" \
               "audiooutput.truehdpassthrough" \
               "audiooutput.volumesteps" \
               "musicplayer.replaygainavoidclipping" \
               "musicplayer.replaygainnogainpreamp" \
               "musicplayer.replaygainpreamp" \
               "musicplayer.replaygaintype" \
               "musicplayer.seekdelay" \
               "musicplayer.seeksteps"
    do
        printf "$tag: " >> $OUTPUTFILE
        value=$(cat /storage/.kodi/userdata/guisettings.xml |grep "\"$tag\"" |grep -o '>.*<' |sed -E 's/[<>]//g')
        [ -n "$value" ] && printf "$value" >> $OUTPUTFILE
        printf "\n" >> $OUTPUTFILE
    done
    printf "mute: " >> $OUTPUTFILE
    value=$(cat /storage/.kodi/userdata/guisettings.xml |awk -F '[<>]' '/mute/ {print $3}')
    [ -n "$value" ] && printf "$value" >> $OUTPUTFILE
    printf "\n" >> $OUTPUTFILE
    printf "volumelevel: " >> $OUTPUTFILE
    value=$(cat /storage/.kodi/userdata/guisettings.xml |awk -F '[<>]' '/fvolumelevel/ {print $3}')
    [ -n "$value" ] && printf "$value" >> $OUTPUTFILE
    printf "\n" >> $OUTPUTFILE
else
    printf " Not Found!\n" >> $OUTPUTFILE
fi

fancycat "/storage/.config/sound.conf" "Unset by user!"
fancycat "/storage/.config/asound.conf" "Unset by user!"
fancycatdir "/storage/.config/pulse-daemon.conf.d" "*.conf" "Unset by user!"

if [ "$1" != "-r" ]; then
    fancycat "/storage/.config/autostart.sh" "Unset by user!"
fi

if [ "$1" = "-l" ] || [ "$1" = "-r" ]; then
    cat $OUTPUTFILE
else
    paste $OUTPUTFILE
fi
