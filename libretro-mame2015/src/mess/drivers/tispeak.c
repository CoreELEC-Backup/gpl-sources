// license:BSD-3-Clause
// copyright-holders:hap, Lord Nightmare
/***************************************************************************

  Texas Instruments 1st-gen. handheld speech devices,

  These devices, mostly edu-toys, are based around an MCU(TMS0270/TMS1100),
  TMS51xx speech, and VSM ROM(s). Newer devices, such as Speak & Music,
  are based around the TMP50C40 and belong in another driver, probably.


----------------------------------------------------------------------------

Known devices on this hardware: (* denotes not dumped, ** denotes pending dump)


ROM (and/or source) code obtained from patents:
Some of these may have pre-release bugs.

    Speak & Spell: US4189779
    Speak & Math: US4946391
    Touch & Tell: US4403965** (patent calls it "Speak & Seek")
    Language Translator: US4631748


Speak & Spell:

This is the original Speak & Spell. TI had done educational toys before, like
Wiz-A-Tron or Little Professor. But the popularity of this product was much
above expectations. TI continued to manufacture many products for this line.

    Speak & Spell (US), 1978
    - MCU: TMC0271*
    - TMS51xx(1/2): 16KB TMC0351NL
    - TMS51xx(2/2): 16KB TMC0352NL
    - notes: keyboard has buttons instead of cheap membrane

    Speak & Spell (US), 1979
    - MCU: TMC0271* (different from 1978 version)
    - TMS51xx(1/2): 16KB TMC0351N2L
    - TMS51xx(2/2): 16KB TMC0352N2L
    - notes: fixed a funny bug with gibberish-talk when Module button is pressed
      with no module inserted

    Speak & Spell (US), 1980
    - MCU: TMC0271* (same as 1979 version)
    - TMS51xx: 16KB CD2350 (rev.A)
    - notes: only 1 VSM, meaning much smaller internal vocabulary

    Speak & Spell (Japan), 1980
    - MCU: TMC0271* (assume same as US 1978 version)
    - TMS51xx(1/2): 16KB CD2321
    - TMS51xx(2/2): 16KB CD2322
    - notes: no local name for the product, words are in English but very low difficulty

    Speak & Spell (UK), 1978
    - MCU: TMC0271* (assume same as US 1978 version)
    - TMS51xx(1/2): 16KB CD2303
    - TMS51xx(2/2): 16KB CD2304
    - notes: voice data was manually altered to give it a UK accent,
      here's a small anecdote from developer:
          "(...) I cannot bear to listen the product even now. I remember the
           word 'butcher' took 3 days - I still don't know if it sounds right."

    Speak & Spell (UK), 1981
    - MCU: TMC0271* (assume same as US 1979 version)
    - TMS51xx: 16KB CD62175
    - notes: this one has a dedicated voice actor

    Speak & Spell (France) "La Dictee Magique", 1980
    - MCU: CD2702**
    - TMS51xx: 16KB CD2352

    Speak & Spell (Germany) "Buddy", 1980
    - MCU: CD2702** (same as French 1980 version)
    - TMS51xx(1/2): 16KB CD2345*
    - TMS51xx(2/2): 16KB CD2346*

    Speak & Spell (Italy) "Grillo Parlante", 1982
    - MCU: CD2702** (same as French 1980 version)
    - TMS51xx: 16KB? CD62190**

    Speak & Spell Compact (US), 1981
    - MCU: CD8011**
    - TMS51xx: 16KB CD2354
    - TMS51xx: 16KB CD2354A (rev.A)
    - notes: no display, MCU is TMS1100 instead of TMS0270

    Speak & Spell Compact (UK) "Speak & Write", 1981
    - MCU: CD8011** (same as US 1981 version)
    - TMS51xx: 16KB CD62174 (rev.A)
    - notes: anecdotes from the developer, the same person working on the original UK version:
          "We included a pencil and writing pad - it was now about 'writing'.",
      and one about the welcome message:
          "I had to manually create a sentence of digital speech from thin air.
           I had to write down a 20 character code which would create each 10/s
           sound bite that made up the phrase "Welcome to Speak and Write".
           It took me 1 week. (...) Even Larry Brantingham was amazed."

Speak & Spell modules:
Note that they are interchangeable, eg. you can use a French module on a US Speak & Spell.

    English:
    - Vowel Power: TMS51xx: 16KB CD2302
    - Number Stumpers 4-6: TMS51xx: 16KB CD2305
    - Number Stumpers 7-8: TMS51xx: 16KB CD2307 (rev.A)
    - Basic Builders: TMS51xx: 16KB CD2308
    - Mighty Verbs: TMS51xx: 16KB CD2309 (rev.B)
    - Homonym Heroes: TMS51xx: 16KB CD2310
    - Vowel Ventures: TMS51xx: 16KB CD2347 (rev.C)
    - Noun Endings: TMS51xx: 16KB CD2348
    - Magnificent Modifiers: TMS51xx: 16KB CD2349
    - E.T. Fantasy: TMS51xx: 16KB CD2360

    French:
    - No.1: Les Mots de Base: TMS51xx: 16KB CD2353 (1st release was called "Module No. 1 de Jacques Capelovici")
    - No.2: Les Mots Difficilies: TMS51xx: 16KB? CD62177*
    - No.3: Les Animaux Familiers: TMS51xx: 16KB? CD62047
    - No.4: Les Magasins De La Rue: TMS51xx: 16KB CD62048
    - No.5: Les Extra-Terrestres: TMS51xx: 16KB? CD62178*

    Italian:
    - Super Modulo: TMS51xx: 16KB? CD62313*


Speak & Math:

    Speak & Math (US), 1980 (renamed to "Speak & Maths" in UK, but is the same product)
    - MCU: CD2704*
    - TMS51xx(1/2): 16KB CD2392
    - TMS51xx(2/2): 16KB CD2393
    - notes: As with the Speak & Spell, the voice actor was a radio announcer.
      However, the phrase "is greater than or less than" had to be added in a
      hurry by one of the TI employees in a hurry, the day before a demo.
      Apparently QA never found out and it ended up in the final product.

    Speak & Math (US), 1986
    - MCU: CD2708
    - TMS51xx(1/2): 16KB CD2381
    - TMS51xx(2/2): 4KB CD2614

    Speak & Math 'Compact' (France) "Les Maths Magiques", 1986?
    - MCU: CP3447-NL* (TMS1100?)
    - CD2801: 16KB? CD62173*
    - notes: this is not the same as "Le Calcul Magique", that's from a
      series centered around a TMS50C40 instead of MCU+TMS51xx


Speak & Read:

    Speak & Read (US), 1980
    - MCU: CD2705**
    - TMS51xx(1/2): 16KB CD2394 (rev.A)
    - TMS51xx(2/2): 16KB CD2395 (rev.A)

Speak & Read modules:

    English:
    - Sea Sights: TMS51xx: 16KB CD2396 (rev.A)
    - Who's Who at the Zoo: TMS51xx: 16KB CD2397
    - A Dog on a Log: TMS51xx: 16KB CD3534 (rev.A)
    - The Seal That Could Fly: TMS51xx: 16KB CD3535*
    - A Ghost in the House: TMS51xx: 16KB CD3536*
    - On the Track: TMS51xx: 16KB CD3538
    - The Third Circle: TMS51xx: 16KB CD3539*
    - The Millionth Knight: TMS51xx: 16KB CD3540*


Touch & Tell:

    Touch & Tell (US), 1981
    - MCU: CD8012**
    - TMS51xx: 4KB CD2610
    - notes: MCU is TMS1100 instead of TMS0270. CD8010 is seen in some devices
      too, maybe an earlier version?

    Touch & Tell (UK), 1981
    - MCU: ?* (assume same as US version)
    - TMS51xx: ?KB CD62170*

    Touch & Tell (France) "Le Livre Magique", 1981
    - MCU: ?* (assume same as US version)
    - TMS51xx: ?KB CD62171*

    Touch & Tell (Germany) "Tipp & Sprich", 1981
    - MCU: ?* (assume same as US version)
    - TMS51xx: ?KB CD62172*

    Touch & Tell (Italy) "Libro Parlante", 1982
    - MCU: ?* (assume same as US version)
    - TMS51xx: ?KB CD62176*


Touch & Tell modules:

    English:
    - Animal Friends: CD2802: 16KB CD2355
    - World of Transportation: CD2802: 16KB CD2361
    - Little Creatures: CD2802: 16KB CD2362
    - E.T.: CD2802: 16KB CD2363**
    - Alphabet Fun: TMS51xx: 4KB CD2611
    - Number Fun: TMS51xx: 4KB CD2612
    - All About Me: TMS51xx: 4KB CD2613


Language Tutor/Translator:

A later device, called Language Teacher, was released without speech hardware.

    Language Tutor (US), 1978
    - MCU: TMC0275*
    - notes: external module is required (see below)

Language Tutor modules:

    - Ingles(1/4): TMS51xx: 16KB CD2311*
    - Ingles(2/4): TMS51xx: 16KB CD2312*
    - Ingles(3/4): TMS51xx: 16KB CD2313*
    - Ingles(4/4): TMS51xx: 16KB CD2314*

    - Spanish(1/4): TMS51xx: 16KB CD2315*
    - Spanish(2/4): TMS51xx: 16KB CD2316*
    - Spanish(3/4): TMS51xx: 16KB CD2317
    - Spanish(4/4): TMS51xx: 16KB CD2318

    - French(1/4): TMS51xx: 16KB CD2327
    - French(2/4): TMS51xx: 16KB CD2328
    - French(3/4): TMS51xx: 16KB CD2329
    - French(4/4): TMS51xx: 16KB CD2330

    - German(1/4): TMS51xx: 16KB CD2331
    - German(2/4): TMS51xx: 16KB CD2332
    - German(3/4): TMS51xx: 16KB CD2333
    - German(4/4): TMS51xx: 16KB CD2334

    - English(1/4): TMC0280: 16KB CD3526**
    - English(2/4): TMC0280: 16KB CD3527**
    - English(3/4): TMC0280: 16KB CD3528**
    - English(4/4): TMC0280: 16KB CD3529**


Other devices:

    Vocaid (US), 1982
    - MCU: CD8012**
    - CD2802: 16KB CD2357
    - notes: MCU is the same as in Touch & Tell, but instead of a toddler's toy,
      you get a serious medical aid device for the voice-impaired.

    Spelling B (US), 1978
    - MCU: TMC0272*
    - ?: TMC1984* (what is this?)
    - notes: this line of toys (Spelling B, Mr. Challenger, Math Marvel) is calculator-sized,
      might have been aimed for older kids. Note that Math Marvel is a TMC1986, no speech.

    Spelling B (US), newer
    - MCU: TMC0274*
    - TMS51xx: ?KB TMC0355 CD2602*

    Spelling B (Germany) "Spelling ABC", 198?
    - MCU: TMC0274* (assume same as US version)
    - TMS51xx: ?KB TMC0355 CD2607*

    Mr. Challenger (US), 1980
    - MCU: TMC0273*
    - TMS51xx: ?KB TMC0355 CD2601*


----------------------------------------------------------------------------

  TODO:
  - why doesn't lantutor work?
  - emulate other known devices


***************************************************************************/

