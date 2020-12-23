/***************************************************************************

	Ground Effects / Super Ground FX					(c) 1993 Taito

	Driver by Bryan McPhail & David Graves.

	Board Info:

	K1100744A
	J1100316A
	Sticker - K11J0744A
	------------------------------------------------------------------------------------------------
	| 2018  2088        43256      43256   MC68EC020RP25   DIP-SW (8) COM20020  SLIDE-SW  LAN-IN   |           Connector G        
	| 2018  2088        D51-21     D51-22              D51-14                                      |           -----------        
	| 2018  2088        43256      43256               (PAL16L8)                                   |       Parts    |   Solder   
	|                                                  D51-15              ADC0809        LAN-OUT  |       ------------------    
	| 2018              D51-23     D51-24              (PAL16L8)                                   |        GND    1 A   GND     
	|                                                  D51-16-1                          JP4       |        GND    2 B   GND     
	| 2018  TC0570SPC                                  (PAL16L8)            EEPROM.164   1-2       |        +12    3 C   +12     
	| 2018                                                                               2-3       |         +5    4 D   +5      
	|                                                                 TC0510NIO          2-3     --|         +5    5 E   +5      
	|    D51-13       D51-03                                                             2-3     |           +5    6 F   +5      
	|                 D51-04                             43256                           1-2     |                 7 H           
	|    TC0470LIN    D51-05                                                             1-2     --|        +13    8 J   +13     
	|                 D51-06                             43256   TC0650FCA               1-2       |        +13    9 K   +13     
	|                 D51-07                                                             2-3       |      SPK_R+  10 L  SPK_R-   
	|    TC0580PIV              43256                    43256      2018                 2-3       |      SPK_L+  11 M  SPK_L-   
	|                           43256     D51-17                                                   |   SPK_REAR-  12 N  SPK_REAR-
	| 514256          TC0480SCP           (PAL16L8)                                                |              13 P           
	| 514256                                                                                      G|         RED  14 R  GREEN    
	| 514256                              D51-10                                                   |        BLUE  15 S  SYNC     
	| 514256   D51-09                     D51-11          TC0620SCC                                |       V-GND  16 T           
	| 514256   D51-08                     D51012                                                   |      METER1  17 U  METER2   
	| 514256          43256                                                                        |    LOCKOUT1  18 V  LOCKOUT2 
	|          MB8421                                        43256                                 |              19 W           
	| D51-18   MB8421             ENSONIC                                                        --|       COIN1  20 X  COIN2    
	| (PAL20L8)       D51-29      OTISR2                     43256      MB87078                  |       SERVICE  21 Y  TEST     
	|                         D51-01                                                             |                22 Z           
	| D51-19          43256           ENSONIC     ENSONIC                                        --|    SHIFT UP  23 a  BRAKE    
	| (PAL20L8)               D51-02  SUPER GLU   ESP-R6    TC511664    TL074  TL074               |   HANDLE VR  24 b  ACCEL VR 
	|                 D51-30                                                                       |    SHIFT DN  25 c           
	| MC68000P12F16MHz                                                  TDA1543 TDA1543            |              26 d           
	|                 40MHz    16MHz  30.476MHz    MC68681                                         |         GND  27 e  GND      
	| D51-20                                                                                       |         GND  28 f  GND      
	| (PAL20L8)                                                                                    |
	------------------------------------------------------------------------------------------------


	Ground Effects combines the sprite system used in Taito Z games with
	the TC0480SCP tilemap chip plus some features from the Taito F3 system.
	It has an extra tilemap chip which is a dead ringer for the TC0100SCN
	(check the inits), like Under Fire.

	Ground Effects is effectively a 30Hz game - though the vblank interrupts
	still come in at 60Hz, the game uses a hardware frame counter to limit
	itself to 30Hz (it only updates things like the video registers every
	other vblank).  There isn't enough cpu power in a 16MHz 68020 to run 
	this game at 60Hz.

	Ground Effects has a network mode - probably uses IRQ 6 and the unknown
	ports at 0xc00000.

***************************************************************************/

