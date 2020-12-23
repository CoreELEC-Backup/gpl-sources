/***************************************************************************

Pong (c) 1972 Atari

driver by Couriersud

Notes:

TODO: please see netlist include files

***************************************************************************/

#include "emu.h"

#include "machine/netlist.h"
#include "netlist/devices/net_lib.h"
#include "sound/dac.h"
#include "video/fixfreq.h"
#include "astring.h"

/*
 * H count width to 512
 * Reset at 1C6 = 454
 * V count width to 512, counts on HReset
 * Reset at 105 = 261

 * Clock = 7.159 MHz

 * ==> 15.768 Khz Horz Freq
 * ==> 60.41 Refresh

 * HBlank 0 to 79
 * HSync 32 to 63
 * VBlank 0 to 15
 * VSync 4 to 7

 * Video = (HVID & VVID ) & (NET & PAD1 & PAD2)

 * Net at 256H alternating at 4V
 *
 *
 * http://www.youtube.com/watch?v=pDrRnJOCKZc
 */

#define MASTER_CLOCK    7159000
#define V_TOTAL         (0x105+1)       // 262
#define H_TOTAL         (0x1C6+1)       // 454

#define HBSTART                 (H_TOTAL)
#define HBEND                   (80)
#define VBSTART                 (V_TOTAL)
#define VBEND                   (16)

#if 0
fixedfreq_interface fixedfreq_mode_pongX2 = {
	MASTER_CLOCK * 2,
	(H_TOTAL-67) * 2, (H_TOTAL-40) * 2, (H_TOTAL-8) * 2, (H_TOTAL) * 2,
	V_TOTAL-22,V_TOTAL-19,V_TOTAL-16,V_TOTAL,
	1,  /* non-interlaced */
	0.31
};
#endif

enum input_changed_enum
{
	IC_PADDLE1,
	IC_PADDLE2,
	IC_COIN,
	IC_SWITCH,
	IC_VR1,
	IC_VR2
};

#if 0
#include "nl_pongd.inc"

#undef SRST
#undef VCC
#undef GND

#include "nl_pong.inc"
#endif

NETLIST_EXTERNAL(pongdoubles);
NETLIST_EXTERNAL(pong_fast);

class pong_state : public driver_device
{
public:
	pong_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
			m_maincpu(*this, "maincpu"),
			m_video(*this, "fixfreq"),

			m_dac(*this, "dac"),                /* just to have a sound device */
			m_sw1a(*this, "maincpu:sw1a"),
			m_sw1b(*this, "maincpu:sw1b")
	{
	}

	// devices
	required_device<netlist_mame_device_t> m_maincpu;
	required_device<fixedfreq_device> m_video;
	required_device<dac_device> m_dac; /* just to have a sound device */

	// sub devices
	required_device<netlist_mame_logic_input_t> m_sw1a;
	required_device<netlist_mame_logic_input_t> m_sw1b;

	//UINT32 screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

	DECLARE_INPUT_CHANGED_MEMBER(input_changed);

	NETDEV_ANALOG_CALLBACK_MEMBER(sound_cb)
	{
		//printf("snd %f\n", newval);
		//dac_w(m_dac, 0, newval*64);
		m_dac->write_unsigned8(64*data);
	}

protected:

	// driver_device overrides
	virtual void machine_start();
	virtual void machine_reset();

	virtual void video_start();

private:

};

static NETLIST_START(pong)

	NETLIST_MEMREGION("maincpu")

NETLIST_END()

void pong_state::machine_start()
{
}

void pong_state::machine_reset()
{
}


void pong_state::video_start()
{
}


INPUT_CHANGED_MEMBER(pong_state::input_changed)
{
	int numpad = (FPTR) (param);

	switch (numpad)
	{
	case IC_SWITCH:
		m_sw1a->write(newval ? 1 : 0);
		m_sw1b->write(newval ? 1 : 0);
		break;
	}
}

