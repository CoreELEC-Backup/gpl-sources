/***************************************************************************
                         upse-spu-internal.h  -  description
                             -------------------
    begin                : Wed May 15 2002
    copyright            : (C) 2002 by Pete Bernert
    email                : BlackDove@addcom.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version. See also the license.txt file for *
 *   additional informations.                                              *
 *                                                                         *
 ***************************************************************************/

#ifndef __UPSE_SPU_INTERNAL_H__
#define __UPSE_SPU_INTERNAL_H__

#include "upse-types.h"
#include "upse-ps1-memory-manager.h"

//*************************************************************************//
// History of changes:
//
// 2002/05/15 - Pete
// - generic cleanup for the Peops release
//
//*************************************************************************//

#define max(a,b)            (((a) > (b)) ? (a) : (b))
#define min(a,b)            (((a) < (b)) ? (a) : (b))

///////////////////////////////////////////////////////////
// struct defines
///////////////////////////////////////////////////////////

typedef struct {
    float lx1;
    float lx2;
    float ly1;
    float ly2;

    float la0;
    float la1;
    float la2;
    float lb1;
    float lb2;

    float hx1[2];
    float hx2[2];
    float hy1[2];
    float hy2[2];

    float ha0;
    float ha1;
    float ha2;
    float hb1;
    float hb2;
} upse_spu_lowpass_info_t;

typedef struct {
    s16 hist[2];
} upse_spu_nyquist_info_t;

typedef struct {
	void *pCore;

    u8 pSpuBuffer[32770];

    upse_audio_callback_func_t cb;
    const void *cb_userdata;

    u32 sampcount;
    u32 decaybegin;
    u32 decayend;

    s16 *pS;

    u32 seektime;
    s32 nextirq;

    upse_spu_lowpass_info_t lowpass;
    upse_spu_nyquist_info_t nyquist;

    upse_module_instance_t *ins;

    s32 downbuf[2][8];
    s32 upbuf[2][8];
    int dbpos, ubpos;

    s32 RateTable[160];
} upse_spu_state_t;

extern void upse_spu_lowpass_filter_reset(upse_spu_state_t *spu);
extern void upse_spu_lowpass_filter_redesign(upse_spu_state_t *spu, int samplerate);
extern void upse_spu_lowpass_filter_process(upse_spu_state_t *spu, s16 *samplebuf, int samplecount);

extern void upse_spu_nyquist_filter_process(upse_spu_state_t *spu, s16 *samplebuf, int samplecount);

#endif
