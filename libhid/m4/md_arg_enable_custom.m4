AC_DEFUN([MD_ARG_ENABLE_CUSTOM],
  [
    AC_ARG_ENABLE([$1],
      AC_HELP_STRING([--enable-$1], [enable $2 (default is $3)]),
      ac_cv_enable_$1=$enableval, ac_cv_enable_$1=$3)
    AC_CACHE_CHECK([whether to enable $2], ac_cv_enable_$1, ac_cv_enable_$1=$3)
  ])
