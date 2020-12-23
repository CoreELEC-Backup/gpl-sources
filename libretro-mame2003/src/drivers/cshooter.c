/* Cross Shooter (c) 1987 Seibu */

/*

Haze's notes :

  - interrupts are probably wrong .. it ends up writing to rom etc.
  - how do the sprites / bg's work .. these big black unknown things on the pcb
    also sound a bit disturbing, what are they?
  - i can't decode the other gfx? rom
  - there don't seem to be any sprites / bg's in ram, interrupts?
  - palette? format isn't understood
  - the other sets ('cshootre' and 'airraid') need decrypting ..
    is the main one protected ? theres a 68705 on it


Stephh's notes (based on the game Z80 code and some tests) :

  - Memory notes (100% guess) :

      * There are still some writes to the ROM area, but I think it's
        related to wrong interrupts and/or incomplete memory mapping.
      * Reads from 0xb0?? seem to be related to sound
      * Write to 0xc500 happens LOTS of time - related to scanlines ?
      * Write to 0xc600 might be used to disable the interrupts and
        the possible communication between CPUs (if they are 2)
      * Write to 0xc700 seems to be done when a coin is inserted
        (also done once during P.O.S.T. - unknown purpose here).
      * Write to 0xc801 might be sort of watchdog as it "pollutes"
        the error.log file.


  - Interrupts notes :

      * I think that they aren't handled correctly : after a few frames,
        the number of lives are reset to 0, causing a "GAME OVER" 8(
			* - or is this protection from the 68705, haze


  - Inputs notes :

      * COINx don't work correcly : see "cshooter_coin_r" read handler.
	* In game, bits 3 and 4 of 0xc202 ("START") are tested,
        while bits 4 and 5 are tested in the "test mode".
      * Pressing STARTx while in game adds lives (depending on the
        "Lives" Dip Switch) for player x.


  - Other notes :

      * 0x0006 contains the "region" stuff (value in 'cshooter' = 0xc4) :
          . bits 2 and 3 determine the manufacturer :
              0x00 : "J.K.H. Corp."         (no logo)
              0x04 : "Taito Corporation."   (+ logo)
              0x08 : "International Games"  (+ logo)
              0x0c : "Seibu Kaihatsu,Inc."  (+ logo)
          . bits 6 and 7 determine the title screen :
              0x00 : unknown - scrambled GFX *probably air raid, haze
              0x40 : unknown - scrambled GFX (alternate entry) *probably air raid, haze
              0x80 : "Cross Shooter"
              0xc0 : "Cross Shooter" (same as 0x80)


  - Addresses :

      * 0xe222 : contents of DSW1 (0xc204)
      * 0xe223 : contents of DSW2 (0xc203)
      * 0xe228 : difficulty (from DSW2)
      * 0xe229 : bonus life (from DSW2 - table at 0x6264)
      * 0xe22b : lives      (from DSW2 - table at 0x7546)
      * 0xe234 : credits (0x00-0xff, even if display is limited to 9)
          . if 1 coin slot , total credits
          . if 2 coin slots, credits for player 1
      * 0xe235 : credits (0x00-0xff, even if display is limited to 9)
          . if 1 coin slot , always 0x00 !
          . if 2 coin slots, credits for player 2
      * 0xe237 : lives for player 1
      * 0xe238 : lives for player 2

*/

#include "driver.h"
#include "sndhrdw/seibu.h"


/* vidhrdw */

data8_t* cshooter_txram;
static struct tilemap *cshooter_txtilemap;

static void get_cstx_tile_info(int tile_index)
{
	int code = (cshooter_txram[tile_index*2]);
	int attr = (cshooter_txram[tile_index*2+1]);
	int rg;
	rg=0;
	if (attr & 0x20) rg = 1;

	SET_TILE_INFO(

			rg,
			code & 0x1ff,
			attr&0x1f,
			0)
}

WRITE_HANDLER(cshooter_txram_w)
{
	cshooter_txram[offset] = data;
	tilemap_mark_tile_dirty(cshooter_txtilemap,offset/2);
}

VIDEO_START(cshooter)
{
	cshooter_txtilemap = tilemap_create(get_cstx_tile_info,tilemap_scan_rows,TILEMAP_OPAQUE,8,8,32, 32);

	return 0;
}

VIDEO_UPDATE(cshooter)
{
	tilemap_draw(bitmap,cliprect,cshooter_txtilemap,0,0);
}


/* main cpu */

