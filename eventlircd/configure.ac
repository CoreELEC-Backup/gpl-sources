dnl Copyright (C) 2009-2010 Paul Bender.
dnl
dnl This file is part of eventlircd.
dnl
dnl eventlircd is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation, either version 2 of the License, or
dnl (at your option) any later version.
dnl
dnl eventlircd is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with eventlircd.  If not, see <http://www.gnu.org/licenses/>.

AC_INIT([eventlircd], [0.0.0])

AC_PREREQ([2.63])

AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign dist-bzip2])

AC_PREFIX_DEFAULT([/usr])

AM_SILENT_RULES

AC_PROG_CC
AC_PROG_CC_C99
AM_PROG_CC_C_O
AC_PROG_AWK
AC_PROG_GREP
AC_PROG_SED
AC_PROG_INSTALL

dnl The program makes use of some C99 features such as stdbool.h
if test "x$ac_cv_prog_cc_c99" = "xno" ; then
    AC_MSG_ERROR([the compiler is not C99 compliant])
fi

dnl ${AWK} must support the '--non-decimal-data' option because
dnl evkey_code_to_name.h.sh and evkey_type.h use ${AWK} to process the evkey
dnl defines in linux/input.h some of which are hexidecimal rather than decimal.
if ! ${AWK} --non-decimal-data "" > /dev/null 2>&1 ; then
    AC_MSG_ERROR([${AWK} does not support the command line argument '--non-decimal-data'])
fi

PKG_PROG_PKG_CONFIG

AC_HEADER_STDC
AC_HEADER_STDBOOL
AC_HEADER_TIME

dnl
dnl Fine grain header checks. This is overkill, but better safe than sorry.
dnl
dnl Single Unix Specification Version 3 headers.
AC_CHECK_HEADER([errno.h],[],[AC_MSG_ERROR([failed to find the required header file errno.h])])               dnl C89
AC_CHECK_HEADER([fcntl.h],[],[AC_MSG_ERROR([failed to find the required header file fcntl.h])])               dnl POSIX
AC_CHECK_HEADER([signal.h],[],[AC_MSG_ERROR([failed to find the required header file signal.h])])             dnl C89
AC_CHECK_HEADER([stdbool.h],[],[AC_MSG_ERROR([failed to find the required header file stdbool.h])])           dnl C99
AC_CHECK_HEADER([stddef.h],[],[AC_MSG_ERROR([failed to find the required header file stddef.h])])             dnl C89
AC_CHECK_HEADER([stdint.h],[],[AC_MSG_ERROR([failed to find the required header file stdint.h])])             dnl POSIX
AC_CHECK_HEADER([stdio.h],[],[AC_MSG_ERROR([failed to find the required header file stdio.h])])               dnl C89
AC_CHECK_HEADER([stdlib.h],[],[AC_MSG_ERROR([failed to find the required header file stdlib.h])])             dnl C89
AC_CHECK_HEADER([string.h],[],[AC_MSG_ERROR([failed to find the required header file string.h])])             dnl C89
AC_CHECK_HEADER([sys/select.h],[],[AC_MSG_ERROR([failed to find the required header file sys/select.h])])     dnl POSIX
AC_CHECK_HEADER([sys/socket.h],[],[AC_MSG_ERROR([failed to find the required header file sys/socket.h])])     dnl POSIX
AC_CHECK_HEADER([sys/stat.h],[],[AC_MSG_ERROR([failed to find the required header file sys/stat.h])])         dnl POSIX
AC_CHECK_HEADER([sys/time.h],[],[AC_MSG_ERROR([failed to find the required header file sys/time.h])])         dnl POSIX
AC_CHECK_HEADER([sys/types.h],[],[AC_MSG_ERROR([failed to find the required header file sys/types.h])])       dnl POSIX
AC_CHECK_HEADER([sys/un.h],[],[AC_MSG_ERROR([failed to find the required header file sys/un.h])])             dnl XSI
AC_CHECK_HEADER([syslog.h],[],[AC_MSG_ERROR([failed to find the required header file syslog.h])])             dnl XSI
AC_CHECK_HEADER([unistd.h],[],[AC_MSG_ERROR([failed to find the required header file unistd.h])])             dnl POSIX
dnl Misc headers.
AC_CHECK_HEADER([getopt.h],[],[AC_MSG_ERROR([failed to find the required header file getopt.h])])
AC_CHECK_HEADER([sysexits.h],[],[AC_MSG_ERROR([failed to find the required header file sysexits.h])])
dnl Linux headers.
AC_CHECK_HEADER([linux/input.h],[],[AC_MSG_ERROR([failed to find the required header file linux/input.h])])   dnl
AC_CHECK_HEADER([linux/limits.h],[],[AC_MSG_ERROR([failed to find the required header file linux/limits.h])]) dnl
AC_CHECK_HEADER([linux/types.h],[],[AC_MSG_ERROR([failed to find the required header file linux/types.h])])   dnl
AC_CHECK_HEADER([linux/uinput.h],[],[AC_MSG_ERROR([failed to find the required header file linux/uinput.h])]) dnl
dnl libudev headers.
AC_CHECK_HEADER([libudev.h],[],[AC_MSG_ERROR([failed to find the required header file libudev.h])])

