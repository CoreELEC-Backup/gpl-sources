dnl -*- mode: m4 -*-
AC_PREREQ(2.52)

AC_INIT([dbus-glib], [0.110],
  [https://bugs.freedesktop.org/enter_bug.cgi?product=dbus&component=GLib])

AC_CANONICAL_HOST

AM_INIT_AUTOMAKE([1.9 subdir-objects])

AC_CONFIG_HEADERS(config.h)

# Honor aclocal flags
ACLOCAL="$ACLOCAL $ACLOCAL_FLAGS"
AC_CONFIG_MACRO_DIR(m4)

 ## must come before we use the $USE_MAINTAINER_MODE variable later
AM_MAINTAINER_MODE

m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

# libtool versioning - this applies to libdbus
#
# See http://sources.redhat.com/autobook/autobook/autobook_91.html#SEC91 for details
#

## increment if the interface has additions, changes, removals.
LT_CURRENT=5

## increment any time the source changes; set to
##  0 if you increment CURRENT
LT_REVISION=4

## increment if any interfaces have been added; set to 0
## if any interfaces have been changed or removed. removal has
## precedence over adding, so set to 0 if both happened.
LT_AGE=3

AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)


AC_PROG_CC
AC_ISC_POSIX
AC_HEADER_STDC

AC_ARG_ENABLE(tests, AS_HELP_STRING([--enable-tests],[enable unit test code]),enable_tests=$enableval,enable_tests=$USE_MAINTAINER_MODE)
AC_ARG_ENABLE(ansi, AS_HELP_STRING([--enable-ansi],[enable -ansi -pedantic gcc flags]),enable_ansi=$enableval,enable_ansi=no)
AC_ARG_ENABLE(gcov, AS_HELP_STRING([--enable-gcov],[compile with coverage profiling instrumentation (gcc only)]),enable_gcov=$enableval,enable_gcov=no)
AC_ARG_ENABLE(bash-completion, AS_HELP_STRING([--enable-bash-completion],[install bash completion scripts]),enable_bash_completion=$enableval,enable_bash_completion=yes)

AM_CONDITIONAL(DBUS_BASH_COMPLETION, test x$enable_bash_completion = xyes)
if test x$enable_bash_completion = xyes; then
  AC_DEFINE(DBUS_BASH_COMPLETION,1,[Enable bash completion])
fi

AC_ARG_WITH(dbus-binding-tool,
  AS_HELP_STRING([--with-dbus-binding-tool=[filename]],[Use external dbus-binding-tool program]),
  [DBUS_BINDING_TOOL=$withval],[DBUS_BINDING_TOOL=\$\(top_builddir\)/dbus/dbus-binding-tool])
AC_SUBST(DBUS_BINDING_TOOL)


dnl DBUS_BUILD_TESTS controls unit tests built in to .c files 
dnl and also some stuff in the test/ subdir
AM_CONDITIONAL(DBUS_BUILD_TESTS, test x$enable_tests = xyes)
if test x$enable_tests = xyes; then
    AC_DEFINE(DBUS_BUILD_TESTS,1,[Build test code])
fi

AC_ARG_ENABLE([asserts],
  [AS_HELP_STRING([--enable-asserts], [include assertion checks])],
  [enable_asserts=$enableval], [enable_asserts=$USE_MAINTAINER_MODE])
if test x$enable_asserts = xno; then
    AC_DEFINE([G_DISABLE_ASSERT], [1], [Disable GLib assertion macros])
fi

AC_ARG_ENABLE([checks],
  [AS_HELP_STRING([--enable-checks], [include sanity checks on public API])],
  [enable_checks=$enableval], [enable_checks=yes])
if test x$enable_checks = xno; then
    AC_DEFINE([G_DISABLE_CHECKS], [1], [Disable GLib public API sanity checking])
fi

#### gcc warning flags

AC_DEFUN([AC_CC_TRY_FLAG], [
  AC_MSG_CHECKING([whether gcc understands $1])

  ac_save_CFLAGS="$CFLAGS"
  CFLAGS="$CFLAGS $1"

  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[]], [[]])], [ac_cc_flag=yes], [ac_cc_flag=no])
  CFLAGS="$ac_save_CFLAGS"

  if test "x$ac_cc_flag" = "xyes"; then
    ifelse([$2], , :, [$2])
  else
    ifelse([$3], , :, [$3])
  fi
  AC_MSG_RESULT([$ac_cc_flag])
])

