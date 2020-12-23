/***************************************************************************

Renegade
(c)1986 Taito

Nekketsu Kouha Kunio Kun
(c)1986 Technos Japan

Nekketsu Kouha Kunio Kun (bootleg)

driver by Phil Stroffolino, Carlos A. Lozano, Rob Rosenbrock

to enter test mode, hold down P1+P2 and press reset

NMI is used to refresh the sprites
IRQ is used to handle coin inputs

Known issues:
- coin counter isn't working properly (interrupt related?)

Memory Map (Preliminary):

Working RAM
  $24			used to mirror bankswitch state
  $25			coin trigger state
  $26			#credits (decimal)
  $27 -  $28	partial credits
  $2C -  $2D	sprite refresh trigger (used by NMI)
  $31			live/demo (if live, player controls are read from input ports)
  $32			indicates 2 player (alternating) game, or 1 player game
  $33			active player
  $37			stage number
  $38			stage state (for stages with more than one part)
  $40			game status flags; 0x80 indicates time over, 0x40 indicates player dead
 $220			player health
 $222 -  $223	stage timer
 $48a -  $48b	horizontal scroll buffer
 $511 -  $690	sprite RAM buffer
 $693			num pending sound commands
 $694 -  $698	sound command queue

$1002			#lives
$1014 - $1015	stage timer - separated digits
$1017 - $1019	stage timer: (ticks,seconds,minutes)
$101a			timer for palette animation
$1020 - $1048	high score table
$10e5 - $10ff	68705 data buffer

Video RAM
$1800 - $1bff	text layer, characters
$1c00 - $1fff	text layer, character attributes
$2000 - $217f	MIX RAM (96 sprites)
$2800 - $2bff	BACK LOW MAP RAM (background tiles)
$2C00 - $2fff	BACK HIGH MAP RAM (background attributes)
$3000 - $30ff	COLOR RG RAM
$3100 - $31ff	COLOR B RAM

Registers
$3800w	scroll(0ff)
$3801w	scroll(300)
$3802w	sound command
$3803w	screen flip (0=flip; 1=noflip)

$3804w	send data to 68705
$3804r	receive data from 68705

$3805w	bankswitch
$3806w	watchdog?
$3807w	coin counter

$3800r	'player1'
		xx		start buttons
		  xx		fire buttons
			xxxx	joystick state

$3801r	'player2'
		xx		coin inputs
		  xx		fire buttons
			xxxx	joystick state

$3802r	'DIP2'
		x		unused?
		 x		vblank
		  x 	0: 68705 is ready to send information
		   x		1: 68705 is ready to receive information
			xx		3rd fire buttons for player 2,1
			  xx	difficulty

$3803r 'DIP1'
		x		screen flip
		 x		cabinet type
		  x 	bonus (extra life for high score)
		   x		starting lives: 1 or 2
			xxxx	coins per play

ROM
$4000 - $7fff	bankswitched ROM
$8000 - $ffff	ROM

***************************************************************************/

#include "driver.h"
#include "vidhrdw/generic.h"
#include "cpu/m6502/m6502.h"
#include "cpu/m6809/m6809.h"

extern VIDEO_UPDATE( renegade );
extern VIDEO_START( renegade );
WRITE_HANDLER( renegade_scroll0_w );
WRITE_HANDLER( renegade_scroll1_w );
WRITE_HANDLER( renegade_videoram_w );
WRITE_HANDLER( renegade_videoram2_w );
WRITE_HANDLER( renegade_flipscreen_w );

extern UINT8 *renegade_videoram2;

/********************************************************************************************/

static WRITE_HANDLER( adpcm_play_w )
{
	int offs;
	int len;

	offs = (data - 0x2c) * 0x2000;
	len = 0x2000*2;

	/* kludge to avoid reading past end of ROM */
	if (offs + len > 0x20000)
		len = 0x1000;

	if (offs >= 0 && offs+len <= 0x20000)
		ADPCM_play(0,offs,len);
	else
		logerror("out of range adpcm command: 0x%02x\n",data);
}

static WRITE_HANDLER( sound_w )
{
	soundlatch_w(offset,data);
	cpu_set_irq_line(1,M6809_IRQ_LINE,HOLD_LINE);
}

/********************************************************************************************/
/*	MCU Simulation
**
**	Renegade and Nekketsu Kouha Kunio Kun MCU behaviors are identical,
**	except for the initial MCU status byte, and command encryption table.
*/

static int mcu_type;
static const UINT8 *mcu_encrypt_table;
static int mcu_encrypt_table_len;

static const UINT8 renegade_xor_table[0x37] = {
	0x8A,0x48,0x98,0x48,0xA9,0x00,0x85,0x14,0x85,0x15,0xA5,0x11,0x05,0x10,0xF0,0x21,
	0x46,0x11,0x66,0x10,0x90,0x0F,0x18,0xA5,0x14,0x65,0x12,0x85,0x14,0xA5,0x15,0x65,
	0x13,0x85,0x15,0xB0,0x06,0x06,0x12,0x26,0x13,0x90,0xDF,0x68,0xA8,0x68,0xAA,0x38,
	0x60,0x68,0xA8,0x68,0xAA,0x18,0x60
};

