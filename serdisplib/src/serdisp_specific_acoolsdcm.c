/*
 *************************************************************************
 *
 * serdisp_specific_acoolsdcm.c
 * routines for controlling
 *   Alphacool 200x64 and 240x128 USB-displays
 *   SDC-Megtron 240x128 USB-display
 *
 *************************************************************************
 *
 * copyright (C) 2007       fen <fen@init-6.org>
 * copyright (C) 2007       nessie <nessie10@gmx.net> (original code)
 *
 * adaptation for SDC-Megtron 240x128 LCD
 * copyright (C) 2009       smart-display-company
 * email     info@smart-display-company.de
 *
 * additional maintenance:
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

/*
 * Display organisation 200x64:
 *
 * Display is 200 by 64 pixel. The display transfer buffer uses the
 * following layout:
 *
 * data[0]    column 0,   rows 0 to 7,  msb is row 0, lsb is row 7
 * data[1]    column 1,   rows 0 to 7,  msb is row 0, lsb is row 7
 * ...
 * data[199]  column 199, rows 0 to 7,  msb is row 0, lsb is row 7
 * data[200]  column 0,   rows 8 to 15, msb is row 8, lsb is row 15
 * data[201]  column 1,   rows 8 to 15, msb is row 8, lsb is row 15
 * ...
 * data[399]  column 199, rows 8 to 15, msb is row 8, lsb is row 15
 * ...
 *
 * every byte represents 8 rows of a column. the buffer is 
 * 200 * 64 / 8 = 1600 bytes.
 *
 *
 * Display organisation 240x128:
 *
 * Display is 240 by 128 pixel. The display transfer buffer uses the
 * following layout:
 *
 * data[0]    column 0 to 7,     row 0, msb is column 0,   lsb is column 7
 * data[1]    column 8 to 15,    row 0, msb is column 8,   lsb is column 15
 * ...
 * data[239]  column 232 to 239, row 0, msb is column 232, lsb is column 239
 * data[240]  column 0 to 7,     row 1, msb is column 0,   lsb is column 7
 * data[241]  column 8 to 15,    row 1, msb is column 8,   lsb is column 15
 * ...
 *
 * every byte represents 8 columns of a row. the buffer is
 * 240 * 128 / 8 = 3840 bytes.
 */

/*
 * includes
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../config.h"

#include "serdisplib/serdisp_connect.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_fctptr.h"
#include "serdisplib/serdisp_connect_usb.h"

#if (SERDISP_VERSION_CODE > SERDISP_VERSION(1,97))
 #include "serdisplib/serdisp_colour.h"
#endif

#ifdef HAVE_LIBPTHREAD
 #include "serdisplib/serdisp_gpevents.h"
#endif


/*
 * alphacool constants
 */

#define ALPHACOOL_INIT              0x31
#define ALPHACOOL_GFX_WRITE_INIT    0x32
#define ALPHACOOL_GFX_WRITE_END     0x33
#define ALPHACOOL_GFX_WRITE_DATA    0x34
#define ALPHACOOL_GFX_CLEAR         0x35
#define ALPHACOOL_TXT_WRITE_INIT    0x36
#define ALPHACOOL_TXT_WRITE_END     0x37
#define ALPHACOOL_TXT_WRITE_DATA    0x38
#define ALPHACOOL_TXT_CLEAR         0x39
#define ALPHACOOL_CLEAR             0x3A
#define ALPHACOOL_CS                0x3B
#define ALPHACOOL_WRITE_DATA        0x3C
#define ALPHACOOL_WRITE_AUTO_DATA   0x3D
#define ALPHACOOL_WRITE_COMMAND     0x3E
#define ALPHACOOL_WRITE_COMMAND1    0x3F
#define ALPHACOOL_WRITE_COMMAND2    0x40
#define ALPHACOOL_READ_DATA         0x41
#define ALPHACOOL_READ_AUTO_DATA    0x42
#define ALPHACOOL_BKL               0x43
#define ALPHACOOL_AUTO_WRITE_RESET  0x44
#define ALPHACOOL_AUTO_READ_RESET   0x45
#define ALPHACOOL_SET_FONT          0x46
#define ALPHACOOL_DEVICE            0x47
#define ALPHACOOL_GET_ADDRESS       0x48
#define ALPHACOOL_SET_ADDRESS       0x49


/*
 * SDC-megtron constants + in-source display driving documentation
 */

#define USB_LCDAPI_REQUEST        0xA0
#define USB_LCDAPI_GFXDRAW        0xA1

/* firmware API commands for USB_LCDAPI_REQUEST */
#define USB_LCDCMD_VERSION        0x01 /* get current firmware version (word) */
#define USB_LCDCMD_RESET          0x10 /* reset module (also USB!) */
#define USB_LCDCMD_INIT           0x11 /* reset and re-init T6963 */
#define USB_LCDCMD_CLRSCR         0x12 /* clear screen (see "clrscr option" below) */
#define USB_LCDCMD_STORELIGHT     0x13 /* store light value to EE as default (word, 0-255) */
#define USB_LCDCMD_GETSTOREDLIGHT 0x14 /* get the stored value from EE */

#define USB_LCDCMD_SETTEXTMODE    0x15 /* module supports overlay text (8x8) - see "text modes" below */
#define USB_LCDCMD_GETTEXTMODE    0x16
#define USB_LCDCMD_SETCURSORTYPE  0x17 /* see "cursor types" below */ 
#define USB_LCDCMD_GETCURSORTYPE  0x18
#define USB_LCDCMD_SETCURSORPOS   0x19 /* Col/Row shifted to parameter word CCRR, col 0-29, row 0-15 */
#define USB_LCDCMD_WRITE_TEXT     0x1A /* writes a string at given Col,Row to TXT screen */ 
#define USB_LCDCMD_PRINT_TEXT     0x1B /* terminal style writing at current cursor pos */ 
#define USB_LCDCMD_READ_TEXT      0x1C /* get the current TXT screen (480 chars) */ 
#define USB_LCDCMD_READ_GFX       0x1D /* get the current GFX screen (3840 byte) */ 

#define USB_LCDCMD_SETLIGHT       0x20 /* set backlight (word) 0-255 */ 
#define USB_LCDCMD_GETLIGHT       0x21 /* get current backlight value */ 
#define USB_LCDCMD_SETINVERT      0x22 /* module can manage inverted screen by itself, value is also stored to EE */
#define USB_LCDCMD_GETINVERT      0x23 /* get current setting from EE */ 

