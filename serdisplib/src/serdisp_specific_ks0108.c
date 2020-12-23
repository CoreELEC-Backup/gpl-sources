/*
 *************************************************************************
 *
 * serdisp_specific_ks0108.c
 * routines for controlling samsung ks0108-based displays 
 * (eg: powertip pg-12864 series)
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

#include "serdisplib/serdisp_connect.h"
/*#include "serdisplib/serdisp_specific_ks0108.h"*/
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"


#undef  OPT_USEOLDUPDATEALGO
#define OPT_USEUPDATEDELTAALGO


/*
 * command constants
 */

#define CMD_DISPLAYON   0x3F
#define CMD_DISPLAYOFF  0x3E

#define CMD_STRTLINEADR 0xC0
#define CMD_SPAGEADR    0xB8
#define CMD_SCOLADR     0x40

/* define shorts for signals to make coding simpler. must match order of serdisp_ks0108_wiresignals[] */
#define SIG_D0          (dd->sdcd->signals[ 0])
#define SIG_D1          (dd->sdcd->signals[ 1])
#define SIG_D2          (dd->sdcd->signals[ 2])
#define SIG_D3          (dd->sdcd->signals[ 3])
#define SIG_D4          (dd->sdcd->signals[ 4])
#define SIG_D5          (dd->sdcd->signals[ 5])
#define SIG_D6          (dd->sdcd->signals[ 6])
#define SIG_D7          (dd->sdcd->signals[ 7])
#define SIG_A0          (dd->sdcd->signals[ 8])
#define SIG_CS1         (dd->sdcd->signals[ 9])
#define SIG_CS2         (dd->sdcd->signals[10])
#define SIG_EN          (dd->sdcd->signals[11])
#define SIG_RESET       (dd->sdcd->signals[12])
#define SIG_BACKLIGHT   (dd->sdcd->signals[13])


/* different display types/models supported by this driver */
#define DISPID_KS0108      1
#define DISPID_CTINCLUD    2


serdisp_wiresignal_t serdisp_ks0108_wiresignals[] = {
 /*  type   signame   actlow   cord  index */
   {SDCT_PP, "A0",         0,   'C',    8 }
  ,{SDCT_PP, "CS1",        0,   'C',    9 }
  ,{SDCT_PP, "CS2",        0,   'C',   10 }
  ,{SDCT_PP, "EN",         0,   'C',   11 }
  ,{SDCT_PP, "RESET",      1,   'D',   12 }
  ,{SDCT_PP, "BACKLIGHT",  0,   'D',   13 }
};


serdisp_wiredef_t serdisp_ks0108_wiredefs[] = {
   {  0, SDCT_PP, "Standard", "DATA8,A0:nSELIN,CS1:nAUTO,CS2:INIT,EN:nSTRB", "Standard wiring"}
};


serdisp_options_t serdisp_ks0108_options[] = {
   /*  name       aliasnames min  max mod int defines  */
   {  "WIDTH",     "",        64, 256, 64, 0,  ""}
  ,{  "HEIGHT",    "",        64,  64, 64, 0,  ""}
  ,{  "DELAY",     "",         0,  -1,  1, 1,  ""}
  ,{  "BACKLIGHT", "",         0,   1,  1, 1,  ""}
};

serdisp_options_t serdisp_ctinclud_options[] = {
   /*  name       aliasnames  min  max mod flags              defines  */
   {  "DELAY",     "",         0,  -1,  1, SD_OPTIONFLAG_RW,  ""}
};


/* internal typedefs and functions */

#define TRANSFER_CMD     0
#define TRANSFER_DATA    1
#define TRANSFER_RESET   2


typedef struct serdisp_ks0108_specific_s {
  byte currcs;         /* current select chip */
  int  controllers;    /* amount of controllers */
  long chipselect[4];  /* supports up to 4 controllers -> CS1 + CS2 -> 4 possibilities */
  void (*fp_transfer)  (serdisp_t* dd, int type, byte item);
  void (*fp_switchcs)  (serdisp_t* dd, byte controller);
} serdisp_ks0108_specific_t;


static void  serdisp_ks0108_init               (serdisp_t*);
static void  serdisp_ks0108_update             (serdisp_t*);
static int   serdisp_ks0108_setoption          (serdisp_t*, const char*, long);
static void  serdisp_ks0108_close              (serdisp_t*);

