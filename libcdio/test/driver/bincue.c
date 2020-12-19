/* -*- C -*-
  Copyright (C) 2004, 2006, 2008, 2010, 2011, 2012
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

/*
   Regression test for BIN/CUE device driver: lib/driver/image/bincue.c.
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
#ifdef HAVE_UNISTD_H
#include <unistd.h> /* chdir */
#endif

#include <cdio/cdio.h>
#include <cdio/logging.h>
#include "helper.h"

#ifndef DATA_DIR
#define DATA_DIR "../data"
#endif

#define NUM_GOOD_CUES 2
#define NUM_BAD_CUES 7
int
main(int argc, const char *argv[])
{
  const char *cue_file[NUM_GOOD_CUES] = {
    "cdda.cue",
    "isofs-m1.cue",
  };

  const char *badcue_file[NUM_BAD_CUES] = {
    "bad-cat1.cue",
    "bad-cat2.cue",
    "bad-cat3.cue",
    "bad-mode1.cue",
    "bad-msf-1.cue",
    "bad-msf-2.cue",
    "bad-msf-3.cue",
  };
  int ret=0;
  unsigned int i;
  char psz_cuefile[500];
  unsigned int verbose = (argc > 1);

  psz_cuefile[sizeof(psz_cuefile)-1] = '\0';
  cdio_loglevel_default = (argc > 1) ? CDIO_LOG_DEBUG : CDIO_LOG_WARN;
  for (i=0; i<NUM_GOOD_CUES; i++) {
    char *psz_binfile;
    snprintf(psz_cuefile, sizeof(psz_cuefile)-1,
             "%s/%s", DATA_DIR, cue_file[i]);
    psz_binfile = cdio_is_cuefile(psz_cuefile);
    if (!psz_binfile) {
      printf("Incorrect: %s doesn't parse as a CDRWin CUE file.\n",
             cue_file[i]);
      ret=i+1;
    } else {
        if (verbose)
            printf("Correct: %s parses as a CDRWin CUE file.\n",
                   cue_file[i]);
      free(psz_binfile);
    }
  }

  for (i=0; i<NUM_BAD_CUES; i++) {
    char *psz_binfile;
    snprintf(psz_cuefile, sizeof(psz_cuefile)-1,
             "%s/%s", DATA_DIR, badcue_file[i]);
    psz_binfile=cdio_is_cuefile(psz_cuefile);
    if (!psz_binfile) {
        if (verbose)
            printf("Correct: %s doesn't parse as a CDRWin CUE file.\n",
                   badcue_file[i]);
    } else {
      printf("Incorrect: %s parses as a CDRWin CUE file.\n",
             badcue_file[i]);
      free(psz_binfile);
      ret+=50*i+1;
      break;
    }
  }

  {
    CdIo_t *p_cdio;
    snprintf(psz_cuefile, sizeof(psz_cuefile)-1,
             "%s/%s", DATA_DIR, "cdda.cue");
    p_cdio  = cdio_open (psz_cuefile, DRIVER_UNKNOWN);
    if (!p_cdio) {
      printf("Can't open cdda.cue\n");
    } else {
      char *psz_device;

      /* Just test performing some operations. */
      cdio_set_blocksize(p_cdio, 2048);

#ifdef HAVE_CHDIR
      if (0 == chdir(DATA_DIR))
#endif
      {
          psz_device = cdio_get_default_device(p_cdio);

          check_mmc_supported(p_cdio, 1);
          check_access_mode(p_cdio, "image");
          // check_get_arg_source(p_cdio, psz_device);

          /* Could chdir to srcdir to hedge the bet? */
          if (psz_device)
              free(psz_device);
          else {
              /* Unless we do the chdir, will fail. So don't set as an
               * error. */
              printf("Can't get default device\n");
          }
          cdio_set_speed(p_cdio, 5);
      }

      cdio_destroy(p_cdio);

    }

  }

  return ret;
}
