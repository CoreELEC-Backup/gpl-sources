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

//every baudrate I could find is in here in an #ifdef block
//so it should compile on everything

#ifndef BAUDRATE
#define BAUDRATE

static struct sbaudrate
{
  int rate;
  int symbol;
}
baudrates[] =
{
#ifdef B50
  { 50, B50 },
#endif
#ifdef B75
  { 75, B75 },
#endif
#ifdef B110
  { 110, B110 },
#endif
#ifdef B134
  { 134, B134 },
#endif
#ifdef B150
  { 150, B150 },
#endif
#ifdef B200
  { 200, B200 },
#endif
#ifdef B300
  { 300, B300 },
#endif
#ifdef B600
  { 600, B600 },
#endif
#ifdef B1200
  { 1200, B1200 },
#endif
#ifdef B1800
  { 1800, B1800 },
#endif
#ifdef B2400
  { 2400, B2400 },
#endif
#ifdef B4800
  { 4800, B4800 },
#endif
#ifdef B9600
  { 9600, B9600 },
#endif
#ifdef B14400
  { 14400, B14400 },
#endif
#ifdef B19200
  { 19200, B19200 },
#endif
#ifdef B28800
  { 28800, B28800 },
#endif
#ifdef B38400
  { 38400, B38400 },
#endif
#ifdef B57600
  { 57600, B57600 },
#endif
#ifdef B76800
  { 76800, B76800 },
#endif
#ifdef B115200
  { 115200, B115200 },
#endif
#ifdef B230400
  { 230400, B230400 },
#endif
#ifdef B250000
  { 250000, B250000 },
#endif
#ifdef B460800
  { 460800, B460800 },
#endif
#ifdef B500000
  { 500000, B500000 },
#endif
#ifdef B576000
  { 576000, B576000 },
#endif
#ifdef B921600
  { 921600, B921600 },
#endif
#ifdef B1000000
  { 1000000, B1000000 },
#endif
#ifdef B1152000
  { 1152000, B1152000 },
#endif
#ifdef B1500000
  { 1500000, B1500000 },
#endif
#ifdef B2000000
  { 2000000, B2000000 },
#endif
#ifdef B2500000
  { 2500000, B2500000 },
#endif
#ifdef B3000000
  { 3000000, B3000000 },
#endif
#ifdef B3500000
  { 3500000, B3500000 },
#endif
#ifdef B4000000
  { 4000000, B4000000 },
#endif
  { -1, -1}
};

#endif //BAUDRATE