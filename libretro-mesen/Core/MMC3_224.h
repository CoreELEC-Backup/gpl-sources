#pragma once
#include "stdafx.h"
#include "MMC3.h"

//iNES Mapper 224 is used for the ���ȑ� (Jncota) KT-008 PCB. It's an MMC3 clone that supports 1024 KiB of PRG-ROM
//through an additional outer bank register at $5000.
//http://wiki.nesdev.com/w/index.php/INES_Mapper_224
class MMC3_224 : public MMC3
{
private:
	uint8_t _outerBank;

protected:
	void InitMapper() override
	{
		_outerBank = 0;

		AddRegisterRange(0x5000, 0x5003, MemoryOperation::Write);
		MMC3::InitMapper();
	}

	void StreamState(bool saving) override
	{
		MMC3::StreamState(saving);
		Stream(_outerBank);
	}

	void UpdatePrgMapping() override
	{
		uint8_t outerBank = _outerBank << 6;
		if(_prgMode == 0) {
			SelectPRGPage(0, (_registers[6] & 0x3F) | outerBank);
			SelectPRGPage(1, (_registers[7] & 0x3F) | outerBank);
			SelectPRGPage(2, 0x3E | outerBank);
			SelectPRGPage(3, 0x3F | outerBank);
		} else if(_prgMode == 1) {
			SelectPRGPage(0, 0x3E | outerBank);
			SelectPRGPage(1, (_registers[6] & 0x3F) | outerBank);
			SelectPRGPage(2, (_registers[7] & 0x3F) | outerBank);
			SelectPRGPage(3, 0x3F | outerBank);
		}
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(addr < 0x8000) {
			if(addr == 0x5000) {
				_outerBank = (value >> 2) & 0x01;
				UpdatePrgMapping();
			}
		} else {
			MMC3::WriteRegister(addr, value);
		}
	}
};