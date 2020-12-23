/*****************************************************************************
 * desc_1c.h: ISO/IEC 13818-1 Descriptor 0x1c (MPEG-4 audio descriptor)
 *****************************************************************************
 * Copyright (C) 2011 Unix Solutions Ltd.
 *
 * Authors: Georgi Chorbadzhiyski <georgi@unixsol.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject
 * to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *****************************************************************************/

/*
 * Normative references:
 *  - ISO/IEC 13818-1:2007(E) (MPEG-2 Systems)
 */

#ifndef __BITSTREAM_MPEG_DESC_1C_H__
#define __BITSTREAM_MPEG_DESC_1C_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>
#include <bitstream/mpeg/psi/desc_1b.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x1c (MPEG-4 audio descriptor)
 *****************************************************************************/
#define DESC1C_HEADER_SIZE      (DESC_HEADER_SIZE + 1)

static inline void desc1c_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x1c);
    desc_set_length(p_desc, DESC1C_HEADER_SIZE - DESC_HEADER_SIZE);
}

#define desc1c_validate                           desc1b_validate
#define desc1c_get_mpeg4_audio_profile_and_level  desc1b_get_mpeg4_visual_profile_and_level
#define desc1c_set_mpeg4_audio_profile_and_level  desc1b_set_mpeg4_visual_profile_and_level

static inline void desc1c_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque, "<MPEG4_AUDIO_DESC audio_profile_and_level=\"0x%02x\"/>",
                 desc1c_get_mpeg4_audio_profile_and_level(p_desc));
        break;
    default:
        pf_print(opaque,"    - desc 1c mpeg4_audio audio_profile_and_level=0x%02x",
                 desc1c_get_mpeg4_audio_profile_and_level(p_desc));
    }
}

#ifdef __cplusplus
}
#endif

#endif