static INPUT_PORTS_START( pong )
	PORT_START( "PADDLE0" ) /* fake input port for player 1 paddle */
	PORT_BIT( 0xff, 0x00, IPT_PADDLE ) PORT_SENSITIVITY(2) PORT_KEYDELTA(100) PORT_CENTERDELTA(0)   NETLIST_ANALOG_PORT_CHANGED("maincpu", "pot0")

	PORT_START( "PADDLE1" ) /* fake input port for player 2 paddle */
	PORT_BIT( 0xff, 0x00, IPT_PADDLE ) PORT_SENSITIVITY(2) PORT_KEYDELTA(100) PORT_CENTERDELTA(0) PORT_PLAYER(2) NETLIST_ANALOG_PORT_CHANGED("maincpu", "pot1")

	PORT_START("IN0") /* fake as well */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )     NETLIST_LOGIC_PORT_CHANGED("maincpu", "coinsw")

	PORT_DIPNAME( 0x06, 0x00, "Game Won" )          PORT_DIPLOCATION("SW1A:1,SW1B:1") PORT_CHANGED_MEMBER(DEVICE_SELF, pong_state, input_changed, IC_SWITCH)
	PORT_DIPSETTING(    0x00, "11" )
	PORT_DIPSETTING(    0x06, "15" )

	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SERVICE )  PORT_NAME("Antenna") NETLIST_LOGIC_PORT_CHANGED("maincpu", "antenna")

	PORT_START("VR1")
	PORT_ADJUSTER( 50, "VR1 - 50k, Paddle 1 adjustment" )   NETLIST_ANALOG_PORT_CHANGED("maincpu", "vr0")
	PORT_START("VR2")
	PORT_ADJUSTER( 50, "VR2 - 50k, Paddle 2 adjustment" )   NETLIST_ANALOG_PORT_CHANGED("maincpu", "vr1")

INPUT_PORTS_END

static INPUT_PORTS_START( pongd )
	PORT_START( "PADDLE0" ) /* fake input port for player 1 paddle */
	PORT_BIT( 0xff, 0x00, IPT_PADDLE ) PORT_SENSITIVITY(2) PORT_KEYDELTA(100) PORT_CENTERDELTA(0)   NETLIST_ANALOG_PORT_CHANGED("maincpu", "pot0")

	PORT_START( "PADDLE1" ) /* fake input port for player 2 paddle */
	PORT_BIT( 0xff, 0x00, IPT_PADDLE ) PORT_SENSITIVITY(2) PORT_KEYDELTA(100) PORT_CENTERDELTA(0) PORT_PLAYER(2) NETLIST_ANALOG_PORT_CHANGED("maincpu", "pot1")

	PORT_START( "PADDLE2" ) /* fake input port for player 3 paddle */
	PORT_BIT( 0xff, 0x00, IPT_PADDLE ) PORT_SENSITIVITY(2) PORT_KEYDELTA(100) PORT_CENTERDELTA(0) PORT_PLAYER(3) NETLIST_ANALOG_PORT_CHANGED("maincpu", "pot2")

	PORT_START( "PADDLE3" ) /* fake input port for player 4 paddle */
	PORT_BIT( 0xff, 0x00, IPT_PADDLE ) PORT_SENSITIVITY(2) PORT_KEYDELTA(100) PORT_CENTERDELTA(0) PORT_PLAYER(4) NETLIST_ANALOG_PORT_CHANGED("maincpu", "pot3")

	PORT_START("IN0") /* fake as well */
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_COIN1 )     NETLIST_LOGIC_PORT_CHANGED("maincpu", "coinsw")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_START1 )     NETLIST_LOGIC_PORT_CHANGED("maincpu", "startsw")

