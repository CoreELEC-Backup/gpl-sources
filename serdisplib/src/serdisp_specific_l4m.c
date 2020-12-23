/*
 *************************************************************************
 *
 * serdisp_specific_l4m.c
 * routines for controlling displays controlled by linux4media usb-interface
 *
 * supported:
 * - linux4media E-5i-USB interface with RC-LCD-B 128x64 LCD
 * - linux4media E-5i-USB interface with 132x65 colour display
 *
 *************************************************************************
 *
 * copyright (C) 2007-2010  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
 *
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>

#include <time.h>

#include "serdisplib/serdisp_connect.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_colour.h"


/* #define OPT_USEOLDUPDATEALGO */

/* 1 pixel == 2 byte (RGB 565) */
#define OPT_COLMODE2BYTE

/* different display types/models supported by this driver */
#define DISPID_L4ME5I       1
#define DISPID_L4M132C      2

#define MAXPACKETSIZE       60
#define MAXCHUNK            (MAXPACKETSIZE - 4)

serdisp_options_t serdisp_l4m_options[] = {
   /*  name          aliasnames   min    max mod int defines  */
   {  "RAWCMD",      "",            0,   255,  1, 1,  ""}      /* for development - internal use only */
};

serdisp_options_t serdisp_l4m132c_options[] = {
   /*  name          aliasnames   min    max mod int defines  */
   {  "DEPTH",       "COLS,COLOURS", 1,    16, 16, 0,  "MONOCHROME=1,MONO=1,64k=16"}
  ,{  "CONTRAST",    "",             0,    10,  1, 1,  ""}
  ,{  "RESMODE",     "",             0,     1,  1, 0,  "132x65=0,128x64=1"}
  ,{  "POSTOFFMODE", "POSTOFF",      0,     1,  1, 0,  "CLOCK=1"}
  ,{  "BGCOLOUR",    "BGCOLOR,BGCOL",0,0xFFFFFF,  1, 1,  ""}
  ,{  "FGCOLOUR",    "FGCOLOR,FGCOL",0,0xFFFFFF,  1, 1,  ""}
  ,{  "ALARMHOUR",   "ALHOUR",       0,    23,  1, 0,  ""}
  ,{  "ALARMMINUTE", "ALMIN",        0,    59,  1, 0,  ""}
  ,{  "ALARMDAYS",   "ALDAYS",       0,  0x7F,  1, 0,  "OFF=0,ALL=127"}
  ,{  "BRIGHTNESS",  "",             0,   100,  1, 1,  ""}      /* brightness [0 .. 100] */
  ,{  "RAWCMD",      "",             0,   255,  1, 1,  ""}      /* for development - internal use only */
};


/* internal typedefs and functions */

static void serdisp_l4m_init           (serdisp_t*);
static int  serdisp_l4m_setoption      (serdisp_t*, const char*, long);
static void serdisp_l4m_close          (serdisp_t*);

static void serdisp_l4m_update_l4me5i  (serdisp_t*);

static void serdisp_l4m_update_l4m132c (serdisp_t*);
static void serdisp_l4m_update_l4m132c_mono (serdisp_t*);
static void serdisp_l4m_clear_l4m132c  (serdisp_t*);


typedef struct serdisp_l4m132c_specific_s {
  int  postoffmode;
  int  resmode;
  long fgcol;
  long bgcol;
  byte alarmhour, alarmminute, alarmdays;
} serdisp_l4m132c_specific_t;


static serdisp_l4m132c_specific_t* serdisp_l4m132c_internal_getStruct(serdisp_t* dd) {
  return (serdisp_l4m132c_specific_t*)(dd->specific_data);
}


