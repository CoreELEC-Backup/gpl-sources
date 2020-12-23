#!/usr/bin/perl
#
# Convert VDRAdmin-AM autotimer (vdradmin.at) into
# epgsearch searchtimer (epgsearch.conf)
#
# Version: 0.2beta  2006-08-06
#
# Author: Mike Constabel (vejoun @ vdrportal)
#         Andreas Mair   (amair  @ vdrportal)
#
# You need:
#
# - VDRAdmin-AM >= 3.4.6
# - Epgsearch >= 0.9.16
#

use Socket;
use Getopt::Std;

#---------------------------------------------------------------------------

$Usage = qq{
Usage: $0 [options] -i filename -o filename -m 0|1
       $0 [options] -i filename -o filename -s -m 0|1
       $0 [options] -i filename -s -m 0|1

Action:  -i filename           input file, vdradmind.at
         -o filename           output file, epgsearch.conf.new (file will be overwritten!)
         -s                    send to epgsearch via svdrp (needs running vdr with epgsearch plugin)
         -m 0|1                which new searchmode for autotimers with active status: yes, once
                               0 means the whole string must match
                               1 means every word must match [default]
                               In epgsearch 0 is default, vdradmin only use 1

Options: -r                    autotimers with regular expressions will be disabled while converting,
                               with this option you can enable the corresponding searchtimers.
                               You should not do this! Check the converted searchtimers and enable
                               them manually! Wrong searchtimers will flood your timers.
         -d host               Host for SVDRP import
         -p port               Port for SVDRP import
         -h                    Show this help text

};

die $Usage if (!getopts('i:o:srm:d:p:h') || !$opt_i || $opt_h || !($opt_o || $opt_s) || !($opt_m == 0 || $opt_m == 1));

my $AT_FILENAME		= $opt_i;
my $EPGSEARCH_FILENAME	= $opt_o;
my $SENDSVDRP		= $opt_s ? 1 : 0;
my $EnableRegex		= $opt_r ? 1 : 0;
my $Searchmode		= defined $opt_m ? $opt_m : 1;
my $Dest		= $opt_d ? $opt_d : "localhost";
my $Port		= $opt_p ? $opt_p : 6419;

my $Timeout = 30; # max. seconds to wait for svdrp  response

my $conf_str="%i:%s:%i:%s:%s:%i:%s:%i:%i:%i:%i:%i:0:::%i:%i:%s:%i:%s:%s:%s:%s:%s:0:0:0::%i:0:1:1:0:0:0:0:0:1:0:0::1";

# [ID=0], pattern, time?, starttime, stopptime, channel? (0,1), channel, match case (0,1), search mode (0,4), title, subtitle, description, [duration=0], [min=""], [max=""], searchtimer? (0,1),
# use week? (0,1), weekday "0", series? (0,1), dir, prio, lft, bstart, bstop, [VPS=0], [action=0], [use ext. epg=0], [ext. epg=""], done? (0,1), [repeats=0], [ctitle=1], [csubtitle=1], [cdescription=0],
# [cext. epg=0], [reapeats within days=0], [delete after x days=0], [keep=0], [min before switch=1], [nach x rec pausieren=0], [blacklist=0], [blacklist-id=""], [fuzzy=0]

sub AT_Read {
  my (@at);
  if (-s $AT_FILENAME) {
    open(AT_FILE, $AT_FILENAME) || printf("Can't open %s", $AT_FILENAME);
    while (<AT_FILE>) {
      chomp;
      next if ($_ eq "");
      my ($active, $pattern, $section, $start, $stop, $episode, $prio, $lft, $channel, $directory, $done, $weekday, $buffers, $bstart, $bstop) = split(/\:/, $_);
      $pattern   =~ s/\|/\:/g;
      $pattern   =~ s/\\:/\|/g;
      $directory =~ s/\|/\:/g;
      push(
        @at,
          {  active    => $active,
             pattern   => $pattern,
             section   => $section,
             start     => $start,
             stop      => $stop,
             buffers   => $buffers,
             bstart    => $bstart,
             bstop     => $bstop,
             episode   => $episode,
             prio      => $prio,
             lft       => $lft,
             channel   => $channel,
             directory => $directory,
             done      => $done,
             weekdays  => $weekday
          }
      );
    }
    close(AT_FILE);
  }
  return (@at);
}

