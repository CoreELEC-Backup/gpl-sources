#!/bin/sh

VERSION="0.5"

SUFFIX=
if test "x$1" = x--suffix; then
	shift
	SUFFIX="-$1"
	shift
fi
OUT="$1"

if test "x$SUFFIX" != 'x'; then
	v="$VERSION$SUFFIX"
else
	v="$VERSION"
fi

echo "const char rfkill_version[] = \"$v\";" > "$OUT"
