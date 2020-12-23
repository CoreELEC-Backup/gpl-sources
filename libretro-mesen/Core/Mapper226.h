#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Mapper226 : public BaseMapper
{
protected:
	uint8_t _registers[2];

	virtual uint16_t GetPRGPageSize() override { return 0x4000; }
	virtual uint16_t GetCHRPageSize() override { return 0x2000; }

	void InitMapper() override
	{
		_registers[0] = 0;
		_registers[1] = 0;

		SelectPRGPage(0, 0);
		SelectPRGPage(1, 1);
		SelectCHRPage(0, 0);
	}

	virtual void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		Stream(_registers[0], _registers[1]);
	}

	virtual void Reset(bool softReset) override
	{
		if(softReset) {
			_registers[0] = 0;
			_registers[1] = 0;

			SelectPRGPage(0, 0);
			SelectPRGPage(1, 1);
			SelectCHRPage(0, 0);
		}
	}

	virtual uint8_t GetPrgPage()
	{
		return (_registers[0] & 0x1F) | ((_registers[0] & 0x80) >> 2) | ((_registers[1] & 0x01) << 6);
	}

	void UpdatePrg()
	{
		uint8_t prgPage = GetPrgPage();
		if(_registers[0] & 0x20) {
			SelectPRGPage(0, prgPage);
			SelectPRGPage(1, prgPage);
		} else {
			SelectPRGPage(0, prgPage & 0xFE);
			SelectPRGPage(1, (prgPage & 0xFE) + 1);
		}
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		switch(addr & 0x8001) {
			case 0x8000: _registers[0] = value;	break;
			case 0x8001: _registers[1] = value; break;
		}

		UpdatePrg();

		SetMirroringType(_registers[0] & 0x40 ? MirroringType::Vertical : MirroringType::Horizontal);
	}
};
