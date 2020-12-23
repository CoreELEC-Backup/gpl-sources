dnl TAR filesystem support
AC_DEFUN([mc_VFS_TARFS],
[
    AC_ARG_ENABLE([vfs-tar],
		    AS_HELP_STRING([--enable-vfs-tar], [Support for tar filesystem @<:@yes@:>@]))
    if test "$enable_vfs" = "yes" -a x"$enable_vfs_tar" != x"no"; then
	enable_vfs_tar="yes"
	mc_VFS_ADDNAME([tar])
	AC_DEFINE([ENABLE_VFS_TAR], [1], [Support for tar filesystem])
    fi
    AM_CONDITIONAL(ENABLE_VFS_TAR, [test "$enable_vfs" = "yes" -a x"$enable_vfs_tar" = x"yes"])
])
