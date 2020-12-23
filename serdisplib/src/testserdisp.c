/*
 *************************************************************************
 *
 * testserdisp.c
 * program for testing serdisplib
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
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


#if defined(__linux__)
#include <getopt.h>
#endif

#include "serdisplib/serdisp.h"

#include <sys/time.h>
#if 0
#if defined(__NetBSD__) || defined(__FreeBSD__) || defined(__OpenBSD__)
 #include <sys/time.h>
#else
 #include <sys/resource.h>
#endif
#endif

/* version information */
#define TESTSERDISP_VERSION_MAJOR    1
#define TESTSERDISP_VERSION_MINOR    4
#define TESTSERDISP_VERSION_EXTRA

/*
 *   some help functions
 */
void draw_bar(serdisp_t* dd, int x, int y, int w, int h, int hor, int type, long colour) {
  int i,j;
  for (j = y; j < y + ((hor)? h : w); j += ((type) ? 2 : 1))
    for (i = x; i < x + ((hor)? w : h); i += ((type) ? 2 : 1))
      serdisp_setcolour(dd, i, j, colour);
}


void draw_digit(serdisp_t* dd, int x, int y, int digit, int segwidth, int thick, long colour) {
  if (digit < 0 || digit > 9) return;

  draw_bar(dd, x, y,                    segwidth, thick, 1, !(digit != 1 && digit != 4), colour);
  draw_bar(dd, x, y + segwidth - thick, segwidth, thick, 1, !(digit != 1 && digit != 7 && digit != 0), colour);
  draw_bar(dd, x, y + 2*(segwidth - thick), segwidth, thick, 1, !(digit != 1 && digit != 4 && digit != 7), colour);

  draw_bar(dd, x, y,                    segwidth, thick, 0, !(digit == 4 || digit == 5 || digit == 6 || digit == 8 || digit ==  9 || digit == 0), colour);
  draw_bar(dd, x + segwidth - thick, y,  segwidth, thick, 0, !(digit != 5 && digit != 6), colour);
  draw_bar(dd, x, y + segwidth - thick,                    segwidth, thick, 0, !(digit == 2 || digit == 6 || digit == 8 || digit == 0), colour);
  draw_bar(dd, x + segwidth - thick, y + segwidth - thick,  segwidth, thick, 0, !(digit != 2), colour);

}


void draw_number(serdisp_t* dd, int x, int y, int number, int segwidth, int thick, int alignr, long colour) {
  int digits, i;

  int num = number;

  if (num < 0 && num > 9999) return;

  if (num >= 1000) digits = 4;
  else if (num >= 100) digits = 3;
  else if (num >= 10) digits = 2;
  else digits = 1;

  if (alignr) x -= digits*segwidth+digits*(thick-1);

  for (i = digits-1; i >= 0; i--) {
    draw_digit(dd, x + i*segwidth + i*thick, y, num % 10, segwidth, thick, colour);
    num /= 10;
  }
}



/*
 *   some test functions
 */

void draw_testpic(serdisp_t* dd, int id, long colour) {
  if (id < 0 || id > 1) return;

  if (id == 0) {
    int segwidth = (serdisp_getheight(dd)<64)?9:11;
    if (serdisp_getwidth(dd) < 48) segwidth = 7;

    draw_bar(dd, 0, 0, 2, 2, 1, 0, colour);
    draw_bar(dd, serdisp_getwidth(dd)-2, 0, 2, 2, 1, 0, colour);
    draw_bar(dd, 0, serdisp_getheight(dd)-2, 2, 2, 1, 0, colour);
    draw_bar(dd, serdisp_getwidth(dd)-2, serdisp_getheight(dd)-2, 2, 2, 1, 0, colour);
  
    if (serdisp_getwidth(dd) >= 48) 
      draw_bar(dd, serdisp_getwidth(dd) / 2 -1 + serdisp_getwidth(dd)%2,     0, serdisp_getheight(dd), 2-serdisp_getwidth(dd)%2, 0, 0, colour);

    if (serdisp_getheight(dd) >= 48) 
      draw_bar(dd, 0, serdisp_getheight(dd) / 2 - 1 + serdisp_getheight(dd)%2, serdisp_getwidth(dd), 2-serdisp_getheight(dd)%2, 1, 0, colour);

    draw_number(dd, 4, 4, serdisp_getwidth(dd), segwidth, 3, 0, colour);
    draw_number(dd, serdisp_getwidth(dd)-4+1, serdisp_getheight(dd)-4-2*segwidth+3, serdisp_getheight(dd), segwidth, 3, 1, colour);
  } else if (id == 1) {
    int segwidth = (serdisp_getheight(dd)<64)?9:11;
    int thick = 3;
    int currx = thick;
    int curry = thick;
    int cnt = 0;
    long cols[7];

    if (serdisp_getwidth(dd) < 48) segwidth = 7;

    if (serdisp_getcolours(dd) >= 256) {
      cols[0] = SD_COL_YELLOW;
      cols[1] = SD_COL_MAGENTA;
      cols[2] = SD_COL_RED;
      cols[3] = SD_COL_CYAN;
      cols[4] = SD_COL_GREEN;
      cols[5] = SD_COL_BLUE;
      cols[6] = colour;
    } else {
      int i;
      for (i = 0; i < 7; i++)
        cols[i] = colour;
    }

    while (curry < serdisp_getheight(dd)) {
      draw_digit(dd, currx, curry, cnt%10, segwidth, thick, cols[cnt%7]);
      if (currx+segwidth+thick < serdisp_getwidth(dd)) {
       currx += segwidth+thick;
      } else {
       currx = thick;
       curry += 2*segwidth;
      }
      cnt++;
    }
  }
  serdisp_update(dd);
}