#include "emu.h"
#include "cpu/tms0980/tms0980.h"
#include "sound/tms5110.h"
#include "machine/tms6100.h"
#include "bus/generic/slot.h"
#include "bus/generic/carts.h"

#include "lantutor.lh"
#include "snspell.lh"

// The master clock is a single stage RC oscillator into TMS5100 RCOSC:
// In an early 1979 Speak & Spell, C is 68pf, R is a 50kohm trimpot which is set to around 33.6kohm
// (measured in-circuit). CPUCLK is this osc freq /2, ROMCLK is this osc freq /4.
// The typical osc freq curve for TMS5100 is unknown. Let's assume it is set to the default frequency,
// which is 640kHz for 8KHz according to the TMS5100 documentation.

#define MASTER_CLOCK (640000)


class tispeak_state : public driver_device
{
public:
	tispeak_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_tms5100(*this, "tms5100"),
		m_tms6100(*this, "tms6100"),
		m_cart(*this, "cartslot"),
		m_button_matrix(*this, "IN"),
		m_display_wait(33),
		m_display_maxy(1),
		m_display_maxx(0)
	{ }

	// devices
	required_device<tms0270_cpu_device> m_maincpu;
	required_device<tms5100_device> m_tms5100;
	required_device<tms6100_device> m_tms6100;
	optional_device<generic_slot_device> m_cart;
	required_ioport_array<9> m_button_matrix;

