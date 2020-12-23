/*
 *************************************************************************
 *
 * serdisp_messages.c
 * debugging, logging, and error messages and funtions
 *
 *************************************************************************
 *
 * copyright (C) 2003-2008  wolfgang astleitner
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

#include "serdisplib/serdisp_messages.h"
#include <stdio.h>

int  sd_debuglevel   = 0;          /* debug level. -1: no, 0: little debugging. 2: verbose debugging */
int  sd_errorcode    = 0;          /* error code */
char sd_errormsg[255]= "";         /* last error message */
int  sd_runtimeerror   = 0;        /* runtime error occured (==1) */

FILE* sd_logmedium = (FILE*)0;