/* callback-function for setting non-standard options */
static void* serdisp_l4m132c_getvalueptr (serdisp_t* dd, const char* optionname, int* typesize) {
  if (serdisp_compareoptionnames(dd, optionname, "POSTOFFMODE")) {
    *typesize = sizeof(int);
    return &(serdisp_l4m132c_internal_getStruct(dd)->postoffmode);
  } else if (serdisp_compareoptionnames(dd, optionname, "RESMODE")) {
    *typesize = sizeof(int);
    return &(serdisp_l4m132c_internal_getStruct(dd)->resmode);
  } else if (serdisp_compareoptionnames(dd, optionname, "FGCOLOUR")) {
    *typesize = sizeof(long);
    return &(serdisp_l4m132c_internal_getStruct(dd)->fgcol);
  } else if (serdisp_compareoptionnames(dd, optionname, "BGCOLOUR")) {
    *typesize = sizeof(long);
    return &(serdisp_l4m132c_internal_getStruct(dd)->bgcol);
  } else if (serdisp_compareoptionnames(dd, optionname, "DEPTH")) {
    *typesize = sizeof(byte);
    return &(dd->depth);
  } else if (serdisp_compareoptionnames(dd, optionname, "ALARMHOUR")) {
    *typesize = sizeof(byte);
    return &(serdisp_l4m132c_internal_getStruct(dd)->alarmhour);
  } else if (serdisp_compareoptionnames(dd, optionname, "ALARMMINUTE")) {
    *typesize = sizeof(byte);
    return &(serdisp_l4m132c_internal_getStruct(dd)->alarmminute);
  } else if (serdisp_compareoptionnames(dd, optionname, "ALARMDAYS")) {
    *typesize = sizeof(byte);
    return &(serdisp_l4m132c_internal_getStruct(dd)->alarmdays);
  }
  return 0;
}


/* main functions */


