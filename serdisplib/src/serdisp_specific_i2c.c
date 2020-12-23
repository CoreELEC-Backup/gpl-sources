/*
 *************************************************************************
 *
 * serdisp_specific_i2c.c
 * routines for controlling i2c-based displays (eg: found in some older ericsson)
 *
 *************************************************************************
 *
 * copyright (C) 2003-2010  wolfgang astleitner
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
/*#include "serdisplib/serdisp_specific_i2c.h"*/
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_colour.h"

/*#define OPT_NOOUTPUT*/
/*#define OPT_USEOLDUPDATEALGO*/


#define SIG_SDA         (dd->sdcd->signals[0])
#define SIG_SCLK        (dd->sdcd->signals[1])
#define SIG_RESET       (dd->sdcd->signals[2])
#define SIG_BACKLIGHT   (dd->sdcd->signals[3])


/* different display types/models supported by this driver */
#define DISPID_ERICSSONT2X  1
#define DISPID_ERICSSONR520 2
#if 0
#define DISPID_ERICSSONR320 3
#endif

serdisp_wiresignal_t serdisp_i2c_wiresignals[] = {
 /*  type   signame   actlow   cord  index */
   {SDCT_PP, "SDA",        0,   'D',    0 }
  ,{SDCT_PP, "SCLK",       0,   'C',    1 }
  ,{SDCT_PP, "RESET",      1,   'D',    2 }
  ,{SDCT_PP, "BACKLIGHT",  0,   'D',    3 }
};

serdisp_wiredef_t serdisp_i2c_wiredefs[] = {
   {  0, SDCT_PP, "Original", "SDA:D0,SCLK:D1", "Original wiring"}
  ,{  1, SDCT_PP, "Rifer", "SDA:D0,SCLK:INIT", "Rifer wiring"}
  ,{  2, SDCT_PP, "OrigReset", "SDA:D0,SCLK:D1,RESET:D5", "Original wiring with software reset"}
};


serdisp_options_t serdisp_i2c_options[] = {
   /*  name       aliasnames min  max mod flag defines  */
   {  "DELAY",     "",         0,  -1,  1,  1,  ""}
};


/* private functions */
static void   serdisp_i2c_init       (serdisp_t*);
static void   serdisp_i2c_update     (serdisp_t*);
static int    serdisp_i2c_setoption  (serdisp_t*, const char*, long);
static void   serdisp_i2c_close      (serdisp_t*);
static void   serdisp_i2c_transfer   (serdisp_t*, int, byte);

static void   serdisp_i2c_t2x_cmd_init  (serdisp_t* dd);
static void   serdisp_i2c_r520_cmd_init (serdisp_t* dd);


typedef struct serdisp_i2c_specific_s {
  byte address;
  byte address2;
  
  byte curr_address; 

  void (*fp_cmd_init)       (struct serdisp_s* dd);
  void (*fp_cmd_blank)      (struct serdisp_s* dd);
  void (*fp_cmd_normal)     (struct serdisp_s* dd);
  void (*fp_cmd_allon)      (struct serdisp_s* dd);
  void (*fp_cmd_invert)     (struct serdisp_s* dd);
  void (*fp_cmd_contrast)   (struct serdisp_s* dd, int contrast);
  void (*fp_cmd_setpos)     (struct serdisp_s* dd, int x, int page);
} serdisp_i2c_specific_t;


static serdisp_i2c_specific_t* serdisp_i2c_internal_getStruct(serdisp_t* dd) {
  return (serdisp_i2c_specific_t*)(dd->specific_data);
}


static void serdisp_i2c_internal_i2cSIGNALS(serdisp_t* dd, int sda, int sclk) {
  long td = 0;

  if (dd->feature_backlight && dd->curr_backlight)
    td |= SIG_BACKLIGHT;

  if (sda) td |= SIG_SDA;
  if (sclk) td |= SIG_SCLK;
  /* write signals to the output device */
  SDCONN_write(dd->sdcd, td, 0);
  /* delay some time after write operation */
  sdtools_nsleep(dd->delay);
}