	// misc common
	UINT16 m_r;                         // MCU R-pins data
	UINT16 m_o;                         // MCU O-pins data
	int m_power_on;
	int m_filament_on;

	// display common
	int m_display_wait;                 // led/lamp off-delay in microseconds (default 33ms)
	int m_display_maxy;                 // display matrix number of rows
	int m_display_maxx;                 // display matrix number of columns
	
	UINT32 m_display_state[0x20];	    // display matrix rows data
	UINT16 m_display_segmask[0x20];     // if not 0, display matrix row is a digit, mask indicates connected segments
	UINT32 m_display_cache[0x20];       // (internal use)
	UINT8 m_display_decay[0x20][0x20];  // (internal use)

	TIMER_DEVICE_CALLBACK_MEMBER(display_decay_tick);
	void display_update();
	void display_matrix_seg(int maxx, int maxy, UINT32 setx, UINT32 sety, UINT16 segmask);

	// cartridge
	UINT32 m_cart_max_size;
	UINT8* m_cart_base;
	DECLARE_DEVICE_IMAGE_LOAD_MEMBER(tispeak_cartridge);
	DECLARE_DRIVER_INIT(snspell);
	DECLARE_DRIVER_INIT(lantutor);

	DECLARE_READ8_MEMBER(snspell_read_k);
	DECLARE_WRITE16_MEMBER(snmath_write_o);
	DECLARE_WRITE16_MEMBER(snspell_write_o);
	DECLARE_WRITE16_MEMBER(snspell_write_r);
	DECLARE_WRITE16_MEMBER(lantutor_write_r);

	DECLARE_INPUT_CHANGED_MEMBER(power_button);
	void power_off();

	virtual void machine_reset();
	virtual void machine_start();
};



/***************************************************************************

  File Handling

***************************************************************************/

DEVICE_IMAGE_LOAD_MEMBER(tispeak_state, tispeak_cartridge)
{
	UINT32 size = m_cart->common_get_size("rom");

	if (size > m_cart_max_size)
	{
		image.seterror(IMAGE_ERROR_UNSPECIFIED, "Invalid file size");
		return IMAGE_INIT_FAIL;
	}

	m_cart->rom_alloc(size, GENERIC_ROM8_WIDTH, ENDIANNESS_LITTLE);
	m_cart->common_load_rom(m_cart->get_rom_base(), size, "rom");

	return IMAGE_INIT_PASS;
}


DRIVER_INIT_MEMBER(tispeak_state, snspell)
{
	m_cart_max_size = 0x4000;
	m_cart_base = memregion("tms6100")->base() + 0x8000;
}

DRIVER_INIT_MEMBER(tispeak_state, lantutor)
{
	m_cart_max_size = 0x10000;
	m_cart_base = memregion("tms6100")->base();
}



/***************************************************************************

  VFD Display

***************************************************************************/

// The device may strobe the outputs very fast, it is unnoticeable to the user.
// To prevent flickering here, we need to simulate a decay.

void tispeak_state::display_update()
{
	UINT32 active_state[0x20];

	for (int y = 0; y < m_display_maxy; y++)
	{
		active_state[y] = 0;

		for (int x = 0; x < m_display_maxx; x++)
		{
			// turn on powered segments
			if (m_power_on && m_filament_on && m_display_state[y] >> x & 1)
				m_display_decay[y][x] = m_display_wait;

			// determine active state
			int ds = (m_display_decay[y][x] != 0) ? 1 : 0;
			active_state[y] |= (ds << x);
		}
	}

	// on difference, send to output
	for (int y = 0; y < m_display_maxy; y++)
		if (m_display_cache[y] != active_state[y])
		{
			if (m_display_segmask[y] != 0)
				output_set_digit_value(y, active_state[y] & m_display_segmask[y]);

			const int mul = (m_display_maxx <= 10) ? 10 : 100;
			for (int x = 0; x < m_display_maxx; x++)
			{
				int state = active_state[y] >> x & 1;
				output_set_lamp_value(y * mul + x, state);

				// bit coords for svg2lay
				char buf[10];
				sprintf(buf, "%d.%d", y, x);
				output_set_value(buf, state);
			}
		}

	memcpy(m_display_cache, active_state, sizeof(m_display_cache));
}

TIMER_DEVICE_CALLBACK_MEMBER(tispeak_state::display_decay_tick)
{
	// slowly turn off unpowered segments
	for (int y = 0; y < m_display_maxy; y++)
		for (int x = 0; x < m_display_maxx; x++)
			if (m_display_decay[y][x] != 0)
				m_display_decay[y][x]--;
	
	display_update();
}

void tispeak_state::display_matrix_seg(int maxx, int maxy, UINT32 setx, UINT32 sety, UINT16 segmask)
{
	m_display_maxx = maxx;
	m_display_maxy = maxy;

	// update current state
	UINT32 colmask = (1 << maxx) - 1;
	for (int y = 0; y < maxy; y++)
	{
		m_display_segmask[y] &= segmask;
		m_display_state[y] = (sety >> y & 1) ? (setx & colmask) : 0;
	}
	
	display_update();
}



/***************************************************************************

  I/O

***************************************************************************/

// common/snspell

READ8_MEMBER(tispeak_state::snspell_read_k)
{
	// the Vss row is always on
	UINT8 k = m_button_matrix[8]->read();

	// read selected button rows
	for (int i = 0; i < 8; i++)
		if (m_r >> i & 1)
			k |= m_button_matrix[i]->read();

	return k;
}

