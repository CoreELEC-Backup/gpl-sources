#pragma once
#include "stdafx.h"
#include "BaseMapper.h"
#include "A12Watcher.h"

class Mapper117 : public BaseMapper
{
private:
	uint8_t _irqCounter;
	uint8_t _irqReloadValue;
	bool _irqEnabled;
	bool _irqEnabledAlt;
	A12Watcher _a12Watcher;

protected:
	virtual uint16_t GetPRGPageSize() override { return 0x2000; }
	virtual uint16_t GetCHRPageSize() override { return 0x400; }

	void InitMapper() override
	{
		_irqEnabled = false;
		_irqEnabledAlt = false;
		_irqCounter = 0;
		_irqReloadValue = 0;

		SelectPrgPage4x(0, -4);
	}

	void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		SnapshotInfo a12Watcher{ &_a12Watcher };
		Stream(_irqCounter, _irqEnabled, _irqEnabledAlt, _irqReloadValue, a12Watcher);
	}

	void NotifyVRAMAddressChange(uint16_t addr) override
	{
		if(_a12Watcher.UpdateVramAddress(addr, _console->GetPpu()->GetFrameCycle()) == A12StateChange::Rise) {
			if(_irqEnabled && _irqEnabledAlt && _irqCounter) {
				_irqCounter--;
				if(_irqCounter == 0) {
					_console->GetCpu()->SetIrqSource(IRQSource::External);
					_irqEnabledAlt = false;
				}
			}
		}
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		switch(addr) {
			case 0x8000: case 0x8001: case 0x8002: case 0x8003:
				SelectPRGPage(addr & 0x03, value ); 
				break;
			
			case 0xA000: case 0xA001: case 0xA002: case 0xA003:
			case 0xA004: case 0xA005: case 0xA006: case 0xA007:
				SelectCHRPage(addr & 0x07, value);
				break;

			case 0xC001: _irqReloadValue = value; break;
			case 0xC002: _console->GetCpu()->ClearIrqSource(IRQSource::External); break;

			case 0xC003:
				_irqCounter = _irqReloadValue;
				_irqEnabledAlt = true;
				break;

			case 0xD000: SetMirroringType(value & 0x01 ? MirroringType::Horizontal : MirroringType::Vertical); break;

			case 0xE000:
				_irqEnabled = (value & 0x01) == 0x01;
				_console->GetCpu()->ClearIrqSource(IRQSource::External);
				break;
		}
	}
};