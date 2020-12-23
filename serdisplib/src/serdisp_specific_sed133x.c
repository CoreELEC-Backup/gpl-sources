/*
 *************************************************************************
 *
 * serdisp_specific_sed133x.c
 * routines for controlling displays controlled by
 *   * sed1330/sed1335/s1d13700
 *   * usb13700 usb module
 *
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
/*#include "serdisplib/serdisp_specific_sed133x.h"*/
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"
#if (SERDISP_VERSION_CODE > SERDISP_VERSION(1,97))
 #include "serdisplib/serdisp_colour.h"
#endif

#include "serdisplib/serdisp_fctptr.h"

/*#define OPT_USEOLDUPDATEALGO */


/*
 * command constants
 */

#define CMD_SYSTEMSET     0x40
#define CMD_SCROLL        0x44
#define CMD_SLEEPIN       0x53
#define CMD_DISPON        0x59
#define CMD_DISPOFF       0x58
#define CMD_CSRFORM       0x5D
#define CMD_CGRAMADR      0x5C
#define CMD_CSRDIRRIGHT   0x4C
#define CMD_CSRDIRLEFT    0x4D
#define CMD_CSRDIRUP      0x4E
#define CMD_CSRDIRDOWN    0x4F
#define CMD_HDOTSCR       0x5A
#define CMD_GREYSCALE     0x60
#define CMD_OVLAY         0x5B
#define CMD_CSRW          0x46
#define CMD_MWRITE        0x42

/*
 * values used mainly for initialization
 */


#define INTERFACE_8080      0
#define INTERFACE_6800      1


/* define shorts for signals to make coding simpler. must match order of serdisp_sed133x_wiresignals[] */
#define SIG_D0          (dd->sdcd->signals[ 0])
#define SIG_D1          (dd->sdcd->signals[ 1])
#define SIG_D2          (dd->sdcd->signals[ 2])
#define SIG_D3          (dd->sdcd->signals[ 3])
#define SIG_D4          (dd->sdcd->signals[ 4])
#define SIG_D5          (dd->sdcd->signals[ 5])
#define SIG_D6          (dd->sdcd->signals[ 6])
#define SIG_D7          (dd->sdcd->signals[ 7])
#define SIG_CS          (dd->sdcd->signals[ 8])
#define SIG_A0          (dd->sdcd->signals[ 9])
#define SIG_WR          (dd->sdcd->signals[10])
#define SIG_RD          (dd->sdcd->signals[11])
#define SIG_RESET       (dd->sdcd->signals[12])
#define SIG_BACKLIGHT   (dd->sdcd->signals[13])


/* different display types/models supported by this driver */
#define DISPID_SED133X   1
#define DISPID_S1D13700  2
#define DISPID_USB13700  3


serdisp_wiresignal_t serdisp_sed133x_wiresignals[] = {
 /*  type   signame   actlow   cord  index */
   {SDCT_PP, "CS",         1,   'C',    8 }
  ,{SDCT_PP, "A0",         0,   'C',    9 }
  ,{SDCT_PP, "WR",         1,   'C',   10 }
  ,{SDCT_PP, "RD",         1,   'C',   11 }
  ,{SDCT_PP, "RESET",      1,   'D',   12 }
  ,{SDCT_PP, "BACKLIGHT",  0,   'D',   13 }
};

/* wirings are taken from graphlcd-base-0.1.2-pre4, docs/DRIVER.sed1330 */

serdisp_wiredef_t serdisp_sed133x_wiredefs[] = {
   {  0, SDCT_PP, "Original", "DATA8,CS:nSELIN,A0:INIT,WR:nAUTO,RD:nSTRB", "Original wiring"}
  ,{  1, SDCT_PP, "PowerLCD", "DATA8,CS:nAUTO,A0:INIT,WR:nSTRB,RD:nSELIN", "PowerLCD wiring"}
  ,{  2, SDCT_PP, "LCDProc",  "DATA8,CS:nSTRB,A0:nSELIN,WR:nAUTO,RD:INIT", "LCDProc wiring"}
  ,{  3, SDCT_PP, "Tweakers", "DATA8,CS:nSTRB,A0:nSELIN,WR:INIT,RD:nAUTO", "Tweakers wiring"}
  ,{  4, SDCT_PP, "YASEDW",   "DATA8,CS:nSELIN,A0:nAUTO,WR:nSTRB,RD:INIT", "YASEDW wiring"}
};

serdisp_options_t serdisp_sed133x_options[] = {
   /*  name          aliasnames   min    max mod int defines  */
   {  "WIDTH",       "",            0,   640,  8, 0,  ""}
  ,{  "HEIGHT",      "",            0,   256,  8, 0,  ""}
  ,{  "DELAY",       "",            0,    -1,  1, 1,  ""}
  ,{  "OSCILLATOR",  "OSC,FOSC", 1000, 10000,  1, 1,  ""}                /* kHz */
  ,{  "INTERFACE",   "MODE",        0,     1,  1, 0,  "8080=0,6800=1"}
  ,{  "DUALPANEL",   "WS",          0,     1,  1, 0,  ""}
};

serdisp_options_t serdisp_s1d13700_options[] = {
   /*  name          aliasnames   min    max mod int defines  */
   {  "WIDTH",       "",            0,   640,  8, 0,  ""}
  ,{  "HEIGHT",      "",            0,   240,  8, 0,  ""}
  ,{  "DELAY",       "",            0,    -1,  1, 1,  ""}
  ,{  "OSCILLATOR",  "OSC,FOSC", 1000, 15000,  1, 1,  ""}                /* kHz */
  ,{  "INTERFACE",   "MODE",        0,     1,  1, 0,  "8080=0,6800=1"}
  ,{  "DUALPANEL",   "WS",          0,     1,  1, 0,  ""}
/*  ,{  "DEPTH",       "",            1,     4,  2, 0,  ""}*/
};

