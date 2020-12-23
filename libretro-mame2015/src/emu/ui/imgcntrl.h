/***************************************************************************

    ui/imgcntrl.h

    MESS's clunky built-in file manager

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

***************************************************************************/

#pragma once

#ifndef __UI_IMGCNTRL_H__
#define __UI_IMGCNTRL_H__

#include "drivenum.h"

// ======================> ui_menu_control_device_image

class ui_menu_control_device_image : public ui_menu {
public:
	ui_menu_control_device_image(running_machine &machine, render_container *container, device_image_interface *image);
	virtual ~ui_menu_control_device_image();
	virtual void populate();
	virtual void handle();

protected:
	enum {
		START_FILE, START_OTHER_PART, START_SOFTLIST,
		SELECT_PARTLIST, SELECT_ONE_PART, SELECT_OTHER_PART,
		SELECT_FILE, CREATE_FILE, CREATE_CONFIRM, CHECK_CREATE, DO_CREATE, SELECT_SOFTLIST,
		LAST_ID
	};

	// protected instance variables
	int state;
	device_image_interface *image;
	int submenu_result;
	astring current_directory;
	astring current_file;

	// methods
	virtual void hook_load(astring filename, bool softlist);

	bool create_ok;

private:
	// instance variables
	bool create_confirmed;
	//bool softlist_done;
	const software_info *swi;
	const software_part *swp;
	class software_list_device *sld;
	astring software_info_name;

	// methods
	void test_create(bool &can_create, bool &need_confirm);
	void load_software_part();
};


#endif /* __UI_IMGCNTRL_H__ */
