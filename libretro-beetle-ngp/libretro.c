#include "mednafen/mednafen.h"
#include "mednafen/mempatcher.h"
#include "mednafen/git.h"
#include "mednafen/general.h"
#include <libretro.h>
#include <streams/file_stream.h>

#include "libretro_core_options.h"
#include "mednafen/state.h"
#include "mednafen/state_helpers.h"

#ifdef _MSC_VER
#include <compat/msvc.h>
#endif

#include <string.h>

/* core options */
static int RETRO_SAMPLE_RATE = 44100;

static int RETRO_PIX_BYTES = 2;
static int RETRO_PIX_DEPTH = 15;

/* ==================================================== */

struct retro_perf_callback perf_cb;
retro_get_cpu_features_t perf_get_cpu_features_cb = NULL;
retro_log_printf_t log_cb;
static retro_video_refresh_t video_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

static bool overscan;

static MDFN_Surface *surf;

static bool failed_init;

static bool libretro_supports_bitmasks = false;

static void hookup_ports(bool force);

static bool initial_ports_hookup = false;

static char retro_base_directory[1024];
static char retro_base_name[1024];
static char retro_save_directory[1024];

/*---------------------------------------------------------------------------
 * NEOPOP : Emulator as in Dreamland
 *
 * Copyright (c) 2001-2002 by neopop_uk
 *---------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 2 of the License, or
 *	(at your option) any later version. See also the license.txt file for
 *	additional informations.
 *---------------------------------------------------------------------------
 */

#include "mednafen/ngp/neopop.h"
#include "mednafen/general.h"

#include "mednafen/ngp/TLCS-900h/TLCS900h_interpret.h"
#include "mednafen/ngp/TLCS-900h/TLCS900h_registers.h"
#include "mednafen/ngp/Z80_interface.h"
#include "mednafen/ngp/interrupt.h"
#include "mednafen/ngp/mem.h"
#include "mednafen/ngp/rom.h"
#include "mednafen/ngp/gfx.h"
#include "mednafen/ngp/sound.h"
#include "mednafen/ngp/dma.h"
#include "mednafen/ngp/bios.h"
#include "mednafen/ngp/flash.h"
#include "mednafen/ngp/system.h"

extern uint8 CPUExRAM[16384];

ngpgfx_t *NGPGfx;

COLOURMODE system_colour = COLOURMODE_AUTO;

static uint8 *chee;

static int32 z80_runtime;

extern bool NGPFrameSkip;
extern int32_t ngpc_soundTS;

static void Emulate(EmulateSpecStruct *espec, int16_t *sound_buf)
{
   bool MeowMeow        = false;

   espec->DisplayRect.x = 0;
   espec->DisplayRect.y = 0;
   espec->DisplayRect.w = 160;
   espec->DisplayRect.h = 152;

   if(espec->VideoFormatChanged)
      ngpgfx_set_pixel_format(NGPGfx, espec->surface->depth);

   if(espec->SoundFormatChanged)
      MDFNNGPC_SetSoundRate(espec->SoundRate);

   storeB(0x6F82, *chee);

   MDFNMP_ApplyPeriodicCheats();

   ngpc_soundTS         = 0;
   NGPFrameSkip         = espec->skip;

   do
   {
      int32 timetime = (uint8)TLCS900h_interpret();
      MeowMeow |= updateTimers(espec->surface, timetime);
      z80_runtime += timetime;

      while(z80_runtime > 0)
      {
         int z80rantime = Z80_RunOP();

         if (z80rantime < 0) /* Z80 inactive, so take up all run time! */
         {
            z80_runtime = 0;
            break;
         }

         z80_runtime -= z80rantime << 1;

      }
   }while(!MeowMeow);


   espec->MasterCycles = ngpc_soundTS;
   espec->SoundBufSize = MDFNNGPCSOUND_Flush(sound_buf,
         espec->SoundBufMaxSize);
}

