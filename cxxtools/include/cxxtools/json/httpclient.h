/*
 * Copyright (C) 2011 by Tommi Maekitalo
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
#ifndef CXXTOOLS_JSON_HTTPCLIENT_H
#define CXXTOOLS_JSON_HTTPCLIENT_H

#include <cxxtools/remoteclient.h>
#include <string>

namespace cxxtools
{

class SelectorBase;

namespace net
{
    class Uri;
    class AddrInfo;
}

namespace json
{

    class HttpClientImpl;

    class HttpClient : public RemoteClient
    {
            HttpClient(HttpClient&);
            void operator= (const HttpClient&);

        public:
            HttpClient();

            HttpClient(SelectorBase& selector, const std::string& addr,
                   unsigned short port, const std::string& url);

            HttpClient(SelectorBase& selector, const net::Uri& uri);

            HttpClient(const std::string& addr, unsigned short port, const std::string& url);

            explicit HttpClient(const net::Uri& uri);

            virtual ~HttpClient();

            void connect(const net::AddrInfo& addrinfo, const std::string& url);

            void connect(const net::Uri& uri);

            void connect(const std::string& addr, unsigned short port,
                         const std::string& url);

            void url(const std::string& url);
            void auth(const std::string& username, const std::string& password);

            void clearAuth();

            void setSelector(SelectorBase& selector);

            void beginCall(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

            void endCall();

            void call(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc);

            std::size_t timeout() const;

            void timeout(std::size_t t);

            const std::string& url() const;

            const IRemoteProcedure* activeProcedure() const;

            void cancel();

            void wait(std::size_t msecs = WaitInfinite);

        private:
            HttpClientImpl* _impl;
    };

}

}

#endif
