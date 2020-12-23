#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class A65AS : public BaseMapper
{
protected:
	uint16_t GetPRGPageSize() override { return 0x4000; }
	uint16_t GetCHRPageSize() override { return 0x2000; }

	void InitMapper() override
	{
		SelectCHRPage(0, 0);
		WriteRegister(0x8000, 0);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(value & 0x40) {
			SelectPrgPage2x(0, value & 0x1E);
		} else {
			SelectPRGPage(0, ((value & 0x30) >> 1) | (value & 0x07));
			SelectPRGPage(1, ((value & 0x30) >> 1) | 0x07);
		}
		
		if(value & 0x80) {
			SetMirroringType(value & 0x20 ? MirroringType::ScreenBOnly : MirroringType::ScreenAOnly);
		} else {
			SetMirroringType(value & 0x08 ? MirroringType::Horizontal : MirroringType::Vertical);
		}
	}
};