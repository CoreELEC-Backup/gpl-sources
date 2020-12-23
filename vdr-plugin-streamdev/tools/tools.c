#include "tools/tools.h"

#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdarg.h>

void *operator new(size_t nSize, void *p) throw () {
	return p;
}

