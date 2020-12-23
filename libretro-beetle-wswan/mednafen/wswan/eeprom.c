/* Cygne
 *
 * Copyright notice for this file:
 *  Copyright (C) 2002 Dox dox@space.pl
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
#include <ctype.h>
#include <string.h>

#include "wswan.h"
#include "eeprom.h"
#include "wswan-memory.h"
#include "../state_inline.h"

uint8_t wsEEPROM[2048];
static uint8_t iEEPROM[0x400];
static const uint8_t iEEPROM_Init[0x400] = 
{
   255,255,255,255,255,255,192,255,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,127,0,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255, 
   0,252,255,1,255,253,255,253,255,253,255,253,
   255,253,255,253,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   0,0,3,3,0,0,0,64,128,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,
   135,5,140,9,5,12,139,12,144,0,0,2,
   0,76,165,0,128,0,0,0,255,127,255,127,
   255,127,255,127,255,127,255,127,255,127,255,127,
   255,127,255,127,255,127,255,127,255,127,255,127,
   255,127,255,127,255,127,255,127,255,127,255,127,
   255,127,255,127,255,127,255,127,255,127,255,127,
   255,127,255,127,255,127,255,127,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   0,0,6,6,6,6,6,0,0,0,0,0,
   1,128,15,0,1,1,1,15,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,
   'C'-54,'Y'-54,'G'-54,'N'-54,'E'-54,0,0,0,0,0,0,0,0,0,0,
   0,32,1,1,33,1,4,0,1,
   0,152,60,127,74,1,53,1,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255
};

static uint8_t iEEPROM_Command, EEPROM_Command;
static uint16_t iEEPROM_Address, EEPROM_Address;

uint8_t WSwan_EEPROMRead(uint32_t A)
{
   switch(A)
   {
      default:
         break;
      case 0xBA:
         return(iEEPROM[(iEEPROM_Address << 1) & 0x3FF]);
      case 0xBB:
         return(iEEPROM[((iEEPROM_Address << 1) | 1) & 0x3FF]);
      case 0xBC:
         return(iEEPROM_Address >> 0);
      case 0xBD:
         return(iEEPROM_Address >> 8);
      case 0xBE:
         if(iEEPROM_Command & 0x20)
            return iEEPROM_Command|2;
         if(iEEPROM_Command & 0x10)
            return iEEPROM_Command|1;
         return iEEPROM_Command | 3;
      case 0xC4:
         return(wsEEPROM[(EEPROM_Address << 1) & (eeprom_size - 1)]);
      case 0xC5:
         return(wsEEPROM[((EEPROM_Address << 1) | 1) & (eeprom_size - 1)]);
      case 0xC6:
         return(EEPROM_Address >> 0);
      case 0xC7:
         return(EEPROM_Address >> 8);
      case 0xC8:
         if(EEPROM_Command & 0x20)
            return EEPROM_Command|2;
         if(EEPROM_Command & 0x10)
            return EEPROM_Command|1;
         return EEPROM_Command | 3;
   }

   return 0;
}


void WSwan_EEPROMWrite(uint32_t A, uint8_t V)
{
   switch(A)
   {
      case 0xBA:
         iEEPROM[(iEEPROM_Address << 1) & 0x3FF] = V;
         break;
      case 0xBB:
         iEEPROM[((iEEPROM_Address << 1) | 1) & 0x3FF] = V;
         break;
      case 0xBC:
         iEEPROM_Address &= 0xFF00; iEEPROM_Address |= (V << 0);
         break;
      case 0xBD:
         iEEPROM_Address &= 0x00FF; iEEPROM_Address |= (V << 8);
         break;
      case 0xBE:
         iEEPROM_Command = V;
         break;
      case 0xC4:
         wsEEPROM[(EEPROM_Address << 1) & (eeprom_size - 1)] = V;
         break;
      case 0xC5:
         wsEEPROM[((EEPROM_Address << 1) | 1) & (eeprom_size - 1)] = V;
         break;
      case 0xC6:
         EEPROM_Address &= 0xFF00; EEPROM_Address |= (V << 0);
         break;
      case 0xC7:
         EEPROM_Address &= 0x00FF; EEPROM_Address |= (V << 8);
         break;
      case 0xC8:
         EEPROM_Command = V;
         break;
   }
}

void WSwan_EEPROMReset(void)
{
   iEEPROM_Command = EEPROM_Command = 0;
   iEEPROM_Address = EEPROM_Address = 0;
}

#define  mBCD16(value) ( (((((value)%100) / 10) <<4)|((value)%10)) | ((((((value / 100)%100) / 10) <<4)|((value / 100)%10))<<8) )
#define INT16_TO_BCD(A)  ((((((A) % 100) / 10) * 16 + ((A) % 10))) | (((((((A) / 100) % 100) / 10) * 16 + (((A) / 100) % 10))) << 8))

void WSwan_EEPROMInit(const char *Name, const uint16_t BYear, const uint8_t BMonth, const uint8_t BDay, const uint8_t Sex, const uint8_t Blood)
{
   unsigned x;
   uint16_t bcd_BYear;

   memset(wsEEPROM, 0, 2048);
   memcpy(iEEPROM, iEEPROM_Init, 0x400);

   for(x = 0; x < 16; x++)
   {
      uint8_t zechar = 0;

      if(x < strlen(Name))
      {
         char tc = toupper(Name[x]);
         if(tc == ' ') zechar = 0;
         else if(tc >= '0' && tc <= '9') zechar = tc - '0' + 0x1;
         else if(tc >= 'A' && tc <= 'Z') zechar = tc - 'A' + 0xB;
         else if(tc >= 'a' && tc <= 'z') zechar = tc - 'a' + 0xB + 26;
      }
      iEEPROM[0x360 + x] = zechar;
   }

   bcd_BYear      = INT16_TO_BCD(BYear);

   iEEPROM[0x370] = (bcd_BYear >> 8) & 0xFF;
   iEEPROM[0x371] = (bcd_BYear >> 0) & 0xFF;
   iEEPROM[0x372] = mBCD(BMonth);
   iEEPROM[0x373] = mBCD(BDay);
   iEEPROM[0x374] = Sex;
   iEEPROM[0x375] = Blood;
}

int WSwan_EEPROMStateAction(StateMem *sm, int load, int data_only)
{
   SFORMAT StateRegs[] =
   {
      SFVARN(iEEPROM_Command, "iEEPROM_Command"),
      SFVARN(iEEPROM_Address, "iEEPROM_Address"),
      SFVARN(EEPROM_Command,  "EEPROM_Command"),
      SFVARN(EEPROM_Address,  "EEPROM_Address"),
      SFARRAYN((iEEPROM), (sizeof(iEEPROM)), "iEEPROM"),
      SFARRAYN(eeprom_size ? wsEEPROM : NULL, eeprom_size, "EEPROM"),
      { 0, 0, 0, 0 }
   };

   if(!MDFNSS_StateAction(sm, load, data_only, StateRegs, "EEPR", false))
      return(0);

   return(1);
}
