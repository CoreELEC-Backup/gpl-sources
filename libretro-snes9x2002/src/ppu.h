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
#ifndef _PPU_H_
#define _PPU_H_

#include "rops.h"

#define FIRST_VISIBLE_LINE 1

extern uint8 GetBank;
extern uint16 SignExtend [2];

#define TILE_2BIT 0
#define TILE_4BIT 1
#define TILE_8BIT 2

#define MAX_2BIT_TILES 4096
#define MAX_4BIT_TILES 2048
#define MAX_8BIT_TILES 1024

#define PPU_H_BEAM_IRQ_SOURCE (1 << 0)
#define PPU_V_BEAM_IRQ_SOURCE (1 << 1)
#define GSU_IRQ_SOURCE     (1 << 2)
#define SA1_IRQ_SOURCE     (1 << 7)
#define SA1_DMA_IRQ_SOURCE (1 << 5)

typedef struct
{
   uint32  Count [6];
   uint32  Left [6][6];
   uint32  Right [6][6];
} ClipData;

typedef struct
{
   bool8  ColorsChanged;
   uint8  HDMA;
   bool8  HDMAStarted;
   uint8  MaxBrightness;
   bool8  LatchedBlanking;
   bool8  OBJChanged;
   bool8  RenderThisFrame;
   bool8  DirectColourMapsNeedRebuild;
   uint32 FrameCount;
   uint32 RenderedFramesCount;
   uint32 DisplayedRenderedFrameCount;
   uint32 SkippedFrames;
   uint32 FrameSkip;
   uint8*  TileCache [3];
   uint8*  TileCached [3];
   bool8  FirstVRAMRead;
   bool8  LatchedInterlace;
   bool8  DoubleWidthPixels;
   int    RenderedScreenHeight;
   int    RenderedScreenWidth;
   uint32 Red [256];
   uint32 Green [256];
   uint32 Blue [256];
   uint8*  XB;
   uint32 ScreenColors [256];
   int     PreviousLine;
   int     CurrentLine;
   int     Controller;
   uint32 Joypads[5];
   uint32 SuperScope;
   uint32 Mouse[2];
   int    PrevMouseX[2];
   int    PrevMouseY[2];
   ClipData Clip [2];
} InternalPPU;

typedef struct
{
   short  HPos;
   uint16 VPos;
   uint16 Name;
   uint8  VFlip;
   uint8  HFlip;
   uint8  Priority;
   uint8  Palette;
   uint8  Size;
} SOBJ;

typedef struct
{
   uint8  BGMode;
   uint8  BG3Priority;
   uint8  Brightness;

   struct
   {
      bool8 High;
      uint8 Increment;
      uint16 Address;
      uint16 Mask1;
      uint16 FullGraphicCount;
      uint16 Shift;
   } VMA;

   struct
   {
      uint16 SCBase;
      uint16 VOffset;
      uint16 HOffset;
      uint8 BGSize;
      uint16 NameBase;
      uint16 SCSize;
      bool8 OffsetsChanged; //-chg
   } BG [4];

   bool8  CGFLIP;
   uint16 CGDATA [256];
   uint8  FirstSprite;
   uint8  LastSprite;
   SOBJ OBJ [128];
   uint8  OAMPriorityRotation;
   uint16 OAMAddr;

   uint8  OAMFlip;
   uint16 OAMTileAddress;
   uint16 IRQVBeamPos;
   uint16 IRQHBeamPos;
   uint16 VBeamPosLatched;
   uint16 HBeamPosLatched;

   uint8  HBeamFlip;
   uint8  VBeamFlip;
   uint8  HVBeamCounterLatched;

   short  MatrixA;
   short  MatrixB;
   short  MatrixC;
   short  MatrixD;
   short  CentreX;
   short  CentreY;
   uint8  Joypad1ButtonReadPos;
   uint8  Joypad2ButtonReadPos;

   uint8  CGADD;
   uint8  FixedColourRed;
   uint8  FixedColourGreen;
   uint8  FixedColourBlue;
   uint16 SavedOAMAddr;
   uint16 ScreenHeight;
   uint32 WRAM;
   uint8  BG_Forced;
   bool8  ForcedBlanking;
   bool8  OBJThroughMain;
   bool8  OBJThroughSub;
   uint8  OBJSizeSelect;
   uint16 OBJNameBase;
   bool8  OBJAddition;
   uint8  OAMReadFlip;
   uint8  OAMData [512 + 32];
   bool8  VTimerEnabled;
   bool8  HTimerEnabled;
   short  HTimerPosition;
   uint8  Mosaic;
   bool8  BGMosaic [4];
   bool8  Mode7HFlip;
   bool8  Mode7VFlip;
   uint8  Mode7Repeat;
   uint8  Window1Left;
   uint8  Window1Right;
   uint8  Window2Left;
   uint8  Window2Right;
   uint8  ClipCounts [6];
   uint8  ClipWindowOverlapLogic [6];
   uint8  ClipWindow1Enable [6];
   uint8  ClipWindow2Enable [6];
   bool8  ClipWindow1Inside [6];
   bool8  ClipWindow2Inside [6];
   bool8  RecomputeClipWindows;
   uint8  CGFLIPRead;
   uint16 OBJNameSelect;
   bool8  Need16x8Mulitply;
   uint8  Joypad3ButtonReadPos;
   uint8  MouseSpeed[2];
   uint16 SavedOAMAddr2;
   uint16 OAMWriteRegister;
   uint8 BGnxOFSbyte;
} SPPU;

