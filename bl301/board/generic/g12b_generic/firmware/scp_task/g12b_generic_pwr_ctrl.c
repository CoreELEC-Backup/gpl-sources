
/*
 * board/generic/g12b_generic/firmware/scp_task/g12b_generic_pwr_ctrl.c
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
#ifdef CONFIG_CEC_WAKEUP
#include <cec_tx_reg.h>
#endif
#ifdef CONFIG_GPIO_WAKEUP
#include <gpio_key.h>
#endif

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

extern struct config_value_uint usr_pwr_key;
extern struct config_value_uint usr_ir_proto;
extern struct config_value_uint usr_pwr_key_mask;
extern struct config_value_char14 cec_osd_name;

static void set_vddee_voltage(unsigned int target_voltage)
{
	unsigned int to, pwm_size = 0;
	static int (*pwm_voltage_ee)[2];

	/* BOOT_9 = H use PWM_CFG0(0.67v-0.97v), =L use PWM_CFG1(0.69v-0.89v) */
	/*set BOOT_9 input mode*/
	writel((readl(PREG_PAD_GPIO0_EN_N) | 0x200), PREG_PAD_GPIO0_EN_N);
	if (((readl(PREG_PAD_GPIO0_EN_N) & 0x200 ) == 0x200) &&
			((readl(PREG_PAD_GPIO0_I) & 0x200 ) == 0x0)) {
		uart_puts("use vddee new table!");
		uart_puts("\n");
		pwm_voltage_ee = pwm_voltage_table_ee_new;
		pwm_size = ARRAY_SIZE(pwm_voltage_table_ee_new);
	} else {
		uart_puts("use vddee table!");
		uart_puts("\n");
		pwm_voltage_ee = pwm_voltage_table_ee;
		pwm_size = ARRAY_SIZE(pwm_voltage_table_ee);
	}

	for (to = 0; to < pwm_size; to++) {
		if (pwm_voltage_ee[to][1] >= target_voltage) {
			break;
		}
	}

	if (to >= pwm_size) {
		to = pwm_size - 1;
	}

	writel(*(*(pwm_voltage_ee + to)), AO_PWM_PWM_B);
}

static void power_off_at_24M(unsigned int suspend_from)
{
	if (!enable_5V_system_power.val)
	{
		/*set gpioH_8 low/high to power off vcc 5v*/
		writel(readl(PREG_PAD_GPIO3_EN_N) ^ (1 << 8), PREG_PAD_GPIO3_EN_N);
		writel(readl(PERIPHS_PIN_MUX_C) & (~(0xf)), PERIPHS_PIN_MUX_C);
	}

#ifdef CONFIG_VCCKA_GPIO
	/*set gpioao_4 low to power off vcck_a*/
	writel(readl(AO_GPIO_O) & (~(1 << CONFIG_VCCKA_GPIO)), AO_GPIO_O);
	writel(readl(AO_GPIO_O_EN_N) & (~(1 << CONFIG_VCCKA_GPIO)), AO_GPIO_O_EN_N);
	writel(readl(AO_RTI_PIN_MUX_REG) & (~(0xf << 16)), AO_RTI_PIN_MUX_REG);
#endif

	if (!enable_wol.val) {
		/*set test_n low to power off vcck_b & vcc 3.3v*/
		writel(readl(AO_GPIO_O) & (~(1 << 31)), AO_GPIO_O);
		writel(readl(AO_GPIO_O_EN_N) & (~(1 << 31)), AO_GPIO_O_EN_N);
		writel(readl(AO_RTI_PIN_MUX_REG1) & (~(0xf << 28)), AO_RTI_PIN_MUX_REG1);
	}

	/*step down ee voltage*/
	set_vddee_voltage(CONFIG_VDDEE_SLEEP_VOLTAGE);
}

