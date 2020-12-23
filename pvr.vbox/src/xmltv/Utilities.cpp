/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "Utilities.h"

#include <algorithm>
#include <iomanip>
#include <iterator>
#include <sstream>
#include <stdexcept>

#include <lib/tinyxml2/tinyxml2.h>

using namespace xmltv;

const char* Utilities::XMLTV_DATETIME_FORMAT = "%Y%m%d%H%M%S";
const char* Utilities::XMLTV_TIMEZONE_OFFSET_FORMAT = "%03d%02d";

namespace
{

  // Adapted from https://stackoverflow.com/a/31533119

  // Conversion from UTC date to second, signed 64-bit adjustable epoch version.
  // Written by François Grieu, 2015-07-21; public domain.

  long long MakeTime(int year, int month, int day)
  {
    return static_cast<long long>(year) * 365 + year / 4 - year / 100 * 3 / 4 + (month + 2) * 153 / 5 + day;
  }

  long long GetUTCTime(int year, int mon, int mday, int hour, int min, int sec)
  {
    int m = mon - 1;
    int y = year + 100;

    if (m < 2)
    {
      m += 12;
      --y;
    }

    return (((MakeTime(y, m, mday) - MakeTime(1970 + 99, 12, 1)) * 24 + hour) * 60 + min) * 60 + sec;
  }

  long long ParseDateTime(const std::string& strDate)
  {
    int year = 2000;
    int mon = 1;
    int mday = 1;
    int hour = 0;
    int min = 0;
    int sec = 0;
    char offset_sign = '+';
    int offset_hours = 0;
    int offset_minutes = 0;

    std::sscanf(strDate.c_str(), "%04d%02d%02d%02d%02d%02d %c%02d%02d", &year, &mon, &mday, &hour, &min, &sec,
                &offset_sign, &offset_hours, &offset_minutes);

    long offset_of_date = (offset_hours * 60 + offset_minutes) * 60;
    if (offset_sign == '-')
      offset_of_date = -offset_of_date;

    return GetUTCTime(year, mon, mday, hour, min, sec) - offset_of_date;
  }

} // unnamed namespace

std::string Utilities::GetTimezoneOffset(const std::string timestamp)
{
  std::string xmltvTime = timestamp;
  std::string tzOffset = "";

  // Make sure the timestamp doesn't contain a space before the timezone offset
  xmltvTime.erase(std::remove_if(xmltvTime.begin(), xmltvTime.end(), isspace), xmltvTime.end());

  if (xmltvTime.length() > 14)
    tzOffset = xmltvTime.substr(14);

  return tzOffset;
}

int Utilities::GetTimezoneAdjustment(const std::string tzOffset)
{
  // Sanity check
  if (tzOffset.length() != 5)
    return 0;

  int hours = 0;
  int minutes = 0;

  sscanf(tzOffset.c_str(), XMLTV_TIMEZONE_OFFSET_FORMAT, &hours, &minutes);

  // Make minutes negative if hours is
  if (hours < 0)
    minutes = -minutes;

  return (hours * 3600) + (minutes * 60);
}

time_t Utilities::XmltvToUnixTime(const std::string& time)
{
  return static_cast<time_t>(ParseDateTime(time));
}

std::string Utilities::UnixTimeToXmltv(const time_t timestamp, const std::string tzOffset /* = ""*/)
{
  // Adjust the timestamp according to the timezone
  time_t adjustedTimestamp = timestamp + GetTimezoneAdjustment(tzOffset);

  // Format the timestamp
  std::tm tm = *std::gmtime(&adjustedTimestamp);

  char buffer[20];
  strftime(buffer, sizeof(buffer), XMLTV_DATETIME_FORMAT, &tm);

  std::string xmltvTime(buffer);

  // Append the timezone offset
  if (tzOffset.empty())
    xmltvTime += "+0000";
  else
    xmltvTime += tzOffset;

  return xmltvTime;
}

std::string Utilities::UnixTimeToDailyTime(const time_t timestamp, const std::string tzOffset /* = ""*/)
{
  // Adjust the timestamp according to the timezone
  time_t adjustedTimestamp = timestamp + GetTimezoneAdjustment(tzOffset);

  // Format the timestamp
  std::tm tm = *std::gmtime(&adjustedTimestamp);

  char buffer[20];
  strftime(buffer, sizeof(buffer), XMLTV_DATETIME_FORMAT, &tm);

  std::string xmltvTime(buffer);
  // hours start after yyyymmdd (8 chars), minutes after hh
  return xmltvTime.substr(8, 2) + xmltvTime.substr(10, 2);
}

// Borrowed from https://github.com/xbmc/xbmc/blob/master/xbmc/URL.cpp
std::string Utilities::UrlDecode(const std::string& strURLData)
{
  std::string strResult;
  /* result will always be less than source */
  strResult.reserve(strURLData.length());
  for (unsigned int i = 0; i < strURLData.size(); ++i)
  {
    int kar = static_cast<unsigned char>(strURLData[i]);
    if (kar == '+')
    {
      strResult += ' ';
    }
    else if (kar == '%')
    {
      if (i < strURLData.size() - 2)
      {
        std::string strTmp;
        strTmp.assign(strURLData.substr(i + 1, 2));
        int dec_num = -1;
        sscanf(strTmp.c_str(), "%x", reinterpret_cast<unsigned int*>(&dec_num));
        if (dec_num < 0 || dec_num > 255)
          strResult += kar;
        else
        {
          strResult += static_cast<char>(dec_num);
          i += 2;
        }
      }
      else
        strResult += kar;
    }
    else
    {
      strResult += kar;
    }
  }
  return strResult;
}

int Utilities::QueryIntText(const tinyxml2::XMLElement* element)
{
  int value = 0;

  if (element->GetText())
  {
    try
    {
      const char* pText = element->GetText();
      if (!pText)
        throw std::invalid_argument("No text in element");
      std::string content = pText;
      value = std::stoi(content);
    }
    catch (std::invalid_argument)
    {
    }
  }

  return value;
}

unsigned int Utilities::QueryUnsignedText(const tinyxml2::XMLElement* element)
{
  unsigned int value = 0;

  if (element->GetText())
  {
    try
    {
      const char* pText = element->GetText();
      if (!pText)
        throw std::invalid_argument("No text in element");
      std::string content = pText;
      value = static_cast<unsigned int>(std::stoi(content));
    }
    catch (std::invalid_argument)
    {
    }
  }

  return value;
}

// Adapted from http://stackoverflow.com/a/17708801
std::string Utilities::UrlEncode(const std::string& value)
{
  std::ostringstream escaped;
  escaped.fill('0');
  escaped << std::hex;

  for (auto i = value.cbegin(), n = value.cend(); i != n; ++i)
  {
    std::string::value_type c = (*i);

    // Keep alphanumeric and other accepted characters intact
    if (c < 0 || isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
    {
      escaped << c;
      continue;
    }

    // Any other characters are percent-encoded
    escaped << '%' << std::setw(2) << int(static_cast<unsigned char>(c));
  }

  return escaped.str();
}

// Borrowed from http://stackoverflow.com/a/8581865
std::string Utilities::ConcatenateStringList(const std::vector<std::string>& vector,
                                             const std::string& separator /* = ", "*/)
{
  std::ostringstream oss;

  if (!vector.empty())
  {
    std::copy(vector.begin(), vector.end() - 1, std::ostream_iterator<std::string>(oss, separator.c_str()));

    oss << vector.back();
  }

  return oss.str();
}
