/***************************************************************************

	Gaelco game hardware from 1991-1996

	Driver by Manuel Abadia

***************************************************************************/

#include "driver.h"
#include "vidhrdw/generic.h"
#include "cpu/m6809/m6809.h"
#include "cpu/m68000/m68000.h"

extern data16_t *gaelco_vregs;
extern data16_t *gaelco_videoram;
extern data16_t *gaelco_spriteram;

/* from vidhrdw/gaelco.c */
WRITE16_HANDLER( gaelco_vram_w );


#define TILELAYOUT8(NUM) static struct GfxLayout tilelayout8_##NUM =	\
{																		\
	8,8,									/* 8x8 tiles */				\
	NUM/8,									/* number of tiles */		\
	4,										/* bitplanes */				\
	{ 0*NUM*8, 1*NUM*8, 2*NUM*8, 3*NUM*8 }, /* plane offsets */			\
	{ 0,1,2,3,4,5,6,7 },												\
	{ 0*8,1*8,2*8,3*8,4*8,5*8,6*8,7*8 },								\
	8*8																	\
}

#define TILELAYOUT16(NUM) static struct GfxLayout tilelayout16_##NUM =				\
{																					\
	16,16,									/* 16x16 tiles */						\
	NUM/32,									/* number of tiles */					\
	4,										/* bitplanes */							\
	{ 0*NUM*8, 1*NUM*8, 2*NUM*8, 3*NUM*8 }, /* plane offsets */						\
	{ 0,1,2,3,4,5,6,7, 16*8+0,16*8+1,16*8+2,16*8+3,16*8+4,16*8+5,16*8+6,16*8+7 },	\
	{ 0*8,1*8,2*8,3*8,4*8,5*8,6*8,7*8, 8*8,9*8,10*8,11*8,12*8,13*8,14*8,15*8 },		\
	32*8																			\
}

