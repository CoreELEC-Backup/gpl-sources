#pragma once
#include "stdafx.h"
#include "BaseMapper.h"
#include "CPU.h"
#include "Sunsoft5bAudio.h"

class SunsoftFme7 : public BaseMapper
{
private:
	unique_ptr<Sunsoft5bAudio> _audio;
	uint8_t _command;
	uint8_t _workRamValue;
	bool _irqEnabled;
	bool _irqCounterEnabled;
	uint16_t _irqCounter;

protected:
	virtual uint16_t GetPRGPageSize() override { return 0x2000; }
	virtual uint16_t GetCHRPageSize() override { return 0x400; }
	virtual uint32_t GetWorkRamSize() override { return 0x8000; }
	virtual uint32_t GetWorkRamPageSize() override { return 0x2000; }
	virtual uint32_t GetSaveRamSize() override { return 0x8000; }
	virtual uint32_t GetSaveRamPageSize() override { return 0x2000; }

	void InitMapper() override
	{
		_audio.reset(new Sunsoft5bAudio(_console));

		_command = 0;
		_workRamValue = 0;
		_irqEnabled = false;
		_irqCounterEnabled = false;
		_irqCounter = 0;

		SelectPRGPage(3, -1);

		UpdateWorkRam();
	}

	void StreamState(bool saving) override
	{
		BaseMapper::StreamState(saving);
		SnapshotInfo audio{ _audio.get() };
		Stream(_command, _workRamValue, _irqEnabled, _irqCounterEnabled, _irqCounter, audio);
		if(!saving) {
			UpdateWorkRam();
		}
	}

	void ProcessCpuClock() override
	{
		if(_irqCounterEnabled) {
			_irqCounter--;
			if(_irqCounter == 0xFFFF) {
				if(_irqEnabled) {
					_console->GetCpu()->SetIrqSource(IRQSource::External);
				}
			}
		}

		_audio->Clock();
	}

	void UpdateWorkRam()
	{
		if(_workRamValue & 0x40) {
			MemoryAccessType accessType = (_workRamValue & 0x80) ? MemoryAccessType::ReadWrite : MemoryAccessType::NoAccess;
			SetCpuMemoryMapping(0x6000, 0x7FFF, _workRamValue & 0x3F, HasBattery() ? PrgMemoryType::SaveRam : PrgMemoryType::WorkRam, accessType);
		} else {
			SetCpuMemoryMapping(0x6000, 0x7FFF, _workRamValue & 0x3F, PrgMemoryType::PrgRom);
		}
	}

	void WriteRegister(uint16_t addr, uint8_t value) override
	{
		switch(addr & 0xE000) {
			case 0x8000:
				_command = value;
				break;
			case 0xA000:
				switch(_command) {
					case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7:
						SelectCHRPage(_command, value);
						break;

					case 8: {
						_workRamValue = value;
						UpdateWorkRam();
						break;
					}

					case 9: case 0xA: case 0xB:
						SelectPRGPage(_command - 9, value & 0x3F);
						break;

					case 0xC:
						switch(value & 0x03) {
							case 0: SetMirroringType(MirroringType::Vertical); break;
							case 1: SetMirroringType(MirroringType::Horizontal); break;
							case 2: SetMirroringType(MirroringType::ScreenAOnly); break;
							case 3: SetMirroringType(MirroringType::ScreenBOnly); break;
						}
						break;

					case 0xD:
						_irqEnabled = (value & 0x01) == 0x01;
						_irqCounterEnabled = (value & 0x80) == 0x80;
						_console->GetCpu()->ClearIrqSource(IRQSource::External);
						break;

					case 0xE:
						_irqCounter = (_irqCounter & 0xFF00) | value;
						break;

					case 0xF:
						_irqCounter = (_irqCounter & 0xFF) | (value << 8);
						break;
				}
				break;

			case 0xC000:
			case 0xE000:
				_audio->WriteRegister(addr, value);
				break;
		}
	}
};