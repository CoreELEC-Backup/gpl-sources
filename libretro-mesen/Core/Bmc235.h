#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Bmc235 : public BaseMapper
{
private:
	bool _openBus = false;
protected:
	virtual uint16_t GetPRGPageSize() override { return 0x4000; }
	virtual uint16_t GetCHRPageSize() override { return 0x2000; }

	void InitMapper() override
	{
		SelectPrgPage2x(0, 0);
		SelectCHRPage(0, 0);
	}

	void Reset(bool softReset) override
	{
		SelectPrgPage2x(0, 0);
		_openBus = false;
	}

	void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		Stream(_openBus);
		if(!saving && _openBus) {
			RemoveCpuMemoryMapping(0x8000, 0xFFFF);
		}
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		SetMirroringType((addr & 0x0400) ? MirroringType::ScreenAOnly : (addr & 0x2000) ? MirroringType::Horizontal : MirroringType::Vertical);

		const uint8_t config[4][4][2] = {
			{ { 0x00, 0 }, { 0x00, 1 }, { 0x00, 1 }, { 0x00, 1 } },
			{ { 0x00, 0 }, { 0x00, 1 }, { 0x20, 0 }, { 0x00, 1 } },
			{ { 0x00, 0 }, { 0x00, 1 }, { 0x20, 0 }, { 0x40, 0 } },
			{ { 0x00, 0 }, { 0x20, 0 }, { 0x40, 0 }, { 0x60, 0 } }
		};

		uint8_t mode;
		switch(GetPRGPageCount()) {
			case 64: mode = 0; break;
			case 128: mode = 1; break;
			case 256: mode = 2; break;
			default: mode = 3; break;
		};		

		uint8_t bank = config[mode][addr >> 8 & 0x03][0] | (addr & 0x1F);
		
		_openBus = false;
		if(config[mode][addr >> 8 & 0x03][1]) {
			//Open bus
			_openBus = true;
			RemoveCpuMemoryMapping(0x8000, 0xFFFF);
		} else if(addr & 0x800) {
			bank = (bank << 1) | (addr >> 12 & 0x01);
			SelectPRGPage(0, bank);
			SelectPRGPage(1, bank);
		} else {
			SelectPrgPage2x(0, bank << 1);
		}
	}
};