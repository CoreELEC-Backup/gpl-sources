#!/usr/bin/perl

# Copyright (C) 2006 Trent Piepho <xyzzy@speakeasy.org>
# Scans a tree of Linux Kernel style Makefiles, and builds lists of what
# builds what.
# Outputs three lists:
# 1.  Kconfig variables associated with the kernel module(s) they turn on
# 2.  Kernel modules associated with their source file(s)
# 3.  Kconfig variables associated with all the source file(s) they turn on
#
# Optional options:
#   Prefix relative to source tree root to start scanning in.  This
#   will be stripped from the beginnings of all filenames printed out.
#     Default is 'linux/drivers/media'
#   Root of source tree
#     Default is to use 'hg root' command and if that fails the cwd
#
# Most usefull with grep, for example
# List of modules and source files used by DVB_BUDGET_AV
# deps.pl | grep DVB_BUDGET_AV
#
# Kconfig variable and kernel module that use dvb-usb-init.c
# deps.pl | grep dvb-usb-init.c
#
# Kconfig variable and source files that make dvb-core.ko
# deps.pl | grep dvb-core.ko
#
# Also has some ability to check Makefiles for errors
use strict;
use FileHandle;
use Getopt::Long;
use Pod::Usage;

# Controls what will be displayed
my $check;
my $show_modules;
my $show_files_per_module;
my $show_files_per_config;
my $show_all;
my $help;
my $man;

# Directory to start in.  Will be stripped from all filenames printed out.
my $prefix = 'linux/drivers/media/';

GetOptions(
	"path=s" => \$prefix,
	"extra_check" => \$check,
	"show_modules" => \$show_modules,
	"show_files_per_module" => \$show_files_per_module,
	"show_files_per_config" => \$show_files_per_config,
	"show_all" => \$show_all,
	'help|?' => \$help,
	man => \$man
) or pod2usage(2);

pod2usage(1) if $help;
pod2usage(-exitstatus => 0, -verbose => 2) if $man;

if ($show_all) {
	$show_modules = 1;
	$show_files_per_module = 1;
	$show_files_per_config = 1;
}

# List of Makefile's opened
my %makefiles = ();

# For each Kconfig variable, a list of modules it builds
my %config = ();

# For each module that is made up of multiple source files, list of sources
my %multi = ();

