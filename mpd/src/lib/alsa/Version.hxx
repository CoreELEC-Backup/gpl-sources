/*
 * Copyright 2003-2020 The Music Player Daemon Project
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

#ifndef MPD_ALSA_VERSION_HXX
#define MPD_ALSA_VERSION_HXX

#include "util/Compiler.h"

#include <cstdint>

static constexpr uint_least32_t
MakeAlsaVersion(uint_least32_t major, uint_least32_t minor,
		uint_least32_t subminor) noexcept
{
	return (major << 16) | (minor << 8) | subminor;
}

/**
 * Wrapper for snd_asoundlib_version() which translates the resulting
 * string to an integer constructed with MakeAlsaVersion().
 */
gcc_const
uint_least32_t
GetRuntimeAlsaVersion() noexcept;

#endif
