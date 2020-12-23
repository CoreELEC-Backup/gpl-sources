/***************************************************************************

    mame.c

    Controls execution of the core MAME system.

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

****************************************************************************

    Since there has been confusion in the past over the order of
    initialization and other such things, here it is, all spelled out
    as of January, 2008:

    main()
        - does platform-specific init
        - calls mame_execute() [mame.c]

        mame_execute() [mame.c]
            - calls mame_validitychecks() [validity.c] to perform validity checks on all compiled drivers
            - begins resource tracking (level 1)
            - calls create_machine [mame.c] to initialize the running_machine structure
            - calls init_machine() [mame.c]

            init_machine() [mame.c]
                - calls fileio_init() [fileio.c] to initialize file I/O info
                - calls config_init() [config.c] to initialize configuration system
                - calls input_init() [input.c] to initialize the input system
                - calls output_init() [output.c] to initialize the output system
                - calls state_init() [state.c] to initialize save state system
                - calls state_save_allow_registration() [state.c] to allow registrations
                - calls palette_init() [palette.c] to initialize palette system
                - calls render_init() [render.c] to initialize the rendering system
                - calls ui_init() [ui.c] to initialize the user interface
                - calls generic_machine_init() [machine/generic.c] to initialize generic machine structures
                - calls generic_video_init() [video/generic.c] to initialize generic video structures
                - calls generic_sound_init() [audio/generic.c] to initialize generic sound structures
                - calls timer_init() [timer.c] to reset the timer system
                - calls osd_init() [osdepend.h] to do platform-specific initialization
                - calls input_port_init() [inptport.c] to set up the input ports
                - calls rom_init() [romload.c] to load the game's ROMs
                - calls memory_init() [memory.c] to process the game's memory maps
                - calls watchdog_init() [watchdog.c] to initialize the watchdog system
                - calls the driver's DRIVER_INIT callback
                - calls device_list_start() [devintrf.c] to start any devices
                - calls video_init() [video.c] to start the video system
                - calls tilemap_init() [tilemap.c] to start the tilemap system
                - calls crosshair_init() [crsshair.c] to configure the crosshairs
                - calls sound_init() [sound.c] to start the audio system
                - calls debugger_init() [debugger.c] to set up the debugger
                - calls the driver's MACHINE_START, SOUND_START, and VIDEO_START callbacks
                - calls cheat_init() [cheat.c] to initialize the cheat system
                - calls image_init() [image.c] to initialize the image system

            - calls config_load_settings() [config.c] to load the configuration file
            - calls nvram_load [machine/generic.c] to load NVRAM
            - calls ui_display_startup_screens() [ui.c] to display the startup screens
            - begins resource tracking (level 2)
            - calls soft_reset() [mame.c] to reset all systems

                -------------------( at this point, we're up and running )----------------------

            - calls scheduler->timeslice() [schedule.c] over and over until we exit
            - ends resource tracking (level 2), freeing all auto_mallocs and timers
            - calls the nvram_save() [machine/generic.c] to save NVRAM
            - calls config_save_settings() [config.c] to save the game's configuration
            - calls all registered exit routines [mame.c]
            - ends resource tracking (level 1), freeing all auto_mallocs and timers

        - exits the program

***************************************************************************/

#include "emu.h"
#include "emuopts.h"
#include "osdepend.h"
#include "config.h"
#include "debugger.h"
#include "render.h"
#include "cheat.h"
//MKCHAMP - ADDING HEADER FILE HISCORE FOR INCLUSION
#include "hiscore.h"
#include "ui/ui.h"
#include "uiinput.h"
#include "crsshair.h"
#include "validity.h"
#include "debug/debugcon.h"
#include <time.h>

//**************************************************************************
//  MACHINE MANAGER
//**************************************************************************

machine_manager* machine_manager::m_manager = NULL;

osd_interface &machine_manager::osd() const
{
	return m_osd;
}


machine_manager* machine_manager::instance(emu_options &options,osd_interface &osd)
{
	if(!m_manager)
	{
		m_manager = global_alloc(machine_manager(options,osd));
	}
	return m_manager;
}

machine_manager* machine_manager::instance()
{
	return m_manager;
}

