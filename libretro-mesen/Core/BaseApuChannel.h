#pragma once
#include "stdafx.h"
#include "IMemoryHandler.h"
#include "EmulationSettings.h"
#include "Snapshotable.h"
#include "SoundMixer.h"
#include "Console.h"

class BaseApuChannel : public IMemoryHandler, public Snapshotable
{
private:
	SoundMixer *_mixer;
	uint32_t _previousCycle;
	AudioChannel _channel;
	NesModel _nesModel;

protected:
	int8_t _lastOutput;
	uint16_t _timer = 0;
	uint16_t _period = 0;
	shared_ptr<Console> _console;

	AudioChannel GetChannel()
	{
		return _channel;
	}

public:
	virtual void Clock() = 0;
	virtual bool GetStatus() = 0;

	BaseApuChannel(AudioChannel channel, shared_ptr<Console> console, SoundMixer *mixer)
	{
		_channel = channel;
		_mixer = mixer;
		_console = console;
		_nesModel = NesModel::NTSC;
		
		Reset(false);
	}	

	virtual void Reset(bool softReset)
	{
		_timer = 0;
		_period = 0;
		_lastOutput = 0;
		_previousCycle = 0;
		if(_mixer) {
			//_mixer is null/not needed for MMC5 square channels
			_mixer->Reset();
		}
	}

	virtual void StreamState(bool saving) override
	{
		if(!saving) {
			_previousCycle = 0;
		}

		Stream(_lastOutput, _timer, _period, _nesModel);
	}

	void SetNesModel(NesModel model)
	{
		_nesModel = model;
	}

	NesModel GetNesModel()
	{
		if(_nesModel == NesModel::NTSC || _nesModel == NesModel::Dendy) {
			//Dendy APU works with NTSC timings
			return NesModel::NTSC;
		} else {
			return _nesModel;
		}
	}

	void Run(uint32_t targetCycle)
	{
		int32_t cyclesToRun = targetCycle - _previousCycle;
		while(cyclesToRun > _timer) {
			cyclesToRun -= _timer + 1;
			_previousCycle += _timer + 1;
			Clock();
			_timer = _period;
		}

		_timer -= cyclesToRun;
		_previousCycle = targetCycle;
	}

	uint8_t ReadRAM(uint16_t addr) override
	{
		return 0;
	}

	void AddOutput(int8_t output)
	{
		if(output != _lastOutput) {
			_mixer->AddDelta(_channel, _previousCycle, output - _lastOutput);
			_lastOutput = output;
		}
	}

	void EndFrame()
	{
		_previousCycle = 0;
	}
};