#include "driver.h"
#include "cpu/m68000/m68000.h"
#include "vidhrdw/generic.h"
#include "vidhrdw/taitoic.h"
#include "machine/eeprom.h"

VIDEO_START( groundfx );
VIDEO_UPDATE( groundfx );

/* F3 sound */
READ16_HANDLER(f3_68000_share_r);
WRITE16_HANDLER(f3_68000_share_w);
READ16_HANDLER(f3_68681_r);
WRITE16_HANDLER(f3_68681_w);
READ16_HANDLER(es5510_dsp_r);
WRITE16_HANDLER(es5510_dsp_w);
WRITE16_HANDLER(f3_volume_w);
WRITE16_HANDLER(f3_es5505_bank_w);
void f3_68681_reset(void);
extern data32_t *f3_shared_ram;

static UINT16 coin_word, frame_counter=0;
static UINT16 port_sel = 0;
extern UINT16 groundfx_rotate_ctrl[8];
static data32_t *groundfx_ram;

/***********************************************************
				COLOR RAM

Extract a standard version of this
("taito_8bpg_palette_word_w"?) to Taitoic.c ?
***********************************************************/

static WRITE32_HANDLER( color_ram_w )
{
	int a,r,g,b;
	COMBINE_DATA(&paletteram32[offset]);

	{
		a = paletteram32[offset];
		r = (a &0xff0000) >> 16;
		g = (a &0xff00) >> 8;
		b = (a &0xff);

		palette_set_color(offset,r,g,b);
	}
}


/***********************************************************
				INTERRUPTS
***********************************************************/

static void groundfx_interrupt5(int x)
{
	cpu_set_irq_line(0,5,HOLD_LINE); //from 5... ADC port
}


/**********************************************************
				EPROM
**********************************************************/

static data8_t default_eeprom[128]=
{
	0x02,0x01,0x11,0x12,0x01,0x01,0x01,0x00,0x80,0x80,0x30,0x01,0x00,0x00,0x62,0x45,
	0xe0,0xa0,0xff,0x28,0xff,0xff,0xfa,0xd7,0x33,0x28,0x00,0x00,0x33,0x28,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0xe0,0xa0,0xff,0x28,0xff,0xff,0xff,0xff,0xfa,0xd7,
	0x33,0x28,0x00,0x00,0x33,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
	0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};

static struct EEPROM_interface groundfx_eeprom_interface =
{
	6,				/* address bits */
	16,				/* data bits */
	"0110",			/* read command */
	"0101",			/* write command */
	"0111",			/* erase command */
	"0100000000",	/* unlock command */
	"0100110000",	/* lock command */
};

static NVRAM_HANDLER( groundfx )
{
	if (read_or_write)
		EEPROM_save(file);
	else {
		EEPROM_init(&groundfx_eeprom_interface);
		if (file)
			EEPROM_load(file);
		else
			EEPROM_set_data(default_eeprom,128);  /* Default the gun setup values */
	}
}


/**********************************************************
			GAME INPUTS
**********************************************************/


static READ32_HANDLER( groundfx_input_r )
{
	switch (offset)
	{
		case 0x00:
		{
			return (input_port_0_word_r(0,0) << 16) | input_port_1_word_r(0,0) |
				  (EEPROM_read_bit() << 7) | frame_counter;
		}

		case 0x01:
		{
			return input_port_2_word_r(0,0) | (coin_word << 16);
		}
 	}

	return 0xffffffff;
}