#define GFXDECODEINFO(NUM,ENTRIES) static struct GfxDecodeInfo gfxdecodeinfo_##NUM[] =	\
{																						\
	{ REGION_GFX1, 0x000000, &tilelayout8_##NUM,0,	ENTRIES },							\
	{ REGION_GFX1, 0x000000, &tilelayout16_##NUM,0,	ENTRIES },							\
	{ -1 }																				\
}

/*============================================================================
							BIG KARNAK
  ============================================================================*/

VIDEO_START( bigkarnk );
VIDEO_UPDATE( bigkarnk );


static MEMORY_READ16_START( bigkarnk_readmem )
	{ 0x000000, 0x07ffff, MRA16_ROM },			/* ROM */
	{ 0x100000, 0x101fff, MRA16_RAM },			/* Video RAM */
	{ 0x102000, 0x103fff, MRA16_RAM },			/* Screen RAM */
	{ 0x200000, 0x2007ff, MRA16_RAM },			/* Palette */
	{ 0x440000, 0x440fff, MRA16_RAM },			/* Sprite RAM */
	{ 0x700000, 0x700001, input_port_0_word_r },/* DIPSW #1 */
	{ 0x700002, 0x700003, input_port_1_word_r },/* DIPSW #2 */
	{ 0x700004, 0x700005, input_port_2_word_r },/* INPUT #1 */
	{ 0x700006, 0x700007, input_port_3_word_r },/* INPUT #2 */
	{ 0x700008, 0x700009, input_port_4_word_r },/* Service + Test */
	{ 0xff8000, 0xffffff, MRA16_RAM },			/* Work RAM */
MEMORY_END

WRITE16_HANDLER( bigkarnk_sound_command_w )
{
	if (ACCESSING_LSB){
		soundlatch_w(0,data & 0xff);
		cpu_set_irq_line(1,M6809_FIRQ_LINE,HOLD_LINE);
	}
}

WRITE16_HANDLER( bigkarnk_coin_w )
{
	if (ACCESSING_LSB){
		switch ((offset >> 3)){
			case 0x00:	/* Coin Lockouts */
			case 0x01:
				coin_lockout_w( (offset >> 3) & 0x01, ~data & 0x01);
				break;
			case 0x02:	/* Coin Counters */
			case 0x03:
				coin_counter_w( (offset >> 3) & 0x01, data & 0x01);
				break;
		}
	}
}

static MEMORY_WRITE16_START( bigkarnk_writemem )
	{ 0x000000, 0x07ffff, MWA16_ROM },								/* ROM */
	{ 0x100000, 0x101fff, gaelco_vram_w, &gaelco_videoram },		/* Video RAM */
	{ 0x102000, 0x103fff, MWA16_RAM },								/* Screen RAM */
	{ 0x108000, 0x108007, MWA16_RAM, &gaelco_vregs },				/* Video Registers */
//	{ 0x10800c, 0x10800d, watchdog_reset_w },						/* INT 6 ACK/Watchdog timer */
	{ 0x200000, 0x2007ff, paletteram16_xBBBBBGGGGGRRRRR_word_w, &paletteram16 },/* Palette */
	{ 0x440000, 0x440fff, MWA16_RAM, &gaelco_spriteram },			/* Sprite RAM */
	{ 0x70000e, 0x70000f, bigkarnk_sound_command_w },				/* Triggers a FIRQ on the sound CPU */
	{ 0x70000a, 0x70003b, bigkarnk_coin_w },						/* Coin Counters + Coin Lockout */
	{ 0xff8000, 0xffffff, MWA16_RAM },								/* Work RAM */
MEMORY_END


static MEMORY_READ_START( bigkarnk_readmem_snd )
	{ 0x0000, 0x07ff, MRA_RAM },				/* RAM */
	{ 0x0800, 0x0801, OKIM6295_status_0_r },	/* OKI6295 */
	{ 0x0a00, 0x0a00, YM3812_status_port_0_r },	/* YM3812 */
	{ 0x0b00, 0x0b00, soundlatch_r },			/* Sound latch */
	{ 0x0c00, 0xffff, MRA_ROM },				/* ROM */
MEMORY_END

static MEMORY_WRITE_START( bigkarnk_writemem_snd )
	{ 0x0000, 0x07ff, MWA_RAM },				/* RAM */
	{ 0x0800, 0x0800, OKIM6295_data_0_w },		/* OKI6295 */
//	{ 0x0900, 0x0900, MWA_NOP },				/* enable sound output? */
	{ 0x0a00, 0x0a00, YM3812_control_port_0_w },/* YM3812 */
	{ 0x0a01, 0x0a01, YM3812_write_port_0_w },	/* YM3812 */
	{ 0x0c00, 0xffff, MWA_ROM },				/* ROM */
MEMORY_END

INPUT_PORTS_START( bigkarnk )
	PORT_START	/* DSW #1 */
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x0c, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, "Free Play (if Coin B too)" )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0xc0, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, "Free Play (if Coin A too)" )

	PORT_START	/* DSW #2 */
	PORT_DIPNAME( 0x07, 0x06, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x07, "0" )
	PORT_DIPSETTING(    0x06, "1" )
	PORT_DIPSETTING(    0x05, "2" )
	PORT_DIPSETTING(    0x04, "3" )
	PORT_DIPSETTING(    0x03, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x01, "6" )
	PORT_DIPSETTING(    0x00, "7" )
	PORT_DIPNAME( 0x18, 0x08, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x18, "1" )
	PORT_DIPSETTING(    0x10, "2" )
	PORT_DIPSETTING(    0x08, "3" )
	PORT_DIPSETTING(    0x00, "4" )
	PORT_DIPNAME( 0x20, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, "Impact" )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )

	PORT_START	/* 1P INPUTS & COINSW */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START	/* 2P INPUTS & STARTSW */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START	/* Service + Test */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_DIPNAME( 0x02, 0x02, "Go to test mode now" )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_BIT( 0xfc, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END


TILELAYOUT8(0x100000);
TILELAYOUT16(0x100000);

GFXDECODEINFO(0x100000,64);


static struct YM3812interface bigkarnk_ym3812_interface =
{
	1,						/* 1 chip */
	3580000,				/* 3.58 MHz? */
	{ 100 },					/* volume */
	{ 0 }					/* IRQ handler */
};

static struct OKIM6295interface bigkarnk_okim6295_interface =
{
	1,                  /* 1 chip */
	{ 8000 },			/* 8000 KHz? */
	{ REGION_SOUND1 },  /* memory region */
	{ 100 }				/* volume */
};


static MACHINE_DRIVER_START( bigkarnk )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000, 10000000)	/* MC68000P10, 10 MHz */
	MDRV_CPU_MEMORY(bigkarnk_readmem,bigkarnk_writemem)
	MDRV_CPU_VBLANK_INT(irq6_line_hold,1)

	MDRV_CPU_ADD(M6809, 8867000/4)	/* 68B09, 2.21675 MHz? */
	MDRV_CPU_FLAGS(CPU_AUDIO_CPU)
	MDRV_CPU_MEMORY(bigkarnk_readmem_snd,bigkarnk_writemem_snd)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)
	MDRV_INTERLEAVE(10)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*16, 32*16)
	MDRV_VISIBLE_AREA(0, 320-1, 16, 256-1)
	MDRV_GFXDECODE(gfxdecodeinfo_0x100000)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(bigkarnk)
	MDRV_VIDEO_UPDATE(bigkarnk)

	/* sound hardware */
	MDRV_SOUND_ADD(YM3812, bigkarnk_ym3812_interface)
	MDRV_SOUND_ADD(OKIM6295, bigkarnk_okim6295_interface)
