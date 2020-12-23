/*
 *************************************************************************
 *
 * multidisplay.c
 * display images and text using serdisplib
 *
 *************************************************************************
 *
 * copyright (C) 2003-2010  wolfgang astleitner
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

#ifdef HAVE_GD2_GD_H
  #include "gd2/gd.h"
  #include "gd2/gdfontt.h"
  #include "gd2/gdfonts.h"
  #include "gd2/gdfontmb.h"
  #include "gd2/gdfontl.h"
  #include "gd2/gdfontg.h"
#elif defined(HAVE_GD_GD_H)
  #include "gd/gd.h"
  #include "gd/gdfontt.h"
  #include "gd/gdfonts.h"
  #include "gd/gdfontmb.h"
  #include "gd/gdfontl.h"
  #include "gd/gdfontg.h"
#else /* HAVE_GD_H */
  #include "gd.h"
  #include "gdfontt.h"
  #include "gdfonts.h"
  #include "gdfontmb.h"
  #include "gdfontl.h"
  #include "gdfontg.h"
#endif


#if defined(__linux__)
#include <getopt.h>
#endif

#include "serdisplib/serdisp.h"
#include "image.h"

#if defined(__NetBSD__) || defined(__FreeBSD__) || defined(__OpenBSD__)
  #include <sys/time.h>
#else
  #include <sys/resource.h>
#endif

/* version information */
#define MULTIDISPLAY_VERSION_MAJOR    1
#define MULTIDISPLAY_VERSION_MINOR    1
#define MULTIDISPLAY_VERSION_EXTRA


#define WHITE (colours-1)
#define BLACK (0)


void drawstring(gdImagePtr im, gdFontPtr gdfont, int fgcolour, int bgcolour, int x, int y, unsigned char* str, int outline) {
/*  int colours = gdImageColorsTotal(im);*/
  if (outline) {
    gdImageString(im, gdfont, x+1, y-1, str, bgcolour);
    gdImageString(im, gdfont, x-1, y-1, str, bgcolour);
    gdImageString(im, gdfont, x,   y-1, str, bgcolour);
    gdImageString(im, gdfont, x+1, y+1, str, bgcolour);
    gdImageString(im, gdfont, x-1, y+1, str, bgcolour);
    gdImageString(im, gdfont, x+1, y,   str, bgcolour);
    gdImageString(im, gdfont, x-1, y,   str, bgcolour);
    gdImageString(im, gdfont, x  , y+1, str, bgcolour);
  }
  gdImageString(im, gdfont, x, y, str, fgcolour);
}


void drawstringFT(gdImagePtr im, int* brect, char* fontname, double ptsize, int fgcolour, int bgcolour, int x, int y, char* str, int outline) {
/*  int colours = gdImageColorsTotal(im);*/
  if (outline) {
    gdImageStringFT(im, brect, bgcolour, fontname, ptsize, 0, x+1, y-1, str);
    gdImageStringFT(im, brect, bgcolour, fontname, ptsize, 0, x-1, y-1, str);
    gdImageStringFT(im, brect, bgcolour, fontname, ptsize, 0, x,   y-1, str);
    gdImageStringFT(im, brect, bgcolour, fontname, ptsize, 0, x+1, y+1, str);
    gdImageStringFT(im, brect, bgcolour, fontname, ptsize, 0, x-1, y+1, str);
    gdImageStringFT(im, brect, bgcolour, fontname, ptsize, 0, x+1, y,   str);
    gdImageStringFT(im, brect, bgcolour, fontname, ptsize, 0, x-1, y,   str);
    gdImageStringFT(im, brect, bgcolour, fontname, ptsize, 0, x  , y+1, str);
  }
  gdImageStringFT(im, brect, fgcolour, fontname, ptsize, 0, x, y, str);
}


