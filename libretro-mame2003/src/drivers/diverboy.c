/* Diver Boy
 (c)1992 Device Electronics

 TODO:

 Sound doesn't seem entirely correct, but it might just be this bad in the original

 ----

 Here's the info about this dump:

 Name:            DiverBoy
 Manufacturer:    Unknow
 Year:            Unknow
 Date Dumped:     17-07-2002 (DD-MM-YYYY)

 CPU:             68000, Z80
 SOUND:           OKIM6295
 GFX:             Unknown

 About the game:

 The worst game i have :) Enjoy it so much as me :D

 ----

 Stephh's notes :

  - COIN3 gives ("Coinage" * 2) coins/credits :

     COIN1/2    COIN3
      4C_1C     2C_1C
      3C_1C    special   (see below)
      2C_1C     1C_1C
      1C_1C     1C_2C
      1C_2C     1C_4C
      1C_3C     1C_6C
      1C_4C     1C_8C
      1C_6C     1C_12C

    when "Coinage" set to 3C_1C, pressing COIN3 has this effect :

      * 1st coin : nothing
      * 2nd coin : adds 1 credit
      * 3rd coin : adds 1 credit
      * 4th coin : nothing
      * 5th coin : adds 1 credit
      * 6th coin : adds 1 credit ...

*/

#include "driver.h"

extern data16_t *diverboy_spriteram;
extern size_t diverboy_spriteram_size;

VIDEO_START(diverboy);
VIDEO_UPDATE(diverboy);


static WRITE16_HANDLER( soundcmd_w )
{
	if (ACCESSING_LSB)
	{
		soundlatch_w(0,data & 0xff);
		cpu_set_irq_line(1,0,HOLD_LINE);
	}
}

static WRITE_HANDLER( okibank_w )
{
	/* bit 2 might be reset */
//	usrintf_showmessage("%02x",data);

	OKIM6295_set_bank_base(0,(data & 3) * 0x20000);
}



static MEMORY_READ16_START( diverboy_readmem )
	{ 0x000000, 0x03ffff, MRA16_ROM },
	{ 0x040000, 0x04ffff, MRA16_RAM },
	{ 0x080000, 0x083fff, MRA16_RAM },
	{ 0x180000, 0x180001, input_port_0_word_r },
	{ 0x180002, 0x180003, input_port_1_word_r },
	{ 0x180008, 0x180009, input_port_2_word_r },
//	{ 0x18000a, 0x18000b, MRA16_NOP },
MEMORY_END

static MEMORY_WRITE16_START( diverboy_writemem )
	{ 0x000000, 0x03ffff, MWA16_ROM },
	{ 0x040000, 0x04ffff, MWA16_RAM }, /* main ram */
	{ 0x080000, 0x083fff, MWA16_RAM, &diverboy_spriteram, &diverboy_spriteram_size },
	{ 0x100000, 0x100001, soundcmd_w },
	{ 0x140000, 0x1407ff, paletteram16_xxxxBBBBGGGGRRRR_word_w, &paletteram16 },
//	{ 0x18000c, 0x18000d, MWA16_NOP },
	{ 0x320000, 0x3207ff, MWA16_RAM }, /* ?? */
	{ 0x322000, 0x3227ff, MWA16_RAM }, /* ?? */
//	{ 0x340000, 0x340001, MWA16_NOP },
//	{ 0x340002, 0x340003, MWA16_NOP },
MEMORY_END

static MEMORY_READ_START( snd_readmem )
	{ 0x0000, 0x7fff, MRA_ROM },
	{ 0x8000, 0x87ff, MRA_RAM },
	{ 0xa000, 0xa000, soundlatch_r },
	{ 0x9800, 0x9800, OKIM6295_status_0_r },
MEMORY_END

static MEMORY_WRITE_START( snd_writemem )
	{ 0x0000, 0x7fff, MWA_ROM },
	{ 0x8000, 0x87ff, MWA_RAM },
	{ 0x9000, 0x9000, okibank_w },
	{ 0x9800, 0x9800, OKIM6295_data_0_w },
MEMORY_END



INPUT_PORTS_START( diverboy )
	PORT_START	// 0x180000.w
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_PLAYER1 )	// unused ?
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_PLAYER1 )	// unused ?
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_PLAYER1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_PLAYER1 )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )		// "Dive"
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )		// unknown effect
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_PLAYER2 )	// unused ?
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_PLAYER2 )	// unused ?
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_PLAYER2 )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_PLAYER2 )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )		// "Dive"
	PORT_BIT( 0x2000, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )		// unknown effect
	PORT_BIT( 0x4000, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START	// 0x180002.w
	PORT_DIPNAME( 0x07, 0x00, DEF_STR( Coinage ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPSETTING(    0x08, "3" )
	PORT_DIPNAME( 0x10, 0x10, "Display Copyright" )
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x60, 0x00, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x00, "Easy" )
	PORT_DIPSETTING(    0x20, "Normal" )
	PORT_DIPSETTING(    0x40, "Hard" )
	PORT_DIPSETTING(    0x60, "Hardest" )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(    0x80, DEF_STR( No ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Yes ) )

	PORT_START	// 0x180008.w
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN3 )	// read notes
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNKNOWN )	// must be 00 - check code at 0x001680
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END