MACHINE_DRIVER_END


ROM_START( bigkarnk )
	ROM_REGION( 0x080000, REGION_CPU1, 0 )	/* 68000 code */
	ROM_LOAD16_BYTE(	"d16",	0x000000, 0x040000, CRC(44fb9c73) SHA1(c33852b37afea15482f4a43cb045434660e7a056) )
	ROM_LOAD16_BYTE(	"d19",	0x000001, 0x040000, CRC(ff79dfdd) SHA1(2bfa440299317967ba2018d3a148291ae0c144ae) )

	ROM_REGION( 0x01e000, REGION_CPU2, 0 )	/* 6809 code */
	ROM_LOAD(	"d5",	0x000000, 0x010000, CRC(3b73b9c5) SHA1(1b1c5545609a695dab87d611bd53e0c3dd91e6b7) )

	ROM_REGION( 0x400000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "h5",	0x000000, 0x080000, CRC(20e239ff) SHA1(685059340f0f3a8e3c98702bd760dae685a58ddb) )
	ROM_RELOAD(		0x080000, 0x080000 )
	ROM_LOAD( "h10",0x100000, 0x080000, CRC(ab442855) SHA1(bcd69d4908ff8dc1b2215d2c2d2e54b950e0c015) )
	ROM_RELOAD(		0x180000, 0x080000 )
	ROM_LOAD( "h8",	0x200000, 0x080000, CRC(83dce5a3) SHA1(b4f9473e93c96f4b86c446e89d13fd3ef2b03996) )
	ROM_RELOAD(		0x280000, 0x080000 )
	ROM_LOAD( "h6",	0x300000, 0x080000, CRC(24e84b24) SHA1(c0ad6ce1e4b8aa7b9c9a3db8bb0165e90f4b48ed) )
	ROM_RELOAD(		0x380000, 0x080000 )

	ROM_REGION( 0x040000, REGION_SOUND1, 0 )	/* ADPCM samples - sound chip is OKIM6295 */
	ROM_LOAD( "d1",	0x000000, 0x040000, CRC(26444ad1) SHA1(804101b9bbb6e1b6d43a1e9d91737f9c3b27802a) )
