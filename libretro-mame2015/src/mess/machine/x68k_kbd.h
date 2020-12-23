#ifndef X68K_KBD_H_
#define X68K_KBD_H_

#include "bus/rs232/keyboard.h"

class x68k_keyboard_device : public serial_keyboard_device
{
public:
	x68k_keyboard_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	virtual ioport_constructor device_input_ports() const;

protected:
	virtual void device_start();
	virtual void device_reset();
	virtual void rcv_complete();

private:
	virtual UINT8 keyboard_handler(UINT8 last_code, UINT8 *scan_line);
	void write(UINT8 data);

	required_ioport m_io_kbd8;
	required_ioport m_io_kbd9;
	required_ioport m_io_kbda;
	required_ioport m_io_kbdb;
	required_ioport m_io_kbdd;
	required_ioport m_io_kbde;

	int m_delay;  // keypress delay after initial press
	int m_repeat; // keypress repeat rate
	int m_enabled;  // keyboard enabled?

	UINT8 m_key_down[15*8];
	int m_repeat_code;
	int m_until_repeat;
};

extern const device_type X68K_KEYBOARD;

#endif /* X68KKBD_H_ */
