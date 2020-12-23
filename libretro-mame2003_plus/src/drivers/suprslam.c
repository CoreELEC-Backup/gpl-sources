/*** DRIVER INFORMATION & NOTES ***********************************************

Super Slams - Driver by David Haywood
   Sound Information from R.Belmont
   DSWs corrected by Stephh

TODO :

sprite offset control?
priorities (see hi-score table during attract mode)
unknown reads / writes (also KONAMI chip ..?)

WORKING NOTES :

68k interrupts
lev 1 : 0x64 : 0000 0406 - vblank?
lev 2 : 0x68 : 0000 06ae - x
lev 3 : 0x6c : 0000 06b4 - x
lev 4 : 0x70 : 0000 06ba - x
lev 5 : 0x74 : 0000 06c0 - x
lev 6 : 0x78 : 0000 06c6 - x
lev 7 : 0x7c : 0000 06cc - x

******************************************************************************/


/*** README INFORMATION *******************************************************

Super Slam
(C) 1995 Banpresto

(C) 1995 Banpresto / Toei Animation

PCB: VSEP-26
CPU: TMP68HC000P16 (68000, 64 pin DIP)
SND: Z8400B (Z80B, 40 pin DIP), YM2610, YM3016
OSC: 14.318180 MHz (Near VS920D & 053936), 24.000000 MHz, 32.000 MHz (Both near VS920F & EB26IC66)
Other Chips:
            Fujitsu CG10103 145 9520 Z14    (160 Pin PQFP)
            VS9210 4L06F1056 JAPAN 9525EAI  (176 Pin PQFP)
            VS920F 4L01F1435 JAPAN 9524EAI  (100 Pin PQFP)
            VS920E 4L06F1057 JAPAN 9533EAI  (176 pin PQFP)
            VS9209 4L01F1429 JAPAN 9523EAI  (64 pin PQFP)
            VS920D 4L04F1689 JAPAN 9524EAI  (160 pin PQFP)
            KONAMI KS10011-PF 053936 PSAC2 9522 Z02 (80 pin PQFP)

RAM:
            LGS GM76C28K-10 x 1 (Connected/Near Z80B)
            LGS GM76C28K-10 x 2 (Connected/Near 053936)
            SEC KM6264BLS-7 x 2 (Connected/Near VS920D)
            SEC KM6264BLS-7 x 2 (Connected/Near VS9210)
            UM61256FK-15 x 2 (Connected/Near VS9210)
            CY7C195-25PC x 1     -\
            UM61256FK-15 x 4       > (Connected/Near Fujitsu CG10103 145 9520 Z14)
            SEC KM6264BLS-7 x 4  -/
            LGS GM76C28K-10 x 2 (Connected/Near VS920F)
            LGS GM76C28K-10 x 2 (Connected/Near VS920E)
            UM61256FK-15 x 2 (Connected/Near 68000 & VS9209)

PALs: (4 total, not dumped, 2 located near 68000, 1 near Z80B, 1 near VS9210)

DIPs: 8 position x 3 (ALL DIPs linked to VS9209)

Info taken from sheet supplied with PCB, no info for SW3.

ROMs: (on ALL ROMs is written only "EB26")

EB26_100.BIN	16M Mask	\
EB26_101.BIN	16M Mask	|
EB26IC09.BIN	16M Mask	|  GFX (near VS9210, 053936 & VS920D)
EB26IC10.BIN	16M Mask	|
EB26IC12.BIN	16M Mask	/
EB26IC36.BIN	16M Mask
EB26IC43.BIN	16M Mask	   GFX (Near VS920F & VS920E)
EB26IC59.BIN	8M Mask		   Sound (Near YM2610)
EB26IC66.BIN	16M Mask	   Sound (Near YM2610)
EB26IC38.BIN	27C1001	           Sound Program (Near Z80B)
EB26IC47.BIN	27C240		\
EB26IC73.BIN	27C240		/  Main Program

******************************************************************************/

#include "driver.h"
#include "vidhrdw/konamiic.h"


extern data16_t *suprslam_screen_videoram, *suprslam_bg_videoram,*suprslam_sp_videoram, *suprslam_spriteram;

/* in vidhrdw */

WRITE16_HANDLER( suprslam_screen_videoram_w );
WRITE16_HANDLER( suprslam_bg_videoram_w );
VIDEO_START( suprslam );
VIDEO_UPDATE( suprslam );
WRITE16_HANDLER (suprslam_bank_w);


