/////////////////////////////////////////////////////////////////////////////
//
// @file ddcicommon.cpp @brief Digital Devices Common Interface plugin for VDR.
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

#include "ddcicommon.h"

#include <vdr/remux.h>   // TS_SIZE, TS_SYNC_BYTE

//------------------------------------------------------------------------

uchar *CheckTsSync( uchar *data, int length, int &skipped )
{
	skipped = 0;
	if (*data != TS_SYNC_BYTE) {
		skipped = 1;
		while (skipped < length
				&& (data[ skipped ] != TS_SYNC_BYTE
						|| (((length - skipped) > TS_SIZE) && (data[ skipped + TS_SIZE ] != TS_SYNC_BYTE)))) {
			skipped++;
		}
	}
	return data + skipped;
}

//------------------------------------------------------------------------

bool CheckAllSync( uchar *data, int length, uchar *&posnsync )
{
	posnsync = NULL;
	length -= length % TS_SIZE;
	int len = 0;
	while ((len < length) && data[len] == TS_SYNC_BYTE) {
		len += TS_SIZE;
	}
	bool ret = (len != length);
	if (!ret)
		posnsync = &data[len];
	return ret;
}