static WRITE32_HANDLER( groundfx_input_w )
{
	switch (offset)
	{
		case 0x00:
		{
			if (ACCESSING_MSB32)	/* $500000 is watchdog */
			{
				watchdog_reset_w(0,data >> 24);
			}

			if (ACCESSING_LSB32)
			{
				EEPROM_set_clock_line((data & 0x20) ? ASSERT_LINE : CLEAR_LINE);
				EEPROM_write_bit(data & 0x40);
				EEPROM_set_cs_line((data & 0x10) ? CLEAR_LINE : ASSERT_LINE);
				return;
			}

			return;
		}

		case 0x01:
		{
			if (ACCESSING_MSB32)
			{
				coin_lockout_w(0,~data & 0x01000000);
				coin_lockout_w(1,~data & 0x02000000);
				coin_counter_w(0, data & 0x04000000);
				coin_counter_w(1, data & 0x08000000);
				coin_word = (data >> 16) &0xffff;
			}
		}
	}
}

static READ32_HANDLER( groundfx_adc_r )
{
	return (input_port_3_word_r(0,0) << 8) | input_port_4_word_r(0,0);
}

static WRITE32_HANDLER( groundfx_adc_w )
{
	/* One interrupt per input port (4 per frame, though only 2 used).
		1000 cycle delay is arbitrary */
	timer_set(TIME_IN_CYCLES(1000,0),0, groundfx_interrupt5);
}

static WRITE32_HANDLER( rotate_control_w )	/* only a guess that it's rotation */
{
		if (ACCESSING_LSW32)
		{
			groundfx_rotate_ctrl[port_sel] = data;
			return;
		}

		if (ACCESSING_MSW32)
		{
			port_sel = (data &0x70000) >> 16;
		}
}


static WRITE32_HANDLER( motor_control_w )
{
/*
	Standard value poked is 0x00910200 (we ignore lsb and msb
	which seem to be always zero)

	0x0, 0x8000 and 0x9100 are written at startup

	Two bits are written in test mode to this middle word
	to test gun vibration:

	........ .x......   P1 gun vibration
	........ x.......   P2 gun vibration
*/
}

/***********************************************************
			 MEMORY STRUCTURES
***********************************************************/

static MEMORY_READ32_START( groundfx_readmem )
	{ 0x000000, 0x1fffff, MRA32_ROM },
	{ 0x200000, 0x21ffff, MRA32_RAM },	/* main CPUA ram */
	{ 0x300000, 0x303fff, MRA32_RAM },	/* sprite ram */
	{ 0x500000, 0x500007, groundfx_input_r },
	{ 0x600000, 0x600003, groundfx_adc_r },
	{ 0x700000, 0x7007ff, MRA32_RAM },
	{ 0x800000, 0x80ffff, TC0480SCP_long_r },	  /* tilemaps */
	{ 0x830000, 0x83002f, TC0480SCP_ctrl_long_r },	// debugging
	{ 0x900000, 0x90ffff, TC0100SCN_long_r },	/* piv tilemaps */
	{ 0x920000, 0x92000f, TC0100SCN_ctrl_long_r },
	{ 0xa00000, 0xa0ffff, MRA32_RAM }, /* palette ram */
	{ 0xb00000, 0xb003ff, MRA32_RAM },	// ?? single bytes
	{ 0xc00000, 0xc00007, MRA32_NOP }, /* Network? */
MEMORY_END

static MEMORY_WRITE32_START( groundfx_writemem )
	{ 0x000000, 0x1fffff, MWA32_ROM },
	{ 0x200000, 0x21ffff, MWA32_RAM, &groundfx_ram },
	{ 0x300000, 0x303fff, MWA32_RAM, &spriteram32, &spriteram_size },
	{ 0x400000, 0x400003, motor_control_w },	/* gun vibration */
	{ 0x500000, 0x500007, groundfx_input_w },	/* eerom etc. */
	{ 0x600000, 0x600003, groundfx_adc_w },
	{ 0x700000, 0x7007ff, MWA32_RAM, &f3_shared_ram },
	{ 0x800000, 0x80ffff, TC0480SCP_long_w },	  /* tilemaps */
	{ 0x830000, 0x83002f, TC0480SCP_ctrl_long_w },
	{ 0x900000, 0x90ffff, TC0100SCN_long_w },	/* piv tilemaps */
	{ 0x920000, 0x92000f, TC0100SCN_ctrl_long_w },
	{ 0xa00000, 0xa0ffff, color_ram_w, &paletteram32 },
	{ 0xb00000, 0xb003ff, MWA32_RAM },	// single bytes, blending ??
	{ 0xd00000, 0xd00003, rotate_control_w },	/* perhaps port based rotate control? */
	/* f00000 is seat control? */
