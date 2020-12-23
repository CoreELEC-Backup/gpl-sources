#pragma once
#include "stdafx.h"
#include "BaseMapper.h"
#include "CPU.h"

class VRC3 : public BaseMapper
{
private:
	bool _irqEnableOnAck = false;
	bool _irqEnabled = false;
	bool _smallCounter = false;
	uint16_t _irqReload = 0;
	uint16_t _irqCounter = 0;

protected:
	virtual uint16_t GetPRGPageSize() override { return 0x4000; }
	virtual uint16_t GetCHRPageSize() override { return 0x2000; }

	void InitMapper() override
	{
		SelectPRGPage(1, -1);
		SelectCHRPage(0, 0);
	}

	virtual void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		Stream(_irqEnableOnAck, _smallCounter, _irqEnabled, _irqCounter, _irqReload);
	}

	virtual void ProcessCpuClock() override
	{
		if(_irqEnabled) {
			if(_smallCounter) {
				uint8_t smallCounter = _irqCounter & 0xFF;
				smallCounter++;
				if(_smallCounter == 0) {
					smallCounter = _irqReload & 0xFF;
					_console->GetCpu()->SetIrqSource(IRQSource::External);
				}
				_irqCounter = (_irqCounter & 0xFF00) | smallCounter;
			} else {
				_irqCounter++;
				if(_irqCounter == 0) {
					_irqCounter = _irqReload;
					_console->GetCpu()->SetIrqSource(IRQSource::External);
				}
			}
		}
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		switch(addr & 0xF000) {
			case 0x8000: _irqReload = (_irqReload & 0xFFF0) | (value & 0x0F); break;
			case 0x9000: _irqReload = (_irqReload & 0xFF0F) | ((value & 0x0F) << 4); break;
			case 0xA000: _irqReload = (_irqReload & 0xF0FF) | ((value & 0x0F) << 8); break;
			case 0xB000: _irqReload = (_irqReload & 0x0FFF) | ((value & 0x0F) << 12); break;
			case 0xC000:
				_irqEnabled = (value & 0x02) == 0x02;
				if(_irqEnabled) {
					_irqCounter = _irqReload;
				}
				_smallCounter = (value & 0x04) == 0x04;
				_irqEnableOnAck = (value & 0x01) == 0x01;
				_console->GetCpu()->ClearIrqSource(IRQSource::External);
				break;
			case 0xD000:
				_console->GetCpu()->ClearIrqSource(IRQSource::External);
				_irqEnabled = _irqEnableOnAck;				
				break;
			case 0xF000: SelectPRGPage(0, value & 0x07); break;
		}
	}
};
