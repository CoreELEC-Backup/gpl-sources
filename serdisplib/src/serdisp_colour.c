/** \file    serdisp_colour.c
  *
  * \brief   Colour and colour space specific functions
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

#include "serdisplib/serdisp_control.h"
#include "serdisplib/serdisp_colour.h"

/* internal functions */
static long     serdisp_transcolour_bw      (serdisp_t* dd, long colour);
static long     serdisp_transgrey_bw        (serdisp_t* dd, byte greyvalue);
static long     serdisp_lookupcolour_bw     (serdisp_t* dd, long colour);
static byte     serdisp_lookupgrey_bw       (serdisp_t* dd, long colour);

static long     serdisp_transcolour_grey2_4 (serdisp_t* dd, long colour);
static long     serdisp_transgrey_grey2_4   (serdisp_t* dd, byte greyvalue);
static long     serdisp_lookupcolour_grey2_4(serdisp_t* dd, long colour);
static byte     serdisp_lookupgrey_grey2_4  (serdisp_t* dd, long colour);

static long     serdisp_transcolour_grey8   (serdisp_t* dd, long colour);
static long     serdisp_transgrey_grey8     (serdisp_t* dd, byte greyvalue);
static long     serdisp_lookupcolour_grey8  (serdisp_t* dd, long colour);
static byte     serdisp_lookupgrey_grey8    (serdisp_t* dd, long colour);

static long     serdisp_transcolour_rgb332  (serdisp_t* dd, long colour);
static long     serdisp_transgrey_rgb332    (serdisp_t* dd, byte greyvalue);
static long     serdisp_lookupcolour_rgb332 (serdisp_t* dd, long colour);
static byte     serdisp_lookupgrey_rgb332   (serdisp_t* dd, long colour);

static long     serdisp_transcolour_bgr332  (serdisp_t* dd, long colour);
static long     serdisp_transgrey_bgr332    (serdisp_t* dd, byte greyvalue);
static long     serdisp_lookupcolour_bgr332 (serdisp_t* dd, long colour);
static byte     serdisp_lookupgrey_bgr332   (serdisp_t* dd, long colour);

static long     serdisp_transcolour_rgb444  (serdisp_t* dd, long colour);
static long     serdisp_transgrey_rgb444    (serdisp_t* dd, byte greyvalue);
static long     serdisp_lookupcolour_rgb444 (serdisp_t* dd, long colour);
static byte     serdisp_lookupgrey_rgb444   (serdisp_t* dd, long colour);

static long     serdisp_transcolour_rgb565  (serdisp_t* dd, long colour);
static long     serdisp_transgrey_rgb565    (serdisp_t* dd, byte greyvalue);
static long     serdisp_lookupcolour_rgb565 (serdisp_t* dd, long colour);
static byte     serdisp_lookupgrey_rgb565   (serdisp_t* dd, long colour);

static long     serdisp_transcolour_bgr565  (serdisp_t* dd, long colour);
static long     serdisp_transgrey_bgr565    (serdisp_t* dd, byte greyvalue);
static long     serdisp_lookupcolour_bgr565 (serdisp_t* dd, long colour);
static byte     serdisp_lookupgrey_bgr565   (serdisp_t* dd, long colour);

static long     serdisp_transcolour_rgb666  (serdisp_t* dd, long colour);
static long     serdisp_transgrey_rgb666    (serdisp_t* dd, byte greyvalue);
static long     serdisp_lookupcolour_rgb666 (serdisp_t* dd, long colour);
static byte     serdisp_lookupgrey_rgb666   (serdisp_t* dd, long colour);

static long     serdisp_transcolour_rgb888  (serdisp_t* dd, long colour);
static long     serdisp_transgrey_rgb888    (serdisp_t* dd, byte greyvalue);
static long     serdisp_lookupcolour_rgb888 (serdisp_t* dd, long colour);
static byte     serdisp_lookupgrey_rgb888   (serdisp_t* dd, long colour);

