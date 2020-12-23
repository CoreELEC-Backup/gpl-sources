/*
 *************************************************************************
 *
 * image.h
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

#ifndef SERDISP_TOOLS_IMAGE_H
#define SERDISP_TOOLS_IMAGE_H

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

#include "serdisplib/serdisp_control.h"
#include "serdisplib/serdisp_messages.h"


/* define 'byte' if not available yet */
#ifndef byte
  #define byte unsigned char
#endif

#ifndef BOOL
  #define BOOL int
#endif


typedef struct sdtim_frame_s {
  gdImagePtr            image;     /* gd image */
  long                  delay;     /* delay to next frame (in ms) */
  struct sdtim_frame_s* next;      /* pointer to next frame */
} sdtim_frame_t;


#ifdef __cplusplus
extern "C" {
#endif

  sdtim_frame_t*    sdtim_loadimage           (char* imagefile, int* frames,  serdisp_t* dd, gdImagePtr template, int scalealgo, int flag_truecolour, int flag_dither);

#ifdef __cplusplus
    }
#endif

#endif /* SERDISP_TOOLS_IMAGE_H */
