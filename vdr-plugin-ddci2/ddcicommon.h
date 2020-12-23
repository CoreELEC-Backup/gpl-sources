/////////////////////////////////////////////////////////////////////////////
//
// @file ddcicommon.h @brief Digital Devices Common Interface plugin for VDR.
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

#ifndef __DDCICOMMON_H
#define __DDCICOMMON_H

#include <vdr/tools.h>

/*
 * General definitions
 */

#ifndef STAT_BUFF_DEFAULT
# define STAT_BUFF_DEFAULT  false
#endif

static const bool STAT_DDCIRECVBUF   = STAT_BUFF_DEFAULT;
static const bool STAT_DDCITSSENDBUF = STAT_BUFF_DEFAULT;
static const bool STAT_DDCITSRECVBUF = STAT_BUFF_DEFAULT;

// timeout for package buffer printing
static const int DBG_PKG_TMO = 10000;

/*
 * General purpose functions
 */

/**
 * Checks if on the first position of data the TS_SYNC_BYTE is present. if
 * not, it is searched and skipped is set to the number of skipped bytes.
 * You need to check skipped on function return, to determine, if the returned
 * pointer is valid or not.
 * @param data the data to check/find
 * @param length the length of the data
 * @param skipped number of bytes skipped in data
 * @return pointer to TS_SYNC_BYTE
 */
extern uchar *CheckTsSync( uchar *data, int length, int &skipped );

/**
 * Checks if the whole buffer contains every TS_SIZE a TS_SYNC_BYTE, beginning
 * with the first byte. The search stops at the first non TS_SYNC_BYTE position.
 * posnsync is set to the first non TS_SYNC_BYTE position or NULL, if the whole
 * buffer contains the expected TS_SYNC_BYTEs.
 * @param data the data to check
 * @param length the length of the data
 * @param posnsync set to the first non TS_SYNC_BYTE position
 * @return true, if the whole buffer contains the expected TS_SYNC_BYTEs
 */
extern bool CheckAllSync( uchar *data, int length, uchar *&posnsync );

#endif //__DDCICOMMON_H
