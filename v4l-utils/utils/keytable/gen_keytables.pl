#!/usr/bin/perl
use strict;
use File::Find;

my @ir_files = (
	"drivers/media/usb/dvb-usb/af9005-remote.c",
	"drivers/media/usb/dvb-usb/az6027.c",
	"drivers/media/usb/dvb-usb/cinergyT2-core.c",
	"drivers/media/usb/dvb-usb/dibusb-common.c",
	"drivers/media/usb/dvb-usb/digitv.c",
	"drivers/media/usb/dvb-usb/m920x.c",
	"drivers/media/usb/dvb-usb/nova-t-usb2.c",
	"drivers/media/usb/dvb-usb/opera1.c",
	"drivers/media/usb/dvb-usb/vp702x.c",
);

my $debug = 1;
my $dir="rc_keymaps";
my $deftype = "unknown";

my $keyname="";
my $out;
my $read=0;
my $type = $deftype;
my $variant = $deftype;
my $check_type = 0;
my $name;
my $warn;
my $warn_all;
my %rc_map_names;

my $kernel_dir = shift or die "Need a file name to proceed.";

sub flush($$)
{
	my $filename = shift;
	my $legacy = shift;
	my $defined;

	return if (!$keyname || !$out);
	print "Creating $dir/$keyname.toml\n";
	open OUT, ">$dir/$keyname.toml";
	print OUT "[[protocols]]\n";
	print OUT "name = \"$keyname\"\n";
	print OUT "protocol = \"$type\"\n";
	if ($type eq "nec" || $type eq "rc5" || $type eq "rc6" || $type eq "sony") {
		print OUT "variant = \"$variant\"\n";
	}
	print OUT "[protocols.scancodes]\n";
	print OUT $out;
	close OUT;

	if (!$name && !$legacy) {
		$warn++;
	} else {
		$defined = 1 if ($rc_map_names{$name});
	}

	if ($defined) {
		printf OUT_MAP "*\t%-24s %s.toml\n", $rc_map_names{$name} , $keyname;
	} else {
		my $fname = $filename;
		$fname =~ s,.*/,,;
		printf OUT_MAP "# *\t*\t\t\t %-20s # found in %s\n", "$keyname.toml", $fname;
	}

	$keyname = "";
	$out = "";
	$type = $deftype;
	$name = "";
}

sub parse_file($$)
{
	my $filename = shift;
	my $legacy = shift;

	my $num_tables = 0;
	my %scancodes = ();
	$warn = 0;

	next if ($filename =~ m/\.mod.c/);

	printf "processing file $filename\n" if ($debug);
	open IN, "<$filename" or die "couldn't find $filename";
	# read the entire file
	my $file = do { local $/ = undef; <IN> };
	close IN;

	# remove comments
	$file =~ s,/\*.*?\*/,,sg;
	$file =~ s,//[^\n]*,,sg;

	my @lines = split /\n/, $file;

	foreach (@lines) {
		if (m/struct\s+rc_map_table\s+(\w[\w\d_]+)/) {
			flush($filename, $legacy);

			$keyname = $1;
			$keyname =~ s/^rc_map_//;
			$keyname =~ s/_table$//;
			$read = 1;
			$num_tables++;
			%scancodes = ();
			next;
		}
		if (m/struct\s+rc_map_list.*=\s+{/) {
			$check_type = 1;
			next;
		}
		if ($check_type) {
			if (m/\.name\s*=\s*(RC_MAP_[^\s\,]+)/) {
				$name = $1;
				$keyname = $1;
				$keyname =~ s/^RC_MAP_//;
				$keyname =~ tr/A-Z/a-z/;
				$keyname =~ s/_table$//;
			}

			if (m/^\s*}/) {
				$check_type = 0;
				next;
			}
			if (m/RC_PROTO_([\w\d_]+)/) {
				$variant = lc $1;
				$type = $1;

				# Proper name the RC6 protocol
				$type =~ s/^RC6_MCE$/RC6/;

				# Proper name the RC-5-SZ protocol
				$type =~ s/^RC5_SZ$/RC-5-SZ/;

				# NECX protocol variant uses nec decoder
				$type =~ s/^NECX$/NEC/;

				# NEC32 protocol variant uses nec decoder
				$type =~ s/^NEC32$/NEC/;
				$type = lc $type;
			}
			next;
		}

		if ($read) {
			if (m/(0x[\dA-Fa-f]+)[\s\,]+(KEY|BTN)(\_[^\s\,\}]+)/) {
				my $scancode = hex($1);
				my $keycode = "$2$3";

				if (exists($scancodes{$scancode})) {
					printf STDERR "WARNING: duplicate scancode $1 in file $filename, set to $keycode and $scancodes{$scancode}\n";
				} else {
					$out .= "$1 = \"$keycode\"\n";
					$scancodes{$scancode} = $keycode;
				}
				next;
			}
			if (m/\}/) {
				$read = 0;
			}
		}
	}

	flush($filename, $legacy);

	printf STDERR "WARNING: keyboard name not found on %d tables at file $filename\n", $warn if ($warn);
	print STDERR "WARNING: no tables found at $filename\n" if (!$num_tables);

	$warn_all += $warn;
}

sub parse_dir()
{
	my $file = $File::Find::name;

	return if ($file =~ m/^\./);

	return if (! ($file =~ m/\.c$/));

	parse_file $file, 0;
}

sub sort_dir()
{
	sort @_;
}

sub parse_rc_map_names($)
{
	my $filename = shift;

	$warn = 0;

	printf "processing file $filename\n" if ($debug);
	open IN, "<$filename" or die "couldn't find $filename";
	while (<IN>) {
		if (m/^\s*\#define\s+(RC_MAP[^\s]+)\s+\"(.*)\"/) {
			$rc_map_names{$1} = $2;
		}
	}
}

# Main logic
#

parse_rc_map_names "$kernel_dir/include/media/rc-map.h";

open OUT_MAP, ">rc_maps.cfg";
print OUT_MAP << "EOF";
#
# Keymaps table
#
# This table creates an association between a keycode file and a kernel
# driver. It can be used to automatically override a keycode definition.
#
# Although not yet tested, it is mented to be added at udev.
#
# To use, you just need to run:
#	./ir-keytable -a
#
# Or, if the remote is not the first device:
#	./ir-keytable -a -s rc1		# for RC at rc1
#

# Format:
#	driver - name of the driver provided via uevent - use * for any driver
#	table -  RC keymap table, provided via uevent - use * for any table
#	file - file name. If directory is not specified, it will default to
#		/etc/rc_keymaps.
# For example:
# driver	table				file
# cx8800	*				./keycodes/rc5_hauppauge_new.toml
# *		rc-avermedia-m135a-rm-jx	./keycodes/kworld_315u.toml
# saa7134	rc-avermedia-m135a-rm-jx	./keycodes/keycodes/nec_terratec_cinergy_xs.toml
# em28xx	*				./keycodes/kworld_315u.toml
# *		*				./keycodes/rc5_hauppauge_new.toml

# Table to automatically load the rc maps for the bundled IR's provided with the
# devices supported by the linux kernel

#driver table                    file
EOF

find({wanted => \&parse_dir, preprocess => \&sort_dir, no_chdir => 1}, "$kernel_dir/drivers/media/rc/keymaps");

foreach my $file (@ir_files) {
	parse_file "$kernel_dir/$file", 1;
}

printf STDERR "WARNING: there are %d tables not defined at rc_maps.h\n", $warn_all if ($warn_all);
close OUT_MAP;
