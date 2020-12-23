#!/bin/sh

wd=`pwd`
if [ -f ../sh_util/Makefile.in ] ; then
	cd ..
fi
for f in ncftp libncftp sh_util vis sio Strn ; do
	if [ ! -f "$f" ] && [ ! -d "$f" ] ; then
		echo "Missing directory $f ?" 1>&2
		exit 1
	fi
done

TMPDIR=/tmp
TAR=""
TARFLAGS=""
GZIPFLAGS="-9"

if [ "$#" -lt 2 ] ; then
	TARDIR="ncftp"
	STGZFILE="$TARDIR.tar.gz"
else
	TARDIR="$1"
	STGZFILE="$2"
	if [ "$#" -eq 4 ] ; then
		# I.e., called from Makefile
		TAR="$3"
		TARFLAGS="$4"
	fi
fi


XZ=`which xz 2>/dev/null`
if [ -z "$XZ" ] ; then
	XZ=":"
	BZIP=`which bzip2 2>/dev/null`	# Only look for ol' bzip unless we don't have xz
fi
[ -z "$BZIP" ] && BZIP=":"

SXGZFILE=`echo "$STGZFILE" | sed 's/\.tar\.gz/.tar.xz/g'`
SBGZFILE=`echo "$STGZFILE" | sed 's/\.tar\.gz/.tar.bz2/g'`
ZIPFILE=`echo "$STGZFILE" | sed 's/\.tar\.gz/.zip/g'`
rm -rf $TMPDIR/TAR
mkdir -p -m755 $TMPDIR/TAR/$TARDIR 2>/dev/null

chmod 755 configure sh/*

find . -depth -follow -type f | sed '
/\/autoconf$/d
/\/autoheader$/d
/\/\./d
/\/samples/d
/libncftp\/configure$/d
/sio\/configure$/d
/Strn\/configure$/d
/\.git$/d
/\.svn$/d
/DerivedData/d
/xcuserdata/d
/\.xcuserstate/d
/\.o$/d
/\.so$/d
/\.dylib$/d
/\.a$/d
/\.lib$/d
/\.la$/d
/\.lo$/d
/\.ncb$/d
/\.suo$/d
/\.user$/d
/\.msi$/d
/\.pdb$/d
/\.idb$/d
/\.pch$/d
/\.gch$/d
/\.cpch$/d
/\.dSYM/d
/\.ipa$/d
/\.hmap$/d
/SunWS_cache/d
/\.ilk$/d
/\.res$/d
/\.aps$/d
/\.opt$/d
/\.plg$/d
/\.obj$/d
/\.exe$/d
/\.app$/d
/\.zip$/d
/\.gz$/d
/\.xz$/d
/\.bz2$/d
/\.tgz$/d
/\.tar$/d
/\.swp$/d
/\.orig$/d
/\.rej$/d
/\/\._/d
/\/Makefile\.bin$/d
/\.bin$/d
/\/bin/d
/\/core$/d
/\/ccdv$/d
/\/[Rr]elease$/d
/\/[Dd]ebug$/d
/\/sio\/.*\//d
/shit/d
/\/upload/d
/\/Strn\.version/d
/\/sio\.version/d
/\/config\.h\.in$/p
/\/config\.guess$/p
/\/config\.sub$/p
/\/config\./d
/\/configure\.in/p
/\/configure\./d
/\/Makefile$/d
/\/OLD/d
/\/old/d' | cut -c3- > "$wd/doc/manifest"

if [ -f "$wd/sh/unix2dos.sh" ] ; then
	cp "$wd/doc/manifest" "$wd/doc/manifest.txt" 
	$wd/sh/unix2dos.sh "$wd/doc/manifest.txt"
fi

cpio -Lpdm $TMPDIR/TAR/$TARDIR < "$wd/doc/manifest"
chmod -R a+rX "$TMPDIR/TAR/$TARDIR"

find $TMPDIR/TAR/$TARDIR -type f '(' -name '*.[ch]' -or -name '*.[ch]pp' -or -name '*.in' ')' -exec $wd/sh/dos2unix.sh {} \;

if [ "$TAR" = "" ] || [ "$TARFLAGS" = "" ] ; then
	x=`tar --help 2>&1 | sed -n 's/.*owner=NAME.*/owner=NAME/g;/owner=NAME/p'`
	case "$x" in
		*owner=NAME*)
			TARFLAGS="-c --owner=bin --group=bin -f"
			TAR=tar
			;;
		*)
			TARFLAGS="cf"
			TAR=tar
			x2=`gtar --help 2>&1 | sed -n 's/.*owner=NAME.*/owner=NAME/g;/owner=NAME/p'`
			case "$x2" in
				*owner=NAME*)
					TARFLAGS="-c --owner=bin --group=bin -f"
					TAR=gtar
					;;
			esac
			;;
	esac
fi

( cd $TMPDIR/TAR ; zip -q -r -o -9 $ZIPFILE $TARDIR )
cp $TMPDIR/TAR/$ZIPFILE .

( cd $TMPDIR/TAR ; $TAR $TARFLAGS - $TARDIR | ${GZIP-gzip} $GZIPFLAGS -c > $STGZFILE )
cp $TMPDIR/TAR/$STGZFILE .
[ -f "$ZIPFILE" ] && touch -r "$ZIPFILE" "$STGZFILE"

if [ "$BZIP" != ":" ] ; then
	( cd $TMPDIR/TAR ; $TAR $TARFLAGS - $TARDIR | $BZIP $BZIPFLAGS -c > $SBGZFILE )
	cp $TMPDIR/TAR/$SBGZFILE .
	[ -f "$ZIPFILE" ] && touch -r "$ZIPFILE" "$SBGZFILE"
fi

if [ "$XZ" != ":" ] ; then
	( cd $TMPDIR/TAR ; $TAR $TARFLAGS - $TARDIR | $XZ $XZFLAGS -c > $SXGZFILE )
	cp $TMPDIR/TAR/$SXGZFILE .
	[ -f "$ZIPFILE" ] && touch -r "$ZIPFILE" "$SXGZFILE"
fi

chmod 644 "$STGZFILE" "$SBGZFILE" "$SXGZFILE" "$ZIPFILE" 2>/dev/null
rm -rf $TMPDIR/TAR
for tgz in "$STGZFILE" "$SBGZFILE" "$SXGZFILE" "$ZIPFILE" ; do
	if [ -f "$tgz" ] && [ -s "$tgz" ] ; then
		touch -r ncftp/version.c "$tgz"
	fi
done
echo "* Finished archiving at `date`."
/bin/ls -l -d ncftp/version.c "$STGZFILE" "$SBGZFILE" "$SXGZFILE" "$ZIPFILE" 2>/dev/null
exit 0
