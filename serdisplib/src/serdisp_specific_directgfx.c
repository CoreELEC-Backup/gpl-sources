/*
 *************************************************************************
 *
 * serdisp_specific_directgfx.c
 * routines for drawing content to SDL / GL / ... output
 *
 *************************************************************************
 *
 * copyright (C)  2008-2010  wolfgang astleitner
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
#include <sys/ioctl.h>

#include "../config.h"

#include "serdisplib/serdisp_connect.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_colour.h"

#include "serdisplib/serdisp_fctptr.h"


/* #define OPT_USEOLDUPDATEALGO */

/*
 * constants
 */

/* different display types/models supported by this driver */
#define DISPID_SDL   1


serdisp_options_t serdisp_directgfx_options[] = {
   /*  name          aliasnames     min    max mod int defines  */
   {  "DEPTH",       "COLS,COLOURS", 1,    32,  2, 0,  "64k=16,4096=12,4k=12,256=8"}
  ,{  "WIDTH",       "",             1,  1024,  1, 0,  ""}
  ,{  "HEIGHT",      "",             1,  1024,  1, 0,  ""}
/*  ,{  "FULLSCREEN",  "FS",           0,     1,  1, 0,  ""}*/
};


/* internal typedefs and functions */

static void serdisp_directgfx_init      (serdisp_t*);
static void serdisp_directgfx_update    (serdisp_t*);
static int  serdisp_directgfx_setoption (serdisp_t*, const char*, long);
static void serdisp_directgfx_close     (serdisp_t*);


typedef struct serdisp_directgfx_specific_s {
  int fullscreen;
  void* screen;
} serdisp_directgfx_specific_t;


static serdisp_directgfx_specific_t* serdisp_directgfx_internal_getStruct(serdisp_t* dd) {
  return (serdisp_directgfx_specific_t*)(dd->specific_data);
}


/* callback-function for setting non-standard options */
static void* serdisp_directgfx_getvalueptr (serdisp_t* dd, const char* optionname, int* typesize) {
  if (serdisp_compareoptionnames(dd, optionname, "DEPTH")) {
    *typesize = sizeof(byte);
    return &(dd->depth);
  } else if (serdisp_compareoptionnames(dd, optionname, "FULLSCREEN")) {
    *typesize = sizeof(int);
    return &(serdisp_directgfx_internal_getStruct(dd)->fullscreen);
  }
  return 0;
}



/* main functions */


/* *********************************
   serdisp_t* serdisp_directgfx_setup(sdcd, dispname, optionstring)
   *********************************
   sets up a display descriptor fitting to dispname and extra
   *********************************
   sdcd             ... output device handle
   dispname         ... display name (case-insensitive)
   optionstring     ... option string containing individual options
   *********************************
   returns a display descriptor
*/
serdisp_t* serdisp_directgfx_setup(const serdisp_CONN_t* sdcd, const char* dispname, const char* optionstring) {
  serdisp_t* dd;

  if ( ! SDFCTPTR_checkavail(SDFCTPTR_LIBSDL) ) {
    sd_error(SERDISP_ERUNTIME, "%s(): libSDL is not loaded.", __func__);
    return (serdisp_t*)0;
  }

  if (! (dd = (serdisp_t*)sdtools_malloc(sizeof(serdisp_t)) ) ) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate display descriptor", __func__);
    return (serdisp_t*)0;
  }
  memset(dd, 0, sizeof(serdisp_t));

  if (! (dd->specific_data = (void*) sdtools_malloc( sizeof(serdisp_directgfx_specific_t)) )) {
    free(dd);
    return (serdisp_t*)0;
  }
  memset(dd->specific_data, 0, sizeof(serdisp_directgfx_specific_t));

  /* output devices like SDL supported here */
  /* assign dd->dsp_id */
  if (serdisp_comparedispnames("SDL", dispname))
    dd->dsp_id = DISPID_SDL;
  else {  /* should not occur */
    sd_error(SERDISP_ENOTSUP, "display '%s' not supported by serdisp_specific_directgfx.c", dispname);
    return (serdisp_t*)0;
  }

  dd->sdcd = (serdisp_CONN_t*)sdcd;

  /* supported output devices */
  dd->connection_types  = SERDISPCONNTYPE_OUT;


  serdisp_directgfx_internal_getStruct(dd)->fullscreen = 1;

  dd->feature_contrast  = 0;
  dd->feature_backlight = 0;
  dd->feature_invert    = 0;

  /* per display settings */

  dd->width             = 320;
  dd->height            = 240;
  dd->depth             = 32;

  /* max. delta for optimised update algorithm */
  dd->optalgo_maxdelta  = 0;  /* unused, bbox is used for optimisation */

  dd->delay = 0;

  /* finally set some non display specific defaults */

  dd->curr_rotate       = 0;         /* unrotated display */
  dd->curr_invert       = 0;         /* display not inverted */

  dd->curr_backlight    = 0;         /* start with backlight on */

  /* supported colour spaces */
  dd->colour_spaces     = SD_CS_GREYSCALE | SD_CS_RGB332 | SD_CS_RGB444 | SD_CS_RGB565;
  dd->colour_spaces    |= SD_CS_TRUECOLOUR | SD_CS_ATRUECOLOUR;
  dd->colour_spaces    |= SD_CS_SELFEMITTING;

  dd->fp_init           = &serdisp_directgfx_init;
  dd->fp_update         = &serdisp_directgfx_update;
