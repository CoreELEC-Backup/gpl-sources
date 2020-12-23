/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "Request.h"

#include <map>
#include <string>
#include <vector>

namespace vbox
{
  namespace request
  {

    /**
     * Represents an API request
     */
    class ApiRequest : public Request
    {
    public:
      ApiRequest(const std::string& method, const std::string& hostname, int upnpPort);
      virtual ~ApiRequest(){};

      /**
       * Adds a request parameter with the specified value
       *
       * @param name  The name.
       * @param value The value.
       */
      void AddParameter(const std::string& name, const std::string& value);
      void AddParameter(const std::string& name, int value);
      void AddParameter(const std::string& name, unsigned int value);

      void SetTimeout(int timeout);

      virtual vbox::response::ResponseType GetResponseType() const override;
      virtual std::string GetLocation(std::string url) const override;
      virtual std::string GetIdentifier() const override;

    private:
      /**
       * The method name
       */
      std::string m_method;

      /**
       * The request parameters (and their values)
       */
      std::map<std::string, std::vector<std::string>> m_parameters;

      /**
       * The timeout to use for the request. Defaults to zero which means the
       * default underlying systems timeout is used.
       */
      int m_timeout;

      /**
       * List of methods that can take an optional "ExternalIP" parameter
       */
      static const std::vector<std::string> externalCapableMethods;

      /**
       * List of methods that return XMLTV responses
       */
      static const std::vector<std::string> xmltvMethods;
    };
  } // namespace request
} // namespace vbox
