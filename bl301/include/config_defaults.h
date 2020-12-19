/*
 * config_defaults.h - sane defaults for everyone
 *
 * Copyright (c) 2009 Analog Devices Inc.
 *
 * Licensed under the GPL-2 or later.
 */

#ifndef _CONFIG_DEFAULTS_H_
#define _CONFIG_DEFAULTS_H_

/* Support bootm-ing different OSes */
#define CONFIG_BOOTM_LINUX 1
#define CONFIG_BOOTM_NETBSD 1
#define CONFIG_BOOTM_PLAN9 1
#define CONFIG_BOOTM_RTEMS 1
#define CONFIG_BOOTM_VXWORKS 1

#define CONFIG_GZIP 1
#define CONFIG_ZLIB 1
#define CONFIG_PARTITIONS 1

#ifndef CONFIG_SPL_BUILD
#define CONFIG_DM_WARN
#define CONFIG_DM_DEVICE_REMOVE
#define CONFIG_DM_STDIO
#endif

/* other functions */
#define CONFIG_NEED_BL301 1

/*config the default parameters for adc power key*/
#define CONFIG_ADC_POWER_KEY_CHAN   2  /*channel range: 0-7*/
#define CONFIG_ADC_POWER_KEY_VAL    0  /*sample value range: 0-1023*/

/*
 * config CoreELEC config.ini key for wake up
 * user config.ini key value: unsigned int
*/
struct __attribute__ ((aligned (4))) config_value_uint {
  // first unique ID of the variable as hash of the ID string
  unsigned int config_id_a;
  // default value of variable
  unsigned int val;
  // second unique ID of the variable as hash of the ID string
  unsigned int config_id_b;
};

/*
 * config CoreELEC config.ini key for CEC OSD name
 * user config.ini key value: char[14]
*/
struct __attribute__ ((aligned (4))) config_value_char14 {
  // first unique ID of the variable as hash of the ID string
  unsigned int config_id_a;
  // default value of variable
  char val[14];
  // second unique ID of the variable as hash of the ID string
  unsigned int config_id_b;
};

#endif
