/*
 * UPSE: the unix playstation sound emulator.
 *
 * Filename: upse_ps1_spu_register_io.c
 * Purpose: libupse: PS1 SPU register I/O operations
 *
 * Copyright (c) 2007 William Pitcock <nenolod@sacredspiral.co.uk>
 * Portions copyright (c) 2002 Neill Corlett
 *
 * UPSE is free software, released under the GNU General Public License,
 * version 2.
 *
 * A copy of the GNU General Public License, version 2, is included in
 * the UPSE source kit as COPYING.
 *
 * UPSE is offered without any warranty of any kind, explicit or implicit.
 */

#include "upse-types.h"
#include "upse-internal.h"
#include "upse-ps1-spu-base.h"
#include "upse-spu-internal.h"

#include "upse.h"
#include "upse-ps1-memory-manager.h"
#include "Neill/spu.h"

////////////////////////////////////////////////////////////////////////
// WRITE REGISTERS: called by main emu
////////////////////////////////////////////////////////////////////////

void upse_ps1_spu_write_register(upse_spu_state_t *spu, u32 reg, u16 val)
{
	spu_sh( spu->pCore, reg, val );
}

////////////////////////////////////////////////////////////////////////
// READ REGISTER: called by main emu
////////////////////////////////////////////////////////////////////////

u16 upse_ps1_spu_read_register(upse_spu_state_t *spu, u32 reg)
{
	return spu_lh( spu->pCore, reg );
}
