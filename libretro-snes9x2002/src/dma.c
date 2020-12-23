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
#include "cpuexec.h"
#include "missing.h"
#include "dma.h"
#include "apu.h"
#include "gfx.h"
#ifdef USE_SA1
#include "sa1.h"
#endif

//SDD1
#include "sdd1emu.h"
uint8 buffer[0x10000];
//SDD1//

extern int HDMA_ModeByteCounts [8];
extern uint8* HDMAMemPointers [8];
extern uint8* HDMABasePointers [8];

static int S9xCompareSDD1IndexEntries(const void* p1, const void* p2)
{
   return (*(uint32*) p1 - * (uint32*) p2);
}

/**********************************************************************************************/
/* S9xDoDMA()                                                                                   */
/* This function preforms the general dma transfer                                            */
/**********************************************************************************************/
void S9xDoDMA(uint8 Channel)
{
   int count, inc;
   uint8 Work;
   bool8 in_sa1_dma = FALSE;
   uint8* in_sdd1_dma = NULL;
   SDMA *d;

   if (Channel > 7 || CPU.InDMA)
      return;

   CPU.InDMA = TRUE;
   d         = &DMA[Channel];

   count     = d->TransferBytes;

   if (count == 0)
      count = 0x10000;

   inc = d->AAddressFixed ? 0 : (!d->AAddressDecrement ? 1 : -1);

   if ((d->ABank == 0x7E || d->ABank == 0x7F) && d->BAddress == 0x80)
   {
      d->AAddress += d->TransferBytes;
      goto update_address;
   }
   switch (d->BAddress)
   {
   case 0x18:
   case 0x19:
      if (IPPU.RenderThisFrame)
#ifdef __DEBUG__
         printf("FLUSH_REDRAW by DMA BAddress %x", d->BAddress);
#endif
      FLUSH_REDRAW();

      break;
   }

   if (Settings.SDD1)
   {
      if (d->AAddressFixed && Memory.FillRAM [0x4801] > 0)
      {
         uint32 address;

         // Hacky support for pre-decompressed S-DD1 data
         inc      = !d->AAddressDecrement ? 1 : -1;
         address  = (((d->ABank << 16) | d->AAddress) & 0xfffff) << 4;

         address |= Memory.FillRAM [0x4804 + ((d->ABank - 0xc0) >> 4)];
         if (Settings.SDD1Pack)
         {
            uint8* in_ptr = GetBasePointer(((d->ABank << 16) | d->AAddress));
            in_ptr += d->AAddress;

            SDD1_decompress(buffer, in_ptr, d->TransferBytes);
            in_sdd1_dma = buffer;
         }
         else
         {
            void* ptr = bsearch(&address, Memory.SDD1Index,
                                Memory.SDD1Entries, 12, S9xCompareSDD1IndexEntries);
            if (ptr)
               in_sdd1_dma = *(uint32*)((uint8*) ptr + 4) + Memory.SDD1Data;
         }
      }

      Memory.FillRAM [0x4801] = 0;
   }

#ifdef USE_SA1
   if (d->BAddress == 0x18 && SA1.in_char_dma && (d->ABank & 0xf0) == 0x40)
#else
   if (d->BAddress == 0x18 && (d->ABank & 0xf0) == 0x40)
#endif
   {
      int i;
      // Perform packed bitmap to PPU character format conversion on the
      // data before transmitting it to V-RAM via-DMA.
      int num_chars = 1 << ((Memory.FillRAM [0x2231] >> 2) & 7);
      int depth = (Memory.FillRAM [0x2231] & 3) == 0 ? 8 :
                  (Memory.FillRAM [0x2231] & 3) == 1 ? 4 : 2;

      int bytes_per_char = 8 * depth;
      int bytes_per_line = depth * num_chars;
      int char_line_bytes = bytes_per_char * num_chars;
      uint32 addr = (d->AAddress / char_line_bytes) * char_line_bytes;
      uint8* base = GetBasePointer((d->ABank << 16) + addr) + addr;
      uint8* buffer = &Memory.ROM [MAX_ROM_SIZE - 0x10000];
      uint8* p = buffer;
      uint32 inc = char_line_bytes - (d->AAddress % char_line_bytes);
      uint32 char_count = inc / bytes_per_char;

      in_sa1_dma = TRUE;

      //printf ("%08x,", base); fflush (stdout);
      //printf ("depth = %d, count = %d, bytes_per_char = %d, bytes_per_line = %d, num_chars = %d, char_line_bytes = %d\n",
      //depth, count, bytes_per_char, bytes_per_line, num_chars, char_line_bytes);

      switch (depth)
      {
      case 2:
         for (i = 0; i < count; i += inc, base += char_line_bytes,
               inc = char_line_bytes, char_count = num_chars)
         {
            uint8* line = base + (num_chars - char_count) * 2;
            uint32 j;
            for (j = 0; j < char_count && p - buffer < count;
                  j++, line += 2)
            {
               uint8* q;
               int l;
               q = line;
               for (l = 0; l < 8; l++, q += bytes_per_line)
               {
                  int b;
                  for (b = 0; b < 2; b++)
                  {
                     uint8 r = *(q + b);
                     *(p + 0) = (*(p + 0) << 1) | ((r >> 0) & 1);
                     *(p + 1) = (*(p + 1) << 1) | ((r >> 1) & 1);
                     *(p + 0) = (*(p + 0) << 1) | ((r >> 2) & 1);
                     *(p + 1) = (*(p + 1) << 1) | ((r >> 3) & 1);
                     *(p + 0) = (*(p + 0) << 1) | ((r >> 4) & 1);
                     *(p + 1) = (*(p + 1) << 1) | ((r >> 5) & 1);
                     *(p + 0) = (*(p + 0) << 1) | ((r >> 6) & 1);
                     *(p + 1) = (*(p + 1) << 1) | ((r >> 7) & 1);
                  }
                  p += 2;
               }
            }
         }
         break;
      case 4:
         for (i = 0; i < count; i += inc, base += char_line_bytes,
               inc = char_line_bytes, char_count = num_chars)
         {
            uint8* line = base + (num_chars - char_count) * 4;
            uint32 j;
            for (j = 0; j < char_count && p - buffer < count;
                  j++, line += 4)
            {
               uint8* q = line;
               int l;
               for (l = 0; l < 8; l++, q += bytes_per_line)
               {
                  int b;
                  for (b = 0; b < 4; b++)
                  {
                     uint8 r = *(q + b);
                     *(p +  0) = (*(p +  0) << 1) | ((r >> 0) & 1);
                     *(p +  1) = (*(p +  1) << 1) | ((r >> 1) & 1);
                     *(p + 16) = (*(p + 16) << 1) | ((r >> 2) & 1);
                     *(p + 17) = (*(p + 17) << 1) | ((r >> 3) & 1);
                     *(p +  0) = (*(p +  0) << 1) | ((r >> 4) & 1);
                     *(p +  1) = (*(p +  1) << 1) | ((r >> 5) & 1);
                     *(p + 16) = (*(p + 16) << 1) | ((r >> 6) & 1);
                     *(p + 17) = (*(p + 17) << 1) | ((r >> 7) & 1);
                  }
                  p += 2;
               }
               p += 32 - 16;
            }
         }
         break;
      case 8:
         for (i = 0; i < count; i += inc, base += char_line_bytes,
               inc = char_line_bytes, char_count = num_chars)
         {
            uint8* line = base + (num_chars - char_count) * 8;
            uint32 j;
            for (j = 0; j < char_count && p - buffer < count;
                  j++, line += 8)
            {
               uint8* q;
               int l;
               q = line;
               for (l = 0; l < 8; l++, q += bytes_per_line)
               {
                  int b;
                  for (b = 0; b < 8; b++)
                  {
                     uint8 r = *(q + b);
                     *(p +  0) = (*(p +  0) << 1) | ((r >> 0) & 1);
                     *(p +  1) = (*(p +  1) << 1) | ((r >> 1) & 1);
                     *(p + 16) = (*(p + 16) << 1) | ((r >> 2) & 1);
                     *(p + 17) = (*(p + 17) << 1) | ((r >> 3) & 1);
                     *(p + 32) = (*(p + 32) << 1) | ((r >> 4) & 1);
                     *(p + 33) = (*(p + 33) << 1) | ((r >> 5) & 1);
                     *(p + 48) = (*(p + 48) << 1) | ((r >> 6) & 1);
                     *(p + 49) = (*(p + 49) << 1) | ((r >> 7) & 1);
                  }
                  p += 2;
               }
               p += 64 - 16;
            }
         }
         break;
      }

   }

   if (!d->TransferDirection)
   {
      uint8 *base;
      uint16 p;

#ifdef VAR_CYCLES
      //reflects extra cycle used by DMA
      CPU.Cycles += 8 * (count + 1);
#else
      //needs fixing for the extra DMA cycle
      CPU.Cycles += (1 + count) + ((1 + count) >> 2);
#endif
      base = GetBasePointer((d->ABank << 16) + d->AAddress);
      p    = d->AAddress;

      if (!base)
         base = Memory.ROM;

      if (in_sa1_dma)
      {
         base = &Memory.ROM [MAX_ROM_SIZE - 0x10000];
         p = 0;
      }

      if (in_sdd1_dma)
      {
         base = in_sdd1_dma;
         p = 0;
      }

      if (inc > 0)
         d->AAddress += count;
      else if (inc < 0)
         d->AAddress -= count;

      if (d->TransferMode == 0 || d->TransferMode == 2)
      {
         switch (d->BAddress)
         {
         case 0x04:
            do
            {
               Work = *(base + p);
               REGISTER_2104(Work);
               p += inc;
            }
            while (--count > 0);
            break;
         case 0x18:
            IPPU.FirstVRAMRead = TRUE;
            if (!PPU.VMA.FullGraphicCount)
            {
               do
               {
                  Work = *(base + p);
                  REGISTER_2118_linear(Work);
                  p += inc;
               }
               while (--count > 0);
            }
            else
            {
               do
               {
                  Work = *(base + p);
                  REGISTER_2118_tile(Work);
                  p += inc;
               }
               while (--count > 0);
            }
            break;
         case 0x19:
            IPPU.FirstVRAMRead = TRUE;
            if (!PPU.VMA.FullGraphicCount)
            {
               do
               {
                  Work = *(base + p);
                  REGISTER_2119_linear(Work);
                  p += inc;
               }
               while (--count > 0);
            }
            else
            {
               do
               {
                  Work = *(base + p);
                  REGISTER_2119_tile(Work);
                  p += inc;
               }
               while (--count > 0);
            }
            break;
         case 0x22:
            do
            {
               Work = *(base + p);
               REGISTER_2122(Work);
               p += inc;
            }
            while (--count > 0);
            break;
         case 0x80:
            do
            {
               Work = *(base + p);
               REGISTER_2180(Work);
               p += inc;
            }
            while (--count > 0);
            break;
         default:
            do
            {
               Work = *(base + p);
               S9xSetPPU(Work, 0x2100 + d->BAddress);
               p += inc;
            }
            while (--count > 0);
            break;
         }
      }
      else if (d->TransferMode == 1 || d->TransferMode == 5)
      {
         if (d->BAddress == 0x18)
         {
            // Write to V-RAM
            IPPU.FirstVRAMRead = TRUE;
            if (!PPU.VMA.FullGraphicCount)
            {
               while (count > 1)
               {
                  Work = *(base + p);
                  REGISTER_2118_linear(Work);
                  p += inc;

                  Work = *(base + p);
                  REGISTER_2119_linear(Work);
                  p += inc;
                  count -= 2;
               }
               if (count == 1)
               {
                  Work = *(base + p);
                  REGISTER_2118_linear(Work);
                  p += inc;
               }
            }
            else
            {
               while (count > 1)
               {
                  Work = *(base + p);
                  REGISTER_2118_tile(Work);
                  p += inc;

                  Work = *(base + p);
                  REGISTER_2119_tile(Work);
                  p += inc;
                  count -= 2;
               }
               if (count == 1)
               {
                  Work = *(base + p);
                  REGISTER_2118_tile(Work);
                  p += inc;
               }
            }
         }
         else
         {
            // DMA mode 1 general case
            while (count > 1)
            {
               Work = *(base + p);
               S9xSetPPU(Work, 0x2100 + d->BAddress);
               p += inc;

               Work = *(base + p);
               S9xSetPPU(Work, 0x2101 + d->BAddress);
               p += inc;
               count -= 2;
            }
            if (count == 1)
            {
               Work = *(base + p);
               S9xSetPPU(Work, 0x2100 + d->BAddress);
               p += inc;
            }
         }
      }
      else if (d->TransferMode == 3)
      {
         do
         {
            Work = *(base + p);
            S9xSetPPU(Work, 0x2100 + d->BAddress);
            p += inc;
            if (count <= 1)
               break;

            Work = *(base + p);
            S9xSetPPU(Work, 0x2100 + d->BAddress);
            p += inc;
            if (count <= 2)
               break;

            Work = *(base + p);
            S9xSetPPU(Work, 0x2101 + d->BAddress);
            p += inc;
            if (count <= 3)
               break;

            Work = *(base + p);
            S9xSetPPU(Work, 0x2101 + d->BAddress);
            p += inc;
            count -= 4;
         }
         while (count > 0);
      }
      else if (d->TransferMode == 4)
      {
         do
         {
            Work = *(base + p);
            S9xSetPPU(Work, 0x2100 + d->BAddress);
            p += inc;
            if (count <= 1)
               break;

            Work = *(base + p);
            S9xSetPPU(Work, 0x2101 + d->BAddress);
            p += inc;
            if (count <= 2)
               break;

            Work = *(base + p);
            S9xSetPPU(Work, 0x2102 + d->BAddress);
            p += inc;
            if (count <= 3)
               break;

            Work = *(base + p);
            S9xSetPPU(Work, 0x2103 + d->BAddress);
            p += inc;
            count -= 4;
         }
         while (count > 0);
      }
   }
   else
   {
      do
      {
         switch (d->TransferMode)
         {
         case 0:
         case 2:
#ifndef VAR_CYCLES
            CPU.Cycles += 1;
#endif
            Work = S9xGetPPU(0x2100 + d->BAddress);
            S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
            d->AAddress += inc;
            --count;
            break;

         case 1:
         case 5:
#ifndef VAR_CYCLES
            CPU.Cycles += 3;
#endif
            Work = S9xGetPPU(0x2100 + d->BAddress);
            S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
            d->AAddress += inc;
            if (!--count)
               break;

            Work = S9xGetPPU(0x2101 + d->BAddress);
            S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
            d->AAddress += inc;
            count--;
            break;

         case 3:
#ifndef VAR_CYCLES
            CPU.Cycles += 6;
#endif
            Work = S9xGetPPU(0x2100 + d->BAddress);
            S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
            d->AAddress += inc;
            if (!--count)
               break;

            Work = S9xGetPPU(0x2100 + d->BAddress);
            S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
            d->AAddress += inc;
            if (!--count)
               break;

            Work = S9xGetPPU(0x2101 + d->BAddress);
            S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
            d->AAddress += inc;
            if (!--count)
               break;

            Work = S9xGetPPU(0x2101 + d->BAddress);
            S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
            d->AAddress += inc;
            count--;
            break;

         case 4:
#ifndef VAR_CYCLES
            CPU.Cycles += 6;
#endif
            Work = S9xGetPPU(0x2100 + d->BAddress);
            S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
            d->AAddress += inc;
            if (!--count)
               break;

            Work = S9xGetPPU(0x2101 + d->BAddress);
            S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
            d->AAddress += inc;
            if (!--count)
               break;

            Work = S9xGetPPU(0x2102 + d->BAddress);
            S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
            d->AAddress += inc;
            if (!--count)
               break;

            Work = S9xGetPPU(0x2103 + d->BAddress);
            S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
            d->AAddress += inc;
            count--;
            break;

         default:
            count = 0;
            break;
         }
      }
      while (count);
   }

   //#ifdef SPC700_C
#ifdef SPC700_SHUTDOWN
   CPU.APU_APUExecuting = Settings.APUEnabled;
#endif
   asm_APU_EXECUTE(1); // execute but only in normal mode
   //#endif
   while (CPU.Cycles > CPU.NextEvent)
      S9xDoHBlankProcessing();

update_address:
   // Super Punch-Out requires that the A-BUS address be updated after the
   // DMA transfer.
   Memory.FillRAM[0x4302 + (Channel << 4)] = (uint8) d->AAddress;
   Memory.FillRAM[0x4303 + (Channel << 4)] = d->AAddress >> 8;

   // Secret of the Mana requires that the DMA bytes transfer count be set to
   // zero when DMA has completed.
   Memory.FillRAM [0x4305 + (Channel << 4)] = 0;
   Memory.FillRAM [0x4306 + (Channel << 4)] = 0;

   DMA[Channel].IndirectAddress = 0;
   d->TransferBytes = 0;

   CPU.InDMA = FALSE;
}

