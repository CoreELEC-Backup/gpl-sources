/*
  Copyright (C) 2013 Rocky Bernstein <rocky@gnu.org>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
   Regression test for lib/driver/track.c

   To compile as standalone program:
gcc -g3 -Wall -DHAVE_CONFIG_H -I../.. -I../../include track.c ../../lib/driver/.libs/libcdio.a -o track
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#define __CDIO_CONFIG_H__ 1
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <cdio/cdio.h>
#include <cdio/cd_types.h>
#include <cdio/logging.h>

#ifndef DATA_DIR
#define DATA_DIR "../data"
#endif

static void
log_handler (cdio_log_level_t level, const char message[])
{
  switch(level) {
  case CDIO_LOG_DEBUG:
  case CDIO_LOG_INFO:
    return;
  default:
    printf("cdio %d message: %s\n", level, message);
  }
}

int
main(int argc, const char *argv[])
{
  CdIo_t *cdObj;
  track_t i_track;
  lsn_t lsn;

  cdio_log_set_handler (log_handler);

  if (cdio_have_driver(DRIVER_BINCUE)) {
      cdObj = cdio_open(DATA_DIR "/cdda.cue", DRIVER_UNKNOWN);
  } else if (cdio_have_driver(DRIVER_CDRDAO)) {
      cdObj = cdio_open(DATA_DIR "/cdda.toc", DRIVER_UNKNOWN);
  } else {
    printf("-- You don't have enough drivers for this test\n");
    return 77;
  }

  i_track = cdio_get_track(cdObj, 1000);
  if (i_track != CDIO_INVALID_TRACK) {
      printf("LSN 1000 is too large should have gotten CDIO_INVALID_TRACK back\n");
      return 1;
  }

  for(lsn=0; lsn<10; lsn++) {
      i_track = cdio_get_track(cdObj, lsn);
      if (i_track != 1) {
	  printf("LSN %d should return 1. Got %d\n", lsn, i_track);
	  return 3;
      }
  }

  i_track = cdio_get_track(cdObj, 302);
  if (i_track != CDIO_CDROM_LEADOUT_TRACK) {
      printf("LSN %d should return leadout. Got %d\n", 302, i_track);
      return 4;
  }

  for(lsn=301; lsn > 300; lsn--) {
      i_track = cdio_get_track(cdObj, lsn);
      if (i_track != 1) {
	  printf("LSN %d should return 1. Got %d\n", lsn, i_track);
	  return 4;
      }
  }

  cdio_destroy(cdObj);

  return 0;
}
