//
// DAC (really, Synchronous Serial Interface) Handler
//
// Originally by David Raingeard
// GCC/SDL port by Niels Wagenaar (Linux/WIN32) and Caz (BeOS)
// Rewritten by James Hammons
// (C) 2010 Underground Software
//
// JLH = James Hammons <jlhamm@acm.org>
//
// Who  When        What
// ---  ----------  -------------------------------------------------------------
// JLH  01/16/2010  Created this log ;-)
// JLH  04/30/2012  Changed SDL audio handler to run JERRY
//

// Need to set up defaults that the BIOS sets for the SSI here in DACInit()... !!! FIX !!!
// or something like that... Seems like it already does, but it doesn't seem to
// work correctly...! Perhaps just need to set up SSI stuff so BUTCH doesn't get
// confused...

// After testing on a real Jaguar, it seems clear that the I2S interrupt drives
// the audio subsystem. So while you can drive the audio at a *slower* rate than
// set by SCLK, you can't drive it any *faster*. Also note, that if the I2S
// interrupt is not enabled/running on the DSP, then there is no audio. Also,
// audio can be muted by clearing bit 8 of JOYSTICK (JOY1).
//
// Approach: We can run the DSP in the host system's audio IRQ, by running the
// DSP for the alloted time (depending on the host buffer size & sample rate)
// by simply reading the L/R_I2S (L/RTXD) registers at regular intervals. We
// would also have to time the I2S/TIMER0/TIMER1 interrupts in the DSP as well.
// This way, we can run the host audio IRQ at, say, 48 KHz and not have to care
// so much about SCLK and running a separate buffer and all the attendant
// garbage that comes with that awful approach.
//
// There would still be potential gotchas, as the SCLK can theoretically drive
// the I2S at 26590906 / 2 (for SCLK == 0) = 13.3 MHz which corresponds to an
// audio rate 416 KHz (dividing the I2S rate by 32, for 16-bit stereo). It
// seems doubtful that anything useful could come of such a high rate, and we
// can probably safely ignore any such ridiculously high audio rates. It won't
// sound the same as on a real Jaguar, but who cares? :-)

#include "dac.h"

#include "cdrom.h"
#include "dsp.h"
#include "event.h"
#include "jerry.h"
#include "jaguar.h"
#include "log.h"
#include "m68000/m68kinterface.h"
#include "settings.h"

#include "../libretro.h"

extern retro_audio_sample_batch_t audio_batch_cb;

#define BUFFER_SIZE			0x10000				// Make the DAC buffers 64K x 16 bits
#define DAC_AUDIO_RATE		48000				// Set the audio rate to 48 KHz

#define SILENCE 0

// Jaguar memory locations

#define LTXD			0xF1A148
#define RTXD			0xF1A14C
#define LRXD			0xF1A148
#define RRXD			0xF1A14C
#define SCLK			0xF1A150
#define SMODE			0xF1A154

// Global variables
uint16_t * sampleBuffer;
static int bufferIndex = 0;
static int numberOfSamples = 0;
static bool bufferDone = false;

// Local variables

//static uint8_t SCLKFrequencyDivider = 19;			// Default is roughly 22 KHz (20774 Hz in NTSC mode)

// Private function prototypes

void DACInit(void)
{
   uint32_t riscClockRate;
   uint32_t cyclesPerSample;

   DACReset();

   *ltxd = SILENCE;
   lrxd = SILENCE;
   *sclk = 19;									// Default is roughly 22 KHz

   riscClockRate = (vjs.hardwareTypeNTSC ? RISC_CLOCK_RATE_NTSC : RISC_CLOCK_RATE_PAL);
   cyclesPerSample = riscClockRate / DAC_AUDIO_RATE;
   WriteLog("DAC: RISC clock = %u, cyclesPerSample = %u\n", riscClockRate, cyclesPerSample);
}


// Reset the sound buffer FIFOs
void DACReset(void)
{
   *ltxd = SILENCE;
   lrxd = SILENCE;
}

void DACPauseAudioThread(bool state)
{
}


void DACDone(void)
{
}

