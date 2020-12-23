#ifndef TOOLBOX_SOURCE_H
#define TOOLBOX_SOURCE_H

#include "tools/tools.h"

#include <sys/types.h>
#include <string>

/* cTBSource provides an abstract interface for input and output. It can
   be used to have common access to different types of UNIX-files. */

class cTBSource {
private:
	int m_Filed;

	size_t m_BytesRead;
	size_t m_BytesWritten;

	std::string m_LineBuffer;

public:
	cTBSource(void);
	virtual ~cTBSource();

	/* SysRead() implements the low-level read on the source. It will store
	   data into the area pointed to by Buffer, which is at least Length
	   bytes in size. It will return the exact number of bytes read (which
	   can be fewer than requested). On error, -1 is returned, and errno
	   is set to an appropriate value. */
	virtual ssize_t SysRead(void *Buffer, size_t Length) const = 0;

	/* SysWrite() implements the low-level write on the source. It will write
	   at most Length bytes of the data pointed to by Buffer. It will return 
	   the exact number of bytes written (which can be fewer than requested). 
	   On error, -1 is returned, and errno is set to an appropriate value. */
	virtual ssize_t SysWrite(const void *Buffer, size_t Length) const = 0;

	/* IsOpen() returns true, if this source refers to a valid descriptor. 
	   It is not checked whether this source is really open, so only if 
	   opened by the appropriate Methods this function will return the 
	   correct value */
	virtual bool IsOpen(void) const { return m_Filed != -1; }

	/* Open() associates this source with the descriptor Filed, setting it
	   to non-blocking mode if IsUnixFd in true. Returns true on success,
	   and false on error, setting errno to appropriately. 
	   If you want to implement sources that can't be represented by UNIX 
	   filedescriptors, you can use Filed to store any useful information 
	   about the source.  
	   This must be called by any derivations in an appropriate Method (like
	   open for files, connect for sockets). */
	virtual bool Open(int Filed, bool IsUnixFd = true);

	/* Close() resets the source to the uninitialized state (IsOpen() == false)
	   and must be called by any derivations after really closing the source.
	   Returns true on success and false on error, setting errno appropriately.
	   The object is in closed state afterwards, even if an error occurred. */
	virtual bool Close(void);

	/* Read() reads at most Length bytes into the storage pointed to by Buffer,
	   which must be at least Length bytes in size, using the SysRead()-
	   Interface. It retries if an EINTR occurs (i.e. the low-level call was 
	   interrupted). It returns the exact number of bytes read (which can be 
	   fewer than requested). On error, -1 is returned, and errno is set 
	   appropriately. */
	ssize_t Read(void *Buffer, size_t Length);

	/* Write() writes at most Length bytes from the storage pointed to by 
	   Buffer, using the SysWrite()-Interface. It retries if EINTR occurs 
	   (i.e. the low-level call was interrupted). It returns the exact number 
	   of bytes written (which can be fewer than requested). On error, -1 is 
	   returned and errno is set appropriately. */
	ssize_t Write(const void *Buffer, size_t Length);

	/* TimedWrite() tries to write Length bytes from the storage pointed to by
	   Buffer within the time specified by TimeoutMs, using the Write()-
	   Interface. On success, true is returned. On error, false is returned
	   and errno is set appropriately. TimedRead only works on UNIX file 
	   descriptor sources. */
	bool TimedWrite(const void *Buffer, size_t Length, uint TimeoutMs);
	
	bool SafeWrite(const void *Buffer, size_t Length);

	/* ReadUntil() tries to read at most Length bytes into the storage pointed
	   to by Buffer, which must be at least Length bytes in size, within the
	   time specified by TimeoutMs, using the Read()-Interface. Reading stops 
	   after the character sequence Seq has been read and on end-of-file. 
	   Returns the number of bytes read (if that is equal to Length, you have
	   to check if the buffer ends with Seq), or -1 on error, in which case
	   errno is set appropriately. */
	ssize_t ReadUntil(void *Buffer, size_t Length, const char *Seq, 
			uint TimeoutMs);

	/* BytesRead() returns the exact number of bytes read through the Read()
	   method since Close() has been called on this source (or since its 
	   creation). */
	size_t BytesRead(void) const { return m_BytesRead; }

	/* BytesWritten() returns the exact number of bytes written through the 
	   Write() method since Close() has been called on this source (or since 
	   its creation). */
	size_t BytesWritten(void) const { return m_BytesWritten; }

	/* operator int() returns the descriptor (or informative number) associated
	   with this source. */
	operator int() const { return m_Filed; }
};

#endif // TOOLBOX_SOURCE_H
