/*
 *************************************************************************
 *
 * serdisp_specific_nokcol.c
 * routines for controlling nokia colour displays using the following controllers:
 * * Seiko S1D15F14  (nokia 3510i, 3530)
 * * Seiko S1D15G10  (nokia 6100 and compliant)
 *
 *************************************************************************
 *
 * copyright (C) 2006-2010  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
 *
 * S1D15G10 support contributed by:
 *           (C)      2007  Abhishek Dutta
 * email     thelinuxmaniac@gmail.com
 * additional maintenance and enhancements by
 * copyright (C) 2007-2008  wolfgang astleitner
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
/*#include "serdisplib/serdisp_specific_nokcol.h"*/
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_colour.h"


/* #define OPT_USEOLDUPDATEALGO */

/* (only 8-bit mode): use modified colour table used in nokia 6100 lcd library: 
    http://www.apetech.de/article.php?artId=3&nnId=12  */
#define USE_APETECH_COLTABLE

/* 10ms; twice to be sure */
#define RESET_TIME  (10*1000)

/*
 * constants
 */

#define INI_MINGREYSCVAL  0x10   /* minimum value for greyscale position set */
#define INI_MAXGREYSCVAL  0x75   /* maximum value for greyscale position set,  < 0xFF !! */

#define INI_COLMOD_8      0x02   /* 8 bit / pixel */
#define INI_COLMOD_12     0x03   /* 12 bit / pixel */
#define INI_COLMOD_16     0x05   /* 16 bit / pixel */


/* define shorts for signals to make coding simpler. must match order of serdisp_nokcol_wiresignals[] */
#define SIG_SDATA       (dd->sdcd->signals[ 0])
#define SIG_SCLK        (dd->sdcd->signals[ 1])
#define SIG_CS          (dd->sdcd->signals[ 2])
#define SIG_A0          (dd->sdcd->signals[ 3])
#define SIG_RESET       (dd->sdcd->signals[ 4])
#define SIG_BACKLIGHT   (dd->sdcd->signals[ 5])


/* different display types/models supported by this driver */
#define DISPID_N3510I   1
#define DISPID_S1D15G10 2


serdisp_wiresignal_t serdisp_nokcol_wiresignals[] = {
 /*  type   signame   actlow   cord  index */
   {SDCT_PP, "SDATA",      0,   'D',    0 }
  ,{SDCT_PP, "SCLK",       0,   'C',    1 }
  ,{SDCT_PP, "CS",         0,   'D',    2 }
  ,{SDCT_PP, "A0",         0,   'D',    3 }  /* only used in 8-bit mode. n3510i uses 9-bit mode */
  ,{SDCT_PP, "RESET",      1,   'D',    4 }
  ,{SDCT_PP, "BACKLIGHT",  0,   'D',    5 }
};


serdisp_wiredef_t serdisp_nokcol_wiredefs[] = {
   {  0, SDCT_PP, "SerDispBG",      "SDATA:D0,SCLK:D1,CS:D2,RESET:D5,BACKLIGHT:D7", "SerDisp wiring, w/ backlight"}
  ,{  1, SDCT_PP, "SerDisp",        "SDATA:D0,SCLK:D1,CS:D2,RESET:D5",              "SerDisp wiring, w/o backlight"}
  ,{  2, SDCT_PP, "SerDispBGHWRes", "SDATA:D0,SCLK:D1,CS:D2,BACKLIGHT:D7",          "SerDisp wiring, w/ hardware reset, w/ backlight"}
  ,{  3, SDCT_PP, "SerDispHWRes",   "SDATA:D0,SCLK:D1,CS:D2",                       "SerDisp wiring, w/ hardware reset, w/o backlight"}
};

serdisp_options_t serdisp_nokcol_options[] = {
   /*  name          aliasnames    min    max mod int defines  */
/*   {  "DEPTH",       "COLS,COLOURS", 8,    16,  4, 0,  "65536=16,64k=16,4096=12,4k=12,256=8"}*/
   {  "DEPTH",       "COLS,COLOURS", 8,    12,  4, 0,  "4096=12,4k=12,256=8"}
  ,{  "CONTRAST",    "",             0,    10,  1, 1,  ""}
  ,{  "BACKLIGHT",   "",             0,     1,  1, 1,  ""}
  ,{  "BRIGHTNESS", "",              0,   100,  1, 1,  ""}      /* brightness [0 .. 100] */
#if SUPPORT_DELAY  
  ,{  "DELAY",       "",             0,    -1,  1, 1,  ""}
#endif
};

/* options for S1D15G10 based displays.  12bit colour depth supported since 1.97.8 */
serdisp_options_t serdisp_s1d15g10_options[] = {
   /*  name          aliasnames    min    max mod int defines  */
   {  "DEPTH",       "COLS,COLOURS", 8,    12,  4, 0,  "4096=12,4k=12,256=8" }
  ,{  "CONTRAST",    "",             0,    10,  1, 1,  ""}
  ,{  "BACKLIGHT",   "",             0,     1,  1, 1,  ""}
  ,{  "OFFSETX",     "",             0,     2,  1, 0,  ""}
  ,{  "OFFSETY",     "",             0,     2,  1, 0,  ""}
  ,{  "BRIGHTNESS", "",              0,   100,  1, 1,  ""}      /* brightness [0 .. 100] */
#if SUPPORT_DELAY  
  ,{  "DELAY",       "",             0,    -1,  1, 1,  ""}
#endif
};



