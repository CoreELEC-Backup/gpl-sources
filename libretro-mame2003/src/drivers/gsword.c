/* Great Swordsman (Taito) 1984

TODO:
- I haven't really tried to make Joshi Volley work. It's booting into
  service mode now, it might not be hard to fix it. It has only two Z80.


Credits:
- Steve Ellenoff: Original emulation and Mame driver
- Jarek Parchanski: Dip Switch Fixes, Color improvements, ADPCM Interface code
- Tatsuyuki Satoh: sound improvements, NEC 8741 emulation,adpcm improvements
- Charlie Miltenberger: sprite colors improvements & precious hardware
			information and screenshots

Trick:
If you want fight with ODILION swordsman patch program for 1st CPU
at these addresses, otherwise you won't never fight with him.

		ROM[0x2256] = 0
		ROM[0x2257] = 0
		ROM[0x2258] = 0
		ROM[0x2259] = 0
		ROM[0x225A] = 0


There are 3 Z80s and two AY-3-8910s..

Prelim memory map (last updated 6/15/98)
*****************************************
GS1		z80 Main Code	(8K)	0000-1FFF
Gs2     z80 Game Data   (8K)    2000-3FFF
Gs3     z80 Game Data   (8K)    4000-5FFF
Gs4     z80 Game Data   (8K)    6000-7FFF
Gs5     z80 Game Data   (4K)    8000-8FFF
Gs6     Sprites         (8K)
Gs7     Sprites         (8K)
Gs8		Sprites			(8K)
Gs10	Tiles			(8K)
Gs11	Tiles			(8K)
Gs12    3rd z80 CPU &   (8K)
        ADPCM Samples?
Gs13    ADPCM Samples?  (8K)
Gs14    ADPCM Samples?  (8K)
Gs15    2nd z80 CPU     (8K)    0000-1FFF
Gs16    2nd z80 Data    (8K)    2000-3FFF
*****************************************

**********
*Main Z80*
**********

	9000 - 9fff	Work Ram
        982e - 982e Free play
        98e0 - 98e0 Coin Input
        98e1 - 98e1 Player 1 Controls
        98e2 - 98e2 Player 2 Controls
        9c00 - 9c30 (Hi score - Scores)
        9c78 - 9cd8 (Hi score - Names)
        9e00 - 9e7f Sprites in working ram!
        9e80 - 9eff Sprite X & Y in working ram!

	a000 - afff	Sprite RAM & Video Attributes
        a000 - a37F	???
        a380 - a77F	Sprite Tile #s
        a780 - a7FF	Sprite Y & X positions
        a980 - a980	Background Tile Bank Select
        ab00 - ab00	Background Tile Y-Scroll register
        ab80 - abff	Sprite Attributes(X & Y Flip)

	b000 - b7ff	Screen RAM
	b800 - ffff	not used?!

PORTS:
7e 8741-#0 data port
7f 8741-#1 command / status port

*************
*2nd Z80 CPU*
*************
0000 - 3FFF ROM CODE
4000 - 43FF WORK RAM

write
6000 adpcm sound command for 3rd CPU

PORTS:
00 8741-#2 data port
01 8741-#2 command / status port
20 8741-#3 data port
21 8741-#3 command / status port
40 8741-#1 data port
41 8741-#1 command / status port

read:
60 fake port #0 ?
61 ay8910-#0 read port
data / ay8910-#0 read
80 fake port #1 ?
81 ay8910-#1 read port

write:
60 ay8910-#0 controll port
61 ay8910-#0 data port
80 ay8910-#1 controll port
81 ay8910-#1 data port
   ay8910-A  : NMI controll ?
a0 unknown
e0 unknown (watch dog?)

*************
*3rd Z80 CPU*
*************
0000-5fff ROM

read:
a000 adpcm sound command

write:
6000 MSM5205 reset and data

*************
I8741 communication data

reg: 0->1 (main->2nd) /     : (1->0) 2nd->main :
 0 : DSW.2 (port)           : DSW.1(port)
 1 : DSW.1                  : DSW.2
 2 : IN0 / sound error code :
 3 : IN1 / ?                :
 4 : IN2                    :
 4 : IN3                    :
 5 :                        :
 6 :                        : DSW0?
 7 :                        : ?

******************************************/

#include "driver.h"
#include "vidhrdw/generic.h"
#include "cpu/z80/z80.h"
#include "machine/tait8741.h"


