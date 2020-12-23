/*
 * Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.
 *
 * (c) Copyright 1996 - 2001 Gary Henderson (gary.henderson@ntlworld.com) and
 *                           Jerremy Koot (jkoot@snes9x.com)
 *
 * Super FX C emulator code
 * (c) Copyright 1997 - 1999 Ivar (ivar@snes9x.com) and
 *                           Gary Henderson.
 * Super FX assembler emulator code (c) Copyright 1998 zsKnight and _Demo_.
 *
 * DSP1 emulator code (c) Copyright 1998 Ivar, _Demo_ and Gary Henderson.
 * C4 asm and some C emulation code (c) Copyright 2000 zsKnight and _Demo_.
 * C4 C code (c) Copyright 2001 Gary Henderson (gary.henderson@ntlworld.com).
 *
 * (c) Copyright 2014 - 2016 Daniel De Matteis. (UNDER NO CIRCUMSTANCE 
 * WILL COMMERCIAL RIGHTS EVER BE APPROPRIATED TO ANY PARTY)
 *
 * DOS port code contains the works of other authors. See headers in
 * individual files.
 *
 * Snes9x homepage: http://www.snes9x.com
 *
 * Permission to use, copy, modify and distribute Snes9x in both binary and
 * source form, for non-commercial purposes, is hereby granted without fee,
 * providing that this license information and copyright notice appear with
 * all copies and any derived work.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event shall the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Snes9x is freeware for PERSONAL USE only. Commercial users should
 * seek permission of the copyright holders first. Commercial use includes
 * charging money for Snes9x or software derived from Snes9x.
 *
 * The copyright holders request that bug fixes and improvements to the code
 * should be forwarded to them so everyone can benefit from the modifications
 * in future versions.
 *
 * Super NES and Super Nintendo Entertainment System are trademarks of
 * Nintendo Co., Limited and its subsidiary companies.
 */
#include "snes9x.h"
#include "memmap.h"
#include "ppu.h"
#include "sdd1.h"
#include "display.h"

void S9xSetSDD1MemoryMap(uint32 bank, uint32 value)
{
   int c;

   bank = 0xc00 + bank * 0x100;
   value = value * 1024 * 1024;

   for (c = 0; c < 0x100; c += 16)
   {
      uint8* block = &Memory.ROM [value + (c << 12)];
      int i;

      for (i = c; i < c + 16; i++)
         Memory.Map [i + bank] = block;
   }
}

void S9xResetSDD1(void)
{
   int i;

   memset(&Memory.FillRAM [0x4800], 0, 4);

   for (i = 0; i < 4; i++)
   {
      Memory.FillRAM [0x4804 + i] = i;
      S9xSetSDD1MemoryMap(i, i);
   }
}

void S9xSDD1PostLoadState(void)
{
   int i;
   for (i = 0; i < 4; i++)
      S9xSetSDD1MemoryMap(i, Memory.FillRAM [0x4804 + i]);
}
