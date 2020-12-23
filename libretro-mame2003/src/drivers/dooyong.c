/***************************************************************************

Dooyong games

driver by Nicola Salmoria

The Last Day   Z80     Z80 2xYM2203
Gulf Storm     Z80     Z80 2xYM2203
Pollux         Z80     Z80 2xYM2203
Blue Hawk      Z80     Z80 YM2151 OKI6295
Sadari         Z80     Z80 YM2151 OKI6295
Gun Dealer '94 Z80     Z80 YM2151 OKI6295
Super-X        68000   Z80 YM2151 OKI6295
R-Shark        68000   Z80 YM2151 OKI6295

These games all run on different but similar hardware. A common thing that they
all have is tilemaps hardcoded in ROM.

TODO:
- video driver is not optimized at all
- port A of both of the YM2203 is constantly read and stored in memory -
  function unknown
- some of the sound programs often write to the ROM area - is this just a bug, or
  is there something connected there?
Last Day:
- sprite/fg priority is not understood (tanks, boats should pass below bridges)
- when you insert a coin, the demo sprites continue to move in the background.
  Maybe the whole background and sprites are supposed to be disabled.
Gulf Storm:
- sprite/fg priority is not understood
- there seem to be some invisible enemies around the first bridge
Blue Hawk:
- sprite/fg priority is not understood
Primella:
- does the game really support cocktail mode as service mode suggests?
- are buttons 2 and 3 used as service mode suggests?
R-Shark, Super-X:
- sprite/fg priority is not understood

***************************************************************************/

#include "driver.h"
#include "vidhrdw/generic.h"


extern unsigned char *lastday_txvideoram;
extern unsigned char *lastday_bgscroll,*lastday_fgscroll,*bluehawk_fg2scroll;
extern data16_t *rshark_scroll1,*rshark_scroll2,*rshark_scroll3,*rshark_scroll4;

WRITE_HANDLER( lastday_ctrl_w );
WRITE_HANDLER( pollux_ctrl_w );
WRITE_HANDLER( primella_ctrl_w );
WRITE16_HANDLER( rshark_ctrl_w );
VIDEO_UPDATE( lastday );
VIDEO_UPDATE( gulfstrm );
VIDEO_UPDATE( pollux );
VIDEO_UPDATE( bluehawk );
VIDEO_UPDATE( primella );
VIDEO_UPDATE( rshark );
VIDEO_EOF( dooyong );
VIDEO_EOF( rshark );



static WRITE_HANDLER( lastday_bankswitch_w )
{
 	int bankaddress;
	unsigned char *RAM = memory_region(REGION_CPU1);

	bankaddress = 0x10000 + (data & 0x07) * 0x4000;
	cpu_setbank(1,&RAM[bankaddress]);

if (data & 0xf8) usrintf_showmessage("bankswitch %02x",data);
}

static WRITE_HANDLER( flip_screen_w )
{
	flip_screen_set(data);
}



static MEMORY_READ_START( lastday_readmem )
	{ 0x0000, 0x7fff, MRA_ROM },
	{ 0x8000, 0xbfff, MRA_BANK1 },
	{ 0xc010, 0xc010, input_port_0_r },
	{ 0xc011, 0xc011, input_port_1_r },
	{ 0xc012, 0xc012, input_port_2_r },
	{ 0xc013, 0xc013, input_port_3_r },	/* DSWA */
	{ 0xc014, 0xc014, input_port_4_r },	/* DSWB */
	{ 0xc800, 0xffff, MRA_RAM },
MEMORY_END

static MEMORY_WRITE_START( lastday_writemem )
	{ 0x0000, 0xbfff, MWA_ROM },
	{ 0xc000, 0xc004, MWA_RAM, &lastday_bgscroll },
	{ 0xc008, 0xc00c, MWA_RAM, &lastday_fgscroll },
	{ 0xc010, 0xc010, lastday_ctrl_w },	/* coin counter, flip screen */
	{ 0xc011, 0xc011, lastday_bankswitch_w },
	{ 0xc012, 0xc012, soundlatch_w },
	{ 0xc800, 0xcfff, paletteram_xxxxBBBBGGGGRRRR_w, &paletteram },
	{ 0xd000, 0xdfff, MWA_RAM, &lastday_txvideoram },
	{ 0xe000, 0xefff, MWA_RAM },
	{ 0xf000, 0xffff, MWA_RAM, &spriteram, &spriteram_size },
MEMORY_END

static MEMORY_READ_START( pollux_readmem )
	{ 0x0000, 0x7fff, MRA_ROM },
	{ 0x8000, 0xbfff, MRA_BANK1 },
	{ 0xc000, 0xefff, MRA_RAM },
	{ 0xf000, 0xf000, input_port_0_r },
	{ 0xf001, 0xf001, input_port_1_r },
	{ 0xf002, 0xf002, input_port_2_r },
	{ 0xf003, 0xf003, input_port_3_r },
	{ 0xf004, 0xf004, input_port_4_r },
	{ 0xf800, 0xffff, MRA_RAM },
MEMORY_END

static MEMORY_WRITE_START( pollux_writemem )
	{ 0x0000, 0xbfff, MWA_ROM },
	{ 0xc000, 0xcfff, MWA_RAM },
	{ 0xd000, 0xdfff, MWA_RAM, &spriteram, &spriteram_size },
	{ 0xe000, 0xefff, MWA_RAM, &lastday_txvideoram },
	{ 0xf000, 0xf000, lastday_bankswitch_w },
	{ 0xf008, 0xf008, pollux_ctrl_w },	/* coin counter, flip screen */
	{ 0xf010, 0xf010, soundlatch_w },
	{ 0xf018, 0xf01c, MWA_RAM, &lastday_bgscroll },
	{ 0xf020, 0xf024, MWA_RAM, &lastday_fgscroll },
	{ 0xf800, 0xffff, paletteram_xRRRRRGGGGGBBBBB_w, &paletteram },
MEMORY_END

static MEMORY_READ_START( bluehawk_readmem )
	{ 0x0000, 0x7fff, MRA_ROM },
	{ 0x8000, 0xbfff, MRA_BANK1 },
	{ 0xc000, 0xc000, input_port_0_r },
	{ 0xc001, 0xc001, input_port_1_r },
	{ 0xc002, 0xc002, input_port_2_r },
	{ 0xc003, 0xc003, input_port_3_r },
	{ 0xc004, 0xc004, input_port_4_r },
	{ 0xc800, 0xffff, MRA_RAM },
MEMORY_END

static MEMORY_WRITE_START( bluehawk_writemem )
	{ 0x0000, 0xbfff, MWA_ROM },
	{ 0xc000, 0xc000, flip_screen_w },
	{ 0xc008, 0xc008, lastday_bankswitch_w },
	{ 0xc010, 0xc010, soundlatch_w },
	{ 0xc018, 0xc01c, MWA_RAM, &bluehawk_fg2scroll },
	{ 0xc040, 0xc044, MWA_RAM, &lastday_bgscroll },
	{ 0xc048, 0xc04c, MWA_RAM, &lastday_fgscroll },
	{ 0xc800, 0xcfff, paletteram_xRRRRRGGGGGBBBBB_w, &paletteram },
	{ 0xd000, 0xdfff, MWA_RAM, &lastday_txvideoram },
	{ 0xe000, 0xefff, MWA_RAM, &spriteram, &spriteram_size },
	{ 0xf000, 0xffff, MWA_RAM },
MEMORY_END

static MEMORY_READ_START( primella_readmem )
	{ 0x0000, 0x7fff, MRA_ROM },
	{ 0x8000, 0xbfff, MRA_BANK1 },
	{ 0xc000, 0xcfff, MRA_RAM },
	{ 0xd000, 0xd3ff, MRA_RAM },
	{ 0xe000, 0xefff, MRA_RAM },
	{ 0xf800, 0xf800, input_port_0_r },
	{ 0xf810, 0xf810, input_port_1_r },
	{ 0xf820, 0xf820, input_port_2_r },
	{ 0xf830, 0xf830, input_port_3_r },
	{ 0xf840, 0xf840, input_port_4_r },
MEMORY_END

static MEMORY_WRITE_START( primella_writemem )
	{ 0x0000, 0xbfff, MWA_ROM },
	{ 0xc000, 0xcfff, MWA_RAM },
	{ 0xd000, 0xd3ff, MWA_RAM },	/* what is this? looks like a palette? scratchpad RAM maybe? */
	{ 0xe000, 0xefff, MWA_RAM, &lastday_txvideoram },
	{ 0xf000, 0xf7ff, paletteram_xRRRRRGGGGGBBBBB_w, &paletteram },
	{ 0xf800, 0xf800, primella_ctrl_w },	/* bank switch, flip screen etc */
	{ 0xf810, 0xf810, soundlatch_w },
	{ 0xfc00, 0xfc04, MWA_RAM, &lastday_bgscroll },
	{ 0xfc08, 0xfc0c, MWA_RAM, &lastday_fgscroll },
MEMORY_END

static MEMORY_READ16_START( rshark_readmem )
	MEMORY_ADDRESS_BITS(20) /* super-x needs this and is similar */
	{ 0x000000, 0x03ffff, MRA16_ROM },
	{ 0x040000, 0x04cfff, MRA16_RAM },
	{ 0x04d000, 0x04dfff, MRA16_RAM },
	{ 0x04e000, 0x04ffff, MRA16_RAM },
	{ 0x0c0002, 0x0c0003, input_port_0_word_r },
	{ 0x0c0004, 0x0c0005, input_port_1_word_r },
	{ 0x0c0006, 0x0c0007, input_port_2_word_r },
MEMORY_END

static MEMORY_WRITE16_START( rshark_writemem )
	MEMORY_ADDRESS_BITS(20) /* super-x needs this and is similar */
	{ 0x000000, 0x03ffff, MWA16_ROM },
	{ 0x040000, 0x04cfff, MWA16_RAM },
	{ 0x04d000, 0x04dfff, MWA16_RAM, &spriteram16, &spriteram_size },
	{ 0x04e000, 0x04ffff, MWA16_RAM },
	{ 0x0c4000, 0x0c4009, MWA16_RAM, &rshark_scroll4 },
	{ 0x0c4010, 0x0c4019, MWA16_RAM, &rshark_scroll3 },
	{ 0x0c8000, 0x0c8fff, paletteram16_xRRRRRGGGGGBBBBB_word_w, &paletteram16 },
	{ 0x0c0012, 0x0c0013, soundlatch_word_w },
	{ 0x0c0014, 0x0c0015, rshark_ctrl_w },	/* flip screen + unknown stuff */
	{ 0x0cc000, 0x0cc009, MWA16_RAM, &rshark_scroll2 },
	{ 0x0cc010, 0x0cc019, MWA16_RAM, &rshark_scroll1 },