/* internal typedefs and functions */

static void serdisp_nokcol_init      (serdisp_t*);
static void serdisp_nokcol_update    (serdisp_t*);
static int  serdisp_nokcol_setoption (serdisp_t*, const char*, long);
static void serdisp_nokcol_close     (serdisp_t*);

static void serdisp_nokcol_transfer_signals (serdisp_t* dd, int mode, byte item);
static void serdisp_nokcol_transfer_spi     (serdisp_t* dd, int mode, byte item);
static void serdisp_nokcol_writecmd         (serdisp_t* dd, byte cmd);
static void serdisp_nokcol_writedata        (serdisp_t* dd, byte data);
static void serdisp_nokcol_writecommit      (serdisp_t* dd, byte data);


typedef struct serdisp_nokcol_specific_s {
  void (*fp_transfer)  (serdisp_t* dd, int type, byte item);
} serdisp_nokcol_specific_t;


static serdisp_nokcol_specific_t* serdisp_nokcol_internal_getStruct(serdisp_t* dd) {
  return (serdisp_nokcol_specific_t*)(dd->specific_data);
}



void serdisp_nokcol_transfer_signals(serdisp_t* dd, int mode, byte item) {
  /* mode:  0 .. data   1 ... command   2 ... cs commit */

  if (mode == 2) {  /* a command or data-stream is committed when changing /CS to HIGH */
    /* init with LSB from last command / data */
    long td = (item & 0x01) ? SIG_SDATA : 0;

    if (dd->feature_backlight && dd->curr_backlight)
      td |= SIG_BACKLIGHT;

    /* SCLK high during the entire CS commit */
    td |= SIG_SCLK;

    /* CS -> high  */
    SDCONN_writedelay(dd->sdcd, td | SIG_CS, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata, dd->delay);
    /* CS -> low  */
    SDCONN_writedelay(dd->sdcd, td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata, dd->delay);
  } else {
    /* 'signal byte': contains all signal bits controlling the display */
    long td;
    /* same as td, but with clock-signal enabled */
    long td_clk;
    byte t_data;
    int i;

    t_data = item;

    td = 0;

    if (dd->feature_backlight && dd->curr_backlight)
      td |= SIG_BACKLIGHT;

    /* 9-bit mode: send D/C bit */
    if (!SIG_A0) {
      if (mode == 0)  /* data */
        td |= SIG_SDATA;  /* send 1 for data, 0 for command */

      td_clk = td | SIG_SCLK;

      /* write command/data bit */
      SDCONN_writedelay(dd->sdcd, td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata, dd->delay);
      SDCONN_writedelay(dd->sdcd, td_clk, dd->sdcd->io_flags_writecmd, dd->delay);
#ifdef EXACT_TRANSFER
      SDCONN_writedelay(dd->sdcd, td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata, dd->delay);
#endif
    }

    /* loop through all 8 bits and transfer them to the display */
    /* start with bit 7 (MSB) */
    for (i = 0; i <= 7; i++) {
      /* write a single bit to SIG_SDATA */
      if (t_data & 0x80)  /* bit == 1 */
        td |= SIG_SDATA;
      else                /* bit == 0 */
        td &= (0xffffffff - SIG_SDATA);

      /* 8-bit mode: set D/C signal when LSB */
      if (SIG_A0 && i == 7)
        if (mode == 0)  /* data */
          td |= SIG_A0;  /* send 1 for data, 0 for command */

      /* clock => high */
      td_clk = td | SIG_SCLK;

      /* write content to display */
      SDCONN_writedelay(dd->sdcd, td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata, dd->delay);

      /* set clock to high */
      SDCONN_writedelay(dd->sdcd, td_clk, dd->sdcd->io_flags_writecmd, dd->delay);

#ifdef EXACT_TRANSFER
      /* set clock to low */
      SDCONN_writedelay(dd->sdcd, td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata, dd->delay);
#endif

      /* shift byte so that next bit is on the first (MSB) position */
      t_data = (t_data & 0x7f) << 1;
    }
  }
}


void serdisp_nokcol_transfer_spi(serdisp_t* dd, int mode, byte item) {
  /* mode:  0 .. data   1 ... command   2 ... cs commit */
  /* flags:
       ---- --xx: mode
   */
  SDCONN_writedelay(
    dd->sdcd, (mode  << 16) | (long)item, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata, 
    dd->delay
  );
}



void serdisp_nokcol_writecmd(serdisp_t* dd, byte cmd) {
  serdisp_nokcol_internal_getStruct(dd)->fp_transfer(dd, 1, cmd);
/*  serdisp_nokcol_internal_getStruct(dd)->fp_transfer(dd, 2, cmd); */ /* commit */
}


void serdisp_nokcol_writedata(serdisp_t* dd, byte data) {
  serdisp_nokcol_internal_getStruct(dd)->fp_transfer(dd, 0, data);
}