/*** SOUND *******************************************************************/

static int pending_command;

static WRITE16_HANDLER( sound_command_w )
{
	if (ACCESSING_LSB)
	{
		pending_command = 1;
		soundlatch_w(offset,data & 0xff);
		cpu_set_irq_line(1, IRQ_LINE_NMI, PULSE_LINE);
	}
}

#if 0
static READ16_HANDLER( pending_command_r )
{
	return pending_command;
}
#endif

static WRITE_HANDLER( pending_command_clear_w )
{
	pending_command = 0;
}

static WRITE_HANDLER( suprslam_sh_bankswitch_w )
{
	unsigned char *RAM = memory_region(REGION_CPU2);
	int bankaddress;

	bankaddress = 0x10000 + (data & 0x03) * 0x8000;
	cpu_setbank(1,&RAM[bankaddress]);
}

/*** MEMORY MAPS *************************************************************/

static MEMORY_READ16_START( suprslam_readmem )
	{ 0x000000, 0x0fffff, MRA16_ROM },
	{ 0xfb0000, 0xfb1fff, MRA16_RAM },
	{ 0xfc0000, 0xfcffff, MRA16_RAM },
	{ 0xfd0000, 0xfdffff, MRA16_RAM },
	{ 0xfe0000, 0xfe0fff, MRA16_RAM },
	{ 0xff0000, 0xff1fff, MRA16_RAM },
	{ 0xff8000, 0xff8fff, MRA16_RAM },
	{ 0xffa000, 0xffafff, MRA16_RAM },
	{ 0xfff000, 0xfff001, input_port_0_word_r },
	{ 0xfff002, 0xfff003, input_port_1_word_r },
	{ 0xfff004, 0xfff005, input_port_2_word_r },
	{ 0xfff006, 0xfff007, input_port_3_word_r },
	{ 0xfff008, 0xfff009, input_port_4_word_r },
MEMORY_END

static MEMORY_WRITE16_START( suprslam_writemem )
	{ 0x000000, 0x0fffff, MWA16_ROM },
	{ 0xfb0000, 0xfb1fff, MWA16_RAM, &suprslam_spriteram },
	{ 0xfc0000, 0xfcffff, MWA16_RAM, &suprslam_sp_videoram },
	{ 0xfd0000, 0xfdffff, MWA16_RAM },
	{ 0xfe0000, 0xfe0fff, suprslam_screen_videoram_w, &suprslam_screen_videoram },
	{ 0xff0000, 0xff1fff, suprslam_bg_videoram_w, &suprslam_bg_videoram },
/*	{ 0xff2000, 0xff203f, MWA16_RAM },  // ?? /*/
	{ 0xff8000, 0xff8fff, MWA16_RAM, &K053936_0_linectrl },
	{ 0xff9000, 0xff9001, sound_command_w },
	{ 0xffa000, 0xffafff, paletteram16_xGGGGGBBBBBRRRRR_word_w, &paletteram16 },
	{ 0xffd000, 0xffd01f, MWA16_RAM, &K053936_0_ctrl },
	{ 0xffe000, 0xffe001, suprslam_bank_w },
MEMORY_END

static MEMORY_READ_START( sound_readmem )
	{ 0x0000, 0x77ff, MRA_ROM },
	{ 0x7800, 0x7fff, MRA_RAM },
	{ 0x8000, 0xffff, MRA_BANK1 },
MEMORY_END

static MEMORY_WRITE_START( sound_writemem )
	{ 0x0000, 0x77ff, MWA_ROM },
	{ 0x7800, 0x7fff, MWA_RAM },
	{ 0x8000, 0xffff, MWA_ROM },
MEMORY_END

static PORT_READ_START( suprslam_sound_readport )
	{ 0x04, 0x04, soundlatch_r },
	{ 0x08, 0x08, YM2610_status_port_0_A_r },
	{ 0x0a, 0x0a, YM2610_status_port_0_B_r },
PORT_END

static PORT_WRITE_START( suprslam_sound_writeport )
	{ 0x00, 0x00, suprslam_sh_bankswitch_w },
	{ 0x04, 0x04, pending_command_clear_w },
	{ 0x08, 0x08, YM2610_control_port_0_A_w },
	{ 0x09, 0x09, YM2610_data_port_0_A_w },
	{ 0x0a, 0x0a, YM2610_control_port_0_B_w },
	{ 0x0b, 0x0b, YM2610_data_port_0_B_w },
