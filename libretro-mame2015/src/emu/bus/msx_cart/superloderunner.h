#ifndef __MSX_CART_SUPERLODERUNNER_H
#define __MSX_CART_SUPERLODERUNNER_H

#include "bus/msx_cart/cartridge.h"


extern const device_type MSX_CART_SUPERLODERUNNER;


class msx_cart_superloderunner : public device_t
						, public msx_cart_interface
{
public:
	msx_cart_superloderunner(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();

	virtual void initialize_cartridge();

	virtual DECLARE_READ8_MEMBER(read_cart);

	DECLARE_WRITE8_MEMBER(banking);

	void restore_banks();

private:
	UINT8 m_selected_bank;
	UINT8 *m_bank_base;
};


#endif
