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

#include "responder.h"
#include "rpcserverimpl.h"
#include <cxxtools/serviceprocedure.h>
#include <cxxtools/serviceregistry.h>
#include <cxxtools/remoteexception.h>
#include <cxxtools/textstream.h>
#include <cxxtools/utf8codec.h>
#include <cxxtools/log.h>

log_define("cxxtools.json.responder")

namespace cxxtools
{
namespace json
{
Responder::Responder(ServiceRegistry& serviceRegistry)
    : _serviceRegistry(serviceRegistry)
{
}

Responder::~Responder()
{
}

void Responder::begin()
{
    _deserializer.begin();
    _parser.begin(_deserializer);
}

void Responder::finalize(std::ostream& out)
{
    log_trace("finalize");

    std::string methodName;
    ServiceProcedure* proc = 0;

    TextOStream ts(out, new Utf8Codec());
    JsonFormatter formatter;

    formatter.begin(ts);

    formatter.beginObject(std::string(), std::string());
    formatter.addValueString("jsonrpc", "string", L"2.0");

    try
    {
        _deserializer.si()->getMember("method") >>= methodName;

        log_debug("method = " << methodName);
        proc = _serviceRegistry.getProcedure(methodName);
        if( ! proc )
            throw std::runtime_error("no such procedure \"" + methodName + '"');

        // compose arguments
        IComposer** args = proc->beginCall();

        // process args
        const SerializationInfo* paramsPtr = _deserializer.si()->findMember("params");

        // params may be ommited in request
        SerializationInfo emptyParams;

        const SerializationInfo& params = paramsPtr ? *paramsPtr : emptyParams;

        SerializationInfo::ConstIterator it = params.begin();
        if (args)
        {
            for (int a = 0; args[a]; ++a)
            {
                if (it == params.end())
                    throw RemoteException("argument expected");
                args[a]->fixup(*it);
                ++it;
            }
        }

        if (it != params.end())
            throw RemoteException("too many arguments");

        IDecomposer::formatEach(_deserializer.si()->getMember("id"), formatter);

        IDecomposer* result;
        result = proc->endCall();

        formatter.beginValue("result");
        result->format(formatter);
        formatter.finishValue();
    }
    catch (const RemoteException& e)
    {
        log_debug("method \"" << methodName << "\" exited with RemoteException: " << e.what());

        formatter.beginObject("error", std::string());

        formatter.addValueInt("code", "int", static_cast<Formatter::int_type>(e.rc()));
        formatter.addValueStdString("message", std::string(), e.what());

        formatter.finishObject();
    }
    catch (const std::exception& e)
    {
        log_debug("method \"" << methodName << "\" exited with exception: " << e.what());
        formatter.addValueStdString("error", std::string(), e.what());
    }

    formatter.finishObject();

    if (proc)
        _serviceRegistry.releaseProcedure(proc);
}

bool Responder::advance(char ch)
{
    return _parser.advance(ch) != 0;
}

}
}
