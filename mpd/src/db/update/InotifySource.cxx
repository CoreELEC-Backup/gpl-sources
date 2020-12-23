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

#include "InotifySource.hxx"
#include "InotifyDomain.hxx"
#include "io/FileDescriptor.hxx"
#include "system/FatalError.hxx"
#include "system/Error.hxx"
#include "Log.hxx"

#include <cerrno>
#include <climits>
#include <cstdint>

#include <sys/inotify.h>

bool
InotifySource::OnSocketReady([[maybe_unused]] unsigned flags) noexcept
{
	uint8_t buffer[4096];
	static_assert(sizeof(buffer) >= sizeof(struct inotify_event) + NAME_MAX + 1,
		      "inotify buffer too small");

	auto ifd = GetSocket().ToFileDescriptor();
	ssize_t nbytes = ifd.Read(buffer, sizeof(buffer));
	if (nbytes < 0)
		FatalSystemError("Failed to read from inotify");
	if (nbytes == 0)
		FatalError("end of file from inotify");

	const uint8_t *p = buffer, *const end = p + nbytes;

	while (true) {
		const size_t remaining = end - p;
		const auto *event =
			(const struct inotify_event *)p;
		if (remaining < sizeof(*event) ||
		    remaining < sizeof(*event) + event->len)
			break;

		const char *name;
		if (event->len > 0 && event->name[event->len - 1] == 0)
			name = event->name;
		else
			name = nullptr;

		callback(event->wd, event->mask, name, callback_ctx);
		p += sizeof(*event) + event->len;
	}

	return true;
}

static FileDescriptor
InotifyInit()
{
	FileDescriptor fd;
	if (!fd.CreateInotify())
		throw MakeErrno("inotify_init() has failed");

	return fd;
}

InotifySource::InotifySource(EventLoop &_loop,
			     mpd_inotify_callback_t _callback, void *_ctx)
	:SocketMonitor(SocketDescriptor::FromFileDescriptor(InotifyInit()),
		       _loop),
	 callback(_callback), callback_ctx(_ctx)
{
	ScheduleRead();
}

int
InotifySource::Add(const char *path_fs, unsigned mask)
{
	auto ifd = GetSocket().ToFileDescriptor();
	int wd = inotify_add_watch(ifd.Get(), path_fs, mask);
	if (wd < 0)
		throw MakeErrno("inotify_add_watch() has failed");

	return wd;
}

void
InotifySource::Remove(unsigned wd) noexcept
{
	auto ifd = GetSocket().ToFileDescriptor();
	int ret = inotify_rm_watch(ifd.Get(), wd);
	if (ret < 0 && errno != EINVAL)
		LogErrno(inotify_domain, "inotify_rm_watch() has failed");

	/* EINVAL may happen here when the file has been deleted; the
	   kernel seems to auto-unregister deleted files */
}