#define USB_LCDCMD_SET_FLAG       0x27
#define USB_LCDCMD_RESET_FLAG     0x28
#define USB_LCDCMD_READ_FLAGS     0x29
#define USB_LCDCMD_WRITE_FLAGS    0x2A
#define USB_LCDCMD_STOREBOOTLOGO  0x2B /* store/internaly transfer the current screen as startup image to flash */ 
#define USB_LCDCMD_DRAWBOOTLOGO   0x2C /* draw stored image from flash to screen */ 
#define USB_LCDCMD_READBOOTLOGO   0x2D /* read startup image data from flash (3840 byte) */
#define USB_LCDCMD_SETFLIPPED     0x2E
                                       /* module can not really manage a flipped screen due to performance reasons.
                                          only the startup image is automatically flipped. if option is used, application
                                          should read option first and must flip content by itself (if option is set) before
                                          sending pixel data to LCD screen.
                                          "flip" means left/top is at right/bottom and each pixel byte must be mirrored
                                          e.g. 11000001 => 10000011
                                       */
#define USB_LCDCMD_GETFLIPPED     0x2F

/* firmware config Flags */
#define LCD_FLAG_BOOTLOGO_POWERUP  0x0001
#define LCD_FLAG_BOOTLOGO_RESUME   0x0002
#define LCD_FLAG_BOOTLOGO_CONFIG   0x0004
#define LCD_FLAG_SHUTDOWN_SUSPEND  0x0008
#define LCD_FLAG_SHUTDOWN_RELEASE  0x0010
#define LCD_FLAG_BOOTLOGO_RELEASE  0x0040
#define LCD_FLAG_SHUTDOWN_EFFECT   0x0080

/* text modes */
#define LCD_TEXT_MODE_OR           0x0000
#define LCD_TEXT_MODE_XOR          0x0001
#define LCD_TEXT_MODE_AND          0x0002
#define LCD_TEXT_MODE_STORE_EE     0x8000

/* clrscr option 
   text screen is overlay to GFX screen, you can delete complete screen or just one of them
*/
#define LCD_BOTH_CLRSCR            0x00   
#define LCD_GFX_CLRSCR             0x01   
#define LCD_TEXT_CLRSCR            0x02

/* cursor types */
#define LCD_CURSOR_OFF             0x0000
#define LCD_CURSOR_ON              0x0001
#define LCD_CURSOR_BLINK           0x0002
#define LCD_CURSOR_1_LINE          0x0100
#define LCD_CURSOR_2_LINE          0x0200
#define LCD_CURSOR_3_LINE          0x0300
#define LCD_CURSOR_4_LINE          0x0400
#define LCD_CURSOR_5_LINE          0x0500
#define LCD_CURSOR_6_LINE          0x0600
#define LCD_CURSOR_7_LINE          0x0700
#define LCD_CURSOR_8_LINE          0x0800
#define LCD_CURSOR_STORE_EE        0x8000 /* also store the given type to EEPROM */
/* make a firmware API command:
  
  simple command: (like USB_LCDCMD_RESET)
  fp_usb_control_msg(USBHandle, USB_TYPE_VENDOR, USB_LCDAPI_REQUEST, 0, USB_LCDCMD_XXX, NULL, 0, TIMEOUT)

  command and parameter: (like USB_LCDCMD_SETLIGHT)
  fp_usb_control_msg(USBHandle, USB_TYPE_VENDOR, USB_LCDAPI_REQUEST, wValue, USB_LCDCMD_XXX, NULL, 0, TIMEOUT)


  command and data: (like USB_LCDCMD_PRINT_TEXT)
  fp_usb_control_msg(USBHandle, USB_TYPE_VENDOR, USB_LCDAPI_REQUEST, 0, USB_LCDCMD_XXX, pStr, pStrLength, TIMEOUT)
  remarks: text without 0 termination char, max. 30 chars

  command, parameter and data: (like USB_LCDCMD_WRITE_TEXT)
  fp_usb_control_msg(USBHandle, USB_TYPE_VENDOR, USB_LCDAPI_REQUEST, wValue, USB_LCDCMD_XXX, pStr, pStrLength, TIMEOUT)
  remarks: wValue for USB_LCDCMD_WRITE_TEXT is CCRR, Col:0-29, Row:0-15

  command and read parameter (word): (like USB_LCDCMD_GETLIGHT)
  fp_usb_control_msg(USBHandle, USB_TYPE_VENDOR|USB_ENDPOINT_IN, USB_LCDAPI_REQUEST, 0, USB_LCDCMD_XXX, (char*)&wValue, 2, TIMEOUT)

  command and read data: (like USB_LCDCMD_READ_TEXT, DataSizeToRead = 480)
  fp_usb_control_msg(USBHandle, USB_TYPE_VENDOR|USB_ENDPOINT_IN, USB_LCDAPI_REQUEST, 0, USB_LCDCMD_XXX, (char*)pData, pDataSizeToRead, TIMEOUT)

  
  sending data to screen:

  start transfer:
  fp_usb_control_msg(USBHandle, USB_TYPE_VENDOR, USB_LCDAPI_GFXDRAW, LEFT_TOP, RIGHT_BOTTOM, NULL, 0, TIMEOUT)
  LEFT_TOP and RIGHT_BOTTOM: XXYY, X:0-239,Y:0-127 
  X must be multible of 8!

  and send pixel data:
  fp_usb_bulk_write(USBHandle, EP_DISPLAY_DATA_OUT, (char*)pPixelData, wDataLength, TIMEOUT)
  remarks: wDataLength MUST be multible of 128 byte! (this additional bytes are not drawn to screen)
*/



/*
 * additional constants
 */

#define AC_200x64                     0
#define AC_240x128                    1

#define AC_MD_ROW                     0
#define AC_MD_COL                     1

#define DISPID_ALPHACOOL              1
#define DISPID_SDCMEGTRON             2

/*
 * endpoint constants
 */

/*
 * macros
 */

#define check(expr)                   serdisp_acoolsdcm_runtime_check(expr, __func__, __FILE__, __LINE__)

serdisp_options_t alphacool_options[] = {
   /*  name       aliasnames min  max mod int defines  */
   {  "BACKLIGHT", "",         0,   1,  1, 1,  "ON=1,OFF=0,YES=1,NO=0"}  /* backlight on/off */
  ,{  "OPTALGO", "",           0,   1,  1, 1,  "ON=1,OFF=0,YES=1,NO=0"}  /* optimized algo */
  ,{  "BRIGHTNESS", "",        0, 100,  1, 1,  ""}      /* brightness, 0: bg-light off, <>0: dd->curr_backlight */
};

serdisp_options_t sdcmegtron_options[] = {
   /*  name       aliasnames min  max mod int defines  */
   {  "BACKLIGHT", "",         0,   1,  1, 1,  "ON=1,OFF=0,YES=1,NO=0"}  /* backlight on/off */
  ,{  "OPTALGO", "",           0,   1,  1, 1,  "ON=1,OFF=0,YES=1,NO=0"}  /* optimized algo */
  ,{  "BACKLIGHTLEVEL", "BGLEVEL", 0, 0xFF,  1, 1,  ""}                  /* backlight level */
  ,{  "BRIGHTNESS", "",        0, 100,  1, 1,  ""}                       /* brightness [0 .. 100] */
};

