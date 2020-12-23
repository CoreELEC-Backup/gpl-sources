/***************************************************************************

Gun Dealer memory map

driver by Nicola Salmoria

Yam! Yam!? runs on the same hardware but has a protection device which can
           access RAM at e000. Program writes to e000 and expects a value back
		   at e001, then jumps to subroutines at e010 and e020. Also, the
		   player and coin inputs appear magically at e004-e006.

0000-7fff ROM
8000-bfff ROM (banked)
c400-c7ff palette RAM
c800-cfff background video RAM
d000-dfff foreground (scrollable) video RAM.
e000-ffff work RAM

read:
c000      DSW0
c001      DSW1
c004      COIN (Gun Dealer only)
c005      IN1 (Gun Dealer only)
c006      IN0 (Gun Dealer only)

write:
c010-c011 foreground scroll x lo-hi (Yam Yam)
c012-c013 foreground scroll y lo-hi (Yam Yam)
c014      flip screen
c015      Yam Yam only, maybe reset protection device
c016      ROM bank selector
c020-c021 foreground scroll x hi-lo (Gun Dealer)
c022-c023 foreground scroll y hi-lo (Gun Dealer)

I/O:
read:
01        YM2203 read

write:
00        YM2203 control
01        YM2203 write

Interrupts:
Runs in interrupt mode 0, the interrupt vectors are 0xcf (RST 08h) and
0xd7 (RST 10h)

***************************************************************************/

#include "driver.h"


extern unsigned char *gundealr_bg_videoram,*gundealr_fg_videoram;

WRITE_HANDLER( gundealr_paletteram_w );
WRITE_HANDLER( gundealr_bg_videoram_w );
WRITE_HANDLER( gundealr_fg_videoram_w );
WRITE_HANDLER( gundealr_fg_scroll_w );
WRITE_HANDLER( yamyam_fg_scroll_w );
WRITE_HANDLER( gundealr_flipscreen_w );
VIDEO_UPDATE( gundealr );
VIDEO_START( gundealr );


static int input_ports_hack;

static INTERRUPT_GEN( yamyam_interrupt )
{
	if (cpu_getiloops() == 0)
	{
		if (input_ports_hack)
		{
			unsigned char *RAM = memory_region(REGION_CPU1);
			RAM[0xe004] = readinputport(4);	/* COIN */
			RAM[0xe005] = readinputport(3);	/* IN1 */
			RAM[0xe006] = readinputport(2);	/* IN0 */
		}
		cpu_set_irq_line_and_vector(0, 0, HOLD_LINE, 0xd7);	/* RST 10h vblank */
	}
	else if ((cpu_getiloops() & 1) == 1)
		cpu_set_irq_line_and_vector(0, 0, HOLD_LINE, 0xcf);	/* RST 08h sound (hand tuned) */
}

static WRITE_HANDLER( yamyam_bankswitch_w )
{
 	int bankaddress;
	unsigned char *RAM = memory_region(REGION_CPU1);

	bankaddress = 0x10000 + (data & 0x07) * 0x4000;
	cpu_setbank(1,&RAM[bankaddress]);
}

static WRITE_HANDLER( yamyam_protection_w )
{
	unsigned char *RAM = memory_region(REGION_CPU1);

logerror("e000 = %02x\n",RAM[0xe000]);
	RAM[0xe000] = data;
	if (data == 0x03) RAM[0xe001] = 0x03;
	if (data == 0x04) RAM[0xe001] = 0x04;
	if (data == 0x05) RAM[0xe001] = 0x05;
	if (data == 0x0a) RAM[0xe001] = 0x08;
	if (data == 0x0d) RAM[0xe001] = 0x07;

	if (data == 0x03)
	{
		/*
		read dip switches
		3a 00 c0  ld   a,($c000)
		47        ld   b,a
		3a 01 c0  ld   a,($c001)
		c9        ret
		*/
		RAM[0xe010] = 0x3a;
		RAM[0xe011] = 0x00;
		RAM[0xe012] = 0xc0;
		RAM[0xe013] = 0x47;
		RAM[0xe014] = 0x3a;
		RAM[0xe015] = 0x01;
		RAM[0xe016] = 0xc0;
		RAM[0xe017] = 0xc9;
	}
	if (data == 0x05)
	{
		/*
		add a to hl
		c5        push    bc
		010000    ld      bc,#0000
		4f        ld      c,a
		09        add     hl,bc
		c1        pop     bc
		c9        ret
		*/
		RAM[0xe020] = 0xc5;
		RAM[0xe021] = 0x01;
		RAM[0xe022] = 0x00;
		RAM[0xe023] = 0x00;
		RAM[0xe024] = 0x4f;
		RAM[0xe025] = 0x09;
		RAM[0xe026] = 0xc1;
		RAM[0xe027] = 0xc9;
		/*
		lookup data in table
		cd20e0    call    #e020
		7e        ld      a,(hl)
		c9        ret
		*/
		RAM[0xe010] = 0xcd;
		RAM[0xe011] = 0x20;
		RAM[0xe012] = 0xe0;
		RAM[0xe013] = 0x7e;
		RAM[0xe014] = 0xc9;
	}
}



