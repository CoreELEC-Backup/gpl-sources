/*
 * music player command (mpc)
 * Copyright 2003-2018 The Music Player Daemon Project
 * http://www.musicpd.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef CHAR_CONV_H
#define CHAR_CONV_H

#include "config.h"
#include "Compiler.h"

#include <stdbool.h>

#ifdef HAVE_ICONV

/**
 * Initializes the character set conversion library.
 *
 * @param enable_input allow conversion from locale to UTF-8
 * @param enable_output allow conversion from UTF-8 to locale
 */
void
charset_init(bool enable_input, bool enable_output);

void charset_deinit(void);

gcc_pure
const char *
charset_to_utf8(const char *from);

gcc_pure
const char *
charset_from_utf8(const char *from);

#else

static inline void
charset_init(bool disable_input, bool disable_output)
{
	(void)disable_input;
	(void)disable_output;
}

static inline void
charset_deinit(void)
{
}

static inline const char *
charset_to_utf8(const char *from)
{
	return from;
}

static inline const char *
charset_from_utf8(const char *from)
{
	return from;
}

#endif

#endif