WRITE16_MEMBER(tispeak_state::snspell_write_r)
{
	// R15: filament on
	m_filament_on = data & 0x8000;
	
	// R13: power-off request, on falling edge
	if ((m_r >> 13 & 1) && !(data >> 13 & 1))
		power_off();

	// R0-R7: input mux and select digit (+R8 if the device has 9 digits)
	// other bits: MCU internal use
	m_r = data & 0x21ff;
	display_matrix_seg(16, 16, m_o, m_r, 0x3fff);
}

WRITE16_MEMBER(tispeak_state::snspell_write_o)
{
	// reorder opla to led14seg, plus DP as d14 and AP as d15:
	// E,D,C,G,B,A,I,M,L,K,N,J,[AP],H,F,[DP] (sidenote: TI KLMN = MAME MLNK)
	m_o = BITSWAP16(data,12,15,10,7,8,9,11,6,13,3,14,0,1,2,4,5);
	display_matrix_seg(16, 16, m_o, m_r, 0x3fff);
}


void tispeak_state::power_off()
{
	m_maincpu->set_input_line(INPUT_LINE_RESET, ASSERT_LINE);
	m_tms5100->reset();
	m_tms6100->reset();

	m_power_on = 0;
}


// snmath specific

WRITE16_MEMBER(tispeak_state::snmath_write_o)
{
	// reorder opla to led14seg, plus DP as d14 and AP as d15:
	// [DP],D,C,H,F,B,I,M,L,K,N,J,[AP],E,G,A (sidenote: TI KLMN = MAME MLNK)
	m_o = BITSWAP16(data,12,0,10,7,8,9,11,6,3,14,4,13,1,2,5,15);
	display_matrix_seg(16, 16, m_o, m_r, 0x3fff);
}


// lantutor specific

WRITE16_MEMBER(tispeak_state::lantutor_write_r)
{
	// same as default, except R13 is used for an extra digit
	m_filament_on = data & 0x8000;
	m_r = data & 0x21ff;
	display_matrix_seg(16, 16, m_o, m_r, 0x3fff);
}



/***************************************************************************

  Inputs

***************************************************************************/

INPUT_CHANGED_MEMBER(tispeak_state::power_button)
{
	int on = (int)(FPTR)param;

	if (on && !m_power_on)
	{
		m_power_on = 1;
		m_maincpu->set_input_line(INPUT_LINE_RESET, CLEAR_LINE);
	}
	else if (!on && m_power_on)
		power_off();
}

static INPUT_PORTS_START( snspell )
	PORT_START("IN.0") // R0
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_A) PORT_CHAR('A')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_B) PORT_CHAR('B')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_C) PORT_CHAR('C')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_D) PORT_CHAR('D')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_E) PORT_CHAR('E')

	PORT_START("IN.1") // R1
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_F) PORT_CHAR('F')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_G) PORT_CHAR('G')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_H) PORT_CHAR('H')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_I) PORT_CHAR('I')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_J) PORT_CHAR('J')

	PORT_START("IN.2") // R2
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_K) PORT_CHAR('K')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_L) PORT_CHAR('L')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_M) PORT_CHAR('M')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_N) PORT_CHAR('N')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_O) PORT_CHAR('O')

	PORT_START("IN.3") // R3
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_P) PORT_CHAR('P')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Q) PORT_CHAR('Q')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_R) PORT_CHAR('R')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_S) PORT_CHAR('S')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_T) PORT_CHAR('T')

	PORT_START("IN.4") // R4
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_U) PORT_CHAR('U')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_V) PORT_CHAR('V')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_W) PORT_CHAR('W')
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_X) PORT_CHAR('X')
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Y) PORT_CHAR('Y')

	PORT_START("IN.5") // R5
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Z) PORT_CHAR('Z')
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_QUOTE) PORT_CHAR('\'')
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_0) PORT_NAME("Module")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_DEL) PORT_CODE(KEYCODE_BACKSPACE) PORT_NAME("Erase")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_ENTER) PORT_NAME("Enter")

	PORT_START("IN.6") // R6
	PORT_BIT( 0x1f, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("IN.7") // R7
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_PGDN) PORT_NAME("Off") // -> auto_power_off
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_1) PORT_NAME("Go")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_2) PORT_NAME("Replay")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_3) PORT_NAME("Repeat")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_4) PORT_NAME("Clue")

	PORT_START("IN.8") // Vss!
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_5) PORT_NAME("Mystery Word")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_6) PORT_NAME("Secret Code")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_7) PORT_NAME("Letter")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_8) PORT_NAME("Say It")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_9) PORT_CODE(KEYCODE_PGUP) PORT_NAME("Spell/On") PORT_CHANGED_MEMBER(DEVICE_SELF, tispeak_state, power_button, (void *)1)
INPUT_PORTS_END


