#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Kaiser202 : public BaseMapper
{
	uint16_t _irqReloadValue;
	uint16_t _irqCounter;
	bool _irqEnabled;
	uint8_t _selectedReg;
	uint8_t _prgRegs[4];

protected:
	virtual uint16_t GetPRGPageSize() override { return 0x2000; }
	virtual uint16_t GetCHRPageSize() override { return 0x0400; }

	void InitMapper() override
	{
		_irqReloadValue = 0;
		_irqCounter = 0;
		_irqEnabled = 0;
		_selectedReg = 0;
		memset(_prgRegs, 0, sizeof(_prgRegs));
		
		SelectPRGPage(3, -1);
	}

	void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		Stream(_irqReloadValue, _irqCounter, _irqEnabled, _selectedReg, _prgRegs[0], _prgRegs[1], _prgRegs[2], _prgRegs[3]);

		if(!saving) {
			SetCpuMemoryMapping(0x6000, 0x7FFF, _prgRegs[3], PrgMemoryType::PrgRom, MemoryAccessType::ReadWrite);
		}
	}

	void ProcessCpuClock() override
	{
		if(_irqEnabled) {
			_irqCounter++;
			if(_irqCounter == 0xFFFF) {
				_irqCounter = _irqReloadValue;
				_console->GetCpu()->SetIrqSource(IRQSource::External);
			}
		}
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		switch(addr & 0xF000) {
			case 0x8000: _irqReloadValue = (_irqReloadValue & 0xFFF0) | (value & 0x0F); break;
			case 0x9000: _irqReloadValue = (_irqReloadValue & 0xFF0F) | ((value & 0x0F) << 4); break;
			case 0xA000: _irqReloadValue = (_irqReloadValue & 0xF0FF) | ((value & 0x0F) << 8); break;
			case 0xB000: _irqReloadValue = (_irqReloadValue & 0x0FFF) | ((value & 0x0F) << 12); break;
			
			case 0xC000: 
				_irqEnabled = (value != 0);
				if(_irqEnabled) {
					_irqCounter = _irqReloadValue;
				}
				_console->GetCpu()->ClearIrqSource(IRQSource::External);
				break;

			case 0xD000: _console->GetCpu()->ClearIrqSource(IRQSource::External); break;
			case 0xE000: _selectedReg = (value & 0x0F) - 1; break;

			case 0xF000: 
				if(_selectedReg < 3) {
					_prgRegs[_selectedReg] = ((_prgRegs[_selectedReg]) & 0x10) | (value & 0x0F);
				} else if(_selectedReg < 4) {
					//For Kaiser7032 (Mapper 142)
					_prgRegs[_selectedReg] = value;
					SetCpuMemoryMapping(0x6000, 0x7FFF, value, PrgMemoryType::PrgRom, MemoryAccessType::ReadWrite);
				}

				switch(addr & 0xFC00) {
					case 0xF000: {
						uint8_t bank = addr & 0x03;
						if(bank < 3) {
							_prgRegs[bank] = (value & 0x10) | (_prgRegs[bank] & 0x0F);
						}
						break;
					}

					case 0xF800:
						SetMirroringType(value & 0x01 ? MirroringType::Vertical : MirroringType::Horizontal);
						break;

					case 0xFC00:
						SelectCHRPage(addr & 0x07, value);
						break;
				}

				SelectPRGPage(0, _prgRegs[0]);
				SelectPRGPage(1, _prgRegs[1]);
				SelectPRGPage(2, _prgRegs[2]);
				break;
		}
	}
};