MEMORY_END

/******************************************************************************/

static MEMORY_READ16_START( sound_readmem )
	{ 0x000000, 0x07ffff, MRA16_RAM },
	{ 0x140000, 0x140fff, f3_68000_share_r },
	{ 0x200000, 0x20001f, ES5505_data_0_r },
	{ 0x260000, 0x2601ff, es5510_dsp_r },
	{ 0x280000, 0x28001f, f3_68681_r },
	{ 0xc00000, 0xcfffff, MRA16_BANK1 },
	{ 0xff8000, 0xffffff, MRA16_RAM },
MEMORY_END

static MEMORY_WRITE16_START( sound_writemem )
	{ 0x000000, 0x07ffff, MWA16_RAM },
	{ 0x140000, 0x140fff, f3_68000_share_w },
	{ 0x200000, 0x20001f, ES5505_data_0_w },
	{ 0x260000, 0x2601ff, es5510_dsp_w },
	{ 0x280000, 0x28001f, f3_68681_w },
	{ 0x300000, 0x30003f, f3_es5505_bank_w },
	{ 0x340000, 0x340003, f3_volume_w }, /* 8 channel volume control */
	{ 0xc00000, 0xcfffff, MWA16_ROM },
	{ 0xff8000, 0xffffff, MWA16_RAM },
MEMORY_END


/***********************************************************
			 INPUT PORTS (dips in eprom)
***********************************************************/

INPUT_PORTS_START( groundfx )
	PORT_START      /* IN0 */
	PORT_BIT( 0xffff, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START      /* IN1 */
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH,  IPT_SPECIAL ) /* Frame counter */
	PORT_BIT( 0x0002, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_SPECIAL )	/* reserved for EEROM */
	PORT_BIT( 0x0100, IP_ACTIVE_LOW,  IPT_BUTTON3 ) /* shift hi */
	PORT_BIT( 0x0200, IP_ACTIVE_LOW,  IPT_BUTTON1 ) /* brake */
	PORT_BIT( 0x0400, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x0800, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x1000, IP_ACTIVE_LOW,  IPT_BUTTON2 ) /* shift low */
	PORT_BIT( 0x2000, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x4000, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x8000, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START      /* IN2 */
	PORT_BITX(0x01, IP_ACTIVE_LOW,  IPT_SERVICE, DEF_STR( Service_Mode ), KEYCODE_F2, IP_JOY_NONE )
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_SERVICE1 )
	PORT_BIT( 0x04, IP_ACTIVE_LOW,  IPT_COIN1 )
	PORT_BIT( 0x08, IP_ACTIVE_LOW,  IPT_COIN2 )
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_LOW,  IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_LOW,  IPT_UNUSED )

	PORT_START	/* IN 2, steering wheel */
	PORT_ANALOG( 0xff, 0x7f, IPT_AD_STICK_X | IPF_REVERSE | IPF_PLAYER1, 25, 15, 0, 0xff )

	PORT_START	/* IN 3, accel */
	PORT_ANALOG( 0xff, 0x00, IPT_AD_STICK_Y | IPF_PLAYER1, 20, 10, 0, 0xff)

	PORT_START	/* IN 4, sound volume */
	PORT_ANALOG( 0xff, 0x00, IPT_AD_STICK_X | IPF_REVERSE | IPF_PLAYER2, 20, 10, 0, 0xff)

	PORT_START	/* IN 5, unknown */
	PORT_ANALOG( 0xff, 0x00, IPT_AD_STICK_Y | IPF_PLAYER2, 20, 10, 0, 0xff)
