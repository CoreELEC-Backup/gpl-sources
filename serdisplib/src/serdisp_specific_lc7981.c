/*
 *************************************************************************
 *
 * serdisp_specific_lc7981.c
 * routines for controlling displays controlled by Sanyo LC7981
 *
 * supported:
 * - DG-16080 160x80 monochrome
 *
 *************************************************************************
 *
 * copyright (C) 2009-2010  wolfgang astleitner
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

/*
 * values used mainly for initialization
 */


/* define shorts for signals to make coding simpler. must match order of serdisp_lc7981_wiresignals[] */
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
#define SIG_RW          (dd->sdcd->signals[10])
#define SIG_E           (dd->sdcd->signals[11])
#define SIG_RESET       (dd->sdcd->signals[12])
#define SIG_BACKLIGHT   (dd->sdcd->signals[13])


/* different display types/models supported by this driver */
#define DISPID_DG16080      1
#define DISPID_DG1608011    2


serdisp_wiresignal_t serdisp_lc7981_wiresignals[] = {
 /*  type   signame   actlow   cord  index */
   {SDCT_PP, "CS",         1,   'C',    8 }
  ,{SDCT_PP, "RS",         0,   'C',    9 }
  ,{SDCT_PP, "RW",         0,   'C',   10 }
  ,{SDCT_PP, "E",          0,   'C',   11 }
  ,{SDCT_PP, "RESET",      1,   'D',   12 }
  ,{SDCT_PP, "BACKLIGHT",  0,   'D',   13 }
};

/*wirings */
serdisp_wiredef_t serdisp_lc7981_wiredefs[] = {
   {  0, SDCT_PP, "Simple",        "DATA8,RS:INIT,E:nSTRB", "Simple wiring"}
  ,{  1, SDCT_PP, "SimpleSWRes",   "DATA8,RS:INIT,E:nSTRB,RESET:nSELIN", "Simple wiring w/ software reset"}
  ,{  2, SDCT_PP, "Standard",      "DATA8,RS:INIT,E:nSTRB,RW:nAUTO,CS:nSELIN", "Standard wiring"}
  ,{  3, SDCT_PP, "StandardSWRes", "DATA8,RS:INIT,E:nSTRB,RW:nAUTO,RESET:nSELIN", "Simple wiring w/ software reset"}
  ,{  4, SDCT_PP, "Backlight",     "DATA8,RS:nAUTO,E:nSTRB,BACKLIGHT:INIT", "Backlight support"}
  ,{  5, SDCT_PP, "BacklightSWRes","DATA8,RS:nAUTO,E:nSTRB,BACKLIGHT:INIT,RESET:nSELIN", "Backlight support w/ software reset"}
};

serdisp_options_t serdisp_lc7981_options[] = {
   /*  name          aliasnames   min    max mod int defines  */
   {  "DELAY",       "",            0,    -1,  1, 1,  ""}
  ,{  "BACKLIGHT",   "",            0,     1,  1, 1,  ""}
  ,{  "BRIGHTNESS", "",             0,   100,  1, 1,  ""}      /* brightness, 0: bg-light off, <>0: dd->curr_backlight */
};


/* internal typedefs and functions */

static void serdisp_lc7981_init        (serdisp_t*);
static void serdisp_lc7981_update      (serdisp_t*);
static int  serdisp_lc7981_setoption   (serdisp_t*, const char*, long);
static void serdisp_lc7981_close       (serdisp_t*);

static void serdisp_lc7981_transfer    (serdisp_t*, int, byte);
static void serdisp_lc7981_writecmd    (serdisp_t*, byte);
static void serdisp_lc7981_writedata   (serdisp_t*, byte);


