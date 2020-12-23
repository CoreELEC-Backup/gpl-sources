#pragma once
#include "stdafx.h"
#include "MMC3.h"

class MMC3_MaliSB : public MMC3
{
protected:
	void SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::Default) override
	{
		MMC3::SelectCHRPage(slot, (page & 0xDD) | ((page & 0x20) >> 4) | ((page & 0x02) << 4), memoryType);
	}

	void SelectPRGPage(uint16_t slot, uint16_t page, PrgMemoryType memoryType = PrgMemoryType::PrgRom) override
	{
		MMC3::SelectPRGPage(slot, (page & 0x03) | ((page & 0x08) >> 1) | ((page & 0x04) << 1), memoryType);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(addr >= 0xC000) {
			addr = (addr & 0xFFFE) | ((addr >> 2) & 0x01) | ((addr >> 3) & 0x01);
		} else {
			addr = (addr & 0xFFFE) | ((addr >> 3) & 0x01);
		}
		MMC3::WriteRegister(addr, value);
	}
};