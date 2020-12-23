/**
 * Copyright (C) 2009 
 *	- Salvatore Santagati <salvatore.santagati@gmail.com>
 *	- Abdur Rab <c.abdur@yahoo.com>
 *
 * All rights reserved.
 *
 * This program is free software; under the terms of the 
 * GNU General Public License as published by the Free Software Foundation; 
 * either version 2 of the License, or (at your option) any later version.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * @ Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer. 
 *
 * @ Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef MKCUE_H
#define MKCUE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

#ifndef MKCORE_H
#include "mkcore.h"
#endif

/* --- @print_cue_index@ --- *
 *
 * Arguments:   @file_ptrs *fptrs@ = input file
 * 		@struct_cue *cue@ =  pointer struct of cue sheet
 *
 *
 * Returns:	---	
 *
 * Use:	 	print index of image.
 */
void  print_cue_index ( file_ptrs* fptrs, struct_cue *cue );

/* --- @print_cue_track@ --- *
 *
 * Arguments:   @file_ptrs *fptrs@ = input file
 * 		@struct_cue *cue@ =  pointer struct of cue sheet
 *
 *
 * Returns:	---
 *
 * Use:	 	print track of image.
 */
void print_cue_track ( file_ptrs* fptrs, struct_cue* cue );

/* --- @print_cue_file --- *
 *
 * Arguments:   @file_ptrs *fptrs@ = input file
 * 		@struct_cue *cue@ =  pointer struct of cue sheet
 *
 *
 * Returns:	---	
 *
 * Use:	 	print file name of image.
 */
void print_cue_file ( file_ptrs* fptrs, char *file_input );

/* --- @print_cue@ --- *
 *
 * Arguments:   @file_ptrs *fptrs@ = input file
 * 		 @struct_cue *cue@ =  pointer struct of cue sheet
 *
 *
 * Returns:	mode of image, @-1@ otherwise
 *
 *  Use:	print cue sheet of image.	
 */
/*
void print_cue ( file_ptrs* fptrs, struct_cue* cue );
*/

/* --- @create_first_track@ --- *
 *
 * Arguments:   @file_ptrs *fptrs @ = input file
 * 		@image_struct *img_struct@ = pointer struct of type image and pregap of image
 *		@char *file_input@  =  name of input file
 *		@struct_cue *cue@ = pointer struct of cuesheet
 *
 * Returns:     ---
 *
 * Use:         detect track of image.
 */
void create_first_track ( file_ptrs* fptrs, image_struct* img_struct, char *file_input, struct_cue* cue );

/* --- @create_raw_cue@ --- *
 *
 * Arguments:   @file_ptrs *fptrs @ = input file
 * 		@image_struct *img_struct@ = pointer struct of type image and pregap of image
 *		@char *file_input@  = name of input file
 *
 *
 * Returns:     ---
 *
 * Use:         generate a cuesheet for raw image.
 */
void create_raw_cue ( file_ptrs* fptrs, image_struct* img_struct, char *file_input );

/* --- @create_iso_cue@ --- *
 *
 * Arguments:   @file_ptrs *fptrs @ = input file
 * 		@image_struct *img_struct@ = pointer struct of type image and pregap of image
 *		@char *file_input@  = name of input file
 *
 *
 * Returns:     ---
 *
 * Use:         generate a cuesheet for iso/udf image.
 */
void create_iso_cue ( file_ptrs* fptrs, image_struct* img_struct, char *file_input );

/* --- @track_vcd_cue@ --- *
 *
 * Arguments:   @file_ptrs *fptrs @ = input file
 * 		@image_struct *img_struct@ = pointer struct of type image and pregap of image
 *		@struct_cue *cue@ = pointer struct of cuesheet
 *		@off_t n_loop@  = number of byte from where the block starts
 *
 *
 * Returns:	Zeor on success, @-1@ on error.     
 *
 * Use:         detect track of VCD/SVCD image.
 */
int track_vcd_cue ( file_ptrs* fptrs, image_struct* img_struct, struct_cue* cue, off_t n_loop );
 
/* --- @create_vcd_cue@ --- *
 *
 * Arguments:   @file_ptrs *fptrs @ = input file
 * 		@image_struct *img_struct@ = pointer struct of type image and pregap of image
 *		@char *file_input@  = name of input file
 *
 *
 * Returns:     Zeor on success, @-1@ on error. 
 *
 * Use:         generate a cuesheet for vcd.
 */
int create_vcd_cue ( file_ptrs* fptrs, image_struct* img_struct, char *file_input );

/* --- @create_cue@ --- *
 *
 * Arguments:   @file_ptrs *fptrs @ = input file
 * 		@image_struct *img_struct@ = pointer struct of type image and pregap of image
 *
 *
 * Returns:     Zeor on success, @-1@ on error. 
 *
 * Use:         Get info from file for generate a cuesheet file descriptor.
 */
int create_cue ( file_ptrs* fptrs,  image_struct*  img_struct, char *file_input );

#ifdef __cplusplus
}       /* extern "C" */
#endif  /* __cplusplus */

#endif /* MKCUE_H */