#if 0
	PORT_DIPNAME( 0x06, 0x00, "Game Won" )          PORT_DIPLOCATION("SW1A:1,SW1B:1") PORT_CHANGED_MEMBER(DEVICE_SELF, pong_state, input_changed, IC_SWITCH)
	PORT_DIPSETTING(    0x00, "11" )
	PORT_DIPSETTING(    0x06, "15" )

	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_SERVICE )  PORT_NAME("Antenna") NETLIST_LOGIC_PORT_CHANGED("maincpu", "antenna")

	PORT_START("VR1")
	PORT_ADJUSTER( 50, "VR1 - 50k, Paddle 1 adjustment" )   NETLIST_ANALOG_PORT_CHANGED("maincpu", "vr0")
	PORT_START("VR2")
	PORT_ADJUSTER( 50, "VR2 - 50k, Paddle 2 adjustment" )   NETLIST_ANALOG_PORT_CHANGED("maincpu", "vr1")
#endif
INPUT_PORTS_END

static MACHINE_CONFIG_START( pong, pong_state )

	/* basic machine hardware */
	MCFG_DEVICE_ADD("maincpu", NETLIST_CPU, NETLIST_CLOCK)
	MCFG_NETLIST_SETUP(pong)

	MCFG_NETLIST_ANALOG_INPUT("maincpu", "vr0", "ic_b9_R.R")
	MCFG_NETLIST_ANALOG_MULT_OFFSET(1.0 / 100.0 * RES_K(50), RES_K(56) )
	MCFG_NETLIST_ANALOG_INPUT("maincpu", "vr1", "ic_a9_R.R")
	MCFG_NETLIST_ANALOG_MULT_OFFSET(1.0 / 100.0 * RES_K(50), RES_K(56) )
	MCFG_NETLIST_ANALOG_INPUT("maincpu", "pot0", "ic_b9_POT.DIAL")
	MCFG_NETLIST_ANALOG_INPUT("maincpu", "pot1", "ic_a9_POT.DIAL")
	MCFG_NETLIST_LOGIC_INPUT("maincpu", "sw1a", "sw1a.POS", 0, 0x01)
	MCFG_NETLIST_LOGIC_INPUT("maincpu", "sw1b", "sw1b.POS", 0, 0x01)
	MCFG_NETLIST_LOGIC_INPUT("maincpu", "coinsw", "coinsw.POS", 0, 0x01)
	MCFG_NETLIST_LOGIC_INPUT("maincpu", "antenna", "antenna.IN", 0, 0x01)

	MCFG_NETLIST_ANALOG_OUTPUT("maincpu", "snd0", "sound", pong_state, sound_cb, "")
	MCFG_NETLIST_ANALOG_OUTPUT("maincpu", "vid0", "videomix", fixedfreq_device, update_vid, "fixfreq")

	/* video hardware */
	MCFG_FIXFREQ_ADD("fixfreq", "screen")
	MCFG_FIXFREQ_MONITOR_CLOCK(MASTER_CLOCK)
	MCFG_FIXFREQ_HORZ_PARAMS(H_TOTAL-67,H_TOTAL-40,H_TOTAL-8,H_TOTAL)
	MCFG_FIXFREQ_VERT_PARAMS(V_TOTAL-22,V_TOTAL-19,V_TOTAL-12,V_TOTAL)
	MCFG_FIXFREQ_FIELDCOUNT(1)
	MCFG_FIXFREQ_SYNC_THRESHOLD(0.31)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("dac", DAC, 48000)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( pongf, pong )

	/* basic machine hardware */
	MCFG_DEVICE_MODIFY("maincpu")
	MCFG_NETLIST_SETUP(pong_fast)

MACHINE_CONFIG_END

static MACHINE_CONFIG_START( pongd, pong_state )

	/* basic machine hardware */
	MCFG_DEVICE_ADD("maincpu", NETLIST_CPU, NETLIST_CLOCK)
	MCFG_NETLIST_SETUP(pongdoubles)