static long     serdisp_transcolour_rgba888 (serdisp_t* dd, long colour);
static long     serdisp_transgrey_rgba888   (serdisp_t* dd, byte greyvalue);
static long     serdisp_lookupcolour_rgba888(serdisp_t* dd, long colour);
static byte     serdisp_lookupgrey_rgba888  (serdisp_t* dd, long colour);

static long     serdisp_colourdistance      (long col1, long col2);


/**
  * \brief   set a colour in the display buffer
  *
  * This function sets the colour information <em>hardware independendly</em>.\n
  * The information is changed in the <em>display buffer</em> only.
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  * \param   colour        colour \n
  *                        format: \p 0xAARRGGBB , \p AA .. alpha, \p RR .. red, \p GG .. green, \p BB .. blue
  *
  * \b Examples: \n
  * both set a red pixel in the display buffer at position \p (10/20)
  * \code
  * serdisp_setcolour(dd, 10, 20, 0xFFFF0000); 
  * serdisp_setcolour(dd, 10, 20, SD_COL_RED); 
  * \endcode
  */
void serdisp_setcolour(serdisp_t* dd, int x, int y, long colour) {
  serdisp_setpixel(dd, x, y, serdisp_transcolour(dd, colour));
}


/**
  * \brief   set grey value in the display buffer
  *
  * This function sets the grey value <em>hardware independendly</em>.\n
  * The information is changed in the <em>display buffer</em> only.
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  * \param   grey          grey value; format: <tt>[0 .. 255]</tt>
  *
  * \b Examples: \n
  * set a grey value in the display buffer at position \p (10/20)
  * \code
  * serdisp_setgrey(dd, 10, 20, 0xCC); 
  * \endcode
  */
void serdisp_setgrey(serdisp_t* dd, int x, int y, byte grey) {
  serdisp_setpixel(dd, x, y, serdisp_transgrey(dd, grey));
}


/**
  * \brief   gets colour value at position (x/y)
  *
  * Gets the <em>hardware independend</em> colour value at position (x/y)
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  *
  * \return hardware independend colour value at (x/y) \n
  *         format: <tt>0xAARRGGBB</tt>, \p AA .. alpha, \p RR .. red, \p GG .. green, \p BB .. blue
  */
long serdisp_getcolour(serdisp_t* dd, int x, int y) {
  return serdisp_lookupcolour(dd, serdisp_getpixel(dd, x, y));
}


/**
  * \brief   gets grey value at position (x/y)
  *
  * Gets the grey value at position (x/y). colour values are converted to the corresponding greyscale values.
  *
  * \param   dd            device descriptor
  * \param   x             x-position
  * \param   y             y-position
  *
  * \return grey value ,format: <tt>[0 .. 255]</tt>
  */
byte serdisp_getgrey(serdisp_t* dd, int x, int y) {
  return serdisp_lookupgrey(dd, serdisp_getpixel(dd, x, y));
}


/**
  * \brief   initialises colour conversion function pointers
  *
  * \param   dd            device descriptor
  *
  * \retval   0            no error occured
  * \retval  -1            depth/colour-space combination not supported
  *
  * \since 1.97.9
  */