static const UINT8 kuniokun_xor_table[0x2a] = {
	0x48,0x8a,0x48,0xa5,0x01,0x48,0xa9,0x00,0x85,0x01,0xa2,0x10,0x26,0x10,0x26,0x11,
	0x26,0x01,0xa5,0x01,0xc5,0x00,0x90,0x04,0xe5,0x00,0x85,0x01,0x26,0x10,0x26,0x11,
	0xca,0xd0,0xed,0x68,0x85,0x01,0x68,0xaa,0x68,0x60
};

DRIVER_INIT( kuniokun )
{
	mcu_type = 0x85;
	mcu_encrypt_table = kuniokun_xor_table;
	mcu_encrypt_table_len = 0x2a;
}
DRIVER_INIT( renegade )
{
	mcu_type = 0xda;
	mcu_encrypt_table = renegade_xor_table;
	mcu_encrypt_table_len = 0x37;
}

#define MCU_BUFFER_MAX 6
static UINT8 mcu_buffer[MCU_BUFFER_MAX];
static size_t mcu_input_size;
static int mcu_output_byte;
static int mcu_key;

static READ_HANDLER( mcu_reset_r )
{
	mcu_key = -1;
	mcu_input_size = 0;
	mcu_output_byte = 0;
	return 0;
}

static WRITE_HANDLER( mcu_w )
{
	mcu_output_byte = 0;

	if( mcu_key<0 ){
		mcu_key = 0;
		mcu_input_size = 1;
		mcu_buffer[0] = data;
	}
	else {
		data ^= mcu_encrypt_table[mcu_key];
		if( ++mcu_key==mcu_encrypt_table_len) mcu_key = 0;
		if( mcu_input_size<MCU_BUFFER_MAX ) mcu_buffer[mcu_input_size++] = data;
	}
}

static void mcu_process_command( void )
{
	mcu_input_size = 0;
	mcu_output_byte = 0;

	switch( mcu_buffer[0] ){
		case 0x10:
		mcu_buffer[0] = mcu_type;
		break;

		case 0x26: /* sound code -> sound command */
		{
			int sound_code = mcu_buffer[1];
			static const UINT8 sound_command_table[256] = {
				0xa0,0xa1,0xa2,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,
				0x8d,0x8e,0x8f,0x97,0x96,0x9b,0x9a,0x95,0x9e,0x98,0x90,0x93,0x9d,0x9c,0xa3,0x91,
				0x9f,0x99,0xa6,0xae,0x94,0xa5,0xa4,0xa7,0x92,0xab,0xac,0xb0,0xb1,0xb2,0xb3,0xb4,
				0xb5,0xb6,0xb7,0xb8,0xb9,0xba,0xbb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x20,
				0x50,0x50,0x90,0x30,0x30,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x00,0x00,0x80,0xa0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x40,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x20,0x00,0x00,0x10,0x10,0x00,0x00,0x90,0x30,0x30,0x30,0xb0,0xb0,0xb0,0xb0,0xf0,
				0xf0,0xf0,0xf0,0xd0,0xf0,0x00,0x00,0x00,0x00,0x10,0x10,0x50,0x30,0xb0,0xb0,0xf0,
				0xf0,0xf0,0xf0,0xf0,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x10,
				0x10,0x10,0x30,0x30,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
				0x00,0x00,0x00,0x00,0x00,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,
				0x0f,0x0f,0x0f,0x0f,0x0f,0x8f,0x8f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,0x0f,
				0x0f,0x0f,0x0f,0x0f,0x0f,0xff,0xff,0xff,0xef,0xef,0xcf,0x8f,0x8f,0x0f,0x0f,0x0f
			};
			mcu_buffer[0] = 1;
			mcu_buffer[1] = sound_command_table[sound_code];
		}
		break;

		case 0x33: /* joy bits -> joy dir */
		{
			int joy_bits = mcu_buffer[2];
			static const UINT8 joy_table[0x10] = {
				0,3,7,0,1,2,8,0,5,4,6,0,0,0,0,0
			};
			mcu_buffer[0] = 1;
			mcu_buffer[1] = joy_table[joy_bits&0xf];
		}
		break;

		case 0x44: /* 0x44,0xff,DSW2,stage# -> difficulty */
		{
			int difficulty = mcu_buffer[2]&0x3;
			int stage = mcu_buffer[3];
			const UINT8 difficulty_table[4] = { 5,3,1,2 };
			int result = difficulty_table[difficulty];
			if( stage==0 ) result--;
			result += stage/4;
			mcu_buffer[0] = 1;
			mcu_buffer[1] = result;
		}

		case 0x55: /* 0x55,0x00,0x00,0x00,DSW2 -> timer */
		{
			int difficulty = mcu_buffer[4]&0x3;
			const UINT16 table[4] = {
				0x4001,0x5001,0x1502,0x0002
			};

			mcu_buffer[0] = 3;
			mcu_buffer[2] = table[difficulty]>>8;
			mcu_buffer[3] = table[difficulty]&0xff;
		}
		break;

		case 0x41: { /* 0x41,0x00,0x00,stage# -> ? */
//			int stage = mcu_buffer[3];
			mcu_buffer[0] = 2;
			mcu_buffer[1] = 0x20;
			mcu_buffer[2] = 0x78;
		}
		break;

		case 0x40: /* 0x40,0x00,difficulty,enemy_type -> enemy health */
		{
			int difficulty = mcu_buffer[2];
			int enemy_type = mcu_buffer[3];
			int health;

			if( enemy_type<=4 || (enemy_type&1)==0 ) health = 0x18 + difficulty*8;
			else health = 0x06 + difficulty*2;
			logerror("e_type:0x%02x diff:0x%02x -> 0x%02x\n", enemy_type, difficulty, health );
			mcu_buffer[0] = 1;
			mcu_buffer[1] = health;
		}
		break;

		case 0x42: /* 0x42,0x00,stage#,character# -> enemy_type */
		{
			int stage = mcu_buffer[2]&0x3;
			int indx = mcu_buffer[3];
			int enemy_type=0;

			if( indx==0 ){
				enemy_type = 1+stage; /* boss */
			}
			else {
				switch( stage ){
					case 0x00:
					if( indx<=2 ) enemy_type = 0x06; else enemy_type = 0x05;
					break;

					case 0x01:
					if( indx<=2 ) enemy_type = 0x0a; else enemy_type = 0x09;
					break;

					case 0x02:
					if( indx<=3 ) enemy_type = 0x0e; else enemy_type = 0xd;
					break;

					case 0x03:
					enemy_type = 0x12;
					break;
				}
			}
			mcu_buffer[0] = 1;
			mcu_buffer[1] = enemy_type;
		}
		break;

		default:
		logerror("unknown MCU command: %02x\n", mcu_buffer[0] );
		break;
	}
}