/* *********************************
   serdisp_t* serdisp_l4m_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle (not used in here)
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_l4m_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate display descriptor", __func__);
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));

  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("L4ME5I", dispname))
    dd->dsp_id = DISPID_L4ME5I;
  else if (serdisp_comparedispnames("L4M132C", dispname))
    dd->dsp_id = DISPID_L4M132C;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_l4m.c", dispname);
    return (serdisp_t*)0;
  }

  /* specific data for L4M132C-modules */
  if (dd->dsp_id == DISPID_L4M132C) {
    if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_l4m132c_specific_t)) )) {
      serdisp_freeresources(dd);
      return (serdisp_t*)0;
    }

    memset(dd->specific_data, 0, sizeof(serdisp_l4m132c_specific_t));
  }


  /* default setup for function pointers */
  dd->fp_init           = &serdisp_l4m_init;
  dd->fp_update         = &serdisp_l4m_update_l4me5i;
  dd->fp_close          = &serdisp_l4m_close;
  dd->fp_setoption      = &serdisp_l4m_setoption;


  /* per display settings */

  dd->width             = 128;
  dd->height            = 64;
  dd->depth             = 1;
  dd->feature_contrast  = 0;
  dd->feature_backlight = 0;
  dd->feature_invert    = 1;

  dd->dsparea_width     = 58840;     /* active viewing area in micrometres              */
  dd->dsparea_height    = 35150;     /*   according to datasheet of Powertip PG12864WRF */

  /*
  dd->min_contrast      = 0x1;
  dd->max_contrast      = 0xA;
  */

  if (dd->dsp_id == DISPID_L4M132C) {
    dd->width             = 132;
    dd->height            = 65;
    dd->depth             = 16;

    dd->feature_contrast  = 1;
    dd->min_contrast      = 0x02;
    dd->max_contrast      = 0x9F;
    dd->mid_contrast      = 0x35;

    dd->dsparea_width     = 60000;     /* active viewing area in micrometres  */
    dd->dsparea_height    = 31000;     /*   measured */

    /* supported colour space */
    dd->colour_spaces     = SD_CS_RGB565;

    serdisp_l4m132c_internal_getStruct(dd)->postoffmode = 0;  /* no clock after power off */
    serdisp_l4m132c_internal_getStruct(dd)->resmode = 0;  /* default mode: 132x65 */
    serdisp_l4m132c_internal_getStruct(dd)->fgcol = 0xFFFFFFL;
    serdisp_l4m132c_internal_getStruct(dd)->bgcol = 0;

    dd->fp_update         = &serdisp_l4m_update_l4m132c;
    dd->fp_clear          = &serdisp_l4m_clear_l4m132c;

    dd->fp_getvalueptr    = &serdisp_l4m132c_getvalueptr;
  }

  /* max. delta for optimised update algorithm */
  dd->optalgo_maxdelta    = MAXCHUNK;

  /* finally set some non display specific defaults */

  dd->curr_rotate         = 0;         /* unrotated display */
  dd->curr_invert         = 0;         /* display not inverted */

  /* supported output devices */
  dd->connection_types  = SERDISPCONNTYPE_PARPORT | SERDISPCONNTYPE_HIDDEV;

  if (dd->dsp_id == DISPID_L4ME5I) {
    serdisp_setupstructinfos(dd, 0, 0, serdisp_l4m_options);
  } else {
    serdisp_setupstructinfos(dd, 0, 0, serdisp_l4m132c_options);
  }

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    serdisp_freeresources(dd);
    return (serdisp_t*)0;
  }

  if (dd->dsp_id == DISPID_L4M132C && serdisp_l4m132c_internal_getStruct(dd)->resmode == 1) {
    dd->width             = 128;
    dd->height            = 64;
  }

  if (dd->dsp_id == DISPID_L4M132C && dd->depth == 1) {
    dd->colour_spaces     = SD_CS_SCRBUFCUSTOM | SD_CS_GREYSCALE;
    dd->fp_update         = &serdisp_l4m_update_l4m132c_mono;
#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
    dd->fp_setsdpixel     = &sdtools_generic_setsdpixel_greyhoriz;
    dd->fp_getsdpixel     = &sdtools_generic_getsdpixel_greyhoriz;
#else
    dd->fp_setpixel       = &sdtools_generic_setpixel_greyhoriz;
    dd->fp_getpixel       = &sdtools_generic_getpixel_greyhoriz;
#endif
    dd->scrbuf_size = sizeof(byte) * (  ((dd->width + 7) / 8) * dd->height); 
    dd->scrbuf_chg_size = sizeof(byte) * (  (((dd->width + 7) / 8 + 7) / 8) * dd->height); 
  }

  return dd;
}



/* *********************************
   void serdisp_l4m_init(dd)
   *********************************
   initialise a linux4media usb-based display
   *********************************
   dd     ... display descriptor
*/
void serdisp_l4m_init(serdisp_t* dd) {
  SDCONN_write(dd->sdcd, 0x10000 | 0x01, 0);  /* init lcd */
  if (dd->dsp_id == DISPID_L4M132C) {
    SDCONN_write(dd->sdcd, 0x10000 | 0x54, 0); /* disable clock */
  }

  if (dd->dsp_id == DISPID_L4M132C && serdisp_l4m132c_internal_getStruct(dd)->resmode == 0) {
    SDCONN_write(dd->sdcd, 0x1F, 0);  /* set resolution (ignored by older firmware versions) */
    SDCONN_write(dd->sdcd, 0, 0);     /* 1: 128x64 (default), 0: 132x65 */
    SDCONN_commit(dd->sdcd);
  }

  if ( ! dd->fp_clear) {
    /* clear lcd.  with L4ME5I command 0x02 does not work correctly ->
       send flag to tell write-functions to fix this */
    SDCONN_write(dd->sdcd, ((dd->dsp_id == DISPID_L4ME5I) ? 0x020000 : 0) |  0x010000 | 0x02, 0);
  }

  /* send dummy clear to ensure customised colour settings for depth == 1 */
  if (dd->dsp_id == DISPID_L4M132C && dd->depth == 1) {
    serdisp_clear(dd);
  }

  SDCONN_write(dd->sdcd, 0x10000 | 0x05, 0);  /* set normal display */
  SDCONN_write(dd->sdcd, 0x10000 | 0x06, 0);  /* autoadjust polarity (== bind pol. to empty screen) */

  SDCONN_commit(dd->sdcd);

  sd_debug(2, "%s(): done with initialising", __func__);
}