/* 
 * private function prototypes
 */

static void   serdisp_acoolsdcm_init                (serdisp_t *dd);
static void   serdisp_acoolsdcm_update              (serdisp_t *dd);
static int    serdisp_acoolsdcm_setoption           (serdisp_t *dd, const char *option, long value);
static void   serdisp_acoolsdcm_close               (serdisp_t *dd);
static void   serdisp_acoolsdcm_clear               (serdisp_t *dd);

#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
static void   serdisp_acoolsdcm_setpixel            (serdisp_t *dd, int x, int y, SDCol sdcol);
static SDCol  serdisp_acoolsdcm_getpixel            (serdisp_t *dd, int x, int y);
#else
static void   serdisp_acoolsdcm_setpixel            (serdisp_t *dd, int x, int y, long colour);
static long   serdisp_acoolsdcm_getpixel            (serdisp_t *dd, int x, int y);
#endif
static void*  serdisp_acoolsdcm_getvalueptr         (serdisp_t *dd, const char *optionname, int *typesize);
static void   serdisp_acoolsdcm_getrect             (serdisp_t *dd, int *x1, int *y1, int *x2, int *y2, int *is_empty);
static void   serdisp_acoolsdcm_transfer_rect       (serdisp_t *dd, int x1, int y1, int x2, int y2);
static int    serdisp_acoolsdcm_runtime_check       (int condition, const char *function, const char *file, const int line);

static int    sdcmegtron_bglevel2brightness         (int bglevel);
static int    sdcmegtron_brightness2bglevel         (int brightness);

#ifdef HAVE_LIBPTHREAD
static int    updating_wait                         (int* upd_flag);
static void   updating_post                         (int* upd_flag);
#endif
/*
 * private structures
 */

typedef struct serdisp_acoolsdcm_data_s
{
  byte *trans_scrbuf;     /* transfer buffer */
  int  optalgo;           /* use optimized algo */
  byte type;              /* display type */
  int  mode;              /* buffer mode */
  byte bglevel;           /* backlight level (sdcmegtron only) */
#ifdef HAVE_LIBPTHREAD
  int  updating;          /* flag: display is currently being updated */
#endif
} serdisp_acoolsdcm_data_t;

/* *********************************
   serdisp_t* serdisp_acoolsdcm_setup(dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... port descriptor
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_acoolsdcm_setup(const serdisp_CONN_t *sdcd, const char *dispname, const char *optionstring) {
  serdisp_t*                dd = 0;
  serdisp_acoolsdcm_data_t* data = 0;
  serdisp_usbdev_t*         usbitems = 0;

  byte                      type = 0;

  sd_debug(2, ">>> %s(sdcd=%p, dispname='%s', optionstring='%s')", __func__, sdcd, dispname, optionstring);

  if(check(dispname == 0 || optionstring == 0)) { return (serdisp_t *)0; }

  if ( ! SDFCTPTR_checkavail(SDFCTPTR_LIBUSB) ) {
    sd_error(SERDISP_ERUNTIME, "%s(): libusb is not loaded but is a requirement for serdisp_specific_acoolsdcm.c.", __func__);
    return (serdisp_t*)0;
  }

  if(sdcd) {
    /* maybe null in case of wirequery */

    usbitems = (serdisp_usbdev_t*)(sdcd->extra);
  }

  /* allocate descriptor */

  if(!(dd = (serdisp_t *)sdtools_malloc(sizeof(serdisp_t)))) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate display descriptor", __func__);
    return (serdisp_t *)0;
  }

  memset(dd, 0, sizeof(serdisp_t));

  /* allocate private data */

  if(!(data = sdtools_malloc(sizeof(serdisp_acoolsdcm_data_t)))) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate private data", __func__);
    free(dd);
    dd = 0;
    return (serdisp_t *)0;
  }

  memset(data, 0, sizeof(serdisp_acoolsdcm_data_t));

  dd->specific_data = data;

  /* assign dd->dsp_id */

  if(serdisp_comparedispnames("ALPHACOOL", dispname)) {
    dd->dsp_id = DISPID_ALPHACOOL;
  } else if(serdisp_comparedispnames("SDCMEGTRON", dispname)) {
    dd->dsp_id = DISPID_SDCMEGTRON;
  } else {
    sd_error(SERDISP_ENOTSUP, "%s(): display type '%s' not supported", __func__, dispname);
    free(dd->specific_data);
    free(dd);
    dd = 0;
    return (serdisp_t *)0;
  }

  /* detect display type */

  if(usbitems) {
    if (dd->dsp_id == DISPID_ALPHACOOL) {
      if(fp_usb_control_msg(usbitems->usb_dev, USB_TYPE_VENDOR|USB_ENDPOINT_IN, 
                                               ALPHACOOL_DEVICE, 0, 0, (char *)&type, 1,
                                               usbitems->write_timeout) == 1) {
        if(type == AC_200x64) {
          dd->width       = 200;        /* display width */
          dd->height      = 64;         /* display height */
          data->optalgo   = 1;          /* optimizer on */
          data->type      = AC_200x64;  /* display type */
          data->mode      = AC_MD_COL;  /* buffer organization */
        } else if(type == AC_240x128) {
          dd->width       = 240;        /* display width */
          dd->height      = 128;        /* display height */
          data->optalgo   = 1;          /* optimizer on */
          data->type      = AC_240x128; /* display type */
          data->mode      = AC_MD_ROW;  /* buffer organization */
        } else {
          sd_error(SERDISP_ENOTSUP, "%s(): display type '%d' not supported", __func__, type);
          free(dd->specific_data);
          free(dd);
          dd = 0;
          return (serdisp_t *)0;
        }

        sd_debug(1, "%s(): detected %dx%d display", __func__, dd->width, dd->height);
      } else {
        /* use default if usb device is unavailable */
        dd->width       = 240;
        dd->height      = 128;
        data->optalgo   = 1;

        sd_debug(1, "%s(): detection failed - using %dx%d as default", __func__, dd->width, dd->height);
      }
    } else {  /* DISPID_SDCMEGTRON */
      dd->width       = 240;        /* display width */
      dd->height      = 128;        /* display height */
      data->optalgo   = 1;          /* optimizer on */
      data->type      = AC_240x128; /* display type */
      data->mode      = AC_MD_ROW;  /* buffer organization */

      if(fp_usb_control_msg(usbitems->usb_dev, USB_TYPE_VENDOR|USB_ENDPOINT_IN,
                                               USB_LCDAPI_REQUEST, 0, USB_LCDCMD_GETSTOREDLIGHT, (char*)&(data->bglevel), 2,
                                               usbitems->write_timeout) == 1) {
        /* be sure we have at least something */
        if(data->bglevel < 1) data->bglevel = 0xFF;
      } else {
        sd_debug(1, "%s(): USB_LCDCMD_GETSTOREDLIGHT failed", __func__);
      }
    }
  }

  dd->depth             = 1;  /* display is monochrome */
  dd->feature_contrast  = 0;  /* no contrast support */
  dd->feature_invert    = 1;  /* can invert */
  dd->curr_invert       = 0;  /* not yet inverted */
  dd->curr_rotate       = 0;  /* not yet rotated */
  dd->feature_backlight = 1;  /* has backlight */
  dd->curr_backlight    = 1;  /* backlight is on */
  dd->connection_types  = SERDISPCONNTYPE_PARPORT;
  dd->fp_init           = &serdisp_acoolsdcm_init;
  dd->fp_update         = &serdisp_acoolsdcm_update;
  dd->fp_setoption      = &serdisp_acoolsdcm_setoption;
  dd->fp_close          = &serdisp_acoolsdcm_close;
  dd->fp_getvalueptr    = &serdisp_acoolsdcm_getvalueptr;
  dd->fp_clear          = &serdisp_acoolsdcm_clear;

