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


#include "tnt/ecppc/generator.h"
#include <tnt/stringescaper.h>
#include <iterator>
#include <zlib.h>
#include <vector>
#include <algorithm>
#include <cxxtools/log.h>
#include <tnt/httpmessage.h>
#include <tnt/componentfactory.h>

log_define("tntnet.generator")

namespace tnt
{
  namespace ecppc
  {
    namespace
    {
      class BStringPrinter
      {
          std::ostream& out;
          unsigned mincol, maxcol, col;

        public:
          explicit BStringPrinter(std::ostream& out_, unsigned mincol_,
            unsigned maxcol_, unsigned col_)
            : out(out_),
              mincol(mincol_),
              maxcol(maxcol_),
              col(col_)
            { }

          void print(const char* data, unsigned count);
          void print(char ch);
      };

      void BStringPrinter::print(const char* data, unsigned count)
      {
        for (unsigned n = 0; n < count; ++n)
          print(data[n]);
      }

      void BStringPrinter::print(char ch)
      {
        stringescaper s;
        const char* d = s(ch);
        unsigned len = strlen(d);
        col += len;
        if (col >= maxcol)
        {
          out << "\"\n";
          for (unsigned n = 0; n < mincol; ++n)
            out << ' ';
          out << '"';
          col = mincol + len;
        }
        out << d;
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Generator
    //
    Generator::Generator(const std::string& componentName)
      : raw(false),
        maincomp(componentName),
        haveCloseComp(false),
        closeComp(componentName),
        currentComp(&maincomp),
        externData(false),
        compress(false),
        c_time(0),
        linenumbersEnabled(true)
    {
      time_t cur;
      time(&cur);
      gentime = asctime(localtime(&cur));
    }

    bool Generator::hasScopevars() const
    {
      if (maincomp.hasScopevars())
        return true;
      for (subcomps_type::const_iterator it = subcomps.begin(); it != subcomps.end(); ++it)
        if (it->hasScopevars())
          return true;
      return false;
    }

    void Generator::addImage(const std::string& name, const std::string& content,
        const std::string& mime, time_t c_time_)
    {
      MultiImageType mi;
      mi.name = name;
      mi.mime = mime;
      mi.c_time = c_time_;
      multiImages.push_back(mi);

      data.push_back(content);
    }

    void Generator::onLine(unsigned lineno, const std::string& file)
    {
      log_debug("onLine(" << lineno << ", \"" << file << "\")");
      curline = lineno;
      curfile = file;
    }

    void Generator::onHtml(const std::string& html)
    {
      log_debug("onHtml(\"" << html << "\")");

      data.push_back(html);

      std::ostringstream m;
      m << "  reply.out() << data[" << (data.count() - 1) << "]; // ";

      std::transform(
        html.begin(),
        html.end(),
        std::ostream_iterator<const char*>(m),
        stringescaper(false));

      m << '\n';
      currentComp->addHtml(m.str());

      log_debug("onHtml(\"" << html << "\") end, data.size()=" << data.count());
    }

    void Generator::onExpression(const std::string& expr)
    {
      std::ostringstream m;
      printLine(m);
      m << "  reply.sout() << (" << expr << ");\n";
      currentComp->addHtml(m.str());
    }

    void Generator::onHtmlExpression(const std::string& expr)
    {
      std::ostringstream m;
      printLine(m);
      m << "  reply.out() << (" << expr << ");\n";
      currentComp->addHtml(m.str());
    }

    void Generator::onCpp(const std::string& code)
    {
      std::ostringstream m;
      printLine(m);
      m << code << '\n';
      currentComp->addHtml(m.str());
    }

    void Generator::onPre(const std::string& code)
    {
      std::ostringstream m;
      printLine(m);
      m << code << '\n';
      pre += m.str();
    }

    void Generator::onInit(const std::string& code)
    {
      std::ostringstream m;
      printLine(m);
      m << code << '\n';
      init += m.str();
    }

    void Generator::onCleanup(const std::string& code)
    {
      std::ostringstream m;
      printLine(m);
      m << code << '\n';
      cleanup += m.str();
    }

    void Generator::onArg(const std::string& name,
      const std::string& value)
    {
      currentComp->addArg(name, value);
    }

    void Generator::onGet(const std::string& name,
      const std::string& value)
    {
      currentComp->addGet(name, value);
    }

    void Generator::onPost(const std::string& name,
      const std::string& value)
    {
      currentComp->addPost(name, value);
    }

    void Generator::onAttr(const std::string& name,
      const std::string& value)
    {
      if (attr.find(name) != attr.end())
        throw std::runtime_error("duplicate attr " + name);
      attr.insert(attr_type::value_type(name, value));
    }

    void Generator::onCall(const std::string& comp,
      const comp_args_type& args, const std::string& pass_cgi,
      const paramargs_type& paramargs, const std::string& cppargs)
    {
      currentComp->addCall(curline, curfile, comp, args, pass_cgi, paramargs, cppargs);
    }

    void Generator::onEndCall(const std::string& comp)
    {
      currentComp->addEndCall(curline, curfile, comp);
    }

    void Generator::onShared(const std::string& code)
    {
      shared += code;
    }

    void Generator::startComp(const std::string& name,
      const cppargs_type& cppargs)
    {
      subcomps.push_back(Subcomponent(name, maincomp, cppargs));
      Subcomponent& s = subcomps.back();
      currentComp = &s;
      maincomp.addSubcomp(name);
    }

    void Generator::onComp(const std::string& code)
    {
      currentComp = &maincomp;
    }

    void Generator::startClose()
    {
      if (haveCloseComp)
        throw std::runtime_error("dumplicate close-part");
      haveCloseComp = true;
      currentComp = &closeComp;
    }

    void Generator::endClose()
    {
      currentComp = &maincomp;
    }

    void Generator::onCondExpr(const std::string& cond, const std::string& expr, bool htmlexpr)
    {
      std::ostringstream m;
      printLine(m);
      m << "  if (" << cond << ")\n";
      if (htmlexpr)
        m << "    reply.out() << ";
      else
        m << "    reply.sout() << ";
      m << expr << ";\n";
      currentComp->addHtml(m.str());
    }

    void Generator::onConfig(const std::string& name, const std::string& value)
    {
      configs.push_back(tnt::ecppc::Variable(name, value));
    }

    void Generator::onScope(scope_container_type container, scope_type scope,
      const std::string& type, const std::string& var, const std::string& init_)
    {
      log_debug("onScope type=\"" << type << "\" var=\"" << var << "\" init=\"" << init_ << '"');
      tnt::ecppc::Component* comp = (scope == ecpp::page_scope ? &maincomp : currentComp);

      comp->addScopevar(Scopevar(container, scope, type, var, init_, curline, curfile));
    }

    void Generator::onInclude(const std::string& file)
    {
      currentComp->addHtml("  // <%include> " + file + '\n');
    }

    void Generator::onIncludeEnd(const std::string& file)
    {
      currentComp->addHtml("  // </%include>\n");
    }

    void Generator::startI18n()
    {
      externData = true;
    }

    void Generator::getIntro(std::ostream& out, const std::string& filename) const
    {
      out << "////////////////////////////////////////////////////////////////////////\n"
             "// " << filename << "\n"
             "// generated with ecppc\n"
             "//\n\n";
    }

    void Generator::getHeaderIncludes(std::ostream& out) const
    {
      if (multiImages.empty() && !isRawMode())
        out << "#include <tnt/ecpp.h>\n"
               "#include <tnt/convert.h>\n";
      else
        out << "#include <tnt/mbcomponent.h>\n";
    }

    void Generator::getPre(std::ostream& out) const
    {
      out << "// <%pre>\n"
          << pre
          << "// </%pre>\n";
    }

    void Generator::getClassDeclaration(std::ostream& out) const
    {
      if (multiImages.empty() && !isRawMode())
      {
        out << "class _component_ : public tnt::EcppComponent\n"
               "{\n"
               "    _component_& main()  { return *this; }\n\n" 
               "  protected:\n"
               "    ~_component_();\n\n"
               "  public:\n"
               "    _component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl);\n\n"
               "    unsigned operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam);\n";
      }
      else
      {
        out << "class _component_ : public tnt::MbComponent\n"
               "{\n"
               "    _component_& main()  { return *this; }\n\n" ;
        if (compress)
          out << "    tnt::DataChunks data;\n\n";
        out << "  protected:\n"
               "    ~_component_();\n\n"
               "  public:\n"
               "    _component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl);\n\n";
      }

      if (haveCloseComp)
        out << "    unsigned endTag(tnt::HttpRequest& request, tnt::HttpReply& reply,\n"
               "                    tnt::QueryParams& qparam);\n";
      if (!attr.empty())
        out << "    std::string getAttribute(const std::string& name,\n"
               "      const std::string& def = std::string()) const;\n\n";

      if (!configs.empty())
      {
        out << "    // <%config>\n";
        for (variable_declarations::const_iterator it = configs.begin();
             it != configs.end(); ++it)
          it->getConfigHDecl(out);
        out << "    // </%config>\n\n";
      }

      // declare subcomponents
      for (subcomps_type::const_iterator i = subcomps.begin(); i != subcomps.end(); ++i)
        i->getHeader(out);

      // instantiate subcomponents
      for (subcomps_type::const_iterator i = subcomps.begin(); i != subcomps.end(); ++i)
        out << "    " << i->getName() << "_type " << i->getName() << ";\n";

      out << "};\n\n";
    }

    void Generator::getCppIncludes(std::ostream& out) const
    {
      out << "#include <tnt/httprequest.h>\n"
             "#include <tnt/httpreply.h>\n"
             "#include <tnt/httpheader.h>\n"
             "#include <tnt/http.h>\n"
             "#include <tnt/data.h>\n"
             "#include <tnt/componentfactory.h>\n";

      if (!configs.empty())
        out << "#include <tnt/comploader.h>\n"
               "#include <tnt/tntconfig.h>\n";

      if (compress)
        out << "#include <tnt/zdata.h>\n";

      out << "#include <cxxtools/log.h>\n"
             "#include <stdexcept>\n\n";
    }

    void Generator::getFactoryDeclaration(std::ostream& code) const
    {
      if (configs.empty())
      {
        code << "static tnt::ComponentFactoryImpl<_component_> Factory(\"" << maincomp.getName() << "\");\n\n";
      }
      else
      {
        code << "class _component_Factory : public tnt::ComponentFactoryImpl<_component_>\n"
                "{\n"
                "  public:\n"
                "    _component_Factory()\n"
                "      : tnt::ComponentFactoryImpl<_component_>(\"" << maincomp.getName() << "\")\n"
                "      { }\n"
                "    tnt::Component* doCreate(const tnt::Compident& ci,\n"
                "      const tnt::Urlmapper& um, tnt::Comploader& cl);\n"
                "    virtual void doConfigure(const tnt::TntConfig& config);\n"
                "};\n\n"
                "tnt::Component* _component_Factory::doCreate(const tnt::Compident& ci,\n"
                "  const tnt::Urlmapper& um, tnt::Comploader& cl)\n"
                "{\n"
                "  return new _component_(ci, um, cl);\n"
                "}\n\n"
                "void _component_Factory::doConfigure(const tnt::TntConfig& config)\n"
                "{\n"
                "  // <%config>\n";
        for (variable_declarations::const_iterator it = configs.begin();
             it != configs.end(); ++it)
          it->getConfigInit(code);
        code << "  // </%config>\n"
                "}\n\n"
                "static _component_Factory factory;\n\n";
      }
    }

    void Generator::getCppBody(std::ostream& code) const
    {
      getFactoryDeclaration(code);

      if (!data.empty())
      {
        if (compress)
        {
          code << "static tnt::Zdata rawData(\n\"";

          uLongf s = data.size() + data.size() / 100 + 100;
          std::vector<char> p(s);
          char* buffer = &p[0];

          int z_ret = ::compress(reinterpret_cast<Bytef*>(buffer), &s, (const Bytef*)data.ptr(), data.size());

          if (z_ret != Z_OK)
          {
            throw std::runtime_error(std::string("error compressing data: ") +
              (z_ret == Z_MEM_ERROR ? "Z_MEM_ERROR" :
               z_ret == Z_BUF_ERROR ? "Z_BUF_ERROR" :
               z_ret == Z_DATA_ERROR ? "Z_DATA_ERROR" : "unknown error"));
          }

          BStringPrinter bs(code, 2, 120, 30);
          bs.print(buffer, s);

          code << "\",\n  " << s << ", " << data.size() << ");\n\n";
        }
        else
        {
          code << "static const char* rawData = \"";

          BStringPrinter bs(code, 2, 120, 30);
          bs.print(data.ptr(), data.size());

          code << "\";\n\n";
        }
      }

      // multi-images
      if (!multiImages.empty())
      {
        code << "static const char* urls[] = {\n";

        for (MultiImagesType::const_iterator it = multiImages.begin();
             it != multiImages.end(); ++it)
          code << "  \"" << it->name << "\",\n";

        code << "};\n\n"
                "static const char* mimetypes[] = {\n";

        for (MultiImagesType::const_iterator it = multiImages.begin();
             it != multiImages.end(); ++it)
          code << "  \"" << it->mime << "\",\n";

        code << "};\n\n"
                "const char* ctimes[] = {\n";

        for (MultiImagesType::const_iterator it = multiImages.begin();
             it != multiImages.end(); ++it)
          code << "  \"" << tnt::HttpMessage::htdate(it->c_time) << "\",\n";

        code << "};\n\n";

        if (compress)
        {
          code << "_component_::_component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl)\n"
                  "  : MbComponent(ci, um, cl)\n"
                  "{\n"
                  "  ::rawData.addRef();\n"
                  "  data.setData(::rawData);\n"
                  "  init(::rawData, ::urls, ::mimetypes, ::ctimes);\n"
                  "}\n\n"
                  "_component_::~_component_()\n"
                  "{\n"
                  "  ::rawData.release();\n"
                  "}\n\n";
        }
        else
        {
          code << "_component_::_component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl)\n"
                  "  : MbComponent(ci, um, cl, ::rawData, ::urls, ::mimetypes, ::ctimes)\n"
                  "{ }\n\n"
                  "_component_::~_component_()\n"
                  "{ }\n\n";
        }
      }
      else if (isRawMode())
      {
        code << "static const char* mimetype = \"" << mimetype << "\";\n"
                "static const char* c_time = \"" << tnt::HttpMessage::htdate(c_time) << "\";\n\n";

        if (compress)
        {
          code << "_component_::_component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl)\n"
                  "  : MbComponent(ci, um, cl)\n"
                  "{\n"
                  "  ::rawData.addRef();\n"
                  "  data.setData(::rawData);\n"
                  "  init(::rawData, ::mimetype, ::c_time);\n"
                  "}\n\n"
                  "_component_::~_component_()\n"
                  "{\n"
                  "  ::rawData.release();\n"
                  "}\n\n";
        }
        else
        {
          code << "_component_::_component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl)\n"
                  "  : MbComponent(ci, um, cl, ::rawData, ::mimetype, ::c_time)\n"
                  "{ }\n\n"
                  "_component_::~_component_()\n"
                  "{ }\n\n";
        }
      }
      else
      {
        // logger, %shared and constructor
        //
        code << "// <%shared>\n"
             << shared
             << "// </%shared>\n\n";
        code << "// <%config>\n";
        for (variable_declarations::const_iterator it = configs.begin();
             it != configs.end(); ++it)
          it->getConfigDecl(code);
        code << "// </%config>\n\n"
                "#define SET_LANG(lang) \\\n"
                "     do \\\n"
                "     { \\\n"
                "       request.setLang(lang); \\\n"
                "       reply.setLocale(request.getLocale()); \\\n";
        if (externData)
          code << "       data.setData(getData(request, rawData)); \\\n";
        code << "     } while (false)\n\n"
             << "_component_::_component_(const tnt::Compident& ci, const tnt::Urlmapper& um, tnt::Comploader& cl)\n"
                "  : EcppComponent(ci, um, cl)";

        // initialize subcomponents
        for (subcomps_type::const_iterator i = subcomps.begin(); i != subcomps.end(); ++i)
          code << ",\n"
                  "    " << i->getName() << "(*this, \"" << i->getName() << "\")";

        code << "\n{\n";

        if (compress)
          code << "  rawData.addRef();\n";

        code << "  // <%init>\n"
             << init
             << "  // </%init>\n"
                "}\n\n"
                "_component_::~_component_()\n"
                "{\n"
                "  // <%cleanup>\n"
             << cleanup
             << "  // </%cleanup>\n";

        if (compress)
          code << "  rawData.release();\n";

        code << "}\n\n"
                "unsigned _component_::operator() (tnt::HttpRequest& request, tnt::HttpReply& reply, tnt::QueryParams& qparam)\n"
             << "{\n"
                "  log_trace(\"" << maincomp.getName() << " \" << qparam.getUrl());\n\n";

        if (raw)
          code << "  reply.setKeepAliveHeader();\n\n";
        if (!mimetype.empty())
          code << "  reply.setContentType(\"" << mimetype << "\");\n";

        if (c_time)
          code << "  {\n"
                  "    std::string s = request.getHeader(tnt::httpheader::ifModifiedSince);\n"
                  "    if (s == \"" << tnt::HttpMessage::htdate(c_time) << "\")\n"
                  "      return HTTP_NOT_MODIFIED;\n"
                  "  }\n";

        if (!data.empty())
        {
          if (externData)
            code << "  tnt::DataChunks data(getData(request, rawData));\n";
          else
            code << "  tnt::DataChunks data(rawData);\n";
        }

        if (multiImages.empty())
        {
          if (c_time)
            code << "  reply.setHeader(tnt::httpheader::lastModified, \""
                 << tnt::HttpMessage::htdate(c_time) << "\");\n";
          if (raw)
            code << "  reply.setContentLengthHeader(data.size(0));\n"
                    "  reply.setDirectMode();\n";

          code << '\n';
          maincomp.getBody(code, linenumbersEnabled);
          code << "}\n\n";
        }
        else
        {
          // multi-image-component
          code << "  const char* url = request.getPathInfo().c_str();\n\n"
                  "  log_debug(\"search for \\\"\" << url << '\"');\n\n"
                  "  urls_iterator it = std::lower_bound(urls_begin, urls_end, url, charpLess);\n"
                  "  if (it == urls_end || strcmp(url, *it) != 0)\n"
                  "  {\n"
                  "    log_info(\"binary file \\\"\" << url << \"\\\" not found\");\n"
                  "    return DECLINED;\n"
                  "  }\n"
                  "  unsigned url_idx = it - urls_begin;\n\n"

                  "  reply.setKeepAliveHeader();\n"
                  "  reply.setContentType(mimetypes[url_idx]);\n\n"

                  "  std::string s = request.getHeader(tnt::httpheader::ifModifiedSince);\n"
                  "  if (s == url_c_time[url_idx])\n"
                  "    return HTTP_NOT_MODIFIED;\n\n"

                  "  reply.setHeader(tnt::httpheader::lastModified, url_c_time[url_idx]);\n"
                  "  reply.setContentLengthHeader(data.size(url_idx));\n"
                  "  reply.setDirectMode();\n"
                  "  reply.out() << data[url_idx];\n"
                  "  return HTTP_OK;\n"
                  "}\n\n";
        }

        if (haveCloseComp)
          closeComp.getDefinition(code, externData, linenumbersEnabled);

        if (!attr.empty())
        {
          code << "// <%attr>\n"
                  "std::string _component_::getAttribute(const std::string& name, const std::string& def) const\n"
                  "{\n";
          for (attr_type::const_iterator it = attr.begin();
               it != attr.end(); ++it)
            code << "  if (name == \"" << it->first << "\")\n"
                    "    return " << it->second << ";\n";
          code << "  return def;\n"
                  "} // </%attr>\n\n";
        }

        for (subcomps_type::const_iterator i = subcomps.begin();
             i != subcomps.end(); ++i)
          i->getDefinition(code, externData, linenumbersEnabled);
      }
    }

    void Generator::printLine(std::ostream& out) const
    {
      if (linenumbersEnabled)
        out << "#line " << (curline + 1) << " \"" << curfile << "\"\n";
    }

    void Generator::getCpp(std::ostream& code, const std::string& filename) const
    {
      getIntro(code, filename);

      getHeaderIncludes(code);
      getCppIncludes(code);

      if (multiImages.empty() && !isRawMode())
        code << "log_define(\"" << maincomp.getLogCategory() << "\")\n\n";

      getPre(code);

      code << "\n" 
              "namespace\n"
              "{\n";

      getClassDeclaration(code);

      getCppBody(code);

      code << "} // namespace\n";
    }
  }
}
