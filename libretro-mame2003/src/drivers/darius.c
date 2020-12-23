/***************************************************************************

Darius    (c) Taito 1986
======

David Graves, Jarek Burczynski

Sound panning and other enhancements: Hiromitsu Shioya

Sources:		MAME Rastan driver
			Raine source - invaluable for this driver -
			many thanks to Richard Bush and the Raine Team.

				*****

Rom Versions
------------

Darius appears to be a revision of Dariusj (as well as being
for a different sales area). It has continue facilities, missing
in Dariusj, and extra sprites which are used when continuing.
It also has 2 extra program roms.


Use of PC080SN
--------------

This game uses 3 x PC080SN for generating tilemaps. They must be
mapped somehow to a single memory block and set of scroll registers.
There is an additional text tilemap on top of this, to allow for
both background planes scrolling. This need is presumably what led
to the TC0100SCN tilemap chip, debuted in Ninja Warriors (c)1987.
(The TC0100SCN includes a separate text layer.)


ADPCM Z80
---------

This writes the rom area whenever an interrupt occurs. It has no ram
therefore no stack to store registers or return addresses: so the
standard Z80 writes to the stack become irrelevant.


Dumpers Notes
=============

Darius (Old JPN Ver.)
(c)1986 Taito

-----------------------
Sound Board
K1100228A
CPU 	:Z80 x2
Sound	:YM2203C x2
OSC 	:8.000MHz
-----------------------
A96_56.18
A96_57.33

-----------------------
M4300067A
K1100226A
CPU 	:MC68000P8 x2
OSC 	:16000.00KHz
-----------------------
A96_28.152
A96_29.185
A96_30.154
A96_31.187

A96_32.157
A96_33.190
A96_34.158
A96_35.191

A96_36.175
A96_37.196
A96_38.176
A96_39.197
A96_40.177
A96_41.198
A96_42.178
A96_43.199
A96_44.179
A96_45.200
A96_46.180
A96_47.201

-----------------------
K1100227A
OSC 	:26686.00KHz
Other	:PC080SN x3
-----------------------
A96_48.103
A96_48.24
A96_48.63
A96_49.104
A96_49.25
A96_49.64
A96_50.105
A96_50.26
A96_50.65
A96_51.131
A96_51.47
A96_51.86
A96_52.132
A96_52.48
A96_52.87
A96_53.133
A96_53.49
A96_53.88

A96_54.142
A96_55.143

A96-24.163
A96-25.164
A96-26.165


TODO
====

When you add a coin there is temporary volume distortion of other
sounds.

***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <math.h>

#include "driver.h"
#include "state.h"
#include "vidhrdw/generic.h"
#include "vidhrdw/taitoic.h"
#include "cpu/z80/z80.h"
#include "sndhrdw/taitosnd.h"


MACHINE_INIT( darius );

VIDEO_START( darius );
VIDEO_UPDATE( darius );

static UINT16 cpua_ctrl;
static UINT16 coin_word=0;

extern data16_t *darius_fg_ram;
READ16_HANDLER ( darius_fg_layer_r );
WRITE16_HANDLER( darius_fg_layer_w );

static size_t sharedram_size;
static data16_t *sharedram;


static READ16_HANDLER( sharedram_r )
{
	return sharedram[offset];
}

static WRITE16_HANDLER( sharedram_w )
{
	COMBINE_DATA(&sharedram[offset]);
}

void parse_control( void )	/* assumes Z80 sandwiched between 68Ks */
{
	/* bit 0 enables cpu B */
	/* however this fails when recovering from a save state
	   if cpu B is disabled !! */
	cpu_set_reset_line(2,(cpua_ctrl &0x1) ? CLEAR_LINE : ASSERT_LINE);

}

static WRITE16_HANDLER( cpua_ctrl_w )
{
	if ((data &0xff00) && ((data &0xff) == 0))
		data = data >> 8;	/* for Wgp */
	cpua_ctrl = data;

	parse_control();

	logerror("CPU #0 PC %06x: write %04x to cpu control\n",activecpu_get_pc(),data);
}

static WRITE16_HANDLER( darius_watchdog_w )
{
	watchdog_reset_w(0,data);
}

static READ16_HANDLER( paletteram16_r )
{
	return paletteram16[offset];
}


/**********************************************************
                        GAME INPUTS
**********************************************************/

static READ16_HANDLER( darius_ioc_r )
{
	switch (offset)
	{
		case 0x01:
			return (taitosound_comm_r(0) & 0xff);	/* sound interface read */

		case 0x04:
			return input_port_0_word_r(0,mem_mask);	/* IN0 */

		case 0x05:
			return input_port_1_word_r(0,mem_mask);	/* IN1 */

		case 0x06:
			return input_port_2_word_r(0,mem_mask);	/* IN2 */

		case 0x07:
			return coin_word;	/* bits 3&4 coin lockouts, must return zero */

		case 0x08:
			return input_port_3_word_r(0,mem_mask);	/* DSW */
	}

logerror("CPU #0 PC %06x: warning - read unmapped ioc offset %06x\n",activecpu_get_pc(),offset);

	return 0xff;
}

static WRITE16_HANDLER( darius_ioc_w )
{
	switch (offset)
	{
		case 0x00:	/* sound interface write */

			taitosound_port_w (0, data & 0xff);
			return;

		case 0x01:	/* sound interface write */

			taitosound_comm_w (0, data & 0xff);
			return;

		case 0x28:	/* unknown, written by both cpus - always 0? */
//usrintf_showmessage(" address %04x value %04x",offset,data);
			return;

		case 0x30:	/* coin control */
			/* bits 7,5,4,0 used on reset */
			/* bit 4 used whenever bg is blanked ? */
			coin_lockout_w(0, ~data & 0x02);
			coin_lockout_w(1, ~data & 0x04);
			coin_counter_w(0, data & 0x08);
			coin_counter_w(1, data & 0x40);
			coin_word = data &0xffff;
//usrintf_showmessage(" address %04x value %04x",offset,data);
			return;
	}

logerror("CPU #0 PC %06x: warning - write unmapped ioc offset %06x with %04x\n",activecpu_get_pc(),offset,data);
}


/***********************************************************
                     MEMORY STRUCTURES
***********************************************************/

static MEMORY_READ16_START( darius_readmem )
	{ 0x000000, 0x05ffff, MRA16_ROM },
	{ 0x080000, 0x08ffff, MRA16_RAM },		/* main RAM */
	{ 0xc00000, 0xc0001f, darius_ioc_r },	/* inputs, sound */
	{ 0xd00000, 0xd0ffff, PC080SN_word_0_r },	/* tilemaps */
	{ 0xd80000, 0xd80fff, paletteram16_r },	/* palette */
	{ 0xe00100, 0xe00fff, MRA16_RAM },		/* sprite ram */
	{ 0xe01000, 0xe02fff, sharedram_r },
	{ 0xe08000, 0xe0ffff, darius_fg_layer_r },	/* front tilemap */
	{ 0xe10000, 0xe10fff, MRA16_RAM },		/* ??? */
MEMORY_END

static MEMORY_WRITE16_START( darius_writemem )
	{ 0x000000, 0x05ffff, MWA16_ROM },
	{ 0x080000, 0x08ffff, MWA16_RAM },
	{ 0x0a0000, 0x0a0001, cpua_ctrl_w },
	{ 0x0b0000, 0x0b0001, darius_watchdog_w },
	{ 0xc00000, 0xc0007f, darius_ioc_w },	/* coin ctr & lockout, sound */
	{ 0xd00000, 0xd0ffff, PC080SN_word_0_w },
	{ 0xd20000, 0xd20003, PC080SN_yscroll_word_0_w },
	{ 0xd40000, 0xd40003, PC080SN_xscroll_word_0_w },
	{ 0xd50000, 0xd50003, PC080SN_ctrl_word_0_w },
	{ 0xd80000, 0xd80fff, paletteram16_xBBBBBGGGGGRRRRR_word_w, &paletteram16 },
	{ 0xe00100, 0xe00fff, MWA16_RAM, &spriteram16, &spriteram_size },
	{ 0xe01000, 0xe02fff, sharedram_w, &sharedram, &sharedram_size },
	{ 0xe08000, 0xe0ffff, darius_fg_layer_w, &darius_fg_ram },
	{ 0xe10000, 0xe10fff, MWA16_RAM },