serdisp_options_t serdisp_usb13700_options[] = {
   /*  name          aliasnames   min    max mod int defines  */
   {  "WIDTH",       "",            0,   640,  8, 0,  "AUTO=0"}
  ,{  "HEIGHT",      "",            0,   240,  8, 0,  "AUTO=0"}
  ,{  "BACKLIGHT",   "",            0,     1,  1, 1,  ""}
  ,{  "DISPCLKDIV",  "DISPCLK",     0,    10,  1, 1,  "AUTO=0"}
  ,{  "FPSHIFTDIV",  "FPSHIFT",    -1,     2,  1, 1,  "AUTO=-1"}
  ,{  "TCRCRDIFF",   "TCRCR",       0,   253,  1, 0,  "AUTO=0,DEFAULT=1"}
};


/* internal typedefs and functions */

static void serdisp_sed133x_init      (serdisp_t*);
static void serdisp_sed133x_update    (serdisp_t*);
static int  serdisp_sed133x_setoption (serdisp_t*, const char*, long);
static void serdisp_sed133x_close     (serdisp_t*);

static void serdisp_usb13700_init      (serdisp_t*);
static void serdisp_usb13700_update    (serdisp_t*);
static int  serdisp_usb13700_setoption (serdisp_t*, const char*, long);
static void serdisp_usb13700_close     (serdisp_t*);

static void serdisp_sed133x_transfer   (serdisp_t* dd, int iscmd, byte item);
static void serdisp_sed133x_writecmd   (serdisp_t* dd, byte cmd);
static void serdisp_sed133x_writedata  (serdisp_t* dd, byte data);
static void serdisp_sed133x_systemset  (serdisp_t* dd, int* cr);


typedef struct serdisp_sed133x_specific_s {
  int  interfacemode;
  int  fosc;
  int  ws;
  /* the following values are used by usb13700 only */
  int  dispclkdiv;
  int  fpshiftdiv;
  int  tcrcrdiff;
  /* usb13700 only: values that are stored in the module */
  int  pre_width;
  int  pre_height;
  int  pre_dispclkdiv;
  int  pre_fpshiftdiv;
  int  pre_tcrcrdiff;
} serdisp_sed133x_specific_t;


static serdisp_sed133x_specific_t* serdisp_sed133x_internal_getStruct(serdisp_t* dd) {
  return (serdisp_sed133x_specific_t*)(dd->specific_data);
}


void serdisp_sed133x_transfer(serdisp_t* dd, int iscmd, byte item) {
  long item_split = 0;
  long td_clk1 = 0;
  long td_clk2 = 0;
  long td_clk3 = 0;
  int i;

  /* active-low signals are internally seen active-high because they will be auto-inverted later if needed */
  if (serdisp_sed133x_internal_getStruct(dd)->interfacemode == INTERFACE_6800) {
    td_clk1 = SIG_WR | SIG_RD;
    td_clk2 = SIG_WR;
    td_clk3 = SIG_WR | SIG_RD;
  } else {
    td_clk1 = 0;
    td_clk2 = SIG_WR;
    td_clk3 = 0;
  }

  if (SIG_CS) {
    td_clk1 |= SIG_CS;
    td_clk2 |= SIG_CS;
    td_clk3 |= SIG_CS;
  }

  if (iscmd) {
    td_clk1 |= SIG_A0;
    td_clk2 |= SIG_A0;
    td_clk3 |= SIG_A0;
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



void serdisp_sed133x_writecmd(serdisp_t* dd, byte cmd) {
  serdisp_sed133x_transfer(dd, 1, cmd);
}


void serdisp_sed133x_writedata(serdisp_t* dd, byte data) {
  serdisp_sed133x_transfer(dd, 0, data);
}


/* callback-function for setting non-standard options */
static void* serdisp_sed133x_getvalueptr (serdisp_t* dd, const char* optionname, int* typesize) {
  if (serdisp_compareoptionnames(dd, optionname, "OSCILLATOR")) {
    *typesize = sizeof(int);
    return &(serdisp_sed133x_internal_getStruct(dd)->fosc);
  } else if (serdisp_compareoptionnames(dd, optionname, "INTERFACE")) {
    *typesize = sizeof(int);
    return &(serdisp_sed133x_internal_getStruct(dd)->interfacemode);
  } else if (serdisp_compareoptionnames(dd, optionname, "DUALPANEL")) {
    *typesize = sizeof(int);
    return &(serdisp_sed133x_internal_getStruct(dd)->ws);
  } else if (serdisp_compareoptionnames(dd, optionname, "DISPCLKDIV")) {
    *typesize = sizeof(int);
    return &(serdisp_sed133x_internal_getStruct(dd)->dispclkdiv);
  } else if (serdisp_compareoptionnames(dd, optionname, "FPSHIFTDIV")) {
    *typesize = sizeof(int);
    return &(serdisp_sed133x_internal_getStruct(dd)->fpshiftdiv);
  } else if (serdisp_compareoptionnames(dd, optionname, "TCRCRDIFF")) {
    *typesize = sizeof(int);
    return &(serdisp_sed133x_internal_getStruct(dd)->tcrcrdiff);
  }
  return 0;
}

/* main functions */


/* *********************************
   serdisp_t* serdisp_sed133x_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle (not used in here)
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_sed133x_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;

  int scrbuf_columns;


  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "serdisp_sed133x_setup(): cannot allocate display descriptor");
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));

  /* "SED133X" and "S1D13700" based displays supported in here */
  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("SED133X", dispname)) {
    dd->dsp_id = DISPID_SED133X;
  } else if (serdisp_comparedispnames("S1D13700", dispname)) {
    dd->dsp_id = DISPID_S1D13700;
  } else if (serdisp_comparedispnames("USB13700", dispname)) {
    if ( ! SDFCTPTR_checkavail(SDFCTPTR_LIBUSB) ) {
      sd_error(SERDISP_ERUNTIME, "%s(): libusb is not loaded but is a requirement for display '%s'", __func__, dispname);
      free(dd);
      dd = 0;
      return (serdisp_t*)0;
    }
    dd->dsp_id = DISPID_USB13700;
  } else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_sed133x.c", dispname);
    free(dd);
    dd = 0;
    return (serdisp_t*)0;
  }

  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_sed133x_specific_t)) )) {
    free(dd);
    return (serdisp_t*)0;
  }
  memset(dd->specific_data, 0, sizeof(serdisp_sed133x_specific_t));

  /* per display settings */

  dd->width             = 240;
  dd->height            = 64;
  dd->depth             = 1;
  dd->feature_contrast  = 0;
  dd->feature_backlight = 0;
  dd->feature_invert    = 0;