MEMORY_END

static MEMORY_READ16_START( superx_readmem )
	MEMORY_ADDRESS_BITS(20)
	{ 0x000000, 0x03ffff, MRA16_ROM },
	{ 0x0d0000, 0x0dcfff, MRA16_RAM },
	{ 0x0dd000, 0x0ddfff, MRA16_RAM },
	{ 0x0de000, 0x0dffff, MRA16_RAM },
	{ 0x080002, 0x080003, input_port_0_word_r },
	{ 0x080004, 0x080005, input_port_1_word_r },
	{ 0x080006, 0x080007, input_port_2_word_r },
MEMORY_END

static MEMORY_WRITE16_START( superx_writemem )
	MEMORY_ADDRESS_BITS(20)
	{ 0x000000, 0x03ffff, MWA16_ROM },
	{ 0x0d0000, 0x0dcfff, MWA16_RAM },
	{ 0x0dd000, 0x0ddfff, MWA16_RAM, &spriteram16, &spriteram_size },
	{ 0x0de000, 0x0dffff, MWA16_RAM },
	{ 0x084000, 0x084009, MWA16_RAM, &rshark_scroll4 },
	{ 0x084010, 0x084019, MWA16_RAM, &rshark_scroll3 },
	{ 0x088000, 0x088fff, paletteram16_xRRRRRGGGGGBBBBB_word_w, &paletteram16 },
	{ 0x080012, 0x080013, soundlatch_word_w },
	{ 0x080014, 0x080015, rshark_ctrl_w },	/* flip screen + unknown stuff */
	{ 0x08c000, 0x08c009, MWA16_RAM, &rshark_scroll2 },
	{ 0x08c010, 0x08c019, MWA16_RAM, &rshark_scroll1 },
MEMORY_END



static MEMORY_READ_START( lastday_sound_readmem )
	{ 0x0000, 0x7fff, MRA_ROM },
	{ 0xc000, 0xc7ff, MRA_RAM },
	{ 0xc800, 0xc800, soundlatch_r },
	{ 0xf000, 0xf000, YM2203_status_port_0_r },
	{ 0xf001, 0xf001, YM2203_read_port_0_r },
	{ 0xf002, 0xf002, YM2203_status_port_1_r },
	{ 0xf003, 0xf003, YM2203_read_port_1_r },
MEMORY_END

static MEMORY_WRITE_START( lastday_sound_writemem )
	{ 0x0000, 0x7fff, MWA_ROM },
	{ 0xc000, 0xc7ff, MWA_RAM },
	{ 0xf000, 0xf000, YM2203_control_port_0_w },
	{ 0xf001, 0xf001, YM2203_write_port_0_w },
	{ 0xf002, 0xf002, YM2203_control_port_1_w },
	{ 0xf003, 0xf003, YM2203_write_port_1_w },
MEMORY_END

static MEMORY_READ_START( pollux_sound_readmem )
	{ 0x0000, 0xefff, MRA_ROM },
	{ 0xf000, 0xf7ff, MRA_RAM },
	{ 0xf800, 0xf800, soundlatch_r },
	{ 0xf802, 0xf802, YM2203_status_port_0_r },
	{ 0xf803, 0xf803, YM2203_read_port_0_r },
	{ 0xf804, 0xf804, YM2203_status_port_1_r },
	{ 0xf805, 0xf805, YM2203_read_port_1_r },
MEMORY_END

static MEMORY_WRITE_START( pollux_sound_writemem )
	{ 0x0000, 0xefff, MWA_ROM },
	{ 0xf000, 0xf7ff, MWA_RAM },
	{ 0xf802, 0xf802, YM2203_control_port_0_w },
	{ 0xf803, 0xf803, YM2203_write_port_0_w },
	{ 0xf804, 0xf804, YM2203_control_port_1_w },
	{ 0xf805, 0xf805, YM2203_write_port_1_w },
MEMORY_END

static MEMORY_READ_START( bluehawk_sound_readmem )
	{ 0x0000, 0xefff, MRA_ROM },
	{ 0xf000, 0xf7ff, MRA_RAM },
	{ 0xf800, 0xf800, soundlatch_r },
	{ 0xf809, 0xf809, YM2151_status_port_0_r },
	{ 0xf80a, 0xf80a, OKIM6295_status_0_r },
MEMORY_END

static MEMORY_WRITE_START( bluehawk_sound_writemem )
	{ 0x0000, 0xefff, MWA_ROM },
	{ 0xf000, 0xf7ff, MWA_RAM },
	{ 0xf808, 0xf808, YM2151_register_port_0_w },
	{ 0xf809, 0xf809, YM2151_data_port_0_w },
	{ 0xf80a, 0xf80a, OKIM6295_data_0_w },
MEMORY_END



INPUT_PORTS_START( lastday )
	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_TILT )	/* maybe, but I'm not sure */
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START
	PORT_SERVICE( 0x01, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0xc2, 0xc2, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x42, DEF_STR( 2C_1C ) )
//	PORT_DIPSETTING(    0xc0, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0xc2, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x82, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_6C ) )

	PORT_START
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPSETTING(    0x02, "2" )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x08, "Easy" )
	PORT_DIPSETTING(    0x0c, "Normal" )
	PORT_DIPSETTING(    0x04, "Hard" )
	PORT_DIPSETTING(    0x00, "Hardest" )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(    0x30, "Every 200000" )
	PORT_DIPSETTING(    0x20, "Every 240000" )
	PORT_DIPSETTING(    0x10, "280000" )
	PORT_DIPSETTING(    0x00, "None" )
	PORT_DIPNAME( 0x40, 0x40, "Speed" )
	PORT_DIPSETTING(    0x00, "Low" )
	PORT_DIPSETTING(    0x40, "High" )
	PORT_DIPNAME( 0x80, 0x80, "Allow Continue" )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Yes ) )
INPUT_PORTS_END

INPUT_PORTS_START( gulfstrm )
	PORT_START
	PORT_SERVICE( 0x01, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0xc2, 0xc2, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x42, DEF_STR( 2C_1C ) )
//	PORT_DIPSETTING(    0xc0, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0xc2, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x82, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_6C ) )

	PORT_START
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPSETTING(    0x02, "2" )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x08, "Easy" )
	PORT_DIPSETTING(    0x0c, "Normal" )
	PORT_DIPSETTING(    0x04, "Hard" )
	PORT_DIPSETTING(    0x00, "Hardest" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "Allow Continue" )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Yes ) )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 | IPF_PLAYER2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 | IPF_PLAYER1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_VBLANK )	/* ??? */
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

INPUT_PORTS_START( pollux )
	PORT_START
	PORT_SERVICE( 0x01, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0xc2, 0xc2, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x42, DEF_STR( 2C_1C ) )
//	PORT_DIPSETTING(    0xc0, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0xc2, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x82, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_6C ) )

	PORT_START
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPSETTING(    0x02, "2" )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x08, "Easy" )
	PORT_DIPSETTING(    0x0c, "Normal" )
	PORT_DIPSETTING(    0x04, "Hard" )
	PORT_DIPSETTING(    0x00, "Hardest" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "Allow Continue" )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Yes ) )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 | IPF_PLAYER1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 | IPF_PLAYER2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

INPUT_PORTS_START( bluehawk )
	PORT_START
	PORT_SERVICE( 0x01, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0xc2, 0xc2, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x42, DEF_STR( 2C_1C ) )
//	PORT_DIPSETTING(    0xc0, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0xc2, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x82, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_6C ) )

	PORT_START
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "1" )
	PORT_DIPSETTING(    0x02, "2" )
	PORT_DIPSETTING(    0x03, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x08, "Easy" )
	PORT_DIPSETTING(    0x0c, "Normal" )
	PORT_DIPSETTING(    0x04, "Hard" )
	PORT_DIPSETTING(    0x00, "Hardest" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "Allow Continue" )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Yes ) )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 | IPF_PLAYER1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 | IPF_PLAYER2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

INPUT_PORTS_START( primella )
	PORT_START
	PORT_SERVICE( 0x01, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x04, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x30, 0x30, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0xc2, 0xc2, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x42, DEF_STR( 2C_1C ) )
//	PORT_DIPSETTING(    0xc0, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0xc2, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x82, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x80, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x40, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_6C ) )

	PORT_START
	PORT_DIPNAME( 0x03, 0x01, "Show Girl" )
	PORT_DIPSETTING(    0x00, "Skip Skip Skip" )
	PORT_DIPSETTING(    0x03, "Dress Dress Dress" )
	PORT_DIPSETTING(    0x02, "Dress Half Half" )
	PORT_DIPSETTING(    0x01, "Dress Half Nake" )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x08, "Easy" )
	PORT_DIPSETTING(    0x0c, "Normal" )
	PORT_DIPSETTING(    0x04, "Hard" )
	PORT_DIPSETTING(    0x00, "Hardest" )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, "Allow Continue" )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Yes ) )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 | IPF_PLAYER1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_BUTTON3 | IPF_PLAYER2 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

INPUT_PORTS_START( rshark )
	PORT_START
	PORT_SERVICE( 0x0001, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0030, 0x0030, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0030, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( 1C_2C ) )
	PORT_DIPNAME( 0x00c2, 0x00c2, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(      0x0042, DEF_STR( 2C_1C ) )
//	PORT_DIPSETTING(      0x00c0, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x00c2, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(      0x0082, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x0300, 0x0300, DEF_STR( Lives ) )
	PORT_DIPSETTING(      0x0000, "1" )
	PORT_DIPSETTING(      0x0200, "2" )
	PORT_DIPSETTING(      0x0300, "3" )
	PORT_DIPSETTING(      0x0100, "4" )
	PORT_DIPNAME( 0x0c00, 0x0c00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x0800, "Easy" )
	PORT_DIPSETTING(      0x0c00, "Normal" )
	PORT_DIPSETTING(      0x0400, "Hard" )
	PORT_DIPSETTING(      0x0000, "Hardest" )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, "Allow Continue" )
	PORT_DIPSETTING(      0x0000, DEF_STR( No ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Yes ) )

	PORT_START
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 | IPF_PLAYER1 )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BUTTON4 | IPF_PLAYER1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_BUTTON3 | IPF_PLAYER2 )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_BUTTON4 | IPF_PLAYER2 )

	PORT_START
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END