MEMORY_END

static MEMORY_READ16_START( darius_cpub_readmem )
	{ 0x000000, 0x03ffff, MRA16_ROM },
	{ 0x040000, 0x04ffff, MRA16_RAM },	/* local RAM */
	{ 0xe01000, 0xe02fff, sharedram_r },
MEMORY_END

static MEMORY_WRITE16_START( darius_cpub_writemem )
	{ 0x000000, 0x03ffff, MWA16_ROM },
	{ 0x040000, 0x04ffff, MWA16_RAM },
	{ 0xc00000, 0xc0007f, darius_ioc_w },	/* only writes $c00050 (?) */
	{ 0xd80000, 0xd80fff, paletteram16_xBBBBBGGGGGRRRRR_word_w },
	{ 0xe00100, 0xe00fff, spriteram16_w },	/* some writes */
	{ 0xe01000, 0Xe02fff, sharedram_w },
	{ 0xe08000, 0xe0ffff, darius_fg_layer_w },	/* a few writes */
MEMORY_END


/*****************************************************
                        SOUND
*****************************************************/

static int banknum = -1;
static int adpcm_command = 0;
static int nmi_enable = 0;

static void reset_sound_region(void)
{
	cpu_setbank( STATIC_BANK1, memory_region(REGION_CPU2) + (banknum * 0x8000) + 0x10000 );
//	cpu_setbank( 1, memory_region(REGION_CPU2) + (banknum * 0x8000) + 0x10000 );

}

static WRITE_HANDLER( sound_bankswitch_w )
{
		banknum = data &0x03;
		reset_sound_region();
//		banknum = data;
//		reset_sound_region();
}

static WRITE_HANDLER( adpcm_command_w )
{
	adpcm_command = data;
	/* logerror("#ADPCM command write =%2x\n",data); */
}

#if 0
static WRITE_HANDLER( display_value )
{
	usrintf_showmessage("d800=%x",data);
}
#endif


/*****************************************************
               Sound mixer/pan control
*****************************************************/

static UINT32 darius_def_vol[0x10];

#define DARIUS_VOL_MAX    (3*2 + 2)
#define DARIUS_PAN_MAX    (2 + 2 + 1)	/* FM 2port + PSG 2port + DA 1port */
static UINT8 darius_vol[DARIUS_VOL_MAX];
static UINT8 darius_pan[DARIUS_PAN_MAX];

static void update_fm0( void )
{
	int left, right;
	left  = (        darius_pan[0]  * darius_vol[6])>>8;
	right = ((0xff - darius_pan[0]) * darius_vol[6])>>8;
	mixer_set_stereo_volume( 6, left, right ); /* FM #0 */
}

static void update_fm1( void )
{
	int left, right;
	left  = (        darius_pan[1]  * darius_vol[7])>>8;
	right = ((0xff - darius_pan[1]) * darius_vol[7])>>8;
	mixer_set_stereo_volume( 7, left, right ); /* FM #1 */
}

static void update_psg0( int port )
{
	int left, right;
	left  = (        darius_pan[2]  * darius_vol[port])>>8;
	right = ((0xff - darius_pan[2]) * darius_vol[port])>>8;
	mixer_set_stereo_volume( port, left, right );
}

static void update_psg1( int port )
{
	int left, right;
	left  = (        darius_pan[3]  * darius_vol[port + 3])>>8;
	right = ((0xff - darius_pan[3]) * darius_vol[port + 3])>>8;
	mixer_set_stereo_volume( port + 3, left, right );
}

static void update_da( void )
{
	int left, right;
	left  = darius_def_vol[(darius_pan[4]>>4)&0x0f];
	right = darius_def_vol[(darius_pan[4]>>0)&0x0f];
	mixer_set_stereo_volume( 8, left, right );
}

static WRITE_HANDLER( darius_fm0_pan )
{
	darius_pan[0] = data&0xff;  /* data 0x00:right 0xff:left */
	update_fm0();
}

static WRITE_HANDLER( darius_fm1_pan )
{
	darius_pan[1] = data&0xff;
	update_fm1();
}

static WRITE_HANDLER( darius_psg0_pan )
{
	darius_pan[2] = data&0xff;
	update_psg0( 0 );
	update_psg0( 1 );
	update_psg0( 2 );
}

static WRITE_HANDLER( darius_psg1_pan )
{
	darius_pan[3] = data&0xff;
	update_psg1( 0 );
	update_psg1( 1 );
	update_psg1( 2 );
}

static WRITE_HANDLER( darius_da_pan )
{
	darius_pan[4] = data&0xff;
	update_da();
}

/**** Mixer Control ****/

static WRITE_HANDLER( darius_write_portA0 )
{
	// volume control FM #0 PSG #0 A
	//usrintf_showmessage(" pan %02x %02x %02x %02x %02x", darius_pan[0], darius_pan[1], darius_pan[2], darius_pan[3], darius_pan[4] );
	//usrintf_showmessage(" A0 %02x A1 %02x B0 %02x B1 %02x", port[0], port[1], port[2], port[3] );
	darius_vol[0] = darius_def_vol[(data>>4)&0x0f];
	darius_vol[6] = darius_def_vol[(data>>0)&0x0f];
	update_fm0();
	update_psg0( 0 );
}

static WRITE_HANDLER( darius_write_portA1 )
{
	// volume control FM #1 PSG #1 A
	//usrintf_showmessage(" pan %02x %02x %02x %02x %02x", darius_pan[0], darius_pan[1], darius_pan[2], darius_pan[3], darius_pan[4] );
	darius_vol[3] = darius_def_vol[(data>>4)&0x0f];
	darius_vol[7] = darius_def_vol[(data>>0)&0x0f];
	update_fm1();
	update_psg1( 0 );
}

static WRITE_HANDLER( darius_write_portB0 )
{
	// volume control PSG #0 B/C
	//usrintf_showmessage(" pan %02x %02x %02x %02x %02x", darius_pan[0], darius_pan[1], darius_pan[2], darius_pan[3], darius_pan[4] );
	darius_vol[1] = darius_def_vol[(data>>4)&0x0f];
	darius_vol[2] = darius_def_vol[(data>>0)&0x0f];
	update_psg0( 1 );
	update_psg0( 2 );
}

static WRITE_HANDLER( darius_write_portB1 )
{
	// volume control PSG #1 B/C
	//usrintf_showmessage(" pan %02x %02x %02x %02x %02x", darius_pan[0], darius_pan[1], darius_pan[2], darius_pan[3], darius_pan[4] );
	darius_vol[4] = darius_def_vol[(data>>4)&0x0f];
	darius_vol[5] = darius_def_vol[(data>>0)&0x0f];
	update_psg1( 1 );
	update_psg1( 2 );
}


/*****************************************************
           Sound memory structures / ADPCM
*****************************************************/

static MEMORY_READ_START( darius_sound_readmem )
	{ 0x0000, 0x7fff, MRA_BANK1 },
	{ 0x8000, 0x8fff, MRA_RAM },
	{ 0x9000, 0x9000, YM2203_status_port_0_r },
	{ 0x9001, 0x9001, YM2203_read_port_0_r },
	{ 0xa000, 0xa000, YM2203_status_port_1_r },
	{ 0xa001, 0xa001, YM2203_read_port_1_r },
	{ 0xb000, 0xb000, MRA_NOP },
	{ 0xb001, 0xb001, taitosound_slave_comm_r },
MEMORY_END

