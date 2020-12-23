/*                                                                  -*- c++ -*-
Copyright (C) 2004-2013 Christian Wieninger

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
Or, point your browser to http://www.gnu.org/licenses/old-licenses/gpl-2.0.html

The author can be reached at cwieninger@gmx.de

The project's page is at http://winni.vdr-developer.org/epgsearch
*/

#ifdef __FreeBSD__
#include <stdint.h>
#endif
#include "menu_deftimercheckmethod.h"
#include "menu_myedittimer.h"

const char *cMenuDefTimerCheckMethod::CheckModes[3];

cDefTimerCheckModes DefTimerCheckModes;

// -- cDefTimerCheckMode -----------------------------------------------------------------
bool cDefTimerCheckMode::Parse(const char *s)
{
    char *line;
    char *pos;
    char *pos_next;
    int parameter = 1;
    int valuelen;

#define MAXVALUELEN (10 * MaxFileName)

    char value[MAXVALUELEN];

    pos = line = strdup(s);
    pos_next = pos + strlen(pos);
    if (*pos_next == '\n') *pos_next = 0;
    while (*pos) {
        while (*pos == ' ') pos++;
        if (*pos) {
            if (*pos != '|') {
                pos_next = strchr(pos, '|');
                if (!pos_next)
                    pos_next = pos + strlen(pos);
                valuelen = pos_next - pos + 1;
                if (valuelen > MAXVALUELEN) {
                    LogFile.eSysLog("entry '%s' is too long. Will be truncated!", pos);
                    valuelen = MAXVALUELEN;
                }
                strn0cpy(value, pos, valuelen);
                pos = pos_next;
                switch (parameter) {
                case 1:
                    channelID = tChannelID::FromString(value);
                    break;
                case 2:
                    mode = atol(value);
                    break;
                default:
                    break;
                } //switch
            }
            parameter++;
        }
        if (*pos) pos++;
    } //while

    free(line);
    return (parameter >= 2) ? true : false;
}

cString cDefTimerCheckMode::ToText(void) const
{
    cString buffer = cString::sprintf("%s|%d", *channelID.ToString(), mode);
    return buffer;
}

bool cDefTimerCheckMode::Save(FILE *f)
{
    if (mode == 0) return true; // don't save the default
    return fprintf(f, "%s\n", *ToText()) > 0;
}

int cDefTimerCheckModes::GetMode(const cChannel* channel)
{
    if (!channel) return 0;
    tChannelID ChannelID = channel->GetChannelID();
    for (cDefTimerCheckMode *defMode = First(); defMode; defMode = Next(defMode))
        if (defMode->channelID == ChannelID)
            return defMode->mode;
    return 0;
}

void cDefTimerCheckModes::SetMode(const cChannel* channel, int mode)
{
    if (!channel) return;
    tChannelID ChannelID = channel->GetChannelID();
    for (cDefTimerCheckMode *defMode = First(); defMode; defMode = Next(defMode))
        if (defMode->channelID == ChannelID) {
            defMode->mode = mode;
            return;
        }
    Add(new cDefTimerCheckMode(ChannelID, mode));
}

// --- cMenuDefTimerCheckMethod ---------------------------------------------------------

cMenuDefTimerCheckMethod::cMenuDefTimerCheckMethod()
    : cOsdMenu(tr("Default timer check method"), 20)
{
    SetMenuCategory(mcSetupPlugins);
    CheckModes[0] = tr("no check");
    CheckModes[UPD_CHDUR] = tr("by channel and time");
    CheckModes[UPD_EVENTID] = tr("by event ID");

    modes = NULL;
    Set();
}

cMenuDefTimerCheckMethod::~cMenuDefTimerCheckMethod()
{
    delete [] modes;
}

void cMenuDefTimerCheckMethod::Set()
{
    int current = Current();
    Clear();

    delete modes;
    LOCK_CHANNELS_READ;
    modes = new int[Channels->Count()];
    int i = 0;
    for (const cChannel *channel = Channels->First(); channel; channel = Channels->Next(channel), i++) {
        if (!channel->GroupSep() && *channel->Name()) {
            modes[i] = DefTimerCheckModes.GetMode(channel);
            Add(new cMenuEditStraItem(channel->Name(), &modes[i], 3, CheckModes));
        }
    }
    SetCurrent(Get(current));
}

eOSState cMenuDefTimerCheckMethod::ProcessKey(eKeys Key)
{
    eOSState state = cOsdMenu::ProcessKey(Key);

    if (state == osUnknown) {
        switch (Key) {
        case kOk: {
            int i = 0;
            LOCK_CHANNELS_READ;
            for (const cChannel *channel = Channels->First(); channel; channel = Channels->Next(channel), i++)
                if (!channel->GroupSep() && *channel->Name())
                    DefTimerCheckModes.SetMode(channel, modes[i]);
            DefTimerCheckModes.Save();
            return osBack;
        }
        default:
            break;
        }
    }
    return state;
}





