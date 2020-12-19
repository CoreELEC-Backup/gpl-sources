# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) 2009-2013 Stephan Raue (stephan@openelec.tv)
# Copyright (C) 2013 Lutz Fiebach (lufie@openelec.tv)
# Copyright (C) 2018 Team LibreELEC
# Copyright (C) 2018-present Team CoreELEC (https://coreelec.org)

import os
import re
import glob
import time
import json
import xbmc
import xbmcgui
import tarfile
import oeWindows
import threading
import subprocess
import shutil
from xml.dom import minidom
import datetime
import tempfile
from functools import cmp_to_key

class updates:

    ENABLED = False
    KERNEL_CMD = None
    UPDATE_REQUEST_URL = None
    UPDATE_DOWNLOAD_URL = None
    LOCAL_UPDATE_DIR = None
    menu = {'2': {
        'name': 32005,
        'menuLoader': 'load_menu',
        'listTyp': 'list',
        'InfoText': 707,
        }}

    def __init__(self, oeMain):
        try:
            oeMain.dbg_log('updates::__init__', 'enter_function', oeMain.LOGDEBUG)
            self.oe = oeMain
            self.struct = {
                'update': {
                    'order': 1,
                    'name': 32013,
                    'settings': {
                        'AutoUpdate': {
                            'name': 32014,
                            'value': 'auto',
                            'action': 'set_auto_update',
                            'type': 'multivalue',
                            'values': ['auto', 'manual'],
                            'InfoText': 714,
                            'order': 1,
                            },
                        'SubmitStats': {
                            'name': 32021,
                            'value': '1',
                            'action': 'set_value',
                            'type': 'bool',
                            'InfoText': 772,
                            'order': 2,
                            },
                        'UpdateNotify': {
                            'name': 32365,
                            'value': '1',
                            'action': 'set_value',
                            'type': 'bool',
                            'InfoText': 715,
                            'order': 3,
                            },
                        'ShowCustomChannels': {
                            'name': 32016,
                            'value': '0',
                            'action': 'set_custom_channel',
                            'type': 'bool',
                            'parent': {
                                'entry': 'AutoUpdate',
                                'value': ['manual'],
                                },
                            'InfoText': 761,
                            'order': 4,
                            },
                        'CustomChannel1': {
                            'name': 32017,
                            'value': '',
                            'action': 'set_custom_channel',
                            'type': 'text',
                            'parent': {
                                'entry': 'ShowCustomChannels',
                                'value': ['1'],
                                },
                            'InfoText': 762,
                            'order': 5,
                            },
                        'CustomChannel2': {
                            'name': 32018,
                            'value': '',
                            'action': 'set_custom_channel',
                            'type': 'text',
                            'parent': {
                                'entry': 'ShowCustomChannels',
                                'value': ['1'],
                                },
                            'InfoText': 762,
                            'order': 6,
                            },
                        'CustomChannel3': {
                            'name': 32019,
                            'value': '',
                            'action': 'set_custom_channel',
                            'type': 'text',
                            'parent': {
                                'entry': 'ShowCustomChannels',
                                'value': ['1'],
                                },
                            'InfoText': 762,
                            'order': 7,
                            },
                        'Channel': {
                            'name': 32015,
                            'value': '',
                            'action': 'set_channel',
                            'type': 'multivalue',
                            'parent': {
                                'entry': 'AutoUpdate',
                                'value': ['manual'],
                                },
                            'values': [],
                            'InfoText': 760,
                            'order': 8,
                            },
                        'Build': {
                            'name': 32020,
                            'value': '',
                            'action': 'do_manual_update',
                            'type': 'button',
                            'parent': {
                                'entry': 'AutoUpdate',
                                'value': ['manual'],
                                },
                            'InfoText': 770,
                            'order': 9,
                            },
                        'Update2NextStable': {
                            'name': 32030,
                            'value': '0',
                            'action': 'set_value',
                            'type': 'bool',
                            'parent': {
                                'entry': 'AutoUpdate',
                                'value': ['auto'],
                                },
                            'InfoText': 716,
                            'order': 10,
                            },
                        },
                    },
                'rpieeprom': {
                    'order': 2,
                    'name': 32022,
                    'settings': {
                        'bootloader': {
                            'name': 'dummy',
                            'value': '',
                            'action': 'set_rpi_bootloader',
                            'type': 'bool',
                            'InfoText': 32025,
                            'order': 1,
                            },
                        'vl805': {
                            'name': 32026,
                            'value': '',
                            'action': 'set_rpi_vl805',
                            'type': 'bool',
                            'InfoText': 32027,
                            'order': 2,
                            },
                        },
                    },
                }

            self.keyboard_layouts = False
            self.nox_keyboard_layouts = False
            self.last_update_check = 0
            self.arrVariants = {}
            self.oe.dbg_log('updates::__init__', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('updates::__init__', 'ERROR: (' + repr(e) + ')')

    def start_service(self):
        try:
            self.oe.dbg_log('updates::start_service', 'enter_function', self.oe.LOGDEBUG)
            self.is_service = True
            self.load_values()
            self.set_auto_update()
            del self.is_service
            self.oe.dbg_log('updates::start_service', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('updates::start_service', 'ERROR: (' + repr(e) + ')')

    def stop_service(self):
        try:
            self.oe.dbg_log('updates::stop_service', 'enter_function', self.oe.LOGDEBUG)
            if hasattr(self, 'update_thread'):
                self.update_thread.stop()
            self.oe.dbg_log('updates::stop_service', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('updates::stop_service', 'ERROR: (' + repr(e) + ')')

    def do_init(self):
        try:
            self.oe.dbg_log('updates::do_init', 'enter_function', self.oe.LOGDEBUG)
            self.oe.dbg_log('updates::do_init', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('updates::do_init', 'ERROR: (' + repr(e) + ')')

    def exit(self):
        self.oe.dbg_log('updates::exit', 'enter_function', self.oe.LOGDEBUG)
        self.oe.dbg_log('updates::exit', 'exit_function', self.oe.LOGDEBUG)
        pass

    # Identify connected GPU card (card0, card1 etc.)
    def get_gpu_card(self):
        for root, dirs, files in os.walk("/sys/class/drm", followlinks=False):
            for dir in dirs:
                try:
                    with open(os.path.join(root, dir, 'status'), 'r') as infile:
                        for line in [x for x in infile if x.replace('\n', '') == 'connected']:
                            return dir.split("-")[0]
                except:
                    pass
            break

        return 'card0'

    # Return driver name, eg. 'i915', 'i965', 'nvidia', 'nvidia-legacy', 'amdgpu', 'radeon', 'vmwgfx', 'virtio-pci' etc.
    def get_hardware_flags_x86_64(self):
        gpu_props = {}
        gpu_driver = ""

        gpu_card = self.get_gpu_card()
        self.oe.dbg_log('updates::get_hardware_flags_x86_64', 'Using card: %s' % gpu_card, self.oe.LOGDEBUG)

        gpu_path = self.oe.execute('/usr/bin/udevadm info --name=/dev/dri/%s --query path 2>/dev/null' % gpu_card, get_result=1).replace('\n','')
        self.oe.dbg_log('updates::get_hardware_flags_x86_64', 'gpu path: %s' % gpu_path, self.oe.LOGDEBUG)

        if gpu_path:
            drv_path = os.path.dirname(os.path.dirname(gpu_path))
            props = self.oe.execute('/usr/bin/udevadm info --path=%s --query=property 2>/dev/null' % drv_path, get_result=1)

            if props:
                for key, value in [x.strip().split('=') for x in props.strip().split('\n')]:
                    gpu_props[key] = value
            self.oe.dbg_log('updates::get_gpu_type', 'gpu props: %s' % gpu_props, self.oe.LOGDEBUG)
            gpu_driver = gpu_props.get("DRIVER", "")

        if not gpu_driver:
            gpu_driver = self.oe.execute('lspci -k | grep -m1 -A999 "VGA compatible controller" | grep -m1 "Kernel driver in use" | cut -d" " -f5', get_result=1).replace('\n','')

        if gpu_driver == 'nvidia' and os.path.realpath('/var/lib/nvidia_drv.so').endswith('nvidia-legacy_drv.so'):
            gpu_driver = 'nvidia-legacy'

        self.oe.dbg_log('updates::get_hardware_flags_x86_64', 'gpu driver: %s' % gpu_driver, self.oe.LOGDEBUG)

        return gpu_driver if gpu_driver else "unknown"

    def get_hardware_flags_dtflag(self):
        if os.path.exists('/usr/bin/dtflag'):
            dtflag = self.oe.execute('/usr/bin/dtflag', get_result=1).rstrip('\x00\n')
        else:
            dtflag = "unknown"

        self.oe.dbg_log('system::get_hardware_flags_dtflag', 'ARM board: %s' % dtflag, self.oe.LOGDEBUG)

        return dtflag

    def get_hardware_flags(self):
        if self.oe.PROJECT == "Generic":
            return self.get_hardware_flags_x86_64()
        elif self.oe.PROJECT == "Amlogic-ce":
            return self.oe.execute('/usr/bin/dtname', get_result=1).rstrip('\x00\n')
        elif self.oe.PROJECT in ['Allwinner', 'Amlogic', 'NXP', 'Qualcomm', 'Rockchip', 'RPi', 'Samsung' ]:
            return self.get_hardware_flags_dtflag()
        else:
            self.oe.dbg_log('updates::get_hardware_flags', 'Project is %s, no hardware flag available' % self.oe.PROJECT, self.oe.LOGDEBUG)
            return ""

    def load_values(self):
        try:
            self.oe.dbg_log('updates::load_values', 'enter_function', self.oe.LOGDEBUG)

            # Hardware flags
            self.hardware_flags = self.get_hardware_flags()
            self.oe.dbg_log('system::load_values', 'loaded hardware_flag %s' % self.hardware_flags, self.oe.LOGDEBUG)

            # AutoUpdate

            value = self.oe.read_setting('updates', 'AutoUpdate')
            if not value is None:
                self.struct['update']['settings']['AutoUpdate']['value'] = value
            value = self.oe.read_setting('updates', 'SubmitStats')
            if not value is None:
                self.struct['update']['settings']['SubmitStats']['value'] = value
            value = self.oe.read_setting('updates', 'UpdateNotify')
            if not value is None:
                self.struct['update']['settings']['UpdateNotify']['value'] = value
            if os.path.isfile('%s/SYSTEM' % self.LOCAL_UPDATE_DIR):
                self.update_in_progress = True
            value = self.oe.read_setting('updates', 'Update2NextStable')
            if not value is None:
                self.struct['update']['settings']['Update2NextStable']['value'] = value

            # Manual Update

            value = self.oe.read_setting('updates', 'Channel')
            if not value is None:
                self.struct['update']['settings']['Channel']['value'] = value
            value = self.oe.read_setting('updates', 'ShowCustomChannels')
            if not value is None:
                self.struct['update']['settings']['ShowCustomChannels']['value'] = value

            value = self.oe.read_setting('updates', 'CustomChannel1')
            if not value is None:
                self.struct['update']['settings']['CustomChannel1']['value'] = value
            value = self.oe.read_setting('updates', 'CustomChannel2')
            if not value is None:
                self.struct['update']['settings']['CustomChannel2']['value'] = value
            value = self.oe.read_setting('updates', 'CustomChannel3')
            if not value is None:
                self.struct['update']['settings']['CustomChannel3']['value'] = value

            self.update_json = self.build_json()

            self.struct['update']['settings']['Channel']['values'] = self.get_channels()
            self.struct['update']['settings']['Build']['values'] = self.get_available_builds()

            # RPi4 EEPROM updating
            if self.oe.RPI_CPU_VER == '3':
                self.rpi_flashing_state = self.get_rpi_flashing_state()
                if self.rpi_flashing_state['incompatible']:
                    self.struct['rpieeprom']['hidden'] = 'true'
                else:
                    self.struct['rpieeprom']['settings']['bootloader']['value'] = self.get_rpi_eeprom('BOOTLOADER')
                    self.struct['rpieeprom']['settings']['bootloader']['name'] = '%s (%s)' % (self.oe._(32024), self.rpi_flashing_state['bootloader']['state'])
                    self.struct['rpieeprom']['settings']['vl805']['value'] = self.get_rpi_eeprom('VL805')
                    self.struct['rpieeprom']['settings']['vl805']['name'] = '%s (%s)' % (self.oe._(32026), self.rpi_flashing_state['vl805']['state'])
            else:
                self.struct['rpieeprom']['hidden'] = 'true'

            self.oe.dbg_log('updates::load_values', 'exit_function', self.oe.LOGDEBUG)

        except Exception as e:
            self.oe.dbg_log('updates::load_values', 'ERROR: (' + repr(e) + ')')

    def load_menu(self, focusItem):
        try:
            self.oe.dbg_log('updates::load_menu', 'enter_function', self.oe.LOGDEBUG)
            self.oe.winOeMain.build_menu(self.struct)
            self.oe.dbg_log('updates::load_menu', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('updates::load_menu', 'ERROR: (' + repr(e) + ')')

    def set_value(self, listItem):
        try:
            self.oe.dbg_log('updates::set_value', 'enter_function', self.oe.LOGDEBUG)
            self.struct[listItem.getProperty('category')]['settings'][listItem.getProperty('entry')]['value'] = listItem.getProperty('value')
            self.oe.write_setting('updates', listItem.getProperty('entry'), str(listItem.getProperty('value')))
            self.oe.dbg_log('updates::set_value', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('updates::set_value', 'ERROR: (' + repr(e) + ')')

    def set_auto_update(self, listItem=None):
        try:
            self.oe.dbg_log('updates::set_auto_update', 'enter_function', self.oe.LOGDEBUG)
            if not listItem == None:
                self.set_value(listItem)
            if not hasattr(self, 'update_disabled'):
                if not hasattr(self, 'update_thread'):
                    self.update_thread = updateThread(self.oe)
                    self.update_thread.start()
                else:
                    self.update_thread.wait_evt.set()
                self.oe.dbg_log('updates::set_auto_update', str(self.struct['update']['settings']['AutoUpdate']['value']), self.oe.LOGINFO)
            self.oe.dbg_log('updates::set_auto_update', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('updates::set_auto_update', 'ERROR: (' + repr(e) + ')')

    def set_channel(self, listItem=None):
        try:
            self.oe.dbg_log('updates::set_channel', 'enter_function', self.oe.LOGDEBUG)
            if not listItem == None:
                self.set_value(listItem)
            self.struct['update']['settings']['Build']['values'] = self.get_available_builds()
            self.oe.dbg_log('updates::set_channel', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('updates::set_channel', 'ERROR: (' + repr(e) + ')')

    def set_custom_channel(self, listItem=None):
        try:
            self.oe.dbg_log('updates::set_custom_channel', 'enter_function', self.oe.LOGDEBUG)
            if not listItem == None:
                self.set_value(listItem)
            self.update_json = self.build_json()
            self.struct['update']['settings']['Channel']['values'] = self.get_channels()
            if not self.struct['update']['settings']['Channel']['values'] is None:
                if not self.struct['update']['settings']['Channel']['value'] in self.struct['update']['settings']['Channel']['values']:
                    self.struct['update']['settings']['Channel']['value'] = None
            self.struct['update']['settings']['Build']['values'] = self.get_available_builds()
            self.oe.dbg_log('updates::set_custom_channel', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('updates::set_custom_channel', 'ERROR: (' + repr(e) + ')')

    def custom_sort_train(self, a, b):
        a_items = a.split('-')
        b_items = b.split('-')

        a_builder = a_items[0]
        b_builder = b_items[0]

        if (a_builder == b_builder):
          return (float(b_items[1]) - float(a_items[1]))
        elif (a_builder < b_builder):
          return -1
        elif (a_builder > b_builder):
          return +1

    def get_channels(self):
        try:
            self.oe.dbg_log('updates::get_channels', 'enter_function', self.oe.LOGDEBUG)
            channels = []
            self.oe.dbg_log('updates::get_channels', str(self.update_json), self.oe.LOGDEBUG)
            if not self.update_json is None:
                for channel in self.update_json:
                    channels.append(channel)
            self.oe.dbg_log('updates::get_channels', 'exit_function', self.oe.LOGDEBUG)
            return sorted(list(set(channels)), key=cmp_to_key(self.custom_sort_train))
        except Exception as e:
            self.oe.dbg_log('updates::get_channels', 'ERROR: (' + repr(e) + ')')

    def do_manual_update(self, listItem=None):
        try:
            self.oe.dbg_log('updates::do_manual_update', 'enter_function', self.oe.LOGDEBUG)
            self.struct['update']['settings']['Build']['value'] = ''
            update_json = self.build_json(notify_error=True)
            if update_json is None:
                return
            self.update_json = update_json
            builds = self.get_available_builds()
            self.struct['update']['settings']['Build']['values'] = builds
            xbmcDialog = xbmcgui.Dialog()
            buildSel = xbmcDialog.select(self.oe._(32020), builds)
            if buildSel > -1:
                listItem = builds[buildSel]
                self.struct['update']['settings']['Build']['value'] = listItem
                channel = self.struct['update']['settings']['Channel']['value']
                regex = re.compile(self.update_json[channel]['prettyname_regex'])
                longname = '-'.join([self.oe.DISTRIBUTION, self.oe.ARCHITECTURE, self.oe.VERSION])
                if regex.search(longname):
                    version = regex.findall(longname)[0]
                else:
                    version = self.oe.VERSION
                if self.struct['update']['settings']['Build']['value'] != '':
                    self.update_file = self.update_json[self.struct['update']['settings']['Channel']['value']]['url'] + self.get_available_builds(self.struct['update']['settings']['Build']['value'])
                    message = '%s: %s\n%s: %s\n%s' % (self.oe._(32188), version, self.oe._(32187), self.struct['update']['settings']['Build']['value'], self.oe._(32180))
                    answer = xbmcDialog.yesno('CoreELEC Update', message)
                    xbmcDialog = None
                    del xbmcDialog
                    if answer:
                        self.update_in_progress = True
                        self.do_autoupdate()
                self.struct['update']['settings']['Build']['value'] = ''
            self.oe.dbg_log('updates::do_manual_update', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('updates::do_manual_update', 'ERROR: (' + repr(e) + ')')

    def get_json(self, url=None):
        try:
            self.oe.dbg_log('updates::get_json', 'enter_function', self.oe.LOGDEBUG)
            if url is None:
                url = self.UPDATE_DOWNLOAD_URL % ('update.coreelec.org', '', 'releases.php')
            data = self.oe.load_url(url)
            if not data is None:
                update_json = json.loads(data)
            else:
                update_json = None
            self.oe.dbg_log('updates::get_json', 'exit_function', self.oe.LOGDEBUG)
            return update_json
        except Exception as e:
            self.oe.dbg_log('updates::get_json', 'ERROR: (' + repr(e) + ')')

    def build_json(self, notify_error=False):
        try:
            self.oe.dbg_log('updates::build_json', 'enter_function', self.oe.LOGDEBUG)
            update_json = self.get_json()
            if self.struct['update']['settings']['ShowCustomChannels']['value'] == '1':
                custom_urls = []
                for i in 1,2,3:
                    custom_urls.append(self.struct['update']['settings']['CustomChannel' + str(i)]['value'])
                for custom_url in custom_urls:
                    if custom_url != '':
                        custom_update_json = self.get_json(custom_url)
                        if not custom_update_json is None:
                            for channel in custom_update_json:
                                update_json[channel] = custom_update_json[channel]
                        elif notify_error:
                            ok_window = xbmcgui.Dialog()
                            answer = ok_window.ok(self.oe._(32191), 'Custom URL is not valid, or currently inaccessible.\n\n%s' % custom_url)
                            if not answer:
                                return
            self.oe.dbg_log('updates::build_json', 'exit_function', self.oe.LOGDEBUG)
            return update_json
        except Exception as e:
            self.oe.dbg_log('updates::build_json', 'ERROR: (' + repr(e) + ')')

    def get_available_builds(self, shortname=None):
        try:
            self.oe.dbg_log('updates::get_available_builds', 'enter_function', self.oe.LOGDEBUG)
            channel = self.struct['update']['settings']['Channel']['value']
            update_files = []
            build = None
            if not self.update_json is None:
                if channel != '':
                    if channel in self.update_json:
                        regex = re.compile(self.update_json[channel]['prettyname_regex'])
                        if self.oe.ARCHITECTURE in self.update_json[channel]['project']:
                            for i in sorted(self.update_json[channel]['project'][self.oe.ARCHITECTURE]['releases'], key=int, reverse=True):
                                if shortname is None:
                                    update_files.append(regex.findall(self.update_json[channel]['project'][self.oe.ARCHITECTURE]['releases'][i]['file']['name'])[0].strip('.tar'))
                                else:
                                    build = self.update_json[channel]['project'][self.oe.ARCHITECTURE]['releases'][i]['file']['name']
                                    if shortname in build:
                                        break
            self.oe.dbg_log('updates::get_available_builds', 'exit_function', self.oe.LOGDEBUG)
            if build is None:
                return update_files
            else:
                return build
        except Exception as e:
            self.oe.dbg_log('updates::get_available_builds', 'ERROR: (' + repr(e) + ')')

    def fetch_update_json(self):
        try:
            self.oe.dbg_log('updates::fetch_update_json', 'enter_function', 0)

            versions = []
            if self.struct['update']['settings']['Update2NextStable']['value'] == '1':
                if self.oe.LAST_STABLE:
                    versions.append(self.oe.LAST_STABLE)

            if self.oe.BUILDER_VERSION:
                versions.append(self.oe.BUILDER_VERSION)
            else:
                versions.append(self.oe.VERSION)

            for ver in versions:
                url = '%s?i=%s&d=%s&pa=%s&v=%s&f=%s&os=%s' % (
                    self.UPDATE_REQUEST_URL,
                    self.oe.url_quote(self.oe.SYSTEMID),
                    self.oe.url_quote(self.oe.DISTRIBUTION),
                    self.oe.url_quote(self.oe.ARCHITECTURE),
                    self.oe.url_quote(ver),
                    self.oe.url_quote(self.hardware_flags),
                    self.oe.url_quote(self.oe.VERSION_ID),
                    )
                if self.oe.BUILDER_NAME:
                   url += '&b=%s' % self.oe.url_quote(self.oe.BUILDER_NAME)
                if self.struct['update']['settings']['SubmitStats']['value'] == '0':
                   url += '&nostats'

                self.oe.dbg_log('updates::fetch_update_json', 'URL: %s' % url, 0)
                update_json = self.oe.load_url(url)
                if update_json != '':
                    break
            return update_json

        except Exception as e:
            self.oe.dbg_log('updates::fetch_update_json', 'ERROR: (' + repr(e) + ')')

    def check_updates_v2(self, force=False):
        try:
            self.oe.dbg_log('updates::check_updates_v2', 'enter_function', self.oe.LOGDEBUG)
            if hasattr(self, 'update_in_progress'):
                self.oe.dbg_log('updates::check_updates_v2', 'Update in progress (exit)', self.oe.LOGDEBUG)
                return
            update_json = self.fetch_update_json()
            self.oe.dbg_log('updates::check_updates_v2', 'RESULT: %s' % repr(update_json), self.oe.LOGDEBUG)
            if update_json != '':
                update_json = json.loads(update_json)
                self.last_update_check = time.time()
                if 'update' in update_json['data'] and 'folder' in update_json['data']:
                    self.update_file = self.UPDATE_DOWNLOAD_URL % (update_json['data']['host'], update_json['data']['folder'], update_json['data']['update'])
                    if self.struct['update']['settings']['UpdateNotify']['value'] == '1':
                        self.oe.notify(self.oe._(32363), self.oe._(32364))
                    if self.struct['update']['settings']['AutoUpdate']['value'] == 'auto' and force == False:
                        self.update_in_progress = True
                        self.do_autoupdate(None, True)
                    else:
                        if self.oe.BUILD == 'official':
                            if self.struct['update']['settings']['UpdateNotify']['value'] == '1':
                                ceUpdate = xbmcgui.Dialog().yesno('CoreELEC', 'An update is available, would you like to download it now?')
                                if(ceUpdate):
                                    self.update_in_progress = True
                                    self.do_autoupdate(None, True)
            self.oe.dbg_log('updates::check_updates_v2', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('updates::check_updates_v2', 'ERROR: (' + repr(e) + ')')

    def do_autoupdate(self, listItem=None, silent=False):
        try:
            self.oe.dbg_log('updates::do_autoupdate', 'enter_function', self.oe.LOGDEBUG)
            if hasattr(self, 'update_file'):
                if not os.path.exists(self.LOCAL_UPDATE_DIR):
                    os.makedirs(self.LOCAL_UPDATE_DIR)
                downloaded = self.oe.download_file(self.update_file, self.oe.TEMP + 'update_file', silent)
                if not downloaded is None:
                    self.update_file = self.update_file.split('/')[-1]
                    if self.struct['update']['settings']['UpdateNotify']['value'] == '1':
                        self.oe.notify(self.oe._(32363), self.oe._(32366))
                    shutil.move(self.oe.TEMP + 'update_file', self.LOCAL_UPDATE_DIR + self.update_file)
                    subprocess.call('sync', shell=True, stdin=None, stdout=None, stderr=None)
                    ceReboot = xbmcgui.Dialog().yesno('CoreELEC', 'An update has been downloaded, would you like to reboot now to apply it?')
                    if(ceReboot):
                        xbmc.restart()
                    if silent == False:
                        self.oe.winOeMain.close()
                        self.oe.xbmcm.waitForAbort(1)
                        xbmc.executebuiltin('Reboot')
                else:
                    delattr(self, 'update_in_progress')

            self.oe.dbg_log('updates::do_autoupdate', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('updates::do_autoupdate', 'ERROR: (' + repr(e) + ')')

    def get_rpi_flashing_state(self):
        try:
            self.oe.dbg_log('updates::get_rpi_flashing_state', 'enter_function', self.oe.LOGDEBUG)

            jdata = {
                        'EXITCODE': 'EXIT_FAILED',
                        'BOOTLOADER_CURRENT': 0, 'BOOTLOADER_LATEST': 0,
                        'VL805_CURRENT': '', 'VL805_LATEST': ''
                    }

            state = {
                        'incompatible': True,
                        'bootloader': {'state': '', 'current': 'unknown', 'latest': 'unknown'},
                        'vl805': {'state': '', 'current': 'unknown', 'latest': 'unknown'}
                    }

            with tempfile.NamedTemporaryFile(mode='r', delete=True) as machine_out:
                console_output = self.oe.execute('/usr/bin/.rpi-eeprom-update.real -j -m "%s"' % machine_out.name, get_result=1).split('\n')
                if os.path.getsize(machine_out.name) != 0:
                    state['incompatible'] = False
                    jdata = json.load(machine_out)

            self.oe.dbg_log('updates::get_rpi_flashing_state', 'console output: %s' % console_output, self.oe.LOGDEBUG)
            self.oe.dbg_log('updates::get_rpi_flashing_state', 'json values: %s' % jdata, self.oe.LOGDEBUG)

            if jdata['BOOTLOADER_CURRENT'] != 0:
                state['bootloader']['current'] = datetime.datetime.utcfromtimestamp(jdata['BOOTLOADER_CURRENT']).strftime('%Y-%m-%d')

            if jdata['BOOTLOADER_LATEST'] != 0:
                state['bootloader']['latest'] = datetime.datetime.utcfromtimestamp(jdata['BOOTLOADER_LATEST']).strftime('%Y-%m-%d')

            if jdata['VL805_CURRENT']:
                state['vl805']['current'] = jdata['VL805_CURRENT']

            if jdata['VL805_LATEST']:
                state['vl805']['latest'] = jdata['VL805_LATEST']

            if jdata['EXITCODE'] in ['EXIT_SUCCESS', 'EXIT_UPDATE_REQUIRED']:
                if jdata['BOOTLOADER_LATEST'] > jdata['BOOTLOADER_CURRENT']:
                    state['bootloader']['state'] = self.oe._(32028) % (state['bootloader']['current'], state['bootloader']['latest'])
                else:
                    state['bootloader']['state'] = self.oe._(32029) % state['bootloader']['current']

                if jdata['VL805_LATEST'] and jdata['VL805_LATEST'] > jdata['VL805_CURRENT']:
                    state['vl805']['state'] = self.oe._(32028) % (state['vl805']['current'], state['vl805']['latest'])
                else:
                    state['vl805']['state'] = self.oe._(32029) % state['vl805']['current']

            self.oe.dbg_log('updates::get_rpi_flashing_state', 'state: %s' % state, self.oe.LOGDEBUG)
            self.oe.dbg_log('updates::get_rpi_flashing_state', 'exit_function', self.oe.LOGDEBUG)
            return state
        except Exception as e:
            self.oe.dbg_log('updates::get_rpi_flashing_state', 'ERROR: (' + repr(e) + ')')
            return {'incompatible': True}

    def get_rpi_eeprom(self, device):
        try:
            self.oe.dbg_log('updates::get_rpi_eeprom', 'enter_function', self.oe.LOGDEBUG)
            values = []
            if os.path.exists(self.RPI_FLASHING_TRIGGER):
                with open(self.RPI_FLASHING_TRIGGER, 'r') as trigger:
                    values = trigger.read().split('\n')
            self.oe.dbg_log('updates::get_rpi_eeprom', 'values: %s' % values, self.oe.LOGDEBUG)
            self.oe.dbg_log('updates::get_rpi_eeprom', 'exit_function', self.oe.LOGDEBUG)
            return 'true' if ('%s="yes"' % device) in values else 'false'
        except Exception as e:
            self.oe.dbg_log('updates::get_rpi_eeprom', 'ERROR: (' + repr(e) + ')')

    def set_rpi_eeprom(self):
        try:
            self.oe.dbg_log('updates::set_rpi_eeprom', 'enter_function', self.oe.LOGDEBUG)
            bootloader = (self.struct['rpieeprom']['settings']['bootloader']['value'] == 'true')
            vl805 = (self.struct['rpieeprom']['settings']['vl805']['value'] == 'true')
            self.oe.dbg_log('updates::set_rpi_eeprom', 'states: [%s], [%s]' % (bootloader, vl805), self.oe.LOGDEBUG)
            if bootloader or vl805:
                values = []
                values.append('BOOTLOADER="%s"' % ('yes' if bootloader else 'no'))
                values.append('VL805="%s"' % ('yes' if vl805 else 'no'))
                with open(self.RPI_FLASHING_TRIGGER, 'w') as trigger:
                    trigger.write('\n'.join(values))
            else:
                if os.path.exists(self.RPI_FLASHING_TRIGGER):
                    os.remove(self.RPI_FLASHING_TRIGGER)

            self.oe.dbg_log('updates::set_rpi_eeprom', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('updates::set_rpi_eeprom', 'ERROR: (' + repr(e) + ')')

    def set_rpi_bootloader(self, listItem):
        try:
            self.oe.dbg_log('updates::set_rpi_bootloader', 'enter_function', self.oe.LOGDEBUG)
            value = 'false'
            if listItem.getProperty('value') == 'true':
                if xbmcgui.Dialog().yesno('Update RPi Bootloader', '%s\n\n%s' % (self.oe._(32023), self.oe._(32326))):
                    value = 'true'
            self.struct[listItem.getProperty('category')]['settings'][listItem.getProperty('entry')]['value'] = value
            self.set_rpi_eeprom()
            self.oe.dbg_log('updates::set_rpi_bootloader', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('updates::set_rpi_bootloader', 'ERROR: (' + repr(e) + ')')

    def set_rpi_vl805(self, listItem):
        try:
            self.oe.dbg_log('updates::set_rpi_vl805', 'enter_function', self.oe.LOGDEBUG)
            value = 'false'
            if listItem.getProperty('value') == 'true':
                if xbmcgui.Dialog().yesno('Update RPi USB3 Firmware', '%s\n\n%s' % (self.oe._(32023), self.oe._(32326))):
                    value = 'true'
            self.struct[listItem.getProperty('category')]['settings'][listItem.getProperty('entry')]['value'] = value
            self.set_rpi_eeprom()
            self.oe.dbg_log('updates::set_rpi_vl805', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('updates::set_rpi_vl805', 'ERROR: (' + repr(e) + ')')

class updateThread(threading.Thread):

    def __init__(self, oeMain):
        try:
            oeMain.dbg_log('updates::updateThread::__init__', 'enter_function', oeMain.LOGDEBUG)
            self.oe = oeMain
            self.stopped = False
            self.wait_evt = threading.Event()
            threading.Thread.__init__(self)
            self.oe.dbg_log('updates::updateThread', 'Started', self.oe.LOGINFO)
            self.oe.dbg_log('updates::updateThread::__init__', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('updates::updateThread::__init__', 'ERROR: (' + repr(e) + ')')

    def stop(self):
        try:
            self.oe.dbg_log('updates::updateThread::stop()', 'enter_function', self.oe.LOGDEBUG)
            self.stopped = True
            self.wait_evt.set()
            self.oe.dbg_log('updates::updateThread::stop()', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('updates::updateThread::stop()', 'ERROR: (' + repr(e) + ')')

    def run(self):
        try:
            self.oe.dbg_log('updates::updateThread::run', 'enter_function', self.oe.LOGDEBUG)
            while self.stopped == False:
                if not xbmc.Player().isPlaying():
                    self.oe.dictModules['updates'].check_updates_v2()
                if not hasattr(self.oe.dictModules['updates'], 'update_in_progress'):
                    self.wait_evt.wait(21600)
                else:
                    self.oe.notify(self.oe._(32363), self.oe._(32364))
                    self.wait_evt.wait(3600)
                self.wait_evt.clear()
            self.oe.dbg_log('updates::updateThread', 'Stopped', self.oe.LOGINFO)
            self.oe.dbg_log('updates::updateThread::run', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('updates::updateThread::run', 'ERROR: (' + repr(e) + ')')