static void  serdisp_ks0108_transfer_parport   (serdisp_t*, int, byte);
static void  serdisp_ks0108_switchcs_parport   (serdisp_t*, byte);

static void  serdisp_ks0108_transfer_autoclock (serdisp_t*, int, byte);
static void  serdisp_ks0108_switchcs_autoclock (serdisp_t*, byte);



static serdisp_ks0108_specific_t* serdisp_ks0108_internal_getStruct(serdisp_t* dd) {
  return (serdisp_ks0108_specific_t*)(dd->specific_data);
}



/* *********************************
   serdisp_t* serdisp_ks0108_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle (not used in here)
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_ks0108_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate display descriptor", __func__);
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));

  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_ks0108_specific_t)) )) {
    free(dd);
    return (serdisp_t*)0;
  }

  memset(dd->specific_data, 0, sizeof(serdisp_ks0108_specific_t));

  /* "KS0108"-based displays supported in here (eg. powertip pg-12864 series)  */
  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("KS0108", dispname))
    dd->dsp_id = DISPID_KS0108;
  else if (serdisp_comparedispnames("CTINCLUD", dispname))
    dd->dsp_id = DISPID_CTINCLUD;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_ks0108.c", dispname);
    return (serdisp_t*)0;
  }

  dd->width             = 128;
  dd->height            = 64;
  dd->depth             = 1;
  dd->startxcol         = 0;
  dd->feature_contrast  = 0;
  dd->feature_invert    = 0;
  dd->curr_rotate       = 0;         /* unrotated display */
  dd->connection_types  = SERDISPCONNTYPE_PARPORT;
  dd->fp_init           = &serdisp_ks0108_init;
  dd->fp_update         = &serdisp_ks0108_update;
  dd->fp_setoption      = &serdisp_ks0108_setoption;
  dd->fp_close          = &serdisp_ks0108_close;

  serdisp_ks0108_internal_getStruct(dd)->fp_transfer = &serdisp_ks0108_transfer_parport;
  serdisp_ks0108_internal_getStruct(dd)->fp_switchcs = &serdisp_ks0108_switchcs_parport;

  dd->delay             = 180;  /* may be reduced when using short and shielded wires */
  dd->optalgo_maxdelta  = 3;

  if (dd->dsp_id == DISPID_CTINCLUD) {
    dd->connection_types  = SERDISPCONNTYPE_PARPORT | SERDISPCONNTYPE_IOW24;

    serdisp_ks0108_internal_getStruct(dd)->fp_transfer = &serdisp_ks0108_transfer_autoclock;
    serdisp_ks0108_internal_getStruct(dd)->fp_switchcs = &serdisp_ks0108_switchcs_autoclock;

    dd->delay             = 0;  /* auto-clocking by iowarrior */
    dd->optalgo_maxdelta  = 6;
  }

  /* serdisp_ks0108_internal_getStruct(dd)->controllers = 2; */  /* calculated in serdisp_ks0108_init() */

  serdisp_ks0108_internal_getStruct(dd)->currcs = 0;  /* pre-init currcs with controller 0 */

  if (dd->dsp_id == DISPID_CTINCLUD) {
    serdisp_setupstructinfos(dd, 0, 0, serdisp_ctinclud_options);
  } else {
    serdisp_setupstructinfos(dd, serdisp_ks0108_wiresignals, serdisp_ks0108_wiredefs, serdisp_ks0108_options);
  }

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    free(dd);
    dd = 0;
    return (serdisp_t*)0;
  }

  /* force c't include display to width=128 (== ignore user-defined options for width other than 128 */
  if (dd->dsp_id == DISPID_CTINCLUD) {
    if (dd->width != 128) {
      dd->width = 128;
      serdisp_ks0108_internal_getStruct(dd)->controllers = 2;
      sd_debug(0, "%s(): c't includ display only supports 128x64 => width will be forced to 128", __func__);
    }
  }
  return dd;
}



