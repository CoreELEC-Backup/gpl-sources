#/bin/bash

# Set default tree locations, if you want, at the shell ENV
#GIT_TREE=$HOME/v4l-dvb
#HG_TREE=$HOME/v4l-dvb-hg

# Should be adjusted to the environment
TMP_TREE=/tmp/oldtree
KERNVER_FILE=./v4l/scripts/etc/kern_version
FIXPATCHES=./v4l/scripts/etc/fixdiffs/*
BLACKLIST=./v4l/scripts/etc/blacklist.txt
GENTREE=./v4l/scripts/gentree.pl

if [ "$1" == "--strip-dead-code" ]; then
	GENTREE_ARGS="$1"
	shift
fi

if [ "$2" != "" ]; then
	#
	# Two arguments were given. One tree should be hg and the other git
	#
	if [ -e $1/.hg/hgrc ]; then
		HG_TREE=$1
		GIT_TREE=$2
	else
		HG_TREE=$2
		GIT_TREE=$1
	fi
else
	#
	# If just one argument is selected, and it is called from one tree
	#    use the other argument for the other tree type
	#    otherwise, use default plus the given tree name
	#
	if [ "$1" != "" ]; then
		if [ -e $1/.hg/hgrc ]; then
			HG_TREE=$1
			if [ -e .git/config ]; then
				GIT_TREE=.
			fi
		elif [ -e $1/.git/config ]; then
			GIT_TREE=$1
			if [ -e .hg/hgrc ]; then
				HG_TREE=.
			fi
		fi
	fi
fi

if [ "$GIT_TREE" == "" ]; then
	echo "No git tree were provided."
	ERROR=1
fi

if [ "$HG_TREE" == "" ]; then
	echo "No mercurial tree were provided."
	ERROR=1
fi

if [ "$ERROR" !=  "" ]; then
	echo "Usage: $0 [--strip-dead-code] <tree1> [<tree2>]"
	exit -1
fi

if [ ! -e "$GIT_TREE/.git/config" ]; then
	echo "$GIT_TREE is not a git tree. Should specify a git tree to compare with the $HG_TREE mercurial tree"
	exit -1
fi

if [ ! -e "$HG_TREE/.hg/hgrc" ]; then
	echo "$HG_TREE is not a mercurial tree. Should specify -hg tree to compare with the $GIT_TREE git tree"
	exit -1
fi

echo "comparing $HG_TREE -hg tree with $GIT_TREE -git tree."


run() {
	echo $@
	$@
}

echo removing oldtree..
run rm -rf $TMP_TREE
echo creating an oldtree..
run $GENTREE $GENTREE_ARGS `cat $KERNVER_FILE` $HG_TREE/linux $TMP_TREE >/dev/null

echo applying the fix patches
for i in $FIXPATCHES; do
	echo $i
	run patch --no-backup-if-mismatch -R -d $TMP_TREE -p2 -i $i -s
	diffstat -p1 $i
done

echo removing rej/orig from $GIT_TREE
run find $GIT_TREE -name '*.rej' -exec rm '{}' \;
run find $GIT_TREE -name '*.orig' -exec rm '{}' \;

echo removing rej/orig from oldtree
run find $TMP_TREE -name '*.rej' -exec rm '{}' \;
run find $TMP_TREE -name '*.orig' -exec rm '{}' \;

echo generating "/tmp/diff"
diff -upr $TMP_TREE $GIT_TREE|grep -v ^Somente |grep -v ^Only>/tmp/diff
echo "generating /tmp/diff2 (loose diff0)"
diff -uprBw $TMP_TREE $GIT_TREE|grep -v Somente |grep -v ^Only>/tmp/diff2
echo generating /tmp/somente2 for a complete oldtree-only files
diff -upr $TMP_TREE $GIT_TREE|grep ^Somente|grep "drivers/media" |grep -vr ".o$" |grep -v ".mod.c"|grep -v ".o.cmd" |grep -v modules.order >/tmp/somente2
diff -upr $TMP_TREE $GIT_TREE|grep ^Only|grep "drivers/media" |grep -vr ".o$" |grep -v ".mod.c"|grep -v ".o.cmd" |grep -v modules.order >>/tmp/somente2

echo generating /tmp/somente for oldtree-only files
cp /tmp/somente2 /tmp/s$$
for i in `cat $BLACKLIST`; do
	cat /tmp/s$$ | grep -v "Somente.* $i" >/tmp/s2$$
	mv /tmp/s2$$ /tmp/s$$
done
mv /tmp/s$$ /tmp/somente
echo
echo diffstat -p1 /tmp/diff
diffstat -p1 /tmp/diff
