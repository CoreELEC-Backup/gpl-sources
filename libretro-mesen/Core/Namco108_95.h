#pragma once

#include "stdafx.h"
#include "Namco108.h"

class Namco108_95 : public Namco108
{
protected:
	virtual void WriteRegister(uint16_t addr, uint8_t value) override
	{
		Namco108::WriteRegister(addr, value);

		if(addr & 0x01) {
			uint8_t nameTable1 = (_registers[0] >> 5) & 0x01;
			uint8_t nameTable2 = (_registers[1] >> 5) & 0x01;

			SetNametables(nameTable1, nameTable1, nameTable2, nameTable2);
		}
	}
};