#if (SERDISP_VERSION_CODE > SERDISP_VERSION(1,97))
  dd->colour_spaces     = SD_CS_SCRBUFCUSTOM | SD_CS_GREYSCALE;
#endif

  /* max. delta for optimised update algorithm */
  dd->optalgo_maxdelta  = 3;

  dd->delay = 0;
  serdisp_sed133x_internal_getStruct(dd)->interfacemode = INTERFACE_8080;
  serdisp_sed133x_internal_getStruct(dd)->fosc = 9600;
  serdisp_sed133x_internal_getStruct(dd)->ws = 0;

  /* finally set some non display specific defaults */

  dd->curr_rotate       = 0;         /* unrotated display */
  dd->curr_invert       = 0;         /* display not inverted */

  /* supported output devices */
  dd->connection_types  = SERDISPCONNTYPE_PARPORT;

  dd->fp_init           = &serdisp_sed133x_init;
  dd->fp_update         = &serdisp_sed133x_update;
  dd->fp_close          = &serdisp_sed133x_close;
  dd->fp_setoption      = &serdisp_sed133x_setoption;
  dd->fp_getvalueptr    = &serdisp_sed133x_getvalueptr;

#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
  dd->fp_setsdpixel     = &sdtools_generic_setsdpixel_greyhoriz;
  dd->fp_getsdpixel     = &sdtools_generic_getsdpixel_greyhoriz;
#else
  dd->fp_setpixel       = &sdtools_generic_setpixel_greyhoriz;
  dd->fp_getpixel       = &sdtools_generic_getpixel_greyhoriz;
