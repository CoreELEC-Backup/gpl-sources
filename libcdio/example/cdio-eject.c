/*
  Copyright (C) 2007, 2008, 2009 Rocky Bernstein <rocky@gnu.org>
  
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
#ifdef HAVE_CONFIG_H
#include "config.h"
#define __CDIO_CONFIG_H__ 1
#endif

#ifdef HAVE_STDIO_H
#include <stdio.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <cdio/cdio.h>

static void usage(char * progname)
  {
  fprintf(stderr, "Usage: %s [-t] <device>\n", progname);
  }

int main(int argc, char ** argv)
  {
  driver_return_code_t err;
  int close_tray = 0;
  const char * device = NULL;
  
  if(argc < 2 || argc > 3)
    {
    usage(argv[0]);
    return -1;
    }

  if((argc == 3) && strcmp(argv[1], "-t"))
    {
    usage(argv[0]);
    return -1;
    }

  if(argc == 2)
    device = argv[1];
  else if(argc == 3)
    {
    close_tray = 1;
    device = argv[2];
    }

  if(close_tray)
    {
    err = cdio_close_tray(device, NULL);
    if(err)
      {
      fprintf(stderr, "Closing tray failed for device %s: %s\n",
              device, cdio_driver_errmsg(err));
      return -1;
      }
    }
  else
    {
    err = cdio_eject_media_drive(device);
    if(err)
      {
      fprintf(stderr, "Ejecting failed for device %s: %s\n",
              device, cdio_driver_errmsg(err));
      return -1;
      }
    }

  return 0;
  
  }