int serdisp_sdcol_init (serdisp_t* dd) {
  switch (serdisp_getdepth(dd)) {
    case 1:
      dd->fp_transcolour   = serdisp_transcolour_bw;
      dd->fp_transgrey     = serdisp_transgrey_bw;
      dd->fp_lookupcolour  = serdisp_lookupcolour_bw;
      dd->fp_lookupgrey    = serdisp_lookupgrey_bw;
      break;
    case 2:
    case 4:
      dd->fp_transcolour   = serdisp_transcolour_grey2_4;
      dd->fp_transgrey     = serdisp_transgrey_grey2_4;
      dd->fp_lookupcolour  = serdisp_lookupcolour_grey2_4;
      dd->fp_lookupgrey    = serdisp_lookupgrey_grey2_4;
      break;
    case 8:
      if (dd->colour_spaces & SD_CS_RGB332) {
        if (SD_CS_ISBGR(dd)) {
          dd->fp_transcolour   = serdisp_transcolour_bgr332;
          dd->fp_transgrey     = serdisp_transgrey_bgr332;
          dd->fp_lookupcolour  = serdisp_lookupcolour_bgr332;
          dd->fp_lookupgrey    = serdisp_lookupgrey_bgr332;
        } else {
          dd->fp_transcolour   = serdisp_transcolour_rgb332;
          dd->fp_transgrey     = serdisp_transgrey_rgb332;
          dd->fp_lookupcolour  = serdisp_lookupcolour_rgb332;
          dd->fp_lookupgrey    = serdisp_lookupgrey_rgb332;
        }
      } else {
        dd->fp_transcolour   = serdisp_transcolour_grey8;
        dd->fp_transgrey     = serdisp_transgrey_grey8;
        dd->fp_lookupcolour  = serdisp_lookupcolour_grey8;
        dd->fp_lookupgrey    = serdisp_lookupgrey_grey8;
      }
      break;
    case 12:
      if (SD_CS_ISBGR(dd))
        return -1;
      dd->fp_transcolour   = serdisp_transcolour_rgb444;
      dd->fp_transgrey     = serdisp_transgrey_rgb444;
      dd->fp_lookupcolour  = serdisp_lookupcolour_rgb444;
      dd->fp_lookupgrey    = serdisp_lookupgrey_rgb444;
      break;
    case 16:
      if (dd->colour_spaces & SD_CS_RGB565) {
        if (SD_CS_ISBGR(dd)) {
          dd->fp_transcolour   = serdisp_transcolour_bgr565;
          dd->fp_transgrey     = serdisp_transgrey_bgr565;
          dd->fp_lookupcolour  = serdisp_lookupcolour_bgr565;
          dd->fp_lookupgrey    = serdisp_lookupgrey_bgr565;
        } else {
          dd->fp_transcolour   = serdisp_transcolour_rgb565;
          dd->fp_transgrey     = serdisp_transgrey_rgb565;
          dd->fp_lookupcolour  = serdisp_lookupcolour_rgb565;
          dd->fp_lookupgrey    = serdisp_lookupgrey_rgb565;
        }
      }
      break;
    case 18:
      if (SD_CS_ISBGR(dd))
        return -1;
      dd->fp_transcolour   = serdisp_transcolour_rgb666;
      dd->fp_transgrey     = serdisp_transgrey_rgb666;
      dd->fp_lookupcolour  = serdisp_lookupcolour_rgb666;
      dd->fp_lookupgrey    = serdisp_lookupgrey_rgb666;
      break;
    case 24:
      if (SD_CS_ISBGR(dd))
        return -1;
      dd->fp_transcolour   = serdisp_transcolour_rgb888;
      dd->fp_transgrey     = serdisp_transgrey_rgb888;
      dd->fp_lookupcolour  = serdisp_lookupcolour_rgb888;
      dd->fp_lookupgrey    = serdisp_lookupgrey_rgb888;
      break;
    case 32:
      if (SD_CS_ISBGR(dd))
        return -1;
      dd->fp_transcolour   = serdisp_transcolour_rgba888;
      dd->fp_transgrey     = serdisp_transgrey_rgba888;
      dd->fp_lookupcolour  = serdisp_lookupcolour_rgba888;
      dd->fp_lookupgrey    = serdisp_lookupgrey_rgba888;
      break;
    default:
      return -1;
  }
  return 0;
}


/**
  * \brief   translates a colour value to the hardware dependend value
  *
  * Translates an \p 0xAARRGGBB colour value to a hardware dependend value that is suitable for serdisp_setpixel().
  *
  * \param   dd            device descriptor
  * \param   colour        \p 0xAARRGGBB colour value
  *
  * \return translated, serdisp_setpixel()-compliant, hardware dependend colour value
  */
long serdisp_transcolour(serdisp_t* dd, long colour) {
  return dd->fp_transcolour(dd, colour);
}



/**
  * \brief   translates a grey value to the hardware dependend value
  *
  * Translates a grey value to a hardware dependend value that is suitable for serdisp_setpixel()
  *
  * \param   dd            device descriptor
  * \param   greyvalue     grey value
  *
  * \return translated, serdisp_setpixel()-compliant, hardware dependend colour value
  */
long serdisp_transgrey(serdisp_t* dd, byte greyvalue) {
  return dd->fp_transgrey(dd, greyvalue);
}