/* *********************************
   void serdisp_l4m_update_l5me5i(dd)
   *********************************
   updates the display using display-buffer scrbuf+scrbuf_chg
   *********************************
   dd     ... display descriptor
   *********************************

   the display is redrawn using a time-saving algorithm:
*/
void serdisp_l4m_update_l4me5i(serdisp_t* dd) {
  int x;
  int page;
  int pages = (dd->height+7)/8;

#ifdef OPT_USEOLDUPDATEALGO
  for (page = 0; page < pages; page++) {
    for(x = 0; x < dd->width; x++) {
      if ((x % MAXCHUNK) == 0) {
        int chunk = MAXCHUNK;
        if (x > dd->width - chunk)
          chunk = dd->width - ((dd->width / MAXCHUNK) * MAXCHUNK);
        SDCONN_commit(dd->sdcd);
        SDCONN_write(dd->sdcd, 0x12, 0);
        SDCONN_write(dd->sdcd, page, 0);
        SDCONN_write(dd->sdcd, x,    0);
        SDCONN_write(dd->sdcd, chunk, 0);
      }
      SDCONN_write(dd->sdcd, dd->scrbuf[ dd->width * page  +  x], 0);
    }
  }
  SDCONN_commit(dd->sdcd);

#else /* OPT_USEOLDUPDATEALGO */

  /* display is drawn using an optimising algorithm which tries to only send as few data as possible to the display */

  int col, col_start;
  byte data;

  col_start = 0;  /* start column (if previous page has already set some columns in current page) */

  for (page = 0; page < pages; page++) {

    col = col_start;

    while (col < dd->width) {
      if ( dd->scrbuf_chg[ col + dd->width *(page/8)] & ( 1 << (page%8)) ) {
        int endm = dd->optalgo_maxdelta;

        if (page == 7 && col > (dd->width - dd->optalgo_maxdelta) )
          endm = dd->width - col;

        SDCONN_commit(dd->sdcd);
        SDCONN_write(dd->sdcd, 0x12, 0);
        SDCONN_write(dd->sdcd, page, 0);
        SDCONN_write(dd->sdcd, col, 0);
        SDCONN_write(dd->sdcd, endm, 0);

        for (x = col ; x < col + endm; x++) {
          int x_i = x;
          int page_i = page;

          if (x >= dd->width) {
            x_i = x - dd->width;
            page_i = page + 1;
          }

          data = dd->scrbuf[x_i + dd->width * page_i ];

          if (dd->curr_invert && !(dd->feature_invert))
            data = ~data;

          SDCONN_write(dd->sdcd, data, 0);
          dd->scrbuf_chg[ x_i + dd->width * (page_i/8)] &= 0xFF - (1 << (page_i%8)) ;
        }
        SDCONN_commit(dd->sdcd);

        col += endm;

        col_start = (col >= dd->width) ? (col - dd->width) : 0;
      } else {
        col++;
      }

    }
  }
#endif /* OPT_USEOLDUPDATEALGO */

  /* add an extra NOP to avoid erraneous pixels when releasing parport */
  SDCONN_commit(dd->sdcd); /* if streaming: be sure that every data is transmitted */
}




