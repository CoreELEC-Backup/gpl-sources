/////////////////////////////////////////////////////////////////////////////
//
// @file ddci.h @brief Digital Devices Common Interface plugin for VDR.
//
// Copyright (c) 2013 - 2014 by Jasmin Jessich.  All Rights Reserved.
//
// Contributor(s):
//
// License: GPLv2
//
// This file is part of vdr_plugin_ddci2.
//
// vdr_plugin_ddci2 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// vdr_plugin_ddci2 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with vdr_plugin_ddci2.  If not, see <http://www.gnu.org/licenses/>.
//
/////////////////////////////////////////////////////////////////////////////


#ifndef __DDCI2_H
#define __DDCI2_H

#include <vdr/config.h>
#include <vdr/remux.h>   // TS_SIZE

#if (VDRVERSNUM >= 20303)
# define DDCI_MTD         1    // MTD enabled
# define DDCI_RB_CLR_MTX  0    /* cRingBufferLinear::Clear is thread save, when
                                * executed from reader thread */
#else
# define DDCI_MTD         0  // no MTD in older versions
# define DDCI_RB_CLR_MTX  1  /* cRingBufferLinear::Clear is not thread save and
                              * we need a mutex */
#endif

#if (VDRVERSNUM >= 20304)
# define DDCI_DECRYPT_MORE  1  /* Decrypt as much as possible at once */
#else
# define DDCI_DECRYPT_MORE  0  // older VDR can't handle more packets
#endif

#if DDCI_RB_CLR_MTX

#define DDCI_RB_CLR_MTX_DECL(_m)  cMutex _m;
#define DDCI_RB_CLR_MTX_LOCK(_m)  cMutexLock MutexLock( _m );

#else

#define DDCI_RB_CLR_MTX_DECL(_m)
#define DDCI_RB_CLR_MTX_LOCK(_m)

#endif // DDCI_RB_CLR_MTX

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

static const int BUF_MARGIN = TS_SIZE;

// global config options
extern int cfgIgnAct;
extern int cfgClrSct;
extern int cfgBufSz;
extern int cfgSleepTmo;

inline bool CfgIgnAct()
{
	return !!cfgIgnAct;   // !! .. convert integer to bool
}

inline bool CfgIsClrSct()
{
	return cfgClrSct != 0;
}

inline int CfgGetBufSz()
{
	return cfgBufSz;
}

inline int CfgGetSleepTmo()
{
	return cfgSleepTmo;
}

inline int CalcRbBufSz()
{
	// cRingBufferLinear requires one margin and 1 byte for internal reasons
	return (BUF_MARGIN * (CfgGetBufSz() + 1)) + 1;
}

#endif // __DDCI2_H
