/** \file    serdisp_control.c
  *
  * \brief   Functions for controlling serial lc-displays (eg: optrex323, nokia displays, ... )
  * \date    (C) 2003-2010
  * \author  wolfgang astleitner (mrwastl@users.sourceforge.net)
  */

/*
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

#include "../config.h"

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <strings.h>

#include "serdisplib/serdisp_control.h"
#include "serdisplib/serdisp_connect.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_colour.h"

#include "serdisplib/serdisp_fctptr.h"


typedef struct serdisp_setup_s {
  char* dispname;
  char* aliasnames;
  serdisp_t* (*fp_setup) (const serdisp_CONN_t*, const char*, const char*);
  char* defaultoptions;
  char* description;
} serdisp_setup_t;


/* supported displays */
serdisp_setup_t serdisp_displays[] = {
  /* display name   alias names                 function pointer         default options         description            */
#if WITH_DRIVER_SED153X
  {"OPTREX323",    "",                         serdisp_sed153x_setup,   "",                     "Optrex 323 display"},
  {"LSU7S1011A",   "ALPS",                     serdisp_sed153x_setup,   "WIRING=1",             "ALPS display with display module kit by pollin"},
  {"E08552",       "",                         serdisp_sed153x_setup,   "WIRING=2",             "EPSON E0855-2 display with display module kit by pollin"},
#endif
#if WITH_DRIVER_PCD8544
  {"PCD8544",      "",                         serdisp_pcd8544_setup,   "WIRING=1",             "generic driver for PCD8544-based displays"},
  {"PCF8511",      "",                         serdisp_pcd8544_setup,   "WIRING=1",             "generic driver for PCF8511-based displays"},
  {"LPH7366",      "",                         serdisp_pcd8544_setup,   "WIRING=0",             "LPH7366 display with backlight"},
  {"LPH7690",      "",                         serdisp_pcd8544_setup,   "WIRING=1",             "LPH7690 display"},
#endif
#if WITH_DRIVER_SED156X
  {"NOKIA7110",    "SED1565",                  serdisp_sed156x_setup,   "",                     "Nokia 7110 display (SED1565-based)"},
  {"NEC21A",       "SKYPER",                   serdisp_sed156x_setup,   "WIRING=1",             "NEC 21a (Skyper) display module"},
  {"LPH7508",      "",                         serdisp_sed156x_setup,   "WIRING=2",             "LPH7508 display module with display module kit by pollin"},
  {"HP12542R",     "",                         serdisp_sed156x_setup,   "WIRING=3",             "Hyundai HP12542R display module with display module kit by pollin"},
#endif
#if WITH_DRIVER_I2C
  {"ERICSSONT2X",  "E///T2X",                  serdisp_i2c_setup,       "",                     "Ericsson T20/T28/T29 i2c-displays"},
  {"ERICSSONR520", "E///R520,R520",            serdisp_i2c_setup,       "",                     "Ericsson R520/T65 i2c-displays"},
#endif
#if WITH_DRIVER_T6963
  {"T6963",        "T6963C",                   serdisp_t6963_setup,     "",                     "generic driver for T6963-based displays"},
  {"TLX1391",      "",                         serdisp_t6963_setup,     "WIDTH=128;HEIGHT=128", "Toshiba TLX1391 display (T6963-based)"},
  {"T6963SERMOD",  "",                         serdisp_t6963_setup,     "MODE=SER;WIRING=3;FONT=6", "T6963-based display module with built-in parallel to serial converter"},
#endif
#if WITH_DRIVER_SED133X
  {"SED133X",      "SED1330,SED1335",          serdisp_sed133x_setup,   "",                     "generic driver for SED1330/SED1335-based displays"},
  {"S1D13700",     "",                         serdisp_sed133x_setup,   "",                     "generic driver for S1D13700-based displays"},
 #if HAVE_LIBUSB
  {"USB13700",     "",                         serdisp_sed133x_setup,   "",                     "displays driven by USB13700 USB module"},
 #endif
#endif
#if WITH_DRIVER_NOKCOL
  {"N3510I",       "N3530",                    serdisp_nokcol_setup,    "",                     "driver for Nokia 3510i/3530 displays (S1D15G14-based)"},
  {"S1D15G10",     "N6100",                    serdisp_nokcol_setup,    "",                     "driver for Nokia 6100 and compliant displays (S1D15G10-based)"},
#endif
#if WITH_DRIVER_KS0108
  {"KS0108",       "",                         serdisp_ks0108_setup,    "",                     "generic driver for KS0108-based displays"},
  {"CTINCLUD",     "",                         serdisp_ks0108_setup,    "",                     "c't includ USB-display"},
#endif
#if WITH_DRIVER_LH155
  {"LH155",        "",                         serdisp_lh155_setup,     "",                     "generic driver for LH155-based displays"},
  {"SHARP240",     "",                         serdisp_lh155_setup,     "",                     "Sharp M078CKA 240x64 (LH155-based)"},
#endif
#if WITH_DRIVER_SSDOLED
  {"OLED96X36X1",  "",                         serdisp_ssdoled_setup,   "",                     "Pictiva 96X36 OLED 1bit"},
  {"OLED96X64X16", "",                         serdisp_ssdoled_setup,   "",                     "Pictiva 96X64 colour OLED 16bit"},
  {"OLED128X64X4", "",                         serdisp_ssdoled_setup,   "",                     "Pictiva 128X64 yellow OLED 4bit"},
  {"BL160128A", "",                            serdisp_ssdoled_setup,   "",                     "Bolymin BL160128A colour OLED 18bit"},
  {"4DOLED282815", "4DOLED128",                serdisp_ssdoled_setup,   "",                     "4DOLED-282815 colour OLED 18bit"},
#endif
#if WITH_DRIVER_L4M
  {"L4ME5I",       "",                         serdisp_l4m_setup,       "",                     "linux4media USB-interface E-5i-USB"},
  {"L4M132C",      "",                         serdisp_l4m_setup,       "",                     "linux4media USB-interface w/ 132x65 colour display"},
#endif
#if WITH_DRIVER_GOLDELOX
  {"GOLDELOX",     "UOLED160,UOLED128",        serdisp_goldelox_setup,  "",                     "displays driven by GOLDELOX-MD1 module"},
#endif
#if WITH_DRIVER_STV8105
  {"STV8105",      "OLED256X64X4",             serdisp_stv8105_setup,   "",                     "Pictiva 256X64 yellow OLED 4bit"},
#endif
#if WITH_DRIVER_ACOOLSDCM
  {"ALPHACOOL",    "",                         serdisp_acoolsdcm_setup, "",                     "Alphacool USB-displays 200x64 or 240x128"},
  {"SDCMEGTRON",   "",                         serdisp_acoolsdcm_setup, "",                     "SDC-Megtron USB-displays 240x128"},
#endif
#if WITH_DRIVER_DIRECTGFX
  {"SDL",          "",                         serdisp_directgfx_setup, "",                     "direct output to SDL window"},
#endif
#if WITH_DRIVER_LC7981
  {"DG16080",       "",                        serdisp_lc7981_setup, "",                        "DG-16080 160x80"},
  {"DG1608011",     "",                        serdisp_lc7981_setup, "",                        "DG-16080-11 160x80 (non-square pixels)"},
#endif
};

/* *** in the queue but not working yet *** */
#if 0
  /* r320 support not tested yet */
  {"ERICSSONR320", "E///R320,R320",            serdisp_i2c_setup,       "",                     "Ericsson R320 i2c-display"}
#endif


/* standard options + aliasnames and defines 
   (eg: for better legibility one may write BACKLIGHT=YES instead of BACKLIGHT=1 
*/
serdisp_options_t serdisp_standardoptions[] = {
   /*  name        aliasnames  min  max  mod    flag               defines  */
   {  "ROTATE",     "ROT",       0, 360,  90,   SD_OPTIONFLAG_STD |
                                                SD_OPTIONFLAG_RW,  "1=180,YES=180,NO=0,TRUE=180,FALSE=0,TOGGLE=2"}
  ,{  "INVERT",     "INV",       0,   1,   1,   SD_OPTIONFLAG_STD |
                                                SD_OPTIONFLAG_RW,  "YES=1,NO=0,TRUE=1,FALSE=0"}
  ,{  "DSPAREAWIDTH", "DSPAREAW",0,  -1,  -1,   SD_OPTIONFLAG_STD, ""}
  ,{  "DSPAREAHEIGHT","DSPAREAH",0,  -1,  -1,   SD_OPTIONFLAG_STD, ""}
  ,{  "SELFEMITTING", "",        0,   1,  -1,   SD_OPTIONFLAG_STD, "YES=1,NO=0,TRUE=1,FALSE=0"}
  ,{  "CONTRAST",   "",         -1,  -1,  -1,   0,                 ""}
  ,{  "BACKLIGHT",  "BG",       -1,  -1,  -1,   0,                 "YES=1,NO=0,TRUE=1,FALSE=0,ON=1,OFF=0"}
  ,{  "DELAY",      "",         -1,  -1,  -1,   0,                 "NONE=0"}
  ,{  "WIDTH",      "W",        -1,  -1,  -1,   0,                 ""}
  ,{  "HEIGHT",     "H",        -1,  -1,  -1,   0,                 ""}
  ,{  "DEPTH",      "",         -1,  -1,  -1,   0,                 ""}
  ,{  "BRIGHTNESS", "",         -1,  -1,  -1,   0,                 ""}
};



/* prototypes for functions with local-only scope */
static char* serdisp_getwiresignalname(serdisp_t* dd, int idx);
static int   serdisp_setupwirings(serdisp_t* dd, const char* wiredef, int wiredef_len);


/**
  * \brief   initialises a display
  *
  * \param   sdcd          output device handle
  * \param   dispname      display name
  * \param   optionstring  option string (extra options eg. from outside)
  *
  * \retval  !NULL         device descriptor
  * \retval  NULL          unsuccuessful initialisation 
  *
  * \b Example: \n
  * \code
  * serdisp_t* dd;
  * dd = serdisp_init(sdcd, "PCD8544", "WIRING=1;INVERT=YES");
  * \endcode
  */