static READ_HANDLER( mcu_r )
{
	int result = 1;

	if( mcu_input_size ) mcu_process_command();

	if( mcu_output_byte<MCU_BUFFER_MAX )
		result = mcu_buffer[mcu_output_byte++];

	return result;
}

/********************************************************************************************/

static int bank;

static WRITE_HANDLER( bankswitch_w )
{
	if( (data&1)!=bank )
	{
		UINT8 *RAM = memory_region(REGION_CPU1);
		bank = data&1;
		cpu_setbank(1,&RAM[ bank?0x10000:0x4000 ]);
	}
}

static INTERRUPT_GEN( renegade_interrupt )
{
/*
	static int coin;
	int port = readinputport(1) & 0xc0;
	if (port != 0xc0 ){
		if (coin == 0){
			coin = 1;
			return irq0_line_hold();
		}
	}
	else coin = 0;
*/

	static int count;
	count = !count;
	if( count )
		cpu_set_irq_line(0, IRQ_LINE_NMI, PULSE_LINE);
	else
		cpu_set_irq_line(0, 0, HOLD_LINE);
}

static WRITE_HANDLER( renegade_coin_counter_w )
{
	//coin_counter_w(offset,data);
}


/********************************************************************************************/

static MEMORY_READ_START( main_readmem )
	{ 0x0000, 0x37ff, MRA_RAM },
	{ 0x3800, 0x3800, input_port_0_r }, /* Player#1 controls, P1,P2 start */
	{ 0x3801, 0x3801, input_port_1_r }, /* Player#2 controls, coin triggers */
	{ 0x3802, 0x3802, input_port_2_r }, /* DIP2  various IO ports */
	{ 0x3803, 0x3803, input_port_3_r }, /* DIP1 */
	{ 0x3804, 0x3804, mcu_r },
	{ 0x3805, 0x3805, mcu_reset_r },
	{ 0x4000, 0x7fff, MRA_BANK1 },
	{ 0x8000, 0xffff, MRA_ROM },
MEMORY_END

static MEMORY_WRITE_START( main_writemem )
	{ 0x0000, 0x17ff, MWA_RAM },
	{ 0x1800, 0x1fff, renegade_videoram2_w, &renegade_videoram2 },
	{ 0x2000, 0x27ff, MWA_RAM, &spriteram },
	{ 0x2800, 0x2fff, renegade_videoram_w, &videoram },
	{ 0x3000, 0x30ff, paletteram_xxxxBBBBGGGGRRRR_split1_w, &paletteram },
	{ 0x3100, 0x31ff, paletteram_xxxxBBBBGGGGRRRR_split2_w, &paletteram_2 },
	{ 0x3800, 0x3800, renegade_scroll0_w },
	{ 0x3801, 0x3801, renegade_scroll1_w },
	{ 0x3802, 0x3802, sound_w },
	{ 0x3803, 0x3803, renegade_flipscreen_w },
	{ 0x3804, 0x3804, mcu_w },
	{ 0x3805, 0x3805, bankswitch_w },
	{ 0x3806, 0x3806, MWA_NOP }, // ?? watchdog
	{ 0x3807, 0x3807, renegade_coin_counter_w },
	{ 0x4000, 0xffff, MWA_ROM },
MEMORY_END

static MEMORY_READ_START( sound_readmem )
	{ 0x0000, 0x0fff, MRA_RAM },
	{ 0x1000, 0x1000, soundlatch_r },
	{ 0x2801, 0x2801, YM3526_status_port_0_r },
	{ 0x8000, 0xffff, MRA_ROM },
MEMORY_END

static MEMORY_WRITE_START( sound_writemem )
	{ 0x0000, 0x0fff, MWA_RAM },
	{ 0x1800, 0x1800, MWA_NOP }, // this gets written the same values as 0x2000
	{ 0x2000, 0x2000, adpcm_play_w },
	{ 0x2800, 0x2800, YM3526_control_port_0_w },
	{ 0x2801, 0x2801, YM3526_write_port_0_w },
	{ 0x3000, 0x3000, MWA_NOP }, /* adpcm related? stereo pan? */
	{ 0x8000, 0xffff, MWA_ROM },
