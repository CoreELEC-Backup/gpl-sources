/*
 *************************************************************************
 *
 * serdisp_specific_pcd8544.c
 * routines for controlling the following monochrome displays (found in some nokia phones)
 * * LPH7366, LPH7677, LPH7779 (controller: PCD8544): 84x48
 * * LPH7690 (controller unknown): 96x60
 * * controller PCF8511: 96x64
 *
 *************************************************************************
 *
 * copyright (C) 2003-2010  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
 *
 * PCF8511 support and 9bit serial communication contributed by:
 * Copyright (C) 2006-2007 Jeroen Domburg
 * email     serdisp@jeroen.ietsmet.nl
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
/*#include "serdisplib/serdisp_specific_pcd8544.h"*/
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"


#define PutData(_dd, _data) serdisp_pcd8544_transfer((_dd), (1), (_data))
#define PutCtrl(_dd, _data) serdisp_pcd8544_transfer((_dd), (0), (_data))

/*
 * constants
 */


#define SIG_SI          (dd->sdcd->signals[0])
#define SIG_SCL         (dd->sdcd->signals[1])
#define SIG_DC          (dd->sdcd->signals[2])
#define SIG_CS          (dd->sdcd->signals[5])
#define SIG_RESET       (dd->sdcd->signals[3])
#define SIG_BACKLIGHT   (dd->sdcd->signals[4])
#define SIG_CS          (dd->sdcd->signals[5])


/* different display types/models supported by this driver */
#define DISPID_PCD8544  1
#define DISPID_LPH7366  2
#define DISPID_LPH7690  3
#define DISPID_PCF8511  4


serdisp_wiresignal_t serdisp_pcd8544_wiresignals[] = {
 /*  type   signame   actlow   cord  index */
   {SDCT_PP, "SI",         0,   'D',    0 }
  ,{SDCT_PP, "SCL",        0,   'C',    1 }
  ,{SDCT_PP, "DC",         0,   'D',    2 }
  ,{SDCT_PP, "RESET",      1,   'D',    3 }
  ,{SDCT_PP, "BACKLIGHT",  0,   'D',    4 }
  ,{SDCT_PP, "CS",         1,   'D',    5 }
};


serdisp_wiredef_t serdisp_pcd8544_wiredefs[] = {
   {  0, SDCT_PP, "SerDispBG",      "SI:D0,SCL:D1,DC:D2,RESET:D5,BACKLIGHT:D7", "SerDisp wiring, w/ backlight"}
  ,{  1, SDCT_PP, "SerDisp",        "SI:D0,SCL:D1,DC:D2,RESET:D5",              "SerDisp wiring, w/o backlight"}
  ,{  2, SDCT_PP, "SerDispBGHWRes", "SI:D0,SCL:D1,DC:D2,BACKLIGHT:D7",          "SerDisp wiring, w/ hardware reset, w/ backlight"}
  ,{  3, SDCT_PP, "SerDispHWRes",   "SI:D0,SCL:D1,DC:D2",                       "SerDisp wiring, w/ hardware reset, w/o backlight"}
  ,{  4, SDCT_PP, "Rifer",          "1:D2,1:D3,SI:nSTRB,SCL:nLINEFD,DC:D0,RESET:D1", "Rifer wiring"}
  ,{  5, SDCT_PP, "Tuto3310",       "1:D5,SI:D1,SCL:D0,DC:D4,RESET:D2",         "Tuto3310 (David) wiring"}
};

serdisp_wiredef_t serdisp_pcf8511_wiredefs[] = {
   {  0, SDCT_PP, "SerDispBG",      "SI:D0,SCL:D1,CS:D2,RESET:D5,BACKLIGHT:D7", "SerDisp wiring, w/ backlight"}
  ,{  1, SDCT_PP, "SerDisp",        "SI:D0,SCL:D1,CS:D2,RESET:D5",              "SerDisp wiring, w/o backlight"}
  ,{  2, SDCT_PP, "SerDispBGHWRes", "SI:D0,SCL:D1,CS:D2,BACKLIGHT:D7",          "SerDisp wiring, w/ hardware reset, w/ backlight"}
  ,{  3, SDCT_PP, "SerDispHWRes",   "SI:D0,SCL:D1,CS:D2",                       "SerDisp wiring, w/ hardware reset, w/o backlight"}
};


