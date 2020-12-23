/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <ctime>
#include <string>
#include <vector>

namespace tinyxml2
{
  class XMLElement;
}

namespace xmltv
{
  class Utilities
  {
  public:
    /**
      * The XMLTV datetime format string
      */
    static const char* XMLTV_DATETIME_FORMAT;

    /**
     * The XMLTV datetime timezone offset format
     */
    static const char* XMLTV_TIMEZONE_OFFSET_FORMAT;

    /**
     * Returns the timezone offset part of the specified XMLTV timestamp
     * @param timestamp an XMLTV timestamp
     * @return the timezone offset, e.g. "+0200", or an empty string if no
     * timezone offset could be parsed.
     */
    static std::string GetTimezoneOffset(const std::string timestamp);

    /**
     * Converts the specified timezone offset into the number of seconds it
     * differs from UTC
     * @param tzOffset the timezone offset, e.g. "+0200"
     * @return the number of seconds to adjust
     */
    static int GetTimezoneAdjustment(const std::string tzOffset);

    /**
      * Converts an XMLTV datetime string into a UTC UNIX timestamp
      * @param time e.g. "20120228001500+0200"
      * @return a UTC UNIX timestamp
      */
    static time_t XmltvToUnixTime(const std::string& time);

    /**
      * Converts a UTC time_t to an XMLTV datetime string, optionally adjusted
      * for the specified timezone offset
      * @param time a UNIX timestamp
      * @param tzOffset the timezone offset, e.g. "+0200"
      * @return e.g. "20120228001500+0200"
      */
    static std::string UnixTimeToXmltv(const time_t timestamp, const std::string tzOffset = "");

    /**
    * Converts a UTC time_t a 24-hour hhmm string, optionally adjusted
    * for the specified timezone offset
    * @param time a UNIX timestamp
    * @param tzOffset the timezone offset, e.g. "+0200"
    * @return e.g. "1700"
    */
    static std::string UnixTimeToDailyTime(const time_t timestamp, const std::string tzOffset = "");

    /**
     * Parses the contents of the specified element into an integer. We need
     * this for backward-compatibility with older versions of tinyxml2.
     */
    static int QueryIntText(const tinyxml2::XMLElement* element);

    /**
     * Parses the contents of the specified element into an unsigned integer.
     * We need this for backward-compatibility with older versions of tinyxml2.
     */
    static unsigned int QueryUnsignedText(const tinyxml2::XMLElement* element);

    /**
     * URL-encodes the specified string
     *
     * @param name the string to encode
     * @return the encoded string
     */
    static std::string UrlEncode(const std::string& string);

    /**
      * Decodes the specified URL
      *
      * @param name the string to decode
      * @return the decoded string
      */
    static std::string UrlDecode(const std::string& string);

    /**
     * Concatenates the contents of vector with the specified separator
     * @param vector the vector
     * @param separator the separator
     * @return a concatenated string
     */
    static std::string ConcatenateStringList(const std::vector<std::string>& vector,
                                             const std::string& separator = ", ");

    /**
    * Validates the given text (if exists) and returns it as a valid std::string
    * @cStr the c-style string
    * @return value is "" if cStr is NULL, and an std::string of the cStr
    */
    static std::string GetStdString(const char* cStr)
    {
      if (cStr)
        return cStr;
      return "";
    }
  };
} // namespace xmltv
