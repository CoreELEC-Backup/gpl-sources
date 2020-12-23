/*
 *************************************************************************
 *
 * serdisp_specific_goldelox.c
 * routines for controlling 4D Systems uOLED and uLED displays controlled 
 * by the GOLDELOX-MD1 module
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

#include "serdisplib/serdisp_connect.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_colour.h"


/* #define OPT_USEOLDUPDATEALGO */

/*
 * constants
 */

/* different display types/models supported by this driver */
#define DISPID_GOLDELOX   1


serdisp_options_t serdisp_goldelox_options[] = {
   /*  name          aliasnames    min    max mod int defines  */
   {  "DEPTH",       "COLS,COLOURS", 8,    16,  8, 0,  "65536=16,64k=16,256=8"}
  ,{  "CONTRAST",    "",             0,    10,  1, 1,  ""}
  ,{  "BACKLIGHT",   "",             0,     1,  1, 1,  ""}
  ,{  "BRIGHTNESS", "",              0,   100,  1, 1,  ""}     /* brightness [0 .. 100] */
};


/* internal typedefs and functions */

static void serdisp_goldelox_init      (serdisp_t*);
static void serdisp_goldelox_update    (serdisp_t*);
static int  serdisp_goldelox_setoption (serdisp_t*, const char*, long);
static void serdisp_goldelox_clear     (serdisp_t*);
static void serdisp_goldelox_close     (serdisp_t*);

static int  serdisp_goldelox_is_oled   (byte);
static int  serdisp_goldelox_translate_rescode (byte code);
static int  read_ACK                   (serdisp_t*);
static void write_byte                 (serdisp_t*, byte);
static void do_commit                  (serdisp_t*);


typedef struct serdisp_goldelox_specific_s {
  int is_oled;
} serdisp_goldelox_specific_t;


static serdisp_goldelox_specific_t* serdisp_goldelox_internal_getStruct(serdisp_t* dd) {
  return (serdisp_goldelox_specific_t*)(dd->specific_data);
}




int serdisp_goldelox_is_oled(byte code) {
  return (code == 0x00) ? 1: 0;
}


int serdisp_goldelox_translate_rescode(byte code) {
  switch(code) {
    case 0x22: return 220;
    case 0x28: return 128;
    case 0x32: return 320;
    case 0x60: return 160;
    case 0x64: return  64;
    case 0x76: return 176;
    case 0x96: return  96;

    default:   return 0;
  }
}


int read_ACK(serdisp_t* dd) {
  byte retval;

  SDCONN_commit(dd->sdcd);
  retval = (byte) SDCONN_read(dd->sdcd, 0);

  if (retval != 0x06) {  /* received NACK  (0x06 == ACK) */
    sd_error(SERDISP_ERUNTIME, "%s(): received NACK", __func__);
    sd_runtimeerror = 1;
    return 0;
  } else {  /* ACK ok */
    return 1;
  }
}



void write_byte (serdisp_t* dd, byte item) {
  SDCONN_writedelay(dd->sdcd, item, 0, dd->delay);
}


void do_commit (serdisp_t* dd) {
  SDCONN_commit(dd->sdcd);
}


/* callback-function for setting non-standard options */
static void* serdisp_goldelox_getvalueptr (serdisp_t* dd, const char* optionname, int* typesize) {
  if (serdisp_compareoptionnames(dd, optionname, "DEPTH")) {
    *typesize = sizeof(byte);
    return &(dd->depth);
  }
  return 0;
}





/* main functions */


