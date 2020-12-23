/*
 *  Copyright (C) 2016-2020 Garrett Brown
 *  Copyright (C) 2016-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#include "XArcadeScanner.h"
#include "XArcadeDefines.h"
#include "XArcadeDevice.h"
#include "XArcadeUtils.h"
#include "utils/CommonDefines.h" // for INVALID_FD

#include <kodi/AddonBase.h>

#include <fcntl.h>
#include <glob.h>
#include <linux/input.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace XARCADE;

DeviceVector CXArcadeScanner::GetDevices()
{
  DeviceVector devices;

  glob_t pglob;
  int rc = glob("/dev/input/event*", 0, nullptr, &pglob);
  if (rc != 0)
  {
    kodi::Log(ADDON_LOG_ERROR, "Failed to open event devices");
    return devices;
  }

  for (unsigned int ctr = 0; ctr < pglob.gl_pathc; ++ctr)
  {
    const char* filename = pglob.gl_pathv[ctr];

    int fevdev = open(filename, O_RDONLY | O_NONBLOCK);
    if (fevdev == INVALID_FD)
      continue;

    char name[256] = { };
    ioctl(fevdev, EVIOCGNAME(sizeof(name)), name);

    if (CXArcadeUtils::IsXArcadeDevice(name))
    {
      // Found device
      devices.emplace_back(std::make_shared<CXArcadeDevice>(fevdev, m_nextIndex++));
    }
    else
    {
      close(fevdev);
    }
  }

  globfree(&pglob);

  return devices;
}