serdisp_t* serdisp_init(serdisp_CONN_t* sdcd, const char dispname[], const char optionstring[]) {
  serdisp_t* dd = (serdisp_t*)0;
  int found = 0;
  int i = 0;
  int displayidx = 0;

  char* patternptr = (char*) optionstring;
  int patternlen = -1;
  int patternborder = strlen(patternptr);
  char* valueptr = 0;
  int valuelen = 0;

  sd_debug(2, "serdisp_init(): entering; dispname: %s, optionstring: %s", dispname, optionstring);

  displayidx = serdisp_getdispindex(dispname);

  if (displayidx < 0) {
    sd_error(SERDISP_ENOTSUP, "display '%s' not in display table", dispname);
    return (serdisp_t*)0;
  }

  /* setup display */
  dd = serdisp_displays[displayidx].fp_setup(sdcd, dispname, optionstring);

  if (!dd) {
    sd_debug(1, "serdisp_init(); display %s could not be initialised. last error: %s", dispname, sd_errormsg);
    return (serdisp_t*)0;
  }

  /* if dsp_name not already set to dispname in specific setup, do it here */
  if (!dd->dsp_name) {
    dd->dsp_name = (char*)dispname;
  }

  /* if dsp_optionstring not already set to optionstring in specific setup, do it here */
  if (!dd->dsp_optionstring) {
    dd->dsp_optionstring = (char*)optionstring;
  }

  /* unsupported connection type */
  if (! (dd->connection_types & sdcd->conntype)) {
    serdisp_freeresources(dd);
    dd = 0;
    sd_error(SERDISP_ENOTSUP, "selected connection type is unsupported");
    return (serdisp_t*)0;
  }

  /* unsupported protocol */
  if (! (sdcd->protocol == SDPROTO_DONTCARE || dd->supp_protocols & sdcd->protocol)) {
    serdisp_freeresources(dd);
    dd = 0;
    sd_error(SERDISP_ENOTSUP, "selected protocol is unsupported");
    return (serdisp_t*)0;
  }

  /* initialise other items to default values not set by fp_setup */
  if (! dd->fp_setpixel)
    dd->fp_setpixel = &sdtools_generic_setpixel;

  if (! dd->fp_getpixel)
    dd->fp_getpixel = &sdtools_generic_getpixel;

  if (! dd->depth)
    dd->depth = 1;


  /* filter incompatible settings */
  if (dd->depth <=0 || dd->depth > 32/* || (8 % dd->depth)*/) {   /* unsupported colour depth */
    sd_debug(1, "serdisp_init(); colour depth must be 1, 2, 4, or 8. depth %d is not supported (%s)", dd->depth, dispname);
    serdisp_freeresources(dd);
    dd = 0;
    return (serdisp_t*)0;
  }


  if (! dd->scrbuf_size) {
    /* cifiob ... how many colour items can be stored into one byte */
    /* cifiob = 8 / dd->depth; */
    /* dd->scrbuf_size = sizeof(byte) * (dd->width + dd->xcolgaps) * (( dd->height +dd->ycolgaps + (cifiob-1))/cifiob);  */
    /* dd->scrbuf_chg_size = sizeof(byte) * (dd->width + dd->xcolgaps)  * ((( dd->height +dd->ycolgaps + (cifiob*8-1))/(cifiob*8))); */

    if (dd->depth >= 8) {
      int bitspercol = (dd->depth == 18) ? 24 : dd->depth; /* for speed reasons, align depth 18 to 24 bits in screen buffer */
      dd->scrbuf_size = sizeof(byte) * (( (dd->width + dd->xcolgaps) * (dd->height + dd->ycolgaps) * bitspercol + 7)  / 8); 

      /* one byte is able to store 8 change infos. one change info == one changed pixel */
      dd->scrbuf_chg_size = sizeof(byte) * ( ((  (dd->width+dd->xcolgaps) + 7) >> 3) * (dd->height+dd->ycolgaps) ); 
    } else {
      int cifiob = 8 / dd->depth;
      dd->scrbuf_size = sizeof(byte) * (dd->width + dd->xcolgaps) * (( dd->height +dd->ycolgaps + (cifiob-1))/cifiob); 
      dd->scrbuf_chg_size = sizeof(byte) * (dd->width + dd->xcolgaps)  * ((( dd->height +dd->ycolgaps + (cifiob*8-1))/(cifiob*8)));
    }
  }


  if (! dd->scrbuf) {
    /* allocate screen buffer buffer */
    if (! (dd->scrbuf = (byte*) sdtools_malloc( dd->scrbuf_size ) ) ) {
      sd_error(SERDISP_EMALLOC, "serdisp_init(): cannot allocate screen buffer");
      serdisp_freeresources(dd);
      dd = 0;
      return (serdisp_t*)0;
    }
  }

  if (! dd->scrbuf_chg && dd->scrbuf_chg_size) {
    /* allocate memory for screen-change buffer (only if scrbuf_chg_size is > 0) */
    if (! (dd->scrbuf_chg = (byte*) sdtools_malloc( dd->scrbuf_chg_size ) )) {
      sd_error(SERDISP_EMALLOC, "serdisp_init(): cannot allocate screen change buffer");
      serdisp_freeresources(dd);
      dd = 0;
      return (serdisp_t*)0;
    }
  }

  if (! dd->scrbuf_bits_used) {
    /* all 8 out of 8 bits are used in screen buffer (exception of default behaviour: t6963 with fontwidth=6) */
    dd->scrbuf_bits_used = 8;
  }

  /* colour spaces */
  if (!dd->colour_spaces) { /* auto detect */
    switch (serdisp_getdepth(dd)) {
      case 1:
      case 2:
      case 4:
        dd->colour_spaces = SD_CS_GREYSCALE;
        break;
      /* depth 8 may either be 256 grey levels or RGB332: so no auto-detect */
      case 12:
        dd->colour_spaces = SD_CS_RGB444;
        break;
      case 18:
        dd->colour_spaces = SD_CS_RGB666;
        break;
      default:
        sd_error(SERDISP_ENOTSUP, "serdisp_init(): cannot auto-detect colour space. dd->colour_spaces has to be set for this driver");
        serdisp_freeresources(dd);
        dd = 0;
        return (serdisp_t*)0; 
    }
  }

  /* indexed colour table */
  /* initialise colour table (greyscale values and indexed colour schemes only; leave uninitialised otherwise) */

  /* no default colour table (only for indexed colours) */
  if ((! dd->ctable) && (! SD_CS_ISDIRECTCOLOUR(dd)) && ( dd->colour_spaces & SD_CS_INDEXED_SPACE)  ) {

    if (! (dd->ctable = (long*) sdtools_malloc( sizeof(long) *  serdisp_getcolours(dd) ) ) ) {
      sd_error(SERDISP_EMALLOC, "serdisp_init(): cannot allocate indexed colour table");
      serdisp_freeresources(dd);
      dd = 0;
      return (serdisp_t*)0;
    }

    /* special case for depth == 1: keep backward compatibility with old versions: 
     *                              white = idx[0]; black = idx[1] */
    if (serdisp_getdepth(dd) == 1) {
      serdisp_setcoltabentry(dd, 0, serdisp_GREY2ARGB(0xFF));
      serdisp_setcoltabentry(dd, 1, serdisp_GREY2ARGB(0x00));
    } else if (serdisp_getdepth(dd) <= 8) {
      int j, c, colours = serdisp_getcolours(dd);

      for (j = 0; j < colours; j++) {
        c = (255 / (colours - 1 )) * j;
        serdisp_setcoltabentry(dd, j, serdisp_GREY2ARGB(c));
      }
    }
  }
  
  if ( ! dd->fp_transcolour || ! dd->fp_transgrey || ! dd->fp_lookupcolour || ! dd->fp_lookupgrey ) {
    if (serdisp_sdcol_init(dd)) {
      sd_error(SERDISP_ENOTSUP, "serdisp_init(): cannot initialise colour-space specific colour functions.");
      serdisp_freeresources(dd);
      dd = 0;
      return (serdisp_t*)0; 
    }
  }

  dd->sdcd = sdcd;

  found = 0;
  /* search for wiring-definition */
  if (optionstring && strlen(optionstring) > 0) {

    while( !found && (patternptr = sdtools_nextpattern(patternptr, ';', &patternlen, &patternborder)) ) {

      if ( strncasecmp(patternptr, "WIRING=", strlen("WIRING=")) == 0 ||
           strncasecmp(patternptr, "WIRE=", strlen("WIRE=")) == 0
         ) {
        found = 1;
      }
    }
  }

  if (!found) {
    /* if no wiring-definition found in extern optionstring string, try defaultoptions string in serdisp_displays[] */
    if (serdisp_displays[displayidx].defaultoptions && strlen(serdisp_displays[displayidx].defaultoptions) > 0) {
      found = 0;
  
      patternptr = serdisp_displays[displayidx].defaultoptions;
      patternlen = -1;
      patternborder = strlen(serdisp_displays[displayidx].defaultoptions);

      while( !found && (patternptr = sdtools_nextpattern(patternptr, ';', &patternlen, &patternborder)) ) {
        if ( strncasecmp(patternptr, "WIRING=", strlen("WIRING=")) == 0 ||
             strncasecmp(patternptr, "WIRE=", strlen("WIRE=")) == 0
           ) {
          found = 1;
        }
      }
    }
  }

  if (found) {
    char* idxpos = index(patternptr, '=');
    int keylen = patternlen;

    /* '=' found and position not outside patternlen? */
    if (idxpos &&  serdisp_ptrstrlen(idxpos, patternptr) < patternlen ) {
      keylen = serdisp_ptrstrlen(idxpos, patternptr);
      valueptr = ++idxpos;
      valuelen = patternlen - keylen - 1;
    } else {
      sd_error(SERDISP_ERUNTIME, "serdisp_init(): invalid/incomplete wiring definition");
      serdisp_freeresources(dd);
      dd = 0;
      return (serdisp_t*)0; 
    }
    if (serdisp_setupwirings(dd, valueptr, valuelen)) {
      serdisp_freeresources(dd);
      dd = 0;
      return (serdisp_t*)0; 
    }
  } else {  /* nothing found: use default wiring number 0 */
    if (serdisp_setupwirings(dd, "0", 0)) {
      serdisp_freeresources(dd);
      dd = 0;
      return (serdisp_t*)0; 
    }
  }
  

  sd_debug(2, "serdisp_init(): wiring (before fp_init()):");
  sd_debug(2, "=======================");
  if (sdcd->conntype == SERDISPCONNTYPE_PARPORT) {
    sd_debug(2, "    signal-id    __CCSSDD  signal-name");
    sd_debug(2, "  -----------    --------  -----------");
  }
  for (i = 0; i < SD_MAX_SUPP_SIGNALS; i++)
    if (sdcd->signals[i])
      sd_debug(2, "  signals[%2d]: 0x%08lx  %s", i, sdcd->signals[i], serdisp_getwiresignalname(dd,i));
  sd_debug(2, "  invert mask: 0x%08lx", sdcd->signals_invert);
  sd_debug(2, "      perm on: 0x%08lx", sdcd->signals_permon);
  sd_debug(2, " ");
  sd_debug(2, "  io_flags_readstatus: 0x%02x", dd->sdcd->io_flags_readstatus);
  sd_debug(2, "  io_flags_writedata:  0x%02x", dd->sdcd->io_flags_writedata);
  sd_debug(2, "  io_flags_writecmd:   0x%02x", dd->sdcd->io_flags_writecmd);
  sd_debug(2, " ");

  if (dd->ctable) {
    sd_debug(2, "serdisp_init(): colour table:");
    sd_debug(2, "=============================");
    sd_debug(2, "    idx    AARRGGBB");
    sd_debug(2, "  -----    --------");
    for (i = 0; i < serdisp_getcolours(dd); i++)
      sd_debug(2, "  %5d  0x%08lX", i, dd->ctable[i]);
    sd_debug(2, " ");
  }

  dd->fp_init(dd);

  sd_debug(2, "serdisp_init(): information:");
  sd_debug(2, "============================");
  sd_debug(2, "  supported colour spaces: 0x%08lx", dd->colour_spaces);
  sd_debug(2, " ");

  sd_debug(2, "serdisp_init(): wiring (after fp_init()):");
  sd_debug(2, "=======================");
  if (sdcd->conntype == SERDISPCONNTYPE_PARPORT) {
    sd_debug(2, "    signal-id    __CCSSDD  signal-name");
    sd_debug(2, "  -----------    --------  -----------");
  }
  for (i = 0; i < SD_MAX_SUPP_SIGNALS; i++)
    if (sdcd->signals[i])
      sd_debug(2, "  signals[%2d]: 0x%08lx  %s", i, sdcd->signals[i], serdisp_getwiresignalname(dd,i));
  sd_debug(2, "  invert mask: 0x%08lx", sdcd->signals_invert);
  sd_debug(2, "      perm on: 0x%08lx", sdcd->signals_permon);
  sd_debug(2, " ");
  sd_debug(2, "  io_flags_readstatus: 0x%02x", dd->sdcd->io_flags_readstatus);
  sd_debug(2, "  io_flags_writedata:  0x%02x", dd->sdcd->io_flags_writedata);
  sd_debug(2, "  io_flags_writecmd:   0x%02x", dd->sdcd->io_flags_writecmd);
  sd_debug(2, "  (0x01: WRITEDB  0x02: WRITECB  0x04: READDB  0x08: READSB   0x10: READCB)");  
  sd_debug(2, " ");

  if (dd->ctable) {
    sd_debug(2, "serdisp_init(): colour table:");
    sd_debug(2, "=============================");
    sd_debug(2, "    idx    AARRGGBB");
    sd_debug(2, "  -----    --------");
    for (i = 0; i < serdisp_getcolours(dd); i++)
      sd_debug(2, "  %5d  0x%08lX", i, dd->ctable[i]);
    sd_debug(2, " ");
  }

  /* post-init settings */

  serdisp_setoption(dd, "INVERT", dd->curr_invert);
  /* set backlight if supported by the driver */
  /* exception: curr_backlight == -1: already initialised (eg. by remote driver) */
  if (dd->feature_backlight && dd->curr_backlight != -1)
    serdisp_setoption(dd, "BACKLIGHT", dd->curr_backlight);
  /* set default contrast if curr_contrast == 0; else re-set contrast */
  /* exception: curr_contrast == -1: already initialised (eg. by remote driver) */
  if (dd->feature_contrast && dd->curr_contrast != -1) {
    if (dd->curr_contrast == 0) 
      serdisp_setoption(dd, "CONTRAST", MAX_CONTRASTSTEP / 2);
    else
      serdisp_setoption(dd, "CONTRAST", sdtools_contrast_hw2norm(dd, dd->curr_contrast));
  }

  return dd;
}