ROM_END


/*============================================================================
					BIOMECHANICAL TOY & MANIAC SQUARE
  ============================================================================*/

VIDEO_START( maniacsq );
VIDEO_UPDATE( maniacsq );


static MEMORY_READ16_START( maniacsq_readmem )
	{ 0x000000, 0x0fffff, MRA16_ROM },			/* ROM */
	{ 0x100000, 0x101fff, MRA16_RAM },			/* Video RAM */
	{ 0x200000, 0x2007ff, MRA16_RAM },			/* Palette */
	{ 0x440000, 0x440fff, MRA16_RAM },			/* Sprite RAM */
	{ 0x700000, 0x700001, input_port_0_word_r },/* DIPSW #2 */
	{ 0x700002, 0x700003, input_port_1_word_r },/* DIPSW #1 */
	{ 0x700004, 0x700005, input_port_2_word_r },/* INPUT #1 */
	{ 0x700006, 0x700007, input_port_3_word_r },/* INPUT #2 */
	{ 0x70000e, 0x70000f, OKIM6295_status_0_lsb_r },/* OKI6295 status register */
	{ 0xff0000, 0xffffff, MRA16_RAM },			/* Work RAM */
MEMORY_END

static WRITE16_HANDLER( OKIM6295_bankswitch_w )
{
	unsigned char *RAM = memory_region(REGION_SOUND1);

	if (ACCESSING_LSB){
		memcpy(&RAM[0x30000], &RAM[0x40000 + (data & 0x0f)*0x10000], 0x10000);
	}
}

static MEMORY_WRITE16_START( maniacsq_writemem )
	{ 0x000000, 0x0fffff, MWA16_ROM },								/* ROM */
	{ 0x100000, 0x101fff, gaelco_vram_w, &gaelco_videoram },		/* Video RAM */
	{ 0x108000, 0x108007, MWA16_RAM, &gaelco_vregs },				/* Video Registers */
//	{ 0x10800c, 0x10800d, watchdog_reset_w },						/* INT 6 ACK/Watchdog timer */
	{ 0x200000, 0x2007ff, paletteram16_xBBBBBGGGGGRRRRR_word_w, &paletteram16 },/* Palette */
	{ 0x440000, 0x440fff, MWA16_RAM, &gaelco_spriteram },			/* Sprite RAM */
	{ 0x70000c, 0x70000d, OKIM6295_bankswitch_w },					/* OKI6295 bankswitch */
	{ 0x70000e, 0x70000f, OKIM6295_data_0_lsb_w },					/* OKI6295 data register */
	{ 0xff0000, 0xffffff, MWA16_RAM },								/* Work RAM */
MEMORY_END


INPUT_PORTS_START( maniacsq )

PORT_START	/* DSW #2 */
	PORT_SERVICE( 0x01, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x02, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x02, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x00, "Sound Type" )
	PORT_DIPSETTING(    0x00, "Stereo" )
	PORT_DIPSETTING(    0x08, "Mono" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x20, DEF_STR( On ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x40, "Easy" )
	PORT_DIPSETTING(    0xc0, "Normal" )
	PORT_DIPSETTING(    0x80, "Hard" )
	PORT_DIPSETTING(    0x00, "Hardest" )

PORT_START	/* DSW #1 */
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, "1C/1C or Free Play (if Coin A too)" )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x50, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, "1C/1C or Free Play (if Coin B too)" )

PORT_START	/* 1P INPUTS & COINSW */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

PORT_START	/* 2P INPUTS & STARTSW */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )
INPUT_PORTS_END


