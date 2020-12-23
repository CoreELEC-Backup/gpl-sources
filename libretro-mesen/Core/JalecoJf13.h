#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class JalecoJf13 : public BaseMapper
{
protected:
	virtual uint16_t RegisterStartAddress() override { return 0x6000; }
	virtual uint16_t RegisterEndAddress() override { return 0x7FFF; }
	virtual uint16_t GetPRGPageSize() override { return 0x8000; }
	virtual uint16_t GetCHRPageSize() override { return 0x2000; }

	void InitMapper() override
	{
		SelectPRGPage(0, 0);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		switch(addr & 0x7000) {
			case 0x6000:
				SelectPRGPage(0, (value & 0x30) >> 4);
				SelectCHRPage(0, (value & 0x03) | ((value >> 4) & 0x04));
				break;

			case 0x7000:
				//Audio not supported
				break;
		}
		
	}
};