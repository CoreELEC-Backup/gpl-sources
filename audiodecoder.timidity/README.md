# audiodecoder.timidity addon for Kodi

This is a [Kodi](https://kodi.tv) audio decoder addon for midi files.

[![License: GPL v2](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](LICENSE.md)
[![Build Status](https://travis-ci.org/xbmc/audiodecoder.timidity.svg?branch=Matrix)](https://travis-ci.org/xbmc/audiodecoder.timidity/branches)
[![Build Status](https://dev.azure.com/teamkodi/binary-addons/_apis/build/status/xbmc.audiodecoder.timidity?branchName=Matrix)](https://dev.azure.com/teamkodi/binary-addons/_build/latest?definitionId=2&branchName=Matrix)
[![Build Status](https://jenkins.kodi.tv/view/Addons/job/xbmc/job/audiodecoder.timidity/job/Matrix/badge/icon)](https://jenkins.kodi.tv/blue/organizations/jenkins/xbmc%2Faudiodecoder.timidity/branches/)

## Requirements

In order to use this addon you have to set a [SoundFont]("https://en.wikipedia.org/wiki/SoundFont") file in Kodi's addon settings.

## Build instructions

When building the addon you have to use the correct branch depending on which version of Kodi you're building against. 
If you want to build the addon to be compatible with the latest kodi `master` commit, you need to checkout the branch with the current kodi codename.
Also make sure you follow this README from the branch in question.

### Linux

The following instructions assume you will have built Kodi already in the `kodi-build` directory 
suggested by the README.

1. `git clone --branch master https://github.com/xbmc/xbmc.git`
2. `git clone https://github.com/xbmc/audiodecoder.timidity.git`
3. `cd audiodecoder.timidity && mkdir build && cd build`
4. `cmake -DADDONS_TO_BUILD=audiodecoder.timidity -DADDON_SRC_PREFIX=../.. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../../xbmc/kodi-build/addons -DPACKAGE_ZIP=1 ../../xbmc/cmake/addons`
5. `make`

The addon files will be placed in `../../xbmc/kodi-build/addons` so if you build Kodi from source and run it directly 
the addon will be available as a system addon.
