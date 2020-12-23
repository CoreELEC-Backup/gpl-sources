// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    osdepend.h

    OS-dependent code interface.

*******************************************************************c********/

#pragma once

#ifndef __OSDOBJ_COMMON_H__
#define __OSDOBJ_COMMON__

#include "osdepend.h"
#include "modules/osdmodule.h"
#include "modules/font/font_module.h"
#include "modules/netdev/netdev_module.h"
#include "modules/midi/midi_module.h"
#include "cliopts.h"

//============================================================
//  Defines
//============================================================

#define OSDCOMMAND_LIST_MIDI_DEVICES    "listmidi"
#define OSDCOMMAND_LIST_NETWORK_ADAPTERS "listnetwork"

#define OSDOPTION_DEBUGGER              "debugger"
#define OSDOPTION_WATCHDOG              "watchdog"

#define OSDOPTION_MULTITHREADING        "multithreading"
#define OSDOPTION_NUMPROCESSORS         "numprocessors"
#define OSDOPTION_BENCH                 "bench"

#define OSDOPTION_VIDEO                 "video"
#define OSDOPTION_NUMSCREENS            "numscreens"
#define OSDOPTION_WINDOW                "window"
#define OSDOPTION_MAXIMIZE              "maximize"
#define OSDOPTION_KEEPASPECT            "keepaspect"
#define OSDOPTION_UNEVENSTRETCH         "unevenstretch"
#define OSDOPTION_WAITVSYNC             "waitvsync"
#define OSDOPTION_SYNCREFRESH           "syncrefresh"

#define OSDOPTION_SCREEN                "screen"
#define OSDOPTION_ASPECT                "aspect"
#define OSDOPTION_RESOLUTION            "resolution"
#define OSDOPTION_VIEW                  "view"

#define OSDOPTION_SWITCHRES             "switchres"

#define OSDOPTION_SOUND                 "sound"
#define OSDOPTION_AUDIO_LATENCY         "audio_latency"

#define OSDOPTION_FILTER                "filter"
#define OSDOPTION_PRESCALE              "prescale"

#define OSDOPTION_SHADER_MAME           "glsl_shader_mame"
#define OSDOPTION_SHADER_SCREEN         "glsl_shader_screen"
#define OSDOPTION_GLSL_FILTER           "gl_glsl_filter"
#define OSDOPTION_GL_GLSL               "gl_glsl"
#define OSDOPTION_GL_PBO                "gl_pbo"
#define OSDOPTION_GL_VBO                "gl_vbo"
#define OSDOPTION_GL_NOTEXTURERECT      "gl_notexturerect"
#define OSDOPTION_GL_FORCEPOW2TEXTURE   "gl_forcepow2texture"


#define OSDOPTVAL_AUTO                  "auto"
#define OSDOPTVAL_NONE                  "none"



//============================================================
//  TYPE DEFINITIONS
//============================================================

class osd_options : public cli_options
{
public:
	// construction/destruction
	osd_options();

	int watchdog() const { return int_value(OSDOPTION_WATCHDOG); }

	// performance options
	bool multithreading() const { return bool_value(OSDOPTION_MULTITHREADING); }
	const char *numprocessors() const { return value(OSDOPTION_NUMPROCESSORS); }
	int bench() const { return int_value(OSDOPTION_BENCH); }

	// video options
	const char *video() const { return value(OSDOPTION_VIDEO); }
	int numscreens() const { return int_value(OSDOPTION_NUMSCREENS); }
	bool window() const { return bool_value(OSDOPTION_WINDOW); }
	bool maximize() const { return bool_value(OSDOPTION_MAXIMIZE); }
	bool keep_aspect() const { return bool_value(OSDOPTION_KEEPASPECT); }
	bool uneven_stretch() const { return bool_value(OSDOPTION_UNEVENSTRETCH); }
	bool wait_vsync() const { return bool_value(OSDOPTION_WAITVSYNC); }
	bool sync_refresh() const { return bool_value(OSDOPTION_SYNCREFRESH); }

	// per-window options
	const char *screen() const { return value(OSDOPTION_SCREEN); }
	const char *aspect() const { return value(OSDOPTION_ASPECT); }
	const char *resolution() const { return value(OSDOPTION_RESOLUTION); }
	const char *view() const { return value(OSDOPTION_VIEW); }
	const char *screen(int index) const { astring temp; return value(temp.format("%s%d", OSDOPTION_SCREEN, index)); }
	const char *aspect(int index) const { astring temp; return value(temp.format("%s%d", OSDOPTION_ASPECT, index)); }
	const char *resolution(int index) const { astring temp; return value(temp.format("%s%d", OSDOPTION_RESOLUTION, index)); }
	const char *view(int index) const { astring temp; return value(temp.format("%s%d", OSDOPTION_VIEW, index)); }

