#!/bin/sh

FILES="oscam-config-global.c oscam-config-account.c oscam-config-reader.c"

echo "** Checking config tables stored in: $FILES"

if [ ! -f globals.h ]
then
	echo "ERROR: Run this script in the oscam source directory (where globals.h file is)."
	exit 1
fi

check_int() {
	DEF=$1
	TYPE=$2
	echo "== Checking $DEF -> config parser expects '$TYPE' type but globals.h is THE CORRECT ONE"
	for VAR in `cat $FILES | grep $DEF | grep -w OFS | tr -d ' \t' | cut -d\( -f3 | cut -d\) -f1`
	do
		[ $VAR = "cacheex.maxhop" ] && continue
		awk '{print $1 " " $2 }' globals.h | grep -vE "^[#/]" | sed -e 's|;||g' | grep -w $VAR | while read Z
		do
			if [ "$TYPE $VAR" != "$Z" ]
			then
				printf "globals.h: %-32s | config: $TYPE $VAR\n" "$Z"
			fi
		done
	done
}

check_int DEF_OPT_INT8   int8_t
check_int DEF_OPT_UINT8  uint8_t
check_int DEF_OPT_INT32  int32_t
check_int DEF_OPT_UINT32 uint32_t

echo "== Checking DEF_OPT_STR (strings) -> config parser expects 'char *' type"
for VAR in `cat $FILES | grep DEF_OPT_STR | grep OFS | awk '{print $3}' | sed "s|OFS(||;s|)||;s|,||"`
do
	grep -w $VAR globals.h | grep -vwE "(\*$VAR|#include)" | grep -w --color $VAR
done

echo "== Checking DEF_OPT_SSTR (static strings) -> config parser expects 'char[x]'"
for VAR in `cat $FILES | grep DEF_OPT_SSTR | grep OFS | awk '{print $3}' | sed "s|OFS(||;s|)||;s|,||"`
do
	grep -w $VAR globals.h | grep -vE "(\[|#define)" | grep -w --color $VAR
done

echo "== Checking DEF_OPT_HEX (arrays) -> config parser expects 'uint8_t[x]' type"
for VAR in `cat $FILES | grep DEF_OPT_HEX | grep OFS | awk '{print $3}' | sed "s|OFS(||;s|)||;s|,||"`
do
	grep -w $VAR globals.h | grep -vw uint8_t | grep -w --color $VAR
done
