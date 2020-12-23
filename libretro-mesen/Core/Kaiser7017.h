#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Kaiser7017 : public BaseMapper
{
	uint8_t _prgReg;
	MirroringType _mirroring;
	uint16_t _irqCounter;
	bool _irqEnabled;

protected:
	uint16_t GetPRGPageSize() override { return 0x4000; }
	uint16_t GetCHRPageSize() override { return 0x2000; }
	bool AllowRegisterRead() override { return true; }
	uint16_t RegisterStartAddress() override { return 0x4020; }
	uint16_t RegisterEndAddress() override { return 0x5FFF; }

	void InitMapper() override
	{
		RemoveRegisterRange(0x4020, 0x5FFF, MemoryOperation::Read);
		AddRegisterRange(0x4030, 0x4030, MemoryOperation::Read);

		SelectCHRPage(0, 0);

		_prgReg = 0;
		_mirroring = MirroringType::Vertical;
		_irqCounter = 0;
		_irqEnabled = false;

		UpdateState();
	}

	void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		Stream(_prgReg, _mirroring, _irqCounter, _irqEnabled);
	}

	void UpdateState()
	{
		SelectPRGPage(0, _prgReg);
		SelectPRGPage(1, 2);
		SetMirroringType(_mirroring);
	}

	void ProcessCpuClock() override
	{
		if(_irqEnabled && _irqCounter) {
			_irqCounter--;
			if(_irqCounter == 0) {
				_irqEnabled = false;
				_console->GetCpu()->SetIrqSource(IRQSource::External);
			}
		}
	}

	uint8_t ReadRegister(uint16_t addr) override
	{
		bool irqPending = _console->GetCpu()->HasIrqSource(IRQSource::External);
		_console->GetCpu()->ClearIrqSource(IRQSource::External);
		return irqPending ? 0x01 : 0x00;
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if((addr & 0xFF00) == 0x4A00) {
			_prgReg = ((addr >> 2) & 0x03) | ((addr >> 4) & 0x04);
		} else if((addr & 0xFF00) == 0x5100) {
			UpdateState();
		} else if(addr == 0x4020) {
			_console->GetCpu()->ClearIrqSource(IRQSource::External);
			_irqCounter = (_irqCounter & 0xFF00) | value;
		} else if(addr == 0x4021) {
			_console->GetCpu()->ClearIrqSource(IRQSource::External);
			_irqCounter = (_irqCounter & 0xFF) | (value << 8);
			_irqEnabled = true;
		} else if(addr == 0x4025) {
			_mirroring = ((value >> 3) & 0x01) ? MirroringType::Horizontal : MirroringType::Vertical;
		}
	}
};