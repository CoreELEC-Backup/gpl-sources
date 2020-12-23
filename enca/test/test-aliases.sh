#! /bin/sh
# Purpose: check for iconv charsets, namely without libiconv.  Enca used to
# keep the @...@ in alias list.
. $srcdir/setup.sh
$ENCA --name aliases --list charsets | grep '@' && DIE=1
. $srcdir/finish.sh
