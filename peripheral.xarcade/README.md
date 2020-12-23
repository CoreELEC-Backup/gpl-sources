[![License: GPL-2.0-or-later](https://img.shields.io/badge/License-GPL%20v2+-blue.svg)](LICENSE.md)
[![Build Status](https://travis-ci.org/kodi-game/peripheral.xarcade.svg?branch=Matrix)](https://travis-ci.org/kodi-game/peripheral.xarcade/branches)
[![Build Status](https://jenkins.kodi.tv/view/Addons/job/kodi-game/job/peripheral.xarcade/job/Matrix/badge/icon)](https://jenkins.kodi.tv/blue/organizations/jenkins/kodi-game%2Fperipheral.xarcade/branches)

# X-Arcade Tankstick driver for Kodi

![X-Arcade Tankstick](peripheral.xarcade/resources/icon.png)

This is a peripheral add-on for Kodi that enables input from the X-Arcade Tankstick. I found [Xarcade2Jstick](https://github.com/petrockblog/Xarcade2Jstick) to be a helpful reference. Thank you Xarcade2Jstick author!

## Wiki help

The wiki article is: [HOW-TO:X-Arcade Tankstick in Kodi](https://kodi.wiki/view/HOW-TO:X-Arcade_Tankstick_in_Kodi).

## Fixing permissions

In general the `/dev/input/event*` devices can only be opened by root. This is to prevent keystroke logging security attacks.

To give non-root users read access to the X-Arcade Tankstick, place [80-xarcade.rules](rules/80-xarcade.rules) in `/etc/udev/rules.d/`.