void draw_coltestpic(serdisp_t* dd) {
  long colourw, colourh, colourbar, colourcor;
  int segwidth = (serdisp_getheight(dd)<64)?9:11;
  if (serdisp_getwidth(dd) < 48) segwidth = 7;

  if (SD_CS_ISGREY(dd)) {
    colourw   = serdisp_GREY2ARGB(0xAA);  /* light grey */
    colourh   = serdisp_GREY2ARGB(0x55);  /* dark grey */
    colourbar = serdisp_GREY2ARGB(SD_CS_ISSELFEMITTING(dd) ? 0xFF : 0x00);  /* black */
    colourcor = serdisp_GREY2ARGB(SD_CS_ISSELFEMITTING(dd) ? 0xFF : 0x00);  /* black */
  } else {
    colourw   = SD_COL_RED;
    colourh   = SD_COL_GREEN;
    colourbar = SD_COL_BLUE;
    colourcor = SD_CS_ISSELFEMITTING(dd) ? SD_COL_WHITE : SD_COL_BLACK;
  }
  
  draw_bar(dd, 0, 0, 2, 2, 1, 0, colourcor);
  draw_bar(dd, serdisp_getwidth(dd)-2, 0, 2, 2, 1, 0, colourcor);
  draw_bar(dd, 0, serdisp_getheight(dd)-2, 2, 2, 1, 0, colourcor);
  draw_bar(dd, serdisp_getwidth(dd)-2, serdisp_getheight(dd)-2, 2, 2, 1, 0, colourcor);

  if (serdisp_getwidth(dd) >= 48) 
    draw_bar(dd, serdisp_getwidth(dd) / 2 -1 + serdisp_getwidth(dd)%2,     0, serdisp_getheight(dd), 2-serdisp_getwidth(dd)%2, 0, 0, colourbar);

  if (serdisp_getheight(dd) >= 48) 
    draw_bar(dd, 0, serdisp_getheight(dd) / 2 - 1 + serdisp_getheight(dd)%2, serdisp_getwidth(dd), 2-serdisp_getheight(dd)%2, 1, 0, colourbar);

  draw_number(dd, 4, 4, serdisp_getwidth(dd), segwidth, 3, 0, colourw);
  draw_number(dd, serdisp_getwidth(dd)-4+1, serdisp_getheight(dd)-4-2*segwidth+3, serdisp_getheight(dd), segwidth, 3, 1, colourh);
  serdisp_update(dd);
}


void draw_colrgbpic(serdisp_t* dd) {
  int w = serdisp_getwidth(dd);
  int h = serdisp_getheight(dd);

  int sw1 = w/6;
  int sw2 = w/6+w/6;
  int sw3 = w/6+w/6+w/3;
  int sh1 = h/4;
  int sh2 = h/4+h/4;

  /* draw a red, a green, a blue, and a white area */
  /* +1 in width and heights: eliminate rounding errors */
  draw_bar(dd, 0, 0,     w/2, h/2, 1, 0, SD_COL_RED);
  draw_bar(dd, w/2, 0,   w/2+1, h/2, 1, 0, SD_COL_GREEN);
  draw_bar(dd, 0, h/2,   w/2, h/2+1, 1, 0, SD_COL_BLUE);
  draw_bar(dd, w/2, h/2, w/2+1, h/2+1, 1, 0, SD_COL_WHITE);

  /* sw1, sw2, ...: avoid gaps caused through rounding errors */
  draw_bar(dd, sw1, sh1, sw2-sw1, h/2,     1, 0, SD_COL_MAGENTA);
  draw_bar(dd, sw2, sh1, sw3-sw2, sh2-sh1, 1, 0, SD_COL_YELLOW);
  draw_bar(dd, sw2, sh2, sw3-sw2, h/4,     1, 0, SD_COL_BLACK);
  draw_bar(dd, sw3, sh1, w/6, h/2,         1, 0, SD_COL_CYAN); 
}


