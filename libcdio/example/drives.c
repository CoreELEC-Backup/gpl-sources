/*
  Copyright (C) 2003, 2004, 2006, 2008, 2009, 2012
  Rocky Bernstein <rocky@gnu.org>
  
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

/* Simple program to show drivers installed and what the default 
   CD-ROM drive is and what CD drives are available. */
#ifdef HAVE_CONFIG_H
#include "config.h"
#define __CDIO_CONFIG_H__ 1
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#include <cdio/cdio.h>
#include <cdio/cd_types.h>
#include <cdio/logging.h>

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

static void 
print_drive_class(const char *psz_msg, cdio_fs_anal_t bitmask, bool b_any) {
  char **ppsz_cd_drives=NULL, **c;

  printf("-- %s...\n", psz_msg);
  ppsz_cd_drives = cdio_get_devices_with_cap(NULL, bitmask, b_any);
  if (NULL != ppsz_cd_drives) 
    for( c = ppsz_cd_drives; *c != NULL; c++ ) {
      printf("-- Drive %s\n", *c);
    }

  cdio_free_device_list(ppsz_cd_drives);
  printf("-----\n");
}

int
main(int argc, const char *argv[])
{
  char **ppsz_cd_drives=NULL, **c;
  
  cdio_log_set_handler (log_handler);

  /* Print out a list of CD-drives */
  ppsz_cd_drives = cdio_get_devices(DRIVER_DEVICE);
  if (NULL != ppsz_cd_drives) 
    for( c = ppsz_cd_drives; *c != NULL; c++ ) {
      printf("-- Drive %s\n", *c);
    }

  cdio_free_device_list(ppsz_cd_drives);
  ppsz_cd_drives = NULL;
  
  printf("-----\n");

  /* Print out a list of CD-drives the harder way. */
  print_drive_class("-- All CD-ROM drives (again)", CDIO_FS_MATCH_ALL, false);
  print_drive_class("-- CD-ROM drives with a CD-DA loaded...",
		    CDIO_FS_AUDIO, false);
  print_drive_class("-- CD-ROM drives with some sort of ISO 9660 filesystem...", 
		    CDIO_FS_ANAL_ISO9660_ANY, true);
  print_drive_class("-- (S)VCD drives...", CDIO_FS_ANAL_VCD_ANY, true);
  return 0;
  
}