static MEMORY_WRITE_START( darius_sound_writemem )
	{ 0x0000, 0x7fff, MWA_ROM },
	{ 0x8000, 0x8fff, MWA_RAM },
	{ 0x9000, 0x9000, YM2203_control_port_0_w },
	{ 0x9001, 0x9001, YM2203_write_port_0_w },
	{ 0xa000, 0xa000, YM2203_control_port_1_w },
	{ 0xa001, 0xa001, YM2203_write_port_1_w },
	{ 0xb000, 0xb000, taitosound_slave_port_w },
	{ 0xb001, 0xb001, taitosound_slave_comm_w },
	{ 0xc000, 0xc000, darius_fm0_pan },
	{ 0xc400, 0xc400, darius_fm1_pan },
	{ 0xc800, 0xc800, darius_psg0_pan },
	{ 0xcc00, 0xcc00, darius_psg1_pan },
	{ 0xd000, 0xd000, darius_da_pan },
	{ 0xd400, 0xd400, adpcm_command_w },	/* ADPCM command for second Z80 to read from port 0x00 */
//	{ 0xd800, 0xd800, display_value },	/* ??? */
	{ 0xdc00, 0xdc00, sound_bankswitch_w },
MEMORY_END

static MEMORY_READ_START( darius_sound2_readmem )
	{ 0x0000, 0xffff, MRA_ROM },
	/* yes, no RAM */
MEMORY_END

static MEMORY_WRITE_START( darius_sound2_writemem )
	{ 0x0000, 0xffff, MWA_NOP },	/* writes rom whenever interrupt occurs - as no stack */
	/* yes, no RAM */
MEMORY_END


static void darius_adpcm_int (int data)
{
	if (nmi_enable)
	{
		cpu_set_nmi_line(3, PULSE_LINE);
	}
}

static struct MSM5205interface msm5205_interface =
{
	1,				/* 1 chip */
	384000, 			/* 384KHz */
	{ darius_adpcm_int},	/* interrupt function */
	{ MSM5205_S48_4B},	/* 8KHz   */
	{ 50 }			/* volume */
};

static READ_HANDLER( adpcm_command_read )
{
	/* logerror("read port 0: %02x  PC=%4x\n",adpcm_command, activecpu_get_pc() ); */
	return adpcm_command;
}

static READ_HANDLER( readport2 )
{
	return 0;
}

static READ_HANDLER( readport3 )
{
	return 0;
}

static WRITE_HANDLER ( adpcm_nmi_disable )
{
	nmi_enable = 0;
	/* logerror("write port 0: NMI DISABLE  PC=%4x\n", data, activecpu_get_pc() ); */
}

static WRITE_HANDLER ( adpcm_nmi_enable )
{
	nmi_enable = 1;
	/* logerror("write port 1: NMI ENABLE   PC=%4x\n", activecpu_get_pc() ); */
}

static WRITE_HANDLER( adpcm_data_w )
{
	MSM5205_data_w (0,   data         );
	MSM5205_reset_w(0, !(data & 0x20) );	/* my best guess, but it could be output enable as well */
}

static PORT_READ_START( darius_sound2_readport )
	{ 0x00, 0x00, adpcm_command_read },
	{ 0x02, 0x02, readport2 },	/* ??? */
	{ 0x03, 0x03, readport3 },	/* ??? */
PORT_END

static PORT_WRITE_START( darius_sound2_writeport )
	{ 0x00, 0x00, adpcm_nmi_disable },
	{ 0x01, 0x01, adpcm_nmi_enable },
	{ 0x02, 0x02, adpcm_data_w },
PORT_END


/***********************************************************
                      INPUT PORTS, DIPs
***********************************************************/


#define TAITO_COINAGE_WORLD_16 \
	PORT_DIPNAME( 0x0030, 0x0030, DEF_STR( Coin_A ) ) \
	PORT_DIPSETTING(      0x0000, DEF_STR( 4C_1C ) ) \
	PORT_DIPSETTING(      0x0010, DEF_STR( 3C_1C ) ) \
	PORT_DIPSETTING(      0x0020, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(      0x0030, DEF_STR( 1C_1C ) ) \
	PORT_DIPNAME( 0x00c0, 0x00c0, DEF_STR( Coin_B ) ) \
	PORT_DIPSETTING(      0x00c0, DEF_STR( 1C_2C ) ) \
	PORT_DIPSETTING(      0x0080, DEF_STR( 1C_3C ) ) \
	PORT_DIPSETTING(      0x0040, DEF_STR( 1C_4C ) ) \
	PORT_DIPSETTING(      0x0000, DEF_STR( 1C_6C ) )

#define TAITO_COINAGE_JAPAN_16 \
	PORT_DIPNAME( 0x0030, 0x0030, DEF_STR( Coin_A ) ) \
	PORT_DIPSETTING(      0x0010, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(      0x0030, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(      0x0000, DEF_STR( 2C_3C ) ) \
	PORT_DIPSETTING(      0x0020, DEF_STR( 1C_2C ) ) \
	PORT_DIPNAME( 0x00c0, 0x00c0, DEF_STR( Coin_B ) ) \
	PORT_DIPSETTING(      0x0040, DEF_STR( 2C_1C ) ) \
	PORT_DIPSETTING(      0x00c0, DEF_STR( 1C_1C ) ) \
	PORT_DIPSETTING(      0x0000, DEF_STR( 2C_3C ) ) \
	PORT_DIPSETTING(      0x0080, DEF_STR( 1C_2C ) )

#define TAITO_DIFFICULTY_16 \
	PORT_DIPNAME( 0x0300, 0x0300, DEF_STR( Difficulty ) ) \
	PORT_DIPSETTING(      0x0200, "Easy" ) \
	PORT_DIPSETTING(      0x0300, "Medium" ) \
	PORT_DIPSETTING(      0x0100, "Hard" ) \
	PORT_DIPSETTING(      0x0000, "Hardest" )

#define DARIUS_PLAYERS_INPUT( player ) \
	PORT_START \
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_UP    | IPF_8WAY | player ) \
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | player ) \
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | player ) \
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | player ) \
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | player ) \
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | player ) \
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN ) \
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

#define DARIUS_SYSTEM_INPUT \
	PORT_START \
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 ) \
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_COIN2 ) \
	PORT_BIT( 0x04, IP_ACTIVE_LOW,  IPT_START1 )  \
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_START2 ) \
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_SERVICE1 ) \
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_TILT ) \
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNKNOWN ) \
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNKNOWN )


INPUT_PORTS_START( darius )
	DARIUS_PLAYERS_INPUT( IPF_PLAYER1 )

	DARIUS_PLAYERS_INPUT( IPF_PLAYER2 )

	DARIUS_SYSTEM_INPUT

	PORT_START	/* DSW */
	PORT_DIPNAME( 0x0001, 0x0001, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, "Autofire" )
	PORT_DIPSETTING(      0x0002, "Normal" )
	PORT_DIPSETTING(      0x0000, "Fast" )
	PORT_SERVICE( 0x0004, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( On ) )
	TAITO_COINAGE_WORLD_16
	TAITO_DIFFICULTY_16
	PORT_DIPNAME( 0x0c00, 0x0c00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(      0x0800, "every 600k" )
	PORT_DIPSETTING(      0x0c00, "600k only" )
	PORT_DIPSETTING(      0x0400, "800k only" )
	PORT_DIPSETTING(      0x0000, "none" )
	PORT_DIPNAME( 0x3000, 0x3000, DEF_STR( Lives ) )
	PORT_DIPSETTING(      0x3000, "3" )
	PORT_DIPSETTING(      0x2000, "4" )
	PORT_DIPSETTING(      0x1000, "5" )
	PORT_DIPSETTING(      0x0000, "6" )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, "Allow Continue" )
	PORT_DIPSETTING(      0x0000, DEF_STR( No ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Yes ) )
INPUT_PORTS_END

