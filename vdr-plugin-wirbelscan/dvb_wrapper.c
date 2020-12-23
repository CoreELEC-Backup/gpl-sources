/*
 * dvb_wrapper.c: wirbelscan - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */

#include <linux/dvb/frontend.h>
#include <linux/dvb/version.h>
#include <vdr/dvbdevice.h>
#include "common.h"
#include "dvb_wrapper.h"


void PrintDvbApi(std::string& s) {
  s = "compiled for DVB API ";
  char c[7]; // 2 + 1 + 3 + 1
  snprintf(c, 7, "%d.%d", DVB_API_VERSION, DVB_API_VERSION_MINOR);
  s += c;
}

unsigned int GetFrontendStatus(cDevice * dev) {
  cDvbDevice* dvbdevice = (cDvbDevice*) dev;
  fe_status_t value;
  char devstr[256];
  snprintf(devstr, 256, "/dev/dvb/adapter%d/frontend%d",
                       dvbdevice->Adapter(),
                       dvbdevice->Frontend());

  int fe = open(devstr, O_RDONLY | O_NONBLOCK);
  if (fe < 0) {
     dlog(0, "%s: could not open %s", __FUNCTION__, *devstr);
     return 0;
     }
  if (IOCTL(fe, FE_READ_STATUS, &value) < 0) {
     close(fe);
     dlog(0, "%s: could not read %s", __FUNCTION__, *devstr);
     return 0;
     }
  close(fe);
  return value;
}


unsigned int GetCapabilities(cDevice * dev) {
  cDvbDevice* dvbdevice = (cDvbDevice*) dev;
  struct dvb_frontend_info fe_info;
  char devstr[256];
  snprintf(devstr, 256, "/dev/dvb/adapter%d/frontend%d",
                        dvbdevice->Adapter(),
                        dvbdevice->Frontend());

  int fe = open(devstr, O_RDONLY | O_NONBLOCK);
  if (fe < 0)
     return 0;

  if (IOCTL(fe, FE_GET_INFO, &fe_info) < 0) {
     close(fe);
     dlog(0, "%s: could not read %s", __FUNCTION__, *devstr);
     return 0;
     }
  close(fe);
  return fe_info.caps;
}


bool GetTerrCapabilities(cDevice* dev, bool* CodeRate, bool* Modulation, bool* Inversion, bool* Bandwidth, bool* Hierarchy,
                          bool* TransmissionMode, bool* GuardInterval, bool* DvbT2) {
  unsigned int cap = GetCapabilities(dev);
  if (cap == 0)
     return false;
  *CodeRate         = cap & FE_CAN_FEC_AUTO;
  *Modulation       = cap & FE_CAN_QAM_AUTO;
  *Inversion        = cap & FE_CAN_INVERSION_AUTO; 
  *Bandwidth        = cap & FE_CAN_BANDWIDTH_AUTO;
  *Hierarchy        = cap & FE_CAN_HIERARCHY_AUTO;
  *TransmissionMode = cap & FE_CAN_GUARD_INTERVAL_AUTO;
  *GuardInterval    = cap & FE_CAN_TRANSMISSION_MODE_AUTO;
  *DvbT2            = cap & FE_CAN_2G_MODULATION;
  return true; 
}


bool GetCableCapabilities(cDevice* dev, bool* Modulation, bool* Inversion) {
  int cap = GetCapabilities(dev);
  if (cap < 0)
     return false;

  *Modulation       = cap & FE_CAN_QAM_AUTO;
  *Inversion        = cap & FE_CAN_INVERSION_AUTO;
  return true; 
}


bool GetAtscCapabilities(cDevice* dev, bool* Modulation, bool* Inversion, bool* VSB, bool* QAM) {
  int cap = GetCapabilities(dev);
  if (cap < 0)
     return false;

  *Modulation       = cap & FE_CAN_QAM_AUTO;
  *Inversion        = cap & FE_CAN_INVERSION_AUTO;
  *VSB              = cap & FE_CAN_8VSB;
  *QAM              = cap & FE_CAN_QAM_256;
  return true; 
}


bool GetSatCapabilities(cDevice * dev, bool *CodeRate, bool *Modulation, bool *RollOff, bool *DvbS2) {
  int cap = GetCapabilities(dev);
  if (cap < 0)
     return false;
  *CodeRate         = cap & FE_CAN_FEC_AUTO;
  *Modulation       = cap & FE_CAN_QAM_AUTO;
  *RollOff          = cap & 0 /* there is no capability flag foreseen for rolloff auto? */ ;
  *DvbS2            = cap & FE_CAN_2G_MODULATION;
  return true; 
}
