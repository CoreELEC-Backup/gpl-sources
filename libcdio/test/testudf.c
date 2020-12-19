/*
  Copyright (C) 2013 Rocky Bernstein <rocky@gnu.org>
  Copyright (C) 2013-2014 Pete Batard <pete@akeo.ie>

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

/* Tests reading UDF info from an UDF image.  */

#ifndef DATA_DIR
#define DATA_DIR "./data"
#endif
#define UDF_IMAGE DATA_DIR "/udf102.iso"

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
#ifdef HAVE_STRING_H
#include <string.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#include <cdio/cdio.h>
#include <cdio/udf.h>

#define EXPECTED_NAME    "FéжΘvrier"
#define EXPECTED_LENGTH  10

int
main(int argc, const char *argv[])
{
  char const *psz_fname = UDF_IMAGE;
  char volume_id[192];         /* 3*64 to account for UTF-8 */
  udf_t* p_udf = NULL;
  udf_dirent_t *p_udf_root = NULL, *p_udf_file = NULL;
  int64_t file_length;
  int rc=0;

  p_udf = udf_open(psz_fname);

  if (NULL == p_udf) {
    fprintf(stderr, "Couldn't open %s as an UDF image\n",
      psz_fname);
    return 1;
  }

  p_udf_root = udf_get_root(p_udf, true, 0);
  if (NULL == p_udf_root) {
    fprintf(stderr, "Could not locate UDF root directory\n");
    rc=2;
    goto exit;
  }

  if ( (udf_get_logical_volume_id(p_udf, volume_id, sizeof(volume_id)) <= 0)
    || (strcmp(EXPECTED_NAME, volume_id) != 0) ) {
    fprintf(stderr, "Unexpected UDF logical volume ID\n");
    rc=3;
    goto exit;
  }
  printf("-- Good! Volume id matches expected UTF-8 data\n");

  p_udf_file = udf_fopen(p_udf_root, EXPECTED_NAME);
  if (!p_udf_file) {
    fprintf(stderr, "Could not locate expected file in UDF image\n");
    rc=4;
    goto exit;
  }
  printf("-- Good! File name matches expected UTF-8 data\n");

  file_length = udf_get_file_length(p_udf_file);
  if (file_length != EXPECTED_LENGTH) {
    fprintf(stderr, "Unexpected UDF file length\n");
    rc=5;
    goto exit;
  }
  printf("-- Good! File length matches expected length\n");

 exit:
  if (p_udf_root != NULL)
    udf_dirent_free(p_udf_root);
  if (p_udf_file != NULL)
    udf_dirent_free(p_udf_file);
  udf_close(p_udf);
  return rc;
}