static void serdisp_i2c_internal_i2cSTART(serdisp_t* dd) {
  /* start with SDA and SCLK set to high */
  serdisp_i2c_internal_i2cSIGNALS(dd, 1, 1);
  /* SDA -> low; SCLK stays high */
  serdisp_i2c_internal_i2cSIGNALS(dd, 0, 1);
  /* SDA -> low; SCLK -> low */
  serdisp_i2c_internal_i2cSIGNALS(dd, 0, 0);

  /* after i2c-start: send i2c-address */
  serdisp_i2c_transfer(dd, 0, (serdisp_i2c_internal_getStruct(dd))->curr_address);
}

static void serdisp_i2c_internal_i2cSTOP(serdisp_t* dd) {
  /* start with SDA set to low, and SCLK set to high */
  serdisp_i2c_internal_i2cSIGNALS(dd, 0, 1);
  /* SDA -> high; SCLK stays high */
  serdisp_i2c_internal_i2cSIGNALS(dd, 1, 1);
  /* SDA -> high; SCLK -> low */
  serdisp_i2c_internal_i2cSIGNALS(dd, 1, 0);
}

/* individual commands per controller */

/* t2x */
void serdisp_i2c_t2x_cmd_init(serdisp_t* dd) {
  serdisp_i2c_transfer(dd, 0, 0x9); /* == cmd_normal */
}


static void serdisp_i2c_t2x_cmd_blank(serdisp_t* dd) {
  serdisp_i2c_transfer(dd, 0, 0x3);
}


static void serdisp_i2c_t2x_cmd_normal(serdisp_t* dd) {
  serdisp_i2c_transfer(dd, 0, 0x9);
}

static void serdisp_i2c_t2x_cmd_allon(serdisp_t* dd) {
  serdisp_i2c_transfer(dd, 0, 0x2);
}

static void serdisp_i2c_t2x_cmd_setpos(serdisp_t* dd, int x, int page) {
  serdisp_i2c_transfer(dd, 0, 0x9 | (page<<5));
  serdisp_i2c_transfer(dd, 0, x);
}


/* ericsson r520 */
void serdisp_i2c_r520_cmd_init(serdisp_t* dd) {
  unsigned char r520_initseq [] = {  /* from z01's i2c traffic trace */
   0x30, 0x21, 0x31, 0x80, 0xAC, 0xD0, 0xFF, 0x8, 0x43, 0x9, 0x0, 0xA, 0x12, 0xB, 0x9, 0xC, 0x0, 0xD, 0x0, 0xF, 0x1
  };
  int i;

  for (i = 0; i < sizeof(r520_initseq) ; i ++)
    serdisp_i2c_transfer(dd, 0, r520_initseq[i]);
}


static void serdisp_i2c_r520_cmd_blank(serdisp_t* dd) {
  serdisp_i2c_transfer(dd, 0, 0x1);  /* found using trial&error */
}


static void serdisp_i2c_r520_cmd_normal(serdisp_t* dd) {
  serdisp_i2c_transfer(dd, 0, 0x2);  /* trial&error (and was also the 2nd line in z01's trace) */
}

static void serdisp_i2c_r520_cmd_invert(serdisp_t* dd) {
  serdisp_i2c_transfer(dd, 0, 0x3);  /* found using trial&error */
}

static void serdisp_i2c_r520_cmd_allon(serdisp_t* dd) {
/*  serdisp_i2c_transfer(dd, 0, 0x2);*/
}

static void serdisp_i2c_r520_cmd_setpos(serdisp_t* dd, int x, int page) {
  serdisp_i2c_transfer(dd, 0, 0x42);  /* extracted from z01's trace */
  serdisp_i2c_transfer(dd, 0, page);
  serdisp_i2c_transfer(dd, 0, x);
}



#if 0
/* ericsson r320 */
void serdisp_i2c_r320_cmd_init(serdisp_t* dd) {
  /* see example at pcd8548 datasheet, p26 */
  serdisp_i2c_transfer(dd, 0, 0x00);  /* control byte with cleared Co bit and D/C set to logical 0 */
  serdisp_i2c_transfer(dd, 0, 0x21);  /* select extended instruction set (H=1) */
  serdisp_i2c_transfer(dd, 0, 0x15);  /* bias */
  serdisp_i2c_transfer(dd, 0, 0xea);  /* Vop */
  serdisp_i2c_transfer(dd, 0, 0x20);  /* select normal instruction set (H=0) */
  serdisp_i2c_transfer(dd, 0, 0x0C);  /* display control; set normal mode (D=1, E=0) */
}

