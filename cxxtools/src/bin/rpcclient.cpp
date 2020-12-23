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

#include <cxxtools/bin/rpcclient.h>
#include "rpcclientimpl.h"

namespace cxxtools
{
namespace bin
{

RpcClient::RpcClient(SelectorBase& selector, const std::string& addr, unsigned short port, const std::string& domain)
    : _impl(new RpcClientImpl(selector, addr, port, domain))
{ 
}

RpcClient::RpcClient(const std::string& addr, unsigned short port, const std::string& domain)
    : _impl(new RpcClientImpl(addr, port, domain))
{ 
}

RpcClient::~RpcClient()
{
    delete _impl;
}

void RpcClient::setSelector(SelectorBase& selector)
{
    if (!_impl)
        _impl = new RpcClientImpl(std::string(), 0, std::string());
    _impl->setSelector(selector);
}

void RpcClient::connect(const std::string& addr, unsigned short port, const std::string& domain)
{
    if (!_impl)
        _impl = new RpcClientImpl(addr, port, domain);
    else
        _impl->connect(addr, port, domain);
}

void RpcClient::close()
{
    if (_impl)
        _impl->close();
}

void RpcClient::beginCall(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc)
{
    _impl->beginCall(r, method, argv, argc);
}

void RpcClient::endCall()
{
    _impl->endCall();
}

void RpcClient::call(IComposer& r, IRemoteProcedure& method, IDecomposer** argv, unsigned argc)
{
    _impl->call(r, method, argv, argc);
}

const IRemoteProcedure* RpcClient::activeProcedure() const
{
    return _impl->activeProcedure();
}

void RpcClient::wait(std::size_t msecs)
{
    _impl->wait(msecs);
}

void RpcClient::cancel()
{
    _impl->cancel();
}

const std::string& RpcClient::domain() const
{
    return _impl->domain();
}

void RpcClient::domain(const std::string& p)
{
    _impl->domain(p);
}

}
}
