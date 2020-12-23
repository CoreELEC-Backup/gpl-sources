#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Bmc11160 : public BaseMapper
{
protected:
	uint16_t GetPRGPageSize() override { return 0x8000; }
	uint16_t GetCHRPageSize() override { return 0x2000; }

	void InitMapper() override
	{
	}

	void Reset(bool softReset) override
	{
		BaseMapper::Reset(softReset);
		WriteRegister(0x8000, 0);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		uint8_t bank = (value >> 4) & 0x07;
		SelectPRGPage(0, bank);
		SelectCHRPage(0, (bank << 2) | (value & 0x03));
		SetMirroringType(value & 0x80 ? MirroringType::Vertical : MirroringType::Horizontal);
	}
};