static void neopop_reset_internal(void)
{
   ngpgfx_power(NGPGfx);
   Z80_reset();
   reset_int();
   reset_timers();

   reset_memory();
   BIOSHLE_Reset();
   reset_registers();	/* TLCS900H registers */
   reset_dma();
}

void neopop_reset(void)
{
   neopop_reset_internal();
}

static int Load(const char *name, struct MDFNFILE *fp,
      const uint8_t *data, size_t size)
{
   if ((data != NULL) && (size != 0))
   {
      if (!(ngpc_rom.data = (uint8 *)malloc(size)))
         return(0);
      ngpc_rom.length = size;
      memcpy(ngpc_rom.data, data, size);
   }
   else
   {
      if(!(ngpc_rom.data = (uint8 *)malloc(fp->size)))
         return(0);

      ngpc_rom.length = fp->size;
      memcpy(ngpc_rom.data, fp->data, fp->size);
   }

   rom_loaded();

   MDFNMP_Init(1024, 1024 * 1024 * 16 / 1024);

   NGPGfx = (ngpgfx_t*)calloc(1, sizeof(*NGPGfx));
   NGPGfx->layer_enable = 1 | 2 | 4;

   EmulatedNGP.fps = (uint32)((uint64)6144000 * 65536 * 256 / 515 / 198); /* 3072000 * 2 * 10000 / 515 / 198 */

   MDFNNGPCSOUND_Init();

   MDFNMP_AddRAM(16384, 0x4000, CPUExRAM);

   SetFRM(); /* Set up fast read memory mapping */

   bios_install();

   z80_runtime = 0;

   neopop_reset();

   return(1);
}

static void CloseGame(void)
{
   rom_unload();
   if (NGPGfx)
      free(NGPGfx);
   NGPGfx = NULL;
}

static void SetInput(int port, const char *type, void *ptr)
{
   if(!port)
      chee = (uint8 *)ptr;
}

int StateAction(StateMem *sm, int load, int data_only)
{
   SFORMAT StateRegs[] =
   {
      SFVARN(z80_runtime, "z80_runtime"),
      SFARRAY(CPUExRAM, 16384),
      SFVARN_BOOL(FlashStatusEnable, "FlashStatusEnable"),
      SFEND
   };

   SFORMAT TLCS_StateRegs[] =
   {
      { &pc, (uint32_t)sizeof(pc), MDFNSTATE_RLSB, "PC" },
      { &sr, (uint32_t)sizeof(sr), MDFNSTATE_RLSB, "SR" },
      { &f_dash, (uint32_t)sizeof(f_dash), MDFNSTATE_RLSB, "F_DASH" },
      { gpr, (uint32_t)(4 * sizeof(uint32_t)), MDFNSTATE_RLSB32, "GPR" },
      { gprBank[0], (uint32_t)(4 * sizeof(uint32_t)), MDFNSTATE_RLSB32, "GPRB0" },
      { gprBank[1], (uint32_t)(4 * sizeof(uint32_t)), MDFNSTATE_RLSB32, "GPRB1" },
      { gprBank[2], (uint32_t)(4 * sizeof(uint32_t)), MDFNSTATE_RLSB32, "GPRB2" },
      { gprBank[3], (uint32_t)(4 * sizeof(uint32_t)), MDFNSTATE_RLSB32, "GPRB3" },
      { 0, 0, 0, 0 }
   };

   if(!MDFNSS_StateAction(sm, load, data_only, StateRegs, "MAIN", false))
      return(0);

   if(!MDFNSS_StateAction(sm, load, data_only, TLCS_StateRegs, "TLCS", false))
      return(0);

   if(!MDFNNGPCDMA_StateAction(sm, load, data_only))
      return(0);

   if(!MDFNNGPCSOUND_StateAction(sm, load, data_only))
      return(0);

   if(!ngpgfx_StateAction(NGPGfx, sm, load, data_only))
      return(0);

   if(!MDFNNGPCZ80_StateAction(sm, load, data_only))
      return(0);

   if(!int_timer_StateAction(sm, load, data_only))
      return(0);

   if(!BIOSHLE_StateAction(sm, load, data_only))
      return(0);

   if(!FLASH_StateAction(sm, load, data_only))
      return(0);

   if(load)
   {
      RecacheFRM();
      changedSP();
   }
   return(1);
}

