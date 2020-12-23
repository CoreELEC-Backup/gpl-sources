#pragma once
#include "MMC1.h"

class MMC1_155 : public MMC1
{
protected :
	void UpdateState()
	{
		//WRAM disable bit does not exist in mapper 155
		_state.RegE000 &= 0x0F;

		MMC1::UpdateState();
	}
};