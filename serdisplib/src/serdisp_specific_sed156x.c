/*
 *************************************************************************
 *
 * serdisp_specific_sed156x.c
 * routines for controlling sed156x-based displays (eg.: nokia 7110 display)
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
/*#include "serdisplib/serdisp_specific_sed156x.h"*/
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"


/* #undef OPT_USEOLDUPDATEALGO */


#define PutData(_dd, _data) serdisp_sed156x_transfer((_dd), (1), (_data))
#define PutCtrl(_dd, _data) serdisp_sed156x_transfer((_dd), (0), (_data))

/*
 * command constants
 */

#define CMD_NOP         0xE3

#define CMD_DISPLAYON   0xAF
#define CMD_DISPLAYOFF  0xAE

#define CMD_DSPLINESTRT 0x40

#define CMD_REVERSE     0xA7
#define CMD_NOREVERSE   0xA6

#define CMD_ALLBLACK    0xA5
#define CMD_NOALLBLACK  0xA4

#define CMD_STRTLINEADR 0xA0
#define CMD_SPAGEADR    0xB0
#define CMD_SCOLHIADR   0x10
#define CMD_SCOLLOADR   0x00

#define CMD_RESET       0xE2

/*
 * values used mainly for initialization
 */

#define INI_BIAS_1DIV9  0xA2
#define INI_BIAS_1DIV7  0xA3

#define INI_ADCNORMAL   0xA0
#define INI_ADCREVERSE  0xA1

#define INI_STATINDOFF  0xAC
#define INI_STATINDON   0xAD

#define INI_COMDIRNORM  0xC0
#define INI_COMDIRREV   0xC8

#define INI_V5_INTRES   0x20
#define INI_POW_CONTR   0x28
#define INI_EL_VOL_SET  0x81

#define INI_DUTY_1DIV24 0xA8
#define INI_DUTY_1DIV32 0xA9

#define INI_DUTYPLUS1_0 0xAA
#define INI_DUTYPLUS1_1 0xAB

#define INI_POWSUPP_OFF 0x24
#define INI_POWSUPP_ON  0x25
#define INI_POWON_COMPL 0xED
#define INI_ELVOL_CONTR 0x80

#define INI_OUTPSTATREG 0xC0

#define INTERFACE_8080    0
#define INTERFACE_6800    1
#define INTERFACE_SERIAL  2

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
#define SIG_CS          (dd->sdcd->signals[11])

/* ser interface only */
#define SIG_SI          (dd->sdcd->signals[14])
#define SIG_SCL         (dd->sdcd->signals[15])
#define SIG_DC          (dd->sdcd->signals[16])
#define SIG_ICS         (dd->sdcd->signals[17])

/* ser and par interface, optional */
#define SIG_RESET       (dd->sdcd->signals[12])
#define SIG_BACKLIGHT   (dd->sdcd->signals[13])


/* different display types/models supported by this driver */
#define DISPID_NOKIA7110  1
#define DISPID_NEC21A     2
#define DISPID_LPH7508    3
#define DISPID_HP12542R   4

/* internal typedefs and functions */

static void serdisp_sed156x_init        (serdisp_t*);
static void serdisp_sed156x_update      (serdisp_t*);
static int  serdisp_sed156x_setoption   (serdisp_t*, const char*, long);
static void serdisp_sed156x_close       (serdisp_t*);

static void serdisp_sed156x_transfer    (serdisp_t*, int, byte);

typedef struct serdisp_sed156x_specific_s {
  int  interfacemode;
} serdisp_sed156x_specific_t;


static serdisp_sed156x_specific_t* serdisp_sed156x_internal_getStruct(serdisp_t* dd) {
  return (serdisp_sed156x_specific_t*)(dd->specific_data);
}