/**
  * \brief   gets version code
  *
  * \return  version code of serdisp library
  *
  * \since   1.98
  */
long serdisp_getversioncode(void) {
  return (long) SERDISP_VERSION_CODE;
}


/**
  * \brief   gets width of display
  *
  * \param   dd            device descriptor
  *
  * \return  width of display
  */
int serdisp_getwidth(serdisp_t* dd) {
  return ((dd->curr_rotate <= 1) ? dd->width : dd->height);
}


/**
  * \brief   gets height of display
  *
  * \param   dd            device descriptor
  *
  * \return  height of display
  */
int serdisp_getheight(serdisp_t* dd) {
  return ((dd->curr_rotate <= 1) ? dd->height : dd->width);
}


/**
  * \brief   gets amount of colours
  *
  * get amount of colours supported by the configuration currently used
  *
  * \param   dd            device descriptor
  *
  * \return  amount of supported colours
  */
int serdisp_getcolours(serdisp_t* dd) {
  /* depth 32 = depth 24 + alpha channel, thus only 2^24 different colours */
  if (dd->depth >= 24)
    return (1 << 24);
  return 1 << dd->depth;
}


/**
  * \brief   gets colour depth
  *
  * get colour depth supported by the configuration currently used
  *
  * \param   dd            device descriptor
  *
  * \return  colour depth
  *
  * \since   1.95
  */
int serdisp_getdepth(serdisp_t* dd) {
  return dd->depth;
}


/**
  * \brief   gets pixel aspect ratio
  *
  * get pixel aspect ratio in percent (to avoid floating-point values)
  *
  * \li pixels are quadratic: 100 will be returned
  * \li pixel width is twice pixel height: 200 will be returned
  * \li pixel width is half of pixel height: 50 will be returned
  *
  * \em formula:
  * \verbatim

               w * ph       w, h: amount of pixels
     f = 100 * ------     pw, ph: display area (in micrometres, but unit has no influence on the calc.)
               h * pw          f: pixel aspect ratio in percent  \endverbatim
  *
  *
  * \param   dd            device descriptor
  *
  * \return  pixel ascpect ratio in percent
  *
  * \since   1.95
  */
int serdisp_getpixelaspect(serdisp_t* dd) {
  if (dd->dsparea_width && dd->dsparea_height) {
    if (dd->curr_rotate <= 1)  /* display rotated 0 or 180 degrees */
      return ( (100 * dd->width * dd->dsparea_height) / (dd->height * dd->dsparea_width));
    else                       /* display rotated 90 or 270 degrees */
      return ( (100 * dd->height * dd->dsparea_width) / (dd->width * dd->dsparea_height));
  } else {  /* display area dimensions unknown: default to quadratic pixels */
    return 100;
  }
}


/**
  * \brief   gets serdisp connect descriptor
  *
  * get serdisp connect descriptor used by the display
  *
  * \param   dd            device descriptor
  *
  * \return  serdisp connect descriptor (output device)
  */
serdisp_CONN_t* serdisp_getSDCONN(serdisp_t* dd) {
  return dd->sdcd;
}


/**
  * \brief   changes a display feature 
  *
  * \param   dd            device descriptor
  * \param   feature       feature to change:\n
  *          FEATURE_CONTRAST   .. change display contrast (value: 0-MAX_CONTRAST)\n
  *          FEATURE_BACKLIGHT  .. 0: off, 1: on, 2: toggle\n
  *          FEATURE_INVERT     .. 0: normal display, 1: inverted display, 2: toggle\n
  *          FEATURE_ROTATE     .. 0: normal, 1 or 180: bottom-up, 90: 90 degrees, 270: 270 degrees
  * \param   value         value for option (see above)
  *
  * \deprecated   superseded by serdisp_setoption()
  */
void serdisp_feature(serdisp_t* dd, int feature, int value) {
  switch (feature) {
    case FEATURE_CONTRAST:
      serdisp_setoption(dd, "CONTRAST", value);
      break;
    case FEATURE_INVERT:
      serdisp_setoption(dd, "INVERT", value);
      break;
    case FEATURE_BACKLIGHT:
      serdisp_setoption(dd, "BACKLIGHT", value);
      break;
    case FEATURE_ROTATE:
      serdisp_setoption(dd, "ROTATE", value);
      break;
  }
}


/**
  * \brief   changes a display option
  *
  * change a display option (replaces serdisp_feature())
  *
  * \param   dd            device descriptor
  * \param   optionname    name of option to change
  * \param   value         value for option
  *
  * \since   1.96
  */
void serdisp_setoption(serdisp_t* dd, const char* optionname, long value) {
  int idx;

  /* if no specific implementation of needed option: use generic one */
  if (!dd->fp_setoption(dd, optionname, value)) {
    if ( ((idx = serdisp_getstandardoptionindex(optionname)) != -1 ) && (idx == serdisp_getstandardoptionindex("INVERT"))) {
      int oldvalue = dd->curr_invert;

      /* option "INVERT" not defined in driver although dd->feature_invert is set to 1: set it back to 0 */
      if (dd->feature_invert) {
        dd->feature_invert = 0;
      }

      if (value < 2) 
        dd->curr_invert = value;
      else
        dd->curr_invert = (dd->curr_invert) ? 0 : 1;

      if (oldvalue != dd->curr_invert)
        serdisp_rewrite(dd);
    } else if ( ((idx = serdisp_getstandardoptionindex(optionname)) != -1 ) && (idx == serdisp_getstandardoptionindex("ROTATE"))) {
      int oldval = dd->curr_rotate;

      int newval = sdtools_rotate_deg2intern(dd, (int)value);

      /* rotate content only when 180 degree rotation (else clear display) */
      if (oldval != newval) {
        if ((oldval & 0x02) == (newval & 0x02))
          sdtools_generic_rotate(dd);
        else
          serdisp_clear(dd);

        dd->curr_rotate = newval;
      }
    }
    /* silently ignore other - unknown - options */
  }
}


/**
  * \brief   gets the value of a display option
  *
  * change a display option (old: 'feature') (new, preferred version)
  *
  * usually this function is only used for returning numeric values
  *
  * using the following string hack strings may also be returned:\n
  * if typesize is requested and filled with size == 0 then the option is of type 'string'\n
  * its address is returned as 'long' and has to be re-casted like so:
  *
  * \code
  * int typesize;
  * long retval = serdisp_getoption(dd, "name", &typesize);
  * if (typesize == 0)
  *   char* str = (char*) retval;
  * \endcode
  *
  * \param[in]    dd            device descriptor
  * \param[in]    optionname    name of option
  * \param[out]   typesize      pointer to value containing size of option's type (in byte)
  *                             (or, if 0 is passed here, this will be ignored)
  *
  * \retval       <>-1          value of display option
  * \retval       -1            option is unknown
  *
  * \since   1.96
  */
