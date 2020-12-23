#pragma once
#include "stdafx.h"
#include "IMemoryHandler.h"
#include "SoundMixer.h"
#include "OggMixer.h"
#include "Snapshotable.h"

struct HdPackData;
class Console;

class HdAudioDevice : public IMemoryHandler, public Snapshotable
{
private:
	HdPackData *_hdData;
	uint8_t _album;
	uint8_t _playbackOptions;
	bool _trackError;
	OggMixer* _oggMixer;
	int32_t _lastBgmTrack;
	uint8_t _bgmVolume;
	uint8_t _sfxVolume;
	
	bool PlayBgmTrack(uint8_t track, uint32_t startOffset);
	bool PlaySfx(uint8_t sfxNumber);
	void ProcessControlFlags(uint8_t flags);

protected:
	void StreamState(bool saving) override;

public:
	HdAudioDevice(shared_ptr<Console> console, HdPackData *hdData);

	void GetMemoryRanges(MemoryRanges &ranges) override;
	void WriteRAM(uint16_t addr, uint8_t value) override;
	uint8_t ReadRAM(uint16_t addr) override;
};