serdisp_options_t serdisp_pcd8544_options[] = {
   /*  name       aliasnames min  max mod int defines  */
   {  "DELAY",     "",         0,  -1,  1, 1,  ""}
  ,{  "CONTRAST",  "",         0,  10,  1, 1,  ""}
  ,{  "BRIGHTNESS","",         0, 100,  1, 1,  ""}      /* brightness [0 .. 100] */
  ,{  "BACKLIGHT", "",         0,   1,  1, 1,  ""}
};


/* private functions */
static void serdisp_pcd8544_init      (serdisp_t*);
static void serdisp_pcd8544_update    (serdisp_t*);
static int  serdisp_pcd8544_setoption (serdisp_t*, const char*, long);
static void serdisp_pcd8544_close     (serdisp_t*);

static void serdisp_pcd8544_transfer  (serdisp_t*, int, byte);


/* *********************************
   serdisp_t* serdisp_pcd8544_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle (not used in here)
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_pcd8544_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "serdisp_pcd8544_setup(): cannot allocate display descriptor");
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));


  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("PCD8544", dispname))
    dd->dsp_id = DISPID_PCD8544;
  else if (serdisp_comparedispnames("LPH7366", dispname))
    dd->dsp_id = DISPID_LPH7366;
  else if (serdisp_comparedispnames("LPH7690", dispname))
    dd->dsp_id = DISPID_LPH7690;
  else if (serdisp_comparedispnames("PCF8511", dispname))
    dd->dsp_id = DISPID_PCF8511;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_pcd8544.c", dispname);
    return (serdisp_t*)0;
  }


  dd->width             = 84;
  dd->height            = 48;
  dd->depth             = 1;
  dd->dsparea_width     = 29000;     /* display area in micrometres (datasheet lph7366) */
  dd->dsparea_height    = 19500;
  dd->min_contrast      = 50;        /* values < 50: display is unreadable */
  dd->max_contrast      = 0x7F;
  dd->feature_contrast  = 1;
  dd->feature_backlight = 1;
  dd->feature_invert    = 1;
  dd->curr_rotate       = 0;         /* unrotated display */
  dd->curr_backlight    = 1;         /* start with backlight on */
  dd->connection_types  = SERDISPCONNTYPE_PARPORT;
  dd->fp_init           = &serdisp_pcd8544_init;
  dd->fp_update         = &serdisp_pcd8544_update;
  dd->fp_setoption      = &serdisp_pcd8544_setoption;
  dd->fp_close          = &serdisp_pcd8544_close;

  dd->delay             = 0;

  if (dd->dsp_id == DISPID_PCF8511) {
    serdisp_setupstructinfos(dd, serdisp_pcd8544_wiresignals, serdisp_pcf8511_wiredefs, serdisp_pcd8544_options);
  } else {
    serdisp_setupstructinfos(dd, serdisp_pcd8544_wiresignals, serdisp_pcd8544_wiredefs, serdisp_pcd8544_options);
  }


  if (dd->dsp_id == DISPID_LPH7690) {
    int i;

    dd->width             = 96;
    dd->height            = 60;
    dd->dsparea_width     = 30500;  /* display area in micrometres (measured) */
    dd->dsparea_height    = 24000;
    dd->min_contrast      = 0x17;   /* values < 0x17: display is unreadable */
    dd->max_contrast      = 0x5F;   /* values > 0x5F: display is too dark */


    if (! (dd->yreloctab = (int*) sdtools_malloc( sizeof(int) * (dd->height + dd->ycolgaps) ) ) ) {
      sd_error(SERDISP_EMALLOC, "serdisp_pcd8544_setup(): cannot allocate relocation table");
      free(dd);
      dd = 0;
      return (serdisp_t*)0;
    }

    /* first row on display == second 'row' (second bit-row in page 0) in display memory */
    /* this is valid here because height = 60 (60/8 = 7.5 => 4 rows available for y-shifting */
    /* if i+shiftvalue would exceed that, out of bound errors would occur when calculating index in screen change buffer */
    for (i = 0; i < dd->height; i++)
      dd->yreloctab[i] = i+1;
  } else if (dd->dsp_id == DISPID_PCF8511) {
    dd->width             = 96;
    dd->height            = 64;
    dd->dsparea_width     = 0;        /* not known yet*/
    dd->dsparea_height    = 0;
    dd->min_contrast      = 2;
    dd->max_contrast      = 6;
  }

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    serdisp_freeresources(dd);
    dd = 0;
    return (serdisp_t*)0;    
  }

  return dd;
}



