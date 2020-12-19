#!/usr/bin/env bash

# This file is part of PulseAudio.
#
# PulseAudio is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# PulseAudio is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with PulseAudio; if not, see <http://www.gnu.org/licenses/>.

if [ -f .git/hooks/pre-commit.sample -a ! -f .git/hooks/pre-commit ] ; then
    cp -p .git/hooks/pre-commit.sample .git/hooks/pre-commit && \
    chmod +x .git/hooks/pre-commit && \
    echo "Activated pre-commit hook."
fi

if [ -f .tarball-version ]; then
    echo "Marking tarball version as modified."
    echo -n `cat .tarball-version | sed 's/-rebootstrapped$//'`-rebootstrapped >.tarball-version
fi

# We check for this here, because if pkg-config is not found in the
# system, it's likely that the pkg.m4 macro file is also not present,
# which will make PKG_PROG_PKG_CONFIG be undefined and the generated
# configure file faulty.
if ! pkg-config --version &>/dev/null; then
    echo "pkg-config is required to bootstrap this program"
    exit 1
fi
# Other necessary programs
if ! autopoint --version &>/dev/null ; then
    echo "autopoint is required to bootstrap this program"
    exit 1
fi

autoreconf --force --install --verbose

if test "x$NOCONFIGURE" = "x"; then
    CFLAGS="$CFLAGS -g -O0" ./configure --enable-force-preopen "$@" && \
        make clean
fi
