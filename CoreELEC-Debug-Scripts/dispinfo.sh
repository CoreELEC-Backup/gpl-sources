#!/bin/bash
#
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2018-present Team CoreELEC (https://coreelec.org)
#
# Collect CoreELEC display information
#
#####################################################
#
# Comand Line Arguments
# -l = Show local only
# -r = Remove items that are redundant between debug scripts, and show local only
#
#####################################################

OUTPUTFILE="/storage/dispinfo.txt"

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


printf "CoreELEC Display Information...\n\n" > $OUTPUTFILE

if [ "$1" != "-r" ]; then
    fancycat "/etc/os-release" "Not Found!"
    fancycat "/proc/device-tree/coreelec-dt-id" "Not Found!"
    fancycat "/proc/device-tree/le-dt-id" "Not Found!"
    fancycat "/proc/cmdline" "Not Found!"
fi
fancycat "/sys/devices/virtual/amhdmitx/amhdmitx0/edid" "Not Found!"
fancycat "/sys/devices/virtual/amhdmitx/amhdmitx0/edid_parsing" "Not Found!"
fancycat "/sys/devices/virtual/amhdmitx/amhdmitx0/rawedid" "Not Found!"
fancycat "/sys/devices/virtual/amhdmitx/amhdmitx0/config" "Not Found!"
fancycat "/sys/devices/virtual/amhdmitx/amhdmitx0/dc_cap" "Not Found!"
fancycat "/sys/devices/virtual/amhdmitx/amhdmitx0/dv_cap" "Not Found!"
fancycat "/sys/devices/virtual/amhdmitx/amhdmitx0/attr" "Not Found!"
fancycat "/sys/devices/virtual/amhdmitx/amhdmitx0/disp_cap" "Not Found!"
fancycat "/sys/devices/virtual/amhdmitx/amhdmitx0/vesa_cap" "Not Found!"
fancychk "/flash/vesa.enable"
fancycat "/sys/devices/virtual/amhdmitx/amhdmitx0/custom_mode" "Not Found!"
fancycat "/sys/devices/virtual/amhdmitx/amhdmitx0/preferred_mode" "Not Found!"
fancycat "/sys/devices/virtual/amhdmitx/amhdmitx0/hdr_cap" "Not Found!"
fancycat "/sys/module/am_vecm/parameters/hdr_mode" "Not Found!"
fancycat "/sys/module/am_vecm/parameters/sdr_mode" "Not Found!"
fancycat "/sys/class/display/vinfo" "Not Found!"

printf "------------ kodi display settings ------------" >> $OUTPUTFILE
if [ -f /storage/.kodi/userdata/guisettings.xml ]; then
    printf "\n" >> $OUTPUTFILE
    for tag in "coreelec.amlogic.limit8bit" \
               "coreelec.amlogic.force422" \
               "coreelec.amlogic.deinterlacing" \
               "coreelec.amlogic.noisereduction" \
               "coreelec.amlogic.hdr2sdr" \
               "coreelec.amlogic.sdr2hdr" \
               "videoplayer.adjustrefreshrate" \
               "videoplayer.useamcodec" \
               "videoplayer.useamcodech264" \
               "videoplayer.useamcodecmpeg2" \
               "videoplayer.useamcodecmpeg4" \
               "videoplayer.usedisplayasclock" \
               "videoscreen.whitelist" \
               "lookandfeel.skin"
    do
        printf "$tag: " >> $OUTPUTFILE
        value=$(cat /storage/.kodi/userdata/guisettings.xml |grep "\"$tag\"" |grep -o '>.*<' |sed -E 's/[<>]//g')
        [ -n "$value" ] && printf "$value" >> $OUTPUTFILE
        printf "\n" >> $OUTPUTFILE
    done
else
    printf " Not Found!\n" >> $OUTPUTFILE
fi

fancycat "/storage/.kodi/userdata/disp_cap" "Unset by user!"
fancycat "/storage/.kodi/userdata/disp_add" "Unset by user!"
if [ "$1" != "-r" ]; then
    fancycat "/flash/boot.ini"  "Not Found!"
    fancycat "/flash/config.ini"  "Not Found!"
    fancycat "/storage/.config/autostart.sh" "Unset by user!"
fi

if [ "$1" = "-l" ] || [ "$1" = "-r" ]; then
    cat $OUTPUTFILE
else
    paste $OUTPUTFILE
fi
