#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Edu2000 : public BaseMapper
{
private:
	uint8_t _reg;

protected:
	uint16_t GetPRGPageSize() override { return 0x8000; }
	uint16_t GetCHRPageSize() override { return 0x2000; }
	uint32_t GetWorkRamSize() override { return 0x8000; }
	uint32_t GetWorkRamPageSize() override { return 0x2000; }

	void InitMapper() override
	{
		_reg = 0;
		UpdatePrg();
		SelectCHRPage(0, 0);
	}

	void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		Stream(_reg);
		if(!saving) {
			UpdatePrg();
		}
	}

	void UpdatePrg()
	{
		SelectPRGPage(0, _reg & 0x1F);
		SetCpuMemoryMapping(0x6000, 0x7FFF, (_reg >> 6) & 0x03, PrgMemoryType::WorkRam);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		_reg = value;
		UpdatePrg();
	}
};