	// full screen options
	bool switch_res() const { return bool_value(OSDOPTION_SWITCHRES); }

	// sound options
	const char *sound() const { return value(OSDOPTION_SOUND); }
	int audio_latency() const { return int_value(OSDOPTION_AUDIO_LATENCY); }

	// video options
	bool filter() const { return bool_value(OSDOPTION_FILTER); }
	int prescale() const { return int_value(OSDOPTION_PRESCALE); }

	// OpenGL specific options
	bool gl_force_pow2_texture() const { return bool_value(OSDOPTION_GL_FORCEPOW2TEXTURE); }
	bool gl_no_texture_rect() const { return bool_value(OSDOPTION_GL_NOTEXTURERECT); }
	bool gl_vbo() const { return bool_value(OSDOPTION_GL_VBO); }
	bool gl_pbo() const { return bool_value(OSDOPTION_GL_PBO); }
	bool gl_glsl() const { return bool_value(OSDOPTION_GL_GLSL); }
	bool glsl_filter() const { return bool_value(OSDOPTION_GLSL_FILTER); }
	const char *shader_mame(int index) const { astring temp; return value(temp.format("%s%d", OSDOPTION_SHADER_MAME, index)); }
	const char *shader_screen(int index) const { astring temp; return value(temp.format("%s%d", OSDOPTION_SHADER_SCREEN, index)); }

private:
	static const options_entry s_option_entries[];
};

// ======================> osd_interface

// description of the currently-running machine
class osd_common_t : public osd_interface, osd_output
{
public:
	// construction/destruction
	osd_common_t(osd_options &options);
	virtual ~osd_common_t();

	// FIXME: simply option handling
	virtual void register_options();

	// general overridables
	virtual void init(running_machine &machine);
	virtual void update(bool skip_redraw, UINT32 flags);

	// audio overridables
	virtual void update_audio_stream(const INT16 *buffer, int samples_this_frame);
	virtual void set_mastervolume(int attenuation);
	virtual bool no_sound();

	// input overridables
	virtual void customize_input_type_list(simple_list<input_type_entry> &typelist);

	// font overridables
	virtual osd_font *font_open(const char *name, int &height);
	virtual void font_close(osd_font *font);
	virtual bool font_get_bitmap(osd_font *font, unicode_char chnum, bitmap_argb32 &bitmap, INT32 &width, INT32 &xoffs, INT32 &yoffs);

	// video overridables
	virtual void *get_slider_list();

	// command option overrides
	virtual bool execute_command(const char *command);

	osd_font *font_alloc() { return m_font_module->font_alloc(); }

	osd_midi_device *create_midi_device() { return m_midi->create_midi_device(); }

	// FIXME: everything below seems to be osd specific and not part of
	//        this INTERFACE but part of the osd IMPLEMENTATION

	// getters
	running_machine &machine() { assert(m_machine != NULL); return *m_machine; }

	virtual void init_subsystems();

	virtual bool video_init();
	virtual void video_register();
	virtual bool window_init();

	virtual bool output_init();

	virtual void exit_subsystems();
	virtual void video_exit();
	virtual void window_exit();
	virtual void input_exit();
	virtual void output_exit();

	virtual void osd_exit();

	virtual void video_options_add(const char *name, void *type);

	osd_options &options() { return m_options; }

	// osd_output interface ...
	virtual void output_callback(osd_output_channel channel, const char *msg, va_list args);

protected:
   virtual bool input_init();


private:
	// internal state
	running_machine *   m_machine;
	osd_options& m_options;

	osd_module_manager m_mod_man;
	font_module *m_font_module;

	void update_option(const char * key, dynamic_array<const char *> &values);
	// FIXME: should be elsewhere
	osd_module *select_module_options(const core_options &opts, const astring &opt_name)
	{
		astring opt_val = opts.value(opt_name);
		if (opt_val == "auto")
			opt_val = "";
		else if (!m_mod_man.type_has_name(opt_name, opt_val))
		{
			osd_printf_warning("Value %s not supported for option %s - falling back to auto\n", opt_val.cstr(), opt_name.cstr());
			opt_val = "";
		}
		return m_mod_man.select_module(opt_name, opt_val);
	}

	template<class C>
	C select_module_options(const core_options &opts, const astring &opt_name)
	{
		return dynamic_cast<C>(select_module_options(opts, opt_name));
	}

protected:
	midi_module* m_midi;
private:
	dynamic_array<const char *> m_video_names;
};

#endif  /* __OSDOBJ_COMMON_H__ */
