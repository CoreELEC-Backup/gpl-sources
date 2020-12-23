/*
 *************************************************************************
 *
 * serdisp_specific_lh155.c
 * routines for controlling displays controlled by lh155-compliant controllers
 *
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
/*#include "serdisplib/serdisp_specific_lh155.h"*/
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"

/* #define OPT_USEOLDUPDATEALGO */


/*
 * command constants
 */

#define CMD_XADDRLO       0x00
#define CMD_XADDRHI       0x10
#define CMD_YADDRLO       0x20
#define CMD_YADDRHI       0x30
#define CMD_DISPSTARTLO   0x40
#define CMD_DISPSTARTHI   0x50
#define CMD_NLINEALTLO    0x60
#define CMD_NLINEALTHI    0x70
#define CMD_DISPCONTR1    0x80
#define INI_DISPOFF       0x00
#define INI_DISPON        0x01
#define INI_MODNORMAL     0x00
#define INI_MODALLON      0x02
#define CMD_DISPCONTR2    0x90
#define INI_SWAPOFF       0x00
#define INI_SWAPON        0x02
#define INI_REVERSEOFF    0x00
#define INI_REVERSEON     0x08
#define CMD_INCRCONTR     0xA0
#define INI_INCRX         0x01
#define INI_INCRY         0x02
#define INI_INCRBOTH      0x03
#define CMD_POWCONTR1     0xB0
#define INI_POWEROFF      0x00
#define INI_POWERON       0x02
#define CMD_POWCONTR2     0xD0
#define CMD_POWCONTR3     0xE0
#define CMD_RESETOFF      0xF0
#define CMD_RESETON       0xF1


/*
 * values used mainly for initialization
 */


#define INTERFACE_8080      0
#define INTERFACE_6800      1


/* define shorts for signals to make coding simpler. must match order of serdisp_lh155_wiresignals[] */
#define SIG_D0          (dd->sdcd->signals[ 0])
#define SIG_D1          (dd->sdcd->signals[ 1])
#define SIG_D2          (dd->sdcd->signals[ 2])
#define SIG_D3          (dd->sdcd->signals[ 3])
#define SIG_D4          (dd->sdcd->signals[ 4])
#define SIG_D5          (dd->sdcd->signals[ 5])
#define SIG_D6          (dd->sdcd->signals[ 6])
#define SIG_D7          (dd->sdcd->signals[ 7])
#define SIG_CS          (dd->sdcd->signals[ 8])
#define SIG_RS          (dd->sdcd->signals[ 9])
#define SIG_WR          (dd->sdcd->signals[10])
#define SIG_RD          (dd->sdcd->signals[11])
#define SIG_RESET       (dd->sdcd->signals[12])
#define SIG_BACKLIGHT   (dd->sdcd->signals[13])


/* different display types/models supported by this driver */
#define DISPID_LH155     1
#define DISPID_SHARP240  2


serdisp_wiresignal_t serdisp_lh155_wiresignals[] = {
 /*  type   signame   actlow   cord  index */
   {SDCT_PP, "CS",         1,   'C',    8 }
  ,{SDCT_PP, "RS",         0,   'C',    9 }
  ,{SDCT_PP, "WR",         1,   'C',   10 }
  ,{SDCT_PP, "RD",         1,   'C',   11 }
  ,{SDCT_PP, "RESET",      1,   'D',   12 }
  ,{SDCT_PP, "BACKLIGHT",  0,   'D',   13 }
};

/* 1st wiring is taken from pollin's test-assembling */

serdisp_wiredef_t serdisp_lh155_wiredefs[] = {
   {  0, SDCT_PP, "Original", "DATA8,CS:nSELIN,RS:nAUTO,WR:nSTRB,RESET:INIT", "Original wiring"}
/*  ,{  4, SDCT_PP, "template",   "DATA8,CS:nSELIN,RS:nAUTO,WR:nSTRB,RD:INIT", "template line"}*/
};

