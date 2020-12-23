#ifndef __NES_SUNSOFT_H
#define __NES_SUNSOFT_H

#include "nxrom.h"
#include "sound/ay8910.h"


// ======================> nes_sunsoft_1_device

class nes_sunsoft_1_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_sunsoft_1_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual DECLARE_WRITE8_MEMBER(write_m);

	virtual void pcb_reset();
};


// ======================> nes_sunsoft_2_device

class nes_sunsoft_2_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_sunsoft_2_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual DECLARE_WRITE8_MEMBER(write_h);

	virtual void pcb_reset();
};


// ======================> nes_sunsoft_3_device

class nes_sunsoft_3_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_sunsoft_3_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);
	virtual DECLARE_WRITE8_MEMBER(write_h);

	virtual void pcb_reset();

private:
	UINT16 m_irq_count;
	int m_irq_enable, m_irq_toggle;

	static const device_timer_id TIMER_IRQ = 0;
	emu_timer *irq_timer;
};


// ======================> nes_sunsoft_4_device

class nes_sunsoft_4_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_sunsoft_4_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);
	nes_sunsoft_4_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual DECLARE_READ8_MEMBER(read_m);
	virtual DECLARE_WRITE8_MEMBER(write_m);
	virtual DECLARE_WRITE8_MEMBER(sun4_write);
	virtual DECLARE_WRITE8_MEMBER(write_h) { sun4_write(space, offset, data, mem_mask); }

	virtual void pcb_reset();

protected:
	void sun4_mirror(int mirror, int mirr0, int mirr1);
	int m_reg, m_latch1, m_latch2, m_wram_enable;
};

// ======================> nes_sunsoft_fme7_device

class nes_sunsoft_fme7_device : public nes_nrom_device
{
public:
	// construction/destruction
	nes_sunsoft_fme7_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);
	nes_sunsoft_fme7_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	// device-level overrides
	virtual void device_start();
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);
	virtual DECLARE_READ8_MEMBER(read_m);
	virtual DECLARE_WRITE8_MEMBER(write_m);
	virtual DECLARE_WRITE8_MEMBER(fme7_write);
	virtual DECLARE_WRITE8_MEMBER(write_h) { fme7_write(space, offset, data, mem_mask); }

	virtual void pcb_reset();

private:
	UINT16 m_irq_count;
	int m_irq_enable;

	static const device_timer_id TIMER_IRQ = 0;
	emu_timer *irq_timer;

	UINT8 m_latch;
	UINT8 m_wram_bank;
};


// ======================> nes_sunsoft_5_device

class nes_sunsoft_5_device : public nes_sunsoft_fme7_device
{
public:
	// construction/destruction
	nes_sunsoft_5_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	virtual machine_config_constructor device_mconfig_additions() const;

	virtual DECLARE_WRITE8_MEMBER(write_h);

private:
	required_device<ay8910_device> m_ym2149;
};




// device type definition
extern const device_type NES_SUNSOFT_1;
extern const device_type NES_SUNSOFT_2;
extern const device_type NES_SUNSOFT_3;
extern const device_type NES_SUNSOFT_4;
extern const device_type NES_SUNSOFT_FME7;
extern const device_type NES_SUNSOFT_5;

#endif
