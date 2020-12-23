// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    speaker.c

    Speaker output sound device.

***************************************************************************/

#include "emu.h"
#include "emuopts.h"
#include "osdepend.h"
#include "config.h"
#include "sound/wavwrite.h"



//**************************************************************************
//  DEBUGGING
//**************************************************************************

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

// device type definition
const device_type SPEAKER = &device_creator<speaker_device>;



//**************************************************************************
//  LIVE SPEAKER DEVICE
//**************************************************************************

//-------------------------------------------------
//  speaker_device - constructor
//-------------------------------------------------

speaker_device::speaker_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, SPEAKER, "Speaker", tag, owner, clock, "speaker", __FILE__),
		device_mixer_interface(mconfig, *this),
		m_x(0.0),
		m_y(0.0),
		m_z(0.0)
{
}


//-------------------------------------------------
//  ~speaker_device - destructor
//-------------------------------------------------

speaker_device::~speaker_device()
{
}


//-------------------------------------------------
//  static_set_position - configuration helper to
//  set the speaker position
//-------------------------------------------------

void speaker_device::static_set_position(device_t &device, double x, double y, double z)
{
	speaker_device &speaker = downcast<speaker_device &>(device);
	speaker.m_x = x;
	speaker.m_y = y;
	speaker.m_z = z;
}


//-------------------------------------------------
//  mix - mix in samples from the speaker's stream
//-------------------------------------------------

void speaker_device::mix(INT32 *leftmix, INT32 *rightmix, int &samples_this_update, bool suppress)
{
	// skip if no stream
	if (m_mixer_stream == NULL)
		return;

	// update the stream, getting the start/end pointers around the operation
	int numsamples;
	const stream_sample_t *stream_buf = m_mixer_stream->output_since_last_update(0, numsamples);

	// set or assert that all streams have the same count
	if (samples_this_update == 0)
	{
		samples_this_update = numsamples;

		// reset the mixing streams
		memset(leftmix, 0, samples_this_update * sizeof(*leftmix));
		memset(rightmix, 0, samples_this_update * sizeof(*rightmix));
	}
	assert(samples_this_update == numsamples);

	// mix if sound is enabled
	if (!suppress)
	{
		// if the speaker is centered, send to both left and right
		if (m_x == 0)
			for (int sample = 0; sample < samples_this_update; sample++)
			{
				leftmix[sample] += stream_buf[sample];
				rightmix[sample] += stream_buf[sample];
			}

		// if the speaker is to the left, send only to the left
		else if (m_x < 0)
			for (int sample = 0; sample < samples_this_update; sample++)
				leftmix[sample] += stream_buf[sample];

		// if the speaker is to the right, send only to the right
		else
			for (int sample = 0; sample < samples_this_update; sample++)
				rightmix[sample] += stream_buf[sample];
	}
}


//-------------------------------------------------
//  device_start - handle device startup
//-------------------------------------------------

void speaker_device::device_start()
{
	// dummy save to make device.c happy
}
