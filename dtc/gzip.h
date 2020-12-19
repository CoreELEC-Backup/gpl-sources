/*
 * Simple tool for CoreELEC installation on eMMC
 *
 * Copyright (C) 2019 Team CoreELEC, vpeter, Portisch
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
 */

#ifndef __ZLIB_H__
#define __ZLIB_H__

#include <stdio.h>
#include <stdlib.h>
#include "zlib.h"

// enable gzip mode of zlib
#define     CONFIG_GZIP

// Maximum value for memLevel in deflateInit2
#define     DEF_MEM_LEVEL                   8

#ifndef     CONFIG_GZIP_COMPRESS_DEF_SZ
#define     CONFIG_GZIP_COMPRESS_DEF_SZ     0x200
#endif

#define     ZALLOC_ALIGNMENT                16

// gzip flag byte
#define     ASCII_FLAG                      0x01 // bit 0 set: file probably ascii text
#define     HEAD_CRC                        0x02 // bit 1 set: header CRC present
#define     EXTRA_FIELD                     0x04 // bit 2 set: extra field present
#define     ORIG_NAME                       0x08 // bit 3 set: original file name present
#define     COMMENT                         0x10 // bit 4 set: file comment present
#define     RESERVED                        0xE0 // bits 5..7: reserved

extern int gzip(void *dst, unsigned long *lenp,
    unsigned char *src, unsigned long srclen);
extern int gunzip(void *dst, int dstlen,
    unsigned char *src, unsigned long *lenp);

#endif