#endif

  /* special settings for usb13700 module */
  if (dd->dsp_id == DISPID_USB13700) {
    byte buf_get_conf[64];

    if (!sdcd) {
      sd_error(SERDISP_ERUNTIME, "%s(): sdcd not initialised", __func__);
      return (serdisp_t*)0;
    }

    dd->width             = 0;  /* auto */
    dd->height            = 0;  /* auto */
    dd->feature_backlight = 1;

    dd->optalgo_maxdelta  = 40; /* 40 bytes min. difference */
    dd->curr_backlight    = 1;

    serdisp_sed133x_internal_getStruct(dd)->dispclkdiv =  0;   /* auto */
    serdisp_sed133x_internal_getStruct(dd)->fpshiftdiv = -1;   /* auto */
    serdisp_sed133x_internal_getStruct(dd)->tcrcrdiff =   0;   /* auto */

    dd->connection_types  = SERDISPCONNTYPE_PARPORT;

    dd->fp_init           = &serdisp_usb13700_init;
    dd->fp_update         = &serdisp_usb13700_update;
    dd->fp_close          = &serdisp_usb13700_close;
    dd->fp_setoption      = &serdisp_usb13700_setoption;

/*    if (sdcd)
      sdcd->conntype = SDHWT_USB13700;*/
    SDCONN_write((serdisp_CONN_t*)sdcd,              0xfe, 0);
    SDCONN_write((serdisp_CONN_t*)sdcd,                 1, 0);  /* CMD_GET_BASICCONFIGDATA */
    SDCONN_write((serdisp_CONN_t*)sdcd,                10, 0);  /* MSG_COMMAND */
    SDCONN_commit((serdisp_CONN_t*)sdcd);
    SDCONN_readstream((serdisp_CONN_t*)sdcd, buf_get_conf, 64);

    /* sometimes the first command may fail. simple workaround: send it twice ... */
    if (buf_get_conf[0] != 1 /* CMD_GET_BASICCONFIGDATA */ || buf_get_conf[1] != 20 /* MSG_REPLY */) {
      SDCONN_write((serdisp_CONN_t*)sdcd,              0xfe, 0);
      SDCONN_write((serdisp_CONN_t*)sdcd,                 1, 0);  /* CMD_GET_BASICCONFIGDATA */
      SDCONN_write((serdisp_CONN_t*)sdcd,                10, 0);  /* MSG_COMMAND */
      SDCONN_commit((serdisp_CONN_t*)sdcd);
      SDCONN_readstream((serdisp_CONN_t*)sdcd, buf_get_conf, 64);
    }

    if (buf_get_conf[0] != 1 /* CMD_GET_BASICCONFIGDATA */ || buf_get_conf[1] != 20 /* MSG_REPLY */) {
      sd_error(SERDISP_ERUNTIME, "%s(): unable to get basic config data from usb13700 module", __func__);
      return (serdisp_t*)0;
    }

    serdisp_sed133x_internal_getStruct(dd)->pre_width  = buf_get_conf[2] | (buf_get_conf[3]<<8);
    serdisp_sed133x_internal_getStruct(dd)->pre_height = buf_get_conf[4] | (buf_get_conf[5]<<8);
    serdisp_sed133x_internal_getStruct(dd)->pre_dispclkdiv = buf_get_conf[57];
    serdisp_sed133x_internal_getStruct(dd)->pre_fpshiftdiv = buf_get_conf[58];
    serdisp_sed133x_internal_getStruct(dd)->pre_tcrcrdiff = buf_get_conf[59];

    sd_debug(1, "%s():CMD_GET_BASICCONFIGDATA:", __func__);
    sd_debug(1, "%s():------------------------", __func__);
    sd_debug(1, "%s():   Width/Height: %d/%d", __func__, 
             serdisp_sed133x_internal_getStruct(dd)->pre_width,
             serdisp_sed133x_internal_getStruct(dd)->pre_height
    );
    sd_debug(1, "%s():    Device Name: \"%32s\"", __func__, &buf_get_conf[6]);
    sd_debug(1, "%s():      User Name: \"%16s\"", __func__, &buf_get_conf[38]);
    sd_debug(1, "%s():           Mode: %d", __func__, buf_get_conf[54]);
    sd_debug(1, "%s():       Firmware: %d.%d", __func__, buf_get_conf[55], buf_get_conf[56]);
    sd_debug(1, "%s():     DispClkDiv: %d", __func__, serdisp_sed133x_internal_getStruct(dd)->pre_dispclkdiv);
    sd_debug(1, "%s():     FPSHIFTDiv: %d", __func__, serdisp_sed133x_internal_getStruct(dd)->pre_fpshiftdiv);
    sd_debug(1, "%s():      TCRCRDiff: %d", __func__, serdisp_sed133x_internal_getStruct(dd)->pre_tcrcrdiff);

    if (buf_get_conf[55] == 0 && buf_get_conf[56] < 9) {
      sd_error(SERDISP_ERUNTIME, "%s(): firmware version %d.%d detected. only firmware versions >= 0.9 are supported.", 
                                 __func__, buf_get_conf[55], buf_get_conf[56]
      );
      return (serdisp_t*)0;
    }
  }


  switch (dd->dsp_id) {
    case DISPID_S1D13700:
      serdisp_setupstructinfos(dd, serdisp_sed133x_wiresignals, serdisp_sed133x_wiredefs, serdisp_s1d13700_options);
    break;
    case DISPID_USB13700:
      serdisp_setupstructinfos(dd, 0, 0, serdisp_usb13700_options);
    break;
    default:
      serdisp_setupstructinfos(dd, serdisp_sed133x_wiresignals, serdisp_sed133x_wiredefs, serdisp_sed133x_options);
  }

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    free(dd->specific_data);
    free(dd);
    dd = 0;
    return (serdisp_t*)0;
  }

  /* setup auto values for usb13700 module */
  if (dd->dsp_id == DISPID_USB13700) {
    if (dd->width <= 0)    dd->width =  serdisp_sed133x_internal_getStruct(dd)->pre_width;
    if (dd->height <= 0)   dd->height = serdisp_sed133x_internal_getStruct(dd)->pre_height;
    if (serdisp_sed133x_internal_getStruct(dd)->dispclkdiv <= 0)
       serdisp_sed133x_internal_getStruct(dd)->dispclkdiv = serdisp_sed133x_internal_getStruct(dd)->pre_dispclkdiv;
    if (serdisp_sed133x_internal_getStruct(dd)->fpshiftdiv <= -1)
       serdisp_sed133x_internal_getStruct(dd)->fpshiftdiv = serdisp_sed133x_internal_getStruct(dd)->pre_fpshiftdiv;
    if (serdisp_sed133x_internal_getStruct(dd)->tcrcrdiff <= 0)
       serdisp_sed133x_internal_getStruct(dd)->tcrcrdiff = serdisp_sed133x_internal_getStruct(dd)->pre_tcrcrdiff;
  }

  scrbuf_columns = (dd->width + 7) / 8; 

  /* calculate (non default) screen buffer size */
  dd->scrbuf_size = sizeof(byte) * (scrbuf_columns * dd->height * dd->depth ); 

  /* one byte is able to store 8 change infos */
  dd->scrbuf_chg_size = sizeof(byte) * ( ((int)((scrbuf_columns + 7)/8)) * ( dd->height * dd->depth ));

  return dd;
}


/* *********************************
   void serdisp_sed133x_systemset(dd, int* cr)
   *********************************
   initialisation of sed133x-based display / command systemset
   *********************************
   dd     ... display descriptor
   &cr    ... address where to store calculated [C/R]
   *********************************
*/
void serdisp_sed133x_systemset(serdisp_t* dd, int* cr) {
  int opt_fx = 8, opt_fy = 1, opt_ws;
  int opt_vc, opt_cr, opt_tcr, opt_ap;

  opt_vc = (dd->width + 7) / opt_fx;      /* characters per line */
  opt_cr = opt_vc * dd->depth;            /* [CR] = Round([FX] / 8) * [VC] * DEPTH */

  opt_ws = serdisp_sed133x_internal_getStruct(dd)->ws;

  opt_ap = opt_cr;   /* AP .. horizontal address range of virtual screen. == cr (depth already included) */

  /* TCR must satisfy two conditions:

     1)  fosc >= ([TCR] * 9 + 1) * [LF] *fFR    LF = lines per frame => height; fFR = 70 Hz
         =>
         [TCR] = ((fosc / (height * 70)) - 1 ) / 9

     2)  [TCR] >= [CR] + 4

  */
  opt_tcr = ((serdisp_sed133x_internal_getStruct(dd)->fosc * 1000 / (dd->height * 70)) - 1) / 9;
  if (opt_tcr < opt_cr + 4) {
    opt_tcr = opt_cr + 4;
    sd_debug(1, "%s(): fosc too low. corrected to meet condition [TRC] >= [CR] + 4", __func__);
  }

  sd_debug(2, "%s(): VC: %d, CR: %d, TCR: %d, AP: %d", __func__, opt_vc, opt_cr, opt_tcr, opt_ap);


  serdisp_sed133x_writecmd (dd, CMD_SYSTEMSET);
  /*     bit | id  |   SED133X                               |   S1D13700
         ----|-----|-----------------------------------------|-----------------------------------
          7  | DR  |  0: normal operation                    | 0 (n/a)
             |     |  1: additional shift-clock cycles       |
          6  | TL  |  0: LCD-mode                            | 0 (n/a)
             |     |  1: TV-mode                             |
          5  | IV  |  0: screen top-line correction  1: no screen top-line correction
          4  | -   |  1 (reserved)
          3  | WS  |  0: single-panel                1: dual-panel drive
          2  | M2  |  0: 8-pixel                     1: 16-pixel character height
          1  | M1  |  0: CG RAM1/32 char                     | 0 (reserved)
             |     |  1: CG RAM1+RAM2/64 char                |
          0  | M0  |  0: internal CG ROM             1: external CG ROM
   */
  serdisp_sed133x_writedata(dd, 0x30 | (opt_ws<<3) );                /* 0|0|IV=1|1|ws|M2=0|M1=0|M0=0 */
  /*     bit | id  |   SED133X                               |   S1D13700
         ----|-----|-----------------------------------------|-----------------------------------
          7  | MOD |  0: 16-line AC drive            1: two-frame AC-drive
             | (WF)|
         6-4 | --  |  0 (n/a)
         3-0 | FX  |  horizontal character size in pixels - 1
   */
  serdisp_sed133x_writedata(dd, 0x80 | (opt_fx -1) );                /* MOD=1|0|0|0|FX=fx-1| */
  serdisp_sed133x_writedata(dd, (opt_fy-1) );                        /* [FY] - 1 */
  serdisp_sed133x_writedata(dd, (opt_cr-1) );                        /* [CR] - 1 */
  serdisp_sed133x_writedata(dd, (opt_tcr -1));                       /* [TCR] - 1 */
  serdisp_sed133x_writedata(dd, (dd->height - 1));                   /* [LF] - 1     LF  lines per frame */
  serdisp_sed133x_writedata(dd, (byte)((opt_ap & 0x00FF)));          /* AP low byte */
  serdisp_sed133x_writedata(dd, (byte)( (opt_ap & 0xFF00) >> 8 ) );  /* AP high byte */

  if (cr)
    *cr = opt_cr;
} 


