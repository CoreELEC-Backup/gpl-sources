/*
 * Copyright (C) 2011 Tommi Maekitalo
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

#include "scanner.h"
#include <cxxtools/log.h>
#include <cxxtools/remoteexception.h>
#include <cxxtools/deserializerbase.h>

log_define("cxxtools.bin.scanner")

namespace cxxtools
{
namespace bin
{

void Scanner::begin(DeserializerBase& handler, IComposer& composer)
{
    _vp.begin(handler);
    _deserializer = &handler;
    _composer = &composer;
    _deserializer->begin();
    _state = state_0;
    _failed = false;
    _errorCode = 0;
    _errorMessage.clear();
}

bool Scanner::advance(char ch)
{
    switch (_state)
    {
        case state_0:
            if (ch == '\xc1')
            {
                _failed = false;
                _state = state_value;
            }
            else if (ch == '\xc2')
            {
                _failed = true;
                _state = state_errorcode;
                _count = 4;
            }
            else
                throw std::runtime_error("response expected");
            break;

        case state_value:
            if (_vp.advance(ch))
            {
                _composer->fixup(*_deserializer->si());
                _deserializer->si()->clear();
                _state = state_end;
            }
            break;

        case state_errorcode:
            _errorCode = (_errorCode << 8) | ch;
            if (--_count == 0)
                _state = state_errormessage;
            break;

        case state_errormessage:
            if (ch == '\0')
                _state = state_end;
            else
                _errorMessage += ch;
            break;

        case state_end:
            if (ch == '\xff')
            {
                log_debug("reply finished");
                return true;
            }
            else
                throw std::runtime_error("end of response marker expected");
            break;
    }

    return false;
}

void Scanner::checkException()
{
    if (_failed)
        throw RemoteException(_errorMessage, _errorCode);
}

}
}