INPUT_PORTS_START( dariuse )
	DARIUS_PLAYERS_INPUT( IPF_PLAYER1 )

	DARIUS_PLAYERS_INPUT( IPF_PLAYER2 )

	DARIUS_SYSTEM_INPUT

	PORT_START	/* DSW */
	PORT_DIPNAME( 0x0001, 0x0001, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, "Autofire" )
	PORT_DIPSETTING(      0x0002, "Normal" )
	PORT_DIPSETTING(      0x0000, "Fast" )
	PORT_SERVICE( 0x0004, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( On ) )
	TAITO_COINAGE_JAPAN_16
	TAITO_DIFFICULTY_16
	PORT_DIPNAME( 0x0c00, 0x0c00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(      0x0800, "every 600k" )
	PORT_DIPSETTING(      0x0c00, "600k only" )
	PORT_DIPSETTING(      0x0400, "800k only" )
	PORT_DIPSETTING(      0x0000, "none" )
	PORT_DIPNAME( 0x3000, 0x3000, DEF_STR( Lives ) )
	PORT_DIPSETTING(      0x3000, "3" )
	PORT_DIPSETTING(      0x2000, "4" )
	PORT_DIPSETTING(      0x1000, "5" )
	PORT_DIPSETTING(      0x0000, "6" )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, "Allow Continue" )
	PORT_DIPSETTING(      0x0000, DEF_STR( No ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Yes ) )
INPUT_PORTS_END

INPUT_PORTS_START( dariusj )
	DARIUS_PLAYERS_INPUT( IPF_PLAYER1 )

	DARIUS_PLAYERS_INPUT( IPF_PLAYER2 )

	DARIUS_SYSTEM_INPUT

	PORT_START	/* DSW */
	PORT_DIPNAME( 0x0001, 0x0001, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, "Autofire" )
	PORT_DIPSETTING(      0x0002, "Normal" )
	PORT_DIPSETTING(      0x0000, "Fast" )
	PORT_SERVICE( 0x0004, IP_ACTIVE_LOW )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Demo_Sounds ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( On ) )
	TAITO_COINAGE_JAPAN_16
	TAITO_DIFFICULTY_16
	PORT_DIPNAME( 0x0c00, 0x0c00, DEF_STR( Bonus_Life ) )
	PORT_DIPSETTING(      0x0800, "every 600k" )
	PORT_DIPSETTING(      0x0c00, "600k only" )
	PORT_DIPSETTING(      0x0400, "800k only" )
	PORT_DIPSETTING(      0x0000, "none" )
	PORT_DIPNAME( 0x3000, 0x3000, DEF_STR( Lives ) )
	PORT_DIPSETTING(      0x3000, "3" )
	PORT_DIPSETTING(      0x2000, "4" )
	PORT_DIPSETTING(      0x1000, "5" )
	PORT_DIPSETTING(      0x0000, "6" )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
INPUT_PORTS_END

/**************************************************************
                           GFX DECODING
**************************************************************/

static struct GfxLayout tilelayout =
{
	16,16,	/* 16*16 sprites */
	RGN_FRAC(1,1),
	4,	/* 4 bits per pixel */
        { 24, 8, 16, 0 },       /* pixel bits separated */
	{ 0, 1, 2, 3, 4, 5, 6, 7,
	  0+ 32*8, 1+ 32*8, 2+ 32*8, 3+ 32*8, 4+ 32*8, 5+ 32*8, 6+ 32*8, 7+ 32*8 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32,
	  64*8 + 0*32, 64*8 + 1*32, 64*8 + 2*32, 64*8 + 3*32,
	  64*8 + 4*32, 64*8 + 5*32, 64*8 + 6*32, 64*8 + 7*32 },
	128*8	/* every sprite takes 128 consecutive bytes */
};

static struct GfxLayout charlayout =
{
	8,8,	/* 8*8 characters */
	RGN_FRAC(1,1),
	4,	/* 4 bits per pixel */
	{ 0, 1, 2, 3 },
	{ 0*4, 1*4, 2*4, 3*4, 4*4, 5*4, 6*4, 7*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8	/* every sprite takes 32 consecutive bytes */
};

static struct GfxLayout char2layout =
{
	8,8,	/* 8*8 characters */
	RGN_FRAC(1,1),
	2,	/* 2 bits per pixel */
	{ 0, 8 },	/* pixel bits separated */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16 },
	16*8	/* every sprite takes 32 consecutive bytes */
};

static struct GfxDecodeInfo darius_gfxdecodeinfo[] =
{
	{ REGION_GFX2, 0, &tilelayout,   0, 256 },	/* sprites */
	{ REGION_GFX1, 0, &charlayout,   0, 256 },	/* scr tiles */
	{ REGION_GFX3, 0, &char2layout,  0, 256 },	/* top layer scr tiles */
	{ -1 } /* end of array */
};


/**************************************************************
                        YM2203 (SOUND)
**************************************************************/

/* handler called by the YM2203 emulator when the internal timers cause an IRQ */
static void irqhandler(int irq)	/* assumes Z80 sandwiched between 68Ks */
{
	cpu_set_irq_line(1,0,irq ? ASSERT_LINE : CLEAR_LINE);
}

static struct YM2203interface ym2203_interface =
{
	2,			/* 2 chips */
	4000000,	/* 4 MHz ??? */
	{ YM2203_VOL(60,20), YM2203_VOL(60,20) },
	{ 0, 0 },		/* portA read */
	{ 0, 0 },
	{ darius_write_portA0, darius_write_portA1 },	/* portA write */
	{ darius_write_portB0, darius_write_portB1 },	/* portB write */
	{ irqhandler, 0 }
};


/***********************************************************
                       MACHINE DRIVERS
***********************************************************/

