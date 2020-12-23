/*
 *      vdr-plugin-robotv - roboTV server plugin for VDR
 *
 *      Copyright (C) 2016 Alexander Pipelka
 *
 *      https://github.com/pipelka/vdr-plugin-robotv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "parser_h265.h"

// nal_unit_type values from H.265/HEVC (2014) Table 7-1.
#define RASL_R   9
#define BLA_W_LP 16
#define CRA_NUT  21
#define VPS_NUT  32
#define SPS_NUT  33
#define PPS_NUT  34
#define AUD_NUT  35

#define PREFIX_SEI_NUT 39
#define SUFFIX_SEI_NUT 40

ParserH265::ParserH265(TsDemuxer* demuxer) : ParserH264(demuxer) {
}

int ParserH265::parsePayload(unsigned char* data, int length) {
    int o = 0;
    int sps_start = -1;
    int nal_len = 0;

    m_frameType = StreamInfo::FrameType::UNKNOWN;

    if(length < 4) {
        return length;
    }

    // iterate through all NAL units
    while((o = findStartCode(data, length, o, 0x00000001, 0x00FFFFFF)) >= 0) {
        o += 4;

        if(o >= length) {
            return length;
        }

        uint8_t nal_type = (data[o] & 0x7E) >> 1;

        // key frame ?
        if(nal_type >= BLA_W_LP && nal_type <= CRA_NUT) {
            m_frameType = StreamInfo::FrameType::IFRAME;
        }

        // PPS_NUT
        if(nal_type == PPS_NUT && length - o > 1) {
            o++;
            uint8_t* pps_data = extractNal(data, length, o, nal_len);

            if(pps_data != NULL) {
                m_demuxer->setVideoDecoderData(NULL, 0, pps_data, nal_len);
                delete[] pps_data;
            }
        }

        // VPS_NUT
        else if(nal_type == VPS_NUT && length - o > 1) {
            o++;
            uint8_t* vps_data = extractNal(data, length, o, nal_len);

            if(vps_data != NULL) {
                m_demuxer->setVideoDecoderData(NULL, 0, NULL, 0, vps_data, nal_len);
                delete[] vps_data;
            }
        }

        // SPS_NUT
        else if(nal_type == SPS_NUT && length - o > 1) {
            o++;
            sps_start = o;
        }
    }

    // exit if we do not have SPS data
    if(sps_start == -1) {
        return length;
    }

    // extract SPS
    uint8_t* nal_data = extractNal(data, length, sps_start, nal_len);

    if(nal_data == NULL) {
        return length;
    }

    // register SPS data (decoder specific data)
    m_demuxer->setVideoDecoderData(nal_data, nal_len, NULL, 0);

    int width = 0;
    int height = 0;
    pixel_aspect_t pixelaspect = { 1, 1 };

    bool rc = parseSps(nal_data, nal_len, pixelaspect, width, height);
    delete[] nal_data;

    if(!rc) {
        return length;
    }

    double PAR = (double)pixelaspect.num / (double)pixelaspect.den;
    double DAR = (PAR * width) / height;

    m_rate = 50;
    m_scale = 1;

    m_demuxer->setVideoInformation(m_scale, m_rate, height, width, (int)(DAR * 10000));
    return length;
}

void ParserH265::skipScalingList(BitStream& bs) {
    for(int sizeId = 0; sizeId < 4; sizeId++) {
        for(int matrixId = 0; matrixId < 6; matrixId += sizeId == 3 ? 3 : 1) {
            if(!bs.getBit()) {  // scaling_list_pred_mode_flag[sizeId][matrixId]
                // scaling_list_pred_matrix_id_delta[sizeId][matrixId]
                readGolombUe(&bs);
            }
            else {
                int coefNum = std::min(64, 1 << (4 + (sizeId << 1)));

                if(sizeId > 1) {
                    // scaling_list_dc_coef_minus8[sizeId - 2][matrixId]
                    readGolombSe(&bs);
                }

                for(int i = 0; i < coefNum; i++) {
                    readGolombSe(&bs); // scaling_list_delta_coef
                }
            }
        }
    }
}

void ParserH265::skipShortTermRefPicSets(BitStream& bs) {
    int numShortTermRefPicSets = readGolombUe(&bs);
    bool interRefPicSetPredictionFlag = false;
    int numNegativePics = 0;
    int numPositivePics = 0;
    int previousNumDeltaPocs = 0;

    for(int stRpsIdx = 0; stRpsIdx < numShortTermRefPicSets; stRpsIdx++) {
        if(stRpsIdx != 0) {
            interRefPicSetPredictionFlag = bs.getBit();
        }

        if(interRefPicSetPredictionFlag) {
            bs.skipBits(1); // delta_rps_sign
            readGolombUe(&bs); // abs_delta_rps_minus1

            for(int j = 0; j <= previousNumDeltaPocs; j++) {
                if(bs.getBit()) {  // used_by_curr_pic_flag[j]
                    bs.skipBits(1); // use_delta_flag[j]
                }
            }
        }
        else {
            numNegativePics = readGolombUe(&bs);
            numPositivePics = readGolombUe(&bs);
            previousNumDeltaPocs = numNegativePics + numPositivePics;

            for(int i = 0; i < numNegativePics; i++) {
                readGolombUe(&bs); // delta_poc_s0_minus1[i]
                bs.skipBits(1); // used_by_curr_pic_s0_flag[i]
            }

            for(int i = 0; i < numPositivePics; i++) {
                readGolombUe(&bs); // delta_poc_s1_minus1[i]
                bs.skipBits(1); // used_by_curr_pic_s1_flag[i]
            }
        }
    }
}

bool ParserH265::parseSps(uint8_t* buf, int len, pixel_aspect_t& pixelaspect, int& width, int& height) {
    BitStream bs(buf, len * 8);
    bs.skipBits(8 + 4); // NAL header, sps_video_parameter_set_id
    int maxSubLayersMinus1 = bs.getBits(3);
    bs.skipBits(1); // sps_temporal_id_nesting_flag

    // profile_tier_level(1, sps_max_sub_layers_minus1)
    bs.skipBits(88); // if (profilePresentFlag) {...}
    bs.skipBits(8); // general_level_idc
    int toSkip = 0;

    for(int i = 0; i < maxSubLayersMinus1; i++) {
        if(bs.getBits(1) == 1) {  // sub_layer_profile_present_flag[i]
            toSkip += 89;
        }

        if(bs.getBits(1) == 1) {  // sub_layer_level_present_flag[i]
            toSkip += 8;
        }
    }

    bs.skipBits(toSkip);

    if(maxSubLayersMinus1 > 0) {
        bs.skipBits(2 * (8 - maxSubLayersMinus1));
    }

    readGolombUe(&bs); // sps_seq_parameter_set_id
    int chromaFormatIdc = readGolombUe(&bs);

    if(chromaFormatIdc == 3) {
        bs.skipBits(1); // separate_colour_plane_flag
    }

    width = readGolombUe(&bs);
    height = readGolombUe(&bs);

    if(bs.getBit()) {  // conformance_window_flag
        int confWinLeftOffset = readGolombUe(&bs);
        int confWinRightOffset = readGolombUe(&bs);
        int confWinTopOffset = readGolombUe(&bs);
        int confWinBottomOffset = readGolombUe(&bs);
        // H.265/HEVC (2014) Table 6-1
        int subWidthC = chromaFormatIdc == 1 || chromaFormatIdc == 2 ? 2 : 1;
        int subHeightC = chromaFormatIdc == 1 ? 2 : 1;
        width -= subWidthC * (confWinLeftOffset + confWinRightOffset);
        height -= subHeightC * (confWinTopOffset + confWinBottomOffset);
    }

    readGolombUe(&bs); // bit_depth_luma_minus8
    readGolombUe(&bs); // bit_depth_chroma_minus8
    int log2MaxPicOrderCntLsbMinus4 = readGolombUe(&bs);

    for(int i = bs.getBit() ? 0 : maxSubLayersMinus1; i <= maxSubLayersMinus1; i++) {
        readGolombUe(&bs); // sps_max_dec_pic_buffering_minus1[i]
        readGolombUe(&bs); // sps_max_num_reorder_pics[i]
        readGolombUe(&bs); // sps_max_latency_increase_plus1[i]
    }

    readGolombUe(&bs); // log2_min_luma_coding_block_size_minus3
    readGolombUe(&bs); // log2_diff_max_min_luma_coding_block_size
    readGolombUe(&bs); // log2_min_luma_transform_block_size_minus2
    readGolombUe(&bs); // log2_diff_max_min_luma_transform_block_size
    readGolombUe(&bs); // max_transform_hierarchy_depth_inter
    readGolombUe(&bs); // max_transform_hierarchy_depth_intra

    // if (scaling_list_enabled_flag) { if (sps_scaling_list_data_present_flag) {...}}
    if(bs.getBit() && bs.getBit()) {
        skipScalingList(bs);
    }

    bs.skipBits(2); // amp_enabled_flag (1), sample_adaptive_offset_enabled_flag (1)

    if(bs.getBit()) {  // pcm_enabled_flag
        // pcm_sample_bit_depth_luma_minus1 (4), pcm_sample_bit_depth_chroma_minus1 (4)
        bs.skipBits(8);
        readGolombUe(&bs); // log2_min_pcm_luma_coding_block_size_minus3
        readGolombUe(&bs); // log2_diff_max_min_pcm_luma_coding_block_size
        bs.skipBits(1); // pcm_loop_filter_disabled_flag
    }

    // Skips all short term reference picture sets.
    skipShortTermRefPicSets(bs);

    if(bs.getBit()) {  // long_term_ref_pics_present_flag
        // num_long_term_ref_pics_sps
        for(uint32_t i = 0; i < readGolombUe(&bs); i++) {
            int ltRefPicPocLsbSpsLength = log2MaxPicOrderCntLsbMinus4 + 4;
            // lt_ref_pic_poc_lsb_sps[i], used_by_curr_pic_lt_sps_flag[i]
            bs.skipBits(ltRefPicPocLsbSpsLength + 1);
        }
    }

    bs.skipBits(2); // sps_temporal_mvp_enabled_flag, strong_intra_smoothing_enabled_flag

    pixelaspect.num = 1;
    pixelaspect.den = 1;

    if(bs.getBit()) {  // vui_parameters_present_flag
        if(bs.getBit()) {  // aspect_ratio_info_present_flag
            unsigned int aspect_ratio_idc = bs.getBits(8);

            if(aspect_ratio_idc == 255) {
                pixelaspect.num = bs.getBits(16);
                pixelaspect.den = bs.getBits(16);
            }
            else if(aspect_ratio_idc < sizeof(m_aspect_ratios) / sizeof(pixel_aspect_t)) {
                pixelaspect = m_aspect_ratios[aspect_ratio_idc];
            }
        }
    }

    return true;
}
