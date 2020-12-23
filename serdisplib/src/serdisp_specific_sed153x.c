/*
 *************************************************************************
 *
 * serdisp_specific_sed153x.c
 * routines for controlling sed153x-based displays 
 * (eg: optrex323, Alps LSU7S1011A a.k.a. 'pollin'-display)
 *
 *************************************************************************
 *
 * copyright (C) 2003-2010  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
 *
 *************************************************************************
 *
 * initially based on:
 *   http://www.thiemo.net/projects/orpheus/optrex/
 *
 *************************************************************************
 *
 * contributions:
 *
 * initial EPSON E08552 display support by Tobias Stoeber
 * (http://www.to-st.de/public/linux/lcd/e08552/)
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
/*#include "serdisplib/serdisp_specific_sed153x.h"*/
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"


#undef OPT_USEOLDUPDATEALGO

#define PutData(_dd, _data) serdisp_sed153x_transfer((_dd), (1), (_data))
#define PutCtrl(_dd, _data) serdisp_sed153x_transfer((_dd), (0), (_data))

/*
 * command constants
 */

#define CMD_NOP         0x00

#define CMD_DISPLAYON   0xAF
#define CMD_DISPLAYOFF  0xAE

#define CMD_REVERSE     0xA7
#define CMD_NOREVERSE   0xA6

#define CMD_ALLBLACK    0xA5
#define CMD_NOALLBLACK  0xA4

#define INI_STARTLINE   0x40

#define CMD_STRTLINEADR 0xA0
#define CMD_SPAGEADR    0xB0
#define CMD_SCOLHIADR   0x10
#define CMD_SCOLLOADR   0x00

#define CMD_RESET       0xE2

/*
 * values used mainly for initialization
 */

#define INI_BIAS_1DIV5  0xA2
#define INI_BIAS_1DIV6  0xA3

#define INI_ADCNORMAL   0xA0
#define INI_ADCREVERSE  0xA1

#define INI_COMDIRNORM  0xC0
#define INI_COMDIRREV   0xC8

#define INI_V5_CONTR    0x80
#define INI_POW_CONTR   0x2F  /* pollin documentation */

/* par interface only */
#define SIG_D0          (dd->sdcd->signals[0])
#define SIG_D1          (dd->sdcd->signals[1])
#define SIG_D2          (dd->sdcd->signals[2])
#define SIG_D3          (dd->sdcd->signals[3])
#define SIG_D4          (dd->sdcd->signals[4])
#define SIG_D5          (dd->sdcd->signals[5])
#define SIG_D6          (dd->sdcd->signals[6])
#define SIG_D7          (dd->sdcd->signals[7])

#define SIG_A0          (dd->sdcd->signals[8])
#define SIG_RD          (dd->sdcd->signals[9])
#define SIG_WR          (dd->sdcd->signals[10])

/* both interface */
#define SIG_CS          (dd->sdcd->signals[11])
#define SIG_RESET       (dd->sdcd->signals[12])
#define SIG_BACKLIGHT   (dd->sdcd->signals[13])

/* serial interface */
#define SIG_SI          (dd->sdcd->signals[14])
#define SIG_SCL         (dd->sdcd->signals[15])
#define SIG_DC          (dd->sdcd->signals[16])
#define SIG_ICS         (dd->sdcd->signals[17])

/* interface method */
#define INTERFACE_SERIAL    0
#define INTERFACE_PARALLEL  1

/* different display types/models supported by this driver */
#define DISPID_OPTREX323   1
#define DISPID_LSU7S1011A  2
#define DISPID_E08552      3

serdisp_wiresignal_t serdisp_sed153x_wiresignals[] = {
 /*  type   signame   actlow   cord  index */
  /* serial data transfer */
   {SDCT_PP, "SI",         0,   'D',    14 }
  ,{SDCT_PP, "SCL",        0,   'C',    15 }
  ,{SDCT_PP, "DC",         0,   'D',    16 }
  ,{SDCT_PP, "ICS",        1,   'D',    17 }
  /* parallel data transfer */
  ,{SDCT_PP, "A0",         0,   'C',     8 }
  ,{SDCT_PP, "WR",         1,   'C',    10 }
  ,{SDCT_PP, "RD",         1,   'C',     9 }
  /* used with both */
  ,{SDCT_PP, "CS",         0,   'D',    11 }
  ,{SDCT_PP, "RESET",      1,   'D',    12 }
  ,{SDCT_PP, "BACKLIGHT",  0,   'D',    13 }
};