#define CLIP_OR 0
#define CLIP_AND 1
#define CLIP_XOR 2
#define CLIP_XNOR 3

typedef struct
{
   bool8  TransferDirection;
   bool8  AAddressFixed;
   bool8  AAddressDecrement;
   uint8  TransferMode;

   uint8  ABank;
   uint16 AAddress;
   uint16 Address;
   uint8  BAddress;

   // General DMA only:
   uint16 TransferBytes;

   // H-DMA only:
   bool8  HDMAIndirectAddressing;
   uint16 IndirectAddress;
   uint8  IndirectBank;
   uint8  Repeat;
   uint8  LineCount;
   uint8  FirstLine;
} SDMA;

//void S9xUpdateScreen ();
void S9xResetPPU();
void S9xFixColourBrightness();
void S9xUpdateJoypads();
void S9xProcessMouse(int which1);
void S9xSuperFXExec();

void S9xSetPPU(uint8 Byte, uint16 Address);
uint8 S9xGetPPU(uint16 Address);
void S9xSetCPU(uint8 Byte, uint16 Address);
uint8 S9xGetCPU(uint16 Address);

void S9xInitC4();
void S9xSetC4(uint8 Byte, uint16 Address);
uint8 S9xGetC4(uint16 Address);
void S9xSetC4RAM(uint8 Byte, uint16 Address);
uint8 S9xGetC4RAM(uint16 Address);

extern SPPU PPU;
extern SDMA DMA [8];
extern InternalPPU IPPU;

#include "gfx.h"
#include "memmap.h"

static INLINE uint8 REGISTER_4212()
{
   GetBank = 0;
   if (CPU.V_Counter >= PPU.ScreenHeight + FIRST_VISIBLE_LINE &&
         CPU.V_Counter < PPU.ScreenHeight + FIRST_VISIBLE_LINE + 3)
      GetBank = 1;

   GetBank |= CPU.Cycles >= Settings.HBlankStart ? 0x40 : 0;
   if (CPU.V_Counter >= PPU.ScreenHeight + FIRST_VISIBLE_LINE)
      GetBank |= 0x80; /* XXX: 0x80 or 0xc0 ? */

   return (GetBank);
}

/*
static INLINE void FLUSH_REDRAW ()
{
    if (IPPU.PreviousLine != IPPU.CurrentLine)
   S9xUpdateScreen ();
}
*/

#define FLUSH_REDRAW()     if (IPPU.PreviousLine != IPPU.CurrentLine) S9xUpdateScreen ()


