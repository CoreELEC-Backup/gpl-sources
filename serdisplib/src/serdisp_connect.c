/** \file    serdisp_connect.c
  *
  * \brief   Functions for accessing supported output devices (parport, serial device, ..)
  * \date    (C) 2003-2010
  * \author  wolfgang astleitner (mrwastl@users.sourceforge.net)
  */

/*
 *************************************************************************
 *
 * parport part initially based on:
 *   http://www.thiemo.net/projects/orpheus/optrex/
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

#include "../config.h"


#include <syslog.h>

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <termios.h>
#include <sys/ioctl.h>

/*#include <sys/resource.h>*/


#include "serdisplib/serdisp_connect.h"
#include "serdisplib/serdisp_tools.h"
#include "serdisplib/serdisp_messages.h"
#include "serdisplib/serdisp_fctptr.h"

#ifdef HAVE_LIBUSB
#include "serdisplib/serdisp_connect_usb.h"
#endif /* HAVE_LIBUSB */


/* some global defines */

/* directIO only with linux and i386 */
#if defined(__linux__) && (defined(__i386__) || defined(__x86_64__))
  #define __sd_linux_use_directIO__ 1
#endif


/* some os-specific includes / defines */
#if defined(__linux__)
  #include <linux/ppdev.h>
  #include <linux/parport.h>
  #if defined (__sd_linux_use_directIO__)  /* sys/io.h only if directIO is supported */
    #include <sys/io.h>
  #endif
  #include <stdlib.h>
  #define OUTB(_d, _p)  outb( (_d), (_p) )
  #define INB(_p)       inb( (_p) )
  #define SERDISP_DEFAULTDEVICE     "/dev/parport0"
#elif defined(__FreeBSD__) || defined(__OpenBSD__)
  #define PPWDATA    PPISDATA
  #define PPWCONTROL PPISCTRL
  #define PPRDATA    PPIGDATA
  #define PPRCONTROL PPIGCTRL
  #define PPRSTATUS  PPIGSTATUS
  #include <dev/ppbus/ppi.h>
  #define SERDISP_DEFAULTDEVICE     "/dev/ppi0"
/* -- netbsd support suspended because it doesn't compile 
      and no-one seems to give a damn anyways ... --
  #elif defined(__NetBSD__)
    #define PPWDATA    PPISDATA
    #define PPWCONTROL PPISCTRL
    #define PPRDATA    PPIGDATA
    #define PPRCONTROL PPIGCTRL
    #define PPRSTATUS  PPIGSTATUS
    #include <dev/ppbus/ppui.h>
    #define SERDISP_DEFAULTDEVICE     "/dev/ppi0"
*/
#elif defined(__sun__)
  #define PPWDATA    ECPPIOC_SETDATA
  #define PPRDATA    ECPPIOC_GETDATA
  #include <sys/ecppio.h>
  #include <sys/ecppsys.h>
  #define SERDISP_DEFAULTDEVICE     "/dev/ecpp0"
#else
  #error Unsupported operating system!
  #define PPWDATA (-1)
#endif

/* hiddev-specific defines. only available with linux */
/* some os-specific includes / defines */
#if defined(__linux__)
  #include <sys/types.h>
  #include <asm/types.h>
  #include <linux/hiddev.h>
  #define __sd_available_hiddev__ 1
#endif


/* supported connection types. incl. aliases */
serdisp_conntype_t serdisp_connection_types[] = {
   {"PAR",     SDCT_PP,   "" }
  ,{"PARPORT", SDCT_PP,   "" }
  ,{"SERRAW",  SDCT_SRAW, "" } 
  ,{"SERPORT", SDCT_SRAW,  "" }
/*  ,{"I2C",     SDCT_I2C, "" } */
  ,{"IOW24",   SDCT_IOW24,  "" }
#if defined(__sd_available_hiddev__)
  ,{"HID",     SDCT_HIDDEV,  "" }
  ,{"HIDDEV",  SDCT_HIDDEV,  "" }
#endif
  ,{"RS232",   SDCT_RS232,   "" }
  ,{"OUT",     SDCT_OUT,   "" }
};



/* signal names that are recognised by the parser for customisable wirings */
typedef struct serdisp_signalnames_s {
  short conntype;
  short hardwaretype;
  long value;
  int  activelow;
  char* name;
  char* aliasnames;
} serdisp_signalnames_t;



/* parallel port: signal names + aliases. needed for customisable wiring */
serdisp_signalnames_t serdisp_signalnames[] = {
/* conntype  hw-type       value   activelow  name  aliases */
   {SDCT_PP, SDHWT_ALL,    SD_PP_D0,    0    , "D0", "AD0"}
  ,{SDCT_PP, SDHWT_ALL,    SD_PP_D1,    0    , "D1", "AD1"}
  ,{SDCT_PP, SDHWT_ALL,    SD_PP_D2,    0    , "D2", "AD2"}
  ,{SDCT_PP, SDHWT_ALL,    SD_PP_D3,    0    , "D3", "AD3"}
  ,{SDCT_PP, SDHWT_ALL,    SD_PP_D4,    0    , "D4", "AD4"}
  ,{SDCT_PP, SDHWT_ALL,    SD_PP_D5,    0    , "D5", "AD5"}
  ,{SDCT_PP, SDHWT_ALL,    SD_PP_D6,    0    , "D6", "AD6"}
  ,{SDCT_PP, SDHWT_ALL,    SD_PP_D7,    0    , "D7", "AD7"}
  ,{SDCT_PP, SDHWT_ALL,    SD_PP_S3,    0    , "S3", "ERROR"}
  ,{SDCT_PP, SDHWT_ALL,    SD_PP_S4,    0    , "S4", "SELECT,SLCT"}
  ,{SDCT_PP, SDHWT_ALL,    SD_PP_S5,    0    , "S5", "PE,PERROR"}
  ,{SDCT_PP, SDHWT_ALL,    SD_PP_S6,    0    , "S6", "ACK,ACKNOWLEDGE"}
  ,{SDCT_PP, SDHWT_ALL,    SD_PP_S7,    1    , "S7", "nBUSY"}
  ,{SDCT_PP, SDHWT_PARSER, SD_PP_C0,    1    , "C0", "nSTROBE,nSTRB"}
  ,{SDCT_PP, SDHWT_PARSER, SD_PP_C1,    1    , "C1", "nLINEFD,nAUTO, nAUTOFD"}
  ,{SDCT_PP, SDHWT_PARSER, SD_PP_C2,    0    , "C2", "INIT"}
  ,{SDCT_PP, SDHWT_PARSER, SD_PP_C3,    1    , "C3", "nSELECT,nSELIN,nSEL"}
  ,{SDCT_PP, SDHWT_USB,    SD_PP_C0,    0    , "C0", "BD0,nSTROBE,nSTRB"}
  ,{SDCT_PP, SDHWT_USB,    SD_PP_C1,    0    , "C1", "BD1,nLINEFD,nAUTO, nAUTOFD"}
  ,{SDCT_PP, SDHWT_USB,    SD_PP_C2,    0    , "C2", "BD2,INIT"}
  ,{SDCT_PP, SDHWT_USB,    SD_PP_C3,    0    , "C3", "BD3,nSELECT,nSELIN,nSEL"}
  ,{SDCT_PP, SDHWT_USB,    SD_PP_C4,    0    , "C4", "BD4"}  /* only when using devices with more than 12 output signals */
  ,{SDCT_PP, SDHWT_USB,    SD_PP_C5,    0    , "C5", "BD5"}  /* (eg. certain usb-chips) */
  ,{SDCT_PP, SDHWT_USB,    SD_PP_C6,    0    , "C6", "BD6"}
  ,{SDCT_PP, SDHWT_USB,    SD_PP_C7,    0    , "C7", "BD7"}
};



/* default devices / ports: 
 * devices/ports not found in here have to be specified using a connection type prefix 
 * (eg. "PAR:/dev/somepardev0" or "SERRAW:/dev/sometty0") 
 */

/* matching modes: */
#define SD_MATCHMODE_STARTSWITH   0x02
#define SD_MATCHMODE_EXACT        0x04
#define SD_MATCHMODE_PORT         0x08


typedef struct serdisp_defaultdevs_s {
  char* devname;
  char* connname;
  int matchmode;
} serdisp_defaultdevs_t;


