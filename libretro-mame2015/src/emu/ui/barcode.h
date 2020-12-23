/***************************************************************************

    ui/barcode.h

    "Barcode Reader" control

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

***************************************************************************/

#pragma once

#ifndef __UI_BARCODE_H__
#define __UI_BARCODE_H__

#include "machine/bcreader.h"
#include "ui/devctrl.h"

class ui_menu_barcode_reader : public ui_menu_device_control<barcode_reader_device> {
public:
	ui_menu_barcode_reader(running_machine &machine, render_container *container, barcode_reader_device *device);
	virtual ~ui_menu_barcode_reader();
	virtual void populate();
	virtual void handle();

private:
	char  m_barcode_buffer[20];
};


#endif // __UI_BARCODE_H__
