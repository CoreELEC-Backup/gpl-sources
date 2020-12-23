#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Kaiser7012 : public BaseMapper
{
protected:
	uint16_t GetPRGPageSize() override { return 0x8000; }
	uint16_t GetCHRPageSize() override { return 0x2000; }

	void InitMapper() override
	{
		SelectPRGPage(0, 1);
		SelectCHRPage(0, 0);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		switch(addr) {
			case 0xE0A0: SelectPRGPage(0, 0); break;
			case 0xEE36: SelectPRGPage(0, 1); break;
		}
	}
};