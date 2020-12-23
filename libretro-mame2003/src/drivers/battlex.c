/* battlex.c - by David Haywood

Stephh's notes :

  - I don't know exactly how to call the "Free Play" Dip Switch 8(
    It's effect is the following :
      * you need to insert at least one credit and start a game
      * when the game is over, you can start another games WITHOUT
        inserting another coins
    Note that the number of credits is decremented though.
    Credits are BCD coded on 3 bytes (0x000000-0x999999) at addresses
    0xa039 (LSB), 0xa03a and 0xa03b (MSB), but only the LSB is displayed.

   - Setting the flipscreen dip to ON also hides the copyright message (?)

TO DO :

  - missing starfield

  - game speed, its seems to be controlled by the IRQ's, how fast should it
    be? firing seems frustratingly inconsistant

  - colors match Tim's screen shots, but there's no guarantee RGB are in the
    correct order.
*/

/*

Battle Cross (c)1982 Omori

CPU: Z80A
Sound: AY-3-8910
Other: 93419 (in socket marked 93219)

RAM: 4116(x12), 2114(x2), 2114(x6)
PROMS: none

XTAL: 10.0 MHz

*/


#include "driver.h"
#include "vidhrdw/generic.h"


extern WRITE_HANDLER( battlex_palette_w );
extern WRITE_HANDLER( battlex_videoram_w );
extern WRITE_HANDLER( battlex_scroll_x_lsb_w );
extern WRITE_HANDLER( battlex_scroll_x_msb_w );
extern WRITE_HANDLER( battlex_flipscreen_w );

extern PALETTE_INIT( battlex );
extern VIDEO_START( battlex );
extern VIDEO_UPDATE( battlex );


/*** MEMORY & PORT READ / WRITE **********************************************/

static MEMORY_READ_START( readmem )
	{ 0x0000, 0x5fff, MRA_ROM },
	{ 0x8000, 0x8fff, MRA_RAM }, /* not read? */
	{ 0x9000, 0x91ff, MRA_RAM }, /* not read? */
	{ 0xa000, 0xa3ff, MRA_RAM },
	{ 0xe000, 0xe03f, MRA_RAM }, /* not read? */
MEMORY_END

static MEMORY_WRITE_START( writemem )
	{ 0x0000, 0x5fff, MWA_ROM },
	{ 0x8000, 0x8fff, battlex_videoram_w, &videoram },
	{ 0x9000, 0x91ff, MWA_RAM, &spriteram },
	{ 0xa000, 0xa3ff, MWA_RAM }, /* main */
	{ 0xe000, 0xe03f, battlex_palette_w }, /* probably palette */
MEMORY_END

static PORT_READ_START( readport )
	{ 0x00, 0x00, input_port_0_r },
	{ 0x01, 0x01, input_port_1_r },
	{ 0x02, 0x02, input_port_2_r },
	{ 0x03, 0x03, input_port_3_r },
MEMORY_END


static PORT_WRITE_START( writeport )
	{ 0x10, 0x10, battlex_flipscreen_w },
	/* verify all of these */
	{ 0x22, 0x22, AY8910_write_port_0_w },
	{ 0x23, 0x23, AY8910_control_port_0_w },

	/* 0x30 looks like scroll, but can't be ? changes (increases or decreases)
		depending on the direction your ship is facing on lev 2. at least */
	{ 0x30, 0x30, MWA_NOP },

	{ 0x32, 0x32, battlex_scroll_x_lsb_w },
	{ 0x33, 0x33, battlex_scroll_x_msb_w },
MEMORY_END

/*** INPUT PORTS *************************************************************/

INPUT_PORTS_START( battlex )
	PORT_START	/* IN0 */
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0x04, 0x04, "Allow Continue" )			// Not on 1st stage
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x08, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x00, "Freeze" )				// VBLANK ?
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x10, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START	/* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_BUTTON1 | IPF_COCKTAIL )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_START1 )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_START2 )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START	/* IN2 */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP    | IPF_8WAY )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_8WAY )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN  | IPF_8WAY )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT  | IPF_8WAY )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_COCKTAIL )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_COCKTAIL )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_COCKTAIL )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_COCKTAIL )

	PORT_START	/* IN3 */
	PORT_DIPNAME( 0x07, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_6C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_7C ) )
	PORT_DIPNAME( 0x18, 0x10, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPSETTING(    0x08, "2" )
	PORT_DIPSETTING(    0x10, "3" )
	PORT_DIPSETTING(    0x18, "5" )
	PORT_DIPNAME( 0x60, 0x20, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x00, "5000" )
	PORT_DIPSETTING(    0x20, "10000" )
	PORT_DIPSETTING(    0x40, "15000" )
	PORT_DIPSETTING(    0x60, "20000" )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Free_Play ) )		// See notes
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )
INPUT_PORTS_END

/*** GFX DECODE **************************************************************/


static struct GfxLayout battlex_charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0,1,2,3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4 },
	{ 0*8*4, 1*8*4, 2*8*4, 3*8*4, 4*8*4, 5*8*4, 6*8*4, 7*8*4 },
	8*8*4
};


static struct GfxLayout battlex_spritelayout =
{
	16,16,
	RGN_FRAC(1,3),
	3,
	{ 0,RGN_FRAC(1,3),RGN_FRAC(2,3) },
	{ 7,6,5,4,3,2,1,0,
		15,14,13,12,11,10,9,8 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
		8*16, 9*16, 10*16, 11*16, 12*16, 13*16, 14*16, 15*16 },
	16*16
};


