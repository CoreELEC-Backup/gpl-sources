# LAME Audio Encoder add-on for Kodi

This is a [Kodi](https://kodi.tv) LAME audio encoder add-on.

#### CI Testing
[![License: GPL-2.0-or-later](https://img.shields.io/badge/License-GPL%20v2+-blue.svg)](LICENSE.md)
[![Build Status](https://travis-ci.org/xbmc/audioencoder.lame.svg?branch=Matrix)](https://travis-ci.org/xbmc/audioencoder.lame/branches)
[![Build Status](https://dev.azure.com/teamkodi/binary-addons/_apis/build/status/xbmc.audioencoder.lame?branchName=Matrix)](https://dev.azure.com/teamkodi/binary-addons/_build/latest?definitionId=22&branchName=Matrix)
[![Build Status](https://jenkins.kodi.tv/view/Addons/job/xbmc/job/audioencoder.lame/job/Matrix/badge/icon)](https://jenkins.kodi.tv/blue/organizations/jenkins/xbmc%2Faudioencoder.lame/branches/)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/5120/badge.svg)](https://scan.coverity.com/projects/5120)
<!--- [![Build Status](https://ci.appveyor.com/api/projects/status/github/xbmc/audioencoder.lame?branch=Matrix&svg=true)](https://ci.appveyor.com/project/xbmc/audioencoder-lame?branch=Matrix) -->

## Build instructions

When building the addon you have to use the correct branch depending on which version of Kodi you're building against.
If you want to build the addon to be compatible with the latest kodi `master` commit, you need to checkout the branch with the current kodi codename.
Also make sure you follow this README from the branch in question.

### Linux

1. `git clone --branch master https://github.com/xbmc/xbmc.git`
2. `git clone --branch Matrix https://github.com/audioencoder.lame/audioencoder.lame.git`
3. `cd audioencoder.lame && mkdir build && cd build`
4. `cmake -DADDONS_TO_BUILD=audioencoder.lame -DADDON_SRC_PREFIX=../.. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../../xbmc/addons -DPACKAGE_ZIP=1 ../../xbmc/project/cmake/addons`
5. `make`

The add-on files will be placed in `../../xbmc/addons` so if you build Kodi from source and run it directly 
the add-on will be available as a system add-on.

##### Useful links

* [Kodi's add-ons development support](https://forum.kodi.tv/forumdisplay.php?fid=26)