void serdisp_nokcol_writecommit(serdisp_t* dd, byte data) {
  serdisp_nokcol_internal_getStruct(dd)->fp_transfer(dd, 2, data);
}


/* callback-function for setting non-standard options */
static void* serdisp_nokcol_getvalueptr (serdisp_t* dd, const char* optionname, int* typesize) {
  if (serdisp_compareoptionnames(dd, optionname, "DEPTH")) {
    *typesize = sizeof(byte);
    return &(dd->depth);
  } else if (serdisp_compareoptionnames(dd, optionname, "OFFSETX")) {
    *typesize = sizeof(int);
    return &(dd->startxcol);
  } else if (serdisp_compareoptionnames(dd, optionname, "OFFSETY")) {
    *typesize = sizeof(int);
    return &(dd->startycol);
  }
  return 0;
}

/* main functions */


/* *********************************
   serdisp_t* serdisp_nokcol_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle (not used in here)
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_nokcol_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate display descriptor", __func__);
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));

  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_nokcol_specific_t)) )) {
    free(dd);
    return (serdisp_t*)0;
  }
  memset(dd->specific_data, 0, sizeof(serdisp_nokcol_specific_t));

  /* nokia colour displays supported in here */
  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("N3510I", dispname))
    dd->dsp_id = DISPID_N3510I;
  else if (serdisp_comparedispnames("S1D15G10", dispname))
    dd->dsp_id = DISPID_S1D15G10;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_nokcol.c", dispname);
    return (serdisp_t*)0;
  }

  /* per display settings */

  dd->width             = 98;
  dd->height            = 67;
  dd->depth             = 12;
  dd->dsparea_width     = 31000;     /* display area in micrometres (measured) */
  dd->dsparea_height    = 25000;
  dd->min_contrast      = 0x20;
  dd->max_contrast      = 0x5F;
/*  dd->min_contrast      = 0;
  dd->max_contrast      = 0x7F;*/
  dd->feature_contrast  = 1;
  dd->feature_backlight = 1;
  dd->feature_invert    = 1;

  /* max. delta for optimised update algorithm */
  dd->optalgo_maxdelta  = 6;

  dd->delay = 0;

  /* finally set some non display specific defaults */

  dd->curr_rotate       = 0;         /* unrotated display */
  dd->curr_invert       = 0;         /* display not inverted */

  dd->curr_backlight    = 1;         /* start with backlight on */

  /* supported output devices */
  dd->connection_types  = SERDISPCONNTYPE_PARPORT;
  dd->supp_protocols    = SDPROTO_EMULATION | SDPROTO_SPI;

  /* supported colour spaces */
  dd->colour_spaces     = SD_CS_RGB444 | SD_CS_RGB332;

  dd->fp_init           = &serdisp_nokcol_init;
  dd->fp_update         = &serdisp_nokcol_update;
  dd->fp_close          = &serdisp_nokcol_close;
  dd->fp_setoption      = &serdisp_nokcol_setoption;
  dd->fp_getvalueptr    = &serdisp_nokcol_getvalueptr;

  if (dd->dsp_id == DISPID_S1D15G10) {
    serdisp_setupstructinfos(dd, serdisp_nokcol_wiresignals, serdisp_nokcol_wiredefs, serdisp_s1d15g10_options);
  } else {
    serdisp_setupstructinfos(dd, serdisp_nokcol_wiresignals, serdisp_nokcol_wiredefs, serdisp_nokcol_options);
  }


  if (dd->dsp_id == DISPID_S1D15G10) {
    dd->width             = 130;
    dd->height            = 130;
    dd->dsparea_width     = 27000;     /* display area in micrometres (measured) */
    dd->dsparea_height    = 27000;
    /* startxcol / startycol can be changed through options SHIFTX and SHIFTY */
    dd->startxcol         = 1;         /* default: 1st visible display column == 1 */
    dd->startycol         = 1;         /* default: 1st visible display line == 1 */

    dd->feature_backlight = 0;

    dd->min_contrast      = 0x15; /* 0x00; */
    dd->max_contrast      = 0x2A; /* 0x3F; */
  }

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    serdisp_freeresources(dd);
    dd = 0;
    return (serdisp_t*)0;    
  }

  if ( (dd->dsp_id == DISPID_S1D15G10) && (dd->startxcol % 2) && (dd->depth == 12) ) {
    int j;

    dd->xcolgaps = 2;

    if (! (dd->xreloctab = (int*) sdtools_malloc( sizeof(int) * dd->width)) ) {
      sd_error(SERDISP_EMALLOC, "%s: cannot allocate relocation table", __func__);
      serdisp_freeresources(dd);
      dd = 0;
      return (serdisp_t*)0;
    }

    for (j = 0; j < dd->width; j++)
      dd->xreloctab[j] = j + dd->startxcol;
  }

  sd_debug(2, "%s(): colour depth: %d", __func__, dd->depth);

  return dd;
}



