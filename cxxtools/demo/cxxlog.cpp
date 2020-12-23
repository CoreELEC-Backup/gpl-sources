/*
 * Copyright (C) 2008 Tommi Maekitalo
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

#include <iostream>
#include <algorithm>
#include <iterator>
#include <cxxtools/arg.h>
#include <cxxtools/log.h>


// Normally the category parameter is a constant.
//
// log_define defines a function, which fetches a logger class using that
// constant. Since this is done only the first time something is logged, it is
// important to set the category before the first logging output statement.
//
std::string category;
log_define(category)

int main(int argc, char* argv[])
{
  try
  {
    cxxtools::Arg<bool> fatal(argc, argv, 'f');
    cxxtools::Arg<bool> fatal_l(argc, argv, "--fatal");
    cxxtools::Arg<bool> error(argc, argv, 'e');
    cxxtools::Arg<bool> error_l(argc, argv, "--error");
    cxxtools::Arg<bool> warn(argc, argv, 'w');
    cxxtools::Arg<bool> warn_l(argc, argv, "--warn");
    cxxtools::Arg<bool> info(argc, argv, 'i');
    cxxtools::Arg<bool> info_l(argc, argv, "--info");
    cxxtools::Arg<bool> debug(argc, argv, 'd');
    cxxtools::Arg<bool> debug_l(argc, argv, "--debug");
    cxxtools::Arg<std::string> properties(argc, argv, 'p', "log.properties");
    cxxtools::Arg<std::string> properties_l(argc, argv, "--properties", properties);

    if (argc <= 2)
    {
      std::cerr << "usage: " << argv[0] << " options category message\n"
                   "\toptions:  -f|--fatal\n"
                   "\t          -e|--error\n"
                   "\t          -w|--warn\n"
                   "\t          -i|--info\n"
                   "\t          -d|--debug\n"
                   "\t          -p|--properties filename" << std::endl;
      return -1;
    }

    category = argv[1];

    log_init(properties_l.getValue());

    std::ostringstream msg;
    std::copy(argv + 2,
              argv + argc,
              std::ostream_iterator<char*>(msg, " "));

    if (fatal || fatal_l)
      log_fatal(msg.str());

    if (error || error_l)
      log_error(msg.str());

    if (warn || warn_l)
      log_warn(msg.str());

    if (info || info_l)
      log_info(msg.str());

    if (debug || debug_l)
      log_debug(msg.str());
  }
  catch (const std::exception& e)
  {
    std::cerr << e.what() << std::endl;
  }
}

