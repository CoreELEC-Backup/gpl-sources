/*
 * arch/arm/cpu/armv8/gxl/firmware/scp_task/gpio_key.c
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

#include <gpio_key.h>
#include <aml_gpio.h>

static unsigned int g_bankindex;

unsigned int gpio_wakeup_keyno = 0;
struct meson_bank gpio_bank = { 0 };

static struct meson_bank meson_gxl_periphs_banks[] = {
	/* name  first  last  offset  in */
	BANK_PERIPHS("X",    GPIOX_0,    GPIOX_18,  0,  0x1a),
	BANK_PERIPHS("DV",   GPIODV_24,  GPIODV_29, 0,  0x0e),
	BANK_PERIPHS("H",    GPIOH_0,    GPIOH_9,   20, 0x11),
	BANK_PERIPHS("BOOT", BOOT_0,     BOOT_15,   0,  0x14),
	BANK_PERIPHS("CARD", CARD_0,     CARD_6,    20, 0x14),
	BANK_PERIPHS("CLK",  GPIOCLK_0,  GPIOCLK_1, 28, 0x17),
	BANK_PERIPHS("Z",    GPIOZ_14,   GPIOZ_15,  0,  0x17)
};

static struct meson_bank meson_gxl_aobus_banks[] = {
	/* name  first  last  offset  in */
	BANK_AOBUS("AO",    GPIOAO_0,    GPIOAO_9,    0, 0x0a),
	BANK_AOBUS("TESTN", GPIO_TEST_N, GPIO_TEST_N, 0, 0x0a)
};

int bank_match(unsigned int min, unsigned int max)
{
		return ((gpio_wakeup_keyno >= min) && (gpio_wakeup_keyno < max));
}

int init_gpio_key(void)
{
	if (!gpio_wakeup_keyno || (gpio_wakeup_bank == UNKNOWN))
		return 0;

	g_bankindex = 0xFF;

	switch (gpio_wakeup_bank) {
		case PERIPHS:
			else if (bank_match(GPIOX_0, GPIOX_18))
				g_bankindex = 0;
			else if (bank_match(GPIODV_24, GPIODV_29))
				g_bankindex = 1;
			else if (bank_match(GPIOH_0, GPIOH_9))
				g_bankindex = 2;
			else if (bank_match(BOOT_0, BOOT_15))
				g_bankindex = 3;
			else if (bank_match(CARD_0, CARD_6))
				g_bankindex = 4;
			else if (bank_match(GPIOCLK_0, GPIOCLK_1))
				g_bankindex = 5;
			else if (bank_match(GPIOZ_14, GPIOZ_15))
				g_bankindex = 6;
			break;
		case AOBUS:
			if (bank_match(GPIOAO_0, GPIOAO_9))
				g_bankindex = 0;
			else if (bank_match(GPIO_TEST_N, GPIO_TEST_N))
				g_bankindex = 1;
			break;
	}

	if (g_bankindex == 0xFF) {
		wait_uart_empty();
		uart_puts("WAKEUP GPIO FAIL - invalid key: 0x");
		uart_put_hex(gpio_wakeup_keyno, 16);
		uart_puts(", bank: 0x");
		uart_put_hex(gpio_wakeup_bank, 8);
		uart_puts("\n");
		wait_uart_empty();
		return 0;
	}

	switch (gpio_wakeup_bank) {
		case PERIPHS:
			gpio_bank = meson_gxl_periphs_banks[g_bankindex];
			break;
		case AOBUS:
			gpio_bank = meson_gxl_aobus_banks[g_bankindex];
			break;
	}

	return 1;
}

int gpio_debounce(unsigned int *gpio_level)
{
	unsigned int count = 0;
	unsigned int current_gpio_level;
	unsigned int reg, bit;

	reg = gpio_bank.regs[REG_IN].reg;
	bit = gpio_wakeup_keyno - gpio_bank.first + gpio_bank.offset;

	current_gpio_level = readl(reg) & (1 << bit);

	while (1) {
		_udelay(1000);

		if ((readl(reg) & (1 << bit)) == current_gpio_level)
			count++;
		else
			return 0;

		if (count == 20) {
			*gpio_level = current_gpio_level >> bit;
			return 1;
		}
	}

	return 0;
}

unsigned int last_gpio_level = 0xFF;

int gpio_detect_key(void)
{
	unsigned int new_gpio_level;

	gpio_debounce(&new_gpio_level);

	if (last_gpio_level == 0xFF)
		last_gpio_level = new_gpio_level;
	else if (new_gpio_level != last_gpio_level)
		return 1;

	return 0;
}