void DSPSampleCallback(void)
{
   sampleBuffer[bufferIndex + 0] = *ltxd;
   sampleBuffer[bufferIndex + 1] = *rtxd;
   bufferIndex += 2;

   if (bufferIndex == numberOfSamples)
   {
      bufferDone = true;
      return;
   }

   SetCallbackTime(DSPSampleCallback, 1000000.0 / (double)DAC_AUDIO_RATE, EVENT_JERRY);
}

// Approach: Run the DSP for however many cycles needed to correspond to whatever sample rate
// we've set the audio to run at. So, e.g., if we run it at 48 KHz, then we would run the DSP
// for however much time it takes to fill the buffer. So with a 2K buffer, this would correspond
// to running the DSP for 0.042666... seconds. At 26590906 Hz, this would correspond to
// running the DSP for 1134545 cycles. You would then sample the L/RTXD registers every
// 1134545 / 2048 = 554 cycles to fill the buffer. You would also have to manage interrupt
// timing as well (generating them at the proper times), but that shouldn't be too difficult...
// If the DSP isn't running, then fill the buffer with L/RTXD and exit.

//
// Callback routine to fill audio buffer
//
// Note: The samples are packed in the buffer in 16 bit left/16 bit right pairs.
//       Also, length is the length of the buffer in BYTES
//

void SDLSoundCallback(void * userdata, uint16_t * buffer, int length)
{
   /* 1st, check to see if the DSP is running. If not, fill the buffer with L/RXTD and exit. */
   if (!DSPIsRunning())
   {
      unsigned i;
      for(i = 0; i < length; i += 2)
      {
         buffer[i + 0] = *ltxd;
         buffer[i + 1] = *rtxd;
      }

      return;
   }

   // The length of time we're dealing with here is 1/48000 s, so we multiply this
   // by the number of cycles per second to get the number of cycles for one sample.
   //uint32_t riscClockRate = (vjs.hardwareTypeNTSC ? RISC_CLOCK_RATE_NTSC : RISC_CLOCK_RATE_PAL);
   //uint32_t cyclesPerSample = riscClockRate / DAC_AUDIO_RATE;
   // This is the length of time
   //	timePerSample = (1000000.0 / (double)riscClockRate) * ();

   // Now, run the DSP for that length of time for each sample we need to make

   bufferIndex     = 0;
   sampleBuffer    = buffer;
   // If length is the length of the sample buffer in BYTES, then shouldn't the # of
   // samples be / 4? No, because we bump the sample count by 2, so this is OK.
   numberOfSamples = length;
   bufferDone      = false;

   SetCallbackTime(DSPSampleCallback, 1000000.0 / (double)DAC_AUDIO_RATE, EVENT_JERRY);

   // These timings are tied to NTSC, need to fix that in event.cpp/h! [FIXED]
   do
   {
      double timeToNextEvent = GetTimeToNextEvent(EVENT_JERRY);

      DSPExec(USEC_TO_RISC_CYCLES(timeToNextEvent));

      HandleNextEvent(EVENT_JERRY);
   }
   while (!bufferDone);
   audio_batch_cb((int16_t*)sampleBuffer, length / 2);
}

// LTXD/RTXD/SCLK/SMODE ($F1A148/4C/50/54)
void DACWriteByte(uint32_t offset, uint8_t data, uint32_t who)
{
   if (offset == SCLK + 3)
      DACWriteWord(offset - 3, (uint16_t)data, UNKNOWN);
}


void DACWriteWord(uint32_t offset, uint16_t data, uint32_t who)
{
   if (offset == LTXD + 2)
      *ltxd = data;
   else if (offset == RTXD + 2)
      *rtxd = data;
   else if (offset == SCLK + 2)					// Sample rate
   {
      *sclk = data & 0xFF;
      JERRYI2SInterruptTimer = -1;
      RemoveCallback(JERRYI2SCallback);
      JERRYI2SCallback();
   }
   else if (offset == SMODE + 2)
      *smode = data;
}

uint8_t DACReadByte(uint32_t offset, uint32_t who)
{
   return 0xFF;
}

uint16_t DACReadWord(uint32_t offset, uint32_t who)
{
   if (offset == LRXD || offset == RRXD)
      return 0x0000;
   else if (offset == LRXD + 2)
      return lrxd;
   else if (offset == RRXD + 2)
      return rrxd;

   return 0xFFFF;	// May need SSTAT as well... (but may be a Jaguar II only feature)
}
