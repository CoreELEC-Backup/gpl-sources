
/*
 * board/khadas/kvim3/firmware/scp_task/pwr_ctrl.c
 *
 * Copyright (C) 2018 Amlogic, Inc. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <gpio.h>
#include "g12b_generic_pwm_ctrl.h"
#ifdef CONFIG_GPIO_WAKEUP
#include <gpio_key.h>
#endif

#include <g12b_generic_pwr_ctrl.c>

static void set_gpio_level(int pin, int high)
{
	//pin 0: sda 1: scl
	if (pin == 1)
	{
		if (high == 1)
		{
			writel(readl(AO_GPIO_O) | (1 << 2), AO_GPIO_O);
			writel(readl(AO_GPIO_O_EN_N) & (~(1 << 2)), AO_GPIO_O_EN_N);
		}
		else
		{
			writel(readl(AO_GPIO_O) & (~(1 << 2)), AO_GPIO_O);
			writel(readl(AO_GPIO_O_EN_N) & (~(1 << 2)), AO_GPIO_O_EN_N);
		}
	writel(readl(AO_RTI_PIN_MUX_REG) & (~(0xf << 8)), AO_RTI_PIN_MUX_REG);
	}
	else

	{
		if (high == 1)
		{
			writel(readl(AO_GPIO_O) | (1 << 3), AO_GPIO_O);
			writel(readl(AO_GPIO_O_EN_N) & (~(1 << 3)), AO_GPIO_O_EN_N);
		}
		else
		{
			writel(readl(AO_GPIO_O) & (~(1 << 3)), AO_GPIO_O);
			writel(readl(AO_GPIO_O_EN_N) & (~(1 << 3)), AO_GPIO_O_EN_N);
		}
	writel(readl(AO_RTI_PIN_MUX_REG) & (~(0xf << 12)), AO_RTI_PIN_MUX_REG);
	}
}

static void i2c_start(void)
{
	set_gpio_level(0, 1); //sda high
	_udelay(1);
	set_gpio_level(1, 1); //scl high
	_udelay(1);
	set_gpio_level(0, 0); //sda low
	_udelay(1);
}

static void i2c_stop(void)
{
	set_gpio_level(1, 0); //scl low
	_udelay(2);
	set_gpio_level(1, 1); //scl high
	_udelay(2);
	set_gpio_level(0, 0); //sda low
	_udelay(2);
	set_gpio_level(0, 1); //sda high
	_udelay(1);
}

static void i2c_send(unsigned char data)
{
	unsigned char i = 0;
	for(; i < 8 ; i++)
	{
		set_gpio_level(1, 0); //scl low
		_udelay(1);
		if (data & 0x80)
			set_gpio_level(0, 1); //sda high
		else
			set_gpio_level(0, 0); //sda low
		data <<= 1;
		_udelay(1);
		set_gpio_level(1, 1); //scl high
		_udelay(1);
	}
	_udelay(3);
	set_gpio_level(1, 0);
	_udelay(2);
	set_gpio_level(1, 1);
	_udelay(3);
}

static void power_on_at_mcu(void)
{
	// switch off system led
	i2c_start();
	i2c_send(0x18 << 1);
	i2c_send(0x28);
	i2c_send(0x00);
	i2c_stop();
}

static void power_off_at_mcu(unsigned int shutdown)
{
	// switch system led to breath mode
	if (shutdown == SYS_SUSPEND) {
		i2c_start();
		i2c_send(0x18 << 1);
		i2c_send(0x28);
		i2c_send(0x02);
		i2c_stop();
	// switch off system led
	} else if (shutdown == SYS_POWEROFF) {
		i2c_start();
		i2c_send(0x18 << 1);
		i2c_send(0x28);
		i2c_send(0x00);
		i2c_stop();
	}
}

static void pwr_op_init(struct pwr_op *pwr_op)
{
	g12b_generic_pwr_op_init(pwr_op);
	pwr_op->power_on_at_mcu = power_on_at_mcu;
	pwr_op->power_off_at_mcu = power_off_at_mcu;
}
