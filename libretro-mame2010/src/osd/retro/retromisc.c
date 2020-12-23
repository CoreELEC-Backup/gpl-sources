#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#ifndef WIIU
#include <sys/mman.h>
#else
#include <stdlib.h>
#endif
#include <signal.h>
#ifdef MAME_DEBUG
#include <unistd.h>
#endif
#endif

// MAME headers
#include "osdcore.h"


//============================================================
//  osd_alloc_executable
//
//  allocates "size" bytes of executable memory.  this must take
//  things like NX support into account.
//============================================================

void *osd_alloc_executable(size_t size)
{
#if defined(_WIN32) && !defined(_XBOX)
   return VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
#elif defined(__APPLE__)
   return (void *)mmap(0, size, PROT_EXEC|PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, -1, 0);
#elif defined(WIIU)
	return malloc(size);
#else
   return (void *)mmap(0, size, PROT_EXEC|PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED, 0, 0);
#endif
}

//============================================================
//  osd_free_executable
//
//  frees memory allocated with osd_alloc_executable
//============================================================

void osd_free_executable(void *ptr, size_t size)
{
#if defined(WIN32)
   VirtualFree(ptr, 0, MEM_RELEASE);
#elif defined(WIIU)
	free(ptr);
#else
   munmap(ptr, size);
#endif
}

//============================================================
//  osd_break_into_debugger
//============================================================

void osd_break_into_debugger(const char *message)
{
	#ifdef MAME_DEBUG
	printf("MAME exception: %s\n", message);
	printf("Attempting to fall into debugger\n");
	kill(getpid(), SIGTRAP);
	#else
	printf("Ignoring MAME exception: %s\n", message);
	#endif
}
