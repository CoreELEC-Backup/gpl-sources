#!/usr/bin/perl

use warnings;
use strict;
use autodie;

my $changelog = $ARGV[0] or die "Usage: $0 debian/changelog\n";

open(my $fh, '<', $changelog);
my $line = <$fh>;
close($fh);

my ($ver) = $line =~ /^whois \s+ \( ( [^\)]+ ) \) \s+ \S+/x;
die "Version number not found in $changelog!\n" if not $ver;

$ver =~ s/ ( ~bpo\d+\+\d+ | \+b\d+ | ~deb\d+.* | ubuntu\d+ | \+dyson\d+ ) $//x;

# The version number must not deviate from this format or the -V option
# to RIPE-like servers will break. If needed, update the previous regexp.
die "Invalid version number in $changelog!\n"
	unless $ver =~ /^ \d+\.\d+ ( \.\d+ )? $/x;

print qq|#define VERSION "$ver"\n|;

