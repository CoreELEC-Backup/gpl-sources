/*
 * Copyright (C) 2005-2008 Tommi Maekitalo
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * As a special exception, you may use this file as part of a free
 * software library without restriction. Specifically, if other files
 * instantiate templates or use macros or inline functions from this
 * file, or you compile this file and link it with other files to
 * produce an executable, this file does not by itself cause the
 * resulting executable to be covered by the GNU General Public
 * License. This exception does not however invalidate any other
 * reasons why the executable file might be covered by the GNU Library
 * General Public License.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */


#include "cxxtools/regex.h"
#include <stdexcept>
#include <locale>
#include <cctype>

namespace cxxtools
{
  unsigned RegexSMatch::size() const
  {
    unsigned n;
    for (n = 0; n < 10 && matchbuf[n].rm_so >= 0; ++n)
      ;

    return n;
  }

  std::string RegexSMatch::get(unsigned n) const
  {
    return has(n) ? str.substr(matchbuf[n].rm_so,
                               matchbuf[n].rm_eo - matchbuf[n].rm_so)
                  : std::string();
  }

  std::string RegexSMatch::format(const std::string& s) const
  {
    enum state_type
    {
      state_0,
      state_esc,
      state_var0,
      state_var1,
      state_1
    } state;

    state = state_0;
    std::string ret;

    for (std::string::const_iterator it = s.begin(); it != s.end(); ++it)
    {
      char ch = *it;

      switch (state)
      {
        case state_0:
          if (ch == '$')
            state = state_var0;
          else if (ch == '\\')
            state = state_esc;
          break;

        case state_esc:
          ret += ch;
          state = state_1;
          break;

        case state_var0:
          if (std::isdigit(ch))
          {
            ret = std::string(s.begin(), it - 1);
            regoff_t s = matchbuf[ch - '0'].rm_so;
            regoff_t e = matchbuf[ch - '0'].rm_eo;
            if (s >= 0 && e >= 0)
              ret.append(str, s, e-s);
            state = state_1;
          }
          else
            state = state_0;
          break;

        case state_1:
          if (ch == '$')
            state = state_var1;
          else if (ch == '\\')
            state = state_esc;
          else
            ret += ch;
          break;

        case state_var1:
          if (std::isdigit(ch))
          {
            regoff_t s = matchbuf[ch - '0'].rm_so;
            regoff_t e = matchbuf[ch - '0'].rm_eo;
            if (s >= 0 && e >= 0)
              ret.append(str, s, e-s);
            state = state_1;
          }
          else if (ch == '$')
            ret += '$';
          else
          {
            ret += '$';
            ret += ch;
          }
          break;
      }
    }

    switch (state)
    {
      case state_0:
      case state_var0:
        return s;

      case state_esc:
        return ret + '\\';

      case state_var1:
        return ret + '$';

      case state_1:
        return ret;
    }

    return ret;
  }

  void Regex::checkerr(int ret) const
  {
    if (ret != 0)
    {
      char errbuf[256];
      regerror(ret, expr.getPointer(), errbuf, sizeof(errbuf));
      throw std::runtime_error(errbuf);
    }
  }

  bool Regex::match(const std::string& str_, int eflags) const
  {
    RegexSMatch smatch;
    return match(str_, smatch, eflags);
  }

  bool Regex::match(const std::string& str_, RegexSMatch& smatch, int eflags) const
  {
    if (expr.getPointer() == 0)
    {
      smatch.matchbuf[0].rm_so = 0;
      return true;
    }

    smatch.str = str_;
    int ret = regexec(expr.getPointer(), str_.c_str(),
        sizeof(smatch.matchbuf) / sizeof(regmatch_t), smatch.matchbuf, eflags);

    if (ret ==REG_NOMATCH)
      return false;

    checkerr(ret);
    return true;
  }

}
