#pragma once

#ifndef __HC55516_H__
#define __HC55516_H__

class hc55516_device : public device_t,
									public device_sound_interface
{
public:
	hc55516_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	hc55516_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source);
	~hc55516_device() {}

	/* sets the digit (0 or 1) */
	void digit_w(int digit);

	/* sets the clock state (0 or 1, clocked on the rising edge) */
	void clock_w(int state);

	/* returns whether the clock is currently LO or HI */
	int clock_state_r();

protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples);

	void start_common(UINT8 _shiftreg_mask, int _active_clock_hi);

	// internal state
	sound_stream *m_channel;
	int     m_active_clock_hi;
	UINT8   m_shiftreg_mask;

	UINT8   m_last_clock_state;
	UINT8   m_digit;
	UINT8   m_new_digit;
	UINT8   m_shiftreg;

	INT16   m_curr_sample;
	INT16   m_next_sample;

	UINT32  m_update_count;

	double  m_filter;
	double  m_integrator;

	double  m_charge;
	double  m_decay;
	double  m_leak;

	inline int is_external_oscillator();
	inline int is_active_clock_transition(int clock_state);
	inline int current_clock_state();
	void process_digit();
};

extern const device_type HC55516;

class mc3417_device : public hc55516_device
{
public:
	mc3417_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
protected:
	// device-level overrides
	virtual void device_start();

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples);
};

extern const device_type MC3417;

class mc3418_device : public hc55516_device
{
public:
	mc3418_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
protected:
	// device-level overrides
	virtual void device_start();

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples);
};

extern const device_type MC3418;


#endif /* __HC55516_H__ */
