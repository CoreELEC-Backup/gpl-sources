#ifndef PSXANALOG_H_
#define PSXANALOG_H_

#include "ctlrport.h"

extern const device_type PSX_DUALSHOCK;
extern const device_type PSX_ANALOG_JOYSTICK;

class psx_analog_controller_device :    public device_t,
										public device_psx_controller_interface
{
public:
	psx_analog_controller_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);

	virtual ioport_constructor device_input_ports() const;
	DECLARE_INPUT_CHANGED_MEMBER(change_mode);
protected:
	virtual void device_reset();
	virtual void device_start() {}
	enum {
		JOYSTICK,
		DUALSHOCK
	} m_type;
private:
	virtual bool get_pad(int count, UINT8 *odata, UINT8 idata);
	UINT8 pad_data(int count, bool analog);

	bool m_confmode;
	bool m_analogmode;
	bool m_analoglock;

	UINT8 m_temp;
	UINT8 m_cmd;

	required_ioport m_pad0;
	required_ioport m_pad1;
	required_ioport m_rstickx;
	required_ioport m_rsticky;
	required_ioport m_lstickx;
	required_ioport m_lsticky;
};

class psx_dualshock_device : public psx_analog_controller_device
{
public:
	psx_dualshock_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
};

class psx_analog_joystick_device : public psx_analog_controller_device
{
public:
	psx_analog_joystick_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
};

#endif /* PSXANALOG_H_ */
