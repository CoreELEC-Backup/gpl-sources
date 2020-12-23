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

#include <string>

namespace vbox
{
  namespace request
  {

    /**
     * Represents a local file request
     */
    class FileRequest : public Request
    {
    public:
      FileRequest(const std::string& path) : m_path(path) {}
      virtual ~FileRequest() {}

      virtual response::ResponseType GetResponseType() const override
      {
        // Currently we always expect local files to contain XMLTV
        return response::ResponseType::XMLTV;
      }

      virtual std::string GetLocation(std::string url) const override { return m_path; }

      virtual std::string GetIdentifier() const override { return "FileRequest for \"" + m_path + "\""; }

    private:
      std::string m_path;
    };
  } // namespace request
} // namespace vbox
