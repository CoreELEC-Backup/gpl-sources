#ifndef __NES_MMCX_H
#define __NES_MMCX_H

#include "nes_slot.h"
#include "sound/samples.h"


// ======================> nes_nrom_device

class nes_nrom_device : public device_t,
						public device_nes_cart_interface
{
public:
	// construction/destruction
	nes_nrom_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);
	nes_nrom_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual void device_start() { common_start(); }

	virtual void pcb_reset();

	void common_start();
};


// ======================> nes_nrom368_device

class nes_nrom368_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_nrom368_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual DECLARE_READ8_MEMBER(read_l);
	virtual DECLARE_READ8_MEMBER(read_m);
	virtual DECLARE_READ8_MEMBER(read_h);
};


// ======================> nes_fcbasic_device

class nes_fcbasic_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_fcbasic_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// emulate the additional WRAM
};


// ======================> nes_axrom_device

class nes_axrom_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_axrom_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual DECLARE_WRITE8_MEMBER(write_h);

	virtual void pcb_reset();
};


// ======================> nes_bxrom_device

class nes_bxrom_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_bxrom_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual DECLARE_WRITE8_MEMBER(write_h);

	virtual void pcb_reset();
};


// ======================> nes_cnrom_device

class nes_cnrom_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_cnrom_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);
	nes_cnrom_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual DECLARE_READ8_MEMBER(chr_r);
	virtual DECLARE_WRITE8_MEMBER(write_h);

	virtual void pcb_reset();

private:
	UINT8 m_chr_open_bus;
};


// ======================> nes_cprom_device

class nes_cprom_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_cprom_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual DECLARE_WRITE8_MEMBER(write_h);

	virtual void pcb_reset();
};


// ======================> nes_gxrom_device

class nes_gxrom_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_gxrom_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual DECLARE_WRITE8_MEMBER(write_h);

	virtual void pcb_reset();
};


// ======================> nes_uxrom_device

class nes_uxrom_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_uxrom_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual DECLARE_WRITE8_MEMBER(write_h);

	virtual void pcb_reset();
};


// ======================> nes_uxrom_cc_device

class nes_uxrom_cc_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_uxrom_cc_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual DECLARE_WRITE8_MEMBER(write_h);

	virtual void pcb_reset();
};


// ======================> nes_un1rom_device

class nes_un1rom_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_un1rom_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual DECLARE_WRITE8_MEMBER(write_h);

	virtual void pcb_reset();
};


// ======================> nes_nochr_device

class nes_nochr_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_nochr_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual DECLARE_READ8_MEMBER(chr_r);
	virtual DECLARE_WRITE8_MEMBER(chr_w);
};



// device type definition
extern const device_type NES_NROM;
extern const device_type NES_NROM368;
extern const device_type NES_FCBASIC;
extern const device_type NES_AXROM;
extern const device_type NES_BXROM;
extern const device_type NES_CNROM;
extern const device_type NES_CPROM;
extern const device_type NES_GXROM;
extern const device_type NES_UXROM;
extern const device_type NES_UXROM_CC;
extern const device_type NES_UN1ROM;
extern const device_type NES_NOCHR;

#endif