#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
  dd->fp_setsdpixel     = &serdisp_acoolsdcm_setpixel;
  dd->fp_getsdpixel     = &serdisp_acoolsdcm_getpixel;
#else
  dd->fp_setpixel       = &serdisp_acoolsdcm_setpixel;
  dd->fp_getpixel       = &serdisp_acoolsdcm_getpixel;
#endif

#if (SERDISP_VERSION_CODE > SERDISP_VERSION(1,97))
  dd->colour_spaces     = SD_CS_SCRBUFCUSTOM | SD_CS_GREYSCALE;
#endif

  if (dd->dsp_id == DISPID_ALPHACOOL) {
    serdisp_setupstructinfos(dd, 0, 0, alphacool_options);
  } else {
    serdisp_setupstructinfos(dd, 0, 0, sdcmegtron_options);
  }

#if 0
#if (SERDISP_VERSION_CODE > SERDISP_VERSION(1,97))
  /* add gpevset */
  if (! (dd->gpevset = (SDGP_gpevset_t*) sdtools_malloc( sizeof(SDGP_gpevset_t)) )) {
    sd_debug(0, 
      "serdisp_sed153x_init(): cannot allocate memory for event structures. continuing without command processor support ..."
    );
  }
#endif
#endif

  /* parse and set options */

  if(serdisp_setupoptions(dd, dispname, optionstring)) {
    free(dd->specific_data);
    free(dd);
    dd = 0;
    return (serdisp_t *)0;
  }

  /* correct bglevel if option 'BRIGHTNESS' was set */
  if (dd->dsp_id == DISPID_SDCMEGTRON && dd->curr_dimming) {
    data->bglevel = sdcmegtron_brightness2bglevel(100 - dd->curr_dimming);
  }

  sd_debug(2, "<<< %s()", __func__);
  return(dd);
}

/* *********************************
   void serdisp_acoolsdcm_init(dd)
   *********************************
   initialise an alphacool USB-display
   *********************************
   dd     ... display descriptor
*/
void serdisp_acoolsdcm_init(serdisp_t *dd) {
  serdisp_usbdev_t*          usbitems = 0;
  serdisp_acoolsdcm_data_t*  data     = 0;

  sd_debug(2, ">>> %s(dd=%p)", __func__, dd);

  if(check(dd == 0)) { return; }

  usbitems = (serdisp_usbdev_t *)(dd->sdcd->extra);
  data     = (serdisp_acoolsdcm_data_t *)(dd->specific_data);

  if(check(usbitems == 0 || data == 0)) { return; }


  if (dd->dsp_id == DISPID_ALPHACOOL) {
    /* init display */
    if(fp_usb_control_msg(usbitems->usb_dev, USB_TYPE_VENDOR, ALPHACOOL_INIT, 0,
                                             0, NULL, 0, usbitems->write_timeout) < 0) {
      sd_error(SERDISP_ERUNTIME, "%s(): request 'ALPHACOOL_INIT' failed", __func__);
    }
  }

#if 0
  /* clear display */

  if(fp_usb_control_msg(usbitems->usb_dev, USB_TYPE_VENDOR, ALPHACOOL_CLEAR, 0,
                                           0, NULL, 0, usbitems->write_timeout) < 0) {
    sd_error(SERDISP_ERUNTIME, "%s(): request 'ALPHACOOL_CLEAR' failed", __func__);
  }
#endif

  /* set backlight */

  serdisp_acoolsdcm_setoption(dd, "BACKLIGHT", dd->curr_backlight);

  /* alloc transferbuffer */

  if(data->trans_scrbuf) {
   /* free previously allocated buffer (if any) */

   free(data->trans_scrbuf);
   data->trans_scrbuf = 0;
  }

  if(!(data->trans_scrbuf = (byte *)sdtools_malloc(sizeof(byte) * dd->scrbuf_size))) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate translation screenbuffer", __func__);
    return;
  }

  memset(data->trans_scrbuf, 0, dd->scrbuf_size);

  sd_debug(2, "<<< %s()", __func__);
  return;
}

/* *********************************
   void serdisp_acoolsdcm_update(dd)
   *********************************
   updates the display
   *********************************
   dd     ... display descriptor
   *********************************
*/
void serdisp_acoolsdcm_update(serdisp_t *dd) {
  int x1 = 0, y1 = 0, x2 = 0, y2 = 0; /* coords */
  int is_empty = 0;                   /* indicates empty rect */
  serdisp_acoolsdcm_data_t *data = 0;

  sd_debug(2, ">>> %s(dd=%p)", __func__, dd);

  if(check(dd == 0)) { return; }

  data = (serdisp_acoolsdcm_data_t *)(dd->specific_data);

  if(check(data == 0)) { return; }

  /* 
   * compute area to transfer - start with the whole display
   * area. values are byte boundary.
   */

  switch(data->mode) {
    case AC_MD_ROW:
      x1 = 0;
      y1 = 0;
      x2 = (dd->width / 8) - 1;
      y2 = dd->height -1;
      break;

    case AC_MD_COL:
      x1 = 0;
      y1 = 0;
      x2 = dd->width - 1;
      y2 = (dd->height / 8) - 1;
      break;

    default:
      return;
  }

  /* 
   * if optimizer has been requested by the user
   * try to make the rectangle to transfer out of
   * the display buffer as small as possible
   */

  if(data->optalgo)
  {
    sd_debug(2, "    using optimized algo");

    /* detect the smallest rectangle to transfer */

    serdisp_acoolsdcm_getrect(dd, &x1, &y1, &x2, &y2, &is_empty);

    if(!is_empty)
    {
      if(data->type == AC_200x64)
      {
        /*
         * this display type has a firmware bug. if the update region
         * is entirly in the right display half the update fails and
         * the left half will show the right half too. to avoid this
         * we make sure that at least 1 pixel of the update region
         * is in the left half of the display.
         */

        if(x1 > 99)
        {
          x1 = 99;
        }
      }

      /* transfer the rectangle to the display */

      serdisp_acoolsdcm_transfer_rect(dd, x1, y1, x2, y2);
    }
  }
  else
  {
    sd_debug(2, "    optimizer disabled");

    /* transfer the whole buffer */

    serdisp_acoolsdcm_transfer_rect(dd, x1, y1, x2, y2);
  }

  /* reset change buffer */

  memset(dd->scrbuf_chg, 0x00, dd->scrbuf_chg_size);

  sd_debug(2, "<<< %s()", __func__);
  return;
}

