/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include "../response/Response.h"

#include <string>

namespace vbox
{
  namespace request
  {

    /**
     * Interface for requests
     */
    class Request
    {
    public:
      virtual ~Request() {};

      /**
       * @return the type of response this request leads to
       */
      virtual response::ResponseType GetResponseType() const = 0;

      /**
       * @return the request location
       */
      virtual std::string GetLocation(std::string url) const = 0;

      /**
       * @return an identifier for this request (mainly for logging purposes)
       */
      virtual std::string GetIdentifier() const = 0;
    };
  } // namespace request
} // namespace vbox
