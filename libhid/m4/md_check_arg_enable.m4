AC_DEFUN([MD_CHECK_ARG_ENABLE],
  [
    if test "$ac_cv_enable_$1" = "yes"; then
      echo nada > /dev/null
      $2
    else
      echo nada > /dev/null
      $3
    fi
  ])