/* *********************************
   void serdisp_acoolsdcm_getrect(dd, x1, y1, x2, y2, is_empty)
   *********************************
   extracts the rectangle to transfer. returns
   coords of smallest rect in x1/y1/x2/y2.
   *********************************
   dd     ... display descriptor
   x1/y1  ... upper left of search area
   x2/y2  ... lower right of search area
   is_empty ... return value for empty flag

   defs for mode AC_MD_ROW (row mode):
     x = value for column (byte based)
     y = value for row

   defs for mode AC_MD_COL (col mode):
     x = value for column
     y = value for row (byte based)
   *********************************
*/
void serdisp_acoolsdcm_getrect(serdisp_t *dd, int *x1, int *y1, int *x2, int *y2, int *is_empty)
{
  serdisp_acoolsdcm_data_t* data = 0;
  int   x = 0, y = 0;
  int   x1_n = 0, y1_n = 0;
  int   x2_n = 0, y2_n = 0;
  int   bp = 0, byte_offset = 0, bit_offset = 0;
  byte  d = 0;


  sd_debug(2, ">>> %s(dd=%p, x1=%d, y1=%d, x2=%d, y2=%d)", __func__, dd, *x1, *y1, *x2, *y2);

  if(check(dd == 0 || *x1 >= *x2 || *y1 >= *y2)) { return; }

  data = (serdisp_acoolsdcm_data_t *)(dd->specific_data);

  if(check(data == 0)) { return; }

  /* init values with caller values */

  x1_n = *x2;
  y1_n = *y2;
  x2_n = *x1;
  y2_n = *y1;

  /* not found anything - consider rect as empty */

  *is_empty = 1;

  /* 
   * search smallest rect in the change buffer
   * the buffer is bit oriented - so we need to do
   * a little extra math to mask the bits out
   * of the bytes. each bit represents the 
   * corresponding byte of the display buffer. 
   * ergo we have a 30 bit wide and 128 bit height 
   * buffer which results in 480 bytes for a 
   * 240x128 pixel display. the bit is set by
   * the setpixel sub if the byte in the buffer
   * is changed. this is some kind of delta bitmap
   * to the last picture sent to the display.
   */

  for(y=*y1; y<=*y2; y++)
  {
    for(x=*x1; x<=*x2; x++)
    {
      switch(data->mode)
       {
        case AC_MD_ROW:
          bp = y * (dd->width / 8) + x;
          break;

        case AC_MD_COL:
          bp = y * dd->width + x;
          break;

        default: 
          return;
       }

      byte_offset = bp / 8;
      bit_offset  = bp % 8;

      d = dd->scrbuf_chg[byte_offset] & (0x80 >> bit_offset);

      if(d) {
        /*
         * this bit is set - set the rect
         * accordingly...
         */

        if(x<x1_n) { x1_n = x; }
        if(x>x2_n) { x2_n = x; }
        if(y<y1_n) { y1_n = y; }
        if(y>y2_n) { y2_n = y; }

        /* tell the caller that we found something */

        *is_empty = 0;
      }
    }
  }

  /* transfer coords to the caller */

  if(!*is_empty)
  {
    *x1 = x1_n;
    *y1 = y1_n;
    *x2 = x2_n;
    *y2 = y2_n;
  }

  sd_debug(2, "<<< %s()", __func__);
  return;
}

