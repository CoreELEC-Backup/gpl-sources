/** \file    common.c
  *
  * \brief   Common functions for simple tasks
  * \date    (C) 2009
  * \author  wolfgang astleitner (mrwastl@users.sourceforge.net)
  */

/*
 *************************************************************************
 * This program is free software; you can redistribute it and/or modify   
 * it under the terms of the GNU General Public License as published by   
 * the Free Software Foundation; either version 2 of the License, or (at  
 * your option) any later version.                                        
 *                                                                        
 * This program is distributed in the hope that it will be useful, but    
 * WITHOUT ANY WARRANTY; without even the implied warranty of             
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      
 * General Public License for more details.                               
 *                                                                        
 * You should have received a copy of the GNU General Public License      
 * along with this program; if not, write to the Free Software            
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA              
 * 02111-1307, USA.  Or, point your browser to                            
 * http://www.gnu.org/copyleft/gpl.html                                   
 *************************************************************************
 */

#include "../config.h"

#include <stdio.h>
#include <string.h>

#include "common.h"
#include "serdisplib/serdisp_tools.h"

/* uint16_t, uint32_t, ... */
#include <inttypes.h>

/**
  * \brief   reads an uncompressed fixed size BMP image into a pre-allocated buffer
  *
  * \param   bmpfile       file name of BMP file to read
  * \param   imgbuffer     pre-allocated image buffer (size: ((width/8) * height) * depth)
  * \param   width         requested width of image (error if different)
  * \param   height        requested height of image (error if different)
  * \param   depth         requested depth of image (error if different)
  *
  * \retval   0            success
  * \retval  -1            error occurred (error message: sd_geterrormsg())
  *
  * \since  1.97.9
  */
int common_read_simplebmp(char* bmpfile, byte* imgbuffer, int width, int height, int depth) {
  byte bmpheader[54];  /* no structure because of alignment and endian problems */
  byte dummybuffer[8]; /* dummy place for reading alignment row data */
  FILE* f = NULL;
  int rc;
  uint32_t w, h, compr, bpp, offset;
  int rowsize_aligned = ((depth * width + 31 ) / 32 ) * 4;  /* 32-bit dwords */
  int rowsize = ((depth * width) / 8);
  int row;

  if ( (f = fopen(bmpfile, "rb")) == NULL) {
    sd_error(SERDISP_EACCES, "%s(): unable to open file %s", __func__, bmpfile);
    return -1;
  }

  /* nota bene: BMP header is little endian! */
  rc = fread(bmpheader, 1, sizeof(bmpheader), f);

  w =      (bmpheader[21]<<24) + (bmpheader[20]<<16) + (bmpheader[19]<<8) + bmpheader[18];
  h =      (bmpheader[25]<<24) + (bmpheader[24]<<16) + (bmpheader[23]<<8) + bmpheader[22];
  compr =  (bmpheader[33]<<24) + (bmpheader[32]<<16) + (bmpheader[31]<<8) + bmpheader[30];
  bpp =    (bmpheader[29]<<8) + bmpheader[28];
  offset = (bmpheader[13]<<24) + (bmpheader[12]<<16) + (bmpheader[11]<<8) + bmpheader[10];

  if (rc != sizeof(bmpheader) || bmpheader[0] != 0x42 || bmpheader[1] != 0x4d || 
      /* avoid out of bounds error when dummy reading into imgbuffer */
      ((offset - sizeof(bmpheader)) > 20) || ( ((int)offset - sizeof(bmpheader)) < 0)
     ) {
    sd_error(SERDISP_ERUNTIME, "%s(): no valid BMP file: %s", __func__, bmpfile);
    fclose(f);
    return -1;
  }


  if (w != width || h != height || compr != 0 || bpp != depth || depth != 1) {
  /* only depth == 1 supported at the moment */
#if 0
    if (depth == 1)
#endif
      sd_error(SERDISP_ERUNTIME,
               "%s(): format of BMP %s must be: %d/%d, monochrome (1 bits/pixel), uncompressed",
               __func__, bmpfile, width, height
              );
#if 0
    else
      sd_error(SERDISP_ERUNTIME,
               "%s(): format of BMP %s must be: %d/%d, depth %d, uncompressed",
               __func__, bmpfile, width, height, depth
              );
#endif
    fclose(f);
    return -1;
  }

  /* advance to offset of image data (dummy read) */
  rc = fread(imgbuffer, 1, offset - sizeof(bmpheader), f);
  if (rc != (offset - sizeof(bmpheader)) ) {
    sd_error(SERDISP_ERUNTIME, "%s(): corrupt BMP file: %s", __func__, bmpfile);
    fclose(f);
    return -1;
  }

  /* read image data */
  row = height - 1;
  while (row >= 0) {
    int rc2 = 0;
    rc = fread(&(imgbuffer[row*rowsize]), 1, rowsize, f);
    if (rc > 0 && rowsize_aligned > rowsize) {
      rc2 = fread(dummybuffer, 1, rowsize_aligned - rowsize, f);
    }
    if ( (rc != rowsize) || (rc2 < 0)) {
      sd_error(SERDISP_ERUNTIME, "%s(): corrupt BMP file: %s", __func__, bmpfile);
      fclose(f);
      return -1;
    }
    row --;
  }

  fclose(f);
  return 0;
}


