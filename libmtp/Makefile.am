SUBDIRS=src examples util doc
ACLOCAL_AMFLAGS=-I m4

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_DATA=libmtp.pc

EXTRA_DIST=libmtp.pc libmtp.sh COPYING README.windows.txt

# This stuff only makes sense on Linux so only
# build and ship it on Linux.
if USE_LINUX
udevrulesdir=@UDEV@/rules.d
hwdbdir=@UDEV@/hwdb.d

CLEANFILES = libmtp.usermap @UDEV_RULES@ libmtp.fdi libmtp.hwdb
endif