/* *********************************
   void serdisp_sed133x_init(dd)
   *********************************
   initialise a sed133x-based display
   *********************************
   dd     ... display descriptor
*/
void serdisp_sed133x_init(serdisp_t* dd) {
  int opt_cr, opt_ws;
  int opt_sad = 0;                  /* start address for graphics layer */

  opt_ws = serdisp_sed133x_internal_getStruct(dd)->ws;

  if (dd->dsp_id == DISPID_S1D13700) {
    SDCONN_usleep(dd->sdcd, 10000); /* after power on: wait 10 millisecs */
    serdisp_sed133x_writecmd (dd, CMD_SYSTEMSET);   /* wake up s1d13700 from power save mode */
    SDCONN_usleep(dd->sdcd, 10000); /* after wake up: wait 10 millisecs */
  }

  serdisp_sed133x_systemset(dd, &opt_cr);

  serdisp_sed133x_writecmd (dd, CMD_DISPON);
  serdisp_sed133x_writedata(dd, 0x04);        /* SAD1 on, cursor off */

  if (dd->dsp_id == DISPID_S1D13700) {
    serdisp_sed133x_writecmd (dd, CMD_GREYSCALE);
    serdisp_sed133x_writedata(dd, dd->depth >> 1);
  }

  serdisp_sed133x_writecmd (dd, CMD_SCROLL);
  serdisp_sed133x_writedata(dd, (byte)(opt_sad & 0x00FF));
  serdisp_sed133x_writedata(dd, (byte)((opt_sad & 0xFF00) >> 8));
  serdisp_sed133x_writedata(dd, ((opt_ws) ? (dd->height >> 1) : dd->height) +1);
  serdisp_sed133x_writedata(dd, (byte)(opt_sad & 0x00FF));
  serdisp_sed133x_writedata(dd, (byte)((opt_sad & 0xFF00) >> 8));
  serdisp_sed133x_writedata(dd, ((opt_ws) ? (dd->height >> 1) : dd->height) +1);
  if (opt_ws) {
    serdisp_sed133x_writedata(dd, (byte)(opt_sad & 0x00FF));
    serdisp_sed133x_writedata(dd, (byte)((opt_sad & 0xFF00) >> 8));
  }

  serdisp_sed133x_writecmd (dd, CMD_CSRDIRRIGHT);  /* set cursor movement direction to right */

  serdisp_sed133x_writecmd (dd, CMD_HDOTSCR);
  serdisp_sed133x_writedata(dd, 0);   /* no horizontal scrolling */

  serdisp_sed133x_writecmd (dd, CMD_OVLAY);
  serdisp_sed133x_writedata(dd, 0x04);  /* DMO=1 (only one layer), MX=00 (layering: don't care) */

  sd_debug(2, "serdisp_sed133x_init(): done with initialising");
}



/* *********************************
   void serdisp_sed133x_update(dd)
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
void serdisp_sed133x_update(serdisp_t* dd) {
  int i, col;
  int max_col = ((dd->width + dd->xcolgaps + 7) / 8) * dd->depth;
  byte data;

#ifdef OPT_USEOLDUPDATEALGO
  /* unoptimised display update (slow. all pixels are redrawn) */

  serdisp_sed133x_writecmd (dd, CMD_CSRW);
  serdisp_sed133x_writedata(dd, 0x00);
  serdisp_sed133x_writedata(dd, 0x00);

  serdisp_sed133x_writecmd (dd, CMD_MWRITE);

  for (i = 0; i < dd->height; i++) {
    for (col = 0; col < max_col; col++) {
      data = dd->scrbuf [ max_col * i + col];

      /* if (dd->curr_invert && !(dd->feature_invert)) */
      if (dd->curr_invert)
        data = ~data;

      serdisp_sed133x_writedata(dd, data);
    }
  }


