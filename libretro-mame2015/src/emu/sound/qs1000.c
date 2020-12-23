/***************************************************************************

    qs1000.c

    QS1000 device emulator.

****************************************************************************

    The QS1000 is a 32-voice wavetable synthesizer, believed to be based on
    the OPTi 82C941. It contains an 8051 core, 256b of RAM and an (undumped)
    internal program ROM. The internal ROM can be bypassed in favour of an
    external ROM. Commands are issued to the chip via the 8051 serial port.

    The QS1000 can access 24Mb of sample ROM. To reduce demand on the CPU,
    instrument parameters such as playback rate, envelope and filter values
    are encoded in ROM and directly accessed by the wavetable engine.
    There are table entries for every note of every instrument.

    Registers
    =========

    [200] = Key on/off
            0 = Key on
            1 = ?
            2 = key off
    [201] = Address byte 0 (LSB)
    [202] = Address byte 1
    [203] = Address byte 2
    [204] = Pitch
    [205] = Pitch high byte? (Usually 0)
    [206] = Left volume
    [207] = Right volume
    [208] = Volume
    [209] = ?
    [20a] = ?
    [20b] = ?
    [20c] = ?
    [20d] = Velocity
    [20e] = Channel select
    [20f] = Modulation
    [210] = Modulation
    [211] = 0 - Select global registers?
            3 - Select channel registers?

    Velocity register values for MIDI range 0-127:

    01 01 01 01 01 01 01 02 02 03 03 04 04 05 05 06
    06 07 07 08 08 09 09 0A 0A 0B 0B 0C 0C 0D 0D 0E
    0E 0F 10 11 11 12 13 14 14 15 16 17 17 18 19 1A
    1A 1B 1C 1D 1D 1E 1F 20 20 21 22 23 23 24 25 26
    26 27 28 29 29 2A 2B 2C 2C 2D 2E 2F 2F 30 31 32
    35 38 3B 3E 41 44 47 4A 4D 50 4F 51 52 53 54 56
    57 58 59 5B 5C 5D 5E 60 61 62 63 65 66 67 6A 6B
    6C 6E 6F 70 71 73 74 75 76 78 79 7A 7B 7D 7E 7F

    (TODO: Other register values)

    This is the sequence of register writes used to play the Iron Fortress credit sound:

    [211] 0     Select global registers?
    [200] 1     ?
    [203] d6    Address byte 2
    [202] a9    Address byte 1
    [201] 1     Address byte 0
    [204] 80    Pitch
    [205] 0     ?
    [206] 80    Left volume
    [207] 80    Right volume
    [208] b3    Volume
    [209] 0     ?
    [20a] ff    ?
    [20b] 0     ?
    [20c] 0     ?
    [20d] 78    Velocity
    [211] 3     Select channel registers
    [20e] 0     Select channel
    [200] 0     Key on


    Sound Headers
    =============

    The address registers point to a 6 byte entry in the sound ROM:

    [019be0]
    097b 397f 1510
    ^    ^    ^
    |    |    |
    |    |    +----- Sound descriptor pointer
    |    +---------- ?
    +--------------- Playback frequency (fixed point value representing 24MHz clock periods)

    This in turn points to a 24 byte descriptor:

    [1510]:
    0 4502D 4508E 45F91 D0 7F 0F 2A 1F 90 00 FF
    ^ ^     ^     ^     ^  ^  ^  ^  ^  ^  ^  ^
    | |     |     |     |  |  |  |  |  |  |  |
    | |     |     |     |  |  |  |  |  |  |  +-- ?
    | |     |     |     |  |  |  |  |  |  +----- ?
    | |     |     |     |  |  |  |  |  +-------- ?
    | |     |     |     |  |  |  |  +----------- ?
    | |     |     |     |  |  |  +-------------- ?
    | |     |     |     |  |  +----------------- Bit 7: Format (0:PCM 1:ADPCM)
    | |     |     |     |  +-------------------- ?
    | |     |     |     +----------------------- ?
    | |     |     +----------------------------- Loop end address
    | |     +----------------------------------- Loop start address
    | +----------------------------------------- Start address
    +------------------------------------------- Address most-significant nibble (shared with loop addresses)

    * The unknown parameters are most likely envelope and filter parameters.
    * Is there a loop flag or do sounds loop indefinitely until stopped?


    TODO:
    * Looping is currently disabled
    * Figure out unknown sound header parameters
    * Figure out and implement envelopes and filters
    * Pitch bending
    * Dump the internal ROM

***************************************************************************/
#include "emu.h"
#include "qs1000.h"


