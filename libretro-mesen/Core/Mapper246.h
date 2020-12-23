#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Mapper246 : public BaseMapper
{
protected:
	virtual uint16_t RegisterStartAddress() override { return 0x6000; }
	virtual uint16_t RegisterEndAddress() override { return 0x67FF; }
	virtual uint16_t GetPRGPageSize() override { return 0x2000; }
	virtual uint16_t GetCHRPageSize() override { return 0x0800; }

	void InitMapper() override
	{
		SelectPRGPage(3, 0xFF);
	}

	virtual void Reset(bool softReset) override
	{
		SelectPRGPage(3, 0xFF);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if((addr & 0x7) <= 0x3) {
			SelectPRGPage(addr & 0x03, value);
		} else {
			SelectCHRPage(addr & 0x03, value);
		}
	}
};