/* *********************************
   void serdisp_l4m_update_l4m132c(dd)
   *********************************
   updates the display using display-buffer scrbuf+scrbuf_chg (colour display module)
   *********************************
   dd     ... display descriptor
   *********************************

   the display is redrawn using a time-saving algorithm:
*/
void serdisp_l4m_update_l4m132c(serdisp_t* dd) {
  int x;
  int y;
#ifdef OPT_COLMODE2BYTE
  int pixelsize = 2;
#else
  int pixelsize = 3;
#endif

  int p1, p2;

#ifdef OPT_USEOLDUPDATEALGO
  int maxchunk = MAXCHUNK / pixelsize;

  for (y = 0; y < dd->height; y++) {
    for(x = 0; x < dd->width; x++) {
      if ((x % maxchunk) == 0) {
        int chunk = maxchunk;
        if (x > dd->width - chunk)
          chunk = dd->width - ((dd->width / maxchunk) * maxchunk);
        SDCONN_commit(dd->sdcd);
#ifdef OPT_COLMODE2BYTE
        SDCONN_write(dd->sdcd, 0x17, 0);
#else
        SDCONN_write(dd->sdcd, 0x16, 0);
#endif
        SDCONN_write(dd->sdcd, y, 0);
        SDCONN_write(dd->sdcd, x,    0);
        SDCONN_write(dd->sdcd, chunk * pixelsize, 0);
      }
      p1 = dd->scrbuf[y * dd->width*2 + x*2 + 0];
      p2 = dd->scrbuf[y * dd->width*2 + x*2 + 1];
#ifdef OPT_COLMODE2BYTE
      SDCONN_write(dd->sdcd, (p1 & 0x07) | ((p2 & 0x1F) << 3), 0);
      SDCONN_write(dd->sdcd, (p2 & 0xE0) | ((p1 & 0xF8) >> 3), 0);
#else
      SDCONN_write(dd->sdcd, p2 & 0x1F, 0);  /* blue */
      SDCONN_write(dd->sdcd, ((p1 & 0x07) << 3) | ((p2 & 0xE0) >> 5), 0);  /* green */
      SDCONN_write(dd->sdcd, (p1 & 0xF8) >> 3, 0);  /* red */
#endif
    }
  }
  SDCONN_commit(dd->sdcd);

#else /* OPT_USEOLDUPDATEALGO */

  /* display is drawn using an optimising algorithm which tries to only send as few data as possible to the display */

  int col;

  for (y = 0; y < dd->height; y++) {

    col = 0;

    while (col < dd->width) {
      if ( dd->scrbuf_chg[(col >> 3) + y * ((dd->width + 7 ) >> 3)] & ( 1 << (col%8) )) {
        int endm = dd->optalgo_maxdelta / pixelsize;

        if (col > (dd->width - (dd->optalgo_maxdelta / pixelsize)) )
          endm = dd->width - col;

        SDCONN_commit(dd->sdcd);
#ifdef OPT_COLMODE2BYTE
        SDCONN_write(dd->sdcd, 0x17, 0);
#else
        SDCONN_write(dd->sdcd, 0x16, 0);
#endif
        SDCONN_write(dd->sdcd, y, 0);
        SDCONN_write(dd->sdcd, col, 0);
        SDCONN_write(dd->sdcd, endm * pixelsize, 0);

        for (x = col ; x < col + endm; x++) {
          int x_i = x;
          int y_i = y;

          p1 = dd->scrbuf[y_i * dd->width*2 + x_i*2 + 0];
          p2 = dd->scrbuf[y_i * dd->width*2 + x_i*2 + 1];
#ifdef OPT_COLMODE2BYTE
          SDCONN_write(dd->sdcd, (p1 & 0x07) | ((p2 & 0x1F) << 3), 0);
          SDCONN_write(dd->sdcd, (p2 & 0xE0) | ((p1 & 0xF8) >> 3), 0);
#else
          SDCONN_write(dd->sdcd, p2 & 0x1F, 0);  /* blue */
          SDCONN_write(dd->sdcd, ((p1 & 0x07) << 3) | ((p2 & 0xE0) >> 5), 0);  /* green */
          SDCONN_write(dd->sdcd, (p1 & 0xF8) >> 3, 0);  /* red */
#endif
          dd->scrbuf_chg[ (x_i >> 3) + y_i * ((dd->width + 7 ) >> 3)] &=  (0xFF ^ (1 << (x_i % 8)));
        }
        SDCONN_commit(dd->sdcd);

        col += endm;
      } else {
        col++;
      }

    }
  }
#endif /* OPT_USEOLDUPDATEALGO */

  SDCONN_commit(dd->sdcd); /* if streaming: be sure that every data is transmitted */
}



