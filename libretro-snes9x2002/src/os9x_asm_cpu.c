#include "snes9x.h"
#include "apu.h"
#include "ppu.h"
#include "cpuexec.h"
//#include "cpuops.h"

#include "os9x_asm_cpu.h"

//#define __debug_c_irq__
//#define __debug_c_nmi__
//#define __debug_c_hblank__
//#define __debug_c_io__

void asm_S9xSetPCBase(uint32 Address)
{
#ifdef __debug_c_setpc__
   printf("spcb\n");
#endif
   S9xSetPCBase(Address);
}


#ifdef _C_GW_
#define PushW(w) \
    S9xSetWord (w, Registers.S.W - 1);\
    Registers.S.W -= 2;
#define PushB(b)\
    S9xSetByte (b, Registers.S.W--);


void asm_S9xMainLoop(void)
{
   //S9xPackStatus();
   //printf("asmMainLoop Enter(0x%08x).\n", CPU.Flags);
   asmMainLoop(&CPU);
   //printf("asmMainLoop Exit(0x%08x, %d).\n", CPU.PC - CPU.PCBase, CPU.Cycles);
   //S9xUnpackStatus();
}

void asm_S9xDoHBlankProcessing(void)
{
#ifdef __debug_c_hblank__
   printf("hblank\n");
#endif
   // S9xUnpackStatus(); // not needed
   S9xDoHBlankProcessing();
   // S9xPackStatus();
}


uint8 asm_S9xGetByte(uint32 Address)
{
#ifdef __debug_c_io__
   printf("gb\n");
#endif
   return S9xGetByte(Address);
}

uint16 asm_S9xGetWord(uint32 Address)
{
#ifdef __debug_c_io__
   printf("gw\n");
#endif
   return S9xGetWord(Address);
}


void asm_S9xSetByte(uint32 Address, uint8 value)
{
#ifdef __debug_c_io__
   printf("sb\n");
#endif
   S9xSetByte(value, Address);
}

void asm_S9xSetWord(uint32 Address, uint16 value)
{
#ifdef __debug_c_io__
   printf("sw\n");
#endif
   S9xSetWord(value, Address);
}


void asm_S9xOpcode_NMI(void)
{
#ifdef __debug_c_nmi__
   printf("nmi\n");
#endif
   // S9xUnpackStatus(); // not needed

   if (!CheckEmulation())
   {
      PushB(Registers.PB);
      PushW(CPU.PC - CPU.PCBase);
      // S9xPackStatus ();
      PushB(Registers.PL);
      ClearDecimal();
      SetIRQ();

      Registers.PB = 0;
      // c = 0; // unused
#ifdef USE_SA1
      if (Settings.SA1 && (Memory.FillRAM [0x2209] & 0x20))
         S9xSetPCBase(Memory.FillRAM [0x220c] |
                      (Memory.FillRAM [0x220d] << 8));
      else
#endif
         S9xSetPCBase(S9xGetWord(0xFFEA));
#ifdef VAR_CYCLES
      CPU.Cycles += TWO_CYCLES;
#else
      CPU.Cycles += 8;
#endif
   }
   else
   {
      PushW(CPU.PC - CPU.PCBase);
      // S9xPackStatus (); // not needed
      PushB(Registers.PL);
      ClearDecimal();
      SetIRQ();

      Registers.PB = 0;
      // ICPU.ShiftedPB = 0; // unused
#ifdef USE_SA1
      if (Settings.SA1 && (Memory.FillRAM [0x2209] & 0x20))
         S9xSetPCBase(Memory.FillRAM [0x220c] |
                      (Memory.FillRAM [0x220d] << 8));
      else
#endif
         S9xSetPCBase(S9xGetWord(0xFFFA));
#ifdef VAR_CYCLES
      CPU.Cycles += ONE_CYCLE;
#else
      CPU.Cycles += 6;
#endif
   }

   // S9xPackStatus(); // not needed
}

void asm_S9xOpcode_IRQ(void)
{
#ifdef __debug_c_irq__
   printf("irq\n");
#endif
   // S9xUnpackStatus(); // not needed

   if (!CheckEmulation())
   {
      PushB(Registers.PB);
      PushW(CPU.PC - CPU.PCBase);
      // S9xPackStatus (); // not needed
      PushB(Registers.PL);
      ClearDecimal();
      SetIRQ();

      Registers.PB = 0;
      // ICPU.ShiftedPB = 0; // unused

#ifdef USE_SA1
      if (Settings.SA1 && (Memory.FillRAM [0x2209] & 0x40))
         S9xSetPCBase(Memory.FillRAM [0x220e] |
                      (Memory.FillRAM [0x220f] << 8));
      else
#endif
         S9xSetPCBase(S9xGetWord(0xFFEE));
#ifdef VAR_CYCLES
      CPU.Cycles += TWO_CYCLES;
#else
      CPU.Cycles += 8;
#endif
   }
   else
   {
      PushW(CPU.PC - CPU.PCBase);
      // S9xPackStatus (); // not needed
      PushB(Registers.PL);
      ClearDecimal();
      SetIRQ();

      Registers.PB = 0;
      // ICPU.ShiftedPB = 0; // unused

#ifdef USE_SA1
      if (Settings.SA1 && (Memory.FillRAM [0x2209] & 0x40))
         S9xSetPCBase(Memory.FillRAM [0x220e] |
                      (Memory.FillRAM [0x220f] << 8));
      else
#endif
         S9xSetPCBase(S9xGetWord(0xFFFE));
#ifdef VAR_CYCLES
      CPU.Cycles += ONE_CYCLE;
#else
      CPU.Cycles += 6;
#endif
   }

   // S9xPackStatus(); // not needed
}
#endif