static struct GfxDecodeInfo gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &battlex_charlayout,      0, 8 },
	{ REGION_GFX2, 0, &battlex_spritelayout, 16*8, 8 },
	{ -1 } /* end of array */
};

/*** SOUND *******************************************************************/

static struct AY8910interface battlex_ay8910_interface =
{
	1,	/* 1 chip */
	10000000/8,
	{ 30 },
	{ 0 },
	{ 0 },
	{ 0 },
	{ 0 }
};

/*** MACHINE DRIVERS *********************************************************/

static MACHINE_DRIVER_START( battlex )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80,10000000/2 )		 /* 10 MHz, divided ? (Z80A CPU) */
	MDRV_CPU_MEMORY(readmem,writemem)
	MDRV_CPU_PORTS(readport,writeport)
	MDRV_CPU_VBLANK_INT(irq0_line_pulse,8) /* controls game speed? */

	MDRV_FRAMES_PER_SECOND(56) /* The video syncs at 15.8k H and 56 V (www.klov.com) */
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(16*8+64)

	MDRV_PALETTE_INIT(battlex)
	MDRV_VIDEO_START(battlex)
	MDRV_VIDEO_UPDATE(battlex)

	/* sound hardware */
	MDRV_SOUND_ADD(AY8910, battlex_ay8910_interface)
MACHINE_DRIVER_END


/*** ROM LOADING *************************************************************/

ROM_START( battlex )
	ROM_REGION( 0x10000, REGION_CPU1, 0 )	/* 64k for code */
	ROM_LOAD( "p-rom1.6",    0x0000, 0x1000, CRC(b00ae551) SHA1(32a963fea23ea58fc3aab93cc814784a932f045e) )
	ROM_LOAD( "p-rom2.5",    0x1000, 0x1000, CRC(e765bb11) SHA1(99671e63f4c7d3d8754277451f0b35cba03b532d) )
	ROM_LOAD( "p-rom3.4",    0x2000, 0x1000, CRC(21675a91) SHA1(5bbd5b53b1a1b7aaed5d8c7b09b57f35e4a774dc) )
	ROM_LOAD( "p-rom4.3",    0x3000, 0x1000, CRC(fff1ccc4) SHA1(2cb9b096b30e441559e57992df8f30aee46b1f1c) )
	ROM_LOAD( "p-rom5.2",    0x4000, 0x1000, CRC(ceb63d38) SHA1(92cab905d009c59115f52172ba7d01c8ff8991d7) )
	ROM_LOAD( "p-rom6.1",    0x5000, 0x1000, CRC(6923f601) SHA1(e6c33cbd8d8679299d7b2c568d56f96ed3073971) )

	ROM_REGION( 0x4000, REGION_GFX1, 0 )
	/* filled in later */
//	ROM_LOAD( "2732.e",    0x0000, 0x1000, CRC(126842b7) SHA1(2da4f64e077232c1dd0853d07d801f9781517850) )

	ROM_REGION( 0x3000, REGION_GFX2, 0 )
	ROM_LOAD( "2732.f",    0x0000, 0x1000, CRC(2b69287a) SHA1(30c0edaec44118b95ec390bd41c1bd49a2802451) )
	ROM_LOAD( "2732.h",    0x1000, 0x1000, CRC(9f4c3bdd) SHA1(e921ecafefe54c033d05d9cd289808e971ac7940) )
	ROM_LOAD( "2732.j",    0x2000, 0x1000, CRC(c1345b05) SHA1(17194c8ec961990222bd295ff1d036a64f497b0e) )

	ROM_REGION( 0x1000, REGION_USER1, 0 ) /* line colours? or bad? */
	ROM_LOAD( "2732.d",    0x0000, 0x1000, CRC(750a46ef) SHA1(b6ab93e084ab0b7c6ad90ee6431bc1b7ab9ed46d) )

	ROM_REGION( 0x1000, REGION_USER2, 0 )
	ROM_LOAD( "2732.e",    0x0000, 0x1000, CRC(126842b7) SHA1(2da4f64e077232c1dd0853d07d801f9781517850) )
ROM_END

static DRIVER_INIT( battlex )
{
	UINT8 *cold    = memory_region       ( REGION_USER1 );
	UINT8 *mskd    = memory_region       ( REGION_USER2 );
	UINT8 *dest    = memory_region       ( REGION_GFX1 );

	int outcount;

	/* convert gfx data from 1bpp + color block mask to straight 4bpp */
	for (outcount = 0; outcount < (0x1000/8); outcount++)
	{
		int linecount;
		for (linecount = 0; linecount < 8; linecount ++)
		{
			int bitmask = 0x01;
			int bitcount;

			for (bitcount = 0;bitcount < 8 ; bitcount ++)
			{
				int bit, col;
				bit = (mskd[outcount*8+linecount] & bitmask) >> bitcount;

				if (bit) col = (cold[outcount*8+(linecount&~1)+(bitcount/4)] & 0x0f) << 4;
				else col = (cold[outcount*8+(linecount&~1)+(bitcount/4)] & 0xf0);

				dest[outcount*32 + linecount*4 + bitcount /2] |= (col >> (4*(bitcount & 1)));
				bitmask = bitmask << 1;
			}
		}
	}
}

/*** GAME DRIVERS ************************************************************/

GAMEX( 1982, battlex, 0, battlex, battlex, battlex, ROT180, "Omori Electric", "Battle Cross", GAME_IMPERFECT_GRAPHICS )