if test "x$GCC" = "xyes"; then
  AC_CC_TRY_FLAG([-Wfloat-equal],
                 [ac_flag_float_equal=yes],
                 [ac_flag_float_equal=no])

  changequote(,)dnl
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wchar-subscripts[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wchar-subscripts" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wmissing-declarations[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wmissing-declarations" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wmissing-prototypes[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wmissing-prototypes" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wnested-externs[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wnested-externs" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wpointer-arith[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wpointer-arith" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wcast-align[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wcast-align" ;;
  esac

  if test "x$ac_flag_float_equal" = "xyes"; then
    case " $CFLAGS " in
    *[\ \	]-Wfloat-equal[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -Wfloat-equal" ;;
    esac
  fi

  case " $CFLAGS " in
  *[\ \	]-Wsign-compare[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wsign-compare" ;;
  esac
  
  # This one is special - it's not a warning override.
  # http://bugs.freedesktop.org/show_bug.cgi?id=10599
  # is the bug for DBus core.
  case " $CFLAGS " in
  *[\ \	]-fno-strict-aliasing[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -fno-strict-aliasing" ;;
  esac  

  if test "x$enable_ansi" = "xyes"; then
    case " $CFLAGS " in
    *[\ \	]-ansi[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -ansi" ;;
    esac

    case " $CFLAGS " in
    *[\ \	]-D_POSIX_C_SOURCE*) ;;
    *) CFLAGS="$CFLAGS -D_POSIX_C_SOURCE=199309L" ;;
    esac

    case " $CFLAGS " in
    *[\ \	]-D_BSD_SOURCE[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -D_BSD_SOURCE" ;;
    esac

    case " $CFLAGS " in
    *[\ \	]-pedantic[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -pedantic" ;;
    esac
  fi
  if test x$enable_gcov = xyes; then
    case " $CFLAGS " in
    *[\ \	]-fprofile-arcs[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -fprofile-arcs" ;;
    esac
    case " $CFLAGS " in
    *[\ \	]-ftest-coverage[\ \	]*) ;;
    *) CFLAGS="$CFLAGS -ftest-coverage" ;;
    esac

    ## remove optimization
    CFLAGS=`echo "$CFLAGS" | sed -e 's/-O[0-9]*//g'`
  fi
  changequote([,])dnl
else
  if test x$enable_gcov = xyes; then
    AC_MSG_ERROR([--enable-gcov can only be used with gcc])
  fi
fi

LT_INIT

changequote(,)dnl
# compress spaces in flags
CFLAGS=`echo "$CFLAGS" | sed -e 's/ +/ /g'`
CPPFLAGS=`echo "$CPPFLAGS" | sed -e 's/ +/ /g'`
changequote([,])dnl

if test x$enable_gcov = xyes; then
     # so that config.h changes when you toggle gcov support
     AC_DEFINE_UNQUOTED([DBUS_GCOV_ENABLED],
       [__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__],
       [Defined to the gcc version if gcov is enabled, to force a rebuild due to config.h changing])
fi

#### Various functions
AC_SEARCH_LIBS([socket], [socket])

dnl check for socklen_t
AC_MSG_CHECKING(whether socklen_t is defined)
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
]],[[
socklen_t foo;
foo = 1;
]])],dbus_have_socklen_t=yes,dbus_have_socklen_t=no)
AC_MSG_RESULT($dbus_have_socklen_t)

if test "x$dbus_have_socklen_t" = "xyes"; then
    AC_DEFINE(HAVE_SOCKLEN_T,1,[Have socklen_t type])
fi

#### Sort out XML library

# see what we have
AC_CHECK_LIB(expat, XML_ParserCreate_MM,
             [ AC_CHECK_HEADERS(expat.h, have_expat=true, have_expat=false) ],
             have_expat=false)

if ! $have_expat ; then
  AC_MSG_ERROR([expat library not found, check config.log for failed attempts])
fi

XML_LIBS=-lexpat
XML_CFLAGS=


#### Set up final flags

PKG_CHECK_MODULES([DBUS], [dbus-1 >= 1.8])
AC_SUBST([DBUS_CFLAGS])
AC_SUBST([DBUS_LIBS])

# Glib detection
AC_DEFINE([GLIB_VERSION_MAX_ALLOWED], [GLIB_VERSION_2_40],
  [Warn on use of APIs added after GLib 2.40])
AC_DEFINE([GLIB_VERSION_MIN_REQUIRED], [GLIB_VERSION_2_32],
  [Warn on use of APIs deprecated before GLib 2.32])
PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.40, gobject-2.0 >= 2.40, gio-2.0 >= 2.40])

