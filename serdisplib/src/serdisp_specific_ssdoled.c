/*
 *************************************************************************
 *
 * serdisp_specific_ssdoled.c
 * routines for controlling displays controlled by ssd-series OLED-controllers
 *
 * supported:
 * - Osram Pictiva 96x36x1,  1.0", controller SSD0303
 * - Osram Pictiva 96x64x16, 1.0", controller SSD1332
 * - Osram Pictiva 128x64x4, 2.7", controller SSD0323
 * - Bolymin BL160128A 160x128x18, controller SSD1353
 *
 * contributions:
 * - 4DOLED-282815 128x128x18 1.5" controller SSD1339  added by jopka@kvidex.ru
 *************************************************************************
 *
 * copyright (C) 2006-2010  wolfgang astleitner
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
 * command constants
 */

/* single byte commands (defines valid for all controllers used here) */
#define CMD_COLUMNLO      0x00
#define CMD_COLUMNHI      0x10
#define CMD_PAGEADDRESS   0xB0

#define CMD_NOP           0xE3

/*
 * values used mainly for initialization
 */


#define INTERFACE_8080      0
#define INTERFACE_6800      1
#define INTERFACE_SPI       2
#define INTERFACE_I2C       3


/* define shorts for signals to make coding simpler. must match order of serdisp_ssdoled_wiresignals[] */
#define SIG_SCLK        (dd->sdcd->signals[ 0])
#define SIG_SDA         (dd->sdcd->signals[ 1])

#define SIG_D0          (dd->sdcd->signals[ 0])
#define SIG_D1          (dd->sdcd->signals[ 1])
#define SIG_D2          (dd->sdcd->signals[ 2])
#define SIG_D3          (dd->sdcd->signals[ 3])
#define SIG_D4          (dd->sdcd->signals[ 4])
#define SIG_D5          (dd->sdcd->signals[ 5])
#define SIG_D6          (dd->sdcd->signals[ 6])
#define SIG_D7          (dd->sdcd->signals[ 7])   /* also needed in SPI mode */
#define SIG_CS          (dd->sdcd->signals[ 8])   /* also needed in SPI mode */
#define SIG_DC          (dd->sdcd->signals[ 9])
#define SIG_WR          (dd->sdcd->signals[10])
#define SIG_RD          (dd->sdcd->signals[11])
#define SIG_RESET       (dd->sdcd->signals[12])


/* different display types/models supported by this driver */
#define DISPID_OLED96X36X1  1
#define DISPID_OLED96X64X16 2
#define DISPID_OLED128X64X4 3
#define DISPID_BL160128A    4
#define DISPID_4DOLED282815 5


serdisp_wiresignal_t serdisp_ssdoled_wiresignals[] = {
 /*  type   signame   actlow   cord  index */
   {SDCT_PP, "CS",         1,   'C',    8 }
  ,{SDCT_PP, "DC",         0,   'C',    9 }
  ,{SDCT_PP, "WR",         1,   'C',   10 }
  ,{SDCT_PP, "RD",         1,   'C',   11 }
  ,{SDCT_PP, "RESET",      1,   'D',   12 }
  /* I2C or SPI: */
  ,{SDCT_PP, "SCLK",       0,   'C',    0 }
  ,{SDCT_PP, "SDA",        0,   'D',    1 }
};

/*wirings */
serdisp_wiredef_t serdisp_ssdoled_wiredefs[] = {
   {  0, SDCT_PP, "Original",      "DATA8,CS:nSELIN,DC:INIT,WR:nAUTO,RD:nSTRB", "Original wiring"}
  ,{  1, SDCT_PP, "OriginalSWRes", "DATA8,CS:nSELIN,DC:INIT,RESET:nAUTO,WR:nSTRB", "Original wiring w/ software reset"}
  ,{  2, SDCT_PP, "I2C",           "SCLK:D0,SDA:D1",                            "simple I2C wiring"}
  ,{  3, SDCT_PP, "SPI",           "SCLK:D0,SDA:D1,CS:D2,DC:D3",                "simple SPI wiring"}
  ,{  4, SDCT_PP, "Simple",        "DATA8,DC:INIT,WR:nLINEFD",                  "simple parallel port wiring"}
};

serdisp_options_t serdisp_ssdoled_options[] = {
   /*  name          aliasnames   min    max mod int defines  */
   {  "DELAY",       "",            0,    -1,  1, 1,  ""}
  ,{  "CONTRAST",    "",            0,    10,  1, 1,  ""}
  ,{  "BRIGHTNESS", "",             0,   100,  1, 1,  ""}      /* brightness [0 .. 100] */
/*  ,{  "INTERFACE",   "MODE",        0,     3,  1, 0,  "8080=0,6800=1,SPI=2,SERIAL=2,I2C=3"} */  /* i2c not yet supported */
  ,{  "INTERFACE",   "MODE",        0,     2,  1, 0,  "8080=0,6800=1,SPI=2,SERIAL=2"}
  ,{  "RAWCMD",      "",            0,   255,  1, 1,  ""}      /* for development - internal use only */
};


serdisp_options_t serdisp_oled96x64x16_options[] = {
   /*  name          aliasnames   min    max mod int defines  */
   {  "DEPTH",       "COLS,COLOURS",8,    16,  8, 0,  "65536=16,64k=16,256=8"}
  ,{  "DELAY",       "",            0,    -1,  1, 1,  ""}
  ,{  "CONTRAST",    "",            0,    10,  1, 1,  ""}
  ,{  "BRIGHTNESS", "",             0,   100,  1, 1,  ""}      /* brightness [0 .. 100] */
  ,{  "INTERFACE",   "MODE",        0,     2,  1, 0,  "8080=0,6800=1,SPI=2,SERIAL=2"}
  ,{  "RAWCMD",      "",            0,   255,  1, 1,  ""}      /* for development - internal use only */
};

serdisp_options_t serdisp_oled128x64x4_options[] = {
   /*  name          aliasnames   min    max mod int defines  */
   {  "DELAY",       "",            0,    -1,  1, 1,  ""}
  ,{  "CONTRAST",    "",            0,    10,  1, 1,  ""}
  ,{  "BRIGHTNESS", "",             0,   100,  1, 1,  ""}      /* brightness [0 .. 100] */
  ,{  "INTERFACE",   "MODE",        0,     2,  1, 0,  "8080=0,6800=1,SPI=2,SERIAL=2"}
  ,{  "RAWCMD",      "",            0,   255,  1, 1,  ""}      /* for development - internal use only */
};

serdisp_options_t serdisp_bl160128a_options[] = {
   /*  name          aliasnames   min    max mod int defines  */
   {  "DEPTH",       "COLS,COLOURS",8,    18,  2, 0,  "256K=18,65536=16,64k=16,256=8"}
  ,{  "DELAY",       "",            0,    -1,  1, 1,  ""}
  ,{  "CONTRAST",    "",            0,    10,  1, 1,  ""}
  ,{  "BRIGHTNESS",  "",            0,   100,  1, 1,  ""}      /* brightness [0 .. 100] */
  ,{  "SLOPPYSIGNAL","SLOPPY",      0,     1,  1, 1,  ""}
  ,{  "GSTABLECORR", "GSCORR",     50,   300,  1, 1,  ""}
  ,{  "RAWCMD",      "",            0,   255,  1, 1,  ""}      /* for development - internal use only */
};