void power_on_at_24M(unsigned int suspend_from)
{
	/*step up ee voltage*/
	set_vddee_voltage(CONFIG_VDDEE_INIT_VOLTAGE);

	if (!enable_wol.val) {
		/*set test_n high to power on vcck_b & vcc 3.3v*/
		writel(readl(AO_GPIO_O) | (1 << 31), AO_GPIO_O);
		writel(readl(AO_GPIO_O_EN_N) & (~(1 << 31)), AO_GPIO_O_EN_N);
		writel(readl(AO_RTI_PIN_MUX_REG1) & (~(0xf << 28)), AO_RTI_PIN_MUX_REG1);
		_udelay(100);
	}

#ifdef CONFIG_VCCKA_GPIO
	/*set gpioao_4 high to power on vcck_a*/
	writel(readl(AO_GPIO_O) | (1 << 4), AO_GPIO_O);
	writel(readl(AO_GPIO_O_EN_N) & (~(1 << 4)), AO_GPIO_O_EN_N);
	writel(readl(AO_RTI_PIN_MUX_REG) & (~(0xf << 16)), AO_RTI_PIN_MUX_REG);
	_udelay(100);
#endif

	if (!enable_5V_system_power.val)
	{
		/*set gpioH_8 high/low to power on vcc 5v*/
		writel(readl(PREG_PAD_GPIO3_EN_N) ^ (1 << 8), PREG_PAD_GPIO3_EN_N);
		writel(readl(PERIPHS_PIN_MUX_C) & (~(0xf)), PERIPHS_PIN_MUX_C);
	}

	_udelay(10000);
}

static void get_wakeup_source(void *response, unsigned int suspend_from)
{
	struct wakeup_info *p = (struct wakeup_info *)response;
	struct wakeup_gpio_info *gpio;
	unsigned val;
	unsigned i = 0;

	p->status = RESPONSE_OK;
	val = (AUTO_WAKEUP_SRC | REMOTE_WAKEUP_SRC |
			RTC_WAKEUP_SRC | ETH_PHY_GPIO_SRC);

#ifdef CONFIG_CEC_WAKEUP
	val |= CECB_WAKEUP_SRC;
#endif

#ifdef CONFIG_POWER_BUTTON
	val |= POWER_KEY_WAKEUP_SRC;
	gpio = &(p->gpio_info[i]);
	gpio->wakeup_id = POWER_KEY_WAKEUP_SRC;
	gpio->gpio_in_idx = CONFIG_POWER_BUTTON;
	gpio->gpio_in_ao = 1;
	gpio->gpio_out_idx = -1;
	gpio->gpio_out_ao = -1;
	gpio->irq = CONFIG_POWER_BUTTON_IRQ;
	gpio->trig_type = CONFIG_POWER_BUTTON_EDGE;
	p->gpio_info_count = ++i;
#endif

#ifdef CONFIG_BT_WAKEUP
	val |= BT_WAKEUP_SRC;
	gpio = &(p->gpio_info[i]);
	gpio->wakeup_id = BT_WAKEUP_SRC;
	gpio->gpio_in_idx = CONFIG_BT_WAKEUP;
	gpio->gpio_in_ao = 0;
	gpio->gpio_out_idx = -1;
	gpio->gpio_out_ao = -1;
	gpio->irq = CONFIG_BT_WAKEUP_IRQ;
	gpio->trig_type	= CONFIG_BT_WAKEUP_EDGE;
	p->gpio_info_count = ++i;
#endif

#ifdef CONFIG_WOL
	if (enable_wol.val) {
		gpio = &(p->gpio_info[i]);
		gpio->wakeup_id = ETH_PHY_GPIO_SRC;
		gpio->gpio_in_idx = CONFIG_WOL;
		gpio->gpio_in_ao = 0;
		gpio->gpio_out_idx = -1;
		gpio->gpio_out_ao = -1;
		gpio->irq = CONFIG_WOL_IRQ;
		gpio->trig_type = CONFIG_WOL_EDGE;
		p->gpio_info_count = ++i;
	}
#endif

	p->sources = val;
}
extern void __switch_idle_task(void);