INTERRUPT_GEN( cshooter_interrupt )
{
	cpu_set_irq_line_and_vector(0, 0, HOLD_LINE, 0x08); // almost certainly wrong?
}


static int cshooter_counter;

static MACHINE_INIT( cshooter )
{
	cshooter_counter = 0;
}

READ_HANDLER ( cshooter_coin_r )
{
	/* Even reads must return 0xff - Odd reads must return the contents of input port 5.
	   Code at 0x5061 is executed once during P.O.S.T. where there is one read.
	   Code at 0x50b4 is then executed each frame (not sure) where there are 2 reads. */

	return ( (cshooter_counter++ & 1) ? 0xff : input_port_5_r(0) );
}

WRITE_HANDLER ( cshooter_c500_w )
{
}

WRITE_HANDLER ( cshooter_c700_w )
{
}


static MEMORY_READ_START( readmem )
	{ 0x0000, 0x7fff, MRA_ROM },
	{ 0x8000, 0x87ff, MRA_RAM },			// to be confirmed
	{ 0xb000, 0xb0ff, MRA_RAM },			// sound related ?
	{ 0xc000, 0xc1ff, MRA_RAM },
	{ 0xc200, 0xc200, input_port_0_r },
	{ 0xc201, 0xc201, input_port_1_r },
	{ 0xc202, 0xc202, input_port_2_r },
	{ 0xc203, 0xc203, input_port_3_r },
	{ 0xc204, 0xc204, input_port_4_r },
	{ 0xc205, 0xc205, cshooter_coin_r },	// hack until I understand
	{ 0xd000, 0xd7ff, MRA_RAM },
	{ 0xd800, 0xdfff, MRA_RAM },
	{ 0xe000, 0xffff, MRA_RAM },
MEMORY_END

static MEMORY_WRITE_START( writemem )
	{ 0x0000, 0x7fff, MWA_ROM },
	{ 0x8000, 0x87ff, MWA_RAM },			// to be confirmed
	{ 0xc000, 0xc1ff, paletteram_BBGGGRRR_w, &paletteram },	// guess, maybe not
	{ 0xc500, 0xc500, cshooter_c500_w },
	{ 0xc600, 0xc600, MWA_NOP },			// see notes
	{ 0xc700, 0xc700, cshooter_c700_w },
	{ 0xc801, 0xc801, MWA_NOP },			// see notes
	{ 0xd000, 0xd7ff, cshooter_txram_w, &cshooter_txram },
	{ 0xd800, 0xdfff, MWA_RAM },
	{ 0xe000, 0xffff, MWA_RAM },
MEMORY_END

static PORT_READ_START( readport )
PORT_END

static PORT_WRITE_START( writeport )
PORT_END


/* Sound CPU */

static MEMORY_READ_START( s_readmem )
	{ 0x0000, 0x3fff, MRA_ROM },
	{ 0xf800, 0xffff, MRA_RAM },
MEMORY_END

static MEMORY_WRITE_START( s_writemem )
	{ 0x0000, 0x3fff, MWA_ROM },
	{ 0xc000, 0xc000, MWA_NOP }, // YM2203_control_port_0_w ?
	{ 0xc001, 0xc001, MWA_NOP }, // YM2203_write_port_0_w
	{ 0xc800, 0xc800, MWA_NOP }, // YM2203_control_port_1_w ?
	{ 0xc801, 0xc801, MWA_NOP }, // YM2203_write_port_1_w
	{ 0xf800, 0xffff, MWA_RAM },
MEMORY_END

static PORT_READ_START( s_readport )
PORT_END

static PORT_WRITE_START( s_writeport )
PORT_END