/* *********************************
   serdisp_t* serdisp_goldelox_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_goldelox_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;
  byte infobuffer[5];


  if (! sdcd ) {
    sd_error(SERDISP_EMALLOC, "%s(): output device not open", __func__);
    return (serdisp_t*)0;
  }

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate display descriptor", __func__);
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));


  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_goldelox_specific_t)) )) {
    free(dd);
    return (serdisp_t*)0;
  }
  memset(dd->specific_data, 0, sizeof(serdisp_goldelox_specific_t));

  /* 4D systems display-modules (driven by GOLDELOX-MD1 module) supported in here */
  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("GOLDELOX", dispname))
    dd->dsp_id = DISPID_GOLDELOX;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_goldelox.c", dispname);
    return (serdisp_t*)0;
  }

  dd->sdcd = (serdisp_CONN_t*)sdcd;

  /* supported output devices */
  dd->connection_types  = SERDISPCONNTYPE_RS232;

  /* force device-check at this point because _setup already does some communication with the display */
  if (! (dd->connection_types & dd->sdcd->conntype)) {
    sd_error(SERDISP_EDEVNOTSUP, "'%s' only supports 'RS232' as connection type (try using 'RS232:<device>')", dispname);
    free(dd->specific_data);
    free(dd);
    return (serdisp_t*)0;
  }

  /* set RS232 parameters (will be set / initialised by SDCONN_confinit() */
  dd->sdcd->rs232.baudrate = B230400;

  write_byte (dd, 0x55);  /* baud autodetect */
  read_ACK   (dd);

  write_byte (dd, 0x56);  /* request display information */
  write_byte (dd, 0x00);  /* param: 0x00 */
  do_commit  (dd);
  SDCONN_readstream(dd->sdcd, infobuffer, 5);

  /* returns 5 values: 
     0: device type:   0x00: OLED, 0x01: LCD, 0x02: VGA
     1: hardware rev.
     2: firmware rev.
     3: horizontal resolution (encoded)
     4: vertical resolution (encoded)
   */

  serdisp_goldelox_internal_getStruct(dd)->is_oled = serdisp_goldelox_is_oled(infobuffer[0]);

  dd->feature_contrast  = 1;
  dd->feature_backlight = (serdisp_goldelox_internal_getStruct(dd)->is_oled) ? 0 : 1;
  dd->feature_invert    = 0;


  /* per display settings */

  dd->width             = serdisp_goldelox_translate_rescode(infobuffer[3]);
  dd->height            = serdisp_goldelox_translate_rescode(infobuffer[4]);
  dd->depth             = 16;
  dd->min_contrast      = 1 /*0x0*/;
  dd->max_contrast      = 9 /*0x15*/;


  /* max. delta for optimised update algorithm */
  dd->optalgo_maxdelta  = 6;

  dd->delay = 0;

  /* finally set some non display specific defaults */

  dd->curr_rotate       = 0;         /* unrotated display */
  dd->curr_invert       = 0;         /* display not inverted */

  dd->curr_backlight    = 1;         /* start with backlight on */

  /* supported colour spaces */
  dd->colour_spaces     = SD_CS_RGB332 | SD_CS_RGB565;
  if(serdisp_goldelox_internal_getStruct(dd)->is_oled)
    dd->colour_spaces |= SD_CS_SELFEMITTING;

  dd->fp_init           = &serdisp_goldelox_init;
  dd->fp_update         = &serdisp_goldelox_update;
  dd->fp_clear          = &serdisp_goldelox_clear;
  dd->fp_close          = &serdisp_goldelox_close;
  dd->fp_setoption      = &serdisp_goldelox_setoption;
  dd->fp_getvalueptr    = &serdisp_goldelox_getvalueptr;

  serdisp_setupstructinfos(dd, 0, 0, serdisp_goldelox_options);

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    serdisp_freeresources(dd);
    dd = 0;
    return (serdisp_t*)0;    
  }

  sd_debug(2, "%s(): detected display information: w/h: %d/%d   is_oled: %d", 
           __func__, dd->width, dd->height, serdisp_goldelox_internal_getStruct(dd)->is_oled
  );

  sd_debug(2, "%s(): colour depth: %d", __func__, dd->depth);

  return dd;
}



/* *********************************
   void serdisp_goldelox_init(dd)
   *********************************
   initialise a GOLDELOX-MD1-based display
   *********************************
   dd     ... display descriptor
*/
void serdisp_goldelox_init(serdisp_t* dd) {
  /* adapt maxdelta for optimising algo if depth != 8 */
  dd->optalgo_maxdelta = (dd->optalgo_maxdelta * dd->depth) / 8;

  sd_debug(2, "%s(): done with initialising", __func__);
}