/* devices where the corresponding connection type is pre-defined */
/* -> instead of  PARPORT:/dev/parport0  one may simply write:  /dev/parport0 */
serdisp_defaultdevs_t serdisp_defaultdevices[] = {
   {"/dev/parport",    "PARPORT", SD_MATCHMODE_STARTSWITH }
  ,{"/dev/ppi",        "PARPORT", SD_MATCHMODE_STARTSWITH }
  ,{"/dev/ecpp",       "PARPORT", SD_MATCHMODE_STARTSWITH }
  ,{"0x378",           "PARPORT", SD_MATCHMODE_EXACT }
  ,{"0x278",           "PARPORT", SD_MATCHMODE_EXACT }
  ,{"/dev/ttyS",       "SERRAW",  SD_MATCHMODE_STARTSWITH } 
  ,{"/dev/ttyUSB",     "SERRAW",  SD_MATCHMODE_STARTSWITH } 
  ,{"/dev/cua",        "SERRAW",  SD_MATCHMODE_STARTSWITH } 
  ,{"0x3f8",           "SERRAW",  SD_MATCHMODE_PORT }
  ,{"0x2f8",           "SERRAW",  SD_MATCHMODE_PORT }
  ,{"/dev/iowarrior",  "IOW24",   SD_MATCHMODE_STARTSWITH }
#if defined(__sd_available_hiddev__)
  ,{"/dev/hiddev",     "HIDDEV",  SD_MATCHMODE_STARTSWITH }
  ,{"/dev/usb/hiddev", "HIDDEV",  SD_MATCHMODE_STARTSWITH }
#endif
};

/* internal functions */
static int      SDCONN_confinit            (serdisp_CONN_t* sdcd);


/**
  * \brief   opens an output device
  *
  * \param   sdcdev        device name or port-number of device to open
  *
  * \b Format: <tt>[conntype:]device</tt>
  * \li conntype (connection type) is case insensitive
  * \li no connection type is needed for known devices (list below) 
  *
  * <b>List of known devices:</b>
    \verbatim
    device           conntype remark
    ---------------- -------- ----------------------------------------
    /dev/parportX    PAR      or PARPORT (alias name)
    /dev/ppiX        PAR      bsd
    /dev/ecppX       PAR      solaris
    0x378            PAR      first parallel port, linux, direct IO
    0x278            PAR      second parallel port, linux, direct IO
    /dev/ttySX       SERRAW
    /dev/ttyUSBX     SERRAW
    /dev/cuaX        SERRAW
    0x3f8            SERRAW   first serial port, linux, direct IO
    0x2f8            SERRAW   second serial port, linux, direct IO    \endverbatim
  *
  * \b Examples:
    \verbatim
    sdcdev                   remark
    -----------------------  -----------------------------------------------
    "/dev/parport0"          parallel port, ioctl, linux
    "0x378"                  parallel port, direct IO, linux 86 only
    "/dev/ecpp0"             parallel port, ioctl, solaris
    "/dev/ttyS0"             serial port, ioctl, should be os-indepentend (POSIX)
    "0x3f8"                  serial port, direct IO, linux x86 only

    "serraw:/dev/ttyS0"      serial device, ioctl
    "SERRAW:/dev/ttyS0"      the same as above because connection type is case-insensitive
    "SERPORT:/dev/ttyS0"     the same because SERPORT and SERRAW are synonyms
    "serraw:0x3f8"           serial device, direct IO
    "par:/dev/parport0"      linux, ioctl    \endverbatim
  *
  * \retval      !NULL     serdisp connect descriptor
  * \retval      NULL      operation was unsuccessful
  *
  * \attention
  * direct IO is only supported with linux (for now - maybe someone may try if this is also valid with freebsd).
  *
  */
