/***************************************************************************

    ui/slotopt.h

    Internal menu for the slot options.

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

***************************************************************************/

#pragma once

#ifndef __UI_SLOTOPT_H__
#define __UI_SLOTOPT_H__

//#include "drivenum.h"

class ui_menu_slot_devices : public ui_menu {
public:
	ui_menu_slot_devices(running_machine &machine, render_container *container);
	virtual ~ui_menu_slot_devices();
	virtual void populate();
	virtual void handle();

private:
	device_slot_option *slot_get_current_option(device_slot_interface *slot);
	int slot_get_current_index(device_slot_interface *slot);
	int slot_get_length(device_slot_interface *slot);
	const char *slot_get_next(device_slot_interface *slot);
	const char *slot_get_prev(device_slot_interface *slot);
	const char *slot_get_option(device_slot_interface *slot, int index);
	void set_slot_device(device_slot_interface *slot, const char *val);
};

#endif  /* __UI_SLOTOPT_H__ */