static void DoSimpleCommand(int cmd)
{
   switch(cmd)
   {
      case MDFN_MSC_POWER:
      case MDFN_MSC_RESET:
         neopop_reset();
         break;
   }
}

static void SetLayerEnableMask(uint64 mask)
{
 ngpgfx_SetLayerEnableMask(NGPGfx, mask);
}

static const InputDeviceInputInfoStruct IDII[] =
{
 { "up", "UP ↑", 0, IDIT_BUTTON, "down" },
 { "down", "DOWN ↓", 1, IDIT_BUTTON, "up" },
 { "left", "LEFT ←", 2, IDIT_BUTTON, "right" },
 { "right", "RIGHT →", 3, IDIT_BUTTON, "left" },
 { "a", "A", 5, IDIT_BUTTON_CAN_RAPID,  NULL },
 { "b", "B", 6, IDIT_BUTTON_CAN_RAPID, NULL },
 { "option", "OPTION", 4, IDIT_BUTTON, NULL },
};
static InputDeviceInfoStruct InputDeviceInfo[] =
{
 {
  "gamepad",
  "Gamepad",
  NULL,
  NULL,
  sizeof(IDII) / sizeof(InputDeviceInputInfoStruct),
  IDII,
 }
};

static const InputPortInfoStruct PortInfo[] =
{
 { "builtin", "Built-In", sizeof(InputDeviceInfo) / sizeof(InputDeviceInfoStruct), InputDeviceInfo, "gamepad" }
};

MDFNGI EmulatedNGP = {0};

static void MDFNGI_reset(MDFNGI *gameinfo)
{
 gameinfo->MasterClock = MDFN_MASTERCLOCK_FIXED(6144000);
 gameinfo->fps = 0;
 gameinfo->multires = false; /* Multires possible? */

 gameinfo->lcm_width = 160;
 gameinfo->lcm_height = 152;
 gameinfo->dummy_separator = NULL;

 gameinfo->nominal_width = 160;
 gameinfo->nominal_height = 152;

 gameinfo->fb_width = 160;
 gameinfo->fb_height = 152;

 gameinfo->soundchan = 2;
}

static bool MDFNI_LoadGame(const char *name)
{
   struct MDFNFILE *GameFile = file_open(name);

   if(!GameFile)
      goto error;

   if(Load(name, GameFile, NULL, 0) <= 0)
      goto error;

   file_close(GameFile);
   GameFile     = NULL;

   return true;

error:
   if (GameFile)
      file_close(GameFile);
   GameFile     = NULL;
   MDFNGI_reset(&EmulatedNGP);
   return false;
}

static bool MDFNI_LoadGameData(const uint8_t *data, size_t size)
{
   if(Load("", NULL, data, size) <= 0)
      goto error;
   return true;

error:
   MDFNGI_reset(&EmulatedNGP);
   return false;
}

static void MDFNI_CloseGame(void)
{
   CloseGame();
   MDFNGI_reset(&EmulatedNGP);
}

static void extract_basename(char *buf, const char *path, size_t size)
{
   char *ext        = NULL;
   const char *base = strrchr(path, '/');
   if (!base)
      base = strrchr(path, '\\');
   if (!base)
      base = path;

   if (*base == '\\' || *base == '/')
      base++;

   strncpy(buf, base, size - 1);
   buf[size - 1] = '\0';

   ext = strrchr(buf, '.');
   if (ext)
      *ext = '\0';
}

static bool update_video = false;
static bool update_audio = false;