static MEMORY_READ_START( readmem )
	{ 0x0000, 0x7fff, MRA_ROM },
	{ 0x8000, 0xbfff, MRA_BANK1 },
	{ 0xc000, 0xc000, input_port_0_r },	/* DSW0 */
	{ 0xc001, 0xc001, input_port_1_r },	/* DSW1 */
	{ 0xc004, 0xc004, input_port_2_r },	/* COIN (Gun Dealer only) */
	{ 0xc005, 0xc005, input_port_3_r },	/* IN1 (Gun Dealer only) */
	{ 0xc006, 0xc006, input_port_4_r },	/* IN0 (Gun Dealer only) */
	{ 0xc400, 0xffff, MRA_RAM },
MEMORY_END

static MEMORY_WRITE_START( writemem )
	{ 0x0000, 0xbfff, MWA_ROM },
	{ 0xc010, 0xc013, yamyam_fg_scroll_w },		/* Yam Yam only */
	{ 0xc014, 0xc014, gundealr_flipscreen_w },
	{ 0xc016, 0xc016, yamyam_bankswitch_w },
	{ 0xc020, 0xc023, gundealr_fg_scroll_w },	/* Gun Dealer only */
	{ 0xc400, 0xc7ff, gundealr_paletteram_w, &paletteram },
	{ 0xc800, 0xcfff, gundealr_bg_videoram_w, &gundealr_bg_videoram },
	{ 0xd000, 0xdfff, gundealr_fg_videoram_w, &gundealr_fg_videoram },
	{ 0xe000, 0xffff, MWA_RAM },
MEMORY_END

static PORT_READ_START( readport )
	{ 0x01, 0x01, YM2203_read_port_0_r },
PORT_END

static PORT_WRITE_START( writeport )
	{ 0x00, 0x00, YM2203_control_port_0_w },
	{ 0x01, 0x01, YM2203_write_port_0_w },
PORT_END



INPUT_PORTS_START( gundealr )
	PORT_START	/* DSW0 */
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x0c, "Easy" )
	PORT_DIPSETTING(    0x08, "Medium" )
	PORT_DIPSETTING(    0x04, "Hard" )
	PORT_DIPSETTING(    0x00, "Hardest" )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START	/* DSW1 */
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0x38, 0x38, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x38, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x28, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, "0" )
	PORT_DIPSETTING(    0x40, "1" )
	PORT_DIPSETTING(    0x80, "2" )
	PORT_DIPSETTING(    0xc0, "3" )

	PORT_START	/* COIN */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_4WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_4WAY | IPF_PLAYER2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_4WAY | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_4WAY | IPF_PLAYER2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */

	PORT_START	/* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_4WAY | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_4WAY | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_4WAY | IPF_PLAYER1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_4WAY | IPF_PLAYER1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */
INPUT_PORTS_END

