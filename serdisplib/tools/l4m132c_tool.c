/*
 *************************************************************************
 *
 * l4m132c_tool.c
 * tool for linux4media l4m132c display modules
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

/* uint16_t, uint32_t, ... */
#include <inttypes.h>

/* version information */
#define L4M132CTOOL_VERSION_MAJOR    1
#define L4M132CTOOL_VERSION_MINOR    0


/* usage, help, ...
   iserr: -1 ... print version information only
           0 ... print help/usage to stdout
           1 ... print error message + help/usage to stderr
*/
void print_usage(int iserr) {
  FILE* f = (iserr > 0) ? stderr : stdout;

  fprintf(f, "  l4m132c_tool version %d.%d   (using serdisplib version %d.%d)\n", 
    L4M132CTOOL_VERSION_MAJOR, L4M132CTOOL_VERSION_MINOR,
    SERDISP_VERSION_GET_MAJOR(serdisp_getversioncode()), 
    SERDISP_VERSION_GET_MINOR(serdisp_getversioncode())
  );
  fprintf(f, "  (C) 2009-2010 by Wolfgang Astleitner\n");

  fprintf(f, "\n");

  if (iserr == -1) return;

  fprintf(f, "Usage: l4m132c_tool [<options>]\n\n");
  fprintf(f, "  Options: (default values in squared brackets)\n");
  fprintf(f, "    -p dev|port      output device or port\n");
  fprintf(f, "    -t               set date/time to local date/time\n");
  fprintf(f, "    -c value     [5] contrast / brightness; value=[0, 10]\n");
  fprintf(f, "    -a \"HH:MM\"       set alarm time\n");
  fprintf(f, "    -d \"%%6543210\"    set alarm days (bit field, to be entered as hexadecimal or decimal)\n");
  fprintf(f, "                       bit 6: saturday   bit 5: friday\n");
  fprintf(f, "                       bit 4: thursday   bit 3: wednesday\n");
  fprintf(f, "                       bit 2: tuesday    bit 1: monday\n");
  fprintf(f, "                       bit 0: sunday\n");
  fprintf(f, "                     examples:\n");
  fprintf(f, "                       -d 0x7F: %%1111111 -> all days\n");
  fprintf(f, "                       -d 0x41: %%1000001 -> saturday and sunday\n");
  fprintf(f, "                       -d 0:    de-activate alarm\n");
  fprintf(f, "    -f imagefile     change bootlogo (only uncompressed BMP with depth=1 and 128x64 are accepted)\n");
  fprintf(f, "    -v               verbose (-v repeated: <= 2: log to syslog, >= 3: log to stderr, >= 5: log to stdout)\n");
  fprintf(f, "    -V               version information\n");
  fprintf(f, "            \n");
  fprintf(f, "  Examples: \n");
  fprintf(f, "    l4m132c_tool -p \"/dev/hiddev0\"                      # show bootlogo\n");
  fprintf(f, "    l4m132c_tool -p \"/dev/hiddev0\" -t                   # set date/time\n");
  fprintf(f, "    l4m132c_tool -p \"/dev/hiddev0\" -f bootlogo.bmp      # change bootlogo\n");
  fprintf(f, "    l4m132c_tool -p \"/dev/hiddev0\" -a \"23:30\" -d 0x7F # set alarm at 23:30, all days\n");
  fprintf(f, "            \n");
}



