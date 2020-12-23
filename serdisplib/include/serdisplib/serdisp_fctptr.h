/** \file    serdisp_fctptr.h
  *
  * \brief   Initialisation of additional libraries and function pointers
  * \date    (C) 2010
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

/** \addtogroup SERDISP_CONNECT

  \section SDFP_INTRODUCTION Introduction
  serdisp_fctptr.h offers functions for initialising additional libraries and function pointers
  and requesting information about the additional libraries.


  * @{
  */


#ifndef SERDISP_FCTPTR_H
#define SERDISP_FCTPTR_H

/* define 'byte' if not available yet */
#ifndef byte
  #define byte unsigned char
#endif

#define SDFCTPTR_LIBUSB      1
#define SDFCTPTR_PTHREAD     2
#define SDFCTPTR_NETSOCK     3
#define SDFCTPTR_LIBSDL      4


#ifdef __cplusplus
extern "C" {
#endif
   void SDFCTPTR_init        (void);
   int  SDFCTPTR_checkavail  (int libID);
   void SDFCTPTR_cleanup     (void);
#ifdef __cplusplus
    }
#endif


#ifdef HAVE_LIBUSB
 #include <usb.h>

extern void            (*fp_usb_init)             (void);
extern usb_dev_handle* (*fp_usb_open)             (struct usb_device* dev);
extern int             (*fp_usb_close)            (usb_dev_handle* dev);
extern int             (*fp_usb_reset)            (usb_dev_handle* dev);
extern int             (*fp_usb_interrupt_read)   (usb_dev_handle* dev, int ep, char* bytes, int size, int timeout);
extern int             (*fp_usb_release_interface)(usb_dev_handle* dev, int interface);
extern int             (*fp_usb_find_busses)      (void);
extern int             (*fp_usb_find_devices)     (void);
extern struct usb_bus* (*fp_usb_get_busses)       (void);
extern int             (*fp_usb_claim_interface)  (usb_dev_handle* dev, int interface);

extern int             (*fp_usb_bulk_read)        (usb_dev_handle* dev, int ep, char* bytes, int size, int timeout);
extern int             (*fp_usb_bulk_write)       (usb_dev_handle* dev, int ep, char* bytes, int size, int timeout);
extern int             (*fp_usb_control_msg)      (usb_dev_handle* dev, int requesttype, int request,
                                                   int value, int index, char *bytes, int size, int timeout);
extern int             (*fp_usb_clear_halt)       (usb_dev_handle *dev, unsigned int ep);

extern int             (*fp_usb_set_altinterface) (usb_dev_handle *dev, int alternate);
extern int             (*fp_usb_set_configuration)(usb_dev_handle *dev, int configuration);
extern int             (*fp_usb_get_string_simple)(usb_dev_handle *dev, int index, char *buf, size_t buflen);

extern int             (*fp_usb_detach_kernel_driver_np) (usb_dev_handle* dev, int interface);
#endif /* HAVE_LIBUSB */


#ifdef HAVE_LIBSDL
 #ifdef HAVE_SDL_SDL_H
  #include "SDL/SDL.h"
 #else
  #include "SDL.h"
 #endif

 #include <inttypes.h>

 #define dfn_SDL_MUSTLOCK                  SDL_MUSTLOCK

extern  int            (*fp_SDL_Init)             (uint32_t);
extern  SDL_Surface*   (*fp_SDL_SetVideoMode)     (int, int, int, uint32_t);
extern  char*          (*fp_SDL_GetError)         (void);
extern  void           (*fp_SDL_WM_SetCaption)    (const char*, const char*);
extern  int            (*fp_SDL_LockSurface)      (SDL_Surface*);
extern  void           (*fp_SDL_UnlockSurface)    (SDL_Surface*);
extern  int            (*fp_SDL_Flip)             (SDL_Surface*);
extern  void           (*fp_SDL_FreeSurface)      (SDL_Surface*);
extern  uint32_t       (*fp_SDL_MapRGB)           (SDL_PixelFormat*, uint8_t, uint8_t, uint8_t);
extern  void           (*fp_SDL_Quit)             (void);
#endif /* HAVE_LIBSDL */


#endif /* SERDISP_FCTPTR_H */

/*! @} */
