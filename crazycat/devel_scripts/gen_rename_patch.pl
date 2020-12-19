#!/usr/bin/perl
use strict;

my $debug=0;
my $from;
my $to;
my $sed;

while (<>) {
	$from = $1 if (/^rename from (.*)/);
	$to = $1 if (/^rename to (.*)/);

	if ($from && $to) {
		printf "$from -> $to\n" if ($debug);
		$from =~ s/\./\\./g;
		$to =~ s/\./\\./g;
		$sed .= "\t-e 's,$from,$to,g' \\\n";
		$from = 0;
		$to = 0;
	}
}

$sed =~ s/\s*\|\s* \\\s*$//;
$sed =~ s/^\s*//;

open OUT, ">rename_patch.sh";
print OUT "#!/bin/bash\n\n";
print OUT "if [ \"\$1\" == \"\" ]; then echo \"usage: $0 <in_patch>\"; exit -1; fi\n\n";
print OUT "OLD=\"\$1.old\"\n";
print OUT "mv \$1 \$OLD && ";
print OUT "cat \$OLD | sed $sed >\$1 && \\\n";
print OUT "echo -e \"old patch stored as \"\$OLD\"\\n\$1 rewrote.\"\n";
close OUT;

chmod(0755, "rename_patch.sh");
