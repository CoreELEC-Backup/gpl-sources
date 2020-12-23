/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "Response.h"

#include "../../xmltv/Utilities.h"
#include "../Exceptions.h"

using namespace tinyxml2;
using namespace vbox::response;

Response::Response()
{
  // Some XMLTV files have weird line endings, try to account for that
  m_document =
      std::unique_ptr<XMLDocument>(new XMLDocument(/*processEntities = */ true, tinyxml2::PRESERVE_WHITESPACE));

  m_error.code = ErrorCode::SUCCESS;
  m_error.description = "";
}

Response::~Response()
{
}

void Response::ParseRawResponse(const std::string& rawResponse)
{
  // Try to parse the response as XML
  if (m_document->Parse(rawResponse.c_str(), rawResponse.size()) != XML_NO_ERROR)
    throw vbox::InvalidXMLException("XML parsing failed: " + std::string(m_document->ErrorName()));

  // Parse the response status
  ParseStatus();
}

void Response::ParseStatus()
{
  int errorCode;
  std::string errorDescription;

  XMLNode* rootElement = m_document->RootElement();
  XMLElement* statusElement = rootElement->FirstChildElement(GetStatusElementName().c_str());

  // Not all response types always return the status element
  if (statusElement)
  {
    XMLElement* errCodeEl = statusElement->FirstChildElement("ErrorCode");
    XMLElement* errDescEl = statusElement->FirstChildElement("ErrorDescription");

    if (errCodeEl)
    {
      errorCode = xmltv::Utilities::QueryIntText(errCodeEl);
      m_error.code = static_cast<ErrorCode>(errorCode);
    }

    if (errDescEl)
    {
      errorDescription = xmltv::Utilities::GetStdString(errDescEl->GetText());
      m_error.description = errorDescription;
    }
  }
}

XMLElement* Response::GetReplyElement() const
{
  XMLNode* rootElement = m_document->RootElement();
  return rootElement->FirstChildElement("Reply");
}

XMLElement* XMLTVResponse::GetReplyElement() const
{
  return m_document->RootElement();
}

XMLElement* RecordingResponse::GetReplyElement() const
{
  return m_document->RootElement();
}
