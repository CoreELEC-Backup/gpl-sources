#ifndef _MEDNAFEN_H
#define _MEDNAFEN_H

#include "mednafen-types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _(String) (String)

#include "math_ops.h"
#include "git.h"

#ifdef _WIN32
#define strcasecmp _stricmp
#endif

extern MDFNGI *MDFNGameInfo;

#include "settings.h"

void MDFN_PrintError(const char *format, ...);
void MDFN_printf(const char *format, ...);
void MDFN_DispMessage(const char *format, ...);

void MDFN_LoadGameCheats(void *override);
void MDFN_FlushGameCheats(int nosave);

void MDFN_MidSync(EmulateSpecStruct *espec);
void MDFN_MidLineUpdate(EmulateSpecStruct *espec, int y);

#include "mednafen-endian.h"

#endif