serdisp_wiresignal_t serdisp_sed156x_wiresignals[] = {
 /*  type   signame   actlow   cord   index */
  /* parallel data bus */
   {SDCT_PP, "CS",         1,   'C',    11 }
  ,{SDCT_PP, "A0",         0,   'C',     8 }
  ,{SDCT_PP, "WR",         1,   'C',    10 }
  ,{SDCT_PP, "RD",         1,   'C',     9 }
  /* serial data transfer */
  ,{SDCT_PP, "SI",         0,   'D',    14 }
  ,{SDCT_PP, "SCL",        0,   'C',    15 }
  ,{SDCT_PP, "DC",         0,   'D',    16 }
  ,{SDCT_PP, "ICS",        1,   'D',    17 }
  /* both */
  ,{SDCT_PP, "RESET",      1,   'D',    12 }
  ,{SDCT_PP, "BACKLIGHT",  0,   'D',    13 }
};


/* parallel port wiring is based on sed1330/powerlcd-wiring */
serdisp_wiredef_t serdisp_sed156x_wiredefs[] = {
   {  0, SDCT_PP, "SerDTSerdisp", "SI:D0,SCL:D1,DC:D2,ICS:D4,RESET:D5,BACKLIGHT:D7", "Serial Data Transfer - Serdisp Wiring"}
  ,{  1, SDCT_PP, "ParDTSerdisp", "DATA8,CS:nAUTO,A0:INIT,WR:nSTRB,RD:nSELIN", "Parallel Data Transfer - Serdisp Wiring"}
  ,{  2, SDCT_PP, "ParDTPollin",  "DATA8,CS:nSTRB,A0:nAUTO,WR:nSELIN,RESET:INIT", "Parallel Data Transfer - Pollin Wiring"}
  ,{  3, SDCT_PP, "ParDTHyundai", "DATA8,CS:INIT,A0:nSELIN,RESET:nAUTO,RD:nSTRB", "Parallel Data Transfer - Hyundai HP12542R Wiring"}
  ,{  4, SDCT_PP, "ParDTHyundaiHWRes", "DATA8,CS:INIT,A0:nSELIN,BACKLIGHT:nAUTO,RD:nSTRB", 
                                       "Parallel Data Transfer - Hyundai HP12542R Wiring w/ HW-reset and backlight support"}
};


serdisp_options_t serdisp_sed156x_options[] = {
   /*  name       aliasnames min  max mod int defines  */
   {  "DELAY",     "",         0,  -1,  1, 1,  ""}
  ,{  "CONTRAST",  "",         0,  10,  1, 1,  ""}
  ,{  "BACKLIGHT", "",         0,   1,  1, 1,  ""}
  ,{  "BRIGHTNESS", "",        0, 100,  1, 1,  ""}      /* brightness [0 .. 100] */
};


