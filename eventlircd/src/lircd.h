/*
 * Copyright (C) 2009-2010 Paul Bender.
 *
 * This file is part of eventlircd.
 *
 * eventlircd is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * eventlircd is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with eventlircd.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef _EVENTLIRCD_LIRCD_H_
#define _EVENTLIRCD_LIRCD_H_ 1

/*
 * Single Unix Specification Version 3 headers.
 */
#include <sys/stat.h>     /* POSIX */
/*
 * Linux headers.
 */
#include <linux/input.h>  /* */

int lircd_init(const char *path, mode_t mode, const char *release_suffix);
int lircd_exit();
int lircd_send(const struct input_event *event, const char *name, unsigned int repeat_count, const char *remote);

#endif
