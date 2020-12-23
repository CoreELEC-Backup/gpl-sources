/*
 *************************************************************************
 *
 * serdisp_specific_stv8105.c
 * routines for controlling displays controlled by stv8105 OLED-controller
 *
 * supported:
 * - Osram Pictiva 256x64x4  3.2", controller STV8105
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

/* #define OPT_NOISEFIX */


/*
 * values used mainly for initialization
 */


#define INTERFACE_PARALLEL  0
#define INTERFACE_SPI       1


/* define shorts for signals to make coding simpler. must match order of serdisp_stv8105_wiresignals[] */
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
#define SIG_RESET       (dd->sdcd->signals[11])


/* different display types/models supported by this driver */
#define DISPID_OLED256X64X4  1


serdisp_wiresignal_t serdisp_stv8105_wiresignals[] = {
 /*  type   signame   actlow   cord  index */
   {SDCT_PP, "CS",         1,   'C',    8 }
  ,{SDCT_PP, "DC",         0,   'C',    9 }
  ,{SDCT_PP, "WR",         1,   'C',   10 }
  ,{SDCT_PP, "RESET",      1,   'D',   11 }
  /* I2C or SPI: */
  ,{SDCT_PP, "SCLK",       0,   'C',    0 }
  ,{SDCT_PP, "SDA",        0,   'D',    1 }
};

/*wirings */
serdisp_wiredef_t serdisp_stv8105_wiredefs[] = {
   {  0, SDCT_PP, "Original",      "DATA8,CS:nSELIN,DC:INIT,WR:nAUTO", "Original wiring"}
  ,{  1, SDCT_PP, "OriginalSWRes", "DATA8,CS:nSELIN,DC:INIT,RESET:nAUTO,WR:nSTRB", "Original wiring w/ software reset"}
  ,{  2, SDCT_PP, "SPI",           "SCLK:D0,SDA:D1,CS:D2,DC:D3",                "simple SPI wiring"}
};

serdisp_options_t serdisp_stv8105_options[] = {
   /*  name          aliasnames   min    max mod int defines  */
   {  "DEPTH",       "COLS,COLOURS",1,     4,  2, 0,  "MONOCHROME=1,MONO=1"}
  ,{  "DELAY",       "",            0,    -1,  1, 1,  ""}
  ,{  "CONTRAST",    "",            0,    10,  1, 1,  ""}
  ,{  "BRIGHTNESS", "",             0,   100,  1, 1,  ""}      /* brightness [0 .. 100] */
  ,{  "OPTION",      "",            1,     2,  1, 0,  ""}
  ,{  "INTERFACE",   "MODE",        0,     1,  1, 0,  "PAR=0,PARALLEL=0,SERIAL=1,SPI=1"}
};


/* internal typedefs and functions */

static void serdisp_stv8105_init        (serdisp_t*);
static void serdisp_stv8105_update      (serdisp_t*);
static int  serdisp_stv8105_setoption   (serdisp_t*, const char*, long);
static void serdisp_stv8105_close       (serdisp_t*);

static void serdisp_stv8105_update      (serdisp_t*);

static void serdisp_stv8105_transfer    (serdisp_t* dd, int iscmd, byte item);
static void serdisp_stv8105_writecmd    (serdisp_t* dd, byte cmd);
static void serdisp_stv8105_writedata   (serdisp_t* dd, byte data);


typedef struct serdisp_stv8105_specific_s {
  int  interfacemode;
  int  option;          /* power options:
                           option == 1: VPP/VCOL/VROW = 12/10/7 V; rio elegance yellow =>  75 cd/m2
                           option == 2: VPP/VCOL/VROW = 15/12/9 V; rio elegance yellow => 100 cd/m2
                         */
} serdisp_stv8105_specific_t;


static serdisp_stv8105_specific_t* serdisp_stv8105_internal_getStruct(serdisp_t* dd) {
  return (serdisp_stv8105_specific_t*)(dd->specific_data);
}


