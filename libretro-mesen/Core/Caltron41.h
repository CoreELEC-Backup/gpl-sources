#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Caltron41 : public BaseMapper
{
private:
	uint8_t _prgBank;
	uint8_t _chrBank;

protected:
	virtual uint16_t GetPRGPageSize() override { return 0x8000; }
	virtual uint16_t GetCHRPageSize() override { return 0x2000; }
	virtual uint16_t RegisterStartAddress() override { return 0x8000; }
	virtual uint16_t RegisterEndAddress() override { return 0xFFFF; }

	void InitMapper() override
	{
		AddRegisterRange(0x6000, 0x67FF, MemoryOperation::Write);
	}

	void Reset(bool softReset) override
	{
		_chrBank = 0;
		_prgBank = 0;
		WriteRegister(0x6000, 0);
		WriteRegister(0x8000, 0);
	}

	void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		Stream(_prgBank, _chrBank);
	}
	
	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(addr <= 0x67FF) {
			_prgBank = addr & 0x07;
			_chrBank = (_chrBank & 0x03) | ((addr >> 1) & 0x0C);
			SelectPRGPage(0, _prgBank);
			SelectCHRPage(0, _chrBank);
			SetMirroringType(addr & 0x20 ? MirroringType::Horizontal : MirroringType::Vertical);
		} else {
			//"Note that the Inner CHR Bank Select only can be written while the PRG ROM bank is 4, 5, 6, or 7"
			if(_prgBank >= 4) {
				_chrBank = (_chrBank & 0x0C) | (value & 0x03);
				SelectCHRPage(0, _chrBank);
			}
		}
	}
};