extern WRITE_HANDLER( gsword_charbank_w );
extern WRITE_HANDLER( gsword_videoctrl_w );
extern WRITE_HANDLER( gsword_videoram_w );
extern WRITE_HANDLER( gsword_scroll_w );

extern PALETTE_INIT( josvolly );
extern PALETTE_INIT( gsword );
extern VIDEO_START( gsword );
extern VIDEO_UPDATE( gsword );

extern size_t gsword_spritexy_size;

extern UINT8 *gsword_spritexy_ram;
extern UINT8 *gsword_spritetile_ram;
extern UINT8 *gsword_spriteattrib_ram;

static int coins;
static int fake8910_0,fake8910_1;
static int gsword_nmi_step,gsword_nmi_count;


#if 0
static int gsword_coins_in(void)
{
	/* emulate 8741 coin slot */
	if (readinputport(4)&0xc0)
	{
		logerror("Coin In\n");
		return 0x80;
	}
	logerror("NO Coin\n");
	return 0x00;
}
#endif

static READ_HANDLER( gsword_8741_2_r )
{
	switch (offset)
	{
	case 0x01: /* start button , coins */
		return readinputport(0);
	case 0x02: /* Player 1 Controller */
		return readinputport(1);
	case 0x04: /* Player 2 Controller */
		return readinputport(3);
	default:
		logerror("8741-2 unknown read %d PC=%04x\n",offset,activecpu_get_pc());
	}
	/* unknown */
	return 0;
}

static READ_HANDLER( gsword_8741_3_r )
{
	switch (offset)
	{
	case 0x01: /* start button  */
		return readinputport(2);
	case 0x02: /* Player 1 Controller? */
		return readinputport(1);
	case 0x04: /* Player 2 Controller? */
		return readinputport(3);
	}
	/* unknown */
	logerror("8741-3 unknown read %d PC=%04x\n",offset,activecpu_get_pc());
	return 0;
}

static struct TAITO8741interface gsword_8741interface=
{
	4,         /* 4 chips */
	{ TAITO8741_MASTER,TAITO8741_SLAVE,TAITO8741_PORT,TAITO8741_PORT },  /* program mode */
	{ 1,0,0,0 },							     /* serial port connection */
	{ input_port_7_r,input_port_6_r,gsword_8741_2_r,gsword_8741_3_r }    /* port handler */
};

MACHINE_INIT( gsword )
{
	int i;

	for(i=0;i<4;i++) TAITO8741_reset(i);
	coins = 0;
	gsword_nmi_count = 0;
	gsword_nmi_step  = 0;

	TAITO8741_start(&gsword_8741interface);
}

static INTERRUPT_GEN( gsword_snd_interrupt )
{
	if( (gsword_nmi_count+=gsword_nmi_step) >= 4)
	{
		gsword_nmi_count = 0;
		cpu_set_irq_line(1, IRQ_LINE_NMI, PULSE_LINE);
	}
}

static WRITE_HANDLER( gsword_nmi_set_w )
{
	switch(data)
	{
	case 0x02:
		/* needed to disable NMI for memory check */
		gsword_nmi_step  = 0;
		gsword_nmi_count = 0;
		break;
	case 0x0d:
	case 0x0f:
		gsword_nmi_step  = 4;
		break;
	case 0xfe:
	case 0xff:
		gsword_nmi_step  = 4;
		break;
	}
	/* bit1= nmi disable , for ram check */
	logerror("NMI controll %02x\n",data);
}

static WRITE_HANDLER( gsword_AY8910_control_port_0_w )
{
	AY8910_control_port_0_w(offset,data);
	fake8910_0 = data;
}
static WRITE_HANDLER( gsword_AY8910_control_port_1_w )
{
	AY8910_control_port_1_w(offset,data);
	fake8910_1 = data;
}

static READ_HANDLER( gsword_fake_0_r )
{
	return fake8910_0+1;
}
static READ_HANDLER( gsword_fake_1_r )
{
	return fake8910_1+1;
}

WRITE_HANDLER( gsword_adpcm_data_w )
{
	MSM5205_data_w (0,data & 0x0f); /* bit 0..3 */
	MSM5205_reset_w(0,(data>>5)&1); /* bit 5    */
	MSM5205_vclk_w(0,(data>>4)&1);  /* bit 4    */
}

WRITE_HANDLER( adpcm_soundcommand_w )
{
	soundlatch_w(0,data);
	cpu_set_nmi_line(2, PULSE_LINE);
}

