/* -*- C -*-
  Copyright (C) 2008, 2010, 2011, 2012 Rocky Bernstein <rocky@gnu.org>
  
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
   Regression test for Nero image driver: lib/driver/image/nrg.c.
*/
#if defined(HAVE_CONFIG_H)
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
#include <cdio/logging.h>
#include <cdio/cdtext.h>
#include "helper.h"

#ifndef DATA_DIR
#define DATA_DIR "../data"
#endif

#define NUM_FIELDS 2

int
main(int argc, const char *argv[])
{
  char psz_nrgfile[500];
  CdIo_t *p_cdio;
  const char *cdtext_check[NUM_FIELDS] = {
    "Richard Stallman", 
    "Join us now we have the software"
  };
  const int cdtext_fields[NUM_FIELDS] = {CDTEXT_FIELD_PERFORMER, CDTEXT_FIELD_TITLE};

  cdio_loglevel_default = (argc > 1) ? CDIO_LOG_DEBUG : CDIO_LOG_INFO;
  /* snprintf(psz_nrgfile, sizeof(psz_nrgfile)-1,
	     "%s/%s", DATA_DIR, cue_file[i]);
  */
  if (!cdio_have_driver(DRIVER_NRG)) return(77);
  
  snprintf(psz_nrgfile, sizeof(psz_nrgfile)-1, "%s/%s",  
	   DATA_DIR, "p1.nrg");

  p_cdio = cdio_open_nrg(psz_nrgfile);
  if (!p_cdio) {
    printf("Can't open Nero image file: %s.\n", psz_nrgfile);
    return(1);
  }
  {
    unsigned int i;
    cdtext_t *p_cdtext = cdio_get_cdtext(p_cdio);
    if (!p_cdtext) return(1);
    for (i=0; i<NUM_FIELDS; i++) {
      const char *psz_field = cdtext_get_const(p_cdtext, cdtext_fields[i], 0);
      if (!psz_field)
	return(2);
      if (0 != strncmp(psz_field, cdtext_check[i], strlen(cdtext_check[i]))) {
	printf("CD-Text compare mismatch.\n");
	printf("expected:\n\t'%s'\ngot:\n\t'%s'\n", 
	       cdtext_check[i], psz_field);
	return(3);
      }
    }
  }

  check_mmc_supported(p_cdio, 1);
  check_access_mode(p_cdio, "image");
  check_get_arg_source(p_cdio, psz_nrgfile);

  cdio_destroy(p_cdio);

  return 0;
}
