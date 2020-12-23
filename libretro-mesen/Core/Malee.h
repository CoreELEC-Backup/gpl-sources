#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Malee : public BaseMapper
{
protected:
	uint16_t GetPRGPageSize() override { return 0x800; }
	uint16_t GetCHRPageSize() override { return 0x2000; }

	void InitMapper() override
	{
		SelectPrgPage4x(0, 0);
		SelectPrgPage4x(1, 4);
		SelectPrgPage4x(2, 8);
		SelectPrgPage4x(3, 12);

		SelectCHRPage(0, 0);

		SetCpuMemoryMapping(0x6000, 0x67FF, 16, PrgMemoryType::PrgRom);
	}
};