INPUT_PORTS_START( gundealt )
	PORT_START	/* DSW0 */
	PORT_DIPNAME( 0x01, 0x01, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(    0x0c, "Easy" )
	PORT_DIPSETTING(    0x08, "Medium" )
	PORT_DIPSETTING(    0x04, "Hard" )
	PORT_DIPSETTING(    0x00, "Hardest" )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START	/* DSW1 */
	PORT_DIPNAME( 0x07, 0x07, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0x38, 0x38, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x00, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x08, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x10, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x18, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x38, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(    0x28, DEF_STR( 1C_3C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 1C_4C ) )
	PORT_DIPNAME( 0xc0, 0xc0, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x00, "0" )
	PORT_DIPSETTING(    0x40, "1" )
	PORT_DIPSETTING(    0x80, "2" )
	PORT_DIPSETTING(    0xc0, "3" )

	PORT_START	/* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_4WAY | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_4WAY | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_4WAY | IPF_PLAYER1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_4WAY | IPF_PLAYER1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */

	PORT_START	/* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_4WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_4WAY | IPF_PLAYER2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_4WAY | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_4WAY | IPF_PLAYER2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */

	PORT_START	/* COIN */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

INPUT_PORTS_START( yamyam )
	PORT_START	/* DSW0 */
	PORT_DIPNAME( 0x03, 0x00, DEF_STR( Lives ) )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPSETTING(    0x01, "4" )
	PORT_DIPSETTING(    0x02, "5" )
	PORT_DIPSETTING(    0x03, "6" )
	PORT_DIPNAME( 0x0c, 0x00, "Difficulty?" )
	PORT_DIPSETTING(    0x00, "Easy?" )
	PORT_DIPSETTING(    0x04, "Medium?" )
	PORT_DIPSETTING(    0x08, "Hard?" )
	PORT_DIPSETTING(    0x0c, "Hardest?" )
	PORT_DIPNAME( 0x10, 0x00, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Cabinet ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Upright ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Cocktail ) )
	PORT_DIPNAME( 0x80, 0x00, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x80, DEF_STR( On ) )

	PORT_START	/* DSW1 */
	PORT_DIPNAME( 0x07, 0x00, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 2C_1C ) )
/*	PORT_DIPSETTING(    0x03, DEF_STR( 1C_1C ) ) */
/*	PORT_DIPSETTING(    0x02, DEF_STR( 1C_1C ) ) */
/*	PORT_DIPSETTING(    0x01, DEF_STR( 1C_1C ) ) */
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x38, 0x00, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(    0x38, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x30, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x28, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x20, DEF_STR( 2C_1C ) )
/*	PORT_DIPSETTING(    0x18, DEF_STR( 1C_1C ) ) */
/*	PORT_DIPSETTING(    0x10, DEF_STR( 1C_1C ) ) */
/*	PORT_DIPSETTING(    0x08, DEF_STR( 1C_1C ) ) */
	PORT_DIPSETTING(    0x00, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x40, 0x00, DEF_STR( Free_Play ) )
	PORT_DIPSETTING(    0x00, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x40, DEF_STR( On ) )
	PORT_SERVICE( 0x80, IP_ACTIVE_LOW )

	PORT_START	/* COIN */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_COIN2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_START2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_TILT )	/* "TEST" */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START	/* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_4WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_4WAY | IPF_PLAYER2 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_4WAY | IPF_PLAYER2 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_4WAY | IPF_PLAYER2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */

	PORT_START	/* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_4WAY | IPF_PLAYER1 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT | IPF_4WAY | IPF_PLAYER1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN | IPF_4WAY | IPF_PLAYER1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_UP | IPF_4WAY | IPF_PLAYER1 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER1 )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER1 )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )	/* probably unused */
INPUT_PORTS_END



static struct GfxLayout charlayout =
{
	8,8,
	RGN_FRAC(1,1),
	4,
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8
};

static struct GfxLayout spritelayout =
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

static struct GfxDecodeInfo gfxdecodeinfo[] =
{
	{ REGION_GFX1, 0, &charlayout,     0, 16 },	/* colors 0-255 */
	{ REGION_GFX2, 0, &spritelayout, 256, 16 },	/* colors 256-511 */
	{ -1 } /* end of array */
};



static struct YM2203interface ym2203_interface =
{
	1,			/* 1 chip */
	1500000,	/* 1.5 MHz ?????? */
	{ YM2203_VOL(25,25) },
	{ 0 },
	{ 0 },
	{ 0 },
	{ 0 }
};



static MACHINE_DRIVER_START( gundealr )

	/* basic machine hardware */
	MDRV_CPU_ADD(Z80, 8000000)	/* 8 MHz ??? */
	MDRV_CPU_MEMORY(readmem,writemem)
	MDRV_CPU_PORTS(readport,writeport)
	MDRV_CPU_VBLANK_INT(yamyam_interrupt,4)	/* ? */

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 32*8-1, 2*8, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(512)

	MDRV_VIDEO_START(gundealr)
	MDRV_VIDEO_UPDATE(gundealr)

	/* sound hardware */
	MDRV_SOUND_ADD(YM2203, ym2203_interface)
MACHINE_DRIVER_END



