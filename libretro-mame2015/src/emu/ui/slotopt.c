/*********************************************************************

    ui/slotopt.c

    Internal menu for the slot options.

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

*********************************************************************/

#include "emu.h"

#include "ui/ui.h"
#include "ui/slotopt.h"
#include "ui/devopt.h"


/*-------------------------------------------------
    ui_slot_get_current_option - returns
-------------------------------------------------*/
device_slot_option *ui_menu_slot_devices::slot_get_current_option(device_slot_interface *slot)
{
	const char *current;
	if (slot->fixed())
	{
		current = slot->default_option();
	}
	else
	{
		astring temp;
		current = machine().options().main_value(temp, slot->device().tag() + 1);
	}

	return slot->option(current);
}

/*-------------------------------------------------
    ui_slot_get_current_index - returns
-------------------------------------------------*/
int ui_menu_slot_devices::slot_get_current_index(device_slot_interface *slot)
{
	const device_slot_option *current = slot_get_current_option(slot);

	if (current != NULL)
	{
		int val = 0;
		for (const device_slot_option *option = slot->first_option(); option != NULL; option = option->next())
		{
			if (option == current)
				return val;

			if (option->selectable())
				val++;
		}
	}

	return -1;
}

/*-------------------------------------------------
    ui_slot_get_length - returns
-------------------------------------------------*/
int ui_menu_slot_devices::slot_get_length(device_slot_interface *slot)
{
	int val = 0;
	for (const device_slot_option *option = slot->first_option(); option != NULL; option = option->next())
		if (option->selectable())
			val++;

	return val;
}

/*-------------------------------------------------
    ui_slot_get_next - returns
-------------------------------------------------*/
const char *ui_menu_slot_devices::slot_get_next(device_slot_interface *slot)
{
	int idx = slot_get_current_index(slot);
	if (idx < 0)
		idx = 0;
	else
		idx++;

	if (idx >= slot_get_length(slot))
		return "";

	return slot_get_option(slot, idx);
}

/*-------------------------------------------------
    ui_slot_get_prev - returns
-------------------------------------------------*/
const char *ui_menu_slot_devices::slot_get_prev(device_slot_interface *slot)
{
	int idx = slot_get_current_index(slot);
	if (idx < 0)
		idx = slot_get_length(slot) - 1;
	else
		idx--;

	if (idx < 0)
		return "";

	return slot_get_option(slot, idx);
}

/*-------------------------------------------------
    ui_slot_get_option - returns
-------------------------------------------------*/
const char *ui_menu_slot_devices::slot_get_option(device_slot_interface *slot, int index)
{
	if (index >= 0)
	{
		int val = 0;
		for (const device_slot_option *option = slot->first_option(); option != NULL; option = option->next())
		{
			if (val == index)
				return option->name();

			if (option->selectable())
				val++;
		}
	}

	return "";
}


/*-------------------------------------------------
    ui_set_use_natural_keyboard - specifies
    whether the natural keyboard is active
-------------------------------------------------*/

void ui_menu_slot_devices::set_slot_device(device_slot_interface *slot, const char *val)
{
	astring error;
	machine().options().set_value(slot->device().tag()+1, val, OPTION_PRIORITY_CMDLINE, error);
	assert(!error);
}

/*-------------------------------------------------
    menu_slot_devices_populate - populates the main
    slot device menu
-------------------------------------------------*/

ui_menu_slot_devices::ui_menu_slot_devices(running_machine &machine, render_container *container) : ui_menu(machine, container)
{
}

void ui_menu_slot_devices::populate()
{
	/* cycle through all devices for this system */
	slot_interface_iterator iter(machine().root_device());
	for (device_slot_interface *slot = iter.first(); slot != NULL; slot = iter.next())
	{
		/* record the menu item */
		const device_slot_option *option = slot_get_current_option(slot);
		astring opt_name;
		if (option == NULL)
			opt_name.cpy("------");
		else
		{
			opt_name.cpy(option->name());
			if (slot->fixed() || slot_get_length(slot) == 0)
				opt_name.cat(" [internal]");
		}

		item_append(slot->device().tag() + 1, opt_name, (slot->fixed() || slot_get_length(slot) == 0) ? 0 : (MENU_FLAG_LEFT_ARROW | MENU_FLAG_RIGHT_ARROW), (void *)slot);
	}
	item_append(MENU_SEPARATOR_ITEM, NULL, 0, NULL);
	item_append("Reset",  NULL, 0, (void *)1);
}

ui_menu_slot_devices::~ui_menu_slot_devices()
{
}

/*-------------------------------------------------
    ui_menu_slot_devices - menu that
-------------------------------------------------*/

void ui_menu_slot_devices::handle()
{
	/* process the menu */
	const ui_menu_event *menu_event = process(0);

	if (menu_event != NULL && menu_event->itemref != NULL)
	{
		if ((FPTR)menu_event->itemref == 1 && menu_event->iptkey == IPT_UI_SELECT)
		{
			machine().options().add_slot_options(false);
			machine().schedule_hard_reset();
		}
		else if (menu_event->iptkey == IPT_UI_LEFT || menu_event->iptkey == IPT_UI_RIGHT)
		{
			device_slot_interface *slot = (device_slot_interface *)menu_event->itemref;
			const char *val = (menu_event->iptkey == IPT_UI_LEFT) ? slot_get_prev(slot) : slot_get_next(slot);
			set_slot_device(slot, val);
			reset(UI_MENU_RESET_REMEMBER_REF);
		}
		else if (menu_event->iptkey == IPT_UI_SELECT)
		{
			device_slot_interface *slot = (device_slot_interface *)menu_event->itemref;
			device_slot_option *option = slot_get_current_option(slot);
			if (option)
				ui_menu::stack_push(auto_alloc_clear(machine(), ui_menu_device_config(machine(), container, slot, option)));
		}
	}
}