#define MEDNAFEN_CORE_NAME_MODULE "ngp"
#define MEDNAFEN_CORE_NAME "Beetle NeoPop"
#define MEDNAFEN_CORE_VERSION "v0.9.36.1"
#define MEDNAFEN_CORE_EXTENSIONS "ngp|ngc|ngpc|npc"
#define MEDNAFEN_CORE_TIMING_FPS 60.25
#define MEDNAFEN_CORE_GEOMETRY_BASE_W 160 
#define MEDNAFEN_CORE_GEOMETRY_BASE_H 152
#define MEDNAFEN_CORE_GEOMETRY_MAX_W 160
#define MEDNAFEN_CORE_GEOMETRY_MAX_H 152
#define MEDNAFEN_CORE_GEOMETRY_ASPECT_RATIO (20.0 / 19.0)
#define FB_WIDTH 160
#define FB_HEIGHT 152
#define FB_MAX_HEIGHT FB_HEIGHT

static void check_system_specs(void)
{
   unsigned level = 0;
   environ_cb(RETRO_ENVIRONMENT_SET_PERFORMANCE_LEVEL, &level);
}

static void check_color_depth(void)
{
   if (RETRO_PIX_DEPTH == 24)
   {
      enum retro_pixel_format rgb888 = RETRO_PIXEL_FORMAT_XRGB8888;

      if(!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &rgb888))
      {
         if(log_cb) log_cb(RETRO_LOG_ERROR, "Pixel format XRGB8888 not supported by platform.\n");

         RETRO_PIX_BYTES = 2;
         RETRO_PIX_DEPTH = 15;
      }
   }

   if (RETRO_PIX_BYTES == 2)
   {
#if defined(FRONTEND_SUPPORTS_RGB565)
      enum retro_pixel_format rgb565 = RETRO_PIXEL_FORMAT_RGB565;

      if (environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &rgb565))
      {
         if(log_cb) log_cb(RETRO_LOG_INFO, "Frontend supports RGB565 - will use that instead of XRGB1555.\n");

         RETRO_PIX_DEPTH = 16;
      }
#else
      enum retro_pixel_format rgb555 = RETRO_PIXEL_FORMAT_0RGB1555;

      if (environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &rgb555))
      {
         if(log_cb) log_cb(RETRO_LOG_INFO, "Using default 0RGB1555 pixel format.\n");

         RETRO_PIX_DEPTH = 15;
      }
#endif
   }
}

static void check_variables(void)
{
   struct retro_variable var = {0};

   var.key   = "ngp_language";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      /* user must manually restart core for change to happen */
      if (!strcmp(var.value, "japanese"))
         setting_ngp_language = 0;
      else if (!strcmp(var.value, "english"))
         setting_ngp_language = 1;
   }

   var.key = "ngp_sound_sample_rate";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      int old_value = RETRO_SAMPLE_RATE;

      RETRO_SAMPLE_RATE = atoi(var.value);

      if (old_value != RETRO_SAMPLE_RATE)
         update_audio = true;
   }

   var.key = "ngp_gfx_colors";
   var.value = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) && var.value)
   {
      int old_value = RETRO_PIX_BYTES;

      if (strcmp(var.value, "16bit") == 0)
      {
         RETRO_PIX_BYTES = 2;
         RETRO_PIX_DEPTH = 16;
      }
      else if (strcmp(var.value, "24bit") == 0)
      {
         RETRO_PIX_BYTES = 4;
         RETRO_PIX_DEPTH = 24;
      }

      if (old_value != RETRO_PIX_BYTES)
         update_video = true;
   }
}