#else /* OPT_USEOLDUPDATEALGO */

  /* display is drawn using an optimising algorithm which tries to only send as few data as possible to the display */

  /* more detailed explanations of principle: 
     see serdisp_specific_optrex323.c / i2c.c / pcd8544.c / sed153x.c / sed1565.c 
  */


  int delta; 
  int i_delta = 0; /* i_delta - i: how many columns to transfer in one take */

  i = 0;

  while (i < dd->scrbuf_size) {
    int y_i, col_i, l;

    int y = i / max_col;
    col = i % max_col;

    /* first changed column-page */
    if ( dd->scrbuf_chg[((max_col + 7) / 8) * y + (col / 8)] & ( 1 << (col%8) )) {
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

      serdisp_sed133x_writecmd (dd, CMD_CSRW);
      serdisp_sed133x_writedata(dd, (byte)(i%256) );
      serdisp_sed133x_writedata(dd, (byte)(i/256) );


      serdisp_sed133x_writecmd (dd, CMD_MWRITE);

      for (l = i; l <= i_delta; l++) {
        y_i = l / max_col;
        col_i = l % max_col;

        data = dd->scrbuf [l];

        /* if (dd->curr_invert && !(dd->feature_invert)) */
        if (dd->curr_invert)
          data = ~data;

        serdisp_sed133x_writedata(dd, data);
        dd->scrbuf_chg[ ((max_col + 7) / 8) * y_i + (col_i / 8)] &=  (0xFF ^ (1 << (col_i % 8)));
      }

      i = i_delta+1;

    } else {
      i++;
    }
  } /* while i < scrbuf_size */
#endif /* OPT_USEOLDUPDATEALGO */

  /* add an extra NOP to avoid erraneous pixels when releasing parport */
/*  serdisp_sed133x_writecmd (dd, CMD_DISPON);*/
  serdisp_sed133x_writecmd (dd, CMD_CSRDIRRIGHT);
}



/* *********************************
   int serdisp_sed133x_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_sed133x_setoption(serdisp_t* dd, const char* option, long value) {

  if (dd->feature_backlight && serdisp_compareoptionnames(dd, option, "BACKLIGHT")) {
    if (value < 2) 
      dd->curr_backlight = (int)value;
    else
      dd->curr_backlight = (dd->curr_backlight) ? 0 : 1;
  } else if (serdisp_compareoptionnames(dd, option, "OSCILLATOR")) {
    serdisp_sed133x_internal_getStruct(dd)->fosc = value;

    serdisp_sed133x_systemset(dd, NULL);
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  return 1;
}



/* *********************************
   void serdisp_sed133x_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_sed133x_close(serdisp_t* dd) {
  serdisp_sed133x_writecmd (dd, CMD_DISPOFF);
}


/* *********************************
   void serdisp_usb13700_init(dd)
   *********************************
   initialise a display driven by the usb13700-module
   *********************************
   dd     ... display descriptor
*/
void serdisp_usb13700_init(serdisp_t* dd) {

  if ( (dd->width  != serdisp_sed133x_internal_getStruct(dd)->pre_width)  ||
       (dd->height != serdisp_sed133x_internal_getStruct(dd)->pre_height) ||
       (serdisp_sed133x_internal_getStruct(dd)->dispclkdiv != serdisp_sed133x_internal_getStruct(dd)->pre_dispclkdiv) ||
       (serdisp_sed133x_internal_getStruct(dd)->fpshiftdiv != serdisp_sed133x_internal_getStruct(dd)->pre_fpshiftdiv) ||
       (serdisp_sed133x_internal_getStruct(dd)->tcrcrdiff  != serdisp_sed133x_internal_getStruct(dd)->pre_tcrcrdiff)
     ) {
    byte buf_get_bgconf[8];
    byte buf_get_conf[64];

    sd_debug(1,"%s: calling GET_BASICCONFIGDATA:", __func__);
    sd_debug(1,"%s: W: %d <-> %d", __func__, dd->width, serdisp_sed133x_internal_getStruct(dd)->pre_width);
    sd_debug(1,"%s: H: %d <-> %d", __func__, dd->height, serdisp_sed133x_internal_getStruct(dd)->pre_height);
    sd_debug(1,"%s: D: %d <-> %d", __func__, serdisp_sed133x_internal_getStruct(dd)->dispclkdiv, serdisp_sed133x_internal_getStruct(dd)->pre_dispclkdiv);
    sd_debug(1,"%s: F: %d <-> %d", __func__, serdisp_sed133x_internal_getStruct(dd)->fpshiftdiv, serdisp_sed133x_internal_getStruct(dd)->pre_fpshiftdiv);
    sd_debug(1,"%s: T: %d <-> %d", __func__, serdisp_sed133x_internal_getStruct(dd)->tcrcrdiff, serdisp_sed133x_internal_getStruct(dd)->pre_tcrcrdiff);

    SDCONN_write(dd->sdcd,              0xfe, 0);
    SDCONN_write(dd->sdcd,                 1, 0);  /* CMD_GET_BASICCONFIGDATA */
    SDCONN_write(dd->sdcd,                10, 0);  /* MSG_COMMAND */
    SDCONN_commit(dd->sdcd);
    SDCONN_readstream(dd->sdcd, buf_get_conf, 64);

    if (buf_get_conf[0] == 1 /* CMD_GET_BASICCONFIGDATA */ && buf_get_conf[1] == 20 /* MSG_REPLY */) {

      SDCONN_write(dd->sdcd,              0xfe, 0);
      SDCONN_write(dd->sdcd,                 5, 0);  /* CMD_GET_BACKLIGHTCONFIGDATA */
      SDCONN_write(dd->sdcd,                10, 0);  /* MSG_COMMAND */
      SDCONN_commit(dd->sdcd);
      SDCONN_readstream(dd->sdcd, buf_get_bgconf, 7);
      if (buf_get_bgconf[0] == 5 /* CMD_GET_BACKLIGHTCONFIGDATA */ && buf_get_bgconf[1] == 20 /* MSG_REPLY */) {
        byte buf_get_result[4];
        int i;

        sd_debug(1, "%s():CMD_GET_BACKLIGHTCONFIGDATA:", __func__);
        sd_debug(1, "%s():----------------------------", __func__);
        sd_debug(1, "%s():    Control PWM: %d", __func__, buf_get_bgconf[2]);
        sd_debug(1, "%s():        CCFLPWM: %d", __func__, buf_get_bgconf[3]);
        sd_debug(1, "%s():      BG Config: %d", __func__, buf_get_bgconf[4]);
        sd_debug(1, "%s():  PWM Frequency: %d", __func__, buf_get_bgconf[5] | (buf_get_bgconf[6] << 8));

        SDCONN_write(dd->sdcd,                   0xfe, 0);
        SDCONN_write(dd->sdcd,                      3, 0);  /* CMD_SET_CONFIGDATA */
        SDCONN_write(dd->sdcd,                     10, 0);  /* MSG_COMMAND */
        SDCONN_write(dd->sdcd,      dd->width  & 0xff, 0);  /* [ 3] new width */
        SDCONN_write(dd->sdcd,      dd->width  >>   8, 0);  /* [ 4] */
        SDCONN_write(dd->sdcd,      dd->height & 0xff, 0);  /* [ 5] new height */
        SDCONN_write(dd->sdcd,      dd->height >>   8, 0);  /* [ 6] */
        for (i = 0; i < 16; i++)                            /* [ 7-22] 16 char user name */
          SDCONN_write(dd->sdcd,     buf_get_conf[38+i], 0);
        SDCONN_write(dd->sdcd, serdisp_sed133x_internal_getStruct(dd)->dispclkdiv & 0xff, 0); /* [23] dispclkdiv */
        SDCONN_write(dd->sdcd, serdisp_sed133x_internal_getStruct(dd)->fpshiftdiv & 0xff, 0); /* [24] fpshiftdiv */
        for (i = 0; i < 5; i++)                             /* [25-29] payload from background config data */
          SDCONN_write(dd->sdcd,     buf_get_bgconf[2+i], 0);
        SDCONN_write(dd->sdcd, serdisp_sed133x_internal_getStruct(dd)->tcrcrdiff & 0xff, 0);  /* [30] tcrcrdiff */
        SDCONN_write(dd->sdcd, serdisp_sed133x_internal_getStruct(dd)->tcrcrdiff >    8, 0);  /* [31] tcrcrdiff */
        SDCONN_commit(dd->sdcd);

        SDCONN_readstream(dd->sdcd, buf_get_result, 3);
        if (buf_get_result[0] ==  3 /* CMD_SET_CONFIGDATA */ && 
            buf_get_result[1] == 20 /* MSG_REPLY */ &&
            buf_get_result[2] ==  1 /* CMD_SUCCESS */
        ) {
          sd_debug(2, "%s(): CMD_SET_CONFIGDATA successful", __func__);
        } else {
          sd_error(SERDISP_ERUNTIME, "%s(): CMD_SET_CONFIGDATA unsuccessful", __func__);
        }
      }
    }
  }

  SDCONN_write(dd->sdcd,              0xfe, 0);
  SDCONN_write(dd->sdcd,                 7, 0);  /* CMD_WRITE_S1D13700_COMMAND */
  SDCONN_write(dd->sdcd,                10, 0);  /* MSG_COMMAND */
  SDCONN_write(dd->sdcd,        CMD_DISPON, 0);  /* display on */
  SDCONN_commit(dd->sdcd);
}


