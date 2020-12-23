/***************************************************************************

    IBM-PC printer interface

***************************************************************************/

#ifndef __ISA_LPT_H__
#define __ISA_LPT_H__

#include "isa.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> isa8_lpt_device

class isa8_lpt_device : public device_t,
	public device_isa8_card_interface
{
public:
	// construction/destruction
	isa8_lpt_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// optional information overrides
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;

	bool is_primary() { return m_is_primary; }

	WRITE_LINE_MEMBER(pc_cpu_line);

protected:
	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

private:

	// internal state
	bool m_is_primary;
};

// device type definition
extern const device_type ISA8_LPT;

#endif /* __ISA_LPT_H__ */
