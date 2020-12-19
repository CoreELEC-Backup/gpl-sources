// SPDX-License-Identifier: GPL-2.0-or-later
// Copyright (C) 2018-present Team CoreELEC (https://coreelec.org)

#ifndef _DVB_REG_H_
#define _DVB_REG_H_

#include <linux/amlogic/iomap.h>

#include <linux/amlogic/cpu_version.h>

#define ID_STB_CBUS_BASE		0
#define ID_SMARTCARD_REG_BASE		1
#define ID_ASYNC_FIFO_REG_BASE		2
#define ID_ASYNC_FIFO2_REG_BASE	3
#define ID_RESET_BASE			4
#define ID_PARSER_SUB_START_PTR_BASE	5

long aml_stb_get_base(int id);
#include "c_stb_define.h"
#include "c_stb_regs_define.h"
extern int aml_read_cbus(unsigned int reg);
extern void aml_write_cbus(unsigned int reg, unsigned int val);
extern int aml_read_vcbus(unsigned int reg);
extern void aml_write_vcbus(unsigned int reg, unsigned int val);

#define WRITE_MPEG_REG(_r, _v)   aml_write_cbus(_r, _v)
#define READ_MPEG_REG(_r)        aml_read_cbus(_r)

#define WRITE_CBUS_REG(_r, _v)   aml_write_cbus(_r, _v)
#define READ_CBUS_REG(_r)        aml_read_cbus(_r)

#define WRITE_VCBUS_REG(_r, _v)  aml_write_vcbus(_r, _v)
#define READ_VCBUS_REG(_r)       aml_read_vcbus(_r)

#endif

