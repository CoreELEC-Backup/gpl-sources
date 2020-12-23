/** \file    serdisp_tools.c
  *
  * \brief   Common functions
  * \date    (C) 2003-2010
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

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <strings.h>

#include <sys/time.h>
#if 0
#if defined(__NetBSD__) || defined(__FreeBSD__) || defined(__OpenBSD__)
 #include <sys/time.h>
#else
 #include <sys/resource.h>
#endif
#endif

#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"

/* to be sure to undef malloc vs. rpl_malloc hacks done by configure */
#undef malloc

/* table containing  byte = swapped(byte) */
/* taken from graphlcd/drivers/fct.c */
static const int bit_reverse_table[256] = {
0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0,
0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8,
0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4,
0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec,
0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2,
0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea,
0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6,
0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee,
0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1,
0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9,
0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5,
0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed,
0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3,
0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb,
0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7,
0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef,
0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};


/* table containing bitmasks needed for serdisp_get/setpixel() */
static const byte pixel_bitmask_table[] = {
   0,  /* depth 0: non existant */
0x01,  /* depth 1: 0000 0001 */
0x03,  /* depth 2: 0000 0011 */
   0,  /* depth 3: not supported */
0x0F,  /* depth 4: 0000 1111 */
   0,  /* depth 5: not supported */
   0,  /* depth 6: not supported */
   0,  /* depth 7: not supported */
   0   /* depth 8: not supported */
};



/**
  * \brief   workaround for error-prone strncpy
  *
  * strncpy with workarounds avoiding strncpy-bugs found on solaris10 and/or glibc-linked programs
  *
  * \param   dest          destination string
  * \param   src           source string
  * \param   n             amount of characters to be copied
  *
  * \return pointer to the destination string
  *
  * \since   1.95
  */
char* sdtools_strncpy(char *dest, const char *src, size_t n) {
#if defined(__sun__)
  return (char*) strlcpy(dest, src, n+1);
#else
  char* rv = strncpy(dest, src, n);
  dest[ (strlen(src) < n) ? strlen(src) : n ] = '\0';
  return rv;
#endif
}


/**
  * \brief   zero-byte save malloc
  *
  * zero-byte save malloc (some versions of malloc are defective when size is zero
  *
  * \param   size          amount of bytes to allocate
  *
  * \return pointer to allocated memory (or \p NULL if requested memory could not be allocated)
  *
  * \since   1.97
  */
void *sdtools_malloc(size_t size) {
  if (!size) return (void*)0;
  return (void*)malloc(size);
}


/**
  * \brief   rotates the display buffer
  *
  * rotates the display buffer (and updates the display)
  *
  * \param   dd            device descriptor
  *
  * \since   1.97.2
  */
void sdtools_generic_rotate(serdisp_t* dd) {
  int x, y, w, h, temp;

  w = serdisp_getwidth(dd);
  h = serdisp_getheight(dd);

  for (y = 0; y < h; y++)
    for (x = 0; x < (w+1)/2; x++) {
      temp = serdisp_getpixel(dd, x, y);
      serdisp_setpixel(dd, x, y,  serdisp_getpixel(dd, w-1-x, h-1-y));
      serdisp_setpixel(dd, w-1-x, h-1-y, temp);
    }
  serdisp_update(dd);
}


/**
  * \brief   changes a pixel into the display buffer
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  * \param   colour        monochrome: 0: clear (white), <>0: set (black); else: grey value (supported depths: 1, 2, 4)
  */