serdisp_options_t serdisp_lh155_options[] = {
   /*  name          aliasnames   min    max mod int defines  */
   {  "WIDTH",       "",            0,   240,  8, 0,  ""}
  ,{  "HEIGHT",      "",            0,    64,  1, 0,  ""}
  ,{  "DELAY",       "",            0,    -1,  1, 1,  ""}
/* deactivated. 6800-mode not working correctly */
/*  ,{  "INTERFACE",   "MODE",        0,     1,  1, 0,  "8080=0,6800=1"} */
};


/* internal typedefs and functions */

static void serdisp_lh155_init      (serdisp_t*);
static void serdisp_lh155_update    (serdisp_t*);
static int  serdisp_lh155_setoption (serdisp_t*, const char*, long);
static void serdisp_lh155_close     (serdisp_t*);

static void serdisp_lh155_transfer  (serdisp_t* dd, int iscmd, byte item);
static void serdisp_lh155_writecmd  (serdisp_t* dd, byte cmd);
static void serdisp_lh155_writedata (serdisp_t* dd, byte data);


typedef struct serdisp_lh155_specific_s {
  int  interfacemode;
} serdisp_lh155_specific_t;


static serdisp_lh155_specific_t* serdisp_lh155_internal_getStruct(serdisp_t* dd) {
  return (serdisp_lh155_specific_t*)(dd->specific_data);
}


void serdisp_lh155_transfer(serdisp_t* dd, int iscmd, byte item) {
  long item_split = 0;
  long td_clk1 = 0;
  long td_clk2 = 0;
  long td_clk3 = 0;
  long td_clk4 = 0;
  int i;

  /* active-low signals are internally seen active-high because they will be auto-inverted later if needed */
  if (serdisp_lh155_internal_getStruct(dd)->interfacemode == INTERFACE_6800) {
    td_clk1 = SIG_WR | SIG_RD;
    td_clk2 = SIG_WR | SIG_RD;
    td_clk3 = SIG_WR;
    td_clk4 = SIG_WR | SIG_RD;
  } else {
    td_clk1 = 0;                 /* /CS /WR  */
    td_clk2 = SIG_CS;            /*     /WR  */
    td_clk3 = SIG_WR | SIG_CS;   /*          */
    td_clk4 = SIG_CS;            /*     /WR  */
  }

  if (iscmd) {
    td_clk1 |= SIG_RS;
    td_clk2 |= SIG_RS;
    td_clk3 |= SIG_RS;
    td_clk4 |= SIG_RS;
  }
  
  for (i = 0; i < 8; i++)
    if (item & (1 << i))
       item_split |= dd->sdcd->signals[i];


  td_clk2 |= item_split;
  td_clk3 |= item_split;
  td_clk4 |= item_split;

  SDCONN_write(dd->sdcd, td_clk1, dd->sdcd->io_flags_writecmd);
  sdtools_nsleep(dd->delay);
  SDCONN_write(dd->sdcd, td_clk2, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
  sdtools_nsleep(dd->delay);
  SDCONN_write(dd->sdcd, td_clk3, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
  sdtools_nsleep(dd->delay);
  SDCONN_write(dd->sdcd, td_clk4, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
  sdtools_nsleep(dd->delay);
}



void serdisp_lh155_writecmd(serdisp_t* dd, byte cmd) {
  serdisp_lh155_transfer(dd, 1, cmd);
}


void serdisp_lh155_writedata(serdisp_t* dd, byte data) {
  serdisp_lh155_transfer(dd, 0, data);
}


/* callback-function for setting non-standard options */
static void* serdisp_lh155_getvalueptr (serdisp_t* dd, const char* optionname, int* typesize) {
  if (serdisp_compareoptionnames(dd, optionname, "INTERFACE")) {
    *typesize = sizeof(int);
    return &(serdisp_lh155_internal_getStruct(dd)->interfacemode);
  }
  return 0;
}

/* main functions */


/* *********************************
   serdisp_t* serdisp_lh155_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle (not used in here)
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_lh155_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;
  int scrbuf_columns;

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "serdisp_lh155_setup(): cannot allocate display descriptor");
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));

  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_lh155_specific_t)) )) {
    free(dd);
    return (serdisp_t*)0;
  }

  memset(dd->specific_data, 0, sizeof(serdisp_lh155_specific_t));

  /* "SED133X"-based displays supported in here */
  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("LH155", dispname))
    dd->dsp_id = DISPID_LH155;
  else if (serdisp_comparedispnames("SHARP240", dispname))
    dd->dsp_id = DISPID_SHARP240;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_lh155.c", dispname);
    return (serdisp_t*)0;
  }

  /* per display settings */

  dd->width             = 128;
  dd->height            = 64;
  dd->depth             = 1;
  dd->feature_contrast  = 0;
  dd->feature_backlight = 0;
  dd->feature_invert    = 1;

  /* max. delta for optimised update algorithm */
  dd->optalgo_maxdelta  = 3;

  dd->delay = 0;
  serdisp_lh155_internal_getStruct(dd)->interfacemode = INTERFACE_8080;

  /* finally set some non display specific defaults */

  dd->curr_rotate       = 0;         /* unrotated display */
  dd->curr_invert       = 0;         /* display not inverted */

  /* supported output devices */
  dd->connection_types  = SERDISPCONNTYPE_PARPORT;

  dd->fp_init           = &serdisp_lh155_init;
  dd->fp_update         = &serdisp_lh155_update;
  dd->fp_close          = &serdisp_lh155_close;
  dd->fp_setoption      = &serdisp_lh155_setoption;
  dd->fp_getvalueptr    = &serdisp_lh155_getvalueptr;

#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
  dd->fp_setsdpixel     = &sdtools_generic_setsdpixel_greyhoriz;
  dd->fp_getsdpixel     = &sdtools_generic_getsdpixel_greyhoriz;
#else
  dd->fp_setpixel       = &sdtools_generic_setpixel_greyhoriz;
  dd->fp_getpixel       = &sdtools_generic_getpixel_greyhoriz;
#endif

  serdisp_setupstructinfos(dd, serdisp_lh155_wiresignals, serdisp_lh155_wiredefs, serdisp_lh155_options);

  if (dd->dsp_id == DISPID_SHARP240) {
    dd->width             = 240;
    dd->dsparea_width     = 72000;  /* display area in micrometres (according to datasheet) */
    dd->dsparea_height    = 32000;
  }

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    free(dd->specific_data);
    free(dd);
    dd = 0;
    return (serdisp_t*)0;    
  }

  scrbuf_columns = (dd->width + 7) / 8; 

  /* calculate (non default) screen buffer size */
  dd->scrbuf_size = sizeof(byte) * (scrbuf_columns * ( dd->height +dd->ycolgaps )); 

  /* one byte is able to store 8 change infos */
  dd->scrbuf_chg_size = sizeof(byte) * ( ((int)((scrbuf_columns + 7)/8)) * ( dd->height +dd->ycolgaps ));

  return dd;
}



