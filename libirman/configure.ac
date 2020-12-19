dnl Process this file with autoconf to produce a configure script.
AC_INIT([libirman],[0.5.2])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR(irio.c)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE

AC_DEFUN([AC_REPORT_CONDITIONAL], [
AS_ECHO_N([$1:])
AS_IF(test -z $$1_TRUE,AS_ECHO([yes]),AS_ECHO([no]))])

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC
AC_PROG_LIBTOOL
AC_CHECK_PROG([PKGCONFIG],[pkg-config],[yes],[no])
if test x$PKGCONFIG = xno; then
  AC_MSG_ERROR([Required program pkg-config is missing])
fi

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(fcntl.h sys/time.h unistd.h termios.h memory.hi sys/file.h poll.h sys/poll.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_HEADER_TIME
AC_C_CONST

dnl Checks for library functions.
AC_FUNC_MEMCMP
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(mkfifo select strcspn strerror strspn)

dnl Handle conditional plugin build.
PKG_CHECK_MODULES([LIRC],[lirc-driver > 0.9.3],
                  [build_plugin="yes"],[build_plugin="no"])
AC_ARG_WITH(plugin,
            [AC_HELP_STRING([--with-plugin],[Build LIRC plugin [guessed]])],
            [],
            [with_plugin=$build_plugin])
if test x$with_plugin = xno; then
    AM_CONDITIONAL([BUILD_PLUGIN],[false])
elif test x$build_plugin = xyes; then
    AM_CONDITIONAL([BUILD_PLUGIN],[true])
else
    AC_MSG_ERROR([plugin requested, but suitable LIRC >= 0.9.3 not available])
fi

dnl Checks to see if software test versions should be made
AC_ARG_ENABLE(swtest,
   [  --enable-swtest         Build software-only library and utils [[disabled]]],
   [  install_swtest="${enableval}"],
   [  install_swtest="no"])
AM_CONDITIONAL(BUILD_SWTEST, test x$install_swtest = xyes)

AC_REPORT_CONDITIONAL([BUILD_PLUGIN])
AC_REPORT_CONDITIONAL([BUILD_SWTEST])

dnl Create the makefile and pkg-config file
AC_OUTPUT([Makefile libirman.pc])