serdisp_options_t serdisp_4doled282815_options[] = {
   /*  name          aliasnames   min    max mod int defines  */
   {  "DEPTH",       "COLS,COLOURS",8,    18,  2, 0,  "256K=18,65536=16,64k=16,256=8"}
  ,{  "DELAY",       "",            0,    -1,  1, 1,  ""}
  ,{  "CONTRAST",    "",            0,    10,  1, 1,  ""}
  ,{  "BRIGHTNESS", "",             0,   100,  1, 1,  ""}      /* brightness [0 .. 100] */
  ,{  "INTERFACE",   "MODE",        0,     2,  1, 0,  "8080=0,6800=1,SPI=2,SERIAL=2"}
  ,{  "SLOPPYSIGNAL","SLOPPY",      0,     1,  1, 1,  ""}
  ,{  "GSTABLECORR", "GSCORR",     50,   300,  1, 1,  ""}
  ,{  "RAWCMD",      "",            0,   255,  1, 1,  ""}      /* for development - internal use only */
};


/* internal typedefs and functions */

static void serdisp_ssdoled_init        (serdisp_t*);
static void serdisp_ssdoled_update      (serdisp_t*);
static int  serdisp_ssdoled_setoption   (serdisp_t*, const char*, long);
static void serdisp_ssdoled_close       (serdisp_t*);

static void serdisp_ssdoled_clear       (serdisp_t*);

static void serdisp_oledcolour_cc_update(serdisp_t*);  /* colour displays, both command + command options: D/C=C */
static void serdisp_oledcolour_cd_update(serdisp_t*);  /* colour dipslays, command: D/C=C, command options: D/C=D */
static void serdisp_oled128x64x4_update (serdisp_t*);

static void serdisp_ssdoled_transfer    (serdisp_t* dd, int iscmd, byte item);
static void serdisp_ssdoled_writecmd    (serdisp_t* dd, byte cmd);
static void serdisp_ssdoled_writedata   (serdisp_t* dd, byte data);


typedef struct serdisp_ssdoled_specific_s {
  int  interfacemode;
  int  sloppysignal;
  int  gstablecorr;
} serdisp_ssdoled_specific_t;


static serdisp_ssdoled_specific_t* serdisp_ssdoled_internal_getStruct(serdisp_t* dd) {
  return (serdisp_ssdoled_specific_t*)(dd->specific_data);
}