void S9xStartHDMA(void)
{
   uint8 i;
   //if (Settings.DisableHDMA)
   //IPPU.HDMA = 0;
   //else
   missing.hdma_this_frame = IPPU.HDMA = Memory.FillRAM [0x420c];

   IPPU.HDMAStarted = TRUE;

   for (i = 0; i < 8; i++)
   {
      if (IPPU.HDMA & (1 << i))
      {
         DMA [i].LineCount = 0;
         DMA [i].FirstLine = TRUE;
         DMA [i].Address = DMA [i].AAddress;
      }
      HDMAMemPointers [i] = NULL;
   }
}

uint8 S9xDoHDMA(uint8 byte)
{
   SDMA* p = &DMA [0];

   int d = 0;

   uint8 mask;
   for (mask = 1; mask; mask <<= 1, p++, d++)
   {
      if (byte & mask)
      {
         if (!p->LineCount)
         {
            uint8 line;
            line = S9xGetByte((p->ABank << 16) + p->Address);
            if (line == 0x80)
            {
               p->Repeat = TRUE;
               p->LineCount = 128;
            }
            else
            {
               p->Repeat = !(line & 0x80);
               p->LineCount = line & 0x7f;
            }

            // Disable H-DMA'ing into V-RAM (register 2118) for Hook
            if (!p->LineCount || p->BAddress == 0x18)
            {
               byte &= ~mask;
               p->IndirectAddress += HDMAMemPointers [d] - HDMABasePointers [d];
               Memory.FillRAM [0x4305 + (d << 4)] = (uint8) p->IndirectAddress;
               Memory.FillRAM [0x4306 + (d << 4)] = p->IndirectAddress >> 8;
               continue;
            }

            p->Address++;
            p->FirstLine = 1;
            if (p->HDMAIndirectAddressing)
            {
               p->IndirectBank = Memory.FillRAM [0x4307 + (d << 4)];
               p->IndirectAddress = S9xGetWord((p->ABank << 16) + p->Address);
               p->Address += 2;
            }
            else
            {
               p->IndirectBank = p->ABank;
               p->IndirectAddress = p->Address;
            }
            HDMABasePointers [d] = HDMAMemPointers [d] =
                                      S9xGetMemPointer((p->IndirectBank << 16) + p->IndirectAddress);
         }
         else
         {

            if (!HDMAMemPointers [d])
            {
               if (!p->HDMAIndirectAddressing)
               {
                  p->IndirectBank = p->ABank;
                  p->IndirectAddress = p->Address;
               }

               if (!(HDMABasePointers [d] = HDMAMemPointers [d] =
                                               S9xGetMemPointer((p->IndirectBank << 16) + p->IndirectAddress)))
               {
                  byte &= ~mask;
                  continue;
               }
               // Uncommenting the following line breaks Punchout - it starts
               // H-DMA during the frame.
               //p->FirstLine = TRUE;
            }
         }
         if (p->Repeat && !p->FirstLine)
         {
            p->LineCount--;
            continue;
         }
         switch (p->TransferMode)
         {
         case 0:
#ifndef VAR_CYCLES
            CPU.Cycles += 1;
#else
            CPU.Cycles += 8;
#endif
            S9xSetPPU(*HDMAMemPointers [d]++, 0x2100 + p->BAddress);
            break;
         case 1:
         case 5:
#ifndef VAR_CYCLES
            CPU.Cycles += 3;
#else
            CPU.Cycles += 16;
#endif
            S9xSetPPU(*(HDMAMemPointers [d] + 0), 0x2100 + p->BAddress);
            S9xSetPPU(*(HDMAMemPointers [d] + 1), 0x2101 + p->BAddress);
            HDMAMemPointers [d] += 2;
            break;
         case 2:
         case 6:
#ifndef VAR_CYCLES
            CPU.Cycles += 3;
#else
            CPU.Cycles += 16;
#endif
            S9xSetPPU(*(HDMAMemPointers [d] + 0), 0x2100 + p->BAddress);
            S9xSetPPU(*(HDMAMemPointers [d] + 1), 0x2100 + p->BAddress);
            HDMAMemPointers [d] += 2;
            break;
         case 3:
         case 7:
#ifndef VAR_CYCLES
            CPU.Cycles += 6;
#else
            CPU.Cycles += 32;
#endif
            S9xSetPPU(*(HDMAMemPointers [d] + 0), 0x2100 + p->BAddress);
            S9xSetPPU(*(HDMAMemPointers [d] + 1), 0x2100 + p->BAddress);
            S9xSetPPU(*(HDMAMemPointers [d] + 2), 0x2101 + p->BAddress);
            S9xSetPPU(*(HDMAMemPointers [d] + 3), 0x2101 + p->BAddress);
            HDMAMemPointers [d] += 4;
            break;
         case 4:
#ifndef VAR_CYCLES
            CPU.Cycles += 6;
#else
            CPU.Cycles += 32;
#endif
            S9xSetPPU(*(HDMAMemPointers [d] + 0), 0x2100 + p->BAddress);
            S9xSetPPU(*(HDMAMemPointers [d] + 1), 0x2101 + p->BAddress);
            S9xSetPPU(*(HDMAMemPointers [d] + 2), 0x2102 + p->BAddress);
            S9xSetPPU(*(HDMAMemPointers [d] + 3), 0x2103 + p->BAddress);
            HDMAMemPointers [d] += 4;
            break;
         }
         if (!p->HDMAIndirectAddressing)
            p->Address += HDMA_ModeByteCounts [p->TransferMode];
         p->FirstLine = FALSE;
         p->LineCount--;
      }
   }
   return (byte);
}

void S9xResetDMA()
{
   int c, d;
   for (d = 0; d < 8; d++)
   {
      DMA [d].TransferDirection = FALSE;
      DMA [d].HDMAIndirectAddressing = FALSE;
      DMA [d].AAddressFixed = TRUE;
      DMA [d].AAddressDecrement = FALSE;
      DMA [d].TransferMode = 0xff;
      DMA [d].ABank = 0xff;
      DMA [d].AAddress = 0xffff;
      DMA [d].Address = 0xffff;
      DMA [d].BAddress = 0xff;
      DMA [d].TransferBytes = 0xffff;
   }
   for (c = 0x4300; c < 0x4380; c += 0x10)
   {
      for (d = c; d < c + 12; d++)
         Memory.FillRAM [d] = 0xff;

      Memory.FillRAM [c + 0xf] = 0xff;
   }
}
