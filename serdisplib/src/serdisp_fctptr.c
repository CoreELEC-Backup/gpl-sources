/** \file    serdisp_fctptr.c
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

#include "../config.h"

#include <stdio.h>
#include <string.h>
#include <strings.h>

#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_fctptr.h"

#ifdef WITH_DYNLOADING
#include <dlfcn.h>
#endif /* WITH_DYNLOADING */

static int fp_initialised    = 0;

static int available_libusb  = 0;  /* usb_*() */
static int available_pthread = 0; /* pthread_*() */
static int available_netsock = 0; /* gethostbyname, gethostbyaddr, sethostent, endhostent, ... */
static int available_libSDL  = 0;

#ifdef HAVE_LIBUSB
void            (*fp_usb_init)             (void);
usb_dev_handle* (*fp_usb_open)             (struct usb_device* dev);
int             (*fp_usb_close)            (usb_dev_handle* dev);
int             (*fp_usb_reset)            (usb_dev_handle* dev);
int             (*fp_usb_interrupt_read)   (usb_dev_handle* dev, int ep, char* bytes, int size, int timeout);
int             (*fp_usb_release_interface)(usb_dev_handle* dev, int interface);
int             (*fp_usb_find_busses)      (void);
int             (*fp_usb_find_devices)     (void);
struct usb_bus* (*fp_usb_get_busses)       (void);
int             (*fp_usb_claim_interface)  (usb_dev_handle* dev, int interface);

int             (*fp_usb_bulk_read)        (usb_dev_handle* dev, int ep, char* bytes, int size, int timeout);
int             (*fp_usb_bulk_write)       (usb_dev_handle* dev, int ep, char* bytes, int size, int timeout);
int             (*fp_usb_control_msg)      (usb_dev_handle* dev, int requesttype, int request,
                                            int value, int index, char *bytes, int size, int timeout);
int             (*fp_usb_clear_halt)       (usb_dev_handle *dev, unsigned int ep);

int             (*fp_usb_set_altinterface) (usb_dev_handle *dev, int alternate);
int             (*fp_usb_set_configuration)(usb_dev_handle *dev, int configuration);
int             (*fp_usb_get_string_simple)(usb_dev_handle *dev, int index, char *buf, size_t buflen);

int             (*fp_usb_detach_kernel_driver_np) (usb_dev_handle* dev, int interface);
#endif /* HAVE_LIBUSB */

#ifdef HAVE_LIBSDL
int             (*fp_SDL_Init)             (uint32_t);
SDL_Surface*    (*fp_SDL_SetVideoMode)     (int, int, int, uint32_t);
char*           (*fp_SDL_GetError)         (void);
void            (*fp_SDL_WM_SetCaption)    (const char*, const char*);
int             (*fp_SDL_LockSurface)      (SDL_Surface*);
void            (*fp_SDL_UnlockSurface)    (SDL_Surface*);
int             (*fp_SDL_Flip)             (SDL_Surface*);
void            (*fp_SDL_FreeSurface)      (SDL_Surface*);
uint32_t        (*fp_SDL_MapRGB)           (SDL_PixelFormat*, uint8_t, uint8_t, uint8_t);
void            (*fp_SDL_Quit)             (void);
#endif /* HAVE_LIBSDL */


/**
  * \brief   initialises additional function pointers
  *
  * additional function pointers are initialised when calling this function.
  * if dynamic loading of libraries is used: dlopen/dlsym are used for initialising the function pointers,
  * defines are used if additional libraries are to be linked directly to serdisplib.
  *
  * \attention  this function will only be executed once. further calls will be ignored.
  *
  * \since 1.98.0
  */
void SDFCTPTR_init(void) {
  if (fp_initialised)
    return;

  fp_initialised = 1;

  available_libusb   = 0;
  available_pthread  = 0;
  available_netsock  = 0;
  available_libSDL   = 0;

#ifdef HAVE_LIBUSB
  fp_usb_init                    = usb_init;
  fp_usb_open                    = usb_open;
  fp_usb_close                   = usb_close;
  fp_usb_reset                   = usb_reset;
  fp_usb_interrupt_read          = usb_interrupt_read;
  fp_usb_release_interface       = usb_release_interface;
  fp_usb_find_busses             = usb_find_busses;
  fp_usb_find_devices            = usb_find_devices;
  fp_usb_get_busses              = usb_get_busses;
  fp_usb_claim_interface         = usb_claim_interface;

  fp_usb_bulk_read               = usb_bulk_read;
  fp_usb_bulk_write              = usb_bulk_write;
  fp_usb_control_msg             = usb_control_msg;
  fp_usb_clear_halt              = usb_clear_halt;

  fp_usb_set_altinterface        = usb_set_altinterface;
  fp_usb_set_configuration       = usb_set_configuration;
  fp_usb_get_string_simple       = usb_get_string_simple;

  #ifdef LIBUSB_HAS_DETACH_KERNEL_DRIVER_NP
    fp_usb_detach_kernel_driver_np = usb_detach_kernel_driver_np;
  #else
    fp_usb_detach_kernel_driver_np = 0;
  #endif

  available_libusb = 1;
#endif /* HAVE_LIBUSB */

#ifdef HAVE_LIBSDL
  fp_SDL_Init                    = (int(*)(uint32_t)) SDL_Init;
  fp_SDL_SetVideoMode            = (SDL_Surface*(*)(int, int, int, uint32_t)) SDL_SetVideoMode;
  fp_SDL_GetError                = (char*(*)(void)) SDL_GetError;
  fp_SDL_WM_SetCaption           = (void(*)(const char*, const char*)) SDL_WM_SetCaption;
  fp_SDL_LockSurface             = (int(*)(SDL_Surface*)) SDL_LockSurface;
  fp_SDL_UnlockSurface           = (void(*)(SDL_Surface*)) SDL_UnlockSurface;
  fp_SDL_Flip                    = (int(*)(SDL_Surface*)) SDL_Flip;
  fp_SDL_FreeSurface             = (void(*)(SDL_Surface*)) SDL_FreeSurface;
  fp_SDL_MapRGB                  = (uint32_t(*)(SDL_PixelFormat*, uint8_t, uint8_t, uint8_t)) SDL_MapRGB;
  fp_SDL_Quit                    = (void(*)(void)) SDL_Quit;

  available_libSDL = 1;
#endif /* HAVE_LIBSDL */
}


/**
  * \brief   checks if an additional functionality is supported
  *
  * \param   libID          one of SDFCTPTR_[LIBUSB | PTHREAD | NETSOCK | LIBSDL]
  *
  * \retval 0   additional functionality is not supported
  * \retval 1   additional functionality is supported
  *
  * \since 1.98.0
  */
int SDFCTPTR_checkavail(int libID) {
  switch (libID) {
    case SDFCTPTR_LIBUSB:  return (available_libusb == 1); break;
    case SDFCTPTR_PTHREAD: return (available_pthread == 1); break;
    case SDFCTPTR_NETSOCK: return (available_netsock == 1); break;
    case SDFCTPTR_LIBSDL:  return (available_libSDL == 1); break;
  }
  return 0;
}


/**
  * \brief   closes all open dyn. loaded libraries
  *
  * \since 1.98.0
  */
void SDFCTPTR_cleanup  (void) {
}