long serdisp_getoption(serdisp_t* dd, const char* optionname, int* typesize) {
  int stdidx = serdisp_getstandardoptionindex(optionname);


  if ( serdisp_compareoptionnames(dd, optionname, "INVERT") ) {
    if (typesize)
      *typesize = sizeof(int);

    return (long)(dd->curr_invert);
  } else if ( serdisp_compareoptionnames(dd, optionname, "ROTATE")) {
    if (typesize)
      *typesize = sizeof(int);

    return (long)sdtools_rotate_intern2deg(dd, dd->curr_rotate);
  } else if ( (serdisp_getstandardoptionindex("CONTRAST") == stdidx) && dd->feature_contrast) {
    if (typesize)
      *typesize = sizeof(int);

    return (long)sdtools_contrast_hw2norm(dd, dd->curr_contrast);
  } else if (serdisp_getstandardoptionindex("BRIGHTNESS") == stdidx) {
    if (typesize)
      *typesize = sizeof(int);
    return (long) (100 - dd->curr_dimming);
  } else if ( (serdisp_getstandardoptionindex("BACKLIGHT") == stdidx) && dd->feature_backlight) {
    if (typesize)
      *typesize = sizeof(int);

    return (long) dd->curr_backlight;  
  } else if (serdisp_getstandardoptionindex("WIDTH") == stdidx) {
    if (typesize)
      *typesize = sizeof(int);

    return (long) dd->width;  
  } else if (serdisp_getstandardoptionindex("HEIGHT") == stdidx) {
    if (typesize)
      *typesize = sizeof(int);

    return (long) dd->height;  
  } else if (serdisp_getstandardoptionindex("DEPTH") == stdidx) {
    if (typesize)
      *typesize = sizeof(int);
    return (long) dd->depth;
  } else if (serdisp_getstandardoptionindex("DELAY") == stdidx) {
    if (typesize)
      *typesize = sizeof(long);
    return (long) dd->delay;
  } else if (serdisp_getstandardoptionindex("DSPAREAWIDTH") == stdidx) {
    if (typesize)
      *typesize = sizeof(long);
    return (long) dd->dsparea_width;
  } else if (serdisp_getstandardoptionindex("DSPAREAHEIGHT") == stdidx) {
    if (typesize)
      *typesize = sizeof(long);
    return (long) dd->dsparea_height;
  } else if (serdisp_getstandardoptionindex("SELFEMITTING") == stdidx) {
    if (typesize)
      *typesize = sizeof(byte);
    return (long) ((SD_CS_ISSELFEMITTING(dd)) ? 1 : 0);

  /* driver-specific values */
  } else {
    int i = 0;
    long retval = -1;
    while (i < dd->amountoptions) {
      if (serdisp_compareoptionnames(dd, optionname, dd->options[i].name)) {
        int typesize_temp;
        void* valueptr;
        /* fp_getvalueptr needs to be defined */
        if (dd->fp_getvalueptr) {
          valueptr = dd->fp_getvalueptr(dd, optionname, &typesize_temp);
          switch (typesize_temp) {
            case 1:  retval = *((byte*) valueptr); break;
            case 2:  retval = *((short*) valueptr); break;
            case 4:  retval = *((long*) valueptr); break;
          }
          return retval;
        } else
          return -1;
      }
      i++;
    }
  }
  return -1;
}


/**
  * \brief   tests if option is supported
  *
  * \param   dd            device descriptor
  * \param   optionname    name of option to test
  *
  * \retval  1      option is supported and read/writeable
  * \retval -1      option is supported but read-only
  * \retval  0      option is not supported
  *
  * \since   1.96
  */
int serdisp_isoption(serdisp_t* dd, const char* optionname) {
  serdisp_options_t optiondesc;

  if (!serdisp_getoptiondescription(dd, optionname, &optiondesc))
    return 0;

  return (optiondesc.flag & SD_OPTIONFLAG_RW ) ? 1 : -1;
}


/**
  * \brief   gets a description to a given option
  *
  * \param[in]   dd            device descriptor
  * \param[in]   optionname    name of option (name or aliasname)
  * \param[out]  optiondesc    address of option descriptor
  *
  * \retval  1      option is available
  * \retval  0      option unknown/unsupported
  *
  * \since   1.96
  */
int serdisp_getoptiondescription(serdisp_t* dd, const char* optionname, serdisp_options_t* optiondesc) {
  int stdidx = serdisp_getstandardoptionindex(optionname);
  int optidx = serdisp_getoptionindex(dd, optionname);

  /* special treatment for contrast and backlight: if one is not supported: return 0 (unknown/unsupported) */
  if ( (serdisp_getstandardoptionindex("BACKLIGHT") == stdidx && !dd->feature_backlight) || 
       (serdisp_getstandardoptionindex("CONTRAST") == stdidx && !dd->feature_contrast)
     )
    return 0;

  /* optionname == alias name -> search again using 'name' */
  if (stdidx != -1 && optidx == -1)
    optidx = serdisp_getoptionindex(dd, serdisp_standardoptions[stdidx].name);

  /* read/write standard option or option defined in serdisp_standardoptions[] and not in driver options */
  if (stdidx != -1 && optidx == -1) {
    optiondesc->name       = serdisp_standardoptions[stdidx].name;
    optiondesc->aliasnames = serdisp_standardoptions[stdidx].aliasnames;
    optiondesc->minval     = serdisp_standardoptions[stdidx].minval;
    optiondesc->maxval     = serdisp_standardoptions[stdidx].maxval;
    optiondesc->modulo     = serdisp_standardoptions[stdidx].modulo;
    optiondesc->defines    = serdisp_standardoptions[stdidx].defines;
                             /* mask out SD_OPTIONFLAG_STD */
    optiondesc->flag       = serdisp_standardoptions[stdidx].flag & ((byte)( ~ SD_OPTIONFLAG_STD));
    return 1;
  }

  if (optidx != -1) {
    optiondesc->name       = dd->options[optidx].name;
    optiondesc->aliasnames = (stdidx == -1 || strlen(dd->options[optidx].aliasnames) > 0) 
                             ? dd->options[optidx].aliasnames
                             : serdisp_standardoptions[stdidx].aliasnames;
    optiondesc->minval     = (stdidx == -1 || dd->options[optidx].minval != -1) 
                             ? dd->options[optidx].minval
                             : serdisp_standardoptions[stdidx].minval;
    optiondesc->maxval     = (stdidx == -1 || dd->options[optidx].maxval != -1) 
                             ? dd->options[optidx].maxval
                             : serdisp_standardoptions[stdidx].maxval;
    optiondesc->modulo     = (stdidx == -1 || dd->options[optidx].modulo != -1) 
                             ? dd->options[optidx].modulo
                             : serdisp_standardoptions[stdidx].modulo;
    optiondesc->defines    = (stdidx == -1 || strlen(dd->options[optidx].defines) > 0) 
                             ? dd->options[optidx].defines
                             : serdisp_standardoptions[stdidx].defines;
    optiondesc->flag       = dd->options[optidx].flag;
    return 1;
  }
  return 0;
}


/**
  * \brief   gets the next option description
  *
  * get the next option description (iterates through the options supported by the display)\n\n
  * 
  * initialise <tt>optiondesc</tt> with <tt>optiondesc.name = ""</tt> to start the iteration\n\n
  * 
  * \em eg:
  * \code 
  * serdisp_options_t optiondesc;
  * optiondesc.name = "";
  * 
  * while(serdisp_nextoptiondescription(dd, &optiondesc)) {
  *   printf("%s\n", optiondesc.name);
  * }
  * \endcode
  *
  * \param[in]   dd            device descriptor
  * \param[out]  optiondesc    address of option descriptor
  *
  * \retval  1      successful
  * \retval  0      unsuccessful (no more option has been found)
  *
  * \since   1.96
  */
int serdisp_nextoptiondescription(serdisp_t* dd, serdisp_options_t* optiondesc) {
  int idx;
  int found;
  int foundtemp = 0;


  idx = 0;
  found = 0;
  if (optiondesc->name && strlen(optiondesc->name) > 0) {
    int stdidx = serdisp_getstandardoptionindex(optiondesc->name);

    if (stdidx != -1) {
      foundtemp = 1;  /* current optiondesc->name == standard option */
      idx = stdidx;

      while (!found && idx < sizeof(serdisp_standardoptions) / sizeof(serdisp_options_t) ) {
        idx++;
        /* search next standard option in serdisp_standardoptions[] */

        if ( (idx >= sizeof(serdisp_standardoptions) / sizeof(serdisp_options_t)) ||
             (idx == serdisp_getstandardoptionindex("BACKLIGHT") && !dd->feature_backlight) ||
             (idx == serdisp_getstandardoptionindex("CONTRAST") && !dd->feature_contrast)
        ) {
          ;
        } else {
          found = 1;
        }
      }
    }
  } else {
    /* empty name: return 1st standard option */
    idx = 0;
    found = 1;
  }

  if (found) {
    int retval = serdisp_getoptiondescription(dd, serdisp_standardoptions[idx].name, optiondesc);
    if (!retval) sd_error(SERDISP_ERUNTIME, "standardoption name %s -> retval %d\n",  serdisp_standardoptions[idx].name, retval);
    return 1;
  }

  idx = 0;
  /* driver dep. options */
  if (foundtemp) {
    /* latest option description was last standard option, so return 1st driver dep. option 
       (not already defined in serdisp_standardoptions[]) */
    found = 1;
  } else {
    while (!found && idx < dd->amountoptions ) {
      int optidx = serdisp_getoptionindex(dd, optiondesc->name);
/*      if (serdisp_compareoptionnames(dd, optiondesc->name, dd->options[idx].name)) {*/
      if (optidx == idx) {
        if (idx+1 < dd->amountoptions)
          found = 1;
        idx++;
      } else
        idx++;
    }
  }

  if (found) {
    foundtemp = 0;
    /* if option at index found above equals option already defined in serdisp_standardoptions[] -> skip */
    while (!foundtemp && idx < dd->amountoptions ) {
      int stdidx = serdisp_getstandardoptionindex(dd->options[idx].name);
      if (stdidx == -1)
        foundtemp = 1;
      else
        idx++;
    }
  }

  if (found && foundtemp) {
    int retval = serdisp_getoptiondescription(dd, dd->options[idx].name, optiondesc);
    if (!retval) sd_error(SERDISP_ERUNTIME, "option name %s -> retval 0   idx: %d   amount: %d\n",  
                                           dd->options[idx].name, idx, dd->amountoptions);
    return 1;
  }

  return 0;
}