void sdtools_generic_setpixel (serdisp_t* dd, int x, int y, long colour) {
  int x_i = 0, y_i = 0;

  if (dd->curr_rotate <= 1) {
    if (x >= dd->width || y >= dd->height || x < 0 || y < 0)
      return;
  } else {
    if (x >= dd->height || y >= dd->width || x < 0 || y < 0)
      return;
  }

  switch (dd->curr_rotate) {
    case 0:  /* 0 degrees */
      x_i = (dd->xreloctab) ? dd->xreloctab[x] : x;
      y_i = (dd->yreloctab) ? dd->yreloctab[y] : y;
      break;
    case 1:  /* 180 degrees */
      x_i = (dd->xreloctab) ? dd->xreloctab[dd->width  - 1 - x] : (dd->width  - 1 - x);
      y_i = (dd->yreloctab) ? dd->yreloctab[dd->height - 1 - y] : (dd->height - 1 - y);
      break;
    case 2:  /* 90 degrees */
      x_i = (dd->xreloctab) ? dd->xreloctab[y]                  : y;
      y_i = (dd->yreloctab) ? dd->yreloctab[dd->height - 1 - x] : (dd->height - 1 - x);
      break;
    case 3:  /* 270 degrees */
      x_i = (dd->xreloctab) ? dd->xreloctab[dd->width  - 1 - y] : (dd->width  - 1 - y);
      y_i = (dd->yreloctab) ? dd->yreloctab[x]                  : x;
      break;
  }

  if (dd->depth < 8) {
    int page, pagediv = 8 / dd->depth;
    byte value_orig, value, mask;
    byte coltrans;    /* mask out all inapplicable bits */
    page = y_i / pagediv;

    value_orig = dd->scrbuf[ (dd->width + dd->xcolgaps) * page  +  x_i];

    mask = pixel_bitmask_table[dd->depth] << ((y_i % pagediv)*dd->depth);
    coltrans = (byte)colour & pixel_bitmask_table[dd->depth];
    value = ((0xFF ^ mask) & value_orig) | (coltrans << (((y_i % pagediv))*dd->depth));

    if (value_orig != value) {
      dd->scrbuf[ (dd->width + dd->xcolgaps) * page  +  x_i] = value;

      if (dd->scrbuf_chg) {
        int idx = x_i + (dd->width + dd->xcolgaps) *(page/8);

        /* catch out of bound errors that may occur because of a bug in the display driver */
        if (idx >= dd->scrbuf_chg_size) {
          sd_debug(0, "sdtools_generic_setpixel(): OUTOFBOUND: idx>=scrbuf_chg_size: %d >= %d   x/y: %d/%d  x_i/y_i: %d/%d", 
                      idx, dd->scrbuf_chg_size, x,y, x_i,y_i);
        } else {
          dd->scrbuf_chg[idx] |= (1 << (page%8));
        }
      }
    }
  } else {
    int changed = 0;
    int bitspercol = (dd->depth == 18) ? 24 : dd->depth; /* for speed reasons, align depth 18 to 24 bits in screen buffer */
    int idx_2 = ((x_i  + y_i * (dd->width + dd->xcolgaps)) * (bitspercol << 1)) >> 3;
    int idx = idx_2 >> 1;

    switch(dd->depth) {
      case 8:
        if (dd->scrbuf[idx] != colour) {
          dd->scrbuf[idx] = colour;
          changed = 1;
        }
        break;
      case 12:
        {
          byte r = (colour & 0x00000F00) >> 8;
          byte g = (colour & 0x000000F0) >> 4;
          byte b = (colour & 0x0000000F);
          if (idx_2 & 0x01) { /* RGB -> 2nd nipple of first byte, second byte */
            if ( (dd->scrbuf[idx] & 0x0F) != r) {
              dd->scrbuf[idx] =  (dd->scrbuf[idx] & 0xF0) + r;
              changed = 1;
            }
            if ( dd->scrbuf[idx+1] != ( (g<<4) + b) ) {
              dd->scrbuf[idx+1] =  (g<<4) + b;
              changed = 1;
            }
          } else {
            if ( dd->scrbuf[idx] != ( (r<<4) + g) ) {
              dd->scrbuf[idx] =  (r<<4) + g;
              changed = 1;
            }
            if ( (dd->scrbuf[idx+1] & 0xF0) != (b << 4)) {
              dd->scrbuf[idx+1] =  (dd->scrbuf[idx+1] & 0x0F) + (b << 4);
              changed = 1;
            }
          }
        }
        break;
      case 16:
        if ( dd->scrbuf[idx] != ((colour & 0xFF00) >> 8) ) {
          dd->scrbuf[idx] = ((colour & 0xFF00) >> 8);
          changed = 1;
        }
        if ( dd->scrbuf[idx+1] != (colour & 0xFF) ) {
          dd->scrbuf[idx+1] = (colour & 0x00FF);
          changed = 1;
        }
        break;
      case 18:
        if ( dd->scrbuf[idx] != ((colour & 0x03F000L) >> 12) ) {
          dd->scrbuf[idx] = ((colour & 0x03F000L) >> 12);
          changed = 1;
        }
        if ( dd->scrbuf[idx+1] != ((colour & 0x000FC0L) >> 6) ) {
          dd->scrbuf[idx+1] = ((colour & 0x000FC0L) >> 6);
          changed = 1;
        }
        if ( dd->scrbuf[idx+2] != (colour & 0x00003FL) ) {
          dd->scrbuf[idx+2] = (colour & 0x00003FL);
          changed = 1;
        }
        break;
      case 24:
        if ( dd->scrbuf[idx] != ((colour & 0xFF0000L) >> 16) ) {
          dd->scrbuf[idx] = ((colour & 0xFF0000L) >> 16);
          changed = 1;
        }
        if ( dd->scrbuf[idx+1] != ((colour & 0x00FF00L) >> 8) ) {
          dd->scrbuf[idx+1] = ((colour & 0x00FF00L) >> 8);
          changed = 1;
        }
        if ( dd->scrbuf[idx+2] != (colour & 0x0000FFL) ) {
          dd->scrbuf[idx+2] = (colour & 0x0000FFL);
          changed = 1;
        }
        break;
      case 32:
        if ( dd->scrbuf[idx] != ((colour & 0xFF000000L) >> 24) ) {
          dd->scrbuf[idx] = ((colour & 0xFF000000L) >> 24);
          changed = 1;
        }
        if ( dd->scrbuf[idx+1] != ((colour & 0x00FF0000L) >> 16) ) {
          dd->scrbuf[idx+1] = ((colour & 0x00FF0000L) >> 16);
          changed = 1;
        }
        if ( dd->scrbuf[idx+2] != ((colour & 0x0000FF00L) >> 8) ) {
          dd->scrbuf[idx+2] = ((colour & 0x0000FF00L) >> 8);
          changed = 1;
        }
        if ( dd->scrbuf[idx+3] != (colour & 0x0000FFL) ) {
          dd->scrbuf[idx+3] = (colour & 0x000000FFL);
          changed = 1;
        }
        break;
    }

    if (changed && dd->scrbuf_chg) {
      /* ceil( dd->width / 8) bytes per row. one byte == one change info (== one changed pixel) */
      idx = (x_i >> 3) + y_i * ((dd->width + 7 ) >> 3);

      /* catch out of bound errors that may occur because of a bug in the display driver */
      if (idx >= dd->scrbuf_chg_size) {
        sd_debug(0, "sdtools_generic_setpixel(): OUTOFBOUND: idx>=scrbuf_chg_size: %d >= %d   x/y: %d/%d  x_i/y_i: %d/%d", 
                    idx, dd->scrbuf_chg_size, x,y, x_i,y_i);
      } else
        dd->scrbuf_chg[ idx ] |=  (1 << (x_i % 8));
    }
  }
}


/**
  * \brief   gets a pixel from the display buffer
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  * \return  hardware-dependent grey-level or colour information at (x/y)
  */