static INPUT_PORTS_START( snmath )
	PORT_START("IN.0") // R0
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_0) PORT_CODE(KEYCODE_0_PAD) PORT_NAME("0")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_3) PORT_CODE(KEYCODE_3_PAD) PORT_NAME("3")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_6) PORT_CODE(KEYCODE_6_PAD) PORT_NAME("6")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_9) PORT_CODE(KEYCODE_9_PAD) PORT_NAME("9")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_DEL_PAD) PORT_NAME(".")

	PORT_START("IN.1") // R1
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_1) PORT_CODE(KEYCODE_1_PAD) PORT_NAME("1")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_4) PORT_CODE(KEYCODE_4_PAD) PORT_NAME("4")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_7) PORT_CODE(KEYCODE_7_PAD) PORT_NAME("7")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("IN.2") // R2
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_2) PORT_CODE(KEYCODE_2_PAD) PORT_NAME("2")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_5) PORT_CODE(KEYCODE_5_PAD) PORT_NAME("5")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_8) PORT_CODE(KEYCODE_8_PAD) PORT_NAME("8")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("IN.3") // R3
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_ENTER) PORT_CODE(KEYCODE_ENTER_PAD) PORT_NAME("Enter")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Q) PORT_NAME("Go")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_PGDN) PORT_NAME("Off") // -> auto_power_off
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("IN.4") // R4
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_DEL) PORT_CODE(KEYCODE_BACKSPACE) PORT_NAME("Clear")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_COMMA) PORT_NAME("<")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_STOP) PORT_NAME(">")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_W) PORT_NAME("Repeat")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("IN.5") // R5
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_PLUS_PAD) PORT_NAME("+")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_MINUS_PAD) PORT_NAME("-")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_ASTERISK) PORT_NAME(UTF8_MULTIPLY)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_SLASH_PAD) PORT_NAME(UTF8_DIVIDE) // /
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_E) PORT_NAME("Mix It")

	PORT_START("IN.6") // R6
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_I) PORT_NAME("Number Stumper")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_U) PORT_NAME("Write It")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_Y) PORT_NAME("Greater/Less")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_T) PORT_NAME("Word Problems")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_R) PORT_CODE(KEYCODE_PGUP) PORT_NAME("Solve It/On") PORT_CHANGED_MEMBER(DEVICE_SELF, tispeak_state, power_button, (void *)1)

	PORT_START("IN.7")
	PORT_BIT( 0x1f, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("IN.8")
	PORT_BIT( 0x1f, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END


static INPUT_PORTS_START( lantutor )
	PORT_INCLUDE( snspell )

	PORT_MODIFY("IN.5") // R5
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_MINUS) PORT_NAME("Diacritical")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_SPACE) PORT_NAME("Space")

	PORT_MODIFY("IN.6") // R6
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_1) PORT_NAME("1")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_2) PORT_NAME("2")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_3) PORT_NAME("3")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_4) PORT_NAME("4")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_5) PORT_NAME("5")

	PORT_MODIFY("IN.7") // R7
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_6) PORT_NAME("6")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_7) PORT_NAME("7")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_8) PORT_NAME("8")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_9) PORT_NAME("9")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_0) PORT_NAME("0")

	PORT_MODIFY("IN.8") // Vss!
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_COLON) PORT_NAME("Translate")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_OPENBRACE) PORT_NAME("Learn")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_CLOSEBRACE) PORT_NAME("Phrase")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_EQUALS) PORT_NAME("Link")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYBOARD ) PORT_CODE(KEYCODE_QUOTE) PORT_NAME("Repeat")
INPUT_PORTS_END



/***************************************************************************

  Machine Config

***************************************************************************/

void tispeak_state::machine_reset()
{
	m_power_on = 1;
}

void tispeak_state::machine_start()
{
	// zerofill
	memset(m_display_state, 0, sizeof(m_display_state));
	memset(m_display_cache, ~0, sizeof(m_display_cache));
	memset(m_display_decay, 0, sizeof(m_display_decay));
	memset(m_display_segmask, ~0, sizeof(m_display_segmask)); // !

	m_r = 0;
	m_o = 0;
	m_power_on = 0;
	m_filament_on = 0;

	// register for savestates
	save_item(NAME(m_display_maxy));
	save_item(NAME(m_display_maxx));
	save_item(NAME(m_display_wait));

	save_item(NAME(m_display_state));
	/* save_item(NAME(m_display_cache)); */ // don't save!
	save_item(NAME(m_display_decay));
	save_item(NAME(m_display_segmask));

	save_item(NAME(m_r));
	save_item(NAME(m_o));
	save_item(NAME(m_power_on));
	save_item(NAME(m_filament_on));

	// init cartridge
	if (m_cart != NULL && m_cart->exists())
	{
		astring region_tag;
		memory_region *src = memregion(region_tag.cpy(m_cart->tag()).cat(GENERIC_ROM_REGION_TAG));
		if (src)
			memcpy(m_cart_base, src->base(), src->bytes());
	}
}


static MACHINE_CONFIG_START( snmath, tispeak_state )

	/* basic machine hardware */
	MCFG_CPU_ADD("maincpu", TMS0270, MASTER_CLOCK/2)
	MCFG_TMS1XXX_READ_K_CB(READ8(tispeak_state, snspell_read_k))
	MCFG_TMS1XXX_WRITE_O_CB(WRITE16(tispeak_state, snmath_write_o))
	MCFG_TMS1XXX_WRITE_R_CB(WRITE16(tispeak_state, snspell_write_r))

	MCFG_TMS0270_READ_CTL_CB(DEVREAD8("tms5100", tms5100_device, ctl_r))
	MCFG_TMS0270_WRITE_CTL_CB(DEVWRITE8("tms5100", tms5100_device, ctl_w))
	MCFG_TMS0270_WRITE_PDC_CB(DEVWRITELINE("tms5100", tms5100_device, pdc_w))

	MCFG_TIMER_DRIVER_ADD_PERIODIC("display_decay", tispeak_state, display_decay_tick, attotime::from_msec(1))
	MCFG_DEFAULT_LAYOUT(layout_snspell) // max 9 digits

	/* no video! */

	/* sound hardware */
	MCFG_DEVICE_ADD("tms6100", TMS6100, MASTER_CLOCK/4)

	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("tms5100", TMS5100, MASTER_CLOCK)
	MCFG_TMS5110_M0_CB(DEVWRITELINE("tms6100", tms6100_device, tms6100_m0_w))
	MCFG_TMS5110_M1_CB(DEVWRITELINE("tms6100", tms6100_device, tms6100_m1_w))
	MCFG_TMS5110_ADDR_CB(DEVWRITE8("tms6100", tms6100_device, tms6100_addr_w))
	MCFG_TMS5110_DATA_CB(DEVREADLINE("tms6100", tms6100_device, tms6100_data_r))
	MCFG_TMS5110_ROMCLK_CB(DEVWRITELINE("tms6100", tms6100_device, tms6100_romclock_w))
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.5)
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( snspell, snmath )

	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_TMS1XXX_WRITE_O_CB(WRITE16(tispeak_state, snspell_write_o))

	/* cartridge */
	MCFG_GENERIC_CARTSLOT_ADD("cartslot", generic_plain_slot, "snspell")
	MCFG_GENERIC_EXTENSIONS("vsm")
	MCFG_GENERIC_LOAD(tispeak_state, tispeak_cartridge)

	MCFG_SOFTWARE_LIST_ADD("cart_list", "snspell")
