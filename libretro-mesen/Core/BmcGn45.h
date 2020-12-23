#pragma once
#include "stdafx.h"
#include "MMC3.h"

class BmcGn45 : public MMC3
{
private:
	uint8_t _selectedBlock = 0;
	bool _wramEnabled = false;

protected:
	uint16_t RegisterStartAddress() override { return 0x6000; }
	uint16_t RegisterEndAddress() override { return 0xFFFF; }

	void StreamState(bool saving) override
	{
		MMC3::StreamState(saving);
		Stream(_selectedBlock, _wramEnabled);
	}

	void Reset(bool softReset) override
	{
		MMC3::Reset(softReset);

		if(softReset) {
			_selectedBlock = 0;
			_wramEnabled = false;
			ResetMmc3();
			UpdateState();
		}
	}

	void SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::Default) override
	{
		MMC3::SelectCHRPage(slot, (page & 0x7F) | (_selectedBlock << 3), memoryType);
	}

	void SelectPRGPage(uint16_t slot, uint16_t page, PrgMemoryType memoryType = PrgMemoryType::PrgRom) override
	{
		MMC3::SelectPRGPage(slot, (page & 0x0F) | _selectedBlock, memoryType);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(addr < 0x7000) {
			if(!_wramEnabled) {
				_selectedBlock = addr & 0x30;
				_wramEnabled = (addr & 0x80) != 0;
				UpdateState();
			} else {
				WritePrgRam(addr, value);
			}
		} else if(addr < 0x8000) {
			if(!_wramEnabled) {
				_selectedBlock = value & 0x30;
				UpdateState();
			} else {
				WritePrgRam(addr, value);
			}
		} else {
			MMC3::WriteRegister(addr, value);
		}
	}
};