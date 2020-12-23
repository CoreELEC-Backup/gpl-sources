/*
 *      vdr-plugin-vnsi - KODI server plugin for VDR
 *
 *      Copyright (C) 2003-2006 Petri Hintukainen
 *      Copyright (C) 2010 Alwin Esch (Team XBMC)
 *      Copyright (C) 2011 Alexander Pipelka
 *      Copyright (C) 2015 Team KODI
 *
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with KODI; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

/*
 * Socket wrapper classes
 *
 * Code is taken from xineliboutput plugin.
 *
 */

#include "cxsocket.h"
#include "config.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/tcp.h>
#include <net/if.h>

#include <vdr/config.h>
#include <vdr/tools.h>

#ifndef MSG_MORE
#define MSG_MORE 0
#endif

cxSocket::cxSocket(int h)
  :m_fd(h), m_pollerRead(m_fd), m_pollerWrite(m_fd, true)
{
}

cxSocket::~cxSocket()
{
  if (m_fd >= 0)
    close(m_fd);
}

void cxSocket::Shutdown()
{
  if (m_fd >= 0)
    ::shutdown(m_fd, SHUT_RD);
}

void cxSocket::LockWrite()
{
  m_MutexWrite.Lock();
}

void cxSocket::UnlockWrite()
{
  m_MutexWrite.Unlock();
}

int cxSocket::GetHandle()
{
  return m_fd;
}

void cxSocket::Invalidate()
{
  m_fd = -1;
}

ssize_t cxSocket::write(const void *buffer, size_t size, int timeout_ms, bool more_data)
{
  cMutexLock CmdLock(&m_MutexWrite);

  if (m_fd < 0)
    return 0;

  ssize_t written = (ssize_t)size;
  const unsigned char *ptr = (const unsigned char *)buffer;

  while (size > 0)
  {
    if (!m_pollerWrite.Poll(timeout_ms))
    {
      ERRORLOG("cxSocket::write(fd=%d): poll() failed", m_fd);
      return written-size;
    }

    ssize_t p = ::send(m_fd, ptr, size, (more_data ? MSG_MORE : 0));

    if (p <= 0)
    {
      if (errno == EINTR || errno == EAGAIN)
      {
        DEBUGLOG("cxSocket::write(fd=%d): EINTR during write(), retrying", m_fd);
        continue;
      }
      else if (errno != EPIPE)
        ERRORLOG("cxSocket::write(fd=%d): write() error", m_fd);
      return p;
    }

    ptr  += p;
    size -= p;
  }

  return written;
}

ssize_t cxSocket::read(void *buffer, size_t size, int timeout_ms)
{
  if (m_fd < 0)
    return 0;

  int retryCounter = 0;

  ssize_t missing = (ssize_t)size;
  unsigned char *ptr = (unsigned char *)buffer;

  while (missing > 0)
  {
    if(!m_pollerRead.Poll(timeout_ms))
    {
      ERRORLOG("cxSocket::read(fd=%d): poll() failed at %d/%d", m_fd, (int)(size-missing), (int)size);
      return size-missing;
    }

    ssize_t p = ::read(m_fd, ptr, missing);

    if (p < 0)
    {
      if (retryCounter < 10 && (errno == EINTR || errno == EAGAIN))
      {
        DEBUGLOG("cxSocket::read(fd=%d): EINTR/EAGAIN during read(), retrying", m_fd);
        retryCounter++;
        continue;
      }
      ERRORLOG("cxSocket::read(fd=%d): read() error at %d/%d", m_fd, (int)(size-missing), (int)size);
      return 0;
    }
    else if (p == 0)
    {
      INFOLOG("cxSocket::read(fd=%d): eof, connection closed", m_fd);
      return 0;
    }

    retryCounter = 0;
    ptr  += p;
    missing -= p;
  }

  return size;
}

char *cxSocket::ip2txt(uint32_t ip, unsigned int port, char *str)
{
  // inet_ntoa is not thread-safe (?)
  if (str)
  {
    unsigned int iph =(unsigned int)ntohl(ip);
    unsigned int porth =(unsigned int)ntohs(port);
    if (!porth)
    {
      sprintf(str, "%d.%d.%d.%d",
	      ((iph>>24)&0xff), ((iph>>16)&0xff),
	      ((iph>>8)&0xff), ((iph)&0xff));
    }
    else
    {
      sprintf(str, "%u.%u.%u.%u:%u",
	      ((iph>>24)&0xff), ((iph>>16)&0xff),
	      ((iph>>8)&0xff), ((iph)&0xff),
	      porth);
    }
  }
  return str;
}