void serdisp_ssdoled_transfer(serdisp_t* dd, int iscmd, byte item) {
  int i;

  if (serdisp_ssdoled_internal_getStruct(dd)->interfacemode == INTERFACE_SPI) {
    /* 'signal byte': contains all signal bits controlling the display */
    long td;

    byte t_value;
    int i;

    t_value = item;
    /* SIG_CS -> low == unset active low CS (will be auto-inverted) */
    td = SIG_CS;

    /* if data => DC == high */
    if (!iscmd)
      td |= SIG_DC;

    SDCONN_write(dd->sdcd, td, dd->sdcd->io_flags_writecmd);
    sdtools_nsleep(dd->delay);


    /* loop through all 8 bits and transfer them to the display */
    /* start with bit 7 (MSB) */
    for (i = 0; i <= 7; i++) {
      /* write a single bit to SIG_SDA */
      if (t_value & 0x80)  /* bit == 1 */
        td |= SIG_SDA;
      else              /* bit == 0 */
        td &=  ~ SIG_SDA;

      /* write content to display */
      SDCONN_write(dd->sdcd, td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      sdtools_nsleep(dd->delay);

      /* set clock to high (bit is read on rising edge) */
      SDCONN_write(dd->sdcd, td | SIG_SCLK, dd->sdcd->io_flags_writecmd);
      sdtools_nsleep(dd->delay);

      /* set clock to low again */
      /* 
      SDCONN_write(dd->sdcd, td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      sdtools_nsleep(dd->delay);
      */

      /* shift byte so that next bit is on the first (MSB) position */
      t_value = (t_value & 0x7f) << 1;
   }
    /* commit data/command byte: */
    /* SIG_CS -> high; set active low CS (will be auto-inverted) */
    td &=  ~ SIG_CS;

    SDCONN_write(dd->sdcd, td, dd->sdcd->io_flags_writecmd);
    sdtools_nsleep(dd->delay);
  } else {   /* parallel mode : 6800 or 8080 */
    long item_split = 0;
    long td_clk1 = 0;
    long td_clk2 = 0;
    long td_clk3 = 0;

    /* active-low signals are internally seen active-high because they will be auto-inverted later if needed */
    if (serdisp_ssdoled_internal_getStruct(dd)->interfacemode == INTERFACE_6800) {
      td_clk1 = SIG_WR;
      td_clk2 = 0;
      td_clk3 = SIG_WR;

      if (SIG_CS) {
        /*td_clk1 |= SIG_CS;*/
        td_clk2 |= SIG_CS;
        td_clk3 |= SIG_CS;
      }
    } else {
      td_clk1 = 0;
      td_clk2 = SIG_WR;
      td_clk3 = 0;

      if (SIG_CS) {
        td_clk1 |= SIG_CS;
        td_clk2 |= SIG_CS;
        td_clk3 |= SIG_CS;
      }
    }

    if (!iscmd) {
      td_clk1 |= SIG_DC;
      td_clk2 |= SIG_DC;
      td_clk3 |= SIG_DC;
    }

    for (i = 0; i < 8; i++)
      if (item & (1 << i))
         item_split |= dd->sdcd->signals[i];

    td_clk2 |= item_split;

    SDCONN_write(dd->sdcd, td_clk1, dd->sdcd->io_flags_writecmd);
    sdtools_nsleep(dd->delay);
    SDCONN_write(dd->sdcd, td_clk2, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
    sdtools_nsleep(dd->delay);
    if (serdisp_ssdoled_internal_getStruct(dd)->sloppysignal == 0) {
      SDCONN_write(dd->sdcd, td_clk3, dd->sdcd->io_flags_writecmd);
      sdtools_nsleep(dd->delay);
    }
  }
}


void serdisp_ssdoled_writecmd(serdisp_t* dd, byte cmd) {
  serdisp_ssdoled_transfer(dd, 1, cmd);
}


void serdisp_ssdoled_writedata(serdisp_t* dd, byte data) {
  serdisp_ssdoled_transfer(dd, 0, data);
}


/* callback-function for setting non-standard options */
static void* serdisp_ssdoled_getvalueptr (serdisp_t* dd, const char* optionname, int* typesize) {
  if (serdisp_compareoptionnames(dd, optionname, "INTERFACE")) {
    *typesize = sizeof(int);
    return &(serdisp_ssdoled_internal_getStruct(dd)->interfacemode);
  } else if (serdisp_compareoptionnames(dd, optionname, "SLOPPYSIGNAL")) {
    *typesize = sizeof(int);
    return &(serdisp_ssdoled_internal_getStruct(dd)->sloppysignal);
  } else if (serdisp_compareoptionnames(dd, optionname, "GSTABLECORR")) {
    *typesize = sizeof(int);
    return &(serdisp_ssdoled_internal_getStruct(dd)->gstablecorr);
  }
  return 0;
}

/* main functions */


/* *********************************
   serdisp_t* serdisp_ssdoled_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle (not used in here)
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_ssdoled_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "serdisp_ssdoled_setup(): cannot allocate display descriptor");
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));

  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_ssdoled_specific_t)) )) {
    free(dd);
    return (serdisp_t*)0;
  }

  memset(dd->specific_data, 0, sizeof(serdisp_ssdoled_specific_t));

  /* "SSDxxxx"-based displays supported in here */
  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("OLED96X36X1", dispname))
    dd->dsp_id = DISPID_OLED96X36X1;
  else if (serdisp_comparedispnames("OLED96X64X16", dispname))
    dd->dsp_id = DISPID_OLED96X64X16;
  else if (serdisp_comparedispnames("OLED128X64X4", dispname))
    dd->dsp_id = DISPID_OLED128X64X4;
  else if (serdisp_comparedispnames("BL160128A", dispname))
    dd->dsp_id = DISPID_BL160128A;
  else if (serdisp_comparedispnames("4DOLED282815", dispname))
    dd->dsp_id = DISPID_4DOLED282815;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_ssdoled.c", dispname);
    return (serdisp_t*)0;
  }

  /* default setup for function pointers */
  dd->fp_init           = &serdisp_ssdoled_init;
  dd->fp_update         = &serdisp_ssdoled_update;
  dd->fp_close          = &serdisp_ssdoled_close;
  dd->fp_setoption      = &serdisp_ssdoled_setoption;
  dd->fp_getvalueptr    = &serdisp_ssdoled_getvalueptr;


  /* per display settings */

  dd->width             = 96;
  dd->height            = 36;
  dd->depth             = 1;
  dd->feature_contrast  = 1;
  dd->feature_backlight = 0;
  dd->feature_invert    = 1;

  dd->min_contrast      = 0;
  dd->max_contrast      = 0x3F;

  dd->startxcol         = 36;

  /* max. delta for optimised update algorithm */
  dd->optalgo_maxdelta  = 3;

  dd->delay = 0;
  serdisp_ssdoled_internal_getStruct(dd)->gstablecorr = 100; /* default grey scale table */

  if (dd->dsp_id == DISPID_OLED96X64X16) {
    dd->height            = 64;
    dd->depth             = 16;
    dd->colour_spaces     = SD_CS_SELFEMITTING | SD_CS_BGR | SD_CS_RGB565 | SD_CS_RGB332;
    dd->min_contrast      = 0;
    dd->max_contrast      = 0x0F;  
    dd->dsparea_width     = 20100;     /* according to datasheet */
    dd->dsparea_height    = 13400;

    dd->fp_update         = &serdisp_oledcolour_cc_update;
    dd->optalgo_maxdelta  = 6;
  } else if (dd->dsp_id == DISPID_OLED128X64X4) {
    dd->width             = 128;
    dd->height            = 64;
    dd->depth             = 4;
    dd->colour_spaces     = SD_CS_SELFEMITTING | SD_CS_GREYSCALE;
    dd->min_contrast      = 0x00; /*0x10; */
    dd->max_contrast      = 0x7F; /*0x3F; */
    dd->dsparea_width     = 61400;     /* according to datasheet */
    dd->dsparea_height    = 30700;

    dd->fp_update         = &serdisp_oled128x64x4_update;
#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
    dd->fp_setsdpixel     = &sdtools_generic_setsdpixel_greyhoriz;
    dd->fp_getsdpixel     = &sdtools_generic_getsdpixel_greyhoriz;
#else
    dd->fp_setpixel       = &sdtools_generic_setpixel_greyhoriz;
    dd->fp_getpixel       = &sdtools_generic_getpixel_greyhoriz;
#endif
    dd->optalgo_maxdelta  = 6;
  } else if (dd->dsp_id == DISPID_BL160128A) {
    dd->width             = 160;
    dd->height            = 128;
    dd->depth             = 18;
    dd->colour_spaces     = SD_CS_SELFEMITTING | SD_CS_RGB565 | SD_CS_RGB332 | SD_CS_RGB666;
    dd->min_contrast      = 0;
    dd->max_contrast      = 0x0F;
    dd->dsparea_width     = 37015;     /* according to datasheet */
    dd->dsparea_height    = 30012;

    dd->fp_clear          = &serdisp_ssdoled_clear;
    dd->fp_update         = &serdisp_oledcolour_cd_update;
    dd->optalgo_maxdelta  = 6;

    serdisp_ssdoled_internal_getStruct(dd)->gstablecorr = 200; /* contrast too high with default 100 */
  } else if (dd->dsp_id == DISPID_4DOLED282815) {
    dd->width             = 128;
    dd->height            = 128;
    dd->depth             = 18;
    dd->startycol         = 4;   /* first line on display: 4 */
    dd->colour_spaces     = SD_CS_SELFEMITTING | SD_CS_RGB565 | SD_CS_RGB332 | SD_CS_RGB666;
    dd->min_contrast      = 0x00;
    dd->max_contrast      = 0x0F;

    dd->fp_clear          = &serdisp_ssdoled_clear;
    dd->fp_update         = &serdisp_oledcolour_cd_update;
    dd->optalgo_maxdelta  = 6;

    serdisp_ssdoled_internal_getStruct(dd)->gstablecorr = 200; /* contrast too high with default 100 */
  }

  serdisp_ssdoled_internal_getStruct(dd)->interfacemode = INTERFACE_8080;
  serdisp_ssdoled_internal_getStruct(dd)->sloppysignal =  0; /* stay on the safe side per default */

  /* finally set some non display specific defaults */

  dd->curr_rotate       = 0;         /* unrotated display */
  dd->curr_invert       = 0;         /* display not inverted */

  /* supported output devices */
  dd->connection_types  = SERDISPCONNTYPE_PARPORT;

  if (dd->dsp_id == DISPID_OLED96X64X16) {
    serdisp_setupstructinfos(dd, serdisp_ssdoled_wiresignals, serdisp_ssdoled_wiredefs, serdisp_oled96x64x16_options);
  } else if (dd->dsp_id == DISPID_OLED128X64X4) {
    serdisp_setupstructinfos(dd, serdisp_ssdoled_wiresignals, serdisp_ssdoled_wiredefs, serdisp_oled128x64x4_options);
  } else if (dd->dsp_id == DISPID_BL160128A) {
    serdisp_setupstructinfos(dd, serdisp_ssdoled_wiresignals, serdisp_ssdoled_wiredefs, serdisp_bl160128a_options);
  } else if (dd->dsp_id == DISPID_4DOLED282815) {
    serdisp_setupstructinfos(dd, serdisp_ssdoled_wiresignals, serdisp_ssdoled_wiredefs, serdisp_4doled282815_options);
  } else {
    serdisp_setupstructinfos(dd, serdisp_ssdoled_wiresignals, serdisp_ssdoled_wiredefs, serdisp_ssdoled_options);
  }

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    free(dd->specific_data);
    free(dd);
    dd = 0;
    return (serdisp_t*)0;
  }

  if (dd->dsp_id == DISPID_BL160128A || dd->dsp_id == DISPID_4DOLED282815) {
    if ( ! (dd->depth == 8 || dd->depth == 16 || dd->depth == 18) ) {
      sd_error(SERDISP_ENOTSUP, "%s(): display depth %d not supported (supported depths are: 8, 16, 18)", __func__, dd->depth);
      free(dd->specific_data);
      free(dd);
      dd = 0;
      return (serdisp_t*)0;
    }
  }

  return dd;
}