/**
  * \brief   looks up hardware independ colour value to the hardware dependend value
  *
  * Looks up the corresponding \p 0xAARRGGBB colour value to a 
  * serdisp_setpixel()-compliant colour value.
  *
  * \param   dd            device descriptor
  * \param   colour        serdisp_setpixel()-compliant, hardware dependend colour value
  *
  * \return translated hardware independend colour value, format: \p 0xAARRGGBB
  */
long serdisp_lookupcolour(serdisp_t* dd, long colour) {
  return dd->fp_lookupcolour(dd, colour);
}



/**
  * \brief   looks up hardware independ grey value to the hardware dependend colour value
  *
  * Looks up the corresponding grey value to a serdisp_setpixel()-compliant colour value.
  *
  * \param   dd            device descriptor
  * \param   colour        serdisp_setpixel()-compliant, hardware dependend colour value
  *
  * \return translated grey value, format <tt>[0 .. 255]</tt>
  */
byte serdisp_lookupgrey(serdisp_t* dd, long colour) {
  return dd->fp_lookupgrey(dd, colour);
}





/* *********************************
   void serdisp_setcoltabentry (dd, idx, colour)
   *********************************
   set a colour in the colour table
   *********************************
   dd     ... display descriptor
   idx    ... index in colour table
   colour ... colour (format: 0xAARRGGBB)
   *********************************
*/
void serdisp_setcoltabentry(serdisp_t* dd, int idx, long colour) {
  if ( (!dd->ctable) || idx >= serdisp_getcolours(dd))
    return;
    
  dd->ctable[idx] = colour;
}


/* *********************************
   long serdisp_getcoltabentry (dd, idx)
   *********************************
   get  a colour from the colour table
   *********************************
   dd     ... display descriptor
   idx    ... index in colour table
   *********************************
   returns colour at index idx
*/
long serdisp_getcoltabentry(serdisp_t* dd, int idx) {
  if ( (!dd->ctable) || idx >= serdisp_getcolours(dd)) 
    return 0L;
  
  return dd->ctable[idx];
}


/* internal use */

/* source: http://www.compuphase.com/cmetric.htm */
long serdisp_colourdistance(long col1, long col2) {
  long r1, g1, b1;
  long r2, g2, b2;
  long distr, distg, distb, meanr;
  
  b1 = col1 & 0xFF;  g1 = (col1 & 0xFF00) >> 8; r1 = (col1 & 0xFF0000) >> 16;
  b2 = col2 & 0xFF;  g2 = (col2 & 0xFF00) >> 8; r2 = (col2 & 0xFF0000) >> 16;
  
  distr = r1-r2;  distg = g1-g2; distb = b1-b2;
  meanr = (r1 + r2) >> 1;
  
  /* return distr*distr + distg*distg + distb*distb; */
  return (((512+meanr)*distr*distr)>>8) + 4*distg*distg + (((767-meanr)*distb*distb)>>8);
}


/* 
 * colour scheme functions
 */


/* *********************************
   long serdisp_transcolour_* (dd, colour)
   *********************************
   translates an ARGB colour to a value suitable for setpixel
   *********************************
   dd     ... display descriptor
   colour ... ARGB colour value
   *********************************
   returns a translated, setpixel-compliant colour value
*/
long serdisp_transcolour_bw (serdisp_t* dd, long colour) {
  /* depth 1 (b/w) anomaly: index 0 == white, index 1 == black */
  return ( (serdisp_ARGB2GREY(colour) > 127 ) ? 0x00000000L : 0x00000001L);
}

long serdisp_transcolour_grey2_4(serdisp_t* dd, long colour) {
  long minidx = 0;
  long mincross = serdisp_colourdistance(colour, serdisp_getcoltabentry(dd, 0));
  long tempcolcross;
  int i;
  
  for (i = 1; i < (1 << dd->depth); i++) {
    tempcolcross = serdisp_colourdistance(colour, serdisp_getcoltabentry(dd, i));
    if (tempcolcross < mincross) {
      mincross = tempcolcross;
      minidx = i;
    }
  }
  return minidx;
}