serdisp_wiredef_t serdisp_sed153x_wiredefs[] = {
   {  0, SDCT_PP, "Optrex", "SI:D0,SCL:D1,DC:D2,CS:D3,ICS:D4,RESET:D5,BACKLIGHT:D7", "Optrex wiring, w/ backlight"}
  ,{  1, SDCT_PP, "Alps",   "SI:D0,SCL:D1,DC:D2,CS:D3,ICS:D4,RESET:D5", "Pollin wiring, w/o backlight"}
  ,{  2, SDCT_PP, "Epson",  "DATA8,A0:nSELIN,WR:nSTRB,RESET:INIT,BACKLIGHT:nAUTO", "Parallel Data Transfer - Pollin Wiring"}
};


serdisp_options_t serdisp_sed153x_options[] = {
   /*  name       aliasnames min  max mod int defines  */
   {  "DELAY",     "",         0,  -1,  1, 1,  ""}
  ,{  "CONTRAST",  "",         0,  10,  1, 1,  ""}
  ,{  "BACKLIGHT", "",         0,   1,  1, 1,  ""}
  ,{  "BRIGHTNESS", "",        0, 100,  1, 1,  ""}      /* brightness [0 .. 100] */
};


/* internal typedefs and functions */

static void serdisp_sed153x_init       (serdisp_t*);
static void serdisp_sed153x_update     (serdisp_t*);
static int  serdisp_sed153x_setoption  (serdisp_t*, const char*, long);
static void serdisp_sed153x_close      (serdisp_t*);
static void serdisp_sed153x_transfer   (serdisp_t*, int, byte);


typedef struct serdisp_sed153x_specific_s {
  int   interfacemode;
} serdisp_sed153x_specific_t;

static serdisp_sed153x_specific_t* serdisp_sed153x_internal_getStruct(serdisp_t* dd) {
  return (serdisp_sed153x_specific_t*)(dd->specific_data);
}



