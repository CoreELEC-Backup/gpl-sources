/*
 *************************************************************************
 *
 * image.c
 * reads and stores images (single and animated)
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

#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <strings.h>

#include "image.h"
#include "serdisplib/serdisp_colour.h"
#include "serdisplib/serdisp_tools.h"

/* internal typedef and functions */
static int sdtim_readGLCDheader (FILE* fin, int* width, int* height, int* frames, long* delay);
static gdImagePtr sdtim_readGLCDnextframe (FILE* fin, int width, int height);
static void dithergrey(serdisp_t* dd, gdImagePtr im_in, gdImagePtr im_out, int colours, int sx, int sy, int w, int h);
static int getGreyValue(gdImagePtr im, int x, int y);


/* *********************************
   sdt_image_t* sdtim_readGLCDheader (fin)
   *********************************
   read a GLCD-image
   *********************************
   fin    ... file handle to GLCD image
   *********************************
   returns a serdisp-tools image descriptor or 0 if GLCD image could not be read
*/
int sdtim_readGLCDheader (FILE* fin, int* width, int* height, int* frames, long* delay) {
  int rv;
  int hdrsize = 1*4 + 2*2;
  byte buffer[hdrsize];

  rv = fread( buffer, 1, hdrsize, fin);

  if (rv < hdrsize) {
    sd_error(SERDISP_ERUNTIME, "%s(): invalid image (%d bytes read but at least %d required)", __func__, rv, hdrsize);
    return 0;
  }

  if (strncmp((const char*)buffer, "GLC", sizeof(char)*3) != 0 ) {
    sd_error(SERDISP_ERUNTIME, "%s(): unable to find image signature", __func__);
    return 0;
  }

  *width =  (buffer[5]<<8) + buffer[4];
  *height = (buffer[7]<<8) + buffer[6];

  *delay = 250;

  switch(buffer[3]) {
    case 'D': /* single image */
      *frames = 1;
      break;
    case 'A': /* animated image */
      hdrsize = 2 + 4;
      rv = fread(buffer, 1, hdrsize, fin);

      if (rv < hdrsize) {
        sd_error(SERDISP_ERUNTIME, "%s(): invalid image (rv < hdrsize)", __func__);
        return 0;
      }

      *frames = (buffer[1]<<8) + buffer[0];
      *delay = (buffer[5]<<24) + (buffer[4]<<16) + (buffer[3]<<8) + buffer[2];

      break;
    default:
      sd_error(SERDISP_ERUNTIME, "%s(): invalid image (signature GLCD or GLCA not found)", __func__);
      return 0;
  }

  sd_debug(2, "GLCD image: w/h: %d/%d  frames: %d  delay: %ld\n", *width, *height, *frames, *delay);

  return 1;
}



gdImagePtr sdtim_readGLCDnextframe (FILE* fin, int width, int height) {
  byte* buffer = 0;
  int rv, x, y;
  gdImagePtr im = 0;
  int black, white;

  if (! (buffer = (byte*)sdtools_malloc( height * ((width + 7)/8)) ) ) {
    sd_error(SERDISP_EMALLOC, "%s(): cannot allocate memory for buffer", __func__);
    return 0;
  }
  memset(buffer, 0, height * ((width + 7)/8));

  rv = fread ( buffer, height * ((width + 7)/8), 1, fin);

  im = gdImageCreate(width, height);
  black = gdImageColorAllocate(im, 0,0,0);
  white = gdImageColorAllocate(im, 255,255,255);

  for (y = 0; y < height; y++) {
    for (x = 0; x < width ; x++) {
      gdImageSetPixel(im, x, y,  (( buffer[ ((width+7)/8) * y + x/8  ] & (1 << (7- (x%8))) ) ? black : white ));
    }
  }
  free(buffer);
  return im;
}