/**
  * \brief   gets description to a display
  *
  * \param[in]   displayname   display/device name
  * \param[out]  displaydesc   address of display descriptor
  *
  * \retval  1      display available
  * \retval  0      display unknown / unsupported
  *
  * \b Example:
  * \code
  * serdisp_display_t displaydesc;
  * displayname = "lph7366";
  *
  * int rc = serdisp_getdisplaydescription(displayname, &displaydesc);
  *
  * if (rc)
  *   printf("description for display %s: %s\n", displayname, displaydesc.description);
  * \endcode
  *
  * \since   1.96
  */
int serdisp_getdisplaydescription(const char* displayname, serdisp_display_t* displaydesc) {
  int idx = serdisp_getdispindex(displayname);

  if (idx != -1) {
    displaydesc->dispname = serdisp_displays[idx].dispname;
    displaydesc->aliasnames = serdisp_displays[idx].aliasnames;
    displaydesc->optionstring = serdisp_displays[idx].defaultoptions;
    displaydesc->description = serdisp_displays[idx].description;
    return 1;
  }
  return 0;
}


/**
  * \brief   iterates through supported displays
  *
  * iterates through supported displays \n
  * the iteration is started with assigning an empty string to \p optiondesc.dispname
  *
  * \param[out]  displaydesc   address of display descriptor
  *
  * \retval  1      successful
  * \retval  0      unsuccessful (no more supported diplays have been found)
  * 
  * \b Example: \n\n
  * (prints all supported displays (main display name and alias names))
  * \code
  * serdisp_display_t displaydesc;
  * displaydesc.dispname = "";
  * 
  * while(serdisp_nextdisplaydescription(&displaydesc)) {
  *   printf("name: %s   aliases: %s\n", displaydesc.dispname, displaydesc.aliasnames);
  * }
  * \endcode
  *
  * \since   1.96
  */
int serdisp_nextdisplaydescription(serdisp_display_t* displaydesc) {
  int idx;
  if (displaydesc->dispname && strlen(displaydesc->dispname) > 0) {
    idx = serdisp_getdispindex(displaydesc->dispname);
    if (idx == -1) /* shouldn't occur in theory */
      return 0;
    idx++;
  } else {
    idx = 0;
  }

  if ( idx <  sizeof(serdisp_displays) / sizeof(serdisp_setup_t) ) {
    displaydesc->dispname = serdisp_displays[idx].dispname;
    displaydesc->aliasnames = serdisp_displays[idx].aliasnames;
    displaydesc->optionstring = serdisp_displays[idx].defaultoptions;
    displaydesc->description = serdisp_displays[idx].description;
    return 1;
  }
  return 0;
}


/**
  * \brief   tests if display is supported
  *
  * \param          displayname   name or alias name of display to test
  *
  * \retval  1      display is supported
  * \retval  0      display is unsupported
  *
  * \since   1.96
  */
int serdisp_isdisplay(const char* displayname) {
  serdisp_display_t displaydesc;

  return (serdisp_getdisplaydescription(displayname, &displaydesc) == 1);
}


/**
  * \brief   gets display description for the display given by a device descriptor
  *
  * \param[in]   dd             device descriptor
  * \param[out]  displaydesc    address of display descriptor
  *
  * \b Example:
  * \code
  * serdisp_display_t displaydesc;
  * 
  * serdisp_currdisplaydescription(dd, &displaydesc)) {
  * printf("description of active display: %s\n", displaydesc.description);
  * \endcode
  *
  * \since   1.96
  */
void serdisp_currdisplaydescription(serdisp_t* dd, serdisp_display_t* displaydesc) {
  if (dd) {
    int rv = serdisp_getdisplaydescription(dd->dsp_name, displaydesc);
    if (!rv) {
      /* shouldn't happen in theory */
      sd_debug(0, "%s(): INTERNAL ERROR: no display description found for %s\n", __func__, dd->dsp_name);
      displaydesc->dispname = (char*)0;
      displaydesc->aliasnames = (char*)0;
      displaydesc->optionstring = (char*)0;
      displaydesc->description = (char*)0;
      return;
    }
    displaydesc->optionstring = dd->dsp_optionstring;  
  }
}


/**
  * \brief   gets unprocessed display name
  *
  * gets the display name (unprocessed, spelling as it was used for serdisp_init())
  *
  * \param   dd             device descriptor
  *
  * \return  display name
  *
  * \since   1.96
  */
const char* serdisp_getdisplayname(serdisp_t* dd) {
  return dd->dsp_name;
}


/**
  * \brief   iterates through wiring descriptions for a display
  *
  * iterates through wiring definitions supported by display \p displayname \n
  * the iteration is started with assigning an empty string to \p wiredesc.name
  *
  * \param[in]   displayname             display name or alias name
  * \param[out]  wiredesc                address of wiring descriptor
  *
  * \retval      1       successful
  * \retval      0       unsuccessful (no more wirings found)
  *
  * \b Example:
  * \code
  * serdisp_wiredef_t wiredesc;
  * wiredesc.name = "";
  *
  * while(serdisp_nextwiringdescription("PCD85644", &wiredesc)) {
  *   printf("%s\n", wiredesc.name);
  * }
  * \endcode
  *
  * \since   1.96
  */
int serdisp_nextwiringdescription(const char* displayname, serdisp_wiredef_t* wiredesc) {
  int dispidx = serdisp_getdispindex(displayname);
  int idx = 0;
  serdisp_t* dd;
  int failed = 0;

  if (dispidx == -1) /* display name not found */
    return 0;

  dd = serdisp_displays[dispidx].fp_setup(NULL, displayname, "");

  if (!dd) {
    sd_debug(0, "serdisp_nextwiringdescription(); could not get descriptor for display %s. last error: %s", displayname, sd_errormsg);
    return 0;
  }

  if (!dd->amountwiredefs)
    failed = 1;


  if (!failed && wiredesc->name && strlen(wiredesc->name) > 0) {
    int found = 0;
    idx = 0;
    while (!found && idx < dd->amountwiredefs) {
      if (sdtools_ismatching(wiredesc->name, -1, dd->wiredefs[idx].name, -1) )
        found = 1;

      idx++;
    }
  } else {
    idx = 0;
  }

  if ( !failed && idx <  dd->amountwiredefs ) {
    wiredesc->id = dd->wiredefs[idx].id;
    wiredesc->conntype = dd->wiredefs[idx].conntype;
    wiredesc->name = dd->wiredefs[idx].name;
    wiredesc->definition = dd->wiredefs[idx].definition;
    wiredesc->description = dd->wiredefs[idx].description;
  } else {
    failed = 1;
  }

  serdisp_freeresources(dd);
  return (failed) ? 0 : 1;
}


/**
  * \brief   closes display without erasing it
  *
  * close display but without erasing its content and without switching it off. the output device remains opened.
  *
  * this function may for example be used for programs that want to output something and
  * than exit, but without clearing the display (for this, SDCONN_close() shouldn't be called either)
  *
  * \param   dd            device descriptor
  *
  * \attention
  * this will NOT work as expected with serial port and ioctl
  * (TxD will be set to low in any case -> so display will be w/o power) 
  * so the only solution would be a separate power supply when using ioctl.
  * \n \n
  * this seems to be an operating system specific behaviour and \em cannot be influenced\n \n
  * \p but: directIO works as expected (TxD will NOT be reset after program exit)
  */
void serdisp_close(serdisp_t* dd) {
  sd_debug(2, "%s(): entering", __func__);
  /*  dd->fp_close(dd);*/
  /*  SDCONN_close(dd->sdcd);*/
  serdisp_freeresources(dd);
  dd = 0;
}


/**
  * \brief   closes display
  *
  * clears and switches off the display and releases the output device
  *
  * \param   dd            device descriptor
  *
  * \since   1.93
  */
void serdisp_quit(serdisp_t* dd) {
  sd_debug(2, "%s(): entering", __func__);

  dd->fp_close(dd);
/*  free(dd->scrbuf);
  free(dd->scrbuf_chg);*/
  SDCONN_close(dd->sdcd);
  serdisp_freeresources(dd);
  dd = 0;
}


/**
  * \brief   changes a pixel in the display buffer
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  * \param   colour        colour representation dependent on colour-scheme in use <pre>
    monochrome:       0: pixel not set, <>0: pixel set
    greyscale:        if not 256 colours: index of greyvalue, else greyvalue
    indexed colour:   index of colour-entry
    packed colour:    packed representation (eg. RGB444, RGB332, ...)
    true colour:      eg. 0xAARRGGBB (dependend on RGB-scheme (RGB, BGR, ..)
    </pre>
  * 
  * \attention
  * this function is <i>hardware dependent</i>! \n
  * for <i>hardware independent</i> pixel changing serdisp_setcolour() should be used.
  */
void serdisp_setpixel(serdisp_t* dd, int x, int y, long colour) {
  dd->fp_setpixel(dd, x, y, colour);
}


/**
  * \brief   gets pixel at position (x/y)
  *
  * gets hardware dependend colour information at position (x/y)
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  *
  * \return hardware dependent colour information at (x/y)
  * 
  * \attention
  * this function is <i>hardware dependent</i>! \n
  * for getting the <i>hardware independent</i> colour information serdisp_getcolour() should be used.
  */
long serdisp_getpixel(serdisp_t* dd, int x, int y) {
  return dd->fp_getpixel(dd, x, y);
}


/**
  * \brief   resets the display buffer
  *
  * resets the internal display buffer
  *
  * \param   dd            device descriptor
  *
  * \attention
  * display will \em not be redrawn! \n
  * serdisp_clear() clears \em and redraws the display.
  */
void serdisp_clearbuffer(serdisp_t* dd) {
  sd_debug(2, "%s(): entering", __func__);

  memset(dd->scrbuf, ((SD_CS_ISGREY(dd) || SD_CS_ISSELFEMITTING(dd)) ? 0x00 : 0xFF), dd->scrbuf_size);
/*  memset(dd->scrbuf, ((dd->depth > 1) ? 0xFF : 0x00), dd->scrbuf_size); */
  memset(dd->scrbuf_chg, 0xFF, dd->scrbuf_chg_size);
  sd_debug(2, "%s(): leaving", __func__);
}


/**
  * \brief   updates whole display
  *
  * \param   dd            device descriptor
  */