/* usage, help, ...
   iserr: -1 ... print version information only
           0 ... print help/usage to stdout
           1 ... print error message + help/usage to stderr
*/
void print_usage(int iserr) {
  serdisp_display_t displaydesc;
  FILE* f = (iserr > 0) ? stderr : stdout;

  fprintf(f, "  multidisplay version %d.%d   (using serdisplib version %d.%d)\n", 
    MULTIDISPLAY_VERSION_MAJOR, MULTIDISPLAY_VERSION_MINOR,
    SERDISP_VERSION_GET_MAJOR(serdisp_getversioncode()), 
    SERDISP_VERSION_GET_MINOR(serdisp_getversioncode())
  );
  fprintf(f, "  (C) 2003-2010 by Wolfgang Astleitner\n");

  fprintf(f, "\n");

  if (iserr == -1) return;
  
  fprintf(f, "Usage: multidisplay -n <display name> [<options>]\n\n");
  fprintf(f, "  Options: (default values in squared brackets)\n");
  fprintf(f, "    -n name          display name\n");
  fprintf(f, "    -p dev|port      output device or port\n");
  fprintf(f, "    -o options       options for driver, semicolon-separated key-value pairs\n");
  fprintf(f, "                     eg: -o \"WIRING=1;CONTRAST=2;BACKLIGHT=ON\"\n");
  fprintf(f, "    -D 0|1      [1]  dither (0: no, 1: auto (floyd steinberg))\n");
  fprintf(f, "    -S 0-3      [1]  scaling algorithm:\n");
  fprintf(f, "                     0: don't scale and don't correct aspect ratio\n");
  fprintf(f, "                     1: fit to display if larger but not if smaller (maintaining aspect ratio)\n");
  fprintf(f, "                     2: fit to display if larger or smaller (maintaining aspect ratio)\n");
  fprintf(f, "                     3: fill display (ignoring aspect ratio)\n");
  fprintf(f, "    -L 0|1|>1   [1]  loop count for animated images:\n");
  fprintf(f, "                     0: loops forever\n");
  fprintf(f, "                     1: loop once (default)\n");
  fprintf(f, "                    >1: loop given times\n");
  fprintf(f, "    -i               invert display\n");
  fprintf(f, "    -d delay    [0]  time to delay before program quits\n");
  fprintf(f, "    -r rotate   [0]  rotate display, valid values: 0, 90, 180, 270\n");
  fprintf(f, "    -B 0|1      [1]  backlight (0: no, 1: yes)\n");
  fprintf(f, "    -M message       message\n");
  fprintf(f, "    -m file          display content of file\n");
  fprintf(f, "    -c command       execute command and display its output (also set -y !!)\n");
  fprintf(f, "    -y               -c is only executed when -y is also set (security)\n");
  fprintf(f, "    -f imagefile     display an image\n");
  fprintf(f, "    -F font          text font (\"tiny\", \"small\", \"mediumbold\", \"large\", \"giant\")\n");
  fprintf(f, "                     or truetype font + optional size (eg: \"/some/location/somefont.ttf:16\")\n");
  fprintf(f, "    -Q               quit AND clear display after everything is displayed \n");
  fprintf(f, "    -s ms    [1000]  sleep ms milli seconds between two updates\n");
  fprintf(f, "    -v               verbose (-v repeated: <= 2: log to syslog, >= 3: log to stderr, >= 5: log to stdout)\n");
  fprintf(f, "    -V               version information\n");
  fprintf(f, "            \n");
  fprintf(f, "  Examples: \n");
  fprintf(f, "    multidisplay -n PCD8544 -p \"/dev/parport0\"\n");
  fprintf(f, "    multidisplay -n PCD8544 -p \"0x378\"  # direct IO\n");
  fprintf(f, "    multidisplay -n PCD8544             # default parport device will be used\n\n");
  fprintf(f, "    # use wiring nr. 1, invert, and rotate 90 degrees; display a string\n");
  fprintf(f, "    multidisplay -n PCD8544 -o \"WIRING=1;INV=YES;ROT=90\" -M \"hello world\"\n");
  fprintf(f, "            \n");
  if (!iserr) {
    fprintf(f, "  Supported displays: (-n <displayname> -h ... specific display information)\n");
    displaydesc.dispname = "";
    fprintf(f, "    display name     alias names           description\n");
    fprintf(f, "    ---------------  --------------------  -----------------------------------\n");
    while(serdisp_nextdisplaydescription(&displaydesc)) {      
      fprintf(f, "    %-15s  %-20s  %-35s\n", displaydesc.dispname, displaydesc.aliasnames, displaydesc.description);
    }
    fprintf(f, "            \n");
  } else {
    fprintf(f, "\n");
    fprintf(f, "  Extended help:\n");
    fprintf(f, "    -h                  ... help screen and supported displays\n");
    fprintf(f, "    -n <displayname> -h ... display information (eg. supported wirings for <displayname>)\n\n");
  }
}



