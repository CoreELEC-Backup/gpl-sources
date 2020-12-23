// license:MAME|LGPL-2.1+
// copyright-holders:Michael Zapf
/****************************************************************************

    TI-99 P-Code Card emulation.

    The P-Code card is part of the UCSD p-System support for the TI-99
    computer family. This system is a comprehensive development system for
    creating, running, and debugging programs written in UCSD Pascal.

    The complete system consists of
    - P-Code card, plugged into the Peripheral Expansion Box (PEB)
    - Software on disk:
      + PHD5063: UCSD p-System Compiler
      + PHD5064: UCSD p-System Assembler/Linker
      + PHD5065: UCSD p-System Editor/Filer (2 disks)

    The card has a switch on the circuit board extending outside the PEB
    which allows to turn off the card without removing it. Unlike other
    expansion cards for the TI system, the P-Code card immediately takes
    over control after the system is turned on.

    When the p-System is booted, the screen turns cyan and remains empty.
    There are two beeps, a pause for about 15 seconds, another three beeps,
    and then a welcome text is displayed with a one-line menu at the screen
    top. (Delay times seem unrealistically short; the manual says
    30-60 seconds. To be checked.)
    Many of the functions require one of the disks be inserted in one
    of the disk drives. You can leave the p-System by waiting for the menu
    to appear, and typing H (halt). This returns you to the Master Title
    Screen, and the card is inactive until the system is reset.

    The P-Code card contains the P-Code interpreter which is somewhat
    comparable to today's Java virtual machine. Programs written for the
    p-System are interchangeable between different platforms.

    On the P-Code card we find 12 KiB of ROM, visible in the DSR memory area
    (>4000 to >5FFF). The first 4 KiB (>4000->4FFF) are from the 4732 ROM,
    the second and third 4 KiB (>5000->5FFF) are from a 4764 ROM, switched
    by setting the CRU bit 4 to 1 on the CRU base >1F00.

    CRU base >1F00
        Bit 0: Activate card
        Bit 4: Select bank 2 of the 4764 ROM (0 = bank 1)
        Bit 7: May be connected to an indicator LED which is by default
               wired to bit 0 (on the PCB)

    The lines are used in a slightly uncommon way: the three bits of the
    CRU bit address are A8, A13, and A14 (A15=LSB). Hence, bit 4 is at
    address >1F80, and bit 7 is at address >1F86. These bits are purely
    write-only.

    Moreover, the card contains 48 KiB of GROM memory, occupying the address
    space from G>0000 to G>FFFF in portions of 6KiB at every 8KiB boundary.

    Another specialty of the card is that the GROM contents are accessed via
    another GROM base address than what is used in the console:
    - >5BFC = read GROM data
    - >5BFE = read GROM address
    - >5FFC = write GROM data
    - >5FFE = write GROM address

    This makes the GROM memory "private" to the card; together with the
    rest of the ROM space the ports become invisible when the card is
    deactivated.

    Michael Zapf

    July 2009: First version
    September 2010: Rewritten as device
    February 2012: Rewritten as class

*****************************************************************************/

#include "pcode.h"

#define PCODE_GROM_TAG "pcode_grom"
#define PCODE_ROM_TAG "pcode_rom"

#define PGROM0_TAG "grom0"
#define PGROM1_TAG "grom1"
#define PGROM2_TAG "grom2"
#define PGROM3_TAG "grom3"
#define PGROM4_TAG "grom4"
#define PGROM5_TAG "grom5"
#define PGROM6_TAG "grom6"
#define PGROM7_TAG "grom7"

#define GROMMASK 0x1ffd
#define GROMREAD 0x1bfc
#define GROMWRITE 0x1ffc

#define ACTIVE_TAG "ACTIVE"

#define LOG logerror
#define VERBOSE 1

ti_pcode_card_device::ti_pcode_card_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
: ti_expansion_card_device(mconfig, TI99_P_CODE, "TI-99 P-Code Card", tag, owner, clock, "ti99_pcode", __FILE__)
{
}