/* *********************************
   void serdisp_lh155_init(dd)
   *********************************
   initialise a lh155-based display
   *********************************
   dd     ... display descriptor
*/
void serdisp_lh155_init(serdisp_t* dd) {
  if (SIG_RESET) {
    SDCONN_write(dd->sdcd, SIG_RESET, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
    SDCONN_usleep(dd->sdcd, 300*1000);

    SDCONN_write(dd->sdcd, 0, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
    /*SDCONN_usleep(dd->sdcd, 300*1000);*/
  }
  
  serdisp_lh155_writecmd (dd, CMD_RESETOFF);
  serdisp_lh155_writecmd (dd, CMD_XADDRLO);
  if (dd->dsp_id == DISPID_SHARP240) {
    serdisp_lh155_writecmd (dd, CMD_XADDRHI);
  }
  serdisp_lh155_writecmd (dd, CMD_YADDRLO);
  serdisp_lh155_writecmd (dd, CMD_YADDRHI);
  serdisp_lh155_writecmd (dd, CMD_DISPSTARTLO);
  serdisp_lh155_writecmd (dd, CMD_DISPSTARTHI);
  serdisp_lh155_writecmd (dd, CMD_NLINEALTLO | 0x01);
  serdisp_lh155_writecmd (dd, CMD_NLINEALTHI | 0x00);
  serdisp_lh155_writecmd (dd, CMD_RESETOFF);
  serdisp_lh155_writecmd (dd, CMD_DISPCONTR1 | INI_DISPON);
  serdisp_lh155_writecmd (dd, CMD_DISPCONTR2 | INI_SWAPON);
  serdisp_lh155_writecmd (dd, CMD_INCRCONTR  | INI_INCRBOTH);
  serdisp_lh155_writecmd (dd, CMD_POWCONTR1  | INI_POWERON);
  serdisp_lh155_writecmd (dd, CMD_POWCONTR2  | 0x0E);
  serdisp_lh155_writecmd (dd, CMD_POWCONTR3);
  serdisp_lh155_writecmd (dd, CMD_RESETON);
  serdisp_lh155_writecmd (dd, CMD_POWCONTR3);
  serdisp_lh155_writecmd (dd, CMD_RESETOFF);
  
  sd_debug(2, "serdisp_lh155_init(): done with initialising");
}



/* *********************************
   void serdisp_lh155_update(dd)
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
void serdisp_lh155_update(serdisp_t* dd) {
  int i, col;
  int max_col = ((dd->width + 7) / 8);
  byte data;

#ifdef OPT_USEOLDUPDATEALGO
  /* unoptimised display update (slow. all pixels are redrawn) */

  for (i = 0; i < dd->height; i++) {

    serdisp_lh155_writecmd (dd, CMD_YADDRLO | (i & 0x0F));
    serdisp_lh155_writecmd (dd, CMD_YADDRHI | (i >> 4));

    serdisp_lh155_writecmd (dd, CMD_XADDRLO);
    if (dd->dsp_id == DISPID_SHARP240) {
      serdisp_lh155_writecmd (dd, CMD_XADDRHI);
    }

    for (col = 0; col < max_col; col++) {
      data = dd->scrbuf [ max_col * i + col];

      if (dd->curr_invert && !(dd->feature_invert))
        data = ~data;

      serdisp_lh155_writedata(dd, data);
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
    int last_y = -1;

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

      last_y = -1;
      for (l = i; l <= i_delta; l++) {
        y_i = l / max_col;
        col_i = l % max_col;

        if (y_i != last_y) {
          serdisp_lh155_writecmd (dd, CMD_YADDRLO | (y_i & 0x0F));
          serdisp_lh155_writecmd (dd, CMD_YADDRHI | (y_i >> 4));

          serdisp_lh155_writecmd (dd, CMD_XADDRLO | (col_i & 0x0F) );
          if (dd->dsp_id == DISPID_SHARP240)
            serdisp_lh155_writecmd (dd, CMD_XADDRHI | (col_i >> 4) );
          last_y = y_i;
          sdtools_nsleep(2);  /* some delay between control and data commands (to avoid timing problems) */
        }

        data = dd->scrbuf [l];

        if (dd->curr_invert && !(dd->feature_invert)) 
          data = ~data;

        serdisp_lh155_writedata(dd, data);
        dd->scrbuf_chg[ ((max_col + 7) / 8) * y_i + (col_i / 8)] &=  (0xFF ^ (1 << (col_i % 8)));
      }

      i = i_delta+1;

    } else {
      i++;
    }
  } /* while i < scrbuf_size */
#endif /* OPT_USEOLDUPDATEALGO */
}



/* *********************************
   int serdisp_lh155_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_lh155_setoption(serdisp_t* dd, const char* option, long value) {

  if (dd->feature_invert && serdisp_compareoptionnames(dd, option, "INVERT") ) {
    if (value < 2) 
      dd->curr_invert = (int)value;
    else
      dd->curr_invert = (dd->curr_invert) ? 0 : 1;
    serdisp_lh155_writecmd (dd, CMD_DISPCONTR2 | INI_SWAPON | ( (dd->curr_invert) ? INI_REVERSEON : INI_REVERSEOFF ));
  } else if (dd->feature_backlight && serdisp_compareoptionnames(dd, option, "BACKLIGHT")) {
    if (value < 2) 
      dd->curr_backlight = (int)value;
    else
      dd->curr_backlight = (dd->curr_backlight) ? 0 : 1;  
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  return 1;
}



/* *********************************
   void serdisp_lh155_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_lh155_close(serdisp_t* dd) {
  serdisp_lh155_writecmd (dd, CMD_DISPCONTR1 | INI_DISPOFF);
}

