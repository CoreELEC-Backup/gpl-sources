#ifndef __SNS_EVENT_H
#define __SNS_EVENT_H

#include "snes_slot.h"
#include "cpu/upd7725/upd7725.h"


// ======================> sns_pfest94_device

class sns_pfest94_device : public device_t,
						public device_sns_cart_interface
{
public:
	// construction/destruction
	sns_pfest94_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual void device_reset();
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual ioport_constructor device_input_ports() const;

	required_device<upd7725_device> m_upd7725;
	required_ioport m_dsw;

	virtual void speedup_addon_bios_access();

	// reading and writing
	virtual DECLARE_READ8_MEMBER(read_l);
	virtual DECLARE_READ8_MEMBER(read_h);
	virtual DECLARE_READ8_MEMBER(chip_read);
	virtual DECLARE_WRITE8_MEMBER(chip_write);

	virtual DECLARE_READ32_MEMBER(necdsp_prg_r);
	virtual DECLARE_READ16_MEMBER(necdsp_data_r);

private:
	UINT8 m_base_bank;
	UINT8 m_mask;
	UINT8 m_status;
	UINT32 m_count;

	dynamic_array<UINT32> m_dsp_prg;
	dynamic_array<UINT16> m_dsp_data;

	static const device_timer_id TIMER_EVENT = 0;
	emu_timer *pfest94_timer;
};


// device type definition
extern const device_type SNS_PFEST94;

#endif
