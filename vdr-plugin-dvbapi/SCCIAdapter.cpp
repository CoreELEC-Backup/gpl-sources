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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <dlfcn.h>

#include <linux/dvb/ca.h>
#include <vdr/ci.h>
#include <vdr/thread.h>
#include "SCCIAdapter.h"
#include "Log.h"

// from vdr's ci.c
#define T_CREATE_TC    0x82
#define T_RCV          0x81
#define T_DATA_LAST    0xA0

SCCIAdapter::SCCIAdapter(void)
{
  tcid = 0;

  version = 1;
  memset(slots, 0, sizeof(slots));

  rb = new cRingBufferLinear(KILOBYTE(8), 6 + LEN_OFF, false, "SC-CI adapter read");
  if (rb)
  {
    rb->SetTimeouts(0, CAM_READ_TIMEOUT);
    frame.SetRb(rb);
  }
  SetDescription("SC-CI adapter");
  int SlotIndex = 0;
  for (int i = 0; i < cDevice::NumDevices(); i++)
  {
    if (const cDevice *Device = cDevice::GetDevice(i))
    {
      if (Device->NumProvidedSystems())
      {
        INFOLOG("Creating SCCAMSlot for device %d", Device->CardIndex() + 1);
        slots[SlotIndex] = new SCCAMSlot(this, Device->CardIndex(), SlotIndex, SlotIndex ? slots[0] : NULL);
        SlotIndex++;
      }
    }
  }
  Start();
}

int SCCIAdapter::Read(unsigned char *Buffer, int MaxLength)
{
  cMutexLock lock(&ciMutex);
  if (rb && Buffer && MaxLength > 0)
  {
    int s;
    unsigned char *data = frame.Get(s);
    if (data)
    {
      if (s <= MaxLength)
        memcpy(Buffer, data, s);
      else
        ERRORLOG("internal: sc-ci rb frame size exceeded %d", s);
      frame.Del();
      if (Buffer[2] != 0x80)
        readTimer.Set();
      return s;
    }
  }
  else
    cCondWait::SleepMs(CAM_READ_TIMEOUT);
  if (readTimer.Elapsed() > 2000)
    readTimer.Set();
  return 0;
}

#define TPDU(data,slot)   do { unsigned char *_d=(data); _d[0]=(slot); _d[1]=tcid; } while(0)
#define TAG(data,tag,len) do { unsigned char *_d=(data); _d[0]=(tag); _d[1]=(len); } while(0)
#define SB_TAG(data,sb)   do { unsigned char *_d=(data); _d[0]=0x80; _d[1]=0x02; _d[2]=tcid; _d[3]=(sb); } while(0)

void SCCIAdapter::Write(const unsigned char *buff, int len)
{
  cMutexLock lock(&ciMutex);
  if (buff && len >= 5)
  {
    struct TPDU *tpdu = (struct TPDU *) buff;
    int slot = tpdu->slot;
    if (slots[slot])
    {
      Frame *slotframe = slots[slot]->getFrame();
      switch (tpdu->tag)
      {
      case T_RCV:
        {
          int s;
          unsigned char *d = slotframe->Get(s);
          if (d)
          {
            unsigned char *b;
            if ((b = frame.GetBuff(s + 6)))
            {
              TPDU(b, slot);
              memcpy(b + 2, d, s);
              slotframe->Del(); // delete from rb before Avail()
              SB_TAG(b + 2 + s, slotframe->Avail() > 0 ? 0x80 : 0x00);
              frame.Put();
            }
            else
              slotframe->Del();
          }
          break;
        }
      case T_CREATE_TC:
        {
          tcid = tpdu->data[0];
          unsigned char *b;
          static const unsigned char reqCAS[] = { 0xA0, 0x07, 0x01, 0x91, 0x04, 0x00, 0x03, 0x00, 0x41 };
          if ((b = slotframe->GetBuff(sizeof(reqCAS))))
          {
            memcpy(b, reqCAS, sizeof(reqCAS));
            b[2] = tcid;
            slotframe->Put();
          }
          if ((b = frame.GetBuff(9)))
          {
            TPDU(b, slot);
            TAG(&b[2], 0x83, 0x01);
            b[4] = tcid;
            SB_TAG(&b[5], slotframe->Avail() > 0 ? 0x80 : 0x00);
            frame.Put();
          }
          break;
        }
      case T_DATA_LAST:
        {
          slots[slot]->Process(buff, len);
          unsigned char *b;
          if ((b = frame.GetBuff(6)))
          {
            TPDU(b, slot);
            SB_TAG(&b[2], slotframe->Avail() > 0 ? 0x80 : 0x00);
            frame.Put();
          }
          break;
        }
      }
    }
  }
  else
    DEBUGLOG("short write (buff=%d len=%d)", buff != 0, len);
}

SCCIAdapter::~SCCIAdapter()
{
  DEBUGLOG("%s", __FUNCTION__);

  Cancel(3);

  ciMutex.Lock();
  delete rb;
  rb = 0;
  ciMutex.Unlock();
}

bool SCCIAdapter::Reset(int Slot)
{
  cMutexLock lock(&ciMutex);
  return true;
  //return slots[Slot]->Reset();
}

eModuleStatus SCCIAdapter::ModuleStatus(int Slot)
{
  cMutexLock lock(&ciMutex);
  return (slots[Slot]) ? slots[Slot]->Status() : msNone;
}

bool SCCIAdapter::Assign(cDevice *Device, bool Query)
{
  return true;
}