static MEMORY_READ_START( gsword_readmem )
	{ 0x0000, 0x8fff, MRA_ROM },
	{ 0x9000, 0x9fff, MRA_RAM },
	{ 0xb000, 0xb7ff, MRA_RAM },
MEMORY_END

static MEMORY_WRITE_START( gsword_writemem )
	{ 0x0000, 0x8fff, MWA_ROM },
	{ 0x9000, 0x9fff, MWA_RAM },
	{ 0xa380, 0xa3ff, MWA_RAM, &gsword_spritetile_ram },
	{ 0xa780, 0xa7ff, MWA_RAM, &gsword_spritexy_ram, &gsword_spritexy_size },
	{ 0xa980, 0xa980, gsword_charbank_w },
	{ 0xaa80, 0xaa80, gsword_videoctrl_w },	/* flip screen, char palette bank */
	{ 0xab00, 0xab00, gsword_scroll_w },
	{ 0xab80, 0xabff, MWA_RAM, &gsword_spriteattrib_ram },
	{ 0xb000, 0xb7ff, gsword_videoram_w, &videoram },
MEMORY_END

static MEMORY_READ_START( readmem_cpu2 )
	{ 0x0000, 0x3fff, MRA_ROM },
	{ 0x4000, 0x43ff, MRA_RAM },
MEMORY_END

static MEMORY_WRITE_START( writemem_cpu2 )
	{ 0x0000, 0x3fff, MWA_ROM },
	{ 0x4000, 0x43ff, MWA_RAM },
	{ 0x6000, 0x6000, adpcm_soundcommand_w },
MEMORY_END

static MEMORY_READ_START( readmem_cpu3 )
	{ 0x0000, 0x5fff, MRA_ROM },
	{ 0xa000, 0xa000, soundlatch_r },
MEMORY_END

static MEMORY_WRITE_START( writemem_cpu3 )
	{ 0x0000, 0x5fff, MWA_ROM },
	{ 0x8000, 0x8000, gsword_adpcm_data_w },
MEMORY_END

static PORT_READ_START( readport )
	{ 0x7e, 0x7f, TAITO8741_0_r },
PORT_END

static PORT_WRITE_START( writeport )
	{ 0x7e, 0x7f, TAITO8741_0_w },
PORT_END

static PORT_READ_START( readport_cpu2 )
	{ 0x00, 0x01, TAITO8741_2_r },
	{ 0x20, 0x21, TAITO8741_3_r },
	{ 0x40, 0x41, TAITO8741_1_r },
	{ 0x60, 0x60, gsword_fake_0_r },
	{ 0x61, 0x61, AY8910_read_port_0_r },
	{ 0x80, 0x80, gsword_fake_1_r },
	{ 0x81, 0x81, AY8910_read_port_1_r },
	{ 0xe0, 0xe0, IORP_NOP }, /* ?? */
PORT_END

static PORT_WRITE_START( writeport_cpu2 )
	{ 0x00, 0x01, TAITO8741_2_w },
	{ 0x20, 0x21, TAITO8741_3_w },
	{ 0x40, 0x41, TAITO8741_1_w },
	{ 0x60, 0x60, gsword_AY8910_control_port_0_w },
	{ 0x61, 0x61, AY8910_write_port_0_w },
	{ 0x80, 0x80, gsword_AY8910_control_port_1_w },
	{ 0x81, 0x81, AY8910_write_port_1_w },
	{ 0xa0, 0xa0, IOWP_NOP }, /* ?? */
	{ 0xe0, 0xe0, IOWP_NOP }, /* watch dog ?*/
PORT_END





static MEMORY_READ_START( josvolly_sound_readmem )
	{ 0x0000, 0x0fff, MRA_ROM },
//	{ 0x2000, 0x3fff, MRA_ROM }, another ROM probably, not sure which one (tested on boot)
	{ 0x4000, 0x43ff, MRA_RAM },
//	{ 0xa000, 0xa000, soundlatch_r },
MEMORY_END

static MEMORY_WRITE_START( josvolly_sound_writemem )
	{ 0x0000, 0x0fff, MWA_ROM },
	{ 0x4000, 0x43ff, MWA_RAM },
//	{ 0x8000, 0x8000, gsword_adpcm_data_w },
MEMORY_END

