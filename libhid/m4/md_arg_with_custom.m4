AC_DEFUN([MD_ARG_WITH_CUSTOM],
  [
    AC_ARG_WITH([$1],
      AC_HELP_STRING([--with-$1], [use $2 (default is $3)]),
      ac_cv_use_$1=$withval, ac_cv_use_$1=$3)
    AC_CACHE_CHECK([whether to use $2], ac_cv_use_$1, ac_cv_use_$1=$3)
  ])
