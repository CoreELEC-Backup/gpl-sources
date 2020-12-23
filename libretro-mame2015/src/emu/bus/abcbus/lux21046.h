// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Luxor 55 21046-11/-21/-41 5.25"/8" Controller Card emulation

    Copyright MESS Team.
    Visit http://mamedev.org for licensing and usage restrictions.

*********************************************************************/

#pragma once

#ifndef __LUXOR_55_21046__
#define __LUXOR_55_21046__

#include "emu.h"
#include "abcbus.h"
#include "cpu/z80/z80.h"
#include "cpu/z80/z80daisy.h"
#include "formats/abc800_dsk.h"
#include "machine/wd_fdc.h"
#include "machine/z80dma.h"



//**************************************************************************
//  MACROS / CONSTANTS
//**************************************************************************

#define ADDRESS_ABC832          44
#define ADDRESS_ABC830          45
#define ADDRESS_ABC838          46


#define DRIVE_TEAC_FD55F        0x01
#define DRIVE_BASF_6138         0x02
#define DRIVE_MICROPOLIS_1015F  0x03
#define DRIVE_BASF_6118         0x04
#define DRIVE_MICROPOLIS_1115F  0x05
#define DRIVE_BASF_6106_08      0x08
#define DRIVE_MPI_51            0x09
#define DRIVE_BASF_6105         0x0e
#define DRIVE_BASF_6106         0x0f



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> luxor_55_21046_device

class luxor_55_21046_device :  public device_t,
								public device_abcbus_card_interface
{
public:
	// construction/destruction
	luxor_55_21046_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);
	luxor_55_21046_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual const rom_entry *device_rom_region() const;
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;

	// not really public
	DECLARE_READ8_MEMBER( out_r );
	DECLARE_WRITE8_MEMBER( inp_w );
	DECLARE_WRITE8_MEMBER( _4b_w );
	DECLARE_WRITE8_MEMBER( _9b_w );
	DECLARE_WRITE8_MEMBER( _8a_w );
	DECLARE_READ8_MEMBER( _9a_r );

	DECLARE_WRITE_LINE_MEMBER( dma_int_w );

	DECLARE_READ8_MEMBER(memory_read_byte);
	DECLARE_WRITE8_MEMBER(memory_write_byte);
	DECLARE_READ8_MEMBER(io_read_byte);
	DECLARE_WRITE8_MEMBER(io_write_byte);

	DECLARE_WRITE_LINE_MEMBER( fdc_intrq_w );

	DECLARE_FLOPPY_FORMATS( floppy_formats );

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	// device_abcbus_interface overrides
	virtual void abcbus_cs(UINT8 data);
	virtual int abcbus_csb();
	virtual UINT8 abcbus_inp();
	virtual void abcbus_out(UINT8 data);
	virtual UINT8 abcbus_stat();
	virtual void abcbus_c1(UINT8 data);
	virtual void abcbus_c3(UINT8 data);
	virtual void abcbus_c4(UINT8 data);

private:
	required_device<cpu_device> m_maincpu;
	required_device<z80dma_device> m_dma;
	required_device<fd1793_t> m_fdc;
	required_device<floppy_connector> m_floppy0;
	required_device<floppy_connector> m_floppy1;
	floppy_image_device *m_floppy;
	required_ioport m_sw1;
	required_ioport m_sw2;
	required_ioport m_sw3;

	bool m_cs;                  // card selected
	UINT8 m_status;             // ABC BUS status
	UINT8 m_out;                // ABC BUS data in
	UINT8 m_inp;                // ABC BUS data out
	bool m_fdc_irq;             // FDC interrupt
	int m_dma_irq;              // DMA interrupt
	int m_busy;                 // busy bit
	int m_force_busy;           // force busy bit
};


// ======================> abc830_device

class abc830_device :  public luxor_55_21046_device
{
public:
	// construction/destruction
	abc830_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;
};


// ======================> abc832_device

class abc832_device :  public luxor_55_21046_device
{
public:
	// construction/destruction
	abc832_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;
};


// ======================> abc834_device

class abc834_device :  public luxor_55_21046_device
{
public:
	// construction/destruction
	abc834_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;
};


// ======================> abc838_device

class abc838_device :  public luxor_55_21046_device
{
public:
	// construction/destruction
	abc838_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;
};


// ======================> abc850_floppy_device

class abc850_floppy_device :  public luxor_55_21046_device
{
public:
	// construction/destruction
	abc850_floppy_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;
};


// device type definition
extern const device_type LUXOR_55_21046;
extern const device_type ABC830;
extern const device_type ABC832;
extern const device_type ABC834;
extern const device_type ABC838;
extern const device_type ABC850_FLOPPY;



#endif
