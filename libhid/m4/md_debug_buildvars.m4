AC_DEFUN([MD_DEBUG_BUILDVARS],
  [
    cat <<EOF

.------------------------------------------------------------------------------
| Configuration to be used:
|
|   CPP     : $CPP
|   CPPFLAGS: $CPPFLAGS
|   CC      : $CC
|   CFLAGS  : $CFLAGS
|   CXX     : $CXX
|   CXXFLAGS: $CXXFLAGS
|   LD      : $LD
|   LDFLAGS : $LDFLAGS
\`------------------------------------------------------------------------------
EOF
  ])