/* *********************************
   void serdisp_ks0108_init(dd)
   *********************************
   initialise a ks0108-based display
   *********************************
   dd     ... display descriptor
*/
void serdisp_ks0108_init(serdisp_t* dd) {
  int ctr;  /* loop through controllers */

  /* calculate amount of controllers to be driven - only multiples of 64 are accepted by serdisp_ks0108_options[] */
  serdisp_ks0108_internal_getStruct(dd)->controllers = dd->width / 64;

  if (dd->dsp_id != DISPID_CTINCLUD) {
    if (serdisp_ks0108_internal_getStruct(dd)->controllers <= 2) {
      serdisp_ks0108_internal_getStruct(dd)->chipselect[0] = SIG_CS1;
      serdisp_ks0108_internal_getStruct(dd)->chipselect[1] = SIG_CS2;
    } else {  /* requires a multiplexer (eg 74LS42) */
      serdisp_ks0108_internal_getStruct(dd)->chipselect[0] = 0;
      serdisp_ks0108_internal_getStruct(dd)->chipselect[1] = SIG_CS1;
      serdisp_ks0108_internal_getStruct(dd)->chipselect[2] = SIG_CS2;
      serdisp_ks0108_internal_getStruct(dd)->chipselect[3] = SIG_CS1 | SIG_CS2 ;
    }
    dd->feature_backlight = (SIG_BACKLIGHT) ? 1 : 0;
  }

  serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_RESET, 0);

  serdisp_ks0108_internal_getStruct(dd)->currcs = -1;

  /* the initialisation and command reference can be found in the datasheet of KS0108 */
  for (ctr = 0; ctr < serdisp_ks0108_internal_getStruct(dd)->controllers; ctr ++) {  
    serdisp_ks0108_internal_getStruct(dd)->fp_switchcs(dd, ctr);

    serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_SPAGEADR);
    serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_SCOLADR);
    serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_DISPLAYON);
    serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_STRTLINEADR);
    serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_SPAGEADR);
  }
  SDCONN_commit(dd->sdcd);
  sd_debug(2, "%s(): done with initialising", __func__);
}




/* *********************************
   void serdisp_ks0108_update(dd)
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
void serdisp_ks0108_update(serdisp_t* dd) {
  int ctr;  /* loop through controllers */
  int x, page;
  int pages = (dd->height+7)/8;
  int xoffset;
  byte data;

#ifdef OPT_USEOLDUPDATEALGO
  for (ctr = 0; ctr < 2; ctr ++) {
    xoffset = 64 * ctr;

    serdisp_ks0108_internal_getStruct(dd)->fp_switchcs(dd, ctr);

    serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_STRTLINEADR);
      serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_SCOLADR);
    for (page = 0; page < pages; page++) {
      serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_SPAGEADR | page);

      for(x = 0; x < /*(dd->width + dd->xcolgaps) % 64*/ 64; x++) {
        data = dd->scrbuf[ dd->width * page  +  x + xoffset];

        /* if (dd->curr_invert && !(dd->feature_invert)) */
        if (dd->curr_invert)
          data = ~data;

        serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_DATA, data);
        dd->scrbuf_chg[dd->width * (page/8) + x + xoffset] &= 0xFF - (1 << (page%8)) ;
      }
    }
    SDCONN_commit(dd->sdcd);
  }

#elif defined(OPT_USEUPDATEDELTAALGO)  /* en theorie this should be the fastest update algo */
  int col;
  int page_set;
  int col_delta, delta;
  int max_delta = dd->optalgo_maxdelta;

  serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_STRTLINEADR);

  for (ctr = 0; ctr < serdisp_ks0108_internal_getStruct(dd)->controllers; ctr ++) {  
    xoffset = 64 * ctr;

    for (page = 0; page < pages; page++) {
      col = 0;
      page_set = 0;

      while (col < 64) {
        if ( dd->scrbuf_chg[ col + xoffset + dd->width *(page/8)] & ( 1 << (page%8)) ) {
          col_delta = col;

          delta = 0;
          while (col_delta < 64 - delta - 1 && delta < max_delta) {
            if (dd->scrbuf_chg[ col_delta + 1 + delta + xoffset  + dd->width * (page/8)] & ( 1 << (page%8)) ) {
              col_delta += delta + 1;
              delta = 0;
            } else {
              delta++;
            }
          }

          if (!page_set) {
            serdisp_ks0108_internal_getStruct(dd)->fp_switchcs(dd, ctr);
            serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_SPAGEADR | page);
            page_set = 1;
          }

          serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_SCOLADR | col);
          for (x = col ; x <= col_delta; x++) {
            data = dd->scrbuf[x + xoffset + dd->width * page ];

            /* if (dd->curr_invert && !(dd->feature_invert)) */
            if (dd->curr_invert)
              data = ~data;

            serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_DATA, data);
            dd->scrbuf_chg[ x + xoffset + dd->width * (page/8)] &= 0xFF - (1 << (page%8)) ;
          }
          col = col_delta+1;
        } else {
          col++;
        }
      }
    }
    SDCONN_commit(dd->sdcd);
  }
  /* dummy operation for multiplexed CS-lines (else: last page column will not be drawn) */
  serdisp_ks0108_internal_getStruct(dd)->fp_switchcs(dd, (ctr+1)%serdisp_ks0108_internal_getStruct(dd)->controllers);