void retro_init(void)
{
   struct retro_log_callback log;
   const char *dir = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_LOG_INTERFACE, &log))
      log_cb = log.log;
   else 
      log_cb = NULL;

   if (environ_cb(RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY, &dir) && dir)
      strcpy(retro_base_directory, dir);
   else
   {
      /* TODO: Add proper fallback */
      if (log_cb)
         log_cb(RETRO_LOG_WARN, "System directory is not defined. Fallback on using same dir as ROM for system directory later ...\n");
      failed_init = true;
   }
   
   /* If save directory is defined use it, otherwise use system directory */
   if (environ_cb(RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY, &dir) && dir)
      strcpy(retro_save_directory, dir);
   else
   {
      /* TODO: Add proper fallback */
      if (log_cb)
         log_cb(RETRO_LOG_WARN, "Save directory is not defined. Fallback on using SYSTEM directory ...\n");
      strcpy(retro_save_directory, retro_base_directory);
   }      

   perf_get_cpu_features_cb = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_PERF_INTERFACE, &perf_cb))
      perf_get_cpu_features_cb = perf_cb.get_cpu_features;

   if (environ_cb(RETRO_ENVIRONMENT_GET_INPUT_BITMASKS, NULL))
      libretro_supports_bitmasks = true;

   check_system_specs();
}

void retro_reset(void)
{
   DoSimpleCommand(MDFN_MSC_RESET);
}

bool retro_load_game_special(unsigned a, const struct retro_game_info *b, size_t c)
{
   return false;
}

static void set_volume (uint32_t *ptr, unsigned number)
{
   switch(number)
   {
      case 0:
      default:
         *ptr = number;
         break;
   }
}

#define MAX_PLAYERS 1
#define MAX_BUTTONS 7
static uint8_t input_buf;

static void hookup_ports(bool force)
{
   if (initial_ports_hookup && !force)
      return;

   SetInput(0, "gamepad", &input_buf);

   initial_ports_hookup = true;
}

bool retro_load_game(const struct retro_game_info *info)
{
   struct retro_input_descriptor desc[] = {
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_LEFT,  "D-Pad Left" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_UP,    "D-Pad Up" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_DOWN,  "D-Pad Down" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_RIGHT, "D-Pad Right" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_B,     "A" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_A,     "B" },
      { 0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_START, "Option" },

      { 0 },
   };

   if (!info || failed_init)
      return false;

   environ_cb(RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS, desc);

   overscan = false;
   environ_cb(RETRO_ENVIRONMENT_GET_OVERSCAN, &overscan);

   extract_basename(retro_base_name, info->path, sizeof(retro_base_name));

   check_variables();
   check_color_depth();

#ifdef LOAD_FROM_MEMORY
   if (!MDFNI_LoadGameData((const uint8_t *)info->data, info->size))
      return false;
#else
   if (!MDFNI_LoadGame(info->path))
      return false;
#endif

   MDFN_LoadGameCheats(NULL);
   MDFNMP_InstallReadPatches();

   surf = (MDFN_Surface*)calloc(1, sizeof(*surf));

   if (!surf)
      return false;

   surf->width  = FB_WIDTH;
   surf->height = FB_HEIGHT;
   surf->pitch  = FB_WIDTH;
   surf->depth  = RETRO_PIX_DEPTH;

   surf->pixels = (uint16_t*)calloc(1, FB_WIDTH * FB_HEIGHT * sizeof(uint32_t));

   if (!surf->pixels)
   {
      free(surf);
      return false;
   }

   hookup_ports(true);

   ngpgfx_set_pixel_format(NGPGfx, RETRO_PIX_DEPTH);
   MDFNNGPC_SetSoundRate(RETRO_SAMPLE_RATE);

   update_video = false;
   update_audio = false;

   return true;
}

void retro_unload_game(void)
{
   MDFN_FlushGameCheats(0);
   MDFNI_CloseGame();
   MDFNMP_Kill();

   if (surf)
   {
      if (surf->pixels)
         free(surf->pixels);
      free(surf);
   }
   surf = NULL;
}

