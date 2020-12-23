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

#include "SocketMonitor.hxx"
#include "Loop.hxx"

#include <cassert>
#include <utility>

#ifdef USE_EPOLL
#include <cerrno>
#endif

void
SocketMonitor::Dispatch(unsigned flags) noexcept
{
	flags &= GetScheduledFlags() | IMPLICIT_FLAGS;

	if (flags != 0)
		OnSocketReady(flags);
}

SocketMonitor::~SocketMonitor() noexcept
{
	if (IsDefined())
		Cancel();
}

void
SocketMonitor::Open(SocketDescriptor _fd) noexcept
{
	assert(!fd.IsDefined());
	assert(_fd.IsDefined());

	fd = _fd;
}

SocketDescriptor
SocketMonitor::Steal() noexcept
{
	assert(IsDefined());

	Cancel();

	return std::exchange(fd, SocketDescriptor::Undefined());
}

void
SocketMonitor::Close() noexcept
{
	Steal().Close();
}

bool
SocketMonitor::Schedule(unsigned flags) noexcept
{
	assert(IsDefined());

	if (flags == GetScheduledFlags())
		return true;

	bool success;
	if (scheduled_flags == 0)
		success = loop.AddFD(fd.Get(), flags, *this);
	else if (flags == 0)
		success = loop.RemoveFD(fd.Get(), *this);
	else
		success = loop.ModifyFD(fd.Get(), flags, *this);

	if (success)
		scheduled_flags = flags;
#ifdef USE_EPOLL
	else if (errno == EBADF || errno == ENOENT)
		/* the socket was probably closed by somebody else
		   (EBADF) or a new file descriptor with the same
		   number was created but not registered already
		   (ENOENT) - we can assume that there are no
		   scheduled events */
		/* note that when this happens, we're actually lucky
		   that it has failed - imagine another thread may
		   meanwhile have created something on the same file
		   descriptor number, and has registered it; the
		   epoll_ctl() call above would then have succeeded,
		   but broke the other thread's epoll registration */
		scheduled_flags = 0;
#endif

	return success;
}