GLIB_GENMARSHAL=`$PKG_CONFIG --variable=glib_genmarshal glib-2.0`
AC_SUBST(GLIB_GENMARSHAL)

dnl GLib flags
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)
AC_SUBST(GLIB_THREADS_LIBS)

DBUS_GLIB_TOOL_CFLAGS=$XML_CFLAGS
DBUS_GLIB_TOOL_LIBS="$XML_LIBS"
AC_SUBST(DBUS_GLIB_TOOL_CFLAGS)
AC_SUBST(DBUS_GLIB_TOOL_LIBS)

AC_ARG_VAR([DBUS_RUN_SESSION],
  [The dbus-run-session tool from dbus 1.8 or later])
AC_PATH_PROG([DBUS_RUN_SESSION], [dbus-run-session], [dbus-run-session])

### gtk-doc Documentation
GTK_DOC_CHECK([1.14], [--flavour no-tmpl])

# Make dbus-gmain submodule part of dbus-glib's namespace
AH_BOTTOM([
#define DBUS_GMAIN_FUNCTION_NAME(name) _dbus_g_ ## name
])

AC_CONFIG_FILES([
Makefile
m4/Makefile
doc/Makefile
doc/reference/Makefile
doc/reference/version.xml
dbus/Makefile
dbus/examples/Makefile
dbus/examples/statemachine/Makefile
dbus-gmain/Makefile
test/Makefile
test/core/Makefile
test/interfaces/Makefile
test/data/valid-service-files/debug-glib.service
test/data/valid-service-files/debug-echo.service
test/data/valid-service-files/interfaces-test.service
tools/Makefile
dbus-glib-1.pc
dbus-glib-1-uninstalled.pc
])
AC_OUTPUT

dnl ==========================================================================
echo "
                    D-BUS GLIB BINDINGS $VERSION
                  ==============

	prefix:                   ${prefix}
	exec_prefix:              ${exec_prefix}
	source code location:	  ${srcdir}
	compiler:		  ${CC}
	cflags:		          ${CFLAGS}
	cppflags:		  ${CPPFLAGS}
"

echo "
        Maintainer mode:          ${USE_MAINTAINER_MODE}
        gcc coverage profiling:   ${enable_gcov}
        Building unit tests:      ${enable_tests}
        Building assertions:      ${enable_asserts}
        Building checks:          ${enable_checks}
        Building Gtk-doc docs:    ${enable_gtk_doc}
        Bash Completion:          ${enable_bash_completion}
        Using XML parser:         ${with_xml}
"

if test x$enable_tests = xyes; then
        echo "NOTE: building with unit tests increases the size of the installed library and renders it insecure."
fi
if test x$enable_tests = xyes -a x$enable_asserts = xno; then
        echo "NOTE: building with unit tests but without assertions means tests may not properly report failures (this configuration is only useful when doing something like profiling the tests)"
fi
if test x$enable_gcov = xyes; then
        echo "NOTE: building with coverage profiling is definitely for developers only."
fi
if test x$enable_asserts = xyes; then
        echo "NOTE: building with assertions increases library size and decreases performance."
fi
if test x$enable_checks = xno; then
        echo "NOTE: building without checks for arguments passed to public API makes it harder to debug apps using D-BUS, but will slightly decrease D-BUS library size and _very_ slightly improve performance."
fi
