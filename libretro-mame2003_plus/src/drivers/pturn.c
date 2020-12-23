/*
Parallel Turn
(c) Jaleco, 1984
driver by Tomasz Slanina and Tatsuyuki Satoh

Custom Jaleco chip is some kind of state machine
used for calculate jump offsets.

Top PCB
-------

PCB No. PT-8418
CPU:  Z80A
SND:  AY-3-8010 x 2, Z80A
DIPS: 8 position x 2
RAM:  2114 x 2, MSM2128 x 1 (equivalent to 6116)

Other: Reset Switch near edge connector

       Custom JALECO chip (24 pin DIP) near RAM MSM2128, verified to be NOT 2128 ram.

       Pinouts are :
       Pin 1 hooked to pin 3 of ROM 7
       Pin 2 hooked to pin 4 of ROM 7
       Pin 3 hooked to pin 5 of ROM 7
       Pin 4 hooked to pin 6 of ROM 7
       Pin 5 hooked to pin 7 of ROM 7
       Pin 6 hooked to pin 8 of ROM 7
       Pin 7 hooked to pin 9 of ROM 7
       Pin 8 hooked to pin 10 of ROM 7
       Pin 9 hooked to pin 11 of ROM 7
       Pin 10 hooked to pin 12 of ROM 7
       Pin 11 hooked to pin 13 of ROM 7
       Pin 12 GND
       Pin 13 hooked to pin 13 of 2128 and pin 15 of ROM 7
       Pin 14 hooked to pin 14 of 2128 and pin 16 of ROM 7
       Pin 15 hooked to pin 17 of ROM 7
       Pin 16 hooked to pin 18 of ROM 7
       Pin 17 hooked to pin 19 of ROM 7
       Pin 18 NC
       Pin 19 NC
       Pin 20 hooked to pin 11 of 74LS32 at 4F
       Pin 21 hooked to pin 8 of 74LS32 at 4F
       Pin 22 hooked to pin 24 of ROM 7
       Pin 23 hooked to pin 25 of ROM 7
       Pin 24 +5

NOTE: The archive contains a different ROM7 that was in another archive.
      (I merged all archives since they are identical other than ROM 7 in one archive named 7.BIN)
      Perhaps this is from a bootleg PCB with a workaround for the custom JALECO chip?


ROMS: All ROM labels say only "PROM" and a number.
      1, 2, 3 and 9 near Z80 at 5K, all 2732's
      4, 5, 6 & 7 near custom JALECO chip and Z80 at 3D, all 2764's
      prom_red.3p type TBP24S10
      prom_grn.4p type N82S129
      prom_blu.4r type TBP24S10


LOWER PCB
---------

PCB:  PT-8419
XTAL: ? Stamped KSS5, connected to pins 8 & 13 of 74LS37 at 9P
      Also connected to 2 x 330 Ohm resistors which are in turn connected to pins
      9 & 11 of 9P. Pins 9 & 11 of 9P are joined with a 101 ceramic capacitor.
      UPDATE! When I substitute various speed xtals, a 8.000MHz xtal allows the PCB to work fine.

RAM:  AM93422DC x 2, MB7063 x 1, 2114 x 4

ROMS: All ROM labels say only "PROM" and a number.
      10, 14, 15 & 16 type 2764
      11, 12, 13 type 2732

*/
#include "driver.h"
#include "sound/ay8910.h"
#include "vidhrdw/generic.h"

struct tilemap *pturn_fgmap,*pturn_bgmap;
static int bgbank=0;
static int fgbank=0;
static int bgpalette=0;
static int fgpalette=0;
static int bgcolor=0;


static UINT8 tile_lookup[0x10]=
{
	0x00, 0x10, 0x40, 0x50,
	0x20, 0x30, 0x60, 0x70,
	0x80, 0x90, 0xc0, 0xd0,
	0xa0, 0xb0, 0xe0, 0xf0
};

static void get_pturn_tile_info(int tile_index)
{
	int tileno;
	tileno = videoram[tile_index];

	tileno=tile_lookup[tileno>>4]|(tileno&0xf)|(fgbank<<8);

	SET_TILE_INFO(0,tileno,fgpalette,0)
}



