AC_DEFUN([MD_CONF_COMPILER],
  [
    MD_CHECK_ARG_ENABLE(warnings,
      [
        CFLAGS="$CFLAGS -Wall -W"
        CXXFLAGS="$CXXFLAGS -Wall -W"
        AC_MSG_NOTICE([enabled compiler warnings.])
      ])

    MD_CHECK_ARG_ENABLE(pedantic,
      [
        CFLAGS="$CFLAGS -pedantic"
        CXXFLAGS="$CXXFLAGS -pedantic"
        AC_MSG_NOTICE([enabled pedantic compiler checks.])
      ])

    MD_CHECK_ARG_ENABLE(ansi,
      [
        CFLAGS="$CFLAGS -ansi"
        CXXFLAGS="$CXXFLAGS -ansi"
        AC_MSG_NOTICE([enabled ansi-compliance compiler checks.])
      ])

    MD_CHECK_ARG_ENABLE(werror,
      [
        CFLAGS="$CFLAGS -Werror"
        CXXFLAGS="$CXXFLAGS -Werror"
        AC_MSG_NOTICE([turned compiler warnings into errors.])
      ])
  ])