int main(int argc, char **argv) {
  int ch;
  char sdcdev[51] = "";
  char dispname[51] = "";

  int i, j, c;

  gdImagePtr im = 0;  
  
  sdtim_frame_t* firstframe = 0;
  sdtim_frame_t* currframe = 0;
  
  int im_animframes = 0;
/*  int im_currframe = 0; */

  gdFontPtr gdfont = gdFontSmall;

  char* message = (char*)0;
  char* fontfile = (char*)0;

  char** imagefiles = (char**)0;
  int    imagefiles_cnt = 0;
  int    curr_imagefile = 0;
  
  
  char* catfile = (char*)0;
  char* cmdstring = (char*)0;
  
  char* optionstring = (char*)0;

  char* fontname = (char*)0;
  int fontsize = 8;
  int fontheight = gdfont->h;


  int bgcolour = 0, fgcolour = 0;
  
  int colours = 0;
  int* gdcols = 0;

  int flag_showimage = 0;
  int flag_showmessage = 0;
  int flag_showfile = 0;
  int flag_showcmd = 0;
  int flag_delay = 0;
  int flag_quit = 0;
  int flag_rotate = 0;
  int flag_invert = 0;
  int flag_dither = 1;
  int flag_yes = 0;
  int flag_backlight = 1;
  int flag_scalealgo = 1;   /* fit to display if larger */
  int flag_loops = 1;       /* 0 .. loops forever, 1 (default): loop anim once, > 1: loop n times */
  
  int flag_switchimage = 0; /* 1 .. switch to next image (if -f with more than one images) */

  long delay_sleep = 1000;  /* sleep delay_sleep milliseconds between two updates (default == 1000 (== 1 sec)) */
  
  int curr_loopcount = 0;
  
  
  int tmp_debuglevel = 0;
  
  int flag_truecolour = 0;
  
/*  int flag_wrap = 0;*/

  extern char *optarg;
  extern int optind;
  char* optstring =  "n:p:hD:id:r:B:M:m:c:f:F:yQL:S:s:o:vV";
  

  serdisp_CONN_t* sdcd;
  serdisp_t* dd = 0;

  sd_setdebuglevel(SD_LVL_WARN);

  while ((ch = getopt(argc, argv, optstring)) != -1) {
    switch(ch) {
      case 'h':
        if (strlen(dispname) > 0) {
          serdisp_wiredef_t wiredesc;
          serdisp_display_t displaydesc;
          
          if (serdisp_getdisplaydescription(dispname, &displaydesc)) {
            fprintf(stdout, "  Informations for display '%s': \n", dispname);
            fprintf(stdout, "\n");
            fprintf(stdout, "    display name     alias names           description\n");
            fprintf(stdout, "    ---------------  --------------------  -----------------------------------\n");
            fprintf(stdout, "    %-15s  %-20s  %-35s\n", displaydesc.dispname, displaydesc.aliasnames, displaydesc.description);
            fprintf(stdout, "\n\n");
            fprintf(stdout, "  Supported wirings: \n");
            fprintf(stdout, "\n");
            fprintf(stdout, "    idx  wiring name           description\n");
            fprintf(stdout, "    ---  --------------------  -----------------------------------\n");

            wiredesc.name = "";
            while(serdisp_nextwiringdescription(dispname, &wiredesc)) {
              fprintf(stdout, "    %3d  %-20s  %-35s\n", wiredesc.id, wiredesc.name, wiredesc.description);
            }
            fprintf(stdout, "\n\n");
            fprintf(stdout, "  Default options:     %s\n", 
                    (displaydesc.optionstring && strlen(displaydesc.optionstring) > 0 ) ? displaydesc.optionstring : "none");
            fprintf(stdout, "\n");
          }
        } else
          print_usage(0);
        exit(0);
      case 'n':
        sdtools_strncpy(dispname, optarg, 50);
      break;
      case 'p':
        sdtools_strncpy(sdcdev, optarg, 50);
      break;
      case 'D':
        {
          int arg = atoi(optarg);
          if (arg == 0)
            flag_dither = 0;
        }
      break;
      case 'i':
        flag_invert = 1;
      break;
      case 'f':
        {
          int old_optind = optind;
          int next_opt = -1;
          int next_opt_idx = -1;
          int i;
          char* temp_imagefile;
          
          temp_imagefile = (char*)sdtools_malloc(strlen(optarg)+2);
          sdtools_strncpy(temp_imagefile, optarg, strlen(optarg));
          flag_showimage = 1;

          /* compiler/gnu-independend way to be able to have more than one argument for -f */

          next_opt = getopt(argc, argv, optstring);

          /* next_opt == -1: no more options after this one -> so use argc as border
             optarg == NULL: current optind needs to be -1, otherwise (if next option has an argument) -2
           */
          next_opt_idx = (next_opt == -1) ? argc : ((optarg) ? optind-2 : optind-1);

          imagefiles_cnt = next_opt_idx - old_optind + 1;  /* +1 == temp_imagefile from first -f argument */

          imagefiles = (char**)sdtools_malloc(sizeof(char*) * imagefiles_cnt);
          imagefiles[0] = temp_imagefile;

          for (i = old_optind ; i < next_opt_idx; i++) {
            imagefiles[i-old_optind+1] = (char*)sdtools_malloc(strlen(argv[i])+2);
            sdtools_strncpy(imagefiles[i-old_optind+1], argv[i], strlen(argv[i]));
          }
          

          if (sd_getdebuglevel() >= 2) {
            sd_debug(2, "argument -f: total amount of images: %d\n", imagefiles_cnt);

            for (i = 0; i < imagefiles_cnt; i++)
              sd_debug(2, "image[%02d]: %s\n", i, imagefiles[i]);
          }
          
          if (next_opt != -1) {
           optind = next_opt_idx;
           fprintf(stdout, "Warning: -f should be the last parameter!\n");
          }
        }
      break;
      case 'F':
        if (strcasecmp(optarg, "tiny") == 0)
          gdfont = gdFontTiny;
        else if (strcasecmp(optarg, "mediumbold") == 0)
          gdfont = gdFontMediumBold;
        else if (strcasecmp(optarg, "large") == 0)
          gdfont = gdFontLarge;
        else if (strcasecmp(optarg, "giant") == 0)
          gdfont = gdFontGiant;
        else if (strcasecmp(optarg, "small") == 0)
          gdfont = gdFontSmall;
        else {
          int namelen = strlen(optarg);
          char* idx = index(optarg, ':');
          int brect[8];

          if (idx) {
            namelen =  serdisp_ptrstrlen(idx, optarg);
            fontname = (char*)sdtools_malloc(namelen+2);
            sdtools_strncpy(fontname, optarg, namelen);
            fontsize = atoi(++idx);
            if (fontsize < 1) fontsize = 8;
          } else {
            fontname = (char*)sdtools_malloc(strlen(optarg)+2);
            sdtools_strncpy(fontname, optarg, strlen(optarg));
          }
          if (gdImageStringFT(0, brect, 0, fontname, fontsize, 0, 0, 0, "Tqg|")) {
            gdfont = gdFontSmall;
            free(fontname);
            fontname = (char*)0;
            fontheight = gdfont->h;
          } else {
            fontheight = brect[1]-brect[7];
          }
        }
        if (!fontname)
          fontheight = gdfont->h;
      break;
      case 'M':
        message = (char*)sdtools_malloc(strlen(optarg)+2);
        sdtools_strncpy(message, optarg, strlen(optarg));
        flag_showmessage = 1;
      break;
      case 'Q':
        flag_quit = 1;
      break;
      case 'B':
        {
          int arg = atoi(optarg);
          if (arg >=0 && arg <= 1)
            flag_backlight = arg;
        }
      break;
      case 'd':
        {
          int arg = atoi(optarg);
          if (arg >0)
            flag_delay = arg;
        }
      break;
      case 'r':
        {
          int arg = atoi(optarg);
          switch(arg) {
            case 90: flag_rotate = 90; break;
            case 180: flag_rotate = 180; break;
            case 270: flag_rotate = 270; break;
            default: flag_rotate = 0;
          }
        }
      break;
      case 's':
        {
          long arg = atol(optarg);
          if (arg >0 )
            delay_sleep = arg;
        }
      break;
      case 'L':
        {
          int arg = atoi(optarg);
          if (arg >=0 )
            flag_loops = arg;
        }
      break;
      case 'S':
        {
          int arg = atoi(optarg);
          if (arg >=0 )
            flag_scalealgo = arg;
        }
      break;
      case 'm':
        {
        catfile = (char*)sdtools_malloc(strlen(optarg)+2);
        sdtools_strncpy(catfile, optarg, strlen(optarg));
        flag_showfile = 1;
        }
      break;
/*      case 'w':
        {
          int arg = atoi(optarg);
          if (arg >0)
            flag_wrap = arg;
        }
*/        
      break;
      case 'c':
        {
        cmdstring = (char*)sdtools_malloc(strlen(optarg)+2);
        sdtools_strncpy(cmdstring, optarg, strlen(optarg));
        flag_showcmd = 1;
        }
      break;
      case 'o':
        {
        optionstring = (char*)sdtools_malloc(strlen(optarg)+2);
        sdtools_strncpy(optionstring, optarg, strlen(optarg));
        }
      break;
      case 'y':
        flag_yes = 1;
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

  if (strlen(dispname) < 1) {          
    fprintf(stderr, "required option -n <display name> missing ...\n\n");
    print_usage(1);
    exit(1);
  }
         
  sdcd = SDCONN_open(sdcdev);

  if (sdcd == (serdisp_CONN_t*)0) {  
    fprintf(stderr, "Error: Unable to open %s, additional info: %s\n", sdcdev, sd_geterrormsg());
    exit (1);
  }
   

  dd = serdisp_init(sdcd, dispname, (optionstring) ? optionstring : "");

  if (!dd) {          
    fprintf(stderr, "Error: Unknown display or unable to open %s, additional info: %s\n", dispname, sd_geterrormsg());
    exit(1);
  }

  serdisp_clear(dd);

  if (! flag_backlight)
  serdisp_setoption(dd, "BACKLIGHT", SD_OPTION_NO);
  
  if (flag_rotate)
    serdisp_setoption(dd, "ROTATE", flag_rotate);
  if (flag_invert)
    serdisp_setoption(dd, "INVERT", SD_OPTION_YES);


  if (SD_CS_ISDIRECTCOLOUR(dd)) {  /* packed or true colour space */
    flag_truecolour = 1;
    flag_dither = 0;
    if (SD_CS_ISSELFEMITTING(dd)) {
      fgcolour = gdTrueColor(0x00, 0x00, 0x00);
      bgcolour = gdTrueColor(0xFF, 0xFF, 0xFF);
    } else {
      fgcolour = gdTrueColor(0xFF, 0xFF, 0xFF);
      bgcolour = gdTrueColor(0x00, 0x00, 0x00);
    }
  } else if (dd->depth == 8) {  /* greyscale, 256 greyvalues */
    flag_dither = 0;
  }

  /* create main gd-image */
  if (flag_truecolour)
    im = gdImageCreateTrueColor(serdisp_getwidth(dd), serdisp_getheight(dd));
  else
    im = gdImageCreate(serdisp_getwidth(dd), serdisp_getheight(dd));

  if (!flag_truecolour) {    
    colours = serdisp_getcolours(dd);
    sd_debug(2, "colours supported by display: %d\n", colours);
    gdcols = (int*) sdtools_malloc(colours* sizeof(int));

    /* allocate greylevel colours for main image */
    for (i = 0; i < colours; i++) {
      int val = (255 / (colours - 1)) * i;
      gdcols[i] = gdImageColorAllocate(im, val, val, val);
    }
    if (SD_CS_ISSELFEMITTING(dd)) {
      fgcolour = gdcols[WHITE];
      bgcolour = gdcols[BLACK];
    } else {
      fgcolour = gdcols[BLACK];
      bgcolour = gdcols[WHITE];
    }
  }


  curr_imagefile = 0;
  flag_switchimage = 1;
  
  do {  /* while  loops left || frames left */
    if (flag_showimage) {

      if (flag_switchimage) {      
        sd_debug(1, "image[%02d]: %s",  curr_imagefile, imagefiles[curr_imagefile]);

        /* fill image with background colour (sdtim_loadimage will copy this!) */
        gdImageFilledRectangle(im, 0, 0, gdImageSX(im)-1, gdImageSY(im)-1, bgcolour);

        firstframe = sdtim_loadimage (imagefiles[curr_imagefile], &im_animframes, dd, im, flag_scalealgo, flag_truecolour, flag_dither);
        if (firstframe == (sdtim_frame_t*)0) {
          fprintf(stderr, "Error: Image file %s cannot be opened.\n", imagefiles[curr_imagefile]);
          exit(1);
        }    
        sd_debug(2, "frames: %d,  anim[0].x/y: %d/%d\n", im_animframes, gdImageSX(firstframe->image), gdImageSY(firstframe->image));
        currframe = firstframe;
        flag_switchimage = 0;
      } /* flag_switchimage == 1 */
    } /* flag_showimage */





    gdImageFilledRectangle(im, 0, 0, gdImageSX(im)-1, gdImageSY(im)-1, bgcolour);
    
    if (flag_showimage) {
      gdImageCopy(
        im, currframe->image,
        0, 0, 
        0,0,
        serdisp_getwidth(dd), serdisp_getheight(dd)
      );
    }


    if (flag_showmessage) {
      char* ptrptr;
      char* token;
      char* separator = "\\\n\r\f";
      int brect[8];
      int  h = 0;

      /* fill image with background colour */
      if (!flag_showimage)          
        gdImageFilledRectangle(im, 0, 0, gdImageSX(im)-1, gdImageSY(im)-1, bgcolour);

      /* truetype: calculate string height (else 1st line is swallowed) */
      if (fontname)
        h += fontheight;

      token = (char*)strtok_r (message, separator, &ptrptr);
      do {
        if (fontname)
          drawstringFT(im, brect, fontname, fontsize, fgcolour, bgcolour, 0, h, token, flag_showimage);
        else
          drawstring(im, gdfont, fgcolour, bgcolour, 0, h, (unsigned char*)token, flag_showimage);
        h += fontheight;
      } while( (token = (char*)strtok_r(0, separator, &ptrptr)));

    } else if (flag_showfile || (flag_showcmd && flag_yes)) {
      FILE* fin;
      int  h = 0;
      char buffer[255];
      int brect[8];

      if (flag_showcmd) {
          fin = (FILE*)popen(cmdstring, "r");
      } else {
      
        if (strcmp(catfile, "-") == 0)
          fin = stdin;
        else
          fin = fopen(catfile, "r");
      }

      if (!fin) {
        if (flag_showcmd) {
          fprintf(stderr, "Error: Can't process command string %s.\n", cmdstring);
        } else {
          fprintf(stderr, "Error: Can't read input file %s.\n", catfile);
        }
        if (!flag_showimage) exit(1);
      }    

      /* fill image with background colour */
      if (!flag_showimage)
        gdImageFilledRectangle(im, 0, 0, gdImageSX(im)-1, gdImageSY(im)-1, bgcolour);

      /* truetype: calculate string height (else 1st line is swallowed) */
      if (fontname)
        h += fontheight;

      while (fgets(buffer, 254, fin)) {
        
        buffer[strlen(buffer)-1] = '\0';
        if (fontname)
          drawstringFT(im, brect, fontname, fontsize, fgcolour, bgcolour, 0, h, buffer, flag_showimage);
        else
          drawstring(im, gdfont, fgcolour, bgcolour, 0, h, (unsigned char*)buffer, flag_showimage);

        h += fontheight;

      };

      if (flag_showcmd) {
        pclose(fin);
      } else {
        if (strcmp(catfile, "-") != 0)
          fclose(fin);
      }
    } /* if flag_showfile */


    if (!flag_truecolour) {
      sd_debug(2, "image (before gd -> serdisp ): totalcolours: %d\n", gdImageColorsTotal(im));
      for (i = 0; i < gdImageColorsTotal(im); i++) {
        sd_debug(2, " col[%d] = %02x %02x %02x\n", i, gdImageRed(im, i), gdImageGreen(im, i), gdImageBlue(im, i));
      }
    }

    /* draw image -> display */
    for (j = 0; j < gdImageSY(im); j++)
     for (i = 0; i < gdImageSX(im); i++) {
       c = gdImageGetPixel(im, i, j);      
#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
       serdisp_setsdcol(dd, i, j, serdisp_pack2ARGB(0xFF, gdImageRed(im, c), gdImageGreen(im, c), gdImageBlue(im, c)));
#else
       serdisp_setcolour(dd, i, j, serdisp_pack2ARGB(0xFF, gdImageRed(im, c), gdImageGreen(im, c), gdImageBlue(im, c)));
#endif
     }

  
    /* commit */
    serdisp_update(dd);

    if (flag_showimage) {
      if(im_animframes > 1) {
        usleep(currframe->delay * 1000);

        if (currframe->next) {
          currframe = currframe->next;
        } else {
          currframe = firstframe;
          curr_imagefile++;
          flag_switchimage = 1;
        }                
      } else {  /* single image */
        usleep(delay_sleep * 1000);
        curr_imagefile++;
        flag_switchimage = 1;
      }
      
      /* don't switch if only one image */
      if (imagefiles_cnt == 1)   
        flag_switchimage = 0;
    }
    
    /* all frames done: free image and prepare for next image */
    if (flag_showimage) {
      if (flag_switchimage) {
        /* free memory of current image */
        currframe = firstframe;
        while(currframe) {
          sdtim_frame_t* nextframe = currframe->next;

          gdImageDestroy(currframe->image);
          free(currframe);
          currframe = nextframe;
        }
      }
      
      /* all images done: increase loop count and restart with the first image */
      if (curr_imagefile >= imagefiles_cnt) {
        curr_loopcount++;
        curr_imagefile = 0;
      }
    }
    
    /* loop conditions if no images given */
    if (! flag_showimage) {
      curr_loopcount++;
      if (curr_loopcount < flag_loops)
        usleep(delay_sleep * 1000);
    }

  } while ( (flag_loops == 0 || curr_loopcount < flag_loops /* || (curr_loopcount == flag_loops && curr_imagefile == 0)*/) && 
            (flag_showimage && (curr_imagefile < imagefiles_cnt)) 
          );

  gdImageDestroy(im);


  if (flag_delay)
    sleep(flag_delay);  

  if (message)
    free(message);

  if (fontfile)
    free(fontfile);

  if (catfile)
    free(catfile);

  if (cmdstring)
    free(cmdstring);

  if (imagefiles) {
    int i;
    for (i = 0; i < imagefiles_cnt; i++)
      free(imagefiles[i]);
    free(imagefiles);
  }


  if (flag_quit == 1)
    serdisp_quit(dd);
  else
    serdisp_close(dd);
  return 0;
}