/**
  * \brief   writes an uncompressed BMP image to a file
  *
  * \param   bmpfile       file name of BMP file to write
  * \param   imgbuffer     image buffer (size: ((width/8) * height) * depth)
  * \param   width         width of image
  * \param   height        height of image
  * \param   depth         depth of image
  * \param   overwrite     overwrite existing file (1) or return an error
  *
  * \retval   0            success
  * \retval  -1            error occurred (error message: sd_geterrormsg())
  *
  * \since  1.97.9
  */
int common_write_simplebmp (char* bmpfile, byte* imgbuffer, int width, int height, int depth, int overwrite) {
  byte bmpheader[54 + (1 << depth)*4];  /* no structure because of alignment and endian problems */
  byte dummybuffer[8]; /* dummy place for reading alignment row data */
  FILE* f = NULL;
  int rc;
  uint32_t temp;
  int rowsize_aligned = ((depth * width + 31 ) / 32 ) * 4;  /* 32-bit dwords */
  int rowsize = ((depth * width) / 8);
  int row;

  memset(bmpheader, 0, sizeof(bmpheader));
  memset(dummybuffer, 0, sizeof(dummybuffer));

  bmpheader[0] = 0x42; bmpheader[1] = 0x4d;  /* magic ID 'BM' */
  temp = sizeof(bmpheader) + rowsize * height;  /* complete file size in bytes */
  bmpheader[2] = temp & 0x000000FF;         bmpheader[3] = (temp & 0x0000FF00) >> 8;
  bmpheader[4] = (temp & 0x00FF0000) >> 16; bmpheader[5] = (temp & 0xFF000000) >> 24;
  /* 6 - 9: reserved */
  temp = sizeof(bmpheader);  /* offset from beginning of file to bitmap data */
  bmpheader[10] = temp & 0x000000FF;         bmpheader[11] = (temp & 0x0000FF00) >> 8;
  bmpheader[12] = (temp & 0x00FF0000) >> 16; bmpheader[13] = (temp & 0xFF000000) >> 24;
  temp = 0x28;  /* length of bitmap info header: 28h for windows BMPs */
  bmpheader[14] = temp & 0x000000FF;         bmpheader[15] = (temp & 0x0000FF00) >> 8;
  bmpheader[16] = (temp & 0x00FF0000) >> 16; bmpheader[17] = (temp & 0xFF000000) >> 24;
  temp = width;  /* pixel width */
  bmpheader[18] = temp & 0x000000FF;         bmpheader[19] = (temp & 0x0000FF00) >> 8;
  bmpheader[20] = (temp & 0x00FF0000) >> 16; bmpheader[21] = (temp & 0xFF000000) >> 24;
  temp = height; /* pixel height */
  bmpheader[22] = temp & 0x000000FF;         bmpheader[23] = (temp & 0x0000FF00) >> 8;
  bmpheader[24] = (temp & 0x00FF0000) >> 16; bmpheader[25] = (temp & 0xFF000000) >> 24;
  temp = 1;      /* number of planes, must be 1 */
  bmpheader[26] = temp & 0x000000FF;         bmpheader[27] = (temp & 0x0000FF00) >> 8;
  temp = depth;  /* bits per pixel */
  bmpheader[28] = temp & 0x000000FF;         bmpheader[29] = (temp & 0x0000FF00) >> 8;
  /* 30 - 33: compression typ -> uncompressed = 0  */
  temp = rowsize * height;  /* size of bitmap data */
  bmpheader[34] = temp & 0x000000FF;         bmpheader[35] = (temp & 0x0000FF00) >> 8;
  bmpheader[36] = (temp & 0x00FF0000) >> 16; bmpheader[37] = (temp & 0xFF000000) >> 24;
  temp = 0x0B13;  /* hor/vert resolution in pixel per meter (whatever this is for ...) */
  bmpheader[38] = temp & 0x000000FF;         bmpheader[39] = (temp & 0x0000FF00) >> 8;
  bmpheader[40] = (temp & 0x00FF0000) >> 16; bmpheader[41] = (temp & 0xFF000000) >> 24;
  bmpheader[42] = temp & 0x000000FF;         bmpheader[43] = (temp & 0x0000FF00) >> 8;
  bmpheader[44] = (temp & 0x00FF0000) >> 16; bmpheader[45] = (temp & 0xFF000000) >> 24;
  temp = (1 << depth);   /* number of colours in image */
  bmpheader[46] = temp & 0x000000FF;         bmpheader[47] = (temp & 0x0000FF00) >> 8;
  bmpheader[48] = (temp & 0x00FF0000) >> 16; bmpheader[49] = (temp & 0xFF000000) >> 24;
  temp = (1 << depth);   /* number of important colours in image */
  bmpheader[50] = temp & 0x000000FF;         bmpheader[51] = (temp & 0x0000FF00) >> 8;
  bmpheader[52] = (temp & 0x00FF0000) >> 16; bmpheader[53] = (temp & 0xFF000000) >> 24;
  /* first colour == white */
  bmpheader[54] = 0xFF; bmpheader[55] = 0xFF; bmpheader[56] = 0xFF; bmpheader[57] = 0x00;

  if (!overwrite) {
    /* test if file already exists */
    if ( (f = fopen(bmpfile, "r")) ) {
      fclose(f);
      sd_error(SERDISP_EACCES, "%s(): file %s already exists and no permission to overwrite", __func__, bmpfile);
      return -1;
    }
  }

  if ( (f = fopen(bmpfile, "wb")) == NULL) {
    sd_error(SERDISP_EACCES, "%s(): unable to open file %s", __func__, bmpfile);
    return -1;
  }

  rc = fwrite(bmpheader, 1, sizeof(bmpheader), f);

  /* read image data */
  row = height - 1;
  while (row >= 0) {
    int rc2 = 0;
    rc = fwrite(&(imgbuffer[row*rowsize]), 1, rowsize, f);
    if (rc > 0 && rowsize_aligned > rowsize) {
      rc2 = fwrite(dummybuffer, 1, rowsize_aligned - rowsize, f);
    }
    if ( (rc != rowsize) || (rc2 < 0)) {
      sd_error(SERDISP_ERUNTIME, "%s(): error when writing BMP file: %s", __func__, bmpfile);
      fclose(f);
      return -1;
    }
    row --;
  }

  fclose(f);
  return 0;
}
