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

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "afuzzy.h"

static int afuzzy_checkFLT(const char *t, AFUZZY *fuzzy);

/******************************************************************************
FUNCTION afuzzy_init()
    Initialization of the fuzzy search routine. This applies to the consequent
    calls of the afuzzy_CheckRTR (whole string matching) and afuzzy_CheckSUB
    (substring match) routines. afuzzy_init() should be called for each
    new pattern or error length. The search is case sensitive

ARGUMENTS:
    p           Pattern
    kerr        Number of possible errors. Shouldn't exceed pattern length
    UseFilter   Use agrep filter algorithm that speeds up search.
    fuzzy       pointer to the structure that will be later passes to Check*
                    (the first 6 elements should be NULLs for the first call)

RETURN VALUE:
    none

ALGORITHM
    see. the article on agrep algorithms.
    The only change is accounting transpositions as one edit operation .
******************************************************************************/
void afuzzy_init(const char *p, int kerr, int UseFilter, AFUZZY *fuzzy)
{
    int cnt, p_len, i, l, d, m;
    char PatFilter[sizeof(Uint) * 8 + 1];

    fuzzy->k = kerr;
    m = strlen(p);
    fuzzy->FilterSet = 0;
    memset(fuzzy->Map, 0 , sizeof(fuzzy->Map));

    if (fuzzy->S)
        free(fuzzy->S);
    if (fuzzy->R)
        free(fuzzy->R);
    if (fuzzy->R1)
        free(fuzzy->R1);
    if (fuzzy->RP)
        free(fuzzy->RP);
    if (fuzzy->RI)
        free(fuzzy->RI);
    if (fuzzy->FilterS)
        free(fuzzy->FilterS);

    fuzzy->FilterS = NULL;
    fuzzy->S = (Uint *)calloc(m + 1, sizeof(Uint));
    fuzzy->R = (Uint *)calloc(fuzzy->k + 1, sizeof(Uint));
    fuzzy->R1 = (Uint *)calloc(fuzzy->k + 1, sizeof(Uint));
    fuzzy->RI = (Uint *)calloc(fuzzy->k + 1, sizeof(Uint));
    fuzzy->RP = (Uint *)calloc(fuzzy->k + 1, sizeof(Uint));

    for (i = 0, cnt = 0; i < m; i++) {
        l = fuzzy->Map[(unsigned char)p[i]];
        if (!l) {
            l = fuzzy->Map[(unsigned char)p[i]] = ++cnt;
            fuzzy->S[l] = 0;
        }
        fuzzy->S[l] |= 1 << i;
    }


    for (d = 0; d <= fuzzy->k; d++)
        fuzzy->RI[d] = (1 << d) - 1;

    fuzzy->mask_ok = (1 << (m - 1));
    fuzzy->r_size  = sizeof(Uint) * (fuzzy->k + 1);
    p_len = m;

    if (p_len > (int) sizeof(Uint) * 8)
        p_len = (int) sizeof(Uint) * 8;

    /* If k is zero then no filter is needed! */
    if (fuzzy->k && (p_len >= 2 * (fuzzy->k + 1))) {
        if (UseFilter) {
            fuzzy->FilterSet = 1;
            memset(fuzzy->FilterMap, 0 , sizeof(fuzzy->FilterMap));
            fuzzy->FilterS = (Uint *)calloc(m + 1, sizeof(Uint));

            /* Not let's fill the interleaved pattern */
            int dd = p_len / (fuzzy->k + 1);
            p_len  = dd * (fuzzy->k + 1);

            for (i = 0, cnt = 0; i < dd; i++)
                for (int j = 0; j < fuzzy->k + 1; j++, cnt++)
                    PatFilter[cnt] = (unsigned char)p[j * dd + i];
            PatFilter[p_len] = 0;

            for (i = 0, cnt = 0; i < p_len; i++) {
                l = fuzzy->FilterMap[(unsigned char)PatFilter[i]];
                if (!l) {
                    l = fuzzy->FilterMap[(unsigned char)PatFilter[i]] = ++cnt;
                    fuzzy->FilterS[l] = 0;
                }
                fuzzy->FilterS[l] |= 1 << i;
            }
            fuzzy->filter_ok = 0;
            for (i = p_len - fuzzy->k - 1; i <= p_len - 1; i++) /* k+1 times */
                fuzzy->filter_ok |= 1 << i;

            /* k+1 first bits set to 1 */
            fuzzy->filter_shift = (1 << (fuzzy->k + 2)) - 1;
        }
    }
}

