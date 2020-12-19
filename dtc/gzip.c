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

#include "gzip.h"

static void *zalloc(void *x, unsigned items, unsigned size)
{
  void *p;

  size *= items;
  size = (size + ZALLOC_ALIGNMENT - 1) & ~(ZALLOC_ALIGNMENT - 1);

  p = malloc (size);
  return (p);
}

static void zfree(void *x, void *addr)
{
  free (addr);
}

// Uncompress blocks compressed with zlib without headers
static int zunzip(void *dst, int dstlen, unsigned char *src, unsigned long *lenp,
            int stoponerr, int offset)
{
  z_stream s;
  int r;

  s.zalloc = zalloc;
  s.zfree = zfree;

  r = inflateInit2(&s, -MAX_WBITS);
  if (r != Z_OK) {
    printf ("Error: inflateInit2() returned %d\n", r);
    return -1;
  }

  s.next_in = src + offset;
  s.avail_in = *lenp - offset;
  s.next_out = dst;
  s.avail_out = dstlen;

  do {
    r = inflate(&s, Z_FINISH);
    if (stoponerr == 1 && r != Z_STREAM_END &&
        (s.avail_out == 0 || r != Z_BUF_ERROR)) {
      printf("Error: inflate() returned %d\n", r);
      inflateEnd(&s);
      return -1;
    }

    s.avail_in = *lenp - offset - (int)(s.next_out - (unsigned char*)dst);
  } while (r == Z_BUF_ERROR);

  *lenp = s.next_out - (unsigned char *) dst;
  inflateEnd(&s);
  return 0;
}

//  Compress blocks with zlib
static int zzip(void *dst, unsigned long *lenp, unsigned char *src,
    unsigned long srclen, int stoponerr,
    int (*func)(unsigned long, unsigned long))
{
  z_stream s;
  int r, flush, orig, window;
  unsigned long comp_len, left_len;

  if (!srclen)
    return 0;

#ifndef CONFIG_GZIP
  window = MAX_WBITS;
#else
  window = 2 * MAX_WBITS;
#endif
  orig = *lenp;
  s.zalloc = zalloc;
  s.zfree = zfree;
  s.opaque = Z_NULL;

  r = deflateInit2_(&s, Z_DEFAULT_COMPRESSION, Z_DEFLATED,  window,
      DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
      ZLIB_VERSION, sizeof(z_stream));
  if (r != Z_OK) {
    printf ("Error: deflateInit2_() returned %d\n", r);
    return -1;
  }

  while (srclen > 0) {
    comp_len = (srclen > CONFIG_GZIP_COMPRESS_DEF_SZ) ?
        CONFIG_GZIP_COMPRESS_DEF_SZ : srclen;

    s.next_in = src;
    s.avail_in = comp_len;
    flush = (srclen > CONFIG_GZIP_COMPRESS_DEF_SZ)?
      Z_NO_FLUSH : Z_FINISH;

    do {
      left_len = (*lenp > CONFIG_GZIP_COMPRESS_DEF_SZ) ?
          CONFIG_GZIP_COMPRESS_DEF_SZ : *lenp;
      s.next_out = dst;
      s.avail_out = left_len;

      r = deflate(&s, flush);
      if (r == Z_STREAM_ERROR && stoponerr == 1) {
        printf("Error: deflate() returned %d\n", r);
        r = -1;
        goto bail;
      }

      if (!func) {
        dst += (left_len - s.avail_out);
        *lenp -= (left_len - s.avail_out);
      } else if (left_len - s.avail_out > 0) {
        r = func((unsigned long)dst,
          left_len - s.avail_out);
        if (r < 0)
          goto bail;
      }
    } while (s.avail_out == 0 && (*lenp > 0));

    if (s.avail_in) {
      printf("Deflate failed to consume %u bytes", s.avail_in);
      r = -1;
      goto bail;
    }

    if (*lenp == 0) {
      printf("Deflate need more space to compress "
        "left %lu bytes\n", srclen);
      r = -1;
      goto bail;
    }

    srclen -= comp_len;
    src += comp_len;
  }

  r = 0;
bail:
  deflateEnd(&s);
  *lenp = orig - *lenp;
  return r;
}

int gzip(void *dst, unsigned long *lenp,
    unsigned char *src, unsigned long srclen)
{
  return zzip(dst, lenp, src, srclen, 1, NULL);
}

int gunzip(void *dst, int dstlen,
    unsigned char *src, unsigned long *lenp)
{
  int i, flags;

  // skip header
  i = 10;
  flags = src[3];
  if (src[2] != Z_DEFLATED || (flags & RESERVED) != 0) {
    puts ("Error: Bad gzipped data\n");
    return (-1);
  }

  if ((flags & EXTRA_FIELD) != 0)
    i = 12 + src[10] + (src[11] << 8);

  if ((flags & ORIG_NAME) != 0)
    while (src[i++] != 0)
      ;

  if ((flags & COMMENT) != 0)
    while (src[i++] != 0)
      ;

  if ((flags & HEAD_CRC) != 0)
    i += 2;

  if (i >= *lenp) {
    puts ("Error: gunzip out of data in header\n");
    return (-1);
  }

  return zunzip(dst, dstlen, src, lenp, 1, i);
}