void serdisp_lc7981_transfer(serdisp_t* dd, int iscmd, byte item) {
  int i;

  long item_split = 0;
  long td_clk1 = 0;
  long td_clk2 = 0;
  long td_clk3 = 0;

  /* active-low signals are internally seen active-high because they will be auto-inverted later if needed */
  td_clk1 = 0;
  td_clk2 = SIG_E;
  td_clk3 = 0;

  /* enable backlight if
     * signal SIG_BACKLIGHT is defined
     * dd->curr_backlight = 1
     * brightness > 0 (== dd->curr_dimming != 100)
   */
  if (dd->feature_backlight && dd->curr_backlight && dd->curr_dimming < 100) {
    td_clk1 |= SIG_BACKLIGHT;
    td_clk2 |= SIG_BACKLIGHT;
    td_clk3 |= SIG_BACKLIGHT;
  }

  if (SIG_CS) {
    td_clk1 |= SIG_CS;
    td_clk2 |= SIG_CS;
    td_clk3 |= SIG_CS;
  }

  if (iscmd) {
    td_clk1 |= SIG_RS;
    td_clk2 |= SIG_RS;
    td_clk3 |= SIG_RS;
  }

  for (i = 0; i < 8; i++)
    if (item & (1 << i))
        item_split |= dd->sdcd->signals[i];

  td_clk1 |= item_split;
  td_clk2 |= item_split;

  SDCONN_write(dd->sdcd, td_clk1, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
  sdtools_nsleep(dd->delay);
  SDCONN_write(dd->sdcd, td_clk2, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
  sdtools_nsleep(dd->delay);
  SDCONN_write(dd->sdcd, td_clk3, dd->sdcd->io_flags_writecmd);
  sdtools_nsleep(dd->delay);
}


void serdisp_lc7981_writecmd(serdisp_t* dd, byte cmd) {
  serdisp_lc7981_transfer(dd, 1, cmd);
}


void serdisp_lc7981_writedata(serdisp_t* dd, byte data) {
  serdisp_lc7981_transfer(dd, 0, data);
}


/* main functions */


/* *********************************
   serdisp_t* serdisp_lc7981_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle (not used in here)
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_lc7981_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate display descriptor", __func__);
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));

  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("DG16080", dispname))
    dd->dsp_id = DISPID_DG16080;
  else if (serdisp_comparedispnames("DG1608011", dispname))
    dd->dsp_id = DISPID_DG1608011;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_lc7981.c", dispname);
    return (serdisp_t*)0;
  }

  /* default setup for function pointers */
  dd->fp_init           = &serdisp_lc7981_init;
  dd->fp_update         = &serdisp_lc7981_update;
  dd->fp_close          = &serdisp_lc7981_close;
  dd->fp_setoption      = &serdisp_lc7981_setoption;


  /* per display settings */

  dd->width             = 160;
  dd->height            = 80;
  dd->depth             = 1;
  dd->feature_contrast  = 0;
  dd->feature_backlight = 1;
  dd->feature_invert    = 0;
  dd->curr_backlight    = 1;         /* start with backlight on */

  if (dd->dsp_id == DISPID_DG1608011) {
    dd->dsparea_width     = 67000;     /* display area in micrometres (measured) */
    dd->dsparea_height    = 48500;
  }

  /* max. delta for optimised update algorithm */
  dd->optalgo_maxdelta  = 2;

  dd->delay = 0;

  /* finally set some non display specific defaults */

  dd->curr_rotate       = 0;         /* unrotated display */
  dd->curr_invert       = 0;         /* display not inverted */

  /* supported output devices */
  dd->connection_types  = SERDISPCONNTYPE_PARPORT;

#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
  dd->fp_setsdpixel     = &sdtools_generic_setsdpixel_greyhoriz;
  dd->fp_getsdpixel     = &sdtools_generic_getsdpixel_greyhoriz;
#else
  dd->fp_setpixel       = &sdtools_generic_setpixel_greyhoriz;
  dd->fp_getpixel       = &sdtools_generic_getpixel_greyhoriz;
#endif

  serdisp_setupstructinfos(dd, serdisp_lc7981_wiresignals, serdisp_lc7981_wiredefs, serdisp_lc7981_options);

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    free(dd->specific_data);
    free(dd);
    dd = 0;
    return (serdisp_t*)0;
  }

  return dd;
}



