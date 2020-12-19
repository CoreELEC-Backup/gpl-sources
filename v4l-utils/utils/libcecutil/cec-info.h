/* SPDX-License-Identifier: LGPL-2.1-only */
/*
 * CEC common helper functions
 *
 * Copyright 2017 Cisco Systems, Inc. and/or its affiliates. All rights reserved.
 */

#ifndef _CEC_INFO_H_
#define _CEC_INFO_H_

#include <linux/cec.h>

#define cec_phys_addr_exp(pa) \
        ((pa) >> 12), ((pa) >> 8) & 0xf, ((pa) >> 4) & 0xf, (pa) & 0xf

const char *cec_opcode2s(unsigned opcode);
const char *cec_cdc_opcode2s(unsigned cdc_opcode);
const char *cec_htng_opcode2s(unsigned htng_opcode);
const char *cec_la2s(unsigned la);
const char *cec_la_type2s(unsigned type);
const char *cec_prim_type2s(unsigned type);
const char *cec_version2s(unsigned version);
const char *cec_vendor2s(unsigned vendor);
std::string cec_all_dev_types2s(unsigned types);
std::string cec_rc_src_prof2s(unsigned prof, const std::string &prefix);
std::string cec_dev_feat2s(unsigned feat, const std::string &prefix);
std::string cec_status2s(const struct cec_msg &msg);

void cec_driver_info(const struct cec_caps &caps,
		     const struct cec_log_addrs &laddrs, __u16 phys_addr,
		     const struct cec_connector_info &conn_info);

std::string cec_device_find(const char *driver, const char *adapter);

#endif
