#include "import.h"

/*
  * v 1.0d  TESTED-OK  20060308
  * -----------------------
  *
  * The following SoundEx function is:
  *
  *    (C) Copyright 2002 - 2006, Creativyst, Inc.
  *               ALL RIGHTS RESERVED
  *
  * For more information go to:
  *           http://www.Creativyst.com
  * or email:
  *           Support@Creativyst.com
  *
  * Redistribution and use in source and binary
  * forms, with or without modification, are
  * permitted provided that the following conditions
  * are met:
  *
  *   1. Redistributions of source code must
  *      retain the above copyright notice, this
  *      list of conditions and the following
  *      disclaimer.
  *
  *   2. Redistributions in binary form must
  *      reproduce the above copyright notice,
  *      this list of conditions and the
  *      following disclaimer in the
  *      documentation and/or other materials
  *      provided with the distribution.
  *
  *   3. All advertising materials mentioning
  *      features or use of this software must
  *      display the following acknowledgement:
  *      This product includes software developed
  *      by Creativyst, Inc.
  *
  *   4. The name of Creativyst, Inc. may not be
  *      used to endorse or promote products
  *      derived from this software without
  *      specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY CREATIVYST CORPORATION
  *`AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
  * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
  * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
  * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO,
  * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
  * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
  * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY
  * WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
  * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  *
  * ------------------
  * ------------------
  * FUNCTION NOTES:
  *  1. To avoid all possibility of overwrites make
  *     sure *SoundEx points to a buffer with at least
  *     11 bytes of storage.
  *
  *  2. This function is for 7/8-bit ASCII characters.
  *     Modifications are required for UTF16/32, or for
  *     anything other than the first 7-bits of utf-8.
  *
  *  3. For those embedded guys who will understand this:
  *     This is a true library-grade (i.e. re-usable) function,
  *     meaning it has no dependencies on outside functions
  *     and requires no non-standard libraries be linked in
  *     order for it to work. In this case, since it doesn't
  *     even require the standard C library, it is what C99
  *     (I think) calls a: strictly conforming freestanding
  *     function.
  *
 */