static MACHINE_DRIVER_START( darius )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68000,16000000/2)	/* 8 MHz ? */
	MDRV_CPU_MEMORY(darius_readmem,darius_writemem)
	MDRV_CPU_VBLANK_INT(irq4_line_hold,1)

	MDRV_CPU_ADD(Z80,8000000/2)
	MDRV_CPU_FLAGS(CPU_AUDIO_CPU)	/* 4 MHz ? */
	MDRV_CPU_MEMORY(darius_sound_readmem,darius_sound_writemem)

	MDRV_CPU_ADD(M68000,16000000/2)	/* 8 MHz ? */
	MDRV_CPU_MEMORY(darius_cpub_readmem,darius_cpub_writemem)
	MDRV_CPU_VBLANK_INT(irq4_line_hold,1)

	MDRV_CPU_ADD(Z80,8000000/2) /* 4 MHz ? */
	MDRV_CPU_FLAGS(CPU_AUDIO_CPU)	/* ADPCM player using MSM5205 */
	MDRV_CPU_MEMORY(darius_sound2_readmem,darius_sound2_writemem)
	MDRV_CPU_PORTS(darius_sound2_readport,darius_sound2_writeport)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)
	MDRV_INTERLEAVE(10)	/* 10 CPU slices per frame ? */

	MDRV_MACHINE_INIT(darius)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_DUAL_MONITOR)
	MDRV_ASPECT_RATIO(12,3)
	MDRV_SCREEN_SIZE(108*8, 32*8)
	MDRV_VISIBLE_AREA(0*8, 108*8-1, 1*8, 29*8-1)
	MDRV_GFXDECODE(darius_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(4096*2)

	MDRV_VIDEO_START(darius)
	MDRV_VIDEO_UPDATE(darius)

	/* sound hardware */
	MDRV_SOUND_ATTRIBUTES(SOUND_SUPPORTS_STEREO)
	MDRV_SOUND_ADD(YM2203, ym2203_interface)
	MDRV_SOUND_ADD(MSM5205, msm5205_interface)
MACHINE_DRIVER_END


/***************************************************************************
                                  DRIVERS
***************************************************************************/

ROM_START( darius )
	ROM_REGION( 0x60000, REGION_CPU1, 0 )	/* 68000 code */
	ROM_FILL( 0x00000, 0x60000, 0xffffffff )
	ROM_LOAD16_BYTE( "da-59.bin",   0x00000, 0x10000, CRC(11aab4eb) SHA1(92f795e96a940e8d94abbf429ba4ac119992b991) )
	ROM_LOAD16_BYTE( "da-58.bin",   0x00001, 0x10000, CRC(5f71e697) SHA1(bf959cf82e8e8ba950ab40d9c008ad5de01385aa) )
	ROM_LOAD16_BYTE( "da-61.bin",   0x20000, 0x10000, CRC(4736aa9b) SHA1(05e549d96a053e6b3bc34359267adcd73f98dd4a) )
	ROM_LOAD16_BYTE( "da-66.bin",   0x20001, 0x10000, CRC(4ede5f56) SHA1(88c06aef4b0a3e29fa30c24a57f2d3a05fc9f021) )
	ROM_LOAD16_BYTE( "a96_31.187",  0x40000, 0x10000, CRC(e9bb5d89) SHA1(a5d08129c32b97e2cce84496945766fd32b6506e) )	/* 2 data roms */
	ROM_LOAD16_BYTE( "a96_30.154",  0x40001, 0x10000, CRC(9eb5e127) SHA1(50e2fe5ec7f79ecf1fb5107298da13ef5ab37162) )

   	ROM_REGION( 0x30000, REGION_CPU2, 0 )	/* Z80 sound cpu */
	ROM_LOAD( "a96_57.33",  0x00000, 0x10000, CRC(33ceb730) SHA1(05070ea503ac57ff8445145d6f97115f7aad90a5) )

	ROM_REGION( 0x80000, REGION_CPU3, 0 )	/* 68000 code */
	ROM_LOAD16_BYTE( "a96_33-1.190", 0x00000, 0x10000, CRC(ff186048) SHA1(becb00d2cc69a6d4e839086bd3d902f4e6a99aa6) )
	ROM_LOAD16_BYTE( "a96_32-1.157", 0x00001, 0x10000, CRC(d9719de8) SHA1(9e907cfb5cbe6abebccfbd065d02e7a71c5aa494) )
	ROM_LOAD16_BYTE( "a96_35-1.191", 0x20000, 0x10000, CRC(b3280193) SHA1(f4bad066c16682f9267752c50a31ef64b312f11e) )
	ROM_LOAD16_BYTE( "a96_34-1.158", 0x20001, 0x10000, CRC(ca3b2573) SHA1(4da0d8536e546ea46b2374318e25c30305f4c977) )

	ROM_REGION( 0x10000, REGION_CPU4, 0 )	/* second Z80 driving the ADPCM chip */
	ROM_LOAD( "a96_56.18",      0x00000, 0x10000, CRC(292ef55c) SHA1(67bfe3693e43daece06d4795645d54cd66419e5b) )		/* Z80 prog + ADPCM samples */

	ROM_REGION( 0x60000, REGION_GFX1, ROMREGION_DISPOSE )
	/* There are THREE of each SCR gfx rom on the actual board,
	   making a complete set for every PC080SN tilemap chip */
	ROM_LOAD16_BYTE( "a96_48.24",    0x00000, 0x10000, CRC(39c9b3aa) SHA1(43a91d916c5a09207dfa37413feb5025636f37ae) )	/* 8x8 SCR tiles */
	ROM_LOAD16_BYTE( "a96_49.25",    0x20000, 0x10000, CRC(37a7d88a) SHA1(cede0d810d74ec460dcc4b391bb1acd5a669a7b4) )
	ROM_LOAD16_BYTE( "a96_50.26",    0x40000, 0x10000, CRC(75d738e4) SHA1(634606da46136ab605f5477af5639a20e39b44c4) )
	ROM_LOAD16_BYTE( "a96_51.47",    0x00001, 0x10000, CRC(1bf8f0d3) SHA1(7f36e69336260958282eb663fe71b56410f0ee42) )
	ROM_LOAD16_BYTE( "a96_52.48",    0x20001, 0x10000, CRC(2d9b2128) SHA1(9b72936fbd9dca6ef8302ac6c40a1cec019cebb5) )
	ROM_LOAD16_BYTE( "a96_53.49",    0x40001, 0x10000, CRC(0173484c) SHA1(41d70039bda0965afe89251696ceaec7b7f40c24) )

	ROM_REGION( 0xc0000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD32_BYTE( "a96_44.179",   0x00000, 0x10000, CRC(bbc18878) SHA1(7732ab2a3002f8b500615377dab42ac75451cb3b) )	/* 16x16 sprites */
	ROM_LOAD32_BYTE( "a96_45.200",   0x00001, 0x10000, CRC(616cdd8b) SHA1(74e0c483a68d984a689ea1381ed3a9da2f8a410a) )
	ROM_LOAD32_BYTE( "a96_46.180",   0x00002, 0x10000, CRC(fec35418) SHA1(f0f401c3634e91b81cb8484b7b03f350d382e889) )
	ROM_LOAD32_BYTE( "a96_47.201",   0x00003, 0x10000, CRC(8df9286a) SHA1(4a197e4c38d1750cc316b8710f4a0fef4316be14) )

	ROM_LOAD32_BYTE( "a96_40.177",   0x40000, 0x10000, CRC(b699a51e) SHA1(5fd751dd44618743dc8a3df04cf0a987753a868b) )
	ROM_LOAD32_BYTE( "a96_41.198",   0x40001, 0x10000, CRC(97128a3a) SHA1(257ddd1ba71e6beeaf18e0c5d7006d1d2b6a5edf) )
	ROM_LOAD32_BYTE( "a96_42.178",   0x40002, 0x10000, CRC(7f55ee0f) SHA1(d9ba7b8fbf59308a08613d67e92da6829f6b6db3) )
	ROM_LOAD32_BYTE( "a96_43.199",   0x40003, 0x10000, CRC(c7cad469) SHA1(dbd37aa10f12e4950f8ec6bcd7d150fa55e64742) )

	ROM_LOAD32_BYTE( "da-62.bin",    0x80000, 0x10000, CRC(9179862c) SHA1(be94c7d213a34baf82f974ee1092aba44b072623) )
	ROM_LOAD32_BYTE( "da-63.bin",    0x80001, 0x10000, CRC(fa19cfff) SHA1(58a3ae3270ebe5a162cd62df06da7199843707cf) )
	ROM_LOAD32_BYTE( "da-64.bin",    0x80002, 0x10000, CRC(814c676f) SHA1(a6a64e65a3c163ecfede14b48ea70c20050248c3) )
	ROM_LOAD32_BYTE( "da-65.bin",    0x80003, 0x10000, CRC(14eee326) SHA1(41760fada2a5e34ee6c9250af927baf650d9cfc4) )

 	ROM_REGION( 0x8000, REGION_GFX3, ROMREGION_DISPOSE )			/* 8x8 SCR tiles */
	/* There's only one of each of these on a real board */
	ROM_LOAD16_BYTE( "a96_54.143",   0x0000, 0x4000, CRC(51c02ae2) SHA1(27d2a6c649d047da1f22758569cb36531e3bf8bc) )
	ROM_LOAD16_BYTE( "a96_55.144",   0x0001, 0x4000, CRC(771e4d98) SHA1(0e8ce5d569775883f4bc777b9bd49eb23ba7b42e) )

 	ROM_REGION( 0x1000, REGION_USER1, 0 )
	ROM_LOAD16_BYTE( "a96-24.163",   0x0000, 0x0400, CRC(0fa8be7f) SHA1(079686b5d65b4b966591090d8c0e13e66dc5beca) )	/* proms, currently unused */
	ROM_LOAD16_BYTE( "a96-25.164",   0x0400, 0x0400, CRC(265508a6) SHA1(f8ee1c658b33ae76d8a457a4042d9b4b58247823) )
	ROM_LOAD16_BYTE( "a96-26.165",   0x0800, 0x0400, CRC(4891b9c0) SHA1(1f550a9a4ad3ca379f88f5865ed1b281c7b87f31) )
ROM_END

ROM_START( dariusj )
	ROM_REGION( 0x60000, REGION_CPU1, 0 )	/* 68000 code */
	ROM_FILL( 0x00000, 0x60000, 0xffffffff )
	ROM_LOAD16_BYTE( "a96_29-1.185", 0x00000, 0x10000, CRC(75486f62) SHA1(818b095f2c6cc5764161c3e14ba70fe1c4b2f724) )
	ROM_LOAD16_BYTE( "a96_28-1.152", 0x00001, 0x10000, CRC(fb34d400) SHA1(b14517384f5eadca8b73833bcd81374614b928d4) )
	/* middle area is empty */
	ROM_LOAD16_BYTE( "a96_31.187",   0x40000, 0x10000, CRC(e9bb5d89) SHA1(a5d08129c32b97e2cce84496945766fd32b6506e) )	/* 2 data roms */
	ROM_LOAD16_BYTE( "a96_30.154",   0x40001, 0x10000, CRC(9eb5e127) SHA1(50e2fe5ec7f79ecf1fb5107298da13ef5ab37162) )

   	ROM_REGION( 0x30000, REGION_CPU2, 0 )	/* Z80 sound cpu */
	ROM_LOAD( "a96_57.33",  0x00000, 0x10000, CRC(33ceb730) SHA1(05070ea503ac57ff8445145d6f97115f7aad90a5) )

	ROM_REGION( 0x80000, REGION_CPU3, 0 )	/* 68000 code */
	ROM_LOAD16_BYTE( "a96_33-1.190", 0x00000, 0x10000, CRC(ff186048) SHA1(becb00d2cc69a6d4e839086bd3d902f4e6a99aa6) )
	ROM_LOAD16_BYTE( "a96_32-1.157", 0x00001, 0x10000, CRC(d9719de8) SHA1(9e907cfb5cbe6abebccfbd065d02e7a71c5aa494) )
	ROM_LOAD16_BYTE( "a96_35-1.191", 0x20000, 0x10000, CRC(b3280193) SHA1(f4bad066c16682f9267752c50a31ef64b312f11e) )
	ROM_LOAD16_BYTE( "a96_34-1.158", 0x20001, 0x10000, CRC(ca3b2573) SHA1(4da0d8536e546ea46b2374318e25c30305f4c977) )

	ROM_REGION( 0x10000, REGION_CPU4, 0 )	/* second Z80 driving the ADPCM chip */
	ROM_LOAD( "a96_56.18",      0x00000, 0x10000, CRC(292ef55c) SHA1(67bfe3693e43daece06d4795645d54cd66419e5b) )		/* Z80 prog + ADPCM samples */

	ROM_REGION( 0x60000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "a96_48.24",    0x00000, 0x10000, CRC(39c9b3aa) SHA1(43a91d916c5a09207dfa37413feb5025636f37ae) )	/* 8x8 SCR tiles */
	ROM_LOAD16_BYTE( "a96_49.25",    0x20000, 0x10000, CRC(37a7d88a) SHA1(cede0d810d74ec460dcc4b391bb1acd5a669a7b4) )
	ROM_LOAD16_BYTE( "a96_50.26",    0x40000, 0x10000, CRC(75d738e4) SHA1(634606da46136ab605f5477af5639a20e39b44c4) )
	ROM_LOAD16_BYTE( "a96_51.47",    0x00001, 0x10000, CRC(1bf8f0d3) SHA1(7f36e69336260958282eb663fe71b56410f0ee42) )
	ROM_LOAD16_BYTE( "a96_52.48",    0x20001, 0x10000, CRC(2d9b2128) SHA1(9b72936fbd9dca6ef8302ac6c40a1cec019cebb5) )
	ROM_LOAD16_BYTE( "a96_53.49",    0x40001, 0x10000, CRC(0173484c) SHA1(41d70039bda0965afe89251696ceaec7b7f40c24) )

	ROM_REGION( 0xc0000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD32_BYTE( "a96_44.179",   0x00000, 0x10000, CRC(bbc18878) SHA1(7732ab2a3002f8b500615377dab42ac75451cb3b) )	/* 16x16 sprites */
	ROM_LOAD32_BYTE( "a96_45.200",   0x00001, 0x10000, CRC(616cdd8b) SHA1(74e0c483a68d984a689ea1381ed3a9da2f8a410a) )
	ROM_LOAD32_BYTE( "a96_46.180",   0x00002, 0x10000, CRC(fec35418) SHA1(f0f401c3634e91b81cb8484b7b03f350d382e889) )
	ROM_LOAD32_BYTE( "a96_47.201",   0x00003, 0x10000, CRC(8df9286a) SHA1(4a197e4c38d1750cc316b8710f4a0fef4316be14) )

	ROM_LOAD32_BYTE( "a96_40.177",   0x40000, 0x10000, CRC(b699a51e) SHA1(5fd751dd44618743dc8a3df04cf0a987753a868b) )
	ROM_LOAD32_BYTE( "a96_41.198",   0x40001, 0x10000, CRC(97128a3a) SHA1(257ddd1ba71e6beeaf18e0c5d7006d1d2b6a5edf) )
	ROM_LOAD32_BYTE( "a96_42.178",   0x40002, 0x10000, CRC(7f55ee0f) SHA1(d9ba7b8fbf59308a08613d67e92da6829f6b6db3) )
	ROM_LOAD32_BYTE( "a96_43.199",   0x40003, 0x10000, CRC(c7cad469) SHA1(dbd37aa10f12e4950f8ec6bcd7d150fa55e64742) )

	ROM_LOAD32_BYTE( "a96_36.175",   0x80000, 0x10000, CRC(af598141) SHA1(f3b888bcbd4560cca48187055cbe4107e2b392a6) )
	ROM_LOAD32_BYTE( "a96_37.196",   0x80001, 0x10000, CRC(b48137c8) SHA1(03e98a93f4fa19dfe77da244c002abc84b936a22) )
	ROM_LOAD32_BYTE( "a96_38.176",   0x80002, 0x10000, CRC(e4f3e3a7) SHA1(0baa8a672516bcc4f17f40f429ac3d227de16625) )
	ROM_LOAD32_BYTE( "a96_39.197",   0x80003, 0x10000, CRC(ea30920f) SHA1(91d47b10886d6c243bc676435e300cb3b5fcca33) )

 	ROM_REGION( 0x8000, REGION_GFX3, ROMREGION_DISPOSE )			/* 8x8 SCR tiles */
	ROM_LOAD16_BYTE( "a96_54.143",   0x0000, 0x4000, CRC(51c02ae2) SHA1(27d2a6c649d047da1f22758569cb36531e3bf8bc) )
	ROM_LOAD16_BYTE( "a96_55.144",   0x0001, 0x4000, CRC(771e4d98) SHA1(0e8ce5d569775883f4bc777b9bd49eb23ba7b42e) )

 	ROM_REGION( 0x1000, REGION_USER1, 0 )
	ROM_LOAD16_BYTE( "a96-24.163",   0x0000, 0x0400, CRC(0fa8be7f) SHA1(079686b5d65b4b966591090d8c0e13e66dc5beca) )	/* proms, currently unused */
	ROM_LOAD16_BYTE( "a96-25.164",   0x0400, 0x0400, CRC(265508a6) SHA1(f8ee1c658b33ae76d8a457a4042d9b4b58247823) )
	ROM_LOAD16_BYTE( "a96-26.165",   0x0800, 0x0400, CRC(4891b9c0) SHA1(1f550a9a4ad3ca379f88f5865ed1b281c7b87f31) )
ROM_END

ROM_START( dariuso )
	ROM_REGION( 0x60000, REGION_CPU1, 0 )	/* 68000 code */
	ROM_FILL( 0x00000, 0x60000, 0xffffffff )
	ROM_LOAD16_BYTE( "a96-29.185",   0x00000, 0x10000, CRC(f775162b) SHA1(a17e570c2ba4daf0a3526b45c324c822faac0c8d) )
	ROM_LOAD16_BYTE( "a96-28.152",   0x00001, 0x10000, CRC(4721d667) SHA1(fa9a109054a818f836452215204ce91f2b166ddb) )
	/* middle area is empty */
	ROM_LOAD16_BYTE( "a96_31.187",   0x40000, 0x10000, CRC(e9bb5d89) SHA1(a5d08129c32b97e2cce84496945766fd32b6506e) )	/* 2 data roms */
	ROM_LOAD16_BYTE( "a96_30.154",   0x40001, 0x10000, CRC(9eb5e127) SHA1(50e2fe5ec7f79ecf1fb5107298da13ef5ab37162) )

   	ROM_REGION( 0x30000, REGION_CPU2, 0 )	/* Z80 sound cpu */
	ROM_LOAD( "a96_57.33",  0x00000, 0x10000, CRC(33ceb730) SHA1(05070ea503ac57ff8445145d6f97115f7aad90a5) )

	ROM_REGION( 0x80000, REGION_CPU3, 0 )	/* 68000 code */
	ROM_LOAD16_BYTE( "a96-33.190",   0x00000, 0x10000, CRC(d2f340d2) SHA1(d9175bf4dda5707afb3c57d3b6affe0305084c71) )
	ROM_LOAD16_BYTE( "a96-32.157",   0x00001, 0x10000, CRC(044c9848) SHA1(5293e9e83fd38d0d14e4f3b3a342d88e27ee44d6) )
	ROM_LOAD16_BYTE( "a96-35.191",   0x20000, 0x10000, CRC(b8ed718b) SHA1(8951f9c3c971c5621ec98b63fb27d44f30304c70) )
	ROM_LOAD16_BYTE( "a96-34.158",   0x20001, 0x10000, CRC(7556a660) SHA1(eaa82f3e1f827616ff25e22673d6d2ee54f0ad4c) )

	ROM_REGION( 0x10000, REGION_CPU4, 0 )	/* second Z80 driving the ADPCM chip */
	ROM_LOAD( "a96_56.18",      0x00000, 0x10000, CRC(292ef55c) SHA1(67bfe3693e43daece06d4795645d54cd66419e5b) )		/* Z80 prog + ADPCM samples */

	ROM_REGION( 0x60000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "a96_48.24",    0x00000, 0x10000, CRC(39c9b3aa) SHA1(43a91d916c5a09207dfa37413feb5025636f37ae) )	/* 8x8 SCR tiles */
	ROM_LOAD16_BYTE( "a96_49.25",    0x20000, 0x10000, CRC(37a7d88a) SHA1(cede0d810d74ec460dcc4b391bb1acd5a669a7b4) )
	ROM_LOAD16_BYTE( "a96_50.26",    0x40000, 0x10000, CRC(75d738e4) SHA1(634606da46136ab605f5477af5639a20e39b44c4) )
	ROM_LOAD16_BYTE( "a96_51.47",    0x00001, 0x10000, CRC(1bf8f0d3) SHA1(7f36e69336260958282eb663fe71b56410f0ee42) )
	ROM_LOAD16_BYTE( "a96_52.48",    0x20001, 0x10000, CRC(2d9b2128) SHA1(9b72936fbd9dca6ef8302ac6c40a1cec019cebb5) )
	ROM_LOAD16_BYTE( "a96_53.49",    0x40001, 0x10000, CRC(0173484c) SHA1(41d70039bda0965afe89251696ceaec7b7f40c24) )

	ROM_REGION( 0xc0000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD32_BYTE( "a96_44.179",   0x00000, 0x10000, CRC(bbc18878) SHA1(7732ab2a3002f8b500615377dab42ac75451cb3b) )	/* 16x16 sprites */
	ROM_LOAD32_BYTE( "a96_45.200",   0x00001, 0x10000, CRC(616cdd8b) SHA1(74e0c483a68d984a689ea1381ed3a9da2f8a410a) )
	ROM_LOAD32_BYTE( "a96_46.180",   0x00002, 0x10000, CRC(fec35418) SHA1(f0f401c3634e91b81cb8484b7b03f350d382e889) )
	ROM_LOAD32_BYTE( "a96_47.201",   0x00003, 0x10000, CRC(8df9286a) SHA1(4a197e4c38d1750cc316b8710f4a0fef4316be14) )

	ROM_LOAD32_BYTE( "a96_40.177",   0x40000, 0x10000, CRC(b699a51e) SHA1(5fd751dd44618743dc8a3df04cf0a987753a868b) )
	ROM_LOAD32_BYTE( "a96_41.198",   0x40001, 0x10000, CRC(97128a3a) SHA1(257ddd1ba71e6beeaf18e0c5d7006d1d2b6a5edf) )
	ROM_LOAD32_BYTE( "a96_42.178",   0x40002, 0x10000, CRC(7f55ee0f) SHA1(d9ba7b8fbf59308a08613d67e92da6829f6b6db3) )
	ROM_LOAD32_BYTE( "a96_43.199",   0x40003, 0x10000, CRC(c7cad469) SHA1(dbd37aa10f12e4950f8ec6bcd7d150fa55e64742) )

	ROM_LOAD32_BYTE( "a96_36.175",   0x80000, 0x10000, CRC(af598141) SHA1(f3b888bcbd4560cca48187055cbe4107e2b392a6) )
	ROM_LOAD32_BYTE( "a96_37.196",   0x80001, 0x10000, CRC(b48137c8) SHA1(03e98a93f4fa19dfe77da244c002abc84b936a22) )
	ROM_LOAD32_BYTE( "a96_38.176",   0x80002, 0x10000, CRC(e4f3e3a7) SHA1(0baa8a672516bcc4f17f40f429ac3d227de16625) )
	ROM_LOAD32_BYTE( "a96_39.197",   0x80003, 0x10000, CRC(ea30920f) SHA1(91d47b10886d6c243bc676435e300cb3b5fcca33) )

 	ROM_REGION( 0x8000, REGION_GFX3, ROMREGION_DISPOSE )			/* 8x8 SCR tiles */
	ROM_LOAD16_BYTE( "a96_54.143",   0x0000, 0x4000, CRC(51c02ae2) SHA1(27d2a6c649d047da1f22758569cb36531e3bf8bc) )
	ROM_LOAD16_BYTE( "a96_55.144",   0x0001, 0x4000, CRC(771e4d98) SHA1(0e8ce5d569775883f4bc777b9bd49eb23ba7b42e) )

 	ROM_REGION( 0x1000, REGION_USER1, 0 )
	ROM_LOAD16_BYTE( "a96-24.163",   0x0000, 0x0400, CRC(0fa8be7f) SHA1(079686b5d65b4b966591090d8c0e13e66dc5beca) )	/* proms, currently unused */
	ROM_LOAD16_BYTE( "a96-25.164",   0x0400, 0x0400, CRC(265508a6) SHA1(f8ee1c658b33ae76d8a457a4042d9b4b58247823) )
	ROM_LOAD16_BYTE( "a96-26.165",   0x0800, 0x0400, CRC(4891b9c0) SHA1(1f550a9a4ad3ca379f88f5865ed1b281c7b87f31) )
ROM_END

ROM_START( dariuse )
	ROM_REGION( 0x60000, REGION_CPU1, 0 )	/* 68000 code */
	ROM_FILL( 0x00000, 0x60000, 0xffffffff )
	ROM_LOAD16_BYTE( "dae-68.bin",   0x00000, 0x10000, CRC(ed721127) SHA1(8127f4a9b26b5fb83a381235eef0577d60d1cfd7) )
	ROM_LOAD16_BYTE( "dae-67.bin",   0x00001, 0x10000, CRC(b99aea8c) SHA1(859ada7c472ab2ac308faa775066e79ed1f4ad71) )
	/* middle area is empty */
	ROM_LOAD16_BYTE( "dae-70.bin",   0x40000, 0x10000, CRC(54590b31) SHA1(2b89846f14a5cb19b58ab4999bc5ae11671bbb5a) )	/* 2 data roms */
	ROM_LOAD16_BYTE( "a96_30.154",   0x40001, 0x10000, CRC(9eb5e127) SHA1(50e2fe5ec7f79ecf1fb5107298da13ef5ab37162) )	// dae-69.bin

   	ROM_REGION( 0x30000, REGION_CPU2, 0 )	/* Z80 sound cpu */
	ROM_LOAD( "a96_57.33",  0x00000, 0x10000, CRC(33ceb730) SHA1(05070ea503ac57ff8445145d6f97115f7aad90a5) )

	ROM_REGION( 0x80000, REGION_CPU3, 0 )	/* 68000 code */
	ROM_LOAD16_BYTE( "dae-72.bin",   0x00000, 0x10000, CRC(248ca2cc) SHA1(43b29146d8e2c62dd1fb7dc842fd441a360f2453) )
	ROM_LOAD16_BYTE( "dae-71.bin",   0x00001, 0x10000, CRC(65dd0403) SHA1(8036c35ce5df0727cccb9ece3bfac9577160d4fd) )
	ROM_LOAD16_BYTE( "dae-74.bin",   0x20000, 0x10000, CRC(0ea31f60) SHA1(c9e7eaf8bf3abbef944b7de407d5d5ddaac93e31) )
	ROM_LOAD16_BYTE( "dae-73.bin",   0x20001, 0x10000, CRC(27036a4d) SHA1(426dccb8f559d39460c97bfd4354c74a59af172e) )

	ROM_REGION( 0x10000, REGION_CPU4, 0 )	/* second Z80 driving the ADPCM chip */
	ROM_LOAD( "a96_56.18",      0x00000, 0x10000, CRC(292ef55c) SHA1(67bfe3693e43daece06d4795645d54cd66419e5b) )		/* Z80 prog + ADPCM samples */

	ROM_REGION( 0x60000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "a96_48.24",    0x00000, 0x10000, CRC(39c9b3aa) SHA1(43a91d916c5a09207dfa37413feb5025636f37ae) )	/* 8x8 SCR tiles */
	ROM_LOAD16_BYTE( "a96_49.25",    0x20000, 0x10000, CRC(37a7d88a) SHA1(cede0d810d74ec460dcc4b391bb1acd5a669a7b4) )
	ROM_LOAD16_BYTE( "a96_50.26",    0x40000, 0x10000, CRC(75d738e4) SHA1(634606da46136ab605f5477af5639a20e39b44c4) )
	ROM_LOAD16_BYTE( "a96_51.47",    0x00001, 0x10000, CRC(1bf8f0d3) SHA1(7f36e69336260958282eb663fe71b56410f0ee42) )
	ROM_LOAD16_BYTE( "a96_52.48",    0x20001, 0x10000, CRC(2d9b2128) SHA1(9b72936fbd9dca6ef8302ac6c40a1cec019cebb5) )
	ROM_LOAD16_BYTE( "a96_53.49",    0x40001, 0x10000, CRC(0173484c) SHA1(41d70039bda0965afe89251696ceaec7b7f40c24) )

	ROM_REGION( 0xc0000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD32_BYTE( "a96_44.179",   0x00000, 0x10000, CRC(bbc18878) SHA1(7732ab2a3002f8b500615377dab42ac75451cb3b) )	/* 16x16 sprites */
	ROM_LOAD32_BYTE( "a96_45.200",   0x00001, 0x10000, CRC(616cdd8b) SHA1(74e0c483a68d984a689ea1381ed3a9da2f8a410a) )
	ROM_LOAD32_BYTE( "a96_46.180",   0x00002, 0x10000, CRC(fec35418) SHA1(f0f401c3634e91b81cb8484b7b03f350d382e889) )
	ROM_LOAD32_BYTE( "a96_47.201",   0x00003, 0x10000, CRC(8df9286a) SHA1(4a197e4c38d1750cc316b8710f4a0fef4316be14) )

	ROM_LOAD32_BYTE( "a96_40.177",   0x40000, 0x10000, CRC(b699a51e) SHA1(5fd751dd44618743dc8a3df04cf0a987753a868b) )
	ROM_LOAD32_BYTE( "a96_41.198",   0x40001, 0x10000, CRC(97128a3a) SHA1(257ddd1ba71e6beeaf18e0c5d7006d1d2b6a5edf) )
	ROM_LOAD32_BYTE( "a96_42.178",   0x40002, 0x10000, CRC(7f55ee0f) SHA1(d9ba7b8fbf59308a08613d67e92da6829f6b6db3) )
	ROM_LOAD32_BYTE( "a96_43.199",   0x40003, 0x10000, CRC(c7cad469) SHA1(dbd37aa10f12e4950f8ec6bcd7d150fa55e64742) )

	ROM_LOAD32_BYTE( "a96_36.175",   0x80000, 0x10000, CRC(af598141) SHA1(f3b888bcbd4560cca48187055cbe4107e2b392a6) )
	ROM_LOAD32_BYTE( "a96_37.196",   0x80001, 0x10000, CRC(b48137c8) SHA1(03e98a93f4fa19dfe77da244c002abc84b936a22) )
	ROM_LOAD32_BYTE( "a96_38.176",   0x80002, 0x10000, CRC(e4f3e3a7) SHA1(0baa8a672516bcc4f17f40f429ac3d227de16625) )
	ROM_LOAD32_BYTE( "a96_39.197",   0x80003, 0x10000, CRC(ea30920f) SHA1(91d47b10886d6c243bc676435e300cb3b5fcca33) )

 	ROM_REGION( 0x8000, REGION_GFX3, ROMREGION_DISPOSE )			/* 8x8 SCR tiles */
	ROM_LOAD16_BYTE( "a96_54.143",   0x0000, 0x4000, CRC(51c02ae2) SHA1(27d2a6c649d047da1f22758569cb36531e3bf8bc) )
	ROM_LOAD16_BYTE( "a96_55.144",   0x0001, 0x4000, CRC(771e4d98) SHA1(0e8ce5d569775883f4bc777b9bd49eb23ba7b42e) )

 	ROM_REGION( 0x1000, REGION_USER1, 0 )
	ROM_LOAD16_BYTE( "a96-24.163",   0x0000, 0x0400, CRC(0fa8be7f) SHA1(079686b5d65b4b966591090d8c0e13e66dc5beca) )	/* proms, currently unused */
	ROM_LOAD16_BYTE( "a96-25.164",   0x0400, 0x0400, CRC(265508a6) SHA1(f8ee1c658b33ae76d8a457a4042d9b4b58247823) )
	ROM_LOAD16_BYTE( "a96-26.165",   0x0800, 0x0400, CRC(4891b9c0) SHA1(1f550a9a4ad3ca379f88f5865ed1b281c7b87f31) )
ROM_END


static DRIVER_INIT( darius )
{
//	taitosnd_setz80_soundcpu( 2 );

	cpua_ctrl = 0xff;
	state_save_register_UINT16("main1", 0, "control", &cpua_ctrl, 1);
	state_save_register_func_postload(parse_control);

	banknum = -1;
	// (there are other sound vars that may need saving too) //
	state_save_register_int("sound1", 0, "sound region", &banknum);
	state_save_register_int("sound2", 0, "sound region", &adpcm_command);
	state_save_register_int("sound3", 0, "sound region", &nmi_enable);
	state_save_register_func_postload(reset_sound_region);
}


MACHINE_INIT( darius )
{
	int  i;

	/**** setup sound bank image ****/
	unsigned char *RAM = memory_region(REGION_CPU2);

	for( i = 3; i >= 0; i-- ){
		memcpy( RAM + 0x8000*i + 0x10000, RAM,            0x4000 );
		memcpy( RAM + 0x8000*i + 0x14000, RAM + 0x4000*i, 0x4000 );
	}

	mixer_sound_enable_global_w( 1 );	/* mixer enabled */

	for( i = 0; i < DARIUS_VOL_MAX; i++ ){
		darius_vol[i] = 0x00;	/* min volume */
	}
	for( i = 0; i < DARIUS_PAN_MAX; i++ ){
		darius_pan[i] = 0x80;	/* center */
	}
	for( i = 0; i < 0x10; i++ ){
		//logerror( "calc %d = %d\n", i, (int)(100.0f / (float)pow(10.0f, (32.0f - (i * (32.0f / (float)(0xf)))) / 20.0f)) );
		darius_def_vol[i] = (int)(100.0f / (float)pow(10.0f, (32.0f - (i * (32.0f / (float)(0xf)))) / 20.0f));
	}
}


GAME( 1986, darius,   0,        darius,   darius,   darius,   ROT0, "Taito Corporation Japan", "Darius (World)" )
GAME( 1986, dariusj,  darius,   darius,   dariusj,  darius,   ROT0, "Taito Corporation", "Darius (Japan)" )
GAME( 1986, dariuso,  darius,   darius,   dariusj,  darius,   ROT0, "Taito Corporation", "Darius (Japan old version)" )
GAME( 1986, dariuse,  darius,   darius,   dariuse,  darius,   ROT0, "Taito Corporation", "Darius (Extra) (Japan)" )