INPUT_PORTS_END

/**********************************************************
				GFX DECODING
**********************************************************/

static struct GfxLayout tile16x16_layout =
{
	16,16,	/* 16*16 sprites */
	RGN_FRAC(1,2),
	5,	/* 5 bits per pixel */
	{ RGN_FRAC(1,2), 0, 8, 16, 24,},
	{ 32, 33, 34, 35, 36, 37, 38, 39, 0, 1, 2, 3, 4, 5, 6, 7 },
	{ 0*64, 1*64,  2*64,  3*64,  4*64,  5*64,  6*64,  7*64,
	  8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
	64*16	/* every sprite takes 128 consecutive bytes */
};

static struct GfxLayout charlayout =
{
	16,16,    /* 16*16 characters */
	RGN_FRAC(1,1),
	4,        /* 4 bits per pixel */
	{ 0, 1, 2, 3 },
	{ 1*4, 0*4, 5*4, 4*4, 3*4, 2*4, 7*4, 6*4, 9*4, 8*4, 13*4, 12*4, 11*4, 10*4, 15*4, 14*4 },
	{ 0*64, 1*64, 2*64, 3*64, 4*64, 5*64, 6*64, 7*64, 8*64, 9*64, 10*64, 11*64, 12*64, 13*64, 14*64, 15*64 },
	128*8     /* every sprite takes 128 consecutive bytes */
};

static struct GfxLayout pivlayout =
{
	8,8,    /* 8*8 characters */
	RGN_FRAC(1,2),
	6,      /* 4 bits per pixel */
	{ RGN_FRAC(1,2), RGN_FRAC(1,2)+1, 0, 1, 2, 3 },
	{ 2*4, 3*4, 0*4, 1*4, 6*4, 7*4, 4*4, 5*4 },
	{ 0*32, 1*32, 2*32, 3*32, 4*32, 5*32, 6*32, 7*32 },
	32*8    /* every sprite takes 32 consecutive bytes */
};

static struct GfxDecodeInfo groundfx_gfxdecodeinfo[] =
{
	{ REGION_GFX2, 0x0, &tile16x16_layout,  4096, 512 },
	{ REGION_GFX1, 0x0, &charlayout,        0, 512 },
	{ REGION_GFX3, 0x0, &pivlayout,         0, 512 },
	{ -1 } /* end of array */
};


/***********************************************************
			     MACHINE DRIVERS
***********************************************************/

static MACHINE_INIT( groundfx )
{
	/* Sound cpu program loads to 0xc00000 so we use a bank */
	data16_t *RAM = (data16_t *)memory_region(REGION_CPU2);
	cpu_setbank(1,&RAM[0x80000]);

	RAM[0]=RAM[0x80000]; /* Stack and Reset vectors */
	RAM[1]=RAM[0x80001];
	RAM[2]=RAM[0x80002];
	RAM[3]=RAM[0x80003];

	f3_68681_reset();
}

static struct ES5505interface es5505_interface =
{
	1,					/* total number of chips */
	{ 30476000 / 2 },	/* freq */
	{ REGION_SOUND1 },	/* Bank 0: Unused by F3 games? */
	{ REGION_SOUND1 },	/* Bank 1: All games seem to use this */
	{ YM3012_VOL(100,MIXER_PAN_LEFT,100,MIXER_PAN_RIGHT) },		/* master volume */
};

static INTERRUPT_GEN( groundfx_interrupt )
{
	frame_counter^=1;
	cpu_set_irq_line(0, 4, HOLD_LINE);
}

static MACHINE_DRIVER_START( groundfx )

	/* basic machine hardware */
	MDRV_CPU_ADD(M68EC020, 16000000)	/* 16 MHz */
	MDRV_CPU_MEMORY(groundfx_readmem,groundfx_writemem)
	MDRV_CPU_VBLANK_INT(groundfx_interrupt,1)

