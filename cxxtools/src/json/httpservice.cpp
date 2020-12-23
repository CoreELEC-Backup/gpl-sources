/*
 * Copyright (C) 2011 by Tommi Meakitalo
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
#include "cxxtools/json/httpservice.h"
#include "cxxtools/http/request.h"
#include "httpresponder.h"
#include "cxxtools/log.h"

#include <string.h>

log_define("cxxtools.json.httpservice")

namespace cxxtools
{

namespace json
{

HttpService::~HttpService()
{
}


http::Responder* HttpService::createResponder(const http::Request& request)
{
    const char* contentType = request.header().getHeader("Content-Type");
    if (contentType != 0)
    {
        if (::strncasecmp(contentType, "application/json", 16) == 0 
             || ::strncasecmp(contentType, "application/x-www-form-urlencoded", 33) == 0)
            return new HttpResponder(*this);

        log_warn("invalid content type " << contentType);
    }
    else
        log_warn("missing content type");

    return 0;
}


void HttpService::releaseResponder(http::Responder* resp)
{
    delete resp;
}

}

}
