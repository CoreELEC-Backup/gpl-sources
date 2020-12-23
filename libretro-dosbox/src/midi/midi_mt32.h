#include "mt32emu.h"
#include "mixer.h"
#include "control.h"

//fix for functions that are not the same in all versions of libc
#include "nonlibc.h"

//get system dir
#include "retrodos.h"

//pathnames
#include <string>

class RingBuffer {
private:
	static const unsigned int bufferSize = 1024;
	unsigned int startpos;
	unsigned int endpos;
	Bit32u ringBuffer[bufferSize];

public:
	RingBuffer() {
		startpos = 0;
		endpos = 0;
	}

	bool put(Bit32u data) {
		unsigned int newEndpos = endpos;
		newEndpos++;
		if (newEndpos == bufferSize) newEndpos = 0;
		if (startpos == newEndpos) return false;
		ringBuffer[endpos] = data;
		endpos = newEndpos;
		return true;
	}

	Bit32u get() {
		if (startpos == endpos) return 0;
		Bit32u data = ringBuffer[startpos];
		startpos++;
		if (startpos == bufferSize) startpos = 0;
		return data;
	}
};

static class MidiHandler_mt32 : public MidiHandler {
private:
	MixerChannel *chan;
	MT32Emu::Synth *synth;
	RingBuffer midiBuffer;
	bool open, noise, reverseStereo;

	class MT32ReportHandler : public MT32Emu::ReportHandler {
	protected:
		virtual void onErrorControlROM() {
			LOG_MSG("MT32: Couldn't open Control ROM file");
		}

		virtual void onErrorPCMROM() {
			LOG_MSG("MT32: Couldn't open PCM ROM file");
		}

		virtual void showLCDMessage(const char *message) {
			LOG_MSG("MT32: LCD-Message: %s", message);
		}

		virtual void printDebug(const char *fmt, va_list list);
	} reportHandler;

public:
	MidiHandler_mt32() : open(false), chan(NULL), synth(NULL) {}

	~MidiHandler_mt32() {
		Close();
	}

	const char *GetName(void) {
		return "mt32";
	}

	bool Open(const char *conf) {
		MT32Emu::FileStream controlROMFile;
		MT32Emu::FileStream pcmROMFile;
      
      char* syspath;
      std::string path;
      bool worked = environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY,(void *)&syspath);
      if(!worked)return false;
      
      path = syspath;
      if(path[path.length() - 1] != '/')path += '/';
      path += "MT32_CONTROL.ROM";
      if (!controlROMFile.open(path.c_str())) {
         LOG_MSG("MT32: Control ROM file not found");
         return false;
      }
      path = syspath;
      if(path[path.length() - 1] != '/')path += '/';
      path += "MT32_PCM.ROM";
      if (!pcmROMFile.open(path.c_str())) {
         LOG_MSG("MT32: PCM ROM file not found");
         return false;
      }
      
		const MT32Emu::ROMImage *controlROMImage = MT32Emu::ROMImage::makeROMImage(&controlROMFile);
		const MT32Emu::ROMImage *pcmROMImage = MT32Emu::ROMImage::makeROMImage(&pcmROMFile);
		synth = new MT32Emu::Synth(&reportHandler);
		if (!synth->open(*controlROMImage, *pcmROMImage)) {
			LOG_MSG("MT32: Error initialising emulation");
			return false;
		}

		Section_prop *section = static_cast<Section_prop *>(control->GetSection("midi"));
		if (strcmp(section->Get_string("mt32.reverb.mode"), "auto") != 0) {
			Bit8u reverbsysex[] = {0x10, 0x00, 0x01, 0x00, 0x05, 0x03};
			reverbsysex[3] = (Bit8u)atoi(section->Get_string("mt32.reverb.mode"));
			reverbsysex[4] = (Bit8u)section->Get_int("mt32.reverb.time");
			reverbsysex[5] = (Bit8u)section->Get_int("mt32.reverb.level");
			synth->writeSysex(16, reverbsysex, 6);
			synth->setReverbOverridden(true);
		} else {
			LOG_MSG("MT32: Using default reverb");
		}

		if (strcmp(section->Get_string("mt32.dac"), "auto") != 0) {
			synth->setDACInputMode((MT32Emu::DACInputMode)atoi(section->Get_string("mt32.dac")));
		}

		reverseStereo = strcmp(section->Get_string("mt32.reverse.stereo"), "on") == 0;
		noise = strcmp(section->Get_string("mt32.verbose"), "on") == 0;

		chan = MIXER_AddChannel(mixerCallBack, MT32Emu::SAMPLE_RATE, "MT32");
		chan->Enable(true);

		open = true;
		return true;
	}

	void Close(void) {
		if (!open) return;
		chan->Enable(false);
		MIXER_DelChannel(chan);
		chan = NULL;
		synth->close();
		delete synth;
		synth = NULL;
		open = false;
	}

	void PlayMsg(Bit8u *msg) {
		if (!midiBuffer.put(*(Bit32u *)msg)) LOG_MSG("MT32: Playback buffer full!");
	}

	void PlaySysex(Bit8u *sysex, Bitu len) {
		synth->playSysex(sysex, len);
	}

private:
	static void mixerCallBack(Bitu len);

	void render(Bitu len, Bit16s *buf) {
		Bit32u msg = midiBuffer.get();
		if (msg != 0) synth->playMsg(msg);
		synth->render(buf, len);
		if (reverseStereo) {
			Bit16s *revBuf = buf;
			for(Bitu i = 0; i < len; i++) {
				Bit16s left = revBuf[0];
				Bit16s right = revBuf[1];
				*revBuf++ = right;
				*revBuf++ = left;
			}
		}
		chan->AddSamples_s16(len, buf);
	}
} midiHandler_mt32;

void MidiHandler_mt32::MT32ReportHandler::printDebug(const char *fmt, va_list list) {
	if (midiHandler_mt32.noise) {
		char s[1024];
		strcpy(s, "MT32: ");
		portable_vsnprintf(s + 6, 1017, fmt, list);
		//LOG_MSG(s);
	}
}

void MidiHandler_mt32::mixerCallBack(Bitu len) {
	midiHandler_mt32.render(len, (Bit16s *)MixTemp);
}
