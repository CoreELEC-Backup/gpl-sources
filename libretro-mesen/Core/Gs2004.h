#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Gs2004 : public BaseMapper
{
protected:
	uint16_t GetPRGPageSize() override { return 0x2000; }
	uint16_t GetCHRPageSize() override { return 0x2000; }

	void InitMapper() override
	{
		SelectCHRPage(0, 0);
	}

	void Reset(bool softReset) override
	{
		SetCpuMemoryMapping(0x6000, 0x7FFF, 0x20, PrgMemoryType::PrgRom);
		SelectPrgPage4x(0, 0x07 << 2);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		SelectPrgPage4x(0, (value & 0x07) << 2);
	}
};