int cImport::SoundEx(char *SoundEx,
                     char *WordString,
                     int   LengthOption,
                     int   CensusOption)
{
    int  InSz = 31;
    char WordStr[32];     /* one bigger than InSz */
    int  SoundExLen, WSLen, i;
    char FirstLetter, *p, *p2;

    SoundExLen = WSLen = 0;
    SoundEx[0] = 0;

    if (CensusOption)
    {
        LengthOption = 4;
    }

    if (LengthOption)
    {
        SoundExLen = LengthOption;
    }
    if (SoundExLen > 10)
    {
        SoundExLen = 10;
    }
    if (SoundExLen < 4)
    {
        SoundExLen = 4;
    }

    if (!WordString)
    {
        return(0);
    }

    /* Copy WordString to WordStr
     * without using funcs from other
     * libraries.
    */
    for (p = WordString,p2 = WordStr,i = 0;(*p);p++,p2++,i++)
    {
        if (i >= InSz) break;
        (*p2) = (*p);
    }
    (*p2) = 0;



    /* Convert WordStr to
     * upper-case, without using funcs
     * from other libraries
    */
    for (p = WordStr;(*p);p++)
    {
        if ((*p) >= 'a' && (*p) <= 'z')
        {
            (*p) -= 0x20;
        }
    }


    /* convert all non-alpha
     * chars to spaces
    */
    for (p = WordStr;(*p);p++)
    {
        if ((*p) < 'A' || (*p) > 'Z')
        {
            (*p) = ' ';
        }
    }

    /* Remove leading spaces
    */
    for (i = 0, p = p2 = WordStr;(*p);p++)
    {
        if (!i)
        {
            if ((*p) != ' ')
            {
                (*p2) = (*p);
                p2++;
                i++;
            }
        }
        else
        {
            (*p2) = (*p);
            p2++;
        }
    }
    (*p2) = 0;

    /* Get length of WordStr
    */
    for (i = 0,p = WordStr;(*p);p++) i++;


    /* Remove trailing spaces
    */
    for (;i;i--)
    {
        if (WordStr[i] == ' ')
        {
            WordStr[i] = 0;
        }
        else
        {
            break;
        }
    }

    /* Get length of WordStr
    */
    for (WSLen = 0,p = WordStr;(*p);p++) WSLen++;

    if (!WSLen)
    {
        return(0);
    }



    /* Perform our own multi-letter
     * improvements
     *
     * underscore placeholders (_) will be
     * removed below.
    */
    if (!CensusOption)
    {
        if (WordStr[0] == 'P' && WordStr[1] == 'S')
        {
            WordStr[0] = '_';
        }
        if (WordStr[0] == 'P' && WordStr[1] == 'F')
        {
            WordStr[0] = '_';
        }

        for (i = 0;i < WSLen;i++)
        {
            if (WordStr[i] == 'D' && WordStr[i+1] == 'G')
            {
                WordStr[i] = '_';
                i++;
                continue;
            }
            if (WordStr[i] == 'G' && WordStr[i+1] == 'H')
            {
                WordStr[i] = '_';
                i++;
                continue;
            }
            if (WordStr[i] == 'K' && WordStr[i+1] == 'N')
            {
                WordStr[i] = '_';
                i++;
                continue;
            }
            if (WordStr[i] == 'G' && WordStr[i+1] == 'N')
            {
                WordStr[i] = '_';
                i++;
                continue;
            }
            if (WordStr[i] == 'M' && WordStr[i+1] == 'B')
            {
                WordStr[i+1] = '_';
                i++;
                continue;
            }

            if (WordStr[i] == 'P' && WordStr[i+1] == 'H')
            {
                WordStr[i] = 'F';
                WordStr[i+1] = '_';
                i++;
                continue;
            }
            if (WordStr[i]  ==  'T'  &&
                    WordStr[i+1] == 'C' &&
                    WordStr[i+2] == 'H'
               )
            {

                WordStr[i] = '_';
                i++;
                i++;
                continue;
            }
            if (WordStr[i] == 'M' && WordStr[i+1] == 'P'
                    && (WordStr[i+2] == 'S' ||
                        WordStr[i+2] == 'T' ||
                        WordStr[i+2] == 'Z')
               )
            {
                WordStr[i+1] = '_';
                i++;
            }
        }
    } /* end if(!CensusOption) */

    /* squeeze out underscore characters
     * added as a byproduct of above process
     * (only needed in c styled replace)
    */
    for (p = p2 = WordStr;(*p);p++)
    {
        (*p2) = (*p);
        if ((*p2) != '_')
        {
            p2++;
        }
    }
    (*p2) = 0;

    /* This must be done AFTER our
     * multi-letter replacements
     * since they could change
     * the first letter
    */
    FirstLetter = WordStr[0];

    /* In case we're in CensusOption
     * 1 and the word starts with
     * an 'H' or 'W'
     *  (v1.0c djr: add test for H or W)
    */
    if (FirstLetter == 'H' || FirstLetter == 'W')
    {
        WordStr[0] = '-';
    }

    /* In properly done census
     * SoundEx, the H and W will
     * be squezed out before
     * performing the test
     * for adjacent digits
     * (this differs from how
     * 'real' vowels are handled)
    */
    if (CensusOption == 1)
    {
        for (p = &(WordStr[1]);(*p);p++)
        {
            if ((*p) == 'H' || (*p) == 'W')
            {
                (*p) = '.';
            }
        }
    }

    /* Perform classic SoundEx
     * replacements.
    */
    for (p = WordStr;(*p);p++)
    {
        if ((*p) == 'A'   ||
                (*p) == 'E'   ||
                (*p) == 'I'   ||
                (*p) == 'O'   ||
                (*p) == 'U'   ||
                (*p) == 'Y'   ||
                (*p) == 'H'   ||
                (*p) == 'W'
           )
        {
            (*p) = '0';   /* zero */
        }
        if ((*p) == 'B'   ||
                (*p) == 'P'   ||
                (*p) == 'F'   ||
                (*p) == 'V'
           )
        {
            (*p) = '1';
        }
        if ((*p) == 'C'   ||
                (*p) == 'S'   ||
                (*p) == 'G'   ||
                (*p) == 'J'   ||
                (*p) == 'K'   ||
                (*p) == 'Q'   ||
                (*p) == 'X'   ||
                (*p) == 'Z'
           )
        {
            (*p) = '2';
        }
        if ((*p) == 'D'   ||
                (*p) == 'T'
           )
        {
            (*p) = '3';
        }
        if ((*p) == 'L')
        {
            (*p) = '4';
        }

        if ((*p) == 'M'   ||
                (*p) == 'N'
           )
        {
            (*p) = '5';
        }
        if ((*p) == 'R')
        {
            (*p) = '6';
        }
    }
    /* soundex replacement loop done  */

    /* In properly done census
     * SoundEx, the H and W will
     * be squezed out before
     * performing the test
     * for adjacent digits
     * (this differs from how
     * 'real' vowels are handled)
    */
    if (CensusOption == 1)
    {
        /* squeeze out dots
        */
        for (p = p2 = &WordStr[1];(*p);p++)
        {
            (*p2) = (*p);
            if ((*p2) != '.')
            {
                p2++;
            }
        }
        (*p2) = 0;
    }

    /* squeeze out extra equal adjacent digits
     * (don't include first letter)
     * v1.0c djr (now includes first letter)
    */
    for (p = p2 = &(WordStr[0]);(*p);p++)
    {
        (*p2) = (*p);
        if ((*p2) != p[1])
        {
            p2++;
        }
    }
    (*p2) = 0;



    /* squeeze out spaces and zeros
     * Leave the first letter code
     * to be replaced below.
     * (In case it made a zero)
    */
    for (p = p2 = &WordStr[1];(*p);p++)
    {
        (*p2) = (*p);
        if ((*p2) != ' ' && (*p2) != '0')
        {
            p2++;
        }
    }
    (*p2) = 0;

    /* Get length of WordStr
    */
    for (WSLen = 0,p = WordStr;(*p);p++) WSLen++;


    /* Right pad with zero characters
    */
    for (i = WSLen;i < SoundExLen;i++)
    {
        WordStr[i] = '0';
    }

    /* Size to taste
    */
    WordStr[SoundExLen] = 0;

    /* Replace first digit with
     * first letter.
    */
    WordStr[0] = FirstLetter;

    /* Copy WordStr to SoundEx
    */
    for (p2 = SoundEx,p = WordStr;(*p);p++,p2++)
    {
        (*p2) = (*p);
    }
    (*p2) = 0;

    return(SoundExLen);
}
