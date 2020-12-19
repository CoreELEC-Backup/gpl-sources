/*
  $Id: eject.cpp,v 1.7 2008/03/24 15:30:57 karl Exp $

  Copyright (C) 2005, 2006, 2008 Rocky Bernstein <rocky@gnu.org>
  
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

/* Simple program to eject a CD-ROM drive door and then close it again.

   If a single argument is given, it is used as the CD-ROM device to 
   eject/close. Otherwise a CD-ROM drive will be scanned for.

   See also corresponding C program of a similar name. 
*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#define __CDIO_CONFIG_H__ 1
#endif

#include <stdio.h>
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <cdio++/cdio.hpp>

int
main(int argc, const char *argv[])
{
  driver_id_t driver_id = DRIVER_DEVICE;
  char *psz_drive = NULL;
  CdioDevice device;
  
  if (argc > 1) 
    psz_drive = strdup(argv[1]);

  if (!psz_drive) {
    psz_drive = getDefaultDevice(driver_id);
    if (!psz_drive) {
      printf("Can't find a CD-ROM to perform eject operation\n");
      exit(1);
    }
  }
  try {
    ejectMedia(psz_drive);
    printf("CD in CD-ROM drive %s ejected.\n", psz_drive);
  }
  catch ( DriverOpUninit e ) {
    printf("Can't Eject CD %s from CD-ROM drive: driver is not initialized.\n",
	   psz_drive);
  }
  catch ( DriverOpException e ) {
    printf("Ejecting CD from CD-ROM drive %s operation error:\n\t%s.\n", 
	   psz_drive, e.get_msg());
  }

  try {
    closeTray(psz_drive);
    printf("Closed CD-ROM %s tray.\n", psz_drive);
  }
  catch ( DriverOpException e ) {
    printf("Closing CD-ROM %s tray operation error error:\n\t%s.\n", 
	   psz_drive, e.get_msg());
  }
  free(psz_drive);
  
  return 0;
}