serdisp_CONN_t* SDCONN_open(const char sdcdev[]) {
  serdisp_CONN_t* sdcd = 0;
  char connproto[20];  
  char* devname;
  char* idx;  /* index of ':' in conntype:devicename */

  int found = 0;
  int i = 0;

#if !defined(__sun__)
  int mode;
#else
  struct ecpp_transfer_parms  etp;
  struct ecpp_regs            eregs;
#endif /* !defined (__sun__) */

  /* initialise function pointers */
  SDFCTPTR_init();

  /* forward USB-specific devices to SDCONNusb_open() */
  if (strncasecmp(sdcdev, "USB:", 4) == 0 || strncasecmp(sdcdev, "USB@", 4) == 0) {
#ifdef HAVE_LIBUSB
    return SDCONNusb_open(sdcdev);
#else
    sd_error(SERDISP_ENOPROTOOPT, "libusb-based connections are disabled (library has been built without libusb-support)");
    return (serdisp_CONN_t*)0;
#endif /* HAVE_LIBUSB */
  }

  /* setup dummy sdcd-struct for output drivers */
  if (strncasecmp(sdcdev, "OUT:", 4) == 0) {
    sdcd = (serdisp_CONN_t*)sdtools_malloc( sizeof(serdisp_CONN_t) );
    if( !sdcd ) {
      sd_error(SERDISP_EMALLOC, "%s(): unable to allocate memory for sdcd", __func__);
      return NULL;
    }
    memset( sdcd, 0, sizeof(serdisp_CONN_t) );
    sdcd->sdcdev       = (char*)sdcdev;
    sdcd->conntype     = SERDISPCONNTYPE_OUT;
    sdcd->hardwaretype = SDHWT_OUT;

    return sdcd;
  }


  if (! (sdcd = (serdisp_CONN_t*)sdtools_malloc(sizeof(serdisp_CONN_t)) ) ) {
    return (serdisp_CONN_t*)0;
  }
  memset(sdcd, 0, sizeof(serdisp_CONN_t));

  /* no usb device: default hardwaretype to SDHWT_PARSER */
  sdcd->hardwaretype = SDHWT_PARSER;

  /* no device given -> use default device */
  sdcd->sdcdev = (sdcdev == 0 || strlen((char*)sdcdev) > 0) ? (char*)sdcdev : SERDISP_DEFAULTDEVICE;

  /* split into connection type (+ protocol) and device name */
  idx = index(sdcd->sdcdev, ':');
  if (!idx) {
     devname = sdcd->sdcdev;
     /* look if a default connection type can be found for given device  */
     i = 0;
     found = 0;
     while (!found && i < sizeof(serdisp_defaultdevices) / sizeof(serdisp_defaultdevs_t) ) {
       switch (serdisp_defaultdevices[i].matchmode) {
         case SD_MATCHMODE_STARTSWITH:
           found = ((strlen(serdisp_defaultdevices[i].devname) <= strlen(devname)) && 
                    (strncmp(devname, serdisp_defaultdevices[i].devname, strlen(serdisp_defaultdevices[i].devname)) == 0)
                   );
           break;
         case SD_MATCHMODE_EXACT:
           found = ((strlen(serdisp_defaultdevices[i].devname) == strlen(devname)) && 
                    (strncmp(devname, serdisp_defaultdevices[i].devname, strlen(devname)) == 0)
                   );
           break;
         case SD_MATCHMODE_PORT:
           found = ((strlen(serdisp_defaultdevices[i].devname) == strlen(devname)) && 
                    (strncasecmp(devname, serdisp_defaultdevices[i].devname, strlen(devname)) == 0)
                   );
           break;
       }
       if (!found)
         i++; 
     }
     if (!found) {
       sd_error(SERDISP_EDEVNOTSUP, "no default connection type found for device %s (should be something like: \"SERRAW:/dev/ttyXYZ0\", \"PAR:/dev/parXYZ0\")", devname);
       SDCONN_close(sdcd);
       return (serdisp_CONN_t*)0;
     } else {
       strcpy(connproto, serdisp_defaultdevices[i].connname);
     }
  } else {
     sdtools_strncpy(connproto, sdcd->sdcdev, ((18 < serdisp_ptrdistance(idx, sdcd->sdcdev)) ? 18 : serdisp_ptrdistance(idx, sdcd->sdcdev)));
     connproto[(18 < serdisp_ptrdistance(idx, sdcd->sdcdev)) ? 18 : serdisp_ptrdistance(idx, sdcd->sdcdev)] = '\0';
     devname = (idx+1);
  }

  /* separate connection type from protocol */
  if (index(connproto, '@')) {
    sd_error(SERDISP_EDEVNOTSUP, "protocol differentiation 'conntype@protocol' is not yet supported by this connection type");
    SDCONN_close(sdcd);
    return (serdisp_CONN_t*)0;
  }

  i = 0;
  found = 0;
  /* look if given connection type is suppported */
  while (!found && i < sizeof(serdisp_connection_types) / sizeof(serdisp_conntype_t) ) {
    if (strncasecmp(connproto, serdisp_connection_types[i].connname, 18) == 0) {
      found = 1;
    } else {
      i++; 
    }
  }

  if (!found) {
    sd_error(SERDISP_EDEVNOTSUP, "connection type '%s' is not supported", connproto);
    free (sdcd);
    return (serdisp_CONN_t*)0;
  }

  sdcd->conntype = serdisp_connection_types[i].conntype;

  sdcd->directIO = 0;

  if (sdcd->conntype == SERDISPCONNTYPE_PARPORT) {

#if defined(__sd_linux_use_directIO__)
    long int temp;

    /* if dev starts with "0x" or "0X" -> directIO via port and outp */
    if (strncasecmp(devname, "0x", 2) == 0) {
      temp = strtol(devname, (char **)NULL, 16);
      if (temp <= 0) {
        sd_error(SERDISP_ENXIO, "invalid port %s (cause: %s)", devname, strerror(errno));
        SDCONN_close(sdcd);
        return (serdisp_CONN_t*)0;
      }

      sdcd->port = (int)temp;

      sdcd->directIO = 1;

      if (sdcd->port < 0x400) {
        if (ioperm(sdcd->port, 3, 255) == -1) {
          sd_error(SERDISP_EACCES, "ioperm(0x%X) failed (cause: %s)", sdcd->port, strerror(errno));
          SDCONN_close(sdcd);
          return (serdisp_CONN_t*)0;
        }
      } else {
        if (iopl(3) == -1) {
          sd_error(SERDISP_EACCES, "iopl failed (cause: %s)", strerror(errno));
          SDCONN_close(sdcd);
          return (serdisp_CONN_t*)0;
        }
      }

      /* get current control bits */
      sdcd->pp_ctrlbits_saved = INB( ( (unsigned short int) ((sdcd->port)+2) ) );

      sd_debug(1, "SDCONN_open(): port 0x%x opened successfully", sdcd->port);
    } else {
#else
    {
#endif /* defined(__sd_linux_use_directIO__) */

      sdcd->directIO = 0;

      if ((sdcd->fd = open(devname, O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0) {
        sd_error(SERDISP_ENXIO, "opening device %s failed (cause: %s)", devname, strerror(errno));
        SDCONN_close(sdcd);
        return (serdisp_CONN_t*)0;
      }

    /* thiemo's driver is written for FreeBSD which doesn't need to claim the parport (as it seems). 
       but linux does so.
    */
#if defined(__linux__)
      /*if ( ioctl(sdcd->fd,PPEXCL) ) {
        sd_error(SERDISP_EACCES, "ioctl(PPEXCL) failed for device %s (cause: %s)", devname, strerror(errno));
        SDCONN_close(sdcd);
        return (serdisp_CONN_t*)0;
      }
      */
      if ( ioctl(sdcd->fd,PPCLAIM,0) ) {
        sd_error(SERDISP_EACCES, "ioctl(PPCLAIM) failed for device %s (cause: %s)", devname, strerror(errno));
        SDCONN_close(sdcd);
        return (serdisp_CONN_t*)0;
      }

#if 0
      mode = IEEE1284_MODE_EPP;
      if (ioctl(sdcd->fd, PPSETMODE, &mode) == -1) {
        sd_debug(1, "ioctl(PPSETMODE) failed to set speed to IEEE1284_MODE_EPP, reverting to SPP (cause: %s)", strerror(errno));
#endif
        mode = PARPORT_MODE_PCSPP;
        if (ioctl(sdcd->fd, PPSETMODE, &mode) == -1) {
          sd_error(SERDISP_EACCES, "ioctl(PPSETMODE) failed for device %s (cause: %s)", devname, strerror(errno));
          SDCONN_close(sdcd);
          return (serdisp_CONN_t*)0;
        }
#if 0
      }
#endif


#elif defined(__sun__)
      ioctl(sdcd->fd, ECPPIOC_GETPARMS, &etp);
      etp.mode = ECPP_DIAG_MODE | ECPP_ECP_MODE ;
      if (ioctl(sdcd->fd, ECPPIOC_SETPARMS, &etp) == -1 ) {
        sd_error(SERDISP_EACCES, "ioctl(ECPPIOC_SETPARMS) failed for device %s (cause: %s)", devname, strerror(errno));
        SDCONN_close(sdcd);
        return (serdisp_CONN_t*)0;
      }
#endif /* defined(_sun__ || __linux__) */

#if defined(__sun__)
      /* get current control bits */
      if (ioctl(sdcd->fd, ECPPIOC_GETREGS, &eregs) == -1) {
        sd_error(SERDISP_EACCES, "ioctl(ECPPIOC_GETREGS) failed for device %s (cause: %s)", devname, strerror(errno));
        SDCONN_close(sdcd);
        return (serdisp_CONN_t*)0;
      }
      sdcd->pp_ctrlbits_saved = (byte)eregs.dsr;
#else
      /* get current control bits */
      if (ioctl(sdcd->fd, PPRCONTROL, &mode) == -1) {
        sd_error(SERDISP_EACCES, "ioctl(PPRCONTROL) failed for device %s (cause: %s)", devname, strerror(errno));
        SDCONN_close(sdcd);
        return (serdisp_CONN_t*)0;
      }
      sdcd->pp_ctrlbits_saved = mode;
#endif /* defined(__sun__) */

      sd_debug(1, "SDCONN_open(): device %s opened successfully", devname);
    } /* if (strncasecmp(sdcdev... */

    /* bit 5 can not be read -> so default it to low */
    sdcd->pp_ctrlbits_saved  &= 0xDF;
  } else if (sdcd->conntype == SERDISPCONNTYPE_SERRAW) {

#if defined(__sd_linux_use_directIO__)
    long int temp;

    /* if dev starts with "0x" or "0X" -> directIO via port and outp */
    if (strncasecmp(devname, "0x", 2) == 0) {
      temp = strtol(devname, (char **)NULL, 16);
      if (temp <= 0) {
        sd_error(SERDISP_ENXIO, "invalid port %s (cause: %s)", devname, strerror(errno));
        SDCONN_close(sdcd);
        return (serdisp_CONN_t*)0;
      }
      sdcd->port = (int)temp;

      sdcd->directIO = 1;

      if (ioperm(sdcd->port, 8, 255) == -1) {
        sd_error(SERDISP_EACCES, "ioperm(0x%X) failed (cause: %s)", sdcd->port, strerror(errno));
        SDCONN_close(sdcd);
        return (serdisp_CONN_t*)0;
      }

      /* clear LCR break */
      OUTB( ((byte)0x00), ( (unsigned short int) ((sdcd->port)+3) ));

      usleep(1);

      /* set LCR break (bit 6) -> TxD should now be +12V (low) */  
      OUTB( ((byte)0x40), ( (unsigned short int) ((sdcd->port)+3) ));

      sd_debug(1, "SDCONN_open(): serial port 0x%x opened successfully", sdcd->port);
    } else {
#else
    {
#endif /* defined(__sd_linux_use_directIO__) */
      if ((sdcd->fd = open(devname, O_RDWR | O_NOCTTY | O_NONBLOCK)) < 0) {
        sd_error(SERDISP_ENXIO, "opening device %s failed (cause: %s)", devname, strerror(errno));
        SDCONN_close(sdcd);
        return (serdisp_CONN_t*)0;
      }

      sdcd->directIO = 0;

      /* backup termstate (will be restored when closing device) */
      tcgetattr (sdcd->fd, &(sdcd->termstate_bkp));

      tcgetattr (sdcd->fd, &(sdcd->termstate));
      sdcd->termstate.c_iflag = IGNBRK | IXOFF| IXON | IGNPAR | IGNCR;
      sdcd->termstate.c_oflag = 0;
      sdcd->termstate.c_cflag = B0 | CLOCAL ;
      sdcd->termstate.c_lflag = 0;
      sdcd->termstate.c_cc[VMIN] = 1;
      sdcd->termstate.c_cc[VTIME] = 0;
      tcflush(sdcd->fd, TCIFLUSH);
      tcsetattr (sdcd->fd, TCSANOW, &(sdcd->termstate));

      tcflow(sdcd->fd, TCION);

      /* clear LCR break */
      if (ioctl(sdcd->fd, TIOCCBRK, 0) < 0) {
        sd_error(SERDISP_EACCES, "ioctl(TIOCCBRK) for device %s failed (cause: %s)", devname, strerror(errno));
      }

      usleep(1);

      /* set LCR break -> TxD should now be +12V (low) */  
      if (ioctl (sdcd->fd, TIOCSBRK, 0) < 0) {
        sd_error(SERDISP_EACCES, "ioctl(TIOCSBRK) for device %s failed (cause: %s)", devname, strerror(errno));
      }

      usleep(2);
    } /* if (strncasecmp(sdcdev... */
  } else if (sdcd->conntype == SERDISPCONNTYPE_IOW24) {
    gen_stream_device_t* iow_dev;
    /* struct returned by ioctl IOW_GETINFO (taken from iowkit.c) */
    struct iowarrior_info_s {
      int vendor;     /* vendor id : supposed to be USB_VENDOR_ID_CODEMERCS in all cases */
      int product;    /* product id : depends on type of chip (USB_DEVICE_ID_CODEMERCS_XXXXX)*/
      char serial[9]; /* the serial number of our chip (if a serial-number is not available this is empty string) */
      int revision;   /* revision number of the chip */
      int speed;      /* USB-speed of the device (0=UNKNOWN, 1=LOW, 2=FULL 3=HIGH) */
      int power;      /* power consumption of the device in mA */
      int if_num;     /* the number of the endpoint */
      unsigned int packet_size; /* size of the data-packets on this interface */
    };

    struct iowarrior_info_s info;

    sdcd->directIO = 0;

    if (! (sdcd->extra = (void*)sdtools_malloc(sizeof(gen_stream_device_t)) ) ) {
      sd_error(SERDISP_EMALLOC, "SDCONN_ioen(): unable to allocate memory for iowarrior 24 device struct");
      free(sdcd);
      return (serdisp_CONN_t*)0;
    }
    memset(sdcd->extra, 0, sizeof(gen_stream_device_t));

    iow_dev = (gen_stream_device_t*)(sdcd->extra);
    iow_dev->streamsize = 8;

    if (! (iow_dev->stream = (byte*) sdtools_malloc( sizeof(byte) * iow_dev->streamsize ) ) ) {
      sd_error(SERDISP_EMALLOC, "SDCONN_open(): cannot allocate stream buffer");
      free(sdcd->extra);
      free(sdcd);
      return (serdisp_CONN_t*)0;
    }


    if ((sdcd->fd = open(devname, O_RDWR | O_NONBLOCK)) < 0) {
      sd_error(SERDISP_ENXIO, "opening device %s failed (cause: %s)", devname, strerror(errno));
      SDCONN_close(sdcd);
      return (serdisp_CONN_t*)0;
    }
    if(ioctl(sdcd->fd, IOW_GETINFO, &info) < 0) {
      sd_error(SERDISP_ENXIO, "getting info for device %s failed (cause: %s)", devname, strerror(errno));
      SDCONN_close(sdcd);
      return (serdisp_CONN_t*)0;
    } else {
      if (info.packet_size != 8) {
        sd_error(SERDISP_ENXIO, 
                 "device %s is not a valid iowarrior 24 special mode device (packet size: %d)",
                 devname, info.packet_size);
        SDCONN_close(sdcd);
        return (serdisp_CONN_t*)0;
      } else {
        iow_dev->packetsize = 8;
        sd_debug(2, "IO Warrior24: Speed: %d  Power: %d", info.speed, info.power);
      }
    }

    /* iow24: enable lcd */
    IOW_FILLSTREAM(iow_dev->stream, IOW_LCD_ENABLE_REPORT, 0x01, 0,0,0,0,0,0);
    if(ioctl(sdcd->fd, IOW_WRITE, iow_dev->stream) < 0) {
      sd_error(SERDISP_ERUNTIME, "SDCONN_open(): IOW/LCD enabling  failed");
    }

    /* iow24: enable i2c */
    IOW_FILLSTREAM(iow_dev->stream, IOW_I2C_ENABLE_REPORT, 0x01, 0,0,0,0,0,0);
    if(ioctl(sdcd->fd, IOW_WRITE, iow_dev->stream) < 0) {
      sd_error(SERDISP_ERUNTIME, "SDCONN_open(): IOW/I2C enabling  failed");
    }

    /* iow24: i2c */
    iow_dev->store = 0xCF | 0x10;
    IOW_FILLSTREAM(iow_dev->stream, IOW_I2C_WRITE_REPORT, 0xC2, 0x70, (byte)(iow_dev->store) ,0,0,0,0);  
    if(ioctl(sdcd->fd, IOW_WRITE, iow_dev->stream) < 0) {
      sd_error(SERDISP_ERUNTIME, "SDCONN_open(): IOW/I2C initialising failed");
    }


    iow_dev->laststatus = 0x01 | 0x02;  /* set laststatus to COMMAND (==1), set bglight on */
    iow_dev->streampos = 0;

#if defined(__sd_available_hiddev__)
  } else if (sdcd->conntype == SERDISPCONNTYPE_HIDDEV) {
    gen_stream_device_t* usbitems;

    if (! (sdcd->extra = (void*)sdtools_malloc(sizeof(gen_stream_device_t)) ) ) {
      sd_error(SERDISP_EMALLOC, "SDCONN_open(): unable to allocate memory for hiddev device struct");
      free(sdcd);
      return (serdisp_CONN_t*)0;
    }
    memset(sdcd->extra, 0, sizeof(gen_stream_device_t));

    usbitems = (gen_stream_device_t*)(sdcd->extra);
    usbitems->streamsize = 60;

    if (! (usbitems->stream = (byte*) sdtools_malloc( sizeof(byte) * usbitems->streamsize ) ) ) {
      sd_error(SERDISP_EMALLOC, "SDCONN_open(): cannot allocate stream buffer");
      free(sdcd->extra);
      free(sdcd);
      return (serdisp_CONN_t*)0;
    }

    if ((sdcd->fd = open(devname, O_RDWR)) < 0) {
      sd_error(SERDISP_ENXIO, "opening device %s failed (cause: %s)", devname, strerror(errno));
      SDCONN_close(sdcd);
      return (serdisp_CONN_t*)0;
    }
#endif  /* defined(__sd_available_hiddev__) */
  } else if (sdcd->conntype == SERDISPCONNTYPE_RS232) {
    gen_stream_device_t* streamitems;

    if (! (sdcd->extra = (void*)sdtools_malloc(sizeof(gen_stream_device_t)) ) ) {
      sd_error(SERDISP_EMALLOC, "SDCONN_open(): unable to allocate memory for stream device struct");
      free(sdcd);
      return (serdisp_CONN_t*)0;
    }
    memset(sdcd->extra, 0, sizeof(gen_stream_device_t));

    streamitems = (gen_stream_device_t*)(sdcd->extra);
    streamitems->streamsize = 4096;

    if (! (streamitems->stream = (byte*) sdtools_malloc( sizeof(byte) * streamitems->streamsize ) ) ) {
      sd_error(SERDISP_EMALLOC, "SDCONN_open(): cannot allocate stream buffer");
      free(sdcd->extra);
      free(sdcd);
      return (serdisp_CONN_t*)0;
    }

    streamitems->streampos = 0;

    /* if O_NDELAY is used goldelox will fail */
    if ((sdcd->fd = open(devname, O_RDWR | O_NOCTTY)) < 0) {
      sd_error(SERDISP_ENXIO, "opening device %s failed (cause: %s)", devname, strerror(errno));
      SDCONN_close(sdcd);
      return (serdisp_CONN_t*)0;
    }

    sdcd->directIO = 0;

    /* backup termstate (will be restored when closing device) */
    tcgetattr (sdcd->fd, &(sdcd->termstate_bkp));

    if (tcgetattr (sdcd->fd, &(sdcd->termstate)) < 0) {
      sd_error(SERDISP_ENXIO, "getting attributes for device %s failed (cause: %s)", devname, strerror(errno));
      SDCONN_close(sdcd);
      return (serdisp_CONN_t*)0;
    }
#if 0
    {
      int tiocm = TIOCM_DTR;
      ioctl(sdcd->fd, TIOCMSET, &tiocm); /* control the reset pin */

      tiocm = 0;
      ioctl(sdcd->fd, TIOCMGET, &tiocm);

      if (tiocm & TIOCM_DTR) {
        sd_debug(2, "%s(): DTR is set on device %s", __func__, devname);
      } else {
        sd_debug(1, "%s(): DTR is NOT set on device %s", __func__, devname);
      }
    }
#endif

    /* extra configuration and initialisation will be done by SDCONN_confinit() prior to 1st read/write operation */
    sdcd->needs_confinit = 1;
  } else { /* unsupported connection type */
    free (sdcd);
    return (serdisp_CONN_t*)0;
  }
  return sdcd;
}


/**
  * \brief   closes an output device
  *
  * \param   sdcd          serdisp connect descriptor
  */
void SDCONN_close(serdisp_CONN_t* sdcd) {
  if (sdcd->hardwaretype & SDHWT_OUT) {
    ;
  } else
#ifdef HAVE_LIBUSB
  if (sdcd->hardwaretype & SDHWT_USB) {
    SDCONNusb_close(sdcd);
  } else
#endif /* HAVE_LIBUSB */
  if (sdcd->conntype == SERDISPCONNTYPE_PARPORT) {
    if ( sdcd->directIO && sdcd->port ) {
#if defined(__sd_linux_use_directIO__)
      if (sdcd->port < 0x400)
        ioperm(sdcd->port, 3, 0);
       else
        iopl(0);
#endif /* defined(__sd_linux_use_directIO__) */
    } else if ( ! sdcd->directIO && sdcd->fd) {
#if defined(__linux__)
      ioctl(sdcd->fd, PPRELEASE,0);
#endif /* defined(__linux__) */
      close(sdcd->fd);
    }
  } else if (sdcd->conntype == SERDISPCONNTYPE_SERRAW) {
    if ( sdcd->directIO && sdcd->port ) { /* directIO */
#if defined(__sd_linux_use_directIO__)

      /* clear LCR break */
      OUTB( ((byte)0x00), ( (unsigned short int) ((sdcd->port)+3) ));

      usleep(1);

      ioperm(sdcd->port, 8, 0);
#endif /* defined(__sd_linux_use_directIO__) */
    } else {  /* ioctl */
      ioctl(sdcd->fd, TIOCCBRK, 0);
      usleep(1);
      tcgetattr (sdcd->fd, &(sdcd->termstate_bkp));
      usleep(1);
      close(sdcd->fd);
    }
  } else if (sdcd->conntype == SERDISPCONNTYPE_IOW24) {
    gen_stream_device_t* iow_dev;

    iow_dev = (gen_stream_device_t*)(sdcd->extra);

    iow_dev->store = 0xCF ^ 0x40; /* disable leds and reset display */

    if (iow_dev->packetsize == 8) {  /* only do this when device was valid */
      /* iow24: i2c */
      IOW_FILLSTREAM(iow_dev->stream, IOW_I2C_WRITE_REPORT, 0xC2, 0x70, (byte)(iow_dev->store) ,0,0,0,0);  
      if(ioctl(sdcd->fd, IOW_WRITE, iow_dev->stream) < 0) {
        sd_error(SERDISP_ERUNTIME, "SDCONN_close(): IOW/I2C de-initialising failed");
      }

      /* iow24: disable i2c */
      IOW_FILLSTREAM(iow_dev->stream, IOW_I2C_ENABLE_REPORT, 0x00, 0,0,0,0,0,0);
      if(ioctl(sdcd->fd, IOW_WRITE, iow_dev->stream) < 0) {
        sd_error(SERDISP_ERUNTIME, "SDCONN_close(): IOW/I2C disabling  failed");
      }

      /* iow24: disable lcd */
      IOW_FILLSTREAM(iow_dev->stream, IOW_LCD_ENABLE_REPORT, 0x00, 0,0,0,0,0,0);
      if(ioctl(sdcd->fd, IOW_WRITE, iow_dev->stream) < 0) {
        sd_error(SERDISP_ERUNTIME, "SDCONN_close(): IOW/LCD disabling  failed");
      }
    }

    free(iow_dev->stream);
    free(sdcd->extra);
    close(sdcd->fd);
#if defined(__sd_available_hiddev__)
  } else if (sdcd->conntype == SERDISPCONNTYPE_HIDDEV) {
    gen_stream_device_t* usbitems;

    usbitems = (gen_stream_device_t*)(sdcd->extra);

    free(usbitems->stream);
    free(sdcd->extra);
    close(sdcd->fd);
#endif
  } else if (sdcd->conntype == SERDISPCONNTYPE_RS232) {
    gen_stream_device_t* streamitems;

    streamitems = (gen_stream_device_t*)(sdcd->extra);

    tcsetattr(sdcd->fd, TCSANOW, &(sdcd->termstate_bkp));
    usleep(1);

    free(streamitems->stream);
    free(sdcd->extra);
    close(sdcd->fd);
  } else {
  }
  free(sdcd);
  sdcd = 0;
}


/* *********************************
   int SDCONN_confinit(sdcd)
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
int SDCONN_confinit(serdisp_CONN_t* sdcd) {
  if (!sdcd->needs_confinit) /* bail out if no extra configuration is needed */
    return 0;

#ifdef HAVE_LIBUSB
  if (sdcd->hardwaretype & SDHWT_USB) {
    return SDCONNusb_confinit(sdcd);
  }
#endif
  if (sdcd->hardwaretype & SDHWT_OUT) {
    return 0;
  }

  /* may only be called once */
  sdcd->needs_confinit = 0;

  if (sdcd->conntype == SERDISPCONNTYPE_RS232) {
    unsigned int baudrate = (sdcd->rs232.baudrate) ? sdcd->rs232.baudrate : B1200;

/* pre-init raw-transfer */
#if defined(__sun__)
    /* solaris doesn't support cfmakeraw() */
    sdcd->termstate.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL|IXON);

    sdcd->termstate.c_oflag &= ~OPOST;
    sdcd->termstate.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
    sdcd->termstate.c_cflag &= ~(CSIZE|PARENB);
    sdcd->termstate.c_cflag |= CS8;
#else
    cfmakeraw( &(sdcd->termstate) );
#endif

#if defined(__sun__)
    /* solaris doesn't support cfsetspeed() */
    cfsetospeed( &(sdcd->termstate), baudrate);  /* set initial baudrate */
#else
    cfsetspeed( &(sdcd->termstate), baudrate);  /* set initial baudrate */
#endif

    /* set amount of data bits */
    sdcd->termstate.c_cflag &= ~CSIZE;  /* bit mask for data bits */
    switch(sdcd->rs232.c_cs8_decr) {
      case 1:  sdcd->termstate.c_cflag |= CS7; break;
      case 2:  sdcd->termstate.c_cflag |= CS6; break;
      case 3:  sdcd->termstate.c_cflag |= CS5; break;
      default: sdcd->termstate.c_cflag |= CS8; break;
    }

    if (sdcd->rs232.c_cstopb)
      sdcd->termstate.c_cflag |= CSTOPB;   /* 2 stop bits */
    else
      sdcd->termstate.c_cflag &= ~CSTOPB;  /* 1 stop bits (default) */

    if (sdcd->rs232.c_parenb)
      sdcd->termstate.c_cflag |= PARENB;   /* enable parent bit */
    else
      sdcd->termstate.c_cflag &= ~PARENB;  /* don't enable parent bit (default) */

    if (sdcd->rs232.c_parodd)
      sdcd->termstate.c_cflag |= PARODD;   /* odd parity instead of even */
    else
      sdcd->termstate.c_cflag &= ~PARODD;  /* even parity (default) */

    if (sdcd->rs232.c_cread)
      sdcd->termstate.c_cflag |= CREAD;    /* enable receiver */
    else
      sdcd->termstate.c_cflag &= ~CREAD;   /* don't enable receiver (default) */

    if (sdcd->rs232.c_local)
      sdcd->termstate.c_cflag |= CLOCAL;   /* enable local line */
    else
      sdcd->termstate.c_cflag &= ~CLOCAL;  /* don't enable local line (default) */

    if (sdcd->rs232.c_rtscts)
      sdcd->termstate.c_cflag |= CRTSCTS;  /* enable hardware flow control */
    else
      sdcd->termstate.c_cflag &= ~CRTSCTS; /* no hardware flow control (default) */

    if (tcsetattr(sdcd->fd, TCSANOW, &(sdcd->termstate)) < 0) {
      sd_error(SERDISP_ENXIO, "setting attributes for device '%s' failed (cause: %s)", sdcd->sdcdev, strerror(errno));
      /*SDCONN_close(sdcd);*/
      sd_runtimeerror = 1;
      return -2;
    }

    tcflush(sdcd->fd, TCIOFLUSH); /* flush buffers */

    usleep(2);
    return 1;
  } else {
    /* should never ever occur, but if it happenes to be: sdcd->need_config is mis-configured in driver! */
    sd_error(SERDISP_ERUNTIME, "%s(): switch-case: extra config./init. not supported by device!", __func__);
  }
  return -1;
}


/* *********************************
   void SDCONN_write(sdcd, ldata, flags)
   *********************************
   write a byte to the serdisp connect device
   *********************************
   sdcd   ... serdisp connect descriptor
   ldata  ... data to be written 
   flags  ... which bytes are to read
*/
void SDCONN_write(serdisp_CONN_t* sdcd, long ldata, byte flags) {
  SDCONN_writedelay(sdcd, ldata, flags, 0);
}




/* *********************************
   void SDCONN_writedelay(sdcd, ldata, flags)
   *********************************
   write a byte to the serdisp connect device than delay ns nanoseconds
   *********************************
   sdcd   ... serdisp connect descriptor
   ldata  ... data to be written 
   flags  ... which bytes are to read
   ns     ... nanoseconds delay after write
*/
void SDCONN_writedelay(serdisp_CONN_t* sdcd, long ldata, byte flags, long ns) {
  if (sd_runtime_error())
    return;

  if (sdcd->needs_confinit)
    SDCONN_confinit(sdcd);

  /* if 'flags' is not set: use io_flags_default set in serdisp descriptor */
  if (!flags)
    flags = sdcd->io_flags_default;


  /* set signals that are defined to be permanently high */
  ldata |= sdcd->signals_permon;

  /* invert signals where active-low/active-high don't match */
  ldata ^= sdcd->signals_invert;

#ifdef HAVE_LIBUSB
  if (sdcd->hardwaretype & SDHWT_USB) {
    return SDCONNusb_writedelay(sdcd, ldata, flags, ns);
  }
#endif /* HAVE_LIBUSB */
  if (sdcd->conntype == SERDISPCONNTYPE_PARPORT) {

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

    if ( ! sdcd->directIO) {
      byte t_data;
#if defined(__sun__)
      struct ecpp_regs  eregs;
#endif /* !defined (__sun__) */

      /* control port */
      if ( SD_PP_WRITECB & flags) {
        /* write bits 0-3 from input data and bits 4-7 from stored control byte */
        t_data = (byte) ((ldata & 0x000F0000) >> 16);
        t_data |= (sdcd->pp_ctrlbits_saved & 0xF0);

        /* store back new control byte */
        sdcd->pp_ctrlbits_saved = t_data;

#if defined(__sun__)
        eregs.dcr = 0xF0 | t_data;   /* bits 4-7 need to be 1 when writing control byte */
        if (ioctl(sdcd->fd, ECPPIOC_SETREGS, &eregs) < 0) {
          sd_error(SERDISP_ERUNTIME, "ioctl(ECPPIOC_SETREGS) failed (cause: %s)", strerror(errno));
          sd_runtimeerror = 1;
          return;
        }
#else
        if (ioctl(sdcd->fd, PPWCONTROL, &t_data) < 0) {
          sd_error(SERDISP_ERUNTIME, "ioctl(PPWCONTROL) failed (cause: %s)", strerror(errno));
          sd_runtimeerror = 1;
          return;
        }
#endif /* defined (__sun__) */

      }

      /* data port */
      if ( SD_PP_WRITEDB & flags ) {
        t_data = (byte) (ldata & 0x000000FF);

        if (ioctl(sdcd->fd, PPWDATA, &t_data) < 0) {
          sd_error(SERDISP_ERUNTIME, "ioctl(PPWDATA) failed (cause: %s)", strerror(errno));
          sd_runtimeerror = 1;
          return;
        }
      }

#ifdef __sd_linux_use_directIO__
    } else {
      byte t_data;

     /* control port */
      if ( SD_PP_WRITECB & flags) {
        t_data = (byte) ((ldata & 0x000F0000) >> 16);
        t_data |= (sdcd->pp_ctrlbits_saved & 0xD0);

        /* store back new control byte */
        sdcd->pp_ctrlbits_saved = t_data;
        OUTB( (t_data), ( (unsigned short int) ((sdcd->port)+2) ));
      }

      /* data port */
      if ( SD_PP_WRITEDB & flags ) {
        t_data = (byte) (ldata & 0x000000FF);
        OUTB( (t_data), ( (unsigned short int) (sdcd->port) ));
      }
#endif /* __sd_linux_use_directIO__ */
    }
  } else if (sdcd->conntype == SERDISPCONNTYPE_SERRAW) {
    if ( ! sdcd->directIO) {
      byte t_data;
      int status;

      ioctl(sdcd->fd, TIOCMGET, &status);

      if (status & (TIOCM_DSR | TIOCM_RI) ) {
        sd_debug(0, "SDCONN_open(): DRS and/or RI occured. trying to reset");
        status &= ~TIOCM_DSR;
        status &= ~TIOCM_RI;

        ioctl(sdcd->fd, TIOCMSET, &status);
        ioctl(sdcd->fd, TIOCMGET, &status);

        if (status & (TIOCM_DSR | TIOCM_RI) ) {
          sd_error(SERDISP_ERUNTIME, "resetting DRS/RI failed");
          sd_runtimeerror = 1;
          return;
        }
      }

      t_data = (byte) (ldata & 0x000000FF);

      if (ioctl(sdcd->fd, TIOCMSET, &t_data) < 0) {
        sd_error(SERDISP_ERUNTIME, "ioctl(TIOCMSET) failed (cause: %s)", strerror(errno));
        sd_runtimeerror = 1;
        return;
      }
#ifdef __sd_linux_use_directIO__
    } else {
      byte t_data;

      t_data = (byte) (ldata & 0x000000FF);

      OUTB( (t_data), ( (unsigned short int) ((sdcd->port)+4) ));
#endif /* __sd_linux_use_directIO__ */
    }
  } else if (sdcd->conntype == SERDISPCONNTYPE_IOW24) {
    byte t_data = 0, t_flags = 0;
    byte oldcs;
    gen_stream_device_t* iow_dev;

    /* control byte */
    /* bits 16-23 from ldata */
    t_flags = (byte) ((ldata & 0x00FF0000) >> 16);


    /* data byte */
    t_data = (byte) (ldata & 0x000000FF);

    iow_dev = (gen_stream_device_t*)(sdcd->extra);

    /* t_flags:  0000 00000    t_data:
                        ^^^--- 0 .. t_data == data byte  1 .. t_data == command byte
                        |`---- 0 .. enable CS1           1 .. enable CS2
     */

    oldcs = (iow_dev->store & 0x20) ? 1: 0;

    if ((t_flags & 0x02)  && (t_data != oldcs) ) {  /* cs-line change */

      SDCONN_commit(sdcd);

      /* iow24: i2c */
      iow_dev->store &= 0xCF;
      iow_dev->store |= ((t_data) ? 0x20 : 0x10);
      IOW_FILLSTREAM(iow_dev->stream, IOW_I2C_WRITE_REPORT, 0xC2, 0x70, (byte)(iow_dev->store) ,0,0,0,0);  
      if(ioctl(sdcd->fd, IOW_WRITE, iow_dev->stream) < 0) {
        sd_error(SERDISP_ERUNTIME, "SDCONN_writedelay(): IOW/I2C CS-line change failed");
      }

      sdtools_nsleep(ns);
    } else  if ((iow_dev->laststatus & 0x1) != (t_flags & 0x01) ) {
      SDCONN_commit(sdcd);
      iow_dev->laststatus = (iow_dev->laststatus & 0xFFFE) | (t_flags & 0x01);
    }

    if (t_flags <= 0x01) {
      iow_dev->stream[2 + iow_dev->streampos++] = t_data; /* byte 0 and 1 are needed for repID and value */
      if (iow_dev->streampos >= 6)
        SDCONN_commit(sdcd);
    }
#if defined(__sd_available_hiddev__)
  } else if (sdcd->conntype == SERDISPCONNTYPE_HIDDEV) {
    gen_stream_device_t* usbitems;

    usbitems = (gen_stream_device_t*)(sdcd->extra);

    byte t_data = 0, t_flags = 0;

    t_flags = (byte) ((ldata & 0x00FF0000) >> 16);
    t_data = (byte) (ldata & 0x000000FF);

    if (t_flags & 0x01) {  /* single byte command -> commit immediatly before and afterwards  */
      if (t_data != 0x02 || !(t_flags & 0x02)) {
        SDCONN_commit(sdcd);
        usbitems->stream[usbitems->streampos++] = t_data;
        SDCONN_commit(sdcd);
      } else {
        int page, chunk;
        /* command 0x02 (clear display) not working as expected -> emulate it */
        SDCONN_commit(sdcd);
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
            SDCONN_commit(sdcd);
          }
        }
      }
    } else {  /* multibyte command: -> commit has to be done by the user */
      if (usbitems->streampos < usbitems->streamsize)
        usbitems->stream[usbitems->streampos++] = t_data;
      else   /* should never happen (if so: -> bug in driver serdisp_specific_l4m.c) */
        sd_error(SERDISP_ERUNTIME, "SDCONN_writedelay(): L4M_E-5i/LCD stream out of bounds (%d >= %d)",
                  usbitems->streampos, usbitems->streamsize);
    }
#endif
  } else if (sdcd->conntype == SERDISPCONNTYPE_RS232) {
    gen_stream_device_t* streamitems;

    streamitems = (gen_stream_device_t*)(sdcd->extra);

    if (streamitems->streampos >= streamitems->streamsize)
      SDCONN_commit(sdcd);

    streamitems->stream[streamitems->streampos++] = (byte) (ldata & 0xFF);
  } else {
    /* unable to get here */
  }
  sdcd->debug_count++;

  if (ns)
    sdtools_nsleep(ns);
}

