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

import xbmcaddon

addon = xbmcaddon.Addon(id='service.openvfd')

def getSetting(id):
	return addon.getSetting(id)

def getSettingBool(id):
	value = getSetting(id).lower()
	if (value == 'true'):
		value = True
	else:
		value = False
	return value

def getSettingInt(id):
	return int(getSetting(id))

def getSettingNumber(id):
	return float(getSetting(id))

class vfdSettings:
	def __init__(self):
		self.readValues()

	def isDisplayOn(self):
		return self._displayOn

	def isAdvancedSettings(self):
		return self._displayAdvanced

	def getBrightness(self):
		return self._displayBrightness

	def getDisplayType(self):
		return self._displayType

	def getDisplayController(self):
		return self._displayController

	def isCommonAnode(self):
		return self._commonAnode

	def getDisplay(self):
		value = self.getDisplayType() | (self.getDisplayController() << 24)
		if (self.isCommonAnode()):
			value = value | (1 << 16)
		return value

	def getCharacterIndex(self, i):
		return self._characterIndexes[i]

	def getCharacterIndexes(self):
		return self._characterIndexes

	def isColonOn(self):
		return self._colonOn

	def getModeTempInterval(self):
		return self._modeTempInterval

	def getModeTempDuration(self):
		return self._modeTempDuration

	def getModeDateInterval(self):
		return self._modeDateInterval

	def getModeDateDuration(self):
		return self._modeDateDuration

	def getModeDateFormat(self):
		return self._modeDateFormat

	def isPlaybackTimeEnabled(self):
		return self._modePlaybackTimeEnabled

	def getModePlaybackTimeDuration(self):
		return self._modePlaybackTimeDuration

	def getModePlaybackTimeBehavior(self):
		return self._modePlaybackTimeBehavior

	def isHdmiIndicatorEnabled(self):
		return self._hdmiIndicator

	def isCvbsIndicatorEnabled(self):
		return self._cvbsIndicator

	def isEthIndicatorEnabled(self):
		return self._ethIndicator

	def isWifiIndicatorEnabled(self):
		return self._wifiIndicator

	def isSetupIndicatorEnabled(self):
		return self._setupIndicator

	def isAppsIndicatorEnabled(self):
		return self._appsIndicator

	def isUsbIndicatorEnabled(self):
		return self._usbIndicator

	def isSdIndicatorEnabled(self):
		return self._sdIndicator

	def isPowerIndicatorEnabled(self):
		return self._powerIndicator

	def isStorageIndicatorEnabled(self):
		return self._storageIndicator

	def getStorageIndicatorIcon(self):
		return self._storageIndicatorIcon

	def readValues(self):
		self._modeTempInterval = vfdSettings.__modeTempIntervalValues.get(getSettingInt('mode.temperature.interval'), 0)
		self._modeTempDuration = vfdSettings.__modeTempDurationValues.get(getSettingInt('mode.temperature.duration'), 5)
		self._modePlaybackTimeEnabled  = getSettingBool('mode.playback.on')
		self._modePlaybackTimeDuration = vfdSettings.__modePlaybackTimeDuration.get(getSettingInt('mode.playback.duration'), 0)
		self._modePlaybackTimeBehavior = getSettingInt('mode.playback.behavior')
		self._modeDateInterval = vfdSettings.__modeTempIntervalValues.get(getSettingInt('mode.date.interval'), 0)
		self._modeDateDuration = vfdSettings.__modeTempDurationValues.get(getSettingInt('mode.date.duration'), 5)
		self._modeDateFormat   = getSettingInt('mode.date.format')
		self._displayAdvanced = False
		self._displayOn = getSettingBool('display.on')
		if (self._displayOn):
			self._displayBrightness = getSettingInt('display.brightness')
			self._hdmiIndicator = getSettingBool('display.hdmi.indicator')
			self._cvbsIndicator = getSettingBool('display.cvbs.indicator')
			self._ethIndicator = getSettingBool('display.eth.indicator')
			self._wifiIndicator = getSettingBool('display.wifi.indicator')
			self._setupIndicator = getSettingBool('display.setup.indicator')
			self._appsIndicator = getSettingBool('display.apps.indicator')
			self._usbIndicator = getSettingBool('display.usb.indicator')
			self._sdIndicator = getSettingBool('display.sd.indicator')
			self._powerIndicator = getSettingBool('display.power.indicator')
			self._storageIndicator = getSettingBool('display.storage.indicator')
			self._storageIndicatorIcon = getSetting('display.storage.indicator.icon')
			self._colonOn = getSettingBool('display.colon.on')
			self._displayAdvanced = getSettingBool('display.advanced')
			if (self._displayAdvanced):
				self._displayType = getSettingInt('display.type')
				self._displayController = getSettingInt('display.controller')
				self._commonAnode = getSettingBool('display.common.anode')
				self._characterIndexes = []
				for i in range(7):
					self._characterIndexes.append(getSettingInt('display.char.index{0}'.format(i)))
			else:
				self.__initDefaultValues()
		else:
			self.__initDefaultValues()

	def __initDefaultValues(self):
		if not (self._displayOn):
			self._displayBrightness = 7
			self._hdmiIndicator = True
			self._cvbsIndicator = True
			self._ethIndicator = True
			self._wifiIndicator = True
			self._setupIndicator = True
			self._appsIndicator = True
			self._usbIndicator = True
			self._sdIndicator = True
			self._powerIndicator = True
			self._storageIndicator = False
			self._storageIndicatorIcon = ''
			self._colonOn = False
			self._displayAdvanced = False
		if not (self._displayAdvanced):
			self._displayType = 0
			self._displayController = 0
			self._commonAnode = False
			self._characterIndexes = list(range(0, 7))

	__modeTempIntervalValues = { 0:0, 1:30, 2:60, 3:300, 4:600, 5:900, 6:1800, 7:3600 }
	__modeTempDurationValues = { 0:5, 1:10, 2:15, 3:30 }
	__modePlaybackTimeDuration = { 0:5, 1:10, 2:15, 3:30, 4:0 }