MACHINE_CONFIG_END

static MACHINE_CONFIG_DERIVED( lantutor, snmath )

	/* basic machine hardware */
	MCFG_CPU_MODIFY("maincpu")
	MCFG_TMS1XXX_WRITE_O_CB(WRITE16(tispeak_state, snspell_write_o))
	MCFG_TMS1XXX_WRITE_R_CB(WRITE16(tispeak_state, lantutor_write_r))

	MCFG_DEFAULT_LAYOUT(layout_lantutor)

	/* cartridge */
	MCFG_GENERIC_CARTSLOT_ADD("cartslot", generic_plain_slot, "lantutor")
	MCFG_GENERIC_MANDATORY
	MCFG_GENERIC_EXTENSIONS("vsm,bin")
	MCFG_GENERIC_LOAD(tispeak_state, tispeak_cartridge)

	MCFG_SOFTWARE_LIST_ADD("cart_list", "lantutor")
MACHINE_CONFIG_END



/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( snspell )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "us4189779_tmc0271", 0x0000, 0x1000, BAD_DUMP CRC(d3f5a37d) SHA1(f75ab617a6067d4d3a954a9f86126d2089554df8) ) // typed in from patent 4189779, may have errors

	ROM_REGION( 1246, "maincpu:ipla", 0 )
	ROM_LOAD( "tms0980_default_ipla.pla", 0, 1246, CRC(42db9a38) SHA1(2d127d98028ec8ec6ea10c179c25e447b14ba4d0) )
	ROM_REGION( 2127, "maincpu:mpla", 0 )
	ROM_LOAD( "tms0270_cd2708_mpla.pla", 0, 2127, BAD_DUMP CRC(504b96bb) SHA1(67b691e7c0b97239410587e50e5182bf46475b43) ) // taken from cd2708, need to verify if it's same as tmc0271
	ROM_REGION( 1246, "maincpu:opla", 0 )
	ROM_LOAD( "tms0270_tmc0271_opla.pla", 0, 1246, CRC(9ebe12ab) SHA1(acb4e07ba26f2daca5f1c234885ac0371c7ce87f) )

	ROM_REGION( 0xc000, "tms6100", ROMREGION_ERASEFF ) // 8000-bfff = space reserved for cartridge
	ROM_LOAD( "tmc0351.vsm", 0x0000, 0x4000, CRC(beea3373) SHA1(8b0f7586d2f12c3d4a885fdb528cf23feffa1a3b) ) // cd2300
	ROM_LOAD( "tmc0352.vsm", 0x4000, 0x4000, CRC(d51f0587) SHA1(ddaa484be1bba5fef46b481cafae517e4acaa8ed) ) // cd2301
ROM_END

ROM_START( snspella )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "us4189779_tmc0271", 0x0000, 0x1000, BAD_DUMP CRC(d3f5a37d) SHA1(f75ab617a6067d4d3a954a9f86126d2089554df8) ) // placeholder, use the one we have

	ROM_REGION( 1246, "maincpu:ipla", 0 )
	ROM_LOAD( "tms0980_default_ipla.pla", 0, 1246, CRC(42db9a38) SHA1(2d127d98028ec8ec6ea10c179c25e447b14ba4d0) )
	ROM_REGION( 2127, "maincpu:mpla", 0 )
	ROM_LOAD( "tms0270_cd2708_mpla.pla", 0, 2127, BAD_DUMP CRC(504b96bb) SHA1(67b691e7c0b97239410587e50e5182bf46475b43) ) // placeholder, use the one we have
	ROM_REGION( 1246, "maincpu:opla", 0 )
	ROM_LOAD( "tms0270_tmc0271_opla.pla", 0, 1246, CRC(9ebe12ab) SHA1(acb4e07ba26f2daca5f1c234885ac0371c7ce87f) )

	ROM_REGION( 0xc000, "tms6100", ROMREGION_ERASEFF ) // uses only 1 rom, 8000-bfff = space reserved for cartridge
	ROM_LOAD( "cd2350a.vsm", 0x0000, 0x4000, CRC(2adda742) SHA1(3f868ed8284b723c815a30343057e03467c043b5) )
ROM_END

ROM_START( snspelluk )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "us4189779_tmc0271", 0x0000, 0x1000, BAD_DUMP CRC(d3f5a37d) SHA1(f75ab617a6067d4d3a954a9f86126d2089554df8) ) // placeholder, use the one we have

	ROM_REGION( 1246, "maincpu:ipla", 0 )
	ROM_LOAD( "tms0980_default_ipla.pla", 0, 1246, CRC(42db9a38) SHA1(2d127d98028ec8ec6ea10c179c25e447b14ba4d0) )
	ROM_REGION( 2127, "maincpu:mpla", 0 )
	ROM_LOAD( "tms0270_cd2708_mpla.pla", 0, 2127, BAD_DUMP CRC(504b96bb) SHA1(67b691e7c0b97239410587e50e5182bf46475b43) ) // taken from cd2708, need to verify if it's same as tmc0271
	ROM_REGION( 1246, "maincpu:opla", 0 )
	ROM_LOAD( "tms0270_tmc0271_opla.pla", 0, 1246, CRC(9ebe12ab) SHA1(acb4e07ba26f2daca5f1c234885ac0371c7ce87f) )

	ROM_REGION( 0xc000, "tms6100", ROMREGION_ERASEFF ) // 8000-bfff = space reserved for cartridge
	ROM_LOAD( "cd2303.vsm", 0x0000, 0x4000, CRC(0fae755c) SHA1(b68c3120a63a61db474feb5d71a6e5dd67910d80) )
	ROM_LOAD( "cd2304.vsm", 0x4000, 0x4000, CRC(e2a270eb) SHA1(c13c95ad15f1923a4841f66504e0f22646e71d99) )
ROM_END