static void update_input(void)
{
   static unsigned map[] = {
      RETRO_DEVICE_ID_JOYPAD_UP,    /* X Cursor horizontal-layout games */
      RETRO_DEVICE_ID_JOYPAD_DOWN,  /* X Cursor horizontal-layout games */
      RETRO_DEVICE_ID_JOYPAD_LEFT,  /* X Cursor horizontal-layout games */
      RETRO_DEVICE_ID_JOYPAD_RIGHT, /* X Cursor horizontal-layout games */
      RETRO_DEVICE_ID_JOYPAD_B,
      RETRO_DEVICE_ID_JOYPAD_A,
      RETRO_DEVICE_ID_JOYPAD_START,
   };
   unsigned i, j;
   int16_t ret = 0;
   input_buf   = 0;

   if (libretro_supports_bitmasks)
      ret = input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, RETRO_DEVICE_ID_JOYPAD_MASK);
   else
   {
      for (j = 0; j < (RETRO_DEVICE_ID_JOYPAD_R3+1); j++)
         if (input_state_cb(0, RETRO_DEVICE_JOYPAD, 0, j))
            ret |= (1 << j);
   }

   for (i = 0; i < MAX_BUTTONS; i++)
      if ((map[i] != -1u) && (ret & (1 << map[i])))
         input_buf |= (1 << i);
}

static uint64_t video_frames, audio_frames;

void retro_run(void)
{
   int total = 0;
   int32 SoundBufSize;
   unsigned width, height;
   static int16_t sound_buf[0x10000];
   static MDFN_Rect rects[FB_MAX_HEIGHT];
   EmulateSpecStruct spec = {0};
   bool updated = false;

   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
      check_variables();

   input_poll_cb();

   update_input();

   rects[0].w              = ~0;

   spec.surface            = surf;
   spec.SoundRate          = RETRO_SAMPLE_RATE;
   spec.LineWidths         = rects;
   spec.SoundBufMaxSize    = sizeof(sound_buf) / 2;
   spec.SoundBufSize       = 0;
   spec.VideoFormatChanged = update_video;
   spec.SoundFormatChanged = update_audio;

   if (update_video || update_audio)
   {
      struct retro_system_av_info system_av_info;

      if (update_video)
      {
         memset(&system_av_info, 0, sizeof(system_av_info));
         environ_cb(RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO, &system_av_info);
      }

      retro_get_system_av_info(&system_av_info);
      environ_cb(RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO, &system_av_info);

      surf->depth = RETRO_PIX_DEPTH;

      update_video = false;
      update_audio = false;
   }

   Emulate(&spec, sound_buf);

   SoundBufSize      = spec.SoundBufSize - spec.SoundBufSizeALMS;
   spec.SoundBufSize = spec.SoundBufSizeALMS + SoundBufSize;

   width  = spec.DisplayRect.w;
   height = spec.DisplayRect.h;

   video_cb(surf->pixels, width, height, FB_WIDTH * RETRO_PIX_BYTES);

   video_frames++;
   audio_frames += spec.SoundBufSize;

   for (total = 0; total < spec.SoundBufSize; )
      total += audio_batch_cb(sound_buf + total*2, spec.SoundBufSize - total);

}

void retro_get_system_info(struct retro_system_info *info)
{
   memset(info, 0, sizeof(*info));
   info->library_name     = MEDNAFEN_CORE_NAME;
#ifndef GIT_VERSION
#define GIT_VERSION ""
#endif

#ifdef LOAD_FROM_MEMORY
   info->need_fullpath    = false;
#else
   info->need_fullpath    = true;
#endif

   info->library_version  = MEDNAFEN_CORE_VERSION GIT_VERSION;
   info->valid_extensions = MEDNAFEN_CORE_EXTENSIONS;
   info->block_extract    = false;
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   memset(info, 0, sizeof(*info));
   info->timing.fps            = MEDNAFEN_CORE_TIMING_FPS;
   info->timing.sample_rate    = RETRO_SAMPLE_RATE;
   info->geometry.base_width   = MEDNAFEN_CORE_GEOMETRY_BASE_W;
   info->geometry.base_height  = MEDNAFEN_CORE_GEOMETRY_BASE_H;
   info->geometry.max_width    = MEDNAFEN_CORE_GEOMETRY_MAX_W;
   info->geometry.max_height   = MEDNAFEN_CORE_GEOMETRY_MAX_H;
   info->geometry.aspect_ratio = MEDNAFEN_CORE_GEOMETRY_ASPECT_RATIO;

   check_color_depth();
}

