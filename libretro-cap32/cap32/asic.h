/* Caprice32 - Amstrad CPC Emulator
   (c) Copyright 1997-2004 Ulrich Doewich

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef ASIC_H
#define ASIC_H

#include "cap32.h"

#define ASIC_SPRITES 16
#define ASIC_SPRITE_SIZE 16
#define ASIC_LOCK_SIZE 15
#define NB_DMA_CHANNELS 3
#define ASIC_RAM_INIT 0x4000
#define ASIC_RAM_END 0x7FFF

typedef struct {
   uint32_t source_address;
   uint32_t loop_address;
   uint8_t prescaler;
   // TODO: This must be set to false at CPC reset
   bool enabled;
   bool interrupt;
   int pause_ticks;
   uint8_t tick_cycles;
   int loops;
} t_DMA_channel;

typedef struct {
   t_DMA_channel ch[NB_DMA_CHANNELS];
   uint8_t dcsr;
   uint8_t clear;
} t_dma;

typedef struct {
   bool locked;
   int lock_seq_pos;
   int lock_prev_data;

   uint8_t rmr2;
   bool extend_border;
   int hscroll;
   int vscroll;
   uint8_t sprites[ASIC_SPRITES][ASIC_SPRITE_SIZE][ASIC_SPRITE_SIZE];
   int16_t sprites_x[ASIC_SPRITES];
   int16_t sprites_y[ASIC_SPRITES];
   int16_t sprites_mag_x[ASIC_SPRITES];
   int16_t sprites_mag_y[ASIC_SPRITES];

   t_dma dma;

   bool raster_interrupt;
   uint8_t interrupt_vector;
   int irq_cause;
   int irq_vector;
} t_asic;

extern t_asic asic;
extern uint8_t *pbRegisterPage;

void asic_reset();
void asic_poke_lock_sequence(uint8_t val);
void asic_dma_cycle();
bool asic_register_page_read(uint16_t addr, uint8_t* val);
bool asic_register_page_write(uint16_t addr, uint8_t val);
uint8_t asic_int();

#endif
