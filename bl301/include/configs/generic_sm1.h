/*
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

#ifndef __GENERIC_SM1_H__
#define __GENERIC_SM1_H__

#include <asm/arch/cpu.h>

/*
 * platform power init config
 */
#define CONFIG_VCCK_INIT_VOLTAGE	800		// VCCK power up voltage
#define CONFIG_VDDEE_INIT_VOLTAGE	800		// VDDEE power up voltage
#define CONFIG_VDDEE_SLEEP_VOLTAGE	770		// VDDEE suspend voltage

/* configs for CEC */
#define CONFIG_CEC_WAKEUP
#define CONFIG_CEC_OSD_NAME		"CoreELEC"

/*if use power button wakeup, open it*/
/* Power Key: AO_GPIO[3]*/
#define CONFIG_POWER_BUTTON GPIOAO_3
#define CONFIG_POWER_BUTTON_IRQ IRQ_AO_GPIO0_NUM
#define CONFIG_POWER_BUTTON_EDGE GPIO_IRQ_FALLING_EDGE

#define CONFIG_GPIO_WAKEUP

/*if use bt-wakeup, open it*/
/* Bluetooth: GPIOX_18 */
//#define CONFIG_BT_WAKEUP GPIOX_18
//#define CONFIG_BT_WAKEUP_IRQ IRQ_GPIO1_NUM
//#define CONFIG_BT_WAKEUP_EDGE GPIO_IRQ_FALLING_EDGE

/*if use wol wakeup, open it*/
/* Ethernet: GPIOZ_14 */
#define CONFIG_WOL GPIOZ_14
#define CONFIG_WOL_IRQ IRQ_GPIO1_NUM
#define CONFIG_WOL_EDGE GPIO_IRQ_FALLING_EDGE

/*if device have a vcck_a power enable, open it*/
/* vcck_a: GPIOAO_4*/
//#define CONFIG_VCCKA_GPIO GPIOAO_4

//Enable ir remote wake up for bl30
//#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL1 0xFFFFFFFF
//#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL2 0xFFFFFFFF
//#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL3 0xFFFFFFFF
//#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL4 0xFFFFFFFF
//#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL5 0xFFFFFFFF
//#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL6 0xFFFFFFFF
//#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL7 0xFFFFFFFF
//#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL8 0xFFFFFFFF
//#define CONFIG_IR_REMOTE_POWER_UP_KEY_VAL9 0xFFFFFFFF

#endif // __GENERIC_SM1_H__