ROM_START( snspelluka )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "us4189779_tmc0271", 0x0000, 0x1000, BAD_DUMP CRC(d3f5a37d) SHA1(f75ab617a6067d4d3a954a9f86126d2089554df8) ) // placeholder, use the one we have

	ROM_REGION( 1246, "maincpu:ipla", 0 )
	ROM_LOAD( "tms0980_default_ipla.pla", 0, 1246, CRC(42db9a38) SHA1(2d127d98028ec8ec6ea10c179c25e447b14ba4d0) )
	ROM_REGION( 2127, "maincpu:mpla", 0 )
	ROM_LOAD( "tms0270_cd2708_mpla.pla", 0, 2127, BAD_DUMP CRC(504b96bb) SHA1(67b691e7c0b97239410587e50e5182bf46475b43) ) // placeholder, use the one we have
	ROM_REGION( 1246, "maincpu:opla", 0 )
	ROM_LOAD( "tms0270_tmc0271_opla.pla", 0, 1246, CRC(9ebe12ab) SHA1(acb4e07ba26f2daca5f1c234885ac0371c7ce87f) )

	ROM_REGION( 0xc000, "tms6100", ROMREGION_ERASEFF ) // uses only 1 rom, 8000-bfff = space reserved for cartridge
	ROM_LOAD( "cd62175.vsm", 0x0000, 0x4000, CRC(6e1063d4) SHA1(b5c66c51148c5921ecb8ffccd7a460ae639cdb68) )
ROM_END

ROM_START( snspelljp )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "us4189779_tmc0271", 0x0000, 0x1000, BAD_DUMP CRC(d3f5a37d) SHA1(f75ab617a6067d4d3a954a9f86126d2089554df8) ) // placeholder, use the one we have

	ROM_REGION( 1246, "maincpu:ipla", 0 )
	ROM_LOAD( "tms0980_default_ipla.pla", 0, 1246, CRC(42db9a38) SHA1(2d127d98028ec8ec6ea10c179c25e447b14ba4d0) )
	ROM_REGION( 2127, "maincpu:mpla", 0 )
	ROM_LOAD( "tms0270_cd2708_mpla.pla", 0, 2127, BAD_DUMP CRC(504b96bb) SHA1(67b691e7c0b97239410587e50e5182bf46475b43) ) // taken from cd2708, need to verify if it's same as tmc0271
	ROM_REGION( 1246, "maincpu:opla", 0 )
	ROM_LOAD( "tms0270_tmc0271_opla.pla", 0, 1246, CRC(9ebe12ab) SHA1(acb4e07ba26f2daca5f1c234885ac0371c7ce87f) )

	ROM_REGION( 0xc000, "tms6100", ROMREGION_ERASEFF ) // 8000-bfff = space reserved for cartridge
	ROM_LOAD( "cd2321.vsm", 0x0000, 0x4000, CRC(ac010cce) SHA1(c0200d857b62be696248ac2d684a390c66ab0c31) )
	ROM_LOAD( "cd2322.vsm", 0x4000, 0x4000, CRC(b6f4bba4) SHA1(65d686a9385b5ef3f080a5f47c6b2418bb9455b0) )
ROM_END

ROM_START( ladictee )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "us4189779_tmc0271", 0x0000, 0x1000, BAD_DUMP CRC(d3f5a37d) SHA1(f75ab617a6067d4d3a954a9f86126d2089554df8) ) // placeholder, use the one we have

	ROM_REGION( 1246, "maincpu:ipla", 0 )
	ROM_LOAD( "tms0980_default_ipla.pla", 0, 1246, CRC(42db9a38) SHA1(2d127d98028ec8ec6ea10c179c25e447b14ba4d0) )
	ROM_REGION( 2127, "maincpu:mpla", 0 )
	ROM_LOAD( "tms0270_cd2708_mpla.pla", 0, 2127, BAD_DUMP CRC(504b96bb) SHA1(67b691e7c0b97239410587e50e5182bf46475b43) ) // placeholder, use the one we have
	ROM_REGION( 1246, "maincpu:opla", 0 )
	ROM_LOAD( "tms0270_tmc0271_opla.pla", 0, 1246, BAD_DUMP CRC(9ebe12ab) SHA1(acb4e07ba26f2daca5f1c234885ac0371c7ce87f) ) // placeholder, use the one we have

	ROM_REGION( 0xc000, "tms6100", ROMREGION_ERASEFF ) // uses only 1 rom, 8000-bfff = space reserved for cartridge
	ROM_LOAD( "cd2352.vsm", 0x0000, 0x4000, CRC(181a239e) SHA1(e16043766c385e152b7005c1c010be4c5fccdd9b) )
ROM_END


ROM_START( snmath )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "cd2708n2l", 0x0000, 0x1000, CRC(35937360) SHA1(69c362c75bb459056c09c7fab37c91040485474b) )

	ROM_REGION( 1246, "maincpu:ipla", 0 )
	ROM_LOAD( "tms0980_default_ipla.pla", 0, 1246, CRC(42db9a38) SHA1(2d127d98028ec8ec6ea10c179c25e447b14ba4d0) )
	ROM_REGION( 2127, "maincpu:mpla", 0 )
	ROM_LOAD( "tms0270_cd2708_mpla.pla", 0, 2127, CRC(504b96bb) SHA1(67b691e7c0b97239410587e50e5182bf46475b43) )
	ROM_REGION( 1246, "maincpu:opla", 0 )
	ROM_LOAD( "tms0270_cd2708_opla.pla", 0, 1246, CRC(1abad753) SHA1(53d20b519ed73ce248368047a056836afbe3cd46) )

	ROM_REGION( 0x8000, "tms6100", 0 )
	ROM_LOAD( "cd2381.vsm", 0x0000, 0x4000, CRC(f048dc81) SHA1(e97667d1002de40ab3d702c63b82311480032e0f) )
	ROM_LOAD( "cd2614.vsm", 0x4000, 0x1000, CRC(11989074) SHA1(0e9cf906de9bcdf4acb425535dc442846fc48fa2) )
	ROM_RELOAD(             0x5000, 0x1000 )
	ROM_RELOAD(             0x6000, 0x1000 )
	ROM_RELOAD(             0x7000, 0x1000 )