/* *********************************
   void serdisp_lc7981_init(dd)
   *********************************
   initialise a lc7981-based display
   *********************************
   dd     ... display descriptor
*/
void serdisp_lc7981_init(serdisp_t* dd) {
  /* de-activate backlight support if matching signal not defined */
  if (! SIG_BACKLIGHT)
    dd->feature_backlight = 0;

  if (SIG_RESET)
    SDCONN_write(dd->sdcd, SIG_RESET | ((SIG_CS) ? SIG_CS : 0),
                 dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);

  SDCONN_usleep(dd->sdcd, 5);

  SDCONN_write(dd->sdcd, 
               ((SIG_CS) ? SIG_CS : 0) | ((SIG_BACKLIGHT) ? SIG_BACKLIGHT : 0),
               dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata
  );

  SDCONN_usleep(dd->sdcd, 5);

  /* init sequence taken from http://www.frozeneskimo.com,
     file http://www.frozeneskimo.com/samsunglcd/avr-lc7981-v1/lc7981.c
   */
  serdisp_lc7981_writecmd  (dd, 0x00);  /* mode control */
  serdisp_lc7981_writedata (dd, 0x32);  /* 0011 0010: display on, master on, graphic mode */

  serdisp_lc7981_writecmd  (dd, 0x01);  /* char pitch */
  serdisp_lc7981_writedata (dd, 7);     /* hor. pitch - 1 */

  serdisp_lc7981_writecmd  (dd, 0x02);  /* number of characters */
  serdisp_lc7981_writedata (dd, (dd->width/8)-1);     /* number of bytes in hor. dir - 1 */

  serdisp_lc7981_writecmd  (dd, 0x03);  /* time division number */
  serdisp_lc7981_writedata (dd, 80-1);  /* display duty = 1/80 according to datasheet */

  serdisp_lc7981_writecmd  (dd, 0x04);  /* cursor line */
  serdisp_lc7981_writedata (dd, 0);

  serdisp_lc7981_writecmd  (dd, 0x08);   /* set lower start address */
  serdisp_lc7981_writedata (dd, 0);
  serdisp_lc7981_writecmd  (dd, 0x09);   /* set upper start address */
  serdisp_lc7981_writedata (dd, 0);

  sd_debug(2, "%s(): done with initialising", __func__);
}



