/*
 *************************************************************************
 *
 * serdisp_connect_usb.c
 * routines for accessing usb devices
 *
 *************************************************************************
 *
 * copyright (C) 2003-2010  wolfgang astleitner
 * email     mrwastl@users.sourceforge.net
 *
 *************************************************************************
 * This program is free software; you can redistribute it and/or modify   
 * it under the terms of the GNU General Public License as published by   
 * the Free Software Foundation; either version 2 of the License, or (at  
 * your option) any later version.                                        
 *                                                                        
 * This program is distributed in the hope that it will be useful, but    
 * WITHOUT ANY WARRANTY; without even the implied warranty of             
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU      
 * General Public License for more details.                               
 *                                                                        
 * You should have received a copy of the GNU General Public License      
 * along with this program; if not, write to the Free Software            
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA              
 * 02111-1307, USA.  Or, point your browser to                            
 * http://www.gnu.org/copyleft/gpl.html                                   
 *************************************************************************
 */

/*#define DEBUG_SIGNALS 1*/
#define ENABLE_FTDI 1

/* enable libusb-support for cypress-based chips on non-linux systems only */
#ifndef __linux__
#define ENABLE_CYPRESS 1
#endif

/* use usb_clear_halt() instead of usb_bulk_read() (which seems to cause problems on some linux versions) */
#define IOW24_USE_USB_CLEAR_HALT 1

/* dummy operations to incoming channel. w/o it communication might stall when sending next lcd-command */
#define USBL4ME5I_CLEAR_STALL() { \
          fp_usb_bulk_write(usbitems->usb_dev, usbitems->in_ep, usbitems->stream, 8, 1); \
          fp_usb_bulk_write(usbitems->usb_dev, usbitems->in_ep, usbitems->stream, 8, 1); \
          fp_usb_clear_halt(usbitems->usb_dev, usbitems->in_ep); \
        }

#include "../config.h"

#include <syslog.h>

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
/*#include <fcntl.h>*/
#include <string.h>
#include <strings.h>
#include <sys/ioctl.h>

/*#include <sys/resource.h>*/

#include "serdisplib/serdisp_connect.h"
#include "serdisplib/serdisp_connect_usb.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"

#include "serdisplib/serdisp_fctptr.h"

#ifdef HAVE_LIBPTHREAD
  pthread_mutex_t mutex_usb = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_t mutex_commit = PTHREAD_MUTEX_INITIALIZER;
#endif


/* supported usb-devices */
typedef struct serdisp_usbdevice_s {
  int   vendorID;
  int   productID;
  int   bcdDevice;
  short deviceID;
  int   streamsize;
  byte  claim;           /* interfaces to claim. 0xFF: all, else: bitmask (eg: 0x02: claim interface 1) */
  byte  maxIO;
} serdisp_usbdevices_t;


/* supported usb-devices */
serdisp_usbdevices_t usbdevices[] = {
/*  vendorID  productID   bcdDevice    deviceID             streamsize   claim   maxIO */
   {0x07C0,    0x1501,        -1,      SDHWT_USBIOW24,      8,           0xFF, 16 }
  ,{0x060C,    0x04EB,        -1,      SDHWT_USBALPHACOOL,  8,           0x00, 16 }
  ,{0x4243,    0xee08,        -1,      SDHWT_USBL4ME5I,     60,          0x00, 16 }
  ,{0x16C0,    0x08A2,        -1,      SDHWT_USB13700,      32768,       0xFF, 16 }
  ,{0x152A,    0x8380,        -1,      SDHWT_USBSDCMEGTRON, 8,           0x00, 16 }
#ifdef ENABLE_FTDI
  ,{0x0403,    0x6010,    0x0500,      SDHWT_USBFTDI2232,   4096,        0xFF, 16 }
  ,{0x0403,    0x6001,    0x0200,      SDHWT_USBFTDI245,    4096,        0xFF,  8 }
  ,{0x0403,    0x6001,    0x0400,      SDHWT_USBFTDI245,    4096,        0xFF,  8 }
#endif
};


/* internal functions */
static
usb_dev_handle* SDCONNusb_find             (int vendorID, int productID, int bcdDevice, char* hr_name,
                                            int occurrence, int* dev_idx, struct usb_device** dev_ptr
                                           );
static int      SDCONNusb_claim_interfaces (serdisp_CONN_t* sdcd, byte ifmask, unsigned char* claimed);