ROM_END

ROM_START( snmathp )
	ROM_REGION( 0x1000, "maincpu", 0 )
	// typed in from patent 4946391, verified with source code
	// BTANB note: Mix It does not work at all, this is an original bug in the prototype. There are probably other minor bugs too.
	ROM_LOAD( "us4946391_t2074", 0x0000, 0x1000, CRC(011f0c2d) SHA1(d2e14d72e03ca864abd51da78ffb71a9da82f624) )

	ROM_REGION( 1246, "maincpu:ipla", 0 )
	ROM_LOAD( "tms0980_default_ipla.pla", 0, 1246, CRC(42db9a38) SHA1(2d127d98028ec8ec6ea10c179c25e447b14ba4d0) )
	ROM_REGION( 2127, "maincpu:mpla", 0 )
	ROM_LOAD( "tms0270_cd2708_mpla.pla", 0, 2127, BAD_DUMP CRC(504b96bb) SHA1(67b691e7c0b97239410587e50e5182bf46475b43) ) // taken from cd2708, need to verify if it's same as cd2704
	ROM_REGION( 1246, "maincpu:opla", 0 )
	ROM_LOAD( "tms0270_cd2708_opla.pla", 0, 1246, BAD_DUMP CRC(1abad753) SHA1(53d20b519ed73ce248368047a056836afbe3cd46) ) // "

	ROM_REGION( 0x8000, "tms6100", 0 )
	ROM_LOAD( "cd2392.vsm", 0x0000, 0x4000, CRC(4ed2e920) SHA1(8896f29e25126c1e4d9a47c9a325b35dddecc61f) )
	ROM_LOAD( "cd2393.vsm", 0x4000, 0x4000, CRC(571d5b5a) SHA1(83284755d9b77267d320b5b87fdc39f352433715) )
ROM_END


ROM_START( lantutor )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "us4631748_tmc0275", 0x0000, 0x1000, CRC(22818845) SHA1(1a84f15fb18ca66b1f2bf7491d76fbc56068984d) ) // extracted visually from patent 4631748, verified with source code

	ROM_REGION( 1246, "maincpu:ipla", 0 )
	ROM_LOAD( "tms0980_default_ipla.pla", 0, 1246, CRC(42db9a38) SHA1(2d127d98028ec8ec6ea10c179c25e447b14ba4d0) )
	ROM_REGION( 2127, "maincpu:mpla", 0 )
	ROM_LOAD( "tms0270_cd2708_mpla.pla", 0, 2127, BAD_DUMP CRC(504b96bb) SHA1(67b691e7c0b97239410587e50e5182bf46475b43) ) // taken from cd2708, need to verify if it's same as tmc0275
	ROM_REGION( 1246, "maincpu:opla", 0 )
	ROM_LOAD( "tms0270_tmc0271_opla.pla", 0, 1246, BAD_DUMP CRC(9ebe12ab) SHA1(acb4e07ba26f2daca5f1c234885ac0371c7ce87f) ) // taken from snspell, mostly looks correct

	ROM_REGION( 0x10000, "tms6100", ROMREGION_ERASEFF ) // cartridge area
ROM_END



COMP( 1978, snspell,    0,       0, snspell,  snspell,  tispeak_state, snspell,  "Texas Instruments", "Speak & Spell (US, 1978 version/prototype)", GAME_SUPPORTS_SAVE | GAME_IMPERFECT_SOUND )
COMP( 1980, snspella,   snspell, 0, snspell,  snspell,  tispeak_state, snspell,  "Texas Instruments", "Speak & Spell (US, 1980 version)", GAME_SUPPORTS_SAVE | GAME_IMPERFECT_SOUND | GAME_NOT_WORKING ) // incomplete dump, uses prototype MCU ROM
COMP( 1978, snspelluk,  snspell, 0, snspell,  snspell,  tispeak_state, snspell,  "Texas Instruments", "Speak & Spell (UK, 1978 version)", GAME_SUPPORTS_SAVE | GAME_IMPERFECT_SOUND | GAME_NOT_WORKING ) // incomplete dump, uses prototype MCU ROM
COMP( 1981, snspelluka, snspell, 0, snspell,  snspell,  tispeak_state, snspell,  "Texas Instruments", "Speak & Spell (UK, 1981 version)", GAME_SUPPORTS_SAVE | GAME_IMPERFECT_SOUND | GAME_NOT_WORKING ) // incomplete dump, uses prototype MCU ROM
COMP( 1979, snspelljp,  snspell, 0, snspell,  snspell,  tispeak_state, snspell,  "Texas Instruments", "Speak & Spell (Japan)", GAME_SUPPORTS_SAVE | GAME_IMPERFECT_SOUND | GAME_NOT_WORKING ) // incomplete dump, uses prototype MCU ROM
COMP( 1980, ladictee,   snspell, 0, snspell,  snspell,  tispeak_state, snspell,  "Texas Instruments", "La Dictee Magique (France)", GAME_SUPPORTS_SAVE | GAME_IMPERFECT_SOUND | GAME_NOT_WORKING ) // doesn't work due to missing CD2702 MCU dump, German/Italian version has CD2702 too

COMP( 1986, snmath,     0,       0, snmath,   snmath,   driver_device, 0,        "Texas Instruments", "Speak & Math (US, 1986 version)", GAME_SUPPORTS_SAVE | GAME_IMPERFECT_SOUND )
COMP( 1980, snmathp,    snmath,  0, snmath,   snmath,   driver_device, 0,        "Texas Instruments", "Speak & Math (US, 1980 version/prototype)", GAME_SUPPORTS_SAVE | GAME_IMPERFECT_SOUND | GAME_NOT_WORKING )

COMP( 1979, lantutor,   0,       0, lantutor, lantutor, tispeak_state, lantutor, "Texas Instruments", "Language Tutor (prototype)", GAME_SUPPORTS_SAVE | GAME_IMPERFECT_SOUND | GAME_NOT_WORKING )
