#pragma once
#include "stdafx.h"
#include "MMC3.h"

class MMC3_52 : public MMC3
{
private:
	uint8_t _extraReg = 0;

protected:
	virtual uint16_t RegisterStartAddress() override { return 0x6000; }
	virtual uint16_t RegisterEndAddress() override { return 0xFFFF; }

	virtual void StreamState(bool saving) override
	{
		MMC3::StreamState(saving);
		Stream(_extraReg);
	}

	virtual void Reset(bool softReset) override
	{
		_extraReg = 0;
		UpdateState();
	}

	virtual void SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::Default) override
	{
		if(_extraReg & 0x40) {
			page &= 0x7F;
			page |= ((_extraReg & 0x04) | ((_extraReg >> 4) & 0x03)) << 7;
		} else {
			page &= 0xFF;
			page |= ((_extraReg & 0x04) | ((_extraReg >> 4) & 0x02)) << 7;
		}
		MMC3::SelectCHRPage(slot, page, memoryType);
	}

	virtual void SelectPRGPage(uint16_t slot, uint16_t page, PrgMemoryType memoryType = PrgMemoryType::PrgRom) override
	{
		if(_extraReg & 0x08) {
			page &= 0x0F;
			page |= (_extraReg & 0x07) << 4;
		} else {
			page &= 0x1F;
			page |= (_extraReg & 0x06) << 4;
		}
		MMC3::SelectPRGPage(slot, page, memoryType);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(addr < 0x8000) {
			if(CanWriteToWorkRam()) {
				if((_extraReg & 0x80) == 0) {
					_extraReg = value;
					UpdateState();
				} else {
					BaseMapper::WritePrgRam(addr, value);
				}
			}
		} else {
			MMC3::WriteRegister(addr, value);
		}
	}
};