dnl The absolute path to the header file linux/input.h is needed by 
dnl event_name_to_code.h.sh, evkey_code_to_name.h.sh and evkey_type.h.sh
dnl in order for them to generate the header files event_name_to_code.h,
dnl evkey_code_to_name.h and evkey_type.h respectively
AX_ABSOLUTE_HEADER(linux/input.h)
AX_ABSOLUTE_HEADER(linux/input-event-codes.h)
if test "x$gl_cv_absolute_linux_input_event_codes_h" = "x" ; then
  if test "x$gl_cv_absolute_linux_input_h" = "x" ; then
      AC_MSG_ERROR([failed to find the absolute path to header file linux/input.h or linux/input-event-codes.h])
  fi
  AC_SUBST(ABSOLUTE_LINUX_INPUT_H, $gl_cv_absolute_linux_input_h)
else
  AC_SUBST(ABSOLUTE_LINUX_INPUT_H, $gl_cv_absolute_linux_input_event_codes_h)
fi

PKG_CHECK_MODULES(LIBUDEV, [libudev >= 136])

AC_ARG_WITH(lircd-socket, AS_HELP_STRING([--with-lircd-socket=SOCKET], [lircd socket @<:@LOCALSTATEDIR/run/lirc/lircd@:>@]),
    [LIRCD_SOCKET="$withval"],
    [LIRCD_SOCKET="${localstatedir}/run/lirc/lircd"])
AC_SUBST(LIRCD_SOCKET)

AC_ARG_WITH(evmap-dir, AS_HELP_STRING([--with-evmap-dir=DIR], [event map directory @<:@SYSCONFDIR/eventlircd.d@:>@]),
    [EVMAP_DIR="$withval"],
    [EVMAP_DIR="${sysconfdir}/eventlircd.d"])
AC_SUBST(EVMAP_DIR)

AC_ARG_WITH(udev-dir, AS_HELP_STRING([--with-udev-dir=DIR], [udev directory @<:@/lib/udev@:>@]),
    [UDEV_DIR="$withval"],
    [UDEV_DIR="/lib/udev"])
AC_SUBST(UDEV_DIR)

dnl Only add an optimization flag when one is not already present.
if ! echo " ${CC} ${CFLAGS} " | ${GREP} ' -O@<:@^ @:>@* ' > /dev/null ; then
    AX_C_CHECK_FLAG([-O2],[],[],[CFLAGS="$CFLAGS -O2"],[])
fi
AX_C_CHECK_FLAG([-flto],[],[],[CFLAGS="$CFLAGS -flto"],[])
AX_C_CHECK_FLAG([-fvisibility=hidden],[],[],[CFLAGS="$CFLAGS -fvisibility=hidden"],[])
AX_C_CHECK_FLAG([-Werror],[],[],[CFLAGS="$CFLAGS -Werror"],[])
AX_C_CHECK_FLAG([-Wall],[],[],[CFLAGS="$CFLAGS -Wall"],[])
AX_C_CHECK_FLAG([-Wextra],[],[],[CFLAGS="$CFLAGS -Wextra"],[])
AX_C_CHECK_FLAG([-Wcast-qual],[],[],[CFLAGS="$CFLAGS -Wcast-qual"],[])
dnl This flag is commented out because is causes compilation to fail due to
dnl questionable implicit conversions in some Linux and glibc header files.
dnl AX_C_CHECK_FLAG([-Wconversion],[],[],[CFLAGS="$CFLAGS -Wconversion"],[])
AX_C_CHECK_FLAG([-Wfloat-equal],[],[],[CFLAGS="$CFLAGS -Wfloat-equal"],[])
AX_C_CHECK_FLAG([-Wformat=2],[],[],[CFLAGS="$CFLAGS -Wformat=2"],[])
AX_C_CHECK_FLAG([-Winit-self],[],[],[CFLAGS="$CFLAGS -Winit-self"],[])
AX_C_CHECK_FLAG([-Wmissing-include-dirs],[],[],[CFLAGS="$CFLAGS -Wmissing-include-dirs"],[])
AX_C_CHECK_FLAG([-Wshadow],[],[],[CFLAGS="$CFLAGS "],[-Wshadow])
AX_C_CHECK_FLAG([-Wstrict-overflow=5],[],[],[CFLAGS="$CFLAGS -Wstrict-overflow=5"],[])
AX_C_CHECK_FLAG([-Wundef],[],[],[CFLAGS="$CFLAGS -Wundef"],[])
AX_LD_CHECK_FLAG([-flto],[],[],[LDFLAGS="$LDFLAGS -flto"],[])
AX_LD_CHECK_FLAG([-fuse-ld=gold],[],[],[LDFLAGS="$LDFLAGS -fuse-ld=gold"],[])
AX_LD_CHECK_FLAG([-fuse-linker-plugin],[],[],[LDFLAGS="$LDFLAGS -fuse-linker-plugin"],[])
AX_LD_CHECK_FLAG([-fwhole-program],[],[],[LDFLAGS="$LDFLAGS -fwhole-program"],[])
AX_LD_CHECK_FLAG([-fvisibility=hidden],[],[],[LDFLAGS="$LDFLAGS -fvisibility=hidden"],[])
AX_LD_CHECK_FLAG([-Wl,--as-needed],[],[],[LDFLAGS="$LDFLAGS -Wl,--as-needed"],[])

AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_FILES([Makefile etc/Makefile man/Makefile man/eventlircd.8 man/eventlircd.evmap.5 src/Makefile src/event_name_to_code.h.sh src/evkey_code_to_name.h.sh src/evkey_type.h.sh udev/Makefile udev/lircd_helper udev/wakeup_enable udev/rules.d/98-lircd.rules.disabled udev/rules.d/98-eventlircd.rules.disabled])
AC_OUTPUT
