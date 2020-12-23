/*
 * ui.c - PSP user interface.
 *
 * Written by
 *  Akop Karapetyan <dev@psp.akop.org>
 *
 * This file is part of VICE, the Versatile Commodore Emulator.
 * See README for copyright notice.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 *  02111-1307  USA.
 *
 */

#include "vice.h"
#include "machine.h"
#include "cmdline.h"
#include "uistatusbar.h"
#include "resources.h"
#include "sid.h"
#include "sid-resources.h"
#include "userport_joystick.h"
#if defined(__VIC20__)
#include "c64model.h"
#include "vic20model.h"
#elif defined(__PLUS4__)
#include "c64model.h"
#include "plus4model.h"
#elif defined(__X128__)
#include "c64model.h"
#include "c128model.h"
#elif defined(__PET__)
#include "petmodel.h"
#elif defined(__CBM2__)
#include "cbm2model.h"
#else
#include "c64model.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

int RETROTDE=0;
int RETRODSE=0;
int RETRORESET=0;
int RETROSIDMODL=0;
int RETRORESIDSAMPLING=0;
int RETROAUDIOLEAK=0;
int RETROC64MODL=0;
#if defined(__X128__)
int RETROC128COLUMNKEY=1;
#endif
#if defined(__VIC20__)
int RETROVIC20MEM=0;
#endif
int RETROUSERPORTJOY=-1;
int RETROEXTPAL=-1;
int RETROAUTOSTARTWARP=0;
int RETROBORDERS=0;
int RETROTHEME=0;
int RETROKEYRAHKEYPAD=0;
int RETROKEYBOARDPASSTHROUGH=0;
char RETROEXTPALNAME[512]="pepto-pal";
int retro_ui_finalized = 0;
int cur_port_locked = 0; /* 0: not forced by filename 1: forced by filename */

static const cmdline_option_t cmdline_options[] = {
     { NULL }
};

/* Initialization  */
int ui_resources_init(void)
{
   if (machine_class != VICE_MACHINE_VSID)
      return uistatusbar_init_resources();
   return 0;
}

void ui_resources_shutdown(void)
{

}

int ui_init(void)
{
    return 0;
}

void ui_shutdown(void)
{
}

void ui_check_mouse_cursor(void)
{
   /* needed */
}

void ui_error(const char *format, ...)
{
   char text[512];	   	
   va_list	ap;	

   if (format == NULL)return;		

   va_start(ap,format );		
   vsprintf(text, format, ap);	
   va_end(ap);	
   fprintf(stderr, "ui_error: %s\n", text);
}

int ui_emulation_is_paused(void)
{
    return 0;
}

void ui_pause_emulation(void)
{
}

/* Update all the menus according to the current settings.  */

void ui_update_menus(void)
{
}

int ui_extend_image_dialog(void)
{
   return 0;
}

void ui_dispatch_events(void)
{
}

int ui_cmdline_options_init(void)
{
   return cmdline_register_options(cmdline_options);
}

int ui_init_finish(void)
{
   return 0;
}

extern int log_resources_set_int(const char *name, int value);
extern int log_resources_set_string(const char *name, const char* value);

