#!/bin/bash
#
# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2018-present Team CoreELEC (https://coreelec.org)
#
# Collect output from all CoreELEC Debug Scripts
#
#####################################################
#
# Comand Line Arguments
# -l = Show local only
#
#####################################################

OUTPUTFILE="/storage/ce-debug_info.txt"

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

fancycattail()
{
# $1 = file $2 = options $3 = message if file not found
    printf "------------ $1 ------------" >> $OUTPUTFILE
    if [ -f $1 ]; then
        printf "\n" >> $OUTPUTFILE
        tail $2 $1 | tr '\000' '\n' >> $OUTPUTFILE
    else
        printf " $3\n" >> $OUTPUTFILE
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


printf "CoreELEC Debug Information...\n\n" > $OUTPUTFILE

fancycat "/etc/os-release" "Not Found!"
fancycat "/proc/device-tree/coreelec-dt-id" "Not Found!"
fancycat "/proc/device-tree/le-dt-id" "Not Found!"
fancycat "/proc/cmdline" "Not Found!"
fancycat "/storage/.config/boot.hint" "Not Found!"
fancycat "/storage/.config/boot.status" "Not Found!"
fancycat "/flash/boot.ini"  "Not Found!"
fancycat "/flash/config.ini"  "Not Found!"
fancycattail "/flash/cfgload" "-c +73" "Not Found!"
fancycattail "/flash/aml_autoscript" "-c +73" "Not Found!"
fancycat "/storage/.config/autostart.sh" "Unset by user!"
printf "\n" >> $OUTPUTFILE
fancycat "/storage/init-previous.log" "Not Found!"
printf "\n" >> $OUTPUTFILE

printf "------------ fw_printenv ------------\n" >> $OUTPUTFILE
if [ -e /dev/env ]; then
    fw_printenv >> $OUTPUTFILE
    printf "\n" >> $OUTPUTFILE
else
    printf "Not found! || Not a TV Box?\n" >> $OUTPUTFILE
fi
printf "\n" >> $OUTPUTFILE

printf "------------ lsmod ------------\n" >> $OUTPUTFILE
lsmod >> $OUTPUTFILE
printf "------------ lsusb ------------\n" >> $OUTPUTFILE
lsusb >> $OUTPUTFILE

printf "\n" >> $OUTPUTFILE
if [ -x ./dispinfo.sh ]; then
    ./dispinfo.sh -r >> $OUTPUTFILE
else
    dispinfo -r >> $OUTPUTFILE
fi

printf "\n" >> $OUTPUTFILE
if [ -x ./remoteinfo.sh ]; then
    ./remoteinfo.sh -r >> $OUTPUTFILE
else
    remoteinfo -r >> $OUTPUTFILE
fi

printf "\n" >> $OUTPUTFILE
if [ -x ./audinfo.sh ]; then
    ./audinfo.sh -r >> $OUTPUTFILE
else
    audinfo -r >> $OUTPUTFILE
fi

if [ "$1" = "-l" ]; then
    cat $OUTPUTFILE
else
    paste $OUTPUTFILE
fi
