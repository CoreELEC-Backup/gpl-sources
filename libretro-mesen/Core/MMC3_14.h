#pragma once
#include "stdafx.h"
#include "MMC3.h"

class MMC3_14 : public MMC3
{
private:
	uint8_t _vrcChrRegs[8];
	uint8_t _vrcPrgRegs[2];
	uint8_t _vrcMirroring;
	uint8_t _mode;

protected:
	virtual void InitMapper() override
	{
		_mode = 0;
		_vrcMirroring = 0;
		memset(_vrcPrgRegs, 0, sizeof(_vrcPrgRegs));
		memset(_vrcChrRegs, 0, sizeof(_vrcChrRegs));

		MMC3::InitMapper();
	}

	virtual void StreamState(bool saving) override
	{
		MMC3::StreamState(saving);
		ArrayInfo<uint8_t> prgRegs{ _vrcPrgRegs, 2 };
		ArrayInfo<uint8_t> chrRegs{ _vrcChrRegs, 8 };
		Stream(_mode, _vrcMirroring, prgRegs, chrRegs);
	}

	void UpdateChrMapping() override
	{
		int slotSwap = (GetState().Reg8000 & 0x80) ? 4 : 0;
		int outerBank0 = (_mode & 0x08) ? 0x100 : 0;
		int outerBank1 = (_mode & 0x20) ? 0x100 : 0;
		int outerBank2 = (_mode & 0x80) ? 0x100 : 0;
		SelectCHRPage(0 ^ slotSwap, outerBank0 | (_registers[0] & (~1)));
		SelectCHRPage(1 ^ slotSwap, outerBank0 | _registers[0] | 1);
		SelectCHRPage(2 ^ slotSwap, outerBank0 | (_registers[1] & (~1)));
		SelectCHRPage(3 ^ slotSwap, outerBank0 | _registers[1] | 1);
		SelectCHRPage(4 ^ slotSwap, outerBank1 | _registers[2]);
		SelectCHRPage(5 ^ slotSwap, outerBank1 | _registers[3]);
		SelectCHRPage(6 ^ slotSwap, outerBank2 | _registers[4]);
		SelectCHRPage(7 ^ slotSwap, outerBank2 | _registers[5]);
	}

	void UpdateVrcState()
	{
		SelectPRGPage(0, _vrcPrgRegs[0]);
		SelectPRGPage(1, _vrcPrgRegs[1]);
		SelectPRGPage(2, -2);
		SelectPRGPage(3, -1);

		for(int i = 0; i < 8; i++) {
			SelectCHRPage(i, _vrcChrRegs[i]);
		}
		
		SetMirroringType(_vrcMirroring & 0x01 ? MirroringType::Horizontal : MirroringType::Vertical);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		if(addr == 0xA131) {
			_mode = value;
		}

		if(_mode & 0x02) {
			MMC3::UpdateState();
			MMC3::WriteRegister(addr, value);
		} else {
			if(addr >= 0xB000 && addr <= 0xEFFF) {
				uint8_t regNumber = ((((addr >> 12) & 0x07) - 3) << 1) + ((addr >> 1) & 0x01);
				bool lowBits = (addr & 0x01) == 0x00;
				if(lowBits) {
					_vrcChrRegs[regNumber] = (_vrcChrRegs[regNumber] & 0xF0) | (value & 0x0F);
				} else {
					_vrcChrRegs[regNumber] = (_vrcChrRegs[regNumber] & 0x0F) | ((value & 0x0F) << 4);
				}
			} else {
				switch(addr & 0xF003) {
					case 0x8000: _vrcPrgRegs[0] = value; break;
					case 0x9000: _vrcMirroring = value; break;
					case 0xA000: _vrcPrgRegs[1] = value; break;
				}
			}
			UpdateVrcState();
		}
	}
};