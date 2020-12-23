#!/usr/bin/perl
#
# Dieses Script konvertiert die epgsearchdone.data für
# epgsearch >= 0.9.14
#
# Die neue epgsearchdone.data funktioniert dann nicht mehr
# für epgsearch < 0.9.14
#
# Es werden die epgsearch-Daten aus der Summary in die
# @-Zeile verlegt.
#
# Dies ist nicht zwingend erforderlich. Wenn Du nicht sicher bist,
# lass es ;)
#
# Aufrufen:
#
#  convert_epgsearchdone_data.pl /pfad/zur/epgsearchdone.data/
#
#  z.B. convert_epgsearchdone_data.pl /etc/vdr/plugins/
#
#  "svdrpsend plug epgsearch updd" nicht vergesssen.
#
# Backup erstellen?
#
#  my $backup=1;  Auf 0 oder 1
#
# Mike Constabel vejoun @ vdrportal
# 2006-03-02
#

use strict;

my $backup=1;

die("Kein Parameter, bitte Anleitung im Script lesen.") if ! $ARGV[0];

my $Pfad=$ARGV[0];

my $DateiAlt=$Pfad."/epgsearchdone.data";
my $DateiNeu=$Pfad."/epgsearchdone.data.neu";
my $DateiBak=$Pfad."/epgsearchdone.data.bak";

open(DATEI,"<".$DateiAlt) || die("Datei nicht gefunden: $DateiAlt");
open(NEU,">".$DateiNeu) || die("Datei kann nicht geöffnet werden: $DateiAlt");

my $Zeile = "";
my $epgsearch = "";
my $neuformat = 0;
my $aux = "";

printf ("Konvertiere %s\n",$DateiAlt);

while(<DATEI>) {

  $Zeile = $_;
  $epgsearch="";
  $aux="";
  $neuformat = 0;

  print NEU $Zeile if $Zeile =~ /^[RCTSr].*/;

  $epgsearch = $1 if $Zeile =~ /^D .*(Kanal.+Suchtimer.+S-ID.+)$/;

  if ( $Zeile =~ /^(D .+)\|Kanal.+Suchtimer.+S-ID.+$/ )
  {
    print NEU sprintf("%s\n",$1);
  } elsif ( $Zeile =~ /^(D .+)$/ )
  {
    print NEU sprintf("%s\n",$1);
  }

  if ( $Zeile =~ /^(@ .+)$/ )
  {
    $neuformat=1;
    $aux = $1;
  }

  if ( $neuformat == 1 && $aux !~ /epgsearch/ && $epgsearch ne "" )
  {
    $aux .= "<epgsearch>".$epgsearch."</epgsearch>";
    print NEU sprintf("%s\n",$aux);
  } elsif ( $neuformat == 1  )
  {
    print NEU sprintf("%s\n",$aux);
  }
  print NEU sprintf("@ <epgsearch>%s</epgsearch>\n",$epgsearch) if ( $neuformat == 0 && $epgsearch ne "" );

}

close(DATEI);
close(NEU);

rename $DateiAlt, $DateiBak if ( ! -e $DateiBak && $backup == 1 );
rename $DateiNeu, $DateiAlt;