/* *********************************
   serdisp_t* serdisp_sed156x_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle (not used in here)
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_sed156x_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "serdisp_sed156x_setup(): cannot allocate display descriptor");
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));

  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_sed156x_specific_t)) )) {
    sd_error(SERDISP_EMALLOC, "serdisp_sed156x_setup(): cannot allocate specific display descriptor");
    free(dd);
    return (serdisp_t*)0;
  }

  memset(dd->specific_data, 0, sizeof(serdisp_sed156x_specific_t));


  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("NOKIA7110", dispname))
    dd->dsp_id = DISPID_NOKIA7110;
  else if (serdisp_comparedispnames("NEC21A", dispname))
    dd->dsp_id = DISPID_NEC21A;
  else if (serdisp_comparedispnames("LPH7508", dispname))
    dd->dsp_id = DISPID_LPH7508;
  else if (serdisp_comparedispnames("HP12542R", dispname))
    dd->dsp_id = DISPID_HP12542R;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_sed156x.c", dispname);
    return (serdisp_t*)0;
  }


  dd->width             = 132;
  dd->height            = 65;
  dd->depth             = 1;
  dd->min_contrast      = 0;
  dd->max_contrast      = 0x3F;
  dd->feature_contrast  = 1;
  dd->feature_backlight = 1;
  dd->feature_invert    = 1;
  dd->curr_rotate       = 0;         /* unrotated display */
  dd->curr_backlight    = 1;         /* start with backlight on */
  dd->connection_types  = SERDISPCONNTYPE_PARPORT;
  dd->fp_init           = &serdisp_sed156x_init;
  dd->fp_update         = &serdisp_sed156x_update;
  dd->fp_setoption      = &serdisp_sed156x_setoption;
  dd->fp_close          = &serdisp_sed156x_close;

  serdisp_sed156x_internal_getStruct(dd)->interfacemode = INTERFACE_SERIAL;

  /* nokia 7110's display area is 96 pixels wide and starts shifted by 18 columns !! */
  if (dd->dsp_id == DISPID_NOKIA7110) {
    dd->width           = 96;
    dd->startxcol       = 18;
    dd->min_contrast    = 38;     /* values < 38: display is unreadable */
    dd->dsparea_width   = 34000;  /* display area in micrometres (measured) */
    dd->dsparea_height  = 29000;
  } else if (dd->dsp_id == DISPID_NEC21A) {
    dd->width           = 132;
    dd->height          = 32;
    dd->startxcol       = 0;
    dd->dsparea_width   = 57000;  /* display area in micrometres (measured) */
    dd->dsparea_height  = 19500;
    dd->max_contrast    = 0x1F;
    dd->feature_invert  = 0;      /* set to unsupported, else symbols will be inverted too!! */
    dd->delay           = 2;

    serdisp_sed156x_internal_getStruct(dd)->interfacemode = INTERFACE_8080;
  } else if (dd->dsp_id == DISPID_LPH7508) {
    dd->width           = 100;
    dd->height          = 64;
    dd->startxcol       = 32;
    dd->dsparea_width   = 34000;  /* display area in micrometres (measured) */
    dd->dsparea_height  = 21000; 
    dd->max_contrast    = 0x1F;
    dd->feature_invert  = 0;      /* set to unsupported, else symbols get inverted too!! */
    dd->delay           = 2;

    serdisp_sed156x_internal_getStruct(dd)->interfacemode = INTERFACE_8080;
  } else if (dd->dsp_id == DISPID_HP12542R) {
    dd->width           = 128;
    dd->height          = 64;
    dd->startxcol       = 0;
    dd->dsparea_width   = 54000;  /* display area in micrometres (pollin description pdf) */
    dd->dsparea_height  = 27000; 
    dd->max_contrast    = 0x3F;
    dd->feature_invert  = 1;      
    dd->delay           = 2;

    serdisp_sed156x_internal_getStruct(dd)->interfacemode = INTERFACE_8080;
  }

  serdisp_setupstructinfos(dd, serdisp_sed156x_wiresignals, serdisp_sed156x_wiredefs, serdisp_sed156x_options);

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    free(dd);
    dd = 0;
    return (serdisp_t*)0;    
  }


  return dd;
}