void serdisp_update(serdisp_t* dd) {
  sd_debug(2, "%s(): entering", __func__);

  dd->fp_update(dd);
  sd_debug(2, "%s(): leaving", __func__);
}


/**
  * \brief   clears whole display
  *
  * \param   dd            device descriptor
  */
void serdisp_clear(serdisp_t* dd) {
  sd_debug(2, "%s(): entering", __func__);

  serdisp_clearbuffer(dd);
  if (dd->fp_clear)
    dd->fp_clear(dd);
  else
    dd->fp_update(dd);
  sd_debug(2, "%s(): leaving", __func__);
}


/**
  * \brief   rewrites whole display
  *
  * \param   dd            device descriptor
  */
void serdisp_rewrite(serdisp_t* dd) {
  sd_debug(2, "%s(): entering", __func__);

  memset(dd->scrbuf_chg, 0xFF, dd->scrbuf_chg_size);
  dd->fp_update(dd);
  sd_debug(2, "%s(): leaving", __func__);
}


/**
  * \brief   re-initialises the display
  *
  * \param   dd            device descriptor
  *
  * \retval  1             successful reset
  * \retval  0             unsucessful reset
  */
int serdisp_reset(serdisp_t* dd) {
  sd_debug(2, "%s(): entering", __func__);

  /* close display (but NOt serdisp_close() because this one would also free dd) */
  dd->fp_close(dd);

  sleep(1);

  /* re-init display */
  dd->fp_init(dd);
  if (dd->feature_contrast) serdisp_setoption(dd, "CONTRAST", MAX_CONTRASTSTEP / 2);
  serdisp_rewrite(dd);
  sd_runtimeerror = 0;

  sd_debug(2, "%s(): leaving", __func__);
  return (sd_runtimeerror) ? 0 : 1 ;
}


/**
  * \brief   resets the display
  *
  * resets the display (clears runtime_error flag, closes and reopens device, re-inits display)
  *
  * \param   dd            device descriptor
  *
  * \retval  !NULL         device descriptor
  * \retval  NULL          unsucessful reset
  */
serdisp_t* serdisp_fullreset(serdisp_t* dd) {
  serdisp_CONN_t* sdcd = dd->sdcd;
  char* sdcdev;

  sd_debug(2, "%s(): entering", __func__);

  /* full reset not supported for imported devices */
  if (!sdcd->sdcdev || strlen(sdcd->sdcdev) == 0) {
    sd_debug(1, "%s(): device was imported using SDCONN_import_PP(). thus a full reset is not supported", __func__);
    sd_debug(1, "%s(): serdisp_reset() will be used instead", __func__);
    return (serdisp_reset(dd)) ? dd : 0;
  }

  /* save pointer to string constant sdcd->sdcdev */
  sdcdev = sdcd->sdcdev;

  /* close (force close) device */
  SDCONN_close(sdcd);
  sdcd = 0;

  /* reopen device */
  sdcd = SDCONN_open(sdcdev);

  if (sdcd) {
    /* re-init display */
    dd->sdcd = sdcd;
    sd_runtimeerror = 0;
    dd->fp_init(dd);
    if (dd->feature_contrast) serdisp_setoption(dd, "CONTRAST", MAX_CONTRASTSTEP / 2);

    serdisp_rewrite(dd);
  } else {
    sd_error(SERDISP_ERUNTIME, "%s() failed to re-open device %s", __func__, sdcdev);
    sd_runtimeerror = 1;
    return 0;
  }

  sd_debug(1, "%s(): reset %ssuccessful", __func__, (sd_runtime_error() ? "un" : ""));

  sd_debug(2, "%s(): leaving", __func__);
  return dd ;
}


/**
  * \brief   flashes the display or the background light
  *
  * blinks/flashes either the display or the background light
  *
  * \param   dd            device descriptor
  * \param   what          0: flashes backlight, 1: blinks display by inverting the display
  * \param   cnt           how often should there be blinking
  * \param   delta         delay between two blinking intervals
  */
void serdisp_blink(serdisp_t* dd, int what, int cnt, int delta) {
  int n;

  delta *= 1000;

  /* cnt will be cnt*2 because 10 times blinking ==> 20 times toggling */
  for (n = 1; n <= (cnt<<1); n++) {
    if (what == 0 && dd->feature_backlight) {
      serdisp_setoption(dd, "BACKLIGHT", SD_OPTION_TOGGLE);
      SDCONN_usleep(dd->sdcd, delta);
    } else if (what == 1 /* && dd->feature_invert*/) {
      serdisp_setoption(dd, "INVERT", SD_OPTION_TOGGLE);
      SDCONN_usleep(dd->sdcd, delta);
    }
  }
}


/**
  * \brief   changes an area in the display buffer
  *
  * \param   dd            device descriptor
  * \param   x             x-position top/left
  * \param   y             y-position top/left
  * \param   w             width of content
  * \param   h             height of content
  * \param   data          pixel/colour data (one byte == one pixel)
  *
  * \deprecated this function only works with dephts <= 8 and will be replaced through better functions
  *
  */
void serdisp_setpixels(serdisp_t* dd, int x, int y, int w, int h, byte* data) {
  int i,j;

  if (dd->depth <= 8)
    for (j = 0; j < h; j++)
      for (i = 0; i < w; i++)
        dd->fp_setpixel(dd, i+x, j+y, data[ j*w + i]);
#if 0  /* will not work -> better functions planned as replacement for serdisp_setpixels() */
  else /* 4 bytes per colour. never used/tested, but here for completeness */
    for (j = 0; j < h; j++)
      for (i = 0; i < w; i++)
        /* 4 bytes -> 1 int */
        dd->fp_setpixel(dd, i+x, j+y,  data[ 4*j*w + 4*i+0]      +
                                       (data[ 4*j*w + 4*i+1]<<8)  +
                                       (data[ 4*j*w + 4*i+2]<<16) +
                                       (data[ 4*j*w + 4*i+3]<<24)  );
#endif
}






/* *********************************
   int serdisp_getdispindex(dispname)
   *********************************
   returns index of of dispname in display array or -1 if not found
   *********************************
   dispname   ... display name
   *********************************
   returns index in display array or -1 if not found 
*/
int serdisp_getdispindex(const char* dispname) {
  int idx = 0;

  while (idx < sizeof(serdisp_displays) / sizeof(serdisp_setup_t) ) {
    if (sdtools_ismatching(serdisp_displays[idx].dispname, -1, dispname, -1) ||
        sdtools_isinelemlist(serdisp_displays[idx].aliasnames, dispname, -1) > -1
    ) {
      /* early exit */
      return idx;
    } else {
      idx++; 
    }
  }

  return -1;
}


/* *********************************
   int serdisp_getstandardoptionindex(optionnanme)
   *********************************
   returns index of of standardoption in standard options array or -1 if not found
   *********************************
   optionname   ... name of standard option (if a '=' occurs, only characters till '=' are used)
                    eg: "DEPTH=4;SOMEOPT=4711"  ->  "DEPTH"
   *********************************
   returns index in standardoption array or -1 if not found 
*/
int serdisp_getstandardoptionindex(const char* optionname) {
  int optidx = 0;
  char* tokenidx = strchr(optionname, '=');
  int namelen = (tokenidx) ? serdisp_ptrstrlen(tokenidx, optionname) : -1;

  while (optidx < sizeof(serdisp_standardoptions) / sizeof(serdisp_options_t) ) {
    if (sdtools_ismatching(serdisp_standardoptions[optidx].name, -1, optionname, namelen) ||
        sdtools_isinelemlist(serdisp_standardoptions[optidx].aliasnames, optionname, namelen) > -1
    ) {
      /* early exit */
      return optidx;
    } else {
      optidx++; 
    }
  }
  return -1;
}



/* *********************************
   int serdisp_getoptionindex(dd, optionnanme)
   *********************************
   returns index of of driver specific option in options array or -1 if not found
   *********************************
   optionname   ... name of option (if a '=' occurs, only characters till '=' are used)
                    eg: "DEPTH=4;SOMEOPT=4711"  ->  "DEPTH"
   *********************************
   returns index in driver specific option array or -1 if not found 
*/
int serdisp_getoptionindex(serdisp_t* dd, const char* optionname) {
  int optidx = 0;
  char* tokenidx = strchr(optionname, '=');
  int namelen = (tokenidx) ? serdisp_ptrstrlen(tokenidx, optionname) : -1;
  char* optionname_full = (char*) optionname;

  if (!dd->options)
    return -1;

  /* is optionname an alias which is defined in standard options? if so, use non-alias name of option */
  /* eg: optioname = 'BG' -> use 'BACKLIGHT' instead for searching in dd->options */
  if ( (optidx = serdisp_getstandardoptionindex(optionname)) >= 0) {
    optionname_full = serdisp_standardoptions[optidx].name;
    namelen = -1;  /* in this case optionname_full is \0-terminated */
  }

  optidx = 0;
  while (optidx < dd->amountoptions ) {
    if (sdtools_ismatching(dd->options[optidx].name, -1, optionname_full, namelen) ||
        sdtools_isinelemlist(dd->options[optidx].aliasnames, optionname_full, namelen) > -1
    ) {
      /* early exit */
      return optidx;
    } else {
      optidx++; 
    }
  }
  return -1;
}



/* *********************************
   int serdisp_comparedispnames(dispname1, dispname2)
   *********************************
   compares whether the two display names are equal and respectively aliases or not 
   *********************************
   dispname1   ... display name 1
   dispname2   ... display name 2
   *********************************
   returns 1 (equal or aliases) or 0 (not equal nor aliases)
*/
int serdisp_comparedispnames(const char* dispname1, const char* dispname2) {
  int i1 = serdisp_getdispindex(dispname1);
  int i2 = serdisp_getdispindex(dispname2);
  return ((i1 != -1) && (i1 == i2));
}


/* *********************************
   int serdisp_compareoptionnames(dd, optionname1, optionname2)
   *********************************
   compares whether the two option names are equal and respectively aliases or not 
   option has to be defined by specific driver (being known in standard options is not enough)
   *********************************
   dd            ... display descriptor
   optionname1   ... option name 1
   optionname2   ... option name 2
   *********************************
   returns 1 (equal or aliases) or 0 (not equal nor aliases)
*/
int serdisp_compareoptionnames(serdisp_t* dd, const char* optionname1, const char* optionname2) {
  int si1, si2, i1, i2;
  i1 = serdisp_getoptionindex(dd, optionname1);
  i2 = serdisp_getoptionindex(dd, optionname2);

  if (( i1 != -1) && ( i1 ==  i2))
    return 1;

  /* not equal / aliases when using only driver specific option definitions: try standard options */
  si1 = serdisp_getstandardoptionindex(optionname1);
  si2 = serdisp_getstandardoptionindex(optionname2);

  return (
    (si1 != -1) && (si1 == si2) && 
    /* either driver independent option (flag == 1) or also known in driver specific option array */
    ((serdisp_standardoptions[si1].flag & SD_OPTIONFLAG_STD) || 
     (serdisp_getoptionindex(dd, serdisp_standardoptions[si1].name) != -1)
    )
  );
}