void dithergrey(serdisp_t* dd, gdImagePtr im_in, gdImagePtr im_out, int colours, int sx, int sy, int w, int h) {
  int x, y;
  int xslop, dslop;
  int yslop[w];
  int i, j, k, t, q;
  int range = 255;

  t = ((range+1) * 2) / colours;  /* threshhold factor */
  q = range / (colours-1);        /* quantisation factor */
  j = (9 * range) / 32;
  for (x = 0; x < w; x++)
    yslop[x] = j;

  for (y = 0; y < h; y++) {
    xslop = (7 * range) / 32;
    dslop = range / 32;

    for (x = 0; x < w; x++) {
      i = getGreyValue(im_in, x+sx, y+sy);
      i += xslop + yslop[x];
      j = (i / t) *q;
      if (j > range) j = range;

#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
      j = serdisp_lookupsdgrey(dd, serdisp_transsdgrey(dd, j));
#else
      j = serdisp_lookupgrey(dd, serdisp_transgrey(dd, j));
#endif
      gdImageSetPixel(im_out, x+sx, y+sy, gdImageColorClosest(im_out, j, j, j));

      i = i - j; /* error i - j */
      k = (i >> 4);
      xslop = 7 * k;
      yslop[x] = (5*k) + dslop;
      if (x > 0)
        yslop[x-1] += 3 * k;

      dslop = i - (15 * k);
    }
  }
}


