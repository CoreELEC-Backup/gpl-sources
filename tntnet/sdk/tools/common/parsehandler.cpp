/*
 * Copyright (C) 2003-2005 Tommi Maekitalo
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


#include "tnt/ecpp/parsehandler.h"

namespace tnt
{
  namespace ecpp
  {
    void ParseHandler::start()
    {
    }

    void ParseHandler::end()
    {
    }

    void ParseHandler::onLine(unsigned lineno, const std::string& file)
    {
    }

    void ParseHandler::onHtml(const std::string& html)
    {
    }

    void ParseHandler::onExpression(const std::string& code)
    {
    }

    void ParseHandler::onHtmlExpression(const std::string& code)
    {
    }

    void ParseHandler::onCpp(const std::string& code)
    {
    }

    void ParseHandler::onPre(const std::string& code)
    {
    }

    void ParseHandler::onInit(const std::string& code)
    {
    }

    void ParseHandler::onCleanup(const std::string& code)
    {
    }

    void ParseHandler::onArg(const std::string& name, const std::string& value)
    {
    }

    void ParseHandler::onGet(const std::string& name, const std::string& value)
    {
      onArg(name, value);
    }

    void ParseHandler::onPost(const std::string& name, const std::string& value)
    {
      onArg(name, value);
    }

    void ParseHandler::onAttr(const std::string& name,
      const std::string& value)
    {
    }

    void ParseHandler::onCall(const std::string& comp,
      const comp_args_type& args, const std::string& pass_cgi,
      const paramargs_type& paramargs, const std::string& cppargs)
    {
    }

    void ParseHandler::onEndCall(const std::string& comp)
    {
    }

    void ParseHandler::onShared(const std::string& code)
    {
    }

    void ParseHandler::onScope(scope_container_type container, scope_type scope,
      const std::string& type, const std::string& var, const std::string& init)
    {
    }

    void ParseHandler::startComp(const std::string& arg, const cppargs_type& cppargs)
    {
    }

    void ParseHandler::startClose()
    {
    }

    void ParseHandler::endClose()
    {
    }

    void ParseHandler::onComp(const std::string& code)
    {
    }

    void ParseHandler::onCondExpr(const std::string& cond, const std::string& expr, bool htmlexpr)
    {
    }

    void ParseHandler::onConfig(const std::string& cond, const std::string& value)
    {
    }

    void ParseHandler::tokenSplit(bool start)
    {
    }

    void ParseHandler::onInclude(const std::string& file)
    {
    }

    void ParseHandler::onIncludeEnd(const std::string& file)
    {
    }

    void ParseHandler::startI18n()
    {
    }

    void ParseHandler::endI18n()
    {
    }

  }
}
