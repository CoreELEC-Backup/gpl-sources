AC_DEFUN([MD_CONF_FLAGS],
  [
    CPPFLAGS=$(echo $1 $CPPFLAGS)
    CFLAGS=$(echo $2 $CFLAGS)
    CXXFLAGS=$(echo $3 $CXXFLAGS)
    LDFLAGS=$(echo $4 $LDFLAGS)
  ])
