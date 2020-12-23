#pragma once
#include "stdafx.h"
#include "BaseMapper.h"
#include "MemoryManager.h"

class Dance2000 : public BaseMapper
{
private:
	uint8_t _prgReg;
	uint8_t _mode;
	uint8_t _lastNt;

protected:
	bool AllowRegisterRead() override { return true; }
	uint16_t GetPRGPageSize() override { return 0x4000; }
	uint16_t GetCHRPageSize() override { return 0x1000; }

	void InitMapper() override
	{
		_prgReg = _mode = _lastNt = 0;
		AddRegisterRange(0x5000, 0x5FFF, MemoryOperation::Write);
		RemoveRegisterRange(0x8000, 0xFFFF, MemoryOperation::Write);
		UpdateState();
	}

	void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		Stream(_mode, _prgReg, _lastNt);
	}

	void NotifyVRAMAddressChange(uint16_t addr) override
	{
		if(_mode & 0x02) {
			if((addr & 0x3000) == 0x2000) {
				uint32_t currentNametable = (addr >> 11) & 0x01;
				if(currentNametable != _lastNt) {
					_lastNt = currentNametable;
					SelectCHRPage(0, _lastNt);
				}
			}
		} else {
			if(_lastNt != 0) {
				_lastNt = 0;
				SelectCHRPage(0, _lastNt);
			}
		}
	}

	void UpdateState()
	{
		SelectCHRPage(0, _lastNt);
		SelectCHRPage(1, 1);
		if(_mode & 0x04)
			SelectPrgPage2x(0, (_prgReg & 0x07) << 1);
		else {
			SelectPRGPage(0, _prgReg & 0x0F);
			SelectPRGPage(1, 0);
		}
		SetMirroringType(_mode & 0x01 ? MirroringType::Horizontal : MirroringType::Vertical);
	}

	uint8_t ReadRegister(uint16_t addr) override
	{
		return (_prgReg & 0x40) ? _console->GetMemoryManager()->GetOpenBus() : InternalReadRam(addr);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(addr == 0x5000) {
			_prgReg = value;
			UpdateState();
		} else if(addr == 0x5200) {
			_mode = value;
			if(_mode & 0x04) {
				UpdateState();
			}
		}
	}
};