/* *********************************
   void serdisp_usb13700_update(dd)
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
void serdisp_usb13700_update(serdisp_t* dd) {
  byte data;
  int i;
  int max_col = ((dd->width + dd->xcolgaps + 7) / 8);
#ifdef OPT_USEOLDUPDATEALGO
  int col;
  /* unoptimised display update (slow. all pixels are redrawn) */

  SDCONN_write(dd->sdcd,                    0xfe, 0);
  SDCONN_write(dd->sdcd,                       2, 0);  /* CMD_WRITE_FULLSCREEN */
  SDCONN_write(dd->sdcd,                      10, 0);  /* MSG_COMMAND */
  SDCONN_write(dd->sdcd,  dd->scrbuf_size & 0xff, 0);  /* w */
  SDCONN_write(dd->sdcd,  dd->scrbuf_size >>   8, 0);  /* databytes  */

  for (i = 0; i < dd->height; i++) {
    for (col = 0; col < max_col; col++) {
      data = dd->scrbuf [ max_col * i + col];

      /* if (dd->curr_invert && !(dd->feature_invert)) */
      if (dd->curr_invert)
        data = ~data;

      SDCONN_write(dd->sdcd, (long)data, dd->sdcd->io_flags_writedata);
    }
  }

#else /* OPT_USEOLDUPDATEALGO */

  /* display is drawn using an optimising algorithm which tries to only send as few data as possible to the display */

  int x, y;
  int xt = 0, yt = 0, xb = 0, yb = 0;
  i = sdtools_calc_bbox (dd, 1, &xt, &yt, &xb, &yb);
  /*fprintf(stderr, "[%d] %3d/%3d - %3d/%3d\n", i, xt, yt, xb, yb);*/

  if (i != 0) {
    SDCONN_write(dd->sdcd,                  0xfe, 0);
    SDCONN_write(dd->sdcd,                    13, 0);  /* CMD_WRITE_AREA */
    SDCONN_write(dd->sdcd,                    10, 0);  /* MSG_COMMAND */
    SDCONN_write(dd->sdcd,             xt & 0xff, 0);  /* x */
    SDCONN_write(dd->sdcd,             xt >>   8, 0);  /*  */
    SDCONN_write(dd->sdcd,             yt & 0xff, 0);  /* y */
    SDCONN_write(dd->sdcd,             yt >>   8, 0);  /*  */
    SDCONN_write(dd->sdcd,      (xb-xt+1) & 0xff, 0);  /* w */
    SDCONN_write(dd->sdcd,      (xb-xt+1) >>   8, 0);  /*  */
    SDCONN_write(dd->sdcd,      (yb-yt+1) & 0xff, 0);  /* h */
    SDCONN_write(dd->sdcd,      (yb-yt+1) >>   8, 0);  /*  */
    SDCONN_write(dd->sdcd,                     0, 0);  /* horicontally */
    SDCONN_write(dd->sdcd,                     0, 0);  /* reserved */

    for (y = yt; y <= yb; y++) {
      for (x = xt; x <= xb; x+=8) {
        data = dd->scrbuf [ y * max_col + x/8];

        if (dd->curr_invert)
          data = ~data;

        SDCONN_write(dd->sdcd, data, 0);  /* data */
        dd->scrbuf_chg[ y * (max_col / 8) + (x / 64)] &=  (0xFF ^ (1 << ((x/8) % 8)));
      }
    }
  }
