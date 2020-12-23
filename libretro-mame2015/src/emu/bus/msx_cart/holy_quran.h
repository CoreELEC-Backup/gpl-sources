#ifndef __MSX_CART_HOLY_QURAN_H
#define __MSX_CART_HOLY_QURAN_H

#include "bus/msx_cart/cartridge.h"


extern const device_type MSX_CART_HOLY_QURAN;


class msx_cart_holy_quran : public device_t
						, public msx_cart_interface
{
public:
	msx_cart_holy_quran(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual void device_reset();

	virtual void initialize_cartridge();

	virtual DECLARE_READ8_MEMBER(read_cart);
	virtual DECLARE_WRITE8_MEMBER(write_cart);

	void restore_banks();

private:
	UINT8 m_lookup_prot[256];
	UINT8 m_selected_bank[4];
	UINT8 *m_bank_base[4];
	bool m_decrypt;
};


#endif
