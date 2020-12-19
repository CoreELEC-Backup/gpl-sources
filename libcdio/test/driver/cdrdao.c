/*
  Copyright (C) 2004, 2008, 2010, 2011, 2012
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
   Regression test for cdio_tocfile.
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

#define NUM_GOOD_TOCS 17
#define NUM_BAD_TOCS 8
int
main(int argc, const char *argv[])
{
  const char *toc_file[NUM_GOOD_TOCS] = {
    "cdtext.toc", 
    "t1.toc", 
    "t2.toc", 
    "t3.toc", 
    "t4.toc", 
    "t5.toc", 
    "t6.toc", 
    "t7.toc", 
    "t8.toc", 
    "t9.toc",
    "t10.toc",
    "data1.toc",
    "data2.toc",
    "data5.toc",
    "data6.toc",
    "data7.toc",
    "vcd2.toc",
  };

  const char *badtoc_file[NUM_BAD_TOCS] = {
    "bad-msf-1.toc", 
    "bad-msf-2.toc",
    "bad-msf-3.toc",
    "bad-cat1.toc", 
    "bad-cat2.toc",
    "bad-cat3.toc",
    "bad-file.toc",
    "bad-mode1.toc"
  };
  int ret=0;
  unsigned int i;
  char psz_tocfile[500];
  unsigned int verbose = (argc > 1);
  

#ifdef HAVE_CHDIR
      if (0 == chdir(DATA_DIR))
#endif
      {
          psz_tocfile[sizeof(psz_tocfile)-1] = '\0';
  
          cdio_loglevel_default = verbose ? CDIO_LOG_DEBUG : CDIO_LOG_WARN;
          for (i=0; i<NUM_GOOD_TOCS; i++) {
              CdIo_t *p_cdio;
              snprintf(psz_tocfile, sizeof(psz_tocfile)-1,
                       "%s/%s", DATA_DIR, toc_file[i]);
              if (!cdio_is_tocfile(psz_tocfile) || 
                  !(p_cdio = cdio_open_cdrdao(psz_tocfile))) {
                  fprintf(stderr, 
                          "Incorrect: %s doesn't parse as a cdrdao TOC file.\n",                         toc_file[i]);
                  ret=i+1;
              } else {
                  cdio_destroy(p_cdio);
                  if (verbose)
                      printf("Correct: %s parses as a cdrdao TOC file.\n", 
                             toc_file[i]);
              }
          }
          
          for (i=0; i<NUM_BAD_TOCS; i++) {
              snprintf(psz_tocfile, sizeof(psz_tocfile)-1,
                       "%s/%s", DATA_DIR, badtoc_file[i]);
              if (!cdio_is_tocfile(psz_tocfile)) {
                  if (verbose)
                      printf("Correct: %s doesn't parse as a cdrdao TOC file.\n", 
                         badtoc_file[i]);
              } else {
                  fprintf(stderr, 
                          "Incorrect: %s parses as a cdrdao TOC file.\n", 
                          badtoc_file[i]);
                  ret+=50*i+1;
                  break;
              }
          }

          /*
          {
              CdIo_t *p_cdio;
              snprintf(psz_tocfile, sizeof(psz_tocfile)-1,
                       "%s/%s", DATA_DIR, "cdda.toc");

              p_cdio = cdio_open (psz_tocfile, DRIVER_CDRDAO);
              if (!p_cdio) {
                  fprintf(stderr, "Can't open %s as a cdrdao TOC file.\n", 
                          psz_tocfile);
                  exit(5);
              }
              
              check_mmc_supported(p_cdio, 1);
              check_access_mode(p_cdio, "image");
              check_get_arg_source(p_cdio, psz_tocfile);
              cdio_destroy(p_cdio);
          }
          */
          

      }
  
  return ret;
}
