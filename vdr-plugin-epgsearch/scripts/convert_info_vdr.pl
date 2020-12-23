#!/usr/bin/perl
#
# Dieses Script konvertiert die info.vdrs in das Format, das
# vdr >= 1.3.44 und epgsearch >= 0.9.14 verwenden.
#
# Aufrufen:
#
#  find "/video" -path "*.rec" -exec convert_info_vdr.pl "{}" \;
#
# Bitte die beiden Variablen unter Einstellungen anpassen!
#
# Mike Constabel vejoun @ vdrportal
# 2006-03-01
#

use strict;

## <Einstellungen>

my $backup=1;  # Backup erstellen? 1=Ja (info.bak), 0=Nein
my $keepdata=1; # Die epgsearch-Daten in summary behalten? 1=Ja, 0=Nein

## </Einstellungen>

die("Kein Parameter, bitte Anleitung im Script lesen.") if ! $ARGV[0];

my $Pfad=$ARGV[0];

my $DateiAlt=$Pfad."/info.vdr";
my $DateiNeu=$Pfad."/info.neu";
my $DateiBak=$Pfad."/info.bak";

open(DATEI,"<".$DateiAlt) || die("Datei nicht gefunden: $DateiAlt");
open(NEU,">".$DateiNeu) || die("Datei kann nicht geöffnet werden: $DateiAlt");

my $Zeile = "";
my $epgsearch = "";
my $neuformat = 0;
my $aux = "";

printf ("Konvertiere %s\n",$DateiAlt);

while(<DATEI>) {

  $Zeile = $_;

  print NEU $Zeile if $Zeile =~ /^[CETSX] .+/;

  $epgsearch = $1 if $Zeile =~ /^D .*(Kanal.+Suchtimer.+S-ID.+)$/;

  if ( $Zeile =~ /^(D .+)\|Kanal.+Suchtimer.+S-ID.+$/ && ! $keepdata ) {
    print NEU sprintf("%s\n",$1);
  } elsif ( $Zeile =~ /^(D .+)$/ ) {
    print NEU sprintf("%s\n",$1);
  }

  if ( $Zeile =~ /^(@ .+)$/ ) {
    $neuformat=1;
    $aux = $1;
  }

}

if ( $neuformat == 1 && $aux !~ /epgsearch/ && $epgsearch ne "" ) {
  $aux .= "<epgsearch>".$epgsearch."</epgsearch>";
  print NEU sprintf("%s\n",$aux);
} elsif ( $neuformat == 1  ) {
  print NEU sprintf("%s\n",$aux);
}

print NEU sprintf("@ <epgsearch>%s</epgsearch>\n",$epgsearch) if ( $neuformat == 0 && $epgsearch ne "" );

close(DATEI);
close(NEU);

rename $DateiAlt, $DateiBak if ( ! -e $DateiBak && $backup == 1 );
rename $DateiNeu, $DateiAlt;

