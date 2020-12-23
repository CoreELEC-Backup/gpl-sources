#pragma once
#include "stdafx.h"
#include "BaseMapper.h"
#include "CPU.h"

class Mapper183 : public BaseMapper
{
private:
	uint8_t _chrRegs[8];
	uint8_t _prgReg;
	uint8_t _irqCounter;
	uint8_t _irqScaler;
	bool _irqEnabled;
	bool _needIrq;

protected:
	virtual uint16_t RegisterStartAddress() override { return 0x6000; }
	virtual uint16_t RegisterEndAddress() override { return 0xFFFF; }
	virtual uint16_t GetPRGPageSize() override { return 0x2000; }
	virtual uint16_t GetCHRPageSize() override { return 0x400; }

	void InitMapper() override
	{
		memset(_chrRegs, 0, sizeof(_chrRegs));
		_prgReg = 0;
		_irqCounter = 0;
		_irqScaler = 0;
		_irqEnabled = false;
		_needIrq = false;

		UpdatePrg();
	}

	void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		ArrayInfo<uint8_t> chrRegs{ _chrRegs, 8 };
		Stream(_prgReg, _irqCounter, _irqEnabled, _irqScaler, _needIrq, chrRegs);
	}

	void UpdatePrg()
	{
		SetCpuMemoryMapping(0x6000, 0x7FFF, _prgReg, PrgMemoryType::PrgRom);
		SelectPRGPage(3, -1);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if((addr & 0xF800) == 0x6800) {
			_prgReg = addr & 0x3F;
			UpdatePrg();
		} else if(((addr & 0xF80C) >= 0xB000) && ((addr & 0xF80C) <= 0xE00C)) {
			int slot = (((addr >> 11) - 6) | (addr >> 3)) & 0x07;
			_chrRegs[slot] = (_chrRegs[slot] & (0xF0 >> (addr & 0x04))) | ((value & 0x0F) << (addr & 0x04));
			SelectCHRPage(slot, _chrRegs[slot]);
		} else switch(addr & 0xF80C) {
			case 0x8800: SelectPRGPage(0, value); break;
			case 0xA800: SelectPRGPage(1, value); break;
			case 0xA000: SelectPRGPage(2, value); break;
			case 0x9800:
				switch(value & 0x03) {
					case 0: SetMirroringType(MirroringType::Vertical); break;
					case 1: SetMirroringType(MirroringType::Horizontal); break;
					case 2: SetMirroringType(MirroringType::ScreenAOnly); break;
					case 3: SetMirroringType(MirroringType::ScreenBOnly); break;
				}
				break;

			case 0xF000: _irqCounter = (_irqCounter & 0xF0) | (value & 0x0F); break;
			case 0xF004: _irqCounter = (_irqCounter & 0x0F) | ((value & 0x0F) << 4); break;
			case 0xF008:
				_irqEnabled = value > 0;
				if(!_irqEnabled) {
					_irqScaler = 0;
				}
				_console->GetCpu()->ClearIrqSource(IRQSource::External);
				break;
		}
	}

	virtual void ProcessCpuClock() override
	{
		if(_needIrq) {
			_console->GetCpu()->SetIrqSource(IRQSource::External);
			_needIrq = false;
		}

		_irqScaler++;
		if(_irqScaler == 114) {
			_irqScaler = 0;
			if(_irqEnabled) {
				_irqCounter++;
				if(_irqCounter == 0) {
					_needIrq = true;
				}
			}
		}
	}
};