/* *********************************
   void serdisp_acoolsdcm_transfer_rect(dd, x1, y1, x2, y2)
   *********************************
   transfer a given rect to the display
   *********************************
   dd     ... display descriptor
   x1/y1  ... upper left of area
   x2/y2  ... lower right of area

   defs for mode AC_MD_ROW:
     x = value for column (byte based)
     y = value for row

   defs for mode AC_MD_COL:
     x = value for column
     y = value for row (byte based)
   *********************************
*/
void serdisp_acoolsdcm_transfer_rect(serdisp_t *dd, int x1, int y1, int x2, int y2)
{
  serdisp_usbdev_t*         usbitems = 0;
  serdisp_acoolsdcm_data_t* data = 0;

  int delta_width = 0, delta_height = 0;
  int x =0, y = 0;
  int length = 0, d = 0;

  sd_debug(2, ">>> %s(dd=%p, x1=%d, y1=%d, x2=%d, y2=%d)", __func__, dd, x1, y1, x2, y2);

  if(check(dd == 0)) { return; }

  usbitems = (serdisp_usbdev_t *)(dd->sdcd->extra);
  data     = (serdisp_acoolsdcm_data_t *)(dd->specific_data);

  if(check(usbitems == 0 || data == 0)) { return; }

  /*
   * we need to build the transfer buffer containing only the
   * bytes of the given region. this is rather simple.
   *
   * we do have a second buffer (data->trans_scrbuf) in the size of 
   * the screenbuffer which has been allocated before. so to build
   * the transferbuffer we just copy the given rectangular region
   * from the display buffer and transfer the result to the display.
   */

  delta_width  = x2 - x1 + 1;    /* width of the change region in bytes */
  delta_height = y2 - y1 + 1;    /* height of the change buffer */
  length       = 0;              /* length of the resulting transfer buffer */

  /* copy the transfer buffer */

  for(y=y1; y<=y2; y++)
  {
    for(x=x1; x<=x2; x++)
    { 
      switch(data->mode)
      {
        case AC_MD_ROW:
          d = dd->scrbuf[y * (dd->width / 8) + x];
          break;

        case AC_MD_COL:
          d = dd->scrbuf[y * dd->width + x];
          break;

        default:
          d = 0;
          break;
      }

      /* copy the data and invert if requested */

      data->trans_scrbuf[length] = (dd->curr_invert) ? (d ^ 0xFF) : (d);
      length++;
    }
  }

  if (dd->dsp_id == DISPID_SDCMEGTRON) {
    /* Atmel MCU Bug: data transfer is banked (ping/pong) and each bank must be filled completely.
     * So data length must be a multible of 128 byte
     */
    length = ((length + 127) / 128) * 128;
  }

#ifdef HAVE_LIBPTHREAD
  if (updating_wait(&data->updating) < 0)
    return;
#endif

  /*
   * now we got a transfer buffer (data->trans_scrbuf) with the width 
   * of delta_width and the height of delta_height.
   */

  if (dd->dsp_id == DISPID_ALPHACOOL) {
    /*
     * init the transfer to the diplay by sending the 
     * ALPHACOOL_GFX_WRITE_INIT command to the display.
     * this command requires the coordinates of the area
     * to change as a long (32 bit) value which contains
     * the upper left coordinate, the width in bytes and
     * the height in pixels.
     *
     * the data is organized as YYXXHHWW so that we need
     * to do a little shifting
     */

    if(fp_usb_control_msg(usbitems->usb_dev, USB_TYPE_VENDOR, ALPHACOOL_GFX_WRITE_INIT,
                          ((y1 << 8) | (x1 & 0xFF)), ((delta_height << 8) | (delta_width & 0xFF)),
                          NULL, 0, usbitems->write_timeout) < 0) {
      sd_error(SERDISP_ERUNTIME, "%s(): request 'ALPHACOOL_GFX_WRITE_INIT' failed", __func__);
      return;
    }
  } else { /* DISPID_SDCMEGTRON */
    if(fp_usb_control_msg(usbitems->usb_dev, USB_TYPE_VENDOR, USB_LCDAPI_GFXDRAW,
                          (((x1 * 8) << 8) | (y1 & 0xFF)), ((((x2 + 1) * 8) << 8) | ((y2 + 1) & 0xFF)),
                          NULL, 0, usbitems->write_timeout) < 0) {
      sd_error(SERDISP_ERUNTIME, "%s(): request 'USB_LCDAPI_GFXDRAW' failed", __func__);
      return;
    }
  }

  /*
   * do a bulk write with the buffer which should be copied
   * into the rectangle identified by the prvious command.
   */

  if(fp_usb_bulk_write(usbitems->usb_dev, usbitems->out_ep /*EP_DISPLAY_DATA_OUT*/, 
                                          (char *)(data->trans_scrbuf), length,
                                          usbitems->write_timeout) < 0)
  {
    sd_error(SERDISP_ERUNTIME, "%s(): usb_bulk_write() failed", __func__);
    return;
  }

  if (dd->dsp_id == DISPID_ALPHACOOL) {
    /*
     * reset the endpoint 
     */

    if (fp_usb_control_msg(usbitems->usb_dev, USB_TYPE_VENDOR, ALPHACOOL_GFX_WRITE_END,
                                              0, 0,
                                              NULL, 0, usbitems->write_timeout) < 0) {
      sd_error(SERDISP_ERUNTIME, "%s(): request 'ALPHACOOL_GFX_WRITE_END' failed", __func__);
      return;
    }
  }

#ifdef HAVE_LIBPTHREAD
  updating_post(&data->updating);  /* clear update tag */
#endif

  return;
}

/* *********************************
   int serdisp_acoolsdcm_setoption(dd, optionname, value)
   *********************************
   change a display option
   *********************************
   dd          ... display descriptor
   optionname  ... name of option to change
   value       ... value for option
   *********************************
*/
int serdisp_acoolsdcm_setoption(serdisp_t *dd, const char *optionname, long value) {
  serdisp_usbdev_t*          usbitems = 0;
  serdisp_acoolsdcm_data_t*  data     = 0;

  sd_debug(2, ">>> %s(dd=%p, optionname='%s', value=%ld)", __func__, dd, optionname, value);

  if(check(dd == 0 || optionname == 0)) { return 0; }

  usbitems = (serdisp_usbdev_t *)(dd->sdcd->extra);
  data     = (serdisp_acoolsdcm_data_t *)(dd->specific_data);

  if(check(usbitems == 0 || data == 0)) { return 0; }

#ifdef HAVE_LIBPTHREAD
  /* safety shortcut - ugly temp. hack */
  if (updating_wait(&data->updating) < 0)
    return 0;
#endif

  if(dd->feature_backlight && serdisp_compareoptionnames(dd, optionname, "BACKLIGHT")) {
    /* backlight */
    byte curr_bgvalue = 0;

    if(value < 2) {
      dd->curr_backlight = value;
    } else {
      dd->curr_backlight = (dd->curr_backlight) ? 0 : 1;
    }

    if (dd->dsp_id == DISPID_ALPHACOOL) {
      curr_bgvalue = (dd->curr_dimming == 100) ? 0 : ( (dd->curr_backlight) ? 1 : 0 );
      if(fp_usb_control_msg(usbitems->usb_dev, USB_TYPE_VENDOR, ALPHACOOL_BKL, curr_bgvalue,
                                               0, NULL, 0, usbitems->write_timeout) < 0) {
        sd_error(SERDISP_ERUNTIME, "%s(): request 'ALPHACOOL_BKL' failed", __func__);
      }
    } else { /* DISPID_SDCMEGTRON */
      dd->curr_dimming = 100 - sdcmegtron_bglevel2brightness(data->bglevel);

      curr_bgvalue = (dd->curr_dimming == 100 || !dd->curr_backlight) ? 0 : data->bglevel;
      if(fp_usb_control_msg(usbitems->usb_dev, USB_TYPE_VENDOR, USB_LCDAPI_REQUEST, curr_bgvalue,
                                               USB_LCDCMD_SETLIGHT, NULL, 0, usbitems->write_timeout) < 0) {
        sd_error(SERDISP_ERUNTIME, "%s(): request 'USB_LCDCMD_SETLIGHT' failed", __func__);
      }
    }
  } else if(serdisp_compareoptionnames(dd, optionname, "BACKLIGHTLEVEL") ||
            serdisp_compareoptionnames(dd, optionname, "BRIGHTNESS"))
  {
    byte curr_bgvalue = 0;

    /* DISPID_SDCMEGTRON only! */
    if (serdisp_compareoptionnames(dd, optionname, "BACKLIGHTLEVEL")) {
      data->bglevel = (value > 255 || value < 0) ? 0xFF : (byte)value;
      dd->curr_dimming = 100 - sdcmegtron_bglevel2brightness(data->bglevel);

      curr_bgvalue = (dd->curr_backlight == 0) ? 0 : data->bglevel;
    } else {  /* BRIGHTNESS */
      if (value < 0) value = 0; if (value > 100) value = 100;
      dd->curr_dimming = 100 - value;
      if (dd->dsp_id == DISPID_ALPHACOOL) {
        curr_bgvalue = (value == 0) ? 0 : dd->curr_backlight;
      } else {
        data->bglevel = sdcmegtron_brightness2bglevel(value);
        curr_bgvalue = (dd->curr_backlight == 0) ? 0 : data->bglevel;
      }
    }

    if (dd->dsp_id == DISPID_ALPHACOOL) {
      if(fp_usb_control_msg(usbitems->usb_dev, USB_TYPE_VENDOR, ALPHACOOL_BKL, curr_bgvalue,
                                               0, NULL, 0, usbitems->write_timeout) < 0) {
        sd_error(SERDISP_ERUNTIME, "%s(): request 'ALPHACOOL_BKL' failed", __func__);
      }
    } else {
      if(fp_usb_control_msg(usbitems->usb_dev, USB_TYPE_VENDOR, USB_LCDAPI_REQUEST, curr_bgvalue,
                                               USB_LCDCMD_SETLIGHT, NULL, 0, usbitems->write_timeout) < 0) {
        sd_error(SERDISP_ERUNTIME, "%s(): request 'USB_LCDCMD_SETLIGHT' failed", __func__);
      }
    }
  } else if(serdisp_compareoptionnames(dd, optionname, "OPTALGO")) {
    /* transfer optimizer */

    if(value < 2) {
      data->optalgo = value;
    } else {
      data->optalgo = (data->optalgo) ? 0 : 1;
    }
  } else {
    /* option not found here: try generic one in calling serdisp_setoption(); */

    sd_debug(1, "%s(): option unhandled - using fallback", __func__);
#ifdef HAVE_LIBPTHREAD
    updating_post(&data->updating);
#endif
    return 0;
  }

  sd_debug(2, "<<< %s()", __func__);

#ifdef HAVE_LIBPTHREAD
  updating_post(&data->updating);
#endif

  return 1;
}


