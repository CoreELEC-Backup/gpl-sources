/* Emacs style mode select   -*- C++ -*-
 *-----------------------------------------------------------------------------
 *
 *
 *  PrBoom: a Doom port merged with LxDoom and LSDLDoom
 *  based on BOOM, a modified and improved DOOM engine
 *  Copyright (C) 1999 by
 *  id Software, Chi Hoang, Lee Killough, Jim Flynn, Rand Phares, Ty Halderman
 *  Copyright (C) 1999-2000 by
 *  Jess Haas, Nicolas Kalkhof, Colin Phipps, Florian Schulze
 *  Copyright 2005, 2006 by
 *  Florian Schulze, Colin Phipps, Neil Stevens, Andrey Budko
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 *  02111-1307, USA.
 *
 *-----------------------------------------------------------------------------*/

#ifndef R_FILTER_H
#define R_FILTER_H

#define DITHER_DIM      4
#define FILTER_UVBITS   6
#define FILTER_UVDIM    (1<<FILTER_UVBITS)

extern uint8_t filter_ditherMatrix[DITHER_DIM][DITHER_DIM];
extern uint8_t filter_roundedUVMap[FILTER_UVDIM*FILTER_UVDIM];
extern uint8_t filter_roundedRowMap[4*16];

void R_FilterInit(void);

// Use the dither matrix to determine whether a pixel is on or off based
// on the overall intensity we're trying to simulate
#define filter_getDitheredPixelLevel(x, y, intensity) \
  ((filter_ditherMatrix[(y)&(DITHER_DIM-1)][(x)&(DITHER_DIM-1)] < (intensity)) ? 1 : 0)

// Choose current pixel or next pixel down based on dither of the fractional
// texture V coord. texV is not a true fractional texture coord, so it
// has to be converted using ((texV) - dcvars.yl) >> 8), which was empirically
// derived. the "-dcvars.yl" is apparently required to offset some minor
// shaking in coordinate y-axis and prevents dithering seams
#define FILTER_GETV(x,y,texV,nextRowTexV) \
  (filter_getDitheredPixelLevel(x, y, (((texV) - yl) >> 8)&0xff) ? ((nextRowTexV)>>FRACBITS) : ((texV)>>FRACBITS))

// Choose current column or next column to the right based on dither of the 
// fractional texture U coord
#define filter_getDitheredForColumn(x, y, texV, nextRowTexV) \
  dither_sources[(filter_getDitheredPixelLevel(x, y, filter_fracu))][FILTER_GETV(x,y,texV,nextRowTexV)]

#define filter_getRoundedForColumn(texV, nextRowTexV) \
  filter_getScale2xQuadColors( \
    source[      ((texV)>>FRACBITS)              ], \
    source[      (MAX(0, ((texV)>>FRACBITS)-1))  ], \
    nextsource[  ((texV)>>FRACBITS)              ], \
    source[      ((nextRowTexV)>>FRACBITS)       ], \
    prevsource[  ((texV)>>FRACBITS)              ] \
  ) \
    [ filter_roundedUVMap[ \
      ((filter_fracu>>(8-FILTER_UVBITS))<<FILTER_UVBITS) + \
      ((((texV)>>8) & 0xff)>>(8-FILTER_UVBITS)) \
    ] ]

#define filter_getRoundedForSpan(texU, texV) \
  filter_getScale2xQuadColors( \
    source[ (((texU)>>16)&0x3f) | (((texV)>>10)&0xfc0)            ], \
    source[ (((texU)>>16)&0x3f) | ((((texV)-FRACUNIT)>>10)&0xfc0) ], \
    source[ ((((texU)+FRACUNIT)>>16)&0x3f) | (((texV)>>10)&0xfc0) ], \
    source[ (((texU)>>16)&0x3f) | ((((texV)+FRACUNIT)>>10)&0xfc0) ], \
    source[ ((((texU)-FRACUNIT)>>16)&0x3f) | (((texV)>>10)&0xfc0) ] \
  ) \
    [ filter_roundedUVMap[ \
      (((((texU)>>8) & 0xff)>>(8-FILTER_UVBITS))<<FILTER_UVBITS) + \
      ((((texV)>>8) & 0xff)>>(8-FILTER_UVBITS)) \
    ] ]

uint8_t *filter_getScale2xQuadColors(uint8_t e, uint8_t b, uint8_t f, uint8_t h, uint8_t d);

#define filter_getFilteredForColumn16(depthmap, texV, nextRowTexV) ( \
  VID_PAL16( depthmap(nextsource[(nextRowTexV)>>FRACBITS]),   (filter_fracu*((texV)&0xffff))>>(32-VID_COLORWEIGHTBITS) ) + \
  VID_PAL16( depthmap(source[(nextRowTexV)>>FRACBITS]),       ((0xffff-filter_fracu)*((texV)&0xffff))>>(32-VID_COLORWEIGHTBITS) ) + \
  VID_PAL16( depthmap(source[(texV)>>FRACBITS]),              ((0xffff-filter_fracu)*(0xffff-((texV)&0xffff)))>>(32-VID_COLORWEIGHTBITS) ) + \
  VID_PAL16( depthmap(nextsource[(texV)>>FRACBITS]),          (filter_fracu*(0xffff-((texV)&0xffff)))>>(32-VID_COLORWEIGHTBITS) ))

// Use 16 bit addition here since it's a little faster and the defects from
// such low-accuracy blending are less visible on spans
#define filter_getFilteredForSpan16(depthmap, texU, texV) ( \
  VID_PAL16( depthmap(source[ ((((texU)+FRACUNIT)>>16)&0x3f) | ((((texV)+FRACUNIT)>>10)&0xfc0)]),  (unsigned int)(((texU)&0xffff)*((texV)&0xffff))>>(32-VID_COLORWEIGHTBITS)) + \
  VID_PAL16( depthmap(source[ (((texU)>>16)&0x3f) | ((((texV)+FRACUNIT)>>10)&0xfc0)]),             (unsigned int)((0xffff-((texU)&0xffff))*((texV)&0xffff))>>(32-VID_COLORWEIGHTBITS)) + \
  VID_PAL16( depthmap(source[ (((texU)>>16)&0x3f) | (((texV)>>10)&0xfc0)]),                        (unsigned int)((0xffff-((texU)&0xffff))*(0xffff-((texV)&0xffff)))>>(32-VID_COLORWEIGHTBITS)) + \
  VID_PAL16( depthmap(source[ ((((texU)+FRACUNIT)>>16)&0x3f) | (((texV)>>10)&0xfc0)]),             (unsigned int)(((texU)&0xffff)*(0xffff-((texV)&0xffff)))>>(32-VID_COLORWEIGHTBITS)))

// do red and blue at once for slight speedup
//
#define GETBLENDED16_5050(col1, col2) \
  ((((col1&0xf81f)+(col2&0xf81f))>>1)&0xf81f) | \
  ((((col1&0x07e0)+(col2&0x07e0))>>1)&0x07e0)
//
#define GETBLENDED16_3268(col1, col2) \
  ((((col1&0xf81f)*5+(col2&0xf81f)*11)>>4)&0xf81f) | \
  ((((col1&0x07e0)*5+(col2&0x07e0)*11)>>4)&0x07e0)

#define GETBLENDED16_9406(col1, col2) \
  ((((col1&0xf81f)*15+(col2&0xf81f))>>4)&0xf81f) | \
  ((((col1&0x07e0)*15+(col2&0x07e0))>>4)&0x07e0)

#endif
