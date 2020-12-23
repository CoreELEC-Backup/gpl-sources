/*********************************************************************

    ui/filemngr.c

    MESS's clunky built-in file manager

    TODO
        - Restrict directory listing by file extension

*********************************************************************/

#include "emu.h"
#include "ui/ui.h"
#include "ui/swlist.h"
#include "ui/filemngr.h"
#include "ui/filesel.h"
#include "ui/miscmenu.h"


/***************************************************************************
    FILE MANAGER
***************************************************************************/

//-------------------------------------------------
//  ctor
//-------------------------------------------------

ui_menu_file_manager::ui_menu_file_manager(running_machine &machine, render_container *container, const char *warnings) : ui_menu(machine, container)
{
	// This warning string is used when accessing from the force_file_manager call, i.e.
	// when the file manager is loaded top front in the case of mandatory image devices
	if (warnings)
		m_warnings.cpy(warnings);
	else
		m_warnings.reset();

	m_curr_selected = FALSE;
}


//-------------------------------------------------
//  dtor
//-------------------------------------------------

ui_menu_file_manager::~ui_menu_file_manager()
{
}


//-------------------------------------------------
//  custom_render - perform our special rendering
//-------------------------------------------------

void ui_menu_file_manager::custom_render(void *selectedref, float top, float bottom, float origx1, float origy1, float origx2, float origy2)
{
	const char *path;

	// access the path
	path = selected_device ? selected_device->filename() : NULL;
	extra_text_render(container, top, bottom,
						origx1, origy1, origx2, origy2, NULL, path);
}


void ui_menu_file_manager::fill_image_line(device_image_interface *img, astring &instance, astring &filename)
{
	// get the image type/id
	instance.printf("%s (%s)", img->instance_name(), img->brief_instance_name());

	// get the base name
	if (img->basename() != NULL)
	{
		filename.cpy(img->basename());

		// if the image has been loaded through softlist, also show the loaded part
		if (img->part_entry() != NULL)
		{
			const software_part *tmp = img->part_entry();
			if (tmp->name() != NULL)
			{
				filename.cat(" (");
				filename.cat(tmp->name());
				// also check if this part has a specific part_id (e.g. "Map Disc", "Bonus Disc", etc.), and in case display it
				if (img->get_feature("part_id") != NULL)
				{
					filename.cat(": ");
					filename.cat(img->get_feature("part_id"));
				}
				filename.cat(")");
			}
		}
	}
	else
		filename.cpy("---");
}

//-------------------------------------------------
//  populate
//-------------------------------------------------

void ui_menu_file_manager::populate()
{
	astring buffer, tmp_inst, tmp_name;
	bool first_entry = true;
	astring prev_owner;

	if (m_warnings)
	{
		item_append(m_warnings, NULL, MENU_FLAG_DISABLE, NULL);
		item_append("", NULL, MENU_FLAG_DISABLE, NULL);
	}

	// cycle through all devices for this system
	device_iterator iter(machine().root_device());
	tagmap_t<UINT8> devtags;
	for (device_t *dev = iter.first(); dev != NULL; dev = iter.next())
	{
		bool tag_appended = false;
		if (devtags.add(dev->tag(), 1, FALSE) == TMERR_DUPLICATE)
			continue;

		// check whether it owns an image interface
		image_interface_iterator subiter(*dev);
		if (subiter.count() > 0)
		{
			// if so, cycle through all its image interfaces
			image_interface_iterator subiter(*dev);
			for (device_image_interface *scan = subiter.first(); scan != NULL; scan = subiter.next())
			{
				// if it is a children device, and not something further down the device tree, we want it in the menu!
				if (strcmp(scan->device().owner()->tag(), dev->tag()) == 0)
					if (devtags.add(scan->device().tag(), 1, FALSE) != TMERR_DUPLICATE)
					{
						// check whether we already had some devices with the same owner: if not, output the owner tag!
						if (!tag_appended)
						{
							if (first_entry)
								first_entry = false;
							else
								item_append(MENU_SEPARATOR_ITEM, NULL, 0, NULL);
							buffer.printf("[root%s]", dev->tag());
							item_append(buffer, NULL, 0, NULL);
							tag_appended = true;
						}
						// finally, append the image interface to the menu
						fill_image_line(scan, tmp_inst, tmp_name);
						item_append(tmp_inst, tmp_name, 0, (void *) scan);
					}
			}
		}
	}
	item_append(MENU_SEPARATOR_ITEM, NULL, 0, NULL);
	item_append("Reset",  NULL, 0, (void *)1);

	custombottom = machine().ui().get_line_height() + 3.0f * UI_BOX_TB_BORDER;
}


//-------------------------------------------------
//  handle
//-------------------------------------------------

void ui_menu_file_manager::handle()
{
	// process the menu
	const ui_menu_event *event = process(0);
	if (event != NULL && event->itemref != NULL && event->iptkey == IPT_UI_SELECT)
	{
		if ((FPTR)event->itemref == 1)
		{
			if (m_curr_selected)
				machine().schedule_hard_reset();
		}
		else
		{
			selected_device = (device_image_interface *) event->itemref;
			if (selected_device != NULL)
			{
				m_curr_selected = TRUE;
				ui_menu::stack_push(selected_device->get_selection_menu(machine(), container));

				// reset the existing menu
				reset(UI_MENU_RESET_REMEMBER_POSITION);
			}
		}
	}
}

// force file manager menu
void ui_menu_file_manager::force_file_manager(running_machine &machine, render_container *container, const char *warnings)
{
	// reset the menu stack
	ui_menu::stack_reset(machine);

	// add the quit entry followed by the game select entry
	ui_menu *quit = auto_alloc_clear(machine, ui_menu_quit_game(machine, container));
	quit->set_special_main_menu(true);
	ui_menu::stack_push(quit);
	ui_menu::stack_push(auto_alloc_clear(machine, ui_menu_file_manager(machine, container, warnings)));

	// force the menus on
	machine.ui().show_menu();

	// make sure MAME is paused
	machine.pause();
}