void serdisp_stv8105_transfer(serdisp_t* dd, int iscmd, byte item) {
  int i;

  if (serdisp_stv8105_internal_getStruct(dd)->interfacemode == INTERFACE_SPI) {
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
  } else {   /* parallel mode : equal to 8080-mode of ssdoled.c */
    long item_split = 0;
    long td_clk1 = 0;
    long td_clk2 = 0;
    long td_clk3 = 0;

    /* active-low signals are internally seen active-high because they will be auto-inverted later if needed */
    td_clk1 = 0;
    td_clk2 = SIG_WR;
    td_clk3 = 0;

    if (SIG_CS) {
      td_clk1 |= SIG_CS;
      td_clk2 |= SIG_CS;
      td_clk3 |= SIG_CS;
    }

    if (!iscmd) {
   /*   td_clk1 |= SIG_DC;*/
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
    SDCONN_write(dd->sdcd, td_clk3, dd->sdcd->io_flags_writecmd);
    sdtools_nsleep(dd->delay);
  }
}


void serdisp_stv8105_writecmd(serdisp_t* dd, byte cmd) {
  serdisp_stv8105_transfer(dd, 1, cmd);
}


void serdisp_stv8105_writedata(serdisp_t* dd, byte data) {
  serdisp_stv8105_transfer(dd, 0, data);
}


/* callback-function for setting non-standard options */
static void* serdisp_stv8105_getvalueptr (serdisp_t* dd, const char* optionname, int* typesize) {
  if (serdisp_compareoptionnames(dd, optionname, "INTERFACE")) {
    *typesize = sizeof(int);
    return &(serdisp_stv8105_internal_getStruct(dd)->interfacemode);
  } else if (serdisp_compareoptionnames(dd, optionname, "OPTION")) {
    *typesize = sizeof(int);
    return &(serdisp_stv8105_internal_getStruct(dd)->option);
  }
  return 0;
}

/* main functions */


/* *********************************
   serdisp_t* serdisp_stv8105_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle (not used in here)
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_stv8105_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate display descriptor", __func__);
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));

  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_stv8105_specific_t)) )) {
    free(dd);
    return (serdisp_t*)0;
  }

  memset(dd->specific_data, 0, sizeof(serdisp_stv8105_specific_t));

  /* "SSDxxxx"-based displays supported in here */
  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("OLED256X64X4", dispname))
    dd->dsp_id = DISPID_OLED256X64X4;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_stv8105.c", dispname);
    return (serdisp_t*)0;
  }

  /* default setup for function pointers */
  dd->fp_init           = &serdisp_stv8105_init;
  dd->fp_update         = &serdisp_stv8105_update;
  dd->fp_close          = &serdisp_stv8105_close;
  dd->fp_setoption      = &serdisp_stv8105_setoption;
  dd->fp_getvalueptr    = &serdisp_stv8105_getvalueptr;
#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
  dd->fp_setsdpixel     = &sdtools_generic_setsdpixel_greyhoriz;
  dd->fp_getsdpixel     = &sdtools_generic_getsdpixel_greyhoriz;
#else
  dd->fp_setpixel       = &sdtools_generic_setpixel_greyhoriz;
  dd->fp_getpixel       = &sdtools_generic_getpixel_greyhoriz;
