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

#include "Riff.hxx"
#include "input/InputStream.hxx"
#include "util/ByteOrder.hxx"

#include <cstdint>
#include <limits>
#include <stdexcept>

#include <string.h>

struct riff_header {
	char id[4];
	uint32_t size;
	char format[4];
};

struct riff_chunk_header {
	char id[4];
	uint32_t size;
};

size_t
riff_seek_id3(InputStream &is, std::unique_lock<Mutex> &lock)
{
	/* seek to the beginning and read the RIFF header */

	is.Rewind(lock);

	riff_header header;
	is.ReadFull(lock, &header, sizeof(header));
	if (memcmp(header.id, "RIFF", 4) != 0 ||
	    (is.KnownSize() && FromLE32(header.size) > is.GetSize()))
		throw std::runtime_error("Not a RIFF file");

	while (true) {
		/* read the chunk header */

		riff_chunk_header chunk;
		is.ReadFull(lock, &chunk, sizeof(chunk));

		size_t size = FromLE32(chunk.size);
		if (size > size_t(std::numeric_limits<int>::max()))
			/* too dangerous, bail out: possible integer
			   underflow when casting to off_t */
			throw std::runtime_error("RIFF chunk is too large");

		if (memcmp(chunk.id, "id3 ", 4) == 0 ||
		    memcmp(chunk.id, "ID3 ", 4) == 0)
			/* found it! */
			return size;

		if (size % 2 != 0)
			/* pad byte */
			++size;

		is.Skip(lock, size);
	}
}