/*  dd->fp_clear          = &serdisp_directgfx_clear;*/
  dd->fp_close          = &serdisp_directgfx_close;
  dd->fp_setoption      = &serdisp_directgfx_setoption;
  dd->fp_getvalueptr    = &serdisp_directgfx_getvalueptr;

  serdisp_setupstructinfos(dd, 0, 0, serdisp_directgfx_options);

  /* parse and set options */
  if (serdisp_setupoptions(dd, dispname, optionstring) ) {
    serdisp_freeresources(dd);
    dd = 0;
    return (serdisp_t*)0;
  }

  return dd;
}



/* *********************************
   void serdisp_directgfx_init(dd)
   *********************************
   initialise
   *********************************
   dd     ... display descriptor
*/
void serdisp_directgfx_init(serdisp_t* dd) {
  char caption[40];
  SDL_Surface* screen;

  if (fp_SDL_Init(SDL_INIT_VIDEO) != 0) {
    sd_error(SERDISP_ENOTSUP, "%s(): unable to initialise SDL: %s", __func__, fp_SDL_GetError());
    sd_runtimeerror = 1;
    fp_SDL_Quit();
    return;
  }

  screen = serdisp_directgfx_internal_getStruct(dd)->screen = 
    (void*)fp_SDL_SetVideoMode(dd->width, dd->height, 0 /*sdl_depth*/, SDL_HWSURFACE | SDL_HWACCEL);

  if (screen == NULL) {
    sd_error(SERDISP_ENOTSUP, "%s(): unable to set video mode: %s", __func__, fp_SDL_GetError());
    sd_runtimeerror = 1;
    fp_SDL_Quit();
    return;
  }

  snprintf(caption, sizeof(caption)-1, "serdisplib::SDL %dx%dx%d", screen->w, screen->h, screen->format->BitsPerPixel);
  fp_SDL_WM_SetCaption(caption, NULL);

  sd_debug(2, "%s(): done with initialising", __func__);
}