/* *********************************
   long SDCONN_read(sdcd, flags)
   *********************************
   read a byte from the serdisp connect device
   *********************************
   sdcd   ... serdisp connect descriptor
   flags  ... which bytes are to read
   *********************************
   returns a data read from the serdisp connect device
   =================================================

*/
long SDCONN_read(serdisp_CONN_t* sdcd, byte flags) {
  long ldata = 0;

  if (sd_runtime_error())
    return 0;

  if (sdcd->needs_confinit)
    SDCONN_confinit(sdcd);

#ifdef HAVE_LIBUSB
  if (sdcd->hardwaretype & SDHWT_USB) {
    return SDCONNusb_read(sdcd, flags);
  } else
#endif /* HAVE_LIBUSB */
  if (sdcd->conntype == SERDISPCONNTYPE_PARPORT) {

    if ( ! sdcd->directIO) {
      byte t_data;

      if ( SD_PP_READDB & flags ) {
#if defined(__linux__)
        int direction = 1; /* set bidi-parport to read data*/
        if (ioctl(sdcd->fd, PPDATADIR, &direction) < 0) {
          sd_error(SERDISP_ERUNTIME, "ioctl(PPDATADIR) failed (cause: %s)", strerror(errno));
          sd_runtimeerror = 1;
          return 0;
        }
#endif /* defined(__linux__) */

        if (ioctl(sdcd->fd, PPRDATA, &t_data) < 0) {  /* read data-byte */
          sd_error(SERDISP_ERUNTIME, "ioctl(PPRDATA) failed (cause: %s)", strerror(errno));
          sd_runtimeerror = 1;
          return 0;
        }
        ldata |= (long) (t_data);

#if defined(__linux__)
        direction = 0; /* re-set bidi-parport to write data */
        if (ioctl(sdcd->fd, PPDATADIR, &direction) < 0) {
          sd_error(SERDISP_ERUNTIME, "ioctl(PPDATADIR) failed (cause: %s)", strerror(errno));
          sd_runtimeerror = 1;
          return 0;
        }
#endif /* defined(__linux__) */
      }
      if ( SD_PP_READCB & flags ) {
#if defined(__sun__)
        struct ecpp_regs  eregs;

        eregs.dcr = t_data;
        if (ioctl(sdcd->fd, ECPPIOC_GETREGS, &eregs) < 0) {
          sd_error(SERDISP_ERUNTIME, "ioctl(ECPPIOC_GETREGS) failed (cause: %s)", strerror(errno));
          sd_runtimeerror = 1;
          return 0;
        }
#else
        if (ioctl(sdcd->fd, PPRCONTROL, &t_data) < 0) {
          sd_error(SERDISP_ERUNTIME, "ioctl(PPRCONTROL) failed (cause: %s)", strerror(errno));
          sd_runtimeerror = 1;
          return 0;
        }
#endif /* defined(__sun__) */

        /* store control byte (bit 5 can NOT be read! so restore it from old value saved */
        sdcd->pp_ctrlbits_saved = (t_data & 0xDF) | (sdcd->pp_ctrlbits_saved & 0x20);
        ldata |= ((long)sdcd->pp_ctrlbits_saved) << 16;
      }
#ifdef __sd_linux_use_directIO__
    } else {
      byte t_data;

      if ( SD_PP_READDB & flags ) {
        /* set bidi-parport to read data*/
        sdcd->pp_ctrlbits_saved |= SD_PP_BIDI;
        t_data = sdcd->pp_ctrlbits_saved;
        OUTB( (t_data), ( (unsigned short int) ((sdcd->port)+2) ));

        /* read data-byte */
        t_data = INB( ( (unsigned short int) (sdcd->port) ));
        ldata |= (long) (t_data);

        /* re-set bidi-parport to write data */
        sdcd->pp_ctrlbits_saved &= (0xFF ^ SD_PP_BIDI);
        t_data = sdcd->pp_ctrlbits_saved;
        OUTB( (t_data), ( (unsigned short int) ((sdcd->port)+2) ));
      }
      if ( SD_PP_READCB & flags ) {
        t_data = INB( ( (unsigned short int) ((sdcd->port)+2) ) );
        /* store control byte (bit 5 can NOT be read! so restore it from old value saved */
        sdcd->pp_ctrlbits_saved = (t_data & 0xDF) | (sdcd->pp_ctrlbits_saved & 0x20);
        ldata |= ((long)t_data) << 16;
      }
#endif /* __sd_linux_use_directIO__ */

    }
    /* invert signals if desired by user / wiring / ... */
    ldata ^= sdcd->signals_invert;
    return ldata;
  } else if (sdcd->conntype == SERDISPCONNTYPE_SERRAW) {
    if ( ! sdcd->directIO) {
      byte t_data;

      if (ioctl(sdcd->fd, TIOCMGET, &t_data) < 0) {
        sd_error(SERDISP_ERUNTIME, "ioctl(TIOCMGET) failed (cause: %s)", strerror(errno));
        sd_runtimeerror = 1;
        return 0;
      }
      return (long)(t_data);

#ifdef __sd_linux_use_directIO__
    } else {
      byte t_data;

      t_data = INB( ( (unsigned short int) ((sdcd->port)+2) ) );
      return (long)(t_data);
#endif /* __sd_linux_use_directIO__ */
    }
  } else if (sdcd->conntype == SERDISPCONNTYPE_RS232) {
    char buf[1];
    int rc;

    rc = read(sdcd->fd, buf, 1);

    return (rc > 0) ? (long)buf[0] : -1L;
  } else {
    /* unable to get here */
  }

  return 0;
}