long sdtools_generic_getpixel (serdisp_t* dd, int x, int y) {
  int x_i = 0, y_i = 0;
  long value = 0L;

  if (dd->curr_rotate <= 1) {
    if (x >= dd->width || y >= dd->height || x < 0 || y < 0)
      return 0;
  } else {
    if (x >= dd->height || y >= dd->width || x < 0 || y < 0)
      return 0;
  }

  switch (dd->curr_rotate) {
    case 0:  /* 0 degrees */
      x_i = (dd->xreloctab) ? dd->xreloctab[x] : x;
      y_i = (dd->yreloctab) ? dd->yreloctab[y] : y;
      break;
    case 1:  /* 180 degrees */
      x_i = (dd->xreloctab) ? dd->xreloctab[dd->width  - 1 - x] : (dd->width  - 1 - x);
      y_i = (dd->yreloctab) ? dd->yreloctab[dd->height - 1 - y] : (dd->height - 1 - y);
      break;
    case 2:  /* 90 degrees */
      x_i = (dd->xreloctab) ? dd->xreloctab[y]                  : y;
      y_i = (dd->yreloctab) ? dd->yreloctab[dd->height - 1 - x] : (dd->height - 1 - x);
      break;
    case 3:  /* 270 degrees */
      x_i = (dd->xreloctab) ? dd->xreloctab[dd->width  - 1 - y] : (dd->width  - 1 - y);
      y_i = (dd->yreloctab) ? dd->yreloctab[x]                  : x;
      break;
  }

  if (dd->depth < 8) {
    int page, pagediv = 8 / dd->depth;
    byte mask;
    byte value_byte;

    page = y_i / pagediv;

    value_byte = dd->scrbuf[ (dd->width + dd->xcolgaps) * page  +  x_i];

    mask = pixel_bitmask_table[dd->depth] << ((y_i % pagediv)*dd->depth);

    value_byte &= mask;

    value = (long)(value_byte >> ((y_i % pagediv)*dd->depth));
  } else {
    int bitspercol = (dd->depth == 18) ? 24 : dd->depth; /* for speed reasons, align depth 18 to 24 bits in screen buffer */
    int idx_2 = ((x_i  + y_i * (dd->width + dd->xcolgaps)) * (bitspercol << 1)) >> 3;
    int idx = idx_2 >> 1;


    switch(dd->depth) {
      case 8:
        value = (long) dd->scrbuf[idx];
        break;
      case 12:
        {
          if (idx_2 & 0x01)
            value = (long)(((dd->scrbuf[idx] & 0x0F) << 8) +  dd->scrbuf[idx+1]);
          else
            value = (long)( (dd->scrbuf[idx] << 4) +  ((dd->scrbuf[idx+1] & 0xF0) >> 4));
        }
        break;
      case 16:
        value = (long)((dd->scrbuf[idx] << 8) + dd->scrbuf[idx+1]);
        break;
      case 18:
        value = (long)(((0x3F & dd->scrbuf[idx]) << 12) | ((0x3F & dd->scrbuf[idx+1]) << 6) | (0x3F & dd->scrbuf[idx+2]));
        break;
      case 24:
        value = (long)((dd->scrbuf[idx] << 16) | (dd->scrbuf[idx+1] << 8) | dd->scrbuf[idx+2]);
        break;
      case 32:
        value = (long)((dd->scrbuf[idx] << 24) | (dd->scrbuf[idx+1] << 16) | (dd->scrbuf[idx+2] << 8) | dd->scrbuf[idx+3]);
        break;
    }

  }

  return value;
}


/**
  * \brief   changes a pixel into the display buffer (horizontally organised w/o pages)
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  * \param   colour        monochrome: 0: clear (white), <>0: set (black); else: grey value (dependent on display)
  */
void sdtools_generic_setpixel_greyhoriz (serdisp_t* dd, int x, int y, long colour) {
  int x_i = 0, y_i = 0, col;
  byte value_orig, value, mask;
  int cols;
  int idx = -1;

  if (dd->curr_rotate <= 1) {
    if (x >= dd->width || y >= dd->height || x < 0 || y < 0)
      return;
  } else {
    if (x >= dd->height || y >= dd->width || x < 0 || y < 0)
      return;
  }

  switch (dd->curr_rotate) {
    case 0:  /* 0 degrees */
      x_i = (dd->xreloctab) ? dd->xreloctab[x] : x;
      y_i = (dd->yreloctab) ? dd->yreloctab[y] : y;
      break;
    case 1:  /* 180 degrees */
      x_i = (dd->xreloctab) ? dd->xreloctab[dd->width  - 1 - x] : (dd->width  - 1 - x);
      y_i = (dd->yreloctab) ? dd->yreloctab[dd->height - 1 - y] : (dd->height - 1 - y);
      break;
    case 2:  /* 90 degrees */
      x_i = (dd->xreloctab) ? dd->xreloctab[y]                  : y;
      y_i = (dd->yreloctab) ? dd->yreloctab[dd->height - 1 - x] : (dd->height - 1 - x);
      break;
    case 3:  /* 270 degrees */
      x_i = (dd->xreloctab) ? dd->xreloctab[dd->width  - 1 - y] : (dd->width  - 1 - y);
      y_i = (dd->yreloctab) ? dd->yreloctab[x]                  : x;
      break;
  }

  if (dd->depth == 1) {
    byte scrbuf_bits_used = dd->scrbuf_bits_used;

    cols = (dd->width + dd->xcolgaps + scrbuf_bits_used -1) / scrbuf_bits_used;

    col = x_i / scrbuf_bits_used;

    value_orig = dd->scrbuf[ cols * y_i + col ];

    mask = 1 << (scrbuf_bits_used-1-(x_i % scrbuf_bits_used));

    value = (colour) ? (value_orig | mask) : (value_orig & (0xFF ^ mask));

    idx = ((cols + 7) / (8 / dd->depth) ) * y_i + (col / (8 / dd->depth) ) ;
  } else {
    int coldiv = 8 / dd->depth;
    int mask_shift = ( x_i % coldiv) * dd->depth;

    cols = (dd->width + dd->xcolgaps) / coldiv;

    col = x_i / coldiv;

    value_orig = dd->scrbuf [ cols * y_i + col ];

    mask = pixel_bitmask_table[dd->depth] << mask_shift;

    value = ((0xFF ^ mask) & value_orig) | ((byte)colour << mask_shift);

    idx = y_i * (cols >> 3) + (col >> 3);
/*fprintf(stderr, "value: 0x%02x   value_orig: 0x%02x    col=%d  mask=0x%02x\n", (byte)value, value_orig, col, mask);*/
  }

  if (value_orig != value) {
    dd->scrbuf[ cols * y_i + col ] = value;

    if (dd->scrbuf_chg) {
      /* catch out of bound errors that may occur because of a bug in the display driver */
      if (idx >= dd->scrbuf_chg_size) {
        sd_debug(1, "%s(): OUTOFBOUND: idx>=scrbuf_chg_size: %d >= %d   x/y: %d/%d  x_i/y_i: %d/%d", __func__,
                    idx, dd->scrbuf_chg_size, x,y, x_i,y_i);
      } else {
        dd->scrbuf_chg[idx] |= (1 << (col % 8));
      }
    }
  }
}


