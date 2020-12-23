// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    video.h

    Core MAME video routines.

***************************************************************************/

#pragma once

#ifndef __EMU_H__
#error Dont include this file directly; include emu.h instead.
#endif

#ifndef __VIDEO_H__
#define __VIDEO_H__


//**************************************************************************
//  CONSTANTS
//**************************************************************************

// number of levels of frameskipping supported
const int FRAMESKIP_LEVELS = 12;
const int MAX_FRAMESKIP = FRAMESKIP_LEVELS - 2;

#define LCD_FRAMES_PER_SECOND   30

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// forward references
class render_target;
class screen_device;
struct avi_file;



// ======================> video_manager

class video_manager
{
	friend class screen_device;

public:
	// construction/destruction
	video_manager(running_machine &machine);

	// getters
	running_machine &machine() const { return m_machine; }
	bool skip_this_frame() const { return m_skipping_this_frame; }
	int speed_factor() const { return m_speed; }
	int frameskip() const { return m_auto_frameskip ? -1 : m_frameskip_level; }
	bool throttled() const { return m_throttled; }
	float throttle_rate() const { return m_throttle_rate; }
	bool fastforward() const { return m_fastforward; }

	// setters
	void set_frameskip(int frameskip);
	void set_throttled(bool throttled = true) { m_throttled = throttled; }
	void set_throttle_rate(float throttle_rate) { m_throttle_rate = throttle_rate; }
	void set_fastforward(bool ffwd = true) { m_fastforward = ffwd; }
	void set_output_changed() { m_output_changed = true; }

	// misc
	void toggle_throttle();

	// render a frame
	void frame_update(bool debug = false);

	// current speed helpers
	astring &speed_text(astring &string);
	double speed_percent() const { return m_speed_percent; }

private:
	// internal helpers
	void exit();
	void screenless_update_callback(void *ptr, int param);
	void postload();

	// effective value helpers
	int effective_autoframeskip() const;
	int effective_frameskip() const;
	bool effective_throttle() const;

	// speed and throttling helpers
	int original_speed_setting() const;
	bool finish_screen_updates();
	void update_frameskip();
	void update_refresh_speed();

	// snapshot/movie helpers
	file_error open_next(emu_file &file, const char *extension);

	// internal state
	running_machine &   m_machine;                  // reference to our machine

	// screenless systems
	emu_timer *         m_screenless_frame_timer;   // timer to signal VBLANK start
	bool                m_output_changed;           // did an output element change?

	// throttling calculations
	osd_ticks_t         m_throttle_last_ticks;      // osd_ticks the last call to throttle
	attotime            m_throttle_realtime;        // real time the last call to throttle
	attotime            m_throttle_emutime;         // emulated time the last call to throttle
	UINT32              m_throttle_history;         // history of frames where we were fast enough

	// dynamic speed computation
	osd_ticks_t         m_speed_last_realtime;      // real time at the last speed calculation
	attotime            m_speed_last_emutime;       // emulated time at the last speed calculation
	double              m_speed_percent;            // most recent speed percentage

	// overall speed computation
	UINT32              m_overall_real_seconds;     // accumulated real seconds at normal speed
	osd_ticks_t         m_overall_real_ticks;       // accumulated real ticks at normal speed
	attotime            m_overall_emutime;          // accumulated emulated time at normal speed
	UINT32              m_overall_valid_counter;    // number of consecutive valid time periods

	// configuration
	bool                m_throttled;                // flag: TRUE if we're currently throttled
	float               m_throttle_rate;            // target rate for throttling
	bool                m_fastforward;              // flag: TRUE if we're currently fast-forwarding
	UINT32              m_seconds_to_run;           // number of seconds to run before quitting
	bool                m_auto_frameskip;           // flag: TRUE if we're automatically frameskipping
	UINT32              m_speed;                    // overall speed (*1000)

	// frameskipping
	UINT8               m_empty_skip_count;         // number of empty frames we have skipped
	UINT8               m_frameskip_level;          // current frameskip level
	UINT8               m_frameskip_counter;        // counter that counts through the frameskip steps
	INT8                m_frameskip_adjust;
	bool                m_skipping_this_frame;      // flag: TRUE if we are skipping the current frame
	osd_ticks_t         m_average_oversleep;        // average number of ticks the OSD oversleeps

	static const UINT8      s_skiptable[FRAMESKIP_LEVELS][FRAMESKIP_LEVELS];

	static const attoseconds_t ATTOSECONDS_PER_SPEED_UPDATE = ATTOSECONDS_PER_SECOND / 4;
	static const int PAUSED_REFRESH_RATE = 30;
};

#endif  /* __VIDEO_H__ */