/* *********************************
   void serdisp_ssdoled_init(dd)
   *********************************
   initialise a ssdoled-based display
   *********************************
   dd     ... display descriptor
*/
void serdisp_ssdoled_init(serdisp_t* dd) {
  if (SIG_RESET)
    SDCONN_write(dd->sdcd, SIG_RESET | ((SIG_CS) ? SIG_CS : 0),
                 dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);

  SDCONN_usleep(dd->sdcd, 5);

  SDCONN_write(dd->sdcd, (SIG_CS) ? SIG_CS : 0, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);

  SDCONN_usleep(dd->sdcd, 5);

  if (dd->dsp_id == DISPID_OLED96X36X1) {
    serdisp_ssdoled_writecmd (dd, 0x40);  /* display start line at line 0 */

    serdisp_ssdoled_writecmd (dd, 0x81);  /* contrast control */
    serdisp_ssdoled_writecmd (dd, 0x1F);

    serdisp_ssdoled_writecmd (dd, 0xA1);  /* set segment remap: column addr. 131 => SEG0 */

    serdisp_ssdoled_writecmd (dd, 0xC8);  /* set COM output scan dir: remapped mode */

    serdisp_ssdoled_writecmd (dd, 0xA4);  /* entire display on/off: normal display */

    serdisp_ssdoled_writecmd (dd, 0xA6);  /* set normal/inverse: normal display */

    serdisp_ssdoled_writecmd (dd, 0x2E);  /* deactivate hor scroll */

    serdisp_ssdoled_writecmd (dd, 0xA8);  /* set multiplex ratio */
    serdisp_ssdoled_writecmd (dd, 0x23);  /* 36-1 (36 rows) */


    serdisp_ssdoled_writecmd (dd, 0xAD);  /* set DC-DC on/off */
    serdisp_ssdoled_writecmd (dd, 0x8B);  /* -> on (POR) */

    serdisp_ssdoled_writecmd (dd, 0xAF);  /* turns on OLED panel */

    serdisp_ssdoled_writecmd (dd, 0xD3);  /* set display offset */
    serdisp_ssdoled_writecmd (dd, 0x00);  /* -> 0x00  (POR) */

    serdisp_ssdoled_writecmd (dd, 0xD5);  /* set display clock div. ratio/osci. freq. */
    serdisp_ssdoled_writecmd (dd, 0x21);

    serdisp_ssdoled_writecmd (dd, 0xD8);  /* set area colour mode */
    serdisp_ssdoled_writecmd (dd, 0x00);

    serdisp_ssdoled_writecmd (dd, 0xDA);  /* set COM pins hw config */
    serdisp_ssdoled_writecmd (dd, 0x12);

    serdisp_ssdoled_writecmd (dd, 0xDB);  /* set VCOM deselect level */
    serdisp_ssdoled_writecmd (dd, 0x20);

    serdisp_ssdoled_writecmd (dd, 0xD9);  /* set precharge period */
    serdisp_ssdoled_writecmd (dd, 0x22);
  } else if (dd->dsp_id == DISPID_OLED96X64X16) {
    serdisp_ssdoled_writecmd (dd, CMD_NOP);

    serdisp_ssdoled_writecmd (dd, 0x15);  /* set column address */
    serdisp_ssdoled_writecmd (dd, 0x00);  /* start:  0 */
    serdisp_ssdoled_writecmd (dd, dd->width-1);  /* end:   width-1 */

    serdisp_ssdoled_writecmd (dd, 0x75);  /* set row address */
    serdisp_ssdoled_writecmd (dd, 0x00);  /* start:  0 */
    serdisp_ssdoled_writecmd (dd, dd->height-1); /* end:   height-1 */

    serdisp_ssdoled_writecmd (dd, 0x81);  /* set contrast red */
    serdisp_ssdoled_writecmd (dd, 0x2F);

    serdisp_ssdoled_writecmd (dd, 0x82);  /* set contrast green */
    serdisp_ssdoled_writecmd (dd, 0x2F);

    serdisp_ssdoled_writecmd (dd, 0x83);  /* set contrast blue */
    serdisp_ssdoled_writecmd (dd, 0x2F);

    serdisp_ssdoled_writecmd (dd, 0x87);  /* set master contrast */
    serdisp_ssdoled_writecmd (dd, 0x0F);

    serdisp_ssdoled_writecmd (dd, 0xA0);  /* segement remap, colour-mode */
    serdisp_ssdoled_writecmd (dd, ((dd->depth == 16) ? 0x70 : 0x30));

    serdisp_ssdoled_writecmd (dd, 0xA1);  /* set display start line */
    serdisp_ssdoled_writecmd (dd, 0x00);

    serdisp_ssdoled_writecmd (dd, 0xA2);  /* set display offset */
    serdisp_ssdoled_writecmd (dd, 0x00);

    serdisp_ssdoled_writecmd (dd, 0xA8);  /* multiplex ratio */
    serdisp_ssdoled_writecmd (dd, dd->height-1);  /* height-1 */

    serdisp_ssdoled_writecmd (dd, 0xA4);  /* normal display (not inverted) */

    serdisp_ssdoled_writecmd (dd, 0xAE);  /* entire display off/on: off */

    serdisp_ssdoled_writecmd (dd, 0xAD);  /* set master configuration dc-dc */
    serdisp_ssdoled_writecmd (dd, 0x8E);

    serdisp_ssdoled_writecmd (dd, 0xB3);  /* set clock divide */
    serdisp_ssdoled_writecmd (dd, 0xD0);  /* auto clock */

    serdisp_ssdoled_writecmd (dd, 0xB1);  /* pre-charge */
    serdisp_ssdoled_writecmd (dd, 0x74);

    serdisp_ssdoled_writecmd (dd, 0xBE);  /* VCOMH-level */
    serdisp_ssdoled_writecmd (dd, 0x3F);

    serdisp_ssdoled_writecmd (dd, 0xBB);  /* VPa colour pre-charge red */
    serdisp_ssdoled_writecmd (dd, 0x1F);

    serdisp_ssdoled_writecmd (dd, 0xBC);  /* VPa colour pre-charge green */
    serdisp_ssdoled_writecmd (dd, 0x1F);

    serdisp_ssdoled_writecmd (dd, 0xBD);  /* VPa colour pre-charge blue */
    serdisp_ssdoled_writecmd (dd, 0x1F);

    serdisp_ssdoled_writecmd (dd, 0xB0);  /* set powersafe */
    serdisp_ssdoled_writecmd (dd, 0x00);

    serdisp_ssdoled_writecmd (dd, 0xAF);  /* entire display off/on: on */
  } else if (dd->dsp_id == DISPID_BL160128A) {
    int i, greyvalue;
    double gscorr = (double)(serdisp_ssdoled_internal_getStruct(dd)->gstablecorr) / 100.0;
    byte colour_mode = 0x00;

    serdisp_ssdoled_writecmd (dd, 0xE2);  /* sw reset */

    serdisp_ssdoled_writecmd (dd, 0xFD);  /* command lock */
    serdisp_ssdoled_writedata(dd, 0x12);  /* unlock */

    serdisp_ssdoled_writecmd (dd, 0xAE);  /* entire display off/on: off */

    serdisp_ssdoled_writecmd (dd, CMD_NOP);

    switch(dd->depth) {
      case 16:
        colour_mode = 0x40;
        break;
      case 18:
        colour_mode = 0x80;
        break;
      default:
        colour_mode = 0x00;
    }
    colour_mode |= 0x34;  /* segment remap, colour-mode = RGB */

    serdisp_ssdoled_writecmd (dd, 0xA0);  /* segement remap, colour-mode */
    serdisp_ssdoled_writedata(dd, colour_mode);

    serdisp_ssdoled_writecmd (dd, 0xA1);  /* set display start line */
    serdisp_ssdoled_writedata(dd, 0x00);

    serdisp_ssdoled_writecmd (dd, 0xA2);  /* set display offset */
    serdisp_ssdoled_writedata(dd, 0x00);

    serdisp_ssdoled_writecmd (dd, 0xA8);  /* multiplex ratio */
    serdisp_ssdoled_writedata(dd, dd->height-1);  /* height-1 */

#if 0
    serdisp_ssdoled_writecmd (dd, 0x83);  /* contrast colour A */
    serdisp_ssdoled_writedata(dd, 0x7F/*0xB4*/);

    serdisp_ssdoled_writecmd (dd, 0x82);  /* contrast colour B */
    serdisp_ssdoled_writedata(dd, 0x7F/*0x96*/);

    serdisp_ssdoled_writecmd (dd, 0x81);  /* contrast colour C */
    serdisp_ssdoled_writedata(dd, 0x7F/*0xA0*/);
#endif

    serdisp_ssdoled_writecmd (dd, 0xB8);  /* set grey scale table */
    for (i = 0; i <= 63; i++ ) {
      greyvalue = (int)(127.0 * sdtools_pow( (double)i / (double)63 , gscorr) + 0.5);
      serdisp_ssdoled_writedata(dd, greyvalue);
    }

    serdisp_ssdoled_writecmd (dd, 0xA4);  /* normal display (not inverted) */

    serdisp_ssdoled_writecmd (dd, 0xAF);  /* entire display off/on: on */
  } else if (dd->dsp_id == DISPID_4DOLED282815) {
    int i, greyvalue;
    double gscorr = (double)(serdisp_ssdoled_internal_getStruct(dd)->gstablecorr) / 100.0;
    byte colour_mode = 0x00;

    serdisp_ssdoled_writecmd (dd, CMD_NOP);

    switch(dd->depth) {
      case 16:
        colour_mode = 0x40;
        break;
      case 18:
        colour_mode = 0x80;
        break;
      default:
        colour_mode = 0x00;
    }
    colour_mode |= 0x34;  /* segment remap, colour-mode = RGB */

    serdisp_ssdoled_writecmd (dd, 0xA0);  /* segement remap, colour-mode */
    serdisp_ssdoled_writedata(dd, colour_mode);

    serdisp_ssdoled_writecmd (dd, 0xA1);  /* set display start line */
    serdisp_ssdoled_writedata(dd, 0x00);

    serdisp_ssdoled_writecmd (dd, 0xA2);  /* set display offset */
    serdisp_ssdoled_writedata(dd, 0x00);

    serdisp_ssdoled_writecmd (dd, 0xA6);  /* reset to normal display */

    serdisp_ssdoled_writecmd (dd, 0xAD);  /* set master configuration dc-dc */
    serdisp_ssdoled_writedata(dd, 0x8E);

    serdisp_ssdoled_writecmd (dd, 0xB1);  /* pre-charge */
    serdisp_ssdoled_writedata(dd, 0x74);

    serdisp_ssdoled_writecmd (dd, 0xBE);  /* VCOMH-level */
    serdisp_ssdoled_writedata(dd, 0x3F);

    serdisp_ssdoled_writecmd (dd, 0xB0);  /* set powersafe */
    serdisp_ssdoled_writedata(dd, 0x00);

    serdisp_ssdoled_writecmd (dd, 0xB8);  /* set grey scale table */
    for (i = 0; i <= 31; i++ ) {
      greyvalue = (int)(127.0 * sdtools_pow( (double)i / (double)31 , gscorr) + 0.5);
      serdisp_ssdoled_writedata(dd, greyvalue);
    }

    serdisp_ssdoled_writecmd (dd, 0xAF);  /* entire display off/on: on */
  } else if (dd->dsp_id == DISPID_OLED128X64X4) {

    serdisp_ssdoled_writecmd (dd, 0x15);  /* set column address */
    serdisp_ssdoled_writecmd (dd, 0x00);  /* start:  0 */
    serdisp_ssdoled_writecmd (dd, 63);  /* end:   128/2 - 1 */

    serdisp_ssdoled_writecmd (dd, 0x75);  /* set row address */
    serdisp_ssdoled_writecmd (dd, 0x00);  /* start:  0 */
    serdisp_ssdoled_writecmd (dd, 63);  /* end:   63 */

    serdisp_ssdoled_writecmd (dd, 0x81);  /* set contrast */
    serdisp_ssdoled_writecmd (dd, 0x33);

    serdisp_ssdoled_writecmd (dd, 0x86);  /* current range: full */

    serdisp_ssdoled_writecmd (dd, 0xA0);  /* segement remap, colour-mode */
    serdisp_ssdoled_writecmd (dd, 0x43);

    serdisp_ssdoled_writecmd (dd, 0xA1);  /* set display start line */
    serdisp_ssdoled_writecmd (dd, 0x00);

    serdisp_ssdoled_writecmd (dd, 0xA2);  /* set display offset */
    serdisp_ssdoled_writecmd (dd, 0x44);

    serdisp_ssdoled_writecmd (dd, 0xA8);  /* multiplex ratio */
    serdisp_ssdoled_writecmd (dd, /*0x4F*/ 0x3F );

    serdisp_ssdoled_writecmd (dd, 0xB2);  /* row period */
    serdisp_ssdoled_writecmd (dd, 0x7C);  /*  */

    serdisp_ssdoled_writecmd (dd, 0xA4);  /* normal display (not inverted) */

    serdisp_ssdoled_writecmd (dd, 0xB3);  /* set clock divide */
    serdisp_ssdoled_writecmd (dd, 0xF0);  /*  */

    serdisp_ssdoled_writecmd (dd, 0xB1);  /* phase length */
    serdisp_ssdoled_writecmd (dd, 0x13);

    serdisp_ssdoled_writecmd (dd, 0xBF);  /* VSL */
    serdisp_ssdoled_writecmd (dd, 0x0E);  /*  */

    serdisp_ssdoled_writecmd (dd, 0xBE);  /* VCOMH-level */
    serdisp_ssdoled_writecmd (dd, 0x0B);

    serdisp_ssdoled_writecmd (dd, 0xBC);  /* VP */
    serdisp_ssdoled_writecmd (dd, 0x18);

    serdisp_ssdoled_writecmd (dd, 0xB8);  /* grey scale table */
    serdisp_ssdoled_writecmd (dd, 0x01);
    serdisp_ssdoled_writecmd (dd, 0x11);
    serdisp_ssdoled_writecmd (dd, 0x22);
    serdisp_ssdoled_writecmd (dd, 0x32);
    serdisp_ssdoled_writecmd (dd, 0x43);
    serdisp_ssdoled_writecmd (dd, 0x54);
    serdisp_ssdoled_writecmd (dd, 0x65);
    serdisp_ssdoled_writecmd (dd, 0x76);
#if 0
    serdisp_ssdoled_writecmd (dd, 0xB8);  /* grey scale table */
    serdisp_ssdoled_writecmd (dd, 0x01);
    serdisp_ssdoled_writecmd (dd, 0x11);
    serdisp_ssdoled_writecmd (dd, 0x22);
    serdisp_ssdoled_writecmd (dd, 0x22);
    serdisp_ssdoled_writecmd (dd, 0x33);
    serdisp_ssdoled_writecmd (dd, 0x33);
    serdisp_ssdoled_writecmd (dd, 0x44);
    serdisp_ssdoled_writecmd (dd, 0x44);
#endif

    serdisp_ssdoled_writecmd (dd, 0xAF);  /* entire display off/on: on */
  }

  serdisp_ssdoled_writecmd (dd, CMD_NOP);

  sd_debug(2, "serdisp_ssdoled_init(): done with initialising");
}



