#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Mapper103 : public BaseMapper
{
private:
	bool _prgRamDisabled;
	uint8_t _prgReg;

protected:
	virtual uint16_t GetPRGPageSize() override { return 0x2000; }
	virtual uint16_t GetCHRPageSize() override { return 0x2000; }
	virtual uint32_t GetWorkRamSize() override { return 0x4000; }
	virtual uint32_t GetWorkRamPageSize() override { return 0x2000; }
	virtual uint16_t RegisterStartAddress() override { return 0x6000; }
	virtual uint16_t EndStartAddress() { return 0xFFFF; }

	void InitMapper() override
	{
		_prgRamDisabled = false;
		_prgReg = 0;
		SelectCHRPage(0, 0);
		UpdateState();
	}

	void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		Stream(_prgRamDisabled, _prgReg);
		if(!saving) {
			UpdateState();
		}
	}

	void UpdateState()
	{
		SelectPrgPage4x(0, -4);
		if(_prgRamDisabled) {
			SetCpuMemoryMapping(0x6000, 0x7FFF, _prgReg, PrgMemoryType::PrgRom);
		} else {
			SetCpuMemoryMapping(0x6000, 0x7FFF, 0, PrgMemoryType::WorkRam);
			SetCpuMemoryMapping(0xB800, 0xD7FF, 1, PrgMemoryType::WorkRam);
		}
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		switch(addr & 0xF000) {
			case 0x6000: case 0x7000:
				//Workram is always writeable, even when PRG ROM is mapped to $6000
				_workRam[addr - 0x6000] = value;
				break;

			case 0x8000:
				_prgReg = value & 0x0F;
				UpdateState();
				break;
			
			case 0xB000: case 0xC000: case 0xD000:
				//Workram is always writeable, even when PRG ROM is mapped to $B800-$D7FF
				if(addr >= 0xB800 && addr < 0xD800) {
					_workRam[0x2000 + addr - 0xB800] = value;
				}
				break;

			case 0xE000:
				SetMirroringType(value & 0x08 ? MirroringType::Horizontal : MirroringType::Vertical);
				break;

			case 0xF000:
				_prgRamDisabled = (value & 0x10) == 0x10;
				UpdateState();
				break;
		}
	}
};