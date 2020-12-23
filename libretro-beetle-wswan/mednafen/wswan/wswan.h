#ifndef __WSWAN_H
#define __WSWAN_H

#include "../mednafen-types.h"

enum
{
   WSWAN_SEX_MALE = 1,
   WSWAN_SEX_FEMALE = 2
};

enum
{
   WSWAN_BLOOD_A = 1,
   WSWAN_BLOOD_B = 2,
   WSWAN_BLOOD_O = 3,
   WSWAN_BLOOD_AB = 4
};

#define  mBCD(value) (((value)/10)<<4)|((value)%10)

#ifdef __cplusplus
extern "C" {
#endif

extern          uint32 rom_size;
extern          int wsc;

#ifdef __cplusplus
}
#endif

#endif