PORT_END

/*** INPUT PORTS *************************************************************/

INPUT_PORTS_START( suprslam )

	PORT_START      /* IN1 */
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER1 )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 | IPF_PLAYER1 )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BUTTON4 | IPF_PLAYER1 )

	PORT_START      /* IN1 */
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON3 | IPF_PLAYER2 )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BUTTON4 | IPF_PLAYER2 )

	PORT_START		/* IN0 */
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_COIN3 )				/* Only in "test mode"*/
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_SERVICE2 )				/* "Test"*/
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_TILT )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_SERVICE1 )

	PORT_START	/* DSW */
	PORT_DIPNAME( 0x0001, 0x0001, "Coin Slots" )
	PORT_DIPSETTING(      0x0001, "Common" )
	PORT_DIPSETTING(      0x0000, "Separate" )
	PORT_DIPNAME( 0x000e, 0x000e, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(      0x000a, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x000c, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x000e, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0006, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x0070, 0x0070, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(      0x0050, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(      0x0060, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(      0x0070, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(      0x0030, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( 1C_4C ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( 1C_5C ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( 1C_6C ) )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )

	PORT_START	/* DSW */
	PORT_DIPNAME( 0x0003, 0x0003, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x0002, "Easy" )
	PORT_DIPSETTING(      0x0003, "Normal" )
	PORT_DIPSETTING(      0x0001, "Hard" )
	PORT_DIPSETTING(      0x0000, "Hardest" )
	PORT_DIPNAME( 0x000c, 0x000c, "Play Time" )
	PORT_DIPSETTING(      0x0008, "2:00" )
	PORT_DIPSETTING(      0x000c, "3:00" )
	PORT_DIPSETTING(      0x0004, "4:00" )
	PORT_DIPSETTING(      0x0000, "5:00" )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( On ) )
	PORT_SERVICE( 0x0040, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x0080, 0x0000, "Country" )
	PORT_DIPSETTING(      0x0080, "Japan" )
	PORT_DIPSETTING(      0x0000, "World" )
INPUT_PORTS_END

/*** GFX DECODE **************************************************************/

static struct GfxLayout suprslam_8x8x4_layout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0,1,2,3 },
	{ 4, 0, 12, 8, 20, 16, 28, 24 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	8*32
};

static struct GfxLayout suprslam_16x16x4_layout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0,1,2,3 },
	{ 8, 12, 0, 4, 24, 28, 16, 20,
	32+8, 32+12, 32+0, 32+4, 32+24,32+28,32+16,32+20},
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64,
	  8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64
	},
	16*64
};

static struct GfxDecodeInfo gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &suprslam_8x8x4_layout,   0x000, 16 },
	{ REGION_GFX2, 0, &suprslam_16x16x4_layout, 0x200, 16 },
	{ REGION_GFX3, 0, &suprslam_16x16x4_layout, 0x100, 16 },
	{ -1 } /* end of array */
};

/*** MORE SOUND **************************************************************/

static void irqhandler(int irq)
{
	cpu_set_irq_line(1,0,irq ? ASSERT_LINE : CLEAR_LINE);
}

static struct YM2610interface ym2610_interface =
{
	1,
	8000000,	/* 8 MHz??? */
	{ 25 },
	{ 0 },
	{ 0 },
	{ 0 },
	{ 0 },
	{ irqhandler },
	{ REGION_SOUND1 },
	{ REGION_SOUND2 },
	{ YM3012_VOL(100,MIXER_PAN_LEFT,100,MIXER_PAN_RIGHT) }
};

/*** MACHINE DRIVER **********************************************************/

