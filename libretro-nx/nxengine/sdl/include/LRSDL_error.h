/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2012 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/

/**
 *  @file SDL_error.h
 *  Simple error message routines for SDL
 */

#ifndef _SDL_error_h
#define _SDL_error_h

#include "LRSDL_stdinc.h"

#include "begin_code.h"
/* Set up for C function definitions, even when using C++ */
#ifdef __cplusplus
extern "C" {
#endif

/** 
 *  @name Public functions
 */
/*@{*/
extern DECLSPEC void SDLCALL LRSDL_SetError(const char *fmt, ...);
extern DECLSPEC char * SDLCALL LRSDL_GetError(void);
extern DECLSPEC void SDLCALL LRSDL_ClearError(void);
/*@}*/

/**
 *  @name Private functions
 *  @internal Private error message function - used internally
 */
/*@{*/
#define LRSDL_OutOfMemory()	LRSDL_Error(SDL_ENOMEM)
#define LRSDL_Unsupported()	LRSDL_Error(SDL_UNSUPPORTED)
typedef enum {
	SDL_ENOMEM,
	SDL_EFREAD,
	SDL_EFWRITE,
	SDL_EFSEEK,
	SDL_UNSUPPORTED,
	SDL_LASTERROR
} SDL_errorcode;
extern DECLSPEC void SDLCALL LRSDL_Error(SDL_errorcode code);
/*@}*/

/* Ends C function definitions when using C++ */
#ifdef __cplusplus
}
#endif
#include "close_code.h"

#endif /* _SDL_error_h */