MEMORY_END



INPUT_PORTS_START( renegade )
	PORT_START	/* IN0 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP	  | IPF_8WAY )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 )	/* attack left */
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 )	/* jump */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_START2 )

	PORT_START	/* IN1 */
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT | IPF_8WAY | IPF_PLAYER2 )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT  | IPF_8WAY | IPF_PLAYER2)
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_JOYSTICK_UP	  | IPF_8WAY | IPF_PLAYER2)
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN  | IPF_8WAY | IPF_PLAYER2)
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_BUTTON1 | IPF_PLAYER2)
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_BUTTON2 | IPF_PLAYER2)
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_COIN1 )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_COIN2 )

	PORT_START /* DIP2 */
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(	0x02, "Easy" )
	PORT_DIPSETTING(	0x03, "Normal" )
	PORT_DIPSETTING(	0x01, "Hard" )
	PORT_DIPSETTING(	0x00, "Very Hard" )

	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_BUTTON3 )	/* attack right */
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_BUTTON3 | IPF_PLAYER2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNUSED ) 	/* 68705 status */
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )	/* 68705 status */
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_VBLANK )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_SERVICE1 )

	PORT_START /* DIP1 */
	PORT_DIPNAME( 0x03, 0x03, DEF_STR( Coin_A ) )
	PORT_DIPSETTING(	0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(	0x03, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(	0x02, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(	0x01, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x0c, 0x0c, DEF_STR( Coin_B ) )
	PORT_DIPSETTING(	0x00, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(	0x0c, DEF_STR( 1C_1C ) )
	PORT_DIPSETTING(	0x08, DEF_STR( 1C_2C ) )
	PORT_DIPSETTING(	0x04, DEF_STR( 1C_3C ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Lives ) )
	PORT_DIPSETTING (	0x10, "1" )
	PORT_DIPSETTING (	0x00, "2" )
	PORT_DIPNAME( 0x20, 0x20, "Bonus" )
	PORT_DIPSETTING (	0x20, "30k" )
	PORT_DIPSETTING (	0x00, "None" )
	PORT_DIPNAME(0x40, 0x00, DEF_STR( Cabinet ) )
	PORT_DIPSETTING (  0x00, DEF_STR( Upright ) )
	PORT_DIPSETTING (  0x40, DEF_STR( Cocktail ) )
	PORT_DIPNAME(0x80, 0x80, DEF_STR( Flip_Screen ) )
	PORT_DIPSETTING (  0x80, DEF_STR( Off ) )
	PORT_DIPSETTING (  0x00, DEF_STR( On ) )
INPUT_PORTS_END



static struct GfxLayout charlayout =
{
	8,8, /* 8x8 characters */
	1024, /* 1024 characters */
	3, /* bits per pixel */
	{ 2, 4, 6 },	/* plane offsets; bit 0 is always clear */
	{ 1, 0, 65, 64, 129, 128, 193, 192 }, /* x offsets */
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 }, /* y offsets */
	32*8 /* offset to next character */
};

static struct GfxLayout tileslayout1 =
{
	16,16, /* tile size */
	256, /* number of tiles */
	3, /* bits per pixel */

	/* plane offsets */
	{ 4, 0x8000*8+0, 0x8000*8+4 },

	/* x offsets */
	{ 3, 2, 1, 0, 16*8+3, 16*8+2, 16*8+1, 16*8+0,
		32*8+3,32*8+2 ,32*8+1 ,32*8+0 ,48*8+3 ,48*8+2 ,48*8+1 ,48*8+0 },

	/* y offsets */
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
		8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },

	64*8 /* offset to next tile */
};

static struct GfxLayout tileslayout2 =
{
	16,16, /* tile size */
	256, /* number of tiles */
	3, /* bits per pixel */

	/* plane offsets */
	{ 0, 0xC000*8+0, 0xC000*8+4 },

	/* x offsets */
	{ 3, 2, 1, 0, 16*8+3, 16*8+2, 16*8+1, 16*8+0,
		32*8+3,32*8+2 ,32*8+1 ,32*8+0 ,48*8+3 ,48*8+2 ,48*8+1 ,48*8+0 },

	/* y offsets */
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
		8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },

	64*8 /* offset to next tile */
};

static struct GfxLayout tileslayout3 =
{
	16,16, /* tile size */
	256, /* number of tiles */
	3, /* bits per pixel */

	/* plane offsets */
	{ 0x4000*8+4, 0x10000*8+0, 0x10000*8+4 },

	/* x offsets */
	{ 3, 2, 1, 0, 16*8+3, 16*8+2, 16*8+1, 16*8+0,
		32*8+3,32*8+2 ,32*8+1 ,32*8+0 ,48*8+3 ,48*8+2 ,48*8+1 ,48*8+0 },

	/* y offsets */
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
		8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },

	64*8 /* offset to next tile */
};

static struct GfxLayout tileslayout4 =
{
	16,16, /* tile size */
	256, /* number of tiles */
	3, /* bits per pixel */

	/* plane offsets */
	{ 0x4000*8+0, 0x14000*8+0, 0x14000*8+4 },

