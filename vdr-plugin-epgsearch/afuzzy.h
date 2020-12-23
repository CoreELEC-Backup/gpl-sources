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

#ifndef _AFUZZY_H
#define _AFUZZY_H

#include <stdio.h>
// source from:
/*
  Leonid Boitsov 2002. (itman@narod.ru)
  C version of Stas Namin.
  This code is a GPL software and is distributed under GNU
  public licence without any warranty.
*/

typedef unsigned int Uint;

#define MaxPatSize (sizeof(Uint) * 8)

typedef struct {
    Uint        *R,
                *R1,
                *RP,
                *S,
                *RI;
    Uint        *FilterS;

    int         Map[256];
    int         FilterMap[256];
    int         k;
    Uint        mask_ok;
    Uint        filter_ok;
    Uint        filter_shift;
    int         r_size;
    int         FilterSet;
} AFUZZY;

void afuzzy_init(const char *p, int kerr, int UseFilter, AFUZZY *fuzzy);
void afuzzy_free(AFUZZY *fuzzy);
int afuzzy_checkSUB(const char *t, AFUZZY *fuzzy);

#endif


