/*
 * UPSE: the unix playstation sound emulator.
 *
 * Filename: upse_ps1_dma_manager.c
 * Purpose: libupse: UPSE PS1 DMA Manager
 *
 * Copyright (c) 2007 William Pitcock <nenolod@sacredspiral.co.uk>
 * Portions copyright (c) 1999-2002 Pcsx Team
 * Portions copyright (c) 2004 "Xodnizel"
 *
 * UPSE is free software, released under the GNU General Public License,
 * version 2.
 *
 * A copy of the GNU General Public License, version 2, is included in
 * the UPSE source kit as COPYING.
 *
 * UPSE is offered without any warranty of any kind, explicit or implicit.
 */

#include "upse-internal.h"

void psxDma4(upse_module_instance_t *ins, u32 madr, u32 bcr, u32 chcr)
{				// SPU
    switch (chcr)
    {
      case 0x01000201:		//cpu to spu transfer
	  bcr = (bcr >> 16) * (bcr & 0xffff) * 2;

	  //printf("%08x, %08x\n",madr,bcr);
	  upse_ps1_spu_dma_write_memory(ins->spu, madr, bcr);
	  break;
      case 0x01000200:		//spu to cpu transfer
	  //printf("%08x\n",madr);
	  upse_ps1_spu_dma_read_memory(ins->spu, madr, (bcr >> 16) * (bcr & 0xffff) * 2);
	  break;
    }
}

void psxDma6(upse_module_instance_t *ins, u32 madr, u32 bcr, u32 chcr)
{
    u32 *mem = (u32 *) PSXM(ins, madr);

    if (chcr == 0x11000002)
    {
	while (bcr--)
	{
	    *mem-- = (madr - 4) & 0xffffff;
	    madr -= 4;
	}
	mem++;
	*mem = 0xffffff;
    }
}