#define LOGGING_ENABLED     0


// device type definition
const device_type QS1000 = &device_creator<qs1000_device>;

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

static ADDRESS_MAP_START( qs1000_prg_map, AS_PROGRAM, 8, qs1000_device )
	AM_RANGE(0x0000, 0x7fff) AM_ROM
ADDRESS_MAP_END


static ADDRESS_MAP_START( qs1000_io_map, AS_IO, 8, qs1000_device )
	AM_RANGE(0x0000, 0x00ff) AM_RAM
	AM_RANGE(0x0200, 0x0211) AM_WRITE(wave_w)
	AM_RANGE(MCS51_PORT_P1, MCS51_PORT_P1) AM_READWRITE(p1_r, p1_w)
	AM_RANGE(MCS51_PORT_P2, MCS51_PORT_P2) AM_READWRITE(p2_r, p2_w)
	AM_RANGE(MCS51_PORT_P3, MCS51_PORT_P3) AM_READWRITE(p3_r, p3_w)
ADDRESS_MAP_END


// Machine fragment
static MACHINE_CONFIG_FRAGMENT( qs1000 )
	MCFG_CPU_ADD("cpu", I8052, DERIVED_CLOCK(1, 1))
	MCFG_CPU_PROGRAM_MAP(qs1000_prg_map)
	MCFG_CPU_IO_MAP(qs1000_io_map)
MACHINE_CONFIG_END


// ROM definition for the QS1000 internal program ROM
ROM_START( qs1000 )
	ROM_REGION( 0x10000, "cpu", 0 )
	ROM_LOAD_OPTIONAL( "qs1000.bin", 0x0000, 0x10000, NO_DUMP )
ROM_END


// Wavetable ROM address map
static ADDRESS_MAP_START( qs1000, AS_0, 8, qs1000_device )
	AM_RANGE(0x000000, 0xffffff) AM_ROM AM_REGION("qs1000", 0)
ADDRESS_MAP_END


//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  qs1000_device - constructor
//-------------------------------------------------
qs1000_device::qs1000_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, QS1000, "QS1000", tag, owner, clock, "qs1000", __FILE__),
		device_sound_interface(mconfig, *this),
		device_memory_interface(mconfig, *this),
		m_external_rom(false),
		m_in_p1_cb(*this),
		m_in_p2_cb(*this),
		m_in_p3_cb(*this),
		m_out_p1_cb(*this),
		m_out_p2_cb(*this),
		m_out_p3_cb(*this),
		//m_serial_w_cb(*this),
		m_space_config("samples", ENDIANNESS_LITTLE, 8, 24, 0, NULL),
		m_stream(NULL),
		m_direct(NULL),
		m_cpu(*this, "cpu")
{
	m_address_map[0] = *ADDRESS_MAP_NAME(qs1000);
}


//-------------------------------------------------
//  rom_region - return a pointer to the device's
//  internal ROM region
//-------------------------------------------------
const rom_entry *qs1000_device::device_rom_region() const
{
	return m_external_rom ? NULL : ROM_NAME( qs1000 );
}


