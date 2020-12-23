/*
 *************************************************************************
 *
 * serdisp_specific_t6963.c
 * routines for controlling displays with built-in controller t6963
 *
 *************************************************************************
 *
 * copyright (C) 2003-2010  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
 *
 *************************************************************************
 *
 *  signaling sequence for serial-to-parallel converter by torsten lang
 *  (http://www.vdr-portal.de/board/thread.php?postid=483590#post483590)
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
/*#include "serdisplib/serdisp_specific_t6963.h"*/
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_colour.h"


/*#define OPT_USEOLDUPDATEALGO*/

/*
 * command constants
 */

/* pointer set */
#define CMD_POINTERSET_CURSORPOINTER         0x21
#define CMD_POINTERSET_OFFSETREGISTER        0x22
#define CMD_POINTERSET_ADDRESSPOINTER        0x24
/* control word set */
#define CMD_CONTROLWORDSET_GHADDRESS         0x42
#define CMD_CONTROLWORDSET_GHAREA            0x43
/* mode set */
#define CMD_MODESET_ORCGROM                  0x80
/* display mode set */
#define CMD_DISPLAYMODE_GFXONLY              0x98
#define CMD_DISPLAYMODE_ALLOFF               0x90
/* data auto write */
#define CMD_DATAAUTO_AUTOWRITE               0xB0
#define CMD_DATAAUTO_AUTOMODERESET           0xB2
/* data write */
#define CMD_DATAWRITE_INCREMENT              0xC0


#define INTERFACE_PARALLEL 0
#define INTERFACE_SERIAL   1

/* par interface only */
#define SIG_D0          (dd->sdcd->signals[ 0])
#define SIG_D1          (dd->sdcd->signals[ 1])
#define SIG_D2          (dd->sdcd->signals[ 2])
#define SIG_D3          (dd->sdcd->signals[ 3])
#define SIG_D4          (dd->sdcd->signals[ 4])
#define SIG_D5          (dd->sdcd->signals[ 5])
#define SIG_D6          (dd->sdcd->signals[ 6])
#define SIG_D7          (dd->sdcd->signals[ 7])
#define SIG_CD          (dd->sdcd->signals[ 9])
#define SIG_WR          (dd->sdcd->signals[10])
#define SIG_RD          (dd->sdcd->signals[11])

/* ser interface only */
#define SIG_SI          (dd->sdcd->signals[14])
#define SIG_SCL         (dd->sdcd->signals[15])

/* ser and par interface, optional */
#define SIG_CE          (dd->sdcd->signals[ 8])
#define SIG_RESET       (dd->sdcd->signals[12])
#define SIG_BACKLIGHT   (dd->sdcd->signals[13])

/* different display types/models supported by this driver */
#define DISPID_T6963       1
#define DISPID_TLX1391     2
#define DISPID_T6963SERMOD 3


serdisp_wiresignal_t serdisp_t6963_wiresignals[] = {
 /*  type   signame   actlow   cord   index */
  /* parallel data bus */
   {SDCT_PP, "CD",         0,    'C',     9 }
  ,{SDCT_PP, "WR",         1,    'C',    10 }
  ,{SDCT_PP, "RD",         1,    'C',    11 }
  /* serial data transfer */
  ,{SDCT_PP, "SI",         1,    'D',    14 }
  ,{SDCT_PP, "SCL",        1,    'C',    15 }
  /* both */
  ,{SDCT_PP, "CE",         1,    'C',     8 }
  ,{SDCT_PP, "RESET",      1,    'D',    12 }
  ,{SDCT_PP, "BACKLIGHT",  0,    'D',    13 }
};


