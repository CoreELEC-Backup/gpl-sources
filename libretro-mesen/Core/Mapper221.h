#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Mapper221 : public BaseMapper
{
private:
	uint16_t _mode;
	uint8_t _prgReg;

protected:
	virtual uint16_t GetPRGPageSize() override { return 0x4000; }
	virtual uint16_t GetCHRPageSize() override { return 0x2000; }

	void InitMapper() override
	{
		_prgReg = 0;
		_mode = 0;

		SelectCHRPage(0, 0);

		UpdateState();
	}

	void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		Stream(_mode, _prgReg);
	}

	void UpdateState()
	{
		uint16_t outerBank = (_mode & 0xFC) >> 2;
		if(_mode & 0x02) {
			if(_mode & 0x0100) {
				SelectPRGPage(0, outerBank | _prgReg);
				SelectPRGPage(1, outerBank | 0x07);
			} else {
				SelectPrgPage2x(0, outerBank | (_prgReg & 0x06));
			}
		} else {
			SelectPRGPage(0, outerBank | _prgReg);
			SelectPRGPage(1, outerBank | _prgReg);
		}

		SetMirroringType(_mode & 0x01 ? MirroringType::Horizontal : MirroringType::Vertical);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		switch(addr & 0xC000) {
			case 0x8000:
				_mode = addr;
				UpdateState();
				break;

			case 0xC000:
				_prgReg = addr & 0x07;
				UpdateState();
				break;
		}
	}
};