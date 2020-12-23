/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "ApiRequest.h"

#include "../../xmltv/Utilities.h"

#include <algorithm>

using namespace vbox::request;
using vbox::response::ResponseType;

const std::vector<std::string> ApiRequest::externalCapableMethods = {
    "GetXmltvEntireFile",
    "GetXmltvSection",
    "GetXmltvChannelsList",
    "GetXmltvProgramsList",
    "GetRecordsList"
};

const std::vector<std::string> ApiRequest::xmltvMethods = {
    "GetXmltvEntireFile",
    "GetXmltvSection",
    "GetXmltvChannelsList",
    "GetXmltvProgramsList",
};

ApiRequest::ApiRequest(const std::string& method, const std::string& hostname, int upnpPort) : m_method(method), m_timeout(0)
{
  AddParameter("Method", method);

  // Add external IP and port options to the methods that support it
  if (std::find(externalCapableMethods.cbegin(), externalCapableMethods.cend(), method) !=
      externalCapableMethods.cend())
  {
    AddParameter("ExternalIP", hostname);
    AddParameter("Port", upnpPort);
  }
}

ResponseType ApiRequest::GetResponseType() const
{
  // Determine the response type based on the method name
  if (std::find(xmltvMethods.cbegin(), xmltvMethods.cend(), m_method) != xmltvMethods.cend())
    return ResponseType::XMLTV;
  else if (m_method == "GetRecordsList")
    return ResponseType::RECORDS;

  return ResponseType::GENERIC;
}

std::string ApiRequest::GetLocation(std::string url) const
{
  // Append parameters (including method)
  if (!m_parameters.empty())
  {
    for (auto const& parameter : m_parameters)
    {
      // multiple values possible for each parameter
      for (auto const& value : parameter.second)
      {
        url += "&" + parameter.first + "=";
        url += ::xmltv::Utilities::UrlEncode(value);
      }
    }
  }

  // Optionally append the connection timeout
  if (m_timeout > 0)
    url += "|connection-timeout=" + std::to_string(m_timeout);

  return url;
}

std::string ApiRequest::GetIdentifier() const
{
  return m_method;
}

void ApiRequest::AddParameter(const std::string& name, const std::string& value)
{
  m_parameters[name].push_back(value);
}

void ApiRequest::AddParameter(const std::string& name, int value)
{
  m_parameters[name].push_back(std::to_string(value));
}

void ApiRequest::AddParameter(const std::string& name, unsigned int value)
{
  m_parameters[name].push_back(std::to_string(value));
}

void ApiRequest::SetTimeout(int timeout)
{
  m_timeout = timeout;
}
