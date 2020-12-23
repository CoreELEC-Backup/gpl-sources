#!/usr/bin/perl -w
#
# extract VDR channel ID's from channels conf
#

use strict;
use File::Path;

my $channelsConf = "../channels.conf";
my $Usage = qq{
Usage: $0 (default: ../channels.conf)
       $0 /path/to/channels.conf
};

$channelsConf  = "@ARGV" if @ARGV;

#my $file;
open(MYINPUTFILE, "<$channelsConf") or die("Could not open $channelsConf" . $Usage);

foreach (<MYINPUTFILE>)
{
    chomp;
    if ( /^:.*/ )
    {
        print $_ . "\n";
        next;
    }

    my($line) = $_;

    my(@tokens) = split(":");

    my($chanID) = "$tokens[3]-$tokens[10]-$tokens[11]-$tokens[9] $tokens[0]";

    print $chanID . "\n"

}