/* *********************************
   void serdisp_acoolsdcm_clear(dd)
   *********************************
   clear display
   *********************************
   dd     ... display descriptor
*/
void serdisp_acoolsdcm_clear(serdisp_t* dd) {
  serdisp_usbdev_t* usbitems          = (serdisp_usbdev_t *)(dd->sdcd->extra);
#ifdef HAVE_LIBPTHREAD
  serdisp_acoolsdcm_data_t*  data     = (serdisp_acoolsdcm_data_t *)(dd->specific_data);

  if (updating_wait(&data->updating) < 0)
    return;
#endif

  if (dd->dsp_id == DISPID_ALPHACOOL) {
    if(fp_usb_control_msg(usbitems->usb_dev, USB_TYPE_VENDOR, ALPHACOOL_CLEAR, 0,
                                             0, NULL, 0, usbitems->write_timeout) < 0) {
      sd_error(SERDISP_ERUNTIME, "%s(): request 'ALPHACOOL_CLEAR' failed", __func__);
    }
  } else {
    fp_usb_control_msg(usbitems->usb_dev,USB_TYPE_VENDOR, USB_LCDAPI_REQUEST, 0, USB_LCDCMD_CLRSCR, NULL, 0, 100);
  }

#ifdef HAVE_LIBPTHREAD
  updating_post(&data->updating);
#endif
}


/* *********************************
   int serdisp_acoolsdcm_getvalueptr(dd, optionname, typesize)
   *********************************
   get a display option
   *********************************
   dd          ... display descriptor
   optionname  ... name of option to change
   typesize    ... pointer to typesize storage
*/
void *serdisp_acoolsdcm_getvalueptr(serdisp_t *dd, const char *optionname, int *typesize) {
  serdisp_acoolsdcm_data_t*     data = 0;

  sd_debug(2, ">>> %s(dd=%p, optionname='%s', typesize=%p)", __func__, dd, optionname, typesize);

  if(check(dd == 0 || optionname == 0 || typesize == 0)) { return 0; }

  data = (serdisp_acoolsdcm_data_t *)(dd->specific_data);

  if(check(data == 0)) { return 0; }

  if(serdisp_compareoptionnames(dd, optionname, "OPTALGO")) {
    *typesize = sizeof(int);
    return &(data->optalgo);
  } else if(serdisp_compareoptionnames(dd, optionname, "BACKLIGHTLEVEL")) {
    *typesize = sizeof(byte);
    return &(data->bglevel);
  }

  sd_debug(2, "<<< %s()", __func__);
  return 0;
}

/* *********************************
   void serdisp_acoolsdcm_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_acoolsdcm_close(serdisp_t *dd) {
  serdisp_acoolsdcm_data_t*    data = 0;

#if 0
  unsigned int      last_backlight = 0;
#endif
  serdisp_usbdev_t* usbitems = 0;

  sd_debug(2, ">>> %s(dd=%p)", __func__, dd);

  if(check(dd == 0)) { return; }

  usbitems = (serdisp_usbdev_t *)(dd->sdcd->extra);

  data = (serdisp_acoolsdcm_data_t *)(dd->specific_data);

#if 0
  /* set backlight off, but do not remember it */

  last_backlight = dd->curr_backlight;
  serdisp_acoolsdcm_setoption(dd, "BACKLIGHT", 0);
  dd->curr_backlight = last_backlight;
#endif

  /* clear screen */
  serdisp_clear(dd);
#ifdef HAVE_LIBPTHREAD
  if (updating_wait(&data->updating) >= 0) {
#endif
    if (dd->dsp_id == DISPID_ALPHACOOL) {
      fp_usb_control_msg(usbitems->usb_dev,USB_TYPE_VENDOR, ALPHACOOL_BKL, 0, 0, NULL, 0, 1);
      /*fp_usb_control_msg(usbitems->usb_dev,USB_TYPE_VENDOR, ALPHACOOL_CLEAR, 0, 0, NULL, 0, 1000);*/
    } else {
      fp_usb_control_msg(usbitems->usb_dev,USB_TYPE_VENDOR, USB_LCDAPI_REQUEST, 0, USB_LCDCMD_SETLIGHT, NULL, 0, 100);
      fp_usb_control_msg(usbitems->usb_dev,USB_TYPE_VENDOR, USB_LCDAPI_REQUEST, 0, USB_LCDCMD_CLRSCR, NULL, 0, 100);
    }
#ifdef HAVE_LIBPTHREAD
  }
  updating_post(&data->updating);
#endif


  /* free buffers */

  if(data) {
    if(data->trans_scrbuf) {
      free(data->trans_scrbuf);
      data->trans_scrbuf = 0;
    }

    free(data);
    dd->specific_data = 0;
  }

  sd_debug(2, "<<< %s()", __func__);
  return;
}