/* *********************************
   int SDCONN_readstream(sdcd, buf, count)
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
int SDCONN_readstream(serdisp_CONN_t* sdcd, byte* buf, int count) {
  int rc = 0;

  if (sd_runtime_error())
    return 0;

  if (sdcd->needs_confinit)
    SDCONN_confinit(sdcd);

#ifdef HAVE_LIBUSB
  if (sdcd->hardwaretype & SDHWT_USB) {
    return SDCONNusb_readstream(sdcd, buf, count);
  } else
#endif /* HAVE_LIBUSB */

  rc = read(sdcd->fd, buf, count);

  if( rc < 0 ) {
    if ( errno!=EAGAIN ) {
      sd_error(SERDISP_ERUNTIME, "%s(): could not read from device: %s (%d)", __func__, strerror(errno), errno);
    } else {
      usleep(100);
    }
  }

  return rc;
}



/* *********************************
   void SDCONN_commit(sdcd)
   *********************************
   commits an usb fast write stream (in non-usb modes and non-streaming usb modes: no effect)
   *********************************
   sdcd   ... serdisp connect descriptor
*/
void SDCONN_commit(serdisp_CONN_t* sdcd) {
  if (sdcd->needs_confinit)
    SDCONN_confinit(sdcd);

#ifdef HAVE_LIBUSB
  if (sdcd->hardwaretype & SDHWT_USB) {
    SDCONNusb_commit(sdcd);
  } else
#endif /* HAVE_LIBUSB */
  if (sdcd->conntype == SERDISPCONNTYPE_IOW24) {
    gen_stream_device_t* iow_dev;

    iow_dev = (gen_stream_device_t*)(sdcd->extra);
    iow_dev->stream[0] = IOW_LCD_WRITE_REPORT;  /* iow24: lcd reportID */
    iow_dev->stream[1] = iow_dev->streampos + ((iow_dev->laststatus & 0x01) ? 0 : 0x80);

    if(ioctl(sdcd->fd, IOW_WRITE, iow_dev->stream) < 0) {
      sd_error(SERDISP_ERUNTIME, "SDCONN_commit(): IOW/LCD write failed");
      sd_runtimeerror = 1;
    }
    iow_dev->streampos = 0;    
#if defined(__sd_available_hiddev__)
  } else if (sdcd->conntype == SERDISPCONNTYPE_HIDDEV) {
    struct hiddev_report_info rinfo;
    struct hiddev_usage_ref uref;
    int i;

    gen_stream_device_t* usbitems;

    usbitems = (gen_stream_device_t*)(sdcd->extra);

    if (usbitems->streampos == 0)
      return;

    if (ioctl(sdcd->fd, HIDIOCINITREPORT,0) < 0) {
      sd_error(SERDISP_ERUNTIME, "SDCONN_commit(): sending HIDIOCINITREPORT report failed");
    }

    rinfo.report_type = HID_REPORT_TYPE_OUTPUT;
    rinfo.report_id = HID_REPORT_ID_FIRST;
    rinfo.num_fields = 1;
    if(ioctl(sdcd->fd, HIDIOCGREPORTINFO, &rinfo) < 0) {
      sd_error(SERDISP_ERUNTIME, "SDCONN_commit(): sending HIDIOCGREPORTINFO report failed");
    }

    for (i = 0; i < usbitems->streampos; i++) {
      uref.report_type = rinfo.report_type;
      uref.report_id   = 0;
      uref.field_index = 0;
      uref.usage_index = i;
      /*uref.usage_code = 0xFF000000+i+1;*/
      uref.value = usbitems->stream[i];
      ioctl(sdcd->fd, HIDIOCGUCODE, &uref);
      ioctl(sdcd->fd, HIDIOCSUSAGE, &uref);
    }

    rinfo.num_fields = 1;
    if(ioctl(sdcd->fd, HIDIOCSREPORT, &rinfo) < 0) {
      sd_error(SERDISP_ERUNTIME, "SDCONN_commit(): sending HIDIOCSREPORT report failed (command: 0x%02x)", usbitems->stream[0]);
    }
    usbitems->streampos = 0;
#endif
  } else if (sdcd->conntype == SERDISPCONNTYPE_RS232) {
    gen_stream_device_t* streamitems;

    streamitems = (gen_stream_device_t*)(sdcd->extra);

    if (streamitems->streampos == 0)
      return;

    write(sdcd->fd, streamitems->stream, streamitems->streampos);

    streamitems->streampos = 0;
  }
}