long serdisp_transcolour_grey8 (serdisp_t* dd, long colour) {
  return ((long)(serdisp_ARGB2GREY(colour)));
}


/* 8 bit colour parts -> 3/3/2 bit by throwing away least significant bits */
/* 0xAARRGGBB -> byte(RRRGGGBB) */
long serdisp_transcolour_rgb332 (serdisp_t* dd, long colour) {
  return  ( (( colour & 0x00E00000L) >> 16) | ((colour & 0x0000E000L) >> 11) | ((colour & 0x000000C0L) >> 6) );
}

/* 8 bit colour parts -> 3/3/2 bit by throwing away least significant bits */
/* 0xAARRGGBB -> byte(BBBGGGRR) */
long serdisp_transcolour_bgr332 (serdisp_t* dd, long colour) {
  return  ( (( colour & 0x00C00000L) >> 22) | ((colour & 0x0000E000L) >> 11) | ((colour & 0x000000E0L) >> 0) );
}

/* 8 bit colour part -> 4 bit by throwing away 4 least significant bits */
/* 0xAARRGGBB -> 0x00000RGB */
long serdisp_transcolour_rgb444 (serdisp_t* dd, long colour) {
  return  ( (( colour & 0x00F00000L) >> 12) | ((colour & 0x0000F000L) >> 8) | ((colour & 0x000000F0L) >> 4) );
}

/* 8 bit colour part -> 5 or 6 bit by throwing away 3 or 2 least significant bits */
/* 0xAARRGGBB -> 5bit R 6bit G 5bit B */
long serdisp_transcolour_rgb565 (serdisp_t* dd, long colour) {
  return  ( (( colour & 0x00F80000L) >> 8) | ((colour & 0x0000FC00L) >> 5) | ((colour & 0x000000F8L) >> 3) );
}

/* 8 bit colour part -> 5 or 6 bit by throwing away 3 or 2 least significant bits */
/* 0xAARRGGBB -> 5bit B 6bit G 5bit R */
long serdisp_transcolour_bgr565 (serdisp_t* dd, long colour) {
  return  ( (( colour & 0x00F80000L) >> 19) | ((colour & 0x0000FC00L) >> 5) | ((colour & 0x000000F8L) << 8) );
}

/* 8 bit colour part -> 6 bit by throwing away 2 least significant bits */
/* 0xAARRGGBB -> 6bit R 6bit G 6bit B */
long serdisp_transcolour_rgb666 (serdisp_t* dd, long colour) {
  return  ( (( colour & 0x00FC0000L) >> 6) | ((colour & 0x0000FC00L) >> 4) | ((colour & 0x000000FCL) >> 2) );
}

/* truecolour without alpha */
long serdisp_transcolour_rgb888 (serdisp_t* dd, long colour) {
  return  ( colour & 0x00FFFFFFL );
}

/* truecolour with alpha */
long serdisp_transcolour_rgba888 (serdisp_t* dd, long colour) {
  return colour;
}



/* *********************************
   long serdisp_transgrey_* (dd, greyvalue)
   *********************************
   translates a grey value to a value suitable for setpixel
   *********************************
   dd        ... display descriptor
   greyvalue ... grey value
   *********************************
   returns a translated, setpixel-compliant colour value
*/
long serdisp_transgrey_bw (serdisp_t* dd, byte greyvalue) {
  /* depth 1 (b/w) anomaly: index 0 == white, index 1 == black */
  return ( ( greyvalue > 127 ) ? 0x00000000L : 0x00000001L);
}

long serdisp_transgrey_grey2_4(serdisp_t* dd, byte greyvalue) {
  long minidx = 0;
  long mincross = serdisp_colourdistance(serdisp_GREY2ARGB(greyvalue), serdisp_getcoltabentry(dd, 0));
  long tempcolcross;
  int i;
  
  for (i = 1; i < (1 << dd->depth); i++) {
    tempcolcross = serdisp_colourdistance(serdisp_GREY2ARGB(greyvalue), serdisp_getcoltabentry(dd, i));
    if (tempcolcross < mincross) {
      mincross = tempcolcross;
      minidx = i;
    }
  }
  return minidx;
}


