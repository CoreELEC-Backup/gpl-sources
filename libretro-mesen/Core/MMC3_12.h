#pragma once
#include "stdafx.h"
#include "MMC3.h"

class MMC3_12 : public MMC3
{
private:
	uint8_t _chrSelection = 0;

protected:
	virtual bool ForceMmc3RevAIrqs() override { return true; }

	virtual void InitMapper() override
	{
		AddRegisterRange(0x4020, 0x5FFF);
		MMC3::InitMapper();
	}

	virtual void StreamState(bool saving) override
	{
		MMC3::StreamState(saving);
		Stream(_chrSelection);
	}

	virtual void SelectCHRPage(uint16_t slot, uint16_t page, ChrMemoryType memoryType = ChrMemoryType::Default) override
	{
		if(slot < 4 && (_chrSelection & 0x01)) {
			//0x0000 to 0x0FFF
			page |= 0x100;
		} else if(slot >= 4 && (_chrSelection & 0x10)) {
			//0x1000 to 0x1FFF
			page |= 0x100;
		}

		MMC3::SelectCHRPage(slot, page, memoryType);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(addr <= 0x5FFF) {
			_chrSelection = value;
			UpdateState();
		} else {
			MMC3::WriteRegister(addr, value);
		}
	}
};