/*
 *************************************************************************
 *
 * sdcmegtron_tool.c
 * tool for SDC Megtron display modules
 *
 *************************************************************************
 *
 * copyright (C) 2009-2010  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
 *
 *************************************************************************
 *
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

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/time.h>

#if defined(__linux__)
#include <getopt.h>
#endif

#include "common.h"
#include "serdisplib/serdisp.h"
#include "serdisplib/serdisp_connect_usb.h"

#include "serdisplib/serdisp_fctptr.h"

/* uint16_t, uint32_t, ... */
#include <inttypes.h>

/* version information */
#define SDCMEGTRONTOOL_VERSION_MAJOR    1
#define SDCMEGTRONTOOL_VERSION_MINOR    0

/* default device */
#define SDCMEGTRON_DEFAULTDEVICE        "USB:152a/8380"

#define USB_LCDAPI_REQUEST        0xA0
#define USB_LCDCMD_READ_GFX       0x1D /* get the current GFX screen (3840 byte) */ 
#define USB_LCDCMD_STOREBOOTLOGO  0x2B /* store/internaly transfer the current screen as startup image to flash */ 
#define USB_LCDCMD_DRAWBOOTLOGO   0x2C /* draw stored image from flash to screen */ 
#define USB_LCDCMD_READBOOTLOGO   0x2D /* read startup image data from flash (3840 byte) */
#define USB_LCDCMD_RESET          0x10 /* reset module (also USB!) */


/* usage, help, ...
   iserr: -1 ... print version information only
           0 ... print help/usage to stdout
           1 ... print error message + help/usage to stderr
*/
void print_usage(int iserr) {
  FILE* f = (iserr > 0) ? stderr : stdout;

  fprintf(f, "  sdcmegtron_tool version %d.%d   (using serdisplib version %d.%d)\n", 
    SDCMEGTRONTOOL_VERSION_MAJOR, SDCMEGTRONTOOL_VERSION_MINOR,
    SERDISP_VERSION_GET_MAJOR(serdisp_getversioncode()), 
    SERDISP_VERSION_GET_MINOR(serdisp_getversioncode())
  );
  fprintf(f, "  (C) 2009-2010 by Wolfgang Astleitner\n");

  fprintf(f, "\n");

  if (iserr == -1) return;

  fprintf(f, "Usage: sdcmegtron_tool [<options>]\n\n");
  fprintf(f, "  Options: (default values in squared brackets)\n");
  fprintf(f, "    -p dev              output device (if not given: %s)\n", SDCMEGTRON_DEFAULTDEVICE);
  fprintf(f, "    -b brightness [255] brightness of background light, 0: turn off,  value=[0, 255]\n");
  fprintf(f, "    -f imagefile        change bootlogo (only uncompressed BMP with depth=1 and 240x128 are accepted)\n");
  fprintf(f, "    -l imagefile        save current bootlogo\n");
  fprintf(f, "    -s imagefile        save screenshot of current display content\n");
  fprintf(f, "    -y                  overwrite existing files (valid for options -l and -s)\n");
  fprintf(f, "    -r                  reset the display module\n");
  fprintf(f, "    -v                  verbose (-v repeated: <= 2: log to syslog, >= 3: log to stderr, >= 5: log to stdout)\n");
  fprintf(f, "    -V                  version information\n");
  fprintf(f, "            \n");
  fprintf(f, "  Examples: \n");
  fprintf(f, "    sdcmegtron_tool                          # show bootlogo using default device\n");
  fprintf(f, "    sdcmegtron_tool -p \"%s\"       # show bootlogo\n", SDCMEGTRON_DEFAULTDEVICE);
  fprintf(f, "    sdcmegtron_tool -b 50                    # show bootlogo and set background light to 50\n");
  fprintf(f, "    sdcmegtron_tool -f bootlogo.bmp          # change bootlogo\n");
  fprintf(f, "    sdcmegtron_tool -l bootlogo.bmp          # save current bootlogo, error if file already exists\n");
  fprintf(f, "    sdcmegtron_tool -y -l bootlogo.bmp       # save current bootlogo, overwrite existing file\n");
  fprintf(f, "    sdcmegtron_tool -s screenshot.bmp        # save screenshot, error if file already exists\n");
  fprintf(f, "    sdcmegtron_tool -y -s screenshot.bmp     # save screenshot, overwrite existing file\n");
  fprintf(f, "            \n");
}