static MACHINE_DRIVER_START( suprslam )
	MDRV_CPU_ADD(M68000, 16000000)
	MDRV_CPU_MEMORY(suprslam_readmem,suprslam_writemem)
	MDRV_CPU_VBLANK_INT(irq1_line_hold,1)

	MDRV_CPU_ADD(Z80,8000000/2)
	MDRV_CPU_FLAGS(CPU_AUDIO_CPU)	/* 4 MHz ??? */
	MDRV_CPU_MEMORY(sound_readmem,sound_writemem)
	MDRV_CPU_PORTS(suprslam_sound_readport,suprslam_sound_writeport)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(2300) /* hand-tuned */

	MDRV_GFXDECODE(gfxdecodeinfo)

	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_UPDATE_AFTER_VBLANK)
	MDRV_SCREEN_SIZE(64*8, 64*8)
	MDRV_VISIBLE_AREA(0*8, 40*8-1, 0*8, 28*8-1)
	MDRV_PALETTE_LENGTH(0x800)

	MDRV_VIDEO_START(suprslam)
	MDRV_VIDEO_UPDATE(suprslam)

	MDRV_SOUND_ATTRIBUTES(SOUND_SUPPORTS_STEREO)
	MDRV_SOUND_ADD(YM2610, ym2610_interface)
MACHINE_DRIVER_END

/*** ROM LOADING *************************************************************/

ROM_START( suprslam )
	ROM_REGION( 0x100000, REGION_CPU1, 0 ) /* 68000 Code */
	ROM_LOAD16_WORD_SWAP( "eb26ic47.bin", 0x000000, 0x080000, CRC(8d051fd8) SHA1(1820209306116e5b09cc10a8b3661d232c688b24) )
	ROM_LOAD16_WORD_SWAP( "eb26ic73.bin", 0x080000, 0x080000, CRC(ca4ad383) SHA1(143ee475761fa54d5b3a9f4e3fb3acc8408972fd) )

	ROM_REGION( 0x030000, REGION_CPU2, 0 ) /* Z80 Code */
	ROM_LOAD( "eb26ic38.bin", 0x000000, 0x020000, CRC(153f2c50) SHA1(b70f248cfb18239fcd26e36fb36159f219debf2c) )
	ROM_RELOAD(               0x010000, 0x020000 )

	ROM_REGION( 0x100000, REGION_SOUND1, 0 ) /* Samples */
	ROM_LOAD( "eb26ic59.bin", 0x000000, 0x100000, CRC(4ae4095b) SHA1(62b0600b18febb6cecb6370b03a2d6b7756840a2) )

	ROM_REGION( 0x200000, REGION_SOUND2, 0 ) /* Samples */
	ROM_LOAD( "eb26ic66.bin", 0x000000, 0x200000, CRC(8cb33682) SHA1(0e6189ef0673227d35b9a154e333cc6cf9b65df6) )

	ROM_REGION( 0x200000, REGION_GFX1, ROMREGION_DISPOSE ) /* 8x8x4 'Screen' Layer GFX */
	ROM_LOAD( "eb26ic43.bin", 0x000000, 0x200000, CRC(9dfb0959) SHA1(ba479192a422a55efcf8aa7ff995c914525b4a56) )

	ROM_REGION( 0x800000, REGION_GFX2, ROMREGION_DISPOSE ) /* 16x16x4 Sprites GFX */
	ROM_LOAD( "eb26ic09.bin", 0x000000, 0x200000, CRC(5a415365) SHA1(a59a4ab231980b0540e9a8356a02530217779dbd) )
	ROM_LOAD( "eb26ic10.bin", 0x200000, 0x200000, CRC(a04f3140) SHA1(621ff823d93fecdde801912064ac951727b71677) )
	ROM_LOAD( "eb26_100.bin", 0x400000, 0x200000, CRC(c2ee5eb6) SHA1(4b61e77a0d0f38b542d5e32fa25799a4c85bf651) )
	ROM_LOAD( "eb26_101.bin", 0x600000, 0x200000, CRC(7df654b7) SHA1(3a5ed6ee7cc31566e908b835a065e9bce60389fb) )

	ROM_REGION( 0x400000, REGION_GFX3, ROMREGION_DISPOSE ) /* 16x16x4 BG GFX */
	ROM_LOAD( "eb26ic12.bin", 0x000000, 0x200000, CRC(14561bd7) SHA1(5f69f68a305aba9acb21b844c8aa5b1de60f89ff) )
	ROM_LOAD( "eb26ic36.bin", 0x200000, 0x200000, CRC(92019d89) SHA1(dbf6f8384341707996e4b9e07a3d4f536cf4905b) )
ROM_END

/*** GAME DRIVERS ************************************************************/

GAMEX( 1995, suprslam, 0, suprslam, suprslam, 0, ROT0, "Banpresto / Toei Animation", "Super Slams", GAME_IMPERFECT_GRAPHICS )
