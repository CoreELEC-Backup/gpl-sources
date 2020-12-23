AC_DEFUN([MD_CHECK_GCC3],
  [
    if [[ "$GCC" = "yes" ]]; then
      ver=$(gcc -dD -E - < /dev/null | sed -ne 's,.*__GNUC__ ,,p')
      case $ver in
        *[[^[:digit:]]]*)
          AC_MSG_WARN([Could not determine compiler version. Trying our luck...])
          ;;
        *)
          if [[ $ver -lt 3 ]]; then
            AC_MSG_ERROR([Please upgrade your compiler to gcc 3.x.])
          fi
          ;;
      esac
    fi
  ])
