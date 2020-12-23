#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "osdepend.h"
#include "emu.h"
#include "render.h"
#include "ui/ui.h"
#include "uiinput.h"
#include "drivenum.h"

#include "libretro.h"
#include "retroosd.h"

#include "clifront.h"
#include "osdobj_common.h"
#include "modules/lib/osdlib.h"
#include "modules/osdmodule.h"
#include "modules/font/font_module.h"

#include "libretro_shared.h"

#ifdef _WIN32
char slash = '\\';
#else
char slash = '/';
#endif

/* Args for experimental_commandline */
static char ARGUV[32][1024];
static unsigned char ARGUC=0;
extern retro_audio_sample_batch_t audio_batch_cb;

extern bool g_print_verbose;

typedef struct joystate_t
{
   int button[MAX_BUTTONS];
   int a1[2];
   int a2[2];
}Joystate;

/* rendering target */
render_target *our_target = NULL;

/* input device */
static input_device *retrokbd_device; // KEYBD
static input_device *mouse_device;    // MOUSE
static input_device *joy_device[4];// JOY0/JOY1/JOY2/JOY3
static input_device *Pad_device[4];// PAD0/PAD1/PAD2/PAD3

/* state */
UINT16 retrokbd_state[RETROK_LAST];
int mouseLX;
int mouseLY;
int mouseBUT[4];
static Joystate joystate[4];

int ui_ipt_pushchar=-1;

int mame_reset = -1;

/* core options */
bool hide_nagscreen = false;
bool hide_warnings = false;
bool nobuffer_enable = false;

bool hide_gameinfo = false;
bool mouse_enable = false;
bool cheats_enable = false;
bool alternate_renderer = false;
bool boot_to_osd_enable = false;
bool boot_to_bios_enable = false;
bool experimental_cmdline = false;
bool softlist_enable = false;
bool softlist_auto = false;
bool write_config_enable = false;
bool read_config_enable = false;
bool auto_save_enable = false;
bool throttle_enable = false;
bool game_specific_saves_enable = false;

// emu flags
static int tate = 0;
static int screenRot = 0;
int vertical,orient;
static bool arcade=FALSE;
static int FirstTimeUpdate = 1;

// rom file name and path
char g_rom_dir[1024];
char mediaType[10];
static char MgamePath[1024];
static char MparentPath[1024];
static char MgameName[512];
static char MsystemName[512];
static char gameName[1024];

// args for cores
static char XARGV[64][1024];
static const char* xargv_cmd[64];
int PARAMCOUNT=0;


// path configuration
#define NB_OPTPATH 12

static const char *dir_name[NB_OPTPATH]= {
    "cfg","nvram","hi"/*,"memcard"*/,"input",
    "states" ,"snaps","diff","samples",
    "artwork","cheat","ini","hash"
};

static const char *opt_name[NB_OPTPATH]= {
    "-cfg_directory","-nvram_directory","-hiscore_directory",/*"-memcard_directory",*/"-input_directory",
    "-state_directory" ,"-snapshot_directory","-diff_directory","-samplepath",
    "-artpath","-cheatpath","-inipath","-hashpath"
};

int opt_type[NB_OPTPATH]={ // 0 for save_dir | 1 for system_dir
    0,0,0,0,
    0,0,0,1,
    1,1,1,1
};

#if defined(HAVE_OPENGL) || defined(HAVE_OPENGLES)
static int init3d=1;
#else
#include "rendersw.inc"
#endif