/* *********************************
   void serdisp_ssdoled_update(dd)
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
void serdisp_ssdoled_update(serdisp_t* dd) {
  int x;
  int page;
  int pages = (dd->height+7)/8;


#ifdef OPT_USEOLDUPDATEALGO

  for (page = 0; page < pages; page++) {
    for(x = 0; x < dd->width; x++) {
      if (x==0) {
        serdisp_ssdoled_writecmd(dd, CMD_COLUMNLO    | (dd->startxcol & 0x0F));
        serdisp_ssdoled_writecmd(dd, CMD_COLUMNHI    | (dd->startxcol >> 4));
        serdisp_ssdoled_writecmd(dd, CMD_PAGEADDRESS | page);
      }
      serdisp_ssdoled_writedata(dd, dd->scrbuf[ dd->width * page  +  x]);
    }
  }
  
#else /* OPT_USEOLDUPDATEALGO */

  /* display is drawn using an optimising algorithm which tries to only send as few data as possible to the display */

  /* more detailed explanations of principle: 
     see serdisp_specific_optrex323.c / i2c.c / pcd8544.c / sed153x.c / sed1565.c 
  */

  int col, col_i;
  int page_set;
  int col_delta, delta;
  byte data;
  

  serdisp_ssdoled_writecmd (dd, 0x40 | 0x0);   /* display start line */

  for (page = 0; page < pages; page++) {    

    col = 0;
    page_set = 0;

    while (col < dd->width) {
      if ( dd->scrbuf_chg[ col + dd->width *(page/8)] & ( 1 << (page%8)) ) {

        col_delta = col;

        delta = 0;
        while (col_delta < dd->width - delta - 1 && delta < dd->optalgo_maxdelta) {
          if (dd->scrbuf_chg[ col_delta + 1 + delta + dd->width * (page/8)] & ( 1 << (page%8)) ) {
            col_delta += delta + 1;
            delta = 0;
          } else {
            delta++;
          }
        }

        if (!page_set) {
          serdisp_ssdoled_writecmd(dd, CMD_PAGEADDRESS | page);
          page_set = 1;
        }

        col_i = dd->startxcol + col;
        serdisp_ssdoled_writecmd(dd, CMD_COLUMNLO    | (col_i & 0x0F));
        serdisp_ssdoled_writecmd(dd, CMD_COLUMNHI    | (col_i >> 4));

        for (x = col ; x <= col_delta; x++) {

          data = dd->scrbuf[x + dd->width * page ];

          if (dd->curr_invert && !(dd->feature_invert))
            data = ~data;

          serdisp_ssdoled_writedata(dd, data);
          dd->scrbuf_chg[ x + dd->width * (page/8)] &= 0xFF - (1 << (page%8)) ;
        }

        col = col_delta+1;
      } else {
        col++;
      }

    }
  }