#endif


  /* per display settings */

  dd->width             = 256;
  dd->height            = 64;
  dd->depth             = 4;
  dd->feature_contrast  = 1;
  dd->feature_backlight = 0;
  dd->feature_invert    = 1;

  dd->colour_spaces     = /* SD_CS_SELFEMITTING |*/ SD_CS_GREYSCALE;

  dd->min_contrast      = 0;
  dd->max_contrast      = 0x1F;

  dd->dsparea_width     = 79330;     /* according to datasheet */
  dd->dsparea_height    = 19810;

  /* max. delta for optimised update algorithm */
  dd->optalgo_maxdelta  = 3;

  dd->delay = 0;

  serdisp_stv8105_internal_getStruct(dd)->interfacemode = INTERFACE_PARALLEL;
  serdisp_stv8105_internal_getStruct(dd)->option        = 1;   /* VPP = 12V, VCOL = 10V, VROW = 7V */

  /* finally set some non display specific defaults */

  dd->curr_rotate       = 0;         /* unrotated display */
  dd->curr_invert       = 0;         /* display not inverted */

  /* supported output devices */
  dd->connection_types  = SERDISPCONNTYPE_PARPORT;

  serdisp_setupstructinfos(dd, serdisp_stv8105_wiresignals, serdisp_stv8105_wiredefs, serdisp_stv8105_options);

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    free(dd->specific_data);
    free(dd);
    dd = 0;
    return (serdisp_t*)0;
  }

  /* depth==1 is a special case. don't use flag SD_CS_SELFEMITTING */
  if (dd->depth > 1)
    dd->colour_spaces     |= SD_CS_SELFEMITTING;

  return dd;
}



