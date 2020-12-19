# Copyright 2013 Christoph Reiter
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 2 as
# published by the Free Software Foundation

import os
import re
import glob
import sys
import subprocess
import unittest

from pgi import _compat

os.environ["PYFLAKES_NODOCTEST"] = "1"
try:
    from pyflakes.scripts import pyflakes
except ImportError:
    pyflakes = None


class Error(object):
    IMPORT_UNUSED = "imported but unused"
    REDEF_FUNCTION = "redefinition of function"
    UNABLE_DETECT_UNDEF = "unable to detect undefined names"
    UNDEFINED_PY2_NAME = \
        "undefined name '(unicode|long|basestring|xrange|cmp)'"


class FakeStream(object):
    # skip these by default
    BL = [Error.UNABLE_DETECT_UNDEF]
    if _compat.PY3:
        BL.append(Error.UNDEFINED_PY2_NAME)

    def __init__(self, blacklist=None):
        self.lines = []
        if blacklist is None:
            blacklist = []
        self.bl = self.BL[:] + blacklist

    def write(self, text):
        for p in self.bl:
            if re.search(p, text):
                return
        text = text.strip()
        if not text:
            return
        self.lines.append(text)

    def check(self):
        if self.lines:
            raise Exception("\n" + "\n".join(self.lines))


@unittest.skipUnless(pyflakes, "no pyflakes found")
class TPyFlakes(unittest.TestCase):

    def _run(self, path, **kwargs):
        old_stdout = sys.stdout
        stream = FakeStream(**kwargs)
        try:
            sys.stdout = stream
            for dirpath, dirnames, filenames in os.walk(path):
                for filename in filenames:
                    if filename.endswith('.py'):
                        pyflakes.checkPath(os.path.join(dirpath, filename))
        finally:
            sys.stdout = old_stdout
        stream.check()

    def _run_package(self, mod, *args, **kwargs):
        path = mod.__path__[0]
        self._run(path, *args, **kwargs)

    def test_main(self):
        import pgi
        self._run_package(pgi)

    def test_tests_mixed(self):
        import tests.tests_mixed
        self._run_package(tests.tests_mixed)

    def test_tests_pgi(self):
        import tests.tests_pgi
        self._run_package(tests.tests_pgi)

    def test_tests_pygobject(self):
        import tests.tests_pygobject
        self._run_package(tests.tests_pygobject,
                          blacklist=[Error.IMPORT_UNUSED])