/******************************************************************************
FUNCTION afuzzy_free()
    Cleaning up after previous afuzzy_init() call.

ARGUMENTS:
    fuzzy       pointer to the afuzzy parameters structure

RETURN VALUE:
    none
******************************************************************************/
void afuzzy_free(AFUZZY *fuzzy)
{
    if (fuzzy->S) {
        free(fuzzy->S);
        fuzzy->S = NULL;
    }
    if (fuzzy->R) {
        free(fuzzy->R);
        fuzzy->R = NULL;
    }
    if (fuzzy->R1) {
        free(fuzzy->R1);
        fuzzy->R1 = NULL;
    }
    if (fuzzy->RP) {
        free(fuzzy->RP);
        fuzzy->RP = NULL;
    }
    if (fuzzy->RI) {
        free(fuzzy->RI);
        fuzzy->RI = NULL;
    }
    if (fuzzy->FilterS) {
        free(fuzzy->FilterS);
        fuzzy->FilterS = NULL;
    }
}


/******************************************************************************
FUNCTION afuzzy_CheckSUB()
    Perform a fuzzy pattern substring matching. afuzzy_init() should be
    called previously to initialize the pattern and error length.
    Positive result means that some part of the string given matches the
    pattern with no more than afuzzy->k errors (1 error = 1 letter
    replacement or transposition)

ARGUMENTS:
    t           the string to test
    fuzzy       pointer to the afuzzy parameters structure

RETURN VALUE:
    0   - no match
    > 0 - strings match

ALGORITHM
    ????????????????
******************************************************************************/
int afuzzy_checkSUB(const char *t, AFUZZY *fuzzy)
{
    register char c;
    register int j, d;

    /* For eficciency this case should be little bit optimized */
    if (!fuzzy->k) {
        Uint R = 0, R1;

        for (j = 0; (c = t[j]) != '\0'; j++) {
            R1 = (((R << 1) | 1) & fuzzy->S[fuzzy->Map[(unsigned char)c]]);
            R = R1;

            if (R1 & fuzzy->mask_ok)
                return 1;
        } /* end for (register int j = 0 ... */
        return 0;
    }

    if (fuzzy->FilterSet && !afuzzy_checkFLT(t, fuzzy))
        return 0;

    memcpy(fuzzy->R, fuzzy->RI, fuzzy->r_size); /* R = RI */

    for (j = 0; (c = t[j]); j++) {
        for (d = 0; d <= fuzzy->k; d++) {
            fuzzy->R1[d] = (((fuzzy->R[d] << 1) | 1) &
                            fuzzy->S[fuzzy->Map[(unsigned char)c]]);
            if (d > 0)
                fuzzy->R1[d] |= ((fuzzy->R[d - 1] | fuzzy->R1[d - 1]) << 1) | 1 |
                                fuzzy->R[d - 1];
        }
        if (fuzzy->R1[fuzzy->k] & fuzzy->mask_ok)
            return j;

        memcpy(fuzzy->R, fuzzy->R1, fuzzy->r_size);

    } /* end for (register int j = 0 ... */

    return 0;
}

static int afuzzy_checkFLT(const char *t, AFUZZY *fuzzy)
{
    register Uint FilterR = 0;
    register Uint FilterR1;
    register int j;

    for (j = 0; t[j] != '\0'; j++) {
        FilterR1 = (((FilterR << (fuzzy->k + 1)) | fuzzy->filter_shift) &
                    fuzzy->FilterS[fuzzy->FilterMap[(unsigned char)t[j]]]);
        if (FilterR1 & fuzzy->filter_ok)
            return 1;
        FilterR = FilterR1;
    } /* end for (register int j = 0 ... */

    return 0;
}