#if 0
	MCFG_NETLIST_ANALOG_INPUT("maincpu", "vr0", "ic_b9_R.R")
	MCFG_NETLIST_ANALOG_INPUT_MULT_OFFSET(1.0 / 100.0 * RES_K(50), RES_K(56) )
	MCFG_NETLIST_ANALOG_INPUT("maincpu", "vr1", "ic_a9_R.R")
	MCFG_NETLIST_ANALOG_INPUT_MULT_OFFSET(1.0 / 100.0 * RES_K(50), RES_K(56) )
#endif
	MCFG_NETLIST_ANALOG_INPUT("maincpu", "pot0", "A10_POT.DIAL")
	MCFG_NETLIST_ANALOG_INPUT("maincpu", "pot1", "B10_POT.DIAL")
	MCFG_NETLIST_ANALOG_INPUT("maincpu", "pot2", "B9B_POT.DIAL")
	MCFG_NETLIST_ANALOG_INPUT("maincpu", "pot3", "B9A_POT.DIAL")
	MCFG_NETLIST_LOGIC_INPUT("maincpu", "sw1a", "DIPSW1.POS", 0, 0x01)
	MCFG_NETLIST_LOGIC_INPUT("maincpu", "sw1b", "DIPSW2.POS", 0, 0x01)
	MCFG_NETLIST_LOGIC_INPUT("maincpu", "coinsw", "COIN_SW.POS", 0, 0x01)
	MCFG_NETLIST_LOGIC_INPUT("maincpu", "startsw", "START_SW.POS", 0, 0x01)
#if 0
	MCFG_NETLIST_LOGIC_INPUT("maincpu", "antenna", "antenna.IN", 0, 0x01)
#endif

	MCFG_NETLIST_ANALOG_OUTPUT("maincpu", "snd0", "AUDIO", pong_state, sound_cb, "")
	MCFG_NETLIST_ANALOG_OUTPUT("maincpu", "vid0", "videomix", fixedfreq_device, update_vid, "fixfreq")

	/* video hardware */
	MCFG_FIXFREQ_ADD("fixfreq", "screen")
	MCFG_FIXFREQ_MONITOR_CLOCK(MASTER_CLOCK)
	MCFG_FIXFREQ_HORZ_PARAMS(H_TOTAL-67,H_TOTAL-52,H_TOTAL-8,H_TOTAL)
	MCFG_FIXFREQ_VERT_PARAMS(V_TOTAL-22,V_TOTAL-19,V_TOTAL-12,V_TOTAL)
	MCFG_FIXFREQ_FIELDCOUNT(1)
	MCFG_FIXFREQ_SYNC_THRESHOLD(0.31)

	/* sound hardware */
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("dac", DAC, 48000)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 1.0)

MACHINE_CONFIG_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( pong ) /* dummy to satisfy game entry*/
	ROM_REGION( 0x10000, "maincpu", 0 ) /* enough for netlist */
	ROM_LOAD( "pong.netlist", 0x000000, 0x00457f, CRC(72d5e4fe) SHA1(7bb15828223c34915c5e2869dd7917532a4bb7b4) )
ROM_END

ROM_START( pongf ) /* dummy to satisfy game entry*/
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASE00 )
ROM_END

ROM_START( pongd ) /* dummy to satisfy game entry*/
	ROM_REGION( 0x10000, "maincpu", ROMREGION_ERASE00 )
ROM_END

GAME( 1972, pong,   0, pong,  pong,  driver_device,  0, ROT0, "Atari", "Pong (Rev E) external [TTL]", GAME_SUPPORTS_SAVE)
GAME( 1972, pongf,  0, pongf, pong,  driver_device,  0, ROT0, "Atari", "Pong (Rev E) [TTL]", GAME_SUPPORTS_SAVE )
GAME( 1974, pongd,  0, pongd, pongd, driver_device,  0, ROT0, "Atari", "Pong Doubles [TTL]", GAME_SUPPORTS_SAVE )