//-------------------------------------------------
//  machine_manager - constructor
//-------------------------------------------------

machine_manager::machine_manager(emu_options &options,osd_interface &osd)
		: m_osd(osd),
		m_options(options),
		m_new_driver_pending(NULL),
		m_machine(NULL)
{
}


//-------------------------------------------------
//  ~machine_manager - destructor
//-------------------------------------------------

machine_manager::~machine_manager()
{
	m_manager = NULL;
}


/***************************************************************************
    GLOBAL VARIABLES
***************************************************************************/

//-------------------------------------------------
//  mame_schedule_new_driver - schedule a new game to
//  be loaded
//-------------------------------------------------

void machine_manager::schedule_new_driver(const game_driver &driver)
{
	m_new_driver_pending = &driver;
}


/***************************************************************************
    CORE IMPLEMENTATION
***************************************************************************/
void machine_manager::update_machine()
{
}

/*-------------------------------------------------
    execute - run the core emulation
-------------------------------------------------*/
#ifdef __LIBRETRO__

extern machine_manager *retro_manager;
static running_machine *retro_global_machine;
static const machine_config *retro_global_config;

int ENDEXEC=0;

static bool firstgame = true;
static bool firstrun = true;
bool started_empty = false;

#endif

int machine_manager::execute()
{
#ifndef __LIBRETRO__
	bool started_empty = false;

	bool firstgame = true;
	bool firstrun = true;
#endif
	// loop across multiple hard resets
	bool exit_pending = false;
	int error = MAMERR_NONE;

	while (error == MAMERR_NONE && !exit_pending)
	{
		m_new_driver_pending = NULL;

		// if no driver, use the internal empty driver
		const game_driver *system = m_options.system();
		if (system == NULL)
		{
			system = &GAME_NAME(___empty);
			if (firstgame)
				started_empty = true;
		}

		firstgame = false;

		// parse any INI files as the first thing
		if (m_options.read_config())
		{
			m_options.revert(OPTION_PRIORITY_INI);
			astring errors;
			m_options.parse_standard_inis(errors);
		}

		// otherwise, perform validity checks before anything else
		if (system != NULL)
		{
			validity_checker valid(m_options);
			valid.check_shared_source(*system);
		}
#ifdef __LIBRETRO__

		retro_global_config= global_alloc(machine_config(*system, m_options));

	        retro_global_machine=global_alloc(running_machine(*retro_global_config, *this));

		set_machine(&(*retro_global_machine));

		error = retro_global_machine->run(firstrun);
		firstrun = false;

		goto retro_handle;

#else
		// create the machine configuration
		machine_config config(*system, m_options);

		// create the machine structure and driver
		running_machine machine(config, *this);

		set_machine(&machine);

		// run the machine
		error = machine.run(firstrun);
		firstrun = false;

		// check the state of the machine
		if (m_new_driver_pending)
		{
			astring old_system_name(m_options.system_name());
			bool new_system = (old_system_name != m_new_driver_pending->name);
			// first: if we scheduled a new system, remove device options of the old system
			// notice that, if we relaunch the same system, there is no effect on the emulation
			if (new_system)
				m_options.remove_device_options();
			// second: set up new system name (and the related device options)
			m_options.set_system_name(m_new_driver_pending->name);
			// third: if we scheduled a new system, take also care of ramsize options
			if (new_system)
			{
				astring error_string;
				m_options.set_value(OPTION_RAMSIZE, "", OPTION_PRIORITY_CMDLINE, error_string);
			}
			firstrun = true;
		}
		else
		{
			if (machine.exit_pending()) m_options.set_system_name("");
		}

		if (machine.exit_pending() && (!started_empty || (system == &GAME_NAME(___empty))))
			exit_pending = true;

		// machine will go away when we exit scope
		set_machine(NULL);
#endif
	}
	// return an error
	return error;
#ifdef __LIBRETRO__
retro_handle:
	return 1;
#endif
}

#ifdef __LIBRETRO__
extern int RLOOP,retro_pause;
extern void retro_loop(running_machine *machine);
extern void retro_execute();
extern core_options *retro_global_options;

