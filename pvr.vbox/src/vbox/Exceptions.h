/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <stdexcept>

namespace vbox
{
  // Base exception class
  class VBoxException : public std::runtime_error
  {
  public:
    VBoxException(const std::string& message) : std::runtime_error(message) {}
  };

  // Domain-specific exceptions
  class InvalidXMLException : public VBoxException
  {
  public:
    InvalidXMLException(const std::string& message) : VBoxException(message){};
  };

  class InvalidResponseException : public VBoxException
  {
  public:
    InvalidResponseException(const std::string& message) : VBoxException(message){};
  };

  class RequestFailedException : public VBoxException
  {
  public:
    RequestFailedException(const std::string& message) : VBoxException(message){};
  };

  class FirmwareVersionException : public VBoxException
  {
  public:
    FirmwareVersionException(const std::string& message) : VBoxException(message){};
  };
} // namespace vbox