/**
  * \brief   gets a pixel from the display buffer (horizontally organised w/o pages)
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  * \return  hardware-dependent grey-level or colour information at (x/y)
  */
long sdtools_generic_getpixel_greyhoriz (serdisp_t* dd, int x, int y) {
  int x_i = 0, y_i = 0, col;
  byte value, mask;
  int cols;

  if (dd->curr_rotate <= 1) {
    if (x >= dd->width || y >= dd->height || x < 0 || y < 0)
      return 0;
  } else {
    if (x >= dd->height || y >= dd->width || x < 0 || y < 0)
      return 0;
  }

  switch (dd->curr_rotate) {
    case 0:  /* 0 degrees */
      x_i = (dd->xreloctab) ? dd->xreloctab[x] : x;
      y_i = (dd->yreloctab) ? dd->yreloctab[y] : y;
      break;
    case 1:  /* 180 degrees */
      x_i = (dd->xreloctab) ? dd->xreloctab[dd->width  - 1 - x] : (dd->width  - 1 - x);
      y_i = (dd->yreloctab) ? dd->yreloctab[dd->height - 1 - y] : (dd->height - 1 - y);
      break;
    case 2:  /* 90 degrees */
      x_i = (dd->xreloctab) ? dd->xreloctab[y]                  : y;
      y_i = (dd->yreloctab) ? dd->yreloctab[dd->height - 1 - x] : (dd->height - 1 - x);
      break;
    case 3:  /* 270 degrees */
      x_i = (dd->xreloctab) ? dd->xreloctab[dd->width  - 1 - y] : (dd->width  - 1 - y);
      y_i = (dd->yreloctab) ? dd->yreloctab[x]                  : x;
      break;
  }

  if (dd->depth == 1) {
    byte scrbuf_bits_used = dd->scrbuf_bits_used;
    cols = (dd->width + dd->xcolgaps + scrbuf_bits_used -1) / scrbuf_bits_used;

    col = x_i / scrbuf_bits_used;

    value = dd->scrbuf[ cols * y_i + col ];

    mask = 1 << (scrbuf_bits_used-1-(x_i % scrbuf_bits_used));

    value = (value & mask) ? 1 : 0;
  } else {
    int coldiv = 8 / dd->depth;
    int mask_shift = ( x_i % coldiv) * dd->depth;

    cols = (dd->width + dd->xcolgaps) / coldiv;

    col = x_i / coldiv;

    value = dd->scrbuf [ cols * y_i + col ];

    mask = pixel_bitmask_table[dd->depth] << mask_shift;

    value = (value & mask) >> mask_shift;
  }
  return (value);
}


/**
  * \brief   sleeps for the specified number of nanoseconds
  *
  * \param   ns            amount of nanoseconds to sleep \n
  *                        ns = 0: no delay (do nothing) \n
  *                        ns = 1: do a simple delay (one call of <tt>gettimeofday()</tt>) \n
  *                        ns > 1: delay \em ns nanoseconds
  *
  * \attention
  * no scheduling or priority-calls are used because these might cause errors in MPEG-PES streaming of VDR. \n
  * another fact to keep in mind: time spent for scheduling and task-switching usually is much longer than one nanosecond.
  * thus sdtools_nsleep() \em cannot be accurate.
  *
  * \since   1.93
  */
void sdtools_nsleep(long ns) {
  if (ns > 1) {
    /* source: picport.cc by Jaakko Hyvatti
       http://www.iki.fi/hyvatti
    */
    struct timeval tv1, tv2;
    volatile int i;

    gettimeofday(&tv1, 0);
    tv2.tv_sec = tv1.tv_sec;
    tv2.tv_usec = 0xffffffff & (tv1.tv_usec + 1 + (ns + 999) / 1000);
    if (tv2.tv_usec < tv1.tv_usec)
      tv2.tv_sec++;
    for (i = 0; i < 10000; i++) {
      gettimeofday(&tv1, 0);
      if (tv1.tv_sec > tv2.tv_sec || (tv1.tv_sec == tv2.tv_sec && tv1.tv_usec >= tv2.tv_usec))
        break;
    }
  } else if (ns == 1) {
    struct timeval tv1;
    gettimeofday(&tv1, 0);
  }
  /* if <= 0: ignore sdtools_nsleep */
}


/**
  * \brief   swaps bits in a byte
  *
  * swaps all bits in a byte: bit7 becomes bit0, bit6 becomes bit1 and so on. \n
  * a predefined table containing all 256 dupels is used for faster execution. \n 
  * \n
  * Source: graphlcd-plugin: graphlcd/drivers/fct.c \n
  * http://www.powarman.de/ -> VDR Graphic LCD plugin
  *
  * \param   b             input byte
  *
  * \return  swapped byte
  *
  * \since   1.94
  */