/* *********************************
   serdisp_t* serdisp_sed153x_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle (not used in here)
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_sed153x_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "serdisp_sed153x_setup(): cannot allocate display descriptor");
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));

  /* "SED153x"-based displays supported in here (eg. optrex322, Alps LSU7S1011A a.k.a. 'pollin'-display)  */
  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("OPTREX323", dispname))
    dd->dsp_id = DISPID_OPTREX323;
  else if (serdisp_comparedispnames("LSU7S1011A", dispname))
    dd->dsp_id = DISPID_LSU7S1011A;
  else if (serdisp_comparedispnames("E08552", dispname))
    dd->dsp_id = DISPID_E08552;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_sed153x.c", dispname);
    return (serdisp_t*)0;
  }

  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_sed153x_specific_t)) )) {
    sd_error(SERDISP_EMALLOC, "serdisp_sed153x_setup(): cannot allocate specific display descriptor");
    free(dd);
    return (serdisp_t*)0;
  }
  memset(dd->specific_data, 0, sizeof(serdisp_sed153x_specific_t));


  /* set function pointers */
  dd->fp_init           = &serdisp_sed153x_init;
  dd->fp_update         = &serdisp_sed153x_update;
  dd->fp_setoption      = &serdisp_sed153x_setoption;
  dd->fp_close          = &serdisp_sed153x_close;

  dd->width             = 96;
  dd->height            = 32;
  dd->depth             = 1;
  dd->startxcol         = 18;
  dd->min_contrast      = 0;
  dd->max_contrast      = 0x1F;
  dd->feature_contrast  = 1;
  dd->feature_invert    = 1;
  dd->curr_rotate       = 0;         /* unrotated display */
  dd->connection_types  = SERDISPCONNTYPE_PARPORT;

  /* default interface mode */
  serdisp_sed153x_internal_getStruct(dd)->interfacemode = INTERFACE_SERIAL;

  /* optrex 323 */
  if (dd->dsp_id == DISPID_OPTREX323) {
    dd->width             = 117;
    dd->height            = 62;
    dd->startxcol         = 0;
    dd->xcolgaps          = 4;    /* optrex needs 4 more extra entries in xreloctab */
    dd->feature_backlight = 1;
    dd->curr_backlight    = 1;    /* start with backlight on */
  }

  /* Epson E0855-2 display area is 96 pixels wide and starts shifted by 16 columns !! */
  if (dd->dsp_id == DISPID_E08552) {
    dd->width             = 96;
    dd->height            = 32;
    dd->startxcol         = 16;
    dd->feature_backlight = 1;
    dd->curr_backlight    = 1;      /* start with backlight on */
    dd->dsparea_width     = 33000;  /* measured */
    dd->dsparea_height    = 15000; 
    dd->max_contrast      = 0x1F;
    dd->feature_invert    = 0;      /* set to unsupported, else symbols get inverted too!! */
    dd->delay             = 2;

    serdisp_sed153x_internal_getStruct(dd)->interfacemode = INTERFACE_PARALLEL;
  }

  /* wiring and options */
  serdisp_setupstructinfos(dd, serdisp_sed153x_wiresignals, serdisp_sed153x_wiredefs, serdisp_sed153x_options);

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    serdisp_freeresources(dd);
    dd = 0;
    return (serdisp_t*)0;    
  }

  /* allocate relocation tables */
  if (dd->dsp_id != DISPID_E08552) {
    if (! (dd->xreloctab = (int*) sdtools_malloc( sizeof(int) * (dd->width + dd->xcolgaps) ) ) ) {
      sd_error(SERDISP_EMALLOC, "serdisp_sed153x_setup(): cannot allocate relocation table");
      free(dd);
      dd = 0;
      return (serdisp_t*)0;
    }
    if (! (dd->yreloctab = (int*) sdtools_malloc( sizeof(int) * (dd->height + dd->ycolgaps) ) ) ) {
      sd_error(SERDISP_EMALLOC, "serdisp_sed153x_setup(): cannot allocate relocation table");
      free(dd->xreloctab);
      free(dd);
      dd = 0;
      return (serdisp_t*)0;
    }
  }


  /* set up relocation tables */
  if (dd->dsp_id == DISPID_LSU7S1011A) {
    /* translations for LSU7S1011A
 
       x-axis: no translation needed
       y-axis: (0-31) -> (15-0,16-31)
    */
    int j;
    for (j = 0; j < dd->width; j++)
      dd->xreloctab[j] = j;

    /* rows 0 to 15 are mirrored, rows >= 16 are not */
    for (j = 0; j < 16; j++)
      dd->yreloctab[j] = 15-j;
    for (j = 16; j < dd->height; j++)
      dd->yreloctab[j] = j;
    
  } else if (dd->dsp_id == DISPID_OPTREX323) {
    int j;
    /* translations for optrex 323
 
       x-axis: (0-116) -> (117,0-114,118) + unusable (double-sized) columns
       y-axis: (0-61)  -> (54-57,0-53,58-61)
    */
    dd->xreloctab[0] = 117;
    for (j = 1; j < 116; j++) 
      dd->xreloctab[j] = j-1;
    dd->xreloctab[116] = 118;
    /* unusable extra columns */
    dd->xreloctab[117] = 115;
    dd->xreloctab[118] = 116;
    dd->xreloctab[119] = 119;
    dd->xreloctab[120] = 120;

    for (j = 0; j < 4; j++)
      dd->yreloctab[j] = j + 54;
    for (j = 4; j < 58; j++)
      dd->yreloctab[j] = j - 4;
    for (j = 58; j < 62; j++)
      dd->yreloctab[j] = j;
  }

  return dd;
}



