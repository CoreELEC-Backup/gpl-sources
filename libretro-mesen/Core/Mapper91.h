#pragma once
#include "stdafx.h"
#include "MMC3.h"

class Mapper91 : public MMC3
{
protected:
	virtual uint16_t RegisterStartAddress() override { return 0x6000; }
	virtual uint16_t RegisterEndAddress() override { return 0x7FFF; }

	virtual uint16_t GetPRGPageSize() override { return 0x2000; }
	virtual uint16_t GetCHRPageSize() override { return 0x800; }

	void InitMapper() override
	{
		SelectPRGPage(2, -2);
		SelectPRGPage(3, -1);
	}

	void UpdateState() override
	{
		//Do nothing, we are only using MMC3 code to emulate the IRQs
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		switch(addr & 0x7003) {
			case 0x6000: SelectCHRPage(0, value); break;
			case 0x6001: SelectCHRPage(1, value); break;
			case 0x6002: SelectCHRPage(2, value); break;
			case 0x6003: SelectCHRPage(3, value); break;
			case 0x7000: SelectPRGPage(0, value & 0x0F); break;
			case 0x7001: SelectPRGPage(1, value & 0x0F); break;
			case 0x7002: 
				MMC3::WriteRegister(0xE000, value); 
				break;
			case 0x7003: 
				MMC3::WriteRegister(0xC000, 0x07); 
				MMC3::WriteRegister(0xC001, value);
				MMC3::WriteRegister(0xE001, value);
				break;
		}
	}
};