#else /* OPT_USEOOLDUPDATEALGO -> 2DELTA */
  int set_page, last_x;

  for (ctr = 0; ctr < serdisp_ks0108_internal_getStruct(dd)->controllers; ctr ++) {  
    xoffset = 64 * ctr;

    for (page = 0; page < pages; page++) {

      last_x = -2;
      set_page = 1;

      for(x = 0; x < /*dd->width + dd->xcolgaps*/ 64; x++) {

        /* either actual_x or actual_x + 1 has changed  or one of left/right-most */
        if ( dd->scrbuf_chg[x + xoffset + dd->width *(page/8)] & ( 1 << (page%8)) ) {
          if (x > last_x+1 ) {

            if (set_page) {
              serdisp_ks0108_internal_getStruct(dd)->fp_switchcs(dd, ctr);

              serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_SPAGEADR | page);
              set_page = 0;
            }

            /* x-position may be written directly to ks0108 */
            serdisp_ks0108_internal_getStruct(dd)->fp_switchcs(dd, ctr);
            serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_SCOLADR | x);
          }
          data = dd->scrbuf[ dd->width * page  +  x + xoffset];

          /* if (dd->curr_invert && !(dd->feature_invert)) */
          if (dd->curr_invert)
            data = ~data;

          serdisp_ks0108_internal_getStruct(dd)->fp_switchcs(dd, ctr);
          serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_DATA, data);
          dd->scrbuf_chg[x + xoffset + dd->width * (page/8)] &= 0xFF - (1 << (page%8)) ;

          last_x = x;
        }
      }
    }
    SDCONN_commit(dd->sdcd);
  }
#endif  /* OPT_USEOLDUPDATEALGO */
}

/* *********************************
   int serdisp_ks0108_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_ks0108_setoption(serdisp_t* dd, const char* option, long value) {
  if (dd->feature_backlight && serdisp_compareoptionnames(dd, option, "BACKLIGHT") ) {
    if (value < 2) 
      dd->curr_backlight = (int)value;
    else
      dd->curr_backlight = (dd->curr_backlight) ? 0 : 1;
    /* no command for en/disable backlight, so issue 'dummy'-command
       (which indirectly enables/disabled backlight) */
    serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_DISPLAYON);
    SDCONN_commit(dd->sdcd);
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  return 1;
}