void draw_gradient(serdisp_t* dd, int type) { /* type: 0: red, 1: green, 2: blue */
  int w = serdisp_getwidth(dd);
  int h = serdisp_getheight(dd);
  int i, j, stdgrad = 0;
  double cmain, cothers, cotherbase;
  long colour;

  if (type > 3) {
    stdgrad = 1;
    type -= 3;
  }

  if (type == 0) {
    for (i = 0; i < w ; i++) {
      cmain = ( 255.0 / (w-1) ) * i;
      for (j = 0; j < h; j++) {
        colour = 0;
        if (j < h/4)
          colour = serdisp_pack2ARGB (0xFF, (byte)(cmain+0.5), (byte)(cmain+0.5), (byte)(cmain+0.5));
        else if (j < h/2)
          colour = serdisp_pack2ARGB (0xFF, (byte)(cmain+0.5), 0x00, 0x00);
        else if (j < 3*h/4)
          colour = serdisp_pack2ARGB (0xFF, 0x00, (byte)(cmain+0.5), 0x00);
        else
          colour = serdisp_pack2ARGB (0xFF, 0x00, 0x00, (byte)(cmain+0.5));
        serdisp_setcolour(dd, i, j, colour);
      }
    }
  } else {
    for (j = 0; j < h; j++) {
      cotherbase = ( 255.0 / (h-1) ) * j;
      for (i = 0; i < w ; i++) {
        cmain = ( 255.0 / (w-1) ) * i;
        cothers = (stdgrad) ? ( cotherbase / (w-1) ) * i : cotherbase;

        colour = 0;
        switch (type) {
          case 2: /* green */
            colour = serdisp_pack2ARGB (0xFF, (byte)(cothers+0.5), (byte)(cmain+0.5), (byte)(cothers+0.5));
          break;
          case 3: /* blue */
            colour = serdisp_pack2ARGB (0xFF, (byte)(cothers+0.5), (byte)(cothers+0.5), (byte)(cmain+0.5));
          break;
          default: /* red */
            colour = serdisp_pack2ARGB (0xFF, (byte)(cmain+0.5), (byte)(cothers+0.5), (byte)(cothers+0.5));
          break;
        }
        serdisp_setcolour(dd, i, j, colour);
      }
    }
  }
}


void shift_left(serdisp_t* dd, int count) {
  int x, y, i;
  long temp;

  for (i = 0; i < count; i++) {
    for (y = 0; y < serdisp_getheight(dd); y++) {
      temp = serdisp_getcolour(dd, 0, y);
      for (x = 0; x < serdisp_getwidth(dd) - 1; x++)
        serdisp_setcolour(dd, x, y, serdisp_getcolour(dd, x+1, y));
      serdisp_setcolour(dd, serdisp_getwidth(dd)-1, y, temp);
    }
    serdisp_update(dd);
    usleep(10000);
  }
}


void shift_right(serdisp_t* dd, int count) {
  int x, y, i;
  long temp;

  for (i = 0; i < count; i++) {
    for (y = 0; y < serdisp_getheight(dd); y++) {
      temp = serdisp_getcolour(dd, serdisp_getwidth(dd)-1, y);
      for (x = serdisp_getwidth(dd)-2; x >= 0; x--)
        serdisp_setcolour(dd, x+1, y, serdisp_getcolour(dd, x, y));
      serdisp_setcolour(dd, 0, y, temp);
    }
    serdisp_update(dd);
    usleep(10000);
  }
}


void print_usage(int iserr) {
  serdisp_display_t displaydesc;
  FILE* f = (iserr > 0) ? stderr : stdout;

  fprintf(f, "  testserdisp version %d.%d   (using serdisplib version %d.%d)\n", 
    TESTSERDISP_VERSION_MAJOR, TESTSERDISP_VERSION_MINOR,
    SERDISP_VERSION_GET_MAJOR(serdisp_getversioncode()), 
    SERDISP_VERSION_GET_MINOR(serdisp_getversioncode())
  );
  fprintf(f, "  (C) 2003-2010 by Wolfgang Astleitner\n");

  fprintf(f, "\n");

  if (iserr == -1) return;
  
  fprintf(f, "usage: testserdisp -n <display name> [<options>]\n\n");
  fprintf(f, "  Options: (default values in squared brackets)\n");
  fprintf(f, "    -n name          display name\n");
  fprintf(f, "    -p dev|port      output device or port\n");
  fprintf(f, "    -o options       options for driver, semicolon-separated key-value pairs\n");
  fprintf(f, "                     eg: -o \"WIRING=1;CONTRAST=2;BACKLIGHT=ON\"\n");
  fprintf(f, "    -d          [0]  debug level (0 .. no debugging, 2 .. max. debugging)\n");
  fprintf(f, "    -V               version information\n");
  fprintf(f, "            \n");
  fprintf(f, "  Examples: \n");
  fprintf(f, "    testserdisp -n PCD8544 -p \"/dev/parport0\"\n");
  fprintf(f, "    testserdisp -n PCD8544 -p \"0x378\"  # direct IO\n");
  fprintf(f, "    testserdisp -n PCD8544             # default parport device will be used\n");
  fprintf(f, "\n");
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
    fprintf(f, "    -h                  ... supported displays\n");
    fprintf(f, "    -n <displayname> -h ... display information (eg. supported wirings for <displayname>)\n\n");
  }
}



