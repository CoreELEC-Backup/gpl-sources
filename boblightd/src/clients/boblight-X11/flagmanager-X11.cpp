/*
 * boblight
 * Copyright (C) Bob  2009 
 * 
 * boblight is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * boblight is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>

#include "flagmanager-X11.h"
#include "util/misc.h"
#include "config.h"

using namespace std;

CFlagManagerX11::CFlagManagerX11()
{
  //extend the base getopt flags
  //i = interval, u = pixels, x = xgetimage, d = debug
  m_flags += "i:u:xd::";

  m_interval = 0.1;    //default interval is 100 milliseconds
  m_pixels = -1;       //-1 says to the capture classes to use default
  m_method = XRENDER;  //default method is fast xrender
  m_debug = false;     //no debugging by default
  m_debugdpy = NULL;   //default debug dpy is system default
  m_sync = true;       //sync mode enabled by default
}

void CFlagManagerX11::ParseFlagsExtended(int& argc, char**& argv, int& c, char*& optarg)
{
  if (c == 'i') //interval
  {
    bool vblank = false;

    if (optarg[0] == 'v') //starting interval with v means vblank interval
    {
#ifdef HAVE_LIBGL
      optarg++;
      vblank = true;
#else
      throw string("Compiled without opengl support");
#endif
    }

    if (!StrToFloat(optarg, m_interval) || m_interval <= 0.0)
    {
      throw string("Wrong value " + string(optarg) + " for interval");
    }

    if (vblank)
    {
      if (m_interval < 1.0)
      {
        throw string("Wrong value " + string(optarg) + " for vblank interval");
      }
      m_interval *= -1.0; //negative interval means vblank
      optarg--;
    }
  }
  else if (c == 'u') //nr of pixels to use
  {
    if (!StrToInt(optarg, m_pixels) || m_pixels <= 0)
    {
      throw string("Wrong value " + string(optarg) + " for pixels");
    }
  }
  else if (c == 'x') //use crap xgetimage instead of sleek xrender
  {
    m_method = XGETIMAGE;
  }
  else if (c == 'd') //turn on debug mode
  {
    m_debug = true;
    if (optarg)      //optional debug dpy
    {
      m_strdebugdpy = optarg;
      m_debugdpy = m_strdebugdpy.c_str();
    }
  }
}

void CFlagManagerX11::PrintHelpMessage()
{
  cout << "Usage: boblight-X11 [OPTION]\n";
  cout << "\n";
  cout << "  options:\n";
  cout << "\n";
  cout << "  -p  priority, from 0 to 255, default is 128\n";
  cout << "  -s  address:[port], set the address and optional port to connect to\n";
  cout << "  -o  add libboblight option, syntax: [light:]option=value\n";
  cout << "  -l  list libboblight options\n";
  cout << "  -i  set the interval in seconds, default is 0.1\n";
#ifdef HAVE_LIBGL
  cout << "      prefix the value with v to wait for a number of vertical blanks instead\n";
#endif
  cout << "  -u  set the number of pixels/rows to use\n";
  cout << "      default is 64 for xrender and 16 for xgetimage\n";
  cout << "  -x  use XGetImage instead of XRender\n";
  cout << "  -d  debug mode\n";
  cout << "  -f  fork\n";
  cout << "  -y  set the sync mode, default is on, valid options are \"on\" and \"off\"\n";
  cout << "\n";
}