static INLINE void REGISTER_2104(uint8 byte)
{
   if (PPU.OAMAddr & 0x100)
   {
      int addr = ((PPU.OAMAddr & 0x10f) << 1) + (PPU.OAMFlip & 1);
      if (byte != PPU.OAMData [addr])
      {
         SOBJ *pObj;
#ifdef   __DEBUG__
         printf("SetPPU_2104, PPU.OAMData. Byte : %x\n", byte);
#endif

         FLUSH_REDRAW();
         PPU.OAMData [addr] = byte;
         IPPU.OBJChanged = TRUE;

         // X position high bit, and sprite size (x4)
         pObj         = &PPU.OBJ [(addr & 0x1f) * 4];

         pObj->HPos   = (pObj->HPos & 0xFF) | SignExtend[(byte >> 0) & 1];
         pObj++->Size = byte & 2;
         pObj->HPos   = (pObj->HPos & 0xFF) | SignExtend[(byte >> 2) & 1];
         pObj++->Size = byte & 8;
         pObj->HPos   = (pObj->HPos & 0xFF) | SignExtend[(byte >> 4) & 1];
         pObj++->Size = byte & 32;
         pObj->HPos   = (pObj->HPos & 0xFF) | SignExtend[(byte >> 6) & 1];
         pObj->Size   = byte & 128;
      }
      PPU.OAMFlip ^= 1;
      if (!(PPU.OAMFlip & 1))
      {
         ++PPU.OAMAddr;
         PPU.OAMAddr &= 0x1ff;
      }
   }
   else if (!(PPU.OAMFlip & 1))
   {
      PPU.OAMWriteRegister &= 0xff00;
      PPU.OAMWriteRegister |= byte;
      PPU.OAMFlip |= 1;
   }
   else
   {
      int addr;
      uint8 lowbyte, highbyte;

      PPU.OAMWriteRegister &= 0x00ff;
      lowbyte               = (uint8)(PPU.OAMWriteRegister);
      highbyte              = byte;
      PPU.OAMWriteRegister |= byte << 8;

      addr = (PPU.OAMAddr << 1);

      if (lowbyte != PPU.OAMData [addr] ||
            highbyte != PPU.OAMData [addr + 1])
      {
         FLUSH_REDRAW();
#ifdef   __DEBUG__
         printf("SetPPU_2104, PPU.OAMData. Byte : %x\n", byte);
#endif

         PPU.OAMData [addr] = lowbyte;
         PPU.OAMData [addr + 1] = highbyte;
         IPPU.OBJChanged = TRUE;
         if (addr & 2)
         {
            // Tile
            PPU.OBJ[addr = PPU.OAMAddr >> 1].Name = PPU.OAMWriteRegister & 0x1ff;

            // priority, h and v flip.
            PPU.OBJ[addr].Palette = (highbyte >> 1) & 7;
            PPU.OBJ[addr].Priority = (highbyte >> 4) & 3;
            PPU.OBJ[addr].HFlip = (highbyte >> 6) & 1;
            PPU.OBJ[addr].VFlip = (highbyte >> 7) & 1;
         }
         else
         {
            // X position (low)
            PPU.OBJ[addr = PPU.OAMAddr >> 1].HPos &= 0xFF00;
            PPU.OBJ[addr].HPos |= lowbyte;

            // Sprite Y position
            PPU.OBJ[addr].VPos = highbyte;
         }
      }
      PPU.OAMFlip &= ~1;
      ++PPU.OAMAddr;
   }

   Memory.FillRAM [0x2104] = byte;
}

static INLINE void REGISTER_2118(uint8 Byte)
{
   uint32 address;
   if (PPU.VMA.FullGraphicCount)
   {
      uint32 rem = PPU.VMA.Address & PPU.VMA.Mask1;
      address = (((PPU.VMA.Address & ~PPU.VMA.Mask1) +
                  (rem >> PPU.VMA.Shift) +
                  ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3)) << 1) & 0xffff;
      Memory.VRAM [address] = Byte;
   }
   else
      Memory.VRAM[address = (PPU.VMA.Address << 1) & 0xFFFF] = Byte;
   IPPU.TileCached [TILE_2BIT][address >> 4] = FALSE;
   IPPU.TileCached [TILE_4BIT][address >> 5] = FALSE;
   IPPU.TileCached [TILE_8BIT][address >> 6] = FALSE;
   if (!PPU.VMA.High)
   {
#ifdef DEBUGGER
      if (Settings.TraceVRAM && !CPU.InDMA)
      {
         printf("VRAM write byte: $%04X (%d,%d)\n", PPU.VMA.Address,
                Memory.FillRAM[0x2115] & 3,
                (Memory.FillRAM [0x2115] & 0x0c) >> 2);
      }
#endif
      PPU.VMA.Address += PPU.VMA.Increment;
   }
   //    Memory.FillRAM [0x2118] = Byte;
}