/* *********************************
   void serdisp_sed156x_init(dd)
   *********************************
   initialise a sed156x-based display
   *********************************
   dd     ... display descriptor
*/
void serdisp_sed156x_init(serdisp_t* dd) {
  /* auto en/disable backlight feature depending on wiring used */
  dd->feature_backlight = (SIG_BACKLIGHT) ? 1 : 0;

  if (serdisp_sed156x_internal_getStruct(dd)->interfacemode == INTERFACE_SERIAL) {

    /* the initialisation and command reference can be found in the datasheet of SED156x, 8-27 ff */
    /* extremely helpful also has been the work of ing. iulian bergthaller: 
       http://sandiding.tripod.com/n7110.html
    */


    /* reset */
    /* SIG_RESET and SIG_ICS are active low and are auto-inverted by software */
    SDCONN_write(dd->sdcd, (SIG_RESET) ? SIG_RESET : 0, 0); SDCONN_usleep(dd->sdcd, 1000);
    SDCONN_write(dd->sdcd, 0, 0); SDCONN_usleep(dd->sdcd, 1000);

    PutCtrl(dd, INI_BIAS_1DIV7);         /* set bias to 1/7 */
    PutCtrl(dd, INI_ADCREVERSE);         /* ADC select */
    PutCtrl(dd, INI_COMDIRNORM);         /* Common Output Mode Select */

    PutCtrl(dd, INI_V5_INTRES | 0x02);   /* V5 voltage regulator internal resistor ratio */
    PutCtrl(dd, INI_POW_CONTR | 0x07);   /* power controller set: 
                                            Booster circuit + Voltage regulator circuit + 
                                            Voltage follower circuit => ON
                                        */
    PutCtrl(dd, CMD_NOP);

    PutCtrl(dd, CMD_DISPLAYON);

    /* PutCtrl(dd, CMD_ALLBLACK);  usleep(300000); */
    PutCtrl(dd, CMD_NOALLBLACK);

  } else if (dd->dsp_id == DISPID_HP12542R) {
    if (SIG_RESET) {
      SDCONN_write(dd->sdcd, SIG_RESET, 0);
      SDCONN_usleep(dd->sdcd, 1000);
      SDCONN_write(dd->sdcd, 0, 0);
      SDCONN_usleep(dd->sdcd, 1000);
    }
    PutCtrl(dd, CMD_DSPLINESTRT);        /* display line start: 0 */
    PutCtrl(dd, INI_ADCNORMAL);          /* ADC select */
    PutCtrl(dd, INI_BIAS_1DIV7);         /* set bias to 1/7 */
    PutCtrl(dd, INI_COMDIRNORM);         /* Common Output Mode Select */

    PutCtrl(dd, INI_POW_CONTR | 0x07);   /* power controller set: 
                                            Booster circuit + Voltage regulator circuit + 
                                            Voltage follower circuit => ON
                                        */
    PutCtrl(dd, INI_V5_INTRES | 0x00);   /* V5 voltage regulator internal resistor ratio */
    
    
    PutCtrl(dd, INI_STATINDOFF);         /* Static indicator off */

    PutCtrl(dd, CMD_DISPLAYON);

    /* PutCtrl(dd, CMD_ALLBLACK);  usleep(300000);*/
    PutCtrl(dd, CMD_NOALLBLACK);
    PutCtrl(dd, CMD_NOREVERSE);

  } else {

    int i;
    int ost = (dd->dsp_id == DISPID_NEC21A) ? 0x2 : 0xC;

    PutCtrl(dd, CMD_RESET);           /* reset */
    SDCONN_usleep(dd->sdcd, 20000);

    PutCtrl(dd, CMD_DISPLAYOFF);      /* display off during init */

    PutCtrl(dd, CMD_NOREVERSE);       /* set normal */
    PutCtrl(dd, CMD_NOALLBLACK);      /* set all indicator  */
    PutCtrl(dd, INI_DUTY_1DIV32);     /* duty = 1/32 */
    PutCtrl(dd, INI_DUTYPLUS1_1);     /* duty +1 */
    PutCtrl(dd, INI_ADCNORMAL);       /* ADC=0  */

    PutCtrl(dd, INI_OUTPSTATREG+ost); /* output status register set  */
    PutCtrl(dd, INI_POWSUPP_ON);      /* power supp on */
    SDCONN_usleep(dd->sdcd, 1000000); /* sleep 1sec */
    PutCtrl(dd, INI_POWON_COMPL);     /* set power-on completion */
    PutCtrl(dd, INI_ELVOL_CONTR+0xF); /* volume control (set to maximum) */

    PutCtrl(dd, CMD_DSPLINESTRT);     /* display start line set */
    PutCtrl(dd, CMD_DISPLAYON);       /* display on */

    /* clear all symbols */
    PutCtrl(dd, CMD_SPAGEADR | 8);
    for (i = 0; i < dd->width ; i++) {
      PutCtrl(dd, CMD_SCOLHIADR | ((i + dd->startxcol) >> 4));
      PutCtrl(dd, CMD_SCOLLOADR | ((i + dd->startxcol) & 0x0F));
      PutData(dd, 0);      
    }
  }

  sd_debug(2, "serdisp_sed156x_init(): done with initialising");
}


