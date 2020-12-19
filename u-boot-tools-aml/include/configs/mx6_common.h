/*
 * Copyright (c) 2013, NVIDIA CORPORATION.  All rights reserved.
 *
 * SPDX-License-Identifier:	GPL-2.0
 */

#ifndef __MX6_COMMON_H
#define __MX6_COMMON_H

#ifndef CONFIG_MX6UL
#define CONFIG_ARM_ERRATA_743622
#define CONFIG_ARM_ERRATA_751472
#define CONFIG_ARM_ERRATA_794072
#define CONFIG_ARM_ERRATA_761320

#ifndef CONFIG_SYS_L2CACHE_OFF
#define CONFIG_SYS_L2_PL310
#define CONFIG_SYS_PL310_BASE	L2_PL310_BASE
#endif

#define CONFIG_MP
#endif
#define CONFIG_BOARD_POSTCLK_INIT
#define CONFIG_MXC_GPT_HCLK

#define CONFIG_SYS_NO_FLASH

#define CONFIG_SYS_BOOTM_LEN	0x1000000

#include <linux/sizes.h>
#include <asm/arch/imx-regs.h>
#include <asm/imx-common/gpio.h>

#ifndef CONFIG_MX6
#define CONFIG_MX6
#endif

#define CONFIG_DISPLAY_BOARDINFO
#define CONFIG_DISPLAY_CPUINFO
#define CONFIG_SYS_FSL_CLK

/* ATAGs */
#define CONFIG_CMDLINE_TAG
#define CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG
#define CONFIG_REVISION_TAG

/* Boot options */
#if (defined(CONFIG_MX6SX) || defined(CONFIG_MX6SL) || defined(CONFIG_MX6UL))
#define CONFIG_LOADADDR		0x82000000
#ifndef CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_TEXT_BASE	0x87800000
#endif
#else
#define CONFIG_LOADADDR		0x12000000
#ifndef CONFIG_SYS_TEXT_BASE
#define CONFIG_SYS_TEXT_BASE	0x17800000
#endif
#endif
#define CONFIG_SYS_LOAD_ADDR	CONFIG_LOADADDR

#ifndef CONFIG_BOOTDELAY
#define CONFIG_BOOTDELAY	3
#endif

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE
#define CONFIG_CONS_INDEX       1
#define CONFIG_BAUDRATE         115200

/* Filesystems and image support */
#define CONFIG_OF_LIBFDT
#define CONFIG_CMD_BOOTZ
#define CONFIG_SUPPORT_RAW_INITRD
#define CONFIG_CMD_FS_GENERIC
#define CONFIG_DOS_PARTITION
#define CONFIG_CMD_EXT2
#define CONFIG_CMD_EXT4
#define CONFIG_CMD_EXT4_WRITE
#define CONFIG_CMD_FAT

/* Miscellaneous configurable options */
#undef CONFIG_CMD_IMLS
#define CONFIG_SYS_LONGHELP
#define CONFIG_SYS_HUSH_PARSER
#define CONFIG_CMDLINE_EDITING
#define CONFIG_AUTO_COMPLETE
#define CONFIG_SYS_CBSIZE	512
#define CONFIG_SYS_MAXARGS	32
#define CONFIG_SYS_BARGSIZE	CONFIG_SYS_CBSIZE

#ifndef CONFIG_SYS_DCACHE_OFF
#define CONFIG_CMD_CACHE
#endif

/* GPIO */
#define CONFIG_MXC_GPIO

/* MMC */
#define CONFIG_MMC
#define CONFIG_CMD_MMC
#define CONFIG_GENERIC_MMC
#define CONFIG_BOUNCE_BUFFER
#define CONFIG_FSL_ESDHC
#define CONFIG_FSL_USDHC

/* Fuses */
#define CONFIG_CMD_FUSE
#define CONFIG_MXC_OCOTP

#endif
