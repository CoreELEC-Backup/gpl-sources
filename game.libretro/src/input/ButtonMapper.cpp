/*
 *  Copyright (C) 2015-2020 Team Kodi (https://kodi.tv)
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "ButtonMapper.h"
#include "DefaultControllerTranslator.h"
#include "DefaultKeyboardTranslator.h"
#include "InputDefinitions.h"
#include "LibretroDevice.h"
#include "libretro/LibretroDLL.h"
#include "libretro/LibretroEnvironment.h"
#include "libretro/LibretroTranslator.h"
#include "libretro/libretro.h"
#include "log/Log.h"

#include <tinyxml.h>

#include <algorithm>
#include <sstream>

#define BUTTONMAP_XML          "buttonmap.xml"
#define BUTTONMAP_XML_VERSION      2
#define BUTTONMAP_XML_MIN_VERSION  2

using namespace LIBRETRO;

CButtonMapper::CButtonMapper(void)
  : m_bLoadAttempted(false)
{
}

CButtonMapper& CButtonMapper::Get(void)
{
  static CButtonMapper _instance;
  return _instance;
}

bool CButtonMapper::LoadButtonMap(void)
{
  bool bSuccess = false;

  m_devices.clear();

  std::string strFilename = CLibretroEnvironment::Get().GetResourcePath(BUTTONMAP_XML);
  if (strFilename.empty())
  {
    esyslog("Could not locate buttonmap \"%s\"", BUTTONMAP_XML);
  }
  else
  {
    dsyslog("Loading libretro buttonmap %s", strFilename.c_str());

    TiXmlDocument buttonMapXml;
    if (!buttonMapXml.LoadFile(strFilename))
    {
      esyslog("Failed to open file: %s (line %d)", buttonMapXml.ErrorDesc(), buttonMapXml.ErrorRow());
    }
    else
    {
      TiXmlElement* pRootElement = buttonMapXml.RootElement();
      bSuccess = Deserialize(pRootElement);
    }
  }

  return bSuccess;
}

libretro_device_t CButtonMapper::GetLibretroType(const std::string& strControllerId)
{
  // Handle default controller unless it appears in buttonmap.xml
  if (strControllerId == DEFAULT_CONTROLLER_ID && !HasController(DEFAULT_CONTROLLER_ID))
    return RETRO_DEVICE_ANALOG;

  // Handle default keyboard unless it appears in buttonmap.xml
  if (strControllerId == DEFAULT_KEYBOARD_ID && !HasController(DEFAULT_KEYBOARD_ID))
    return RETRO_DEVICE_KEYBOARD;

  libretro_device_t deviceType = RETRO_DEVICE_NONE;

  // Check buttonmap for other controllers
  DeviceIt it = GetDevice(m_devices, strControllerId);
  if (it != m_devices.end())
    deviceType = (*it)->Type();

  return deviceType;
}

libretro_subclass_t CButtonMapper::GetSubclass(const std::string& strControllerId)
{
  // Handle default controller unless it appears in buttonmap.xml
  if (strControllerId == DEFAULT_CONTROLLER_ID && !HasController(DEFAULT_CONTROLLER_ID))
    return RETRO_SUBCLASS_NONE;

  // Handle default keyboard unless it appears in buttonmap.xml
  if (strControllerId == DEFAULT_KEYBOARD_ID && !HasController(DEFAULT_KEYBOARD_ID))
    return RETRO_SUBCLASS_NONE;

  libretro_subclass_t subclass = RETRO_SUBCLASS_NONE;

  // Check buttonmap for other controllers
  DeviceIt it = GetDevice(m_devices, strControllerId);
  if (it != m_devices.end())
    subclass = (*it)->Subclass();

  return subclass;
}

int CButtonMapper::GetLibretroIndex(const std::string& strControllerId, const std::string& strFeatureName)
{
  if (!strControllerId.empty() && !strFeatureName.empty())
  {
    // Handle default controller unless it appears in buttonmap.xml
    if (strControllerId == DEFAULT_CONTROLLER_ID && !HasController(DEFAULT_CONTROLLER_ID))
      return CDefaultControllerTranslator::GetLibretroIndex(strFeatureName);

    // Handle default keyboard unless it appears in buttonmap.xml
    if (strControllerId == DEFAULT_KEYBOARD_ID && !HasController(DEFAULT_KEYBOARD_ID))
      return CDefaultKeyboardTranslator::GetLibretroIndex(strFeatureName);

    // Check buttonmap for other controllers
    std::string mapto = GetFeature(strControllerId, strFeatureName);
    if (!mapto.empty())
      return LibretroTranslator::GetFeatureIndex(mapto);
  }

  return -1;
}

libretro_device_t CButtonMapper::GetLibretroDevice(const std::string& strControllerId, const std::string& strFeatureName) const
{
  if (!strControllerId.empty() && !strFeatureName.empty())
  {
    std::string mapto = GetFeature(strControllerId, strFeatureName);
    if (!mapto.empty())
      return LibretroTranslator::GetLibretroDevice(mapto);
  }

  return RETRO_DEVICE_NONE;
}

int CButtonMapper::GetAxisID(const std::string& strControllerId, const std::string& strFeatureName) const
{
  if (!strControllerId.empty() && !strFeatureName.empty())
  {
    std::string axis = GetAxis(strControllerId, strFeatureName);
    if (!axis.empty())
      return LibretroTranslator::GetAxisID(axis);
  }

  return -1;
}

std::string CButtonMapper::GetControllerFeature(const std::string& strControllerId, const std::string& strLibretroFeature)
{
  std::string feature;

  if (!strControllerId.empty() && !strLibretroFeature.empty())
  {
    // Handle default controller unless it appears in buttonmap.xml
    if (strControllerId == DEFAULT_CONTROLLER_ID && !HasController(DEFAULT_CONTROLLER_ID))
      return CDefaultControllerTranslator::GetControllerFeature(strLibretroFeature);

    // No need to handle keyboard, keyboard input doesn't go from libretro to frontend

    DeviceIt it = GetDevice(m_devices, strControllerId);
    if (it != m_devices.end())
    {
      const DevicePtr &device = *it;

      const FeatureMap& features = device->Features();
      for (auto& featurePair : features)
      {
        const std::string& controllerFeature = featurePair.first;
        const FeatureMapItem& libretroFeature = featurePair.second;

        if (libretroFeature.feature == strLibretroFeature)
        {
          feature = controllerFeature;
          break;
        }
      }
    }
  }

  return feature;
}

bool CButtonMapper::HasController(const std::string& strControllerId) const
{
  bool bFound = false;

  DeviceIt it = GetDevice(m_devices, strControllerId);
  if (it != m_devices.end())
    bFound = true;

  return bFound;
}

std::string CButtonMapper::GetFeature(const std::string& strControllerId, const std::string& strFeatureName) const
{
  std::string mapto;

  DeviceIt it = GetDevice(m_devices, strControllerId);
  if (it != m_devices.end())
  {
    const DevicePtr &device = *it;

    const FeatureMap& features = device->Features();
    for (auto& featurePair : features)
    {
      const std::string& controllerFeature = featurePair.first;
      const std::string& libretroFeature = featurePair.second.feature;

      if (controllerFeature == strFeatureName)
      {
        mapto = libretroFeature;
        break;
      }
    }
  }

  return mapto;
}

std::string CButtonMapper::GetAxis(const std::string& strControllerId, const std::string& strFeatureName) const
{
  std::string axis;

  for (auto& device : m_devices)
  {
    if (device->ControllerID() == strControllerId)
    {
      const FeatureMap& features = device->Features();
      for (auto& featurePair : features)
      {
        const std::string& controllerFeature = featurePair.first;
        const std::string& libretroAxis = featurePair.second.axis;

        if (controllerFeature == strFeatureName)
        {
          axis = libretroAxis;
          break;
        }
      }
      break;
    }
  }

  return axis;
}

bool CButtonMapper::Deserialize(TiXmlElement* pElement)
{
  bool bSuccess = false;

  if (pElement == nullptr ||
      pElement->ValueStr() != BUTTONMAP_XML_ROOT)
  {
    esyslog("Can't find root <%s> tag", BUTTONMAP_XML_ROOT);
  }
  else
  {
    // Assume v1 if unspecified
    unsigned int version = 1;

    const char* strVersion = pElement->Attribute(BUTTONMAP_XML_ATTR_VERSION);
    if (strVersion == nullptr)
    {
      esyslog("Buttonmap version required, expected version %u (min=%u)",
          BUTTONMAP_XML_VERSION, BUTTONMAP_XML_MIN_VERSION);
    }
    else
    {
      std::istringstream ss(strVersion);
      ss >> version;
      if (version < BUTTONMAP_XML_MIN_VERSION)
      {
        esyslog("Buttonmap with version %u too old, expected version %u (min=%u)",
            version, BUTTONMAP_XML_VERSION, BUTTONMAP_XML_MIN_VERSION);
      }
      else
      {
        dsyslog("Detected buttonmap version %u", version);
      }
    }

    if (version >= BUTTONMAP_XML_MIN_VERSION)
    {
      const TiXmlElement* pChild = pElement->FirstChildElement(BUTTONMAP_XML_ELM_CONTROLLER);
      if (!pChild)
      {
        esyslog("Can't find <%s> tag", BUTTONMAP_XML_ELM_CONTROLLER);
      }
      else
      {
        bSuccess = true;

        for ( ; pChild != nullptr; pChild = pChild->NextSiblingElement(BUTTONMAP_XML_ELM_CONTROLLER))
        {
          DevicePtr device(std::make_shared<CLibretroDevice>());

          if (!device->Deserialize(pChild, version))
          {
            bSuccess = false;
            break;
          }

          m_devices.emplace_back(std::move(device));
        }

        if (bSuccess)
          dsyslog("Loaded buttonmap at version %u", version);
      }
    }
  }

  return bSuccess;
}

CButtonMapper::DeviceIt CButtonMapper::GetDevice(const DeviceVector &devices,
                                                 const std::string &controllerId)
{
  return std::find_if(devices.begin(), devices.end(),
    [&controllerId](const DevicePtr &device)
    {
      return device->ControllerID() == controllerId;
    });
}