static void serdisp_i2c_r320_cmd_blank(serdisp_t* dd) {
  serdisp_i2c_transfer(dd, 0, 0x80);  /* control byte with set Co bit and D/C set to logical 0 */
  serdisp_i2c_transfer(dd, 0, 0x08);  /* display control; set blank mode (D=0, E=0) */
}


static void serdisp_i2c_r320_cmd_normal(serdisp_t* dd) {
  serdisp_i2c_transfer(dd, 0, 0x80);  /* control byte with set Co bit and D/C set to logical 0 */
  serdisp_i2c_transfer(dd, 0, 0x0C);  /* display control; set normal mode (D=1, E=0) */
}

static void serdisp_i2c_r320_cmd_allon(serdisp_t* dd) {
  serdisp_i2c_transfer(dd, 0, 0x80);  /* control byte with set Co bit and D/C set to logical 0 */
  serdisp_i2c_transfer(dd, 0, 0x09);  /* display control; set all on mode (D=0, E=0) */
}

static void serdisp_i2c_r320_cmd_invert(serdisp_t* dd) {
  serdisp_i2c_transfer(dd, 0, 0x80);  /* control byte with set Co bit and D/C set to logical 0 */
  serdisp_i2c_transfer(dd, 0, 0x0d);  /* display control; set revese mode (D=1, E=1) */
}

static void serdisp_i2c_r320_cmd_setpos(serdisp_t* dd, int x, int page) {
#if 0
  if (x >= 0 && x <= 0x7f) {
    serdisp_i2c_transfer(dd, 0, 0x80);         /* one control byte */
    serdisp_i2c_transfer(dd, 0, 0x80 | x);     /* set x */
  }
  if (page >= 0 && page <= 0x0f) {
    serdisp_i2c_transfer(dd, 0, 0x80);         /* one control byte */
    serdisp_i2c_transfer(dd, 0, 0x40 | page);  /* set page */
  }
#endif
  serdisp_i2c_transfer(dd, 0, 0x00);           /* one control byte */
  serdisp_i2c_transfer(dd, 0, 0x80 | x);       /* set x */
  serdisp_i2c_transfer(dd, 0, 0x40 | page);    /* set page */
  serdisp_i2c_internal_i2cSTOP(dd);  
  serdisp_i2c_internal_i2cSTART(dd);
  serdisp_i2c_transfer(dd, 0, 0x40);           /* from now on: data bytes until i2c stop */
}
#endif



/* main functions */