void machine_manager::mmchange(){

		// check the state of the machine
		if (m_new_driver_pending)
		{
			astring old_system_name(m_options.system_name());
			bool new_system = (old_system_name != m_new_driver_pending->name);
			// first: if we scheduled a new system, remove device options of the old system
			// notice that, if we relaunch the same system, there is no effect on the emulation
			if (new_system)
				m_options.remove_device_options();
			// second: set up new system name (and the related device options)
			m_options.set_system_name(m_new_driver_pending->name);
			// third: if we scheduled a new system, take also care of ramsize options
			if (new_system)
			{
				astring error_string;
				m_options.set_value(OPTION_RAMSIZE, "", OPTION_PRIORITY_CMDLINE, error_string);
			}
			firstrun = true;
		}
		else
		{
			if (retro_global_machine->exit_pending()) m_options.set_system_name("");
		}

		//FIXME RETRO
		//if (retro_global_machine->exit_pending() && (!started_empty || (system == &GAME_NAME(___empty))))
			//exit_pending = true;
		

}

void free_machineconfig(){

		global_free(retro_global_machine);
		global_free(retro_global_config);

		retro_manager->set_machine(NULL);
}

extern void free_man();


void retro_finish(){
	printf("retro_finish begin\n");
	retro_global_machine->retro_machineexit();
	free_machineconfig();
	free_man();
	printf("retro_finish end\n");
}

/* Called by libretro to save the state on a buffer */
void retro_save_state(retro_buffer_writer &buf)
{
	retro_global_machine->save().retro_write_file(buf);
}

/* Called by libretro to load a state from a buffer */
bool retro_load_state(retro_buffer_reader &buf)
{
	
	save_error r = retro_global_machine->save().retro_read_file(buf);
	if (r == STATERR_NONE ) return true;
	printf("retro_load_state error: %d \n", r);
	return false;
}

void retro_main_loop()
{
	retro_global_machine->retro_loop();

	if(ENDEXEC==1){

		ENDEXEC=0;

		retro_manager->mmchange();

		if(firstrun == true){
			//restart a new driver from UI
			retro_execute();
			return;
		}
		else{ 
			RLOOP=0;
			
			global_free(retro_global_machine);
			global_free(retro_global_config);
			retro_manager->set_machine(NULL);

			printf("exit scope, restart empty driver\n");
			//FIXME restart empty driver else it crash
			// we quit using retroarch (ESC or Menu)
			retro_execute();

		}

	}

}


#endif

/***************************************************************************
    MISCELLANEOUS
***************************************************************************/

/*-------------------------------------------------
    popmessage - pop up a user-visible message
-------------------------------------------------*/

void CLIB_DECL popmessage(const char *format, ...)
{
	if (machine_manager::instance()==NULL || machine_manager::instance()->machine() == NULL) return;

	// if the format is NULL, it is a signal to clear the popmessage
	if (format == NULL)
		machine_manager::instance()->machine()->ui().popup_time(0, " ");

	// otherwise, generate the buffer and call the UI to display the message
	else
	{
		astring temp;
		va_list arg;

		// dump to the buffer
		va_start(arg, format);
		temp.vprintf(format, arg);
		va_end(arg);

		// pop it in the UI
		machine_manager::instance()->machine()->ui().popup_time(temp.len() / 40 + 2, "%s", temp.cstr());
		
		/*
		// also write to error.log
		logerror("popmessage: %s\n", temp.cstr());
		
#ifdef MAME_DEBUG
		// and to command-line in a DEBUG build
		osd_printf_info("popmessage: %s\n", temp.cstr());
#endif
		*/
	}
}


/*-------------------------------------------------
    logerror - log to the debugger and any other
    OSD-defined output streams
-------------------------------------------------*/

void CLIB_DECL logerror(const char *format, ...)
{
	va_list arg;
	va_start(arg, format);
	vlogerror(format, arg);
	va_end(arg);
}


/*-------------------------------------------------
    vlogerror - log to the debugger and any other
    OSD-defined output streams
-------------------------------------------------*/

void CLIB_DECL vlogerror(const char *format, va_list arg)
{
	if (machine_manager::instance()!=NULL && machine_manager::instance()->machine() != NULL)
		machine_manager::instance()->machine()->vlogerror(format, arg);
}