/* *********************************
   void serdisp_stv8105_init(dd)
   *********************************
   initialise a stv8105-based display
   *********************************
   dd     ... display descriptor
*/
void serdisp_stv8105_init(serdisp_t* dd) {
  if (SIG_RESET) {
    SDCONN_write(dd->sdcd, SIG_RESET, dd->sdcd->io_flags_writecmd);
    SDCONN_usleep(dd->sdcd, 5);
    SDCONN_write(dd->sdcd, 0,         dd->sdcd->io_flags_writecmd);
  }

  serdisp_stv8105_writecmd (dd, 0xF2);  /* software reset */
  serdisp_stv8105_writecmd (dd, 0xF2);  /* software reset */

  serdisp_stv8105_writecmd (dd, 0x00);  /* SCLKDIV, clock divider ratio */
  serdisp_stv8105_writecmd (dd, 0x00);  /*  1/1 (120 Hz) */

  serdisp_stv8105_writecmd (dd, 0x01);  /* BRIGHTA, initial brightness adjustment A */
  if (serdisp_stv8105_internal_getStruct(dd)->option == 1)
    serdisp_stv8105_writecmd (dd, 0xA0);  /* recommended value for option 1 */
  else
    serdisp_stv8105_writecmd (dd, 0xC9);  /* recommended value for option 2 */

  serdisp_stv8105_writecmd (dd, 0x02);  /* BRIGHTB, initial brightness adjustment B */
  serdisp_stv8105_writecmd (dd, 0x00);  /*  not used */

  serdisp_stv8105_writecmd (dd, 0x03);  /* DCDF0TRL, DC/DC step up conv. contr. */
  serdisp_stv8105_writecmd (dd, 0x00);  /*  disable internal DC/DC */

  serdisp_stv8105_writecmd (dd, 0x06);  /* VFDETVAL, volt. adj. */
  serdisp_stv8105_writecmd (dd, 0x00);  /*  not used */

  serdisp_stv8105_writecmd (dd, 0x10);  /* DCTRL - dot-matrix control */
  serdisp_stv8105_writecmd (dd, 0x01);  /*  display on */

  serdisp_stv8105_writecmd (dd, 0x11);  /* DOTMTRXDIR, dot matrix display direction */
  serdisp_stv8105_writecmd (dd, 0x00);  /*  no column/row remap */
  serdisp_stv8105_writecmd (dd, 0x12);  /* DOTMTRXSCAN, dot matrix scan line select */
  serdisp_stv8105_writecmd (dd, 0x3F);  /*  scan line = line 64 */

  if (dd->depth == 1) {
    serdisp_stv8105_writecmd (dd, 0x15);  /* GSADDINC, greyscale mode, ram address increment modes */
    serdisp_stv8105_writecmd (dd, 0x83);  /*  x, y, autoincr. on, monochrome mode, picture 1 */

    serdisp_stv8105_writecmd (dd, 0x2D);  /* ODD 1, element 1 in greyscale table */
    serdisp_stv8105_writecmd (dd, 0xFF);  /*  monochrome mode */
  } else if (dd->depth == 2) {
    serdisp_stv8105_writecmd (dd, 0x15);  /* GSADDINC, greyscale mode, ram address increment modes */
    serdisp_stv8105_writecmd (dd, 0x23);  /*  x, y, autoincr. on, depth = 2, picture 1 */

                                          /* no description for depth=2 -> trial & error */
    serdisp_stv8105_writecmd (dd, 0x2B);  /* ODD 3, element 3 in greyscale table */
    serdisp_stv8105_writecmd (dd, 0xFF);  /*  trial & error */
    serdisp_stv8105_writecmd (dd, 0x2C);  /* ODD 2, element 2 in greyscale table */
    serdisp_stv8105_writecmd (dd, 0x3F);  /*  trial & error */
    serdisp_stv8105_writecmd (dd, 0x2D);  /* ODD 1, element 1 in greyscale table */
    serdisp_stv8105_writecmd (dd, 0x0F);  /*  trial & error */
  } else {
    serdisp_stv8105_writecmd (dd, 0x15);  /* GSADDINC, greyscale mode, ram address increment modes */
    serdisp_stv8105_writecmd (dd, 0x03);  /*  x, y, autoincr. on, depth = 4 */
  }


#if 0  /* will be set afterwards by serdisp_control.c */
  serdisp_stv8105_writecmd (dd, 0x16);  /* DIMMERCTRL, dimmer control */
  serdisp_stv8105_writecmd (dd, 0x0B);  /* recommended */
#endif

  serdisp_stv8105_writecmd (dd, 0x17);  /* ROWDRVSEL, row driver mode selection */
  serdisp_stv8105_writecmd (dd, 0x02);  /*  internal row driver, single scanning, no row remap */
  serdisp_stv8105_writecmd (dd, 0x1A);  /* column output control */
  serdisp_stv8105_writecmd (dd, 0x00);  /*  disable column output control */

  serdisp_stv8105_writecmd (dd, 0x1B);  /* OELPERIOD1, setup period 1 */
  serdisp_stv8105_writecmd (dd, 0x0F);  /*  recommended value */

  serdisp_stv8105_writecmd (dd, 0x1C);  /* OELPERIOD2, setup period 2 */
  serdisp_stv8105_writecmd (dd, 0x81);  /*  recommended value */

  serdisp_stv8105_writecmd (dd, 0x1D);  /* OELPERIOD3, setup period 3 */
  serdisp_stv8105_writecmd (dd, 0x81);  /*  recommended value */

  serdisp_stv8105_writecmd (dd, 0x1E);  /* OELPERIOD4, setup period 4 */
  serdisp_stv8105_writecmd (dd, 0x89);  /*  recommended value */

  sd_debug(2, "%s(): done with initialising", __func__);
}



