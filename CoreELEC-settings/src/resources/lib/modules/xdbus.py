# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) 2009-2013 Stephan Raue (stephan@openelec.tv)
# Copyright (C) 2013 Lutz Fiebach (lufie@openelec.tv)
# Copyright (C) 2019-present Team LibreELEC (https://libreelec.tv)
# Copyright (C) 2020-present Team CoreELEC (https://coreelec.org)

import dbus
import dbus.service
import threading
import pgi
pgi.install_as_gi()
from gi.repository import GLib
from dbus.mainloop.glib import DBusGMainLoop


class xdbus:

    ENABLED = False
    menu = {'99': {}}

    def __init__(self, oeMain):
        try:
            oeMain.dbg_log('xdbus::__init__', 'enter_function', oeMain.LOGDEBUG)
            self.oe = oeMain
            self.dbusSystemBus = self.oe.dbusSystemBus
            self.oe.dbg_log('xdbus::__init__', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('xdbus::__init__', 'ERROR: (' + repr(e) + ')', self.oe.LOGERROR)

    def start_service(self):
        try:
            self.oe.dbg_log('xdbus::start_service', 'enter_function', self.oe.LOGDEBUG)
            self.dbusMonitor = dbusMonitor(self.oe)
            self.dbusMonitor.start()
            self.oe.dbg_log('xdbus::start_service', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('xdbus::start_service', 'ERROR: (' + repr(e) + ')', self.oe.LOGERROR)

    def stop_service(self):
        try:
            self.oe.dbg_log('xdbus::stop_service', 'enter_function', self.oe.LOGDEBUG)
            if hasattr(self, 'dbusMonitor'):
                self.dbusMonitor.stop()
                del self.dbusMonitor
            self.oe.dbg_log('xdbus::stop_service', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('xdbus::stop_service', 'ERROR: (' + repr(e) + ')')

    def exit(self):
        pass

    def restart(self):
        try:
            self.oe.dbg_log('xdbus::restart', 'enter_function', self.oe.LOGDEBUG)
            self.stop_service()
            self.start_service()
            self.oe.dbg_log('xdbus::restart', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('xdbus::restart', 'ERROR: (' + repr(e) + ')')


class dbusMonitor(threading.Thread):

    def __init__(self, oeMain):
        try:
            oeMain.dbg_log('xdbus::dbusMonitor::__init__', 'enter_function', oeMain.LOGDEBUG)
            self.monitors = []
            self.oe = oeMain
            self.dbusSystemBus = oeMain.dbusSystemBus
            dbus.mainloop.glib.threads_init()
            DBusGMainLoop(set_as_default=True)
            self.mainLoop = GLib.MainLoop()
            threading.Thread.__init__(self)
            self.oe.dbg_log('xdbus::dbusMonitor::__init__', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('xdbus::dbusMonitor::__init__', 'ERROR: (' + repr(e) + ')', self.oe.LOGERROR)

    def run(self):
        try:
            self.oe.dbg_log('xdbus::dbusMonitor::run', 'enter_function', self.oe.LOGDEBUG)

            self.dbusSystemBus.request_name("com.service.coreelec.settings.xdbus.stoploop")
            busName = dbus.service.BusName("com.service.coreelec.settings.xdbus.stoploop", bus=self.dbusSystemBus)
            dbus.service.Object(busName, "/com/service/coreelec/settings/xdbus/stoploop")

            for strModule in sorted(self.oe.dictModules, key=lambda x: list(self.oe.dictModules[x].menu.keys())):
                module = self.oe.dictModules[strModule]
                if hasattr(module, 'monitor') and module.ENABLED:
                    monitor = module.monitor(self.oe, module)
                    monitor.add_signal_receivers()
                    self.monitors.append(monitor)
            try:
                self.oe.dbg_log('xdbus Monitor started.', '', self.oe.LOGINFO)
                self.mainLoop.run()
                self.oe.dbg_log('xdbus Monitor stopped.', '', self.oe.LOGINFO)
            except:
                pass
            self.oe.dbg_log('xdbus::dbusMonitor::run', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('xdbus::dbusMonitor::run', 'ERROR: (' + repr(e) + ')', self.oe.LOGERROR)

    def stop(self):
        try:
            self.oe.dbg_log('xdbus::dbusMonitor::stop_service', 'enter_function', self.oe.LOGDEBUG)
            self.mainLoop.quit()
            for monitor in self.monitors:
                monitor.remove_signal_receivers()
                monitor = None
            self.oe.dbg_log('xdbus::dbusMonitor::stop_service', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('xdbus::dbusMonitor::stop_service', 'ERROR: (' + repr(e) + ')')