/* *********************************
   void serdisp_l4m_update_l4m132c_mono(dd)
   *********************************
   updates the display using display-buffer scrbuf+scrbuf_chg
   *********************************
   dd     ... display descriptor
   *********************************

   the display is redrawn using a time-saving algorithm:
*/
void serdisp_l4m_update_l4m132c_mono (serdisp_t* dd) {
  int x;
  int y;
  byte data;
  int cols = (dd->width + 7) >> 3;  /* data columns per line */

#ifdef OPT_USEOLDUPDATEALGO
  if (serdisp_l4m132c_internal_getStruct(dd)->resmode == 0) {  /* 132x65 */
    /* FIX for last data byte bug:

       problem with very last possible data byte of last line:
       unused bits will be written into first pixels of last line.
       workaround: mirror first pixel bits into these unused bits of last data byte */
    /* (h - 1) * cols + (cols - 1) == h * cols - 1 */

    for (y = 0; y < dd->height-1; y++) {
      dd->scrbuf[ (y + 1) * cols - 1] |= ((dd->scrbuf[ (y + 1) * cols] & 0xF0) >> 4);
    }

    dd->scrbuf[ dd->height * cols - 1] |= (dd->scrbuf[ (dd->height - 1) * cols] & 0xF0) >> 4;
  }

  for (y = 0; y < dd->height; y++) {
    SDCONN_write(dd->sdcd, 0x12, 0);
    SDCONN_write(dd->sdcd, y, 0);
    SDCONN_write(dd->sdcd, 0, 0);
    SDCONN_write(dd->sdcd, cols, 0);

    for (x = 0; x < cols; x++) {
      data = dd->scrbuf[y*cols + x];
      SDCONN_write(dd->sdcd, data, 0);
    }

    SDCONN_commit(dd->sdcd);
  }

#else /* OPT_USEOLDUPDATEALGO */

  /* display is drawn using an optimising algorithm which tries to only send as few data as possible to the display */

  int rc;
  int xt = 0, yt = 0, xb = 0, yb = 0;
  int chunk = 0;
  int wraparound = 0;

  rc = sdtools_calc_bbox (dd, 1, &xt, &yt, &xb, &yb);
  /*fprintf(stderr, "[%d] %3d/%3d - %3d/%3d\n", rc, xt, yt, xb, yb);*/

  if (rc != 0) {
    chunk = (xb - xt + 1 + 7) / 8;
    wraparound = ((xb - xt + 1) % 8);

    for (y = yt; y <= yb; y++) {

      SDCONN_write(dd->sdcd, 0x12, 0);
      SDCONN_write(dd->sdcd, y, 0);
      SDCONN_write(dd->sdcd, xt, 0);
      SDCONN_write(dd->sdcd, chunk, 0);

      for (x = xt; x <= xb; x+=8) {
        if (wraparound && x >= 128) {
          int y_i = (y < dd->height-1) ? y+1 : y;   /* avoid reading out of bounds */
          data = (dd->scrbuf[y*cols + (x/8)] & 0xF0) | ((dd->scrbuf[y_i*cols + (xt/8)] & 0xF0) >> 4) ;
        } else {
          data = dd->scrbuf[y*cols + (x/8)];
        }
        SDCONN_write(dd->sdcd, data, 0);
      }

      SDCONN_commit(dd->sdcd);
    }
    memset(dd->scrbuf_chg, 0x00, dd->scrbuf_chg_size);
  }

#endif /* OPT_USEOLDUPDATEALGO */

  /* add an extra NOP to avoid erraneous pixels when releasing parport */
  SDCONN_commit(dd->sdcd); /* if streaming: be sure that every data is transmitted */
}