#endif /* OPT_USEOLDUPDATEALGO */

  /* add an extra NOP to avoid erraneous pixels when releasing parport */
  serdisp_ssdoled_writecmd(dd, CMD_NOP);
  SDCONN_commit(dd->sdcd); /* if streaming: be sure that every data is transmitted */
}


/* separate update function for colour oleds, taken from serdisp_nokcol.c */
void serdisp_oledcolour_cc_update(serdisp_t* dd) {
  int i;

#ifdef OPT_USEOLDUPDATEALGO

  serdisp_ssdoled_writecmd (dd, 0x15);  /* set column address */
  serdisp_ssdoled_writecmd (dd, 0x00);  /* start:  0 */
  serdisp_ssdoled_writecmd (dd, dd->width-1);    /* end:   width-1 */

  serdisp_ssdoled_writecmd (dd, 0x75);  /* set row address */
  serdisp_ssdoled_writecmd (dd, 0x00);  /* start:  0 */
  serdisp_ssdoled_writecmd (dd, dd->height-1);    /* end:   height-1 */

  serdisp_ssdoled_writecmd (dd, CMD_NOP);

  for (i = 0; i < dd->scrbuf_size; i++)
    serdisp_ssdoled_writedata(dd, dd->scrbuf[i]);

  serdisp_ssdoled_writecmd(dd, CMD_NOP);

#else /* OPT_USEOLDUPDATEALGO */

  /* display is drawn using an optimising algorithm which tries to only send as few data as possible to the display */

  /* more detailed explanations of principle: 
     see serdisp_specific_optrex323.c / i2c.c / pcd8544.c / sed153x.c / sed1565.c 
  */

  int col;
  byte data;
  int delta; 
  int i_delta = 0; /* i_delta - i: how many columns to transfer in one take */

  int last_col_set = -1;
  int r_transmitted = 0;

  i = 0;

  while (i < dd->width * dd->height) {
    int y_i, col_i, l;

    int y = i / dd->width;

    col = i % dd->width;

    /* first changed column-page */
    if ( dd->scrbuf_chg[(col >> 3) + y * ((dd->width + 7 ) >> 3)] & ( 1 << (col%8) )) {
      i_delta = i;

      delta = 0;
      while (i_delta < dd->width*dd->height-delta-1 && delta < dd->optalgo_maxdelta) {
        y_i = (i_delta+delta+1) / dd->width;
        col_i = (i_delta+delta+1) % dd->width;
        if ( dd->scrbuf_chg[(col_i >> 3) + y_i * ((dd->width + 7 ) >> 3)] & ( 1 << (col_i%8) )) {
          i_delta += delta+1;
          delta = 0;
        } else {
          delta++;
        }
      }

      last_col_set = -1;

      /*fprintf(stderr, "col/y=%02d/%02d (i=%4d i_delta=%4d diff: %2d)\n", col, y, i, i_delta, i_delta-i);*/

      for (l = i; l <= i_delta; l++) {
        int idx_2, idx, bitspercol;

        y_i = l / dd->width;
        col_i = l % dd->width;

        if (last_col_set == -1 || col_i < last_col_set) {  /* start correction needed for CASET */
          serdisp_ssdoled_writecmd (dd, 0x15);         /* set column address */
          serdisp_ssdoled_writecmd (dd, col_i);        /*   start column     */
          serdisp_ssdoled_writecmd (dd, dd->width-1);  /*   end column       */

          serdisp_ssdoled_writecmd (dd, 0x75);         /* set row address */
          serdisp_ssdoled_writecmd (dd, y_i);          /*   start row     */
          serdisp_ssdoled_writecmd (dd, dd->height-1); /*   end row       */

          serdisp_ssdoled_writecmd (dd, CMD_NOP);

          last_col_set = col_i;
          r_transmitted = 0;
        }

        bitspercol = (dd->depth == 18) ? 24 : dd->depth; /* for speed reasons, align depth 18 to 24 bits in screen buffer */
        idx_2 = ((col_i  + y_i * dd->width) * (bitspercol << 1)) / 8;
        idx = idx_2 >> 1;
        switch(dd->depth) {
          case 4:
          case 8:
            data =  dd->scrbuf[idx];
            serdisp_ssdoled_writedata(dd, data);
            break;
          case 16:
            data = dd->scrbuf[idx];
            serdisp_ssdoled_writedata(dd, data);
            data = dd->scrbuf[idx+1];
            serdisp_ssdoled_writedata(dd, data);
            break;
          case 18:
            data = dd->scrbuf[idx];
            serdisp_ssdoled_writedata(dd, data);
            data = dd->scrbuf[idx+1];
            serdisp_ssdoled_writedata(dd, data);
            data = dd->scrbuf[idx+2];
            serdisp_ssdoled_writedata(dd, data);
            break;
        }

        dd->scrbuf_chg[ (col_i >> 3) + y_i * ((dd->width + 7 ) >> 3)] &=  (0xFF ^ (1 << (col_i % 8)));
      }

      i = i_delta+1;

    } else {
      i++;
    }
  } /* while i < scrbuf_size */

#endif /* OPT_USEOLDUPDATEALGO */

  /* add an extra NOP to avoid erraneous pixels when releasing parport */
  serdisp_ssdoled_writecmd(dd, CMD_NOP);
  SDCONN_commit(dd->sdcd); /* if streaming: be sure that every data is transmitted */
}


