#!/bin/sh

# look for some error messages in all test logs

for i in *.txt; do
	echo "Scanning $i"
	last=
	while read line; do
		case "$line" in
			===\ Entering*) last="$line" ;;
			*Assertion*failed*) echo "ASSERTION FAILED: $last" ;;
			*runtime\ error*) echo "RUNTIME ERROR (sanitizer): $last" ;;
			*AddressSanitizer*heap-use-after-free*) echo "RUNTIME ERROR (use after free): $last" ;;
			*Warning:\ assertion*failed*) echo "ASSERTION WARNING: $last" ;;
			*) : ;;
		esac
	done < "$i"
done