	MDRV_CPU_ADD(M68000, 16000000)
	MDRV_CPU_FLAGS(CPU_AUDIO_CPU)
	MDRV_CPU_MEMORY(sound_readmem,sound_writemem)

	MDRV_FRAMES_PER_SECOND(60)
	MDRV_VBLANK_DURATION(DEFAULT_60HZ_VBLANK_DURATION)

	MDRV_MACHINE_INIT(groundfx)
	MDRV_NVRAM_HANDLER(groundfx)

	/* video hardware */
	MDRV_VIDEO_ATTRIBUTES(VIDEO_TYPE_RASTER | VIDEO_NEEDS_6BITS_PER_GUN)
	MDRV_SCREEN_SIZE(40*8, 32*8)
	MDRV_VISIBLE_AREA(0, 40*8-1, 3*8, 32*8-1)
	MDRV_GFXDECODE(groundfx_gfxdecodeinfo)
	MDRV_PALETTE_LENGTH(16384)

	MDRV_VIDEO_START(groundfx)
	MDRV_VIDEO_UPDATE(groundfx)

	/* sound hardware */
	MDRV_SOUND_ATTRIBUTES(SOUND_SUPPORTS_STEREO)
	MDRV_SOUND_ADD(ES5505, es5505_interface)
MACHINE_DRIVER_END

/***************************************************************************
					DRIVERS
***************************************************************************/

ROM_START( groundfx )
	ROM_REGION( 0x200000, REGION_CPU1, 0 )	/* 2048K for 68020 code (CPU A) */
	ROM_LOAD32_BYTE( "d51-24.79", 0x00000, 0x80000, CRC(5caaa031) SHA1(03e727e26df701e3f5e16c5f933d5b29a528945a) )
	ROM_LOAD32_BYTE( "d51-23.61", 0x00001, 0x80000, CRC(462e3c9b) SHA1(7f116ee755748497b911868a948d3e3b5134e475) )
	ROM_LOAD32_BYTE( "d51-22.77", 0x00002, 0x80000, CRC(b6b04d88) SHA1(58685ee8fd788dcbfe318f1e3c06d93e2128034c) )
	ROM_LOAD32_BYTE( "d51-21.59", 0x00003, 0x80000, CRC(21ecde2b) SHA1(c6d3738f34c8e24346e7784b14aeff300ae2d225) )

	ROM_REGION( 0x180000, REGION_CPU2, 0 )
	ROM_LOAD16_BYTE( "d51-29.54", 0x100000, 0x40000,  CRC(4b64f41d) SHA1(040427668d13f7320d23805098d6d0e1aa8d121e) )
	ROM_LOAD16_BYTE( "d51-30.56", 0x100001, 0x40000,  CRC(45f339fe) SHA1(cc7adfb2b86070f5bb426542e3b7ed2a50b3c39e) )

	ROM_REGION( 0x400000, REGION_GFX1, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "d51-08.35", 0x000000, 0x200000, CRC(835b7a0f) SHA1(0131fceabd73b0045b5d4ae0bb2f03efdd407962) )	/* SCR 16x16 tiles */
	ROM_LOAD16_BYTE( "d51-09.34", 0x000001, 0x200000, CRC(6dabd83d) SHA1(3dbd7ea36b9900faa6420af1f1600efe295db74c) )

	ROM_REGION( 0x1000000, REGION_GFX2, ROMREGION_DISPOSE )
	ROM_LOAD32_BYTE( "d51-03.47", 0x800000, 0x200000, CRC(629a5c99) SHA1(cfc1c0b07ecefd6eddb83edcbcf710e8b8de19e4) )	/* OBJ 16x16 tiles */
	ROM_LOAD32_BYTE( "d51-04.48", 0x000000, 0x200000, CRC(f49b14b7) SHA1(31129771159c1295a074c8311344ece525302289) )
	ROM_LOAD32_BYTE( "d51-05.49", 0x000001, 0x200000, CRC(3a2e2cbf) SHA1(ed2c1ca9211b1d70b4767a54e08263a3e4867199) )
	ROM_LOAD32_BYTE( "d51-06.50", 0x000002, 0x200000, CRC(d33ce2a0) SHA1(92c4504344672ea798cd6dd34f4b46848bf9f82b) )
	ROM_LOAD32_BYTE( "d51-07.51", 0x000003, 0x200000, CRC(24b2f97d) SHA1(6980e67b435d189ce897c0301e0411763410ab47) )