byte sdtools_reversebits(byte b) {
  return bit_reverse_table[b];
}


/**
  * \brief   trims a string
  *
  * returns a char-pointer to the first non-space/tabulator character in string \em str. \n
  * if no leading spaces/tabulators: \em str is returned as is. \n
  * no extra memory is allocated.
  *
  * \param   str           string that should be trimmed
  *
  * \return  left-trimmed string
  *
  * \since   1.95
  */
char* sdtools_strlefttrim(const char* str) {
  int n = 0;

  int len = strlen(str);

  while ( n < len && (str[n] == ' ' || str[n] == '\t') )
    n++;

  return (char*) (&str[n]);
}


/**
  * \brief   length of a string without trailing spaces or tabulators
  *
  * returns length of string \em str without any trailing spaces or tabulators
  * (only \em len characters of \em str are used for this operation). \n
  * if no trailing spaces/tabulators is found: \em len will be returned.
  *
  * \param   str           string
  * \param   len           max. number of characters to be processed
  *
  * \return  length of string \em str without trailing spaces or tabulators
  *
  * \b Examples: \n
  *  \verbatim
     sdtools_strtimmedlen("test  ", 6);       -> 4 will be returned
     sdtools_strtimmedlen("test  ", 6);       -> 4 will be returned
     sdtools_strtimmedlen("test   ,xyz", 8);  -> 8 will be returned \endverbatim
  *
  * \since   1.95
  */
int sdtools_strtrimmedlen(const char* str, int len) {
  int n = len;

  while ( n > 0 && (str[n-1] == ' ' || str[n-1] == '\t') )
    n--;

  return n;
}


/**
  * \brief   tests of a string is part of a comma-separated list
  *
  * tests if string \em str is part of the comma-separated list \em elemlist. \n
  * leading and trailing spaces and tabs are ignored.
  * commas are interpreted as separators and are NOT valid in elements!
  *
  * \param   elemlist      comma-separated element list
  * \param   str           string to be searched
  * \param   len           max. number of characters to be processed (or \p -1 if unlimited)
  *
  * \retval   -1            string \em str is not part of the element list
  * \retval  >=0            \em str is the n-th element in the element list
  *
  * \b Examples: \n
  *  \verbatim
     sdtools_isinelemlist(" xyz , abc,cde ", "xyz", -1);   ->  0 will be returned
     sdtools_isinelemlist(" xyz , abc,cde ", "abc", -1);   ->  1 will be returned
     sdtools_isinelemlist(" xyz , abc,cde ", " cde", -1);  ->  2 will be returned (trailing spaces will be ignored)
     sdtools_isinelemlist(" xyz , abc,cde ", "ijk", -1);   -> -1 will be returned
     sdtools_isinelemlist(" xyz , abc,cde ", "c,c", -1);   -> -1 will be returned (',' is a separator but no valid character)
\endverbatim
  *
  * \since   1.95
  */
int sdtools_isinelemlist(const char* elemlist, const char* str, int len) {
  char* lefttrimmedstr = sdtools_strlefttrim(str);
  int nstr;
  int cnt = 0;

  char* lefttrimmedelemlist = (char*)elemlist;
  int nelemlist;
  char* indexpos;

  nstr = (len == -1 ) ? strlen(lefttrimmedstr) : len - serdisp_ptrstrlen(lefttrimmedstr, str);
  nstr = sdtools_strtrimmedlen(lefttrimmedstr, nstr);

  do {
    lefttrimmedelemlist = sdtools_strlefttrim(lefttrimmedelemlist);

    if (strlen(lefttrimmedelemlist) == 0) return -1;

    indexpos = index(lefttrimmedelemlist, ',');

    /* this should never occur but to be sure: catch it and return 0 in such a case (invalid elemlist!) */
    if (indexpos == lefttrimmedelemlist)
      return -1;

    nelemlist = sdtools_strtrimmedlen(
                  lefttrimmedelemlist, 
                  ((indexpos) ? serdisp_ptrstrlen(indexpos, lefttrimmedelemlist) : strlen(lefttrimmedelemlist))
                );

    if (nelemlist == nstr && strncasecmp(lefttrimmedelemlist, lefttrimmedstr, nelemlist) == 0) {
      return cnt;
    } else {
      if (indexpos) {
        if (strlen(indexpos) <= 1) return -1;  /* trailing ',' in elemlist */

        lefttrimmedelemlist=&indexpos[1]; /* skip separator */
      }
    }

    cnt ++;
  } while (indexpos);

  return -1;
}


/**
  * \brief   searches next pattern in a string
  *
  * searches the next pattern delimited with \em delim or \p \\0, leading spaces or tabs are ignored.
  *
  * \em len will contain the length of this pattern without any trainling spaces or tabs. \n
  * for the first pattern, \em len has to be set to \p -1 and, if a pattern can be found, will be changed to its length.
  *
  * \em border contains the maximum characters to take care of. all characters above \em border are ignored. \n
  * if a new pattern was found, \em border will be adapted.
  *
  * \param[in]     str     string to be searched
  * \param[in]     delim   delimiter (eg: <tt>','</tt> or <tt>';'</tt>)
  * \param[in,out] len     length of found pattern (w/o leading and trailing spaces or tabulators)
  * \param[in,out] border  amount of characters that are valid for searching patterns
  *
  * \retval  NULL          no more patterns available
  * \retval  !NULL         pointer to the found pattern (with leading spaces and tabulators eliminated)
  *
  * \b Examples: \n
  *  \verbatim
     str = "elem1=val1, elem2= val2 ,  elem3=val3   ;  xyz=abc, def = ghi ";
     border = 40;  -> ';' and all following characters are ignored
     len = -1;     -> start with -1 to get first parameter

     pattern = sdtools_nextpattern(str, ',', &len, &border);
     -> pattern will contain "elem1=........", len = 10; border = 40; so pattern => "elem1=val1"

     pattern = sdtools_nextpattern(str, ',', &len, &border);
     -> pattern will contain "elem2=........", len = 11; border = 22; so pattern => "elem2= val2"

     pattern = sdtools_nextpattern(str, ',', &len, &border);
     -> pattern will contain "elem3=........", len = 10; border = 13; so pattern => "elem3=val3"

     pattern = sdtools_nextpattern(str, ',', &len, &border);
     -> pattern will contain 0, because the border was exceeded
\endverbatim
  *
  * \since   1.95
  */
