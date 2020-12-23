/*****************************************************************************
 * desc_7a.h: ETSI EN 300 468 Descriptor 0x7a: Enhanced AC-3 descriptor
 *****************************************************************************
 * Copyright (C) 2009-2010 VideoLAN
 *
 * Authors: Christophe Massiot <massiot@via.ecp.fr>
 *          Georgi Chorbadzhiyski <georgi@unixsol.org>
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
 *  - ETSI EN 300 468 V1.11.1 (2010-04) (SI in DVB systems)
 */

#ifndef __BITSTREAM_DVB_DESC_7A_H__
#define __BITSTREAM_DVB_DESC_7A_H__

#include <bitstream/common.h>
#include <bitstream/mpeg/psi/descriptors.h>

#ifdef __cplusplus
extern "C"
{
#endif

/*****************************************************************************
 * Descriptor 0x7a: Enhanced AC-3 descriptor
 *****************************************************************************/
#define DESC7A_HEADER_SIZE      (DESC_HEADER_SIZE + 1)

static inline void desc7a_init(uint8_t *p_desc)
{
    desc_set_tag(p_desc, 0x7a);
    desc_set_length(p_desc, (DESC7A_HEADER_SIZE - DESC_HEADER_SIZE));
    p_desc[2] = 0x00;
}

static inline void desc7a_clear_flags(uint8_t *p_desc)
{
    p_desc[2] = 0x00;
}

#define DEFINE_AC3_FLAG(FLAGNAME, bit) \
    static inline bool desc7a_get_##FLAGNAME##_flag(const uint8_t *p_desc) \
    { \
        return (p_desc[2] & bit) == bit; \
    } \
    \
    static inline void desc7a_set_##FLAGNAME##_flag(uint8_t *p_desc, bool b_##FLAGNAME) \
    { \
        p_desc[2] = b_##FLAGNAME ? (p_desc[2] | bit) : (p_desc[2] &~ bit); \
    }

#define DEFINE_AC3_VALUE(VALUENAME, ofs) \
    static inline uint8_t desc7a_get_##VALUENAME(const uint8_t *p_desc) \
    { \
        if (!desc7a_get_##VALUENAME##_flag(p_desc)) return 0; \
        return p_desc[ofs]; \
    } \
     \
    static inline void desc7a_set_##VALUENAME(uint8_t *p_desc, uint8_t i_##VALUENAME) \
    { \
        if (!desc7a_get_##VALUENAME##_flag(p_desc)) return; \
        p_desc[ofs] = i_##VALUENAME; \
    }

DEFINE_AC3_FLAG (component_type, 0x80)
DEFINE_AC3_VALUE(component_type, DESC7A_HEADER_SIZE)

DEFINE_AC3_FLAG (bsid,   0x40)
DEFINE_AC3_VALUE(bsid,   DESC7A_HEADER_SIZE
                           + desc7a_get_component_type_flag(p_desc))

DEFINE_AC3_FLAG (mainid, 0x20)
DEFINE_AC3_VALUE(mainid, DESC7A_HEADER_SIZE
                           + desc7a_get_component_type_flag(p_desc)
                           + desc7a_get_bsid_flag(p_desc))

DEFINE_AC3_FLAG (asvc,   0x10)
DEFINE_AC3_VALUE(asvc,   DESC7A_HEADER_SIZE
                           + desc7a_get_component_type_flag(p_desc)
                           + desc7a_get_bsid_flag(p_desc)
                           + desc7a_get_mainid_flag(p_desc))

DEFINE_AC3_FLAG (mixinfoexists, 0x08)

DEFINE_AC3_FLAG (substream1, 0x04)
DEFINE_AC3_VALUE(substream1, DESC7A_HEADER_SIZE
                               + desc7a_get_component_type_flag(p_desc)
                               + desc7a_get_bsid_flag(p_desc)
                               + desc7a_get_mainid_flag(p_desc)
                               + desc7a_get_asvc_flag(p_desc))

DEFINE_AC3_FLAG (substream2, 0x02)
DEFINE_AC3_VALUE(substream2, DESC7A_HEADER_SIZE
                               + desc7a_get_component_type_flag(p_desc)
                               + desc7a_get_bsid_flag(p_desc)
                               + desc7a_get_mainid_flag(p_desc)
                               + desc7a_get_asvc_flag(p_desc)
                               + desc7a_get_substream1_flag(p_desc))

DEFINE_AC3_FLAG (substream3, 0x01)
DEFINE_AC3_VALUE(substream3, DESC7A_HEADER_SIZE
                               + desc7a_get_component_type_flag(p_desc)
                               + desc7a_get_bsid_flag(p_desc)
                               + desc7a_get_mainid_flag(p_desc)
                               + desc7a_get_asvc_flag(p_desc)
                               + desc7a_get_substream1_flag(p_desc)
                               + desc7a_get_substream2_flag(p_desc))

#undef DEFINE_AC3_FLAG
#undef DEFINE_AC3_VALUE

static inline void desc7a_set_length(uint8_t *p_desc)
{
    uint8_t i_size = (DESC7A_HEADER_SIZE - DESC_HEADER_SIZE)
                       + desc7a_get_component_type_flag(p_desc)
                       + desc7a_get_bsid_flag(p_desc)
                       + desc7a_get_mainid_flag(p_desc)
                       + desc7a_get_asvc_flag(p_desc)
                       + desc7a_get_substream1_flag(p_desc)
                       + desc7a_get_substream2_flag(p_desc)
                       + desc7a_get_substream3_flag(p_desc);
    return desc_set_length(p_desc, i_size);
}

static inline bool desc7a_validate(const uint8_t *p_desc)
{
    uint8_t i_size = (DESC7A_HEADER_SIZE - DESC_HEADER_SIZE)
                       + desc7a_get_component_type_flag(p_desc)
                       + desc7a_get_bsid_flag(p_desc)
                       + desc7a_get_mainid_flag(p_desc)
                       + desc7a_get_asvc_flag(p_desc)
                       + desc7a_get_substream1_flag(p_desc)
                       + desc7a_get_substream2_flag(p_desc)
                       + desc7a_get_substream3_flag(p_desc);
    return desc_get_length(p_desc) >= i_size;
}

static inline void desc7a_print(const uint8_t *p_desc, f_print pf_print,
                                void *opaque, print_type_t i_print_type)
{
    switch (i_print_type) {
    case PRINT_XML:
        pf_print(opaque,
                 "<AC3_DESC component_type_flag=\"%u\" component_type=\"%u\""
                 " bsid_flag=\"%u\" bsid=\"%u\""
                 " mainid_flag=\"%u\" mainid=\"%u\""
                 " asvc_flag=\"%u\" asvc=\"%u\""
                 " mixinfoexists=\"%u\""
                 " substream1_flag=\"%u\" substream1=\"%u\""
                 " substream2_flag=\"%u\" substream2=\"%u\""
                 " substream3_flag=\"%u\" substream3=\"%u\"/>",
                    desc7a_get_component_type_flag(p_desc), desc7a_get_component_type(p_desc),
                    desc7a_get_bsid_flag(p_desc),   desc7a_get_bsid(p_desc),
                    desc7a_get_mainid_flag(p_desc), desc7a_get_mainid(p_desc),
                    desc7a_get_asvc_flag(p_desc),   desc7a_get_asvc(p_desc),
                    desc7a_get_mixinfoexists_flag(p_desc),
                    desc7a_get_substream1_flag(p_desc), desc7a_get_substream1(p_desc),
                    desc7a_get_substream2_flag(p_desc), desc7a_get_substream2(p_desc),
                    desc7a_get_substream3_flag(p_desc), desc7a_get_substream3(p_desc)
                );
        break;
    default:
        pf_print(opaque,
                 "    - desc 7a ac3 component_type_flag=%u component_type=%u"
                 " bsid_flag=%u bsid=%u"
                 " mainid_flag=%u mainid=%u"
                 " asvc_flag=%u asvc=%u"
                 " mixinfoexists=%u"
                 " substream1_flag=%u substream1=%u"
                 " substream2_flag=%u substream2=%u"
                 " substream3_flag=%u substream3=%u",
                    desc7a_get_component_type_flag(p_desc), desc7a_get_component_type(p_desc),
                    desc7a_get_bsid_flag(p_desc),   desc7a_get_bsid(p_desc),
                    desc7a_get_mainid_flag(p_desc), desc7a_get_mainid(p_desc),
                    desc7a_get_asvc_flag(p_desc),   desc7a_get_asvc(p_desc),
                    desc7a_get_mixinfoexists_flag(p_desc),
                    desc7a_get_substream1_flag(p_desc), desc7a_get_substream1(p_desc),
                    desc7a_get_substream2_flag(p_desc), desc7a_get_substream2(p_desc),
                    desc7a_get_substream3_flag(p_desc), desc7a_get_substream3(p_desc)
                );
    }
}

#ifdef __cplusplus
}
#endif

#endif