sdtim_frame_t* sdtim_loadimage (char* imagefile, int* frames, serdisp_t* dd, gdImagePtr template, int scalealgo, int flag_truecolour, int flag_dither) {
  sdtim_frame_t* firstframe = 0;
  sdtim_frame_t* prevframe = 0;
  FILE* fin; 
  int valid = 1;
  long currdelay = 0;

  gdImagePtr im_file = 0, im_scaled;

  int orig_w, orig_h, scaled_w, scaled_h, start_x, start_y;

  /* normalised width and height. width = 100, height is calculated using pixel aspect ratio and pixel geometry */
  int aspect_w, aspect_h;

  double fact_w = 1.0, fact_h = 1.0, fact;

  int flag_imagetype = 0;   /* 0: gd-processed image, 1: glcd */
  int f;

  fin = fopen(imagefile, "rb");
  if (!fin) {
    sd_error(SERDISP_ERUNTIME, "%s(): image file %s cannot be opened", __func__, imagefile);
    return 0;
  }

  sd_debug(2, "%s\n", strrchr(imagefile, '.'));
  if (strcasecmp(strrchr(imagefile, '.'), ".png") == 0) {
    im_file = gdImageCreateFromPng(fin);
    orig_w = gdImageSX(im_file); orig_h = gdImageSY(im_file);
    *frames = 1;
#if HAVE_GDIMAGECREATEFROMGIF
  } else if (strcasecmp(strrchr(imagefile, '.'), ".gif") == 0) {
    sd_debug(2, "GIF\n");
    im_file = gdImageCreateFromGif(fin);
    orig_w = gdImageSX(im_file); orig_h = gdImageSY(im_file);
    *frames = 1;
#endif
  } else if (strncasecmp(strrchr(imagefile, '.'), ".jp", 3) == 0) {
    sd_debug(2, "JPEG\n");
    im_file = gdImageCreateFromJpeg(fin);
    orig_w = gdImageSX(im_file); orig_h = gdImageSY(im_file);
    *frames = 1;
  } else if (strcasecmp(strrchr(imagefile, '.'), ".glcd") == 0) {
    sd_debug(2, "GLCD\n");
    sdtim_readGLCDheader(fin, &orig_w, &orig_h, frames, &currdelay);
    flag_imagetype = 1;
  }

  sd_debug(2, "orig_w/h: %d/%d   w/h: %d/%d\n", orig_w, orig_h, serdisp_getwidth(dd), serdisp_getheight(dd));
  sd_debug(2, "pixelaspect: %d%%\n", serdisp_getpixelaspect(dd));


  /*      100 * w * ph     w, h: amount of pixels
      f = -------------  pw, ph: display area (normalised: px == 100)
                h * pw        f: pixel aspect ratio in percent

      ==>    / pw is normalised to 100, we search ph:

           f * h * pw
      ph = ------------  =     / pw == 100 
            100 * w

             f * h
         = --------
               w

  */

  aspect_w = 100;
  aspect_h = (serdisp_getpixelaspect(dd) * serdisp_getheight(dd)) / serdisp_getwidth(dd);


  fact_w = (double)(orig_w) / (double) aspect_w;
  fact_h = (double)(orig_h) / (double) aspect_h;

  fact = (fact_w > fact_h) ? fact_w : fact_h;

  switch(scalealgo) {
    case 0:
      /*scaled_w = gdImageSX(im_file);
        scaled_h = (gdImageSY(im_file) * serdisp_getpixelaspect(dd)) / 100;
        start_x = ( scaled_w < serdisp_getwidth(dd) ) ? ((serdisp_getwidth(dd) - scaled_w) >> 1) : 0;
        start_y = ( scaled_h < serdisp_getheight(dd) ) ? ((serdisp_getheight(dd) - scaled_h) >> 1) : 0;
      */
      scaled_w = orig_w;
      scaled_h = orig_h;
      start_x = ( scaled_w < serdisp_getwidth(dd) ) ? ((serdisp_getwidth(dd) - scaled_w) >> 1) : 0;
      start_y = ( scaled_h < serdisp_getheight(dd) ) ? ((serdisp_getheight(dd) - scaled_h) >> 1) : 0;
      break;
    case 1:
    case 2:

      sd_debug(2, "sd_w: %d, sd_h: %d, asp_w: %d, asp_h: %d; fact_w: %f; fact_h: %f; fact: %f\n", serdisp_getwidth(dd), serdisp_getheight(dd), aspect_w, aspect_h, fact_w, fact_h, fact);

      scaled_w = orig_w;
      scaled_h = (orig_h * serdisp_getpixelaspect(dd)) / 100;

      if (scalealgo == 2 || scaled_w > serdisp_getwidth(dd) || scaled_h > serdisp_getheight(dd)) {
        scaled_w = (int)( ((double)(orig_w) / fact) * ( (double)(serdisp_getwidth(dd)) / (double)aspect_w)  );
        scaled_h = (int)( ((double)(orig_h) / fact) * ( (double)(serdisp_getheight(dd)) / (double)aspect_h)  );

        /* clip potential rounding errors */
        if (scaled_w > serdisp_getwidth(dd)) scaled_w = serdisp_getwidth(dd);
        if (scaled_h > serdisp_getheight(dd)) scaled_h = serdisp_getheight(dd);

      }
      start_x = (serdisp_getwidth(dd) - scaled_w) >> 1;
      start_y = (serdisp_getheight(dd)- scaled_h) >> 1;
      break;
    default: /* case 3 */
      scaled_w = serdisp_getwidth(dd);
      scaled_h = serdisp_getheight(dd);
      start_x = 0;
      start_y = 0;
      break;
  }

  sd_debug(2, "scaled_w/h: %d/%d, start_x/y: %d/%d\n", scaled_w, scaled_h, start_x, start_y);


  if (flag_truecolour)
    im_scaled = gdImageCreateTrueColor(serdisp_getwidth(dd), serdisp_getheight(dd));
  else    
    im_scaled = gdImageCreate(serdisp_getwidth(dd), serdisp_getheight(dd));

  f = 0;
  while (valid && (f < *frames)) {
    sdtim_frame_t* currframe;

    if (! (currframe = (sdtim_frame_t*)sdtools_malloc( sizeof(sdtim_frame_t)) ) ) {
      sd_error(SERDISP_EMALLOC, "sdtim_loadimage(): cannot allocate another frame descriptor");
      valid = 0;
      break;
    }
    memset(currframe, 0, sizeof(sdtim_frame_t));

    if (prevframe)
      prevframe->next = currframe;
    else 
      firstframe = currframe;

    if (flag_truecolour)
      currframe->image = gdImageCreateTrueColor(serdisp_getwidth(dd), serdisp_getheight(dd));
    else  {
      currframe->image = gdImageCreate(serdisp_getwidth(dd), serdisp_getheight(dd));
      gdImagePaletteCopy(currframe->image, template);
    }
    /* copy background filling from template */
    gdImageCopy(
      currframe->image, template, 
      0,0, 
      0,0,
      gdImageSX(currframe->image), gdImageSY(currframe->image)
    );

    currframe->delay = currdelay;

    if (flag_imagetype == 0) { /* gd-processed single image */
      gdImageCopyResized(
        im_scaled, im_file, 
        start_x, start_y, 
        0,0,
        scaled_w, scaled_h,
        gdImageSX(im_file), gdImageSY(im_file)
      );

      /* file image no longer needed => free its space */
      gdImageDestroy(im_file);
    } else {  /* serdisplib-processed multiframe image */
      gdImagePtr im_frame = sdtim_readGLCDnextframe (fin, orig_w, orig_h);
      gdImageCopyResized(
        im_scaled, im_frame, 
        start_x, start_y, 
        0,0,
        scaled_w, scaled_h,
        gdImageSX(im_frame), gdImageSY(im_frame)
      );

      gdImageDestroy(im_frame);
    }


    if (flag_dither && gdImageColorsTotal(im_scaled) > 2) {
      sd_debug(2, "dithering frame %d\n", f);
      dithergrey(dd, im_scaled, currframe->image, serdisp_getcolours(dd), start_x, start_y, scaled_w, scaled_h);
    } else {
      if (flag_truecolour) {
        sd_debug(2, "direct copying frame %d (truecolour)\n", f);
        gdImageCopy(
          currframe->image, im_scaled, 
          0, 0, 
          0,0,
          gdImageSX(im_scaled), gdImageSY(im_scaled)
        );
      } else {
        int grey;
        int i,j;

        sd_debug(2, "pixel (colour->grey) copying frame %d (w/o dithering)\n", f);

        for (j = 0; j < gdImageSY(im_scaled); j++)
          for (i = 0; i < gdImageSX(im_scaled); i++) {
#ifdef SD_SUPP_ARCHINDEP_SDCOL_FUNCTIONS
            grey = serdisp_lookupsdgrey(dd, serdisp_transsdgrey(dd, getGreyValue(im_scaled, i, j)));
#else
            grey = serdisp_lookupgrey(dd, serdisp_transgrey(dd, getGreyValue(im_scaled, i, j)));
#endif
            gdImageSetPixel(currframe->image, i, j, gdImageColorClosest(currframe->image, grey, grey, grey));
          }
      }
    }

    if (!flag_truecolour) {
      int i;
      sd_debug(2, "frame[%d] (picture): w/h: %d/%d  totalcolours: %d\n", f, gdImageSX(currframe->image), gdImageSY(currframe->image), gdImageColorsTotal(currframe->image));
      for (i = 0; i < gdImageColorsTotal(currframe->image); i++) {
        sd_debug(2, " col[%d] = %02x %02x %02x\n", i, gdImageRed(currframe->image, i), gdImageGreen(currframe->image, i), gdImageBlue(currframe->image, i));
      }
    }

    prevframe = currframe;
    f++;
  } /* while f */
  gdImageDestroy(im_scaled);

  fclose(fin);

  if (!valid) {
    sdtim_frame_t* currframe = firstframe;
    while(currframe) {
      sdtim_frame_t* nextframe = currframe->next;
      if (currframe->image)
        gdImageDestroy(currframe->image);
      free(currframe);
      currframe = nextframe;
    }

    return (sdtim_frame_t*)0;
  }

  return firstframe;
}


/* grey = .3 * r + .59 * g + .11 * b */
/* enhancement:  use (77*r + 150*g + 28*b) / 255 for better accuracy (thanks to michael reinelt for pointing this out)*/
int getGreyValue(gdImagePtr im, int x, int y) {
  int c = gdImageGetPixel(im, x, y);

  return ( (77 * gdImageRed(im, c) + 150 * gdImageGreen(im, c) + 28 * gdImageBlue(im, c)) / 255);
}
