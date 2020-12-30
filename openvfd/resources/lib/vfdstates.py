################################################################################
#      Copyright (C) 2018 Arthur Liberman (arthur_liberman (at) hotmail.com)
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
################################################################################

import xbmc
import os
from .vfdutils import *

class vfdState(object):
	def __init__(self, ledName):
		self._value = False
		self._hasChanged = False
		self._ledName = ledName

	def _getStr(self, className):
		return '{0} ({1})'.format(className, self._ledName)

	def update(self):
		raise NotImplementedError

	def getValue(self):
		return self._value

	def hasChanged(self):
		return self._hasChanged

	def getLedName(self):
		return self._ledName

	def _update(self, value):
		if (value != self._value):
			self._hasChanged = True
			self._value = value
		else:
			self._hasChanged = False

class vfdIconIndicator(vfdState):
	def __init__(self, on, ledName):
		super(vfdIconIndicator, self).__init__(ledName)
		self._on = on

	def __str__(self):
		return self._getStr('vfdIconIndicator')

	def turnOn(self):
		self._on = True

	def turnOff(self):
		self._on = False

	def toggle(self):
		self._on = not self._on

	def update(self):
		self._update(self._on)

class vfdCondVisibility(vfdState):
	def __init__(self, ledName, cmd):
		super(vfdCondVisibility, self).__init__(ledName)
		self._cmd = cmd

	def __str__(self):
		return self._getStr('vfdCondVisibility')

	def update(self):
		value = xbmc.getCondVisibility(self._cmd)
		self._update(value)

class vfdFileContains(vfdState):
	def __init__(self, ledName, path, strings):
		super(vfdFileContains, self).__init__(ledName)
		self._path = path
		self._strings = strings

	def __str__(self):
		return self._getStr('vfdFileContains')

	def update(self):
		if (os.path.isfile(self._path)):
			with open(self._path, 'rb') as state:
				content = state.read()
			value = self.__checkContent(content)
			self._update(value)
		else:
			self._update(False)

	def __checkContent(self, content):
		ret = False
		for s in self._strings:
			if (s.encode() in content):
				ret = True
				break
		return ret

class vfdNetworkChecker(vfdState):
	def __init__(self, ledName, prefix, strings):
		super(vfdNetworkChecker, self).__init__(ledName)
		self._prefix = prefix
		self._strings = strings
		self._files = []

	def __str__(self):
		return self._getStr('vfdNetworkChecker')

	def update(self):
		value = False
		self.__updateInterfaces()
		for inet in self._files:
			inet.update()
			if inet.getValue():
				value = True
				break
		self._update(value)

	def __updateInterfaces(self):
		for folder, subs, files in os.walk('/sys/class/net'):
			for sub in subs:
				if sub.startswith(self._prefix) and all(sub != inet.getLedName() for inet in self._files):
					path = os.path.realpath(os.path.join(folder, sub, 'operstate'))
					self._files.append(vfdFileContains(sub, path, self._strings))

class vfdWindowChecker(vfdState):
	def __init__(self, ledName, windows):
		super(vfdWindowChecker, self).__init__(ledName)
		self._windows = windows

	def __str__(self):
		return self._getStr('vfdWindowChecker')

	def update(self):
		value = False
		for id in self._windows:
			if (xbmc.getCondVisibility('Window.IsVisible({0})'.format(id))):
				value = True
				break
		self._update(value)

class vfdExtStorageChecker(vfdState):
	def __init__(self, ledName, path):
		super(vfdExtStorageChecker, self).__init__(ledName)
		self._path = path

	def __str__(self):
		return self._getStr('vfdExtStorageChecker')

	def update(self):
		value = False
		for folder, subs, files in os.walk('/dev/disk/by-uuid'):
			for filename in files:
				path = os.path.realpath(os.path.join(folder, filename))
				if (path.startswith(self._path)):
					value = True
					break
		self._update(value)

class vfdExtStorageCount(vfdState):
	def __init__(self, ledName, drives, type):
		super(vfdExtStorageCount, self).__init__(ledName)
		if (drives == None):	# Monitor all drives
			self._drives = None
			drives = self.__getAllDrives()
		else:			# Monitor listed drives
			self._drives = drives
			drives = self.__getSelectedDrives()
		self._driveStats = {key: self.__readStatus(key) for key in drives}
		kodiLogNotice('vfdExtStorageCount.__init__: Drive stats ' + str(self._driveStats))
		self._read = False
		self._write = False
		if (type == 'r'):
			self._read = True
		elif (type == 'w'):
			self._write = True
		elif (type == 'rw'):
			self._read = True
			self._write = True
		else:
			raise Exception('\'type\' must be \'r\', \'w\' or \'rw\'.')

	def update(self):
		value = False
		if (self._drives == None):
			drives = self.__getAllDrives()
		else:
			drives = self.__getSelectedDrives()
		for drive in drives:
			if (not drive in self._driveStats):
				self._driveStats[drive] = None
				kodiLogNotice('vfdExtStorageCount.update: New drive found \'{0}\''.format(drive))
		for path, stats in list(self._driveStats.items()):
			newStats = self.__readStatus(path)
			if (stats != None and newStats != None):
				if (self._read):
					value = value or stats[0] != newStats[0]
				if (self._write):
					value = value or stats[1] != newStats[1]
			self._driveStats[path] = newStats
		self._update(value)

	def __readStatus(self, path):
		path = os.path.join('/sys/block', path, 'stat')
		if (os.path.isfile(path)):
			with open(path, 'rb') as status:
				values = status.read().split()
			return (values[2], values[6])
		else:
			return None

	def __getAllDrives(self):
		drives = []
		for folder, subs, files in os.walk('/sys/block'):
			drives = [sub for sub in subs if (not sub.startswith('loop'))]
		return drives

	def __getSelectedDrives(self):
		return [drive for drive in self.__getAllDrives() if ([d for d in self._drives if drive.startswith(d)])]
