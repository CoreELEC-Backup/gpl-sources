#ifndef _NAOMIM1_H_
#define _NAOMIM1_H_

#include "naomibd.h"

#define MCFG_NAOMI_M1_BOARD_ADD(_tag, _eeprom_tag, _actel_tag, _irq_cb) \
	MCFG_NAOMI_BOARD_ADD(_tag, NAOMI_M1_BOARD, _eeprom_tag, _actel_tag, _irq_cb)

class naomi_m1_board : public naomi_board
{
public:
	naomi_m1_board(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual DECLARE_ADDRESS_MAP(submap, 16);

	DECLARE_READ16_MEMBER(actel_id_r);

protected:
	virtual void device_start();
	virtual void device_reset();

	virtual void board_setup_address(UINT32 address, bool is_dma);
	virtual void board_get_buffer(UINT8 *&base, UINT32 &limit);
	virtual void board_advance(UINT32 size);

private:
	enum { BUFFER_SIZE = 32768 };
	UINT32 key;

	UINT8 *buffer;
	UINT8 dict[111], hist[2];
	UINT64 avail_val;
	UINT32 rom_cur_address, buffer_actual_size, avail_bits;
	bool encryption, stream_ended, has_history;

	void gb_reset();
	UINT32 lookb(int bits);
	void skipb(int bits);
	UINT32 getb(int bits);

	void enc_reset();
	void enc_fill();

	UINT32 get_decrypted_32b();

	void wb(UINT8 byte);
};

extern const device_type NAOMI_M1_BOARD;

#endif