static INLINE void REGISTER_2118_tile(uint8 Byte)
{
   uint32 address;
   uint32 rem = PPU.VMA.Address & PPU.VMA.Mask1;
   address = (((PPU.VMA.Address & ~PPU.VMA.Mask1) +
               (rem >> PPU.VMA.Shift) +
               ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3)) << 1) & 0xffff;
   Memory.VRAM [address] = Byte;
   IPPU.TileCached [TILE_2BIT][address >> 4] = FALSE;
   IPPU.TileCached [TILE_4BIT][address >> 5] = FALSE;
   IPPU.TileCached [TILE_8BIT][address >> 6] = FALSE;
   if (!PPU.VMA.High)
      PPU.VMA.Address += PPU.VMA.Increment;
   //    Memory.FillRAM [0x2118] = Byte;
}

static INLINE void REGISTER_2118_linear(uint8 Byte)
{
   uint32 address;
   Memory.VRAM[address = (PPU.VMA.Address << 1) & 0xFFFF] = Byte;
   IPPU.TileCached [TILE_2BIT][address >> 4] = FALSE;
   IPPU.TileCached [TILE_4BIT][address >> 5] = FALSE;
   IPPU.TileCached [TILE_8BIT][address >> 6] = FALSE;
   if (!PPU.VMA.High)
      PPU.VMA.Address += PPU.VMA.Increment;
   //    Memory.FillRAM [0x2118] = Byte;
}

static INLINE void REGISTER_2119(uint8 Byte)
{
   uint32 address;
   if (PPU.VMA.FullGraphicCount)
   {
      uint32 rem = PPU.VMA.Address & PPU.VMA.Mask1;
      address = ((((PPU.VMA.Address & ~PPU.VMA.Mask1) +
                   (rem >> PPU.VMA.Shift) +
                   ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3)) << 1) + 1) & 0xFFFF;
      Memory.VRAM [address] = Byte;
   }
   else
      Memory.VRAM[address = ((PPU.VMA.Address << 1) + 1) & 0xFFFF] = Byte;
   IPPU.TileCached [TILE_2BIT][address >> 4] = FALSE;
   IPPU.TileCached [TILE_4BIT][address >> 5] = FALSE;
   IPPU.TileCached [TILE_8BIT][address >> 6] = FALSE;
   if (PPU.VMA.High)
   {
#ifdef DEBUGGER
      if (Settings.TraceVRAM && !CPU.InDMA)
      {
         printf("VRAM write word: $%04X (%d,%d)\n", PPU.VMA.Address,
                Memory.FillRAM[0x2115] & 3,
                (Memory.FillRAM [0x2115] & 0x0c) >> 2);
      }
#endif
      PPU.VMA.Address += PPU.VMA.Increment;
   }
   //    Memory.FillRAM [0x2119] = Byte;
}

static INLINE void REGISTER_2119_tile(uint8 Byte)
{
   uint32 rem = PPU.VMA.Address & PPU.VMA.Mask1;
   uint32 address = ((((PPU.VMA.Address & ~PPU.VMA.Mask1) +
                       (rem >> PPU.VMA.Shift) +
                       ((rem & (PPU.VMA.FullGraphicCount - 1)) << 3)) << 1) + 1) & 0xFFFF;
   Memory.VRAM [address] = Byte;
   IPPU.TileCached [TILE_2BIT][address >> 4] = FALSE;
   IPPU.TileCached [TILE_4BIT][address >> 5] = FALSE;
   IPPU.TileCached [TILE_8BIT][address >> 6] = FALSE;
   if (PPU.VMA.High)
      PPU.VMA.Address += PPU.VMA.Increment;
   //    Memory.FillRAM [0x2119] = Byte;
}

static INLINE void REGISTER_2119_linear(uint8 Byte)
{
   uint32 address;
   Memory.VRAM[address = ((PPU.VMA.Address << 1) + 1) & 0xFFFF] = Byte;
   IPPU.TileCached [TILE_2BIT][address >> 4] = FALSE;
   IPPU.TileCached [TILE_4BIT][address >> 5] = FALSE;
   IPPU.TileCached [TILE_8BIT][address >> 6] = FALSE;
   if (PPU.VMA.High)
      PPU.VMA.Address += PPU.VMA.Increment;
   //    Memory.FillRAM [0x2119] = Byte;
}