/* *********************************
   int serdisp_l4m_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_l4m_setoption(serdisp_t* dd, const char* option, long value) {

  if (dd->feature_invert && serdisp_compareoptionnames(dd, option, "INVERT") ) {
    if (value < 2) 
      dd->curr_invert = (int)value;
    else
      dd->curr_invert = (dd->curr_invert) ? 0 : 1;
    /* 0x04: invers mode;  0x05: normal mode */
    SDCONN_write(dd->sdcd, 0x10000 | ((dd->curr_invert) ? 0x04 : 0x05), 0);
  } else if (dd->feature_contrast && 
             (serdisp_compareoptionnames(dd, option, "CONTRAST" ) ||
              serdisp_compareoptionnames(dd, option, "BRIGHTNESS" )
             )
            ) {
    int dimmed_contrast;

    if ( serdisp_compareoptionnames(dd, option, "CONTRAST" ) ) {
      dd->curr_contrast = sdtools_contrast_norm2hw(dd, (int)value);
    } else {
      dd->curr_dimming = 100 - (int)value;
    }

    dimmed_contrast = (((dd->curr_contrast - dd->min_contrast) * (100 - dd->curr_dimming)) / 100) + dd->min_contrast;

    /* workaround to be able to disable backlight or set values < dd->min_contrast */
    if (dd->curr_dimming >= (100 - dd->min_contrast)) {
      dimmed_contrast -= (dd->min_contrast - (100 - dd->curr_dimming));
    }

    SDCONN_write(dd->sdcd, 0x15, 0);
    SDCONN_write(dd->sdcd, dimmed_contrast /*dd->curr_contrast*/, 0);
    SDCONN_commit(dd->sdcd);
  } else if (serdisp_compareoptionnames(dd, option, "RAWCMD")) {
    fprintf(stderr, "val: 0x%02x\n", (byte)(0xFF & value));
    SDCONN_write(dd->sdcd, 0x10000 | ((byte)(0xFF & value)), 0);
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  return 1;
}

/* *********************************
   void serdisp_l4m_clear_l4m132c(dd)
   *********************************
   clear display (colour display module)
   *********************************
   dd     ... display descriptor
*/
void serdisp_l4m_clear_l4m132c(serdisp_t* dd) {
  /* set background colour to white (0x02 (clear display) will use this colour) */
  SDCONN_write(dd->sdcd, 0x14, 0);
  SDCONN_write(dd->sdcd, 0xFF, 0);
  SDCONN_write(dd->sdcd, 0xFF, 0);
  SDCONN_write(dd->sdcd, 0xFF, 0);
  SDCONN_commit(dd->sdcd);

  /* clear display using internal function 0x02 */
  SDCONN_write(dd->sdcd, 0x010000 | 0x02, 0);

  if (dd->dsp_id == DISPID_L4M132C && dd->depth == 1) {
    long fgcol = serdisp_l4m132c_internal_getStruct(dd)->fgcol;
    long bgcol = serdisp_l4m132c_internal_getStruct(dd)->bgcol;

    SDCONN_write(dd->sdcd, 0x13, 0);
    SDCONN_write(dd->sdcd, (byte)((fgcol & 0xFF0000L) >> 16), 0);
    SDCONN_write(dd->sdcd, (byte)((fgcol & 0x00FF00L) >> 8), 0);
    SDCONN_write(dd->sdcd, (byte)(fgcol & 0x0000FFL), 0);
    SDCONN_commit(dd->sdcd);

    SDCONN_write(dd->sdcd, 0x14, 0);
    SDCONN_write(dd->sdcd, (byte)((bgcol & 0xFF0000L) >> 16), 0);
    SDCONN_write(dd->sdcd, (byte)((bgcol & 0x00FF00L) >> 8), 0);
    SDCONN_write(dd->sdcd, (byte)(bgcol & 0x0000FFL), 0);
    SDCONN_commit(dd->sdcd);
  }

  /* restore invert-modus */
  SDCONN_write(dd->sdcd, 0x010000 | ((dd->curr_invert) ? 0x04 : 0x05), 0);
}


