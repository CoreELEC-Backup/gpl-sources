#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Kaiser7031 : public BaseMapper
{
private:
	uint8_t _regs[4];

protected:
	uint16_t GetPRGPageSize() override { return 0x800; }
	uint16_t GetCHRPageSize() override { return 0x2000; }

	void InitMapper() override
	{
		SetMirroringType(MirroringType::Vertical);
		memset(_regs, 0, sizeof(_regs));
		for(int i = 0; i < 16; i++) {
			SelectPRGPage(i, 15 - i);
		}
		SelectCHRPage(0, 0);
		UpdateState();
	}

	void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		Stream(_regs[0], _regs[1], _regs[2], _regs[3]);

		if(!saving) {
			UpdateState();
		}
	}

	void UpdateState()
	{
		for(int i = 0; i < 4; i++) {
			SetCpuMemoryMapping(0x6000 + i * 0x800, 0x67FF + i * 0x800, _regs[i], PrgMemoryType::PrgRom);
		}
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		_regs[(addr >> 11) & 0x03] = value;
		UpdateState();
	}
};