sub open_makefile($) {
	my $file = shift;

	# only open a given Makefile once
	return if exists $makefiles{$file};
	$makefiles{$file} = 1;

	$file =~ m|^(.*)/[^/]*$|;
	my $dir = $1;

	#    print STDERR "opening $root$file (dir=$dir)\n";
	my $in = new FileHandle;
	open $in, '<', "$file" or die "Unable to open Makefile '$file': $!";

	while (<$in>) {
		# print STDERR "Line: $_";
		# Skip comment and blank lines
		next if (/^\s*(#.*)?$/);
		m/^\s*\-?include/ and die "Can't handle includes! In $file";

		# Handle line continuations
		if (/\\\n$/) {
			$_ .= <$in>;
			redo;
		}
		# Eat line continuations in string we will parse
		s/\s*\\\n\s*/ /g;
		# Eat comments
		s/#.*$//;

		if (/^\s*ccflags-(.*)?\s*([:+]?)=\s*(\S.*?)\s*$/) {
			if ($check) {
				print STDERR "Should use '+=' with ccflags-$1 in $file:$.\n$_\n" if ($2 ne "+");
			}
			next;
		}
		if (/^\s*obj-(\S+)\s*([:+]?)=\s*(\S.*?)\s*$/) {
			print STDERR "Should use '+=' in $file:$.\n$_\n" if ($check && $2 ne '+');
			my ($var,$targets) = ($1, $3);
			if ($var =~ /\$\(CONFIG_(\S+)\)$/) {
				$var = $1;
			} elsif ($var !~ /^[ym]$/) {
				print STDERR "Confused by obj assignment '$var' in $file:$.\n$_";
			}
			foreach(split(/\s+/, $targets)) {
				if (m|/$|) { # Ends in /, means it's a directory
					open_makefile("$dir/$_".'Makefile');
				} elsif (/^(\S+)\.o$/) {
					push @{$config{$var}}, "$dir/$1";
				} else {
					print STDERR "Confused by target '$_' in $file:$.\n";
				}
			}
			next;
		}
		if (/(\S+)-objs\s*([:+]?)\s*=\s*(\S.*?)\s*$/) {
			my @files = split(/\s+/, $3);
			foreach my $f (@files) {
				$f =~ s|^(.*)\.o$|$dir/$1|;
			}
			if ($2 eq '+') {
				# Adding to files
				print STDERR "Should use ':=' in $file:$.\n$_\n" if ($check && !exists $multi{"$dir/$1"});
				push @files, split(/\s+/, $multi{"$dir/$1"});
			} else {
				print STDERR "Setting objects twice in $file:$.\n$_\n" if ($check && exists $multi{"$dir/$1"});
			}
			$multi{"$dir/$1"} = "@files";
			next;
		}
		if (/^\s*(\S+)-[ym]?\s*([:+]?)\s*=\s*(\S.*?)\s*$/) {
			my @files = split(/\s+/, $3);
			foreach my $f (@files) {
				$f =~ s|^(.*)\.o$|$dir/$1|;
			}
			if ($2 eq '+') {
				# Adding to files
				print STDERR "Should use ':=' in $file:$.\n$_\n" if ($check && !exists $multi{"$dir/$1"});
				push @files, split(/\s+/, $multi{"$dir/$1"});
			} else {
				print STDERR "Setting objects twice in $file:$.\n$_\n" if ($check && exists $multi{"$dir/$1"});
			}
			$multi{"$dir/$1"} = "@files";
			next;
		}
		if (/^\s*(\S+)-\$\((\S+)\)\s*([:+]?)\s*=\s*(\S.*?)\s*$/) {
			print STDERR "Should use '+=' in $file:$.\n$_\n" if ($check && $3 ne '+');
			my ($var, $files, $targets) = ($2, $3, $4);
			$multi{"$dir/$1"} = $files;
			foreach(split(/\s+/, $targets)) {
				if (m|/$|) { # Ends in /, means it's a directory
					open_makefile("$dir/$_".'Makefile');
				} elsif (/^(\S+)\.o$/) {
					push @{$config{$var}}, "$dir/$1";
				} else {
					print STDERR "Confused by target '$_' in $file:$.\n";
				}
			}
			next;
		}
		if (/^\s*EXTRA_CFLAGS\s*([:+]?)\s*=\s*(\S.*?)\s*$/) {
			if ($check) {
				sub allI { /^-I/ or return 0 foreach split(/\s+/, $_[0]);return 1; }
				my $use = allI($2) ? ':' : '+';
				print STDERR "Should use '$use=' with EXTRA_CFLAGS in $file:$.\n$_\n" if ($1 ne $use);
			}
			next;
		}
		print STDERR "Odd line $file:$.\n$_\n" if ($check);
	}
}

open_makefile($prefix."Makefile");

if ($show_modules) {
	print "# Kconfig variable = kernel modules built\n";
	foreach my $var (keys %config) {
		my @list = @{$config{$var}};
		map { s/^$prefix(.*)$/\1.ko/ } @list;
		printf "%-22s= %s\n", $var, join(' ', @list);
	}
}

if ($show_files_per_module) {
	print "\n# kernel module = source files\n";
	my %modules = ();
	foreach my $mods (values %config) {
		$modules{$_} = 1 foreach @$mods;
	}
	foreach (keys %modules) {
		/$prefix(.*)$/;
		printf "%-30s = ", "$1.ko";
		if (exists $multi{$_}) {
			my @list = split(/\s+/, $multi{$_});
			map { s/^$prefix(.*)$/\1.c/ } @list;
			print join(' ', @list), "\n";
		} else {
			print "$1.c\n";
		}
	}
}

if ($show_files_per_config) {
	print "\n# Kconfig varible = source files\n";
	while (my ($var, $list) = each %config) {
		my @outlist = ();
		foreach (@$list) {
			if (exists $multi{$_}) {
				push @outlist, split(/\s+/, $multi{$_});
			} else {
				push @outlist, $_;
			}
		}
		map { s/^$prefix(.*)$/\1.c/ } @outlist;
		printf "%-22s= %s\n", $var, join(' ', @outlist);
	}
}
exit;

__END__

=head1 NAME

analyze_build.pl - Analyze the Kernel Makefiles to detect its config

=head1 SYNOPSIS

B<analyze_build.pl> [--path path] [--extra_check] [--show_modules]
	[--show_files_per_module] [--show_files_per_config]

=head1 OPTIONS

=over 8

=item B<--path> path

Path for the Kernel sub-tree to check. Default: linux/drivers/media.

=item B<--extra_check>

Enable extra checks

=item B<--show_modules>

Show modules (.ko files) and their corresponding Kconfig option

=item B<--show_files_per_module>

Show C source files for each module (.ko file)

=item B<--show_files_per_config>

Show C source files for each Kconfig option

=item B<--show_all>

Equivalent to  B<--show_modules> B<--show_files_per_module B<--show_files_per_config>

=item B<--help>

Prints a brief help message and exits.

=item B<--man>

Prints the manual page and exits.

=back

=head1 DESCRIPTION

B<search_msg.pl> talk with an IMAP server to read messages with
patches from it.

=head1 BUGS

Report bugs to <linux-media@vger.kernel.org>

=head1 COPYRIGHT

Copyright (c) 2006 Trent Piepho <xyzzy@speakeasy.org>

Copyright (c) 2012 by Mauro Carvalho Chehab

License GPLv2: GNU GPL version 2 <http://gnu.org/licenses/gpl.html>.

This is free software: you are free to change and redistribute it.
There is NO WARRANTY, to the extent permitted by law.

=cut
