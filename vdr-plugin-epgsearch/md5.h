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

/////////////////////////////////////////////////////////////////////////
// MD5.cpp
// Implementation file for MD5 class
//
// This C++ Class implementation of the original RSA Data Security, Inc.
// MD5 Message-Digest Algorithm is copyright (c) 2002, Gary McNickle.
// All rights reserved.  This software is a derivative of the "RSA Data
//  Security, Inc. MD5 Message-Digest Algorithm"
//
// You may use this software free of any charge, but without any
// warranty or implied warranty, provided that you follow the terms
// of the original RSA copyright, listed below.
//
// Original RSA Data Security, Inc. Copyright notice
/////////////////////////////////////////////////////////////////////////
//
// Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
// rights reserved.
//
// License to copy and use this software is granted provided that it
// is identified as the "RSA Data Security, Inc. MD5 Message-Digest
// Algorithm" in all material mentioning or referencing this software
// or this function.
// License is also granted to make and use derivative works provided
// that such works are identified as "derived from the RSA Data
// Security, Inc. MD5 Message-Digest Algorithm" in all material
// mentioning or referencing the derived work.
// RSA Data Security, Inc. makes no representations concerning either
// the merchantability of this software or the suitability of this
// software for any particular purpose. It is provided "as is"
// without express or implied warranty of any kind.
// These notices must be retained in any copies of any part of this
// documentation and/or software.
/////////////////////////////////////////////////////////////////////////

typedef unsigned       int uint4;
typedef unsigned short int uint2;
typedef unsigned      char uchar;

char* PrintMD5(uchar md5Digest[16]);
char* MD5String(char* szString);

class md5
{
// Methods
public:
    md5() {
        Init();
        m_Buffer[0] = 0;
        m_Digest[0] = 0;
        m_Finalized = 0;
    }
    void    Init();
    void    Update(uchar* chInput, uint4 nInputLen);
    void    Finalize();
    uchar*  Digest() {
        return m_Digest;
    }

private:

    void    Transform(uchar* block);
    void    Encode(uchar* dest, uint4* src, uint4 nLength);
    void    Decode(uint4* dest, uchar* src, uint4 nLength);


    inline  uint4   rotate_left(uint4 x, uint4 n) {
        return ((x << n) | (x >> (32 - n)));
    }

    inline  uint4   F(uint4 x, uint4 y, uint4 z) {
        return ((x & y) | (~x & z));
    }

    inline  uint4   G(uint4 x, uint4 y, uint4 z) {
        return ((x & z) | (y & ~z));
    }

    inline  uint4   H(uint4 x, uint4 y, uint4 z) {
        return (x ^ y ^ z);
    }

    inline  uint4   I(uint4 x, uint4 y, uint4 z) {
        return (y ^ (x | ~z));
    }

    inline  void    FF(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
        a += F(b, c, d) + x + ac;
        a = rotate_left(a, s);
        a += b;
    }

    inline  void    GG(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
        a += G(b, c, d) + x + ac;
        a = rotate_left(a, s);
        a += b;
    }

    inline  void    HH(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
        a += H(b, c, d) + x + ac;
        a = rotate_left(a, s);
        a += b;
    }

    inline  void    II(uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac) {
        a += I(b, c, d) + x + ac;
        a = rotate_left(a, s);
        a += b;
    }

// Data
private:
    uint4       m_State[4];
    uint4       m_Count[2];
    uchar       m_Buffer[64];
    uchar       m_Digest[16];
    uchar       m_Finalized;

};
