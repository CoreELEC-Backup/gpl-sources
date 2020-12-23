<p align="center">
  <img src="audiodecoder.asap/icon.png" />
</p>

------------------

[![License: GPL-2.0-or-later](https://img.shields.io/badge/License-GPL%20v2+-blue.svg)](LICENSE.md)
[![Build Status](https://travis-ci.org/xbmc/audiodecoder.asap.svg?branch=Matrix)](https://travis-ci.org/xbmc/audiodecoder.asap/branches)
[![Build Status](https://dev.azure.com/teamkodi/binary-addons/_apis/build/status/xbmc.audiodecoder.asap?branchName=Matrix)](https://dev.azure.com/teamkodi/binary-addons/_build/latest?definitionId=4&branchName=Matrix)
[![Build Status](https://jenkins.kodi.tv/view/Addons/job/xbmc/job/audiodecoder.asap/job/Matrix/badge/icon)](https://jenkins.kodi.tv/blue/organizations/jenkins/xbmc%2Faudiodecoder.asap/branches/)
<!--- [![Build Status](https://ci.appveyor.com/api/projects/status/github/xbmc/audiodecoder.asap?branch=Matrix&svg=true)](https://ci.appveyor.com/project/xbmc/audiodecoder-asap?branch=Matrix) -->

# audiodecoder.asap addon for Kodi

This is a [Kodi](https://kodi.tv) audio decoder addon for SAP files.

ASAP (Another Slight Atari Player) plays on Kodi and converts 8-bit Atari music (*.sap, *.cmc, *.mpt, *.rmt, *.tmc, ...) on modern computers and mobile devices.

## Build instructions

When building the addon you have to use the correct branch depending on which version of Kodi you're building against. 
If you want to build the addon to be compatible with the latest kodi `master` commit, you need to checkout the branch with the current kodi codename.
Also make sure you follow this README from the branch in question.

### Linux

The following instructions assume you will have built Kodi already in the `kodi-build` directory 
suggested by the README.

1. `git clone --branch master https://github.com/xbmc/xbmc.git`
2. `git clone https://github.com/xbmc/audiodecoder.asap.git`
3. `cd audiodecoder.asap && mkdir build && cd build`
4. `cmake -DADDONS_TO_BUILD=audiodecoder.asap -DADDON_SRC_PREFIX=../.. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../../xbmc/kodi-build/addons -DPACKAGE_ZIP=1 ../../xbmc/cmake/addons`
5. `make`

The addon files will be placed in `../../xbmc/kodi-build/addons` so if you build Kodi from source and run it directly 
the addon will be available as a system addon.