/* *********************************
   void serdisp_sed153x_init(dd)
   *********************************
   initialise a sed153x-based display
   *********************************
   dd     ... display descriptor
*/
void serdisp_sed153x_init(serdisp_t* dd) {
  dd->feature_backlight = (SIG_BACKLIGHT) ? 1 : 0;
  
  /* the initialisation and command reference can be found in the datasheet of SED153x */

  if (serdisp_sed153x_internal_getStruct(dd)->interfacemode == INTERFACE_SERIAL) {
    /* reset */
    if (SIG_RESET) {  /* reset signal sent by parport */
      SDCONN_write(dd->sdcd, SIG_RESET | SIG_ICS, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      SDCONN_usleep(dd->sdcd, 5);
      SDCONN_write(dd->sdcd, 0, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      SDCONN_usleep(dd->sdcd, 5);
    } else {  /* reset signal generated by R/C circuit */
      SDCONN_usleep(dd->sdcd, 5);
      SDCONN_write(dd->sdcd, 0, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      SDCONN_usleep(dd->sdcd, 5);
    }

    if (dd->dsp_id == DISPID_LSU7S1011A) {
      PutCtrl(dd, CMD_STRTLINEADR);        /* start display line = 0*/
      PutCtrl(dd, INI_ADCREVERSE);         /* ADC select */
    /*  PutCtrl(dd, INI_ADCNORMAL);   */   /* ADC select */
      PutCtrl(dd, INI_BIAS_1DIV6);         /* set bias to 1/7 */
      PutCtrl(dd, INI_COMDIRNORM);         /* Common Output Mode Select */

      PutCtrl(dd, INI_V5_CONTR | 0x0F);    /* contrast */
      PutCtrl(dd, INI_POW_CONTR);          /* power on */ 
      PutCtrl(dd, CMD_NOALLBLACK);         /* no display test */ 

      PutCtrl(dd, CMD_DISPLAYON);

      PutCtrl(dd, CMD_NOREVERSE);
    } else {

      PutCtrl(dd, CMD_NOP);
      PutCtrl(dd, INI_STARTLINE);
      PutCtrl(dd, INI_BIAS_1DIV5);
      PutCtrl(dd, INI_POW_CONTR);
      PutCtrl(dd, CMD_NOALLBLACK);
      PutCtrl(dd, CMD_DISPLAYON);
      PutCtrl(dd, INI_ADCREVERSE);
      PutCtrl(dd, CMD_NOP);

      PutCtrl(dd, CMD_NOALLBLACK);
      PutCtrl(dd, CMD_DISPLAYON);
    }
  } else if (serdisp_sed153x_internal_getStruct(dd)->interfacemode == INTERFACE_PARALLEL) {
    sd_debug(0, "serdisp_sed153x_init(): parallel initialising");

    /* reset */
    if (SIG_RESET) {  /* reset signal sent by parport */
      SDCONN_write(dd->sdcd, SIG_RESET, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      SDCONN_usleep(dd->sdcd, 5);
      SDCONN_write(dd->sdcd, 0, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      SDCONN_usleep(dd->sdcd, 5);
    } else {  /* reset signal generated by R/C circuit */
      SDCONN_usleep(dd->sdcd, 5);
      SDCONN_write(dd->sdcd, 0, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      SDCONN_usleep(dd->sdcd, 5);
    }

    /* Epson E0855-2 */
    if (dd->dsp_id == DISPID_E08552) {

      int i;
    
      PutCtrl(dd, INI_STARTLINE);          /* start display line */
      PutCtrl(dd, INI_ADCREVERSE);         /* ADC select */
      /* PutCtrl(dd, INI_ADCNORMAL); */         /* ADC select */
      PutCtrl(dd, INI_BIAS_1DIV5);         /* set bias to 1/5 */
      PutCtrl(dd, INI_COMDIRREV);          /* Common Output Mode Select */

      PutCtrl(dd, INI_POW_CONTR);          /* power on */ 

      PutCtrl(dd, INI_V5_CONTR | 0x0F);    /* contrast */
      PutCtrl(dd, CMD_NOALLBLACK);         /* no display test */ 

      PutCtrl(dd, CMD_DISPLAYON);

      PutCtrl(dd, CMD_NOREVERSE);
    
      /* clear all symbols */
      PutCtrl(dd, CMD_SPAGEADR | 8);
      for (i = 0; i < dd->width ; i++) {
        PutCtrl(dd, CMD_SCOLHIADR | ((i + dd->startxcol) >> 4));
        PutCtrl(dd, CMD_SCOLLOADR | ((i + dd->startxcol) & 0x0F));
        PutData(dd, 0);      
      } /* for */
    } /* if DISPID_E08552 */
  } /* (if-)else interfacemode INTERFACE_PARALLEL */

  sd_debug(2, "serdisp_sed153x_init(): done with initialising");
}


/* *********************************
   void serdisp_sed153x_transfer(dd, dc, data)
   *********************************
   transfer a data or command byte to the display
   *********************************
   dd     ... display descriptor
   dc     ... dc = 1: data; dc = 0: command
   data   ... byte to be processed 
*/
void serdisp_sed153x_transfer(serdisp_t* dd, int dc, byte data) {
  if (serdisp_sed153x_internal_getStruct(dd)->interfacemode == INTERFACE_SERIAL) {
    /* 'signal byte': contains all signal bits controlling the display */
    long td;
    /* same as td, but with clock-signal enabled */
    long td_clk;
    byte t_data;
    int i;

    t_data = data;

    td = 0;

#if 0
    /* pre-init 'signal byte' with all signals that are active high */
    if (SIG_RESET)
      td |= SIG_RESET;
#endif

    /* ICS -> high (will be inverted -> low)  ;  CS -> high*/
    td |= SIG_ICS | SIG_CS;

    if (dd->feature_backlight && dd->curr_backlight)
      td |= SIG_BACKLIGHT;

    /* if data => DC == high */
    if (dc)
      td |= SIG_DC;

    SDCONN_write(dd->sdcd, td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
    sdtools_nsleep(dd->delay);


    /* loop through all 8 bits and transfer them to the display */
    /* sed153x starts with bit 7 (MSB) */
    for (i = 0; i <= 7; i++) {
      /* write a single bit to SIG_SI */
      if (t_data & 0x80)  /* bit == 1 */
        td |= SIG_SI;
      else              /* bit == 0 */
        td &= (0xff - SIG_SI);

      /* clock => high */
      td_clk = td | SIG_SCL;

      /* write content to display */
      SDCONN_write(dd->sdcd, td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      sdtools_nsleep(dd->delay);

      /* set clock to high (bit is read on rising edge) */
      SDCONN_write(dd->sdcd, td_clk, dd->sdcd->io_flags_writecmd);
      sdtools_nsleep(dd->delay);

      /* set clock to low again */
      SDCONN_write(dd->sdcd, td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
      sdtools_nsleep(dd->delay);

      /* shift byte so that next bit is on the first (MSB) position */
      t_data = (t_data & 0x7f) << 1;
    }
    /* 'commit' */
    /* done (SIG_ICS -> low (will be inverted -> high), SIG_CS -> low) */
    td &= (0xff - SIG_ICS);
    td &= (0xff - SIG_CS);
    SDCONN_write(dd->sdcd, td, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
    sdtools_nsleep(dd->delay);
  } else if (serdisp_sed153x_internal_getStruct(dd)->interfacemode == INTERFACE_PARALLEL) {
    long item_split = 0;
    long td_clk1 = 0;
    long td_clk2 = 0;
    long td_clk3 = 0;
    long td_clk4 = 0;
    int i;

    /* E0855-2: turn backlight on ?? */
    td_clk1 |= SIG_WR ;
    td_clk2 |= SIG_WR ;
    td_clk3 |= SIG_WR ;

    if (dc) {
      td_clk1 |= SIG_A0;
      td_clk2 |= SIG_A0;
      td_clk3 |= SIG_A0;
      td_clk4 |= SIG_A0;
    }

    if (dd->feature_backlight && dd->curr_backlight) {
      td_clk1 |= SIG_BACKLIGHT;
      td_clk2 |= SIG_BACKLIGHT;
      td_clk3 |= SIG_BACKLIGHT;
      td_clk4 |= SIG_BACKLIGHT;
    }

    for (i = 0; i < 8; i++)
      if (data & (1 << i))
         item_split |= dd->sdcd->signals[i];

    td_clk2 |= item_split;
    td_clk3 |= item_split;

    SDCONN_write(dd->sdcd, td_clk1, dd->sdcd->io_flags_writecmd);
    sdtools_nsleep(dd->delay);
    SDCONN_write(dd->sdcd, td_clk2, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
    sdtools_nsleep(dd->delay);
    SDCONN_write(dd->sdcd, td_clk3, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
    sdtools_nsleep(dd->delay);
    SDCONN_write(dd->sdcd, td_clk4, dd->sdcd->io_flags_writecmd);
    sdtools_nsleep(dd->delay);
  }

  sdtools_nsleep(dd->delay);
}



/* *********************************
   void serdisp_sed153x_update(dd)
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
void serdisp_sed153x_update(serdisp_t* dd) {
  int x, page;
  int pages = (dd->height+7)/8;
  int data;

#ifdef OPT_USEOLDUPDATEALGO
  for (page = 0; page < pages; page++) {
    PutCtrl(dd, CMD_SPAGEADR | page);
    for(x = 0; x < dd->width + dd->xcolgaps; x++) {
      if (x == 0) {
          PutCtrl(dd, CMD_SCOLHIADR | ((x + dd->startxcol) >> 4));
          PutCtrl(dd, CMD_SCOLLOADR | ((x + dd->startxcol) & 0x0F));
      }

      data = dd->scrbuf[ (dd->width + dd->xcolgaps) * page  +  x];

      /* if display doesn't support hardware invert: software invert  */
      if (dd->curr_invert && !(dd->feature_invert))
        data = ~data;      

      PutData(dd, data);
      dd->scrbuf_chg[x + (dd->width + dd->xcolgaps) * (page/8)] &= 0xFF - (1 << (page%8)) ;
    }
  }    
#else /* ! OPT_USEOLDUPDATEALGO */
  int set_page, last_x;
#ifdef DBG_BENCHMARK
  int cntPCp = 0, cntPCc = 0, cntPD = 0;
#endif
  
  for (page = 0; page < pages; page++) {

    last_x = -2;
    set_page = 1;

    for(x = 0; x < dd->width + dd->xcolgaps; x++) {

#ifdef DBG_SCRBUFCHG
      fprintf(stderr, ((dd->scrbuf_chg[x + (dd->width + dd->xcolgaps) *(page/8)]) & ( 1 << (page%8)) ) ? "." : " ");
#endif

      /* either actual_x or actual_x + 1 has changed  or one of left/right-most */
      if ( dd->scrbuf_chg[x + (dd->width + dd->xcolgaps) *(page/8)] & ( 1 << (page%8)) ) {
        if (x > last_x+1 ) {

          if (set_page) {
            PutCtrl(dd, CMD_SPAGEADR | page);

#ifdef DBG_BENCHMARK
            cntPCp++;
            fprintf(stderr, "P");
#endif

            set_page = 0;
          }

          /* x-position may be written directly to sed153x */
          PutCtrl(dd, CMD_SCOLHIADR | ((x + dd->startxcol) >> 4));
          PutCtrl(dd, CMD_SCOLLOADR | ((x + dd->startxcol) & 0x0F));

#ifdef DBG_BENCHMARK
          cntPCc+=2;
          fprintf(stderr, "C");
#endif

        }

        data = dd->scrbuf[ (dd->width + dd->xcolgaps) * page  +  x];

        /* if display doesn't support hardware invert: software invert  */
        if (dd->curr_invert && !(dd->feature_invert))
          data = ~data;

        PutData(dd, data);

#ifdef DBG_BENCHMARK
        cntPD++;
        fprintf(stderr, ".");
#endif

        dd->scrbuf_chg[x + (dd->width + dd->xcolgaps) * (page/8)] &= 0xFF - (1 << (page%8)) ;

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

#endif /* OPT_USEOLDUPDATEALGO */

  /* w/o the following NOP, sed153x displays don't finalize the last data-operation (as it seems) */
  PutCtrl(dd, CMD_NOP);
}



/* *********************************
   int serdisp_sed153x_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_sed153x_setoption(serdisp_t* dd, const char* option, long value) {
  PutCtrl(dd, CMD_NOP);

  if (dd->feature_invert && serdisp_compareoptionnames(dd, option, "INVERT" )) {
    if (value < 2) 
      dd->curr_invert = (int)value;
    else
      dd->curr_invert = (dd->curr_invert) ? 0 : 1;
    PutCtrl(dd, (dd->curr_invert) ? CMD_REVERSE : CMD_NOREVERSE);
  } else if (dd->feature_backlight && serdisp_compareoptionnames(dd, option, "BACKLIGHT") ) {
    if (value < 2) 
      dd->curr_backlight = (int)value;
    else
      dd->curr_backlight = (dd->curr_backlight) ? 0 : 1;
    /* no command for en/disable backlight, so issue 'dummy'-command
       (which indirectly enables/disabled backlight) */
    PutCtrl(dd, CMD_NOP);
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

    PutCtrl(dd, INI_V5_CONTR | dimmed_contrast /*dd->curr_contrast*/);    /* contrast: max. value: 0x1F  */
/*    PutCtrl(dd, CMD_NOP );*/
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  return 1;
}



/* *********************************
   void serdisp_sed153x_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_sed153x_close(serdisp_t* dd) {
  PutCtrl(dd, CMD_DISPLAYOFF);
  /* reset */
  if (serdisp_sed153x_internal_getStruct(dd)->interfacemode == INTERFACE_SERIAL) {
    if (SIG_RESET) {  /* reset signal sent by parport */
      SDCONN_write(dd->sdcd, SIG_RESET | SIG_ICS, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
    } else {
      SDCONN_write(dd->sdcd,             SIG_ICS, dd->sdcd->io_flags_writecmd | dd->sdcd->io_flags_writedata);
    }
  } else if (serdisp_sed153x_internal_getStruct(dd)->interfacemode == INTERFACE_PARALLEL) {
    if (dd->feature_backlight)
      serdisp_setoption(dd, "BACKLIGHT", 0);
    PutCtrl(dd, CMD_RESET);                    /* reset */
  }

  SDCONN_usleep(dd->sdcd, 5);
}