char* sdtools_nextpattern(const char* str, char delim, int* len, int* border) {
  char* strstart = (char*) str;
  int n;
  char* idxpos;

  if (*len >= 0) {  /* *len < 0: return first parameter, else: not first parameter */
    idxpos = index(strstart, delim);
    if ( !idxpos || (((long)idxpos) >= (((long)strstart) + *border)) ) { 
      /* no more delim or found delim out of view => no more param */
      *len = -1; *border = 0;
      return 0;
    }
    strstart = idxpos+1;
    *border -=  serdisp_ptrstrlen(strstart, str);
    *len = *border;
  } else { /* first parameter */
    *len = *border;
  }

  n = 0;
  while ( n < *len && (strstart[n] == ' ' || strstart[n] == '\t') )
    n++;

  strstart = &(strstart[n]);
  *border -= n;
  *len -= n;

  idxpos = index(strstart, delim);

  if ( idxpos && ((long)idxpos < ((long)strstart + (*border))) ) {
    *len = serdisp_ptrstrlen(idxpos, strstart);
  } else {
    *len = *border;
  }

  while ( *len > 0 && (strstart[(*len)-1] == ' ' || strstart[(*len)-1] == '\t') )
    (*len)--;

  return strstart;  
}


/**
  * \brief   compares two strings if they're matching
  *
  * compares two strings whether they match or not. case, leading spaces, and tabs are ignored.
  *
  * \param   str1          string 1
  * \param   len1          length of string 1 (all characters after \em len1 are ignored)
  * \param   str2          string 2
  * \param   len2          length of string 2 (all characters after \em len1 are ignored)
  *
  * \retval  1             strings match
  * \retval  0             strings don't match
  *
  * \since   1.95
  */
int sdtools_ismatching(const char* str1, int len1, const char* str2, int len2) {

  char* startptr1 = sdtools_strlefttrim(str1);
  char* startptr2 = sdtools_strlefttrim(str2);

  if (len1 == -1)
    len1 = strlen(str1);

  if (len2 == -1)
    len2 = strlen(str2);

  len1 -= serdisp_ptrstrlen(startptr1, str1);
  len2 -= serdisp_ptrstrlen(startptr2, str2);

  len1 = sdtools_strtrimmedlen(startptr1, len1);
  len2 = sdtools_strtrimmedlen(startptr2, len2);

  return (len1 == len2  && strncasecmp(startptr1, startptr2, len1) == 0);
}


/**
  * \brief   convert normalised contrast value to hardware contrast value
  *
  * \param   dd            device descriptor
  * \param   normval       normalised contrast value (in [0,MAX_CONTRASTSTEP])
  * \return  contrast value used by the hardware (in [min_contrast, max_contrast])
  *
  * \since   1.97.8
  */
int sdtools_contrast_norm2hw(serdisp_t* dd, int normval) {
  if (!dd->max_contrast || (dd->min_contrast >= dd->max_contrast))
    return 0;
  if (normval < 0)
    normval = 0;
  else if (normval > MAX_CONTRASTSTEP)
    normval = MAX_CONTRASTSTEP;

  if (dd->mid_contrast != 0 && dd->mid_contrast > dd->min_contrast && dd->mid_contrast < dd->max_contrast) {
    if (normval == (MAX_CONTRASTSTEP >> 1)) {
      return dd->mid_contrast;
    } else {
      int mid_i = dd->mid_contrast - dd->min_contrast;  /* shift [min ... mid ... max] to [0 ... mid' ... max'] */
      int max_i = dd->max_contrast - dd->min_contrast;
      /* expo-factor where normval => hwval = dd->mid_contrast */
      /* formula: p = ln(max'/mid') / ln(2) */
      double p = sdtools_log((double)max_i / (double)mid_i) / sdtools_log (2);

      /* find hardware contrast value:  hwval = min + max' * (normval/MAX_CONTRASTSTEP)^p */
      return dd->min_contrast + (int)(max_i * (sdtools_pow( ((double)normval / (double)MAX_CONTRASTSTEP), p))+0.5);
    }
  } else {
    /*
           (max - min) * cnorm + STEP/2
     chw = ---------------------------- + min  ;  STEP/2 corrects potential rounding error
                     STEP
    */
    return  ( ( (dd->max_contrast - dd->min_contrast) * normval + (MAX_CONTRASTSTEP >> 1)) / MAX_CONTRASTSTEP) + dd->min_contrast;
  }
}


/**
  * \brief   convert hardware contrast value to normalised contrast value
  *
  * \param   dd            device descriptor
  * \param   hwval         contrast value used by the hardware (in [min_contrast, max_contrast])
  * \return  normalised contrast value (in [0,MAX_CONTRASTSTEP])
  *
  * \since   1.97.8
  */
