#pragma once
#include "stdafx.h"
#include "../Utilities/FolderUtilities.h"
#include "Console.h"
#include "BaseControlDevice.h"
#include "IBattery.h"
#include "BatteryManager.h"

class BattleBox : public BaseControlDevice, public IBattery
{
private:
	static constexpr int FileSize = 0x200;
	uint8_t _lastWrite = 0;
	uint8_t _address = 0;
	uint8_t _chipSelect = 0;
	uint16_t _data[BattleBox::FileSize/2];
	uint8_t _output = 0;
	bool _writeEnabled = false;

	uint8_t _inputBitPosition = 0;
	uint16_t _inputData = 0;
	bool _isWrite = false;
	bool _isRead = false;

protected:
	void StreamState(bool saving) override
	{
		BaseControlDevice::StreamState(saving);
		ArrayInfo<uint8_t> data{ (uint8_t*)_data, BattleBox::FileSize };
		Stream(_lastWrite, _address, _chipSelect, _output, _writeEnabled, _inputBitPosition, _isWrite, _isRead, _inputData, data);
	}

public:
	BattleBox(shared_ptr<Console> console) : BaseControlDevice(console, BaseControlDevice::ExpDevicePort)
	{
		_console->GetBatteryManager()->LoadBattery(".bb", (uint8_t*)_data, BattleBox::FileSize);
	}

	~BattleBox()
	{
		SaveBattery();
	}

	void SaveBattery() override
	{
		_console->GetBatteryManager()->SaveBattery(".bb", (uint8_t*)_data, BattleBox::FileSize);
	}
	
	uint8_t ReadRAM(uint16_t addr) override
	{
		if(addr == 0x4017) {
			if(_lastWrite & 0x01) {
				_chipSelect ^= 0x01;
				_inputData = 0;
				_inputBitPosition = 0;
			}
			_output ^= 0x01;

			uint8_t readBit = 0;
			if(_isRead) {
				readBit = ((_data[(_chipSelect ? 0x80 : 0) | _address] >> _inputBitPosition) & 0x01) << 3;
			}
			uint8_t writeBit = (_output << 4);
			return readBit | writeBit;
		}
		return 0;
	}

	void WriteRAM(uint16_t addr, uint8_t value) override
	{
		if(value & 0x01 && !(_lastWrite & 0x01)) {
			//Clock
			_inputData &= ~(1 << _inputBitPosition);
			_inputData |= (_output << _inputBitPosition);
			_inputBitPosition++;

			if(_inputBitPosition > 15) {
				if(_isWrite) {
					_data[(_chipSelect ? 0x80 : 0) | _address] = _inputData;
					_isWrite = false;
				} else {
					_isRead = false;

					//done reading addr/command or write data
					uint8_t address = (_inputData & 0x7F);

					uint8_t cmd = ((_inputData & 0x7F00) >> 8) ^ 0x7F;
					switch(cmd) {
						case 0x01:
							//read
							_address = address;
							_isRead = true;
							break;
						case 0x06:
							//program
							if(_writeEnabled) {
								_address = address;
								_isWrite = true;
							}
							break;
						case 0x0C:
							//chip erase
							if(_writeEnabled) {
								memset(_data, 0, BattleBox::FileSize);
							}
							break;

						case 0x0D: break; //busy monitor
						case 0x09: _writeEnabled = true; break; //erase/write enable
						case 0x0B: _writeEnabled = false; break; //erase/write disable
					}
				}
				_inputBitPosition = 0;
			}
		}
		_lastWrite = value;
	}
};