/* separate update function for colour oleds, taken from serdisp_nokcol.c */
void serdisp_oledcolour_cd_update(serdisp_t* dd) {
  int i;

#ifdef OPT_USEOLDUPDATEALGO

  serdisp_ssdoled_writecmd (dd, 0x15);  /* set column address */
  serdisp_ssdoled_writedata(dd, 0x00);  /* start:  0 */
  serdisp_ssdoled_writedata(dd, dd->width-1);    /* end:   width-1 */

  serdisp_ssdoled_writecmd (dd, 0x75);  /* set row address */
  serdisp_ssdoled_writedata(dd, 0x00 + dd->startycol);  /* start:  0 */
  serdisp_ssdoled_writedata(dd, dd->height-1 + dd->startycol);    /* end:   height-1 */

  serdisp_ssdoled_writecmd (dd, 0x5C); /* write data to ram */

  for (i = 0; i < dd->scrbuf_size; i++)
    serdisp_ssdoled_writedata(dd, dd->scrbuf[i]);

  serdisp_ssdoled_writecmd(dd, CMD_NOP);

#else /* OPT_USEOLDUPDATEALGO */

  /* display is drawn using an optimising algorithm which tries to only send as few data as possible to the display */

  /* more detailed explanations of principle: 
     see serdisp_specific_optrex323.c / i2c.c / pcd8544.c / sed153x.c / sed1565.c 
  */

  int col;
  byte data;
  int delta; 
  int i_delta = 0; /* i_delta - i: how many columns to transfer in one take */

  int last_col_set = -1;
  int r_transmitted = 0;

  i = 0;

  while (i < dd->width * dd->height) {
    int y_i, col_i, l;

    int y = i / dd->width;

    col = i % dd->width;

    /* first changed column-page */
    if ( dd->scrbuf_chg[(col >> 3) + y * ((dd->width + 7 ) >> 3)] & ( 1 << (col%8) )) {
      i_delta = i;

      delta = 0;
      while (i_delta < dd->width*dd->height-delta-1 && delta < dd->optalgo_maxdelta) {
        y_i = (i_delta+delta+1) / dd->width;
        col_i = (i_delta+delta+1) % dd->width;
        if ( dd->scrbuf_chg[(col_i >> 3) + y_i * ((dd->width + 7 ) >> 3)] & ( 1 << (col_i%8) )) {
          i_delta += delta+1;
          delta = 0;
        } else {
          delta++;
        }
      }

      last_col_set = -1;

      /*fprintf(stderr, "col/y=%02d/%02d (i=%4d i_delta=%4d diff: %2d)\n", col, y, i, i_delta, i_delta-i);*/

      for (l = i; l <= i_delta; l++) {
        int idx_2, idx, bitspercol;

        y_i = l / dd->width;
        col_i = l % dd->width;

        if (last_col_set == -1 || col_i < last_col_set) {  /* start correction needed for CASET */
          serdisp_ssdoled_writecmd (dd, 0x15);         /* set column address */
          serdisp_ssdoled_writedata(dd, col_i);        /*   start column     */
          serdisp_ssdoled_writedata(dd, dd->width-1);  /*   end column       */

          serdisp_ssdoled_writecmd (dd, 0x75);         /* set row address */
          serdisp_ssdoled_writedata(dd, y_i + dd->startycol);          /*   start row     */
          serdisp_ssdoled_writedata(dd, dd->height-1 + dd->startycol); /*   end row       */

          serdisp_ssdoled_writecmd (dd, 0x5C); /* write data to ram */

          last_col_set = col_i;
          r_transmitted = 0;
        }

        bitspercol = (dd->depth == 18) ? 24 : dd->depth; /* for speed reasons, align depth 18 to 24 bits in screen buffer */
        idx_2 = ((col_i  + y_i * dd->width) * (bitspercol << 1)) / 8;
        idx = idx_2 >> 1;
        switch(dd->depth) {
          case 4:
          case 8:
            data =  dd->scrbuf[idx];
            serdisp_ssdoled_writedata(dd, data);
            break;
          case 16:
            data = dd->scrbuf[idx];
            serdisp_ssdoled_writedata(dd, data);
            data = dd->scrbuf[idx+1];
            serdisp_ssdoled_writedata(dd, data);
            break;
          case 18:
            data = dd->scrbuf[idx];
            serdisp_ssdoled_writedata(dd, data);
            data = dd->scrbuf[idx+1];
            serdisp_ssdoled_writedata(dd, data);
            data = dd->scrbuf[idx+2];
            serdisp_ssdoled_writedata(dd, data);
            break;
        }

        dd->scrbuf_chg[ (col_i >> 3) + y_i * ((dd->width + 7 ) >> 3)] &=  (0xFF ^ (1 << (col_i % 8)));
      }

      i = i_delta+1;

    } else {
      i++;
    }
  } /* while i < scrbuf_size */

#endif /* OPT_USEOLDUPDATEALGO */

  /* add an extra NOP to avoid erraneous pixels when releasing parport */
  serdisp_ssdoled_writecmd(dd, CMD_NOP);
  SDCONN_commit(dd->sdcd); /* if streaming: be sure that every data is transmitted */
}




