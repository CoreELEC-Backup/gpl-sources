#pragma once
#include "stdafx.h"
#include "Snapshotable.h"

class BaseFdsChannel : public Snapshotable
{
protected:
	uint8_t _speed = 0;
	uint8_t _gain = 0;
	bool _envelopeOff = false;
	bool _volumeIncrease = false;
	uint16_t _frequency = 0;

	uint32_t _timer = 0;

	//"Few FDS NSFs write to this register. The BIOS initializes this to $FF."
	uint8_t _masterSpeed = 0xFF;

	void StreamState(bool saving) override
	{
		Stream(_speed, _gain, _envelopeOff, _volumeIncrease, _frequency, _timer, _masterSpeed);
	}

public:
	void SetMasterEnvelopeSpeed(uint8_t masterSpeed)
	{
		_masterSpeed = masterSpeed;
	}

	virtual void WriteReg(uint16_t addr, uint8_t value)
	{
		switch(addr & 0x03) {
			case 0:
				_speed = value & 0x3F;
				_volumeIncrease = (value & 0x40) == 0x40;
				_envelopeOff = (value & 0x80) == 0x80;

				//"Writing to this register immediately resets the clock timer that ticks the volume envelope (delaying the next tick slightly)."
				ResetTimer();

				if(_envelopeOff) {
					//Envelope is off, gain = speed
					_gain = _speed;
				}
				break;

			case 2:
				_frequency = (_frequency & 0x0F00) | value;
				break;

			case 3:
				_frequency = (_frequency & 0xFF) | ((value & 0x0F) << 8);
				break;
		}
	}

	bool TickEnvelope()
	{
		if(!_envelopeOff && _masterSpeed > 0) {
			_timer--;
			if(_timer == 0) {
				ResetTimer();

				if(_volumeIncrease && _gain < 32) {
					_gain++;
				} else if(!_volumeIncrease && _gain > 0) {
					_gain--;
				}
				return true;
			}
		}
		return false;
	}

	uint8_t GetGain()
	{
		return _gain;
	}

	uint16_t GetFrequency()
	{
		return _frequency;
	}

	void ResetTimer()
	{
		_timer = 8 * (_speed + 1) * _masterSpeed;
	}
};