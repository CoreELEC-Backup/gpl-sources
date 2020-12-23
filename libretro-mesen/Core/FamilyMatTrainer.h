#pragma once
#include "stdafx.h"
#include "PowerPad.h"

class FamilyMatTrainer : public PowerPad
{
private:
	uint8_t _ignoreRows = 0;

protected:
	void StreamState(bool saving) override
	{
		PowerPad::StreamState(saving);
		Stream(_ignoreRows);
	}

public:
	FamilyMatTrainer(shared_ptr<Console> console, KeyMappingSet keyMappings) : PowerPad(console, BaseControlDevice::ExpDevicePort, keyMappings)
	{
	}

	uint8_t ReadRAM(uint16_t addr) override
	{
		uint8_t output = 0;
		if(addr == 0x4017) {
			uint8_t pressedKeys[4] = {};
			for(int j = 0; j < 3; j++) {
				if((_ignoreRows >> (2 - j)) & 0x01) {
					//Ignore this row
					continue;
				}
				for(int i = 0; i < 4; i++) {
					pressedKeys[i] |= IsPressed(j * 4 + i) ? 1 : 0;
				}
			}
			output = ~((pressedKeys[0] << 4) | (pressedKeys[1] << 3) | (pressedKeys[2] << 2) | (pressedKeys[3] << 1)) & 0x1E;
		}
		return output;
	}

	void WriteRAM(uint16_t addr, uint8_t value) override
	{
		_ignoreRows = value & 0x07;
	}
};