static PORT_READ_START( josvolly_sound_readport )
	{ 0x00, 0x00, AY8910_read_port_0_r },
	{ 0x40, 0x40, AY8910_read_port_1_r },
PORT_END

static PORT_WRITE_START( josvolly_sound_writeport )
	{ 0x00, 0x00, AY8910_control_port_0_w },
	{ 0x01, 0x01, AY8910_write_port_0_w },
	{ 0x40, 0x40, AY8910_control_port_1_w },
	{ 0x41, 0x41, AY8910_write_port_1_w },
PORT_END




INPUT_PORTS_START( gsword )
	PORT_START	/* IN0 (8741-2 port1?) */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT_IMPULSE( 0x80, IP_ACTIVE_HIGH, IPT_COIN1, 1 )
	PORT_START	/* IN1 (8741-2 port2?) */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_2WAY )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_2WAY )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON3 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT_IMPULSE( 0x80, IP_ACTIVE_HIGH, IPT_COIN1, 1 )
	PORT_START	/* IN2 (8741-3 port1?) */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT_IMPULSE( 0x80, IP_ACTIVE_HIGH, IPT_COIN1, 1 )
	PORT_START	/* IN3  (8741-3 port2?) */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_2WAY | IPF_COCKTAIL )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_2WAY | IPF_COCKTAIL )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON3 | IPF_COCKTAIL )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_COCKTAIL )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON2 | IPF_COCKTAIL )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT_IMPULSE( 0x80, IP_ACTIVE_HIGH, IPT_COIN1, 1 )
	PORT_START	/* IN4 (coins) */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT_IMPULSE( 0x80, IP_ACTIVE_HIGH, IPT_COIN1, 1 )

	PORT_START	/* DSW0 */
	/* NOTE: Switches 0 & 1, 6,7,8 not used 	 */
	/*	 Coins configurations were handled 	 */
	/*	 via external hardware & not via program */
	PORT_DIPNAME( 0x1c, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x1c, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x14, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_5C ) )

	PORT_START      /* DSW1 */
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x0c, 0x04, "Stage 1 Difficulty" )
	PORT_DIPSETTING(    0x00, "Easy" )
	PORT_DIPSETTING(    0x04, "Normal" )
	PORT_DIPSETTING(    0x08, "Hard" )
	PORT_DIPSETTING(    0x0c, "Hardest" )
	PORT_DIPNAME( 0x10, 0x10, "Stage 2 Difficulty" )
	PORT_DIPSETTING(    0x00, "Easy" )
	PORT_DIPSETTING(    0x10, "Hard" )
	PORT_DIPNAME( 0x20, 0x20, "Stage 3 Difficulty" )
	PORT_DIPSETTING(    0x00, "Easy" )
	PORT_DIPSETTING(    0x20, "Hard" )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, "Free Game Round" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START      /* DSW2 */
	PORT_DIPNAME( 0x01, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x01, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x30, 0x00, "Stage Begins" )
	PORT_DIPSETTING(    0x00, "Fencing" )
	PORT_DIPSETTING(    0x10, "Kendo" )
	PORT_DIPSETTING(    0x20, "Rome" )
	PORT_DIPSETTING(    0x30, "Kendo" )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )
INPUT_PORTS_END

static struct GfxLayout gsword_text =
{
	8,8,    /* 8x8 characters */
	1024,	/* 1024 characters */
	2,      /* 2 bits per pixel */
	{ 0, 4 },	/* the two bitplanes for 4 pixels are packed into one byte */
	{ 0, 1, 2, 3, 8*8+0, 8*8+1, 8*8+2, 8*8+3 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 },
	16*8    /* every char takes 16 bytes */
};

static struct GfxLayout gsword_sprites1 =
{
	16,16,   /* 16x16 sprites */
	64*2,    /* 128 sprites */
	2,       /* 2 bits per pixel */
	{ 0, 4 },	/* the two bitplanes for 4 pixels are packed into one byte */
	{ 0, 1, 2, 3, 8*8+0, 8*8+1, 8*8+2, 8*8+3,
			16*8+0, 16*8+1, 16*8+2, 16*8+3, 24*8+0, 24*8+1, 24*8+2, 24*8+3},
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			32*8, 33*8, 34*8, 35*8, 36*8, 37*8, 38*8, 39*8 },
	64*8     /* every sprite takes 64 bytes */
};