static struct GfxLayout lastday_charlayout =
{
	8,8,
	RGN_FRAC(1,2),
	4,
	{ 0, 4, RGN_FRAC(1,2)+0, RGN_FRAC(1,2)+4 },
	{ 0, 1, 2, 3, 8+0, 8+1, 8+2, 8+3 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	16*8
};

static struct GfxLayout bluehawk_charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8
};

static struct GfxLayout tilelayout =
{
	32,32,
	RGN_FRAC(1,1),
	4,
	{ 0*4, 1*4, 2*4, 3*4 },
	{ 0, 1, 2, 3, 16+0, 16+1, 16+2, 16+3,
			32*32+0, 32*32+1, 32*32+2, 32*32+3, 32*32+16+0, 32*32+16+1, 32*32+16+2, 32*32+16+3,
			2*32*32+0, 2*32*32+1, 2*32*32+2, 2*32*32+3, 2*32*32+16+0, 2*32*32+16+1, 2*32*32+16+2, 2*32*32+16+3,
			3*32*32+0, 3*32*32+1, 3*32*32+2, 3*32*32+3, 3*32*32+16+0, 3*32*32+16+1, 3*32*32+16+2, 3*32*32+16+3 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
			8*32, 9*32, 10*32, 11*32, 12*32, 13*32, 14*32, 15*32,
			16*32, 17*32, 18*32, 19*32, 20*32, 21*32, 22*32, 23*32,
			24*32, 25*32, 26*32, 27*32, 28*32, 29*32, 30*32, 31*32 },
	512*8
};

static struct GfxLayout spritelayout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0*4, 1*4, 2*4, 3*4 },
	{ 0, 1, 2, 3, 16+0, 16+1, 16+2, 16+3,
			16*32+0, 16*32+1, 16*32+2, 16*32+3, 16*32+16+0, 16*32+16+1, 16*32+16+2, 16*32+16+3 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
			8*32, 9*32, 10*32, 11*32, 12*32, 13*32, 14*32, 15*32 },
	128*8
};

static struct GfxLayout rshark_spritelayout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4,
			16*32+0*4, 16*32+1*4, 16*32+2*4, 16*32+3*4, 16*32+4*4, 16*32+5*4, 16*32+6*4, 16*32+7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
			8*32, 9*32, 10*32, 11*32, 12*32, 13*32, 14*32, 15*32 },
	128*8
};


static struct GfxDecodeInfo lastday_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &lastday_charlayout,   0, 16 },
	{ REGION_GFX2, 0, &spritelayout,       256, 16 },
	{ REGION_GFX3, 0, &tilelayout,         768, 16 },
	{ REGION_GFX4, 0, &tilelayout,         512, 16 },
	{ -1 } /* end of array */
};

static struct GfxDecodeInfo bluehawk_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &bluehawk_charlayout,  0, 16 },
	{ REGION_GFX2, 0, &spritelayout,       256, 16 },
	{ REGION_GFX3, 0, &tilelayout,         768, 16 },
	{ REGION_GFX4, 0, &tilelayout,         512, 16 },
	{ REGION_GFX5, 0, &tilelayout,           0, 16 },
	{ -1 } /* end of array */
};

static struct GfxDecodeInfo primella_gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &bluehawk_charlayout,  0, 16 },
	/* no sprites */
	{ REGION_GFX2, 0, &tilelayout,         768, 16 },
	{ REGION_GFX3, 0, &tilelayout,         512, 16 },
	{ -1 } /* end of array */
};

static struct GfxDecodeInfo rshark_gfxdecodeinfo[] =
{
	/* no chars */
	{ REGION_GFX1, 0, &rshark_spritelayout,  0, 16 },
	{ REGION_GFX2, 0, &spritelayout,       256, 16 },
	{ REGION_GFX3, 0, &spritelayout,       512, 16 },
	{ REGION_GFX4, 0, &spritelayout,       768, 16 },
	{ REGION_GFX5, 0, &spritelayout,      1024, 16 },
	{ -1 } /* end of array */
};



static void irqhandler(int irq)
{
	cpu_set_irq_line(1,0,irq ? ASSERT_LINE : CLEAR_LINE);
}

READ_HANDLER( unk_r )
{
	return 0;
}

static struct YM2203interface ym2203_interface =
{
	2,			/* 2 chips */
	4000000,	/* 4 MHz ? */
	{ YM2203_VOL(40,40), YM2203_VOL(40,40) },
	{ unk_r, unk_r },
	{ 0 },
	{ 0 },
	{ 0 },
	{ irqhandler }
};

static struct YM2151interface bluehawk_ym2151_interface =
{
	1,			/* 1 chip */
	3579545,	/* 3.579545 MHz ? */
	{ YM3012_VOL(50,MIXER_PAN_CENTER,50,MIXER_PAN_CENTER) },
	{ irqhandler },
	{ 0 }
};

static struct YM2151interface primella_ym2151_interface =
{
	1,			/* 1 chip */
	4000000,	/* measured on super-x */
	{ YM3012_VOL(40,MIXER_PAN_CENTER,40,MIXER_PAN_CENTER) },
	{ irqhandler },
	{ 0 }
};

static struct OKIM6295interface okim6295_interface =
{
	1,                  /* 1 chip */
	{ 1000000/132 },           /* measured on super-x */
	{ REGION_SOUND1 },	/* memory region */
	{ 60 }
};