INPUT_PORTS_START( biomtoy )
	PORT_START	/* DSW #2 */
	PORT_SERVICE( 0x01, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x20, "0" )
	PORT_DIPSETTING(    0x10, "1" )
	PORT_DIPSETTING(    0x30, "2" )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x40, "Easy" )
	PORT_DIPSETTING(    0xc0, "Normal" )
	PORT_DIPSETTING(    0x80, "Hard" )
	PORT_DIPSETTING(    0x00, "Hardest" )

	PORT_START	/* DSW #1 */
	PORT_DIPNAME( 0x0f, 0x0f, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x0e, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x09, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x0a, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0x0f, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x0b, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x0d, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )
	PORT_DIPNAME( 0xf0, 0xf0, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0xe0, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x90, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0xa0, DEF_STR( 3C_2C ) )
	PORT_DIPSETTING(    0xf0, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x60, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x70, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0xb0, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0xd0, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(    0x50, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Free_Play ) )

	PORT_START	/* 1P INPUTS & COINSW */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START	/* 2P INPUTS & STARTSW */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )
INPUT_PORTS_END


static struct OKIM6295interface maniacsq_okim6295_interface =
{
	1,                  /* 1 chip */
	{ 8000 },			/* 8000 KHz? */
	{ REGION_SOUND1 },  /* memory region */
	{ 100 }				/* volume */
};

static MACHINE_DRIVER_START( maniacsq )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000,24000000/2)			/* 12 MHz */
	MDRV_CPU_MEMORY(maniacsq_readmem,maniacsq_writemem)
	MDRV_CPU_VBLANK_INT(irq6_line_hold,1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*16, 32*16)
	MDRV_VISIBLE_AREA(0, 320-1, 16, 256-1)
	MDRV_GFXDECODE(gfxdecodeinfo_0x100000)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_START(maniacsq)
	MDRV_VIDEO_UPDATE(maniacsq)

	/* sound hardware */
	MDRV_SOUND_ADD(OKIM6295, maniacsq_okim6295_interface)
MACHINE_DRIVER_END


ROM_START( maniacsp )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )	/* 68000 code */
	ROM_LOAD16_BYTE(	"d18",	0x000000, 0x020000, CRC(740ecab2) SHA1(8d8583364cc6aeea58ea2b9cb9a2aab2a43a44df) )
	ROM_LOAD16_BYTE(	"d16",	0x000001, 0x020000, CRC(c6c42729) SHA1(1aac9f93d47a4eb57e06e206e9f50e349b1817da) )

	ROM_REGION( 0x400000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "f3",	0x000000, 0x040000, CRC(e7f6582b) SHA1(9e352edf2f71d0edecb54a11ab3fd0e3ec867d42) )
	ROM_RELOAD(		0x080000, 0x040000 )
	/* 0x040000-0x07ffff and 0x0c0000-0x0fffff empty */
	ROM_LOAD( "f2",	0x100000, 0x040000, CRC(ca43a5ae) SHA1(8d2ed537be1dee60096a58b68b735fb50cab3285) )
	ROM_RELOAD(		0x180000, 0x040000 )
	/* 0x140000-0x17ffff and 0x1c0000-0x1fffff empty */
	ROM_LOAD( "f1",	0x200000, 0x040000, CRC(fca112e8) SHA1(2a1412f8f1c856b18b6cc7794191d327a415266f) )
	ROM_RELOAD(		0x280000, 0x040000 )
	/* 0x240000-0x27ffff and 0x2c0000-0x2fffff empty */
	ROM_LOAD( "f0",	0x300000, 0x040000, CRC(6e829ee8) SHA1(b602da8d987c1bafa41baf5d5e5d753e29ff5403) )
	ROM_RELOAD(		0x380000, 0x040000 )
	/* 0x340000-0x37ffff and 0x3c0000-0x3fffff empty */

	ROM_REGION( 0x140000, REGION_SOUND1, 0 )	/* ADPCM samples - sound chip is OKIM6295 */
	ROM_LOAD( "c1",	0x000000, 0x080000, CRC(2557f2d6) SHA1(3a99388f2d845281f73a427d6dc797dce87b2f82) )
	/* 0x00000-0x2ffff is fixed, 0x30000-0x3ffff is bank switched from all the ROMs */
	ROM_RELOAD(		0x040000, 0x080000 )
	ROM_RELOAD(		0x0c0000, 0x080000 )
