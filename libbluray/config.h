/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to 1 if libudfread is to be used for disc image access */
/* #undef ENABLE_UDF */

/* Define to 1 if using libbluray J2ME stack */
/* #undef HAVE_BDJ_J2ME */

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_DIRENT_H */

/* Define to 1 if you have the <dlfcn.h> header file. */
/* #undef HAVE_DLFCN_H */

/* Define to 1 if you have the <errno.h> header file. */
#define HAVE_ERRNO_H 1

/* Define to 1 if you have the <fcntl.h> header file. */
/* #undef HAVE_FCNTL_H */

/* Define this if you have fontconfig library */
/* #undef HAVE_FONTCONFIG */

/* Define this if you have FreeType2 library */
/* #undef HAVE_FT2 */

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <jni.h> header file. */
/* #undef HAVE_JNI_H */

/* Define to 1 if you have the <libgen.h> header file. */
#define HAVE_LIBGEN_H 1

/* Define to 1 if libxml2 is to be used for metadata parsing */
/* #undef HAVE_LIBXML2 */

/* Define to 1 if you have the <linux/cdrom.h> header file. */
/* #undef HAVE_LINUX_CDROM_H */

/* Define to 1 if you have the <malloc.h> header file. */
#define HAVE_MALLOC_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Define to 1 if you have the <mntent.h> header file. */
/* #undef HAVE_MNTENT_H */

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef HAVE_NDIR_H */

/* Define to 1 if you have the <pthread.h> header file. */
/* #undef HAVE_PTHREAD_H */

/* Define to 1 if you have the <stdarg.h> header file. */
#define HAVE_STDARG_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
/* #undef HAVE_STRINGS_H */

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if `d_type' is a member of `struct dirent'. */
/* #undef HAVE_STRUCT_DIRENT_D_TYPE */

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#define HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <time.h> header file. */
#define HAVE_TIME_H 1

/* Define to 1 if you have the <unistd.h> header file. */
/* #undef HAVE_UNISTD_H */

/* "Defines the architecture of the java vm." */
/* #undef JAVA_ARCH */

/* "" */
/* #undef JDK_HOME */

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "libbluray"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "http://www.videolan.org/developers/libbluray.html"

/* Define to the full name of this package. */
#define PACKAGE_NAME "libbluray"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libbluray 1.0.1"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libbluray"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.0.1"

/* Define as the return type of signal handlers (`int' or `void'). */
#define RETSIGTYPE void

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* "Define to 1 if using BD-Java" */
/* #undef USING_BDJAVA */

/* Version number of package */
#define VERSION "1.0.1"

/* Enable large inode numbers on Mac OS X 10.5.  */
#ifndef _DARWIN_USE_64_BIT_INODE
# define _DARWIN_USE_64_BIT_INODE 1
#endif

/* Number of bits in a file offset, on hosts where this is settable. */
#define _FILE_OFFSET_BITS 64

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Define to '0x0501' for IE 5.01. */
#define _WIN32_IE 0x0501

/* Define to '0x0502' for Windows XP SP2 APIs. */
#define _WIN32_WINNT 0x0502