READ8Z_MEMBER( ti_pcode_card_device::readz )
{
	if (m_switch && m_selected && ((offset & m_select_mask)==m_select_value))
	{
		// GROM access
		if ((offset & GROMMASK)==GROMREAD)
		{
			for (int i=0; i < 8; i++) m_grom[i]->readz(space, offset, value, mem_mask);
			if (VERBOSE>5) LOG("ti99_pcode: read from grom %04x: %02x\n", offset&0xffff, *value);
		}
		else
		{
			if ((offset & 0x1000) == 0x0000)
			{
				/* Accesses ROM 4732 (4K) */
				// 0000 xxxx xxxx xxxx
				*value = m_rom[offset & 0x0fff];
				if (VERBOSE>5) LOG("ti99_pcode: read from rom %04x: %02x\n", offset&0xffff, *value);
			}
			else
			{
				// Accesses ROM 4764 (2*4K)
				// We have two banks here which are activated according
				// to the setting of CRU bit 4
				// Bank 0 is the ROM above
				// 0001 xxxx xxxx xxxx   Bank 1
				// 0010 xxxx xxxx xxxx   Bank 2
				*value = m_rom[(m_bank_select<<12) | (offset & 0x0fff)];
				if (VERBOSE>5) LOG("ti99_pcode: read from rom %04x (%02x): %02x\n", offset&0xffff, m_bank_select, *value);
			}
		}
	}
}

/*
    Write a byte in P-Code ROM space. This is only used for setting the
    GROM address.
*/
WRITE8_MEMBER( ti_pcode_card_device::write )
{
	if (m_switch && m_selected)
	{
		if ((offset & m_select_mask)==m_select_value)
		{
			if (VERBOSE>5) LOG("ti99_pcode: write to address %04x: %02x\n", offset & 0xffff, data);
			// 0101 1111 1111 11x0
			if ((offset & GROMMASK) == GROMWRITE)
			{
				for (int i=0; i < 8; i++) m_grom[i]->write(space, offset, data, mem_mask);
			}
		}
	}
}

/*
    Common READY* line from the GROMs.
    At this time we do not emulate GROM READY* since the CPU emulation does
    not yet process READY*. If it did, however, we would have to do a similar
    handling as in peribox (with INTA*): The common READY* line is a logical
    AND of all single READY lines. If any GROM pulls it down, the line goes
    down, and only if all GROMs release it, it pulls up again. We should think
    about a general solution.
*/
WRITE_LINE_MEMBER( ti_pcode_card_device::ready_line )
{
	m_slot->set_ready(state);
}

/*
    CRU read handler. The P-Code card does not offer CRU read lines, so
    we just ignore any request. (Note that CRU lines are not like memory; you
    may be able to write to them, but not necessarily read them again.)
*/
READ8Z_MEMBER(ti_pcode_card_device::crureadz)
{
}

/*
    The CRU write handler.
    Bit 0 = activate card
    Bit 4 = select second bank of high ROM.

    Somewhat uncommon, the CRU address is created from address lines
    A8, A13, and A14 so bit 0 is at 0x1f00, but bit 4 is at 0x1f80. Accordingly,
    bit 7 would be 0x1f86 but it is not used.
*/
WRITE8_MEMBER(ti_pcode_card_device::cruwrite)
{
	if ((offset & 0xff00)==m_cru_base)
	{
		int addr = offset & 0x00ff;

		if (addr==0)
			m_selected = (data != 0);

		if (addr==0x80) // Bit 4 is on address line 8
		{
			m_bank_select = (data+1);   // we're calling this bank 1 and bank 2
			if (VERBOSE>5) LOG("ti99_pcode: select rom bank %d\n", m_bank_select);
		}
	}
}

static GROM_CONFIG(pgrom0_config)
{
	false, 0, PCODE_GROM_TAG, 0x0000, 0x1800, GROMFREQ
};
static GROM_CONFIG(pgrom1_config)
{
	false, 1, PCODE_GROM_TAG, 0x2000, 0x1800, GROMFREQ
};
static GROM_CONFIG(pgrom2_config)
{
	false, 2, PCODE_GROM_TAG, 0x4000, 0x1800, GROMFREQ
};
static GROM_CONFIG(pgrom3_config)
{
	false, 3, PCODE_GROM_TAG, 0x6000, 0x1800, GROMFREQ
};
static GROM_CONFIG(pgrom4_config)
{
	false, 4, PCODE_GROM_TAG, 0x8000, 0x1800, GROMFREQ
};
static GROM_CONFIG(pgrom5_config)
{
	false, 5, PCODE_GROM_TAG, 0xa000, 0x1800, GROMFREQ
};
static GROM_CONFIG(pgrom6_config)
{
	false, 6, PCODE_GROM_TAG, 0xc000, 0x1800, GROMFREQ
};
static GROM_CONFIG(pgrom7_config)
{
	false, 7, PCODE_GROM_TAG, 0xe000, 0x1800, GROMFREQ
};

