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

#include "config.h"
#include "Registry.hxx"
#include "NeighborPlugin.hxx"
#include "plugins/SmbclientNeighborPlugin.hxx"
#include "plugins/UpnpNeighborPlugin.hxx"
#include "plugins/UdisksNeighborPlugin.hxx"

#include <string.h>

const NeighborPlugin *const neighbor_plugins[] = {
#ifdef ENABLE_SMBCLIENT
	&smbclient_neighbor_plugin,
#endif
#ifdef ENABLE_UPNP
	&upnp_neighbor_plugin,
#endif
#ifdef ENABLE_UDISKS
	&udisks_neighbor_plugin,
#endif
	nullptr
};

const NeighborPlugin *
GetNeighborPluginByName(const char *name) noexcept
{
	for (auto i = neighbor_plugins; *i != nullptr; ++i)
		if (strcmp((*i)->name, name) == 0)
			return *i;

	return nullptr;
}