/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( gundealr )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD( "gundealr.1",   0x00000, 0x10000, CRC(5797e830) SHA1(54bd9fbcafdf3fff55d73ecfe26d8e8df0dd55d9) )
	ROM_RELOAD(               0x10000, 0x10000 )	/* banked at 0x8000-0xbfff */

	ROM_REGION( 0x10000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "3.6p",         0x00000, 0x10000, CRC(01f99de2) SHA1(2d9e9c50b0669811beb6fa53c0ff1b240fa939c7) )

	ROM_REGION( 0x20000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "gundealr.2",   0x00000, 0x20000, CRC(7874ec41) SHA1(2d2ff013cc37ce5966aa4b6c6724234655196102) )
ROM_END

ROM_START( gundeala )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD( "gundeala.1",   0x00000, 0x10000, CRC(d87e24f1) SHA1(5ac3e20e5848b9cab2a23e083d2566bfd54502d4) )
	ROM_RELOAD(               0x10000, 0x10000 )	/* banked at 0x8000-0xbfff */

	ROM_REGION( 0x10000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "gundeala.3",   0x00000, 0x10000, CRC(836cf1a3) SHA1(ca57e7fc3e4497d249af963d1c8610e80ca65aa7) )

	ROM_REGION( 0x20000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "gundeala.2",   0x00000, 0x20000, CRC(4b5fb53c) SHA1(3b73d9aeed334aece75f551f5b7f3cec0aedbfaa) )
ROM_END

ROM_START( gundealt )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD( "1.3j",         0x00000, 0x10000, CRC(1d951292) SHA1(a8bd34dfaf31c7dc4f9e0ec1fd7d4e10c5b29a85) )
	ROM_RELOAD(               0x10000, 0x10000 )	/* banked at 0x8000-0xbfff */

	ROM_REGION( 0x10000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "3.6p",         0x00000, 0x10000, CRC(01f99de2) SHA1(2d9e9c50b0669811beb6fa53c0ff1b240fa939c7) )

	ROM_REGION( 0x20000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "2.6b",         0x00000, 0x20000, CRC(508ed0d0) SHA1(ea6b2d07e2e3d4f6c2a622a73b150ee7709b28de) )
ROM_END

ROM_START( yamyam )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD( "b3.f10",       0x00000, 0x20000, CRC(96ae9088) SHA1(a605882dcdcf1e8cf8b0112f614e696d59acfd97) )
	ROM_RELOAD(               0x10000, 0x20000 )	/* banked at 0x8000-0xbfff */

	ROM_REGION( 0x10000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "b2.d16",       0x00000, 0x10000, CRC(cb4f84ee) SHA1(54319ecbd74b763757eb6d17c8f7be0705ab0714) )

	ROM_REGION( 0x20000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "b1.a16",       0x00000, 0x20000, CRC(b122828d) SHA1(90994ba548893a2eacdd58351cfa3952f4af926a) )
ROM_END

/* only gfx are different, code is the same */
ROM_START( wiseguy )
	ROM_REGION( 0x30000, REGION_CPU1, 0 )	/* 64k for code + 128k for banks */
	ROM_LOAD( "b3.f10",       0x00000, 0x20000, CRC(96ae9088) SHA1(a605882dcdcf1e8cf8b0112f614e696d59acfd97) )
	ROM_RELOAD(               0x10000, 0x20000 )	/* banked at 0x8000-0xbfff */

	ROM_REGION( 0x10000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "wguyb2.bin",   0x00000, 0x10000, CRC(1c684c46) SHA1(041bc500e31b02a8bf3ce4683a67de998f938ccc) )

	ROM_REGION( 0x20000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "b1.a16",       0x00000, 0x20000, CRC(b122828d) SHA1(90994ba548893a2eacdd58351cfa3952f4af926a) )
ROM_END



static DRIVER_INIT( gundealr )
{
	input_ports_hack = 0;
}

static DRIVER_INIT( yamyam )
{
	input_ports_hack = 1;
	install_mem_write_handler(0, 0xe000, 0xe000, yamyam_protection_w);
}



GAME( 1990, gundealr, 0,        gundealr, gundealr, gundealr, ROT270, "Dooyong", "Gun Dealer (set 1)" )
GAME( 19??, gundeala, gundealr, gundealr, gundealr, gundealr, ROT270, "Dooyong", "Gun Dealer (set 2)" )
GAME( 1990, gundealt, gundealr, gundealr, gundealt, gundealr, ROT270, "Tecmo", "Gun Dealer (Tecmo)" )
GAME( 1990, yamyam,   0,        gundealr, yamyam,   yamyam,   ROT0,   "Dooyong", "Yam! Yam!?" )
GAME( 1990, wiseguy,  yamyam,   gundealr, yamyam,   yamyam,   ROT0,   "Dooyong", "Wise Guy" )
