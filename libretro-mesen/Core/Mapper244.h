#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Mapper244 : public BaseMapper
{
private:
	uint8_t _lutPrg[4][4] = { 
		{ 0, 1, 2, 3 },
		{ 3, 2, 1, 0 },
		{ 0, 2, 1, 3 },
		{ 3, 1, 2, 0 }
	};

	uint8_t _lutChr[8][8] = { 
		{ 0, 1, 2, 3, 4, 5, 6, 7 }, 
		{ 0, 2, 1, 3, 4, 6, 5, 7 }, 
		{ 0, 1, 4, 5, 2, 3, 6, 7 },
		{ 0, 4, 1, 5, 2, 6, 3, 7 },
		{ 0, 4, 2, 6, 1, 5, 3, 7 },
		{ 0, 2, 4, 6, 1, 3, 5, 7 },
		{ 7, 6, 5, 4, 3, 2, 1, 0 },
		{ 7, 6, 5, 4, 3, 2, 1, 0 }
	};

protected:
	virtual uint16_t GetPRGPageSize() override { return 0x8000; }
	virtual uint16_t GetCHRPageSize() override { return 0x2000; }

	void InitMapper() override
	{
		SelectPRGPage(0, 0);
		SelectCHRPage(0, 0);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(value & 0x08) {
			SelectCHRPage(0, _lutChr[(value >> 4) & 0x07][value & 0x07]);
		} else {
			SelectPRGPage(0, _lutPrg[(value >> 4) & 0x03][value & 0x03]);
		}
	}
};