/****************************/
/*                          */
/*   non public functions   */
/*                          */
/****************************/


char* serdisp_getwiresignalname(serdisp_t* dd, int idx) {
  int found = 0;
  int i = 0;
  while (!found && i < dd->amountwiresignals) {
    if ((dd->wiresignals[i].index == idx) && (dd->sdcd->conntype == dd->wiresignals[i].conntype))
      found = 1;
    else
      i++;
  }
  
  if (found)
    return dd->wiresignals[i].signalname;
  else
    return "";
}


/* *********************************
   int serdisp_setupwirings(sdcd, wiredef, wiredef_len)
   *********************************
   set up wirings
   *********************************
   dd          ... display descriptor
   wiredef     ... string describing wirings
   wiredef_len ... max. of characters to read from wiredef (or 0 if whole string is to be read)
   *********************************
   returns 0 if everything was ok or 1 if an error ocurred
*/
int serdisp_setupwirings(serdisp_t* dd, const char* wiredef, int wiredef_len) {

  char buffer[50];  /* needed for error messages */

  int i, found;
  long tempnum;
  char* tempptr;
  
  char* patternptr = (char*) wiredef;
  int   patternlen = -1;
  int   patternborder = (wiredef_len <= 0) ? strlen(wiredef) : wiredef_len;

  /* initialise io flags */
  dd->sdcd->io_flags_readstatus = 0;
  dd->sdcd->io_flags_writedata  = 0;
  dd->sdcd->io_flags_writecmd   = 0;

  /* setup wiring only for protocols EMULATION (or if protocol == DONTCARE) */
  if ( ! (dd->sdcd->protocol == SDPROTO_DONTCARE || dd->sdcd->protocol == SDPROTO_EMULATION))
    return 0;

  /* no customisable wiring supported by this driver -> return */
  if (! dd->wiresignals && ! dd->wiredefs)
    return 0;

  /* catch some errors */
  if (! dd->wiresignals && dd->wiredefs) {
    sd_error(SERDISP_EINVAL, "array containing wiring definitions without array containing wiring items given");
    return 1;
  }

  if (dd->wiresignals && ! dd->wiredefs) {
    sd_error(SERDISP_EINVAL, "array containing wiring items without array containing wiring definitions given");
    return 1;
  }

  if (strlen(wiredef) <= 0) {
    sd_error(SERDISP_EINVAL, "no or invalid wiring definition given");
    return 1;
  }

  if (! dd->amountwiresignals || ! dd->amountwiredefs) {
    sd_error(SERDISP_EINVAL, "error in driver module: amountwiresignals and/or amountwiredefs undefined. check source code");
    return 1;
  }

  /* no customisable wiring for connection type -> return */
  found = 0;
  i = 0;
  while (!found && i < dd->amountwiredefs) {
    if (dd->wiredefs[i].conntype == dd->sdcd->conntype)
      found = 1;
    else
      i++;
  }
  
  if (!found)
    return 0;
    

  /* test if wiredef = predefined numeric id or string name */

  tempnum = strtol(patternptr, &tempptr, 10);
  /* verify if patternptr contained a valid number */
  if (patternptr == tempptr || ( (*tempptr != '\0') && (tempptr < (patternptr + patternborder)) ) )
    tempnum = -1;   /* wiredef != valid numeric value */
  i = 0;
  found = 0;
  while (!found && i < dd->amountwiredefs) {

    if (dd->sdcd->conntype == dd->wiredefs[i].conntype &&   /* connection matches */
         ( tempnum == dd->wiredefs[i].id ||                 /* wiredef = matching numeric id  or string name */
           sdtools_ismatching(patternptr, patternborder, dd->wiredefs[i].name, -1)
         )
       ) {
      /* found predefined numeric id or string name -> use corresponding wire definition */
      found = 1;
      patternptr = dd->wiredefs[i].definition;
      patternborder = strlen(patternptr);
    }
    i++;
  }


  /* parse wiredef */
  i = 0;
  while( (patternptr = sdtools_nextpattern(patternptr, ',', &patternlen, &patternborder)) ) {
    char* valueptr = 0;
    int valuelen = 0;
    char* idxpos = index(patternptr, ':');
    int keylen = patternlen;
    
    int tabidxkey = 0, tabidxvalue;

    /* ':' found and position not outside patternlen? */
    if (idxpos &&  serdisp_ptrstrlen(idxpos, patternptr) < patternlen ) {
      keylen = serdisp_ptrstrlen(idxpos, patternptr);
      valueptr = ++idxpos;
      valuelen = patternlen - keylen - 1;
    }

    /* no key/value pair: try DATA definitions */
    if ( !valueptr ) {
      if (i != 0) {
        sd_error(SERDISP_EINVAL, "error in wiring definition: a DATA-definition is permitted only at 1st position (eg: 'DATA8,CS:INIT,...')");
        return 1;
      }
      if (dd->sdcd->conntype != SERDISPCONNTYPE_PARPORT) {
        snprintf(buffer, ( (keylen >= 49) ? 50 : keylen+1), "%s", patternptr);
        sd_error(SERDISP_EINVAL, "error in wiring definition: %s is only allowed with parallel wiring", buffer );
        return 1;
      }
      if ( sdtools_ismatching( patternptr, keylen, "DATA4", -1) || sdtools_ismatching( patternptr, keylen, "DATA8", -1)) {
        dd->sdcd->signals[0] = SD_PP_D0;
        dd->sdcd->signals[1] = SD_PP_D1;
        dd->sdcd->signals[2] = SD_PP_D2;
        dd->sdcd->signals[3] = SD_PP_D3;
        
        dd->sdcd->io_flags_writedata |= SD_PP_WRITEDB;
        dd->sdcd->io_flags_readstatus |= SD_PP_READDB;        
      }
      if ( sdtools_ismatching( patternptr, keylen, "DATA8", -1)) {
        dd->sdcd->signals[4] = SD_PP_D4;
        dd->sdcd->signals[5] = SD_PP_D5;
        dd->sdcd->signals[6] = SD_PP_D6;
        dd->sdcd->signals[7] = SD_PP_D7;
      }
    } else {
      found = 0;  /* stays 0 when 'permanent on'-definition */
      
      /* "permanent on" - definitions (eg: 1:nSTRB) */
      if (sdtools_ismatching(patternptr, keylen, "1", -1) ) {
        tabidxvalue = SDCONN_getsignalindex(valueptr, dd->sdcd->conntype, dd->sdcd->hardwaretype);
  
        if (tabidxvalue == -1) {
          snprintf(buffer, ( (valuelen >= 49) ? 50 : valuelen+1), "%s", valueptr);
          sd_error(SERDISP_EINVAL, "error in wiring definition: value '%s' is unknown in this context", buffer );
          return 1;
        }
        dd->sdcd->signals_permon |= SDCONN_getsignalvalue(tabidxvalue);        
      } else {
        tabidxkey = 0;
        while (!found && tabidxkey < dd->amountwiresignals) {

          if ( dd->sdcd->conntype == dd->wiresignals[tabidxkey].conntype && 
               sdtools_ismatching(patternptr, keylen, dd->wiresignals[tabidxkey].signalname, -1) 
             ) {
            found = 1;
          } else
            tabidxkey++;
        }
        if (!found) {
          snprintf(buffer, ( (keylen >= 49) ? 50 : keylen+1), "%s", patternptr);
          sd_error(SERDISP_EINVAL, "error in wiring definition: key '%s' is unknown in this context", buffer );
          return 1;
        }

        tabidxvalue = SDCONN_getsignalindex(valueptr, dd->sdcd->conntype, dd->sdcd->hardwaretype);
  
        if (tabidxvalue == -1) {
          snprintf(buffer, ( (valuelen >= 49) ? 50 : valuelen+1), "%s", valueptr);
          sd_error(SERDISP_EINVAL, "error in wiring definition: value '%s' is unknown in this context", buffer );
          return 1;
        }

        dd->sdcd->signals[ dd->wiresignals[tabidxkey].index ] = SDCONN_getsignalvalue(tabidxvalue);
      }

      if (dd->sdcd->conntype == SERDISPCONNTYPE_PARPORT) {
        char cord = (!found) ? 'C' : dd->wiresignals[tabidxkey].cord;
        
        if ( SDCONN_getsignalvalue(tabidxvalue) & 0x000000FF ) {  /* data bits */
          dd->sdcd->io_flags_readstatus   = 0;
          if (cord == 'D' ) dd->sdcd->io_flags_writedata  |= SD_PP_WRITEDB;
          else if (cord == 'C' ) dd->sdcd->io_flags_writecmd  |= SD_PP_WRITEDB;
          if (cord == 'C') dd->sdcd->io_flags_readstatus  |= SD_PP_READDB;
        } else if ( SDCONN_getsignalvalue(tabidxvalue) & 0x0000FF00 ) {  /* status bits */
          if (cord == 'C') dd->sdcd->io_flags_readstatus  |= SD_PP_READSB;
        } else if ( SDCONN_getsignalvalue(tabidxvalue) & 0x00FF0000 ) {  /* control bits */
          if (cord == 'D') dd->sdcd->io_flags_writecmd  |= SD_PP_WRITECB;
          else if (cord == 'C') dd->sdcd->io_flags_writecmd  |= SD_PP_WRITECB;
        }
      }
      /* default flags when parameter 'flags' for SDCONN_write() is not set (== 0) */
      dd->sdcd->io_flags_default = dd->sdcd->io_flags_writedata;
    }
    i++;
  }

  
  /* calculate active-low/active-high clashes and adapt signals_invert according to these */

  dd->sdcd->signals_invert = 0L;

  for (i = 0; i < SD_MAX_SUPP_SIGNALS; i++) {
    if (dd->sdcd->signals[i]) {
      int activelow = SDCONN_isactivelow(dd->sdcd->signals[i], dd->sdcd->conntype, dd->sdcd->hardwaretype);
      int found;
      int j;
      /* signal == active-low, */
      
      j = 0;
      found = 0;
      while(!found && j < dd->amountwiresignals) {
        if (dd->wiresignals[j].index == i) {
          found = 1;
        } else {
          j++;
        }
      }
      if (found && activelow != dd->wiresignals[j].activelow) {
        dd->sdcd->signals_invert ^= dd->sdcd->signals[i];
      }
    }
  }
  return 0;
}



