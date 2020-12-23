#pragma once
#include "stdafx.h"
#include "BaseMapper.h"

class Bandai74161_7432 : public BaseMapper
{
private:
	bool _enableMirroringControl;

protected:
	virtual uint16_t GetPRGPageSize() override { return 0x4000; }
	virtual uint16_t GetCHRPageSize() override { return 0x2000; }

	void InitMapper() override
	{
		SelectPRGPage(0, 0);
		SelectPRGPage(1, -1);
		SelectCHRPage(0, 0);

		//Hack to make Kamen Rider Club - Gekitotsu Shocker Land work correctly (bad header)
		SetMirroringType(MirroringType::Vertical);
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		bool mirroringBit = (value & 0x80) == 0x80;
		if(mirroringBit) {
			//If any game tries to set the bit to true, assume it will use mirroring switches
			//This is a hack to make as many games as possible work without CRC checks
			_enableMirroringControl = true;
		}

		if(_enableMirroringControl) {
			SetMirroringType(mirroringBit ? MirroringType::ScreenBOnly : MirroringType::ScreenAOnly);
		}
		
		//Biggest PRG ROM I could find for mapper 70/152 is 128kb, so the 4th bit will never be used on those
		SelectPRGPage(0, (value >> 4) & 0x07);
		SelectCHRPage(0, value & 0x0F);
	}

	virtual void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		Stream(_enableMirroringControl);		
	}

public:
	Bandai74161_7432(bool enableMirroringControl) : _enableMirroringControl(enableMirroringControl)
	{
		//According to NesDev Wiki, Mapper 70 is meant to have mirroring forced (by the board) and Mapper 152 allows the code to specify the mirroring type
	}
};