# Copyright 2012,2013 Christoph Reiter
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.

from __future__ import print_function

import os
import sys
import inspect
import unittest
import logging
import platform

# https://pypi.python.org/pypi/faulthandler/
try:
    import faulthandler
except ImportError:
    pass
else:
    faulthandler.enable()


_gi_version = (-1,)
_is_gi = False
_is_pypy = False
_has_cairo = False
_fixme = {}

GIOverflowError = None


def skipUnlessGIVersionAtLeast(*version):
    return unittest.skipIf(_is_gi and _gi_version < version, "gi too old")


def skipIfGIVersion(*version):
    sub = _gi_version[:len(version)]
    return unittest.skipIf(_is_gi and sub == version, "gi version broken")


def skipIfPy3(func):
    return unittest.skipIf(sys.version_info[0] == 3, "skipped on python 3")


def skipIfGI(func):
    if callable(func):
        return unittest.skipIf(_is_gi, "not supported by gi")(func)
    else:
        from pgi import _compat
        assert isinstance(func, _compat.string_types)

        def wrap(f):
            return unittest.skipIf(_is_gi, func)(f)
        return wrap


def skipIfPyPy(message):

    def wrap(f):
        _fixme[f] = "PyPy: %s" % message
        return unittest.skipIf(_is_pypy, message)(f)
    return wrap


def skipUnlessCairo(func):
    return unittest.skipUnless(_has_cairo, "not cairo")(func)


def FIXME(func):
    if callable(func):
        _fixme[func] = None
        return unittest.skip("FIXME")(func)
    else:
        from pgi import _compat
        assert isinstance(func, _compat.string_types)

        def wrap(f):
            _fixme[f] = func
            return unittest.skip("FIXME")(f)
        return wrap


def discover(base, dir_):
    """Discover test subclasses.

    base gets added to sys.path; dir_ is the package name.
    """

    test_classes = []

    sys.path.insert(0, base)
    for entry in os.listdir(os.path.join(base, dir_)):
        if not entry.startswith("test_") or not entry.endswith(".py"):
            continue
        mod = entry[:-3]
        if dir_:
            mod = dir_ + "." + mod
            loaded = getattr(__import__(mod), entry[:-3])
        else:
            loaded = __import__(mod)

        var = vars(loaded)

        if "__all__" in var:
            var = var["__all__"]

        for key in var:
            value = getattr(loaded, key)
            if key.startswith("_"):
                continue
            if not inspect.isclass(value):
                continue
            if issubclass(value, unittest.TestCase):
                test_classes.append(value)
    sys.path.pop(0)

    return test_classes


def test_pep8():
    """Run pep8 pyflakes tests"""

    import pgi
    pgi.install_as_gi()

    current_dir = os.path.join(os.path.dirname(__file__))
    tests = discover(current_dir, "misc")
    tests = [unittest.makeSuite(t) for t in tests]

    run = unittest.TextTestRunner(verbosity=2).run(unittest.TestSuite(tests))

    return len(run.failures) + len(run.errors)


def test(load_gi, backend=None, strict=False, filter_=None, failfast=False):
    """Run the test suite.

    load_gi -- run all tests in the pygobject suite with PyGObject
    backend -- "ctypes" or "cffi"
    strict  -- fail on glib warnings
    filter_ -- filter for test names (class names)
    """

    global _is_gi, _is_pypy, _has_cairo, _gi_version, GIOverflowError

    _is_gi = load_gi
    _is_pypy = platform.python_implementation() == "PyPy"
    _has_cairo = True

    if not load_gi:
        try:
            import cairocffi
            cairocffi.install_as_pycairo()
        except (ImportError, OSError):
            _has_cairo = False
        import pgi
        pgi.install_as_gi()
        try:
            pgi.set_backend(backend)
        except LookupError:
            print("Couldn't load backend: %r" % backend)
            return

    def headline(text):
        return (("### %s " % text) + "#" * 80)[:80]

    import gi

    TYPELIBS = {
        "Gtk": "3.0",
        "Gdk": "3.0",
        "Clutter": "1.0",
        "Regress": "1.0",
        "GIMarshallingTests": "1.0",
        "PangoCairo": "1.0"
    }

    for name, version in TYPELIBS.items():
        try:
            gi.require_version(name, version)
        except ValueError:
            pass

    if load_gi:
        assert gi.__name__ == "gi"
        try:
            _gi_version = gi.version_info
        except AttributeError:
            _gi_version = gi._gobject.pygobject_version

        if _gi_version < (3, 10):
            GIOverflowError = ValueError
        else:
            GIOverflowError = OverflowError

        hl = headline("GI")
    else:
        assert gi.__name__ == "pgi"
        if backend:
            hl = headline("PGI (%s)" % backend)
        else:
            hl = headline("PGI")

        GIOverflowError = OverflowError
    print(hl[:80])

    # gi uses logging
    logging.disable(logging.ERROR)

    if strict:
        # make glib warnings fatal
        from gi.repository import GLib
        GLib.log_set_always_fatal(
            GLib.LogLevelFlags.LEVEL_CRITICAL |
            GLib.LogLevelFlags.LEVEL_ERROR |
            GLib.LogLevelFlags.LEVEL_WARNING)

    current_dir = os.path.join(os.path.dirname(__file__))
    tests = []
    tests = discover(current_dir, "tests_pygobject")
    tests += discover(current_dir, "tests_mixed")
    if not load_gi:
        tests.extend(discover(current_dir, "tests_pgi"))

    if filter_ is not None:
        tests = filter(lambda t: filter_(t.__name__), tests)

    tests = [unittest.makeSuite(t) for t in tests]

    # only in case all get run, so filtered results don't get spammed
    if filter_ is None:
        # collected by the FIXME decorator
        print(headline("FIXME"))
        for item, desc in sorted(_fixme.items(), key=lambda x: repr(x)):
            print(" -> %s.%s" % (item.__module__, item.__name__), end="")
            if desc:
                print("(%s)" % desc)
            else:
                print()

    run = unittest.TextTestRunner(
        verbosity=2, failfast=failfast).run(unittest.TestSuite(tests))

    return len(run.failures) + len(run.errors)