/* *********************************
   void serdisp_nokcol_init(dd)
   *********************************
   initialise a nokcol-based display
   *********************************
   dd     ... display descriptor
*/
void serdisp_nokcol_init(serdisp_t* dd) {
  int i;
  int range;
  byte val = 0;

  /* commands w/ different hexcodes but equal handling */
  byte cmd_rgbset8;
  byte cmd_dispon;

  /* adapt maxdelta for optimising algo if depth != 8 */
  dd->optalgo_maxdelta = (dd->optalgo_maxdelta * dd->depth) / 8;

  /* select transfer modus */
  if (dd->sdcd->protocol == SDPROTO_SPI) {   /* send SPI directly, clocking is done by the receiver */
    SIG_RESET = 0L;
    SIG_BACKLIGHT = 0L;
    SIG_SDATA = 0L;
    SIG_SCLK = 0L;
    serdisp_nokcol_internal_getStruct(dd)->fp_transfer = &serdisp_nokcol_transfer_spi;

    /* set SPI parameters (will be set / initialised by SDCONNxxx_confinit() */
    dd->sdcd->spi.framelen = 5;  /* framelen + 4 -> 9 bits */
    dd->sdcd->spi.cpol     = 1;  /* SK (SCLK) high */
    dd->sdcd->spi.cpha     = 1;  /* DO (SI) write at falling SCLK */
    dd->sdcd->spi.data_high= 1;  /* data = active high; command = active low */
    dd->sdcd->spi.divider =  0;  /* auto */
    dd->sdcd->spi.prescaler= 0;  /* auto */
  } else {  /* emulate SPI protocol (including clocking a.s.o) */
    serdisp_nokcol_internal_getStruct(dd)->fp_transfer = &serdisp_nokcol_transfer_signals;
  }

  if (SIG_RESET)
    SDCONN_writedelay(dd->sdcd, SIG_RESET, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata, dd->delay);
  SDCONN_usleep(dd->sdcd, RESET_TIME);                 /* 10 ms delay */

  if (dd->dsp_id == DISPID_N3510I) {
    /* setup cmd_vars for equal handling stuff */
    cmd_rgbset8 = 0x2D;  /* RGBSET */
    cmd_dispon  = 0x29;  /* DISPON */

    /* do n3510i-specific stuff */
    serdisp_nokcol_writecmd   (dd, 0x01);             /* SWRESET: software reset */
    SDCONN_usleep(dd->sdcd, RESET_TIME);              /* 10 ms delay */

    serdisp_nokcol_writecmd   (dd, 0xC6);             /* INITESC: set up status in the IC */

    /* display setup 1 */
    serdisp_nokcol_writecmd   (dd, 0xB9);             /* REFSET: set up status in the IC */
    serdisp_nokcol_writedata  (dd, 0x00);             /* *****000 for nokia 3510i */
    serdisp_nokcol_writecommit(dd, 0x00);

    serdisp_nokcol_writecmd   (dd, 0xB6);             /* DISCTP: display control */
    serdisp_nokcol_writedata  (dd, INI_MAXGREYSCVAL+1);  /* P17-P10: length of one selection term by the number of issues of the osc clock */
    serdisp_nokcol_writedata  (dd, 0x80);             /* P27 = 1 (no N line inversion), P26-P20: 0 */
    serdisp_nokcol_writedata  (dd, 0x81);             /* P37 = 1 (2 div in idle mode), P35-P33: 000 = bias 1/9, P31: 0 = 98xXXX, P30: 1 = 1/67 */
    serdisp_nokcol_writedata  (dd, 0x54);             /* P47-P40: display line = 84 (duty = 1/82, not relevant for nokia 3510i) */
    serdisp_nokcol_writedata  (dd, 0x45);             /* P57-P50: display line = 69 (duty = 1/67) */
    serdisp_nokcol_writedata  (dd, 0x52);             /* P67-P60: actual drive duty = 82 (not relevant for nokia 3510i) */
    serdisp_nokcol_writedata  (dd, 0x47);             /* P77-P70: actual drive duty = 67 */
    serdisp_nokcol_writecommit(dd, 0x47);

    serdisp_nokcol_writecmd   (dd, 0xB3);             /* GCPSET0: grey scale position set 0 */
    range = (int) (INI_MAXGREYSCVAL - INI_MINGREYSCVAL);
    val = INI_MAXGREYSCVAL;  /* make compiler happy and initialise variable */
    for (i = 0; i < 15; i++) {
      val = INI_MINGREYSCVAL + (byte)( (range / 140) * i*10 );
      serdisp_nokcol_writedata  (dd,  val);
    }
    serdisp_nokcol_writecommit(dd, val);

    serdisp_nokcol_writecmd   (dd, 0xB5);             /* GAMSET: select gamma curve (GCPSET0 or GCPSET1) */
    serdisp_nokcol_writedata  (dd, 0x01);             /* set curve tp GCPSET0 */
    serdisp_nokcol_writecommit(dd, 0x01);

    serdisp_nokcol_writecmd   (dd, 0xBD);             /* COMOUT: common driver output select */
    serdisp_nokcol_writedata  (dd, 0x00);             /* no interlace -> P13-P12: 0  shift direction: P11-P10: don't care about shift -> 0 */
    serdisp_nokcol_writecommit(dd, 0x00);

    /* power supply setup */
    serdisp_nokcol_writecmd   (dd, 0xBE);             /* PWRCTL: power control */
    serdisp_nokcol_writedata  (dd, 0x04);             /* P16: 0 P15:0 P14: 0 internal resistance P13: 0 high power mode P12-P10: 100 = 26.3kHz */
    serdisp_nokcol_writecommit(dd, 0x04);

    serdisp_nokcol_writecmd   (dd, 0x11);             /* SLPOUT: sleep out */

    serdisp_nokcol_writecmd   (dd, 0xBA);             /* VOLCTL: voltage control */
    serdisp_nokcol_writedata  (dd, 0x7F);             /* *111 1111  */
    serdisp_nokcol_writedata  (dd, 0x03);             /* always **** **11  according to data sheet*/
    serdisp_nokcol_writecommit(dd, 0x03);

    serdisp_nokcol_writecmd   (dd, 0x25);             /* WRCNTR: write contrast */
    serdisp_nokcol_writedata  (dd, 0x3F);             /* center value = 63  */
    serdisp_nokcol_writecommit(dd, 0x3F);

    serdisp_nokcol_writecmd   (dd, 0xB7);             /* TMPGRD: temperature gradient set */
    serdisp_nokcol_writedata  (dd, 0x00);             /* P11-P10: 00 avg temp grad = -0.05 %/degree Celsius */
    for (i = 0; i < 13; i++) {
      serdisp_nokcol_writedata  (dd, 0x00);           /* all other: set to 0 according to data sheet */
    }
    serdisp_nokcol_writecommit(dd, 0x00);

    serdisp_nokcol_writecmd   (dd, 0x03);             /* BSTON: booster voltage on */

    /* display setup 2 */
    serdisp_nokcol_writecmd   (dd, 0x21);             /* INVON: display seems to be inverted through polarisation -> de-invert */

                                                      /* partial area, vert scroll def, vert scroll start addr: use reset values */  

    /* display setup 3 */

    serdisp_nokcol_writecmd   (dd, 0x3A);             /* COLMOD: pixel format */
    switch (dd->depth) {
      case 12: val = INI_COLMOD_12; break;
      /*case 16: val = INI_COLMOD_16; break;*/
      default: val = INI_COLMOD_8; break;
    }
    serdisp_nokcol_writedata  (dd, val);              /* 8/12/16 bit / pixel */
    serdisp_nokcol_writecommit(dd, val);
  } else {
    /* setup cmd_vars for equal handling stuff */
    cmd_rgbset8 = 0xCE;  /* RGBSET8 */
    cmd_dispon  = 0xAF;  /* DISPON */

    /* do s1d15g10-specific stuff */

    serdisp_nokcol_writecmd   (dd, 0xCA);             /* DISCTL: software reset */
    serdisp_nokcol_writedata  (dd, 0x03);
    serdisp_nokcol_writedata  (dd, 32);
    serdisp_nokcol_writedata  (dd, 12);
#if 0
    serdisp_nokcol_writedata  (dd, 0x00);
    serdisp_nokcol_writedata  (dd, 0x20);
    serdisp_nokcol_writedata  (dd, 0x00);
#endif
    SDCONN_usleep(dd->sdcd, RESET_TIME);              /* 10 ms delay */

    serdisp_nokcol_writecmd   (dd, 0xBB);             /* COMSCN */
    serdisp_nokcol_writedata  (dd, 0x01);

    serdisp_nokcol_writecmd   (dd, 0xD1);             /* OSCON */

    serdisp_nokcol_writecmd   (dd, 0x94);             /* SLPOUT */

#if 0
    serdisp_nokcol_writecmd   (dd, 0x81);             /* VOLCTR */
    serdisp_nokcol_writedata  (dd, 0x5);  /* make ff */
    serdisp_nokcol_writedata  (dd, 0x01);
#endif

    serdisp_nokcol_writecmd   (dd, 0x20);             /* PWRCTR */
    serdisp_nokcol_writedata  (dd, 0x0f);

    SDCONN_usleep(dd->sdcd, 100*1000);                /* 10 ms delay */

    serdisp_nokcol_writecmd   (dd, 0xA7);             /* DISINV */

    serdisp_nokcol_writecmd   (dd, 0xBC);             /* DATCTL */
    serdisp_nokcol_writedata  (dd, 0x00);
    serdisp_nokcol_writedata  (dd, 0x00);             /*  RGB-arrangment: RGB (not BGR) */
    switch (dd->depth) {   /* depth 12 not working yet -> ignored */
      case 12: val = 0x02; break;                     /* 16 greyscale display TYPE A (12 bit / pixel) */
      default: val = 0x01; break;                     /* 8 greysclae */
    }
    serdisp_nokcol_writedata  (dd, val);              /* 8/12 bit / pixel */

    serdisp_nokcol_writecmd   (dd, 0xAA);             /* ASCSET - area scroll set */
    serdisp_nokcol_writedata  (dd,    0);             /* top block address */
    serdisp_nokcol_writedata  (dd,   41);             /* bottom block address */
    serdisp_nokcol_writedata  (dd,   41);             /* number of specified blocks */
    serdisp_nokcol_writedata  (dd,    1);             /* top screen scroll */

    serdisp_nokcol_writecmd   (dd, 0xAB);             /* SCSTART - scroll start address */
    serdisp_nokcol_writedata  (dd,    0);             /* start block address */
  }

  /* do equal handling stuff (valid for both n3510i and s1d5gxx) */  
                                                      /* colour set: only needed when 8 bit mode */
  if (dd->depth == 8) {
    serdisp_nokcol_writecmd   (dd, cmd_rgbset8);     /* RGBSET: rgb set for 8 bit mode */
#ifdef USE_APETECH_COLTABLE  /* use apetech modified colour table */
    serdisp_nokcol_writedata  (dd, 0x00);            /* red */
    serdisp_nokcol_writedata  (dd, 0x03);
    serdisp_nokcol_writedata  (dd, 0x05);
    serdisp_nokcol_writedata  (dd, 0x07);
    serdisp_nokcol_writedata  (dd, 0x09);
    serdisp_nokcol_writedata  (dd, 0x0B);
    serdisp_nokcol_writedata  (dd, 0x0D);
    serdisp_nokcol_writedata  (dd, 0x0F);

    serdisp_nokcol_writedata  (dd, 0x00);            /* green */ 
    serdisp_nokcol_writedata  (dd, 0x03);
    serdisp_nokcol_writedata  (dd, 0x05);
    serdisp_nokcol_writedata  (dd, 0x07);
    serdisp_nokcol_writedata  (dd, 0x09);
    serdisp_nokcol_writedata  (dd, 0x0B);
    serdisp_nokcol_writedata  (dd, 0x0D);
    serdisp_nokcol_writedata  (dd, 0x0F);

    serdisp_nokcol_writedata  (dd, 0x00);            /* blue */ 
    serdisp_nokcol_writedata  (dd, 0x08);
    serdisp_nokcol_writedata  (dd, 0x0B);
    serdisp_nokcol_writedata  (dd, 0x0F);
#else    /* use colour table as specified in the datasheet */
    serdisp_nokcol_writedata  (dd, 0x00);            /* red */
    serdisp_nokcol_writedata  (dd, 0x02);
    serdisp_nokcol_writedata  (dd, 0x04);
    serdisp_nokcol_writedata  (dd, 0x06);
    serdisp_nokcol_writedata  (dd, 0x09);
    serdisp_nokcol_writedata  (dd, 0x0B);
    serdisp_nokcol_writedata  (dd, 0x0D);
    serdisp_nokcol_writedata  (dd, 0x0F);

    serdisp_nokcol_writedata  (dd, 0x00);            /* green */ 
    serdisp_nokcol_writedata  (dd, 0x02);
    serdisp_nokcol_writedata  (dd, 0x04);
    serdisp_nokcol_writedata  (dd, 0x06);
    serdisp_nokcol_writedata  (dd, 0x09);
    serdisp_nokcol_writedata  (dd, 0x0B);
    serdisp_nokcol_writedata  (dd, 0x0D);
    serdisp_nokcol_writedata  (dd, 0x0F);

    serdisp_nokcol_writedata  (dd, 0x00);            /* blue */ 
    serdisp_nokcol_writedata  (dd, 0x04);
    serdisp_nokcol_writedata  (dd, 0x0B);
    serdisp_nokcol_writedata  (dd, 0x0F);
#endif

    serdisp_nokcol_writecommit(dd, 0x0F);
  }

  SDCONN_commit(dd->sdcd);                           /* if streaming: be sure that every data is transmitted */

  SDCONN_usleep(dd->sdcd, 40*1000);                  /* 40ms for power stabilisation */

  serdisp_nokcol_writecmd   (dd, cmd_dispon);        /* DISPON: memory write */  

  SDCONN_commit(dd->sdcd);                           /* if streaming: be sure that every data is transmitted */

  SDCONN_usleep(dd->sdcd, 20*1000);                  /* wait 20ms */

  sd_debug(2, "%s(): done with initialising", __func__);
}



