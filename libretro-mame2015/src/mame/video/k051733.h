#pragma once
#ifndef __K051733_H__
#define __K051733_H__

class k051733_device : public device_t
{
public:
	k051733_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~k051733_device() {}

	DECLARE_WRITE8_MEMBER( write );
	DECLARE_READ8_MEMBER( read );

protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();
private:
	// internal state
	UINT8    m_ram[0x20];
	UINT8    m_rng;
};

extern const device_type K051733;

#define MCFG_K051733_ADD(_tag) \
	MCFG_DEVICE_ADD(_tag, K051733, 0)

#endif
