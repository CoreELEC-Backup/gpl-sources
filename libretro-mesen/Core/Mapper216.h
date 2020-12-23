#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Mapper216 : public BaseMapper
{
protected:
	uint16_t GetPRGPageSize() override { return 0x8000; }
	uint16_t GetCHRPageSize() override { return 0x2000; }
	bool AllowRegisterRead() override { return true; }

	void InitMapper() override
	{
		WriteRegister(0x8000, 0);
		AddRegisterRange(0x5000, 0x5000);
		RemoveRegisterRange(0x8000, 0xFFFF, MemoryOperation::Read);
	}

	uint8_t ReadRegister(uint16_t addr) override
	{
		//For Videopoker Bonza?
		return 0;
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		SelectPRGPage(0, addr & 0x01);
		SelectCHRPage(0, (addr & 0x0E) >> 1);
	}
};