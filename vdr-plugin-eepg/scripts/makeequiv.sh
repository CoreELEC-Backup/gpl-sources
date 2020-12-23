#!/bin/bash

equiv_file="eepg.equiv"

usage() {
	echo
	[[ "$@" ]] && echo -e "ABORTED! $@\n"
	echo -e "usage: ${0##*/} [channels.conf] [listsources|source to map epg from] <matchname>"
	echo -e "       * the 3rd argument is optional. if its \"matchname\", channel name matching is required for a positive match.\n"
	exit
}

getsources() { awk -F: '{print $4}' "$1" |sort -V |uniq; }
listsources() { getsources "$1"; exit; }

(($# < 2)) && usage
[[ -e "$1" ]] || usage "$1 not found"
[[ "$2" == "listsources" ]] && listsources "$1"
[[ $(getsources "$1" |grep "$2$") ]] || usage "$2 is not a source in $1"
[[ "$3" == "matchname" ]] && match_name=1 || match_name=0
[[ -e "$equiv_file" ]] && rm "$equiv_file"
echo
tput sc
OLDIFS=$IFS; IFS=$'\n'
for i in $(awk -F: -v var="$2" '$4==var' "$1"); do
	matched=0
	mapto_name=" ${i%%:*}"
	mapto_sid=$(awk -F: '{print $10}' <<<"$i")
	mapto_line=$(awk -F: '{print $4"-"$11"-"$12"-"$10"-"$13}' <<<"$i")
	mapto_source=$(awk -F: '{print $4}' <<<"$i")
	for j in $(awk -F: -v var1="$2" -v var2="$mapto_sid" '$4!=var1 && $10==var2' "$1"); do
		mapfrom_source=$(awk -F: '{print $4}' <<<"$j")
		mapfrom_line=$(awk -F: '{print $4"-"$11"-"$12"-"$10"-"$13}' <<<"$j")
		matched=1
		(($match_name)) && {
			[[ " ${j%%:*}" == "$mapto_name" ]] || matched=0
		} || unset mapto_name
	done
	(($matched)) && {
		((matchcount++))
		outline="$mapfrom_line $mapto_line$mapto_name"
		echo "$outline" >>"$equiv_file"
	} || {
		((unmatchcount++))
		array+=( "$i" )
		unset outline
	}
	tput rc
	tput el
	echo "$outline"
	echo -n "matched: $matchcount  -  unmatched: $unmatchcount"
done
IFS=$OLDIFS
echo -e "\n"
((${#array[@]} > 0)) && for i in "${array[@]}"; do echo "NO MATCH: $i"; done
echo "wrote $equiv_file ($matchcount entries)."