/* *********************************
   void serdisp_stv8105_update(dd)
   *********************************
   updates the display using display-buffer scrbuf+scrbuf_chg
   *********************************
   dd     ... display descriptor
   *********************************

   the display is redrawn using a time-saving algorithm
*/
void serdisp_stv8105_update(serdisp_t* dd) {
  int i;
  byte data;

#ifdef OPT_USEOLDUPDATEALGO
  int j;

  for (j = 0 ; j < dd->height; j++) {
    serdisp_stv8105_writecmd (dd, 0x13); /* RAMSXTART, start x */
    serdisp_stv8105_writecmd (dd, 0x00);

    serdisp_stv8105_writecmd (dd, 0x14); /* RAMYSTART, start y */
    serdisp_stv8105_writecmd (dd, j);

    for (i = 0; i < dd->width / (8 / dd->depth); i++) {
      int idx = i + j * (dd->width / (8 / dd->depth));
      data = dd->scrbuf[ idx ];
      if (dd->depth == 1)
        data = sdtools_reversebits(data);
      serdisp_stv8105_writedata(dd, data);
    }
  }

#else /* OPT_USEOLDUPDATEALGO */

  /* display is drawn using an optimising algorithm which tries to only send as few data as possible to the display */

  /* more detailed explanations of principle: 
     see serdisp_specific_optrex323.c / i2c.c / pcd8544.c / sed153x.c / sed1565.c 
  */

  int line;
  int line_set;
  int i_delta, delta;
  int x;
  int cols = (dd->width * dd->depth ) >> 3;  /* == dd->width / ( 8 / dd->depth) */


  for (line = 0; line < dd->height; line++) { 

    i = 0;
    line_set = 0;

    while (i < cols) {
      if ( dd->scrbuf_chg[ line * (cols >> 3) + (i >> 3) ] & ( 1 << (i%8)) ) {

        i_delta = i;

        delta = 0;
        while (i_delta < cols - delta - 1 && delta < dd->optalgo_maxdelta) {
          if (dd->scrbuf_chg[ line * (cols >> 3) + ((i_delta+1) >> 3)  ] & ( 1 << ( (i_delta+1) %8)) ) {
            i_delta += delta + 1;
            delta = 0;
          } else {
            delta++;
          }
        }

        serdisp_stv8105_writecmd (dd, 0x13); /* RAMSXTART, start x */
        serdisp_stv8105_writecmd (dd, i);

        if (!line_set) {
          serdisp_stv8105_writecmd (dd, 0x14); /* RAMYSTART, start y */
          serdisp_stv8105_writecmd (dd, line);

          line_set = 1;
        }

        for (x = i ; x <= i_delta; x++) {

          data = dd->scrbuf[ x + line * cols ];

          if (dd->curr_invert && !(dd->feature_invert))
            data = ~data;

          if (dd->depth == 1)
            data = sdtools_reversebits(data);

          serdisp_stv8105_writedata(dd, data);
          dd->scrbuf_chg[ line * (cols >> 3) + (x >> 3)] &= 0xFF - (1 << (x%8)) ;
        }

        i = i_delta+1;
      } else {
        i++;
      }

    }
  }
#endif /* OPT_USEOLDUPDATEALGO */

#ifdef OPT_NOISEFIX
  serdisp_stv8105_writecmd (dd, 0x00);  /* SCLKDIV , clock divider ratio */
  serdisp_stv8105_writecmd (dd, 0x00);  /* 1/1 (120 Hz) */
#endif

  SDCONN_commit(dd->sdcd); /* if streaming: be sure that every data is transmitted */
}






/* *********************************
   int serdisp_stv8105_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_stv8105_setoption(serdisp_t* dd, const char* option, long value) {

  if (dd->feature_invert && serdisp_compareoptionnames(dd, option, "INVERT") ) {
    if (value < 2) 
      dd->curr_invert = (int)value;
    else
      dd->curr_invert = (dd->curr_invert) ? 0 : 1;
    serdisp_stv8105_writecmd(dd, 0x10);   /* DCTRL, dot-matrix display control */
    serdisp_stv8105_writecmd(dd, (dd->curr_invert) ? 0x05 : 0x01);
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

    serdisp_stv8105_writecmd(dd, 0x16);   /* DIMMERCTRL, dimmmer control */
    serdisp_stv8105_writecmd(dd, dimmed_contrast /*dd->curr_contrast*/ );
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  return 1;
}



/* *********************************
   void serdisp_stv8105_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_stv8105_close(serdisp_t* dd) {
  serdisp_stv8105_writecmd(dd, 0x10);     /* DCTRL, dot-matrix display control */
  serdisp_stv8105_writecmd(dd, 0x00);     /*  display off */
}





