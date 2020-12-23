#ifndef TOOLBOX_SELECT_H
#define TOOLBOX_SELECT_H

#include "tools/tools.h"

#include <sys/types.h>

/* cTBSelect provides an interface for polling UNIX-like file descriptors. */

class cTBSelect {
private:
	int    m_MaxFiled;

	fd_set m_FdsQuery[2];
	fd_set m_FdsResult[2];

public:
	cTBSelect(void);
	virtual ~cTBSelect();

	/* Clear() resets the object for use in a new Select() call. All file 
	   descriptors and their previous states are invalidated. */
	virtual void Clear(void);

	/* Add() adds a file descriptor to be polled in the next Select() call.
	   That call polls if the file is readable if Output is set to false, 
	   writeable otherwise. */
	virtual bool Add(int Filed, bool Output = false);

	/* Select() polls all descriptors added by Add() and returns as soon as
	   one of those changes state (gets readable/writeable), or after
	   TimeoutMs milliseconds, whichever happens first. It returns the number
	   of filedescriptors that have changed state. On error, -1 is returned
	   and errno is set appropriately. */
	virtual int Select(uint TimeoutMs);

	/* Select() polls all descriptors added by Add() and returns as soon as
	   one of those changes state (gets readable/writeable). It returns the 
	   number of filedescriptors that have changed state. On error, -1 is 
	   returned and errno is set appropriately. */
	virtual int Select(void);

	/* CanRead() returns true if the descriptor has changed to readable during
	   the last Select() call. Otherwise false is returned. */
	virtual bool CanRead(int FileNo) const;

	/* CanWrite() returns true if the descriptor has changed to writeable 
	   during the last Select() call. Otherwise false is returned. */
	virtual bool CanWrite(int FileNo) const;
};

inline void cTBSelect::Clear(void) {
	FD_ZERO(&m_FdsQuery[0]);
	FD_ZERO(&m_FdsQuery[1]);
	m_MaxFiled = -1;
}

inline bool cTBSelect::Add(int Filed, bool Output /* = false */) {
	if (Filed < 0) return false;
	FD_SET(Filed, &m_FdsQuery[Output ? 1 : 0]);
	if (Filed > m_MaxFiled) m_MaxFiled = Filed;
	return true;
}

inline bool cTBSelect::CanRead(int FileNo) const {
	if (FileNo < 0) return false;
	return FD_ISSET(FileNo, &m_FdsResult[0]);
}

inline bool cTBSelect::CanWrite(int FileNo) const {
	if (FileNo < 0) return false;
	return FD_ISSET(FileNo, &m_FdsResult[1]);
}

#endif // TOOLBOX_SELECT_H