/* *********************************
   void serdisp_acoolsdcm_setpixel(dd, x, y, colour)
   *********************************
   changes a pixel into the display buffer
   *********************************
   dd     ... display descriptor
   x      ... x-position
   y      ... y-position
   colour ... monochrome: 0: clear (white), <>0: set (black)
*/
#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
void serdisp_acoolsdcm_setpixel(serdisp_t *dd, int x, int y, SDCol colour) {
#else
void serdisp_acoolsdcm_setpixel(serdisp_t *dd, int x, int y, long colour) {
#endif
  serdisp_acoolsdcm_data_t*    data = 0;

  unsigned int x_i = 0, y_i = 0, w_b = 0, x_b = 0, y_b = 0, pos = 0;
  byte         value_orig = 0, value = 0, mask = 0;

  if(check(dd == 0)) { return; }

  data = (serdisp_acoolsdcm_data_t *)(dd->specific_data);

  if(check(data == 0)) { return; }

  if(dd->curr_rotate <= 1) {
    if(x >= dd->width || y >= dd->height || x < 0 || y < 0)
    {
      return;
    }
  } else {
    if(x >= dd->height || y >= dd->width || x < 0 || y < 0) {
      return;
    }
  }

  switch(dd->curr_rotate) {  /* no reloctabs used by alphacool: so we don't have to consider them here */
    case 0:  /* 0 degrees */
      x_i = x;
      y_i = y;
      break;
    case 1:  /* 180 degrees */
      x_i = dd->width  - 1 - x;
      y_i = dd->height - 1 - y;
      break;
    case 2:  /* 90 degrees */
      x_i = y;
      y_i = dd->height - 1 - x;
      break;
    case 3:  /* 270 degrees */
      x_i = dd->width  - 1 - y;
      y_i = x;
      break;
  }

  /* set the bit in the buffer */

  switch(data->mode) {
    case AC_MD_ROW:
      w_b = dd->width / 8;
      x_b = x_i / 8;
      pos = y_i * w_b + x_b;

      value_orig = dd->scrbuf[pos];
      mask       = 0x80 >> (x_i % 8);
      break;

    case AC_MD_COL:
      y_b = y_i / 8;
      pos = y_b * dd->width + x_i;

      value_orig = dd->scrbuf[pos];
      mask       = 0x80 >> (y_i % 8);
      break;

    default:
      return;
  }

  value = (colour) ? (value_orig | mask) : (value_orig & (0xFF ^ mask));

  /* update the change buffer */

  if(value_orig != value) {
    dd->scrbuf[pos] = value;
    dd->scrbuf_chg[pos/8] |= 0x80 >> (pos % 8);
  }
}

/* *********************************
   SDCol serdisp_acoolsdcm_getpixel(dd, x, y)   (arch indep. colour functions)
   long  serdisp_acoolsdcm_getpixel(dd, x, y)   (arch dep. colour functions (using 'long'))
   *********************************
   changes a pixel in the display buffer. supported depths: 1
   *********************************
   dd     ... display descriptor
   x      ... x-position
   y      ... y-position
   *********************************
   returns the grey value (or hw dependend index) at (x/y)
*/
#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
SDCol serdisp_acoolsdcm_getpixel(serdisp_t *dd, int x, int y) {
#else
long  serdisp_acoolsdcm_getpixel(serdisp_t *dd, int x, int y) {
#endif
  serdisp_acoolsdcm_data_t*    data = 0;

  unsigned int x_i = 0, y_i = 0, w_b = 0, x_b = 0, y_b = 0, pos = 0;
  byte         value = 0, mask = 0;

  if(check(dd == 0)) { return 0; }

  data = (serdisp_acoolsdcm_data_t *)(dd->specific_data);

  if(check(data == 0)) { return 0; }

  if(dd->curr_rotate <= 1) {
    if(x >= dd->width || y >= dd->height || x < 0 || y < 0) {
      return 0;
    }
  } else {
    if(x >= dd->height || y >= dd->width || x < 0 || y < 0) {
      return 0;
    }
  }

  switch(dd->curr_rotate) { /* no reloctabs used by alphacool: so we don't have to consider them here */
    case 0:  /* 0 degrees */
      x_i = x;
      y_i = y;
      break;
    case 1:  /* 180 degrees */
      x_i = dd->width  - 1 - x;
      y_i = dd->height - 1 - y;
      break;
    case 2:  /* 90 degrees */
      x_i = y;
      y_i = dd->height - 1 - x;
      break;
    case 3:  /* 270 degrees */
      x_i = dd->width  - 1 - y;
      y_i = x;
      break;
  }

  /* get the corresponding bit from the buffer */

  switch(data->mode) {
    case AC_MD_ROW:
      w_b = dd->width / 8;
      x_b = x_i / 8;
      pos = y_i * w_b + x_b;

      value = dd->scrbuf[pos];
      mask = 0x80 >> (x_i % 8);
      break;

    case AC_MD_COL:
      y_b = y_i / 8;
      pos = y_b * dd->width + x_i;

      value = dd->scrbuf[pos];
      mask  = 0x80 >> (y_i % 8);
      break;

    default:
      return 0;
  }

#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
  return (SDCol)((value & mask) ? 1 : 0);
#else
  return (long)((value & mask) ? 1 : 0);
#endif
}

/* *********************************
   int serdisp_acoolsdcm_runtime_check(condition, function)
   *********************************
   check condition and sets global runtime error if appropriate
   *********************************
   condition ... evals to 'true' if there's an error
   function  ... runtime function name (defined by check macro)
   file      ... runtime file name (defined by check macro)
   line      ... runtime line number (defined by check macro)
   *********************************
   returns the condition
*/
int serdisp_acoolsdcm_runtime_check(int condition, const char *function, const char *file, const int line) {
  if(condition) {
    sd_runtimeerror = 1; 
    sd_error(SERDISP_ERUNTIME, "%s(): runtime error detected (%s, line %d)", function, file, line);
  }

  return(condition);
}


int    sdcmegtron_bglevel2brightness         (int bglevel) {
  /* bglevel: [0 - 255]
     brightness: [0 - 100]
   */
  return (bglevel * 100 ) / 255;
}

int    sdcmegtron_brightness2bglevel         (int brightness) {
  return (brightness * 255) / 100;
}



#ifdef HAVE_LIBPTHREAD
int updating_wait (int* upd_flag) {
  int max_try = 20;
  while (*upd_flag && max_try > 0) {
    usleep(1);
    max_try --;
  }
  if (*upd_flag) {
    return -1;
  }
  *upd_flag = 1;  /* grab it for our uses */
  return 0;
}

void updating_post (int* upd_flag) {
  *upd_flag = 0;
}
#endif