/* *********************************
   serdisp_t* serdisp_i2c_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle (not used in here)
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_i2c_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "serdisp_i2c_setup(): cannot allocate display descriptor");
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));

  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_i2c_specific_t)) )) {
    free(dd);
    return (serdisp_t*)0;
  }

  memset(dd->specific_data, 0, sizeof(serdisp_i2c_specific_t));

  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("ERICSSONT2X", dispname))
    dd->dsp_id = DISPID_ERICSSONT2X;
  else if (serdisp_comparedispnames("ERICSSONR520", dispname))
    dd->dsp_id = DISPID_ERICSSONR520;
#if 0
  else if (serdisp_comparedispnames("ERICSSONR320", dispname))
    dd->dsp_id = DISPID_ERICSSONR320;
#endif
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_i2c.c", dispname);
    return (serdisp_t*)0;
  }


  /* supported output devices */
  dd->connection_types  = SERDISPCONNTYPE_PARPORT | SERDISPCONNTYPE_SERRAW;

  /* default assignments */
  dd->fp_init           = &serdisp_i2c_init;
  dd->fp_update         = &serdisp_i2c_update;
  dd->fp_setoption      = &serdisp_i2c_setoption;
  dd->fp_close          = &serdisp_i2c_close;

  /* per display settings */

  if (dd->dsp_id == DISPID_ERICSSONT2X) {
    dd->width             = 101;
    dd->height            = 33;
    dd->depth             = 1;
    dd->dsparea_width     = 33000;  /* display area in micrometres (measured) */
    dd->dsparea_height    = 12000;
    dd->feature_contrast  = 0;
    dd->feature_invert    = 0;

    dd->optalgo_maxdelta  = 3;

    serdisp_i2c_internal_getStruct(dd)->address = 0x70;
    serdisp_i2c_internal_getStruct(dd)->fp_cmd_init    = &serdisp_i2c_t2x_cmd_init;
    serdisp_i2c_internal_getStruct(dd)->fp_cmd_blank   = &serdisp_i2c_t2x_cmd_blank;
    serdisp_i2c_internal_getStruct(dd)->fp_cmd_normal  = &serdisp_i2c_t2x_cmd_normal;
    serdisp_i2c_internal_getStruct(dd)->fp_cmd_allon   = &serdisp_i2c_t2x_cmd_allon;
    serdisp_i2c_internal_getStruct(dd)->fp_cmd_setpos  = &serdisp_i2c_t2x_cmd_setpos;
  } else if (dd->dsp_id == DISPID_ERICSSONR520) {
    dd->width             = 101;
    dd->height            = 67;
    dd->depth             = 2;
    dd->dsparea_width     = 30000;  /* display area in micrometres (measured) */
    dd->dsparea_height    = 24000; 
    dd->feature_contrast  = 0;
    dd->feature_invert    = 0;  /* bug to find - set to 0 for the moment */

    dd->optalgo_maxdelta  = 3;

    serdisp_i2c_internal_getStruct(dd)->address = 0x76;
    serdisp_i2c_internal_getStruct(dd)->fp_cmd_init    = &serdisp_i2c_r520_cmd_init;
    serdisp_i2c_internal_getStruct(dd)->fp_cmd_blank   = &serdisp_i2c_r520_cmd_blank;
    serdisp_i2c_internal_getStruct(dd)->fp_cmd_normal  = &serdisp_i2c_r520_cmd_normal;
    serdisp_i2c_internal_getStruct(dd)->fp_cmd_allon   = &serdisp_i2c_r520_cmd_allon;
    serdisp_i2c_internal_getStruct(dd)->fp_cmd_setpos  = &serdisp_i2c_r520_cmd_setpos;
    serdisp_i2c_internal_getStruct(dd)->fp_cmd_invert  = &serdisp_i2c_r520_cmd_invert;

    /* allocate and initialise colour table */
#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
    if (! (dd->ctable = (SDCol*) sdtools_malloc( sizeof(SDCol) *  4 ) ) ) {
#else
    if (! (dd->ctable = (long*) sdtools_malloc( sizeof(long) *  4 ) ) ) {
#endif
      sd_error(SERDISP_EMALLOC, "serdisp_i2c_setup(): cannot allocate indexed colour table");
      free(dd->specific_data);
      free(dd);
      return (serdisp_t*)0;
    }

#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
    serdisp_setsdcoltabentry(dd, 0, serdisp_GREY2ARGB(0xFF));  /* white */
    serdisp_setsdcoltabentry(dd, 1, serdisp_GREY2ARGB(0xAA));
    serdisp_setsdcoltabentry(dd, 2, serdisp_GREY2ARGB(0x55));
    serdisp_setsdcoltabentry(dd, 3, serdisp_GREY2ARGB(0x00));  /* black */
#else
    serdisp_setcoltabentry(dd, 0, serdisp_GREY2ARGB(0xFF));  /* white */
    serdisp_setcoltabentry(dd, 1, serdisp_GREY2ARGB(0xAA));
    serdisp_setcoltabentry(dd, 2, serdisp_GREY2ARGB(0x55));
    serdisp_setcoltabentry(dd, 3, serdisp_GREY2ARGB(0x00));  /* black */
#endif

#if 0
  } else if (dd->dsp_id == DISPID_ERICSSONR320) {
    dd->width             = 102;
    dd->height            = 65;
    dd->depth             = 1;
    dd->dsparea_width     = 30000;  /* display area in micrometres (measured) */
    dd->dsparea_height    = 24000; 
    dd->feature_contrast  = 0;
    dd->feature_invert    = 1;

    dd->optalgo_maxdelta  = 3;

    serdisp_i2c_internal_getStruct(dd)->address = 0x7A;
    serdisp_i2c_internal_getStruct(dd)->fp_cmd_init    = &serdisp_i2c_r320_cmd_init;
    serdisp_i2c_internal_getStruct(dd)->fp_cmd_blank   = &serdisp_i2c_r320_cmd_blank;
    serdisp_i2c_internal_getStruct(dd)->fp_cmd_normal  = &serdisp_i2c_r320_cmd_normal;
    serdisp_i2c_internal_getStruct(dd)->fp_cmd_allon   = &serdisp_i2c_r320_cmd_allon;
    serdisp_i2c_internal_getStruct(dd)->fp_cmd_setpos  = &serdisp_i2c_r320_cmd_setpos;
    serdisp_i2c_internal_getStruct(dd)->fp_cmd_invert  = &serdisp_i2c_r320_cmd_invert;
#endif
  } else {
    /* must not occur (else bug in function entry) */
  }


  /* finally set some non display specific defaults */

  dd->curr_rotate       = 0;         /* unrotated display */
  dd->curr_backlight    = 0;         /* start with backlight off */
  dd->curr_invert       = 0;         /* display not inverted */


  serdisp_setupstructinfos(dd, serdisp_i2c_wiresignals, serdisp_i2c_wiredefs, serdisp_i2c_options);

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    free(dd);
    dd = 0;
    return (serdisp_t*)0;    
  }


  /* first address == default address */ 
  serdisp_i2c_internal_getStruct(dd)->curr_address = serdisp_i2c_internal_getStruct(dd)->address;

  return dd;
}



