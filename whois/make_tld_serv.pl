#!/usr/bin/perl

use warnings;
use strict;

while (<>) {
	chomp;
	s/#.*$//;
	s/^\s+//; s/\s+$//;
	next if /^$/;

	die "format error: $_" if not
		(my ($a, $b) = /^\.(\w[\w\d\.-]+)\s+([\w\d\.:-]+|[A-Z]+\s+.*)$/);

	$b =~ s/^W(?:EB)?\s+/\\x01/;
	$b =~ s/^VERISIGN\s+/\\x04" "/;
	$b = "\\x03" if $b eq 'NONE';
	$b = "\\x08" if $b eq 'AFILIAS';
	$b = "\\x0C" if $b eq 'ARPA';
	print qq|    "$a",\t"$b",\n|;
}