/* *********************************
   serdisp_CONN_t* SDCONNusb_open(sdcdev)
   *********************************
   opens an usb device for a serdisp lcd
   *********************************
   sdcdev   ... device identifier.

   format:

       sdcdev = CONNTYPE [ '@' PROTOCOL ] ':' [ occurrence ':' ] ( PRODUCT | DEVICENAME )
     CONNTYPE = 'USB'
     PROTOCOL = 'SPI'
      PRODUCT = vendorID '/' productID [ '/' bcdDevice ]
   DEVICENAME = [ [ manufacturer ] '/' ] product [ '/' serial ]


   annotations:
     DEVICENAME supported since rev.183
     PROTOCOL supported since rev. 192

     if PRODUCT-items (vendorID, productID, bcdDevice) are not numeric (hexadecimal with or without leading '0x')
     than DEVICENAME is assumed


   examples:
     "USB:0x0403/0x6010/0x0500"   (vendorID = 0x0403, productID = 0x6010, bcdDevice = 0x500)
     "USB:403/6010"               (the same, w/o bcdDevice (first device matching 403/6010 is returned))

     "USB:FTDI/DLP2232M"          (identified through manufacturer info and product description)
     "USB:DLP2232M"               (the same, but only product name as identification)
     "USB:0:FTDI/DLP2232M"        (the same, first occurrence (default))
     "USB:1:FTDI/DLP2232M"        (the same, but 2nd occurrence (if > 1 DLP2232M are connected))

     "USB@SPI:DLP2232M"           protcol is SPI (instead of default emulated signalling)

   *********************************
   returns a serdisp connect descriptor or (serdisp_CONN_t*)0 if operation was unsuccessful

*/
serdisp_CONN_t* SDCONNusb_open(const char sdcdev[]) {
  serdisp_CONN_t* sdcd = 0;
  struct usb_device* dev = 0;
  usb_dev_handle* usb_dev = 0;
  char* devname;
  char* hr_name;
  char connproto[20];
  char buffer[10];
  char* idx;  /* index of ':' in conntype:devicename */
  char* endptr;  /* for strtol */
  int occurrence = 0;
  int vendorID = -1;
  int productID = -1;
  int bcdDevice = -1;
  int dev_idx;
  char* idx_proto;
  int protocol;

  serdisp_usbdev_t* usbitems = (serdisp_usbdev_t*)0;

  if ( ! SDFCTPTR_checkavail(SDFCTPTR_LIBUSB) ) {
    sd_error(SERDISP_ERUNTIME, "%s(): libusb is not loaded.", __func__);
    return (serdisp_CONN_t*)0;
  }

  devname = (char*)sdcdev;
  idx = index(devname, ':');

  if (serdisp_ptrdistance(idx, devname) >= 18) {
    sd_error(SERDISP_ENXIO, "%s(): invalid connection type / protocol (too long)", __func__);
    return (serdisp_CONN_t*)0;
  }

  sdtools_strncpy(connproto, devname, serdisp_ptrdistance(idx, devname));

  protocol = SDPROTO_DONTCARE;  /* default: don't care about protocol */
  idx_proto = index(connproto, '@');
  if (idx_proto) {
    if (sdtools_ismatching("SPI", -1, idx_proto+1, -1)) {
      protocol = SDPROTO_SPI;
    } else {
      sd_error(SERDISP_EDEVNOTSUP, "protocol '%s' is undefined", idx_proto+1);
      return (serdisp_CONN_t*)0;
    }
  }

  devname = (idx+1);

  idx = index(devname, ':');

  if (idx) {
    if (serdisp_ptrdistance(idx, devname) >= 8) {
      sd_error(SERDISP_ENXIO, "%s(): invalid occurence ID", __func__);
      return (serdisp_CONN_t*)0;
    }
    sdtools_strncpy(buffer, devname, serdisp_ptrdistance(idx, devname));
    devname = (idx+1);
    occurrence = (int)strtol(buffer, 0, 10);
  }

  hr_name = devname;  /* "vendorID/productID[/bcdDevice]"  might also be  "[manufacturer]/product[/serial]" */

  idx = index(devname, '/');
  if (idx && serdisp_ptrdistance(idx, devname) < 7) {
    sdtools_strncpy(buffer, devname, serdisp_ptrdistance(idx, devname));
    vendorID = (int)strtol(buffer, &endptr, 16);
    if (buffer == endptr || (*endptr != '\0') )  /* value invalid */
      vendorID = -1;
    else
      devname = (idx+1);  
  }


  if (vendorID != -1) {
    idx = index(devname, '/');
    if (!idx) idx = devname + strlen(devname);
    if (serdisp_ptrdistance(idx, devname) < 7) {
      sdtools_strncpy(buffer, devname, serdisp_ptrdistance(idx, devname));
      productID = (int)strtol(buffer, &endptr, 16);
      if (buffer == endptr || (*endptr != '\0') ) { /* value invalid */
        productID = -1;
        vendorID = -1;
      } else
        devname = idx;
    }
  }

  if (productID != -1 && devname[0] == '/') {
    devname++;
    idx = devname + strlen(devname);

    if (serdisp_ptrdistance(idx, devname) < 7) {
      sdtools_strncpy(buffer, devname, serdisp_ptrdistance(idx, devname));
      bcdDevice = (int)strtol(buffer, &endptr, 16);
      if (buffer == endptr || (*endptr != '\0') ) { /* value invalid */
        productID = -1;
        vendorID = -1;
        bcdDevice = -1;
      }
    }
  }

  usb_dev = SDCONNusb_find(vendorID, productID, bcdDevice, hr_name, occurrence, &dev_idx, &dev);

  if (usb_dev) {
    if (! (sdcd = (serdisp_CONN_t*)sdtools_malloc(sizeof(serdisp_CONN_t)) ) ) {
      sd_error(SERDISP_EMALLOC, "%s(): unable to allocate memory for sdcd", __func__);
      return (serdisp_CONN_t*)0;
    }
    memset(sdcd, 0, sizeof(serdisp_CONN_t));

    sdcd->sdcdev = (char*)sdcdev;

    sdcd->conntype = SERDISPCONNTYPE_PARPORT;
    sdcd->hardwaretype = SDHWT_USB;
    sdcd->protocol = protocol;

    if (! (sdcd->extra = (void*)sdtools_malloc(sizeof(serdisp_usbdev_t)) ) ) {
      sd_error(SERDISP_EMALLOC, "%s(): unable to allocate memory for sdcd->extra", __func__);
      free(sdcd);
      return (serdisp_CONN_t*)0;
    }
    memset(sdcd->extra, 0, sizeof(serdisp_usbdev_t));

    usbitems = (serdisp_usbdev_t*)(sdcd->extra);

    /* some initialisations */
    usbitems->devID = dev_idx;  /* index in usbdevices[] */
    usbitems->streamsize = usbdevices[usbitems->devID].streamsize;

    if (! (usbitems->stream = (char*) sdtools_malloc( sizeof(char) * usbitems->streamsize ) ) ) {
      sd_error(SERDISP_EMALLOC, "%s(): cannot allocate stream buffer", __func__);
      free(sdcd->extra);
      free(sdcd);
      return (serdisp_CONN_t*)0;
    }

    usbitems->streampos = 0;

    usbitems->dev = dev;
    usbitems->usb_dev = usb_dev;
    usbitems->read_timeout = 5000;
    usbitems->write_timeout = 5000;
    usbitems->claimed = 0;   /* start with no interfaces claimed */

    if (SDCONNusb_claim_interfaces(sdcd, usbdevices[dev_idx].claim, &(usbitems->claimed)) != 0) {
      /* SDCONNusb_claim_interfaces() will generate an error message on its own -> use that one */
      /*sd_error(SERDISP_EACCES, "%s(): unable to claim interfaces", __func__);*/
      if (usbitems->usb_dev)
        if (fp_usb_close(usbitems->usb_dev))  /* != 0: close failed */
          fp_usb_reset(usbitems->usb_dev);
      if (usbitems->stream)
        free(usbitems->stream);
      if (sdcd->extra)
        free(sdcd->extra);
      return (serdisp_CONN_t*)0;
    }

    switch (usbdevices[usbitems->devID].deviceID) {
#ifdef ENABLE_FTDI
      case SDHWT_USBFTDI2232:
      case SDHWT_USBFTDI245:

        usbitems->out_ep = 0x02;
        usbitems->in_ep = 0x81;

        if (fp_usb_control_msg(usb_dev, 0x40, 0, 0, 1, NULL, 0, usbitems->write_timeout) != 0) {
          sd_error(SERDISP_ERUNTIME, "%s(): resetting FTDI-chip failed", __func__);
        }


        if (fp_usb_control_msg(usb_dev, 0x40, 0, 1, 1, NULL, 0, usbitems->write_timeout) != 0) {
          sd_error(SERDISP_ERUNTIME, "%s(): FTDI: purging of RX buffer failed", __func__);
        }

        if (fp_usb_control_msg(usb_dev, 0x40, 0, 2, 1, NULL, 0, usbitems->write_timeout) != 0) {
          sd_error(SERDISP_ERUNTIME, "%s(): FTDI: purging of TX buffer failed", __func__);
        }


        if (sdcd->protocol == SDPROTO_SPI) {
          if (fp_usb_control_msg(usb_dev, 0x40, 0x0B, 0x0200, 1, NULL, 0, usbitems->write_timeout) != 0) {
            sd_error(SERDISP_ERUNTIME, "%s(): entering MPSSE mode failed", __func__);
          }

          /* extra configuration and initialisation will be done by SDCONNusb_confinit() */
          sdcd->needs_confinit = 1;
        } else { /* sdcd->conntype == SERDISPCONNTYPE_PARPORT */
          if (fp_usb_control_msg(usb_dev, 0x40, 0x0B, 0x0100 | 0xFF , 1, NULL, 0, usbitems->write_timeout) != 0) {
            sd_error(SERDISP_ERUNTIME, "%s(): entering bitbang mode failed", __func__);
          }

          if (fp_usb_control_msg(usb_dev, 0x40, 3, 0, 0x1, NULL, 0, usbitems->write_timeout) != 0) {
            sd_error(SERDISP_ERUNTIME, "%s(): setting baudrate failed", __func__);
          }
        }
      break;
#endif  /* ENABLE_FTDI */
      case SDHWT_USBIOW24: {
        /* only 1 altsetting and 1 endpoint */
        usbitems->out_ep = dev->config->interface[1].altsetting[0].endpoint[0].bEndpointAddress;  

        /* iow24: enable lcd */
        IOW_FILLSTREAM(usbitems->stream, IOW_LCD_ENABLE_REPORT, 0x01, 0,0,0,0,0,0);  
        if (fp_usb_control_msg(
             usbitems->usb_dev, USB_DT_HID, IOW_REQ_SET_REPORT, 0x200, 1, usbitems->stream, 8, usbitems->write_timeout
                           ) < 0) {
          sd_error(SERDISP_ERUNTIME, "%s(): IOW/LCD enabling  failed", __func__);
        }

        /* iow24: enable i2c */
        IOW_FILLSTREAM(usbitems->stream, IOW_I2C_ENABLE_REPORT, 0x01, 0,0,0,0,0,0);  
        if (fp_usb_control_msg(
              usbitems->usb_dev, USB_DT_HID, IOW_REQ_SET_REPORT, 0x200, 1, usbitems->stream, 8, usbitems->write_timeout
                           ) < 0) {
          sd_error(SERDISP_ERUNTIME, "%s(): IOW/I2C enabling  failed", __func__);
        }

        /* iow24: init i2c */
        usbitems->store = 0xCF | 0x10;  /* all leds off (0xCF, leds are active low), enable 1st cs-line (0x10) */
        IOW_FILLSTREAM(usbitems->stream, IOW_I2C_WRITE_REPORT, 0xC2, 0x70, (byte)(usbitems->store) ,0,0,0,0);
        if (fp_usb_control_msg(
              usbitems->usb_dev, USB_DT_HID, IOW_REQ_SET_REPORT, 0x200, 1, usbitems->stream, 8, usbitems->write_timeout
            ) < 0) {
          sd_error(SERDISP_ERUNTIME, "%s(): IOW/I2C initial setting failed", __func__);
        }
        /* read acknowledge */
#ifdef IOW24_USE_USB_CLEAR_HALT
        if (fp_usb_clear_halt(usbitems->usb_dev, usbitems->out_ep) < 0) {
#else
        if (fp_usb_bulk_read(usbitems->usb_dev, usbitems->out_ep, usbitems->stream, 8, usbitems->write_timeout) < 0) {
#endif
            sd_error(SERDISP_ERUNTIME, "%s(): IOW/I2c reading ack report failed", __func__);
        }

        usbitems->laststatus = 1;  /* set laststatus to COMMAND (==1) */
        usbitems->streampos = 0;
      }
      break;
      case SDHWT_USBL4ME5I: {
        struct usb_config_descriptor *config = &dev->config[0];
        int ep0, ep1;
        int ifidx;
        int found = 0;

#ifndef ENABLE_CYPRESS
        sd_error(SERDISP_ENOTSUP, "%s(): support for libusb disabled for this device. please use hiddev instead", __func__);
        return (serdisp_CONN_t*)0;
#endif
        /* find interface for controlling LCD */
        ifidx = 0;
        while (!found && ifidx < config->bNumInterfaces) {
          struct usb_interface *interface = &config->interface[ifidx];
          if (interface->altsetting[0].bInterfaceProtocol == 0) {
            found = 1;
          } else {
            ifidx++;
          }
        }

        /* if no matching interface found: error, if found: claim interface */
        if (!found) {
          sd_error(SERDISP_EACCES, "%s(): unable to find an interface with matching protocol type 0", __func__);
          return (serdisp_CONN_t*)0;
        } else {
          SDCONNusb_claim_interfaces(sdcd, (1 << ifidx), &(usbitems->claimed));
        }

        ep0 = dev->config->interface[ifidx].altsetting[0].endpoint[0].bEndpointAddress;
        ep1 = dev->config->interface[ifidx].altsetting[0].endpoint[1].bEndpointAddress;
        /* test which of these endpoints is incoming ( endpointAddress | 0x80 -> true ) */
        if (ep0 | 0x80) {   /* ep0 == incoming; ep1 == outgoing */
          usbitems->out_ep = ep1;
          usbitems->in_ep = ep0;
        } else {   /* ep1 == incoming; ep0 == outgoing */
          usbitems->out_ep = ep0;
          usbitems->in_ep = ep1;
        }
      }
      break;
      case SDHWT_USBALPHACOOL: {
        fp_usb_set_configuration(usbitems->usb_dev,1);  /* w/o this one, alphacool might fail on init */

        /* if claiming is done before usb_set_configuration() display may hang */
        SDCONNusb_claim_interfaces(sdcd, 0x01, &(usbitems->claimed));
        usbitems->out_ep = 0x03;
        usbitems->in_ep = 0x00;
      }
      break;
      case SDHWT_USBSDCMEGTRON: {
        fp_usb_set_configuration(usbitems->usb_dev,1);  /* w/o this one, alphacool might fail on init */

        /* if claiming is done before usb_set_configuration() display may hang */
        SDCONNusb_claim_interfaces(sdcd, 0x01, &(usbitems->claimed));
        usbitems->out_ep = 0x04;
        usbitems->in_ep = 0x00;
      }
      break;
      case SDHWT_USB13700: {
        /*fp_usb_set_configuration(usbitems->usb_dev,1);*/
        usbitems->out_ep = 0x02;
        usbitems->in_ep = 0x82;
        /* clean up endpoints */
        usbitems->read_timeout = 500;
        fp_usb_clear_halt(usbitems->usb_dev, usbitems->in_ep);
      }
      break;

      default:
        sd_error(SERDISP_ERUNTIME, "%s(): switch(deviceID): hw type unsupported!!!", __func__);
    } /* switch */
  } else {
    sd_error(SERDISP_EACCES, "%s(): unable to find or open a matching device", __func__);
    return (serdisp_CONN_t*)0;
  }

  return sdcd;
}



/* *********************************
   void SDCONNusb_close(sdcd)
   *********************************
   close the device occupied by serdisp
   *********************************
   sdcd     ... serdisp connect descriptor
   *********************************
*/
void SDCONNusb_close(serdisp_CONN_t* sdcd) {
  int i;
  serdisp_usbdev_t* usbitems = (serdisp_usbdev_t*)(sdcd->extra);
  struct usb_config_descriptor *config;

  switch (usbdevices[usbitems->devID].deviceID) {
#ifdef ENABLE_FTDI
    case SDHWT_USBFTDI2232:
    case SDHWT_USBFTDI245:
    break;
#endif
    case SDHWT_USBIOW24: {

      /* iow24: disable lcd */
      IOW_FILLSTREAM(usbitems->stream, IOW_LCD_ENABLE_REPORT, 0x00, 0,0,0,0,0,0);
      if (fp_usb_control_msg(
            usbitems->usb_dev, USB_DT_HID, IOW_REQ_SET_REPORT, 0x200, 1, usbitems->stream, 8, usbitems->write_timeout
                         ) <0) {
        sd_error(SERDISP_ERUNTIME, "%s(): IOW/LCD shutdown failed", __func__);
      }

      /* iow24: de-init i2c signals */
      IOW_FILLSTREAM(usbitems->stream, IOW_I2C_WRITE_REPORT, 0xC2, 0x70,0xFF, 0,0,0,0);
      if (fp_usb_control_msg(
            usbitems->usb_dev, USB_DT_HID, IOW_REQ_SET_REPORT, 0x200, 1, usbitems->stream, 8, usbitems->write_timeout
                         ) < 0) {
        sd_error(SERDISP_ERUNTIME, "%s(): IOW/I2C de-initialising failed", __func__);
      }

      /* iow24: disable i2c */
      IOW_FILLSTREAM(usbitems->stream, IOW_I2C_ENABLE_REPORT, 0x00, 0,0,0,0,0,0);  
      if (fp_usb_control_msg(
            usbitems->usb_dev, USB_DT_HID, IOW_REQ_SET_REPORT, 0x200, 1, usbitems->stream, 8, usbitems->write_timeout
                         ) <0) {
        sd_error(SERDISP_ERUNTIME, "%s(): IOW/I2C shutdown failed", __func__);
      }
      /* i2c read acknowledge */

#ifdef IOW24_USE_USB_CLEAR_HALT
      if (fp_usb_clear_halt(usbitems->usb_dev, usbitems->out_ep) < 0) {
#else
      if (fp_usb_bulk_read(
            usbitems->usb_dev, usbitems->out_ep, usbitems->stream, 8, usbitems->write_timeout
                       ) < 0) {
#endif
        sd_error(SERDISP_ERUNTIME, "%s(): IOW/I2c reading ack report failed", __func__);
      }
    }
    break;
    case SDHWT_USBL4ME5I: {
      /*USBL4ME5I_CLEAR_STALL();*/
      fp_usb_clear_halt(usbitems->usb_dev, usbitems->in_ep);
      fp_usb_clear_halt(usbitems->usb_dev, usbitems->out_ep);
    }
    break;
    case SDHWT_USBALPHACOOL:
    break;
    case SDHWT_USB13700: {
      /*fp_usb_clear_halt(usbitems->usb_dev, usbitems->out_ep);*/
    }
    break;
    default: /* should never ever occur */
      sd_error(SERDISP_ERUNTIME, "%s(): switch-case: hwtype not defined!!!", __func__);
  }

  /* unclaim interfaces */
  config = &(usbitems->dev)->config[0]; /* dev->config->bNumInterfaces crashes with bsd */
  for (i = 0; i < config->bNumInterfaces; i++) {
    if (usbitems->claimed & (1<<i) ) {
      if (!fp_usb_release_interface(usbitems->usb_dev, i) ) {
        sd_debug(1, "%s(): usb_release_interface() successful for interface %d\n", __func__, i);
      } else {
        sd_debug(0, "%s(): usb_release_interface() unsuccessful for interface %d\n", __func__, i);
      }
      usbitems->claimed ^= (1<<i);
    }
  }

  if (fp_usb_close(usbitems->usb_dev))  /* != 0: close failed */
    fp_usb_reset(usbitems->usb_dev);
  free(usbitems->stream);
  free(sdcd->extra);
}


/* *********************************
   int SDCONNusb_confinit(sdcd)
   *********************************
   extra configuration and initialisation before first write/commit/read-operations
   *********************************
   sdcd     ... serdisp connect descriptor
   *********************************
   returns:
      0 ... no configuration needed
      1 ... confinit successful
     -1 ... extra/delayed configuration not supported by this device
     -2 ... unsupported configuration
*/
int SDCONNusb_confinit(serdisp_CONN_t* sdcd) {
  serdisp_usbdev_t* usbitems = (serdisp_usbdev_t*)(sdcd->extra);

  /* bail out if no extra configuration is needed */
  if (!sdcd->needs_confinit)
    return 0;

  /* may only be called once */
  sdcd->needs_confinit = 0;

  switch (usbdevices[usbitems->devID].deviceID) {
#ifdef ENABLE_FTDI
    case SDHWT_USBFTDI2232:
    case SDHWT_USBFTDI245: {
      int sig_sk = (sdcd->spi.cpol) ? 0 : 1;

      /* default initialisation if framelen is not set:
         3-wire SPI with 1 bit D/C, 8 bit data, CPOL = 1, CPHA = 1, DATA = active high, COMMAND = active low
       */
      if (sdcd->spi.framelen == 0) {
        sdcd->spi.framelen = 5;  /* framelen + 4 -> 9 bits */
        sdcd->spi.cpol     = 1;  /* SK (SCLK) high */
        sdcd->spi.cpha     = 1;  /* DO (SI) write at falling SCLK */
        sdcd->spi.data_high= 1;  /* data = active high; command = active low */
      }

      if (sdcd->spi.framelen < 4 || sdcd->spi.framelen > 5) {
        sd_error(SERDISP_ERUNTIME, "%s(): unsupported SPI frame length %d", __func__, sdcd->spi.framelen+4);
        sd_runtimeerror = 1;
        return -2;
      }

      usbitems->stream[usbitems->streampos++] = 0x86;  /* set clock divisor */
      usbitems->stream[usbitems->streampos++] = 0x00;  /* low byte */
      usbitems->stream[usbitems->streampos++] = 0x00;  /* high byte */

      usbitems->stream[usbitems->streampos++] = 0x80;  /* set data bits */
      usbitems->stream[usbitems->streampos++] = 0x00 | sig_sk;  /* CS=low, DI=low, DO=low */
      usbitems->stream[usbitems->streampos++] = 0x0B;  /* CS=out, DI=out, DO=out */

      /* empty non-empty read buffer */
      /*fp_usb_bulk_read(usbitems->usb_dev, usbitems->in_ep, buf, 256, usbitems->read_timeout);*/
      return 1;
    }
    break;
#endif
    default: /* should never ever occur, but if it happenes to be: sdcd->need_config is mis-configured in driver! */
      sd_error(SERDISP_ERUNTIME, "%s(): switch-case: extra config./init. not supported by device!", __func__);
  }
  return -1;
}


/* *********************************
   void SDCONNusb_write(sdcd, ldata, flags)
   *********************************
   write a byte to the serdisp connect device
   *********************************
   sdcd   ... serdisp connect descriptor
   ldata  ... data to be written 
   flags  ... which bytes are to read
*/
void SDCONNusb_write(serdisp_CONN_t* sdcd, long ldata, byte flags) {
  SDCONNusb_writedelay(sdcd, ldata, flags, 0);
}

/* *********************************
   void SDCONNusb_writedelay(sdcd, ldata, flags, ns)
   *********************************
   write a byte to the serdisp connect device
   *********************************
   sdcd   ... serdisp connect descriptor
   ldata  ... data to be written 
   flags  ... which bytes are to read  (ignored in SDCONNusb_*)
   ns     ... nanoseconds delay after write
*/
void SDCONNusb_writedelay(serdisp_CONN_t* sdcd, long ldata, byte flags, long ns) {
  byte t_data = 0, t_flags = 0;
  serdisp_usbdev_t* usbitems = (serdisp_usbdev_t*)(sdcd->extra);

#ifdef DEBUG_SIGNALS
  {
    int l;
    int ddd = ((int) ((ldata & 0x000F0000) >> 8)) | ((int) (ldata & 0x000000FF) );
    for (l = 11; l >= 0; l--) {
      fprintf(stderr, "%s", ((ddd & (1 << l)) ? " #  " : "|   "  ));
      if (l % 4 == 0)
        fprintf(stderr, "  ");
    }
    fprintf(stderr, "\n");
  }
#endif

  /* control byte */
  /* bits 16-23 from ldata */
  t_flags = (byte) ((ldata & 0x00FF0000) >> 16);


  /* data byte */
  t_data = (byte) (ldata & 0x000000FF);

  switch (usbdevices[usbitems->devID].deviceID) {
#ifdef ENABLE_FTDI
    case SDHWT_USBFTDI2232:
    case SDHWT_USBFTDI245:
      if (sdcd->protocol == SDPROTO_SPI) {
        int sig_sk = (sdcd->spi.cpol) ? 0 : 1;

        byte chunksize = 6;  /* commit and data/command need 6 bytes in buffer */

        /* t_flags:  8421 0000
                     0000 8421
                             ^--- ---- ---x: x=0: t_data == data byte, x=1: t_data == command byte
                            ^---- ---- --1*: commit
                           ^----- ---- -x--: x=0: data = 0, command = 1; x=1: data = 1, command = 0
         */

        /* beware of buffer overflow */
        if ((usbitems->streampos + chunksize) >= usbitems->streamsize)
          SDCONNusb_commit(sdcd);

        if (t_flags & 0x02) {  /* t_flags & 0x2 -> commit */
          byte lastbit = ((t_data & 0x1) << 1);

          /* commit  (CS -> high, SK -> low, then pull CS to low leaving SK low) */
          usbitems->stream[usbitems->streampos++] = 0x80;                     /* set signals */
          usbitems->stream[usbitems->streampos++] = 0x08 | lastbit | sig_sk;  /* CS=high */
          usbitems->stream[usbitems->streampos++] = 0x0B;                     /* CS=out, DO=out, SK=out */

          usbitems->stream[usbitems->streampos++] = 0x80;                     /* set signals */
          usbitems->stream[usbitems->streampos++] = 0x00 | lastbit | sig_sk;  /* CS=low */
          usbitems->stream[usbitems->streampos++] = 0x0B;                     /* CS=out, DO=out, SK=out */
        } else {  /* t_flags & 0x1 == 1: command; t_flags & 0x1 == 0: data */
          /*byte dc = (t_flags & 0x1);*/
          byte sig_dc = (t_flags & 0x1);
          byte write_on_falling = (sdcd->spi.cpol == sdcd->spi.cpha) ? 1 : 0;

          /* toggle level of D/C if data-bit==HIGH */
          /*if (t_flags & 0x4)
            dc = (dc) ? 0 : 1;*/
          /* toggle active high vs. active low depending on sdcd->spi.data_high; */
          if (sdcd->spi.data_high)
            sig_dc = (sig_dc) ? 0 : 1;

          /* clock out D/C-bit, data change on raising or falling edge of SK */
          usbitems->stream[usbitems->streampos++] = 0x1A | write_on_falling;
          usbitems->stream[usbitems->streampos++] = 0;       /* 1 bit */
          usbitems->stream[usbitems->streampos++] = sig_dc;

          /* clock out data byte to DO, data change on raising or falling edge of SK  */
          usbitems->stream[usbitems->streampos++] = 0x12 | write_on_falling;
          usbitems->stream[usbitems->streampos++] = 7;       /* clock out 8 bits */
          usbitems->stream[usbitems->streampos++] = t_data;  /* data byte */
        }
      } else { /* SDPROTO_EMULATION */
        usbitems->stream[usbitems->streampos++] = t_data;
      }

      if (usbitems->streampos >= usbitems->streamsize)
        SDCONNusb_commit(sdcd);
    break;
#endif
    case SDHWT_USBIOW24: 
      {
        byte oldcs = (usbitems->store & 0x20) ? 1: 0;

        /* t_flags:  0000  8421
                              ^--- 000x: x=0: t_data == data byte, x=1: t_data == command byte
                             ^---- 0010: cs-line change
                             ^^--- 0011: enable/disable rc5 (t_data=0: disable, t_data=1: enable)
                            ^-^--- 010x: clear (x=0) or set (x=1) leds/pins according to mask in t_data
                            ^^^--- 0110: toggle leds/pins according to mask in t_data
                            ^^^--- 0111: set leds/pins according to mask in t_data
         */
        /* cs-line change  or    set/toogle led mask */
        if (((t_flags == 0x02) && (t_data != oldcs)) || ((t_flags >= 0x04) && (t_flags <= 0x07))) {

          SDCONNusb_commit(sdcd);

#ifdef HAVE_LIBPTHREAD
          if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
            fp_pthread_mutex_lock( &mutex_usb );
#endif

          if (t_flags == 0x02) {
            usbitems->store &= 0xCF;
            usbitems->store |= ((t_data) ? 0x20 : 0x10);
          } else {
            if (t_flags == 0x05) {         /* set according to mask in t_data */
              t_data = usbitems->store | t_data;
            } else if (t_flags == 0x04) {  /* clear according to mask in t_data */
              t_data = usbitems->store & (0xFF ^ t_data);
            } else if (t_flags == 0x06) {  /* toggle according to mask in t_data */
              t_data = usbitems->store ^ t_data;
            }
            /* t_flags == 0x07 -> leave t_data unchanged */

            t_data &= 0xCF;                /* mask out cs-flags (must not be changed in here!) */
            usbitems->store &= 0x30;       /* clear all but cs-flags */
            usbitems->store |= t_data;     /* set new signals */
          }

          /* iow24: i2c: switch lines */
          IOW_FILLSTREAM(usbitems->stream, IOW_I2C_WRITE_REPORT, 0xC2, 0x70, (byte)(usbitems->store) ,0,0,0,0);

          if (fp_usb_control_msg(
                usbitems->usb_dev, USB_DT_HID, IOW_REQ_SET_REPORT, 0x200, 1, usbitems->stream, 8, usbitems->write_timeout
                             ) < 0) {
            sd_error(SERDISP_ERUNTIME, "%s(): IOW/I2C CS change / setting LEDs failed", __func__);

#ifdef IOW24_USE_USB_CLEAR_HALT
          } else if (fp_usb_clear_halt(usbitems->usb_dev, usbitems->out_ep) < 0) {
#else
          } else if (fp_usb_bulk_read(usbitems->usb_dev, usbitems->out_ep, usbitems->stream, 8, usbitems->write_timeout) < 0) {
#endif

            /* read acknowledge */
            sd_error(SERDISP_ERUNTIME, "%s(): IOW/I2c reading ack report failed", __func__);
          }


#ifdef HAVE_LIBPTHREAD
          if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
            fp_pthread_mutex_unlock( &mutex_usb );
        } else if (t_flags == 0x03) {  /* enable/disable rc5-mode */

          SDCONNusb_commit(sdcd);

          if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
            fp_pthread_mutex_lock( &mutex_usb );

          IOW_FILLSTREAM(usbitems->stream, IOW_RC5_ENABLE_REPORT, t_data, 0, 0 ,0,0,0,0);

          if (fp_usb_control_msg(
                usbitems->usb_dev, USB_DT_HID, IOW_REQ_SET_REPORT, 0x200, 1, usbitems->stream, 8, usbitems->write_timeout
                             ) < 0) {
            sd_error(SERDISP_ERUNTIME, "%s(): IOW/RC5 %s RC5-mode failed", ((t_data) ? "enabling" : "disabling"), __func__);
#ifdef IOW24_USE_USB_CLEAR_HALT
          } else if (fp_usb_clear_halt(usbitems->usb_dev, usbitems->out_ep) < 0) {
#else
          } else if (fp_usb_bulk_read(usbitems->usb_dev, usbitems->out_ep, usbitems->stream, 8, usbitems->write_timeout) < 0) {
#endif  /* IOW24_USE_USB_CLEAR_HALT */
            /* read acknowledge */
            sd_error(SERDISP_ERUNTIME, "%s(): IOW/RC5 reading ack report failed", __func__);
          }

          if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
            fp_pthread_mutex_unlock( &mutex_usb );
#endif  /* HAVE_LIBPTHREAD */
        } else  if (usbitems->laststatus != (t_flags & 0x01) ) {
          SDCONNusb_commit(sdcd);
          usbitems->laststatus = (t_flags & 0x01);
        }

        if (t_flags <= 0x01) {
          usbitems->stream[2 + usbitems->streampos++] = t_data; /* byte 0 and 1 are needed for repID and value */
          if (usbitems->streampos+1 >= 6)
            SDCONNusb_commit(sdcd);
        }
      }
      break;
    case SDHWT_USBL4ME5I:
      {
        if (t_flags & 0x01) {  /* single byte command -> commit immediatly before and afterwards  */
          USBL4ME5I_CLEAR_STALL();
          SDCONNusb_commit(sdcd);
          if (t_data != 0x02 || !(t_flags & 0x02)) {
            usbitems->stream[usbitems->streampos++] = t_data;
            SDCONNusb_commit(sdcd);

            if (t_data == 0x01) {
              /* dirty hack to avoid stalling display on some kernel / udev combinations */
              char tmp_buf[60];
              fp_usb_interrupt_read(usbitems->usb_dev, usbitems->in_ep, tmp_buf, (int)sizeof(tmp_buf), 1);
              fp_usb_clear_halt(usbitems->usb_dev, usbitems->in_ep); 
            }
          } else {
            int page, chunk;
            /* command 0x02 (clear display) not working as expected -> emulate it */

            for (page = 0; page <= 7; page++) {
              for (chunk = 0; chunk <= 2; chunk++) {
                int endm = 56;  if ( (chunk+1)*56 > 128) endm = 16;
                usbitems->streampos = 0;
                usbitems->stream[0] = 0x12;
                usbitems->stream[1] = page;
                usbitems->stream[2] = chunk*56;
                usbitems->stream[3] = endm;
                memset(&(usbitems->stream[4]), 0x00, endm);
                usbitems->streampos = 4+endm;
                SDCONNusb_commit(sdcd);
              }
            }
          }
          USBL4ME5I_CLEAR_STALL();
        } else {  /* multibyte command: -> commit has to be done by the user */
          if (usbitems->streampos < usbitems->streamsize)
            usbitems->stream[usbitems->streampos++] = t_data;
          else   /* should never happen (if so: -> bug in driver serdisp_specific_l4m.c) */
            sd_error(SERDISP_ERUNTIME, "%s(): L4M_E-5i/LCD stream out of bounds (%d > %d)",
                     __func__, usbitems->streampos, usbitems->streamsize);
        }
      }
      break;
    case SDHWT_USB13700:
      {
        usbitems->stream[usbitems->streampos++] = t_data;
        if (usbitems->streampos >= usbitems->streamsize)
          SDCONNusb_commit(sdcd);
      }
      break;
  };

  sdcd->debug_count++;
}

/* *********************************
   long SDCONNusb_read(sdcd, flags)
   *********************************
   read a byte from the serdisp connect device
   *********************************
   sdcd   ... serdisp connect descriptor
   flags  ... which bytes are to read
   *********************************
   returns a data read from the serdisp connect device
   =================================================

*/
long SDCONNusb_read(serdisp_CONN_t* sdcd, byte flags) {
  /* dummy function for now */

  if (sd_runtime_error())
    return 0;

  return 0;
}


/* *********************************
   int SDCONNusb_readstream(sdcd, buf, count)
   *********************************
   read a stream from the serdisp connect device into a buffer
   *********************************
   sdcd   ... serdisp connect descriptor
   buf    ... buffer for stream
   count  ... read up to 'count' bytes info the buffer (note: count <= sizeof(buf) !!!)
   *********************************
   returns the number of bytes read.
      0 ... indicates end of stream
     -1 ... error when reading stream
   =================================================

*/
int SDCONNusb_readstream(serdisp_CONN_t* sdcd, byte* buf, int count) {
  int rc = 0;
  serdisp_usbdev_t* usbitems = (serdisp_usbdev_t*)(sdcd->extra);

  if (sd_runtime_error())
    return 0;

  switch (usbdevices[usbitems->devID].deviceID) {
    case SDHWT_USB13700: {
      rc = fp_usb_bulk_read(usbitems->usb_dev, usbitems->in_ep, (char*)buf, count, usbitems->read_timeout );
      if( rc<0 )
        if( errno!=EAGAIN )
          sd_error(SERDISP_ERUNTIME, "%s(): bulk read could not read from device: %s (%d)", __func__, strerror(errno), errno);
    }
    break;
    default: {
      rc = fp_usb_interrupt_read(usbitems->usb_dev, usbitems->out_ep, (char*)buf, count, usbitems->read_timeout );
      if( rc<0 )
        if( errno!=EAGAIN )
          sd_error(SERDISP_ERUNTIME, "%s(): IOW/READ could not read from device: %s (%d)", __func__, strerror(errno), errno);
    }
  }

  return rc;
}



/* *********************************
   void SDCONNusb_commit(sdcd)
   *********************************
   commits an usb write stream
   *********************************
   sdcd   ... serdisp connect descriptor
*/
void SDCONNusb_commit(serdisp_CONN_t* sdcd) {
  serdisp_usbdev_t* usbitems = (serdisp_usbdev_t*)(sdcd->extra);

#ifdef HAVE_LIBPTHREAD
  if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
    fp_pthread_mutex_lock( &mutex_commit );
#endif

  if (usbitems->streampos > 0) {
    switch (usbdevices[usbitems->devID].deviceID) {
#ifdef ENABLE_FTDI
      case SDHWT_USBFTDI2232:
      case SDHWT_USBFTDI245: 
        if(fp_usb_bulk_write(
            usbitems->usb_dev, 
            usbitems->out_ep, 
            usbitems->stream, usbitems->streampos,
            usbitems->write_timeout
                          ) < 0) {
          sd_error(SERDISP_ERUNTIME, "%s(): FTDI: usb_bulk_write() failed (cause: %s)", __func__, strerror(errno));
          sd_runtimeerror = 1;
        }
      break;
#endif
      case SDHWT_USBIOW24: {
        usbitems->stream[0] = IOW_LCD_WRITE_REPORT;  /* iow24: lcd reportID */
        usbitems->stream[1] = usbitems->streampos + ((usbitems->laststatus) ? 0 : 0x80);

        if (fp_usb_control_msg(
              usbitems->usb_dev, USB_DT_HID, IOW_REQ_SET_REPORT, 0x200, 1, usbitems->stream, 8, usbitems->write_timeout
           ) < 0) {
          sd_error(SERDISP_ERUNTIME, "%s(): IOW/LCD write failed (cause: %s)", __func__, strerror(errno));
          sd_runtimeerror = 1;
        }
      }
      break;
      case SDHWT_USBL4ME5I: {
        if (fp_usb_bulk_write(
              usbitems->usb_dev, usbitems->out_ep, usbitems->stream, usbitems->streampos, usbitems->write_timeout
            ) < 0) {
#if 0
          int ii;
          fprintf(stderr, "ep: 0x%02x  length: %d: ", usbitems->out_ep, usbitems->streampos);
          for (ii = 0; ii < usbitems->streampos; ii++)
            fprintf(stderr, "%02x ", usbitems->stream[ii]);
          fprintf(stderr, "\n");
#endif
          sd_error(SERDISP_ERUNTIME, "%s(): L4M_E-5i/LCD commiting buffer failed, error: %s", __func__, strerror(errno));
        }
      }
      break;
      case SDHWT_USB13700: {
        if (fp_usb_bulk_write(
              usbitems->usb_dev, usbitems->out_ep, usbitems->stream, usbitems->streampos, usbitems->write_timeout
            ) < 0) {
          sd_error(SERDISP_ERUNTIME, "%s(): USB13700 commiting buffer failed, error: %s", __func__, strerror(errno));
        }
      }
      break;
    }
    usbitems->streampos = 0;
  }
#ifdef HAVE_LIBPTHREAD
  if ( SDFCTPTR_checkavail(SDFCTPTR_PTHREAD) )
    fp_pthread_mutex_unlock( &mutex_commit );
#endif
}



/* *********************************
   void SDCONNusb_usleep(sdcd, usec)
   *********************************
   delays usec microseconds
   *********************************
   sdcd   ... serdisp connect descriptor
   usec   ... delay timye
*/
void SDCONNusb_usleep(serdisp_CONN_t* sdcd, unsigned long usec) {
  SDCONNusb_commit(sdcd);
  usleep(usec);
}



/* internal use only:
   replace all '/' and ' ' through '_'
*/
void normalise_string(char* str, int maxlen) {
  int i;
  if (maxlen == -1)
    maxlen = strlen(str);
  for (i = 0 ; i < strlen(str) ; i++) {
    if (str[i] == '/' || str[i] == ' ')
      str[i] = '_';
  }
}


/* *********************************
   usb_dev_handle* SDCONNusb_find(vendorID, productID, bcdDevice, hr_name, occurrence, &dev_idx, &dev_ptr)
   *********************************
   find an usb device
   *********************************
   vendorID  ... vendor ID
   productID ... product ID
   bcdDevice ... device ID (if -1 => don't care)
   hr_name   ... human readable device description (eg.: FTDI/DLM2232M)
   &dev_idx  ... index in usbdevices[]
   &dev_ptr  ... pointer to usb device (call by reference)
   *********************************
   returns a handle to an usb device or (usb_dev_handle*)0 if operation was unsuccessful
*/
usb_dev_handle* SDCONNusb_find(int vendorID, int productID, int bcdDevice, char* hr_name, int occurrence, int* dev_idx, struct usb_device** dev_ptr) {
  usb_dev_handle* usb_dev = 0;

  struct usb_bus* bus = 0;
  struct usb_device* dev = 0;
  int occ_count = 0;
  int found = 0;          /* usb device found in device list */
  int usbdev_found = 0;   /* usb device found in usbdevices[] */
  int idx = 0;

  fp_usb_init();
  if (fp_usb_find_busses() < 0) {
    sd_error(SERDISP_ENXIO, "%s(): usb_find_busses() failed. error: %s", __func__, strerror(errno));
    return (usb_dev_handle*)0;
  }
  if (fp_usb_find_devices() < 0) {
    sd_error(SERDISP_ENXIO, "%s(): usb_find_devices() failed. error: %s", __func__, strerror(errno));
    return (usb_dev_handle*)0;
  }

  /* scan usb bus for device given by sdcdev */
  bus = fp_usb_get_busses();
  while (!found && bus) {
    dev = bus->devices;
    while (!found && dev) {
      usb_dev = fp_usb_open(dev);
      if (usb_dev) {
        int hr_match = 0;

        /* no numeric vendorID: try if hr_name contains textual description of device */
        if (vendorID == -1 && strlen(hr_name) > 0) {
          char string[255];

          char* delim_idx;
          int hr_manu_match = -1;
          int hr_prod_match = 0;
          int hr_sern_match = -1;

          char* hr_name_dup = strdup(hr_name);

          delim_idx = index(hr_name_dup, '/');

          if (delim_idx) {
            if (serdisp_ptrdistance(delim_idx, hr_name_dup) > 0) {
              fp_usb_get_string_simple(usb_dev, dev->descriptor.iManufacturer, string, sizeof(string));
              normalise_string(string, -1);
              normalise_string(hr_name_dup, serdisp_ptrdistance(delim_idx, hr_name_dup));
              hr_manu_match = sdtools_ismatching(hr_name, serdisp_ptrdistance(delim_idx, hr_name_dup), string, -1);
            } else {
              hr_manu_match = -1;  /* not given => don't care */
            }

            hr_name = (delim_idx+1);

            delim_idx = index(hr_name_dup, '/');

            fp_usb_get_string_simple(usb_dev, dev->descriptor.iProduct, string, sizeof(string));
            normalise_string(string, -1);
            normalise_string(hr_name_dup, serdisp_ptrdistance(delim_idx, hr_name_dup));
            hr_prod_match =
              sdtools_ismatching(
                hr_name_dup,
                (!delim_idx) ? -1 : serdisp_ptrdistance(delim_idx, hr_name_dup),
                string,
                -1
              );

            if (delim_idx) {
              fp_usb_get_string_simple(usb_dev, dev->descriptor.iSerialNumber, string, sizeof(string));
              normalise_string(string, -1);
              normalise_string((delim_idx+1), -1);
              hr_sern_match = sdtools_ismatching((delim_idx+1), -1, string, -1);
            }
          } else {
            hr_manu_match = -1;
            hr_sern_match = -1;
            fp_usb_get_string_simple(usb_dev, dev->descriptor.iProduct, string, sizeof(string));
            normalise_string(string, -1);
            normalise_string(hr_name_dup, -1);
            hr_prod_match = sdtools_ismatching(hr_name_dup, -1, string, -1);
          }

          free(hr_name_dup);

          hr_match = (hr_manu_match != 0) && (hr_prod_match != 0) && (hr_sern_match != 0);
          if (hr_match) {
            vendorID = dev->descriptor.idVendor;
            productID = dev->descriptor.idProduct;
            bcdDevice = -1;
          }
        }

        if ( hr_match ||
             ( (dev->descriptor.idVendor ==  vendorID) && 
               (dev->descriptor.idProduct == productID) &&
               (bcdDevice == -1 || dev->descriptor.bcdDevice == bcdDevice)
             )
           ) {
          if (occurrence == occ_count) {
            found = 1;
            sd_debug(1, "%s(): device found: 0x%04x/0x%04x/0x%04x", __func__,
                         dev->descriptor.idVendor, dev->descriptor.idProduct, dev->descriptor.bcdDevice);
            idx = 0;
            usbdev_found = 0;
            while (!usbdev_found && idx < sizeof(usbdevices) / sizeof(serdisp_usbdevices_t)) {
              if ( (vendorID == usbdevices[idx].vendorID) && 
                   (productID == usbdevices[idx].productID) &&
                   (bcdDevice == -1 || usbdevices[idx].bcdDevice == -1 || bcdDevice == usbdevices[idx].bcdDevice)
                 ) {
                 usbdev_found = 1;
              } else {
                idx++;
              }
            }

            if (!usbdev_found) {
              sd_error(SERDISP_ENOTSUP, "%s(): device 0x%04x/0x%04x not found in supported devices list", 
                                         __func__, dev->descriptor.idVendor, dev->descriptor.idProduct);
            }

          } else
            occ_count++;  /* device would be ok, but we search for another one */
        }
      }
      if (!found) fp_usb_close(usb_dev);
     if (!found) dev = dev->next;
    }
    bus = bus->next;
  }

  if (found && usbdev_found) {
    *dev_idx = idx;
    *dev_ptr = dev;
    return usb_dev;
  } else {
    *dev_ptr = 0;
    return (usb_dev_handle*)0;
  }
}


/* *********************************
   int SDCONNusb_claim_interfaces(serdisp_CONN_t* sdcd, ifmask, &claimed)
   *********************************
   claim interfaces
   *********************************
   sdcd      ... serdisp connect descriptor
   ifmask    ... interface bitfield (0xFF: claim all interfaces, 0x00: no claiming)
   &claimed  ... bitfield: claimed interfaces (eg: 0000 0011: interface 0 and 1 claimed)
   *********************************
   returns 0 if successful, -1 if not
*/
int SDCONNusb_claim_interfaces(serdisp_CONN_t* sdcd, byte ifmask, unsigned char* claimed) {
  int i, rc;
  serdisp_usbdev_t* usbitems = (serdisp_usbdev_t*)(sdcd->extra);
  struct usb_config_descriptor *config;

  if (usbitems->dev->descriptor.bNumConfigurations == 0) {
     sd_error(SERDISP_ERUNTIME, 
              "%s(): bNumConfigurations == 0. Try connecting device without using a USB hub.",
              __func__
             );
     return -1;
  }

  config = &(usbitems->dev)->config[0]; /* dev->config->bNumInterfaces crashes with bsd */

  /* claim interfaces */
  *claimed = 0;
  for (i = 0; i < config->bNumInterfaces; i++) {
    if ((ifmask == 0xFF) || ( ifmask & (1 << i)) ) {
      /* detach from kernel module if bound to one */
      if (fp_usb_detach_kernel_driver_np)
        fp_usb_detach_kernel_driver_np(usbitems->usb_dev, i);

      if ((rc = fp_usb_claim_interface(usbitems->usb_dev, i)) != 0) {
        sd_error(SERDISP_EACCES, 
                 "%s(): usb_claim_interface() unsuccessful for interface %d. rc=%d, error: %s",
                 __func__, i, rc, strerror(errno)
                );
        return -1;
      } else {
        *claimed |= (1<<i);
        sd_debug(1, "%s(): usb_claim_interface() successful for interface %d\n", __func__, i);
      }
    }
  }
  return 0;
}