int main(int argc, char **argv) {
  int ch;
  char sdcdev[51] = "";

  char* bootlogofile = (char*)0;

  char optionstring[101] = "";

  int flag_settime = 0;
  int flag_alarmtime = 0;
  int flag_alarmdays = 0;
  int flag_setbootlogo = 0;
  int flag_showbootlogo = 0;

  int contrast = 5;

  unsigned short alarm_days   = 0;
  unsigned char  alarm_hour   = 0;
  unsigned char  alarm_minute = 0;

  int tmp_debuglevel = 0;

  extern char *optarg;
  extern int optind;
  char* optstring =  "p:ta:c:d:f:vVh";

  byte imgbuffer[(128/8)*64];

  serdisp_CONN_t* sdcd;
  serdisp_t* dd = 0;

  sd_setdebuglevel(SD_LVL_WARN);

  while ((ch = getopt(argc, argv, optstring)) != -1) {
    switch(ch) {
      case 'h':
        print_usage(0);
        exit(0);
      case 'p':
        sdtools_strncpy(sdcdev, optarg, 50);
      break;
      case 't':
        flag_settime = 1;
      break;
      case 'c': 
        {
          char* tempptr;
          int isvalidnumber = 0;

          flag_settime = 1;
          if (strncasecmp(optarg, "0x", 2) == 0) {
            contrast = (int)strtol(optarg, &tempptr, 16);
          } else {
            contrast = (int)strtol(optarg, &tempptr, 10);
          }

          /* verify if optvalueptr contained a valid number */
          isvalidnumber = ( (optarg == tempptr) || ( (*tempptr != '\0')  ) ) ? 0 : 1;

          if (!isvalidnumber || contrast < 0 || contrast > 10) {
            fprintf(stderr, "Error: Invalid value for option -c: %s\n", optarg);
            exit (1);
          }
        }
      break;
      case 'f':
        {
          int rc;

          bootlogofile = (char*)sdtools_malloc(strlen(optarg)+2);
          sdtools_strncpy(bootlogofile, optarg, strlen(optarg));
          flag_setbootlogo = 1;
          rc = common_read_simplebmp(bootlogofile, imgbuffer, 128, 64, 1);

          if (rc != 0) {
            fprintf(stderr, "Error: %s\n", sd_geterrormsg());
            exit (1);
          }
        }
      break;
      case 'a':
        {
          char* tempptr;
          long value;
          int isvalidnumber;

          flag_alarmtime = 1;

          value = strtol(optarg, &tempptr, 10);

          /* verify if optvalueptr contained a valid number */
          isvalidnumber = /*( (optarg == tempptr) || (*/ (*tempptr != ':') /*) ) */ ? 0 : 1;

          if (isvalidnumber) {
            char* optarg_shift = tempptr+1;

            alarm_hour = (unsigned char) value;

            value = strtol(optarg_shift, &tempptr, 10);

            /* verify if optvalueptr contained a valid number */
            isvalidnumber = ( (optarg_shift == tempptr) || ( (*tempptr != '\0')) ) ? 0 : 1;
            if (isvalidnumber) {
              alarm_minute = (unsigned char) value;
            }
          }

          if (!isvalidnumber) {
            fprintf(stderr, "Error: Invalid value for option -a: %s\n", optarg);
            exit (1);
          }
        }
      break;
      case 'd':
        {
          char* tempptr;
          long value;
          int isvalidnumber;

          flag_alarmdays = 1;

          if ( (strlen(optarg) == 3) && ( ( strncasecmp(optarg, "off", 3) == 0) || (strncasecmp(optarg, "all", 3) == 0) ) ) {
            value = ( strncasecmp(optarg, "off", 3) == 0) ? 0 : 0x7F;
          } else {
            /* accept base 10 and base 16 values (base 16 if value starts with 0x or 0X) */
            if (strncasecmp(optarg, "0x", 2) == 0) {
              value = strtol(optarg, &tempptr, 16);
            } else {
              value = strtol(optarg, &tempptr, 10);
            }

            /* verify if optvalueptr contained a valid number */
            isvalidnumber = ( (optarg == tempptr) || ( (*tempptr != '\0')  ) ) ? 0 : 1;

            if (!isvalidnumber) {
              fprintf(stderr, "Error: Invalid value for option -d: %s\n", optarg);
              exit (1);
            }
          }

          alarm_days = (unsigned short) value;

          /* if -d 0  (deactive alarm): no -a required */
          if (alarm_days == 0) {
            flag_alarmtime = 1;
          }
        }
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
    fprintf(stderr, "Error: -p devicename is missing (eg.: -p /dev/hiddev0)\n");
    exit (1);
  }

  if (flag_setbootlogo && (flag_alarmtime || flag_alarmdays || flag_settime)) {
    fprintf(stderr, "Error: Changing boot logo (-f) and setting alarm or time/date (-a, -d, -s) may not be used altogether\n");
    exit (1);
  }

  /* if no flag given: show bootlogo */
  if (! (flag_setbootlogo || flag_alarmtime || flag_alarmdays || flag_settime) ) {
    flag_showbootlogo = 1;
  }

  if (flag_alarmdays ^ flag_alarmtime) {
    fprintf(stderr, "Error: Both -a and -d are required for setting alarm\n");
    exit (1);
  }

  if (alarm_hour > 23 || alarm_minute > 59 || alarm_days > 0x7f) {
    fprintf(stderr, "Error: Alarm time or bit field for alarm days out of bounds\n");
    exit (1);
  }

  sdcd = SDCONN_open(sdcdev);

  if (sdcd == (serdisp_CONN_t*)0) {
    fprintf(stderr, "Error: Unable to open %s, additional info: %s\n", sdcdev, sd_geterrormsg());
    exit (1);
  }

  if (flag_settime || flag_alarmtime) {
    snprintf(optionstring, 99, "POSTOFFMODE=1;ALARMHOUR=%02d;ALARMMINUTE=%02d;ALARMDAYS=0x%02x;RESMODE=1;CONTRAST=%d",alarm_hour, alarm_minute, alarm_days,contrast);
    sd_debug(1, "optionstring: %s", optionstring);
  } else if (flag_setbootlogo || flag_showbootlogo) {
    snprintf(optionstring, 99, "RESMODE=1");
    sd_debug(1, "optionstring: %s", optionstring);
  }


  dd = serdisp_init(sdcd, "L4M132C", optionstring);

  if (!dd) {
    fprintf(stderr, "Error: Unable to open L4M132C, additional info: %s\n", sd_geterrormsg());
    exit(1);
  }

  if (flag_setbootlogo) {
    int y, x, cols = 128/8;
    byte data;
    for (y = 0; y < dd->height; y++) {
      uint32_t addr = 0x3400 + y*cols;
      SDCONN_write(dd->sdcd, 0x21, 0);
      SDCONN_write(dd->sdcd, (byte)((addr & 0xFF00)>>8), 0);
      SDCONN_write(dd->sdcd, (byte)(addr & 0xFF), 0);
      SDCONN_write(dd->sdcd, cols, 0);

      for (x = 0; x < cols; x++) {
        data = imgbuffer[y*cols + x];
        SDCONN_write(dd->sdcd, data, 0);
      }

      SDCONN_commit(dd->sdcd);
    }
  }

  if (flag_setbootlogo || flag_showbootlogo) {
    SDCONN_write(dd->sdcd, 0x03, 0);
    SDCONN_commit(dd->sdcd);
    serdisp_close(dd);
  } else {
    serdisp_quit(dd);
  }
  return 0;
}

