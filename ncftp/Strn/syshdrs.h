/* syshdrs.h
 *
 * Copyright (c) 1996-2004 Mike Gleason, NcFTP Software.
 * All rights reserved.
 *
 */
#if (defined(WIN32) || defined(_WINDOWS)) && !defined(__CYGWIN__)
#	pragma once
#	define _CRT_SECURE_NO_WARNINGS 1
#	ifndef __MINGW32__
#		pragma warning(disable : 4127)	// warning C4127: conditional expression is constant
#		pragma warning(disable : 4100)	// warning C4100: 'lpReserved' : unreferenced formal parameter
#		pragma warning(disable : 4514)	// warning C4514: unreferenced inline function has been removed
#		pragma warning(disable : 4115)	// warning C4115: '_RPC_ASYNC_STATE' : named type definition in parentheses
#		pragma warning(disable : 4201)	// warning C4201: nonstandard extension used : nameless struct/union
#		pragma warning(disable : 4214)	// warning C4214: nonstandard extension used : bit field types other than int
#		pragma warning(disable : 4115)	// warning C4115: 'IRpcStubBuffer' : named type definition in parentheses
#	endif
	/* We now try for at least Windows 2000 compatibility (0x0500).
	 * The code will still work on older systems, though.
	 * Prior versions used 0x0400 instead.
	 */
#	ifndef WINVER
#		define WINVER 0x0500
#	endif
#	ifndef _WIN32_WINNT
#		define _WIN32_WINNT 0x0500
#	endif
#	include <windows.h>
#	include <sys/types.h>
#	include <errno.h>
#	include <stdio.h>
#	include <string.h>
#	include <stddef.h>
#	include <stdlib.h>
#	include <ctype.h>
#	include <stdarg.h>
#	ifndef strcasecmp
#		define strcasecmp _stricmp
#		define strncasecmp _strnicmp
#	endif
#	ifndef strdup
#		define strdup _strdup
#	endif
#	define HAVE_STRDUP 1
#else /* ---------------------------- UNIX ---------------------------- */
#	if defined(HAVE_CONFIG_H)
#		include <config.h>
#	endif
#	if defined(AIX) || defined(_AIX) || defined(__HOS_AIX__)
#		define _ALL_SOURCE 1
#	endif
#	ifdef HAVE_UNISTD_H
#		include <unistd.h>
#	endif
#	include <sys/types.h>
#	include <errno.h>
#	include <stdio.h>
#	include <string.h>
#	ifdef HAVE_STRINGS_H
#		include <strings.h>
#	endif
#	include <stddef.h>
#	include <stdlib.h>
#	include <ctype.h>
#	include <stdarg.h>
#endif /* ---------------------------- UNIX ---------------------------- */

#include "DStrInternal.h"
#include "Strn.h"

#ifndef HAVE_STRDUP
extern char *strdup(const char *const src);
#endif

/* eof */