/* *********************************
   void serdisp_ks0108_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_ks0108_close(serdisp_t* dd) {
  int ctr;

  for (ctr = 0; ctr < serdisp_ks0108_internal_getStruct(dd)->controllers; ctr ++) {
    serdisp_ks0108_internal_getStruct(dd)->fp_switchcs(dd, ctr);
    serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_DISPLAYOFF);
    SDCONN_commit(dd->sdcd);
  }
  serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_RESET, 0);
}





/* *********************************
   void serdisp_ks0108_transfer_parport(dd, type, item)
   *********************************
   transfer a data or command byte to the display
   *********************************
   dd         ... display descriptor
   type       ... one if TRANSFER_CMD, _DATA, _RESET
   item       ... byte to be processed
*/
void serdisp_ks0108_transfer_parport(serdisp_t* dd, int type, byte item) {
  long item_split = 0;
  long td_clk1 = 0;
  long td_clk2 = 0;
  int ctr;
  int i;

  switch (type) {
    case TRANSFER_CMD:
    case TRANSFER_DATA:

      /* active-low signals are internally seen active-high because they will be auto-inverted later if needed */
      td_clk1 = serdisp_ks0108_internal_getStruct(dd)->chipselect[serdisp_ks0108_internal_getStruct(dd)->currcs] | SIG_EN;
      td_clk2 = serdisp_ks0108_internal_getStruct(dd)->chipselect[serdisp_ks0108_internal_getStruct(dd)->currcs];

      if (type == TRANSFER_DATA) { /* high: data, low: command */
        td_clk1 |= SIG_A0;
        td_clk2 |= SIG_A0;
      }

      for (i = 0; i < 8; i++)
        if (item & (1 << i))
           item_split |= dd->sdcd->signals[i];


      td_clk1 |= item_split;

      /* signal EN from high to low */
      SDCONN_writedelay(dd->sdcd, td_clk1, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata, dd->delay);
      /* EN needs to be kept on this level ~ 12 times as long as needed for a signal change */    
      SDCONN_writedelay(dd->sdcd, td_clk2, dd->sdcd->io_flags_writecmd, 12 * dd->delay); 
      break;
    case TRANSFER_RESET:
      /* reset */

      serdisp_ks0108_internal_getStruct(dd)->currcs = 0;
      for (ctr = 0; ctr < serdisp_ks0108_internal_getStruct(dd)->controllers; ctr ++) {
        serdisp_ks0108_internal_getStruct(dd)->fp_switchcs(dd, ctr);
        if (SIG_RESET) {  /* reset signal sent by parport */
          SDCONN_write(dd->sdcd, SIG_RESET | SIG_EN | serdisp_ks0108_internal_getStruct(dd)->chipselect[ctr], 
                       dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
        }
        /* min. 1 uS according to data sheet. we use 5 to be sure */
        SDCONN_usleep(dd->sdcd, 5);
        SDCONN_write(dd->sdcd, SIG_EN | serdisp_ks0108_internal_getStruct(dd)->chipselect[ctr], 
                     dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
        SDCONN_usleep(dd->sdcd, 5);
      }
      break;
    default:
      break;
  }
}


/* *********************************
   void serdisp_ks0108_transfer_autoclock(dd, type, item)
   *********************************
   transfer a data or command byte to the display
   *********************************
   dd         ... display descriptor
   type       ... one if TRANSFER_CMD, _DATA, _RESET
   item       ... byte to be processed
*/
void serdisp_ks0108_transfer_autoclock(serdisp_t* dd, int type, byte item) {
  long t_item;

  switch (type) {
    case TRANSFER_CMD:
    case TRANSFER_DATA:
      t_item = ((type == TRANSFER_CMD) ? 0x00010000L : 0x0L) + item;
      SDCONN_write(dd->sdcd, t_item, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      break;
    case TRANSFER_RESET:
      break;
    default:
      break;
  }
}




/* *********************************
   void serdisp_ks0108_switchcs_parport(dd, controller)
   *********************************
   switch controller
   *********************************
   dd         ... display descriptor
   controller ... id of controller
*/
void serdisp_ks0108_switchcs_parport(serdisp_t* dd, byte controller) {
  /* only switch if differing from prior controller id */
  if (serdisp_ks0108_internal_getStruct(dd)->currcs != controller) {

    /* swing over to next controller */
    serdisp_ks0108_internal_getStruct(dd)->currcs = controller;

    SDCONN_writedelay(dd->sdcd, 
                      serdisp_ks0108_internal_getStruct(dd)->chipselect[
                       serdisp_ks0108_internal_getStruct(dd)->currcs
                      ],
                      dd->sdcd->io_flags_writecmd, dd->delay >> 2);
  }
}



/* *********************************
   void serdisp_ks0108_switchcs_autoclock(dd, controller)
   *********************************
   switch controller
   *********************************
   dd         ... display descriptor
   controller ... id of controller
*/
void serdisp_ks0108_switchcs_autoclock(serdisp_t* dd, byte controller) {
  /* only switch if differing from prior controller id */
  if (serdisp_ks0108_internal_getStruct(dd)->currcs != controller) {

    /* swing over to next controller */
    serdisp_ks0108_internal_getStruct(dd)->currcs = controller;

    /* small hack to avoid erraneous pixels when switching CS-lines (CMD_STRTLINEADR used as NOP) */
    /* serdisp_ks0108_internal_getStruct(dd)->fp_transfer(dd, TRANSFER_CMD, CMD_STRTLINEADR); */

    /*            ctrl. byte stat.byte  data byte
     * xxxx xxxx  0000 0010  ---- ----  cccc cccc 
     *                   ^-------------------------- indicates chip select, value -> data byte
     */

    SDCONN_writedelay(dd->sdcd, 
                      0x00020000L | ((long)(serdisp_ks0108_internal_getStruct(dd)->currcs)),
                      dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata, 0);

  }
}