/* *********************************
   void serdisp_i2c_init(dd)
   *********************************
   initialise a i2c-based display
   *********************************
   dd     ... display descriptor
*/
void serdisp_i2c_init(serdisp_t* dd) {
  if (dd->sdcd->conntype == SERDISPCONNTYPE_PARPORT) {
    ;  /* wiring set up automatically */
  } else if (dd->sdcd->conntype == SERDISPCONNTYPE_SERRAW) {

    dd->sdcd->io_flags_default = SD_SP_TXDON;

    if (dd->sdcd->directIO) {
      dd->delay = 1;
      /* bit 0: force data terminal ready (DTR); bit 1: force request to send (RTS)*/
      dd->sdcd->signals[0] = 0x1;  /* SIG_SDA */   
      dd->sdcd->signals[1] = 0x2;  /* SIG_SCLK */
    } else {
      dd->delay = 0;
      dd->sdcd->signals[0] = TIOCM_DTR;  /* SIG_SDA */
      dd->sdcd->signals[1] = TIOCM_RTS;  /* SIG_SCLK */
    }
  } else {
    /* shouldn't occur, should be caught by serdisp_init() */
    return;
  }

  dd->feature_backlight = (SIG_BACKLIGHT != 0) ? 1 : 0;

  if (dd->sdcd->conntype == SERDISPCONNTYPE_PARPORT && SIG_RESET) {
    /* RESET to low */
    SDCONN_write(dd->sdcd, SIG_RESET, 0);

    /* delay 5 ms */
    SDCONN_usleep(dd->sdcd, 5);
  }

  SDCONN_usleep(dd->sdcd, 1);   /* if serraw: give display enough time to init after powering up */

  serdisp_i2c_internal_i2cSTART(dd);
  serdisp_i2c_internal_getStruct(dd)->fp_cmd_init(dd);  
  serdisp_i2c_internal_i2cSTOP(dd);  

  sd_debug(2, "serdisp_i2c_init(): done with initialising");
}


/* *********************************
   void serdisp_i2c_transfer(dd, dc, data)
   *********************************
   transfer a data or command byte to the display
   *********************************
   dd     ... display descriptor
   dc     ... dc = 1: data; dc = 0: command (ignored here (i2c needs no D/C line))
   data   ... byte to be processed 
*/
void serdisp_i2c_transfer(serdisp_t* dd, int dc, byte data) {

  int sda;

  byte t_data;
  int i;

  t_data = data;

  /* start-condition: SCLK must be low (SDA-change is only allowed when SCLK = low) */

  /* loop through all 8 bits and transfer them to the display */
  /* start with bit 7 (MSB) */
  for (i = 0; i <= 7; i++) {
    /* write a single bit to PP_SI */
    sda = (t_data & 0x80) ? 1 : 0;

    serdisp_i2c_internal_i2cSIGNALS(dd, sda, 0);
    serdisp_i2c_internal_i2cSIGNALS(dd, sda, 1);
    serdisp_i2c_internal_i2cSIGNALS(dd, sda, 0);

    /* shift byte so that next bit is on the first (MSB) position */
    t_data = (t_data & 0x7f) << 1;
  }

  /* acknoledge byte */
  serdisp_i2c_internal_i2cSIGNALS(dd, 1, 0);

  serdisp_i2c_internal_i2cSIGNALS(dd, 1, 1);

  /* read acknowledge bit when clock is high */
/*
  i = 0;
  t_data = SDCONN_read(dd->sdcd);
  while(i < 100 && t_data) {i++; t_data = SDCONN_read(dd->sdcd);}

*/
  serdisp_i2c_internal_i2cSIGNALS(dd, 1, 0);
}