/* *********************************
   void serdisp_l4m_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_l4m_close(serdisp_t* dd) {
  serdisp_clear(dd);
  SDCONN_write(dd->sdcd, 0x10000 | 0x05, 0); /* force normal mode */

  if (dd->dsp_id == DISPID_L4M132C) {
    /* if alarm is set: force postoffmode */
    if (serdisp_l4m132c_internal_getStruct(dd)->postoffmode == 0 &&
        serdisp_l4m132c_internal_getStruct(dd)->alarmdays != 0) {
      serdisp_l4m132c_internal_getStruct(dd)->postoffmode = 1;
    }

    switch (serdisp_l4m132c_internal_getStruct(dd)->postoffmode) {
      case 1: {   /* enable clock after closing display */
        time_t t;
        struct tm *tm;

        t = time( ((time_t *)0) );
        tm = localtime(&t);

        SDCONN_write(dd->sdcd, 0x51, 0);
        SDCONN_write(dd->sdcd, sdtools_dec2bcd(tm->tm_hour), 0);
        SDCONN_write(dd->sdcd, sdtools_dec2bcd(tm->tm_min), 0);
        SDCONN_write(dd->sdcd, sdtools_dec2bcd(tm->tm_sec), 0);
        SDCONN_commit(dd->sdcd);

        SDCONN_write(dd->sdcd, 0x52, 0);
        SDCONN_write(dd->sdcd, sdtools_dec2bcd(tm->tm_mday), 0);
        SDCONN_write(dd->sdcd, sdtools_dec2bcd(tm->tm_mon + 1), 0);     /* tm_mon in [0, 11] !! */
        SDCONN_write(dd->sdcd, sdtools_dec2bcd(tm->tm_year % 100), 0);  /* tm_year = years since 1900 */
        SDCONN_commit(dd->sdcd);

        SDCONN_write(dd->sdcd, 0x53, 0); /* set time */
        SDCONN_commit(dd->sdcd);
      };
      break;
      default: {  /* clear display using bgcolour = black */
        /* set background colour to black (0x02 (clear display) will use this colour) */
        SDCONN_write(dd->sdcd, 0x14, 0);
        SDCONN_write(dd->sdcd, 0x00, 0);
        SDCONN_write(dd->sdcd, 0x00, 0);
        SDCONN_write(dd->sdcd, 0x00, 0);
        SDCONN_commit(dd->sdcd);
        /* clear display using internal function 0x02 */
        SDCONN_write(dd->sdcd, 0x010000 | 0x02, 0);
      }
    }

    /* enable / disable alarm time */
    SDCONN_write(dd->sdcd, 0x56, 0);
    if (serdisp_l4m132c_internal_getStruct(dd)->alarmdays) {
      SDCONN_write(dd->sdcd, sdtools_dec2bcd(serdisp_l4m132c_internal_getStruct(dd)->alarmminute), 0);
      SDCONN_write(dd->sdcd, sdtools_dec2bcd(serdisp_l4m132c_internal_getStruct(dd)->alarmhour), 0);
      SDCONN_write(dd->sdcd, serdisp_l4m132c_internal_getStruct(dd)->alarmdays, 0);
    } else {
      SDCONN_write(dd->sdcd, 0, 0);
      SDCONN_write(dd->sdcd, 0, 0);
      SDCONN_write(dd->sdcd, 0x00, 0);
    }
    SDCONN_commit(dd->sdcd);
  }
}
