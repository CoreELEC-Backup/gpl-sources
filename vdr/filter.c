/*
 * filter.c: Section filter
 *
 * See the main source file 'vdr.c' for copyright information and
 * how to reach the author.
 *
 * $Id: filter.c 4.3 2017/05/09 08:37:23 kls Exp $
 */

#include "filter.h"
#include "sections.h"

// --- cSectionSyncer --------------------------------------------------------

cSectionSyncer::cSectionSyncer(void)
{
  Reset();
}

void cSectionSyncer::Reset(void)
{
  currentVersion = -1;
  currentSection = -1;
  synced = false;
  complete = false;
  memset(sections, 0x00, sizeof(sections));
}

void cSectionSyncer::Repeat(void)
{
  SetSectionFlag(currentSection, false);
  synced = false;
  complete = false;
}

bool cSectionSyncer::Sync(uchar Version, int Number, int LastNumber)
{
  if (Version != currentVersion) {
     Reset();
     currentVersion = Version;
     }
  if (!synced) {
     if (Number != 0)
        return false;
     else
        synced = true;
     }
  currentSection = Number;
  bool Result = !GetSectionFlag(Number);
  SetSectionFlag(Number, true);
  if (Number == LastNumber)
     complete = true;
  return Result;
}

// --- cFilterData -----------------------------------------------------------

cFilterData::cFilterData(void)
{
  pid = 0;
  tid = 0;
  mask = 0;
  sticky = false;
}

cFilterData::cFilterData(u_short Pid, u_char Tid, u_char Mask, bool Sticky)
{
  pid = Pid;
  tid = Tid;
  mask = Mask;
  sticky = Sticky;
}

cFilterData& cFilterData::operator= (const cFilterData &FilterData)
{
  pid = FilterData.pid;
  tid = FilterData.tid;
  mask = FilterData.mask;
  sticky = FilterData.sticky;
  return *this;
}

bool cFilterData::Is(u_short Pid, u_char Tid, u_char Mask)
{
  return pid == Pid && tid == Tid && mask == Mask;
}

bool cFilterData::Matches(u_short Pid, u_char Tid)
{
  return pid == Pid && tid == (Tid & mask);
}

// --- cFilter ---------------------------------------------------------------

cFilter::cFilter(void)
{
  sectionHandler = NULL;
  on = false;
}

cFilter::cFilter(u_short Pid, u_char Tid, u_char Mask)
{
  sectionHandler = NULL;
  on = false;
  Set(Pid, Tid, Mask);
}

cFilter::~cFilter()
{
  if (sectionHandler)
     sectionHandler->Detach(this);
}

int cFilter::Source(void)
{
  return sectionHandler ? sectionHandler->Source() : 0;
}

int cFilter::Transponder(void)
{
  return sectionHandler ? sectionHandler->Transponder() : 0;
}

const cChannel *cFilter::Channel(void)
{
  return sectionHandler ? sectionHandler->Channel() : NULL;
}

void cFilter::SetStatus(bool On)
{
  if (sectionHandler && on != On) {
     cFilterData *fd = data.First();
     while (fd) {
           if (On)
              sectionHandler->Add(fd);
           else {
              sectionHandler->Del(fd);
              if (!fd->sticky) {
                 cFilterData *next = data.Next(fd);
                 data.Del(fd);
                 fd = next;
                 continue;
                 }
              }
           fd = data.Next(fd);
           }
     on = On;
     }
}

bool cFilter::Matches(u_short Pid, u_char Tid)
{
  if (on) {
     for (cFilterData *fd = data.First(); fd; fd = data.Next(fd)) {
         if (fd->Matches(Pid, Tid))
            return true;
         }
     }
  return false;
}

void cFilter::Set(u_short Pid, u_char Tid, u_char Mask)
{
  Add(Pid, Tid, Mask, true);
}

void cFilter::Add(u_short Pid, u_char Tid, u_char Mask, bool Sticky)
{
  cFilterData *fd = new cFilterData(Pid, Tid, Mask, Sticky);
  data.Add(fd);
  if (sectionHandler && on)
     sectionHandler->Add(fd);
}

void cFilter::Del(u_short Pid, u_char Tid, u_char Mask)
{
  for (cFilterData *fd = data.First(); fd; fd = data.Next(fd)) {
      if (fd->Is(Pid, Tid, Mask)) {
         if (sectionHandler && on)
            sectionHandler->Del(fd);
         data.Del(fd);
         return;
         }
      }
}