int main(int argc, char **argv) {
  int ch;
  char buffer[255];
  int done = 0;
  char sdcdev[51] = "";
  char dispname[51] = "";

  char* parameterstring = 0;

  int flag_measuretime = 0;

  long fgcolour = SD_COL_BLACK;  /* foreground colour (pen-colour) */
  long bgcolour = SD_COL_WHITE;  /* background colour */

  extern char *optarg;

  serdisp_CONN_t* sdcd;
  serdisp_t* dd = 0;

  sd_setdebuglevel(SD_LVL_WARN);

  while ((ch = getopt(argc, argv, "n:p:hd:o:V")) != -1) {
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
      case 'd':
        {
          int arg = atoi(optarg);
          if (arg >=0 && arg <= 2)
            sd_setdebuglevel(arg);
        }
      break;
      case 'p':
        sdtools_strncpy(sdcdev, optarg, 50);
      break;
      case 'o':
        if (parameterstring) free(parameterstring);
        parameterstring = (char*) sdtools_malloc(strlen(optarg)+1);
        sdtools_strncpy(parameterstring, optarg, strlen(optarg));
      break;
      case 'V':
        print_usage(-1);
        exit(0);
      break;
    }
  }

  if (strlen(dispname) < 1) {
    fprintf(stderr, "required option -n <display name> missing ...\n\n");
    print_usage(1);
    if (parameterstring) free(parameterstring);
    exit(1);
  }

  sdcd = SDCONN_open(sdcdev);

  if (sdcd == (serdisp_CONN_t*)0) {  
    fprintf(stderr, "Error: Unable to open %s, additional info: %s\n", sdcdev, sd_geterrormsg());
    if (parameterstring) free(parameterstring);
    exit (1);
  }

  dd = serdisp_init(sdcd, dispname, (parameterstring)?parameterstring:"");

  if (!dd) {
    fprintf(stderr, "Error: Unknown display or unable to open %s, additional info: %s\n", dispname, sd_geterrormsg());
    if (parameterstring) free(parameterstring);
    exit(1);
  }

  /* if OLED: default background is black and pen colour is white */
  if (SD_CS_ISSELFEMITTING(dd)) {
    fgcolour = SD_COL_WHITE;
    bgcolour = SD_COL_BLACK;
  }

  printf("\nenter 'help' to get help\n\n");

  serdisp_clear(dd);

  /* backlight should be set by default options or option string */
  /*serdisp_feature(dd, FEATURE_BACKLIGHT, FEATURE_YES);*/

  /* colour test pic if colour display */
  if (SD_CS_ISDIRECTCOLOUR(dd))
    draw_coltestpic(dd);
  else
    draw_testpic(dd, 0, fgcolour);

  while (!done) {
    struct timeval tv1, tv2;
    long diff_sec, diff_usec;

    printf("> ");
    fgets(buffer, 255, stdin);
    buffer[strlen(buffer)-1]=0;

    gettimeofday(&tv1, 0);
    sdcd->debug_count = 0;

    if (strncmp(buffer, "h", 1) == 0) {
        serdisp_options_t optiondesc;

        printf ("exit | quit                    exit this program\n");
        printf ("close                          exit this program, but don't shut down display\n");
        printf ("test                           draw a test picture\n");
        if (serdisp_getdepth(dd) >= 2) {
          printf ("ctest                          force a colour/greyscale test picture\n");
          printf ("bwtest                         force a black/white test picture\n");
          printf ("gshade                         draw a greyscale shade (grey levels: ascending from left to right)\n");
        }
        if (SD_CS_ISDIRECTCOLOUR(dd)) {
          printf ("rgbtest                        draw an RGB/CMYK testpattern\n");
          printf ("gradient                       draws a colour gradient with four bars W,R,G,B\n");
          printf ("gradient R|G|B                 draws a standard colour gradient with given main colour\n");
          printf ("gradient r|g|b                 draws a colour gradient (different algorithm)\n");
        }
        if (serdisp_getoptiondescription(dd, "BACKLIGHT", &optiondesc) && (optiondesc.flag & SD_OPTIONFLAG_RW)) {
          printf ("bg [1 | 0 | on | off]          switch on/off background light\n");
        }
        printf ("clear | cls                    clear display\n");
        printf ("redraw | re                    redraw display\n");
        printf ("update | upd                   update display\n");
        printf ("invert | inv                   toggle display inversion\n");
        printf ("rotate | rot                   rotate display 180 degrees\n");
        printf ("reset                          re-inits the display\n");
        printf ("fullreset                      resets device and re-inits the display\n");
        printf ("blink <x>                      blink x-times (if no x given: blink once)\n");
        if (serdisp_getoptiondescription(dd, "CONTRAST", &optiondesc) && (optiondesc.flag & SD_OPTIONFLAG_RW)) {
          printf ("contrast <x>, x=[0, 10]        contrast of display (0: lowest contrast, 10: highest)\n");
        }
        printf ("sp <W>x<H>                     set a pixel at WxH. eg: sp 1x1\n");
        printf ("cp <W>x<H>                     clear a pixel at WxH. eg: cp 1x1\n");
        printf ("gp <W>x<H>                     get pixel at WxH. eg: gp 1x1\n");
        printf ("p [<x>, 0 <= x <= 9]           draw a test pattern (p w/o digit for more info)\n");
        printf ("sl <x>                         shift display to left x times\n");
        printf ("sr <x>                         shift display to right x times\n");
        printf ("ts [1 | 0 | on | off]          measure duration of test patterns\n");
        printf ("i | info                       print supported options\n");
        printf ("i | info <option>              print detailed info about an option. eg: i ROTATE\n");
        if (SD_CS_ISDIRECTCOLOUR(dd)) {
          printf ("fg <col>                       set foreground colour (format: AARRGGBB). eg: fg FFFF0000\n");
        } else if (serdisp_getdepth(dd) > 1) {
          printf ("fg <col>                       set foreground colour (<col> ... colour index). eg: fg 2\n");
        }
/*        printf ("raw <x>, 0 <= x <= 255  send raw byte to the device (for hardware test only)\n");*/
    } else if ((strcmp(buffer,"exit") == 0) || (strcmp(buffer,"quit") == 0)) { 
      done = 1;
    } else if ((strcmp(buffer,"close") == 0)) { 
      done = 2;
    } else if(strcmp(buffer,"reset") == 0) {
      int rc = serdisp_reset(dd);
      printf("reset was %ssucessful\n", ((rc) ? "" : "un"));
    } else if(strcmp(buffer,"fullreset") == 0) {
      dd = serdisp_fullreset(dd);
      printf("fullreset was %ssucessful\n", ((dd) ? "" : "un"));
      if (!dd) done = 1;  /* quit */
    } else if(strncmp(buffer,"bg", 2) == 0) {
      int val;
      if (strlen(buffer) < 4) {
        printf ("bg=%s\n", (dd->curr_backlight == 1) ? "on" : "off");
      } else {
        val = (strcmp(buffer+3, "on") == 0 ||strcmp(buffer+3, "1") == 0 ) ? 1 :0;
        serdisp_setoption(dd, "BACKLIGHT", val);
      }
    } else if(strncmp(buffer,"ts", 2) == 0) {
      int val;
      if (strlen(buffer) < 4) {
        printf ("ts=%s\n", (flag_measuretime == 1) ? "on" : "off");
      } else {
        val = (strcmp(buffer+3, "on") == 0 ||strcmp(buffer+3, "1") == 0 ) ? 1 :0;
        flag_measuretime = val;
      }
    } else if(strcmp(buffer,"clear") == 0 || strcmp(buffer,"cls") == 0) {
      serdisp_clear(dd);
    } else if(strcmp(buffer,"redraw") == 0 || strcmp(buffer,"re") == 0) {
      serdisp_rewrite(dd);
    } else if(strcmp(buffer,"update") == 0 || strcmp(buffer,"upd") == 0) {
      serdisp_update(dd);
    } else if(strcmp(buffer,"invert") == 0 || strcmp(buffer,"inv") == 0) {
      serdisp_setoption(dd, "INVERT", SD_OPTION_TOGGLE);
    } else if(strncmp(buffer,"rot", 3) == 0) {
      char* idx = strchr(buffer, ' ');
      int val;

      if (idx)
        val = atoi(idx+sizeof(char));
      else 
        val = SD_OPTION_TOGGLE;

      serdisp_setoption(dd, "ROTATE", val);
    } else if(strncmp(buffer,"contrast", 8) == 0) {
      int temp;
      if (strlen(buffer) <= 9) {
        temp = MAX_CONTRASTSTEP / 2;
      } else {
        temp = atoi(strchr(buffer, ' ')+sizeof(char));
        serdisp_setoption(dd, "CONTRAST", temp);
      }
    } else if(strcmp(buffer,"test") == 0) {
      serdisp_clear(dd);
      /* colour test pic if colour display */
      if (SD_CS_ISDIRECTCOLOUR(dd))
        draw_coltestpic(dd);
      else
        draw_testpic(dd, 0, fgcolour);
    } else if(strcmp(buffer,"ctest") == 0) {
      serdisp_clear(dd);
      draw_coltestpic(dd);
    } else if(strcmp(buffer,"bwtest") == 0) {
      serdisp_clear(dd);
      draw_testpic(dd, 0, fgcolour);
    } else if(strcmp(buffer,"gshade") == 0) {
      int i, j;
      serdisp_clear(dd);
      for (i = 0; i < dd->width ; i++ )
        for (j = 0; j < dd->height; j++)
          serdisp_setgrey(dd, i, j, i);
      serdisp_update(dd);
    } else if(strcmp(buffer,"rgbtest") == 0) {
      draw_colrgbpic(dd);
      printf ("\n  RGB / CMYK test pattern:     \n\n");
      printf ("  -------------------------                   \n");
      printf ("  | R         |         G |      W .. white   \n");
      printf ("  |   -----------------   |      R .. red     \n");
      printf ("  |   |   |   Y   |   |   |      G .. green   \n");
      printf ("  |---| M |-------| C |---|      B .. blue    \n");
      printf ("  |   |   |   K   |   |   |      C .. cyan    \n");
      printf ("  |   -----------------   |      M .. magenta \n");
      printf ("  | B         |         W |      Y .. yellow  \n");
      printf ("  -------------------------      K .. black   \n\n");
      serdisp_update(dd);
    } else if(strncmp(buffer,"gradient", 8) == 0) {
      char* idx = strchr(buffer, ' ');
      char val;
      int type = 0;

      if (idx)
        val = idx[1];  /* no out-of-bound check because switch will catch wrong input and default to ' ' */
      else 
        val = ' ';

      switch (val) {
        case 'r': type = 1; break;
        case 'g': type = 2; break;
        case 'b': type = 3; break;
        case 'R': type = 4; break;
        case 'G': type = 5; break;
        case 'B': type = 6; break;
        default:  type = 0;
      }
      draw_gradient(dd, type);
      serdisp_update(dd);
    } else if(strncmp(buffer,"sp", 2) == 0 || strncmp(buffer,"cp", 2) == 0) {
      int px, py;
      char temp[255];
      long col;

      if (strlen(buffer) < 6) {
        printf("Syntax: sp/cp WxH\n");
      } else {
        sdtools_strncpy(temp,strchr(buffer, ' '), 255-1);
        px = atoi(temp);
        py = atoi(strchr(temp, 'x')+sizeof(char));

        if ( (px < 0) || (py < 0) ) {
          printf("-p: (x,y) not in range. Ignoring it.\n");
          px = 0; py = 0;
        }
        col = (strncmp(buffer, "s", 1) == 0) ? fgcolour : bgcolour;
        printf("%s pixel at: %d/%d\n", (col == fgcolour) ? "set" : "clear", px, py);
        serdisp_setcolour(dd, px, py, col);
        serdisp_update(dd);
      }
    } else if(strncmp(buffer,"gp ", 3) == 0) {
      int px, py;
      char temp[255];
      int val;

      if (strlen(buffer) < 6) {
        printf("Syntax: gp WxH\n");
      } else {
        sdtools_strncpy(temp,strchr(buffer, ' '), 255-1);
        px = atoi(temp);
        py = atoi(strchr(temp, 'x')+sizeof(char));

        if ( (px < 0) || (py < 0) ) {
          printf("-p: (x,y) not in range. Ignoring it.\n");
          px = 0; py = 0;
        }
        val = (strncmp(buffer, "s", 1) == 0) ? 1 :0;
        printf("pixel at (%d/%d): 0x%08x\n", px, py, (unsigned int)serdisp_getcolour(dd, px, py));
      }
    } else if(strncmp(buffer,"p", 1) == 0) {
      int px, py, pattern;

      if (strlen(buffer) <= 2) {
        printf ("patterns: \n");
        printf ("  p 0 ... test picture displaying 7seg.-style digits\n");
        printf ("  p 1 ... draw every 2nd pixel\n");
        printf ("  p 2 ... draw a grid (10 pixels wide)\n");
        printf ("  p 3 ... like 2, but only vertexes\n");
        printf ("  p 4 ... odd lines: draw odd pixels, even lines: draw even pixels\n");
        printf ("  p 5 ... draw diagonal lines (distance between lines: 10 pixels)\n");
        printf ("  p 6 ... like 5, but moves a sprite over diagonal lines\n");
        printf ("  p 7 ... speed test 1 (filling display from up and left)\n");
        printf ("  p 8 ... speed test 2 (running line from up and left)\n");
        printf ("  p 9 ... filled display, one not filled line in the middle (contrast test)\n");

        pattern = 0;
      } else {
        pattern = atoi(strchr(buffer, ' ')+sizeof(char));
      }

      serdisp_clear(dd);

      if ( (pattern < 0) || (pattern > 9)) {
        printf("pattern unknown, setting default pattern.\n");
        pattern = 0;
      }

      if (pattern == 0) { /* draw testpic (fill display with 7 seg digits */
        draw_testpic(dd, 1, fgcolour);
      } else if (pattern == 1) { /* draw every 2nd pixel */
        for (py = 0; py < serdisp_getheight(dd); py += 2)
          for (px = 0; px < serdisp_getwidth(dd); px += 2)
            serdisp_setcolour (dd, px, py, fgcolour);
        serdisp_update(dd);
      } else if (pattern == 2) { /* draw a grid (10 pixels wide */
        for (py = 0; py < serdisp_getheight(dd) ; py += 10)
          for (px = 0; px < serdisp_getwidth(dd); px ++)
            serdisp_setcolour (dd, px, py, fgcolour);
        for (px = 0; px < serdisp_getwidth(dd); px += 10)
          for (py = 0; py < serdisp_getheight(dd) ; py ++)
            serdisp_setcolour (dd, px, py, fgcolour);
        serdisp_update(dd);
      } else if (pattern == 3) { /* same as in 2, but only vertexes */
        for (py = 0; py < serdisp_getheight(dd) ; py += 10)
          for (px = 0; px < serdisp_getwidth(dd); px += 10)
            serdisp_setcolour (dd, px, py, fgcolour);
        serdisp_update(dd);
      } else if (pattern == 4) { /*odd lines: draw odd pixels, even lines: draw even pixels */
        for (py = 0; py < serdisp_getheight(dd); py ++)
          if (py % 2 == 0) {
            for (px = 0; px < serdisp_getwidth(dd); px += 2)
              serdisp_setcolour (dd, px, py, fgcolour);
          } else {
            for (px = serdisp_getwidth(dd) - 1 - ((serdisp_getwidth(dd) % 2 == 0) ? 0 : 1); px >= 0; px -= 2)
              serdisp_setcolour (dd, px, py, fgcolour);
          }
        serdisp_update(dd);
      } else if (pattern == 5) { /* draw diagonal lines (distance between lines: 10 pixels) */
        for (py = 0; py < serdisp_getheight(dd) ; py += 10)
          for (px = 0; px < serdisp_getwidth(dd); px ++)
            serdisp_setcolour (dd, px, py + (px%10), fgcolour);
        for (py = 0; py < serdisp_getheight(dd) ; py += 10)
          for (px = 0; px < serdisp_getwidth(dd); px ++)
            serdisp_setcolour (dd, px, py + 10 - (px%10), fgcolour);
        serdisp_update(dd);
      } else if (pattern == 6) { /* draw diagonal lines (distance between lines: 10 pixels) */
        int i, j, first=1, directionx=1, directiony=1, count = 0;
        int opx=0, opy=0;
        int spritedimx = serdisp_getwidth(dd) / 5;
        int spritedimy = serdisp_getheight(dd) / 5;

        /* draw 4 strips on colour displays (red, green, blue, background colour) */
        if (serdisp_getcolours(dd) >= 256) {
          int len = serdisp_getwidth(dd) / 4;

          for (py = 0; py < serdisp_getheight(dd) ; py ++)
            for (px = 0; px < len; px ++) {
              serdisp_setcolour (dd, px + 0*len, py, SD_COL_RED);
              serdisp_setcolour (dd, px + 1*len, py, SD_COL_GREEN);
              serdisp_setcolour (dd, px + 2*len, py, SD_COL_BLUE);
            }
        }

        /* draw diagonal lines */
        for (py = 0; py < serdisp_getheight(dd) ; py += 10)
          for (px = 0; px < serdisp_getwidth(dd); px ++)
            serdisp_setcolour (dd, px, py + (px%10), fgcolour);
        for (py = 0; py < serdisp_getheight(dd) ; py += 10)
          for (px = 0; px < serdisp_getwidth(dd); px ++)
            serdisp_setcolour (dd, px, py + 10 - (px%10), fgcolour);
        serdisp_update(dd);

        py = 0; px= spritedimx;
        while (count < 5) {
          if (!first) {
            for (j = 0; j < spritedimy; j++)
              for (i = 0; i < spritedimx; i++)
                serdisp_setcolour(
                  dd,
                  opx+i, opy+j,
                  serdisp_getcolour(dd, opx+i, opy+j) ^ 0x00FFFFFFL
                );
          } else
            first = 0;

          for (j = 0; j < spritedimy; j++)
            for (i = 0; i < spritedimx; i++)
             serdisp_setcolour(
               dd,
               px+i, py+j,
               serdisp_getcolour(dd, px+i, py+j) ^ 0x00FFFFFFL
             );

          serdisp_update(dd);

          opx = px; opy = py;

          if (directiony == 1) { /* move down */
            if (py+spritedimy >= serdisp_getheight(dd)) { /* bottom border touched: move up */
              directiony = 0;
              py--;
            } else 
              py++;
          } else {  /* move up */
            if (py < 1) { /* top border touched: move up */
              directiony = 1;
              py++;
            } else 
              py--;
          }

          if (directionx == 1) { /* move left */
            if (px+spritedimx >= serdisp_getwidth(dd)) { /* right border touched: move left */
              directionx = 0;
              px--;
              count++;
            } else 
              px++;
          } else {  /* move right */
            if (px < 1) { /* left border touched: move right */
              directionx = 1;
              px++;
              count++;
            } else 
              px--;
          }

          usleep(1000);
        }
      } else if (pattern == 7) {
        for (py = 0; py < serdisp_getheight(dd); py ++)
          for (px = 0; px < serdisp_getwidth(dd); px ++) {
            serdisp_setcolour(dd, px, py, fgcolour);
            serdisp_update(dd);
          }
        serdisp_clear(dd);
        for (px = 0; px < serdisp_getwidth(dd); px ++)
          for (py = 0; py < serdisp_getheight(dd); py ++) {
            serdisp_setcolour(dd, px, py, fgcolour);
            serdisp_update(dd);
          }
      } else if (pattern == 8) {
        for (px = 0; px < serdisp_getwidth(dd)+2; px ++)
          for (py = 0; py < serdisp_getheight(dd); py ++) {
            serdisp_setcolour(dd, px-2 , py, bgcolour);
            serdisp_setcolour(dd, px, py, fgcolour);
            serdisp_update(dd);
          }
        for (py = 0; py < serdisp_getheight(dd)+2; py ++)
          for (px = 0; px < serdisp_getwidth(dd); px ++) {
            serdisp_setcolour(dd, px, py-2, bgcolour);
            serdisp_setcolour(dd, px, py, fgcolour);
            serdisp_update(dd);
          }
      } else if (pattern == 9) {
        int i,j;
        for (j = 0; j < serdisp_getheight(dd); j ++)
          for (i = 0; i < serdisp_getwidth(dd); i ++)
            serdisp_setcolour(dd, i, j, fgcolour);

        for (i = 0; i < serdisp_getwidth(dd); i ++)
            serdisp_setcolour(dd, i, serdisp_getheight(dd)/2, bgcolour);

        serdisp_update(dd);
      }
    } else if(strncmp(buffer,"blink", 5) == 0) {
      int blink;

      if (strlen(buffer) <= 6) {
        blink = 1;
      } else {
        blink = atoi(strchr(buffer, ' ')+sizeof(char));
      }

      if (dd->feature_backlight)
        serdisp_blink(dd, 0, blink, 100);

      serdisp_blink(dd, 1, blink, 100);
    } else if(strncmp(buffer,"sl ", 3) == 0) {
      int cnt;

      if (strlen(buffer) <= 3) {
        cnt = 1;
      } else {
        cnt = atoi(strchr(buffer, ' ')+sizeof(char));
      }
      shift_left(dd, cnt);
    } else if(strncmp(buffer,"sr ", 3) == 0) {
      int cnt;

      if (strlen(buffer) <= 3) {
        cnt = 1;
      } else {
        cnt = atoi(strchr(buffer, ' ')+sizeof(char));
      }
      shift_right(dd, cnt);
    } else if(strncmp(buffer,"fg", 2) == 0) {
      if (strlen(buffer) <= 4) {
        printf("fg=0x%08lx\n", fgcolour);
      } else {
        char* idx = strchr(buffer, ' ');
        if (!idx) idx = strchr(buffer,'=');
        fgcolour = (long)strtoll(idx+sizeof(char), 0, 16);
      }
    } else if(strncmp(buffer,"raw ", 4) == 0) {
      int raw;

      if (strlen(buffer) <= 4) {
        raw = 0;
        printf("raw: not in range (0 >= x >= 255). Ignoring it.\n");
      } else {
        raw = atoi(strchr(buffer, ' ')+sizeof(char));
      }

      SDCONN_write(dd->sdcd, raw, 0);

    } else if(strncmp(buffer,"rawcmd ", 7) == 0) {
      int raw;

      if (strlen(buffer) <= 8) {
        raw = 0;
        printf("raw: not in range (0 >= x >= 255). Ignoring it.\n");
      } else {
        raw = (int)strtol(strchr(buffer, ' ')+sizeof(char), 0, 16);
      }

      serdisp_setoption(dd, "RAWCMD", (long)raw);

    } else if( (strlen(buffer) == 1 && strncmp(buffer,"i", 1) == 0) || 
               (strlen(buffer) == 4 && strncmp(buffer,"info", 4) == 0)  ||
               (strncmp(buffer,"i ", 2) == 0) || (strncmp(buffer,"info ", 5) == 0)
      ) {

      serdisp_options_t optiondesc;
      char* idx = strchr(buffer, ' ');
      if (idx)
        idx = sdtools_strlefttrim(idx);

      if (idx && strlen(idx) > 0) {
        if (serdisp_getoptiondescription(dd, idx, &optiondesc)) {
          printf("  %15s: %s\n", "name", optiondesc.name);
          if (optiondesc.aliasnames && strlen(optiondesc.aliasnames) > 0) 
            printf("  %15s: %s\n", "alias names", optiondesc.aliasnames);
          if (optiondesc.defines && strlen(optiondesc.defines) > 0)
            printf("  %15s: %s\n", "defines", optiondesc.defines);
          if (optiondesc.minval != -1) printf("  %15s: %ld\n", "min. value", optiondesc.minval);
          if (optiondesc.maxval != -1) printf("  %15s: %ld\n", "max. value", optiondesc.maxval);
          if (optiondesc.modulo != -1 && optiondesc.modulo != 1) printf("  %15s: %ld\n", "modulo", optiondesc.modulo);
          printf("  %15s: %s\n", "mode", (optiondesc.flag & SD_OPTIONFLAG_RW) ? "read/write" : "readonly");

          printf("  %15s: %ld\n", "current value", serdisp_getoption(dd, idx, 0));

        } else {
          printf("  *** option '%s' unknown or unsupported\n", idx);
        }
      } else {

        optiondesc.name = "";

        printf("display name: %s\n", serdisp_getdisplayname(dd));
        printf("options:\n");
        printf("  name            aliasnames                     mode\n");
        printf("  ---------------------------------------------------\n");
        while(serdisp_nextoptiondescription(dd, &optiondesc)) {
          printf("  %-15s %-30s  %2s\n", 
                 optiondesc.name, optiondesc.aliasnames, ((optiondesc.flag & SD_OPTIONFLAG_RW) ? "RW" : "RO")
          );
        }
      }
    } else {
      /* unknown command -> print message (except when empty input) */
      if (strlen(buffer) > 0)
        printf("Unknown command: %s\n", buffer);
    }

    gettimeofday(&tv2, 0);

    diff_sec = tv2.tv_sec - tv1.tv_sec;
    diff_usec = tv2.tv_usec - tv1.tv_usec;

    if (diff_usec < 0) {
      diff_usec = 1000000 + diff_usec;
      diff_sec --;
    }

    if (flag_measuretime) {
      double time_needed = (double)diff_sec * 1000000.0 + (double)diff_usec;
      if (sdcd->debug_count) {
        int debug_count = sdcd->debug_count;
        double time_per_write = (debug_count) ? (time_needed / (double)debug_count) : 0.0;
        printf("time needed: %2ds %03dms %03dus  write() calls: %10d   avg. time per write: %6.3fus\n", 
                             (int)(diff_sec), (int)(diff_usec/1000), (int)(diff_usec%1000), debug_count, time_per_write);
      } else {
        printf("time needed: %2ds %03dms %03dus\n", 
                             (int)(diff_sec), (int)(diff_usec/1000), (int)(diff_usec%1000));
      }
    }
  }

  if (dd) {
    if (done == 1)
      serdisp_quit(dd);
    else
      serdisp_close(dd);
  }

  if (parameterstring) free(parameterstring);
  return 0;
}