serdisp_wiredef_t serdisp_t6963_wiredefs[] = {
   {  0, SDCT_PP, "Standard", "DATA8,CE:nAUTO,CD:INIT,WR:nSTRB,RD:nSELECT", "Standard wiring (used mostly by linux programs)"}
  ,{  1, SDCT_PP, "Windows",  "DATA8,CE:nSTRB,CD:nSELECT,WR:INIT,RD:nAUTO", "Windows wiring (used by many windows programs)"}
  ,{  2, SDCT_PP, "Serial",   "SI:INIT,SCL:nSTRB,CE:nAUTO",                  "Serial wiring for display modules with built-in converter"}
  ,{  3, SDCT_PP, "SerialBG", "SI:INIT,SCL:nSTRB,CE:nAUTO,BACKLIGHT:nSELECT", "Serial wiring for display modules with built-in converter and backlight"}
  ,{  4, SDCT_PP, "StandardBG", "DATA8,CE:nAUTO,CD:INIT,WR:nSTRB,BACKLIGHT:nSELECT", "Standard wiring but with backlight support and w/o ready-check"}
  ,{  5, SDCT_PP, "WindowsBG",  "DATA8,CE:nSTRB,CD:nSELECT,WR:INIT,BACKLIGHT:nAUTO", "Windows wiring but with backlight support and w/o ready-check"}
};


serdisp_options_t serdisp_t6963_options[] = {
   /*  name       aliasnames min  max mod int defines  */
   {  "WIDTH",     "",         0, 256,  2, 0,  ""}
  ,{  "HEIGHT",    "",        16, 256, 16, 0,  ""}
  ,{  "DELAY",     "",         0,  -1,  1, 1,  ""}
  ,{  "BACKLIGHT", "",         0,   1,  1, 1,  ""}
  ,{  "CHECK",     "",         0,   1,  1, 1,  "ON=1,OFF=0,YES=1,NO=0"}
  ,{  "FONTWIDTH", "FONT",     6,   8,  2, 0,  ""}
  ,{  "INTERFACE", "MODE",     0,   1,  1, 0,  "PARALLEL=0,SERIAL=1,PAR=0,SER=1"}
};


/* internal typedef and functions */

static void serdisp_t6963_init        (serdisp_t*);
static void serdisp_t6963_update      (serdisp_t*);
static int  serdisp_t6963_setoption   (serdisp_t*, const char*, long);
static void serdisp_t6963_close       (serdisp_t*);

static void serdisp_t6963_checkready  (serdisp_t* dd);
static void serdisp_t6963_transfer    (serdisp_t* dd, int iscmd, byte item);
static void serdisp_t6963_writedata     (serdisp_t* dd, byte data1);
static void serdisp_t6963_writecmd0data (serdisp_t* dd, byte cmd);
static void serdisp_t6963_writecmd1data (serdisp_t* dd, byte cmd, byte data1);
static void serdisp_t6963_writecmd2data (serdisp_t* dd, byte cmd, byte data2, byte data1);


typedef struct serdisp_t6963_specific_s {
  int  interfacemode;
  byte checkstatus;
} serdisp_t6963_specific_t;



static serdisp_t6963_specific_t* serdisp_t6963_internal_getStruct(serdisp_t* dd) {
  return (serdisp_t6963_specific_t*)(dd->specific_data);
}


static void* serdisp_t6963_getvalueptr (serdisp_t* dd, const char* optionname, int* typesize) {
  if (serdisp_compareoptionnames(dd, optionname, "CHECK")) {
    *typesize = sizeof(byte);
    return &(serdisp_t6963_internal_getStruct(dd)->checkstatus);
  } else if (serdisp_compareoptionnames(dd, optionname, "FONTWIDTH")) {
    *typesize = sizeof(byte);
    return &(dd->scrbuf_bits_used);
  } else if (serdisp_compareoptionnames(dd, optionname, "INTERFACE")) {
    *typesize = sizeof(int);
    return &(serdisp_t6963_internal_getStruct(dd)->interfacemode);
  }
  return 0;
}