/* *********************************
   void SDCONN_usleep(sdcd, usec)
   *********************************
   delays usec microseconds
   *********************************
   sdcd  ... serdisp connect descriptor
   usec  ... delay timye
*/
void SDCONN_usleep(serdisp_CONN_t* sdcd, unsigned long usec) {
#ifdef HAVE_LIBUSB
  if (sdcd->hardwaretype & SDHWT_USB) {
    SDCONNusb_usleep(sdcd, usec);
  } else
#endif /* HAVE_LIBUSB */
    usleep(usec);
}



/* *********************************
   serdisp_CONN_t* SDCONN_import_PP (directIO, hport)
   *********************************
   import an existing already opened parport device / port and create
   a sdcd struct out of it
   *********************************
   directIO   ... 1: yes -> outp-calls, 0: no -> ioctl-calls
   hport      ... if directIO: port (eg: 0x378), else: descriptor for parport dev
   *********************************
   returns serdisp connect descriptor or (serdisp_CONN_t*)0 if operation was unsuccessful

   USE WITH CARE!!! all permissions and stuff like that must be ok before!!
   no checking for validity in here
*/
serdisp_CONN_t* SDCONN_import_PP(int directIO, int hport) {
  serdisp_CONN_t* sdcd = 0;

  /* directIO only with supported systems / architectures */
#ifndef __sd_linux_use_directIO__
  if (directIO)
    return (serdisp_CONN_t*)0;
#endif /* __sd_linux_use_directIO__ */

  if (! (sdcd = (serdisp_CONN_t*)sdtools_malloc(sizeof(serdisp_CONN_t)) ) ) {
    return (serdisp_CONN_t*)0;
  }
  memset(sdcd, 0, sizeof(serdisp_CONN_t));

  sdcd->directIO = directIO;

  if (! sdcd->directIO) {
    sdcd->port = (unsigned short int) hport;
#ifdef __sd_linux_use_directIO__
  } else {
    sdcd->fd = hport;
#endif /* __sd_linux_use_directIO__ */
  }
  return sdcd;
}