sub CONF_Collect {
  my @at = @_;
  my @st;
  my $weekday = $stitle = $ssubtitle = $sdescription = $id = $case = 0;
  my $mode = $Searchmode;
  my $pattern;
  my $directory;

  for my $at (@at) {

    if ($at->{pattern} =~ /^\/(.*)\/(i?)$/) {
      $mode = 4;
      $case = 1 if ($2 eq "i");
      $pattern = $1;
    } else {
      $pattern = $at->{pattern};
    }
    $directory = $at->{directory};
    $pattern   =~ s/\|/\!\^pipe\^\!/g;
    $pattern   =~ s/\:/\|/g;
    $directory =~ s/\|/\!\^pipe\^\!/g;
    $directory =~ s/\:/\|/g;

    $weekday = undef;
    my $wd_bits = 0;
    if ($at->{weekdays} =~ /^(\d)(\d)(\d)(\d)(\d)(\d)(\d)$/) {
    	if ($7) { #Sunday
    	  $weekday  = 0;
	      $wd_bits |= 1;
    	}
    	if ($1) { #Monday
	      $weekday  = 1;
	      $wd_bits |= 2;
    	}
    	if ($2) { #Tuesday
  	    $weekday  = 2;
	      $wd_bits |= 4;
    	}
    	if ($3) { #Wednesday
    	  $weekday  = 3;
	      $wd_bits |= 8;
    	}
    	if ($4) { #Thursday
      	$weekday  = 4;
	      $wd_bits |= 16;
    	}
    	if ($5) { #Friday
	      $weekday  = 5;
	      $wd_bits |= 32;
    	}
    	if ($6) { #Saturday
  	    $weekday  = 6;
	      $wd_bits |= 64;
    	}
    }
    if ($wd_bits !~ /^(1|2|4|8|16|32|64)$/) {
      $weekday = -$wd_bits;
    }
    $stitle       = $at->{section} & 1 ? 1 : 0;
    $ssubtitle    = $at->{section} & 2 ? 1 : 0;
    $sdescription = $at->{section} & 4 ? 1 : 0;

    $id += 1 if ($EPGSEARCH_FILENAME);

    $data = sprintf $conf_str,
    									$id,
    									$pattern,
                      $at->{start} || $at->{stop} ? 1 : 0,
                      $at->{start} ? sprintf("%04i", $at->{start}) : "",
                      $at->{stop} ? sprintf("%04i", $at->{stop}) : "",
                      $at->{channel} ? 1 : 0,
                      $at->{channel} ? $at->{channel} : "",
                      $case,
                      $mode,
                      $stitle,
                      $ssubtitle,
                      $sdescription,
                      ($at->{active} && ($mode != 4 || $EnableRegex)) ? 1 : 0,
                      $weekday eq -127 || $wd_bits eq 0 ? 0 : 1,
                      $weekday,
                      $at->{episode} ? 1 : 0,
                      $directory,
                      $at->{prio} ? $at->{prio} : "",
                      $at->{lft} ? $at->{lft} : "",
                      $at->{buffers} ? $at->{bstart} : "",
                      $at->{buffers} ? $at->{bstop} : "",
                      $at->{done} ? 1 : 0;	#avoid_repeats
    push @st,$data;
  }
  return (@st)
}

sub Error {
  print STDERR "@_\n";
  close(SOCK);
  exit 0;
}

sub SplitLine($) {
  my ($line)=@_;
  if ($line =~ /^([0-9]{3})([- ])(.*)$/) {
    return ($1,$2,$3);
  }
  Error("Unidentified Line: $line");
}

sub WriteSearchtimerSVDRP {
  my @str = @_;

  $SIG{ALRM} = sub { Error("timeout"); };
  alarm($Timeout);

  my $iaddr = inet_aton($Dest)			|| Error("no host: $Dest");
  my $paddr = sockaddr_in($Port, $iaddr);

  my $proto = getprotobyname('tcp');
  socket(SOCK, PF_INET, SOCK_STREAM, $proto)	|| Error("socket: $!");
  connect(SOCK, $paddr)				|| Error("connect: $!");
  select(SOCK); $| = 1;
  select(STDOUT);

  while (<SOCK>) {
    chomp;
    print STDOUT "(1):$_\n";
    my ($code,$sep,$data)=SplitLine($_);
    last if ($code=220 && $sep eq ' ');
  }

  foreach my $line (@str) {
    chomp $line;
    printf SOCK "PLUG epgsearch NEWS %s\r\n", $line;
  }

  while (<SOCK>) {
    chomp;
    print STDOUT "(2):$_\n";
    my ($code,$sep,$data)=SplitLine($_);
    last if ($code==900 && $sep eq ' ');
  }

  print SOCK "quit\r\n";

  while (<SOCK>) {
    chomp;
    print STDOUT "(3):$_\n";
    my ($code,$sep,$data)=SplitLine($_);
    last if ($code==221 && $sep eq ' ');
  }

  close(SOCK)					|| Error("close: $!");
}

sub WriteSearchtimerFile {
  my @str = @_;

  open(STFILE, ">".$EPGSEARCH_FILENAME) || die("Cannot open file ${EPGSEARCH_FILENAME}!");
  print STFILE "#version 2 - DONT TOUCH THIS!\n";
  foreach my $line (@str) {
    chomp $line;
    printf STFILE "%s\n", $line;
  }
  close STFILE
}

#---------------------------------------------------------------------------
# main

my @st;

@st = CONF_Collect(AT_Read());

if ( $SENDSVDRP ) {
  WriteSearchtimerSVDRP(@st);
}

if ( $EPGSEARCH_FILENAME ) {
  WriteSearchtimerFile(@st);
}