static void get_pturn_bg_tile_info(int tile_index)
{
	int tileno,palno;
	tileno = memory_region(REGION_USER1)[tile_index];
	palno=bgpalette;
	if(palno==1)
	{
		palno=25;
	}
	SET_TILE_INFO(1,tileno+bgbank*256,palno,0)
}

VIDEO_START(pturn)
{
	pturn_fgmap = tilemap_create(get_pturn_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT, 8, 8,32,32);
	tilemap_set_transparent_pen(pturn_fgmap,0);
	pturn_bgmap = tilemap_create(get_pturn_bg_tile_info,tilemap_scan_rows,TILEMAP_TRANSPARENT, 8, 8,32,32*8);
	tilemap_set_transparent_pen(pturn_bgmap,0);
	return 0;
}

VIDEO_UPDATE(pturn)
{
	int offs;
	int sx, sy;
	int flipx, flipy;

	fillbitmap(bitmap, bgcolor, &Machine->visible_area);
	tilemap_draw(bitmap,cliprect,pturn_bgmap,0,0);
	for ( offs = 0x80-4 ; offs >=0 ; offs -= 4)
	{
		sy=256-spriteram[offs]-16 ;
		sx=spriteram[offs+3]-16 ;

		flipx=spriteram[offs+1]&0x40;
		flipy=spriteram[offs+1]&0x80;


		if (flip_screen_x)
		{
			sx = 224 - sx;
			flipx ^= 0x40;
		}

		if (flip_screen_y)
		{
			flipy ^= 0x80;
			sy = 224 - sy;
		}

		if(sx|sy)
		{
			drawgfx(bitmap, Machine->gfx[2],
			spriteram[offs+1] & 0x3f ,
			(spriteram[offs+2] & 0x1f),
			flipx, flipy,
			sx,sy,
			cliprect,TRANSPARENCY_PEN,0);
		}
	}
	tilemap_draw(bitmap,cliprect,pturn_fgmap,0,0);
}


static WRITE_HANDLER( pturn_videoram_w )
{
	videoram[offset]=data;
	tilemap_mark_tile_dirty(pturn_fgmap,offset);
}

static int nmi_main, nmi_sub;

static WRITE_HANDLER( nmi_main_enable_w )
{
	nmi_main = data;
}

static WRITE_HANDLER( nmi_sub_enable_w )
{
	nmi_sub = data;
}

static WRITE_HANDLER(sound_w)
{
	soundlatch_w(0,data);
}


static WRITE_HANDLER(bgcolor_w)
{
	bgcolor=data;
}

static WRITE_HANDLER(bg_scrollx_w)
{
	tilemap_set_scrolly(pturn_bgmap, 0, (data>>5)*32*8);
	bgpalette=data&0x1f;
	tilemap_mark_all_tiles_dirty(pturn_bgmap);
}

static WRITE_HANDLER(fgpalette_w)
{
	fgpalette=data&0x1f;
	tilemap_mark_all_tiles_dirty(pturn_fgmap);
}

static WRITE_HANDLER(bg_scrolly_w)
{
	tilemap_set_scrollx(pturn_bgmap, 0, data);
}

static WRITE_HANDLER(fgbank_w)
{
	fgbank=data&1;
	tilemap_mark_all_tiles_dirty(pturn_fgmap);
}

static WRITE_HANDLER(bgbank_w)
{
	bgbank=data&1;
	tilemap_mark_all_tiles_dirty(pturn_bgmap);
}

static WRITE_HANDLER(flip_w)
{
	flip_screen_set(data);
}


static READ_HANDLER (pturn_custom_r)
{
	int addr = (int)offset + 0xc800;

	switch(addr)
	{
		case 0xc803:
			/* pc=4a4,4a7 : dummy read?*/
			return 0x00;

		case 0xCA73:
			/* pc=0x0123 , bit6 must be 0*/
			/* pc=0x0545 , +40 must be 0xfe (check at 0577)*/
			return 0xbe;

		/*case 0xca00:*/
		/*  return 0x00; */ /* pc=0x0131 for protect reset?*/

		case 0xca74:
			/* pc=0x04db ,must be 66 (check at 016A)*/
			return 0x66;
	}
	return 0x00;
}