static struct GfxLayout diverboy_spritelayout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0,1,2,3 },
	{  4, 0,  12, 8,  20, 16, 28, 24,
	  36, 32, 44, 40, 52, 48, 60, 56, },
	{ 0*64, 1*64, 2*64,  3*64,  4*64,  5*64,  6*64,  7*64,
	  8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
	16*64
};

static struct GfxDecodeInfo gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &diverboy_spritelayout, 0, 4*16 },
	{ REGION_GFX2, 0, &diverboy_spritelayout, 0, 4*16 },
	{ -1 } /* end of array */
};



static struct OKIM6295interface okim6295_interface =
{
	1,				/* 1 chip */
	{ 10000 },		/* ???? frequency (Hz) */
	{ REGION_SOUND1 },	/* memory region */
	{ 100 }
};



static MACHINE_DRIVER_START( diverboy )
	MDRV_CPU_ADD(M68000, 12000000) /* guess */
	MDRV_CPU_MEMORY(diverboy_readmem,diverboy_writemem)
	MDRV_CPU_VBLANK_INT(irq6_line_hold,1)

	MDRV_CPU_ADD(Z80, 4000000)
	MDRV_CPU_FLAGS(CPU_AUDIO_CPU)
	MDRV_CPU_MEMORY(snd_readmem,snd_writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	MDRV_GFXDECODE(gfxdecodeinfo)

	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(64*8, 32*8)
	MDRV_VISIBLE_AREA(2*8, 40*8-1, 2*8, 32*8-1)
	MDRV_PALETTE_LENGTH(0x400)

	MDRV_VIDEO_START(diverboy)
	MDRV_VIDEO_UPDATE(diverboy)

	MDRV_SOUND_ADD(OKIM6295, okim6295_interface)
MACHINE_DRIVER_END



ROM_START( diverboy )
	ROM_REGION( 0x40000, REGION_CPU1, 0 ) /* 68000 Code */
	ROM_LOAD16_BYTE( "db_01.bin", 0x00000, 0x20000, CRC(6aa11366) SHA1(714c8a4a64c18632825a734a76a2d1b031106d76) )
	ROM_LOAD16_BYTE( "db_02.bin", 0x00001, 0x20000, CRC(45f8a673) SHA1(4eea1374cafacb4a2e0b623fcb802deb5fca1b3a) )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* z80 */
	ROM_LOAD( "db_05.bin", 0x00000, 0x8000, CRC(ffeb49ec) SHA1(911b13897ff4ace3940bfff4ab88584a93796c24) ) /* this part is empty */
	ROM_CONTINUE( 0x0000, 0x8000 ) /* this part contains the code */

	ROM_REGION( 0x100000, REGION_GFX1, ROMREGION_DISPOSE ) /* GFX */
	ROM_LOAD16_BYTE( "db_08.bin", 0x000000, 0x80000, CRC(7bb96220) SHA1(671b3f218106e594b13ae5f2e680cf2e2cfc5501) )
	ROM_LOAD16_BYTE( "db_09.bin", 0x000001, 0x80000, CRC(12b15476) SHA1(400a5b846f70567de137e0b95586dd9cfc27becb) )

	ROM_REGION( 0x180000, REGION_GFX2, ROMREGION_DISPOSE ) /* GFX */
	ROM_LOAD16_BYTE( "db_07.bin", 0x000000, 0x20000, CRC(18485741) SHA1(a8edceaf34a98f2aa2bfada9d6e06fb82639a4e0) )
	ROM_LOAD16_BYTE( "db_10.bin", 0x000001, 0x20000, CRC(c381d1cc) SHA1(88b97d8893c500951cfe8e7e7f0b547b36bbe2c0) )
	ROM_LOAD16_BYTE( "db_06.bin", 0x040000, 0x20000, CRC(21b4e352) SHA1(a553de67e5dc751ea81ec4739724e0e46e8c5fab) )
	ROM_LOAD16_BYTE( "db_11.bin", 0x040001, 0x20000, CRC(41d29c81) SHA1(448fd5c1b16159d03436b8bd71ffe871c8daf7fa) )

	ROM_REGION( 0x80000, REGION_SOUND1, 0 ) /* Sound */
	ROM_LOAD( "db_03.bin", 0x00000, 0x80000, CRC(50457505) SHA1(faf1c055ec56d2ed7f5e6993cc04d3317bf1c3cc) )

	ROM_REGION( 0x20000, REGION_SOUND2, 0 ) /* Sound */
	ROM_LOAD( "db_04.bin", 0x00000, 0x20000, CRC(01b81da0) SHA1(914802f3206dc59a720af9d57eb2285bc8ba822b) ) /* same as tumble pop?, is this used? */
ROM_END



GAMEX(1992, diverboy, 0, diverboy, diverboy, 0, ORIENTATION_FLIP_X, "Electronic Devices Italy", "Diver Boy", GAME_IMPERFECT_SOUND )
