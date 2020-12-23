#pragma once
#include "MMC1.h"

class FaridSlrom : public MMC1
{
private:
	uint8_t _outerBank;
	bool _locked;

protected:
	void InitMapper() override
	{
		AddRegisterRange(0x6000, 0x7FFF, MemoryOperation::Write);
		MMC1::InitMapper();
	}

	void Reset(bool softReset) override
	{
		MMC1::Reset(softReset);

		_outerBank = 0;
		_locked = false;
		UpdateState();
	}

	void StreamState(bool saving) override
	{
		MMC1::StreamState(saving);
		Stream(_outerBank, _locked);
	}

	void SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType) override
	{
		MMC1::SelectCHRPage(slot, (_outerBank << 2) | (page & 0x1F), memoryType);
	}

	void SelectPRGPage(uint16_t slot, uint16_t page, PrgMemoryType memoryType) override
	{
		MMC1::SelectPRGPage(slot, _outerBank | (page & 0x07), memoryType);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(addr < 0x8000) {
			bool wramEnabled = (_state.RegE000 & 0x10) == 0;
			if(wramEnabled && !_locked) {
				_outerBank = (value & 0x70) >> 1;
				_locked = (value & 0x08) == 0x08;
				UpdateState();
			}
		} else {
			MMC1::WriteRegister(addr, value);
		}
	}
};