/* *********************************
   void serdisp_lc7981_update(dd)
   *********************************
   updates the display using display-buffer scrbuf+scrbuf_chg
   *********************************
   dd     ... display descriptor
   *********************************

   the display is redrawn using a time-saving algorithm:

     background knowledge: after writing a data-entry to the display RAM,
     the RAM address is increased automatically =>
     * try to utilize this auto-increasing
     * on the other hand try to avoid writing of unchanged data

*/
void serdisp_lc7981_update(serdisp_t* dd) {
  int i;
  byte data;

#ifdef OPT_USEOLDUPDATEALGO

  serdisp_lc7981_writecmd  (dd, 0x0A);   /* set cursor lower address */
  serdisp_lc7981_writedata (dd, 0 );
  serdisp_lc7981_writecmd  (dd, 0x0B);   /* set cursor upper start address */
  serdisp_lc7981_writedata (dd, 0);

  for (i = 0; i < dd->scrbuf_size; i++) {
    data = sdtools_reversebits(dd->scrbuf [ i ]);

    if (dd->curr_invert)
      data = ~data;

    /* really sick way of sending data.
       actually it would be possible to do:
       writecmd(0x0C), writedata(data), writedata(data), and so on
       but this would require a very, veeery long delay after each writedata() ...

       writecmd(0x0C), writedata(data), writecmd(0x0C), writedata(data), and so on
       seems to be much faster and even more stable.
     */
    serdisp_lc7981_writecmd  (dd, 0x0C);   /* write display data */
    serdisp_lc7981_writedata (dd, data);
  }

#else /* OPT_USEOLDUPDATEALGO */

  /* display is drawn using an optimising algorithm which tries to only send as few data as possible to the display */

  /* more detailed explanations of principle: 
     see serdisp_specific_optrex323.c / i2c.c / pcd8544.c / sed153x.c / sed1565.c 
  */

  int delta; 
  int i_delta = 0; /* i_delta - i: how many columns to transfer in one take */
  int max_col = dd->width >> 3;
  int max_col_chg = (max_col + 7) / 8;

  i = 0;

  while (i < dd->scrbuf_size) {
    int i_chg = i % max_col;
    int j_chg = i / max_col;

    int li_chg;
    int lj_chg;

    if ( dd->scrbuf_chg[ j_chg * max_col_chg + (i_chg >> 3) ] & ( 1 << (i_chg%8) ) ) {
      int l;

      i_delta = i+1;
      delta = 0;

      while (i_delta < dd->scrbuf_size-delta-1 && delta < dd->optalgo_maxdelta) {
        int i_delta_chg = i_delta % max_col;
        int j_delta_chg = i_delta / max_col;

        if ( dd->scrbuf_chg[ j_delta_chg * max_col_chg + (i_delta_chg >> 3)] & ( 1 << (i_delta_chg%8) ) ) {
          i_delta += delta+1;
          delta = 0;
        } else {
          delta++;
        }
      }

      serdisp_lc7981_writecmd  (dd, 0x0A);   /* set cursor lower address */
      serdisp_lc7981_writedata (dd, i & 0x00FF );
      serdisp_lc7981_writecmd  (dd, 0x0B);   /* set cursor upper start address */
      serdisp_lc7981_writedata (dd, (i & 0xFF00) >> 8 );

      for (l = i; l <= i_delta; l++) {
        data = sdtools_reversebits(dd->scrbuf [ l ]);

        if (dd->curr_invert)
          data = ~data;

        li_chg = l % max_col;
        lj_chg = l / max_col;

        serdisp_lc7981_writecmd  (dd, 0x0C);   /* write display data */
        serdisp_lc7981_writedata (dd, data);
        dd->scrbuf_chg[ lj_chg * max_col_chg + (li_chg >> 3) ] &=  (0xFF ^ (1 << (li_chg % 8)));
      }

      i = i_delta+1;
    } else {
      i++;
    }
  }

#endif /* OPT_USEOLDUPDATEALGO */

  SDCONN_commit(dd->sdcd); /* if streaming: be sure that every data is transmitted */
}


/* *********************************
   int serdisp_lc7981_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_lc7981_setoption(serdisp_t* dd, const char* option, long value) {
  if (dd->feature_backlight && 
      (serdisp_compareoptionnames(dd, option, "BACKLIGHT" ) ||
       serdisp_compareoptionnames(dd, option, "BRIGHTNESS" )
      )
     )
  {
    if ( serdisp_compareoptionnames(dd, option, "BRIGHTNESS" ) ) {
      dd->curr_dimming = 100 - (int)value;
    } else {
      if (value < 2) 
        dd->curr_backlight = (int)value;
      else
        dd->curr_backlight = (dd->curr_backlight) ? 0 : 1;
    }
    /* no command for en/disable backlight, so issue 'dummy'-command
       (which indirectly enables/disabled backlight) */
    serdisp_rewrite(dd);
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  return 1;
}



/* *********************************
   void serdisp_lc7981_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_lc7981_close(serdisp_t* dd) {
  dd->curr_backlight = 0;
  serdisp_lc7981_writecmd  (dd, 0x00);  /* mode control */
  serdisp_lc7981_writedata (dd, 0x12);  /* 0001 0010: display off, master on, graphic mode */
}