	ROM_REGION( 0x400000, REGION_GFX3, ROMREGION_DISPOSE )
	ROM_LOAD16_BYTE( "d51-10.95", 0x000000, 0x100000, CRC(d5910604) SHA1(8efe13884cfdef208394ddfe19f43eb1b9f78ff3) )	/* PIV 8x8 tiles, 6bpp */
	ROM_LOAD16_BYTE( "d51-11.96", 0x000001, 0x100000, CRC(fee5f5c6) SHA1(1be88747f9c71c348dd61a8f0040007df3a3e6a6) )
	ROM_LOAD       ( "d51-12.97", 0x300000, 0x100000, CRC(d630287b) SHA1(2fa09e1821b7280d193ca9a2a270759c3c3189d1) )
	ROM_FILL       (              0x200000, 0x100000, 0 )

	ROM_REGION16_LE( 0x80000, REGION_USER1, 0 )
	ROM_LOAD16_WORD( "d51-13.7", 0x00000,  0x80000,  CRC(36921b8b) SHA1(2130120f78a3b984618a53054fc937cf727177b9) )	/* STY, spritemap */

	ROM_REGION16_BE( 0x1000000, REGION_SOUND1, ROMREGION_SOUNDONLY | ROMREGION_ERASE00 )
	ROM_LOAD16_BYTE( "d51-01.73", 0x000000, 0x200000, CRC(92f09155) SHA1(8015e1997818bb480174394eb43840bf26679bcf) )	/* Ensoniq samples */
	ROM_LOAD16_BYTE( "d51-02.74", 0xc00000, 0x200000, CRC(20a9428f) SHA1(c9033d02a49c72f704808f5f899101617d5814e5) )
ROM_END


static READ32_HANDLER( irq_speedup_r_groundfx )					
{																
	int ptr;													
	if ((activecpu_get_sp()&2)==0) ptr=groundfx_ram[(activecpu_get_sp()&0x1ffff)/4];	
	else ptr=(((groundfx_ram[(activecpu_get_sp()&0x1ffff)/4])&0x1ffff)<<16) | 
	(groundfx_ram[((activecpu_get_sp()&0x1ffff)/4)+1]>>16); 

	if (activecpu_get_pc()==0x1ece && ptr==0x1b9a)					
		cpu_spinuntil_int();			
	
	return groundfx_ram[0xb574/4];									
}


DRIVER_INIT( groundfx )
{
	unsigned int offset,i;
	UINT8 *gfx = memory_region(REGION_GFX3);
	int size=memory_region_length(REGION_GFX3);
	int data;

	/* Speedup handlers */
	install_mem_read32_handler(0, 0x20b574, 0x20b577, irq_speedup_r_groundfx);

	/* make piv tile GFX format suitable for gfxdecode */
	offset = size/2;
	for (i = size/2+size/4; i<size; i++)
	{
		int d1,d2,d3,d4;

		/* Expand 2bits into 4bits format */
		data = gfx[i];
		d1 = (data>>0) & 3;
		d2 = (data>>2) & 3;
		d3 = (data>>4) & 3;
		d4 = (data>>6) & 3;

		gfx[offset] = (d1<<2) | (d2<<6);
		offset++;

		gfx[offset] = (d3<<2) | (d4<<6);
		offset++;
	}
}


GAME( 1992, groundfx, 0, groundfx, groundfx, groundfx, ROT0, "Taito Corporation", "Ground Effects / Super Ground Effects (Japan)" )