#ifndef __OLD_RASTER_FX__
static INLINE void REGISTER_2122(uint8 Byte)
{
   // CG-RAM (palette) write
   
   if (PPU.CGFLIP)
   {
      if ((Byte & 0x7f) != (PPU.CGDATA[PPU.CGADD] >> 8))
      {
         PPU.CGDATA[PPU.CGADD] &= 0x00FF;
         PPU.CGDATA[PPU.CGADD] |= (Byte & 0x7f) << 8;
         if (!(Settings.os9x_hack & PPU_IGNORE_PALWRITE))
         {
#ifdef   __DEBUG__
            printf("SetPPU_2122, CG-RAM (palette) write. PPU.CFGFLIP. Byte : %x\n", Byte);
#endif
            ADD_ROP(ROP_PALETTE, PPU.CGADD | (PPU.CGDATA[PPU.CGADD] << 16));
         }
      }
      PPU.CGADD++;
   }
   else
   {
      if (Byte != (uint8)(PPU.CGDATA[PPU.CGADD] & 0xff))
      {
         PPU.CGDATA[PPU.CGADD] &= 0x7F00;
         PPU.CGDATA[PPU.CGADD] |= Byte;
         if (!(Settings.os9x_hack & PPU_IGNORE_PALWRITE))
         {
#ifdef   __DEBUG__
            printf("SetPPU_2122, CG-RAM (palette) write. !PPU.CFGFLIP. Byte : %x\n", Byte);
#endif
            ADD_ROP(ROP_PALETTE, PPU.CGADD | (PPU.CGDATA[PPU.CGADD] << 16));
         }
      }
   }
   PPU.CGFLIP ^= 1;
   //    Memory.FillRAM [0x2122] = Byte;
}

#else // __OLD_RASTER_FX__
static INLINE void REGISTER_2122(uint8 Byte)
{
   // CG-RAM (palette) write
   
   if (PPU.CGFLIP)
   {
      if ((Byte & 0x7f) != (PPU.CGDATA[PPU.CGADD] >> 8))
      {
         if (!(Settings.os9x_hack & PPU_IGNORE_PALWRITE))
         {
#ifdef   __DEBUG__
            printf("SetPPU_2122, CG-RAM (palette) write. PPU.CFGFLIP. Byte : %x\n", Byte);
#endif
            FLUSH_REDRAW();
         }
         PPU.CGDATA[PPU.CGADD] &= 0x00FF;
         PPU.CGDATA[PPU.CGADD] |= (Byte & 0x7f) << 8;
         IPPU.ColorsChanged = TRUE;
         IPPU.Blue [PPU.CGADD] = (Byte >> 2) & 0x1f;
         IPPU.Green [PPU.CGADD] = (PPU.CGDATA[PPU.CGADD] >> 5) & 0x1f;
         IPPU.ScreenColors [PPU.CGADD] = (uint16) BUILD_PIXEL(IPPU.XB[IPPU.Red[PPU.CGADD]],
                                         IPPU.XB[IPPU.Green[PPU.CGADD]],
                                         IPPU.XB[IPPU.Blue [PPU.CGADD]]);
      }
      PPU.CGADD++;
   }
   else
   {
      if (Byte != (uint8)(PPU.CGDATA[PPU.CGADD] & 0xff))
      {
         if (!(Settings.os9x_hack & PPU_IGNORE_PALWRITE))
         {
#ifdef   __DEBUG__
            printf("SetPPU_2122, CG-RAM (palette) write. !PPU.CFGFLIP. Byte : %x\n", Byte);
#endif
            FLUSH_REDRAW();
         }

         PPU.CGDATA[PPU.CGADD] &= 0x7F00;
         PPU.CGDATA[PPU.CGADD] |= Byte;
         IPPU.ColorsChanged = TRUE;
         IPPU.Red [PPU.CGADD] = Byte & 0x1f;
         IPPU.Green [PPU.CGADD] = (PPU.CGDATA[PPU.CGADD] >> 5) & 0x1f;
         IPPU.ScreenColors [PPU.CGADD] = (uint16) BUILD_PIXEL(IPPU.XB[IPPU.Red[PPU.CGADD]],
                                         IPPU.XB[IPPU.Green[PPU.CGADD]],
                                         IPPU.XB[IPPU.Blue [PPU.CGADD]]);
      }
   }
   PPU.CGFLIP ^= 1;
   //    Memory.FillRAM [0x2122] = Byte;
}

#endif

static INLINE void REGISTER_2180(uint8 Byte)
{
   Memory.RAM[PPU.WRAM++] = Byte;
   PPU.WRAM &= 0x1FFFF;
   Memory.FillRAM [0x2180] = Byte;
}
#endif