//============================================================
//  OSD
//============================================================
const options_entry osd_options::s_option_entries[] =
{
	{ NULL,                                   NULL,       OPTION_HEADER,     "OSD FONT OPTIONS" },
	{ OSD_FONT_PROVIDER,                      "auto",     OPTION_STRING,     "provider for ui font: " },

	{ NULL,                                   NULL,       OPTION_HEADER,     "OSD CLI OPTIONS" },
	{ OSDCOMMAND_LIST_MIDI_DEVICES ";mlist",  "0",        OPTION_COMMAND,    "list available MIDI I/O devices" },
	{ OSDCOMMAND_LIST_NETWORK_ADAPTERS ";nlist", "0",     OPTION_COMMAND,    "list available network adapters" },

	// debugging options
	{ NULL,                                   NULL,       OPTION_HEADER,     "OSD DEBUGGING OPTIONS" },
	{ OSDOPTION_DEBUGGER,                     OSDOPTVAL_AUTO,      OPTION_STRING,    "debugger used : " },
	{ OSDOPTION_WATCHDOG ";wdog",             "0",        OPTION_INTEGER,    "force the program to terminate if no updates within specified number of seconds" },

	// performance options
	{ NULL,                                   NULL,       OPTION_HEADER,     "OSD PERFORMANCE OPTIONS" },
	{ OSDOPTION_MULTITHREADING ";mt",         "0",        OPTION_BOOLEAN,    "enable multithreading; this enables rendering and blitting on a separate thread" },
	{ OSDOPTION_NUMPROCESSORS ";np",          OSDOPTVAL_AUTO,      OPTION_STRING,     "number of processors; this overrides the number the system reports" },
	{ OSDOPTION_BENCH,                        "0",        OPTION_INTEGER,    "benchmark for the given number of emulated seconds; implies -video none -sound none -nothrottle" },
	// video options
	{ NULL,                                   NULL,       OPTION_HEADER,     "OSD VIDEO OPTIONS" },
// OS X can be trusted to have working hardware OpenGL, so default to it on for the best user experience
	{ OSDOPTION_VIDEO,                        OSDOPTVAL_AUTO,     OPTION_STRING,     "video output method: " },
	{ OSDOPTION_NUMSCREENS "(1-4)",           "1",        OPTION_INTEGER,    "number of screens to create; usually, you want just one" },
	{ OSDOPTION_WINDOW ";w",                  "0",        OPTION_BOOLEAN,    "enable window mode; otherwise, full screen mode is assumed" },
	{ OSDOPTION_MAXIMIZE ";max",              "1",        OPTION_BOOLEAN,    "default to maximized windows; otherwise, windows will be minimized" },
	{ OSDOPTION_KEEPASPECT ";ka",             "1",        OPTION_BOOLEAN,    "constrain to the proper aspect ratio" },
	{ OSDOPTION_UNEVENSTRETCH ";ues",         "1",        OPTION_BOOLEAN,    "allow non-integer stretch factors" },
	{ OSDOPTION_WAITVSYNC ";vs",              "0",        OPTION_BOOLEAN,    "enable waiting for the start of VBLANK before flipping screens; reduces tearing effects" },
	{ OSDOPTION_SYNCREFRESH ";srf",           "0",        OPTION_BOOLEAN,    "enable using the start of VBLANK for throttling instead of the game time" },

	// per-window options
	{ NULL,                                   NULL,             OPTION_HEADER,    "OSD PER-WINDOW VIDEO OPTIONS" },
	{ OSDOPTION_SCREEN,                   OSDOPTVAL_AUTO,   OPTION_STRING,    "explicit name of the first screen; 'auto' here will try to make a best guess" },
	{ OSDOPTION_ASPECT ";screen_aspect",  OSDOPTVAL_AUTO,   OPTION_STRING,    "aspect ratio for all screens; 'auto' here will try to make a best guess" },
	{ OSDOPTION_RESOLUTION ";r",          OSDOPTVAL_AUTO,   OPTION_STRING,    "preferred resolution for all screens; format is <width>x<height>[@<refreshrate>] or 'auto'" },
	{ OSDOPTION_VIEW,                     OSDOPTVAL_AUTO,   OPTION_STRING,    "preferred view for all screens" },

	{ OSDOPTION_SCREEN "0",                  OSDOPTVAL_AUTO,   OPTION_STRING,    "explicit name of the first screen; 'auto' here will try to make a best guess" },
	{ OSDOPTION_ASPECT "0",                  OSDOPTVAL_AUTO,   OPTION_STRING,    "aspect ratio of the first screen; 'auto' here will try to make a best guess" },
	{ OSDOPTION_RESOLUTION "0;r0",        OSDOPTVAL_AUTO,   OPTION_STRING,    "preferred resolution of the first screen; format is <width>x<height>[@<refreshrate>] or 'auto'" },
	{ OSDOPTION_VIEW "0",                    OSDOPTVAL_AUTO,   OPTION_STRING,    "preferred view for the first screen" },

	{ OSDOPTION_SCREEN "1",                  OSDOPTVAL_AUTO,   OPTION_STRING,    "explicit name of the second screen; 'auto' here will try to make a best guess" },
	{ OSDOPTION_ASPECT "1",                  OSDOPTVAL_AUTO,   OPTION_STRING,    "aspect ratio of the second screen; 'auto' here will try to make a best guess" },
	{ OSDOPTION_RESOLUTION "1;r1",        OSDOPTVAL_AUTO,   OPTION_STRING,    "preferred resolution of the second screen; format is <width>x<height>[@<refreshrate>] or 'auto'" },
	{ OSDOPTION_VIEW "1",                    OSDOPTVAL_AUTO,   OPTION_STRING,    "preferred view for the second screen" },

	{ OSDOPTION_SCREEN "2",                  OSDOPTVAL_AUTO,   OPTION_STRING,    "explicit name of the third screen; 'auto' here will try to make a best guess" },
	{ OSDOPTION_ASPECT "2",                  OSDOPTVAL_AUTO,   OPTION_STRING,    "aspect ratio of the third screen; 'auto' here will try to make a best guess" },
	{ OSDOPTION_RESOLUTION "2;r2",        OSDOPTVAL_AUTO,   OPTION_STRING,    "preferred resolution of the third screen; format is <width>x<height>[@<refreshrate>] or 'auto'" },
	{ OSDOPTION_VIEW "2",                    OSDOPTVAL_AUTO,   OPTION_STRING,    "preferred view for the third screen" },

	{ OSDOPTION_SCREEN "3",                  OSDOPTVAL_AUTO,   OPTION_STRING,    "explicit name of the fourth screen; 'auto' here will try to make a best guess" },
	{ OSDOPTION_ASPECT "3",                  OSDOPTVAL_AUTO,   OPTION_STRING,    "aspect ratio of the fourth screen; 'auto' here will try to make a best guess" },
	{ OSDOPTION_RESOLUTION "3;r3",        OSDOPTVAL_AUTO,   OPTION_STRING,    "preferred resolution of the fourth screen; format is <width>x<height>[@<refreshrate>] or 'auto'" },
	{ OSDOPTION_VIEW "3",                    OSDOPTVAL_AUTO,   OPTION_STRING,    "preferred view for the fourth screen" },

	// full screen options
	{ NULL,                                   NULL,  OPTION_HEADER,     "OSD FULL SCREEN OPTIONS" },
	{ OSDOPTION_SWITCHRES,                    "0",   OPTION_BOOLEAN,    "enable resolution switching" },

	// sound options
	{ NULL,                                   NULL,  OPTION_HEADER,     "OSD SOUND OPTIONS" },
	{ OSDOPTION_SOUND,                        OSDOPTVAL_AUTO, OPTION_STRING,     "sound output method: " },
	{ OSDOPTION_AUDIO_LATENCY "(1-5)",        "2",   OPTION_INTEGER,    "set audio latency (increase to reduce glitches, decrease for responsiveness)" },

	{ NULL,                                   NULL,   OPTION_HEADER,  "OSD VIDEO OPTIONS" },
	{ OSDOPTION_FILTER ";glfilter;flt",       "1",    OPTION_BOOLEAN, "enable bilinear filtering on screen output" },
	{ OSDOPTION_PRESCALE,                     "1",    OPTION_INTEGER,                 "scale screen rendering by this amount in software" },

#ifdef USE_OPENGL
	// OpenGL specific options
	{ NULL,                                   NULL,   OPTION_HEADER,  "OpenGL-SPECIFIC OPTIONS" },
	{ OSDOPTION_GL_FORCEPOW2TEXTURE,          "0",    OPTION_BOOLEAN, "force power of two textures  (default no)" },
	{ OSDOPTION_GL_NOTEXTURERECT,             "0",    OPTION_BOOLEAN, "don't use OpenGL GL_ARB_texture_rectangle (default on)" },
	{ OSDOPTION_GL_VBO,                       "1",    OPTION_BOOLEAN, "enable OpenGL VBO,  if available (default on)" },
	{ OSDOPTION_GL_PBO,                       "1",    OPTION_BOOLEAN, "enable OpenGL PBO,  if available (default on)" },
	{ OSDOPTION_GL_GLSL,                      "0",    OPTION_BOOLEAN, "enable OpenGL GLSL, if available (default off)" },
	{ OSDOPTION_GLSL_FILTER,                  "1",    OPTION_STRING,  "enable OpenGL GLSL filtering instead of FF filtering 0-plain, 1-bilinear (default)" },
	{ OSDOPTION_SHADER_MAME "0",     OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader set mame bitmap 0" },
	{ OSDOPTION_SHADER_MAME "1",     OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader set mame bitmap 1" },
	{ OSDOPTION_SHADER_MAME "2",     OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader set mame bitmap 2" },
	{ OSDOPTION_SHADER_MAME "3",     OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader set mame bitmap 3" },
	{ OSDOPTION_SHADER_MAME "4",     OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader set mame bitmap 4" },
	{ OSDOPTION_SHADER_MAME "5",     OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader set mame bitmap 5" },
	{ OSDOPTION_SHADER_MAME "6",     OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader set mame bitmap 6" },
	{ OSDOPTION_SHADER_MAME "7",     OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader set mame bitmap 7" },
	{ OSDOPTION_SHADER_MAME "8",     OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader set mame bitmap 8" },
	{ OSDOPTION_SHADER_MAME "9",     OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader set mame bitmap 9" },
	{ OSDOPTION_SHADER_SCREEN "0",   OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader screen bitmap 0" },
	{ OSDOPTION_SHADER_SCREEN "1",   OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader screen bitmap 1" },
	{ OSDOPTION_SHADER_SCREEN "2",   OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader screen bitmap 2" },
	{ OSDOPTION_SHADER_SCREEN "3",   OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader screen bitmap 3" },
	{ OSDOPTION_SHADER_SCREEN "4",   OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader screen bitmap 4" },
	{ OSDOPTION_SHADER_SCREEN "5",   OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader screen bitmap 5" },
	{ OSDOPTION_SHADER_SCREEN "6",   OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader screen bitmap 6" },
	{ OSDOPTION_SHADER_SCREEN "7",   OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader screen bitmap 7" },
	{ OSDOPTION_SHADER_SCREEN "8",   OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader screen bitmap 8" },
	{ OSDOPTION_SHADER_SCREEN "9",   OSDOPTVAL_NONE,  OPTION_STRING,  "custom OpenGL GLSL shader screen bitmap 9" },
#endif

	// End of list
	{ NULL }
};

osd_options::osd_options()
: cli_options()
{
	add_entries(osd_options::s_option_entries);
};


//-------------------------------------------------
//  osd_interface - constructor
//-------------------------------------------------

osd_common_t::osd_common_t(osd_options &options)
	: osd_output(), m_machine(NULL),
		m_options(options)
{
	osd_output::push(this);
}

//-------------------------------------------------
//  osd_interface - destructor
//-------------------------------------------------

osd_common_t::~osd_common_t()
{
	for(int i= 0; i < m_video_names.count(); ++i)
		osd_free(const_cast<char*>(m_video_names[i]));
	//m_video_options,reset();
	osd_output::pop(this);
}

#define REGISTER_MODULE(_O, _X ) { extern const module_type _X; _O . register_module( _X ); }

void osd_common_t::register_options()
{
	REGISTER_MODULE(m_mod_man, FONT_NONE);

#ifndef NO_USE_MIDI
	REGISTER_MODULE(m_mod_man, MIDI_PM);
#endif
	REGISTER_MODULE(m_mod_man, MIDI_NONE);

	// after initialization we know which modules are supported

	const char *names[20];
	int num;
	m_mod_man.get_module_names(OSD_FONT_PROVIDER, 20, &num, names);
	dynamic_array<const char *> dnames;
	for (int i = 0; i < num; i++)
		dnames.append(names[i]);
	update_option(OSD_FONT_PROVIDER, dnames);

	// Register video options and update options
	video_options_add("none", NULL);
	video_register();
	update_option(OSDOPTION_VIDEO, m_video_names);
}

void osd_common_t::update_option(const char * key, dynamic_array<const char *> &values)
{
	astring current_value(m_options.description(key));
	astring new_option_value("");
	for (int index = 0; index < values.count(); index++)
	{
		astring t(values[index]);
		if (new_option_value.len() > 0)
		{
			if( index != (values.count()-1))
				new_option_value.cat(", ");
			else
				new_option_value.cat(" or ");
		}
		new_option_value.cat(t);
	}
	// TODO: core_strdup() is leaked
	m_options.set_description(key, core_strdup(current_value.cat(new_option_value).cstr()));
}


//-------------------------------------------------
//  output_callback  - callback for osd_printf_...
//-------------------------------------------------
void osd_common_t::output_callback(osd_output_channel channel, const char *msg, va_list args)
{
	switch (channel)
	{
		case OSD_OUTPUT_CHANNEL_ERROR:
		case OSD_OUTPUT_CHANNEL_WARNING:
			vfprintf(stderr, msg, args);
			break;
		case OSD_OUTPUT_CHANNEL_INFO:
		case OSD_OUTPUT_CHANNEL_VERBOSE:
		case OSD_OUTPUT_CHANNEL_LOG:
			vfprintf(stdout, msg, args);
			break;
		case OSD_OUTPUT_CHANNEL_DEBUG:
#ifdef MAME_DEBUG
			vfprintf(stdout, msg, args);
#endif
			break;
		default:
			break;
	}
}

//-------------------------------------------------
//  init - initialize the OSD system.
//-------------------------------------------------

void osd_common_t::init(running_machine &machine)
{
	//
	// This function is responsible for initializing the OSD-specific
	// video and input functionality, and registering that functionality
	// with the MAME core.
	//
	// In terms of video, this function is expected to create one or more
	// render_targets that will be used by the MAME core to provide graphics
	// data to the system. Although it is possible to do this later, the
	// assumption in the MAME core is that the user interface will be
	// visible starting at init() time, so you will have some work to
	// do to avoid these assumptions.
	//
	// In terms of input, this function is expected to enumerate all input
	// devices available and describe them to the MAME core by adding
	// input devices and their attached items (buttons/axes) via the input
	// system.
	//
	// Beyond these core responsibilities, init() should also initialize
	// any other OSD systems that require information about the current
	// running_machine.
	//
	// This callback is also the last opportunity to adjust the options
	// before they are consumed by the rest of the core.
	//
	// Future work/changes:
	//
	// Audio initialization may eventually move into here as well,
	// instead of relying on independent callbacks from each system.
	//

	m_machine = &machine;

	osd_options &options = downcast<osd_options &>(machine.options());
	// extract the verbose printing option
	if (options.verbose())
		g_print_verbose = true;

	// ensure we get called on the way out
	machine.add_notifier(MACHINE_NOTIFY_EXIT, machine_notify_delegate(FUNC(osd_common_t::osd_exit), this));
}


//-------------------------------------------------
//  update - periodic system update
//-------------------------------------------------

void osd_common_t::update(bool skip_redraw, UINT32 flags)
{
	//
	// This method is called periodically to flush video updates to the
	// screen, and also to allow the OSD a chance to update other systems
	// on a regular basis. In general this will be called at the frame
	// rate of the system being run; however, it may be called at more
	// irregular intervals in some circumstances (e.g., multi-screen games
	// or games with asynchronous updates).
	//
}

//-------------------------------------------------
//  update_audio_stream - update the stereo audio
//  stream
//-------------------------------------------------

void osd_common_t::update_audio_stream(const INT16 *buffer, int samples_this_frame)
{
	//
	// This method is called whenever the system has new audio data to stream.
	// It provides an array of stereo samples in L-R order which should be
	// output at the configured sample_rate.
	//
   if (retro_pause != -1)
      audio_batch_cb(buffer, samples_this_frame);
}


//-------------------------------------------------
//  set_mastervolume - set the system volume
//-------------------------------------------------

void osd_common_t::set_mastervolume(int attenuation)
{
	//
	// Attenuation is the attenuation in dB (a negative number).
	// To convert from dB to a linear volume scale do the following:
	//    volume = MAX_VOLUME;
	//    while (attenuation++ < 0)
	//       volume /= 1.122018454;      //  = (10 ^ (1/20)) = 1dB
	//
}


//-------------------------------------------------
//  customize_input_type_list - provide OSD
//  additions/modifications to the input list
//-------------------------------------------------

void osd_common_t::customize_input_type_list(simple_list<input_type_entry> &typelist)
{
	//
	// inptport.c defines some general purpose defaults for key and joystick bindings.
	// They may be further adjusted by the OS dependent code to better match the
	// available keyboard, e.g. one could map pause to the Pause key instead of P, or
	// snapshot to PrtScr instead of F12. Of course the user can further change the
	// settings to anything he/she likes.
	//
	// This function is called on startup, before reading the configuration from disk.
	// Scan the list, and change the keys/joysticks you want.
	//
}


//-------------------------------------------------
//  font_open - attempt to "open" a handle to the
//  font with the given name
//-------------------------------------------------

osd_font *osd_common_t::font_open(const char *name, int &height)
{
	return NULL;
}


//-------------------------------------------------
//  font_close - release resources associated with
//  a given OSD font
//-------------------------------------------------

void osd_common_t::font_close(osd_font *font)
{
}


//-------------------------------------------------
//  font_get_bitmap - allocate and populate a
//  BITMAP_FORMAT_ARGB32 bitmap containing the
//  pixel values rgb_t(0xff,0xff,0xff,0xff)
//  or rgb_t(0x00,0xff,0xff,0xff) for each
//  pixel of a black & white font
//-------------------------------------------------

bool osd_common_t::font_get_bitmap(osd_font *font, unicode_char chnum, bitmap_argb32 &bitmap, INT32 &width, INT32 &xoffs, INT32 &yoffs)
{
	return false;
}

//-------------------------------------------------
//  get_slider_list - allocate and populate a
//  list of OS-dependent slider values.
//-------------------------------------------------

void *osd_common_t::get_slider_list()
{
	return NULL;
}

//-------------------------------------------------
//  execute_command - execute a command not yet
//  handled by the core
//-------------------------------------------------

bool osd_common_t::execute_command(const char *command)
{
	if (strcmp(command, OSDCOMMAND_LIST_NETWORK_ADAPTERS) == 0)
	{
		osd_module *om = select_module_options(options(), OSD_NETDEV_PROVIDER);

		if (om->probe())
		{
			om->init();
			osd_list_network_adapters();
			om->exit();
		}

		return true;
	}
	else if (strcmp(command, OSDCOMMAND_LIST_MIDI_DEVICES) == 0)
	{
		osd_module *om = select_module_options(options(), OSD_MIDI_PROVIDER);
		midi_module *pm = select_module_options<midi_module *>(options(), OSD_MIDI_PROVIDER);

		if (om->probe())
		{
			om->init();
			pm->list_midi_devices();
			om->exit();
		}
		return true;
	}

	return false;

}

void osd_common_t::init_subsystems()
{
	if (!video_init())
	{
		video_exit();
		osd_printf_error("video_init: Initialization failed!\n\n\n");
		fflush(stderr);
		fflush(stdout);
		exit(-1);
	}

	input_init();
	output_init();

	m_font_module = select_module_options<font_module *>(options(), OSD_FONT_PROVIDER);

	select_module_options<netdev_module *>(options(), OSD_NETDEV_PROVIDER);

	m_midi = select_module_options<midi_module *>(options(), OSD_MIDI_PROVIDER);

	m_mod_man.init();

}

bool osd_common_t::video_init()
{
	return true;
}

bool osd_common_t::window_init()
{
	return true;
}

bool osd_common_t::no_sound()
{
	return (strcmp(options().sound(),"none")==0) ? true : false;
}

void osd_common_t::video_register()
{
}

bool osd_common_t::input_init()
{
	return true;
}

bool osd_common_t::output_init()
{
	return true;
}

void osd_common_t::exit_subsystems()
{
	video_exit();
	input_exit();
	output_exit();
}

void osd_common_t::video_exit()
{
}

void osd_common_t::window_exit()
{
}

void osd_common_t::input_exit()
{
}

void osd_common_t::output_exit()
{
}

void osd_common_t::osd_exit()
{
	m_mod_man.exit();

	exit_subsystems();
}

void osd_common_t::video_options_add(const char *name, void *type)
{
	//m_video_options.add(name, type, false);
	m_video_names.append(core_strdup(name));
}

//============================================================
//  INPUT
//============================================================

#ifndef RETROK_TILDE
#define RETROK_TILDE 178
#endif

static UINT16 retrokbd_state2[RETROK_LAST];

struct kt_table
{
   const char  *   mame_key_name;
   int retro_key_name;
   input_item_id   mame_key;
};

kt_table ktable[]={
{"A",RETROK_a,ITEM_ID_A},
{"B",RETROK_b,ITEM_ID_B},
{"C",RETROK_c,ITEM_ID_C},
{"D",RETROK_d,ITEM_ID_D},
{"E",RETROK_e,ITEM_ID_E},
{"F",RETROK_f,ITEM_ID_F},
{"G",RETROK_g,ITEM_ID_G},
{"H",RETROK_h,ITEM_ID_H},
{"I",RETROK_i,ITEM_ID_I},
{"J",RETROK_j,ITEM_ID_J},
{"K",RETROK_k,ITEM_ID_K},
{"L",RETROK_l,ITEM_ID_L},
{"M",RETROK_m,ITEM_ID_M},
{"N",RETROK_n,ITEM_ID_N},
{"O",RETROK_o,ITEM_ID_O},
{"P",RETROK_p,ITEM_ID_P},
{"Q",RETROK_q,ITEM_ID_Q},
{"R",RETROK_r,ITEM_ID_R},
{"S",RETROK_s,ITEM_ID_S},
{"T",RETROK_t,ITEM_ID_T},
{"U",RETROK_u,ITEM_ID_U},
{"V",RETROK_v,ITEM_ID_V},
{"W",RETROK_w,ITEM_ID_W},
{"X",RETROK_x,ITEM_ID_X},
{"Y",RETROK_y,ITEM_ID_Y},
{"Z",RETROK_z,ITEM_ID_Z},
{"0",RETROK_0,ITEM_ID_0},
{"1",RETROK_1,ITEM_ID_1},
{"2",RETROK_2,ITEM_ID_2},
{"3",RETROK_3,ITEM_ID_3},
{"4",RETROK_4,ITEM_ID_4},
{"5",RETROK_5,ITEM_ID_5},
{"6",RETROK_6,ITEM_ID_6},
{"7",RETROK_7,ITEM_ID_7},
{"8",RETROK_8,ITEM_ID_8},
{"9",RETROK_9,ITEM_ID_9},
{"F1",RETROK_F1,ITEM_ID_F1},
{"F2",RETROK_F2,ITEM_ID_F2},
{"F3",RETROK_F3,ITEM_ID_F3},
{"F4",RETROK_F4,ITEM_ID_F4},
{"F5",RETROK_F5,ITEM_ID_F5},
{"F6",RETROK_F6,ITEM_ID_F6},
{"F7",RETROK_F7,ITEM_ID_F7},
{"F8",RETROK_F8,ITEM_ID_F8},
{"F9",RETROK_F9,ITEM_ID_F9},
{"F10",RETROK_F10,ITEM_ID_F10},
{"F11",RETROK_F11,ITEM_ID_F11},
{"F12",RETROK_F12,ITEM_ID_F12},
{"F13",RETROK_F13,ITEM_ID_F13},
{"F14",RETROK_F14,ITEM_ID_F14},
{"F15",RETROK_F15,ITEM_ID_F15},
{"Esc",RETROK_ESCAPE,ITEM_ID_ESC},
{"TILDE",RETROK_TILDE,ITEM_ID_TILDE},
{"MINUS",RETROK_MINUS,ITEM_ID_MINUS},
{"EQUALS",RETROK_EQUALS,ITEM_ID_EQUALS},
{"BKCSPACE",RETROK_BACKSPACE,ITEM_ID_BACKSPACE},
{"TAB",RETROK_TAB,ITEM_ID_TAB},
{"(",RETROK_LEFTPAREN,ITEM_ID_OPENBRACE},
{")",RETROK_RIGHTPAREN,ITEM_ID_CLOSEBRACE},
{"ENTER",RETROK_RETURN,ITEM_ID_ENTER},
{"·",RETROK_COLON,ITEM_ID_COLON},
{"\'",RETROK_QUOTE,ITEM_ID_QUOTE},
{"BCKSLASH",RETROK_BACKSLASH,ITEM_ID_BACKSLASH},
///**/BCKSLASH2*/RETROK_,ITEM_ID_BACKSLASH2},
{",",RETROK_COMMA,ITEM_ID_COMMA},
///**/STOP*/RETROK_,ITEM_ID_STOP},
{"/",RETROK_SLASH,ITEM_ID_SLASH},
{"SPACE",RETROK_SPACE,ITEM_ID_SPACE},
{"INS",RETROK_INSERT,ITEM_ID_INSERT},
{"DEL",RETROK_DELETE,ITEM_ID_DEL},
{"HOME",RETROK_HOME,ITEM_ID_HOME},
{"END",RETROK_END,ITEM_ID_END},
{"PGUP",RETROK_PAGEUP,ITEM_ID_PGUP},
{"PGDW",RETROK_PAGEDOWN,ITEM_ID_PGDN},
{"LEFT",RETROK_LEFT,ITEM_ID_LEFT},
{"RIGHT",RETROK_RIGHT,ITEM_ID_RIGHT},
{"UP",RETROK_UP,ITEM_ID_UP},
{"DOWN",RETROK_DOWN,ITEM_ID_DOWN},
{"KO",RETROK_KP0,ITEM_ID_0_PAD},
{"K1",RETROK_KP1,ITEM_ID_1_PAD},
{"K2",RETROK_KP2,ITEM_ID_2_PAD},
{"K3",RETROK_KP3,ITEM_ID_3_PAD},
{"K4",RETROK_KP4,ITEM_ID_4_PAD},
{"K5",RETROK_KP5,ITEM_ID_5_PAD},
{"K6",RETROK_KP6,ITEM_ID_6_PAD},
{"K7",RETROK_KP7,ITEM_ID_7_PAD},
{"K8",RETROK_KP8,ITEM_ID_8_PAD},
{"K9",RETROK_KP9,ITEM_ID_9_PAD},
{"K/",RETROK_KP_DIVIDE,ITEM_ID_SLASH_PAD},
{"K*",RETROK_KP_MULTIPLY,ITEM_ID_ASTERISK},
{"K-",RETROK_KP_MINUS,ITEM_ID_MINUS_PAD},
{"K+",RETROK_KP_PLUS,ITEM_ID_PLUS_PAD},
{"KDEL",RETROK_KP_PERIOD,ITEM_ID_DEL_PAD},
{"KRTRN",RETROK_KP_ENTER,ITEM_ID_ENTER_PAD},
{"PRINT",RETROK_PRINT,ITEM_ID_PRTSCR},
{"PAUSE",RETROK_PAUSE,ITEM_ID_PAUSE},
{"LSHFT",RETROK_LSHIFT,ITEM_ID_LSHIFT},
{"RSHFT",RETROK_RSHIFT,ITEM_ID_RSHIFT},
{"LCTRL",RETROK_LCTRL,ITEM_ID_LCONTROL},
{"RCTRL",RETROK_RCTRL,ITEM_ID_RCONTROL},
{"LALT",RETROK_LALT,ITEM_ID_LALT},
{"RALT",RETROK_RALT,ITEM_ID_RALT},
{"SCRLOCK",RETROK_SCROLLOCK,ITEM_ID_SCRLOCK},
{"NUMLOCK",RETROK_NUMLOCK,ITEM_ID_NUMLOCK},
{"CPSLOCK",RETROK_CAPSLOCK,ITEM_ID_CAPSLOCK},
{"LMETA",RETROK_LMETA,ITEM_ID_LWIN},
{"RMETA",RETROK_RMETA,ITEM_ID_RWIN},
{"MENU",RETROK_MENU,ITEM_ID_MENU},
{"BREAK",RETROK_BREAK,ITEM_ID_CANCEL},
{"-1",-1,ITEM_ID_INVALID},
};

enum
{
   RETROPAD_B,
   RETROPAD_Y,
   RETROPAD_SELECT,
   RETROPAD_START,
   RETROPAD_PAD_UP,
   RETROPAD_PAD_DOWN,
   RETROPAD_PAD_LEFT,
   RETROPAD_PAD_RIGHT,
   RETROPAD_A,
   RETROPAD_X,
   RETROPAD_L,
   RETROPAD_R,
   RETROPAD_L2,
   RETROPAD_R2,
   RETROPAD_L3,
   RETROPAD_R3,
   RETROPAD_TOTAL
};


static const char *Buttons_Name[MAX_BUTTONS]=
{
   "B",		//0
   "Y",		//1
   "SELECT",	//2
   "START",	//3
   "Pad UP",	//4
   "Pad DOWN",	//5
   "Pad LEFT",	//6
   "Pad RIGHT",	//7
   "A",		//8
   "X",		//9
   "L",		//10
   "R",		//11
   "L2",		//12
   "R2",		//13
   "L3",		//14
   "R3",		//15
};

input_item_id PAD_DIR[4][4]=
{
   {ITEM_ID_UP,ITEM_ID_DOWN,ITEM_ID_LEFT,ITEM_ID_RIGHT },
   {ITEM_ID_R ,ITEM_ID_F   ,ITEM_ID_D   ,ITEM_ID_G     },
   {ITEM_ID_I,ITEM_ID_K,ITEM_ID_J,ITEM_ID_L },
   {ITEM_ID_8_PAD ,ITEM_ID_2_PAD   ,ITEM_ID_4_PAD   ,ITEM_ID_6_PAD }
};

//    Default : A ->B1 | B ->B2 | X ->B3 | Y ->B4 | L ->B5 | R ->B6 | keyboard c ->B7 | keyboard v -> B8
int   Buttons_mapping[8]={RETROPAD_A,RETROPAD_B,RETROPAD_X,RETROPAD_Y,RETROPAD_L,RETROPAD_R,RETROK_c,RETROK_v};

static void Input_Binding(running_machine &machine);

static INT32 retrokbd_get_state(void *device_internal, void *item_internal)
{
	UINT8 *itemdata = (UINT8 *)item_internal;
	return *itemdata ;
}

static INT32 generic_axis_get_state(void *device_internal, void *item_internal)
{
	INT32 *axisdata = (INT32 *) item_internal;
	return *axisdata;
}

static INT32 generic_button_get_state(void *device_internal, void *item_internal)
{
	INT32 *itemdata = (INT32 *) item_internal;
	return *itemdata >> 7;
}

#define input_device_item_add_joy(a,b,c,d,e)   joy_device[a]->add_item(b,d,e,c)
#define input_device_item_add_mouse(a,b,c,d,e) mouse_device->add_item(b,d,e,c)
#define input_device_item_add_kbd(a,b,c,d,e)   retrokbd_device->add_item(b,d,e,c)
#define input_device_item_add_pad(a,b,c,d,e)   Pad_device[a]->add_item(b,d,e,c)

void process_keyboard_state(void)
{
   /* TODO: handle mods:SHIFT/CTRL/ALT/META/NUMLOCK/CAPSLOCK/SCROLLOCK */
   unsigned i = 0;
   do
   {
      retrokbd_state[ktable[i].retro_key_name] = input_state_cb(0, RETRO_DEVICE_KEYBOARD, 0,ktable[i].retro_key_name) ? 0x80 : 0;

      if(retrokbd_state[ktable[i].retro_key_name] && retrokbd_state2[ktable[i].retro_key_name] == 0)
      {
         ui_ipt_pushchar=ktable[i].retro_key_name;
         retrokbd_state2[ktable[i].retro_key_name]=1;
      }
      else if(!retrokbd_state[ktable[i].retro_key_name] && retrokbd_state2[ktable[i].retro_key_name] == 1)
         retrokbd_state2[ktable[i].retro_key_name]=0;

      i++;

   }while(ktable[i].retro_key_name!=-1);
}

void process_joypad_state(void)
{
   unsigned i, j;

   for(j = 0;j < 4; j++)
   {
      for(i = 0;i < MAX_BUTTONS; i++)
         joystate[j].button[i] = input_state_cb(j, RETRO_DEVICE_JOYPAD, 0,i)?0x80:0;

      joystate[j].a1[0] = 2 * (input_state_cb(j, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X));
      joystate[j].a1[1] = 2 * (input_state_cb(j, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y));
      joystate[j].a2[0] = 2 * (input_state_cb(j, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_X));
      joystate[j].a2[1] = 2 * (input_state_cb(j, RETRO_DEVICE_ANALOG, RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y));
   }
}

void process_mouse_state(void)
{
   static int mbL = 0, mbR = 0;
   int mouse_l;
   int mouse_r;
   int16_t mouse_x;
   int16_t mouse_y;

   if (!mouse_enable)
      return;

   mouse_x = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_X);
   mouse_y = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_Y);
   mouse_l = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_LEFT);
   mouse_r = input_state_cb(0, RETRO_DEVICE_MOUSE, 0, RETRO_DEVICE_ID_MOUSE_RIGHT);
   mouseLX = mouse_x*INPUT_RELATIVE_PER_PIXEL;
   mouseLY = mouse_y*INPUT_RELATIVE_PER_PIXEL;

   if(mbL==0 && mouse_l)
   {
      mbL=1;
      mouseBUT[0]=0x80;
   }
   else if(mbL==1 && !mouse_l)
   {
      mouseBUT[0]=0;
      mbL=0;
   }

   if(mbR==0 && mouse_r)
   {
      mbR=1;
      mouseBUT[1]=1;
   }
   else if(mbR==1 && !mouse_r)
   {
      mouseBUT[1]=0;
      mbR=0;
   }
}

static void initInput(running_machine &machine)
{
   int i,j,button;
   char defname[20];

   if (mouse_enable)
   {
      //MOUSE
      mouse_device = machine.input().device_class(DEVICE_CLASS_MOUSE).add_device("Mice1");
      // add the axes
      input_device_item_add_mouse(mouse_device , "X", &mouseLX, ITEM_ID_XAXIS, generic_axis_get_state);
      input_device_item_add_mouse(mouse_device , "Y", &mouseLY, ITEM_ID_YAXIS, generic_axis_get_state);
      // add the buttons
      for (button = 0; button < 4; button++)
      {
         input_item_id itemid = (input_item_id) (ITEM_ID_BUTTON1+button);
         sprintf(defname, "B%d", button + 1);
         input_device_item_add_mouse(mouse_device, defname, &mouseBUT[button], itemid, generic_button_get_state);
      }
   }

   //KEYBOARD
   retrokbd_device = machine.input().device_class(DEVICE_CLASS_KEYBOARD).add_device("Retrokdb");

   if (retrokbd_device == NULL)
      fatalerror("KBD Error creating keyboard device\n");


   for(i = 0; i < RETROK_LAST; i++){
      retrokbd_state[i]=0;
      retrokbd_state2[i]=0;
   }

   i=0;
   do{
      input_device_item_add_kbd(retrokbd_device,\
            ktable[i].mame_key_name, &retrokbd_state[ktable[i].retro_key_name],ktable[i].mame_key,retrokbd_get_state);
      i++;
   }while(ktable[i].retro_key_name!=-1);

   //JOY/PAD

   Input_Binding(machine);

   for(i=0;i<4;i++)
   {
      sprintf(defname, "Joy%d", i);
      joy_device[i]=machine.input().device_class(DEVICE_CLASS_JOYSTICK).add_device(defname);

      // add the axes
      input_device_item_add_joy (i, "LX", &joystate[i].a1[0], ITEM_ID_XAXIS, generic_axis_get_state);
      input_device_item_add_joy (i, "LY", &joystate[i].a1[1], ITEM_ID_YAXIS, generic_axis_get_state);
      input_device_item_add_joy (i, "RX", &joystate[i].a2[0], ITEM_ID_RXAXIS, generic_axis_get_state);
      input_device_item_add_joy (i, "RY", &joystate[i].a2[1], ITEM_ID_RYAXIS, generic_axis_get_state);

      //add the buttons
      for(j=0;j<MAX_BUTTONS;j++)joystate[i].button[j] = 0;

      input_device_item_add_joy (i,Buttons_Name[RETROPAD_START],&joystate[i].button[RETROPAD_START],ITEM_ID_START,generic_button_get_state );
      input_device_item_add_joy (i,Buttons_Name[RETROPAD_SELECT],&joystate[i].button[RETROPAD_SELECT],ITEM_ID_SELECT,generic_button_get_state );

      input_device_item_add_joy (i,Buttons_Name[Buttons_mapping[0]],\
            &joystate[i].button[Buttons_mapping[0]],(input_item_id)(ITEM_ID_BUTTON1+0),generic_button_get_state );
      input_device_item_add_joy (i,Buttons_Name[Buttons_mapping[1]],\
            &joystate[i].button[Buttons_mapping[1]],(input_item_id)(ITEM_ID_BUTTON1+1),generic_button_get_state );
      input_device_item_add_joy (i,Buttons_Name[Buttons_mapping[2]],\
            &joystate[i].button[Buttons_mapping[2]],(input_item_id)(ITEM_ID_BUTTON1+2),generic_button_get_state );
      input_device_item_add_joy (i,Buttons_Name[Buttons_mapping[3]],\
            &joystate[i].button[Buttons_mapping[3]],(input_item_id)(ITEM_ID_BUTTON1+3),generic_button_get_state );
      input_device_item_add_joy (i,Buttons_Name[Buttons_mapping[4]],\
            &joystate[i].button[Buttons_mapping[4]],(input_item_id)(ITEM_ID_BUTTON1+4),generic_button_get_state );
      input_device_item_add_joy (i,Buttons_Name[Buttons_mapping[5]],\
            &joystate[i].button[Buttons_mapping[5]],(input_item_id)(ITEM_ID_BUTTON1+5),generic_button_get_state );
      if (Buttons_mapping[6]!=RETROK_c)
      {
         input_device_item_add_joy (i,Buttons_Name[Buttons_mapping[6]],\
               &joystate[i].button[Buttons_mapping[6]],(input_item_id)(ITEM_ID_BUTTON1+6),generic_button_get_state );
      }
      if (Buttons_mapping[7]!=RETROK_v)
      {
         input_device_item_add_joy (i,Buttons_Name[Buttons_mapping[7]],\
               &joystate[i].button[Buttons_mapping[7]],(input_item_id)(ITEM_ID_BUTTON1+7),generic_button_get_state );
      }

      sprintf(defname, "Pad%d", i);
      Pad_device[i] = machine.input().device_class(DEVICE_CLASS_KEYBOARD).add_device(defname);

      input_device_item_add_pad (i,Buttons_Name[RETROPAD_L2], &joystate[i].button[RETROPAD_L2],(input_item_id)(ITEM_ID_TAB+0),retrokbd_get_state );
      input_device_item_add_pad (i,Buttons_Name[RETROPAD_R2], &joystate[i].button[RETROPAD_R2],(input_item_id)(ITEM_ID_F11+0),retrokbd_get_state );
      input_device_item_add_pad (i,Buttons_Name[RETROPAD_L3], &joystate[i].button[RETROPAD_L3],(input_item_id)(ITEM_ID_F2+0),retrokbd_get_state );

      input_device_item_add_pad (i,Buttons_Name[RETROPAD_PAD_UP]   , &joystate[i].button[RETROPAD_PAD_UP]   ,PAD_DIR[i][0],retrokbd_get_state );
      input_device_item_add_pad (i,Buttons_Name[RETROPAD_PAD_DOWN] , &joystate[i].button[RETROPAD_PAD_DOWN] ,PAD_DIR[i][1],retrokbd_get_state );
      input_device_item_add_pad (i,Buttons_Name[RETROPAD_PAD_LEFT] , &joystate[i].button[RETROPAD_PAD_LEFT] ,PAD_DIR[i][2],retrokbd_get_state );
      input_device_item_add_pad (i,Buttons_Name[RETROPAD_PAD_RIGHT], &joystate[i].button[RETROPAD_PAD_RIGHT],PAD_DIR[i][3],retrokbd_get_state );

   }
}

static void Input_Binding(running_machine &machine)
{
   fprintf(stderr, "SOURCE FILE: %s\n", machine.system().source_file);
   fprintf(stderr, "PARENT: %s\n", machine.system().parent);
   fprintf(stderr, "NAME: %s\n", machine.system().name);
   fprintf(stderr, "DESCRIPTION: %s\n", machine.system().description);
   fprintf(stderr, "YEAR: %s\n", machine.system().year);
   fprintf(stderr, "MANUFACTURER: %s\n", machine.system().manufacturer);

   Buttons_mapping[0]=RETROPAD_A;
   Buttons_mapping[1]=RETROPAD_B;
   Buttons_mapping[2]=RETROPAD_X;
   Buttons_mapping[3]=RETROPAD_Y;
   Buttons_mapping[4]=RETROPAD_L;
   Buttons_mapping[5]=RETROPAD_R;

   if (
         (core_stricmp(machine.system().name, "tekken") == 0) ||
         (core_stricmp(machine.system().parent, "tekken") == 0) ||
         (core_stricmp(machine.system().name, "tekken2") == 0) ||
         (core_stricmp(machine.system().parent, "tekken2") == 0)
      )
   {
      /* Tekken 1/2 */

      Buttons_mapping[0]=RETROPAD_Y;
      Buttons_mapping[1]=RETROPAD_X;
      Buttons_mapping[2]=RETROPAD_B;
      Buttons_mapping[3]=RETROPAD_A;
      Buttons_mapping[4]=RETROPAD_L;
      Buttons_mapping[5]=RETROPAD_R;

   }
   else if (
              (core_stricmp(machine.system().name, "souledge") == 0) ||
              (core_stricmp(machine.system().parent, "souledge") == 0) ||
              (core_stricmp(machine.system().name, "soulclbr") == 0) ||
              (core_stricmp(machine.system().parent, "soulclbr") == 0)
           )
   {
      /* Soul Edge/Soul Calibur */

      Buttons_mapping[0]=RETROPAD_Y;
      Buttons_mapping[1]=RETROPAD_X;
      Buttons_mapping[2]=RETROPAD_A;
      Buttons_mapping[3]=RETROPAD_B;
      Buttons_mapping[4]=RETROPAD_L;
      Buttons_mapping[5]=RETROPAD_R;

   }
   else if (
              (core_stricmp(machine.system().name, "doapp") == 0)
           )
   {
      /* Dead or Alive++ */

      Buttons_mapping[0]=RETROPAD_B;
      Buttons_mapping[1]=RETROPAD_Y;
      Buttons_mapping[2]=RETROPAD_X;
      Buttons_mapping[3]=RETROPAD_A;
      Buttons_mapping[4]=RETROPAD_L;
      Buttons_mapping[5]=RETROPAD_R;

   }
   else if (
              (core_stricmp(machine.system().name, "vf") == 0) ||
              (core_stricmp(machine.system().parent, "vf") == 0)
           )
   {
      /* Virtua Fighter */

      Buttons_mapping[0]=RETROPAD_Y;
      Buttons_mapping[1]=RETROPAD_X;
      Buttons_mapping[2]=RETROPAD_B;
      Buttons_mapping[3]=RETROPAD_A;
      Buttons_mapping[4]=RETROPAD_L;
      Buttons_mapping[5]=RETROPAD_R;

   }
   else if (
              (core_stricmp(machine.system().name, "ehrgeiz") == 0) ||
              (core_stricmp(machine.system().parent, "ehrgeiz") == 0)
           )
   {
      /* Ehrgeiz */

      Buttons_mapping[0]=RETROPAD_Y;
      Buttons_mapping[1]=RETROPAD_B;
      Buttons_mapping[2]=RETROPAD_A;
      Buttons_mapping[3]=RETROPAD_X;
      Buttons_mapping[4]=RETROPAD_L;
      Buttons_mapping[5]=RETROPAD_R;

   }
   else if (
              (core_stricmp(machine.system().name, "ts2") == 0) ||
              (core_stricmp(machine.system().parent, "ts2") == 0)
           )
   {
      /* Toshinden 2 */

      Buttons_mapping[0]=RETROPAD_L;
      Buttons_mapping[1]=RETROPAD_Y;
      Buttons_mapping[2]=RETROPAD_X;
      Buttons_mapping[3]=RETROPAD_R;
      Buttons_mapping[4]=RETROPAD_B;
      Buttons_mapping[5]=RETROPAD_A;

   }
   else if (
              (core_stricmp(machine.system().name, "dstlk") == 0) ||
              (core_stricmp(machine.system().parent, "dstlk") == 0) ||
              (core_stricmp(machine.system().name, "hsf2") == 0) ||
              (core_stricmp(machine.system().parent, "hsf2") == 0) ||
              (core_stricmp(machine.system().name, "msh") == 0) ||
              (core_stricmp(machine.system().parent, "msh") == 0) ||
              (core_stricmp(machine.system().name, "mshvsf") == 0) ||
              (core_stricmp(machine.system().parent, "mshvsf") == 0) ||
              (core_stricmp(machine.system().name, "mvsc") == 0) ||
              (core_stricmp(machine.system().parent, "mvsc") == 0) ||
              (core_stricmp(machine.system().name, "nwarr") == 0) ||
              (core_stricmp(machine.system().parent, "nwarr") == 0) ||
              (core_stricmp(machine.system().name, "rvschool") == 0) ||
              (core_stricmp(machine.system().parent, "rvschool") == 0) ||
              (core_stricmp(machine.system().name, "sf2") == 0) ||
              (core_stricmp(machine.system().parent, "sf2") == 0) ||
              (core_stricmp(machine.system().name, "sf2ce") == 0) ||
              (core_stricmp(machine.system().parent, "sf2ce") == 0) ||
              (core_stricmp(machine.system().name, "sf2hf") == 0) ||
              (core_stricmp(machine.system().parent, "sf2hf") == 0) ||
              (core_stricmp(machine.system().name, "sfa") == 0) ||
              (core_stricmp(machine.system().parent, "sfa") == 0) ||
              (core_stricmp(machine.system().name, "sfa2") == 0) ||
              (core_stricmp(machine.system().parent, "sfa2") == 0) ||
              (core_stricmp(machine.system().name, "sfa3") == 0) ||
              (core_stricmp(machine.system().parent, "sfa3") == 0) ||
              (core_stricmp(machine.system().name, "sfex") == 0) ||
              (core_stricmp(machine.system().parent, "sfex") == 0) ||
              (core_stricmp(machine.system().name, "sfex2") == 0) ||
              (core_stricmp(machine.system().parent, "sfex2") == 0) ||
              (core_stricmp(machine.system().name, "sfex2p") == 0) ||
              (core_stricmp(machine.system().parent, "sfex2p") == 0) ||
              (core_stricmp(machine.system().name, "sfexp") == 0) ||
              (core_stricmp(machine.system().parent, "sfexp") == 0) ||
              (core_stricmp(machine.system().name, "sfiii") == 0) ||
              (core_stricmp(machine.system().parent, "sfiii") == 0) ||
              (core_stricmp(machine.system().name, "sfiii2") == 0) ||
              (core_stricmp(machine.system().parent, "sfiii2") == 0) ||
              (core_stricmp(machine.system().name, "sfiii3") == 0) ||
              (core_stricmp(machine.system().parent, "sfiii3") == 0) ||
              (core_stricmp(machine.system().name, "sftm") == 0) ||
              (core_stricmp(machine.system().parent, "sftm") == 0) ||
              (core_stricmp(machine.system().name, "ssf2") == 0) ||
              (core_stricmp(machine.system().parent, "ssf2") == 0) ||
              (core_stricmp(machine.system().name, "ssf2t") == 0) ||
              (core_stricmp(machine.system().parent, "ssf2t") == 0) ||
              (core_stricmp(machine.system().name, "starglad") == 0) ||
              (core_stricmp(machine.system().parent, "starglad") == 0) ||
              (core_stricmp(machine.system().name, "vsav") == 0) ||
              (core_stricmp(machine.system().parent, "vsav") == 0) ||
              (core_stricmp(machine.system().name, "vsav2") == 0) ||
              (core_stricmp(machine.system().parent, "vsav2") == 0) ||
              (core_stricmp(machine.system().name, "xmcota") == 0) ||
              (core_stricmp(machine.system().parent, "xmcota") == 0) ||
              (core_stricmp(machine.system().name, "xmvsf") == 0) ||
              (core_stricmp(machine.system().parent, "xmvsf") == 0)
           )
   {
      /* Capcom CPS-1 and CPS-2 6-button fighting games */

      Buttons_mapping[0]=RETROPAD_Y;
      Buttons_mapping[1]=RETROPAD_X;
      Buttons_mapping[2]=RETROPAD_L;
      Buttons_mapping[3]=RETROPAD_B;
      Buttons_mapping[4]=RETROPAD_A;
      Buttons_mapping[5]=RETROPAD_R;

   }
   else if (
              (core_stricmp(machine.system().name, "dynwar") == 0) ||
              (core_stricmp(machine.system().parent, "dynwar") == 0)
           )
   {
      /* Capcom CPS-1: Dynasty Wars */

      Buttons_mapping[0]=RETROPAD_Y;
      Buttons_mapping[1]=RETROPAD_A;
      Buttons_mapping[2]=RETROPAD_X;
      Buttons_mapping[3]=RETROPAD_B;
      Buttons_mapping[4]=RETROPAD_L;
      Buttons_mapping[5]=RETROPAD_R;
   }
   else if (
              (core_stricmp(machine.system().name, "ddsom") == 0) ||
              (core_stricmp(machine.system().parent, "ddsom") == 0)
           )
   {
      /* Capcom CPS-2: Dungeons & Dragons: Shadow over Mystara (same layout of ddtod) */

      Buttons_mapping[0]=RETROPAD_A;
      Buttons_mapping[1]=RETROPAD_B;
      Buttons_mapping[2]=RETROPAD_Y;
      Buttons_mapping[3]=RETROPAD_X;
      Buttons_mapping[4]=RETROPAD_L;
      Buttons_mapping[5]=RETROPAD_R;
   }
   else if (
              (core_stricmp(machine.system().parent, "aof") == 0) ||
              (core_stricmp(machine.system().parent, "aof2") == 0) ||
              (core_stricmp(machine.system().parent, "aof3") == 0) ||
              (core_stricmp(machine.system().parent, "breakers") == 0) ||
              (core_stricmp(machine.system().parent, "breakrev") == 0) ||
              (core_stricmp(machine.system().parent, "doubledr") == 0) ||
              (core_stricmp(machine.system().parent, "fatfury1") == 0) ||
              (core_stricmp(machine.system().parent, "fatfury2") == 0) ||
              (core_stricmp(machine.system().parent, "fatfury3") == 0) ||
              (core_stricmp(machine.system().parent, "fatfursp") == 0) ||
              (core_stricmp(machine.system().parent, "fightfev") == 0) ||
              (core_stricmp(machine.system().parent, "galaxyfg") == 0) ||
              (core_stricmp(machine.system().parent, "garou") == 0) ||
              (core_stricmp(machine.system().parent, "gowcaizr") == 0) ||
              (core_stricmp(machine.system().parent, "neogeo") == 0) ||
              (core_stricmp(machine.system().parent, "karnovr") == 0) ||
              (core_stricmp(machine.system().parent, "kizuna") == 0) ||
              (core_stricmp(machine.system().parent, "kabukikl") == 0) ||
              (core_stricmp(machine.system().parent, "matrim") == 0) ||
              (core_stricmp(machine.system().parent, "mslug") == 0) ||
              (core_stricmp(machine.system().parent, "mslug2") == 0) ||
              (core_stricmp(machine.system().parent, "mslugx") == 0) ||
              (core_stricmp(machine.system().parent, "mslug3") == 0) ||
              (core_stricmp(machine.system().parent, "mslug4") == 0) ||
              (core_stricmp(machine.system().parent, "mslug5") == 0) ||
              (core_stricmp(machine.system().parent, "kof94") == 0) ||
              (core_stricmp(machine.system().parent, "kof95") == 0) ||
              (core_stricmp(machine.system().parent, "kof96") == 0) ||
              (core_stricmp(machine.system().parent, "kof97") == 0) ||
              (core_stricmp(machine.system().parent, "kof98") == 0) ||
              (core_stricmp(machine.system().parent, "kof99") == 0) ||
              (core_stricmp(machine.system().parent, "kof2000") == 0) ||
              (core_stricmp(machine.system().parent, "kof2001") == 0) ||
              (core_stricmp(machine.system().parent, "kof2002") == 0) ||
              (core_stricmp(machine.system().parent, "kof2003") == 0) ||
              (core_stricmp(machine.system().parent, "lresort") == 0) ||
              (core_stricmp(machine.system().parent, "lastblad") == 0) ||
              (core_stricmp(machine.system().parent, "lastbld2") == 0) ||
              (core_stricmp(machine.system().parent, "ninjamas") == 0) ||
              (core_stricmp(machine.system().parent, "rotd") == 0) ||
              (core_stricmp(machine.system().parent, "rbff1") == 0) ||
              (core_stricmp(machine.system().parent, "rbff2") == 0) ||
              (core_stricmp(machine.system().parent, "rbffspec") == 0) ||
              (core_stricmp(machine.system().parent, "savagere") == 0) ||
              (core_stricmp(machine.system().parent, "sengoku3") == 0) ||
              (core_stricmp(machine.system().parent, "samsho") == 0) ||
              (core_stricmp(machine.system().parent, "samsho2") == 0) ||
              (core_stricmp(machine.system().parent, "samsho3") == 0) ||
              (core_stricmp(machine.system().parent, "samsho4") == 0) ||
              (core_stricmp(machine.system().parent, "samsho5") == 0) ||
              (core_stricmp(machine.system().parent, "samsh5sp") == 0) ||
              (core_stricmp(machine.system().parent, "svc") == 0) ||
              (core_stricmp(machine.system().parent, "viewpoin") == 0) ||
              (core_stricmp(machine.system().parent, "wakuwak7") == 0) ||
              (core_stricmp(machine.system().parent, "wh1") == 0) ||
              (core_stricmp(machine.system().parent, "wh2") == 0) ||
              (core_stricmp(machine.system().parent, "wh2j") == 0) ||
              (core_stricmp(machine.system().parent, "whp") == 0)
           )
   {
      /* Neo Geo */

      Buttons_mapping[0]=RETROPAD_B;
      Buttons_mapping[1]=RETROPAD_A;
      Buttons_mapping[2]=RETROPAD_Y;
      Buttons_mapping[3]=RETROPAD_X;
      Buttons_mapping[4]=RETROPAD_L;
      Buttons_mapping[5]=RETROPAD_R;
   }
   else if (
              (core_stricmp(machine.system().name, "kinst") == 0) ||
              (core_stricmp(machine.system().parent, "kinst") == 0)
           )
   {
      /* Killer Instinct 1 */

      Buttons_mapping[0]=RETROPAD_L;
      Buttons_mapping[1]=RETROPAD_Y;
      Buttons_mapping[2]=RETROPAD_X;
      Buttons_mapping[3]=RETROPAD_R;
      Buttons_mapping[4]=RETROPAD_B;
      Buttons_mapping[5]=RETROPAD_A;

   }
   else if (
              (core_stricmp(machine.system().name, "kinst2") == 0) ||
              (core_stricmp(machine.system().parent, "kinst2") == 0)
           )
   {
      /* Killer Instinct 2 */

      Buttons_mapping[0]=RETROPAD_L;
      Buttons_mapping[1]=RETROPAD_Y;
      Buttons_mapping[2]=RETROPAD_X;
      Buttons_mapping[3]=RETROPAD_B;
      Buttons_mapping[4]=RETROPAD_A;
      Buttons_mapping[5]=RETROPAD_R;

   }
   else if (
              (core_stricmp(machine.system().name, "tektagt") == 0) ||
              (core_stricmp(machine.system().parent, "tektagt") == 0) ||
              (core_stricmp(machine.system().name, "tekken3") == 0) ||
              (core_stricmp(machine.system().parent, "tekken3") == 0)
           )
   {
      /* Tekken 3/Tekken Tag Tournament */

      Buttons_mapping[0]=RETROPAD_Y;
      Buttons_mapping[1]=RETROPAD_X;
      Buttons_mapping[2]=RETROPAD_R;
      Buttons_mapping[3]=RETROPAD_B;
      Buttons_mapping[4]=RETROPAD_A;
      Buttons_mapping[5]=RETROPAD_L;

   }
   else if (
              (core_stricmp(machine.system().name, "mk") == 0) ||
              (core_stricmp(machine.system().parent, "mk") == 0) ||
              (core_stricmp(machine.system().name, "mk2") == 0) ||
              (core_stricmp(machine.system().parent, "mk2") == 0) ||
              (core_stricmp(machine.system().name, "mk3") == 0) ||
              (core_stricmp(machine.system().parent, "mk3") == 0) ||
              (core_stricmp(machine.system().name, "umk3") == 0) ||
              (core_stricmp(machine.system().parent, "umk3") == 0) ||
              (core_stricmp(machine.system().name, "wwfmania") == 0) ||
              (core_stricmp(machine.system().parent, "wwfmania") == 0)
           )
   {
      /* Mortal Kombat 1/2/3/Ultimate/WWF: Wrestlemania */

      Buttons_mapping[0]=RETROPAD_Y;
      Buttons_mapping[1]=RETROPAD_L;
      Buttons_mapping[2]=RETROPAD_X;
      Buttons_mapping[3]=RETROPAD_B;
      Buttons_mapping[4]=RETROPAD_A;
      Buttons_mapping[5]=RETROPAD_R;

   }
   else if (
              (core_stricmp(machine.system().name, "shangon") == 0) ||
              (core_stricmp(machine.system().parent, "shangon") == 0)
           )
   {
      /* Super Hang-On */

      Buttons_mapping[0]=RETROPAD_A;
      Buttons_mapping[1]=RETROPAD_B;
      Buttons_mapping[2]=RETROPAD_Y;
      Buttons_mapping[3]=RETROPAD_X;
      Buttons_mapping[4]=RETROPAD_L;
      Buttons_mapping[5]=RETROPAD_R;
   }
   else if (
              (core_stricmp(machine.system().name, "chasehq") == 0) ||
              (core_stricmp(machine.system().parent, "chasehq") == 0) ||
              (core_stricmp(machine.system().name, "superchs") == 0) ||
              (core_stricmp(machine.system().parent, "superchs") == 0)
           )
   {
      /* Chase H.Q. / Super Chase - Criminal Termination */

      Buttons_mapping[0]=RETROPAD_A;
      Buttons_mapping[1]=RETROPAD_B;
      Buttons_mapping[2]=RETROPAD_X;
      Buttons_mapping[3]=RETROPAD_R;
      Buttons_mapping[4]=RETROPAD_L;
      Buttons_mapping[5]=RETROPAD_Y;
   }
   else if (
              (core_stricmp(machine.system().name, "outrun") == 0) ||
              (core_stricmp(machine.system().parent, "outrun") == 0) ||
              (core_stricmp(machine.system().name, "turbo") == 0) ||
              (core_stricmp(machine.system().parent, "turbo") == 0)
           )
   {
      /* Out Run / Turbo */

      Buttons_mapping[0]=RETROPAD_A;
      Buttons_mapping[1]=RETROPAD_B;
      Buttons_mapping[2]=RETROPAD_R;
      Buttons_mapping[3]=RETROPAD_Y;
      Buttons_mapping[4]=RETROPAD_L;
      Buttons_mapping[5]=RETROPAD_X;
   }
   else if (
              (core_stricmp(machine.system().name, "vr") == 0) ||
              (core_stricmp(machine.system().parent, "vr") == 0)
           )
   {
      /* Virtua Racing */

      Buttons_mapping[0]=RETROPAD_A;
      Buttons_mapping[1]=RETROPAD_B;
      Buttons_mapping[2]=RETROPAD_X;
      Buttons_mapping[3]=RETROPAD_Y;
      Buttons_mapping[4]=RETROPAD_PAD_DOWN;
      Buttons_mapping[5]=RETROPAD_PAD_UP;
      Buttons_mapping[6]=RETROPAD_L;
      Buttons_mapping[7]=RETROPAD_R;
   }
   else if (
              (core_stricmp(machine.system().name, "ddragon") == 0) ||
              (core_stricmp(machine.system().parent, "ddragon") == 0) ||
              (core_stricmp(machine.system().name, "ddragon2") == 0) ||
              (core_stricmp(machine.system().parent, "ddragon2") == 0)
           )
   {
      /* Double Dragon 1/2 */

      Buttons_mapping[0]=RETROPAD_Y;
      Buttons_mapping[1]=RETROPAD_B;
      Buttons_mapping[2]=RETROPAD_A;
      Buttons_mapping[3]=RETROPAD_X;
      Buttons_mapping[4]=RETROPAD_L;
      Buttons_mapping[5]=RETROPAD_R;
   }
   else if (
              (core_stricmp(machine.system().name, "altbeast") == 0) ||
              (core_stricmp(machine.system().parent, "altbeast") == 0)
           )
   {
      /* Altered Beast */

      Buttons_mapping[0]=RETROPAD_A;
      Buttons_mapping[1]=RETROPAD_Y;
      Buttons_mapping[2]=RETROPAD_B;
      Buttons_mapping[3]=RETROPAD_X;
      Buttons_mapping[4]=RETROPAD_L;
      Buttons_mapping[5]=RETROPAD_R;
   }

}

//============================================================
//  main
//============================================================
static int parsePath(char* path, char* gamePath, char* gameName)
{
   int i;
   int slashIndex = -1;
   int dotIndex   = -1;
   int len        = strlen(path);

   if (len < 1)
      return 0;

   for (i = len - 1; i >= 0; i--)
   {
      if (path[i] == slash)
      {
         slashIndex = i;
         break;
      }
      else
         if (path[i] == '.')
            dotIndex = i;
   }
	
   if (slashIndex < 0 && dotIndex >0){
	strcpy(gamePath, ".\0");
   	strncpy(gameName, path , dotIndex );
   	gameName[dotIndex] = 0;
   	return 1;
   }
	
   if (slashIndex < 0 || dotIndex < 0)
      return 0;

   strncpy(gamePath, path, slashIndex);
   gamePath[slashIndex] = 0;
   strncpy(gameName, path + (slashIndex + 1), dotIndex - (slashIndex + 1));
   gameName[dotIndex - (slashIndex + 1)] = 0;

   return 1;
}

static int parseSystemName(char* path, char* systemName)
{
   int i, j = 0;
   int slashIndex[2]={-1,-1};
   int len = strlen(path);

   if (len < 1)
      return 0;

   for (i = len - 1; i >=0; i--)
   {
      if (j<2)
      {
         if (path[i] == slash)
         {
            slashIndex[j] = i;
            j++;
         }
      }
      else
         break;
   }

   if (slashIndex[0] < 0 || slashIndex[1] < 0 )
      return 0;

   strncpy(systemName, path + (slashIndex[1] +1), slashIndex[0]-slashIndex[1]-1);
   return 1;
}

static int parseParentPath(char* path, char* parentPath)
{
   int i, j = 0;
   int slashIndex[2] = {-1,-1};
   int len = strlen(path);

   if (len < 1)
      return 0;

   for (i = len - 1; i >= 0; i--)
   {
      if (j<2)
      {
         if (path[i] == slash)
         {
            slashIndex[j] = i;
            j++;
         }
      }
      else
         break;
   }

   if (slashIndex[0] < 0 || slashIndex[1] < 0 )
      return 0;

   strncpy(parentPath, path, slashIndex[1]);
   return 1;
}

static int getGameInfo(char* gameName, int* rotation, int* driverIndex,bool *Arcade)
{
   int gameFound = 0;
   int num = driver_list::find(gameName);

   if (log_cb)
      log_cb(RETRO_LOG_DEBUG, "Searching for driver %s\n",gameName);

   if (num != -1)
   {
      if (driver_list::driver(num).flags & GAME_TYPE_ARCADE)
      {
         *Arcade=TRUE;
         if (log_cb)
            log_cb(RETRO_LOG_DEBUG, "System type: ARCADE\n");
      }
      else if(driver_list::driver(num).flags& GAME_TYPE_CONSOLE)
      {
         if (log_cb)
            log_cb(RETRO_LOG_DEBUG, "System type: CONSOLE\n");
      }
      else if(driver_list::driver(num).flags& GAME_TYPE_COMPUTER)
      {
         if (log_cb)
            log_cb(RETRO_LOG_DEBUG, "System type: COMPUTER\n");
      }
      gameFound = 1;

      if (log_cb)
         log_cb(RETRO_LOG_INFO, "Game name: %s, Game description: %s\n",
               driver_list::driver(num).name,
               driver_list::driver(num).description);
   }
   else
   {
      if (log_cb)
         log_cb(RETRO_LOG_WARN, "Driver %s not found %i\n",gameName,num);
   }

   return gameFound;
}


void Extract_AllPath(char *srcpath)
{
   int result_value =0;

   /* Split the path to directory
    * and the name without the zip extension. */
   int result = parsePath(srcpath, MgamePath, MgameName);

   if (result == 0)
   {
      strcpy(MgameName,srcpath);
      result_value|=1;
      if (log_cb)
         log_cb(RETRO_LOG_ERROR, "Error parsing game path: %s\n",srcpath);
   }

   /* Split the path to directory and
    * the name without the zip extension. */
   result = parseSystemName(srcpath, MsystemName);
   if (result == 0)
   {
      strcpy(MsystemName,srcpath );
      result_value|=2;
      if (log_cb)
         log_cb(RETRO_LOG_ERROR, "Error parsing system name: %s\n",srcpath);
   }

   /* Get the parent path. */
   result = parseParentPath(srcpath, MparentPath);

   if (result == 0)
   {
      strcpy(MparentPath,srcpath );
      result_value|=4;
      if (log_cb)
         log_cb(RETRO_LOG_ERROR, "Error parsing parent path: %s\n",srcpath);
   }

   if (log_cb)
   {
      log_cb(RETRO_LOG_DEBUG, "Path extraction result: File name=%s\n",srcpath);
      log_cb(RETRO_LOG_DEBUG, "Path extraction result: Game name=%s\n",MgameName);
      log_cb(RETRO_LOG_DEBUG, "Path extraction result: System name=%s\n",MsystemName);
      log_cb(RETRO_LOG_DEBUG, "Path extraction result: Game path=%s\n",MgamePath);
      log_cb(RETRO_LOG_DEBUG, "Path extraction result: Parent path=%s\n",MparentPath);
   }

}

static void Add_Option(const char* option)
{
   static int first = 0;

   if (first == 0)
   {
      PARAMCOUNT=0;
      first++;
   }

   sprintf(XARGV[PARAMCOUNT++], "%s", option);
}

static void Set_Default_Option(void)
{
   /* some hardcoded default options. */

   Add_Option(core);

   if(throttle_enable)
      Add_Option("-throttle");
   else
      Add_Option("-nothrottle");

   Add_Option("-joystick");
   Add_Option("-samplerate");
   Add_Option("48000");

   if(cheats_enable)
      Add_Option("-cheat");
   else
      Add_Option("-nocheat");

   if(mouse_enable)
      Add_Option("-mouse");
   else
      Add_Option("-nomouse");

   if(hide_gameinfo)
      Add_Option("-skip_gameinfo");

   if(hide_nagscreen)
      Add_Option("-skip_nagscreen");

   if(hide_warnings)
      Add_Option("-skip_warnings");

   if(write_config_enable)
      Add_Option("-writeconfig");

   if(read_config_enable)
      Add_Option("-readconfig");
   else
      Add_Option("-noreadconfig");

   if(auto_save_enable)
      Add_Option("-autosave");

   if(game_specific_saves_enable)
   {
      char option[50];
      Add_Option("-statename");
      sprintf(option,"%%g/%s",MgameName);
      Add_Option(option);
   }
}

static void Set_Path_Option(void)
{
   int i;
   char tmp_dir[256];

   /*Setup path option according to retro (save/system) directory,
    * or current if NULL. */

   for(i = 0; i < NB_OPTPATH; i++)
   {
      Add_Option((char*)(opt_name[i]));

      if(opt_type[i] == 0)
      {
         if (retro_save_directory)
            sprintf(tmp_dir, "%s%c%s%c%s", retro_save_directory, slash, core, slash,dir_name[i]);
         else
            sprintf(tmp_dir, "%s%c%s%c%s%c", ".", slash, core, slash,dir_name[i],slash);
      }
      else
      {
         if(retro_system_directory)
            sprintf(tmp_dir, "%s%c%s%c%s", retro_system_directory, slash, core, slash,dir_name[i]);
         else
            sprintf(tmp_dir, "%s%c%s%c%s%c", ".", slash, core, slash,dir_name[i],slash);
      }

      Add_Option((char*)(tmp_dir));
   }

}

//============================================================
//  main
//============================================================

static int execute_game(char* path)
{
   unsigned i;
   char tmp_dir[256];
   int gameRot=0;
   int driverIndex;

   FirstTimeUpdate = 1;

   screenRot = 0;

   for (i = 0; i < 64; i++)
      xargv_cmd[i]=NULL;

   Extract_AllPath(path);

#ifdef WANT_MAME
   //find if the driver exists for MgameName, if not, exit
   if (getGameInfo(MgameName, &gameRot, &driverIndex,&arcade) == 0)
   {
      if (log_cb)
         log_cb(RETRO_LOG_ERROR, "Driver not found: %s\n",MgameName);
      return -2;
   }
#else
   /* Find if the driver exists for MgameName.
    * If not, check if a driver exists for MsystemName.
    * Otherwise, exit. */
   if (getGameInfo(MgameName, &gameRot, &driverIndex,&arcade) == 0)
   {
      if (log_cb)
         log_cb(RETRO_LOG_ERROR, "Driver not found %s\n",MgameName);
      if (getGameInfo(MsystemName, &gameRot, &driverIndex,&arcade) == 0)
      {
         if (log_cb)
            log_cb(RETRO_LOG_ERROR, "System not found: %s\n",MsystemName);
         return -2;
      }
   }

   /* Handle case where Arcade game exists and game on a System also. */
   if(arcade == true)
   {
      if (log_cb)
         log_cb(RETRO_LOG_ERROR, "System not found: %s\n",MsystemName);

      // test system
      if (getGameInfo(MsystemName, &gameRot, &driverIndex,&arcade) != 0)
         arcade=false;
   }

#endif

   /* useless ? */
   if (tate)
   {
      /* horizontal game */
      if (gameRot == ROT0)
         screenRot = 1;
      else if (gameRot &  ORIENTATION_FLIP_X)
         screenRot = 3;
   }
   else
   {
      if (gameRot != ROT0)
      {
         screenRot = 1;
         if (gameRot &  ORIENTATION_FLIP_X)
            screenRot = 2;
      }
   }

   if (log_cb)
   {
      log_cb(RETRO_LOG_INFO, "Creating frontend for game: %s\n",MgameName);
      log_cb(RETRO_LOG_INFO, "Softlists: %d\n",softlist_enable);
   }

   Set_Default_Option();

   Set_Path_Option();

   /* useless ? */
   if (tate)
   {
      if (screenRot == 3)
         Add_Option((char*) "-rol");
   }
   else
   {
      if (screenRot == 2)
         Add_Option((char*)"-rol");
   }

   Add_Option((char*)("-rompath"));

#ifdef WANT_MAME
   sprintf(tmp_dir, "%s", MgamePath);
   Add_Option((char*)(tmp_dir));
   if(!boot_to_osd_enable)
      Add_Option(MgameName);

#else

   if(!boot_to_osd_enable)
   {
      sprintf(tmp_dir, "%s", MgamePath);
      Add_Option((char*)(tmp_dir));

      if(softlist_enable)
      {
         if(!arcade)
         {
            Add_Option(MsystemName);
            if(!boot_to_bios_enable)
            {
               if(!softlist_auto)
                  Add_Option((char*)mediaType);
               Add_Option((char*)MgameName);
            }
         }
         else
            Add_Option((char*)MgameName);
      }
      else
      {
         if (strcmp(mediaType, "-rom") == 0)
            Add_Option(MgameName);
         else
         {
            Add_Option(MsystemName);
            Add_Option((char*)mediaType);
            Add_Option((char*)gameName);
         }
      }
   }
   else
   {
      sprintf(tmp_dir, "%s;%s", MgamePath,MparentPath);
      Add_Option((char*)(tmp_dir));
   }
#endif

   return 0;
}

static void parse_cmdline(const char *argv)
{
   int c,c2;
   static char buffer[512*4];
   enum states
   {
      DULL,
      IN_WORD,
      IN_STRING
   } state = DULL;
   char *p  = NULL;
   char *p2 = NULL;
   char *start_of_word = NULL;

   strcpy(buffer,argv);
   strcat(buffer," \0");

   for (p = buffer; *p != '\0'; p++)
   {
      c = (unsigned char) *p; /* convert to unsigned char for is* functions */

      switch (state)
      {
         case DULL:
            /* not in a word, not in a double quoted string */

            if (isspace(c)) /* still not in a word, so ignore this char */
               continue;

            /* not a space -- if it's a double quote we go to IN_STRING, else to IN_WORD */
            if (c == '"')
            {
               state = IN_STRING;
               start_of_word = p + 1; /* word starts at *next* char, not this one */
               continue;
            }
            state = IN_WORD;
            start_of_word = p; /* word starts here */
            continue;

         case IN_STRING:
            /* we're in a double quoted string, so keep going until we hit a close " */
            if (c == '"')
            {
               /* word goes from start_of_word to p-1 */
               //... do something with the word ...
               for (c2=0, p2 = start_of_word; p2 < p; p2++, c2++)
                  ARGUV[ARGUC][c2] = (unsigned char)*p2;
               ARGUC++;

               state = DULL; /* back to "not in word, not in string" state */
            }
            continue; /* either still IN_STRING or we handled the end above */

         case IN_WORD:
            /* we're in a word, so keep going until we get to a space */
            if (isspace(c))
            {
               /* word goes from start_of_word to p-1 */
               /*... do something with the word ... */
               for (c2=0,p2 = start_of_word; p2 <p; p2++,c2++)
                  ARGUV[ARGUC][c2] = (unsigned char) *p2;
               ARGUC++;

               state = DULL; /* back to "not in word, not in string" state */
            }
            continue; /* either still IN_WORD or we handled the end above */
      }
   }
}

static int execute_game_cmd(char* path)
{
   unsigned i;
   int driverIndex;
   int gameRot=0;
   bool CreateConf = (strcmp(ARGUV[0],"-cc") == 0 || strcmp(ARGUV[0],"-createconfig") == 0) ? 1 : 0;
   bool Only1Arg   = (ARGUC == 1) ? 1 : 0;
   bool Mamecmdopt = strcmp(ARGUV[0],core) == 0 ? 1: 0;

   FirstTimeUpdate = 1;

   screenRot = 0;

   for (i = 0; i < 64; i++)
      xargv_cmd[i]=NULL;

   /* split the path to directory and the name without the zip extension */
   if (parsePath(Only1Arg?path:ARGUV[ARGUC-1], MgamePath, MgameName) == 0)
   {
      if (log_cb)
      log_cb(RETRO_LOG_ERROR, "parse path failed! path=%s.\n", path);
      strcpy(MgameName,path );
   }

   if(Only1Arg)
   {
      /* split the path to directory and the name without the zip extension */
      if (parseSystemName(path, MsystemName) ==0)
      {
         if (log_cb)
            log_cb(RETRO_LOG_ERROR, "parse systemname failed! path=%s\n", path);
         strcpy(MsystemName,path );
      }
   }

   /* Find the game info. Exit if game driver was not found. */
   if (getGameInfo(Only1Arg?MgameName:ARGUV[0], &gameRot, &driverIndex,&arcade) == 0)
   {
      /* handle -cc/-createconfig case */
      if(CreateConf)
      {
         if (log_cb)
            log_cb(RETRO_LOG_INFO, "Create an %s config\n", core);
      }
      else
      {
         if (log_cb)
            log_cb(RETRO_LOG_WARN, "Game not found: %s\n", MgameName);

         if(Only1Arg)
         {
            //test if system exist (based on parent path)
            if (getGameInfo(MsystemName, &gameRot, &driverIndex,&arcade) == 0)
            {
               if (log_cb)
                  log_cb(RETRO_LOG_ERROR, "Driver not found: %s\n", MsystemName);
               if(!Mamecmdopt)return -2;
            }
         }
         else
            if(!Mamecmdopt)return -2;
      }
   }

   if(Only1Arg)
   {
      /* handle case where Arcade game exist and game on a System also */
      if(arcade==true)
      {
         /* test system */
         if (getGameInfo(MsystemName, &gameRot, &driverIndex,&arcade) == 0)
         {
            if (log_cb)
               log_cb(RETRO_LOG_ERROR, "System not found: %s\n", MsystemName);
         }
         else
         {
            if (log_cb)
               log_cb(RETRO_LOG_INFO, "System found: %s\n", MsystemName);
            arcade=false;
         }
      }
   }

   Set_Default_Option();

   Add_Option("-mouse");

   Set_Path_Option();

   if(Only1Arg)
   {
      /* Assume arcade/mess rom with full path or -cc   */
      if(CreateConf)
         Add_Option((char*)"-createconfig");
      else
      {
         Add_Option((char*)"-rp");
         Add_Option((char*)g_rom_dir);
         if(!arcade)
            Add_Option(MsystemName);
         Add_Option(MgameName);
      }
   }
   else if (Mamecmdopt){
       for(i = 1;i < ARGUC; i++)
         Add_Option(ARGUV[i]);
   }
   else
   {
      /* Pass all cmdline args */
      for(i = 0;i < ARGUC; i++)
         Add_Option(ARGUV[i]);
   }

   return 0;
}

static char CMDFILE[512];

int loadcmdfile(char *argv)
{
    int res=0;

    FILE *fp = fopen(argv,"r");

    if( fp != NULL )
    {
	if ( fgets (CMDFILE , 512 , fp) != NULL )
		res=1;	
	fclose (fp);
    }

    return res;
}


/*
#ifdef __cplusplus
extern "C"
#endif
*/
retro_osd_interface *retro_global_osd;

int mmain(int argc, const char *argv)
{
   unsigned i=0;
   //osd_options options;
   //cli_options MRoptions;
   int result = 0;

   static osd_options retro_global_options;

   strcpy(gameName,argv);

   // handle cmd file 
   if (strlen(gameName) >= strlen("cmd")){
           if(!core_stricmp(&gameName[strlen(gameName)-strlen("cmd")], "cmd"))
                       i=loadcmdfile(gameName);     
   }

   if(i==1)
   {
      parse_cmdline(CMDFILE);
      if (log_cb)
         log_cb(RETRO_LOG_INFO, "Starting game from command line:%s\n",CMDFILE);

      result = execute_game_cmd(ARGUV[ARGUC-1]);      

   }
   else
   if(experimental_cmdline)
   {
      parse_cmdline(argv);
      if (log_cb)
         log_cb(RETRO_LOG_INFO, "Starting game from command line:%s\n",gameName);

      result = execute_game_cmd(ARGUV[ARGUC-1]);
   }
   else
   {
      if (log_cb)
         log_cb(RETRO_LOG_INFO, "Starting game:%s\n",gameName);
      result = execute_game(gameName);
   }

   if (result < 0)
      return result;

   if (log_cb)
      log_cb(RETRO_LOG_DEBUG, "Parameters:\n");

   for (i = 0; i < PARAMCOUNT; i++)
   {
      xargv_cmd[i] = (char*)(XARGV[i]);
      if (log_cb)
         log_cb(RETRO_LOG_DEBUG, " %s\n",XARGV[i]);
   }

/*
   retro_osd_interface osd(options);
   osd.register_options();
   cli_frontend frontend(options, osd);
*/
   retro_global_osd= global_alloc(retro_osd_interface(retro_global_options));

   retro_global_osd->register_options();
   cli_frontend frontend(retro_global_options, *retro_global_osd);

   result = frontend.execute(PARAMCOUNT, ( char **)xargv_cmd);

   xargv_cmd[PARAMCOUNT - 2] = NULL;

   return result;
}

#include "../../emu/drawgfx.h"
#include "osdepend.h"

//FIX ME DO CLEAN EXIT
//============================================================
//  constructor
//============================================================

retro_osd_interface::retro_osd_interface(osd_options &options) : osd_common_t(options)
{
}


//============================================================
//  destructor
//============================================================

retro_osd_interface::~retro_osd_interface()
{
}

void retro_osd_interface::osd_exit()
{
   if (log_cb)
      log_cb(RETRO_LOG_INFO, "OSD exit called\n");

#if defined(HAVE_OPENGL) || defined(HAVE_OPENGLES)
   destroy_all_textures();
   if (retro)
      free(retro);
   retro = NULL:
#endif

   osd_common_t::osd_exit();
}

extern int RLOOP;

void retro_osd_interface::init(running_machine &machine)
{
	int gamRot=0;

#if defined(HAVE_OPENGL) || defined(HAVE_OPENGLES)
	/* allocate memory for our structures */
	retro = (retro_info *)malloc(sizeof(*retro));

   if (!retro)
      return;

	memset(retro, 0, sizeof(*retro));
#endif

	osd_common_t::init(machine);
	our_target = machine.render().target_alloc();

	initInput(machine);

	if (log_cb)
		log_cb(RETRO_LOG_INFO, "Screen orientation: %s\n",(machine.system().flags & ORIENTATION_SWAP_XY) ? "VERTICAL" : "HORIZONTAL");

    orient  = (machine.system().flags & ORIENTATION_MASK);
	vertical = (machine.system().flags & ORIENTATION_SWAP_XY);

    gamRot = (ROT270 == orient) ? 1 : gamRot;
    gamRot = (ROT180 == orient) ? 2 : gamRot;
    gamRot = (ROT90  == orient) ? 3 : gamRot;

    /* initialize the subsystems */
	osd_common_t::init_subsystems();

	our_target->compute_minimum_size(fb_width, fb_height);
	fb_pitch = fb_width;

	int width,height;
	our_target->compute_visible_area(1000,1000,1,ROT0,width,height);

	retro_aspect = (float)width/(float)height;
	retro_fps    = ATTOSECONDS_TO_HZ(machine.first_screen()->refresh_attoseconds());

	if (log_cb)
		log_cb(RETRO_LOG_DEBUG, "Screen width=%d height=%d, aspect=%d/%d=%f\n",
            fb_width, fb_height, width,height, retro_aspect);

	NEWGAME_FROM_OSD=1;

	if (log_cb)
		log_cb(RETRO_LOG_INFO, "OSD initialization complete\n");
}

void retro_osd_interface::update(bool skip_redraw, UINT32 flags)
{
	if (FirstTimeUpdate == 1)
		skip_redraw = 0; //force redraw to make sure the video texture is created

   if (!skip_redraw)
   {
      int minwidth, minheight;

      retro_frame_draw_enable(true);

      /* get the minimum width/height for the current layout */

      if (alternate_renderer==false)
         our_target->compute_minimum_size(minwidth, minheight);
      else
      {
         minwidth  = 1600;
         minheight = 1200;
      }

      if (FirstTimeUpdate == 1)
      {
         int gamRot = 0;

         FirstTimeUpdate++;

#if 0
         if (log_cb)
            log_cb(RETRO_LOG_INFO, "game screen w=%i h=%i  rowPixels=%i\n", minwidth, minheight, minwidth);
#endif

         fb_width  = minwidth;
         fb_height = minheight;
         fb_pitch  = minwidth;

         orient  = (flags & ORIENTATION_MASK);
         vertical = (flags & ORIENTATION_SWAP_XY);

         gamRot = (ROT270 == orient) ? 1 : gamRot;
         gamRot = (ROT180 == orient) ? 2 : gamRot;
         gamRot = (ROT90  == orient) ? 3 : gamRot;
      }

      if (minwidth != fb_width || minheight != fb_height || minwidth != fb_pitch)
      {
         fb_width  = minwidth;
         fb_height = minheight;
         fb_pitch  = minwidth;
      }

      if(alternate_renderer)
      {
         fb_width  = fb_pitch = 1600;
         fb_height = 1200;
      }

      /* make that the size of our target */
      our_target->set_bounds(fb_width, fb_height);

      /* get the list of primitives for the target at the current size */

      render_primitive_list &primlist = our_target->get_primitives();

      /* lock them, and then render them */
      osd_lock_acquire(primlist.m_lock);

#if defined(HAVE_OPENGL) || defined(HAVE_OPENGLES)
      gl_draw_primitives(primlist, fb_width, fb_height);
#else
      UINT8 *surfptr = (UINT8 *)retro_get_fb_ptr();
#ifdef M16B
      software_renderer<UINT16, 3,2,3, 11,5,0>::draw_primitives(primlist, surfptr, minwidth, minheight,minwidth );
#else
      software_renderer<UINT32, 0,0,0, 16,8,0>::draw_primitives(primlist, surfptr, minwidth, minheight,minwidth );
#endif

#endif

      osd_lock_release(primlist.m_lock);
   }
	else
      retro_frame_draw_enable(false);


   if(ui_ipt_pushchar!=-1)
   {
		ui_input_push_char_event(machine(), our_target, (unicode_char)ui_ipt_pushchar);
		ui_ipt_pushchar=-1;
   }

RLOOP=0;

}

//============================================================
//  customize_input_type_list
//============================================================
void retro_osd_interface::customize_input_type_list(simple_list<input_type_entry> &typelist)
{
	// This function is called on startup, before reading the
	// configuration from disk. Scan the list, and change the
	// default control mappings you want. It is quite possible
	// you won't need to change a thing.
}
