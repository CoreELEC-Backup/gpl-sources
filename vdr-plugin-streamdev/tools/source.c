#include "tools/source.h"
#include "tools/select.h"
#include "common.h"

#include <vdr/tools.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

cTBSource::cTBSource(void) {
	m_BytesRead = 0;
	m_BytesWritten = 0;
	m_Filed = -1;
}

bool cTBSource::Open(int Filed, bool IsUnixFd) {
	if (IsOpen())
		Close();

	m_Filed = Filed;
	if (IsUnixFd && ::fcntl(m_Filed, F_SETFL, O_NONBLOCK) == -1)
		return false;

	return true;
}

cTBSource::~cTBSource() {
}

bool cTBSource::Close(void) {
	if (!IsOpen()) {
		errno = EBADF;
		return false;
	}

	m_Filed = -1;
	return true;
}

ssize_t cTBSource::Read(void *Buffer, size_t Length) {
	ssize_t res;
	while ((res = SysRead(Buffer, Length)) < 0 && errno == EINTR)
		errno = 0;
	if (res > 0) m_BytesRead += res;
	return res;
}

ssize_t cTBSource::Write(const void *Buffer, size_t Length) {
	ssize_t res;
	while ((res = SysWrite(Buffer, Length)) < 0 && errno == EINTR)
		errno = 0;
	if (res > 0) m_BytesWritten += res;
	return res;
}

bool cTBSource::TimedWrite(const void *Buffer, size_t Length, uint TimeoutMs) {
	cTBSelect sel;
	int ms, offs;
	cTimeMs starttime;

	offs = 0;
	sel.Clear();
	sel.Add(m_Filed, true);
	while (Length > 0) {
		int b;

		ms = TimeoutMs - starttime.Elapsed();
		if (ms <= 0) {
			errno = ETIMEDOUT;
			return false;
		}

		if (sel.Select(ms) == -1)
			return false;

		if (sel.CanWrite(m_Filed)) {
			if ((b = Write((char*)Buffer + offs, Length)) == -1)
				return false;
			offs += b;
			Length -= b;
		}

	}
	return true;
}

bool cTBSource::SafeWrite(const void *Buffer, size_t Length) {
	cTBSelect sel;
	int offs;

	offs = 0;
	sel.Clear();
	sel.Add(m_Filed, true);
	while (Length > 0) {
		int b;

		if (sel.Select() == -1)
			return false;

		if (sel.CanWrite(m_Filed)) {
			if ((b = Write((char*)Buffer + offs, Length)) == -1)
				return false;
			offs += b;
			Length -= b;
		}
	}
	return true;
}

ssize_t cTBSource::ReadUntil(void *Buffer, size_t Length, const char *Seq,
		uint TimeoutMs) {
	int ms;
	size_t len;
	cTBSelect sel;

	if ((len = m_LineBuffer.find(Seq)) != (size_t)-1) {
		if (len > Length) {
			errno = ENOBUFS;
			return -1;
		}
		memcpy(Buffer, m_LineBuffer.data(), len);
		m_LineBuffer.erase(0, len + strlen(Seq));
		Dprintf("ReadUntil: Served from Linebuffer: %d, |%.*s|\n", len, len - 1,
				(char*)Buffer);
		return len;
	}

	cTimeMs starttime;
	ms = TimeoutMs;
	sel.Clear();
	sel.Add(m_Filed, false);
	while (m_LineBuffer.size() < BUFSIZ) {

		if (sel.Select(ms) == -1)
			return -1;
		
		if (sel.CanRead(m_Filed)) {
			int b;

			len = m_LineBuffer.size();
			m_LineBuffer.resize(BUFSIZ);
			if ((b = Read((char*)m_LineBuffer.data() + len, BUFSIZ - len)) == -1) {
				m_LineBuffer.resize(len);
				return -1;
			}
			m_LineBuffer.resize(len + b);

			if ((len = m_LineBuffer.find(Seq)) != (size_t)-1) {
				if (len > Length) {
					errno = ENOBUFS;
					return -1;
				}
				memcpy(Buffer, m_LineBuffer.data(), len);
				m_LineBuffer.erase(0, len + strlen(Seq));
				Dprintf("ReadUntil: Served from Linebuffer: %d, |%.*s|\n", len, len - 1,
						(char*)Buffer);
				return len;
			}
		}

		ms = TimeoutMs - starttime.Elapsed();
		if (ms <= 0) {
			errno = ETIMEDOUT;
			return -1;
		}
	}
	errno = ENOBUFS;
	return -1;
}