/* *********************************
   void serdisp_nokcol_update(dd)
   *********************************
   updates the display using display-buffer scrbuf+scrbuf_chg
   *********************************
   dd     ... display descriptor
   *********************************
   the display is redrawn using a time-saving algorithm
*/
void serdisp_nokcol_update(serdisp_t* dd) {
  int i;
  byte data = 0;
  /* when using depth=12 S1D15G10 requires that the starting column for CASET is even */
  int corrbyte = ((dd->dsp_id == DISPID_S1D15G10) && (dd->startxcol % 2) && (dd->depth == 12)) ? 1 : 0;

  /* CASET,PASET, and RAMWR differ on N3510i and S1D15G10 */
  byte cmd_caset = (dd->dsp_id == DISPID_N3510I) ? 0x2A : 0x15;
  byte cmd_paset = (dd->dsp_id == DISPID_N3510I) ? 0x2B : 0x75;
  byte cmd_ramwr = (dd->dsp_id == DISPID_N3510I) ? 0x2C : 0x5C;

#ifdef OPT_USEOLDUPDATEALGO
  /* unoptimised display update (slow. all pixels are redrawn) */

  serdisp_nokcol_writecmd   (dd, cmd_caset);             /* CASET: set column to 0 */
  serdisp_nokcol_writedata  (dd, dd->startxcol - corrbyte );
  serdisp_nokcol_writedata  (dd, dd->width - 1 + dd->startxcol + corrbyte );
  serdisp_nokcol_writecommit(dd, dd->width - 1 + dd->startxcol + corrbyte );
  serdisp_nokcol_writecmd   (dd, cmd_paset);             /* PASET: set page to dd->startycol */
  serdisp_nokcol_writedata  (dd, dd->startycol);  
  serdisp_nokcol_writedata  (dd, dd->height-1 + dd->startycol);
  serdisp_nokcol_writecommit(dd, dd->height-1 + dd->startycol);
  serdisp_nokcol_writecmd   (dd, cmd_ramwr);             /* RAMWR: memory write */

  for (i = 0; i < dd->scrbuf_size; i++) {
    data = (dd->scrbuf [ i ]);
    serdisp_nokcol_writedata(dd, data);
  }

  serdisp_nokcol_writecommit(dd, data);

#else /* OPT_USEOLDUPDATEALGO */

  /* display is drawn using an optimising algorithm which tries to only send as few data as possible to the display */

  /* more detailed explanations of principle: 
     see serdisp_specific_optrex323.c / i2c.c / pcd8544.c / sed153x.c / sed1565.c 
  */

  int delta; 
  int col;
  int i_delta = 0; /* i_delta - i: how many columns to transfer in one take */

  int need_cs = 0;
  int last_col_set = -1;
  int r_transmitted = 0;
  int last_idx_set = -1;

  i = 0;
  while (i < (dd->width + dd->xcolgaps) * dd->height) {
    int y_i, col_i, l;
    int last_col_i = (dd->width + dd->xcolgaps);

    int y = i / (dd->width + dd->xcolgaps);

    col = i % (dd->width + dd->xcolgaps);

    /* first changed column-page */
    if ( dd->scrbuf_chg[(col >> 3) + y * ((dd->width + dd->xcolgaps + 7 ) >> 3)] & ( 1 << (col%8) )) {
      if ((i % 2) && corrbyte)
        i--;

      i_delta = i;

      delta = 0;
      while (i_delta < (dd->width + dd->xcolgaps) * dd->height -delta-1 && delta < dd->optalgo_maxdelta) {
        y_i = (i_delta+delta+1) / (dd->width + dd->xcolgaps);
        col_i = (i_delta+delta+1) % (dd->width + dd->xcolgaps);
        if ( dd->scrbuf_chg[(col_i >> 3) + y_i * ((dd->width + dd->xcolgaps + 7 ) >> 3)] & ( 1 << (col_i%8) )) {
          i_delta += delta+1;
          delta = 0;
        } else {
          delta++;
        }
      }

      last_col_set = -1;

      /*fprintf(stderr, "col/y=%02d/%02d (i=%4d i_delta=%4d diff: %2d)\n", col, y, i, i_delta, i_delta-i);*/

      l = i;

      while ( l <= i_delta ) {
      /*for (l = i; l <= i_delta; l++) {*/
        int idx_2, idx;

        y_i = l / (dd->width + dd->xcolgaps);
        col_i = l % (dd->width + dd->xcolgaps);

        if (last_col_set == -1 || col_i < last_col_set) {  /* start correction needed for CASET */
          if (need_cs) {
            if (last_idx_set != -1 && corrbyte) {
              data = dd->scrbuf[last_idx_set+2];
              serdisp_nokcol_writedata(dd, data);
            }
            serdisp_nokcol_writecommit(dd, data);
            need_cs = 0;
          }

          serdisp_nokcol_writecmd   (dd, cmd_caset);             /* CASET: set column */
          serdisp_nokcol_writedata  (dd, col_i /* dd->startxcol - corrbyte*/);
          serdisp_nokcol_writedata  (dd, dd->width-1 + dd->xcolgaps /*dd->startxcol + corrbyte*/);
          serdisp_nokcol_writecommit(dd, dd->width-1 + dd->xcolgaps /*dd->startxcol + corrbyte*/);
          serdisp_nokcol_writecmd   (dd, cmd_paset);             /* PASET: set row */
          serdisp_nokcol_writedata  (dd, y_i + dd->startycol);
          serdisp_nokcol_writedata  (dd, dd->height-1 + dd->startycol);
          serdisp_nokcol_writecommit(dd, dd->height-1 + dd->startycol);
          last_col_set = col_i /* + dd->startxcol*/;
          r_transmitted = 0;
          serdisp_nokcol_writecmd   (dd, cmd_ramwr);             /* RAMWR: memory write */
        }

        idx_2 = ((col_i  + y_i * (dd->width + dd->xcolgaps)) * (dd->depth << 1)) / 8;
        idx = idx_2 >> 1;
        switch(dd->depth) {
          case 8:
            data =  dd->scrbuf[idx];
            serdisp_nokcol_writedata(dd, data);
            break;
          case 12:
            {
              if (idx_2 & 0x01) {
                if (r_transmitted) {
                  data = dd->scrbuf[idx+1];
                  serdisp_nokcol_writedata(dd, data);
                  r_transmitted = 0;
                } else {
                  data =  ((dd->scrbuf[idx] & 0x0F) << 4) + ((dd->scrbuf[idx+1] & 0xF0) >> 4);
                  serdisp_nokcol_writedata(dd, data);
                  data = ((dd->scrbuf[idx+1] & 0x0F) << 4) + ((dd->scrbuf[idx+2] & 0xF0) >> 4);
                  serdisp_nokcol_writedata(dd, data);
                  r_transmitted = 1;
                }
              } else {
                if (r_transmitted) {
                  data = ((dd->scrbuf[idx] & 0x0F) << 4) + ((dd->scrbuf[idx+1] & 0xF0) >> 4);
                  serdisp_nokcol_writedata(dd, data);
                  r_transmitted = 0;
                } else {
                  data = dd->scrbuf[idx];
                  serdisp_nokcol_writedata(dd, data);
                  data =  dd->scrbuf[idx+1];
                  serdisp_nokcol_writedata(dd, data);
                  r_transmitted = 1;
                }
              }
              last_idx_set = idx;
            }
            break;
          case 16:
            data = dd->scrbuf[idx];
            serdisp_nokcol_writedata(dd, data);
            data = dd->scrbuf[idx+1];
            serdisp_nokcol_writedata(dd, data);
            break;
        }
        need_cs = 1;

        dd->scrbuf_chg[ (col_i >> 3) + y_i * ((dd->width + dd->xcolgaps + 7 ) >> 3)] &=  (0xFF ^ (1 << (col_i % 8)));

        last_col_i = col_i;
        l++;
      }

      i = i_delta+1;

    } else {
      i++;
    }
  } /* while i < scrbuf_size */
  if (need_cs) {
    if (last_idx_set != -1 && corrbyte) {
      data = dd->scrbuf[last_idx_set+2];
      serdisp_nokcol_writedata(dd, data);
    }
    serdisp_nokcol_writecommit(dd, data);  
    need_cs = 0;
  }
#endif /* OPT_USEOLDUPDATEALGO */

SDCONN_commit(dd->sdcd); /* if streaming: be sure that every data is transmitted */

}