ROM_END


ROM_START( biomtoy )
	ROM_REGION( 0x100000, REGION_CPU1, 0 )	/* 68000 code */
	ROM_LOAD16_BYTE(	"d18",	0x000000, 0x080000, CRC(4569ce64) SHA1(96557aca55779c23f7c2c11fddc618823c04ead0) )
	ROM_LOAD16_BYTE(	"d16",	0x000001, 0x080000, CRC(739449bd) SHA1(711a8ea5081f15dea6067577516c9296239c4145) )

	ROM_REGION( 0x400000, REGION_GFX1, ROMREGION_DISPOSE )
	/* weird gfx ordering */
	ROM_LOAD( "h6",		0x040000, 0x040000, CRC(9416a729) SHA1(425149b3041554579791fc23c09fda6be054e89d) )
	ROM_CONTINUE(		0x0c0000, 0x040000 )
	ROM_LOAD( "j6",		0x000000, 0x040000, CRC(e923728b) SHA1(113eac1de73c74ef7c9d3e2e72599a1ff775176d) )
	ROM_CONTINUE(		0x080000, 0x040000 )
	ROM_LOAD( "h7",		0x140000, 0x040000, CRC(9c984d7b) SHA1(98d43a9c3fa93c9ea55f41475ecab6ca25713087) )
	ROM_CONTINUE(		0x1c0000, 0x040000 )
	ROM_LOAD( "j7",		0x100000, 0x040000, CRC(0e18fac2) SHA1(acb0a3699395a6c68cacdeadda42a785aa4020f5) )
	ROM_CONTINUE(		0x180000, 0x040000 )
	ROM_LOAD( "h9",		0x240000, 0x040000, CRC(8c1f6718) SHA1(9377e838ebb1e16d24072b9b4ed278408d7a808f) )
	ROM_CONTINUE(		0x2c0000, 0x040000 )
	ROM_LOAD( "j9",		0x200000, 0x040000, CRC(1c93f050) SHA1(fabeffa05dae7a83a199a57022bd318d6ad02c4d) )
	ROM_CONTINUE(		0x280000, 0x040000 )
	ROM_LOAD( "h10",	0x340000, 0x040000, CRC(aca1702b) SHA1(6b36b230722270dbfc2f69bd7eb07b9e718db089) )
	ROM_CONTINUE(		0x3c0000, 0x040000 )
	ROM_LOAD( "j10",	0x300000, 0x040000, CRC(8e3e96cc) SHA1(761009f3f32b18139e98f20a22c433b6a49d9168) )
	ROM_CONTINUE(		0x380000, 0x040000 )

	ROM_REGION( 0x140000, REGION_SOUND1, 0 )	/* ADPCM samples - sound chip is OKIM6295 */
	ROM_LOAD( "c1",	0x000000, 0x080000, CRC(0f02de7e) SHA1(a8779370cc36290616794ff11eb3eebfdea5b1a9) )
	/* 0x00000-0x2ffff is fixed, 0x30000-0x3ffff is bank switched from all the ROMs */
	ROM_RELOAD(		0x040000, 0x080000 )
	ROM_LOAD( "c3",	0x0c0000, 0x080000, CRC(914e4bbc) SHA1(ca82b7481621a119f05992ed093b963da70d748a) )
ROM_END



GAME( 1991, bigkarnk, 0,        bigkarnk, bigkarnk, 0, ROT0, "Gaelco", "Big Karnak" )
GAME( 1995, biomtoy,  0,        maniacsq, biomtoy,  0, ROT0, "Gaelco", "Biomechanical Toy (unprotected)" )
GAME( 1996, maniacsp, maniacsq, maniacsq, maniacsq, 0, ROT0, "Gaelco", "Maniac Square (prototype)" )
