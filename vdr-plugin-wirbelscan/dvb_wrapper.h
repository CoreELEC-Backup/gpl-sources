/*
 * dvb_wrapper.h: wirbelscan - A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 */
#ifndef __WIRBELSCAN_DVB_WRAPPER_H_
#define __WIRBELSCAN_DVB_WRAPPER_H_

#include <string>
#include <vdr/dvbdevice.h>
#include "extended_frontend.h"

void PrintDvbApi(std::string& s);

// DVB frontend capabilities
unsigned int GetFrontendStatus(cDevice* dev);

bool GetTerrCapabilities (cDevice* dev, bool* CodeRate, bool* Modulation, bool* Inversion, bool* Bandwidth, bool* Hierarchy, bool* TransmissionMode, bool* GuardInterval, bool* DvbT2);
bool GetCableCapabilities(cDevice* dev, bool* Modulation, bool* Inversion);
bool GetAtscCapabilities (cDevice* dev, bool* Modulation, bool* Inversion, bool* VSB, bool* QAM);
bool GetSatCapabilities  (cDevice* dev, bool* CodeRate, bool* Modulation, bool* RollOff, bool* DvbS2); //DvbS2: true if supported.

#endif
