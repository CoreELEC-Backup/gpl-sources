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

//---------------------------------------------------
// Levenshtein Distance
// by Michael Gilleland, Merriam Park Software
//
// source:
// http://www.merriampark.com/ld.htm#CPLUSPLUS
//
//---------------------------------------------------

#ifndef _DISTANCE_INC_
#define _DISTANCE_INC_

class Distance
{
public:
    int LD(char const *s, char const *t, int maxLength);
private:
    int Minimum(int a, int b, int c);
    int *GetCellPointer(int *pOrigin, int col, int row, int nCols);
    int GetAt(int *pOrigin, int col, int row, int nCols);
    void PutAt(int *pOrigin, int col, int row, int nCols, int x);
};

#endif
