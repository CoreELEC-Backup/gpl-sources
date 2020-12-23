#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Henggedianzi179 : public BaseMapper
{
protected:
	virtual uint16_t GetPRGPageSize() override { return 0x8000; }
	virtual uint16_t GetCHRPageSize() override { return 0x2000; }
	virtual uint16_t RegisterStartAddress() override { return 0x8000; }
	virtual uint16_t RegisterEndAddress() override { return 0xFFFF; }

	void InitMapper() override
	{
		AddRegisterRange(0x5000, 0x5FFF, MemoryOperation::Write);
		SelectPRGPage(0, 0);
		SelectCHRPage(0, 0);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(addr >= 0x8000) {
			SetMirroringType(value & 0x01 ? MirroringType::Horizontal : MirroringType::Vertical);
		} else {
			SelectPRGPage(0, value >> 1);
		}
	}
};