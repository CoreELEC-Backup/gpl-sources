**What It Does**

The eventlircd daemon provides four functions for [LIRC](http://www.lirc.org) devices:
* converting multiple Linux input event devices into an lircd socket,
* separating keyboard and mouse/joystick functionality,
* mapping keyboard shortcut key code sequences to single key codes, and hotplugging using [udev](http://en.wikipedia.org/wiki/Udev).

**How It Does What It Does**

When an lircd supported device is added/removed, udev rules start/stop an LIRC daemon (lircd) to convert the device to a Linux input event device. When an eventlircd supported device is added/removed, udev rules signal eventlircd using eventlircd specific environment variables. An init script starts/stops the eventlircd daemon. In addition, an init script starts/stops any lircd daemons for lircd supported devices that are not detected by udev.

**Why It Exists**

I maintain [MiniMyth](http://www.minimyth.org). Over time, there were requests to support

* Multiple remote controls,
* Remote controls with mouse/joystick functionality,
* Bluetooth remote controls that may not be paired at boot,
* Media Center Edition remote controls that appear as USB HID devices and send keyboard shortcuts, and Remote control hotplugging.

While I was able to configure lircd to support 1, 2 (post 0.8.6) and 3, I was not able to configure lircd to support 4 without device specific lircrc files, and I was not able to configure lircd to support 5. While it is likely that I could have patched lircd to support 4 and 5, I was uneasy with the changes that appeared be required. Therefore, I decided to implement the functionality as a separate daemon used in conjunction with lircd.

**Overlap With Other Software**

The eventlircd functionality has some overlap with the [inputlircd](http://svn.sliepen.eu.org/inputlirc/trunk/) functionality. Both enable converting multiple Linux input event devices into an lircd socket. However, they differ in several ways. The inputlircd daemon does not support hotplugging using udev, separating keyboard and mouse/joystick functionality, or mapping keyboard shortcut key code sequences to single key codes. The eventlircd daemon does not support manually configuring the Linux input event devices, or mapping key codes to non key code names.

The evenlircd functionality has some overlap with the lircd functionality. Both enable converting multiple Linux input event devices into an lircd socket and separating keyboard and mouse/joystick functionality. However, they differ in several ways. The lircd daemon does not support hotplugging using udev, or mapping keyboard shortcut key code sequences to single key codes. The eventlircd daemon does not support manually configuring the Linux input event devices, mapping key codes to non key code names, using devices that are not Linux input event devices, or transmitting IR signals.

**How it works**

The software's architecture is straight forward. At the center is monitor, which monitors a list of file descriptors and calls the file descriptor's handler when the file descriptor is "ready". Initially, lircd creates an lircd socket and adds it to monitor's file descriptor list so that it can watch for lircd client connection requests, and input creates a udev monitor and adds it to monitor's file descriptor list so that it can watch for input event devices. When lircd detects an lircd client connect request, it connects the client and adds it to its client list so that it can send lircd messages to the client. When input detects an input event device that is to be handled by eventlircd, it opens the input device, creates a corresponding mouse/joystick event device (if needed) and adds the input device to monitor's file descriptor list so that it can watch for events. When input detects an event on one of its input devices, it performs the key mapping and sends the resulting mapped event to either the lircd clients or the input device's mouse/joystick event device depending on whether the mapped event is a keyboard or mouse/joystick event.

The daemon (eventlircd) is a sysvinit daemon. While it should be compatible with systemd, it is not a systemd daemon.

* The software has no i18n or l10n.
* The comments in the source code are not in doxygen format.
* The comments in the source code are not complete.

**This daemon was inspired by**

* [Christoph Bartelmus's lircd daemon] (http://www.lirc.org/)
* [Guus Sliepen's inputlircd daemon] (http://svn.sliepen.eu.org/inputlirc/trunk/)
* [Julien Cristau's Xorg udev patch] (http://lists.x.org/archives/xorg-devel/2009-October/002797.html)

