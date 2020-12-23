[![License: GPL-2.0-or-later](https://img.shields.io/badge/License-GPL%20v2+-blue.svg)](LICENSE.md)
[![Build Status](https://travis-ci.org/kodi-pvr/pvr.vbox.svg?branch=Matrix)](https://travis-ci.org/kodi-pvr/pvr.vbox/branches)
[![Build Status](https://dev.azure.com/teamkodi/kodi-pvr/_apis/build/status/kodi-pvr.pvr.vbox?branchName=Matrix)](https://dev.azure.com/teamkodi/kodi-pvr/_build/latest?definitionId=70&branchName=Matrix)
[![Build Status](https://jenkins.kodi.tv/view/Addons/job/kodi-pvr/job/pvr.vbox/job/Matrix/badge/icon)](https://jenkins.kodi.tv/blue/organizations/jenkins/kodi-pvr%2Fpvr.vbox/branches/)
[![Coverity Scan Build Status](https://scan.coverity.com/projects/5120/badge.svg)](https://scan.coverity.com/projects/5120)

# VBox Home TV Gateway PVR Client

This repository provides a [Kodi](http://kodi.tv) PVR addon for interfacing with the VBox Communications XTi TV Gateway devices. This README serves as a quick overview of the functionality and architecture of the addon, to make it easier for others to possible contribute.

## Build instructions

### Linux

1. `git clone --branch master https://github.com/xbmc/xbmc.git`
2. `git clone https://github.com/kodi-pvr/pvr.vbox.git`
3. `cd pvr.vbox && mkdir build && cd build`
4. `cmake -DADDONS_TO_BUILD=pvr.vbox -DADDON_SRC_PREFIX=../.. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=../../xbmc/kodi-build/addons -DPACKAGE_ZIP=1 ../../xbmc/cmake/addons`
5. `make`

### Mac OSX

In order to build the addon on mac the steps are different to Linux and Windows as the cmake command above will not produce an addon that will run in kodi. Instead using make directly as per the supported build steps for kodi on mac we can build the tools and just the addon on it's own. Following this we copy the addon into kodi. Note that we checkout kodi to a separate directory as this repo will only only be used to build the addon and nothing else.

#### Build tools and initial addon build

1. Get the repos
 * `cd $HOME`
 * `git clone https://github.com/xbmc/xbmc xbmc-addon`
 * `git clone https://github.com/kodi-pvr/pvr.vbox`
2. Build the kodi tools
 * `cd $HOME/xbmc-addon/tools/depends`
 * `./bootstrap`
 * `./configure --host=x86_64-apple-darwin`
 * `make -j$(getconf _NPROCESSORS_ONLN)`
3. Build the addon
 * `cd $HOME/xbmc-addon`
 * `make -j$(getconf _NPROCESSORS_ONLN) -C tools/depends/target/binary-addons ADDONS="pvr.vbox" ADDON_SRC_PREFIX=$HOME`

Note that the steps in the following section need to be performed before the addon is installed and you can run it in Kodi.

#### To rebuild the addon and copy to kodi after changes (after the initial addon build)

1. `cd $HOME/pvr.vbox`
2. `./build-install-mac.sh ../xbmc-addon`

If you would prefer to run the rebuild steps manually instead of using the above helper script check the appendix [here](#manual-steps-to-rebuild-the-addon-on-macosx)

### Windows

These instructions may be outdated. I'm assuming here that you'll check out all source code into `C:\Projects`, you'll have to adjust that if you use another path.

1. Check out the Kodi source from Github (`C:\Projects\xbmc`) and make sure you have Microsoft Visual Studio 2013 installed.
2. Check out this repository (`C:\Projects\pvr.vbox`)
3. This step has to be done once only. Go to `C:\Projects\xbmc\project\cmake\addons\addons`. You'll see there's a folder for each binary addon. In order to build your locally checked out copy of the addon instead of the version that is bundled with Kodi you'll need to open the file `pvr.vbox/pvr.vbox.txt` and change its contents to the following: `pvr.vbox file://C:/Projects/pvr.vbox`.
4. Open a command prompt (or Powershell) and browse to `C:\Projects\xbmc\tools\windows`. From here, run `prepare-binary-addons-dev.bat clean` and then `prepare-binary-addons-dev.bat pvr.vbox`.
5. Go to `C:\Projects\xbmc\project\cmake\addons\build` and open and build the `kodi-addons.sln` solution.
6. The addon DLL is built and located in `C:\Projects\xbmc\addons`. If you run Kodi now from inside Visual Studio the addon will appear automatically under "System addons". If you don't want to bother compiling Kodi from source, install it as you normally would and copy the `pvr.vbox` into `%APPDATA%\Kodi\addons`.
7. Run Kodi, configure and enable the addon, then enable Live TV.

## Settings

It's possible to configure the addon to connect to the VBox TV Gateway using both the local netowrk and via the internet address/port. This is useful for e.g. a laptop which is not permanently inside your internal network. When the addon starts it first attempts to make a connection using the local network settings. If that fails, it will try the internet settings instead. The addon restarts itself if the connection is lost so it will automatically switch back without having to restart Kodi.

### Connection - Internal
Connection settings to use when connecting from your local network. For a local network connection the port values should not need to be modified.

* **Hostname or IP address**: The IP address or hostname of your VBox when accessed from the local network.
* **HTTP port**: The port used to connect to your VBox when accessed from the local network. Default value is `80`.
* **HTTPS port**: The port used to connect to your VBox if using HTTPS when accessed from the local network. The default `0` means this is disabled and HTTP will be used instead.
* **UPnP port**: The port used to connect to your VBox via UPnP when accessed from the local network. Default value is `55555`.
* **Connection timeout (seconds)**: The value used (in seconds) to denote when a connection attempt has failed when accessed from the local network. Default value is `3`.

### Connection - External
Connection settings to use when connecting from the internet. The ports should only need to change if you're using asymmetric port forwarding (e.g. port 8080 -> 80 and 12345 -> 55555). The HTTP port is used to communicate with the device while the UPnP port is used when streaming media.

* **Hostname or IP address**: The IP address or hostname of your VBox when accessed from the internet.
* **HTTP port**: The port used to connect to your VBox when accessed from the internet.
* **HTTPS port**: The port used to connect to your VBox if using HTTPS when accessed from the internet. The default `0` means this is disabled and HTTP will be used instead.
* **UPnP port**: The port used to connect to your VBox via UPnP when accessed from the internet. Default value is `55555`.
* **Connection timeout (seconds)**: The value used (in seconds) to denote when a connection attempt has failed when accessed from the internet. Default value is `10`.

### EPG
Settings related to Channels & EPG.

* **Channel numbers set by**: Channel numbers can be set via either of the following two options:
    - `LCN (Logical Channel Number) from backend` - The channel numbers as set on the backend.
    - `Channel index in backend` - Starting from 1 number the channels as per the order they appear on the backend.
* **Reminder time (minutes before programme start)**: The amount of time in minutes prior to a programme start that a reminder should pop up.
* **Skip initial EPG load**: Ignore the initial EPG load. Enabled by default to prevent crash issues on LibreElec/CoreElec.

### Timeshift
Settings related to the timeshift.

* **Enable timeshifting**: If enabled allows pause, rewind and fast-forward of live TV.
* **Timeshift buffer path**: The path where the timeshift buffer files should be stored when timeshifting is enabled. Make sure you have a reasonable amount of disk space available since the buffer will grow indefinitely until you stop watching or switch channels.

### Architecture

This addon has been built from the ground up using C++11. The main functionality is contained within the `vbox` namespace, the only file outside that is `client.cpp` which bridges the gap between the Kodi API and the API used by the main library class, `vbox::VBox`. The idea is to keep the addon code as decoupled from Kodi as possible.

The addon communicates with a VBox TV Gateway using the TV gateway's HTTP API. Since the structure of the responses vary a little bit, a factory is used to construct meaningful objects to represent the various responses. All response-related code is located under the `vbox::response` namespace.

The addon requires XMLTV parsing since that's the format the gateway provides EPG data over. The classes and utilities for handling this are kept separate under the `xmltv` namespace so that the code can potentially be reused.

The `vbox::VBox` class which `client.cpp` interfaces with is designed so that an exception of base type `VBoxException` (which is an extension of `std::runtime_error`) is thrown whenever ever a request fails. A request can fail for various reasons:

* the request failed to execute, i.e. the backend was unavailable
* the XML parsing failed, i.e. the response was invalid
* the request succeeded but the response represented an error

Similar to the XMLTV code, the code for the timeshift buffer is fairly generic and lives in a separate `timeshift` namespace. Currently there is a base class for all buffers and two implementations, a `FilesystemBuffer` which buffers the data to a file on disc, and a `DummyBuffer` which just relays the read operations to the underlying input handle. This is required since Kodi uses a different code paths depending on whether clients handle input streams on their own or not, and we need this particular code path for other features like signal status handling to work.

### Versioning

The addon follows semantic versioning. Each release is tagged with its respective version number. Since each release of Kodi requires a separate branch for the addon, the major version changes whenever the Kodi version changes. This means that versions `1.x.x` are for Kodi v15, versions `2.x.x` are for Kodi v16 and so on.

### Useful links

* [Kodi's PVR user support](http://forum.kodi.tv/forumdisplay.php?fid=167)
* [Kodi's PVR development support](http://forum.kodi.tv/forumdisplay.php?fid=136)
* [Kodi's about VBox TV Gateway](http://kodi.wiki/view/VBox_Home_TV_Gateway)

###  License

The code is licensed under the GNU GPL version 2 license.

## Appendix

### Manual Steps to rebuild the addon on MacOSX

The following steps can be followed manually instead of using the `build-install-mac.sh` in the root of the addon repo after the [initial addon build](#build-tools-and-initial-addon-build) has been completed.

**To rebuild the addon after changes**

1. `rm tools/depends/target/binary-addons/.installed-macosx*`
2. `make -j$(getconf _NPROCESSORS_ONLN) -C tools/depends/target/binary-addons ADDONS="pvr.vbox" ADDON_SRC_PREFIX=$HOME`

or

1. `cd tools/depends/target/binary-addons/macosx*`
2. `make`

**Copy the addon to the Kodi addon directory on Mac**

1. `rm -rf "$HOME/Library/Application Support/Kodi/addons/pvr.vbox"`
2. `cp -rf $HOME/xbmc-addon/addons/pvr.vbox "$HOME/Library/Application Support/Kodi/addons"`
