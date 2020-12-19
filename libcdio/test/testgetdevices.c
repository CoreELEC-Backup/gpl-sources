/*
  Copyright (C) 2008-2009, 2011, 2013 Rocky Bernstein <rocky@gnu.org>

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
   Regression test for cdio_get_devices, cdio_get_devices_with_cap(),
   and cdio_free_device_list()
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
#ifdef HAVE_SYS_UTSNAME_H
#include <sys/utsname.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <cdio/cdio.h>
#include <cdio/cd_types.h>
#include <cdio/logging.h>

#ifndef DATA_DIR
#define DATA_DIR "./data"
#endif

#ifndef __MINGW32__

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


static bool
is_in(char **file_list, const char *file)
{
  char **p;
  for (p = file_list; p != NULL && *p != NULL; p++) {
    if (strcmp(*p, file) == 0) {
      printf("-- File %s found as expected\n", file);
      return true;
    }
  }
  printf("-- Can't find file %s in list\n", file);
  return false;
}
#endif

int
main(int argc, const char *argv[])
{
#if defined(__MINGW32__)
  printf("testgetdevices test skipped until drive recording testing issues resolved\n");
  return 77;
#else
  char **nrg_images=NULL;
  char **bincue_images=NULL;
  char **imgs;
  unsigned int i;
  int ret=0;

  const char *cue_files[2] = {"cdda.cue", "isofs-m1.cue"};
  const char *nrg_files[1] = {"videocd.nrg"};


  cdio_log_set_handler (log_handler);

  if (cdio_have_driver(-1) != false)
    {
      fprintf(stderr, "Bogus driver number -1 should be rejected\n");
      return 5;
    }

#ifdef HAVE_SYS_UTSNAME_H
  {
    struct utsname utsname;
    if (0 == uname(&utsname))
      {
	if (0 == strncmp("Linux", utsname.sysname, sizeof("Linux")))
	  {
	    if (!cdio_have_driver(DRIVER_LINUX))
	      {
		fprintf(stderr,
			"You should have been able to get GNU/Linux driver\n");
		return 6;
	      } else {
		printf("-- Good! You have the GNU/Linux driver installed.\n");
	      }

	  }
	else if (0 == strncmp("CYGWIN", utsname.sysname, sizeof("CYGWIN")))
	  {
	    if (!cdio_have_driver(DRIVER_WIN32))
	      {
		fprintf(stderr,
			"You should have been able to get Win32 driver\n");
		return 6;
	      } else {
		printf("-- Good! You have the Win32 driver installed.\n");
	      }
	  }
	else if (0 == strncmp("Darwin", utsname.sysname, sizeof("Darwin")))
	  {
	    if (!cdio_have_driver(DRIVER_OSX))
	      {
		fprintf(stderr,
			"You should have been able to get OS/X driver\n");
		return 6;
	      } else {
		printf("-- Good! You have the OS/X driver installed.\n");
	      }
	  }
	else if (0 == strncmp("NetBSD", utsname.sysname, sizeof("NetBSD")))
	  {
	    if (!cdio_have_driver(DRIVER_NETBSD))
	      {
		fprintf(stderr,
			"You should have been able to get NetBSD driver\n");
		return 6;
	      } else {
		printf("-- Good! You have the OS/X driver installed.\n");
	      }
	  }
      }
  }
#endif

  if (! (cdio_have_driver(DRIVER_NRG) && cdio_have_driver(DRIVER_BINCUE)) )  {
    printf("You don't have enough drivers for this test\n");
    exit(77);
  }

  bincue_images = cdio_get_devices(DRIVER_BINCUE);

  for (imgs=bincue_images; *imgs != NULL; imgs++) {
    printf("-- bincue image %s\n", *imgs);
  }

  if (ret != 0) return ret;

  if (0 == chdir(DATA_DIR)) {
    int invalid_images = 0;
    nrg_images = cdio_get_devices(DRIVER_NRG);

    for (imgs=nrg_images; *imgs != NULL; imgs++) {
      printf("-- NRG image %s\n", *imgs);
    }

    if (!is_in(nrg_images, nrg_files[0])) {
      cdio_free_device_list(nrg_images);
      return 10;
    }

    for (i=0; i<2; i++) {
      if (is_in(bincue_images, cue_files[i])) {
	printf("-- %s parses as a CDRWIN BIN/CUE csheet.\n",
	       cue_files[i]);
      } else {
	printf("-- %s doesn't parse as a CDRWIN BIN/CUE csheet.\n",
	       cue_files[i]);
	invalid_images += 1;
      }
    }
    printf("invaid images is %d\n", invalid_images);
    ret = invalid_images != 2;
  }

  cdio_free_device_list(nrg_images);
  cdio_free_device_list(bincue_images);
  return ret;
#endif
}