	/* x offsets */
	{ 3, 2, 1, 0, 16*8+3, 16*8+2, 16*8+1, 16*8+0,
		32*8+3,32*8+2 ,32*8+1 ,32*8+0 ,48*8+3 ,48*8+2 ,48*8+1 ,48*8+0 },

	/* y offsets */
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
		8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8, 15*8 },

	64*8 /* offset to next tile */
};

static struct GfxDecodeInfo gfxdecodeinfo[] =
{
	/* 8x8 text, 8 colors */
	{ REGION_GFX1, 0x00000, &charlayout,	 0, 4 },	/* colors	0- 32 */

	/* 16x16 background tiles, 8 colors */
	{ REGION_GFX2, 0x00000, &tileslayout1, 192, 8 },	/* colors 192-255 */
	{ REGION_GFX2, 0x00000, &tileslayout2, 192, 8 },
	{ REGION_GFX2, 0x00000, &tileslayout3, 192, 8 },
	{ REGION_GFX2, 0x00000, &tileslayout4, 192, 8 },

	{ REGION_GFX2, 0x18000, &tileslayout1, 192, 8 },
	{ REGION_GFX2, 0x18000, &tileslayout2, 192, 8 },
	{ REGION_GFX2, 0x18000, &tileslayout3, 192, 8 },
	{ REGION_GFX2, 0x18000, &tileslayout4, 192, 8 },

	/* 16x16 sprites, 8 colors */
	{ REGION_GFX3, 0x00000, &tileslayout1, 128, 4 },	/* colors 128-159 */
	{ REGION_GFX3, 0x00000, &tileslayout2, 128, 4 },
	{ REGION_GFX3, 0x00000, &tileslayout3, 128, 4 },
	{ REGION_GFX3, 0x00000, &tileslayout4, 128, 4 },

	{ REGION_GFX3, 0x18000, &tileslayout1, 128, 4 },
	{ REGION_GFX3, 0x18000, &tileslayout2, 128, 4 },
	{ REGION_GFX3, 0x18000, &tileslayout3, 128, 4 },
	{ REGION_GFX3, 0x18000, &tileslayout4, 128, 4 },

	{ REGION_GFX3, 0x30000, &tileslayout1, 128, 4 },
	{ REGION_GFX3, 0x30000, &tileslayout2, 128, 4 },
	{ REGION_GFX3, 0x30000, &tileslayout3, 128, 4 },
	{ REGION_GFX3, 0x30000, &tileslayout4, 128, 4 },

	{ REGION_GFX3, 0x48000, &tileslayout1, 128, 4 },
	{ REGION_GFX3, 0x48000, &tileslayout2, 128, 4 },
	{ REGION_GFX3, 0x48000, &tileslayout3, 128, 4 },
	{ REGION_GFX3, 0x48000, &tileslayout4, 128, 4 },
	{ -1 }
};



/* handler called by the 3526 emulator when the internal timers cause an IRQ */
static void irqhandler(int linestate)
{
	cpu_set_irq_line(1,M6809_FIRQ_LINE,linestate);
}

static struct YM3526interface ym3526_interface = {
	1,			/* 1 chip (no more supported) */
	3000000,	/* 3 MHz ? (hand tuned) */
	{ 100 }, 	/* volume */
	{ irqhandler },
};

static struct ADPCMinterface adpcm_interface =
{
	1,			/* 1 channel */
	8000,		/* 8000Hz playback */
	REGION_SOUND1,	/* memory region */
	{ 100 } 	/* volume */
};



