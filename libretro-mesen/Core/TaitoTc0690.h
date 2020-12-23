#pragma once
#include "stdafx.h"
#include "TaitoTc0190.h"
#include "MMC3.h"

class TaitoTc0690 : public MMC3
{
private:
	uint8_t _irqDelay;
	bool _isFlintstones;

protected:
	virtual void InitMapper() override
	{
		_irqDelay = 0;
		SelectPRGPage(2, -2);
		SelectPRGPage(3, -1);

		//This cart appears to behave differently (maybe not an identical mapper?)
		//IRQ seems to be triggered at a different timing (approx 100 cpu cycles before regular mapper 48 timings)
		_isFlintstones = _romInfo.SubMapperID == 255;
	}

	virtual void StreamState(bool saving) override
	{
		MMC3::StreamState(saving);
		Stream(_irqDelay);
	}

	virtual void TriggerIrq() override
	{
		//"The IRQ seems to trip a little later than it does on MMC3.  It looks like about a 4 CPU cycle delay from the normal MMC3 IRQ time."
		//A value of 6 removes the shaking from The Jetsons
		_irqDelay = _isFlintstones ? 19 : 6;
	}

	void ProcessCpuClock() override
	{
		if(_irqDelay > 0) {
			_irqDelay--;
			if(_irqDelay == 0) {
				_console->GetCpu()->SetIrqSource(IRQSource::External);
			}
		}
	}

	virtual void WriteRegister(uint16_t addr, uint8_t value) override
	{
		switch(addr & 0xE003) {
			case 0x8000:
				SelectPRGPage(0, value & 0x3F);
				break;
			case 0x8001:
				SelectPRGPage(1, value & 0x3F);
				break;
			case 0x8002:
				SelectCHRPage(0, value * 2);
				SelectCHRPage(1, value * 2 + 1);
				break;
			case 0x8003:
				SelectCHRPage(2, value * 2);
				SelectCHRPage(3, value * 2 + 1);
				break;
			case 0xA000: case 0xA001: case 0xA002: case 0xA003:
				SelectCHRPage(4 + (addr & 0x03), value);
				break;

			case 0xC000:
				//Flintstones expects either $C000 or $C001 to clear the irq flag
				_console->GetCpu()->ClearIrqSource(IRQSource::External);

				_irqReloadValue = (value ^ 0xFF) + (_isFlintstones ? 0 : 1);
				break;
			case 0xC001:
				//Flintstones expects either $C000 or $C001 to clear the irq flag
				_console->GetCpu()->ClearIrqSource(IRQSource::External);

				_irqCounter = 0;
				_irqReload = true;
				break;
			case 0xC002:
				_irqEnabled = true;
				break;
			case 0xC003:
				_irqEnabled = false;
				_console->GetCpu()->ClearIrqSource(IRQSource::External);
				break;

			case 0xE000:
				SetMirroringType((value & 0x40) == 0x40 ? MirroringType::Horizontal : MirroringType::Vertical);
				break;
		}
	}
};