//-------------------------------------------------
//  machine_config_additions - return a pointer to
//  the device's machine fragment
//-------------------------------------------------
machine_config_constructor qs1000_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( qs1000 );
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------
void qs1000_device::device_start()
{
	// Find our direct access
	m_direct = &space().direct();

	// The QS1000 operates at 24MHz. Creating a stream at that rate
	// would be overkill so we opt for a fraction of that rate which
	// gives reasonable results
	m_stream = stream_alloc(0, 2, clock() / 32);

	// Resolve CPU port callbacks
	m_in_p1_cb.resolve_safe(0);
	m_in_p2_cb.resolve_safe(0);
	m_in_p3_cb.resolve_safe(0);

	m_out_p1_cb.resolve_safe();
	m_out_p2_cb.resolve_safe();
	m_out_p3_cb.resolve_safe();

	//m_serial_w_cb.resolve_safe();

	m_cpu->i8051_set_serial_rx_callback(read8_delegate(FUNC(qs1000_device::data_to_i8052),this));

	save_item(NAME(m_serial_data_in));
	save_item(NAME(m_wave_regs));

	for (int i = 0; i < QS1000_CHANNELS; i++)
	{
		save_item(NAME(m_channels[i].m_acc), i);
		save_item(NAME(m_channels[i].m_adpcm_signal), i);
		save_item(NAME(m_channels[i].m_start), i);
		save_item(NAME(m_channels[i].m_addr), i);
		save_item(NAME(m_channels[i].m_adpcm_addr), i);
		save_item(NAME(m_channels[i].m_loop_start), i);
		save_item(NAME(m_channels[i].m_loop_end), i);
		save_item(NAME(m_channels[i].m_freq), i);
		save_item(NAME(m_channels[i].m_flags), i);
		save_item(NAME(m_channels[i].m_regs), i);
		save_item(NAME(m_channels[i].m_adpcm.m_signal), i);
		save_item(NAME(m_channels[i].m_adpcm.m_step), i);
	}
}


//-------------------------------------------------
//  serial_in - send data to the chip
//-------------------------------------------------
void qs1000_device::serial_in(UINT8 data)
{
	m_serial_data_in = data;

	// Signal to the CPU that data is available
	m_cpu->set_input_line(MCS51_RX_LINE, ASSERT_LINE);
	m_cpu->set_input_line(MCS51_RX_LINE, CLEAR_LINE);
}


//-------------------------------------------------
//  set_irq - interrupt the internal CPU
//-------------------------------------------------
void qs1000_device::set_irq(int state)
{
	// Signal to the CPU that data is available
	m_cpu->set_input_line(MCS51_INT1_LINE, state ? ASSERT_LINE : CLEAR_LINE);
}


//-------------------------------------------------
//  data_to_i8052 - called by the 8052 core to
//  receive serial data
//-------------------------------------------------
READ8_MEMBER(qs1000_device::data_to_i8052)
{
	return m_serial_data_in;
}


//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------
void qs1000_device::device_reset()
{
	for (int ch = 0; ch < QS1000_CHANNELS; ++ch)
	{
		m_channels[ch].m_flags = 0;
	}
}


//-------------------------------------------------
//  memory_space_config - return a description of
//  any address spaces owned by this device
//-------------------------------------------------
const address_space_config *qs1000_device::memory_space_config(address_spacenum spacenum) const
{
	return (spacenum == 0) ? &m_space_config : NULL;
}


//-------------------------------------------------
//  device_timer - handle deferred writes and
//  resets as a timer callback
//-------------------------------------------------
void qs1000_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
}


//-------------------------------------------------
//  p0_r
//-------------------------------------------------
READ8_MEMBER( qs1000_device::p0_r )
{
	return 0xff;
}


//-------------------------------------------------
//  p1_r
//-------------------------------------------------
READ8_MEMBER( qs1000_device::p1_r )
{
	return m_in_p1_cb(0);
}


//-------------------------------------------------
//  p2_r
//-------------------------------------------------
READ8_MEMBER( qs1000_device::p2_r )
{
	return m_in_p2_cb(0);
}


//-------------------------------------------------
//  p3_r
//-------------------------------------------------
READ8_MEMBER( qs1000_device::p3_r )
{
	return m_in_p3_cb(0);
}


//-------------------------------------------------
//  p0_w
//-------------------------------------------------
WRITE8_MEMBER( qs1000_device::p0_w )
{
}


//-------------------------------------------------
//  p1_w
//-------------------------------------------------

WRITE8_MEMBER( qs1000_device::p1_w )
{
	m_out_p1_cb((offs_t)0, data);
}


//-------------------------------------------------
//  p2_w
//-------------------------------------------------

WRITE8_MEMBER( qs1000_device::p2_w )
{
	m_out_p2_cb((offs_t)0, data);
}


//-------------------------------------------------
//  p3_w
//-------------------------------------------------

WRITE8_MEMBER( qs1000_device::p3_w )
{
	m_out_p3_cb((offs_t)0, data);
}