static struct GfxLayout gsword_sprites2 =
{
	32,32,    /* 32x32 sprites */
	64,       /* 64 sprites */
	2,       /* 2 bits per pixel */
	{ 0, 4 }, /* the two bitplanes for 4 pixels are packed into one byte */
	{ 0, 1, 2, 3, 8*8+0, 8*8+1, 8*8+2, 8*8+3,
			16*8+0, 16*8+1, 16*8+2, 16*8+3, 24*8+0, 24*8+1, 24*8+2, 24*8+3,
			64*8+0, 64*8+1, 64*8+2, 64*8+3, 72*8+0, 72*8+1, 72*8+2, 72*8+3,
			80*8+0, 80*8+1, 80*8+2, 80*8+3, 88*8+0, 88*8+1, 88*8+2, 88*8+3},
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			32*8, 33*8, 34*8, 35*8, 36*8, 37*8, 38*8, 39*8,
			128*8, 129*8, 130*8, 131*8, 132*8, 133*8, 134*8, 135*8,
			160*8, 161*8, 162*8, 163*8, 164*8, 165*8, 166*8, 167*8 },
	64*8*4    /* every sprite takes (64*8=16x6)*4) bytes */
};

static struct GfxDecodeInfo gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &gsword_text,         0, 64 },
	{ REGION_GFX2, 0, &gsword_sprites1,  64*4, 64 },
	{ REGION_GFX3, 0, &gsword_sprites2,  64*4, 64 },
	{ -1 } /* end of array */
};



static struct AY8910interface ay8910_interface =
{
	2,		/* 2 chips */
	1500000,	/* 1.5 MHz */
	{ 30, 30 },
	{ 0,0 },
	{ 0,0 },
	{ 0,gsword_nmi_set_w }, /* portA write */
	{ 0,0 }
};

static struct MSM5205interface msm5205_interface =
{
	1,				/* 1 chip             */
	384000,				/* 384KHz verified!   */
	{ 0 },				/* interrupt function */
	{ MSM5205_SEX_4B },		/* vclk input mode    */
	{ 60 }
};



static MACHINE_DRIVER_START( josvolly )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 3000000)
	MDRV_CPU_MEMORY(gsword_readmem,gsword_writemem)
	MDRV_CPU_PORTS(readport,writeport)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	MDRV_CPU_ADD(Z80, 3000000)
	MDRV_CPU_FLAGS(CPU_AUDIO_CPU)
	MDRV_CPU_MEMORY(josvolly_sound_readmem,josvolly_sound_writemem)
	MDRV_CPU_PORTS(josvolly_sound_readport,josvolly_sound_writeport)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)
	MDRV_MACHINE_INIT(gsword)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)

	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)
	MDRV_COLORTABLE_LENGTH(64*4+64*4)

	MDRV_PALETTE_INIT(josvolly)
	MDRV_VIDEO_START(gsword)
	MDRV_VIDEO_UPDATE(gsword)

	/* sound hardware */
	MDRV_SOUND_ADD(AY8910, ay8910_interface)
	MDRV_SOUND_ADD(MSM5205, msm5205_interface)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( gsword )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 3000000)
	MDRV_CPU_MEMORY(gsword_readmem,gsword_writemem)
	MDRV_CPU_PORTS(readport,writeport)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	MDRV_CPU_ADD(Z80, 3000000)
	MDRV_CPU_MEMORY(readmem_cpu2,writemem_cpu2)
	MDRV_CPU_PORTS(readport_cpu2,writeport_cpu2)
	MDRV_CPU_VBLANK_INT(gsword_snd_interrupt,4)

	MDRV_CPU_ADD(Z80, 3000000)
	MDRV_CPU_FLAGS(CPU_AUDIO_CPU)
	MDRV_CPU_MEMORY(readmem_cpu3,writemem_cpu3)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)
	MDRV_INTERLEAVE(200) /* Allow time for 2nd cpu to interleave*/

	MDRV_MACHINE_INIT(gsword)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)
	MDRV_COLORTABLE_LENGTH(64*4+64*4)

	MDRV_PALETTE_INIT(gsword)
	MDRV_VIDEO_START(gsword)
	MDRV_VIDEO_UPDATE(gsword)

	/* sound hardware */
	MDRV_SOUND_ADD(AY8910, ay8910_interface)
	MDRV_SOUND_ADD(MSM5205, msm5205_interface)
