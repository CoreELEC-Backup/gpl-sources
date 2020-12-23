/* Mednafen - Multi-system Emulator
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "pce.h"
#include "input.h"
#include "../mednafen-endian.h"

bool AVPad6Enabled[5] = { 0 };

static int InputTypes[5];
static uint8 *data_ptr[5];

static bool AVPad6Which[5]; // Lower(8 buttons) or higher(4 buttons).

static uint16 pce_jp_data[5];

static int64 mouse_last_meow[5];

static int32 mouse_x[5], mouse_y[5];
static uint16 mouse_rel[5];

static uint8 pce_mouse_button[5];
static uint8 mouse_index[5];

static uint8 sel;
static uint8 read_index = 0;

static bool DisableSR;
static bool EnableMultitap;

static void SyncSettings(void);

void PCEINPUT_SettingChanged(const char *name)
{
   SyncSettings();
}

void PCEINPUT_Init(void)
{
   SyncSettings();
}

void PCEINPUT_SetInput(unsigned port, const char *type, uint8 *ptr)
{
   assert(port < 5);

   if (!strcmp(type, "gamepad"))
      InputTypes[port] = 1;
   else if (!strcmp(type, "mouse"))
      InputTypes[port] = 2;
   else
      InputTypes[port] = 0;
   data_ptr[port] = (uint8 *)ptr;
}

void INPUT_TransformInput(void)
{
   for (int x = 0; x < 5; x++)
   {
      if (InputTypes[x] == 1)
      {
         if (DisableSR)
         {
            uint16 tmp = MDFN_de16lsb(data_ptr[x]);

            if ((tmp & 0xC) == 0xC)
               tmp &= ~0xC;

            MDFN_en16lsb(data_ptr[x], tmp);
         }
      }
   }
}

void INPUT_Frame(void)
{
   for (int x = 0; x < 5; x++)
   {
      if (InputTypes[x] == 1)
      {
         uint16 new_data = data_ptr[x][0] | (data_ptr[x][1] << 8);

         if ((new_data & 0x1000)  && !(pce_jp_data[x] & 0x1000))
         {
            AVPad6Enabled[x] = !AVPad6Enabled[x];
            MDFN_DispMessage("%d-button mode selected for pad %d",
                  AVPad6Enabled[x] ? 6 : 2, x + 1);
         }

         pce_jp_data[x] = new_data;
      }
      else if (InputTypes[x] == 2)
      {
         mouse_x[x] += (int16)MDFN_de16lsb(data_ptr[x] + 0);
         mouse_y[x] += (int16)MDFN_de16lsb(data_ptr[x] + 2);
         pce_mouse_button[x] = *(uint8 *)(data_ptr[x] + 4);
      }
   }
}

void INPUT_FixTS(void)
{
   for (int x = 0; x < 5; x++)
   {
      if (InputTypes[x] == 2)
         mouse_last_meow[x] -= HuCPU.timestamp;
   }
}

static INLINE bool CheckLM(int n)
{
   if ((int64)HuCPU.timestamp - mouse_last_meow[n] > 10000)
   {
      mouse_last_meow[n] = HuCPU.timestamp;

      int32 rel_x = (int32)((0 - mouse_x[n]));
      int32 rel_y = (int32)((0 - mouse_y[n]));

      if (rel_x < -127)
         rel_x = -127;
      if (rel_x > 127)
         rel_x = 127;
      if (rel_y < -127)
         rel_y = -127;
      if (rel_y > 127)
         rel_y = 127;

      mouse_rel[n] = ((rel_x & 0xF0) >> 4) | ((rel_x & 0x0F) << 4);
      mouse_rel[n] |= (((rel_y & 0xF0) >> 4) | ((rel_y & 0x0F) << 4)) << 8;

      mouse_x[n] += (int32)(rel_x);
      mouse_y[n] += (int32)(rel_y);

      return (1);
   }
   return (0);
}

static uint8 ReadPortMouse(int n)
{
   uint8 ret = 0xF;

   if (sel & 1)
   {
      CheckLM(n);
      ret ^= 0xF;
      ret ^= mouse_rel[n] & 0xF;

      mouse_rel[n] >>= 4;
   }
   else
      ret ^= pce_mouse_button[n] & 0xF;
   return (ret);
}

static uint8 ReadPortGamepad(int n)
{
   uint8 ret = 0xF;

   if (AVPad6Which[n] && AVPad6Enabled[n])
   {
      if (sel & 1)
         ret ^= 0x0F;
      else
         ret ^= (pce_jp_data[n] >> 8) & 0x0F;
   }
   else
   {
      if (sel & 1)
         ret ^= (pce_jp_data[n] >> 4) & 0x0F;
      else
         ret ^= pce_jp_data[n] & 0x0F;
   }

   return (ret);
}

static uint8 ReadPort(int n)
{
   uint8 ret = 0xF;

   if (!InputTypes[n])
      ret ^= 0xF;
   else if (InputTypes[n] == 2) // Mouse
      ret = ReadPortMouse(n);
   else if (InputTypes[n] == 1) // Gamepad
      ret = ReadPortGamepad(n);

   return (ret);
}

static void WritePort(int n, uint8 V)
{
   if (InputTypes[n] == 1)
   {
      if (!(sel & 1) && (V & 1))
         AVPad6Which[n] = !AVPad6Which[n];
   }
}

static uint8 ReadPortMultitap(int n)
{
   uint8 ret = 0xF;

   if (n > 4)
      ret ^= 0xF;
   else
      ret = ReadPort(n);

   return (ret);
}

static void WritePortMultitap(uint8 V)
{
   for (int i = 0; i < 5; i++)
      WritePort(i, V);

   if ((V & 1) && !(sel & 2) && (V & 2))
      read_index = 0;
   else if ((V & 1) && !(sel & 1))
   {
      if (read_index < 255)
         read_index++;
   }
}

uint8 INPUT_Read(unsigned int A)
{
   uint8 ret = 0xF;
   int tmp_ri = read_index;

   if (EnableMultitap)
      ret = ReadPortMultitap(tmp_ri);
   else
      ret = ReadPort(0);

   if (!PCE_IsCD)
      ret |= 0x80; // Set when CDROM is not attached

   //ret |= 0x40; // PC Engine if set, TG16 if clear.  Let's leave it clear, PC Engine games don't seem to mind if it's clear, but TG16 games barf if it's set.

   ret |= 0x30; // Always-set?

   return (ret);
}

void INPUT_Write(unsigned int A, uint8 V)
{
   if (EnableMultitap)
      WritePortMultitap(V);
   else
      WritePort(0, V);

   sel = V & 3;
}

int INPUT_StateAction(StateMem *sm, int load, int data_only)
{
   SFORMAT StateRegs[] =
   {
      // 0.8.A fix:
      SFARRAYB(AVPad6Enabled, 5),
      SFARRAYB(AVPad6Which, 5),

      SFVARN(mouse_last_meow[0], "mlm_0"),
      SFVARN(mouse_last_meow[1], "mlm_1"),
      SFVARN(mouse_last_meow[2], "mlm_2"),
      SFVARN(mouse_last_meow[3], "mlm_3"),
      SFVARN(mouse_last_meow[4], "mlm_4"),

      SFARRAY32(mouse_x, 5),
      SFARRAY32(mouse_y, 5),
      SFARRAY16(mouse_rel, 5),
      SFARRAY(pce_mouse_button, 5),
      SFARRAY(mouse_index, 5),
      // end 0.8.A fix

      SFARRAY16(pce_jp_data, 5),
      SFVAR(sel),
      SFVAR(read_index),
      SFEND
   };
   int ret = MDFNSS_StateAction(sm, load, data_only, StateRegs, "JOY", false);

   return (ret);
}

// GamepadIDII and GamepadIDII_DSR must be EXACTLY the same except for the RUN+SELECT exclusion in the latter.
static const InputDeviceInputInfoStruct GamepadIDII[] =
{
   {"i", "I", 12, IDIT_BUTTON_CAN_RAPID, NULL},
   {"ii", "II", 11, IDIT_BUTTON_CAN_RAPID, NULL},
   {"select", "SELECT", 4, IDIT_BUTTON, NULL},
   {"run", "RUN", 5, IDIT_BUTTON, NULL},
   {"up", "UP ↑", 0, IDIT_BUTTON, "down"},
   {"right", "RIGHT →", 3, IDIT_BUTTON, "left"},
   {"down", "DOWN ↓", 1, IDIT_BUTTON, "up"},
   {"left", "LEFT ←", 2, IDIT_BUTTON, "right"},
   {"iii", "III", 10, IDIT_BUTTON, NULL},
   {"iv", "IV", 7, IDIT_BUTTON, NULL},
   {"v", "V", 8, IDIT_BUTTON, NULL},
   {"vi", "VI", 9, IDIT_BUTTON, NULL},
   {"mode_select", "2/6 Mode Select", 6, IDIT_BUTTON, NULL},
};
static const InputDeviceInputInfoStruct GamepadIDII_DSR[] =
{
   {"i", "I", 12, IDIT_BUTTON_CAN_RAPID, NULL},
   {"ii", "II", 11, IDIT_BUTTON_CAN_RAPID, NULL},
   {"select", "SELECT", 4, IDIT_BUTTON, "run"},
   {"run", "RUN", 5, IDIT_BUTTON, "select"},
   {"up", "UP ↑", 0, IDIT_BUTTON, "down"},
   {"right", "RIGHT →", 3, IDIT_BUTTON, "left"},
   {"down", "DOWN ↓", 1, IDIT_BUTTON, "up"},
   {"left", "LEFT ←", 2, IDIT_BUTTON, "right"},
   {"iii", "III", 10, IDIT_BUTTON, NULL},
   {"iv", "IV", 7, IDIT_BUTTON, NULL},
   {"v", "V", 8, IDIT_BUTTON, NULL},
   {"vi", "VI", 9, IDIT_BUTTON, NULL},
   {"mode_select", "2/6 Mode Select", 6, IDIT_BUTTON, NULL},
};

static const InputDeviceInputInfoStruct MouseIDII[] =
{
   {"x_axis", "X Axis", -1, IDIT_X_AXIS_REL},
   {"y_axis", "Y Axis", -1, IDIT_Y_AXIS_REL},
   {"left", "Left Button", 0, IDIT_BUTTON, NULL},
   {"right", "Right Button", 1, IDIT_BUTTON, NULL},
};

// If we add more devices to this array, REMEMBER TO UPDATE the hackish array indexing in the SyncSettings() function
// below.
static InputDeviceInfoStruct InputDeviceInfo[] =
{
   // None
   {
      "none",
      "none",
      NULL,
      NULL,
      0,
      NULL
   },

   // Gamepad
   {
      "gamepad",
      "Gamepad",
      NULL,
      NULL,
      sizeof(GamepadIDII) / sizeof(InputDeviceInputInfoStruct),
      GamepadIDII,
   },

   // Mouse
   {
      "mouse",
      "Mouse",
      NULL,
      NULL,
      sizeof(MouseIDII) / sizeof(InputDeviceInputInfoStruct),
      MouseIDII,
   },

};

static const InputPortInfoStruct PortInfo[] =
{
   {"port1", "Port 1", sizeof(InputDeviceInfo) / sizeof(InputDeviceInfoStruct), InputDeviceInfo, "gamepad"},
   {"port2", "Port 2", sizeof(InputDeviceInfo) / sizeof(InputDeviceInfoStruct), InputDeviceInfo, "gamepad"},
   {"port3", "Port 3", sizeof(InputDeviceInfo) / sizeof(InputDeviceInfoStruct), InputDeviceInfo, "gamepad"},
   {"port4", "Port 4", sizeof(InputDeviceInfo) / sizeof(InputDeviceInfoStruct), InputDeviceInfo, "gamepad"},
   {"port5", "Port 5", sizeof(InputDeviceInfo) / sizeof(InputDeviceInfoStruct), InputDeviceInfo, "gamepad"},
};

InputInfoStruct PCEInputInfo =
{
   sizeof(PortInfo) / sizeof(InputPortInfoStruct),
   PortInfo
};

static void SyncSettings(void)
{
   MDFNGameInfo->mouse_sensitivity = MDFN_GetSettingF("pce_fast.mouse_sensitivity");
   EnableMultitap = MDFN_GetSettingB("pce_fast.input.multitap");
   DisableSR = MDFN_GetSettingB("pce_fast.disable_softreset");
}
