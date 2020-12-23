#ifndef __MSX_CART_HFOX_H
#define __MSX_CART_HFOX_H

#include "bus/msx_cart/cartridge.h"


extern const device_type MSX_CART_HFOX;


class msx_cart_hfox : public device_t
						, public msx_cart_interface
{
public:
	msx_cart_hfox(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	virtual void initialize_cartridge();

	virtual DECLARE_READ8_MEMBER(read_cart);
	virtual DECLARE_WRITE8_MEMBER(write_cart);

	void restore_banks();

private:
	UINT8 m_selected_bank[2];
	UINT8 *m_bank_base[2];
};


#endif