/* *********************************
   int serdisp_nokcol_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_nokcol_setoption(serdisp_t* dd, const char* option, long value) {
  if (dd->feature_invert && serdisp_compareoptionnames(dd, option, "INVERT" )) {
    byte cmd_invert, cmd_normal;

    /* note: both displays seem to be hardware inverted: so meaning is inverted here accordingly */
    switch(dd->dsp_id) {
      case DISPID_N3510I:
        cmd_invert = 0x20;  /* INVOFF */
        cmd_normal = 0x21;  /* INVON */
        break;
      default:
        cmd_invert = 0xA6;  /* DISNOR */
        cmd_normal = 0xA7;  /* DISINV */
    }

    if (value < 2) 
      dd->curr_invert = (int)value;
    else
      dd->curr_invert = (dd->curr_invert) ? 0 : 1;
    /* display is hardware inverted: swap INVON and INVOFF */
    serdisp_nokcol_writecmd(dd, (dd->curr_invert) ? cmd_invert : cmd_normal );
  } else if (dd->feature_backlight && serdisp_compareoptionnames(dd, option, "BACKLIGHT")) {
    byte cmd_nop = (dd->dsp_id == DISPID_N3510I) ? 0x00 : 0x25;   /* NOP */

    if (value < 2) 
      dd->curr_backlight = (int)value;
    else
      dd->curr_backlight = (dd->curr_backlight) ? 0 : 1;
    serdisp_nokcol_writecmd   (dd, cmd_nop);     /* NOP */
  } else if (dd->feature_contrast && 
             (serdisp_compareoptionnames(dd, option, "CONTRAST" ) ||
              serdisp_compareoptionnames(dd, option, "BRIGHTNESS" )
             )
            )
  {
    int dimmed_contrast;
    byte cmd_wrcntr = (dd->dsp_id == DISPID_N3510I) ? 0x25 : 0x81;   /* WRCNTR */

    if ( serdisp_compareoptionnames(dd, option, "CONTRAST" ) ) {
      dd->curr_contrast = sdtools_contrast_norm2hw(dd, (int)value);
    } else {
      dd->curr_dimming = 100 - (int)value;
    }

    dimmed_contrast = (((dd->curr_contrast - dd->min_contrast) * (100 - dd->curr_dimming)) / 100) + dd->min_contrast;

    serdisp_nokcol_writecmd   (dd, cmd_wrcntr);                   /* WRCNTR: write contrast */
    serdisp_nokcol_writedata  (dd, dimmed_contrast /*dd->curr_contrast*/);
    if (dd->dsp_id == DISPID_S1D15G10) {
      serdisp_nokcol_writedata  (dd, 0x03);
      serdisp_nokcol_writecommit(dd, 0x03);
    } else {
      serdisp_nokcol_writecommit(dd, dimmed_contrast /*dd->curr_contrast*/);
    }
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  SDCONN_commit(dd->sdcd); /* if streaming: be sure that every data is transmitted */
  return 1;
}



/* *********************************
   void serdisp_nokcol_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_nokcol_close(serdisp_t* dd) {
  byte cmd_dispoff = (dd->dsp_id == DISPID_N3510I) ? 0x28 : 0xAE;   /* DISPOFF */

  serdisp_nokcol_writecmd (dd, cmd_dispoff);        /* DISPOFF: display off */
  SDCONN_writedelay(dd->sdcd, 0, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata, dd->delay);
  SDCONN_commit(dd->sdcd);
}

