#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class TaitoX1017 : public BaseMapper
{
private:
	uint8_t _chrMode;
	uint8_t _chrRegs[6];
	uint8_t _ramPermission[3];

	void UpdateRamAccess()
	{
		SetCpuMemoryMapping(0x6000, 0x63FF, 0, PrgMemoryType::SaveRam, _ramPermission[0] == 0xCA ? MemoryAccessType::ReadWrite : MemoryAccessType::NoAccess);
		SetCpuMemoryMapping(0x6400, 0x67FF, 1, PrgMemoryType::SaveRam, _ramPermission[0] == 0xCA ? MemoryAccessType::ReadWrite : MemoryAccessType::NoAccess);
		
		SetCpuMemoryMapping(0x6800, 0x6BFF, 2, PrgMemoryType::SaveRam, _ramPermission[1] == 0x69 ? MemoryAccessType::ReadWrite : MemoryAccessType::NoAccess);
		SetCpuMemoryMapping(0x6C00, 0x6FFF, 3, PrgMemoryType::SaveRam, _ramPermission[1] == 0x69 ? MemoryAccessType::ReadWrite : MemoryAccessType::NoAccess);
		
		SetCpuMemoryMapping(0x7000, 0x73FF, 4, PrgMemoryType::SaveRam, _ramPermission[2] == 0x84 ? MemoryAccessType::ReadWrite : MemoryAccessType::NoAccess);
	}

protected:
	virtual uint16_t GetPRGPageSize() override { return 0x2000; }
	virtual uint16_t GetCHRPageSize() override { return 0x0400; }
	virtual uint16_t RegisterStartAddress() override { return 0x7EF0; }
	virtual uint16_t RegisterEndAddress() override { return 0x7EFF; }

	virtual uint32_t GetSaveRamSize() override { return 0x1400; }
	virtual uint32_t GetSaveRamPageSize() override { return 0x400; }

	void InitMapper() override
	{
		_chrMode = 0;
		memset(_ramPermission, 0, sizeof(_ramPermission));
		memset(_chrRegs, 0, sizeof(_chrRegs));

		SelectPRGPage(3, -1);

		UpdateRamAccess();
	}

	void UpdateChrBanking()
	{
		if(_chrMode == 0) {
			//Regs 0 & 1 ignore the LSB
			SelectChrPage2x(0, _chrRegs[0] & 0xFE);
			SelectChrPage2x(1, _chrRegs[1] & 0xFE);

			SelectCHRPage(4, _chrRegs[2]);
			SelectCHRPage(5, _chrRegs[3]);
			SelectCHRPage(6, _chrRegs[4]);
			SelectCHRPage(7, _chrRegs[5]);
		} else {
			SelectCHRPage(0, _chrRegs[2]);
			SelectCHRPage(1, _chrRegs[3]);
			SelectCHRPage(2, _chrRegs[4]);
			SelectCHRPage(3, _chrRegs[5]);

			//Regs 0 & 1 ignore the LSB
			SelectChrPage2x(2, _chrRegs[0] & 0xFE);
			SelectChrPage2x(3, _chrRegs[1] & 0xFE);
		}
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		switch(addr) {
			case 0x7EF0:
			case 0x7EF1:
			case 0x7EF2:
			case 0x7EF3:
			case 0x7EF4:
			case 0x7EF5:
				_chrRegs[(addr & 0xF)] = value;
				UpdateChrBanking();
				break;

			case 0x7EF6: 
				SetMirroringType((value & 0x01) == 0x01 ? MirroringType::Vertical : MirroringType::Horizontal);
				_chrMode = (value & 0x02) >> 1;
				UpdateChrBanking();
				break;

			case 0x7EF7: 
			case 0x7EF8:
			case 0x7EF9:
				_ramPermission[(addr & 0xF) - 7] = value;
				UpdateRamAccess();
				break;

			case 0x7EFA:
				SelectPRGPage(0, value >> 2);
				break;

			case 0x7EFB:
				SelectPRGPage(1, value >> 2);
				break;

			case 0x7EFC:
				SelectPRGPage(2, value >> 2);
				break;
		}
	}

	virtual void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		ArrayInfo<uint8_t> ramPermission = { _ramPermission, 3 };
		ArrayInfo<uint8_t> chrRegs = { _chrRegs, 6 };
		Stream(ramPermission, chrRegs, _chrMode);

		if(!saving) {
			UpdateRamAccess();
		}
	}
};