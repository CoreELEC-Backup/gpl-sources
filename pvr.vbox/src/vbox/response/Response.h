/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <memory>
#include <string>

#include <lib/tinyxml2/tinyxml2.h>

namespace vbox
{
  namespace response
  {

    class Response;
    typedef std::unique_ptr<Response> ResponsePtr;

    /**
     * The various response types (indicates what kind of content the response
     * will contain)
     */
    enum ResponseType
    {
      GENERIC = 0,
      XMLTV,
      RECORDS
    };

    /**
     * The possible error codes a response can have
     */
    enum ErrorCode
    {
      SUCCESS = 0,
      UNKNOWN_METHOD,
      GENERAL_ERROR,
      MISSING_PARAMETER,
      ILLEGAL_PARAMETER,
      REQUEST_REJECTED,
      MISSING_METHOD,
      REQUEST_TIMEOUT,
      REQUEST_ABORTED,
    };

    /**
     * Represents an error
     */
    struct Error
    {
      ErrorCode code;
      std::string description;
    };

    /**
     * Represents an API response
     * TODO: Factor out to interface and something like a BasicResponse implementation
     */
    class Response
    {
    public:
      Response();
      virtual ~Response();

      /**
       * Move constructor. It transfers the ownership of
       * the underlying XML document.
       */
      Response(Response&& other)
      {
        if (this != &other)
          m_document = std::move(other.m_document);
      }

      /**
       * Parses the raw XML response
       * @param rawResponse The raw response.
       */
      void ParseRawResponse(const std::string& rawResponse);

      /**
       * @return whether the response was successful
       */
      bool IsSuccessful() const { return GetErrorCode() == ErrorCode::SUCCESS; }

      /**
       * @return the error code
       */
      ErrorCode GetErrorCode() const { return m_error.code; }

      /**
       * @return the error description
       */
      std::string GetErrorDescription() const { return m_error.description; }

      /**
       * Returns the portion of the XML response that represents the actual
       * response content
       * @return pointer to the reply element
       */
      virtual tinyxml2::XMLElement* GetReplyElement() const;

    protected:
      /**
       * Returns the name of the element that represents the request status. The
       * element name varies slightly between different response types so it
       * may be overriden
       * @return the element name
       */
      virtual std::string GetStatusElementName() const { return "Status"; }

      /**
      * The underlying XML response document
      */
      std::unique_ptr<tinyxml2::XMLDocument> m_document;

    private:
      /**
       * Parses the response status for possible errors
       */
      void ParseStatus();

      /**
       * The response error
       */
      Error m_error;
    };

    /**
     * Represents a response that returns XMLTV data
     */
    class XMLTVResponse : public Response
    {
    public:
      virtual tinyxml2::XMLElement* GetReplyElement() const override;

    protected:
      virtual std::string GetStatusElementName() const override { return "Error"; }
    };

    /**
    * Represents a response that returns data about recordings and timers
    */
    class RecordingResponse : public Response
    {
    public:
      virtual tinyxml2::XMLElement* GetReplyElement() const override;

    protected:
      virtual std::string GetStatusElementName() const override { return "Error"; }
    };
  } // namespace response
} // namespace vbox