MACHINE_DRIVER_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( josvolly )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64K for main CPU */
	ROM_LOAD( "aa2-1.2c",     0x0000, 0x2000, CRC(27f740a5) SHA1(3e038386e743fdf718e795a944ff4b631a492958) )
	ROM_LOAD( "aa1-2.2d",     0x2000, 0x2000, CRC(3e02e3e1) SHA1(cc0aee321cf5232438cd6e38635c9060056ad361) )
	ROM_LOAD( "aa0-3.2e",     0x4000, 0x2000, CRC(72843ffe) SHA1(fe70727bbcb0622df81eca2969c1a85398767479) )
	ROM_LOAD( "aa1-4.2f",     0x6000, 0x2000, CRC(22c1466e) SHA1(d86093903e473252c35170e35d7f9ee34194086d) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64K for 2nd CPU */
	ROM_LOAD( "aa3-12.2h",    0x0000, 0x1000, CRC(3796bbf6) SHA1(8741f556ddb06e7779d1e8abc3d06688881f8269) )

	ROM_REGION( 0x04000, REGION_USER1, 0 )	/* music data and samples - not sure where it's mapped */
	ROM_LOAD( "aa0-13.2j",    0x0000, 0x2000, CRC(58cc89ac) SHA1(9785ec27e593b3e249da7a1b6b025c6d573e28f9) )
	ROM_LOAD( "aa0-14.4j",    0x2000, 0x2000, CRC(436fe91f) SHA1(feb29501090c6db911e13ce6e9935ba004b0ce7e) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "aa0-10.9n",    0x0000, 0x2000, CRC(207c4f42) SHA1(4cf2922d55cfc9e68cc07c3252ea3b5619b8aca5) )	/* tiles */
	ROM_LOAD( "aa1-11.9p",    0x2000, 0x1000, CRC(c130464a) SHA1(9d23577b8aaaffeefff3d8f93668d1b2bd0ba3d9) )

	ROM_REGION( 0x2000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "aa0-6.9e",     0x0000, 0x2000, CRC(c2c2401a) SHA1(ef987d53d9e502277086f39b455174d3539572e6) )	/* sprites */

	ROM_REGION( 0x4000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "aa0-7.9f",     0x0000, 0x2000, CRC(da836231) SHA1(209723778b705dba8206b56c3b8f0996f02ba8d5) )
	ROM_LOAD( "aa0-8.9h",     0x2000, 0x2000, CRC(a0426d57) SHA1(d029408e005ea57f4902c081203f3d3980a5f927) )

	ROM_REGION( 0x0460, REGION_PROMS, 0 )
	ROM_LOAD( "a1.10k",       0x0000, 0x0100, CRC(09f7b56a) SHA1(9b82d1d4ebab14b366dc0ca95c933e37811ac155) )	/* palette red? */
	ROM_LOAD( "a2.9k",        0x0100, 0x0100, CRC(852eceac) SHA1(6ed7011b45cf767d6503b92d29a14a7b8e099a76) )	/* palette green? */
	ROM_LOAD( "a3.9j",        0x0200, 0x0100, CRC(1312718b) SHA1(4a7d7eae4d8ea085eead46758832fddac7aff0b0) )	/* palette blue? */
	ROM_LOAD( "a4.8c",        0x0300, 0x0100, CRC(1dcec967) SHA1(4d36842c2fd929a6508a58bc8ea7e0372296e575) )	/* sprite lookup table */
	ROM_LOAD( "003.4e",       0x0400, 0x0020, CRC(43a548b8) SHA1(d01529d7f8f5101232cdf3490fdb2c61bf179181) )	/* address decoder? not used */
	ROM_LOAD( "004.4d",       0x0420, 0x0020, CRC(43a548b8) SHA1(d01529d7f8f5101232cdf3490fdb2c61bf179181) )	/* address decoder? not used */
	ROM_LOAD( "005.3h",       0x0440, 0x0020, CRC(e8d6dec0) SHA1(d15cba9a4b24255d41046b15c2409391ab13ce95) )	/* address decoder? not used */
ROM_END

ROM_START( gsword )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64K for main CPU */
	ROM_LOAD( "gs1",          0x0000, 0x2000, CRC(565c4d9e) SHA1(17b86e86ab95aeb458b8368c8c04666a1ccd9eee) )
	ROM_LOAD( "gs2",          0x2000, 0x2000, CRC(d772accf) SHA1(08028c6f026c118cc375ecff5c24dcb549475633) )
	ROM_LOAD( "gs3",          0x4000, 0x2000, CRC(2cee1871) SHA1(df099209c56f2807e4fdb83c625368f5e7e583e5) )
	ROM_LOAD( "gs4",          0x6000, 0x2000, CRC(ca9d206d) SHA1(887eedc4e10218bf149c84399edd5d1e32c85051) )
	ROM_LOAD( "gs5",          0x8000, 0x1000, CRC(2a892326) SHA1(a2cd91263714480c2569d3bbc73d62d222175e89) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* 64K for 2nd CPU */
	ROM_LOAD( "gs15",         0x0000, 0x2000, CRC(1aa4690e) SHA1(7b0dbc38f3e6af2c9efa44b6759a3cdd9adc992d) )
	ROM_LOAD( "gs16",         0x2000, 0x2000, CRC(10accc10) SHA1(311961bfe852582a9c66aaecf9bc4c8f0ac7fccf) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 )	/* 64K for 3nd z80 */
	ROM_LOAD( "gs12",         0x0000, 0x2000, CRC(a6589068) SHA1(9385abe2449c5c5bac8f49d2afd140acea1791c3) )
	ROM_LOAD( "gs13",         0x2000, 0x2000, CRC(4ee79796) SHA1(3353625903f63910a18fae0a9568a96d75592328) )
	ROM_LOAD( "gs14",         0x4000, 0x2000, CRC(455364b6) SHA1(ebabf077d1ba113c13e7620d61720ed141acb5ad) )

	ROM_REGION( 0x4000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "gs10",         0x0000, 0x2000, CRC(517c571b) SHA1(05572a8ea416922da50143936fda9ba038f0b91e) )	/* tiles */
	ROM_LOAD( "gs11",         0x2000, 0x2000, CRC(7a1d8a3a) SHA1(3f90be9ddba3cf7a879fd69ac67c2b67fd63b9ee) )

	ROM_REGION( 0x2000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "gs6",          0x0000, 0x2000, CRC(1b0a3cb7) SHA1(0b0f17b9844d7310b46110559e09cfc3b50bb38b) )	/* sprites */

	ROM_REGION( 0x4000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "gs7",          0x0000, 0x2000, CRC(ef5f28c6) SHA1(85d943e5c5136d9458118f676b0c79fcf3aaf0c4) )
	ROM_LOAD( "gs8",          0x2000, 0x2000, CRC(46824b30) SHA1(f6880b1c31ae795e3781d16ee96145df1db60328) )

	ROM_REGION( 0x0360, REGION_PROMS, 0 )
	ROM_LOAD( "ac0-1.bpr",    0x0000, 0x0100, CRC(5c4b2adc) SHA1(0a6fdd60bdbd56bb7573147e4a976e5d0ddf43b5) )	/* palette low bits */
	ROM_LOAD( "ac0-2.bpr",    0x0100, 0x0100, CRC(966bda66) SHA1(05439508113b3e51a16ee87d3f4691aa8901ebcb) )	/* palette high bits */
	ROM_LOAD( "ac0-3.bpr",    0x0200, 0x0100, CRC(dae13f77) SHA1(d4d105542955e806311987dd3c4ffce1e13caf91) )	/* sprite lookup table */
	ROM_LOAD( "003",          0x0300, 0x0020, CRC(43a548b8) SHA1(d01529d7f8f5101232cdf3490fdb2c61bf179181) )	/* address decoder? not used */
	ROM_LOAD( "004",          0x0320, 0x0020, CRC(43a548b8) SHA1(d01529d7f8f5101232cdf3490fdb2c61bf179181) )	/* address decoder? not used */
	ROM_LOAD( "005",          0x0340, 0x0020, CRC(e8d6dec0) SHA1(d15cba9a4b24255d41046b15c2409391ab13ce95) )	/* address decoder? not used */
ROM_END



static DRIVER_INIT( gsword )
{
	UINT8 *ROM2 = memory_region(REGION_CPU2);

	ROM2[0x1da] = 0xc3; /* patch for rom self check */
	ROM2[0x726] = 0;    /* patch for sound protection or time out function */
	ROM2[0x727] = 0;
}


GAMEX( 1983, josvolly, 0, josvolly, gsword, 0,      ROT90, "Taito Corporation", "Joshi Volleyball", GAME_NOT_WORKING )
GAMEX( 1984, gsword,   0, gsword,   gsword, gsword, ROT0,  "Taito Corporation", "Great Swordsman", GAME_IMPERFECT_COLORS )
