// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    emu.h

    Core header file to be included by most files.

    NOTE: The contents of this file are designed to meet the needs of
    drivers and devices. In addition to this file, you will also need
    to include the headers of any CPUs or other devices that are required.

    If you find yourself needing something outside of this file in a
    driver or device, think carefully about what you are doing.

***************************************************************************/

#pragma once

#ifndef __EMU_H__
#define __EMU_H__

// core emulator headers -- must be first
#include "emucore.h"
#include "eminline.h"
#include "profiler.h"

// commonly-referenecd utilities imported from lib/util
#include "palette.h"
#include "unicode.h"

// emulator-specific utilities
#include "attotime.h"
#include "hash.h"
#include "fileio.h" // remove me once NVRAM is implemented as device
#include "delegate.h"
#include "devdelegate.h"

// memory and address spaces
// renamed by libretro: Android has a memory.h system header conflict
#include "emumemory.h"
#include "addrmap.h"
#include "memarray.h"

// machine-wide utilities
#include "romload.h"
#include "save.h"

// define machine_config_constructor here due to circular dependency
// between devices and the machine config
class machine_config;
typedef device_t * (*machine_config_constructor)(machine_config &config, device_t *owner, device_t *device);

// I/O
#include "input.h"
#include "ioport.h"
#include "output.h"

// diimage requires uimenu
#include "ui/menu.h"

// devices and callbacks
#include "device.h"
#include "devfind.h"
#include "distate.h"
#include "dimemory.h"
#include "diexec.h"
#include "opresolv.h"
#include "digfx.h"
#include "diimage.h"
#include "dioutput.h"
#include "diserial.h"
#include "dislot.h"
#include "disound.h"
#include "divideo.h"
#include "dinvram.h"
#include "dirtc.h"
#include "didisasm.h"
#include "schedule.h"
#include "timer.h"
#include "dinetwork.h"

// machine and driver configuration
#include "mconfig.h"
#include "gamedrv.h"
#include "parameters.h"

// timers, CPU and scheduling
#include "devcpu.h"

// image-related
#include "softlist.h"
#include "image.h"

// networking
#include "network.h"

// the running machine
#include "mame.h"
#include "machine.h"
#include "driver.h"

// video-related
#include "drawgfx.h"
#include "tilemap.h"
#include "emupal.h"
#include "screen.h"
#include "video.h"

// sound-related
#include "sound.h"
#include "speaker.h"

// generic helpers
#include "devcb.h"
#include "dispatch.h"
#include "drivers/xtal.h"
#include "machine/generic.h"
#include "video/generic.h"

#endif  /* __EMU_H__ */