long serdisp_transgrey_grey8 (serdisp_t* dd, byte greyvalue) {
  return ( (long)greyvalue );
}

long serdisp_transgrey_rgb332 (serdisp_t* dd, byte greyvalue) {
  return ( ((0xE0 & (long)greyvalue)) | ((0xE0 & (long)greyvalue) >> 3) | ((0xC0 & (long)greyvalue) >> 6) );
}

long serdisp_transgrey_bgr332 (serdisp_t* dd, byte greyvalue) {
  return ( ((0xC0 & (long)greyvalue) >> 6) | ((0xE0 & (long)greyvalue) >> 3) | ((0xE0 & (long)greyvalue) >> 0) );
}

long serdisp_transgrey_rgb444 (serdisp_t* dd, byte greyvalue) {
  return ( ((0xF0 & (long)greyvalue) << 4) | ((0xF0 & (long)greyvalue) ) | ((0xF0 & (long)greyvalue) >> 4) );
}

long serdisp_transgrey_rgb565 (serdisp_t* dd, byte greyvalue) {
  return ( ((0xF8 & (long)greyvalue) << 8) | ((0xFC & (long)greyvalue) <<3 ) | ((0xF8 & (long)greyvalue) >> 3) );
}

long serdisp_transgrey_bgr565 (serdisp_t* dd, byte greyvalue) {
  return ( ((0xF8 & (long)greyvalue) << 8) | ((0xFC & (long)greyvalue) <<3 ) | ((0xF8 & (long)greyvalue) >> 3) );
}

long serdisp_transgrey_rgb666 (serdisp_t* dd, byte greyvalue) {
  return ( ((long)(0xFC & greyvalue) << 10) | ((long)(0xFC & greyvalue) <<4 ) | ((long)(0xFC & greyvalue) >> 2) );
}

/* truecolour without alpha */
long serdisp_transgrey_rgb888 (serdisp_t* dd, byte greyvalue) {
  return  ( ((long)greyvalue << 16) | ((long)greyvalue << 8) | (long)greyvalue );
}

/* truecolour with alpha */
long serdisp_transgrey_rgba888 (serdisp_t* dd, byte greyvalue) {
  return  ( 0xFF000000L | ((long)greyvalue << 16) | ((long)greyvalue << 8) | (long)greyvalue );
}



/* *********************************
   long serdisp_lookupcolour_* (dd, colour)
   *********************************
   looks up the corresponding ARGB colour value to a setpixel-compliant colour value
   *********************************
   dd     ... display descriptor
   colour ... setpixel-compliant colour value
   *********************************
   returns a translated colour value (format: 0xAARRGGBB)
*/
long serdisp_lookupcolour_bw(serdisp_t* dd, long colour) {
  /* depth 1 (b/w) anomaly: index 0 == white, index 1 == black */
  return ( (colour > 0x00000000L) ? 0xFF000000L : 0xFFFFFFFFL);
}

long serdisp_lookupcolour_grey2_4(serdisp_t* dd, long colour) {
  return ( serdisp_getcoltabentry(dd, (int)colour) );
}

long serdisp_lookupcolour_grey8(serdisp_t* dd, long colour) {
  return ( serdisp_GREY2ARGB( (byte)(0x000000FFL | colour) ) );
}


/* byte(RRRGGGBB) -> 0xAARRGGBB */
long serdisp_lookupcolour_rgb332(serdisp_t* dd, long colour) {
  return ( 0xFF1F1F3FL |  ((0xE0L & colour) << 16 ) | ((0x1CL & colour) << 11 ) | ((0x00000003L & colour) << 6 ) );
}

/* byte(BBBGGGRR) -> 0xAARRGGBB */
long serdisp_lookupcolour_bgr332(serdisp_t* dd, long colour) {
  return ( 0xFF3F1F1FL |  ((0xE0L & colour) << 0 ) | ((0x1CL & colour) << 11 ) | ((0x00000003L & colour) << 22 ) );
}


