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

#include "distance.h"
#include <string.h>
#ifdef __FreeBSD__
#include <stdlib.h>
#else
#include <malloc.h>
#endif
#include <vdr/tools.h>


//****************************
// Get minimum of three values
//****************************

int Distance::Minimum(int a, int b, int c)
{
    int mi;

    mi = a;
    if (b < mi) {
        mi = b;
    }
    if (c < mi) {
        mi = c;
    }
    return mi;

}

//**************************************************
// Get a pointer to the specified cell of the matrix
//**************************************************

int *Distance::GetCellPointer(int *pOrigin, int col, int row, int nCols)
{
    return pOrigin + col + (row * (nCols + 1));
}

//*****************************************************
// Get the contents of the specified cell in the matrix
//*****************************************************

int Distance::GetAt(int *pOrigin, int col, int row, int nCols)
{
    int *pCell;

    pCell = GetCellPointer(pOrigin, col, row, nCols);
    return *pCell;

}

//*******************************************************
// Fill the specified cell in the matrix with the value x
//*******************************************************

void Distance::PutAt(int *pOrigin, int col, int row, int nCols, int x)
{
    int *pCell;

    pCell = GetCellPointer(pOrigin, col, row, nCols);
    *pCell = x;

}

//*****************************
// Compute Levenshtein distance
//*****************************

int Distance::LD(char const *s, char const *t, int maxLength)
{
    int *d; // pointer to matrix
    int n; // length of s
    int m; // length of t
    int i; // iterates through s
    int j; // iterates through t
    char s_i; // ith character of s
    char t_j; // jth character of t
    int cost; // cost
    int result; // result
    int cell; // contents of target cell
    int above; // contents of cell immediately above
    int left; // contents of cell immediately to left
    int diag; // contents of cell immediately above and to left
    int sz; // number of cells in matrix

    // Step 1

    n = min((int)strlen(s), maxLength);
    m = min((int)strlen(t), maxLength);
    if (n == 0) {
        return m;
    }
    if (m == 0) {
        return n;
    }
    sz = (n + 1) * (m + 1) * sizeof(int);
    d = (int *) malloc(sz);

    // Step 2

    for (i = 0; i <= n; i++) {
        PutAt(d, i, 0, n, i);
    }

    for (j = 0; j <= m; j++) {
        PutAt(d, 0, j, n, j);
    }

    // Step 3

    for (i = 1; i <= n; i++) {

        s_i = s[i - 1];

        // Step 4

        for (j = 1; j <= m; j++) {

            t_j = t[j - 1];

            // Step 5

            if (s_i == t_j) {
                cost = 0;
            } else {
                cost = 1;
            }

            // Step 6

            above = GetAt(d, i - 1, j, n);
            left = GetAt(d, i, j - 1, n);
            diag = GetAt(d, i - 1, j - 1, n);
            cell = Minimum(above + 1, left + 1, diag + cost);
            PutAt(d, i, j, n, cell);
        }
    }

    // Step 7

    result = GetAt(d, n, m, n);
    free(d);
    return result;

}

