#pragma once
#include "stdafx.h"
#include "BaseMapper.h"
#include "CPU.h"

class Smb2j : public BaseMapper
{
private:
	uint16_t _irqCounter;
	bool _irqEnabled;

protected:
	uint16_t GetPRGPageSize() override { return 0x1000; }
	uint16_t GetCHRPageSize() override { return 0x2000; }
	uint16_t RegisterStartAddress() override { return 0x4122; }
	uint16_t RegisterEndAddress() override { return 0x4122; }

	void InitMapper() override
	{
		SelectPrgPage4x(0, 0);
		SelectPrgPage4x(1, 4);
		SelectCHRPage(0, 0);

		if(_prgSize >= 0x10000) {
			AddRegisterRange(0x4022, 0x4022, MemoryOperation::Write);
		}

		SetCpuMemoryMapping(0x5000, 0x5FFF, GetPRGPageCount() - 3, PrgMemoryType::PrgRom);
		SetCpuMemoryMapping(0x6000, 0x6FFF, GetPRGPageCount() - 2, PrgMemoryType::PrgRom);
		SetCpuMemoryMapping(0x7000, 0x7FFF, GetPRGPageCount() - 1, PrgMemoryType::PrgRom);

		_irqCounter = 0;
		_irqEnabled = false;
	}

	void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		Stream(_irqCounter, _irqEnabled);
	}

	void ProcessCpuClock() override
	{
		if(_irqEnabled) {
			_irqCounter = (_irqCounter + 1) & 0xFFF;
			if(_irqCounter == 0) {
				_irqEnabled = false;
				_console->GetCpu()->SetIrqSource(IRQSource::External);
			}
		}
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(addr == 0x4022) {
			SelectPrgPage4x(0, (value & 0x01) << 2);
			SelectPrgPage4x(1, ((value & 0x01) << 2) + 4);
		} else if(addr == 0x4122) {
			_irqEnabled = (value & 0x03) != 0;
			_irqCounter = 0;
			_console->GetCpu()->ClearIrqSource(IRQSource::External);
		}
	}
};