/* *********************************
   void serdisp_directgfx_update(dd)
   *********************************
   updates the display using display-buffer scrbuf+scrbuf_chg
   *********************************
   dd     ... display descriptor
   *********************************
   the display is redrawn using a time-saving algorithm
*/
void serdisp_directgfx_update(serdisp_t* dd) {
  int x, y;

  SDL_Surface* screen = (SDL_Surface*)serdisp_directgfx_internal_getStruct(dd)->screen;
  uint8_t* bufp8;
  uint16_t* bufp16;
  uint32_t* bufp32;
  uint32_t colour;
  uint32_t colour_raw;
  byte ashift = screen->format->Ashift/8;
  byte rshift = screen->format->Rshift/8;
  byte gshift = screen->format->Gshift/8;
  byte bshift = screen->format->Bshift/8;

  if(dfn_SDL_MUSTLOCK( screen ) )
    fp_SDL_LockSurface( screen );

  if (screen->format->BitsPerPixel == dd->depth && dd->depth >= 24) {
    int idx;
    uint8_t* ddbuf = (uint8_t*)dd->scrbuf;
    bufp8 = (uint8_t*)screen->pixels;
    int xstep = (dd->depth == 24) ? 3 : 4;

    int idx_start = 0;
    int idx_end = dd->scrbuf_size;
#ifdef OPT_USEOLDUPDATEALGO
  /* unoptimised display update (slow. all pixels are redrawn) */
#else
    int idx_limit = 0;
    int idx_border = 0;

    if (dd->scrbuf_chg) {
      idx_start = dd->scrbuf_size;
      idx_end = 0;

      idx = 0;
      idx_border = 0;
      while (idx < dd->scrbuf_chg_size) {
        if (dd->scrbuf_chg[idx]) {
          idx_start = idx_border;
          idx_limit = idx;
          idx = dd->scrbuf_chg_size;
        } else {
          idx++;
          idx_border += xstep << 3;
        }
      }
      idx = dd->scrbuf_chg_size - 1;
      idx_border = (idx << 3) * xstep + (xstep << 3) - 1;
      while (idx >= 0 && idx > idx_limit) {
        if (dd->scrbuf_chg[idx]) {
          idx_end = idx_border;
          idx = -1;
        } else {
          idx--;
          idx_border -= xstep << 3;
        }
      }
    }
/*    fprintf(stderr, "idx_start: %d   idx_end: %d\n", idx_start, idx_end);*/
#endif /* OPT_USEOLDUPDATEALGO */
    if (dd->depth == 24) {
      for (idx = idx_start; idx < idx_end; idx += xstep) {
         *(bufp8 + idx + rshift) = *(ddbuf + idx);
         *(bufp8 + idx + gshift) = *(ddbuf + idx + 1);
         *(bufp8 + idx + bshift) = *(ddbuf + idx + 2);

         if (dd->curr_invert) {
           *(bufp8 + idx + rshift) = ~ *(bufp8 + idx + rshift);
           *(bufp8 + idx + gshift) = ~ *(bufp8 + idx + gshift);
           *(bufp8 + idx + bshift) = ~ *(bufp8 + idx + bshift);
         }
      }
    } else {
      for (idx = idx_start; idx < idx_end; idx += xstep) {
         *(bufp8 + idx + ashift) = *(ddbuf + idx);
         *(bufp8 + idx + rshift) = *(ddbuf + idx + 1);
         *(bufp8 + idx + gshift) = *(ddbuf + idx + 2);
         *(bufp8 + idx + bshift) = *(ddbuf + idx + 3);

         if (dd->curr_invert) {
           *(bufp8 + idx + rshift) = ~ *(bufp8 + idx + rshift);
           *(bufp8 + idx + gshift) = ~ *(bufp8 + idx + gshift);
           *(bufp8 + idx + bshift) = ~ *(bufp8 + idx + bshift);
         }
      }
    }
  } else {
    /* unset curr_rotate so that it will not interfere with serdisp_setsdcol() */
    int temp_rotate = dd->curr_rotate;
    dd->curr_rotate = 0;

    for (y = 0; y < dd->height; y++) {
      for (x = 0; x < dd->width; x++) {
#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
        colour_raw = serdisp_getsdcol(dd, x, y);
#else
        colour_raw = (uint32_t)serdisp_getcolour(dd, x, y);
#endif

        if (dd->curr_invert)
          colour_raw = (colour_raw & 0xFF000000) | ~(colour_raw & 0x00FFFFFF);

        colour = fp_SDL_MapRGB(
          screen->format,
          (colour_raw & 0x00FF0000) >> 16,
          (colour_raw & 0x0000FF00) >>  8,
          (colour_raw & 0x000000FF)
        );

        switch(screen->format->BytesPerPixel) {
          case 1: {
            bufp8 = (uint8_t*)screen->pixels + y * screen->pitch + x;
            *bufp8 = (uint8_t)colour;
          }
          break;
          case 2: {
            bufp16 = (uint16_t*)screen->pixels + y * screen->pitch/2 + x;
            *bufp16 = (uint16_t)colour;
          }
          break;
          case 3: {
            bufp8 = (uint8_t*)screen->pixels + y * screen->pitch + x * 3;
            *(bufp8 + rshift) = (colour_raw & 0x00FF0000) >> 16;
            *(bufp8 + gshift) = (colour_raw & 0x0000FF00) >>  8;
            *(bufp8 + bshift) = (colour_raw & 0x000000FF);
          }
          break;
          case 4: {
            bufp32 = (uint32_t*)screen->pixels + y * screen->pitch/4 + x;
            *bufp32 = (uint32_t)colour;
          }
          break;
        }
      }
    }
    /* re-enable curr_rotate */
    dd->curr_rotate = temp_rotate;
  }

#ifndef OPT_USEOLDUPDATEALGO
  memset(dd->scrbuf_chg, 0x00, dd->scrbuf_chg_size);
#endif

  if(dfn_SDL_MUSTLOCK( screen ) )
    fp_SDL_UnlockSurface( screen );

  fp_SDL_Flip(screen);
}



/* *********************************
   int serdisp_directgfx_setoption(dd, option, value)
   *********************************
   change a display option
   *********************************
   dd      ... display descriptor
   option  ... name of option to change
   value   ... value for option
*/
int serdisp_directgfx_setoption(serdisp_t* dd, const char* option, long value) {
#if 0
  if (dd->feature_contrast && serdisp_compareoptionnames(dd, option, "CONTRAST")) {
    dd->curr_contrast = sdtools_contrast_norm2hw(dd, (int)value);
  } else {
#endif
    /* option not found here: try generic one in calling serdisp_setoption(); */
    return 0;
#if 0
  }
  return 1;
#endif
}


/* *********************************
   void serdisp_directgfx_close(dd)
   *********************************
   close (switch off) display
   *********************************
   dd     ... display descriptor
*/
void serdisp_directgfx_close(serdisp_t* dd) {
  fp_SDL_FreeSurface((SDL_Surface*)(serdisp_directgfx_internal_getStruct(dd)->screen));
  fp_SDL_Quit();
}