int sdtools_contrast_hw2norm(serdisp_t* dd, int hwval) {
  if (!dd->max_contrast || (dd->min_contrast >= dd->max_contrast))
    return MAX_CONTRASTSTEP >> 1;
  if (hwval < dd->min_contrast)
    hwval = dd->min_contrast;
  else if (hwval > dd->max_contrast)
    hwval = dd->max_contrast;
  if (dd->mid_contrast != 0 && dd->mid_contrast > dd->min_contrast && dd->mid_contrast < dd->max_contrast) {
    if (hwval == dd->mid_contrast) {
      return (MAX_CONTRASTSTEP >> 1);
    } else {
      int mid_i = dd->mid_contrast - dd->min_contrast;  /* shift [min ... mid ... max] to [0 ... mid' ... max'] */
      int max_i = dd->max_contrast - dd->min_contrast;
      /* expo-factor where normval => dd->mid_contrast */
      /* formula: p = ln(max'/mid') / ln(2) */
      double p = sdtools_log((double)max_i / (double)mid_i) / sdtools_log (2);
      int n = 0;
      int curr_val;

      /* the matching hwval is iterated, which avoid rounding errors. besides: this is no time critical function */
      while(n <= MAX_CONTRASTSTEP) {
        /* find hardware contrast value:  c_hw = min + max' * (n/MAX_CONTRASTSTEP)^p */
        curr_val = dd->min_contrast + (int)(max_i * (sdtools_pow( ((double)n / (double)MAX_CONTRASTSTEP), p))+0.5);
        /* lookup if hwval matches current hw contrast value: if yes: normval is found */
        if (hwval <= curr_val)
          return n;
        n++;
      }
      /* just for paranoia. code should never ever reach this line */
      return MAX_CONTRASTSTEP >> 1;
    }
  } else {
    /*
             (chw - min) * STEP + STEP/2
     cnorm = ---------------------------      ;  STEP/2 corrects potential rounding error
                    max-min
    */
    return  ( (hwval - dd->min_contrast) * MAX_CONTRASTSTEP + (MAX_CONTRASTSTEP >> 1)) / (dd->max_contrast - dd->min_contrast);
  }
}


/**
  * \brief   convert rotation radius in degrees to rotation value in internal representation
  *
  * \param   dd            device descriptor
  * \param   degval        radius in degrees (1==180) (or special case degree=2: toggle 0 <-> 180 or 90 <-> 270)
  * \return  rotation value in internal representation
  *
  * \since   1.97.8
  */
int sdtools_rotate_deg2intern(serdisp_t* dd, int degval) {
  int value = 0;

  /* calculate 'value' so that:
     value = 0 (B00):   0 degrees
             1 (B01): 180 degrees
             2 (B10):  90 degrees
             3 (B11): 270 degrees
  */
  switch (degval) {
    case SD_OPTION_TOGGLE:
      value = dd->curr_rotate  ^ 0x01; /* invert last bit */
      break;
    case   1:
    case 180:
      value = 1;
      break;
    case  90:
      value = 2;
      break;
    case 270:
      value = 3;
      break;
    default:
      value = 0;
      break;
  }
  return value;
}


/**
  * \brief   convert rotation value in internal representation to radius value in degrees
  *
  * \param   dd            device descriptor
  * \param   irepval       rotation value in internal representation
  * \return  rotation radius in degrees
  *
  * \since   1.97.8
  */
int sdtools_rotate_intern2deg(serdisp_t* dd, int irepval) {
  int retval = 0;
  switch (irepval) {
    case 0:
      retval = 0; 
      break;
    case 1:
      retval = 180;
      break;
    case 2:
      retval = 90;
      break;
    case 3:
      retval = 270;
      break;
    default:
      retval = 0;
      break;
  }
  return retval;
}


/**
  * \brief   convert a decimal number to BCD representation
  *
  * \param   num       decimal number (in [0, 99])
  * \return  BCD (binary coded decimal) representation
  *
  * \since   1.97.8
  */
byte sdtools_dec2bcd (byte num) {
  if (num > 99)
    num = 99;  /* safety */
  return ((num / 10) << 4) | (num % 10);
}


/**
  * \brief   calculates bounding box containing changed display information (including x/ycolgaps!)
  *
  * \param   dd        device descriptor
  * \param   hor       (valid only if depth < 8): \n
                       hor = 1: one byte == 1 to 4 pixels \n
                       hor = 0: pixels are organised in pages
  * \param   xt        x top-left
  * \param   yt        y top-left
  * \param   xb        x bottom-left
  * \param   yb        y bottom-left
  * \retval   0        no changed display information found in current bounding box
  * \retval   1        changes detected, new bbox calculated
  * \retval  -1        invalid values or display config: bbox returned == unchanged bbox or whole display area
  *
  * \since   1.97.9
  */
int sdtools_calc_bbox (serdisp_t* dd, int hor, int* xt, int* yt, int* xb, int* yb) {
  /* initial algo by fen <fen@init-6.org> */
  int found = 0;

  int xt_curr, yt_curr;
  int xb_curr, yb_curr;
  int w_abs;
  int h_abs;

  int x = 0, y = 0;
  int xstep = 1, ystep = 1;
  int cols = 0;   /* columns per line */
  int col_div = 0;

  /* scrbuf_bits_used != 8: not yet done */
  if (!dd || !dd->scrbuf_chg || (!hor && (dd->depth > 8)) || dd->scrbuf_bits_used != 8) {
    return -1;
  }

  w_abs = dd->width + dd->xcolgaps;
  h_abs = dd->height + dd->ycolgaps;

  /* auto init if no bounding box given at all */
  if (dd && *xt == 0 && *yt == 0 && *xb == 0 && *yb == 0) {
    *xb = w_abs - 1;
    *yb = h_abs - 1;
  }

  /* invalid input bounding box */
  if( *xt >= *xb || *yt >= *yb) {
    return -1;
  }

  if (dd->depth < 8) {
    col_div = 8 / dd->depth;
    if (hor) {
      xstep = col_div;
      cols = (w_abs + col_div - 1) / col_div;
    } else {
      ystep = col_div;
      cols = w_abs;
    }
  }

  /* align starting point (eg: xstep = 8: *xt = [0, 7] -> 0; *xt = [8, 15] -> 8, a.s.o.) */
  *xt = (*xt / xstep) * xstep;
  *yt = (*yt / ystep) * ystep;

   xt_curr = *xb;
   yt_curr = *yb;
   xb_curr = *xt;
   yb_curr = *yt;

  for (y = *yt; y <= *yb; y += ystep) {
    for (x = *xt; x <= *xb; x += xstep) {
      int isdirty = 0;
      if (hor)
         isdirty = dd->scrbuf_chg [ y * ((cols+col_div-1) / col_div) + ((x/col_div) / 8) ] & ( 1 <<  ( (x/col_div) % 8) );
      else
         isdirty = dd->scrbuf_chg [ ( (y/8) / ystep) * cols + x ] & ( 1 << ((y/8) % ystep));

      if ( isdirty ) {
        if (x < xt_curr) xt_curr = x;
        if (x > xb_curr) xb_curr = x;
        if (y < yt_curr) yt_curr = y;
        if (y > yb_curr) yb_curr = y;
        found = 1;
      }
    }
  }

  if (found) {
    *xt = xt_curr;
    *yt = yt_curr;
    *xb = ((xb_curr / xstep) * xstep) + xstep - 1;
    *yb = ((yb_curr / ystep) * ystep) + ystep - 1;
    if (*xb >= w_abs) *xb = w_abs -1;
    if (*yb >= h_abs) *yb = h_abs -1;
    return 1;
  }

  return 0;
}


