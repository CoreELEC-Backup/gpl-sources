/*
    TiMidity++ -- MIDI to WAVE converter and player
    Copyright (C) 1999-2002 Masanao Izumo <mo@goice.co.jp>
    Copyright (C) 1995 Tuukka Toivonen <tt@cgs.fi>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

   controls.c

   */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */

#include "interface.h"
#include "timidity.h"
#include "controls.h"

/* Minimal control mode */
extern ControlMode dumb_control_mode;
#ifndef DEFAULT_CONTROL_MODE
# define DEFAULT_CONTROL_MODE &dumb_control_mode
#endif

ControlMode *ctl_list[]={
  &dumb_control_mode,
  0
};

ControlMode *ctl=DEFAULT_CONTROL_MODE;

int std_write(int fd, const void *buffer, int size)
{
    /* redirect stdout writes */
    if (fd == 1 && ctl->write)
	return ctl->write((char*)buffer, size);
    return write(fd, buffer, size);
}