static MACHINE_DRIVER_START( renegade )

	/* basic machine hardware */
	MDRV_CPU_ADD(M6502, 1500000)	/* 1.5 MHz? */
	MDRV_CPU_MEMORY(main_readmem,main_writemem)
	MDRV_CPU_VBLANK_INT(renegade_interrupt,2)

	MDRV_CPU_ADD(M6809, 1500000)
	MDRV_CPU_FLAGS(CPU_AUDIO_CPU)	/* ? */
	MDRV_CPU_MEMORY(sound_readmem,sound_writemem)
								/* IRQs are caused by the main CPU */
	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_REAL_60HZ_VBLANK_DURATION*2)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER)
	MDRV_SCREEN_SIZE(32*8, 32*8)
	MDRV_VISIBLE_AREA(1*8, 31*8-1, 0, 30*8-1)
	MDRV_GFXDECODE(gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(256)

	MDRV_VIDEO_START(renegade)
	MDRV_VIDEO_UPDATE(renegade)

	/* sound hardware */
	MDRV_SOUND_ADD(YM3526, ym3526_interface)
	MDRV_SOUND_ADD(ADPCM, adpcm_interface)
MACHINE_DRIVER_END



ROM_START( renegade )
	ROM_REGION( 0x14000, REGION_CPU1, 0 )	/* 64k for code + bank switched ROM */
	ROM_LOAD( "nb-5.bin",     0x08000, 0x8000, CRC(ba683ddf) SHA1(7516fac1c4fd14cbf43481e94c0c26c662c4cd28) )
	ROM_LOAD( "na-5.bin",     0x04000, 0x4000, CRC(de7e7df4) SHA1(7d26ac29e0b5858d9a0c0cdc86c864e464145260) )
	ROM_CONTINUE(			  0x10000, 0x4000 )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* audio CPU (M6809) */
	ROM_LOAD( "n0-5.bin",     0x8000, 0x8000, CRC(3587de3b) SHA1(f82e758254b21eb0c5a02469c72adb86d9577065) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* mcu (missing) */
	ROM_LOAD( "mcu",          0x8000, 0x8000, NO_DUMP )

	ROM_REGION( 0x08000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "nc-5.bin",     0x0000, 0x8000, CRC(9adfaa5d) SHA1(7bdb7bd4387b49e0489f9539161e1ed9d8f9f6a0) )  /* characters */

	ROM_REGION( 0x30000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "n1-5.bin",     0x00000, 0x8000, CRC(4a9f47f3) SHA1(01c94bc4c85314f1e0caa3afe91705875d118c13) ) /* tiles */
	ROM_LOAD( "n6-5.bin",     0x08000, 0x8000, CRC(d62a0aa8) SHA1(a0b55cd3eee352fb91d9bb8c6c4f4f55b2df83e9) )
	ROM_LOAD( "n7-5.bin",     0x10000, 0x8000, CRC(7ca5a532) SHA1(1110aa1c7562805dd4b298ab2860c66a6cc2685b) )
	ROM_LOAD( "n2-5.bin",     0x18000, 0x8000, CRC(8d2e7982) SHA1(72fc85ff7b54be10501a2a24303dadd5f33e5650) )
	ROM_LOAD( "n8-5.bin",     0x20000, 0x8000, CRC(0dba31d3) SHA1(8fe250787debe07e4f6c0002a9f799869b13a5fd) )
	ROM_LOAD( "n9-5.bin",     0x28000, 0x8000, CRC(5b621b6a) SHA1(45c6a688a5b4e9da71133c43cc48eea568557be3) )

	ROM_REGION( 0x60000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "nh-5.bin",     0x00000, 0x8000, CRC(dcd7857c) SHA1(eb530ccc939f2fa42b3c743605d5398f4afe7d7a) ) /* sprites */
	ROM_LOAD( "nd-5.bin",     0x08000, 0x8000, CRC(2de1717c) SHA1(af5a994348301fa888092ae65d08cfb6ad124407) )
	ROM_LOAD( "nj-5.bin",     0x10000, 0x8000, CRC(0f96a18e) SHA1(1f7e11e11d5031b4942d9d05161bcb9466514af8) )
	ROM_LOAD( "nn-5.bin",     0x18000, 0x8000, CRC(1bf15787) SHA1(b3371bf33f8b76a4a9887a7a43dba1f26353e978) )
	ROM_LOAD( "ne-5.bin",     0x20000, 0x8000, CRC(924c7388) SHA1(2f3ee2f28d8b04df6258a3949b7b0f60a3ae358f) )
	ROM_LOAD( "nk-5.bin",     0x28000, 0x8000, CRC(69499a94) SHA1(2e92931ef4e8948e3985f0a242db4137016d8eea) )
	ROM_LOAD( "ni-5.bin",     0x30000, 0x8000, CRC(6f597ed2) SHA1(54d34c13cda1b41ef354f9e6f3ce34673ef6c020) )
	ROM_LOAD( "nf-5.bin",     0x38000, 0x8000, CRC(0efc8d45) SHA1(4fea3165fd279539bfd424f1dc355cbd741bc48d) )
	ROM_LOAD( "nl-5.bin",     0x40000, 0x8000, CRC(14778336) SHA1(17b4048942b5fa8167a7f2b471dbc5a5d3f017ee) )
	ROM_LOAD( "no-5.bin",     0x48000, 0x8000, CRC(147dd23b) SHA1(fa4f9b774845d0333909d876590cda38d19b72d8) )
	ROM_LOAD( "ng-5.bin",     0x50000, 0x8000, CRC(a8ee3720) SHA1(df3d40015b16fa7a9bf05f0ed5741c22f7f152c7) )
	ROM_LOAD( "nm-5.bin",     0x58000, 0x8000, CRC(c100258e) SHA1(0e2124e642b9742a9a0045f460974025048bc2dd) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 ) /* adpcm */
	ROM_LOAD( "n5-5.bin",     0x00000, 0x8000, CRC(7ee43a3c) SHA1(36b14b886096177cdd0bd0c99cbcfcc362b2bc30) )
	ROM_LOAD( "n4-5.bin",     0x10000, 0x8000, CRC(6557564c) SHA1(b3142be9d48eacb43786079a7ae012010f6afabb) )
	ROM_LOAD( "n3-5.bin",     0x18000, 0x8000, CRC(78fd6190) SHA1(995df0e88f5c34946e0634b50bda8c1cc621afaa) )
ROM_END

ROM_START( kuniokun )
	ROM_REGION( 0x14000, REGION_CPU1, 0 )	/* 64k for code + bank switched ROM */
	ROM_LOAD( "nb-01.bin",    0x08000, 0x8000, CRC(93fcfdf5) SHA1(51cdb9377544ae17895e427f21d150ce195ab8e7) ) // original
	ROM_LOAD( "ta18-11.bin",  0x04000, 0x4000, CRC(f240f5cd) SHA1(ed6875e8ad2988e88389d4f63ff448d0823c195f) )
	ROM_CONTINUE(			  0x10000, 0x4000 )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* audio CPU (M6809) */
	ROM_LOAD( "n0-5.bin",     0x8000, 0x8000, CRC(3587de3b) SHA1(f82e758254b21eb0c5a02469c72adb86d9577065) )

	ROM_REGION( 0x10000, REGION_CPU3, 0 ) /* mcu (missing) */
	ROM_LOAD( "mcu",          0x8000, 0x8000, NO_DUMP )

	ROM_REGION( 0x08000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "ta18-25.bin",  0x0000, 0x8000, CRC(9bd2bea3) SHA1(fa79c9d4c71c1dbbf0e14cb8d6870f1f94b9af88) )  /* characters */

	ROM_REGION( 0x30000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "ta18-01.bin",  0x00000, 0x8000, CRC(daf15024) SHA1(f37de97275f52dfbbad7bf8c82f8108e84bcf63e) ) /* tiles */
	ROM_LOAD( "ta18-06.bin",  0x08000, 0x8000, CRC(1f59a248) SHA1(8ab70aa8f0dccbe94240c96835a43b0900d52120) )
	ROM_LOAD( "n7-5.bin",     0x10000, 0x8000, CRC(7ca5a532) SHA1(1110aa1c7562805dd4b298ab2860c66a6cc2685b) )
	ROM_LOAD( "ta18-02.bin",  0x18000, 0x8000, CRC(994c0021) SHA1(9219464decc1b07591d0485502e2bcc0c2d16261) )
	ROM_LOAD( "ta18-04.bin",  0x20000, 0x8000, CRC(55b9e8aa) SHA1(26c91030c53a022c1f1f3131768e8f7ba613168d) )
	ROM_LOAD( "ta18-03.bin",  0x28000, 0x8000, CRC(0475c99a) SHA1(36b7b856e728c68e0dd3ecb844033369a5117270) )

	ROM_REGION( 0x60000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "ta18-20.bin",  0x00000, 0x8000, CRC(c7d54139) SHA1(f76d237a6ee8bbcbf344145d31e532834da7c131) ) /* sprites */
	ROM_LOAD( "ta18-24.bin",  0x08000, 0x8000, CRC(84677d45) SHA1(cb7fe69e13d2d696acbc464b7584c7514cfc7f85) )
	ROM_LOAD( "ta18-18.bin",  0x10000, 0x8000, CRC(1c770853) SHA1(4fe6051265729a9d36b6d3dd826c3f6dcb4a7a25) )
	ROM_LOAD( "ta18-14.bin",  0x18000, 0x8000, CRC(af656017) SHA1(d395d35fe6d8e281596b2df571099b841f979a97) )
	ROM_LOAD( "ta18-23.bin",  0x20000, 0x8000, CRC(3fd19cf7) SHA1(2e45ab95d19664ed16b19c40bdb8d8c506b98dd1) )
	ROM_LOAD( "ta18-17.bin",  0x28000, 0x8000, CRC(74c64c6e) SHA1(7cbb969c89996476d115f2e55be5a5c5f87c344a) )
	ROM_LOAD( "ta18-19.bin",  0x30000, 0x8000, CRC(c8795fd7) SHA1(ef7aebf21dba248383d5b93cba9620a585e244b9) )
	ROM_LOAD( "ta18-22.bin",  0x38000, 0x8000, CRC(df3a2ff5) SHA1(94bf8968a3d927b410e39d4b6ef28cdfd533179f) )
	ROM_LOAD( "ta18-16.bin",  0x40000, 0x8000, CRC(7244bad0) SHA1(ebd93c82f0b8dfffa905927a6884a61c62ea3879) )
	ROM_LOAD( "ta18-13.bin",  0x48000, 0x8000, CRC(b6b14d46) SHA1(065cfb39c141265fbf92abff67a5efe8e258c2ce) )
	ROM_LOAD( "ta18-21.bin",  0x50000, 0x8000, CRC(c95e009b) SHA1(d45a247d4ebf8587a2cd30c83444cc7bd17a3534) )
	ROM_LOAD( "ta18-15.bin",  0x58000, 0x8000, CRC(a5d61d01) SHA1(9bf1f0b8296667db31ff1c34e28c8eda3ce9f7c3) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 ) /* adpcm */
	ROM_LOAD( "ta18-07.bin",  0x00000, 0x8000, CRC(02e3f3ed) SHA1(ab09b3af2c4ab9a36eb1273bcc7c788350048554) )
	ROM_LOAD( "ta18-08.bin",  0x10000, 0x8000, CRC(c9312613) SHA1(fbbdf7c56c34cbee42984e41fcf2a21da2b87a31) )
	ROM_LOAD( "ta18-09.bin",  0x18000, 0x8000, CRC(07ed4705) SHA1(6fd4b78ca846fa602504f06f3105b2da03bcd00c) )
ROM_END

ROM_START( kuniokub )
	ROM_REGION( 0x14000, REGION_CPU1, 0 )	/* 64k for code + bank switched ROM */
	ROM_LOAD( "ta18-10.bin",  0x08000, 0x8000, CRC(a90cf44a) SHA1(6d63d9c29da7b8c5bc391e074b6b8fe6ae3892ae) ) // bootleg
	ROM_LOAD( "ta18-11.bin",  0x04000, 0x4000, CRC(f240f5cd) SHA1(ed6875e8ad2988e88389d4f63ff448d0823c195f) )
	ROM_CONTINUE(			  0x10000, 0x4000 )

	ROM_REGION( 0x10000, REGION_CPU2, 0 ) /* audio CPU (M6809) */
	ROM_LOAD( "n0-5.bin",     0x8000, 0x8000, CRC(3587de3b) SHA1(f82e758254b21eb0c5a02469c72adb86d9577065) )

	ROM_REGION( 0x08000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD( "ta18-25.bin",  0x0000, 0x8000, CRC(9bd2bea3) SHA1(fa79c9d4c71c1dbbf0e14cb8d6870f1f94b9af88) )  /* characters */

	ROM_REGION( 0x30000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD( "ta18-01.bin",  0x00000, 0x8000, CRC(daf15024) SHA1(f37de97275f52dfbbad7bf8c82f8108e84bcf63e) ) /* tiles */
	ROM_LOAD( "ta18-06.bin",  0x08000, 0x8000, CRC(1f59a248) SHA1(8ab70aa8f0dccbe94240c96835a43b0900d52120) )
	ROM_LOAD( "n7-5.bin",     0x10000, 0x8000, CRC(7ca5a532) SHA1(1110aa1c7562805dd4b298ab2860c66a6cc2685b) )
	ROM_LOAD( "ta18-02.bin",  0x18000, 0x8000, CRC(994c0021) SHA1(9219464decc1b07591d0485502e2bcc0c2d16261) )
	ROM_LOAD( "ta18-04.bin",  0x20000, 0x8000, CRC(55b9e8aa) SHA1(26c91030c53a022c1f1f3131768e8f7ba613168d) )
	ROM_LOAD( "ta18-03.bin",  0x28000, 0x8000, CRC(0475c99a) SHA1(36b7b856e728c68e0dd3ecb844033369a5117270) )

	ROM_REGION( 0x60000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD( "ta18-20.bin",  0x00000, 0x8000, CRC(c7d54139) SHA1(f76d237a6ee8bbcbf344145d31e532834da7c131) ) /* sprites */
	ROM_LOAD( "ta18-24.bin",  0x08000, 0x8000, CRC(84677d45) SHA1(cb7fe69e13d2d696acbc464b7584c7514cfc7f85) )
	ROM_LOAD( "ta18-18.bin",  0x10000, 0x8000, CRC(1c770853) SHA1(4fe6051265729a9d36b6d3dd826c3f6dcb4a7a25) )
	ROM_LOAD( "ta18-14.bin",  0x18000, 0x8000, CRC(af656017) SHA1(d395d35fe6d8e281596b2df571099b841f979a97) )
	ROM_LOAD( "ta18-23.bin",  0x20000, 0x8000, CRC(3fd19cf7) SHA1(2e45ab95d19664ed16b19c40bdb8d8c506b98dd1) )
	ROM_LOAD( "ta18-17.bin",  0x28000, 0x8000, CRC(74c64c6e) SHA1(7cbb969c89996476d115f2e55be5a5c5f87c344a) )
	ROM_LOAD( "ta18-19.bin",  0x30000, 0x8000, CRC(c8795fd7) SHA1(ef7aebf21dba248383d5b93cba9620a585e244b9) )
	ROM_LOAD( "ta18-22.bin",  0x38000, 0x8000, CRC(df3a2ff5) SHA1(94bf8968a3d927b410e39d4b6ef28cdfd533179f) )
	ROM_LOAD( "ta18-16.bin",  0x40000, 0x8000, CRC(7244bad0) SHA1(ebd93c82f0b8dfffa905927a6884a61c62ea3879) )
	ROM_LOAD( "ta18-13.bin",  0x48000, 0x8000, CRC(b6b14d46) SHA1(065cfb39c141265fbf92abff67a5efe8e258c2ce) )
	ROM_LOAD( "ta18-21.bin",  0x50000, 0x8000, CRC(c95e009b) SHA1(d45a247d4ebf8587a2cd30c83444cc7bd17a3534) )
	ROM_LOAD( "ta18-15.bin",  0x58000, 0x8000, CRC(a5d61d01) SHA1(9bf1f0b8296667db31ff1c34e28c8eda3ce9f7c3) )

	ROM_REGION( 0x20000, REGION_SOUND1, 0 ) /* adpcm */
	ROM_LOAD( "ta18-07.bin",  0x00000, 0x8000, CRC(02e3f3ed) SHA1(ab09b3af2c4ab9a36eb1273bcc7c788350048554) )
	ROM_LOAD( "ta18-08.bin",  0x10000, 0x8000, CRC(c9312613) SHA1(fbbdf7c56c34cbee42984e41fcf2a21da2b87a31) )
	ROM_LOAD( "ta18-09.bin",  0x18000, 0x8000, CRC(07ed4705) SHA1(6fd4b78ca846fa602504f06f3105b2da03bcd00c) )
ROM_END



GAME( 1986, renegade, 0,		 renegade, renegade, renegade, ROT0, "Technos (Taito America license)", "Renegade (US)" )
GAME( 1986, kuniokun, renegade, renegade, renegade, kuniokun, ROT0, "Technos", "Nekketsu Kouha Kunio-kun (Japan)" )
GAME( 1986, kuniokub, renegade, renegade, renegade, 0, 	   ROT0, "bootleg", "Nekketsu Kouha Kunio-kun (Japan bootleg)" )