static MEMORY_READ_START( main_readmem )
    { 0x0000, 0x7fff, MRA_ROM },
	{ 0xc000, 0xc7ff, MRA_RAM },
	{ 0xc800, 0xcfff, pturn_custom_r },
	{ 0xdfe0, 0xdfe0, MRA_NOP },
	{ 0xf000, 0xf0ff, MRA_RAM },
	{ 0xf800, 0xf800, input_port_0_r },
	{ 0xf801, 0xf801, input_port_1_r },
	{ 0xf802, 0xf802, input_port_2_r },
	{ 0xf804, 0xf804, input_port_4_r }, /* DSW2 */
    { 0xf805, 0xf805, input_port_3_r }, /* DSW1 */
	{ 0xf806, 0xf806, MRA_NOP },/* Protection related, ((val&3)==2) -> jump to 0 */
MEMORY_END

static MEMORY_WRITE_START( main_writemem )
    { 0x0000, 0x7fff, MWA_ROM },
	{ 0xc000, 0xc7ff, MWA_RAM },
	{ 0xc800, 0xcfff, MWA_NOP },
	{ 0xdfe0, 0xdfe0, MWA_NOP },
	{ 0xe000, 0xe3ff, pturn_videoram_w, &videoram },
	{ 0xe400, 0xe400, fgpalette_w },
	{ 0xe800, 0xe800, sound_w },
	{ 0xf000, 0xf0ff, MWA_RAM, &spriteram, &spriteram_size },
	{ 0xf400, 0xf400, bg_scrollx_w },
	{ 0xf800, 0xf800, MWA_NOP },
	{ 0xf801, 0xf801, bgcolor_w },
	{ 0xf803, 0xf803, bg_scrolly_w },
	{ 0xfc00, 0xfc00, flip_w },
	{ 0xfc01, 0xfc01, nmi_main_enable_w },
	{ 0xfc02, 0xfc02, MWA_NOP },/* Unknown */
	{ 0xfc03, 0xfc03, MWA_NOP },/* Unknown */
	{ 0xfc04, 0xfc04, bgbank_w },
	{ 0xfc05, 0xfc05, fgbank_w },
	{ 0xfc06, 0xfc06, MWA_NOP },/* Unknown */
	{ 0xfc07, 0xfc07, MWA_NOP },/* Unknown */
MEMORY_END

static MEMORY_READ_START( sub_readmem )
    { 0x0000, 0x0fff, MRA_ROM },
	{ 0x2000, 0x23ff, MRA_RAM },
	{ 0x3000, 0x3000, soundlatch_r },
	{ 0x4000, 0x4000, MRA_RAM },
MEMORY_END

static MEMORY_WRITE_START( sub_writemem )
    { 0x0000, 0x0fff, MWA_ROM },
	{ 0x2000, 0x23ff, MWA_RAM },
	{ 0x3000, 0x3000, nmi_sub_enable_w },
	{ 0x4000, 0x4000, MWA_RAM },
	{ 0x5000, 0x5000, AY8910_control_port_0_w },
	{ 0x5001, 0x5001, AY8910_write_port_0_w },
	{ 0x6000, 0x6000, AY8910_control_port_1_w },
	{ 0x6001, 0x6001, AY8910_write_port_1_w },
MEMORY_END


static struct GfxLayout charlayout =
{
	8,8,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(0,3), RGN_FRAC(1,3), RGN_FRAC(2,3) },
	{ 0,1,2,3, 4,5,6,7 },
	{ 0*8,1*8,2*8,3*8,4*8,5*8,6*8,7*8 },
	8*8
};


