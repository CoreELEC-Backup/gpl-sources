#ifndef _CYCITV_H_
#define _CYCITV_H_

#define DVB_USB_LOG_PREFIX "cycitv"
#include "dvb-usb.h"

#define deb_info(args...) dprintk(dvb_usb_cycitv_debug, 0x01, args)
#define deb_xfer(args...) dprintk(dvb_usb_cycitv_debug, 0x02, args)
#define deb_rc(args...)   dprintk(dvb_usb_cycitv_debug, 0x04, args)
#define deb_ca(args...)   dprintk(dvb_usb_cycitv_debug, 0x08, args)
#endif