/* 0x00000RGB -> 0xAARRGGBB */
long serdisp_lookupcolour_rgb444(serdisp_t* dd, long colour) {
  return ( 0xFF0F0F0FL |  ((0x00000F00L & colour) << 12 ) | ((0x000000F0L & colour) << 8 ) | ((0x0000000FL & colour) << 4 ) );
}

/* 5bit R  6bit G  5bit B -> 0xAARRGGBB */
long serdisp_lookupcolour_rgb565(serdisp_t* dd, long colour) {
  return ( 0xFF070307L |  ((0xF800L & colour) << 8 ) | ((0x07E0L & colour) << 5 ) | ((0x001FL & colour) << 3 ) );
}

/* 5bit B  6bit G  5bit R -> 0xAARRGGBB */
long serdisp_lookupcolour_bgr565(serdisp_t* dd, long colour) {
  return ( 0xFF070307L |  ((0x001FL & colour) << 19 ) | ((0x07E0L & colour) << 5 ) | ((0xF800L & colour) >> 8 ) );
}

/* 6bit R  6bit G  6bit B -> 0xAARRGGBB */
long serdisp_lookupcolour_rgb666(serdisp_t* dd, long colour) {
  return ( 0xFF030303L |  ((0x03F000L & colour) << 6 ) | ((0x000FC0L & colour) << 4 ) | ((0x00003FL & colour) << 2 ) );
}

/* truecolour without alpha */
long serdisp_lookupcolour_rgb888 (serdisp_t* dd, long colour) {
  return  ( 0xFF000000L | colour );
}

/* truecolour with alpha */
long serdisp_lookupcolour_rgba888 (serdisp_t* dd, long colour) {
  return colour;
}



/* *********************************
   byte serdisp_lookupgrey_* (dd, colour)
   *********************************
   translates a colour/grey value so that it is useable for setpixel
   *********************************
   dd     ... display descriptor
   colour ... setpixel-compliant colour value
   *********************************
   returns an 8-bit grey value
*/
byte serdisp_lookupgrey_bw(serdisp_t* dd, long colour) {
  /* depth 1 (b/w) anomaly: index 0 == white, index 1 == black */
  return ( (colour > 0x00000000L) ? 0 : 255);
}


byte serdisp_lookupgrey_grey2_4(serdisp_t* dd, long colour) {
  return ( serdisp_getcoltabentry(dd, (int)colour) );
}


byte serdisp_lookupgrey_grey8(serdisp_t* dd, long colour) {
  return ( (byte)(0x000000FFL | colour) );
}


byte serdisp_lookupgrey_rgb332(serdisp_t* dd, long colour) {
  return ( serdisp_ARGB2GREY(serdisp_lookupcolour_rgb332(dd, colour)) );
}

byte serdisp_lookupgrey_bgr332(serdisp_t* dd, long colour) {
  return ( serdisp_ARGB2GREY(serdisp_lookupcolour_bgr332(dd, colour)) );
}

byte serdisp_lookupgrey_rgb444(serdisp_t* dd, long colour) {
  return ( serdisp_ARGB2GREY(serdisp_lookupcolour_rgb444(dd, colour)) );
}

byte serdisp_lookupgrey_rgb565(serdisp_t* dd, long colour) {
  return ( serdisp_ARGB2GREY(serdisp_lookupcolour_rgb565(dd, colour)) );
}

byte serdisp_lookupgrey_bgr565(serdisp_t* dd, long colour) {
  return ( serdisp_ARGB2GREY(serdisp_lookupcolour_bgr565(dd, colour)) );
}

byte serdisp_lookupgrey_rgb666(serdisp_t* dd, long colour) {
  return ( serdisp_ARGB2GREY(serdisp_lookupcolour_rgb666(dd, colour)) );
}

/* truecolour without alpha */
byte serdisp_lookupgrey_rgb888 (serdisp_t* dd, long colour) {
  return ( serdisp_ARGB2GREY(serdisp_lookupcolour_rgb888(dd, colour)) );
}

/* truecolour with alpha */
byte serdisp_lookupgrey_rgba888 (serdisp_t* dd, long colour) {
  return ( serdisp_ARGB2GREY(serdisp_lookupcolour_rgba888(dd, colour)) );
}