/* *********************************
   int serdisp_setupoptions(dd, dispname, optionstring)
   *********************************
   set up options
   *********************************
   dd                  ... display descriptor
   dispname            ... display name
   optionstring        ... string with options set by user / outside
   *********************************
   returns 0 if everything was ok or 1 if an error ocurred
*/
int serdisp_setupoptions(serdisp_t* dd, const char* dispname, const char* optionstring) {
  int optloop;
  char* optionptr;
  int optionlen;
  int optionborder;
  int dispidx = serdisp_getdispindex(dispname);

  char buffer[50];  /* needed for error messages */

  if (dispidx < 0) {   
    sd_error(SERDISP_EINVAL, "serdisp_setupoptions(): coding error: display name '%s' unknown", dispname);
    return 1;
  }

  /* loop twice: optloop #0: setup using default options (defaultoptstring),
                 optloop #1: setup using option string (optionstring) 
  */
  for (optloop = 0; optloop <= 1; optloop++) {
    optionptr = (char*) ((optloop == 0) ?  serdisp_displays[dispidx].defaultoptions : optionstring);
    optionlen = -1;
    optionborder = strlen(optionptr);

    /* ignore empty options string */
    if (optionborder < 1) 
      continue;

    /* parse option string */
    while( (optionptr = sdtools_nextpattern(optionptr, ';', &optionlen, &optionborder)) ) {
      char* valueptr = 0;
      int valuelen = 0;
      char* idxpos = index(optionptr, '=');
      int keylen = optionlen;

      int stdoptidx;   /* index of an option found in standard options */
      int foundstdopt;

      int optidx;      /* index of an option found in driver options array */
      int foundopt;

      char* defines;   /* string with defines (aliases)    eg:  NONE=0,OFF=0 */

      char* optvalueptr;
      int optvalueptrlen;

      int usestdopt;

      /* split key=value */

      /* '=' found and position not outside optionlen? */
      if (idxpos &&  serdisp_ptrstrlen(idxpos, optionptr) < optionlen ) {
        keylen = serdisp_ptrstrlen(idxpos, optionptr);
        valueptr = ++idxpos;
        valuelen = optionlen - keylen - 1;
      } else {  /* invalid option */
        snprintf(buffer, ( (keylen >= 49) ? 50 : keylen+1), "%s", optionptr);
        sd_error(SERDISP_EINVAL, "error in option string: no value given for option %s", buffer );
        return 1;
      }

      optvalueptr = valueptr;
      optvalueptrlen = valuelen;

      /* skip wiring definitions (handled elsewhere) */
      if ( strncasecmp(optionptr, "WIRING=", strlen("WIRING=")) == 0 ||
           strncasecmp(optionptr, "WIRE=", strlen("WIRE=")) == 0
         ) {
        continue;
      }

      /* look if option is found in standard options array */
      stdoptidx = serdisp_getstandardoptionindex(optionptr);
      foundstdopt = (stdoptidx >= 0) ? 1 : 0;

      /* look if option is found in driver options array */
      optidx = serdisp_getoptionindex(dd, optionptr);
      foundopt = (optidx >= 0) ? 1 : 0;

      /* is option a driver independend standard option? */
      usestdopt = (foundstdopt && (serdisp_standardoptions[stdoptidx].flag & SD_OPTIONFLAG_STD));

      if (foundopt || usestdopt) {
        long optvalue;
        char* tempptr;
        serdisp_options_t curroption = (usestdopt) ? serdisp_standardoptions[stdoptidx] : dd->options[optidx];

        defines = curroption.defines;

        /* driver depend option but no defines: look if corresponding standard option has defines */
        if ( (!usestdopt) && foundstdopt && (strlen(defines) < 1))
          defines = serdisp_standardoptions[stdoptidx].defines;

        /* look if optvalue is a define (eg: NONE=0) */
        if (strlen(defines) > 0) {
          char* defineptr = defines;
          int definelen = -1;
          int defineborder = strlen(defineptr);
          int definefound = 0;

          while( !definefound && (defineptr = sdtools_nextpattern(defineptr, ',', &definelen, &defineborder)) ) {
            char* defineidxpos = index(defineptr, '=');
            int definekeylen = definelen;
            char* definevalueptr = 0;
            int definevaluelen = 0;

            /* '=' found and position not outside patternlen? */
            if (defineidxpos &&  serdisp_ptrstrlen(defineidxpos, defineptr) < definelen ) {
              definekeylen = serdisp_ptrstrlen(defineidxpos, defineptr);
              definevalueptr = ++defineidxpos;
              definevaluelen = definelen - definekeylen - 1;
            } else {  /* invalid define. should NEVER EVER occur (else: error in source code) */
              snprintf(buffer, ( (definekeylen >= 49) ? 50 : definekeylen+1), "%s", defineptr);
              sd_error(SERDISP_EINVAL, "coding error in define string: no value given for define %s", buffer );
              return 1;
            }

            if (sdtools_ismatching(valueptr, valuelen, defineptr, definekeylen) ) {
              definefound = 1;
              optvalueptr = definevalueptr;
              optvalueptrlen = definevaluelen;      
            }

          }
        }

        /* accept base 10 and base 16 values (base 16 if value starts with 0x or 0X) */
        if (strncasecmp(optvalueptr, "0x", 2) == 0) {
          optvalue = strtol(optvalueptr, &tempptr, 16);
        } else {
          optvalue = strtol(optvalueptr, &tempptr, 10);
        }

        /* verify if optvalueptr contained a valid number */
        if (optvalueptr == tempptr || ( (*tempptr != '\0') && (tempptr < (optvalueptr + optvalueptrlen)) ) ) {
          snprintf(buffer, ( (optvalueptrlen >= 49) ? 50 : optvalueptrlen+1), "%s", optvalueptr);
          sd_error(SERDISP_EINVAL, "invalid option %s", buffer );
          return 1;
        }

        /* check minval, maxval, modulo */
        if ( (curroption.minval != -1L && optvalue < curroption.minval) ||
             (curroption.maxval != -1L && optvalue > curroption.maxval) ||
             (curroption.modulo >= 1L && (optvalue % curroption.modulo != 0) && (optvalue != curroption.minval))
            ) {
          snprintf(buffer, ( (optvalueptrlen >= 49) ? 50 : optvalueptrlen+1), "%s", optvalueptr);
          sd_error(SERDISP_EINVAL, "option %s breaks mininum, maximum, or modulo rules", buffer );
          return 1;
        }

        /* assign option value to dd-structure variable */
        if (foundstdopt) {
          /* this should be solved in a better way ... */
          if (strcasecmp(curroption.name, "ROTATE") == 0) {
            dd->curr_rotate = sdtools_rotate_deg2intern(dd, (int)optvalue);
          } else if (strcasecmp(curroption.name, "INVERT") == 0) {
            dd->curr_invert = (int)optvalue;
          } else if (strcasecmp(curroption.name, "CONTRAST") == 0) {
            dd->curr_contrast = sdtools_contrast_norm2hw(dd, (int)optvalue);
          } else if (strcasecmp(curroption.name, "BRIGHTNESS") == 0) {
            dd->curr_dimming = (100 - (int)optvalue);
          } else if (strcasecmp(curroption.name, "BACKLIGHT") == 0) {
            dd->curr_backlight = (int)optvalue;
          } else if (strcasecmp(curroption.name, "WIDTH") == 0) {
            dd->width = (int)optvalue;
          } else if (strcasecmp(curroption.name, "HEIGHT") == 0) {
            dd->height = (int)optvalue;
          } else if (strcasecmp(curroption.name, "DEPTH") == 0) {
            dd->depth = (int)optvalue;
          } else if (strcasecmp(curroption.name, "DELAY") == 0) {
            dd->delay = optvalue;
          } else if (strcasecmp(curroption.name, "DSPAREAWIDTH") == 0) {
            dd->dsparea_width = optvalue;
          } else if (strcasecmp(curroption.name, "DSPAREAHEIGHT") == 0) {
            dd->dsparea_height = optvalue;
          } else if (strcasecmp(curroption.name, "SELFEMITTING") == 0) {
            if (optvalue)
              dd->colour_spaces |= SD_CS_SELFEMITTING;
            else
              dd->colour_spaces &= (0xFFFFFFFFL ^ SD_CS_SELFEMITTING);
          }
        } else {  /* non standard option */
          if (dd->fp_getvalueptr) {
            int typesize;
            void* specific_valptr;

            specific_valptr = dd->fp_getvalueptr(dd, curroption.name, &typesize);
            if (!specific_valptr) { /* should NEVER EVER occur (else: error in source code) */
              sd_error(SERDISP_EINVAL, "coding error. specific value %s unknown to dd->fp_getvalueptr()", curroption.name);
              return 1;
            }

            switch (typesize) {
              case 1:  *((byte*) specific_valptr) = (byte) optvalue; break;
              case 2:  *((short*) specific_valptr) = (short) optvalue; break;
              case 4:  *((long*) specific_valptr) = (long) optvalue; break;
            }

          } else {  /* spec value, but no function pointer: should NEVER EVER occur (else: error in source code) */
            sd_error(SERDISP_EINVAL, "coding error. no function pointer given for dd->fp_getvalueptr()");
            return 1;
          }
        }

      } else {
        snprintf(buffer, ( (optionlen >= 49) ? 50 : optionlen+1), "%s", optionptr);
        sd_debug(0, "*** WARNING: option %s unsupported by this driver", buffer );
      }
    }
  }

  return 0;
}




/* *********************************
   void serdisp_freeresources(dd)
   *********************************
   frees all resources allocated by serdisplib-instance
   *********************************
   dd     ... display descriptor
   *********************************
*/
void serdisp_freeresources(serdisp_t* dd) {
  if (dd->scrbuf)
    free(dd->scrbuf);
  if (dd->scrbuf_chg)
    free(dd->scrbuf_chg);
  if(dd->specific_data)
    free(dd->specific_data);
  if(dd->xreloctab)
    free(dd->xreloctab);
  if(dd->yreloctab)
    free(dd->yreloctab);
  if(dd->ctable)
    free(dd->ctable);
  free(dd);
  dd = 0;
}
