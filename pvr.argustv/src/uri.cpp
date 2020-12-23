/*
 *  Copyright (C) 2005-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "uri.h"

namespace uri
{
const char ENCODE_BEGIN_CHAR = '%';
const traits SCHEME_TRAITS = {
    0,
    0,
    ':',
    {
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CVA2, CINV,
        CVA2, CVA2, CINV, CVA2, CVA2, CVA2, CVA2, CVA2, CVA2, CVA2, CVA2, CVA2, CVA2, CEND, CINV,
        CINV, CINV, CINV, CINV, CINV, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CINV, CINV, CINV, CINV, CINV, CINV, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CVAL, CVAL, CINV, CINV, CINV, CINV, CINV, // 127 7F
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
    }};
const traits AUTHORITY_TRAITS = {
    (char*)"//",
    0,
    0,
    {
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CEND, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, // 127 7F
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
    }};
const traits PATH_TRAITS = {
    0,
    0,
    0,
    {
        // '/' is invalid
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CVAL, CINV, CINV, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CVAL, CINV, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CINV, CVAL, CINV, CINV, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CINV, CINV, CINV, CINV, CVAL, CINV, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CVAL, CVAL, CINV, CINV, CINV, CVAL, CINV, // 127 7F
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
    }};
const traits QUERY_TRAITS = {
    0,
    '?',
    0,
    {
        // '=' and '&' are invalid
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CVAL, CINV, CINV, CVAL, CVAL, CINV, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CINV, CINV, CINV, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CINV, CINV, CINV, CINV, CVAL, CINV, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CVAL, CVAL, CINV, CINV, CINV, CVAL, CINV, // 127 7F
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
    }};
const traits FRAGMENT_TRAITS = {
    0,
    '#',
    0,
    {
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CVAL, CINV, CINV, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CINV, CVAL, CINV, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CINV, CINV, CINV, CINV, CVAL, CINV, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL, CVAL,
        CVAL, CVAL, CVAL, CINV, CINV, CINV, CVAL, CINV, // 127 7F
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
        CINV, CINV, CINV, CINV, CINV, CINV, CINV, CINV,
    }};

bool parse_hex(const std::string& s, size_t pos, char& chr)
{
  if (s.size() < pos + 2)
    return false;
  unsigned int v;
  unsigned int c = (unsigned int)s[pos];
  if ('0' <= c && c <= '9')
    v = (c - '0') << 4;
  else if ('A' <= c && c <= 'F')
    v = (10 + (c - 'A')) << 4;
  else if ('a' <= c && c <= 'f')
    v = (10 + (c - 'a')) << 4;
  else
    return false;
  c = (unsigned int)s[pos + 1];
  if ('0' <= c && c <= '9')
    v += c - '0';
  else if ('A' <= c && c <= 'F')
    v += 10 + (c - 'A');
  else if ('a' <= c && c <= 'f')
    v += 10 + (c - 'a');
  else
    return false;
  chr = (char)v; // Set output.
  return true;
}

void append_hex(char v, std::string& s)
{
  unsigned int c = (unsigned char)v & 0xF0;
  c >>= 4;
  s.insert(s.end(), (char)((9 < c) ? (c - 10) + 'A' : c + '0'));
  c = v & 0x0F;
  s.insert(s.end(), (char)((9 < c) ? (c - 10) + 'A' : c + '0'));
}

std::string encode(const traits& ts, const std::string& comp)
{
  std::string::const_iterator f = comp.begin();
  std::string::const_iterator anchor = f;
  std::string s;

  for (; f != comp.end();)
  {
    char c = *f;
    if (ts.char_class[(unsigned char)c] < CVAL || c == ENCODE_BEGIN_CHAR)
    { // Must encode.
      s.append(anchor, f); // Catch up to this char.
      s.append(1, ENCODE_BEGIN_CHAR);
      append_hex(c, s); // Convert.
      anchor = ++f;
    }
    else
    {
      ++f;
    }
  }
  return (anchor == comp.begin()) ? comp : s.append(anchor, comp.end());
}

bool decode(std::string& s)
{
  size_t pos = s.find(ENCODE_BEGIN_CHAR);
  if (pos == std::string::npos)
  {
    // Handle the "99%" case fast.
    return true;
  }

  std::string v;
  for (size_t i = 0;;)
  {
    if (pos == std::string::npos)
    {
      v.append(s, i, s.size() - i); // Append up to end.
      break;
    }
    v.append(s, i, pos - i); // Append up to char.
    i = pos + 3; // Skip all 3 chars.
    char c;
    if (!parse_hex(s, pos + 1, c))
    {
      // Convert hex.
      return false;
    }
    v.insert(v.end(), c); // Append converted hex.
    pos = s.find(ENCODE_BEGIN_CHAR, i); // Find next
  }
  s = v;
  return true;
}
} // namespace uri