void serdisp_t6963_checkready(serdisp_t* dd) {
  /* active-low signals are internally seen active-high because they will be auto-inverted later if needed */
  long td_clk1 =          SIG_CD         ;
  long td_clk2 = SIG_CE | SIG_CD | SIG_RD;
  long td_clk3 =          SIG_CD         ;

  int cnt = 0;

  /* check, if at least one of D0, D1, or D3 is high */
  long rc;     /* returned status byte */
  long rc_check = SIG_D0 | SIG_D1 | SIG_D3;

  if (dd->feature_backlight && dd->curr_backlight) {
    td_clk1 |= SIG_BACKLIGHT;
    td_clk2 |= SIG_BACKLIGHT;
    td_clk3 |= SIG_BACKLIGHT;
  }

  do {
    SDCONN_write(dd->sdcd, td_clk1, dd->sdcd->io_flags_writecmd);
    sdtools_nsleep( (dd->delay < 2) ? 2 : dd->delay);
    SDCONN_write(dd->sdcd, td_clk2, dd->sdcd->io_flags_writecmd);
    sdtools_nsleep( (dd->delay < 2) ? 2 : dd->delay);
    rc = SDCONN_read(dd->sdcd, dd->sdcd->io_flags_readstatus);
    sdtools_nsleep( (dd->delay < 2) ? 2 : dd->delay);
    SDCONN_write(dd->sdcd, td_clk3, dd->sdcd->io_flags_writecmd);
    sdtools_nsleep(dd->delay);
    cnt ++;
  } while (  (!(rc & rc_check)) && cnt < 10);

  if (cnt > 1)   
    sd_debug(1, "serdisp_t6963_checkready(): delay too small? (cnt = %d; rc = 0x%08lx)", cnt, rc);
}

