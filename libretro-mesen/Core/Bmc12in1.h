#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Bmc12in1 : public BaseMapper
{
private:
	uint8_t _regs[2];
	uint8_t _mode;

protected:
	uint16_t GetPRGPageSize() override { return 0x4000; }
	uint16_t GetCHRPageSize() override { return 0x1000; }

	void InitMapper() override
	{
		_regs[0] = _regs[1] = 0;
		_mode = 0;
		UpdateState();
	}

	void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		Stream(_regs[0], _regs[1], _mode);
	}

	void UpdateState()
	{
		uint8_t bank = (_mode & 0x03) << 3;
		SelectCHRPage(0, (_regs[0] >> 3) | (bank << 2));
		SelectCHRPage(1, (_regs[1] >> 3) | (bank << 2));
		if(_mode & 0x08) {
			SelectPrgPage2x(0, bank | (_regs[0] & 0x06));
		} else {
			SelectPRGPage(0, bank | (_regs[0] & 0x07));
			SelectPRGPage(1, bank | 0x07);
		}
		SetMirroringType(_mode & 0x04 ? MirroringType::Horizontal : MirroringType::Vertical);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		switch(addr & 0xE000) {
			case 0xA000: _regs[0] = value; UpdateState(); break;
			case 0xC000: _regs[1] = value; UpdateState(); break;
			case 0xE000: _mode = value & 0x0F; UpdateState(); break;
		}
	}
};