#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Mapper231 : public BaseMapper
{
protected:
	virtual uint16_t GetPRGPageSize() override { return 0x4000; }
	virtual uint16_t GetCHRPageSize() override { return 0x2000; }

	void InitMapper() override
	{
		SelectPRGPage(0, 0);
		SelectPRGPage(1, 0);
		SelectCHRPage(0, 0);
	}

	virtual void Reset(bool softReset) override
	{
		SelectPRGPage(0, 0);
		SelectPRGPage(1, 0);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		uint8_t prgBank = ((addr >> 5) & 0x01) | (addr & 0x1E);
		SelectPRGPage(0, prgBank & 0x1E);
		SelectPRGPage(1, prgBank);
		SetMirroringType(addr & 0x80 ? MirroringType::Horizontal : MirroringType::Vertical);
	}
};