int ui_init_finalize(void)
{
   /* Sensible defaults */
   log_resources_set_int("Mouse", 0);
   log_resources_set_int("AutostartPrgMode", 1);

#if defined(__CBM2__) || defined(__PET__)
   log_resources_set_int("CrtcFilter", 0);
   log_resources_set_int("CrtcStretchVertical", 0);
#endif

   /* Core options */
#if defined(__VIC20__)
   if (RETROEXTPAL==-1)
      log_resources_set_int("VICExternalPalette", 0);
   else
   {
      log_resources_set_int("VICExternalPalette", 1);
      log_resources_set_string("VICPaletteFile", RETROEXTPALNAME);
   }
#elif defined(__PLUS4__)
   if (RETROEXTPAL==-1)
      log_resources_set_int("TEDExternalPalette", 0);
   else
   {
      log_resources_set_int("TEDExternalPalette", 1);
      log_resources_set_string("TEDPaletteFile", RETROEXTPALNAME);
   }
#elif defined(__PET__)
   if (RETROEXTPAL==-1)
      log_resources_set_int("CrtcExternalPalette", 0);
   else
   {
      log_resources_set_int("CrtcExternalPalette", 1);
      log_resources_set_string("CrtcPaletteFile", RETROEXTPALNAME);
   }
#elif defined(__CBM2__)
   if (RETROEXTPAL==-1)
      log_resources_set_int("CrtcExternalPalette", 0);
   else
   {
      log_resources_set_int("CrtcExternalPalette", 1);
      log_resources_set_string("CrtcPaletteFile", RETROEXTPALNAME);
   }
#else
   if (RETROEXTPAL==-1)
      log_resources_set_int("VICIIExternalPalette", 0);
   else
   {
      log_resources_set_int("VICIIExternalPalette", 1);
      log_resources_set_string("VICIIPaletteFile", RETROEXTPALNAME);
   }
#endif

   if (RETROUSERPORTJOY==-1)
      log_resources_set_int("UserportJoy", 0);
   else
   {
      log_resources_set_int("UserportJoy", 1);
      log_resources_set_int("UserportJoyType", RETROUSERPORTJOY);
   }

   if (RETROTDE==1)
   {
      log_resources_set_int("DriveTrueEmulation", 1);
      log_resources_set_int("VirtualDevices", 0);
   }
   else
   {
      log_resources_set_int("DriveTrueEmulation", 0);
      log_resources_set_int("VirtualDevices", 1);
   }

   if (RETRODSE==0)
      log_resources_set_int("DriveSoundEmulation", 0);
   else
   {
      log_resources_set_int("DriveSoundEmulation", 1);
      log_resources_set_int("DriveSoundEmulationVolume", RETRODSE);
   }

   log_resources_set_int("AutostartWarp", RETROAUTOSTARTWARP);

#if !defined(__PET__) && !defined(__PLUS4__) && !defined(__VIC20__)
   sid_set_engine_model((RETROSIDMODL >> 8), (RETROSIDMODL & 0xff));
   log_resources_set_int("SidResidSampling", RETRORESIDSAMPLING);
#endif

#if defined(__X64__) || defined(__X64SC__) || defined(__X128__)
   log_resources_set_int("VICIIAudioLeak", RETROAUDIOLEAK);
#elif defined(__VIC20__)
   log_resources_set_int("VICAudioLeak", RETROAUDIOLEAK);
#endif

#if defined(__VIC20__) 
   vic20model_set(RETROC64MODL);
#elif defined(__PLUS4__)
   plus4model_set(RETROC64MODL);
#elif defined(__X128__)
   c128model_set(RETROC64MODL);
#elif defined(__PET__)
   petmodel_set(RETROC64MODL);
#elif defined(__CBM2__)
   cbm2model_set(RETROC64MODL);
#elif defined(__XSCPU64__)
   ;
#else
   c64model_set(RETROC64MODL);
#endif

#if !defined(__PET__) && !defined(__CBM2__)
#if defined(__VIC20__)
   log_resources_set_int("VICBorderMode", RETROBORDERS);
#elif defined(__PLUS4__)
   log_resources_set_int("TEDBorderMode", RETROBORDERS);
#else
   log_resources_set_int("VICIIBorderMode", RETROBORDERS);
#endif
#endif

#if defined(__X128__)
   log_resources_set_int("C128ColumnKey", RETROC128COLUMNKEY);
#endif

#if defined(__VIC20__)
   switch (RETROVIC20MEM)
   {
      case 0:
         log_resources_set_int("RAMBlock0", 0);
         log_resources_set_int("RAMBlock1", 0);
         log_resources_set_int("RAMBlock2", 0);
         log_resources_set_int("RAMBlock3", 0);
         log_resources_set_int("RAMBlock5", 0);
         break;

      case 1:
         log_resources_set_int("RAMBlock0", 1);
         log_resources_set_int("RAMBlock1", 0);
         log_resources_set_int("RAMBlock2", 0);
         log_resources_set_int("RAMBlock3", 0);
         log_resources_set_int("RAMBlock5", 0);
         break;

      case 2:
         log_resources_set_int("RAMBlock0", 0);
         log_resources_set_int("RAMBlock1", 1);
         log_resources_set_int("RAMBlock2", 0);
         log_resources_set_int("RAMBlock3", 0);
         log_resources_set_int("RAMBlock5", 0);
         break;

      case 3:
         log_resources_set_int("RAMBlock0", 0);
         log_resources_set_int("RAMBlock1", 1);
         log_resources_set_int("RAMBlock2", 1);
         log_resources_set_int("RAMBlock3", 0);
         log_resources_set_int("RAMBlock5", 0);
         break;

      case 4:
         log_resources_set_int("RAMBlock0", 0);
         log_resources_set_int("RAMBlock1", 1);
         log_resources_set_int("RAMBlock2", 1);
         log_resources_set_int("RAMBlock3", 1);
         log_resources_set_int("RAMBlock5", 0);
         break;

      case 5:
         log_resources_set_int("RAMBlock0", 1);
         log_resources_set_int("RAMBlock1", 1);
         log_resources_set_int("RAMBlock2", 1);
         log_resources_set_int("RAMBlock3", 1);
         log_resources_set_int("RAMBlock5", 1);
         break;
  }
#endif

   retro_ui_finalized = 1;

   return 0;
}

char* ui_get_file(const char *format,...)
{
   return NULL;
}


#if defined(__X64SC__)
int c64scui_init_early(void)
{
    return 0;
}
#elif defined(__X128__)
int c128ui_init_early(void)
{
    return 0;
}
#elif defined(__XSCPU64__)
int scpu64ui_init_early(void)
{
    return 0;
}
#elif defined(__VIC20__) 
int vic20ui_init_early(void)
{
    return 0;
}
#elif defined(__PET__) 
int petui_init_early(void)
{
    return 0;
}
#elif defined(__PLUS4__) 
int plus4ui_init_early(void)
{
    return 0;
}
#elif defined(__CBM2__) 
int cbm2ui_init_early(void)
{
    return 0;
}
#else
int c64ui_init_early(void)
{
    return 0;
}
#endif
