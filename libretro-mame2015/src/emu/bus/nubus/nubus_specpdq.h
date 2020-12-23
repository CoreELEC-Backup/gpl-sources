#pragma once

#ifndef __NUBUS_SPECPDQ_H__
#define __NUBUS_SPECPDQ_H__

#include "emu.h"
#include "nubus.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> nubus_specpdq_device

class nubus_specpdq_device :
		public device_t,
		public device_video_interface,
		public device_nubus_card_interface
{
public:
		// construction/destruction
		nubus_specpdq_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
		nubus_specpdq_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);

		// optional information overrides
		virtual machine_config_constructor device_mconfig_additions() const;
		virtual const rom_entry *device_rom_region() const;
		virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);

		UINT32 screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
protected:
		// device-level overrides
		virtual void device_start();
		virtual void device_reset();

		DECLARE_READ32_MEMBER(specpdq_r);
		DECLARE_WRITE32_MEMBER(specpdq_w);
		DECLARE_READ32_MEMBER(vram_r);
		DECLARE_WRITE32_MEMBER(vram_w);

public:
		dynamic_buffer m_vram;
		UINT32 *m_vram32;
		UINT32 m_mode, m_vbl_disable;
		UINT32 m_palette_val[256], m_colors[3], m_count, m_clutoffs;
		emu_timer *m_timer;
		astring m_assembled_tag;

private:
		UINT32 m_7xxxxx_regs[0x100000/4];
		int m_width, m_height, m_patofsx, m_patofsy;
		UINT32 m_vram_addr, m_vram_src;
		UINT8 m_fillbytes[256];
		required_device<palette_device> m_palette;
};


// device type definition
extern const device_type NUBUS_SPECPDQ;

#endif  /* __NUBUS_SPECPDQ_H__ */
