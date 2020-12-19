# SPDX-License-Identifier: GPL-2.0-or-later
# Copyright (C) 2009-2013 Stephan Raue (stephan@openelec.tv)
# Copyright (C) 2013 Lutz Fiebach (lufie@openelec.tv)
# Copyright (C) 2019-present Team LibreELEC (https://libreelec.tv)

class about:

    ENABLED = False
    menu = {'99': {
        'name': 32196,
        'menuLoader': 'menu_loader',
        'listTyp': 'other',
        'InfoText': 705,
        }}

    def __init__(self, oeMain):
        try:
            oeMain.dbg_log('about::__init__', 'enter_function', oeMain.LOGDEBUG)
            self.oe = oeMain
            self.controls = {}
            self.oe.dbg_log('about::__init__', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('about::__init__', 'ERROR: (' + repr(e) + ')')

    def menu_loader(self, menuItem):
        try:
            self.oe.dbg_log('about::menu_loader', 'enter_function', self.oe.LOGDEBUG)
            if len(self.controls) == 0:
                self.init_controls()
            self.oe.dbg_log('about::menu_loader', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('about::menu_loader', 'ERROR: (' + repr(e) + ')', self.oe.LOGERROR)

    def exit_addon(self):
        try:
            self.oe.dbg_log('about::exit_addon', 'enter_function', self.oe.LOGDEBUG)
            self.oe.winOeMain.close()
            self.oe.dbg_log('about::exit_addon', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('about::exit_addon', 'ERROR: (' + repr(e) + ')')

    def init_controls(self):
        try:
            self.oe.dbg_log('about::init_controls', 'enter_function', self.oe.LOGDEBUG)
            self.oe.dbg_log('about::init_controls', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('about::init_controls', 'ERROR: (' + repr(e) + ')')

    def exit(self):
        try:
            self.oe.dbg_log('about::exit', 'enter_function', self.oe.LOGDEBUG)
            for control in self.controls:
                try:
                    self.oe.winOeMain.removeControl(self.controls[control])
                except:
                    pass
            self.controls = {}
            self.oe.dbg_log('about::exit', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('about::exit', 'ERROR: (' + repr(e) + ')')

    def do_wizard(self):
        try:
            self.oe.dbg_log('about::do_wizard', 'enter_function', self.oe.LOGDEBUG)
            self.oe.winOeMain.set_wizard_title(self.oe._(32317))
            self.oe.winOeMain.set_wizard_text(self.oe._(32318))
            self.oe.dbg_log('about::do_wizard', 'exit_function', self.oe.LOGDEBUG)
        except Exception as e:
            self.oe.dbg_log('about::do_wizard', 'ERROR: (' + repr(e) + ')')