int main(int argc, char **argv) {
  int ch;
  char sdcdev[51] = "";

  char* imagefile = (char*)0;

  char optionstring[101] = "";

  int flag_setbootlogo = 0;
  int flag_showbootlogo = 0;
  int flag_getbootlogo = 0;
  int flag_getscreenshot = 0;
  int flag_resetmodule = 0;
  int flag_overwrite = 0;

  int bglevel = 255;

  int tmp_debuglevel = 0;

  extern char *optarg;
  extern int optind;
  char* optstring =  "p:b:f:l:s:yrvVh";

  byte imgbuffer[(240/8)*128];

  serdisp_CONN_t*           sdcd;
  serdisp_t*                dd = 0;
  serdisp_usbdev_t*         usbitems = 0;

  sd_setdebuglevel(SD_LVL_WARN);

  while ((ch = getopt(argc, argv, optstring)) != -1) {
    switch(ch) {
      case 'h':
        print_usage(0);
        exit(0);
      case 'p':
        sdtools_strncpy(sdcdev, optarg, 50);
      break;
      case 'b': 
        {
          char* tempptr;
          int isvalidnumber = 0;

          if (strncasecmp(optarg, "0x", 2) == 0) {
            bglevel = (int)strtol(optarg, &tempptr, 16);
          } else {
            bglevel = (int)strtol(optarg, &tempptr, 10);
          }

          /* verify if optvalueptr contained a valid number */
          isvalidnumber = ( (optarg == tempptr) || ( (*tempptr != '\0')  ) ) ? 0 : 1;

          if (!isvalidnumber || bglevel < 0 || bglevel > 255) {
            fprintf(stderr, "Error: Invalid value for option -b: %s\n", optarg);
            exit (1);
          }
        }
      break;
      case 'f':
        {
          imagefile = (char*)sdtools_malloc(strlen(optarg)+2);
          sdtools_strncpy(imagefile, optarg, strlen(optarg));
          flag_setbootlogo = 1;
        }
      break;
      case 'l':
      case 's':
        {
          if (imagefile) {
            free(imagefile);
            fprintf(stderr, "Error: Options -f, -l, and -s may not be combined\n");
            exit (1);
          }

          imagefile = (char*)sdtools_malloc(strlen(optarg)+2);
          sdtools_strncpy(imagefile, optarg, strlen(optarg));
          if (ch == 'l')
            flag_getbootlogo = 1;
          else
            flag_getscreenshot = 1;
        }
      break;
      case 'y':
        flag_overwrite = 1;
      break;
      case 'r':
        flag_resetmodule = 1;
      break;
      case 'v':
        tmp_debuglevel++;
      break;
      case 'V':
        print_usage(-1);
        exit(0);
      break;
    }
  }


  if (tmp_debuglevel) {
    if (tmp_debuglevel > 6) tmp_debuglevel = 6;
    sd_setlogmedium( (tmp_debuglevel <= 2) ? SD_LOG_SYSLOG : ((tmp_debuglevel <= 4) ? SD_LOG_STDERR : SD_LOG_STDOUT));
    if (tmp_debuglevel > 2) sd_setdebuglevel( (tmp_debuglevel % 2) ? 1 : 2);
    else sd_setdebuglevel(tmp_debuglevel);
  }

  if (strlen(sdcdev) == 0) {
    sdtools_strncpy(sdcdev, SDCMEGTRON_DEFAULTDEVICE, 50);
  }

  if (flag_resetmodule && (flag_setbootlogo || flag_getbootlogo || flag_getscreenshot)) {
    fprintf(stderr, "Error: Option -r (reset) may not be combined with bootlogo or screenshot operations\n");
    if (imagefile) free(imagefile);
    exit (1);
  }

  /* if no flag given: show bootlogo */
  if (! (flag_setbootlogo || flag_setbootlogo || flag_getbootlogo || flag_getscreenshot) ) {
    flag_showbootlogo = 1;
  }

  sdcd = SDCONN_open(sdcdev);

  if (sdcd == (serdisp_CONN_t*)0) {
    fprintf(stderr, "Error: Unable to open %s, additional info: %s\n", sdcdev, sd_geterrormsg());
    exit (1);
  }

  if ( ! SDFCTPTR_checkavail(SDFCTPTR_LIBUSB) ) {
    sd_error(SERDISP_ERUNTIME, "%s(): libusb is not loaded.", __func__);
    exit (1);
  }

  snprintf(optionstring, 99, "BGLEVEL=%d",bglevel);
  sd_debug(1, "optionstring: %s", optionstring);


  dd = serdisp_init(sdcd, "SDCMEGTRON", optionstring);

  if (!dd) {
    fprintf(stderr, "Error: Unable to open SDCMEGTRON, additional info: %s\n", sd_geterrormsg());
    exit(1);
  }

  usbitems = (serdisp_usbdev_t *)(dd->sdcd->extra);

  if (flag_setbootlogo) {
    int rc = common_read_simplebmp(imagefile, imgbuffer, 240, 128, 1);
    if (rc != 0) {
      fprintf(stderr, "Error: %s\n", sd_geterrormsg());
      free(imagefile);
      exit (1);
    }

    memcpy(dd->scrbuf, imgbuffer, sizeof(imgbuffer));
    serdisp_rewrite(dd);

    if(fp_usb_control_msg(usbitems->usb_dev, USB_TYPE_VENDOR, USB_LCDAPI_REQUEST, 1,
                                             USB_LCDCMD_STOREBOOTLOGO, NULL, 0, usbitems->write_timeout) < 0) {
        sd_error(SERDISP_ERUNTIME, "%s(): request 'USB_LCDCMD_STOREBOOTLOGO' failed", __func__);
        free(imagefile);
        return -1;
    }
  }

  if (flag_resetmodule) {
    if(fp_usb_control_msg(usbitems->usb_dev, USB_TYPE_VENDOR, USB_LCDAPI_REQUEST, 1,
                                             USB_LCDCMD_RESET, NULL, 0, usbitems->write_timeout) < 0) {
        sd_error(SERDISP_ERUNTIME, "%s(): request 'USB_LCDCMD_RESET' failed", __func__);
        free(imagefile);
        return -1;
    }
    free(imagefile);
    serdisp_close(dd);
    return 0;
  }


  if (flag_getbootlogo || flag_getscreenshot) {
    int rc;
    int usb_cmd = (flag_getbootlogo) ? USB_LCDCMD_READBOOTLOGO : USB_LCDCMD_READ_GFX;
    if(fp_usb_control_msg(usbitems->usb_dev, USB_TYPE_VENDOR|USB_ENDPOINT_IN, USB_LCDAPI_REQUEST, 0,
                                             usb_cmd, (char*)imgbuffer, sizeof(imgbuffer) , usbitems->read_timeout) < 0) {
        sd_error(SERDISP_ERUNTIME, "%s(): request '%s' failed",
                 __func__, ((flag_getbootlogo) ? "USB_LCDCMD_READBOOTLOGO" : "USB_LCDCMD_READ_GFX")
        );
        free(imagefile);
        return -1;
    }
    rc = common_write_simplebmp (imagefile, imgbuffer, 240, 128, 1, flag_overwrite);
    if (rc != 0) {
      fprintf(stderr, "Error: %s\n", sd_geterrormsg());
      free(imagefile);
      return -1;
    }
  }

  if (flag_setbootlogo || flag_showbootlogo || flag_getbootlogo || flag_getscreenshot) {
    if(fp_usb_control_msg(usbitems->usb_dev, USB_TYPE_VENDOR, USB_LCDAPI_REQUEST, 1,
                                             USB_LCDCMD_DRAWBOOTLOGO, NULL, 0, usbitems->write_timeout) < 0) {
        sd_error(SERDISP_ERUNTIME, "%s(): request 'USB_LCDCMD_DRAWBOOTLOGO' failed", __func__);
        free(imagefile);
        return -1;
    }
    serdisp_close(dd);
  } else {
    serdisp_quit(dd);
  }
  return 0;
}

