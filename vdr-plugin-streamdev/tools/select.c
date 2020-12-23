#include "tools/select.h"

#include <vdr/tools.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

cTBSelect::cTBSelect(void) {
	Clear();
}

cTBSelect::~cTBSelect() {
}

int cTBSelect::Select(uint TimeoutMs) {
	struct timeval tv;
	ssize_t res = 0;
	int ms;

	tv.tv_usec = (TimeoutMs % 1000) * 1000;
	tv.tv_sec = TimeoutMs / 1000;
	memcpy(m_FdsResult, m_FdsQuery, sizeof(m_FdsResult));

	if (TimeoutMs == 0)
		return ::select(m_MaxFiled + 1, &m_FdsResult[0], &m_FdsResult[1], NULL, &tv);

	cTimeMs starttime;
	ms = TimeoutMs;
	while (ms > 0 && (res = ::select(m_MaxFiled + 1, &m_FdsResult[0], &m_FdsResult[1], NULL, 
			&tv)) == -1 && errno == EINTR) {
		ms = TimeoutMs - starttime.Elapsed();
		tv.tv_usec = (ms % 1000) * 1000;
		tv.tv_sec = ms / 1000;
		memcpy(m_FdsResult, m_FdsQuery, sizeof(m_FdsResult));
	}
	if (ms <= 0 || res == 0) {
		errno = ETIMEDOUT;
		return -1;
	}
	return res;
}

int cTBSelect::Select(void) {
	ssize_t res;
	do {
		memcpy(m_FdsResult, m_FdsQuery, sizeof(m_FdsResult));
	} while ((res = ::select(m_MaxFiled + 1, &m_FdsResult[0], &m_FdsResult[1], NULL, NULL)) == -1 && errno == EINTR);
	return res;
}
