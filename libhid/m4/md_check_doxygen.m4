AC_DEFUN([MD_CHECK_DOXYGEN],
  [
    AM_CONDITIONAL([DOXYGEN], [false])
    MD_CHECK_ARG_WITH(doxygen, doxygen,
      [
        AC_CHECK_PROG(HAVE_doxygen, $path, yes)
        if test "$HAVE_doxygen" = "yes"; then
          AC_MSG_NOTICE([$path found. using doxygen.])
          AM_CONDITIONAL([DOXYGEN], [true])
        else
          AC_MSG_WARN([$path not found. not using doxygen.])
        fi
      ])
  ])