/* *********************************
   void serdisp_goldelox_update(dd)
   *********************************
   updates the display using display-buffer scrbuf+scrbuf_chg
   *********************************
   dd     ... display descriptor
   *********************************
   the display is redrawn using a time-saving algorithm
*/
void serdisp_goldelox_update(serdisp_t* dd) {
  int i;
  byte data = 0;

#ifdef OPT_USEOLDUPDATEALGO
  /* unoptimised display update (slow. all pixels are redrawn) */

  write_byte (dd, 0x49      );  /* display image */
  write_byte (dd, 0         );  /*   start x */
  write_byte (dd, 0         );  /*   start y */
  write_byte (dd, dd->width );  /*   width */
  write_byte (dd, dd->height);  /*   height */
  write_byte (dd, dd->depth );  /*   colour mode */

  for (i = 0; i < dd->scrbuf_size; i++) {
    data = dd->scrbuf[i];

    if (dd->curr_invert && !(dd->feature_invert))
      data = ~data;

    write_byte (dd, data);
  }

  read_ACK(dd);

#else /* OPT_USEOLDUPDATEALGO */

  /* display is drawn using an optimising algorithm which tries to only send as few data as possible to the display */

  /* more detailed explanations of principle: 
     see serdisp_specific_optrex323.c / i2c.c / pcd8544.c / sed153x.c / sed1565.c 
  */

  int j;

  for (j = 0; j < dd->height; j++) {
    i = 0;
    while (i < dd->width) {
      if ( dd->scrbuf_chg[(i >> 3) + j * ((dd->width + 7 ) >> 3)] & ( 1 << (i%8) )) {
        int i_delta = 0;
        int ii;
        int i_lastchg;

        i_lastchg = i;

        while ( (i_lastchg + i_delta) < (dd->width - 1) && (i_delta < dd->optalgo_maxdelta)) {
          if ( dd->scrbuf_chg[ ( (i_lastchg+i_delta+1) >> 3) + j * ((dd->width + 7 ) >> 3)] & ( 1 << ( (i_lastchg+i_delta+1)%8) ) ) {
            i_lastchg += i_delta + 1;
            i_delta = 0;
          } else {
            i_delta ++;
          }
        }

        write_byte (dd, 0x49      );  /* display image */
        write_byte (dd, i         );  /*   start x */
        write_byte (dd, j         );  /*   start y */
        write_byte (dd, i_lastchg - i + 1);  /*   width */
        write_byte (dd, 1);  /*   height */
        write_byte (dd, dd->depth );  /*   colour mode */

        for (ii = i; ii <= i_lastchg; ii++) {
          int fact = (dd->depth == 8) ? 0 : 1;

          data = dd->scrbuf[ (ii << fact)  + ( (j * dd->width) << fact  ) ];

          if (dd->curr_invert && !(dd->feature_invert))
            data = ~data;

          write_byte (dd, data);

          if (dd->depth == 16) {
            data = dd->scrbuf[ (ii << fact) + 1  + ( (j * dd->width) << fact  ) ];

            if (dd->curr_invert && !(dd->feature_invert))
              data = ~data;

            write_byte (dd, data);
          }

          dd->scrbuf_chg[ (ii >> 3) + j * ((dd->width + 7 ) >> 3)] &=  (0xFF ^ (1 << (ii % 8)));
        }

        read_ACK(dd);

        i = i_lastchg +1;
      } else {
        i++;
      }
    }
  }
#endif /* OPT_USEOLDUPDATEALGO */

  do_commit(dd); /* if streaming: be sure that every data is transmitted */
}



/* *********************************
   int serdisp_goldelox_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_goldelox_setoption(serdisp_t* dd, const char* option, long value) {
  if (dd->feature_backlight && serdisp_compareoptionnames(dd, option, "BACKLIGHT")) {
    if (value < 2) 
      dd->curr_backlight = (int)value;
    else
      dd->curr_backlight = (dd->curr_backlight) ? 0 : 1;

    write_byte (dd, 0x59);  /* display control */
    write_byte (dd, 0x00);  /* mode:  backlight on/off */
    write_byte (dd, dd->curr_backlight);
    read_ACK   (dd);
  } else if (dd->feature_contrast && 
             (serdisp_compareoptionnames(dd, option, "CONTRAST" ) ||
              serdisp_compareoptionnames(dd, option, "BRIGHTNESS" )
             )
            )
  {
    int dimmed_contrast;

    if ( serdisp_compareoptionnames(dd, option, "CONTRAST" ) ) {
      dd->curr_contrast = sdtools_contrast_norm2hw(dd, (int)value);
    } else {
      dd->curr_dimming = 100 - (int)value;
    }

    dimmed_contrast = (((dd->curr_contrast - dd->min_contrast) * (100 - dd->curr_dimming)) / 100) + dd->min_contrast;

    write_byte (dd, 0x59);  /* display control */
    write_byte (dd, 0x02);  /* mode:  display contrast */
    write_byte (dd, dimmed_contrast /*dd->curr_contrast*/);
    read_ACK   (dd);
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  do_commit(dd); /* if streaming: be sure that every data is transmitted */
  return 1;
}


/* *********************************
   void serdisp_goldelox_clear(dd)
   *********************************
   clear display
   *********************************
   dd     ... display descriptor
*/
void serdisp_goldelox_clear(serdisp_t* dd) {
  write_byte (dd, 0x45);  /* erase display */
  read_ACK   (dd);
}


/* *********************************
   void serdisp_goldelox_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_goldelox_close(serdisp_t* dd) {
  write_byte (dd, 0x59);  /* display control */
  write_byte (dd, 0x01);  /* mode:  display on/off */
  write_byte (dd, 0x00);  /* value: display off */
  read_ACK   (dd);

  write_byte (dd, 0x59);  /* display control */
  write_byte (dd, 0x03);  /* mode:  power on/off */
  write_byte (dd, 0x00);  /* value: power off */
  read_ACK   (dd);
}

