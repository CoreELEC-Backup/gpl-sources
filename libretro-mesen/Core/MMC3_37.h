#pragma once
#include "stdafx.h"
#include "MMC3.h"

class MMC3_37 : public MMC3
{
private:
	uint8_t _selectedBlock = 0;

protected:
	virtual uint16_t RegisterStartAddress() override { return 0x6000; }
	virtual uint16_t RegisterEndAddress() override { return 0xFFFF; }

	virtual void StreamState(bool saving) override
	{
		MMC3::StreamState(saving);
		Stream(_selectedBlock);
	}

	virtual void Reset(bool softReset) override
	{
		_selectedBlock = 0;
		UpdateState();
	}

	virtual void SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::Default) override
	{
		if(_selectedBlock >= 4) {
			page |= 0x80;
		}

		MMC3::SelectCHRPage(slot, page, memoryType);
	}

	virtual void SelectPRGPage(uint16_t slot, uint16_t page, PrgMemoryType memoryType = PrgMemoryType::PrgRom) override
	{
		if(_selectedBlock <= 2) {
			page &= 0x07;
		} else if(_selectedBlock == 3) {
			page &= 0x07;
			page |= 0x08;
		} else if(_selectedBlock == 7) {
			page &= 0x07;
			page |= 0x20;
		} else if(_selectedBlock >= 4) {
			page &= 0x0F;
			page |= 0x10;
		}

		MMC3::SelectPRGPage(slot, page, memoryType);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(addr < 0x8000) {
			if(CanWriteToWorkRam()) {
				_selectedBlock = value & 0x07;
				UpdateState();
			}
		} else {
			MMC3::WriteRegister(addr, value);
		}
	}
};