static unsigned int detect_key(unsigned int suspend_from)
{
	int exit_reason = 0;
	unsigned *irq = (unsigned *)WAKEUP_SRC_IRQ_ADDR_BASE;

#ifdef CONFIG_GPIO_WAKEUP
	unsigned int is_gpiokey = 0;
#endif

	dbg_print("CoreELEC ir_pwr_key      = ", usr_pwr_key.val);
	dbg_print("CoreELEC usr_ir_proto    = ", usr_ir_proto.val);
	dbg_print("CoreELEC ir_pwr_key_mask = ", usr_pwr_key_mask.val);
	dbg_print("CoreELEC system_power    = ", enable_5V_system_power.val);
	dbg_print("CoreELEC wake_on_lan     = ", enable_wol.val);
	dbg_prints("CoreELEC cec_osd_name    = ");
	dbg_prints(cec_osd_name.val);
	dbg_prints("\n");

	backup_remote_register();
	init_remote();
#ifdef CONFIG_CEC_WAKEUP
		if (hdmi_cec_func_config & 0x1) {
			remote_cec_hw_reset();
			cec_node_init();
		}
#endif

#ifdef CONFIG_GPIO_WAKEUP
	is_gpiokey = init_gpio_key();
#endif

	do {
#ifdef CONFIG_CEC_WAKEUP
		if (!cec_msg.log_addr)
			cec_node_init();
		else {
			if (readl(AO_CECB_INTR_STAT) & CECB_IRQ_RX_EOM) {
				if (cec_power_on_check())
					exit_reason = CEC_WAKEUP;
			}
		}
#endif
		if (irq[IRQ_AO_IR_DEC] == IRQ_AO_IR_DEC_NUM) {
			irq[IRQ_AO_IR_DEC] = 0xFFFFFFFF;
			if (remote_detect_key())
				exit_reason = REMOTE_WAKEUP;
		}

		if (irq[IRQ_VRTC] == IRQ_VRTC_NUM) {
			irq[IRQ_VRTC] = 0xFFFFFFFF;
			exit_reason = RTC_WAKEUP;
		}

#if defined(CONFIG_WOL) || defined(CONFIG_BT_WAKEUP)
		if (enable_wol.val && (irq[IRQ_GPIO1] == CONFIG_WOL_IRQ)) {
			irq[IRQ_GPIO1] = 0xFFFFFFFF;
#ifdef CONFIG_WOL
			if (!(readl(PREG_PAD_GPIO4_I) & (0x01 << CONFIG_WOL))
					&& (readl(PREG_PAD_GPIO4_EN_N) & (0x01 << CONFIG_WOL)))
				exit_reason = ETH_PHY_GPIO;
#endif
#ifdef CONFIG_BT_WAKEUP
			if (!(readl(PREG_PAD_GPIO2_I) & (0x01 << CONFIG_BT_WAKEUP))
				&& (readl(PREG_PAD_GPIO2_O) & (0x01 << 17))
				&& !(readl(PREG_PAD_GPIO2_EN_N) & (0x01 << 17)))
				exit_reason = BT_WAKEUP;
#endif
		}
#endif

#ifdef CONFIG_POWER_BUTTON
		if (irq[IRQ_AO_GPIO0] == CONFIG_POWER_BUTTON_IRQ) {
			irq[IRQ_AO_GPIO0] = 0xFFFFFFFF;
			if ((readl(AO_GPIO_I) & (1<<CONFIG_POWER_BUTTON)) == 0)
				exit_reason = POWER_KEY_WAKEUP;
		}
#endif

		if (irq[IRQ_ETH_PTM] == IRQ_ETH_PMT_NUM) {
			irq[IRQ_ETH_PTM]= 0xFFFFFFFF;
			exit_reason = ETH_PMT_WAKEUP;
		}

#ifdef CONFIG_GPIO_WAKEUP
		if (is_gpiokey) {
			if (gpio_detect_key())
				exit_reason = GPIO_WAKEUP;
		}
#endif
		if (exit_reason)
			break;
		else
			__switch_idle_task();
	} while (1);

	restore_remote_register();

	return exit_reason;
}

static void g12b_generic_pwr_op_init(struct pwr_op *pwr_op)
{
	pwr_op->power_off_at_24M = power_off_at_24M;
	pwr_op->power_on_at_24M = power_on_at_24M;
	pwr_op->detect_key = detect_key;
	pwr_op->get_wakeup_source = get_wakeup_source;
}