void ti_pcode_card_device::device_start()
{
	m_cru_base = 0x1f00;
	m_grom[0] = static_cast<ti99_grom_device*>(subdevice(PGROM0_TAG));
	m_grom[1] = static_cast<ti99_grom_device*>(subdevice(PGROM1_TAG));
	m_grom[2] = static_cast<ti99_grom_device*>(subdevice(PGROM2_TAG));
	m_grom[3] = static_cast<ti99_grom_device*>(subdevice(PGROM3_TAG));
	m_grom[4] = static_cast<ti99_grom_device*>(subdevice(PGROM4_TAG));
	m_grom[5] = static_cast<ti99_grom_device*>(subdevice(PGROM5_TAG));
	m_grom[6] = static_cast<ti99_grom_device*>(subdevice(PGROM6_TAG));
	m_grom[7] = static_cast<ti99_grom_device*>(subdevice(PGROM7_TAG));
	m_rom = memregion(PCODE_ROM_TAG)->base();
}

void ti_pcode_card_device::device_reset()
{
	if (m_genmod)
	{
		m_select_mask = 0x1fe000;
		m_select_value = 0x174000;
	}
	else
	{
		m_select_mask = 0x7e000;
		m_select_value = 0x74000;
	}
	m_bank_select = 1;
	m_selected = false;

	m_switch = ioport(ACTIVE_TAG)->read();
}

void ti_pcode_card_device::device_config_complete()
{
}

INPUT_CHANGED_MEMBER( ti_pcode_card_device::switch_changed )
{
	if (VERBOSE>7) LOG("ti_pcode_card_device: switch changed to %d\n", newval);
	m_switch = (newval != 0);
}


MACHINE_CONFIG_FRAGMENT( ti99_pcode )
	MCFG_GROM_ADD( PGROM0_TAG, pgrom0_config )
	MCFG_GROM_READY_CALLBACK(WRITELINE(ti_pcode_card_device, ready_line))
	MCFG_GROM_ADD( PGROM1_TAG, pgrom1_config )
	MCFG_GROM_READY_CALLBACK(WRITELINE(ti_pcode_card_device, ready_line))
	MCFG_GROM_ADD( PGROM2_TAG, pgrom2_config )
	MCFG_GROM_READY_CALLBACK(WRITELINE(ti_pcode_card_device, ready_line))
	MCFG_GROM_ADD( PGROM3_TAG, pgrom3_config )
	MCFG_GROM_READY_CALLBACK(WRITELINE(ti_pcode_card_device, ready_line))
	MCFG_GROM_ADD( PGROM4_TAG, pgrom4_config )
	MCFG_GROM_READY_CALLBACK(WRITELINE(ti_pcode_card_device, ready_line))
	MCFG_GROM_ADD( PGROM5_TAG, pgrom5_config )
	MCFG_GROM_READY_CALLBACK(WRITELINE(ti_pcode_card_device, ready_line))
	MCFG_GROM_ADD( PGROM6_TAG, pgrom6_config )
	MCFG_GROM_READY_CALLBACK(WRITELINE(ti_pcode_card_device, ready_line))
	MCFG_GROM_ADD( PGROM7_TAG, pgrom7_config )
	MCFG_GROM_READY_CALLBACK(WRITELINE(ti_pcode_card_device, ready_line))
MACHINE_CONFIG_END

INPUT_PORTS_START( ti99_pcode )
	PORT_START( ACTIVE_TAG )
	PORT_DIPNAME( 0x01, 0x00, "P-Code activation switch" ) PORT_CHANGED_MEMBER(DEVICE_SELF, ti_pcode_card_device, switch_changed, 0)
		PORT_DIPSETTING( 0x00, DEF_STR( Off ) )
		PORT_DIPSETTING( 0x01, DEF_STR( On ) )
INPUT_PORTS_END

ROM_START( ti99_pcode )
	ROM_REGION(0x10000, PCODE_GROM_TAG, 0)
	ROM_LOAD("pcode_g0.bin", 0x0000, 0x10000, CRC(541b3860) SHA1(7be77c216737334ae997753a6a85136f117affb7)) /* TI P-Code card groms */
	ROM_REGION(0x3000, PCODE_ROM_TAG, 0)
	ROM_LOAD("pcode_r0.bin", 0x0000, 0x1000, CRC(3881d5b0) SHA1(a60e0468bb15ff72f97cf6e80979ca8c11ed0426)) /* TI P-Code card rom4732 */
	ROM_LOAD("pcode_r1.bin", 0x1000, 0x2000, CRC(46a06b8b) SHA1(24e2608179921aef312cdee6f455e3f46deb30d0)) /* TI P-Code card rom4764 */
ROM_END

machine_config_constructor ti_pcode_card_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( ti99_pcode );
}

const rom_entry *ti_pcode_card_device::device_rom_region() const
{
	return ROM_NAME( ti99_pcode );
}

ioport_constructor ti_pcode_card_device::device_input_ports() const
{
	return INPUT_PORTS_NAME( ti99_pcode );
}

const device_type TI99_P_CODE = &device_creator<ti_pcode_card_device>;