/* *********************************
   void serdisp_i2c_update(dd)
   *********************************
   updates the display using display-buffer scrbuf+scrbuf_chg
   *********************************
   dd     ... display descriptor
   *********************************

   the display is redrawn using a time-saving algorithm:
   
     background knowledge: after writing a page-entry to the display,
     the x-address is increased automatically =>
     * try to utilize this auto-increasing
       (if a whole horizontal line needs to be redrawn: only 4x PutCtrl per page will be needed)

     * on the other hand try to avoid writing of unchanged data: 
       best case: no need to change any data in a certain page: 0x PutCtrl (set page) + 0x PutCtrl (set xpos) + 0x PutData)

*/
void serdisp_i2c_update(serdisp_t* dd) {
  int x, page, pagediv = 8 / dd->depth;
  int pages = (dd->height+pagediv)/pagediv;
  byte data;

#if defined(OPT_NOOUTPUT) 
  /* for testing */
#elif defined(OPT_USEOLDUPDATEALGO)
  serdisp_i2c_internal_i2cSTART(dd);  
  serdisp_i2c_internal_getStruct(dd)->fp_cmd_setpos(dd, 0, 0);
  for (page = 0; page < pages; page++) {
    for(x = 0; x < dd->width; x++) {
      data = dd->scrbuf[ dd->width * page  +  x];

      /* if display doesn't support hardware invert: software invert  */
      if (dd->curr_invert && !(dd->feature_invert))
        data = ~data;

      serdisp_i2c_transfer(dd, 0, data);
      dd->scrbuf_chg[x + dd->width*(page/8)] &= 0xFF - (1 << (page%8)) ;
    }
  }    
  serdisp_i2c_internal_i2cSTOP(dd);  
#else /* OPT_USEOLDUPDATEALGO */

  /* explanation of optimising algorithm: 

     1) after a sequence setting an absolute address (x/page), an arbitrary amount of data-bytes may be sent
     2) jumping to an absolute address needs a certain amount of bytes and i2c-signals to be sent before the first data
        byte may be sent
        eg: Ericsson T28: 
          i2c-start (3 signal changes) + 
          command (8 bit + 1 ack) + 
          x-address (8 bit + 1 ack) +
          n data-bytes (n times: 8 bit + 1 ack) +
          i2c-stop (3 signal changes)

     so jumping to each changed page column absolutely without worrying about the neighbour page columns may lead to
     unoptimised performance:

     eg: x ... changed 
         . ... unchanged

         byte_ack == one byte + ack bit ( == 9 bits)

         x.x.x.x.x  -> 5 abs. jumps == 5*2*3 signal changes + 5*(1+1+1) byte_acks == 30 sig chg + 15 bytes + 15 ack
         x..x       -> 2 abs. jumps == 2*2*3 signal changes + 2*(1+1+1) byte_acks == 12 sig chg + 6 bytes + 6 ack

     optimal performance:
     absolute jumps only after delta page columns are unchanged (delta -> optalgo_maxdelta in source code)
     all (unchanged) page columns in between are transmitted (but need only 1 byte + 1 ack each)

     eg: delta == 3

         x.x.x.x.x  -> 1 abs. jump == 1*2*3 signal changes + 1*(1+1+9) byte_acks == 6 sig chg + 11 bytes + 11 ack
         x..x       -> 1 abs. jump == 1*2*3 signal changes + 1*(1+1+4) byte_acks == 6 sig chg + 6 bytes + 6 ack

     so with worst-case scenario 'every 4th page column is changed' and delta == 3
     the optimising algorithm would be faster with, with delta > 3 it would be slower than unoptimised absolute address jumping
     (12 sig changes + 9*6 bits  vs. 6 sig changes + 9*6 bits )   
  */

  int status = 0; /* status == 0: i2c-start missing; 1: i2c-start was issued */
  int delta; 
  int mark_abs = 0; /* mark absolute screen buffer position */

  page = 0;
  x = 0;

  while (page < pages) {
    /* first changed column-page */
    if ( dd->scrbuf_chg[x + dd->width*(page/8)] & ( 1 << (page%8)) ) {
      if (!status) {  /* no i2c start yet? issue one! */
        serdisp_i2c_internal_i2cSTART(dd);
        serdisp_i2c_internal_getStruct(dd)->fp_cmd_setpos(dd, x, page);  
        status = 1;
      }
      /* write this column page */
      data = dd->scrbuf[ dd->width * page  +  x];

      /* if display doesn't support hardware invert: software invert  */
      if (dd->curr_invert && !(dd->feature_invert))
        data = ~data;

      serdisp_i2c_transfer(dd, 0, data);
      dd->scrbuf_chg[x + dd->width*(page/8)] &= 0xFF - (1 << (page%8)) ;

      /* save absolute position in screen buffer in case next page column(s) is/are unchanged */
      mark_abs = dd->width*page + x  + 1;

      x++;

      /* advance to next page if x at max */
      if (x == dd->width) {
        page++;
        x = 0;
      }

      delta = 0;
      while (page < pages && delta < dd->optalgo_maxdelta) {

        if ( dd->scrbuf_chg[x + dd->width*(page/8)] & ( 1 << (page%8)) ) {

          /* delta: transfer unchanged page columns */
          if (delta > 0 ) {
            int i;
            for (i = mark_abs; i < mark_abs+delta; i++) {
              data = dd->scrbuf[i];
              if (dd->curr_invert && !(dd->feature_invert))
                data = ~data;
              serdisp_i2c_transfer(dd, 0, data);
            }
          }

          data = dd->scrbuf[ dd->width * page  +  x];
          if (dd->curr_invert && !(dd->feature_invert))
            data = ~data;
          serdisp_i2c_transfer(dd, 0, data);
          dd->scrbuf_chg[x + dd->width*(page/8)] &= 0xFF - (1 << (page%8)) ;

          /* save absolute position in screen buffer in case next page column(s) is/are unchanged */
          mark_abs = dd->width*page + x  + 1;

          delta = 0;
        } else {
          delta++;
        }

        x++;
        if (x == dd->width) {
          page++;
          x = 0;
        }
      }
    } else {
      x++;
      if (x == dd->width) {
        page++;
        x = 0;
      }
    }

    if (status) {
      serdisp_i2c_internal_i2cSTOP(dd);  
      status = 0;
    }
  }

#endif /* OPT_USEOLDUPDATEALGO */

}



