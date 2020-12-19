
/*
 * arch/arm/cpu/armv8/gxl/firmware/scp_task/coreelec_config.c
 *
 * Copyright (C) 2020 CoreELEC All rights reserved.
 *
*/

#include <coreelec_config.h>

/*
 * HASH(s,n) entries will be replaced automatic by makefile
 * s: unique ID string to identify the variable in binary
 * n: unique config.ini key to use for this variable
 * Both strings are without '"'!
*/
#define HASH(s,n) HASHSTR_ ## s

/* customize CEC OSD name, max length 14, no null terminator, set by inject_bl301 from config.ini */
struct config_value_char14 cec_osd_name = {
	.config_id_a = HASH(config_cec_osd_name, cec_osd_name),
	.config_id_b = HASH(config_cec_osd_name, cec_osd_name),
	.val = CONFIG_CEC_OSD_NAME
};

#if defined(CONFIG_WOL) || defined(CONFIG_BT_WAKEUP)
/* Wake-On-Lan set by inject_bl301 from config.ini */
struct config_value_uint enable_wol = {
	.config_id_a = HASH(config_enable_wol, wol),
	.config_id_b = HASH(config_enable_wol, wol),
	.val = 0
};
#endif

/* user ir wake up code set by inject_bl301 from config.ini */
struct config_value_uint usr_pwr_key = {
	.config_id_a = HASH(config_usr_pwr_key, remotewakeup),
	.config_id_b = HASH(config_usr_pwr_key, remotewakeup),
	.val = 0xffffffff
};

/* user ir wake up protocol set by inject_bl301 from config.ini */
struct config_value_uint usr_ir_proto = {
	.config_id_a = HASH(config_usr_ir_proto, decode_type),
	.config_id_b = HASH(config_usr_ir_proto, decode_type),
	.val = 0
};

/* user ir wake up ir code mask set by inject_bl301 from config.ini */
struct config_value_uint usr_pwr_key_mask = {
	.config_id_a = HASH(config_usr_pwr_key_mask, remotewakeupmask),
	.config_id_b = HASH(config_usr_pwr_key_mask, remotewakeupmask),
	.val = 0xffffffff
};