/* separate update function for 128x64 4bit greyscale oled */
void serdisp_oled128x64x4_update(serdisp_t* dd) {
  int i;

#ifdef OPT_USEOLDUPDATEALGO

  serdisp_ssdoled_writecmd(dd, 0x15);  /* set column address */
  serdisp_ssdoled_writecmd(dd, 0x00);  /* start:  0 */
  serdisp_ssdoled_writecmd(dd, 63 );   /* end:   63 */

  serdisp_ssdoled_writecmd(dd, 0x75);  /* set row address */
  serdisp_ssdoled_writecmd(dd, 0x00);  /* start:  0 */
  serdisp_ssdoled_writecmd(dd, 63);    /* end:   63 */

  serdisp_ssdoled_writecmd(dd, CMD_NOP);

  for (i = 0; i < dd->scrbuf_size; i++)
    serdisp_ssdoled_writedata(dd, dd->scrbuf[i]);

  serdisp_ssdoled_writecmd(dd, CMD_NOP);

#else /* OPT_USEOLDUPDATEALGO */

  /* display is drawn using an optimising algorithm which tries to only send as few data as possible to the display */

  /* more detailed explanations of principle: 
     see serdisp_specific_optrex323.c / i2c.c / pcd8544.c / sed153x.c / sed1565.c 
  */

  int col;
  byte data;
  int delta; 
  int i_delta = 0; /* i_delta - i: how many columns to transfer in one take */

  int last_col_set = -1;
  int r_transmitted = 0;
  int cols = dd->width / (8 / dd->depth);

  i = 0;

  while (i < (cols * dd->height) ) {
    int y_i, col_i, l;

    int y = i / cols;

    col = i % cols;

    /* first changed column-page */
    if ( dd->scrbuf_chg[(col >> 3) + y * (( cols + 7 ) >> 3)] & ( 1 << (col%8) )) {
      i_delta = i;

      delta = 0;
      while (i_delta < cols * dd->height-delta-1 && delta < dd->optalgo_maxdelta) {
        y_i = (i_delta+delta+1) / cols;
        col_i = (i_delta+delta+1) % cols;
        if ( dd->scrbuf_chg[(col_i >> 3) + y_i * ((cols + 7 ) >> 3)] & ( 1 << (col_i%8) )) {
          i_delta += delta+1;
          delta = 0;
        } else {
          delta++;
        }
      }

      last_col_set = -1;

      /*fprintf(stderr, "col/y=%02d/%02d (i=%4d i_delta=%4d diff: %2d  cols=%d)\n", col, y, i, i_delta, i_delta-i, cols);*/

      for (l = i; l <= i_delta; l++) {
        int idx;

        y_i = l / cols;
        col_i = l % cols;

        if (last_col_set == -1 || col_i < last_col_set) {  /* start correction needed for CASET */
          serdisp_ssdoled_writecmd(dd, 0x15);         /* set column address */
          serdisp_ssdoled_writecmd(dd, col_i);        /*   start column     */
          serdisp_ssdoled_writecmd(dd, cols - 1);  /*   end column       */

          serdisp_ssdoled_writecmd(dd, 0x75);         /* set row address */
          serdisp_ssdoled_writecmd(dd, y_i);          /*   start row     */
          serdisp_ssdoled_writecmd(dd, dd->height-1); /*   end row       */
          serdisp_ssdoled_writecmd(dd, CMD_NOP);

          last_col_set = col_i;
          r_transmitted = 0;
        }

        idx = y_i * (cols >> 3) + (col_i >> 3);
        switch(dd->depth) {
          case 4:
            data =  dd->scrbuf[l];
            serdisp_ssdoled_writedata(dd, data);
            break;
        }
        dd->scrbuf_chg[ (col_i >> 3) + y_i * (( cols + 7 ) >> 3)] &=  (0xFF ^ (1 << (col_i % 8)));
      }

      i = i_delta+1;

    } else {
      i++;
    }
  } /* while i < scrbuf_size */

#endif /* OPT_USEOLDUPDATEALGO */

  /* add an extra NOP to avoid erraneous pixels when releasing parport */
  serdisp_ssdoled_writecmd(dd, CMD_NOP);
  SDCONN_commit(dd->sdcd); /* if streaming: be sure that every data is transmitted */
}





/* *********************************
   int serdisp_ssdoled_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_ssdoled_setoption(serdisp_t* dd, const char* option, long value) {

  if (dd->feature_invert && serdisp_compareoptionnames(dd, option, "INVERT") ) {
    if (value < 2) 
      dd->curr_invert = (int)value;
    else
      dd->curr_invert = (dd->curr_invert) ? 0 : 1;
    if (dd->dsp_id == DISPID_OLED96X36X1 || dd->dsp_id == DISPID_4DOLED282815)
      serdisp_ssdoled_writecmd(dd, (dd->curr_invert) ? 0xA7 : 0xA6);
    else if (dd->dsp_id == DISPID_OLED96X64X16 || dd->dsp_id == DISPID_BL160128A)
      serdisp_ssdoled_writecmd(dd, (dd->curr_invert) ? 0xA7 : 0xA4);
    else if (dd->dsp_id == DISPID_OLED128X64X4)
      serdisp_ssdoled_writecmd(dd, (dd->curr_invert) ? 0xA7 : 0xA4);
    serdisp_ssdoled_writecmd (dd, CMD_NOP);
  } else if (dd->feature_contrast && 
             (serdisp_compareoptionnames(dd, option, "CONTRAST" ) ||
              serdisp_compareoptionnames(dd, option, "BRIGHTNESS" )
             )
            )
  {
    int  dimmed_contrast;
    byte cmd_contrast = 0x81;

    if ( serdisp_compareoptionnames(dd, option, "CONTRAST" ) ) {
      dd->curr_contrast = sdtools_contrast_norm2hw(dd, (int)value);
    } else {
      dd->curr_dimming = 100 - (int)value;
    }

    dimmed_contrast = (((dd->curr_contrast - dd->min_contrast) * (100 - dd->curr_dimming)) / 100) + dd->min_contrast;

    if (dd->dsp_id == DISPID_OLED96X64X16 || dd->dsp_id == DISPID_BL160128A)
      cmd_contrast = 0x87;
    else if (dd->dsp_id == DISPID_4DOLED282815)
      cmd_contrast = 0xC7;

    serdisp_ssdoled_writecmd(dd, cmd_contrast);
    if (dd->dsp_id == DISPID_BL160128A || dd->dsp_id == DISPID_4DOLED282815) {
      serdisp_ssdoled_writedata(dd, dimmed_contrast);
    } else {
      serdisp_ssdoled_writecmd (dd, dimmed_contrast);
    }
    serdisp_ssdoled_writecmd(dd, CMD_NOP);
  } else if (serdisp_compareoptionnames(dd, option, "RAWCMD")) {
    fprintf(stderr, "val: 0x%02x\n", (byte)(0xFF & value));
    serdisp_ssdoled_writecmd (dd, (byte)(0xFF & value));
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  return 1;
}


/* *********************************
   void serdisp_ssdoled_clear(dd)
   *********************************
   clear display
   *********************************
   dd     ... display descriptor
*/
void serdisp_ssdoled_clear(serdisp_t* dd) {
  /* to be used with BL160128A and 4DOLED282815 only! */
  serdisp_ssdoled_writecmd (dd, (dd->dsp_id == DISPID_BL160128A) ? 0x25 : 0x8E);
  serdisp_ssdoled_writedata(dd, 0);
  serdisp_ssdoled_writedata(dd, dd->startycol);
  serdisp_ssdoled_writedata(dd, dd->width-1);
  serdisp_ssdoled_writedata(dd, dd->height-1 + dd->startycol);

  serdisp_ssdoled_writecmd(dd, CMD_NOP);
}


/* *********************************
   void serdisp_ssdoled_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_ssdoled_close(serdisp_t* dd) {
  serdisp_ssdoled_writecmd (dd, 0xAE);     /* display off */
  serdisp_ssdoled_writecmd (dd, CMD_NOP);
  if (dd->dsp_id == DISPID_BL160128A) {
    serdisp_ssdoled_writecmd (dd, 0xFD);  /* command lock */
    serdisp_ssdoled_writedata(dd, 0x16);  /* lock */
  }
}





