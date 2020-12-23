// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    corestr.h

    Core string functions used throughout MAME.

***************************************************************************/

#pragma once

#ifndef __CORESTR_H__
#define __CORESTR_H__

#include "osdcore.h"
#include <string.h>


/***************************************************************************
    FUNCTION PROTOTYPES
***************************************************************************/

/* since stricmp is not part of the standard, we use this instead */
int core_stricmp(const char *s1, const char *s2);

/* this macro prevents people from using stricmp directly */
#undef stricmp
#define stricmp MUST_USE_CORE_STRICMP_INSTEAD

/* this macro prevents people from using strcasecmp directly */
#undef strcasecmp
#define strcasecmp MUST_USE_CORE_STRICMP_INSTEAD


/* since strnicmp is not part of the standard, we use this instead */
int core_strnicmp(const char *s1, const char *s2, size_t n);

/* this macro prevents people from using strnicmp directly */
#undef strnicmp
#define strnicmp MUST_USE_CORE_STRNICMP_INSTEAD

/* this macro prevents people from using strncasecmp directly */
#undef strncasecmp
#define strncasecmp MUST_USE_CORE_STRNICMP_INSTEAD


/* since strdup is not part of the standard, we use this instead - free with osd_free() */
char *core_strdup(const char *str);

/* this macro prevents people from using strdup directly */
#undef strdup
#define strdup MUST_USE_CORE_STRDUP_INSTEAD


/* additional string compare helper (up to 16 characters at the moment) */
int core_strwildcmp(const char *sp1, const char *sp2);


/* I64 printf helper */
char *core_i64_format(UINT64 value, UINT8 mindigits, bool is_octal);
char *core_i64_hex_format(UINT64 value, UINT8 mindigits);
char *core_i64_oct_format(UINT64 value, UINT8 mindigits);

#endif /* __CORESTR_H__ */