/* *********************************
   void serdisp_sed156x_transfer(dd, isdata, value)
   *********************************
   transfer a data or command byte to the display
   *********************************
   dd     ... display descriptor
   isdata ... isdata = 1: data; isdata = 0: command
   value   ... byte to be processed 
*/
void serdisp_sed156x_transfer(serdisp_t* dd, int isdata, byte value) {
  if (serdisp_sed156x_internal_getStruct(dd)->interfacemode == INTERFACE_SERIAL) {
    /* 'signal byte': contains all signal bits controlling the display */
    long td;
    /* same as td, but with clock-signal enabled */
    long td_clk;

    byte t_value;
    int i;

    t_value = value;

    /* SIG_ICS -> low == unset active low ICS (will be auto-inverted) */
    td = SIG_ICS;


    if (dd->feature_backlight && dd->curr_backlight)
      td |= SIG_BACKLIGHT;

    /* if data => DC == high */
    if (isdata)
      td |= SIG_DC;

    SDCONN_write(dd->sdcd, td, 0);
    sdtools_nsleep(dd->delay);


    /* loop through all 8 bits and transfer them to the display */
    /* optrex and sed156x start with bit 7 (MSB) */
    for (i = 0; i <= 7; i++) {
      /* write a single bit to SIG_SI */
      if (t_value & 0x80)  /* bit == 1 */
        td |= SIG_SI;
      else              /* bit == 0 */
        td &= (0xff - SIG_SI);

      /* clock => high */
      td_clk = td | SIG_SCL;

      /* write content to display */
      SDCONN_write(dd->sdcd, td, 0);
      sdtools_nsleep(dd->delay);

      /* set clock to high (bit is read on rising edge) */
      SDCONN_write(dd->sdcd, td_clk, 0);
      sdtools_nsleep(dd->delay);

      /* set clock to low again */
      SDCONN_write(dd->sdcd, td, 0);
      sdtools_nsleep(dd->delay);

      /* shift byte so that next bit is on the first (MSB) position */
      t_value = (t_value & 0x7f) << 1;
   }
    /* 'commit' */
    /* done (SIG_ICS -> high) */
    /* set active low ICS (will be auto-inverted) */
    td &= (0xffffffff - SIG_ICS);
        
    SDCONN_write(dd->sdcd, td, 0);
    sdtools_nsleep(dd->delay);
  } else {  /* interfacemode == 8080 or 6800 */
    long item_split = 0;
    long td_clk1 = 0;
    long td_clk2 = 0;
    long td_clk3 = 0;
    long td_clk4 = 0;
    int i;

    /* SIG_WR, SIG_RD, and SIG_CS are active-low and will be auto-inverted by software */
    
    /* interfacemode INTERFACE_6800 is not supported at the moment. no known device using this mode ... */
    
    if (dd->dsp_id == DISPID_LPH7508) { /* special case for LPH7508:  as EN is hard-wired, /CS1 is used as clocking signal */
      td_clk1 = SIG_WR ;
      td_clk2 = SIG_WR ;
      td_clk3 = SIG_WR | SIG_CS;
      td_clk4 = SIG_WR ;
    } else if (dd->dsp_id == DISPID_HP12542R) { /* Hyundai HP12542R: RW is hard-wired */
      td_clk1 = SIG_CS | SIG_RD;
      td_clk2 = SIG_CS | SIG_RD;
      td_clk3 = SIG_CS;
      td_clk4 = SIG_CS | SIG_RD;

    } else {  
/*    if (serdisp_sed156x_internal_getStruct(dd)->interfacemode == INTERFACE_8080) { 
*/
      td_clk1 = SIG_CS;
      td_clk2 = SIG_CS;
      td_clk3 = SIG_CS | SIG_WR ;
      td_clk4 = SIG_CS;

/*
    } else {
      td_clk1 = SIG_WR | SIG_RD | SIG_CS;
      td_clk2 = SIG_WR | SIG_RD | SIG_CS;
      td_clk3 = SIG_WR |          SIG_CS;
      td_clk4 = SIG_WR | SIG_RD | SIG_CS;
    }
*/
    }

    if (isdata) {
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
      if (value & (1 << i))
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
   void serdisp_sed156x_update(dd)
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
void serdisp_sed156x_update(serdisp_t* dd) {
  int x, page, data;
  int pages = (dd->height+7)/8;


#ifdef OPT_USEOLDUPDATEALGO
  for (page = 0; page < pages; page++) {
    PutCtrl(dd, CMD_SPAGEADR | page);
    for(x = 0; x < dd->width; x++) {
      if (x == 0) {
          PutCtrl(dd, CMD_SCOLHIADR | ((x + dd->startxcol) >> 4));
          PutCtrl(dd, CMD_SCOLLOADR | ((x + dd->startxcol) & 0x0F));
      }

      data = dd->scrbuf[ dd->width * page  +  x];
      
      /* if display doesn't support hardware invert: software invert  */
      if (dd->curr_invert && !(dd->feature_invert))
        data = ~data;      
      
      PutData(dd, data);
      dd->scrbuf_chg[x + dd->width*(page/8)] &= 0xFF - (1 << (page%8)) ;
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
fprintf(stderr, ((dd->scrbuf_chg[x + dd->width*(page/8)]) & ( 1 << (page%8)) ) ? "." : " ");
#endif

      /* either actual_x or actual_x + 1 has changed  or one of left/right-most */
      if ( dd->scrbuf_chg[x + (dd->width)*(page/8)] & ( 1 << (page%8)) ) {
        if (x > last_x+1 ) {

          if (set_page) {
            PutCtrl(dd, CMD_SPAGEADR | page);
#ifdef DBG_BENCHMARK
            cntPCp++;
            fprintf(stderr, "P");
#endif
            set_page = 0;
          }

          /* x-position may be written directly to sed156x */
          PutCtrl(dd, CMD_SCOLLOADR | ((x + dd->startxcol) & 0x0F));
          PutCtrl(dd, CMD_SCOLHIADR | ((x + dd->startxcol) >> 4));

#ifdef DBG_BENCHMARK 
            cntPCc+=2;
            fprintf(stderr, "C%04x", x);
#endif
        }

        data = dd->scrbuf[ dd->width * page  +  x];
      
        /* if display doesn't support hardware invert: software invert  */
        if (dd->curr_invert && !(dd->feature_invert))
          data = ~data;      
      
        PutData(dd, data);
#ifdef DBG_BENCHMARK
            cntPD++;
            fprintf(stderr, ".");
#endif
        dd->scrbuf_chg[x + dd->width*(page/8)] &= 0xFF - (1 << (page%8)) ;

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

  /* w/o the following NOP, sed156x displays don't finalize the last data-operation (as it seems) */
  PutCtrl(dd, CMD_NOP);
}



/* *********************************
   int serdisp_sed156x_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_sed156x_setoption(serdisp_t* dd, const char* option, long value) {
  PutCtrl(dd, CMD_NOP);

  if (dd->feature_invert && serdisp_compareoptionnames(dd, option, "INVERT") ) {
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

    if (dd->dsp_id == DISPID_NOKIA7110 || dd->dsp_id == DISPID_HP12542R) {
      PutCtrl(dd, INI_EL_VOL_SET );       /* Electronic Volume Mode Set  (double byte command)*/
      PutCtrl(dd, dimmed_contrast /*dd->curr_contrast*/ ); /* contrast: max. value: 0x3F, bits D7, D6 are ignored */
      PutCtrl(dd, CMD_NOP );
    } else {
      PutCtrl(dd, 0x80 + dimmed_contrast /*dd->curr_contrast*/ );    /* contrast: max. value: 0x1F */
    }
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
  }
  return 1;
}



/* *********************************
   void serdisp_sed156x_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_sed156x_close(serdisp_t* dd) {
  PutCtrl(dd, CMD_DISPLAYOFF);

  /* reset */
  if (serdisp_sed156x_internal_getStruct(dd)->interfacemode == INTERFACE_SERIAL) {
    if (SIG_RESET) {
      SDCONN_write(dd->sdcd, SIG_RESET, 0); SDCONN_usleep(dd->sdcd, 1000);
    }
    SDCONN_write(dd->sdcd, 0, 0); SDCONN_usleep(dd->sdcd, 1000);
  } else {
    PutCtrl(dd, CMD_RESET);                   /* reset */
  }
}