#endif /* OPT_USEOLDUPDATEALGO */

  SDCONN_commit(dd->sdcd);
}


/* *********************************
   int serdisp_usb13700_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_usb13700_setoption(serdisp_t* dd, const char* option, long value) {
  if (dd->feature_backlight && serdisp_compareoptionnames(dd, option, "BACKLIGHT")) {
    byte buf[4];

    if (value < 2) 
      dd->curr_backlight = (int)value;
    else
      dd->curr_backlight = (dd->curr_backlight) ? 0 : 1;
    SDCONN_write(dd->sdcd,              0xfe, 0);
    SDCONN_write(dd->sdcd,                 4, 0);  /* CMD_SET_BACKLIGHT */
    SDCONN_write(dd->sdcd,                10, 0);  /* MSG_COMMAND */
    SDCONN_write(dd->sdcd,dd->curr_backlight, 0);  /* on/off */
    SDCONN_write(dd->sdcd,               255, 0);  /* brightness */
    SDCONN_commit(dd->sdcd);
    SDCONN_readstream(dd->sdcd, buf, 3);
    if (!buf[2])  /* if 3rd byte from response == 0: command was unsucessful */
      sd_debug(1, "%s(): unable to switch %s background light", __func__, ((dd->curr_backlight) ? "on" : "off"));
  } else if (serdisp_compareoptionnames(dd, option, "DISPCLKDIV")) {
    byte buf[4];
    serdisp_sed133x_internal_getStruct(dd)->dispclkdiv = value;

    SDCONN_write(dd->sdcd,              0xfe, 0);
    SDCONN_write(dd->sdcd,                23, 0);  /* CMD_SET_S1D13700_CLK_DIV */
    SDCONN_write(dd->sdcd,                10, 0);  /* MSG_COMMAND */
    SDCONN_write(dd->sdcd,serdisp_sed133x_internal_getStruct(dd)->dispclkdiv, 0);  /* dispclkdiv */
    SDCONN_write(dd->sdcd,serdisp_sed133x_internal_getStruct(dd)->fpshiftdiv, 0);  /* tcrcrdiff */
    SDCONN_commit(dd->sdcd);
    SDCONN_readstream(dd->sdcd, buf, 3);

    if (buf[0] == 23 /* CMD_SET_CONFIGDATA */ && 
        buf[1] == 20 /* MSG_REPLY */ &&
        buf[2] ==  1 /* CMD_SUCCESS */
    ) {
      sd_debug(2, "%s(): successfully changed DISPCLKDIV and FPSHIFTDIV (%d / %d)", __func__,
                  serdisp_sed133x_internal_getStruct(dd)->dispclkdiv,
                  serdisp_sed133x_internal_getStruct(dd)->fpshiftdiv
      );
    } else {
      sd_debug(1, "%s(): unable to change DISPCLKDIV and FPSHIFTDIV", __func__);
    }
  } else if (serdisp_compareoptionnames(dd, option, "FPSHIFTDIV")) {
    byte buf[4];
    serdisp_sed133x_internal_getStruct(dd)->fpshiftdiv = value;

    SDCONN_write(dd->sdcd,              0xfe, 0);
    SDCONN_write(dd->sdcd,                23, 0);  /* CMD_SET_S1D13700_CLK_DIV */
    SDCONN_write(dd->sdcd,                10, 0);  /* MSG_COMMAND */
    SDCONN_write(dd->sdcd,serdisp_sed133x_internal_getStruct(dd)->dispclkdiv, 0);  /* dispclkdiv */
    SDCONN_write(dd->sdcd,serdisp_sed133x_internal_getStruct(dd)->fpshiftdiv, 0);  /* tcrcrdiff */
    SDCONN_commit(dd->sdcd);
    SDCONN_readstream(dd->sdcd, buf, 3);

    if (buf[0] == 23 /* CMD_SET_CONFIGDATA */ && 
        buf[1] == 20 /* MSG_REPLY */ &&
        buf[2] ==  1 /* CMD_SUCCESS */
    ) {
      sd_debug(2, "%s(): successfully changed DISPCLKDIV and FPSHIFTDIV (%d / %d)", __func__,
                  serdisp_sed133x_internal_getStruct(dd)->dispclkdiv,
                  serdisp_sed133x_internal_getStruct(dd)->fpshiftdiv
      );
    } else {
      sd_debug(1, "%s(): unable to change DISPCLKDIV and FPSHIFTDIV", __func__);
    }
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  return 1;
}


/* *********************************
   void serdisp_usb13700_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_usb13700_close(serdisp_t* dd) {
  SDCONN_write(dd->sdcd,              0xfe, 0);
  SDCONN_write(dd->sdcd,                 7, 0);  /* CMD_WRITE_S1D13700_COMMAND */
  SDCONN_write(dd->sdcd,                10, 0);  /* MSG_COMMAND */
  SDCONN_write(dd->sdcd,       CMD_DISPOFF, 0);  /* display off */
  SDCONN_commit(dd->sdcd);
}