/* *********************************
   void serdisp_pcd8544_init(dd)
   *********************************
   initialise a pcd8544-based display
   *********************************
   dd     ... display descriptor
*/
void serdisp_pcd8544_init(serdisp_t* dd) {

  if (SIG_RESET)
    SDCONN_write(dd->sdcd, SIG_RESET, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
  SDCONN_usleep(dd->sdcd, 300*1000);


  if (dd->dsp_id == DISPID_PCF8511) {
    PutCtrl(dd, 0xE1);               /* exit powersave */
    PutCtrl(dd, 0x2F);               /* hvgen on */
    PutCtrl(dd, 0x81);               /* set vop: */
    PutCtrl(dd, 0x3F);               /* vop */
    PutCtrl(dd, 0x20+5);             /* set voff */
    PutCtrl(dd, 0x64+3);             /* set mult fact */
    PutCtrl(dd, 0xA4);               /* no all pixels on mode */
    PutCtrl(dd, 0xAF);               /* display on */
    PutCtrl(dd, 0xAF);
  } else {
    PutCtrl(dd, 0x21);               /* EXTENDEDSET: following commands are extended ones */
    PutCtrl(dd, 0xC8);               /* VOLTAGE:     set voltage */
    PutCtrl(dd, 0x06);               /* TEMPCOEFF:   set temp. coefficent */
    if (dd->dsp_id == DISPID_LPH7690)
      PutCtrl(dd, 0x17);             /* BIAS:        set bias value for LPH7690 */
    else 
      PutCtrl(dd, 0x13);             /* BIAS:        set bias value for other displays */
    PutCtrl(dd, 0x20);               /* STANDARDSET: following commands are standard ones */
    PutCtrl(dd, 0x09);               /* DISPBLACK:   switch on display and set to all pixels on */
    PutCtrl(dd, 0x08);               /* DISPOFF:     switch off display */
    PutCtrl(dd, 0x0C);               /* DISPNORM:    switch on display and set to normal mode */
    PutCtrl(dd, 0x40 | 0);           /* SPAGEADDR:   page address (0x40 + value) */
    PutCtrl(dd, 0x80 | 0);           /* SCOLADDR:    column address (0x80 + value) */
    PutCtrl(dd, 0x0C);               /* DISPNORM:  switch on display and set to normal mode */
  }
  sd_debug(2, "serdisp_pcd8544_init(): done with initialising");
}


/* *********************************
   void serdisp_pcd8544_transfer(dd, dc, data)
   *********************************
   transfer a data or command byte to the display
   *********************************
   dd     ... display descriptor
   dc     ... dc = 1: data; dc = 0: command
   data   ... byte to be processed 
*/
void serdisp_pcd8544_transfer(serdisp_t* dd, int dc, byte data) {
  long td;       /* 'signal byte': contains all signal bits controlling the display */
  long td_clk;   /* same as td, but with clock-signal enabled */
  int t_data;    /* data byte (9bit: + data/command bit) */
  int i;
  int mode;      /* 8 or 9 bit */
  int checkmask; /* 8bit: 0x80; 9bit: 0x100 */
  int shiftmask; /* bits not to clear when shifting bits */

  td = 0;

  t_data = (int)data;

  if (SIG_DC) {   /* SIG_DC defined => 8bit serial communication protocol */
    mode = 8;
    checkmask = 0x80;   /* test if bit 7 is set */
    shiftmask = 0x7F;
    t_data = data;
    if (dc) 
      td |= SIG_DC;   /* if data => DC == high */
  } else {        /* SIG_DC not defined => 9bit serial comunication protocol:  1bit D/C + 8bit data */
    mode = 9;
    checkmask = 0x100;  /* test if bit 8 is set */
    shiftmask = 0x0FF;
    if (dc) 
      t_data |= (1 << 8);
  }

  if (dd->feature_backlight && dd->curr_backlight)
    td |= SIG_BACKLIGHT;

  /* loop through all 8 bits and transfer them to the display */
  /* optrex and pcd8544 start with bit 7 (MSB) */
  /* PCF8155: 1bit D/C + 8bit data, bit 7 == MSB */
  for (i = 0; i <= mode-1; i++) {
    /* write a single bit to PP_SI */
    if (t_data & checkmask)  /* bit == 1 */
      td |= SIG_SI;
    else                /* bit == 0 */
      td &= (0xffffffff - SIG_SI);

    /* clock => high */
    td_clk = td | SIG_SCL;

    /* write content to display */
    SDCONN_write(dd->sdcd, td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
    sdtools_nsleep(dd->delay);

    /* set clock to high */
    SDCONN_write(dd->sdcd, td_clk, dd->sdcd->io_flags_writecmd);
    sdtools_nsleep(dd->delay);

    /* set clock to low */
    SDCONN_write(dd->sdcd, td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
    sdtools_nsleep(dd->delay);

    /* shift byte so that next bit is on the first (MSB) position */
    t_data = (t_data & shiftmask) << 1;
  }
  
  if (dd->dsp_id == DISPID_PCF8511) {
    td &= ~SIG_CS;
    SDCONN_write(dd->sdcd, td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
    sdtools_nsleep(dd->delay);
  }
}



/* *********************************
   void serdisp_pcd8544_update(dd)
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
void serdisp_pcd8544_update(serdisp_t* dd) {
  int x, page;
  int pages = (dd->height+7)/8;

  /* set page address  SPAGEADDR + value */
  byte cmd_spageaddr     = (dd->dsp_id == DISPID_PCF8511) ? 0xB0 : 0x40;
  /* set column address, high nibble (SCOLADDRHI + value) */
  byte cmd_scoladdrhi    = (dd->dsp_id == DISPID_PCF8511) ? 0x10 : 0;
  /* set column address, low nibble if split in two parts  (SCOLADDR + value) */
  byte cmd_scoladdr      = (dd->dsp_id == DISPID_PCF8511) ? 0x00 : 0x80;
  /* pages are shifted on displays with PCF8511 like this: page 2,3,4,5,6,7,0,1 */
  int  pageshift         = (dd->dsp_id == DISPID_PCF8511) ? 2 : 0;


#ifdef OPT_USEOLDUPDATEALGO
  for (page = 0; page < pages; page++) {
    PutCtrl(dd, cmd_spageaddr | ((page + pageshift) & 7) ); 
    for(x = 0; x < dd->width; x++) {
      if (x == 0) {  /* set x-pos to 0 */
        if (dd->dsp_id == DISPID_PCF8511)
          PutCtrl(dd, cmd_scoladdrhi);
        PutCtrl(dd, cmd_scoladdr);
      }

      PutData(dd, dd->scrbuf[ dd->width * page  +  x]);
      dd->scrbuf_chg[x] &= 0xFF - (1 << page) ;
    }
  }
#else /* OPT_USEOLDUPDATEALGO */
  int set_page, last_x;
#ifdef DBG_BENCHMARK
  int cntPCp = 0, cntPCc = 0, cntPD = 0;
#endif
  
  for (page = 0; page < pages; page++) {
    last_x = -2;
    set_page = 1;

#ifdef DBG_SCRBUFCHG
fprintf(stderr, "[%d] ", page);
#endif
    for(x = 0; x < dd->width; x++) {

#ifdef DBG_SCRBUFCHG
fprintf(stderr, ((dd->scrbuf_chg[x]) & ( 1 << page) ) ? "." : " ");
#endif

      /* either actual_x or actual_x + 1 has changed  or one of left/right-most */
      if ( dd->scrbuf_chg[x] & ( 1 << page) ) {
        if (x > last_x+1 ) {

          if (set_page) {
            PutCtrl(dd, cmd_spageaddr | ((page + pageshift) & 7) );
#ifdef DBG_BENCHMARK
            cntPCp++;
            fprintf(stderr, "P");
#endif
            set_page = 0;
          }

          /* x-position may be written directly to pcd8544 & co, but pcf85511 needs two cycles */
          if (dd->dsp_id == DISPID_PCF8511) {
            PutCtrl(dd, cmd_scoladdrhi | (x >> 4) );  /* write high nibble */
            PutCtrl(dd, cmd_scoladdr   | (x & 0xF));  /* write low nibble */
          } else {
            PutCtrl(dd, cmd_scoladdr   | x); 
          }

#ifdef DBG_BENCHMARK
            cntPCc+=2;
            fprintf(stderr, "C");
#endif
        }
        PutData(dd, dd->scrbuf[ dd->width * page  +  x]);
#ifdef DBG_BENCHMARK
            cntPD++;
            fprintf(stderr, ".");
#endif
        dd->scrbuf_chg[x] &= (0xFF ^ (1 << page)) ;

        last_x = x;   
      }
          
    }
#ifdef DBG_SCRBUFCHG
fprintf(stderr, "\n");
#endif
#ifdef DBG_BENCHMARK
fprintf(stderr, "\n");
#endif
  }    
#ifdef DBG_BENCHMARK
fprintf(stderr, "P: %2d  C: %2d  D: %2d\n", cntPCp, cntPCc, cntPD);
#endif

#endif
  
  /* w/o the following NOP, PCD8544 displays don't finalize the last data-operation (as it seems) */
  if (! dd->dsp_id == DISPID_PCF8511)
    PutCtrl(dd, 0x00);     /* NOP */
}



/* *********************************
   int serdisp_pcd8544_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_pcd8544_setoption(serdisp_t* dd, const char* option, long value) {
  byte cmd_nop, cmd_invert, cmd_normal;

  switch(dd->dsp_id) {
    case DISPID_PCF8511:
      cmd_nop    = 0x45;  /* NOP */
      cmd_invert = 0xA7;  /* REVERSE */
      cmd_normal = 0xA6;  /* NOREVERSE */
      break;
    default:  /* all others */
      cmd_nop    = 0x00;  /* NOP */
      cmd_invert = 0x0D;  /* REVERSE */
      cmd_normal = 0x0C;  /* NOREVERSE */
  }    
    
  PutCtrl(dd, cmd_nop);   /* NOP */

  if (dd->feature_invert && serdisp_compareoptionnames(dd, option, "INVERT") ) {
    if (value < 2) 
      dd->curr_invert = (int)value;
    else
      dd->curr_invert = (dd->curr_invert) ? 0 : 1;
    PutCtrl(dd, (dd->curr_invert) ? cmd_invert : cmd_normal);
  } else if (dd->feature_backlight && serdisp_compareoptionnames(dd, option, "BACKLIGHT" )) {
    if (value < 2) 
      dd->curr_backlight = (int)value;
    else
      dd->curr_backlight = (dd->curr_backlight) ? 0 : 1;
    /* no command for en/disable backlight, so issue 'dummy'-command
       (which indirectly enables/disabled backlight) */
    PutCtrl(dd, cmd_nop);    /* NOP */
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

    if (dd->dsp_id == DISPID_PCF8511) {
      PutCtrl(dd, 0x81); /* set vop: */
      PutCtrl(dd, 0x3F); /* vop */
      PutCtrl(dd, 0x20 + dimmed_contrast /*dd->curr_contrast*/ );
    } else {
      PutCtrl(dd, 0x21);                       /* EXTENDEDSET: following command is an extended one */
      PutCtrl(dd, 0x80 | dimmed_contrast );    /* 0x80+value:  set contrast */
      PutCtrl(dd, 0x20);                       /* STANDARDSET: following commands are standard ones again */
    }
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  return 1;
}



/* *********************************
   void serdisp_pcd8544_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_pcd8544_close(serdisp_t* dd) {
  byte cmd_dispoff = (dd->dsp_id == DISPID_PCF8511) ? 0xAE : 0x08;   /* DISPOFF */

  PutCtrl(dd, cmd_dispoff);    /*DISPOFF: display off */
  if (SIG_RESET)
    SDCONN_write(dd->sdcd, SIG_RESET, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
}