void retro_deinit(void)
{
   if (surf)
   {
      if (surf->pixels)
         free(surf->pixels);
      free(surf);
   }
   surf = NULL;

   if (log_cb)
   {
      log_cb(RETRO_LOG_INFO, "[%s]: Samples / Frame: %.5f\n",
            MEDNAFEN_CORE_NAME, (double)audio_frames / video_frames);
      log_cb(RETRO_LOG_INFO, "[%s]: Estimated FPS: %.5f\n",
            MEDNAFEN_CORE_NAME, (double)video_frames * 44100 / audio_frames);
   }

   libretro_supports_bitmasks = false;
}

unsigned retro_get_region(void)
{
   return RETRO_REGION_NTSC; /* FIXME: Regions for other cores. */
}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned in_port, unsigned device)
{
}

void retro_set_environment(retro_environment_t cb)
{
   struct retro_vfs_interface_info vfs_iface_info;
   environ_cb = cb;

   libretro_set_core_options(environ_cb);

   vfs_iface_info.required_interface_version = 1;
   vfs_iface_info.iface                      = NULL;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VFS_INTERFACE, &vfs_iface_info))
	   filestream_vfs_init(&vfs_iface_info);
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
   input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}

size_t retro_serialize_size(void)
{
   StateMem st;

   st.data           = NULL;
   st.loc            = 0;
   st.len            = 0;
   st.malloced       = 0;
   st.initial_malloc = 0;

   if (!MDFNSS_SaveSM(&st, 0, 0, NULL, NULL, NULL))
      return 0;

   free(st.data);

   return st.len;
}

bool retro_serialize(void *data, size_t size)
{
   StateMem st;
   bool ret          = false;
   uint8_t *_dat     = (uint8_t*)malloc(size);

   if (!_dat)
      return false;

   /* Mednafen can realloc the buffer so we need to ensure this is safe. */
   st.data           = _dat;
   st.loc            = 0;
   st.len            = 0;
   st.malloced       = size;
   st.initial_malloc = 0;

   ret = MDFNSS_SaveSM(&st, 0, 0, NULL, NULL, NULL);

   memcpy(data, st.data, size);
   free(st.data);

   return ret;
}

bool retro_unserialize(const void *data, size_t size)
{
   StateMem st;

   st.data           = (uint8_t*)data;
   st.loc            = 0;
   st.len            = size;
   st.malloced       = 0;
   st.initial_malloc = 0;

   return MDFNSS_LoadSM(&st, 0, 0);
}

void *retro_get_memory_data(unsigned type)
{
   if(type == RETRO_MEMORY_SYSTEM_RAM)
      return CPUExRAM;
   return NULL;
}

size_t retro_get_memory_size(unsigned type)
{
   if(type == RETRO_MEMORY_SYSTEM_RAM)
      return 16384;
   return 0;
}

void retro_cheat_reset(void) { }
void retro_cheat_set(unsigned a, bool b, const char *c) { }

/* Use a simpler approach to make sure that things go right for libretro. */
void MDFN_MakeFName(MakeFName_Type type, char *s, size_t len,
      int id1, const char *cd1)
{
#ifdef _WIN32
   char slash = '\\';
#else
   char slash = '/';
#endif
   switch (type)
   {
      case MDFNMKF_SAV:
         snprintf(s, len, "%s%c%s%s%s", 
               retro_save_directory, slash, retro_base_name, ".",
               cd1);
         break;
      default:	  
         break;
   }

   if (log_cb)
      log_cb(RETRO_LOG_INFO, "MDFN_MakeFName: %s\n", s);
}