/* internal use only */


int SDCONN_getsignalindex(const char str[], short conntype, short hardwaretype) {
  int i;  
  int n = strlen(str);
  char* idxpos = index(str, ',');

  if (idxpos)
    n = serdisp_ptrstrlen(idxpos, str);

  /* ',' or ';' may end the signal name */
  if (!idxpos) {
    idxpos = index(str, ';');
    if (idxpos)
      n = serdisp_ptrstrlen(idxpos, str);
  }

  i = 0;
  while (i < sizeof(serdisp_signalnames) / sizeof(serdisp_signalnames_t) ) {
    if ((serdisp_signalnames[i].conntype == conntype) &&
        (serdisp_signalnames[i].hardwaretype & hardwaretype) &&
        ( sdtools_ismatching(str, n, serdisp_signalnames[i].name, -1) ||
          sdtools_isinelemlist(serdisp_signalnames[i].aliasnames, str, n) > -1
        )
       ) {
      return i;
    }
    i++;
  }
  return -1;
}



long SDCONN_getsignalvalue(int idx) {
  if (idx < 0 || idx > (sizeof(serdisp_signalnames) / sizeof(serdisp_signalnames_t)))
    return 0;

  return serdisp_signalnames[idx].value;
}


int SDCONN_issignalacticelow(int idx) {
  if (idx < 0 || idx > (sizeof(serdisp_signalnames) / sizeof(serdisp_signalnames_t)))
    return 0;

  return serdisp_signalnames[idx].activelow;
}


char* SDCONN_getsignalname(int idx) {
  if (idx < 0 || idx > (sizeof(serdisp_signalnames) / sizeof(serdisp_signalnames_t)))
    return 0;

  return serdisp_signalnames[idx].name;
}


int SDCONN_isactivelow(long signal, short conntype, short hardwaretype) {
  int i;  

  i = 0;
  while (i < sizeof(serdisp_signalnames) / sizeof(serdisp_signalnames_t) ) {
    if ((serdisp_signalnames[i].conntype == conntype) &&
        (serdisp_signalnames[i].hardwaretype & hardwaretype) &&
        (serdisp_signalnames[i].value == signal)
       ) {
      return serdisp_signalnames[i].activelow;
    }
    i++;
  }
  return 0;
}
