/*
  Copyright (C) 2015  Dmitry V. Levin <ldv@altlinux.org>

  This program is free software: you can redistribute it and/or modify it
  under the terms of the GNU Lesser General Public License as published by
  the Free Software Foundation, either version 2.1 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * These dumb wrappers are for backwards compatibility only.
 * Actual syscall wrappers are long gone to libc.
 */

#include "config.h"

#include <sys/xattr.h>

#ifdef HAVE_VISIBILITY_ATTRIBUTE
# pragma GCC visibility push(default)
#endif

int libattr_setxattr(const char *path, const char *name,
		     void *value, size_t size, int flags)
{
	return setxattr(path, name, value, size, flags);
}

int libattr_lsetxattr(const char *path, const char *name,
		      void *value, size_t size, int flags)
{
	return lsetxattr(path, name, value, size, flags);
}

int libattr_fsetxattr(int filedes, const char *name,
		      void *value, size_t size, int flags)
{
	return fsetxattr(filedes, name, value, size, flags);
}

ssize_t libattr_getxattr(const char *path, const char *name,
			 void *value, size_t size)
{
	return getxattr(path, name, value, size);
}

ssize_t libattr_lgetxattr(const char *path, const char *name,
			  void *value, size_t size)
{
	return lgetxattr(path, name, value, size);
}

ssize_t libattr_fgetxattr(int filedes, const char *name,
			  void *value, size_t size)
{
	return fgetxattr(filedes, name, value, size);
}

ssize_t libattr_listxattr(const char *path, char *list, size_t size)
{
	return listxattr(path, list, size);
}

ssize_t libattr_llistxattr(const char *path, char *list, size_t size)
{
	return llistxattr(path, list, size);
}

ssize_t libattr_flistxattr(int filedes, char *list, size_t size)
{
	return flistxattr(filedes, list, size);
}

int libattr_removexattr(const char *path, const char *name)
{
	return removexattr(path, name);
}

int libattr_lremovexattr(const char *path, const char *name)
{
	return lremovexattr(path, name);
}

int libattr_fremovexattr(int filedes, const char *name)
{
	return fremovexattr(filedes, name);
}

#ifdef HAVE_VISIBILITY_ATTRIBUTE
# pragma GCC visibility pop
#endif