INPUT_PORTS_START( cshooter )
	PORT_START	/* IN0	(0xc200) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* IN1	(0xc201) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_COCKTAIL )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_COCKTAIL )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_COCKTAIL )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_COCKTAIL )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_COCKTAIL )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_COCKTAIL )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* START	(0xc202) */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT_IMPULSE( 0x08, IP_ACTIVE_LOW, IPT_START1, 1 )
	PORT_BIT_IMPULSE( 0x10, IP_ACTIVE_LOW, IPT_START2, 1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* DSW2	(0xc203) */
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x03, "Easy" )
	PORT_DIPSETTING(    0x02, "Medium" )
	PORT_DIPSETTING(    0x01, "Hard" )
	PORT_DIPSETTING(    0x00, "Hardest" )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x0c, "2k 10k 20k" )
	PORT_DIPSETTING(    0x08, "5k 20k 40k" )
	PORT_DIPSETTING(    0x04, "6k 30k 60k" )
	PORT_DIPSETTING(    0x00, "7k 40k 80k" )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x20, "1" )
	PORT_DIPSETTING(    0x10, "2" )
	PORT_DIPSETTING(    0x30, "3" )
	PORT_DIPSETTING(    0x00, "4" )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START	/* DSW1	(0xc204) */
	PORT_DIPNAME( 0x01, 0x01, "Coin Slots" )
	PORT_DIPSETTING(    0x01, "1" )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_SERVICE( 0x02, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x04, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x38, 0x38, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x38, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x28, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_5C ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unused ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START	/* COIN	(0xc205) */
	PORT_BIT_IMPULSE( 0x01, IP_ACTIVE_LOW, IPT_COIN1, 1 )
	PORT_BIT_IMPULSE( 0x02, IP_ACTIVE_LOW, IPT_COIN2, 1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END


static struct GfxLayout cshooter_charlayout =
{
	8,8,		/* 8*8 characters */
	RGN_FRAC(1,1),		/* 512 characters */
	2,			/* 4 bits per pixel */
	{ 0,4 },
	{ 8,9,10,11,0,1,2,3 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	128*2
};

static struct GfxLayout cshooter_unknownlayout =
{
	8,8,		/* 8*8 characters */
	RGN_FRAC(1,1),		/* 512 characters */
	4,			/* 4 bits per pixel */
	{ 0,8,16,24 },
	{ 0,1,2,3,4,5,6,7 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8
};

static struct GfxDecodeInfo gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0,     &cshooter_charlayout,   0, 1  },
	{ REGION_GFX1, 128/8, &cshooter_charlayout,   0, 1  },
	{ REGION_GFX2, 0,     &cshooter_unknownlayout,   0, 1  },
	{ -1 } /* end of array */
};

static MACHINE_DRIVER_START( cshooter )
	MDRV_CPU_ADD(Z80,6000000)		 /* ? MHz */
	MDRV_CPU_MEMORY(readmem,writemem)
	MDRV_CPU_PORTS(readport,writeport)
	MDRV_CPU_VBLANK_INT(cshooter_interrupt,1)

	MDRV_CPU_ADD(Z80,6000000)		 /* ? MHz */
	MDRV_CPU_MEMORY(s_readmem,s_writemem)
	MDRV_CPU_PORTS(s_readport,s_writeport)
//	MDRV_CPU_VBLANK_INT(cshooter_interrupt,1)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	MDRV_MACHINE_INIT(cshooter)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER )
	MDRV_SCREEN_SIZE(256, 256)
	MDRV_VISIBLE_AREA(0, 256-1, 16, 256-16-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(0x1000)

	MDRV_VIDEO_START(cshooter)
	MDRV_VIDEO_UPDATE(cshooter)

	/* sound hardware */
MACHINE_DRIVER_END


/*

-----------------------------
Cross Shooter by TAITO (1987)
-----------------------------
malcor




Location    Type     File ID   Checksum
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
LB 4U       27256      R1        C2F0   [  main program  ]
LB 2U       27512      R2        74EA   [  program/GFX   ]
TB 11A      2764       R3        DFA7   [     fix?       ]
LB 5C       27256      R4        D7B8   [ sound program  ]
LB 7A       82S123     0.BPR     00A1   [   forgrounds   ]
LB 9S       82S129     1.BPR     0194   [ motion objects ]
TB 4E       82S129     2.BPR     00DC   [ motion objects ]
LB J3       68705


Notes:   LB - CPU board        S-0086-002-0B
         TB - GFX board        S-0087-807

         The PCB looks like a prototype, due to the modifications
         to the PCB. The game is probably licenced from Seibu.

         The bipolar PROMs are not used for colour.



Brief hardware overview:
------------------------

Main processor  - Z80 6MHz
                - 68705

GFX             - custom TC15G008AP-0048  SEI0040BU
            3 x - custom TC17G008AN-0015  SEI0020BU
                - custom TC17G005AN-0028  SEI0030BU
            3 x - custom SIPs. No ID, unusually large.

Sound processor - Z80 6MHz (5.897MHz)
            2 x - YM2203C


The game data seems to be small. There may be graphics
data in the custom SIPs. I am not sure though.

*/


ROM_START( cshooter )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	// Main CPU?
	ROM_LOAD( "r1",  0x00000, 0x08000, CRC(fbe8c518) SHA1(bff8319f4892e6d06f1c7a679f67dc8407279cfa) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	// Sub/Sound CPU?
	ROM_LOAD( "r4",  0x00000, 0x08000, CRC(84fed017) SHA1(9a564c9379eb48569cfba48562889277991864d8) )

	ROM_REGION( 0x0800, REGION_CPU3, 0 )	/* 2k for the microcontroller */
	ROM_LOAD( "68705.bin",    0x0000, 0x0800, NO_DUMP )

	ROM_REGION( 0x02000, REGION_GFX1, 0 )	// TX Layer
	ROM_LOAD( "r3",  0x00000, 0x02000, CRC(67b50a47) SHA1(b1f4aefc9437edbeefba5371149cc08c0b55c741) )	// only 1 byte difference with 3.f11, bad dump?

	ROM_REGION( 0x10000, REGION_GFX2, 0 )	// Sprites & Backgrounds ?
	ROM_LOAD( "r2",  0x00000, 0x10000, CRC(5ddf9f4e) SHA1(69e4d422ca272bf2e9f00edbe7d23760485fdfe6) )

	ROM_REGION( 0x220, REGION_PROMS, 0 )
	ROM_LOAD( "0.bpr", 0x0000, 0x0020, CRC(93e2d292) SHA1(af8edd0cfe85f28ede9604cfaf4516d54e5277c9) )	/* priority? (not used) */
	ROM_LOAD( "1.bpr", 0x0020, 0x0100, CRC(cf14ba30) SHA1(3284b6809075756b3c8e07d9705fc7eacb7556f1) )	/* timing? (not used) */
	ROM_LOAD( "2.bpr", 0x0120, 0x0100, CRC(0eaf5158) SHA1(bafd4108708f66cd7b280e47152b108f3e254fc9) )	/* timing? (not used) */
ROM_END

/*

Cross Shooter
(C) J K H Corp  (Seibu?)

Seibu Hardware
PCB is coloured black and supposed to be proto, but mask roms are present......?

PCB No. S-0087-011A-0
CPU: SHARP LH0080B (Z80B)
SND: YM2151, Z80A, SEI80BU 611 787, YM3012, SEI0100BU YM3931
RAM: TMM2015 x 7, TMM2063 x 1
DIPs: 2 x 8 position
OTHER: SEI0020BU TC17G008AN-0015 (x 3), SEI0050BU M  6 4 0 00, SEI10040BU TC15G008AP-0048,
       SEI0030BU TC17G005AN-0026, SEI0060BU TC17G008AN-0024
XTAL: 14.318 MHz (near SEI80BU), xx.000 MHz (cant read speed, near SEI0040BU)

There are 3 BIG custom black (resistor?) packs on the PCB.

ROMS:
Note, all ROMs have official sticker, "(C) SEIBU KAIHATSU INC." and a number.

1.k19  TMM27256      \
2.k20  TMM27512      / Program
3.f11  TMM2764         Gfx?
4.g8   TMM24256 Mask   Sound (Samples?)
5.g6   TMM2764         Sound program


*/

ROM_START( cshootre )
	ROM_REGION( 2*0x10000, REGION_CPU1, 0 )	// Main CPU?
	ROM_LOAD( "1.k19",  0x00000, 0x08000, CRC(71418952) SHA1(9745ca006576381c9e9595d8e42ab276bab80a41) )

	ROM_REGION( 2*0x10000, REGION_CPU2, 0 )	// Sub/Sound CPU?
	ROM_LOAD( "5.6f",  0x00000, 0x02000, CRC(30be398c) SHA1(6c61200ee8888d6270c8cec50423b3b5602c2027) )	// 5.g6

	ROM_REGION( 0x02000, REGION_GFX1, 0 )	// TX Layer
	ROM_LOAD( "3.f11",  0x00000, 0x02000, CRC(704c26d7) SHA1(e5964f409cbc2c4752e3969f3e84ace08d5ad9cb) )	// only 1 byte difference with R3, bad dump?

	ROM_REGION( 0x10000, REGION_GFX2, 0 )	// Sprites & Backgrounds ?
	ROM_LOAD( "2.k20",  0x00000, 0x10000, CRC(5812fe72) SHA1(3b28bff6b62a411d2195bb228952db62ad32ef3d) )

	ROM_REGION( 0x08000, REGION_GFX3, 0 )	// ?? sound ?? unknown rom
	ROM_LOAD( "4.7f",  0x00000, 0x08000, CRC(3cd715b4) SHA1(da735fb5d262908ddf7ed7dacdea68899f1723ff) )	// 4.g8
ROM_END

/*

Air Raid (Seibu 1987)
S-0087-011A-0

            82S129        TMM2015      Z80B  2.19J
SEI0020BU                 TMM2015            1.18J
SEI0020BU         63S281                    TMM2063
SEI0020BU         TMM2015
63S281
SEI0050BU                 3.13F

SEI0040BU                                   TMM2015          on
                                     4.7F    YM2151          x x
TMM2015                TMM2015       5.6F    Z80         sw2  x xxxxx
TMM2015               TMM2015                                  x
SEI0030BU          SEI0060BU                             sw1 xx xxxxx
                                   SEI80BU
                                   SEI0100BU(YM3931) YM3012

*/

ROM_START( airraid )
	ROM_REGION( 2*0x10000, REGION_CPU1, 0 )	// Main CPU?
	ROM_LOAD( "1.16j",  0x00000, 0x08000, CRC(7ac2cedf) SHA1(272831f51a2731e067b5aec6dba6bddd3c5350c9) )

	ROM_REGION( 2*0x10000, REGION_CPU2, 0 )	// Sub/Sound CPU?
	ROM_LOAD( "5.6f",  0x00000, 0x02000, CRC(30be398c) SHA1(6c61200ee8888d6270c8cec50423b3b5602c2027) )

	ROM_REGION( 0x02000, REGION_GFX1, 0 )	// TX Layer
	ROM_LOAD( "3.13e",  0x00000, 0x02000, CRC(672ec0e8) SHA1(a11cd90d6494251ceee3bc7c72f4e7b1580b77e2) )

	ROM_REGION( 0x10000, REGION_GFX2, 0 )	// Sprites & Backgrounds ?
	ROM_LOAD( "2.19j",  0x00000, 0x10000, CRC(842ae6c2) SHA1(0468445e4ab6f42bac786f9a258df3972fd1fde9) )

	ROM_REGION( 0x08000, REGION_GFX3, 0 )	// ?? sound ?? unknown rom
	ROM_LOAD( "4.7f",  0x00000, 0x08000, CRC(3cd715b4) SHA1(da735fb5d262908ddf7ed7dacdea68899f1723ff) )
ROM_END


DRIVER_INIT( cshooter )
{
	/* temp so it boots */
	unsigned char *rom = memory_region(REGION_CPU1);

	rom[0xa2] = 0x00;
	rom[0xa3] = 0x00;
	rom[0xa4] = 0x00;

}

DRIVER_INIT( cshootre )
{
	int A;
	unsigned char *rom = memory_region(REGION_CPU1);
	int diff = memory_region_length(REGION_CPU1) / 2;

	memory_set_opcode_base(0,rom+diff);

	for (A = 0x0000;A < 0x8000;A++)
	{
		/* decode the opcodes */
		rom[A+diff] = rom[A];

		if (BIT(A,5) && !BIT(A,3))
			rom[A+diff] ^= 0x40;

		if (BIT(A,10) && !BIT(A,9) && BIT(A,3))
			rom[A+diff] ^= 0x20;

		if ((BIT(A,10) ^ BIT(A,9)) && BIT(A,1))
			rom[A+diff] ^= 0x02;

		if (BIT(A,9) || !BIT(A,5) || BIT(A,3))
			rom[A+diff] = BITSWAP8(rom[A+diff],7,6,1,4,3,2,5,0);

		/* decode the data */
		if (BIT(A,5))
			rom[A] ^= 0x40;

		if (BIT(A,9) || !BIT(A,5))
			rom[A] = BITSWAP8(rom[A],7,6,1,4,3,2,5,0);
	}


	seibu_sound_decrypt(REGION_CPU2,0x2000);
}



GAMEX( 1987, cshooter, 0,        cshooter, cshooter, cshooter, ROT270, "[Seibu Kaihatsu] (Taito license)",  "Cross Shooter (not encrypted)", GAME_NOT_WORKING | GAME_NO_SOUND )
GAMEX( 1987, cshootre, cshooter, cshooter, cshooter, cshootre, ROT270, "[Seibu Kaihatsu] (J.K.H. license)", "Cross Shooter (encrypted)", GAME_NOT_WORKING | GAME_NO_SOUND )
GAMEX( 1987, airraid,  cshooter, cshooter, cshooter, cshootre, ROT270, "Seibu Kaihatsu",                    "Air Raid (encrypted)", GAME_NOT_WORKING | GAME_NO_SOUND )
