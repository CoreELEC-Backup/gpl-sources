/*
 * utils.c
 *
 * Copyright (C) 2009 Hector Martin <hector@marcansoft.com>
 * Copyright (C) 2009 Nikias Bassen <nikias@gmx.li>
 * Copyright (c) 2013 Federico Mena Quintero
 *
 * This library is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 2.1 of the
 * License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#ifdef __APPLE__
#include <mach/mach_time.h>
#endif

#include "utils.h"

#include "log.h"
#define util_error(...) usbmuxd_log(LL_ERROR, __VA_ARGS__)

void fdlist_create(struct fdlist *list)
{
	list->count = 0;
	list->capacity = 4;
	list->owners = malloc(sizeof(*list->owners) * list->capacity);
	list->fds = malloc(sizeof(*list->fds) * list->capacity);
}
void fdlist_add(struct fdlist *list, enum fdowner owner, int fd, short events)
{
	if(list->count == list->capacity) {
		list->capacity *= 2;
		list->owners = realloc(list->owners, sizeof(*list->owners) * list->capacity);
		list->fds = realloc(list->fds, sizeof(*list->fds) * list->capacity);
	}
	list->owners[list->count] = owner;
	list->fds[list->count].fd = fd;
	list->fds[list->count].events = events;
	list->fds[list->count].revents = 0;
	list->count++;
}

void fdlist_free(struct fdlist *list)
{
	list->count = 0;
	list->capacity = 0;
	free(list->owners);
	list->owners = NULL;
	free(list->fds);
	list->fds = NULL;
}

void fdlist_reset(struct fdlist *list)
{
	list->count = 0;
}

#define CAPACITY_STEP 8

void collection_init(struct collection *col)
{
	col->list = malloc(sizeof(void *) * CAPACITY_STEP);
	memset(col->list, 0, sizeof(void *) * CAPACITY_STEP);
	col->capacity = CAPACITY_STEP;
}

void collection_free(struct collection *col)
{
	free(col->list);
	col->list = NULL;
	col->capacity = 0;
}

void collection_add(struct collection *col, void *element)
{
	int i;
	for(i=0; i<col->capacity; i++) {
		if(!col->list[i]) {
			col->list[i] = element;
			return;
		}
	}
	col->list = realloc(col->list, sizeof(void*) * (col->capacity + CAPACITY_STEP));
	memset(&col->list[col->capacity], 0, sizeof(void *) * CAPACITY_STEP);
	col->list[col->capacity] = element;
	col->capacity += CAPACITY_STEP;
}

void collection_remove(struct collection *col, void *element)
{
	int i;
	for(i=0; i<col->capacity; i++) {
		if(col->list[i] == element) {
			col->list[i] = NULL;
			return;
		}
	}
	util_error("collection_remove: element %p not present in collection %p (cap %d)", element, col, col->capacity);
}

int collection_count(struct collection *col)
{
	int i, cnt = 0;
	for(i=0; i<col->capacity; i++) {
		if(col->list[i])
			cnt++;
	}
	return cnt;
}

void collection_copy(struct collection *dest, struct collection *src)
{
	if (!dest || !src) return;
	dest->capacity = src->capacity;
	dest->list = malloc(sizeof(void*) * src->capacity);
	memcpy(dest->list, src->list, sizeof(void*) * src->capacity);
}

#ifndef HAVE_STPCPY
/**
 * Copy characters from one string into another
 *
 * @note: The strings should not overlap, as the behavior is undefined.
 *
 * @s1: The source string.
 * @s2: The destination string.
 *
 * @return a pointer to the terminating `\0' character of @s1,
 * or NULL if @s1 or @s2 is NULL.
 */
char *stpcpy(char * s1, const char * s2)
{
	if (s1 == NULL || s2 == NULL)
		return NULL;

	strcpy(s1, s2);

	return s1 + strlen(s2);
}
#endif

#ifndef HAVE_CLOCK_GETTIME
typedef int clockid_t;
#define CLOCK_MONOTONIC 1

static int clock_gettime(clockid_t clk_id, struct timespec *ts)
{
	// See http://developer.apple.com/library/mac/qa/qa1398

	uint64_t mach_time, nano_sec;

	static mach_timebase_info_data_t base_info;

	mach_time = mach_absolute_time();

	if (base_info.denom == 0) {
		(void) mach_timebase_info(&base_info);
	}

	if (base_info.numer == 1 && base_info.denom == 1)
		nano_sec = mach_time;
	else
		nano_sec = mach_time * base_info.numer / base_info.denom;

	ts->tv_sec = nano_sec / 1000000000;
	ts->tv_nsec = nano_sec % 1000000000;

	return 0;
}
#endif

void get_tick_count(struct timeval * tv)
{
	struct timespec ts;
	if(0 == clock_gettime(CLOCK_MONOTONIC, &ts)) {
		tv->tv_sec = ts.tv_sec;
		tv->tv_usec = ts.tv_nsec / 1000;
	} else {
		gettimeofday(tv, NULL);
	}
}

/**
 * Get number of milliseconds since the epoch.
 */
uint64_t mstime64(void)
{
	struct timeval tv;
	get_tick_count(&tv);

	// Careful, avoid overflow on 32 bit systems
	// time_t could be 4 bytes
	return ((long long)tv.tv_sec) * 1000LL + ((long long)tv.tv_usec) / 1000LL;
}