//-------------------------------------------------
//  wave_w - process writes to wavetable engine
//-------------------------------------------------

WRITE8_MEMBER( qs1000_device::wave_w )
{
	m_stream->update();

	if (LOGGING_ENABLED)
		printf("QS1000 W[%x] %x\n", 0x200 + offset, data);

	switch (offset)
	{
		case 0x00:
		{
			int ch = m_wave_regs[0xe];

			if (data == 0)
			{
				// TODO
				for (int i = 0; i < 16; ++i)
					m_channels[ch].m_regs[i] = m_wave_regs[i];

				// Key on
				start_voice(ch);
			}
			if (data == 1)
			{
				// ?
			}
			else if (data == 2)
			{
				// Key off
				m_channels[ch].m_flags &= ~QS1000_KEYON;
			}
			break;
		}

		case 0x01:
		case 0x02:
		case 0x03:
		case 0x04:
		case 0x05:
		case 0x06:
		case 0x07:
		case 0x08:
		case 0x09:
		case 0x0a:
		case 0x0b:
		case 0x0c:
		case 0x0d:
		{
			if (m_wave_regs[0x11] == 3)
			{
				// Channel-specific write?
				m_channels[m_wave_regs[0xe]].m_regs[offset] = data;
			}
			else
			{
				// Global write?
				m_wave_regs[offset] = data;
			}
			break;
		}

		default:
			m_wave_regs[offset] = data;
	}
}


//-------------------------------------------------
//  sound_stream_update -
//-------------------------------------------------
void qs1000_device::sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples)
{
	// Rset the output stream
	memset(outputs[0], 0x0, samples * sizeof(*outputs[0]));
	memset(outputs[1], 0x0, samples * sizeof(*outputs[1]));

	// Iterate over voices and accumulate sample data
	for (int ch = 0; ch < QS1000_CHANNELS; ch++)
	{
		qs1000_channel &chan = m_channels[ch];

		UINT8 lvol = chan.m_regs[6];
		UINT8 rvol = chan.m_regs[7];
		UINT8 vol  = chan.m_regs[8];

		if (chan.m_flags & QS1000_PLAYING)
		{
			if (chan.m_flags & QS1000_ADPCM)
			{
				for (int samp = 0; samp < samples; samp++)
				{
					if (chan.m_addr >= chan.m_loop_end)
					{
#if 0 // Looping disabled until envelopes work
						if (chan.m_flags & QS1000_KEYON)
						{
							chan.m_addr = chan.m_loop_start;
						}
						else
#endif
						{
							chan.m_flags &= ~QS1000_PLAYING;
							break;
						}
					}

					// Not too keen on this but it'll do for now
					while (chan.m_start + chan.m_adpcm_addr != chan.m_addr)
					{
						chan.m_adpcm_addr++;

						if (chan.m_start + chan.m_adpcm_addr >=  chan.m_loop_end)
							chan.m_adpcm_addr = chan.m_loop_start - chan.m_start;

						UINT8 data = m_direct->read_raw_byte(chan.m_start + (chan.m_adpcm_addr >> 1));
						UINT8 nibble = (chan.m_adpcm_addr & 1 ? data : data >> 4) & 0xf;
						chan.m_adpcm_signal = chan.m_adpcm.clock(nibble);
					}

					INT8 result = (chan.m_adpcm_signal >> 4);
					chan.m_acc += chan.m_freq;
					chan.m_addr = (chan.m_addr + (chan.m_acc >> 18)) & QS1000_ADDRESS_MASK;
					chan.m_acc &= ((1 << 18) - 1);

					outputs[0][samp] += (result * 4 * lvol * vol) >> 12;
					outputs[1][samp] += (result * 4 * rvol * vol) >> 12;
				}
			}
			else
			{
				for (int samp = 0; samp < samples; samp++)
				{
					if (chan.m_addr >= chan.m_loop_end)
					{
#if 0 // Looping disabled until envelopes work
						if (chan.m_flags & QS1000_KEYON)
						{
							chan.m_addr = chan.m_loop_start;
						}
						else
#endif
						{
							chan.m_flags &= ~QS1000_PLAYING;
							break;
						}
					}

					INT8 result = m_direct->read_raw_byte(chan.m_addr) - 128;

					chan.m_acc += chan.m_freq;
					chan.m_addr = (chan.m_addr + (chan.m_acc >> 18)) & QS1000_ADDRESS_MASK;
					chan.m_acc &= ((1 << 18) - 1);

					outputs[0][samp] += (result * lvol * vol) >> 12;
					outputs[1][samp] += (result * rvol * vol) >> 12;
				}
			}
		}
	}
}