/* *********************************
   int serdisp_i2c_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_i2c_setoption(serdisp_t* dd, const char* option, long value) {

  if (dd->feature_invert && serdisp_compareoptionnames(dd, option, "INVERT") ) {
    if (value < 2) 
      dd->curr_invert = (int)value;
    else
      dd->curr_invert = (dd->curr_invert) ? 0 : 1;

    serdisp_i2c_internal_i2cSTART(dd);  
    if (dd->curr_invert) 
      serdisp_i2c_internal_getStruct(dd)->fp_cmd_invert(dd);
    else
      serdisp_i2c_internal_getStruct(dd)->fp_cmd_normal(dd);
    serdisp_i2c_internal_i2cSTOP(dd);
  } else if (dd->feature_backlight && serdisp_compareoptionnames(dd, option, "BACKLIGHT" )) {
    if (value < 2) 
      dd->curr_backlight = (int)value;
    else
      dd->curr_backlight = (dd->curr_backlight) ? 0 : 1;
    /* no command for en/disable backlight, so issue 'dummy'-command
       (which indirectly enables/disabled backlight) */

    serdisp_i2c_internal_i2cSTART(dd);  
    serdisp_i2c_internal_getStruct(dd)->fp_cmd_normal(dd);
    serdisp_i2c_internal_i2cSTOP(dd);
/*  } else if (dd->feature_contrast && serdisp_compareoptionnames(dd, option, "CONTRAST" )) {

    dd->curr_contrast = sdtools_contrast_norm2hw(dd, (int)value);
*/
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  return 1;
}



/* *********************************
   void serdisp_i2c_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_i2c_close(serdisp_t* dd) {
  serdisp_i2c_setoption(dd, "BACKLIGHT", SD_OPTION_NO);

  serdisp_i2c_internal_i2cSTART(dd);  
  serdisp_i2c_internal_getStruct(dd)->fp_cmd_blank(dd);
  serdisp_i2c_internal_i2cSTOP(dd);
}