#define SD_EPSILON        0.000000000001
#define SD_MAXITER        255

static int sdtools_isinepsilon(double x) {
  return (x >= 0.0 - SD_EPSILON && x <= 0.0 + SD_EPSILON) ? 1 : 0;
}

static double sdtools_abs( double x) {
  return (x < 0.0) ? -x : x;
}


/**
  * \brief   calculates the logarithm of x to base y
  *
  * calculates the logarithm of x to base y in a simple way without requiring math.h nor libm.
  * 
  * \attention
  * this function is not optimised for speed and only usable and accurate for simple usages
  * like calculating a grey value table or an expontential contrast table. \n
  * this function cannot calculate logN(0, base) nor a result for x < 1.0 and base < 1.0.
  * the result will simply be 0 which is incorrect.
  * (because we don't have NaN of inf because we don't include math.h)
  *
  * \param   x         value
  * \param   base      base of logarithm
  * \return            logarithm of x with base y
  *
  * \since   1.97.9
  */
double sdtools_logN (double x, double base) {
  double result = 0.0;
  double term;
  int n = 0;

  /* safety exit: result is INCORRECT in these cases! */
  if (sdtools_isinepsilon(x) || (x < 1.0 && base < 1.0))
    return 0.0;

  /* x < 1 */
  while (x < 1.0) {
    result -= 1.0;
    x *= base;
  }
  /* x >= base */
  while (x >= base) {
    result += 1.0;
    x /= base;
  }
  /* else: result starts with 0.0 */

  /* start with 1/2 */
  term = 0.5;
  x *= x;
  while (n < SD_MAXITER && term > SD_EPSILON) {
    if (x >= base) {
      result += term;
      x = x / base;
    }
    term *= 0.5;
    x *= x;
    n++;
  }
  return (result);
}


/**
  * \brief   calculates the natural logarithm of x
  *
  * calculates the natural logarithm of x in a simple way without requiring math.h nor libm.
  * 
  * \attention
  * this function is not optimised for speed and only usable and accurate for simple usages
  * like calculating a grey value table or an expontential contrast table. \n
  * this function cannot calculate log(0). the result will simply be 0 which is incorrect.
  * (because we don't have NaN of inf because we don't include math.h)
  *
  * \param   x         value
  * \return            natural logarithm of x
  *
  * \since   1.97.9
  */
double sdtools_log(double x) {
  /* ln(x) = log x to base e */
  return sdtools_logN(x, 2.7182818284590452354);
}


/**
  * \brief   base-e exponential function
  *
  * calculates the value of e raised to the power of x without requiring math.h nor libm.
  * 
  * \attention
  * this function is not optimised for speed and only usable and accurate for simple usages
  * like calculating a grey value table or an expontential contrast table.
  *
  * \param   x         value
  * \return            e raised to the power of x
  *
  * \since   1.97.9
  */
double sdtools_exp( double x) {
  /* exp(x) = SUM [ x^n / n! ]; n in [0, infinity] */
  long double fact=1.0;       /* n! */
  long double xpowi = 1.0;    /* x^n */
  long double term;           /* x^n / n! */
  long double sum = 1.0;      /* init. with n=0 => 1.0 */
  long double iterdiff = 0.0; /* difference between two iterations */
  long double lastterm=x+2*SD_EPSILON; /* the first value of item will be  x => start with x + 2*eps */
  int n;  /* iteration */

  n = 1;  /* sum has already been initialised with n=0 -> 1.0 */
  do {
    xpowi *= x;
    fact *= n;
    term = xpowi / fact;
    if (n > 10 && iterdiff < sdtools_abs(lastterm - term))
      return 0.0; /* iterations not converging: result = 0.0 */
    iterdiff = sdtools_abs(lastterm - term);
    sum += term;
    lastterm = term;
    n++;
  } while (n < SD_MAXITER && iterdiff >= SD_EPSILON );
  return (double)sum;
}


/**
  * \brief   power function
  *
  * calculates x raised to the power of y without requiring math.h nor libm.
  * 
  * \attention
  * this function is not optimised for speed and only usable and accurate for simple usages
  * like calculating a grey value table or an expontential contrast table.
  *
  * \param   x         value
  * \param   y         power
  * \return            x raised to the power of y
  *
  * \since   1.97.9
  */
double sdtools_pow( double x, double y) {
  /* pow(x, y) = e^(y * ln(x)) */
  return (sdtools_isinepsilon(x) ? 0.0 : sdtools_exp( y * sdtools_log(x)) );
}