static MACHINE_DRIVER_START( lastday )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 8000000)	/* ??? */
	MDRV_CPU_MEMORY(lastday_readmem,lastday_writemem)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	MDRV_CPU_ADD(Z80, 4000000)
	MDRV_CPU_FLAGS(CPU_AUDIO_CPU)	/* ??? */
	MDRV_CPU_MEMORY(lastday_sound_readmem,lastday_sound_writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_BUFFERS_SPRITERAM)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(8*8, (64-8)*8-1, 1*8, 31*8-1 )
	MDRV_GFXDECODE(lastday_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_EOF(dooyong)
	MDRV_VIDEO_UPDATE(lastday)

	/* sound hardware */
	MDRV_SOUND_ADD(YM2203, ym2203_interface)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( gulfstrm )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 8000000)	/* ??? */
	MDRV_CPU_MEMORY(pollux_readmem,pollux_writemem)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	MDRV_CPU_ADD(Z80, 4000000)
	MDRV_CPU_FLAGS(CPU_AUDIO_CPU)	/* ??? */
	MDRV_CPU_MEMORY(lastday_sound_readmem,lastday_sound_writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_BUFFERS_SPRITERAM)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(8*8, (64-8)*8-1, 1*8, 31*8-1 )
	MDRV_GFXDECODE(lastday_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_EOF(dooyong)
	MDRV_VIDEO_UPDATE(gulfstrm)

	/* sound hardware */
	MDRV_SOUND_ADD(YM2203, ym2203_interface)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( pollux )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 8000000)	/* ??? */
	MDRV_CPU_MEMORY(pollux_readmem,pollux_writemem)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	MDRV_CPU_ADD(Z80, 4000000)
	MDRV_CPU_FLAGS(CPU_AUDIO_CPU)	/* ??? */
	MDRV_CPU_MEMORY(pollux_sound_readmem,pollux_sound_writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_BUFFERS_SPRITERAM)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(8*8, (64-8)*8-1, 1*8, 31*8-1 )
	MDRV_GFXDECODE(lastday_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_EOF(dooyong)
	MDRV_VIDEO_UPDATE(pollux)

	/* sound hardware */
	MDRV_SOUND_ADD(YM2203, ym2203_interface)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( bluehawk )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 8000000)	/* ??? */
	MDRV_CPU_MEMORY(bluehawk_readmem,bluehawk_writemem)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	MDRV_CPU_ADD(Z80, 4000000)
	MDRV_CPU_FLAGS(CPU_AUDIO_CPU)	/* ??? */
	MDRV_CPU_MEMORY(bluehawk_sound_readmem,bluehawk_sound_writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_BUFFERS_SPRITERAM)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(8*8, (64-8)*8-1, 1*8, 31*8-1 )
	MDRV_GFXDECODE(bluehawk_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_EOF(dooyong)
	MDRV_VIDEO_UPDATE(bluehawk)

	/* sound hardware */
	MDRV_SOUND_ADD(YM2151, bluehawk_ym2151_interface)
	MDRV_SOUND_ADD(OKIM6295, okim6295_interface)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( primella )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 8000000)	/* ??? */
	MDRV_CPU_MEMORY(primella_readmem,primella_writemem)
	MDRV_CPU_VBLANK_INT(irq0_line_hold,1)

	MDRV_CPU_ADD(Z80, 4000000)
	MDRV_CPU_FLAGS(CPU_AUDIO_CPU)	/* ??? */
	MDRV_CPU_MEMORY(bluehawk_sound_readmem,bluehawk_sound_writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_BUFFERS_SPRITERAM)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(8*8, (64-8)*8-1, 0*8, 32*8-1 )
	MDRV_GFXDECODE(primella_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(1024)

	MDRV_VIDEO_EOF(dooyong)
	MDRV_VIDEO_UPDATE(primella)

	/* sound hardware */
	MDRV_SOUND_ADD(YM2151, primella_ym2151_interface)
	MDRV_SOUND_ADD(OKIM6295, okim6295_interface)
MACHINE_DRIVER_END

static INTERRUPT_GEN( rshark_interrupt )
{
	if (cpu_getiloops() == 0)
		cpu_set_irq_line(0, 5, HOLD_LINE);
	else
		cpu_set_irq_line(0, 6, HOLD_LINE);
}

static MACHINE_DRIVER_START( rshark )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000, 8000000)	/* measured on super-x */
	MDRV_CPU_MEMORY(rshark_readmem,rshark_writemem)
	MDRV_CPU_VBLANK_INT(rshark_interrupt,2)	/* 5 and 6 */

	MDRV_CPU_ADD(Z80, 4000000)
	MDRV_CPU_FLAGS(CPU_AUDIO_CPU)	/* measured on super-x */
	MDRV_CPU_MEMORY(bluehawk_sound_readmem,bluehawk_sound_writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_BUFFERS_SPRITERAM)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(8*8, (64-8)*8-1, 1*8, 31*8-1 )
	MDRV_GFXDECODE(rshark_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(2048)

	MDRV_VIDEO_EOF(rshark)
	MDRV_VIDEO_UPDATE(rshark)

	/* sound hardware */
	MDRV_SOUND_ADD(YM2151, primella_ym2151_interface)
	MDRV_SOUND_ADD(OKIM6295, okim6295_interface)
MACHINE_DRIVER_END

static MACHINE_DRIVER_START( superx ) // dif mem map

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000, 8000000)	/* measured on super-x */
	MDRV_CPU_MEMORY(superx_readmem,superx_writemem)
	MDRV_CPU_VBLANK_INT(rshark_interrupt,2)	/* 5 and 6 */

	MDRV_CPU_ADD(Z80, 4000000)
	MDRV_CPU_FLAGS(CPU_AUDIO_CPU)	/* measured on super-x */
	MDRV_CPU_MEMORY(bluehawk_sound_readmem,bluehawk_sound_writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_BUFFERS_SPRITERAM)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(8*8, (64-8)*8-1, 1*8, 31*8-1 )
	MDRV_GFXDECODE(rshark_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(2048)

	MDRV_VIDEO_EOF(rshark)
	MDRV_VIDEO_UPDATE(rshark)

	/* sound hardware */
	MDRV_SOUND_ADD(YM2151, primella_ym2151_interface)
	MDRV_SOUND_ADD(OKIM6295, okim6295_interface)
MACHINE_DRIVER_END

ROM_START( lastday )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD( "lday3.bin",    0x00000, 0x10000, CRC(a06dfb1e) SHA1(c6220eda8c01d55862700e369db7291dbbedc8c8) )
	ROM_RELOAD(               0x10000, 0x10000 )				/* banked at 0x8000-0xbfff */
	ROM_LOAD( "lday4.bin",    0x20000, 0x10000, CRC(70961ea6) SHA1(245d3da67abb4a511a024f030de461b9a2b4804e) )	/* banked at 0x8000-0xbfff */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* sound */
	ROM_LOAD( "lday1.bin",    0x0000, 0x8000, CRC(dd4316fd) SHA1(496e6657bb76d91f488a2464d1af1be095ab9105) )	/* empty */
	ROM_CONTINUE(             0x0000, 0x8000 )

	ROM_REGION( 0x8000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "lday2.bin",    0x0000, 0x8000, CRC(83eb572c) SHA1(e915afd55d505bce202206c9ecfa89bad561ef6c) )	/* empty */
	ROM_CONTINUE(             0x0000, 0x8000 )

	ROM_REGION( 0x40000, REGION_GFX2, ROMREGION_DISPOSE )	/* sprites */
	ROM_LOAD16_BYTE( "lday16.bin",   0x00000, 0x20000, CRC(df503504) SHA1(daa58a7bc24415b5f59b7c7cc918bc85de9702a3) )
	ROM_LOAD16_BYTE( "lday15.bin",   0x00001, 0x20000, CRC(cd990442) SHA1(891b2163db23ab0bb40cbadce6e06fc067d0532f) )

	ROM_REGION( 0x80000, REGION_GFX3, ROMREGION_DISPOSE )	/* tiles */
	ROM_LOAD16_BYTE( "lday6.bin",    0x00000, 0x20000, CRC(1054361d) SHA1(52566786ca8177404be8b66fd7de94ac25fc49ea) )
	ROM_LOAD16_BYTE( "lday9.bin",    0x00001, 0x20000, CRC(6952ef4d) SHA1(b4e5ec02e97df213fe0bd4cd8a2ca77d7ecf8ad5) )
	ROM_LOAD16_BYTE( "lday7.bin",    0x40000, 0x20000, CRC(6e57a888) SHA1(8efe876ea3c788b83e8291f7fc6f55b90de158c8) )
	ROM_LOAD16_BYTE( "lday10.bin",   0x40001, 0x20000, CRC(a5548dca) SHA1(9914e01c1739c3bfd868a01e53c9030726ced4ea) )

	ROM_REGION( 0x40000, REGION_GFX4, ROMREGION_DISPOSE )	/* tiles */
	ROM_LOAD16_BYTE( "lday12.bin",   0x00000, 0x20000, CRC(992bc4af) SHA1(94570ebd1ee6acf1871cf914907acd12dca4026e) )
	ROM_LOAD16_BYTE( "lday14.bin",   0x00001, 0x20000, CRC(a79abc85) SHA1(3e63dad11db9b7420331403a1d551d8c041c4cc2) )

	ROM_REGION( 0x20000, REGION_GFX5, 0 )	/* background tilemaps */
	ROM_LOAD16_BYTE( "lday5.bin",    0x00000, 0x10000, CRC(4789bae8) SHA1(6ffecc16eb8c9c783b02c4ef68cb5098b01fafef) )
	ROM_LOAD16_BYTE( "lday8.bin",    0x00001, 0x10000, CRC(92402b9a) SHA1(2ca8078d2687afbe7b6fc5412de16c6fbc11a650) )

	ROM_REGION( 0x20000, REGION_GFX6, 0 )	/* fg tilemaps */
	ROM_LOAD16_BYTE( "lday11.bin",   0x00000, 0x10000, CRC(04b961de) SHA1(7a94c9d0800d79048660cf3758708a346ead33f9) )
	ROM_LOAD16_BYTE( "lday13.bin",   0x00001, 0x10000, CRC(6bdbd887) SHA1(a54f26f9ddd72b8b8f7a030610c1c4a5f94a3358) )
ROM_END

ROM_START( lastdaya )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD( "lday3.bin",    0x00000, 0x10000, CRC(a06dfb1e) SHA1(c6220eda8c01d55862700e369db7291dbbedc8c8) )
	ROM_RELOAD(               0x10000, 0x10000 )				/* banked at 0x8000-0xbfff */
	ROM_LOAD( "lday4.bin",    0x20000, 0x10000, CRC(70961ea6) SHA1(245d3da67abb4a511a024f030de461b9a2b4804e) )	/* banked at 0x8000-0xbfff */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* sound */
	ROM_LOAD( "e1",           0x0000, 0x8000, CRC(ce96e106) SHA1(5ef1f221618abd757e02db79c3d7016100f30c07) )	/* empty */
	ROM_CONTINUE(             0x0000, 0x8000 )

	ROM_REGION( 0x8000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "lday2.bin",    0x0000, 0x8000, CRC(83eb572c) SHA1(e915afd55d505bce202206c9ecfa89bad561ef6c) )	/* empty */
	ROM_CONTINUE(             0x0000, 0x8000 )

	ROM_REGION( 0x40000, REGION_GFX2, ROMREGION_DISPOSE )	/* sprites */
	ROM_LOAD16_BYTE( "lday16.bin",   0x00000, 0x20000, CRC(df503504) SHA1(daa58a7bc24415b5f59b7c7cc918bc85de9702a3) )
	ROM_LOAD16_BYTE( "lday15.bin",   0x00001, 0x20000, CRC(cd990442) SHA1(891b2163db23ab0bb40cbadce6e06fc067d0532f) )

	ROM_REGION( 0x80000, REGION_GFX3, ROMREGION_DISPOSE )	/* tiles */
	ROM_LOAD16_BYTE( "e6",           0x00000, 0x20000, CRC(7623c443) SHA1(abfed648a8cc438dbb7de9c23a663082667ca366) )
	ROM_LOAD16_BYTE( "e9",           0x00001, 0x20000, CRC(717f6a0e) SHA1(0b2d98fa5b8734210df18bce7725972fd42a6e4a) )
	ROM_LOAD16_BYTE( "lday7.bin",    0x40000, 0x20000, CRC(6e57a888) SHA1(8efe876ea3c788b83e8291f7fc6f55b90de158c8) )
	ROM_LOAD16_BYTE( "lday10.bin",   0x40001, 0x20000, CRC(a5548dca) SHA1(9914e01c1739c3bfd868a01e53c9030726ced4ea) )

	ROM_REGION( 0x40000, REGION_GFX4, ROMREGION_DISPOSE )	/* tiles */
	ROM_LOAD16_BYTE( "lday12.bin",   0x00000, 0x20000, CRC(992bc4af) SHA1(94570ebd1ee6acf1871cf914907acd12dca4026e) )
	ROM_LOAD16_BYTE( "lday14.bin",   0x00001, 0x20000, CRC(a79abc85) SHA1(3e63dad11db9b7420331403a1d551d8c041c4cc2) )

	ROM_REGION( 0x20000, REGION_GFX5, 0 )	/* bg tilemaps */
	ROM_LOAD16_BYTE( "e5",           0x00000, 0x10000, CRC(5f801410) SHA1(382c1bcd69a6a5c245d2ba7603bc273fba840c8f) )
	ROM_LOAD16_BYTE( "e8",           0x00001, 0x10000, CRC(a7b8250b) SHA1(4bd79c09dacf69e1993353d7fcc7746d1324e9b0) )

	ROM_REGION( 0x20000, REGION_GFX6, 0 )	/* fg tilemaps */
	ROM_LOAD16_BYTE( "lday11.bin",   0x00000, 0x10000, CRC(04b961de) SHA1(7a94c9d0800d79048660cf3758708a346ead33f9) )
	ROM_LOAD16_BYTE( "lday13.bin",   0x00001, 0x10000, CRC(6bdbd887) SHA1(a54f26f9ddd72b8b8f7a030610c1c4a5f94a3358) )
ROM_END

ROM_START( gulfstrm )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD( "1.l4",         0x00000, 0x20000, CRC(59e0478b) SHA1(dd6e48c6e91ddb087d20336eab79bbadd968d4b1) )
	ROM_RELOAD(               0x10000, 0x20000 )				/* banked at 0x8000-0xbfff */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* sound */
	ROM_LOAD( "3.c5",         0x00000, 0x10000, CRC(c029b015) SHA1(86f8d4f6560cb99e25e8e8baf72dde743a7b9c4c) )

	ROM_REGION( 0x8000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "2.s4",         0x0000, 0x8000, CRC(c2d65a25) SHA1(a198b42c0737b253aca5bab6fb58ab561ccc1d5c) )	/* empty */
	ROM_CONTINUE(             0x0000, 0x8000 )

	ROM_REGION( 0x80000, REGION_GFX2, ROMREGION_DISPOSE )	/* sprites */
	ROM_LOAD16_BYTE( "14.b1",        0x00000, 0x20000, CRC(67bdf73d) SHA1(3e357448b6f255fdec731f143afa3d3149523ed2) )
	ROM_LOAD16_BYTE( "16.c1",        0x00001, 0x20000, CRC(7770a76f) SHA1(4f9f5245f59008b26ed60e636285ea85271744e7) )
	ROM_LOAD16_BYTE( "15.b1",        0x40000, 0x20000, CRC(84803f7e) SHA1(74b694c0d20c5b016b9d7258b0296229972151d5) )
	ROM_LOAD16_BYTE( "17.e1",        0x40001, 0x20000, CRC(94706500) SHA1(8f4a6f7ce20b1b50577271601c2c2632b5a2292c) )

	ROM_REGION( 0x80000, REGION_GFX3, ROMREGION_DISPOSE )	/* tiles */
	ROM_LOAD16_BYTE( "4.d8",         0x00000, 0x20000, CRC(858fdbb6) SHA1(4c317ab6069a8509287d3df88cf4272f512a40a3) )
	ROM_LOAD16_BYTE( "5.b9",         0x00001, 0x20000, CRC(c0a552e8) SHA1(31dcb14eb8815c609b0bf4d5f1ea17b26ab18aec) )
	ROM_LOAD16_BYTE( "6.d8",         0x40000, 0x20000, CRC(20eedda3) SHA1(8c8b1284e07f5380037f8431f2649aa99fd47542) )
	ROM_LOAD16_BYTE( "7.d9",         0x40001, 0x20000, CRC(294f8c40) SHA1(b7afb87510ab52682151ff2b13029427487589ec) )

	ROM_REGION( 0x40000, REGION_GFX4, ROMREGION_DISPOSE )	/* tiles */
	ROM_LOAD16_BYTE( "12.r8",        0x00000, 0x20000, CRC(ec3ad3e7) SHA1(276da309b788091cd6e5faada2ff9e5b0df2caea) )
	ROM_LOAD16_BYTE( "13.r9",        0x00001, 0x20000, CRC(c64090cb) SHA1(5dab576e5f454c62c7826d477b3f699e979753ad) )

	ROM_REGION( 0x20000, REGION_GFX5, 0 )	/* background tilemaps */
	ROM_LOAD16_BYTE( "8.e8",         0x00000, 0x10000, CRC(8d7f4693) SHA1(a7c8573d9e54c8230decc3e88f76ae729d77b096) )
	ROM_LOAD16_BYTE( "9.e9",         0x00001, 0x10000, CRC(34d440c4) SHA1(74b0e15e75f62106177234b6ea54a5d312628802) )

	ROM_REGION( 0x20000, REGION_GFX6, 0 )	/* fg tilemaps */
	ROM_LOAD16_BYTE( "10.n8",        0x00000, 0x10000, CRC(b4f15bf4) SHA1(cb203390c3f917f213807a23c442e43bc6bcfc67) )
	ROM_LOAD16_BYTE( "11.n9",        0x00001, 0x10000, CRC(7dfe4a9c) SHA1(40982b5b266e4a928544ab5ec330080935588c57) )
ROM_END

ROM_START( gulfstr2 )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD( "18.1",         0x00000, 0x20000, CRC(d38e2667) SHA1(3690d708c7be85871d6bb32a774d711a30782126) )
	ROM_RELOAD(               0x10000, 0x20000 )				/* banked at 0x8000-0xbfff */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* sound */
	ROM_LOAD( "3.c5",         0x00000, 0x10000, CRC(c029b015) SHA1(86f8d4f6560cb99e25e8e8baf72dde743a7b9c4c) )

	ROM_REGION( 0x8000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "2.bin",        0x0000, 0x8000, CRC(cb555d96) SHA1(ebc1dee91a09a829db2ae6fc1616c7c989f7f1c2) )	/* empty */
	ROM_CONTINUE(             0x0000, 0x8000 )

	ROM_REGION( 0x80000, REGION_GFX2, ROMREGION_DISPOSE )	/* sprites */
	ROM_LOAD16_BYTE( "14.b1",        0x00000, 0x20000, CRC(67bdf73d) SHA1(3e357448b6f255fdec731f143afa3d3149523ed2) )
	ROM_LOAD16_BYTE( "16.c1",        0x00001, 0x20000, CRC(7770a76f) SHA1(4f9f5245f59008b26ed60e636285ea85271744e7) )
	ROM_LOAD16_BYTE( "15.b1",        0x40000, 0x20000, CRC(84803f7e) SHA1(74b694c0d20c5b016b9d7258b0296229972151d5) )
	ROM_LOAD16_BYTE( "17.e1",        0x40001, 0x20000, CRC(94706500) SHA1(8f4a6f7ce20b1b50577271601c2c2632b5a2292c) )

	ROM_REGION( 0x80000, REGION_GFX3, ROMREGION_DISPOSE )	/* tiles */
	ROM_LOAD16_BYTE( "4.d8",         0x00000, 0x20000, CRC(858fdbb6) SHA1(4c317ab6069a8509287d3df88cf4272f512a40a3) )
	ROM_LOAD16_BYTE( "5.b9",         0x00001, 0x20000, CRC(c0a552e8) SHA1(31dcb14eb8815c609b0bf4d5f1ea17b26ab18aec) )
	ROM_LOAD16_BYTE( "6.d8",         0x40000, 0x20000, CRC(20eedda3) SHA1(8c8b1284e07f5380037f8431f2649aa99fd47542) )
	ROM_LOAD16_BYTE( "7.d9",         0x40001, 0x20000, CRC(294f8c40) SHA1(b7afb87510ab52682151ff2b13029427487589ec) )

	ROM_REGION( 0x40000, REGION_GFX4, ROMREGION_DISPOSE )	/* tiles */
	ROM_LOAD16_BYTE( "12.bin",       0x00000, 0x20000, CRC(3e3d3b57) SHA1(398a6cac7144ba7bacaa36c593bcb4b3c051eb0f) )
	ROM_LOAD16_BYTE( "13.bin",       0x00001, 0x20000, CRC(66fcce80) SHA1(6ab2b7cd49447d374cde40b98db0a6209dcad461) )

	ROM_REGION( 0x20000, REGION_GFX5, 0 )	/* background tilemaps */
	ROM_LOAD16_BYTE( "8.e8",         0x00000, 0x10000, CRC(8d7f4693) SHA1(a7c8573d9e54c8230decc3e88f76ae729d77b096) )
	ROM_LOAD16_BYTE( "9.e9",         0x00001, 0x10000, CRC(34d440c4) SHA1(74b0e15e75f62106177234b6ea54a5d312628802) )

	ROM_REGION( 0x20000, REGION_GFX6, 0 )	/* fg tilemaps */
	ROM_LOAD16_BYTE( "10.bin",       0x00000, 0x10000, CRC(08149140) SHA1(ff0094883ca0fc81bae991d6ea62d0064d6f7c47) )
	ROM_LOAD16_BYTE( "11.bin",       0x00001, 0x10000, CRC(2ed7545b) SHA1(6a70743bbb03ef694310f2b5531f384209db62a1) )
ROM_END

ROM_START( pollux )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD( "pollux2.bin",  0x00000, 0x10000, CRC(45e10d4e) SHA1(ece25fcc0acda9a8cfc00f3132a87469037b5a4e) )
	ROM_RELOAD(               0x10000, 0x10000 )	/* banked at 0x8000-0xbfff */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* sound */
	ROM_LOAD( "pollux3.bin",  0x00000, 0x10000, CRC(85a9dc98) SHA1(a349bfb05d870ba920469066ce5c007363aca348) )

	ROM_REGION( 0x10000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "pollux1.bin",  0x08000, 0x08000, CRC(7f7135da) SHA1(0f77841e52b3d7e731d5142fba9ed5cd57343305) )
	ROM_CONTINUE(             0x00000, 0x08000 )

	ROM_REGION( 0x80000, REGION_GFX2, ROMREGION_DISPOSE )	/* sprites */
	ROM_LOAD16_WORD_SWAP( "polluxm2.bin", 0x00000, 0x80000, CRC(bdea6f7d) SHA1(b418710a6d12aa53037acf7bbec85a26dfac9ebe) )

	ROM_REGION( 0x80000, REGION_GFX3, ROMREGION_DISPOSE )	/* tiles */
	ROM_LOAD16_WORD_SWAP( "polluxm1.bin", 0x00000, 0x80000, CRC(1d2dedd2) SHA1(9bcb1c80f05eabbca2c0738e409d3cadfc14b0c8) )

	ROM_REGION( 0x80000, REGION_GFX4, ROMREGION_DISPOSE )	/* tiles */
	ROM_LOAD16_BYTE( "pollux6.bin",  0x00000, 0x20000, CRC(b0391db5) SHA1(0c522c5074dc7c0a639ebfb7b9a9eddc90314081) )
	ROM_LOAD16_BYTE( "pollux7.bin",  0x00001, 0x20000, CRC(632f6e10) SHA1(a3605cbe7a9dc04cd8c1ab50110f72d93c78208b) )
	ROM_FILL(                        0x40000, 0x40000, 0xff )

	ROM_REGION( 0x20000, REGION_GFX5, 0 )	/* bg tilemaps */
	ROM_LOAD16_BYTE( "pollux9.bin",  0x00000, 0x10000, CRC(378d8914) SHA1(ef95903971673bc26774fe2aff17e1581a7f0eb9) )
	ROM_LOAD16_BYTE( "pollux8.bin",  0x00001, 0x10000, CRC(8859fa70) SHA1(7b1b9edde3f762c7ae1f0b847aa17e30140e9ffa) )

	ROM_REGION( 0x20000, REGION_GFX6, 0 )	/* fg tilemaps */
	ROM_LOAD16_BYTE( "pollux5.bin",  0x00000, 0x10000, CRC(ac090d34) SHA1(6b554450d8d46165e25fd6f12ab4c4b9b63dcd35) )
	ROM_LOAD16_BYTE( "pollux4.bin",  0x00001, 0x10000, CRC(2c6bd3be) SHA1(6648264be83588a01f264e7ec72d84e29e0d4795) )
ROM_END

ROM_START( polluxa )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD( "dooyong2.bin",  0x00000, 0x10000, CRC(e4ea8dbd) SHA1(19652261981672fae896e3065f1f5078f7ae93b6) )
	ROM_RELOAD(               0x10000, 0x10000 )	/* banked at 0x8000-0xbfff */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* sound */
	ROM_LOAD( "pollux3.bin",  0x00000, 0x10000, CRC(85a9dc98) SHA1(a349bfb05d870ba920469066ce5c007363aca348) )

	ROM_REGION( 0x10000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "dooyong1.bin",  0x08000, 0x08000, CRC(a7d820b2) SHA1(bbcc3690f91a4bd4f0cff5da25cbfeceb7a19437) )
	ROM_CONTINUE(             0x00000, 0x08000 )

	ROM_REGION( 0x80000, REGION_GFX2, ROMREGION_DISPOSE )	/* sprites */
	ROM_LOAD16_WORD_SWAP( "polluxm2.bin", 0x00000, 0x80000, CRC(bdea6f7d) SHA1(b418710a6d12aa53037acf7bbec85a26dfac9ebe) )

	ROM_REGION( 0x80000, REGION_GFX3, ROMREGION_DISPOSE )	/* tiles */
	ROM_LOAD16_WORD_SWAP( "polluxm1.bin", 0x00000, 0x80000, CRC(1d2dedd2) SHA1(9bcb1c80f05eabbca2c0738e409d3cadfc14b0c8) )

	ROM_REGION( 0x80000, REGION_GFX4, ROMREGION_DISPOSE )	/* tiles */
	ROM_LOAD16_BYTE( "pollux6.bin",  0x00000, 0x20000, CRC(b0391db5) SHA1(0c522c5074dc7c0a639ebfb7b9a9eddc90314081) )
	ROM_LOAD16_BYTE( "pollux7.bin",  0x00001, 0x20000, CRC(632f6e10) SHA1(a3605cbe7a9dc04cd8c1ab50110f72d93c78208b) )
	ROM_FILL(                        0x40000, 0x40000, 0xff )

	ROM_REGION( 0x20000, REGION_GFX5, 0 )	/* bg tilemaps */
	ROM_LOAD16_BYTE( "pollux9.bin",  0x00000, 0x10000, CRC(378d8914) SHA1(ef95903971673bc26774fe2aff17e1581a7f0eb9) )
	ROM_LOAD16_BYTE( "pollux8.bin",  0x00001, 0x10000, CRC(8859fa70) SHA1(7b1b9edde3f762c7ae1f0b847aa17e30140e9ffa) )

	ROM_REGION( 0x20000, REGION_GFX6, 0 )	/* fg tilemaps */
	ROM_LOAD16_BYTE( "pollux5.bin",  0x00000, 0x10000, CRC(ac090d34) SHA1(6b554450d8d46165e25fd6f12ab4c4b9b63dcd35) )
	ROM_LOAD16_BYTE( "pollux4.bin",  0x00001, 0x10000, CRC(2c6bd3be) SHA1(6648264be83588a01f264e7ec72d84e29e0d4795) )
ROM_END

ROM_START( bluehawk )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD( "rom19",        0x00000, 0x20000, CRC(24149246) SHA1(458fd429a895353b8636c717dcd58d57b8723012) )
	ROM_RELOAD(               0x10000, 0x20000 )	/* banked at 0x8000-0xbfff */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* sound */
	ROM_LOAD( "rom1",         0x00000, 0x10000, CRC(eef22920) SHA1(a3295ae7524df8c4d00ac3da422bbf66c959bf4f) )

	ROM_REGION( 0x10000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "rom3",         0x00000, 0x10000, CRC(c192683f) SHA1(060372b21bf331671c135a074640868eeb5f13ec) )

	ROM_REGION( 0x80000, REGION_GFX2, ROMREGION_DISPOSE )	/* sprites */
	ROM_LOAD16_WORD_SWAP( "dy-bh-m3",     0x00000, 0x80000, CRC(8809d157) SHA1(7f86378f9fcb95ab83b68f37a29732bb8cb3d95a) )

	ROM_REGION( 0x80000, REGION_GFX3, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_WORD_SWAP( "dy-bh-m1",     0x00000, 0x80000, CRC(51816b2c) SHA1(72fb055de7979e40195316ef38a2e8c54be12e2b) )

	ROM_REGION( 0x80000, REGION_GFX4, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_WORD_SWAP( "dy-bh-m2",     0x00000, 0x80000, CRC(f9daace6) SHA1(5e7892bad170ab9bd52426629ad49843fbc31996) )

	ROM_REGION( 0x40000, REGION_GFX5, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_BYTE( "rom6",         0x00000, 0x20000, CRC(e6bd9daa) SHA1(3b478fd02b145d13e49539df5260191a5254be19) )
	ROM_LOAD16_BYTE( "rom5",         0x00001, 0x20000, CRC(5c654dc6) SHA1(f10f64d7114adf7f18ec37c193c524ec80236201) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* OKI6295 samples */
	ROM_LOAD( "rom4",         0x00000, 0x20000, CRC(f7318919) SHA1(8b7e2ffe77603142cf1b9440585f8dfa9199ed05) )
ROM_END

ROM_START( bluehawn )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD( "rom19",        0x00000, 0x20000, CRC(24149246) SHA1(458fd429a895353b8636c717dcd58d57b8723012) )	// ROM2
	ROM_RELOAD(               0x10000, 0x20000 )	/* banked at 0x8000-0xbfff */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* sound */
	ROM_LOAD( "rom1",         0x00000, 0x10000, CRC(eef22920) SHA1(a3295ae7524df8c4d00ac3da422bbf66c959bf4f) )

	ROM_REGION( 0x10000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "rom3ntc",      0x00000, 0x10000, CRC(31eb221a) SHA1(7b893972227047d2f609fd1f97cc006eba2c9579) )

	ROM_REGION( 0x80000, REGION_GFX2, ROMREGION_DISPOSE )	/* sprites */
	ROM_LOAD16_WORD_SWAP( "dy-bh-m3",     0x00000, 0x80000, CRC(8809d157) SHA1(7f86378f9fcb95ab83b68f37a29732bb8cb3d95a) )	// ROM7+ROM8+ROM13+ROM14

	ROM_REGION( 0x80000, REGION_GFX3, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_WORD_SWAP( "dy-bh-m1",     0x00000, 0x80000, CRC(51816b2c) SHA1(72fb055de7979e40195316ef38a2e8c54be12e2b) )	// ROM9+ROM10+ROM15+ROM16

	ROM_REGION( 0x80000, REGION_GFX4, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_WORD_SWAP( "dy-bh-m2",     0x00000, 0x80000, CRC(f9daace6) SHA1(5e7892bad170ab9bd52426629ad49843fbc31996) )	// ROM11+ROM12+ROM17+ROM18

	ROM_REGION( 0x40000, REGION_GFX5, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_BYTE( "rom6",         0x00000, 0x20000, CRC(e6bd9daa) SHA1(3b478fd02b145d13e49539df5260191a5254be19) )
	ROM_LOAD16_BYTE( "rom5",         0x00001, 0x20000, CRC(5c654dc6) SHA1(f10f64d7114adf7f18ec37c193c524ec80236201) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* OKI6295 samples */
	ROM_LOAD( "rom4",         0x00000, 0x20000, CRC(f7318919) SHA1(8b7e2ffe77603142cf1b9440585f8dfa9199ed05) )
ROM_END

ROM_START( sadari )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD( "1.3d",         0x00000, 0x20000, CRC(bd953217) SHA1(6e230103ea01744761ab8a194d0dde6921bee92e) )
	ROM_RELOAD(               0x10000, 0x20000 )				/* banked at 0x8000-0xbfff */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* sound */
	ROM_LOAD( "3.6r",         0x0000, 0x10000, CRC(4786fca6) SHA1(b2347e2f6bbe3dd9d1cc8d8a4af40e7997d5ab74) )

	ROM_REGION( 0x20000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "2.4c",         0x0000, 0x20000, CRC(b2a3f1c6) SHA1(06f0038dc113c8001786157b9c9ee0eda76c2411) )

	/* no sprites */

	ROM_REGION( 0x80000, REGION_GFX2, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_BYTE( "10.10l",       0x00000, 0x20000, CRC(70269ab1) SHA1(055ff484da028f11bb3097652ef4713603870f89) )
	ROM_LOAD16_BYTE( "5.8l",         0x00001, 0x20000, CRC(ceceb4c3) SHA1(db08bbe9d23eb50d5c0603893a6e0368e2b6bbba) )
	ROM_LOAD16_BYTE( "9.10n",        0x40000, 0x20000, CRC(21bd1bda) SHA1(a5c9df8b45b05130374a83e45b3fb7cce76b58f8) )
	ROM_LOAD16_BYTE( "4.8n",         0x40001, 0x20000, CRC(cd318ae5) SHA1(457ccaf1d841ff763878dca8e534b9738510899a) )

	ROM_REGION( 0x80000, REGION_GFX3, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_BYTE( "11.10j",       0x00000, 0x20000, CRC(62a1d580) SHA1(4df60db9ad306a4d8776d10826e802cab27809f7) )
	ROM_LOAD16_BYTE( "6.8j",         0x00001, 0x20000, CRC(c4b13ed7) SHA1(97a33d700a8372b0e4bb13e567afc5ef898e9351) )
	ROM_LOAD16_BYTE( "12.10g",       0x40000, 0x20000, CRC(547b7645) SHA1(fdfe5cccdae1b88736aae702aa55fd642396ce01) )
	ROM_LOAD16_BYTE( "7.8g",         0x40001, 0x20000, CRC(14f20fa3) SHA1(95aabb5a5de976fb62b5cffd3efb2a86b5d62c20) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* OKI6295 samples */
	ROM_LOAD( "8.10r",        0x00000, 0x20000, CRC(9c29a093) SHA1(b6252e0cb8e618cdc4a741ee7ab01058f929fd11) )
ROM_END

ROM_START( gundl94 )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD( "gd94_001.d3",  0x00000, 0x20000, CRC(3a5cc045) SHA1(182743458c36bb6254a39cf9a371fd2b0d72d145) )
	ROM_RELOAD(               0x10000, 0x20000 )				/* banked at 0x8000-0xbfff */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* sound */
	ROM_LOAD( "gd94_003.r6",  0x0000, 0x10000, CRC(ea41c4ad) SHA1(e39e0507f4f370432ef0ca11dbecef176716cec4) )

	ROM_REGION( 0x20000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "gd94_002.c5",  0x0000, 0x20000, CRC(8575e64b) SHA1(08ef8af655a354c30ee3fe587554e418903147f5) )

	/* no sprites */

	ROM_REGION( 0x40000, REGION_GFX2, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_BYTE( "gd94_009.n9",  0x00000, 0x20000, CRC(40eabf55) SHA1(660f4318248001049369e1e715c7ff09d551c256) )
	ROM_LOAD16_BYTE( "gd94_004.n7",  0x00001, 0x20000, CRC(0654abb9) SHA1(c0fcd8ba78db341f46acb523c670d053e3d82b16) )

	ROM_REGION( 0x40000, REGION_GFX3, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_BYTE( "gd94_012.g9",  0x00000, 0x20000, CRC(117c693c) SHA1(e08bd6fbbae8ac657e6a1f9df36983ace941da3a) )
	ROM_LOAD16_BYTE( "gd94_007.g7",  0x00001, 0x20000, CRC(96a72c6d) SHA1(b79a746fc114eb8977591f147e4ea4a4e4f14526) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* OKI6295 samples */
	ROM_LOAD( "gd94_008.r9",  0x00000, 0x20000, CRC(f92e5803) SHA1(69dd11469e9e6bdc7825a5a14994276b50c10a14) )

	ROM_REGION( 0x30000, REGION_CPU3, 0 )	/* extra z80 rom? this doesn't seem to belong to this game! */
	ROM_LOAD( "gd94_011.j9",  0x00000, 0x20000, CRC(d8ad0208) SHA1(5df0f94ef86d7a03bde546e7aafdc0caf8a17076) )
	ROM_RELOAD(               0x10000, 0x20000 )				/* banked at 0x8000-0xbfff */

	ROM_REGION( 0x40000, REGION_GFX4, ROMREGION_DISPOSE )	/* more tiles? they don't seem to belong to this game! */
	ROM_LOAD16_BYTE( "gd94_006.j7",  0x00000, 0x20000, CRC(1d9536fe) SHA1(d72e66a529456c87217f9ba88f7f45aa2aa3e399) )
	ROM_LOAD16_BYTE( "gd94_010.l7",  0x00001, 0x20000, CRC(4b74857f) SHA1(a4413369fdb165c0f12454592181675095f28145) )
ROM_END

ROM_START( primella )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD( "1_d3.bin",     0x00000, 0x20000, CRC(82fea4e0) SHA1(3603c0edda29868d5e282465880e1ad341365f6f) )
	ROM_RELOAD(               0x10000, 0x20000 )				/* banked at 0x8000-0xbfff */

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* sound */
	ROM_LOAD( "gd94_003.r6",  0x0000, 0x10000, CRC(ea41c4ad) SHA1(e39e0507f4f370432ef0ca11dbecef176716cec4) )

	ROM_REGION( 0x20000, REGION_GFX1, ROMREGION_DISPOSE )	/* chars */
	ROM_LOAD( "gd94_002.c5",  0x0000, 0x20000, CRC(8575e64b) SHA1(08ef8af655a354c30ee3fe587554e418903147f5) )

	/* no sprites */

	ROM_REGION( 0x40000, REGION_GFX2, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_BYTE( "7_n9.bin",     0x00000, 0x20000, CRC(20b6a574) SHA1(e180e8440bf2dc22c7d24707fc47d0c70433ecba) )
	ROM_LOAD16_BYTE( "4_n7.bin",     0x00001, 0x20000, CRC(fe593666) SHA1(f511e4881f79de91c501b0026de2ac5b4a59f747) )

	ROM_REGION( 0x40000, REGION_GFX3, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_BYTE( "8_g9.bin",     0x00000, 0x20000, CRC(542ecb83) SHA1(0a4decaad9dde4681f7b6cdab0ae0e4951efc83d) )
	ROM_LOAD16_BYTE( "5_g7.bin",     0x00001, 0x20000, CRC(058ecac6) SHA1(12f70f78b882b6ce08c56f6fa9a1211c3464bf9d) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 )	/* OKI6295 samples */
	ROM_LOAD( "gd94_008.r9",  0x00000, 0x20000, CRC(f92e5803) SHA1(69dd11469e9e6bdc7825a5a14994276b50c10a14) )	/* 6_r9 */
ROM_END

/*

Super X
NTC, 1994

This game runs on Dooyong hardware.

PCB Layout
----------

|-------------------------------------------------|
|     YM3012  Z80            4.7V   62256  62256  |
|     YM2151  1.5U  M6295    5.7U   62256  62256  |
|             6116                                |
|      PAL         -----------      62256  62256  |
|                  |DY208    |      62256  62256  |
|J   DSW1          |DY-OBJ-01|                    |
|A                 -----------                    |
|M       2.3M  62256                 SPXO-M05.10M |
|M       3.3L  62256    6116                      |
|A                      6116                      |
|       68000  --------- SPXB-M03.8J SPXB-MS4.10J |
|   DSW2       |DY160  |                          |
|              |DY-PL-1| SPXB-M04.8F SPXB-MS3.10F |
|              ---------                          |
|    PAL  PAL                                     |
|              --------- SPXB-M01.8C SPXB-MS1.10C |
|     6116     |DY160  |                          |
|8MHz 6116     |DY-PL-1| SPXB-M02.8A SPXB-MS2.10A |
|-------------------------------------------------|

Notes:
      68000 clock: 8.000MHz
        Z80 clock: 4.000MHz
     YM2151 clock: 4.000MHz
      M6295 clock: 1.000MHz, sample rate = /132
            VSync: 60Hz
            HSync: 15.68kHz

ROMs:
     Filename     Type              Use
     --------------------------------------------
           1.5U   27C512            Sound program

           2.3M   27C1000         \ Main Program
           3.3L   27C1000         /

           4.7V   27C1000         \ M6295 samples
           5.5U   27C1000         /

     SPXO-M05.10M 16M MASK 42 pin \
     SPXB-M01.8C   8M MASK 42 pin |
     SPXB-M02.8A         "        | Gfx + Tilemaps
     SPXB-M03.8J         "        |
     SPXB-M04.8F         "        /
     SPXB-MS1.10C  1M MASK 28 pin \
     SPXB-MS2.10A        "        | Gfx (All have fixed bits, this is correct, they contain the upper 4 bits)
     SPXB-MS3.10F        "        |     (of the tilemap data)
     SPXB-MS4.10J        "        /

*/

ROM_START( superx )
	ROM_REGION( 0x40000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD16_BYTE( "2.3m",   0x00000, 0x20000, CRC(be7aebe7) SHA1(81934d861a15a96cf23721ad38f821e1f94ec980) )
	ROM_LOAD16_BYTE( "3.3l",   0x00001, 0x20000, CRC(dc4a25fc) SHA1(660bf33a9ae7534c37353f9690af180268ce7f30) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* sound */
	ROM_LOAD( "1.5u",     0x0000, 0x10000, CRC(6894ce05) SHA1(9726fc3f1e9bebecf498c208ab03007f34936632) )

	ROM_REGION( 0x200000, REGION_GFX1, ROMREGION_DISPOSE )	/* sprite */
	ROM_LOAD16_WORD_SWAP( "spxo-m05.10m",    0x00000, 0x200000, CRC(9120dd84) SHA1(bcf1fdc860d51b9bcfec1e84940ef21dfc41b5dc) )

	ROM_REGION( 0x100000, REGION_GFX2, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_WORD_SWAP( "spxb-m04.8f",    0x00000, 0x100000, CRC(91a7ac6e) SHA1(b7fb79c2e4f5eecb7128b86ee2b1070eed905d2a) ) // bomb

	ROM_REGION( 0x100000, REGION_GFX3, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_WORD_SWAP( "spxb-m03.8j",    0x00000, 0x100000, CRC(8b42861b) SHA1(6eb1f6bfe0b8e987e624a6fe7e025c6918804cf9) ) // title logo

	ROM_REGION( 0x100000, REGION_GFX4, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_WORD_SWAP( "spxb-m02.8a",    0x00000, 0x100000, CRC(21b8db78) SHA1(e7c51c9566ebce5b5db5af48f33e2194b518715f)) // title screen upper background

	ROM_REGION( 0x100000, REGION_GFX5, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_WORD_SWAP( "spxb-m01.8c",    0x00000, 0x100000, CRC(60c69129) SHA1(6871b08e354c7cf5fb16b0ed4562c537e2ce9194) ) // title screen lower background

	ROM_REGION( 0x80000, REGION_GFX6, 0 )	/* top 4 bits of tilemaps */
	ROM_LOAD( "spxb-ms3.10f",    0x00000, 0x20000, CRC(8bf8c77d) SHA1(a89e50bd571e754cb56a17fe4ada6a804e74520b)) // bomb
	ROM_LOAD( "spxb-ms4.10j",    0x20000, 0x20000, CRC(d418a900) SHA1(0d69afa48d3072c7fecfc5d6dd63717b9f61c0fc) ) // title logo
	ROM_LOAD( "spxb-ms2.10a",    0x40000, 0x20000, CRC(5ec87adf) SHA1(cdd0864ea23b2c6d8ace519fc66e77f59813e206) ) // title screen upper background
	ROM_LOAD( "spxb-ms1.10c",    0x60000, 0x20000, CRC(40b4fe6c) SHA1(5ab63ce83522c32039ee33c59e713d2fb37aac44) ) // title screen lower background

	ROM_REGION( 0x40000, REGION_SOUND1, 0 )	/* OKI6295 samples */
	ROM_LOAD( "4.7v",     0x00000, 0x20000, CRC(434290b5) SHA1(3f2fb5aed1f109add17f00fe3a2364eedc7172ae) )
	ROM_LOAD( "5.7u",     0x20000, 0x20000, CRC(ebe6abb4) SHA1(801b22845603f86c7bab77baa6946afc613aebdb) )
ROM_END

/* this set only had 68k roms, sound program, and samples */
ROM_START( superxm )
	ROM_REGION( 0x40000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD16_BYTE( "2_m.3m",   0x00000, 0x20000, CRC(41c50aac) SHA1(75f6470bde217e4b9139d8af97a17ca22c374944) )
	ROM_LOAD16_BYTE( "3_m.3l",   0x00001, 0x20000, CRC(6738b703) SHA1(e37f5f76b1efbd2f5098014ca380d4340204e487) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* sound */
	ROM_LOAD( "1_m.5u",     0x0000, 0x10000,  CRC(319fa632) SHA1(b621ad080e8cf6611fc88d8fc2af5aa4e31e9e01) )

	ROM_REGION( 0x200000, REGION_GFX1, ROMREGION_DISPOSE )	/* sprite */
	ROM_LOAD16_WORD_SWAP( "spxo-m05.10m",    0x00000, 0x200000, CRC(9120dd84) SHA1(bcf1fdc860d51b9bcfec1e84940ef21dfc41b5dc) )

	ROM_REGION( 0x100000, REGION_GFX2, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_WORD_SWAP( "spxb-m04.8f",    0x00000, 0x100000, CRC(91a7ac6e) SHA1(b7fb79c2e4f5eecb7128b86ee2b1070eed905d2a) ) // bomb

	ROM_REGION( 0x100000, REGION_GFX3, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_WORD_SWAP( "spxb-m03.8j",    0x00000, 0x100000, CRC(8b42861b) SHA1(6eb1f6bfe0b8e987e624a6fe7e025c6918804cf9) ) // title logo

	ROM_REGION( 0x100000, REGION_GFX4, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_WORD_SWAP( "spxb-m02.8a",    0x00000, 0x100000, CRC(21b8db78) SHA1(e7c51c9566ebce5b5db5af48f33e2194b518715f)) // title screen upper background

	ROM_REGION( 0x100000, REGION_GFX5, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_WORD_SWAP( "spxb-m01.8c",    0x00000, 0x100000, CRC(60c69129) SHA1(6871b08e354c7cf5fb16b0ed4562c537e2ce9194) ) // title screen lower background

	ROM_REGION( 0x80000, REGION_GFX6, 0 )	/* top 4 bits of tilemaps */
	ROM_LOAD( "spxb-ms3.10f",    0x00000, 0x20000, CRC(8bf8c77d) SHA1(a89e50bd571e754cb56a17fe4ada6a804e74520b)) // bomb
	ROM_LOAD( "spxb-ms4.10j",    0x20000, 0x20000, CRC(d418a900) SHA1(0d69afa48d3072c7fecfc5d6dd63717b9f61c0fc) ) // title logo
	ROM_LOAD( "spxb-ms2.10a",    0x40000, 0x20000, CRC(5ec87adf) SHA1(cdd0864ea23b2c6d8ace519fc66e77f59813e206) ) // title screen upper background
	ROM_LOAD( "spxb-ms1.10c",    0x60000, 0x20000, CRC(40b4fe6c) SHA1(5ab63ce83522c32039ee33c59e713d2fb37aac44) ) // title screen lower background

	ROM_REGION( 0x40000, REGION_SOUND1, 0 )	/* OKI6295 samples */
	ROM_LOAD( "4.7v",     0x00000, 0x20000, CRC(434290b5) SHA1(3f2fb5aed1f109add17f00fe3a2364eedc7172ae) )
	ROM_LOAD( "5.7u",     0x20000, 0x20000, CRC(ebe6abb4) SHA1(801b22845603f86c7bab77baa6946afc613aebdb) )
ROM_END

ROM_START( rshark )
	ROM_REGION( 0x40000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD16_BYTE( "rspl00.bin",   0x00000, 0x20000, CRC(40356b9d) SHA1(28749a0d4c1ac8e094c551594033d47061071d8b) )
	ROM_LOAD16_BYTE( "rspu00.bin",   0x00001, 0x20000, CRC(6635c668) SHA1(242d9c5828e142d5820c75c4e4696fcc5f5ffbb7) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 )	/* sound */
	ROM_LOAD( "rse3.bin",     0x0000, 0x10000, CRC(03c8fd17) SHA1(d59a3d8b731484572384a9d6f24ef4cd200ef661) )

	ROM_REGION( 0x200000, REGION_GFX1, ROMREGION_DISPOSE )	/* sprite */
	ROM_LOAD16_BYTE( "rse4.bin",     0x000000, 0x80000, CRC(b857e411) SHA1(14a8883243f3f1ee661395cbcce7d5d3c08caef8) )
	ROM_LOAD16_BYTE( "rse5.bin",     0x000001, 0x80000, CRC(7822d77a) SHA1(25d34b508a25ab8052d3f73eeb60c7b9e6610db6) )
	ROM_LOAD16_BYTE( "rse6.bin",     0x100000, 0x80000, CRC(80215c52) SHA1(6138804fc2f81cf1366cc1bcca7572e45845ca8a) )
	ROM_LOAD16_BYTE( "rse7.bin",     0x100001, 0x80000, CRC(bd28bbdc) SHA1(b09ce8b21a08d129703f95b6fe9361e7f6614ee3) )

	ROM_REGION( 0x100000, REGION_GFX2, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_BYTE( "rse11.bin",    0x00000, 0x80000, CRC(8a0c572f) SHA1(218c4e4aeacedf459c6c08cc47dd2154b7dd4279) )
	ROM_LOAD16_BYTE( "rse10.bin",    0x00001, 0x80000, CRC(139d5947) SHA1(e371f27091924c605962f0a88d9f1f3deb0c954e) )

	ROM_REGION( 0x100000, REGION_GFX3, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_BYTE( "rse15.bin",    0x00000, 0x80000, CRC(d188134d) SHA1(b0711657ad87166330b471fa449e95d63939b223) )
	ROM_LOAD16_BYTE( "rse14.bin",    0x00001, 0x80000, CRC(0ef637a7) SHA1(827867831f751a5ed4022932b755e128fb5886b6) )

	ROM_REGION( 0x100000, REGION_GFX4, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_BYTE( "rse17.bin",    0x00000, 0x80000, CRC(7ff0f3c7) SHA1(033722dbf69745676b71f7002b413abd3c7bdf3c) )
	ROM_LOAD16_BYTE( "rse16.bin",    0x00001, 0x80000, CRC(c176c8bc) SHA1(98ef043befd2e067012d24299196964a0957b2ea) )

	ROM_REGION( 0x100000, REGION_GFX5, 0 )	/* tiles + tilemaps (together!) */
	ROM_LOAD16_BYTE( "rse21.bin",    0x00000, 0x80000, CRC(2ea665af) SHA1(67445e525016c0873bc2d831230f908388dabd4d) )
	ROM_LOAD16_BYTE( "rse20.bin",    0x00001, 0x80000, CRC(ef93e3ac) SHA1(397afe70c8039eb073589353bd5a9f469e8a6776) )

	ROM_REGION( 0x80000, REGION_GFX6, 0 )	/* top 4 bits of tilemaps */
	ROM_LOAD( "rse12.bin",    0x00000, 0x20000, CRC(fadbf947) SHA1(0d752c2499adca883f281aed95356a7fbf78fe5f) )
	ROM_LOAD( "rse13.bin",    0x20000, 0x20000, CRC(323d4df6) SHA1(9ea0b84f7f565c7ca33335d286e8d4f812b216f2) )
	ROM_LOAD( "rse18.bin",    0x40000, 0x20000, CRC(e00c9171) SHA1(10365ddbf4d60e99758ff0bb5042648c5f0f9c34) )
	ROM_LOAD( "rse19.bin",    0x60000, 0x20000, CRC(d214d1d0) SHA1(98daf875fec0372c719efcfb4457db573261e9f4) )

	ROM_REGION( 0x40000, REGION_SOUND1, 0 )	/* OKI6295 samples */
	ROM_LOAD( "rse1.bin",     0x00000, 0x20000, CRC(0291166f) SHA1(7c4c80cfd921a07b8195306cfbd2f84947aa7d6f) )
	ROM_LOAD( "rse2.bin",     0x20000, 0x20000, CRC(5a26ee72) SHA1(3ceed1f50510993354dd4def577af5cf4c4a4f7a) )
ROM_END



/* The differences between the two lastday sets are only in the sound program
   and graphics. The main program is the same. */
GAMEX(1990, lastday,  0,        lastday,  lastday,  0, ROT270, "Dooyong", "The Last Day (set 1)", GAME_IMPERFECT_GRAPHICS )
GAMEX(1990, lastdaya, lastday,  lastday,  lastday,  0, ROT270, "Dooyong", "The Last Day (set 2)", GAME_IMPERFECT_GRAPHICS )
GAMEX(1991, gulfstrm, 0,        gulfstrm, gulfstrm, 0, ROT270, "Dooyong", "Gulf Storm", GAME_IMPERFECT_GRAPHICS )
GAMEX(1991, gulfstr2, gulfstrm, gulfstrm, gulfstrm, 0, ROT270, "Dooyong (Media Shoji license)", "Gulf Storm (Media Shoji)", GAME_IMPERFECT_GRAPHICS )
GAME( 1991, pollux,   0,        pollux,   pollux,   0, ROT270, "Dooyong", "Pollux (set 1)" )
GAME( 1991, polluxa,  pollux,   pollux,   pollux,   0, ROT270, "Dooyong", "Pollux (set 2)" )
GAMEX(1993, bluehawk, 0,        bluehawk, bluehawk, 0, ROT270, "Dooyong", "Blue Hawk", GAME_IMPERFECT_GRAPHICS )
GAMEX(1993, bluehawn, bluehawk, bluehawk, bluehawk, 0, ROT270, "[Dooyong] (NTC license)", "Blue Hawk (NTC)", GAME_IMPERFECT_GRAPHICS )
GAME( 1993, sadari,   0,        primella, primella, 0, ROT0,   "[Dooyong] (NTC license)", "Sadari" )
GAME( 1994, gundl94,  0,        primella, primella, 0, ROT0,   "Dooyong", "Gun Dealer '94" )
GAME( 1994, primella, gundl94,  primella, primella, 0, ROT0,   "[Dooyong] (NTC license)", "Primella" )
GAMEX(1994, superx,   0,        superx,   rshark,   0, ROT270, "NTC", "Super-X (NTC)", GAME_IMPERFECT_GRAPHICS )
GAMEX(1994, superxm,  superx,   superx,   rshark,   0, ROT270, "Mitchell", "Super-X (Mitchell)", GAME_IMPERFECT_GRAPHICS )
GAMEX(1995, rshark,   0,        rshark,   rshark,   0, ROT270, "Dooyong", "R-Shark", GAME_IMPERFECT_GRAPHICS )
