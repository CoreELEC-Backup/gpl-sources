#pragma once

#ifndef __COCO_PAK_H__
#define __COCO_PAK_H__

#include "emu.h"
#include "cococart.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> coco_pak_device

class coco_pak_device :
		public device_t,
		public device_cococart_interface
{
public:
		// construction/destruction
		coco_pak_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
		coco_pak_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);

		// optional information overrides
		virtual machine_config_constructor device_mconfig_additions() const;
		virtual const rom_entry *device_rom_region() const;

		virtual UINT8* get_cart_base();
protected:
		// device-level overrides
		virtual void device_start();
		virtual void device_reset();

		// internal state
		device_image_interface *m_cart;
		cococart_slot_device *m_owner;
};


// device type definition
extern const device_type COCO_PAK;

// ======================> coco_pak_banked_device

class coco_pak_banked_device :
		public coco_pak_device
{
public:
		// construction/destruction
		coco_pak_banked_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

protected:
		// device-level overrides
		virtual void device_reset();
		virtual DECLARE_WRITE8_MEMBER(write);
private:
		void banked_pak_set_bank(UINT32 bank);
};


// device type definition
extern const device_type COCO_PAK_BANKED;
#endif  /* __COCO_PAK_H__ */
