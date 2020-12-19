dnl library version number
m4_define([udfread_major], 1)
m4_define([udfread_minor], 1)
m4_define([udfread_micro], 0)
m4_define([udfread_version],[udfread_major.udfread_minor.udfread_micro])

dnl shared library version (.so version)
dnl
dnl - If there are no interface changes, increase revision.
dnl - If interfaces have been added, increase current and age. Set revision to 0.
dnl - If interfaces have been changed or removed, increase current and set age and revision to 0.
dnl
dnl Library file name will be libbluray.so.(current-age).age.revision
dnl
m4_define([lt_current],  1)
m4_define([lt_revision], 0)
m4_define([lt_age],      1)

dnl initilization
AC_INIT([libudfread], udfread_version, [http://www.videolan.org/developers/libudfread.html])
AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_MACRO_DIR([m4])
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE([foreign tar-ustar dist-bzip2 no-dist-gzip subdir-objects])
AC_CONFIG_HEADERS(config.h)

dnl Enable silent rules only when available (automake 1.11 or later).
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

dnl os type
case "${host_os}" in
  "")
    SYS=unknown
    ;;
  *mingw32* | *cygwin* | *wince* | *mingwce*)
    case "${host_os}" in
      *wince* | *mingwce* | *mingw32ce*)
        SYS=mingwce
        ;;
      *mingw32*)
        SYS=mingw32
        ;;
    esac
    ;;
  *)
    SYS="${host_os}"
    ;;
esac

dnl configure options
AC_ARG_ENABLE([optimizations],
  [AS_HELP_STRING([--disable-optimizations], [disable optimizations @<:@default=enabled@:>@])])
AC_ARG_ENABLE([extra-warnings],
  [AS_HELP_STRING([--disable-extra-warnings], [set extra warnings @<:@default=enabled@:>@])])

dnl required programs
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL

LT_INIT

dnl required system services
AC_SYS_LARGEFILE

dnl required headers
AC_CHECK_HEADERS([unistd.h fcntl.h pthread.h])

dnl required functions
AS_IF([test "${SYS}" != "mingw32"], [
  AC_CHECK_FUNC([pread],, [AC_MSG_ERROR("Function pread not found.")])])

dnl compiler warnings
CC_CHECK_CFLAGS_APPEND([-Wall -Wsign-compare -Wextra])
CC_CHECK_CFLAGS_APPEND([-std=c99 -pedantic])
CC_CHECK_CFLAGS_APPEND([-Wdisabled-optimization -Wpointer-arith ]dnl
[-Wredundant-decls -Wcast-qual -Wwrite-strings -Wtype-limits -Wundef ]dnl
[-Wmissing-prototypes -Wshadow])
CC_CHECK_CFLAGS_APPEND([-Werror=implicit-function-declaration ]dnl
                       [-Werror-implicit-function-declaration],
                       [break;])

AS_IF([test "x$enable_extra_warnings" != "xno"], [
  CC_CHECK_CFLAGS_APPEND([-Wextra -Winline])
])

AS_IF([test "x$enable_optimizations" != "xno"], [
  CC_CHECK_CFLAGS_APPEND([-O3 -fomit-frame-pointer])
])

dnl export library version number
UDFREAD_VERSION_MAJOR=udfread_major()
AC_SUBST(UDFREAD_VERSION_MAJOR)
UDFREAD_VERSION_MINOR=udfread_minor()
AC_SUBST(UDFREAD_VERSION_MINOR)
UDFREAD_VERSION_MICRO=udfread_micro()
AC_SUBST(UDFREAD_VERSION_MICRO)

AC_DEFINE([HAVE_UDFREAD_VERSION_H], 1, ["Define to 1 if you have udfread-version.h header file."])

dnl export library (.so) version
UDFREAD_LTVERSION="lt_current():lt_revision():lt_age()"
AC_SUBST(UDFREAD_LTVERSION)

dnl generate output files
AC_CONFIG_FILES([Makefile src/udfread.pc src/udfread-version.h])
AC_OUTPUT
