AC_DEFUN([MD_CONF_DEBUGGING],
  [
    CFLAGS=$(echo $CFLAGS | sed -e 's,-g[[^[:space:]]]*,,g')
    CXXFLAGS=$(echo $CXXFLAGS | sed -e 's,-g[[^[:space:]]]*,,g')
    MD_CHECK_ARG_ENABLE(debug,
      [
        DEBUG_FLAG=-g
        MD_CHECK_ARG_ENABLE(gdbdebug, [DEBUG_FLAG='-g3 -ggdb'])
        CFLAGS="$(echo $CFLAGS | sed -e 's,-O[[[:digit:]]]*,,g') -O0"
        CXXFLAGS="$(echo $CXXFLAGS | sed -e 's,-O[[[:digit:]]]*,,g') -O0"
        CFLAGS="$CFLAGS $DEBUG_FLAG"
        CXXFLAGS="$CXXFLAGS $DEBUG_FLAG"
      ],
      [
        CPPFLAGS="$CPPFLAGS -DNDEBUG"
      ])
  ])
