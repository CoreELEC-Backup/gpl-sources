#ifndef _LIBRETRO_OPTIMIZATIONS_H_
#define _LIBRETRO_OPTIMIZATIONS_H_

#include "streams/file_stream_transforms.h"
extern int kNetGame;

#ifdef USE_CYCLONE
#define SEK_CORE_C68K (0)
#define SEK_CORE_M68K (1)
extern int nSekCpuCore;  // 0 - c68k, 1 - m68k
#endif

#endif
