/*
 *  Copyright (C) 2015-2020 Team Kodi
 *  Copyright (C) 2015 Sam Stenvall
 *  https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "Content.h"

#include "../../vbox/VBox.h"
#include "../../xmltv/Guide.h"
#include "../../xmltv/Utilities.h"
#include "../Channel.h"

#include <lib/tinyxml2/tinyxml2.h>

using namespace tinyxml2;
using namespace vbox;
using namespace vbox::response;

std::string Content::GetString(const std::string& parameter) const
{
  const XMLElement* element = GetParameterElement(parameter);

  if (element)
    return xmltv::Utilities::GetStdString(element->GetText());

  return "";
}

int Content::GetInteger(const std::string& parameter) const
{
  int value = 0;

  const XMLElement* element = GetParameterElement(parameter);
  if (element)
    value = xmltv::Utilities::QueryIntText(element);

  return value;
}

unsigned int Content::GetUnsignedInteger(const std::string& parameter) const
{
  unsigned int value = 0;

  XMLElement* element = GetParameterElement(parameter);
  if (element)
    value = xmltv::Utilities::QueryUnsignedText(element);

  return value;
}

tinyxml2::XMLElement* Content::GetParameterElement(const std::string& parameter) const
{
  return m_content->FirstChildElement(parameter.c_str());
}

std::vector<ChannelPtr> XMLTVResponseContent::GetChannels() const
{
  std::vector<ChannelPtr> channels;

  // Remember the index each channel had, it's needed for certain API requests
  unsigned int index = 1;

  for (XMLElement* element = m_content->FirstChildElement("channel"); element != NULL;
       element = element->NextSiblingElement("channel"))
  {
    ChannelPtr channel = CreateChannel(element);
    channel->m_index = index++;
    channels.push_back(channel);
  }

  return channels;
}

::xmltv::Guide XMLTVResponseContent::GetGuide() const
{
  return ::xmltv::Guide(m_content);
}

ChannelPtr XMLTVResponseContent::CreateChannel(const tinyxml2::XMLElement* xml) const
{
  // Extract data from the various <display-name> elements
  const XMLElement* displayElement = xml->FirstChildElement("display-name");
  std::string name(xmltv::Utilities::GetStdString(displayElement->GetText()));
  displayElement = displayElement->NextSiblingElement("display-name");
  std::string type(xmltv::Utilities::GetStdString(displayElement->GetText()));
  displayElement = displayElement->NextSiblingElement("display-name");
  std::string uniqueId(xmltv::Utilities::GetStdString(displayElement->GetText()));
  displayElement = displayElement->NextSiblingElement("display-name");
  std::string encryption(xmltv::Utilities::GetStdString(displayElement->GetText()));
  std::string xmltvName = ::xmltv::Utilities::UrlDecode(xml->Attribute("id"));

  // Create the channel with some basic information
  ChannelPtr channel(new Channel(uniqueId, xmltvName, name, xml->FirstChildElement("url")->Attribute("src")));

  // Extract the LCN (optional)
  displayElement = displayElement->NextSiblingElement("display-name");

  if (displayElement)
  {
    // The LCN is sometimes just a digit and sometimes lcn_X
    std::string lcnValue(xmltv::Utilities::GetStdString(displayElement->GetText()));

    if (lcnValue.find("lcn_") != std::string::npos)
      lcnValue = lcnValue.substr(4);

    channel->m_number = static_cast<unsigned int>(std::stoi(lcnValue));
  }

  // Set icon URL if it exists
  const char* iconUrl = xml->FirstChildElement("icon")->Attribute("src");
  if (iconUrl != NULL)
    channel->m_iconUrl = iconUrl;

  // Set radio and encryption status
  channel->m_radio = type == "Radio";
  channel->m_encrypted = encryption == "Encrypted";

  return channel;
}

std::vector<RecordingPtr> RecordingResponseContent::GetRecordings() const
{
  std::vector<RecordingPtr> recordings;

  for (XMLElement* element = m_content->FirstChildElement("record"); element != NULL;
       element = element->NextSiblingElement("record"))
  {
    RecordingPtr recording = CreateRecording(element);
    recordings.push_back(std::move(recording));
  }

  return recordings;
}

std::vector<SeriesRecordingPtr> RecordingResponseContent::GetSeriesRecordings() const
{
  std::vector<SeriesRecordingPtr> allSeries;

  for (XMLElement* element = m_content->FirstChildElement("record-series"); element != NULL;
       element = element->NextSiblingElement("record-series"))
  {
    SeriesRecordingPtr series = CreateSeriesRecording(element);
    allSeries.push_back(std::move(series));
  }

  return allSeries;
}

RecordingPtr RecordingResponseContent::CreateRecording(const tinyxml2::XMLElement* xml) const
{
  // Extract mandatory properties
  std::string channelId = xmltv::Utilities::UrlDecode(xmltv::Utilities::GetStdString(xml->Attribute("channel")));

  const XMLElement* element = xml->FirstChildElement("channel-name");
  if (!element)
    return nullptr;

  std::string channelName = xmltv::Utilities::GetStdString(element->GetText());

  element = xml->FirstChildElement("state");
  if (!element)
    return nullptr;

  RecordingState state = GetState(xmltv::Utilities::GetStdString(element->GetText()));

  // Construct the object
  RecordingPtr recording(new Recording(channelId, channelName, state));

  // Add additional properties
  recording->m_startTime = xmltv::Utilities::GetStdString(xml->Attribute("start"));

  element = xml->FirstChildElement("record-id");
  if (element)
    recording->m_id = xmltv::Utilities::QueryUnsignedText(element);

  element = xml->FirstChildElement("series-id");
  if (element)
    recording->m_seriesId = xmltv::Utilities::QueryUnsignedText(element);

  // TODO: External recordings don't have an end time, default to one hour
  if (xml->Attribute("stop") != NULL)
    recording->m_endTime = xmltv::Utilities::GetStdString(xml->Attribute("stop"));
  else
    recording->m_endTime = xmltv::Utilities::UnixTimeToXmltv(time(nullptr) + 86400);

  std::time_t now = std::time(nullptr);
  std::time_t startTime = xmltv::Utilities::XmltvToUnixTime(recording->m_startTime);
  std::time_t endTime = xmltv::Utilities::XmltvToUnixTime(recording->m_endTime);
  if (startTime > now && now < endTime)
    recording->m_duration = static_cast<int>(now - startTime);
  else
    recording->m_duration = static_cast<int>(endTime - startTime);

  element = xml->FirstChildElement("programme-title");
  if (element)
    recording->m_title = xmltv::Utilities::GetStdString(element->GetText());
  else
  {
    // TODO: Some recordings don't have a name, especially external ones
    if (state == RecordingState::EXTERNAL)
      recording->m_title = "External recording (channel " + channelName + ")";
    else
      recording->m_title = "Unnamed recording (channel " + channelName + ")";
  }

  // Some recordings may have certain tags, but they can be empty
  element = xml->FirstChildElement("programme-desc");
  if (element)
    recording->m_description = xmltv::Utilities::GetStdString(element->GetText());
  element = xml->FirstChildElement("url");
  if (element)
    recording->m_url = xmltv::Utilities::GetStdString(element->GetText());

  // Extract the "local target" (filename), it is needed on rare occasions
  element = xml->FirstChildElement("LocalTarget");
  if (element)
    recording->m_filename = xmltv::Utilities::GetStdString(element->GetText());

  return recording;
}

static void AddWeekdayBits(unsigned int& rWeekdays, const char* pWeekdaysText)
{
  static unsigned int days[7] = {PVR_WEEKDAY_SUNDAY,   PVR_WEEKDAY_MONDAY, PVR_WEEKDAY_TUESDAY, PVR_WEEKDAY_WEDNESDAY,
                                 PVR_WEEKDAY_THURSDAY, PVR_WEEKDAY_FRIDAY, PVR_WEEKDAY_SATURDAY};
  unsigned int dayInWeek = 0;
  char* pDay;
  char buf[32];

  strncpy(buf, pWeekdaysText, sizeof(buf) - 1);
  pDay = strtok(buf, ",");

  while (pDay)
  {
    dayInWeek = atoi(pDay);
    if (dayInWeek < 1 || dayInWeek > 7)
      continue;
    rWeekdays |= days[dayInWeek - 1];
    pDay = strtok(NULL, ",");
  }
}

SeriesRecordingPtr RecordingResponseContent::CreateSeriesRecording(const tinyxml2::XMLElement* xml) const
{
  // Extract mandatory properties
  std::string channelId = xmltv::Utilities::UrlDecode(xmltv::Utilities::GetStdString(xml->Attribute("channel")));

  // Construct the object
  SeriesRecordingPtr series(new SeriesRecording(channelId));

  series->m_id = atoi(xmltv::Utilities::GetStdString(xml->Attribute("series-id")).c_str());
  const XMLElement* element = xml->FirstChildElement("schedule-record-id");

  if (element)
    series->m_scheduledId = xmltv::Utilities::QueryIntText(element);

  element = xml->FirstChildElement("programme-title");
  if (element)
    series->m_title = xmltv::Utilities::GetStdString(element->GetText());
  else

    // Some recordings may have certain tags, but they can be empty
    element = xml->FirstChildElement("programme-desc");
  if (element)
    series->m_description = xmltv::Utilities::GetStdString(element->GetText());

  element = xml->FirstChildElement("start");
  if (element)
    series->m_startTime = xmltv::Utilities::GetStdString(element->GetText());

  element = xml->FirstChildElement("crid");

  if (element && element->GetText())
    series->m_fIsAuto = true;
  else
  {
    element = xml->FirstChildElement("stop");
    if (element)
      series->m_endTime = xmltv::Utilities::GetStdString(element->GetText());

    element = xml->FirstChildElement("days-in-week");
    // add day-bits to m_weekdays according to the days in the "days-in-week" tag
    if (element)
      AddWeekdayBits(series->m_weekdays, xmltv::Utilities::GetStdString(element->GetText()).c_str());
  }

  return series;
}


RecordingState RecordingResponseContent::GetState(const std::string& state) const
{
  if (state == "recorded")
    return RecordingState::RECORDED;
  else if (state == "recording")
    return RecordingState::RECORDING;
  else if (state == "scheduled")
    return RecordingState::SCHEDULED;
  else if (state == "Error")
    return RecordingState::RECORDING_ERROR;
  else
    return RecordingState::EXTERNAL;
}