static struct GfxLayout spritelayout =
{
	32,32,
	RGN_FRAC(1,3),
	3,
	{ RGN_FRAC(0,3),RGN_FRAC(1,3),RGN_FRAC(2,3) },
	{ 0, 1, 2, 3, 4, 5, 6, 7,
	 8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7,
	16*8+0, 16*8+1, 16*8+2, 16*8+3, 16*8+4, 16*8+5, 16*8+6, 16*8+7,
	24*8+0, 24*8+1, 24*8+2, 24*8+3, 24*8+4, 24*8+5, 24*8+6, 24*8+7 },
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
	32*8, 33*8, 34*8, 35*8, 36*8, 37*8, 38*8, 39*8,
	64*8, 65*8, 66*8, 67*8, 68*8, 69*8, 70*8, 71*8,
	96*8, 97*8, 98*8, 99*8, 100*8, 101*8, 102*8, 103*8 },
	128*8
};

static struct GfxDecodeInfo gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,   0x000, 32 },
	{ REGION_GFX2, 0, &charlayout,   0x000, 32 },
	{ REGION_GFX3, 0, &spritelayout, 0x000, 32 },
	{ -1 }
};

INPUT_PORTS_START( pturn )
	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON3 | IPF_PLAYER1 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON3 | IPF_PLAYER1 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_HIGH,IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH,IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH,IPT_SERVICE1 ) /* service coin */
	PORT_BIT( 0x10, IP_ACTIVE_HIGH,IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH,IPT_START2 )
	PORT_BIT( 0xc8, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "5" )
	PORT_DIPSETTING(    0x02, "7" )
	PORT_DIPSETTING(    0x03, "Infinite (Cheat)")
	PORT_DIPNAME( 0x0c, 0x08, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x0c, "100000" )
	PORT_DIPSETTING(    0x08, "50000" )
	PORT_DIPSETTING(    0x04, "20000" )
	PORT_DIPSETTING(    0x00, "None" )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Cocktail ) )
	PORT_BIT( 0xb0, IP_ACTIVE_HIGH, IPT_UNUSED ) /* marked as "NOT USED" in doc */

	PORT_START
	PORT_DIPNAME( 0x07, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 6C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x38, 0x00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x38, DEF_STR( 6C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x28, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x40, 0x00, "Freeze" )
	PORT_DIPSETTING(    0x00, "Normal Display" )
	PORT_DIPSETTING(    0x40, "Stop Motion" )
	PORT_DIPNAME( 0x80, 0x00, "Language" ) /* marked as "NOT USED" in doc */
	PORT_DIPSETTING(    0x00, "English" )
	PORT_DIPSETTING(    0x80, "Japanese" )
INPUT_PORTS_END

static INTERRUPT_GEN( pturn_sub_intgen )
{
	if(nmi_sub)
	{
		cpu_set_irq_line(1,IRQ_LINE_NMI,PULSE_LINE);
	}
}

static INTERRUPT_GEN( pturn_main_intgen )
{
	if (nmi_main)
	{
		cpu_set_irq_line(0,IRQ_LINE_NMI,PULSE_LINE);
	}
}

static MACHINE_INIT( pturn )
{
	soundlatch_clear_w(0,0);
}

static struct AY8910interface ay8910_interface =
{
	2,
	2000000,
	{ 50,50 },
	{ 0 },
	{ 0 },
	{ 0 },
	{ 0 }
};


static MACHINE_DRIVER_START( pturn )
	MDRV_CPU_ADD(Z80, 12000000/3)
	MDRV_CPU_MEMORY(main_readmem, main_writemem)
	MDRV_CPU_VBLANK_INT(pturn_main_intgen,1)
	MDRV_MACHINE_INIT(pturn)

	MDRV_CPU_ADD(Z80, 12000000/3)
	/* audio CPU */
	MDRV_CPU_MEMORY(sub_readmem, sub_writemem)
	MDRV_CPU_VBLANK_INT(pturn_sub_intgen,3)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	MDRV_GFXDECODE(gfxdecodeinfo)

	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MDRV_PALETTE_LENGTH(0x100)
	MDRV_PALETTE_INIT(RRRR_GGGG_BBBB)

	MDRV_VIDEO_START(pturn)
	MDRV_VIDEO_UPDATE(pturn)

	/* sound hardware */
	MDRV_SOUND_ADD(AY8910, ay8910_interface)
MACHINE_DRIVER_END


ROM_START( pturn )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )
	ROM_LOAD( "prom4.8d", 0x00000,0x2000, CRC(d3ae0840) SHA1(5ac5f2626de7865cdf379cf15ae3872e798b7e25))
	ROM_LOAD( "prom6.8b", 0x02000,0x2000, CRC(65f09c56) SHA1(c0a7a1bfaacfc4af14d8485e2b5f2c604937a1e4))
	ROM_LOAD( "prom5.7d", 0x04000,0x2000, CRC(de48afb4) SHA1(9412288b63cf3ae8c9522b1fcacc4aa36ac7a23c))
	ROM_LOAD( "prom7.7b", 0x06000,0x2000, CRC(bfaeff9f) SHA1(63972c311f28971e121fbccd4c0d78edbdb6bdb4))

	ROM_REGION( 0x10000, REGION_CPU2, 0 )
	ROM_LOAD( "prom9.5n", 0x00000,0x1000, CRC(8b4d944e) SHA1(6f956d972c2c2ef875378910b80ca59701710957))

	ROM_REGION( 0x3000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "prom1.8k", 0x00000,0x1000, CRC(10aba36d) SHA1(5f9ce00365b3be91f0942b282b3cfc0c791baf98))
	ROM_LOAD( "prom2.7k", 0x01000,0x1000, CRC(b8a4d94e) SHA1(78f9db58ceb4a87ab2744529b0e7ad3eb826e627))
	ROM_LOAD( "prom3.6k", 0x02000,0x1000, CRC(9f51185b) SHA1(84690556da013567133b7d8fcda25b9fb831e4b0))

	ROM_REGION( 0x3000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "prom11.16f", 0x000000, 0x01000, CRC(129122c6) SHA1(feb6d9abddb4d888b49861a32a063d009ca994aa) )
	ROM_LOAD( "prom12.16h", 0x001000, 0x01000, CRC(69b09323) SHA1(726749b625052984e1d8c71eb69511c35ca75f9c) )
	ROM_LOAD( "prom13.16k", 0x002000, 0x01000, CRC(e9f67599) SHA1(b2eb144c8ce9ff57bd66ba57705d5e242115ef41) )

	ROM_REGION( 0x6000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "prom14.16l", 0x000000, 0x02000, CRC(ffaa0b8a) SHA1(20b1acc2562e493539fe34d056e6254e4b2458be) )
	ROM_LOAD( "prom15.16m", 0x002000, 0x02000, CRC(41445155) SHA1(36d81b411729447ca7ff712ac27d8a0f2015bcac) )
	ROM_LOAD( "prom16.16p", 0x004000, 0x02000, CRC(94814c5d) SHA1(e4ab6c0ae94184d5270cadb887f56e3550b6d9f2) )

	ROM_REGION( 0x0300, REGION_PROMS, 0 )
  	ROM_LOAD( "prom_red.3p", 0x0000, 0x0100, CRC(505fd8c2) SHA1(f2660fe512c76412a7b9f4be21fe549dd59fbda0) )
	ROM_LOAD( "prom_grn.4p", 0x0100, 0x0100, CRC(6a00199d) SHA1(ff0ac7ae83d970778a756f7445afed3785fc1150) )
	ROM_LOAD( "prom_blu.4r", 0x0200, 0x0100, CRC(7b4c5788) SHA1(ca02b12c19be7981daa070533455bd4d227d56cd) )

	ROM_REGION( 0x2000, REGION_USER1, 0 )
	ROM_LOAD( "prom10.16d", 0x0000,0x2000, CRC(a96e3c95) SHA1(a3b1c1723fcda80c11d9858819659e5e9dfe5dd3))
ROM_END



GAMEX( 1984, pturn,  0, pturn,  pturn,  0, ROT90,   "Jaleco", "Parallel Turn",	GAME_IMPERFECT_COLORS )
