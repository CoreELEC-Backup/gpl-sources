/*
 *  vdr-plugin-dvbapi - softcam dvbapi plugin for VDR
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef ___SCCIADAPTER_H
#define ___SCCIADAPTER_H

#include "SCCAMSlot.h"
#include "Frame.h"
#include "CAPMT.h"

class SCCAMSlot;

#define TDPU_SIZE_INDICATOR 0x80

struct TPDU
{
  unsigned char slot;
  unsigned char tcid;
  unsigned char tag;
  unsigned char len;
  unsigned char data[1];
};

class SCCIAdapter : public cCiAdapter
{
private:
  cMutex ciMutex;
  SCCAMSlot *slots[MAXDEVICES];
  int version;
  int tcid;
  cTimeMs readTimer;
  Frame frame;
  cRingBufferLinear *rb;

public:
  SCCIAdapter(void);
  ~SCCIAdapter();
  int GetVersion()
  {
    return version;
  }
  virtual int Read(unsigned char *Buffer, int MaxLength);
  virtual void Write(const unsigned char *Buffer, int Length);
  virtual bool Reset(int Slot);
  virtual eModuleStatus ModuleStatus(int Slot);
  virtual bool Assign(cDevice *Device, bool Query = false);
};

#endif // ___SCCIADAPTER_H
