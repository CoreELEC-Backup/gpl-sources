/*
 *  Copyright (C) 2005-2020 Team Kodi
 *  Copyright (C) 2010 Marcel Groothuis
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "rest.h"

#include <kodi/Filesystem.h>
#include <kodi/General.h>

int cRest::Get(const std::string& command, const std::string& arguments, Json::Value& json_response)
{
  std::string response;
  int retval;
  retval = httpRequest(command, arguments, false, response);

  if (retval != E_FAILED)
  {
    if (response.length() != 0)
    {
      std::string jsonReaderError;
      Json::CharReaderBuilder jsonReaderBuilder;
      std::unique_ptr<Json::CharReader> const reader(jsonReaderBuilder.newCharReader());

      if (!reader->parse(response.c_str(), response.c_str() + response.size(), &json_response,
                         &jsonReaderError))
      {
        kodi::Log(ADDON_LOG_DEBUG, "Failed to parse %s: \n%s\n", response.c_str(),
                  jsonReaderError.c_str());
        return E_FAILED;
      }
    }
    else
    {
      kodi::Log(ADDON_LOG_DEBUG, "Empty response");
      return E_EMPTYRESPONSE;
    }
  }

  return retval;
}

int cRest::Post(const std::string& command,
                const std::string& arguments,
                Json::Value& json_response)
{
  std::string response;
  int retval;
  retval = httpRequest(command, arguments, true, response);

  if (retval != E_FAILED)
  {
    if (response.length() != 0)
    {
      std::string jsonReaderError;
      Json::CharReaderBuilder jsonReaderBuilder;
      std::unique_ptr<Json::CharReader> const reader(jsonReaderBuilder.newCharReader());

      if (!reader->parse(response.c_str(), response.c_str() + response.size(), &json_response,
                         &jsonReaderError))
      {
        kodi::Log(ADDON_LOG_DEBUG, "Failed to parse %s: \n%s\n", response.c_str(),
                  jsonReaderError.c_str());
        return E_FAILED;
      }
    }
    else
    {
      kodi::Log(ADDON_LOG_DEBUG, "Empty response");
      return E_EMPTYRESPONSE;
    }
  }

  return retval;
}

int httpRequest(const std::string& command,
                const std::string& arguments,
                const bool write,
                std::string& json_response)
{
  std::string strUrl = command;

  if (write)
  { // POST http request
    kodi::vfs::CFile file;
    if (file.OpenFileForWrite(strUrl, 0))
    {
      int rc = file.Write(arguments.c_str(), arguments.length());
      if (rc >= 0)
      {
        std::string result;
        result.clear();
        std::string buffer;
        while (file.ReadLine(buffer))
          result.append(buffer);
        json_response = result;
        return 0;
      }
    }
  }
  else
  { // GET http request
    strUrl += arguments;
    kodi::vfs::CFile file;
    if (file.OpenFile(strUrl, 0))
    {
      std::string result;
      std::string buffer;
      while (file.ReadLine(buffer))
        result.append(buffer);
      json_response = result;
      return 0;
    }
  }

  return -1;
}