void serdisp_t6963_transfer(serdisp_t* dd, int iscmd, byte item) {
  if (serdisp_t6963_internal_getStruct(dd)->interfacemode == INTERFACE_PARALLEL) {
    long item_split = 0;
    /* active-low signals are internally seen active-high because they will be auto-inverted later if needed */
    long td_clk1 = 0;
    long td_clk2 = SIG_WR | SIG_CE;
    long td_clk3 = 0;
    long td_clk4 = 0;
    int i;

    if (iscmd) {
      td_clk1 |= SIG_CD;
      td_clk2 |= SIG_CD;
      td_clk3 |= SIG_CD;
    } else {
      td_clk4 |= SIG_CD;
    }

    for (i = 0; i < 8; i++)
      if (item & (1 << i))
         item_split |= dd->sdcd->signals[i];


    td_clk2 |= item_split;
    td_clk3 |= item_split;

    if (dd->feature_backlight && dd->curr_backlight) {
      td_clk1 |= SIG_BACKLIGHT;
      td_clk2 |= SIG_BACKLIGHT;
      td_clk3 |= SIG_BACKLIGHT;
      td_clk4 |= SIG_BACKLIGHT;
    }

    if ( serdisp_t6963_internal_getStruct(dd)->checkstatus)
      serdisp_t6963_checkready(dd);

    SDCONN_write(dd->sdcd, td_clk1, dd->sdcd->io_flags_writecmd);
    sdtools_nsleep(dd->delay);
    SDCONN_write(dd->sdcd, td_clk2, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
    sdtools_nsleep(dd->delay);
    SDCONN_write(dd->sdcd, td_clk3, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
    sdtools_nsleep(dd->delay);
    SDCONN_write(dd->sdcd, td_clk4, dd->sdcd->io_flags_writecmd);
    sdtools_nsleep(dd->delay);
  } else {  /* interfacemode == INTERFACE_SERIAL */
    long td = 0;
    long td_toggle = 0;
    int i;

    td = SIG_CE;

    if (dd->feature_backlight && dd->curr_backlight)
      td |= SIG_BACKLIGHT;

    SDCONN_write(dd->sdcd, td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
    sdtools_nsleep(dd->delay);

    for (i = 7; i >= 0; i--) {
      if (item & (1 << i))
        td = SIG_CE | SIG_SI;
      else
        td = SIG_CE;

      if (dd->feature_backlight && dd->curr_backlight)
        td |= SIG_BACKLIGHT;

      td_toggle = td | SIG_SCL;

      SDCONN_write(dd->sdcd, td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      sdtools_nsleep(dd->delay);
      SDCONN_write(dd->sdcd, td_toggle, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      sdtools_nsleep(dd->delay);
    }
    /* from torsten's patch    (RD* ignored,  WR (sclk) not relevant here)
                T6963CSetControl(CEHI | CDLO); // CD down (data)
                T6963CSetControl(CELO | CDLO); // CE down
                T6963CSetControl(CEHI | CDLO); // CE up  
       vs 
                T6963CSetControl(CEHI | CDHI); // CD up (command)
                T6963CSetControl(CELO | CDHI); // CE down
                T6963CSetControl(CEHI | CDHI); // CE up
                T6963CSetControl(CEHI | CDLO); // CD down
     */

    if (dd->feature_backlight && dd->curr_backlight)
      td = SIG_BACKLIGHT;
    else
      td = 0;

    if (iscmd) {
      SDCONN_write(dd->sdcd, SIG_CE | SIG_SI | td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      sdtools_nsleep(dd->delay);
      SDCONN_write(dd->sdcd,          SIG_SI | td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      sdtools_nsleep(dd->delay);
      SDCONN_write(dd->sdcd, SIG_CE | SIG_SI | td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      sdtools_nsleep(dd->delay);
    } else {
      SDCONN_write(dd->sdcd, SIG_CE          | td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      sdtools_nsleep(dd->delay);
      SDCONN_write(dd->sdcd,               0 | td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      sdtools_nsleep(dd->delay);
    }
    SDCONN_write(dd->sdcd,   SIG_CE          | td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
    sdtools_nsleep(dd->delay);
  }
}


void serdisp_t6963_writedata(serdisp_t* dd, byte data1) {
  serdisp_t6963_transfer(dd, 0, data1);
}

void serdisp_t6963_writecmd0data(serdisp_t* dd, byte cmd) {
  serdisp_t6963_transfer(dd, 1, cmd);
}

void serdisp_t6963_writecmd1data(serdisp_t* dd, byte cmd, byte data1) {
  serdisp_t6963_transfer(dd, 0, data1);
  serdisp_t6963_transfer(dd, 1, cmd);
}

void serdisp_t6963_writecmd2data(serdisp_t* dd, byte cmd, byte data2, byte data1) {
  serdisp_t6963_transfer(dd, 0, data1);
  serdisp_t6963_transfer(dd, 0, data2);
  serdisp_t6963_transfer(dd, 1, cmd);
}


/* main functions */


/* *********************************
   serdisp_t* serdisp_t6963_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle (not used in here)
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_t6963_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;
  int scrbuf_columns;

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "serdisp_t6963_setup(): cannot allocate display descriptor");
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));

  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_t6963_specific_t)) )) {
    free(dd);
    return (serdisp_t*)0;
  }

  memset(dd->specific_data, 0, sizeof(serdisp_t6963_specific_t));

  /* "T6963"-based displays supported in here */
  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("T6963", dispname))
    dd->dsp_id = DISPID_T6963;
  else if (serdisp_comparedispnames("TLX1391", dispname))
    dd->dsp_id = DISPID_TLX1391;
  else if (serdisp_comparedispnames("T6963SERMOD", dispname))
    dd->dsp_id = DISPID_T6963SERMOD;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_t6963.c", dispname);
    return (serdisp_t*)0;
  }

  dd->width             = 240;
  dd->height            = 128;
  dd->depth             = 1;
  dd->colour_spaces     = SD_CS_SCRBUFCUSTOM | SD_CS_GREYSCALE;
  dd->feature_contrast  = 0;
  dd->feature_invert    = 0;
  dd->curr_backlight    = 1;         /* start with backlight on */

  /* max. delta for optimised update algorithm */
  dd->optalgo_maxdelta  = 4;

  /* default: fontwidth = 8 */
  dd->scrbuf_bits_used = 8;

  serdisp_t6963_internal_getStruct(dd)->checkstatus = 0;

  /* finally set some non display specific defaults */

  dd->curr_rotate       = 0;         /* unrotated display */
  dd->curr_invert       = 0;         /* display not inverted */

  /* supported output devices */
  dd->connection_types  = SERDISPCONNTYPE_PARPORT;

  /* default interface mode */
  serdisp_t6963_internal_getStruct(dd)->interfacemode = INTERFACE_PARALLEL;

  dd->fp_init           = &serdisp_t6963_init;
  dd->fp_update         = &serdisp_t6963_update;
  dd->fp_close          = &serdisp_t6963_close;
  dd->fp_setoption      = &serdisp_t6963_setoption;

#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
  dd->fp_setsdpixel     = &sdtools_generic_setsdpixel_greyhoriz;
  dd->fp_getsdpixel     = &sdtools_generic_getsdpixel_greyhoriz;
#else
  dd->fp_setpixel       = &sdtools_generic_setpixel_greyhoriz;
  dd->fp_getpixel       = &sdtools_generic_getpixel_greyhoriz;
#endif

  dd->fp_getvalueptr    = &serdisp_t6963_getvalueptr;

  dd->delay = -1;

  serdisp_setupstructinfos(dd, serdisp_t6963_wiresignals, serdisp_t6963_wiredefs, serdisp_t6963_options);

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    free(dd->specific_data);
    free(dd);
    dd = 0;
    return (serdisp_t*)0;    
  }

  /* no status check when serial mode */
  if ( serdisp_t6963_internal_getStruct(dd)->interfacemode == INTERFACE_SERIAL) {
    serdisp_t6963_internal_getStruct(dd)->checkstatus = 0;
  }

  /* if status should not be checked AND delay not set  -> set delay to 100 */
  if ( (serdisp_t6963_internal_getStruct(dd)->checkstatus == 0) && (dd->delay == -1)) {
    dd->delay = 100;
  }
  /* status check AND delay not set -> set delay to 0 */
  if (dd->delay == -1)
    dd->delay = 0;

  scrbuf_columns = (dd->width + dd->xcolgaps + dd->scrbuf_bits_used - 1) / dd->scrbuf_bits_used;

  /* calculate (non default) screen buffer size */
  dd->scrbuf_size = sizeof(byte) * (scrbuf_columns * ( dd->height +dd->ycolgaps )); 

  /* one byte is able to store 8 change infos */
  dd->scrbuf_chg_size = sizeof(byte) * ( ((int)((scrbuf_columns + 7)/8)) * ( dd->height +dd->ycolgaps ));

  return dd;
}



/* *********************************
   void serdisp_t6963_init(dd)
   *********************************
   initialise a t6963-based display
   *********************************
   dd     ... display descriptor
*/
void serdisp_t6963_init(serdisp_t* dd) {
  byte gfx_area;
  int fontwidth = (int)(dd->scrbuf_bits_used);

  /* force de-activation of ready-check when SIG_RD not defined */
  if (!SIG_RD) {  
    serdisp_t6963_internal_getStruct(dd)->checkstatus = 0;
  }

  /* auto en/disable backlight feature depending on wiring used */
  dd->feature_backlight = (SIG_BACKLIGHT) ? 1 : 0;

  gfx_area = (dd->width + fontwidth - 1) / fontwidth;

  serdisp_t6963_writecmd0data(dd, CMD_MODESET_ORCGROM);
  serdisp_t6963_writecmd2data(dd, CMD_CONTROLWORDSET_GHADDRESS, 0, 0);
  serdisp_t6963_writecmd2data(dd, CMD_CONTROLWORDSET_GHAREA, gfx_area, gfx_area);
  serdisp_t6963_writecmd2data(dd, CMD_POINTERSET_ADDRESSPOINTER, 0, 0);
  serdisp_t6963_writecmd0data(dd, CMD_DISPLAYMODE_GFXONLY);

  sd_debug(2, "serdisp_t6963_init(): done with initialising");
}



/* *********************************
   void serdisp_t6963_update(dd)
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
void serdisp_t6963_update(serdisp_t* dd) {
  int i, col;
  int fontwidth = (int)(dd->scrbuf_bits_used);
  int max_col = (dd->width + dd->xcolgaps + fontwidth - 1) / fontwidth;
  byte data;

#ifdef OPT_USEOLDUPDATEALGO
  /* unoptimised display update (slow. all pixels are redrawn) */

  serdisp_t6963_writecmd0data(dd, CMD_MODESET_ORCGROM);
  serdisp_t6963_writecmd2data(dd, CMD_CONTROLWORDSET_GHADDRESS, 0, 0);
  serdisp_t6963_writecmd2data(dd, CMD_CONTROLWORDSET_GHAREA, 0, max_col);
  serdisp_t6963_writecmd0data(dd, CMD_DISPLAYMODE_GFXONLY);
  serdisp_t6963_writecmd2data(dd, CMD_POINTERSET_ADDRESSPOINTER, 0, 0);
  serdisp_t6963_writecmd0data(dd, CMD_DATAAUTO_AUTOWRITE);

  for (i = 0; i < dd->height; i++) {
    for (col = 0; col < max_col; col++) {
      data = dd->scrbuf [ max_col * i + col];

      if (dd->curr_invert && !(dd->feature_invert))
        data = ~data;

      serdisp_t6963_writedata(dd, data);
    }
  }
  serdisp_t6963_writecmd0data(dd, CMD_DATAAUTO_AUTOMODERESET);
#else /* OPT_USEOLDUPDATEALGO */

  /* display is drawn using an optimising algorithm which tries to only send as few data as possible to the display */

  /* more detailed explanations of principle: 
     see serdisp_specific_optrex323.c / i2c.c / pcd8544.c / sed153x.c / sed1565.c 
  */


  int status = 0; /* status == 0: opening sequence missing; 1: opening sequence was issued */
  int delta; 
  int i_delta = 0; /* i_delta - i: how many columns to transfer in one take */

  i = 0;

  while (i < dd->scrbuf_size) {
    int y_i, col_i;

    int y = i / max_col;
    col = i % max_col;

    /* first changed column-page */
    if ( dd->scrbuf_chg[((max_col + 7) / 8) * y + (col / 8)] & ( 1 << (col%8) )) {
      if (!status) {  /* start address not set already? */
        serdisp_t6963_writecmd0data(dd, CMD_MODESET_ORCGROM);
        serdisp_t6963_writecmd2data(dd, CMD_CONTROLWORDSET_GHADDRESS, 0, 0);
        serdisp_t6963_writecmd2data(dd, CMD_CONTROLWORDSET_GHAREA, 0, max_col);
        serdisp_t6963_writecmd0data(dd, CMD_DISPLAYMODE_GFXONLY);
        status = 1;
      }

      i_delta = i;

      delta = 0;
      while (i_delta < dd->scrbuf_size-delta-1 && delta < dd->optalgo_maxdelta) {
        y_i = (i_delta+delta+1) / max_col;
        col_i = (i_delta+delta+1) % max_col;

        if ( dd->scrbuf_chg[((max_col + 7) / 8) * y_i + (col_i / 8)] & ( 1 << (col_i%8) )) {
          i_delta += delta+1;
          delta = 0;
        } else {
          delta++;
        }
      }

      serdisp_t6963_writecmd2data(dd, CMD_POINTERSET_ADDRESSPOINTER, i/256, i%256);

      /* i == i_delta: only one data column to update */      
      if (i == i_delta) {
        y_i = i / max_col;
        col_i = i % max_col;

        data = dd->scrbuf [i];
        if (dd->curr_invert && !(dd->feature_invert))
          data = ~data;

        serdisp_t6963_writecmd1data(dd, CMD_DATAWRITE_INCREMENT, data);

        dd->scrbuf_chg[ ((max_col + 7) / 8) * y_i + (col_i / 8)] &=  (0xFF ^ (1 << (col_i % 8)));
        i++;
      } else {
        int l;

        serdisp_t6963_writecmd0data(dd, CMD_DATAAUTO_AUTOWRITE);

        for (l = i; l <= i_delta; l++) {
          y_i = l / max_col;
          col_i = l % max_col;
    
          data = dd->scrbuf [l];

          if (dd->curr_invert && !(dd->feature_invert))
            data = ~data;

          serdisp_t6963_writedata(dd, data);
          dd->scrbuf_chg[ ((max_col + 7) / 8) * y_i + (col_i / 8)] &=  (0xFF ^ (1 << (col_i % 8)));
        }
        serdisp_t6963_writecmd0data(dd, CMD_DATAAUTO_AUTOMODERESET);
        i = i_delta+1;
      }
    } else {
      i++;
    }
  } /* while i < scrbuf_size */
#endif /* OPT_USEOLDUPDATEALGO */

}



/* *********************************
   int serdisp_t6963_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_t6963_setoption(serdisp_t* dd, const char* option, long value) {

  if (dd->feature_backlight && serdisp_compareoptionnames(dd, option, "BACKLIGHT") ) {
    if (value < 2) 
      dd->curr_backlight = (int)value;
    else
      dd->curr_backlight = (dd->curr_backlight) ? 0 : 1;
      /* dummy command */
      serdisp_t6963_writecmd0data(dd, CMD_DISPLAYMODE_GFXONLY);
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  return 1;
}



/* *********************************
   void serdisp_t6963_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_t6963_close(serdisp_t* dd) {
  serdisp_t6963_writecmd0data(dd, CMD_DISPLAYMODE_ALLOFF);
}

