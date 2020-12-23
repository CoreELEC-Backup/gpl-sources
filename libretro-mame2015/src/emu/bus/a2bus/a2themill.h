/*********************************************************************

    a2themill.h

    Implementation of the Stellation Two The Mill 6809 card

*********************************************************************/

#ifndef __A2BUS_THEMILL__
#define __A2BUS_THEMILL__

#include "emu.h"
#include "a2bus.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class a2bus_themill_device:
	public device_t,
	public device_a2bus_card_interface
{
public:
	// construction/destruction
	a2bus_themill_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);
	a2bus_themill_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const;

	DECLARE_READ8_MEMBER( dma_r );
	DECLARE_WRITE8_MEMBER( dma_w );

protected:
	virtual void device_start();
	virtual void device_reset();

	// overrides of standard a2bus slot functions
	virtual UINT8 read_c0nx(address_space &space, UINT8 offset);
	virtual void write_c0nx(address_space &space, UINT8 offset, UINT8 data);
	virtual bool take_c800();

	required_device<cpu_device> m_6809;

private:
	bool m_bEnabled;
	bool m_flipAddrSpace;
	bool m_6809Mode;
	UINT8 m_status;
};

// device type definition
extern const device_type A2BUS_THEMILL;

#endif /* __A2BUS_THEMILL__ */