void qs1000_device::start_voice(int ch)
{
	UINT32 table_addr = (m_channels[ch].m_regs[0x01] << 16) | (m_channels[ch].m_regs[0x02] << 8) | m_channels[ch].m_regs[0x03];

	// Fetch the sound information
	UINT16 freq = (m_direct->read_raw_byte(table_addr + 0) << 8) | m_direct->read_raw_byte(table_addr + 1);
	UINT16 word1 = (m_direct->read_raw_byte(table_addr + 2) << 8) | m_direct->read_raw_byte(table_addr + 3);
	UINT16 base = (m_direct->read_raw_byte(table_addr + 4) << 8) | m_direct->read_raw_byte(table_addr + 5);

	if (LOGGING_ENABLED)
		printf("[%.6x] Freq:%.4x  ????:%.4x  Addr:%.4x\n", table_addr, freq, word1, base);

	// See Raccoon World and Wyvern Wings NULL sound
	if (freq == 0)
		return;

	// Fetch the sample pointers and flags
	UINT8 byte0 = m_direct->read_raw_byte(base);

	UINT32 start_addr;

	start_addr  = byte0 << 16;
	start_addr |= m_direct->read_raw_byte(base + 1) << 8;
	start_addr |= m_direct->read_raw_byte(base + 2) << 0;
	start_addr &= QS1000_ADDRESS_MASK;

	UINT32 loop_start;

	loop_start = (byte0 & 0xf0) << 16;
	loop_start |= m_direct->read_raw_byte(base + 3) << 12;
	loop_start |= m_direct->read_raw_byte(base + 4) << 4;
	loop_start |= m_direct->read_raw_byte(base + 5) >> 4;
	loop_start &= QS1000_ADDRESS_MASK;

	UINT32 loop_end;

	loop_end = (byte0 & 0xf0) << 16;
	loop_end |= (m_direct->read_raw_byte(base + 5) & 0xf) << 16;
	loop_end |= m_direct->read_raw_byte(base + 6) << 8;
	loop_end |= m_direct->read_raw_byte(base + 7);
	loop_end &= QS1000_ADDRESS_MASK;

	UINT8 byte8 = m_direct->read_raw_byte(base + 8);

	if (LOGGING_ENABLED)
	{
		UINT8 byte9 = m_direct->read_raw_byte(base + 9);
		UINT8 byte10 = m_direct->read_raw_byte(base + 10);
		UINT8 byte11 = m_direct->read_raw_byte(base + 11);
		UINT8 byte12 = m_direct->read_raw_byte(base + 12);
		UINT8 byte13 = m_direct->read_raw_byte(base + 13);
		UINT8 byte14 = m_direct->read_raw_byte(base + 14);
		UINT8 byte15 = m_direct->read_raw_byte(base + 15);

		printf("[%.6x] Sample Start:%.6x  Loop Start:%.6x  Loop End:%.6x  Params: %.2x %.2x %.2x %.2x %.2x %.2x %.2x %.2x\n", base, start_addr, loop_start, loop_end, byte8, byte9, byte10, byte11, byte12, byte13, byte14, byte15);
	}

	m_channels[ch].m_acc = 0;
	m_channels[ch].m_start = start_addr;
	m_channels[ch].m_addr = start_addr;
	m_channels[ch].m_loop_start = loop_start;
	m_channels[ch].m_loop_end = loop_end;
	m_channels[ch].m_freq = freq;
	m_channels[ch].m_flags = QS1000_PLAYING | QS1000_KEYON;

	if (byte8 & 0x08)
	{
		m_channels[ch].m_adpcm.reset();
		m_channels[ch].m_adpcm_addr = -1;
//      m_channels[ch].m_adpcm_signal = -2;
		m_channels[ch].m_flags |= QS1000_ADPCM;
	}
}
