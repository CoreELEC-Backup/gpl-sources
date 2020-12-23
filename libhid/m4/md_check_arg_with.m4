AC_DEFUN([MD_CHECK_ARG_WITH],
  [
    if test "$ac_cv_use_$1" != "no"; then
      if test "$ac_cv_use_$1" = "yes"; then
        ac_cv_use_$1=$2
      fi
      path=$ac_cv_use_$1
      $3
    else
      AC_MSG_NOTICE([$1 disabled by configuration (use --with-$1 to enable).])
      $4
    fi
  ])
