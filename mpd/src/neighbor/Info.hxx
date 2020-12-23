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

#ifndef MPD_NEIGHBOR_INFO_HXX
#define MPD_NEIGHBOR_INFO_HXX

#include <string>

struct NeighborInfo {
	std::string uri;
	std::string display_name;

	template<typename U, typename DN>
	NeighborInfo(U &&_uri, DN &&_display_name)
		:uri(std::forward<U>(_uri)),
		 display_name(std::forward<DN>(_display_name)) {}
};

#endif
