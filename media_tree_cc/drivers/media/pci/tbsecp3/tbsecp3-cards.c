/*
    TBS ECP3 FPGA based cards PCIe driver

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "tbsecp3.h"

struct tbsecp3_board tbsecp3_boards[] = {
	[TBSECP3_BOARD_TBS6205] = {
		.board_id	= TBSECP3_BOARD_TBS6205,
		.name		= "TurboSight TBS 6205 (Quad DVB-T/T2/C)",
		.eeprom_i2c	= 1,
		.adapters	= 4,
		.adap_config	= {
			{
				.ts_in = 0,
				.i2c_bus_nr = 0,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(0, 0),
			}, 
			{
				.ts_in = 1,
				.i2c_bus_nr = 1,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(2, 0),
			},
			{
				.ts_in = 2,
				.i2c_bus_nr = 2,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(1, 0),
			},
			{
				.ts_in = 3,
				.i2c_bus_nr = 3,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(3, 0),
			}
		}
	},
	[TBSECP3_BOARD_TBS6281SE] = {
		.board_id	= TBSECP3_BOARD_TBS6281SE,
		.name		= "TurboSight TBS 6281SE (Dual DVB-T/T2/C)",
		.eeprom_i2c	= 1,
		.adapters	= 2,
		.adap_config	= {
			{
				.ts_in = 1,
				.i2c_bus_nr = 1,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(0, 0),
			}, 
			{
				.ts_in = 2,
				.i2c_bus_nr = 2,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(2, 0),
			},
		}
	},	
	[TBSECP3_BOARD_TBS6290SE] = {
		.board_id	= TBSECP3_BOARD_TBS6290SE,
		.name		= "TurboSight TBS 6290SE (Dual DVB-T/T2/C + 2xCI) ",
		.eeprom_i2c	= 1,
		.adapters	= 2,
		.adap_config	= {
			{
				.ts_in = 0,
				.i2c_bus_nr = 0,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_NONE,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(0, 0),

			}, 
			{
				.ts_in = 1,
				.i2c_bus_nr = 1,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_NONE,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(2, 0),

			},
		}
	},	
	[TBSECP3_BOARD_TBS6209] = {
		.board_id	= TBSECP3_BOARD_TBS6209,
		.name		= "Turbosight TBS 6209 (Octa DVB-T/T2/C2/C(j83-a/b/c) ISDB-T)",
		.eeprom_i2c	= 0,
		.adapters	= 8,
		.adap_config ={
			{
				.ts_in = 0,
				.i2c_bus_nr = 0,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(0, 0),
			}, 
			{
				.ts_in = 1,
				.i2c_bus_nr = 0,
			},
			{
				.ts_in = 2,
				.i2c_bus_nr = 1,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(1, 0),
			},
			{
				.ts_in = 3,
				.i2c_bus_nr = 1,
			},
			{
				.ts_in = 4,
				.i2c_bus_nr = 2,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(2, 0),
				
			},
			{
				.ts_in = 5,
				.i2c_bus_nr = 2,
			},
			{
				.ts_in = 6,
				.i2c_bus_nr = 3,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(3, 0),
				
			},
			{
				.ts_in = 7,
				.i2c_bus_nr = 3,
			}
		
		}
	},
	[TBSECP3_BOARD_TBS6522] = {
		.board_id	= TBSECP3_BOARD_TBS6522,
		.name		= "TurboSight TBS 6522 (Dual MultiStandard)",
		.adapters	= 2,
		.eeprom_i2c	= 0,
		.adap_config	= {
			{
				.ts_in = 1,
				.i2c_bus_nr = 0,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(0, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(0, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(0, 1),
			},
			{
				.ts_in = 0,
				.i2c_bus_nr = 0,
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(1, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(1, 1),
			},
		}
	},
	[TBSECP3_BOARD_TBS6528] = {
		.board_id	= TBSECP3_BOARD_TBS6528,
		.name		= "TurboSight TBS 6528 MultiStandard + CI ",
		.adapters	= 1,
		.eeprom_i2c	= 0,
		.adap_config	= {
			{
				.ts_in = 0,
				.i2c_bus_nr = 0,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(0, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(0, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(0, 1),
			},
		}
	},	
	[TBSECP3_BOARD_TBS6590] = {
		.board_id	= TBSECP3_BOARD_TBS6590,
		.name		= "TurboSight TBS 6590 Dual MultiStandard + 2xCI ",
		.adapters	= 2,
		.eeprom_i2c	= 0,
		.adap_config	= {
			{
				.ts_in = 0,
				.i2c_bus_nr = 0,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(0, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(0, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(0, 1),
			},
			{
				.ts_in = 1,
				.i2c_bus_nr = 0,
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(1, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(1, 1),
			},
		}
	},
	[TBSECP3_BOARD_TBS6902] = {
		.board_id	= TBSECP3_BOARD_TBS6902,
		.name		= "TurboSight TBS 6902 (Dual DVB-S/S2)",
		.adapters	= 2,
		.eeprom_i2c	= 1,
		.adap_config	= {
			{
				.ts_in = 1,
				.i2c_bus_nr = 1,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(1, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(1, 2),
			},	
			{
				.ts_in = 0,
				.i2c_bus_nr = 0,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(0, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(0, 2),
			}
			
		}
	},
	[TBSECP3_BOARD_TBS6903] = {
		.board_id	= TBSECP3_BOARD_TBS6903,
		.name		= "TurboSight TBS 6903 (Dual DVB-S/S2)",
		.adapters	= 2,
		.eeprom_i2c	= 1,
		.adap_config	= {
			{
				.ts_in = 0,
				.i2c_bus_nr = 0,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(0, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(0, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(0, 1),
			}, 
			{
				.ts_in = 1,
				.i2c_bus_nr = 0,
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(1, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(1, 1),
			},
		}
	},
	[TBSECP3_BOARD_TBS6904] = {
		.board_id	= TBSECP3_BOARD_TBS6904,
		.name		= "TurboSight TBS 6904 (Quad DVB-S/S2)",
		.adapters	= 4,
		.eeprom_i2c	= 1,
		.adap_config	= {
			{
				.ts_in = 3,
				.i2c_bus_nr = 3,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(3, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(3, 2),
			},
			{
				.ts_in = 2,
				.i2c_bus_nr = 2,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(2, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(2, 2),
			},
			{
				.ts_in = 1,
				.i2c_bus_nr = 1,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(1, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(1, 2),
			},
			{
				.ts_in = 0,
				.i2c_bus_nr = 0,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(0, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(0, 2),
			}, 
		}
	},
	[TBSECP3_BOARD_TBS6905] = {
		.board_id	= TBSECP3_BOARD_TBS6905,
		.name		= "TurboSight TBS 6905 (Quad DVB-S/S2)",
		.adapters	= 4,
		.eeprom_i2c	= 2,
		.adap_config	= {
			{
				.ts_in = 2,
				.i2c_bus_nr = 2,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(2, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(2, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(2, 1),
			},
			{
				.ts_in = 3,
				.i2c_bus_nr = 2,
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(3, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(3, 1),
			},
			{
				.ts_in = 0,
				.i2c_bus_nr = 0,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(0, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(0, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(0, 1),
			},
			{
				.ts_in = 1,
				.i2c_bus_nr = 0,
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(1, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(1, 1),
			},
		}
	},
	[TBSECP3_BOARD_TBS6908] = {
		.board_id	= TBSECP3_BOARD_TBS6908,
		.name		= "TurboSight TBS 6908 (Quad DVB-S/S2)",
		.adapters	= 4,
		.eeprom_i2c	= 2,
		.adap_config	= {
			{
				.ts_in = 2,
				.i2c_bus_nr = 2,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(2, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(2, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(2, 1),
			},
			{
				.ts_in = 3,
				.i2c_bus_nr = 2,
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(3, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(3, 1),
			},
			{
				.ts_in = 0,
				.i2c_bus_nr = 0,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(0, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(0, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(0, 1),
			},
			{
				.ts_in = 1,
				.i2c_bus_nr = 0,
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(1, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(1, 1),
			},
		}
	},
	[TBSECP3_BOARD_TBS6909] = {
		.board_id	= TBSECP3_BOARD_TBS6909,
		.name		= "TurboSight TBS 6909 (Octa DVB-S/S2)",
		.adapters	= 8,
		.eeprom_i2c	= 0,
		.eeprom_addr	= 0x10,
		.adap_config	= {
			{
				.ts_in = 0,
				.i2c_bus_nr = 0,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(0, 0),
			}, 
			{
				.ts_in = 1,
				.i2c_bus_nr = 0,
			},
			{
				.ts_in = 2,
				.i2c_bus_nr = 0,
			},
			{
				.ts_in = 3,
				.i2c_bus_nr = 0,
			},
			{
				.ts_in = 4,
				.i2c_bus_nr = 0,
			},
			{
				.ts_in = 5,
				.i2c_bus_nr = 0,
			},
			{
				.ts_in = 6,
				.i2c_bus_nr = 0,
			},
			{
				.ts_in = 7,
				.i2c_bus_nr = 0,
			}
		}
	},
	[TBSECP3_BOARD_TBS6910] = {
		.board_id	= TBSECP3_BOARD_TBS6910,
		.name		= "TurboSight TBS 6910 (Dual DVB-S/S2 + 2xCI)",
		.adapters	= 2,
		.eeprom_i2c	= 1,
		.adap_config	= {
			{
				.ts_in = 0,
				.i2c_bus_nr = 0,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(0, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(0, 2),
			},
			{
				.ts_in = 1,
				.i2c_bus_nr = 1,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(1, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(1, 2),
			},
		}
	},
	[TBSECP3_BOARD_TBS6909X] = {
		.board_id	= TBSECP3_BOARD_TBS6909X,
		.name		= "TurboSight TBS 6909x (Octa DVB-S/S2/S2X)",
		.adapters	= 8,
		.eeprom_i2c	= 0,
		.eeprom_addr	= 0x10,
		.adap_config	= {
			{
				.ts_in = 0,
				.i2c_bus_nr = 0,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(0, 0),
			}, 
			{
				.ts_in = 1,
				.i2c_bus_nr = 0,
			},
			{
				.ts_in = 2,
				.i2c_bus_nr = 0,
			},
			{
				.ts_in = 3,
				.i2c_bus_nr = 0,
			},
			{
				.ts_in = 4,
				.i2c_bus_nr = 0,
			},
			{
				.ts_in = 5,
				.i2c_bus_nr = 0,
			},
			{
				.ts_in = 6,
				.i2c_bus_nr = 0,
			},
			{
				.ts_in = 7,
				.i2c_bus_nr = 0,
			}
		}
	},
	[TBSECP3_BOARD_TBS6903X] = {
		.board_id	= TBSECP3_BOARD_TBS6903X,
		.name		= "TurboSight TBS 6903x (Dual DVB-S/S2/S2X)",
		.adapters	= 2,
		.eeprom_i2c	= 0,
		.adap_config	= {
			{
				.ts_in = 0,
				.i2c_bus_nr = 0,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(0, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(0, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(0, 1),
			}, 
			{
				.ts_in = 1,
				.i2c_bus_nr = 0,
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(1, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(1, 1),
			},
		}
	},
	[TBSECP3_BOARD_TBS6904X] = {
		.board_id	= TBSECP3_BOARD_TBS6904X,
		.name		= "TurboSight TBS 6904x (Quad DVB-S/S2/S2x)",
		.adapters	= 4,
		.eeprom_i2c	= 0,
		.adap_config	= {
			{
				.ts_in = 3,
				.i2c_bus_nr = 1,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(1, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(3, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(3, 1),
			},
			{
				.ts_in = 2,
				.i2c_bus_nr = 1,
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(2, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(2, 1),
			},
			{
				.ts_in = 1,
				.i2c_bus_nr = 0,
				.gpio.demod_reset.lvl = TBSECP3_GPIODEF_LOW,
				.gpio.demod_reset.nr  = TBSECP3_GPIO_PIN(0, 0),
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(1, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(1, 1),
			},
			{
				.ts_in = 0,
				.i2c_bus_nr = 0,
				.gpio.lnb_power.lvl   = TBSECP3_GPIODEF_LOW,
				.gpio.lnb_power.nr    = TBSECP3_GPIO_PIN(0, 2),
				.gpio.lnb_voltage.lvl = TBSECP3_GPIODEF_HIGH,
				.gpio.lnb_voltage.nr  = TBSECP3_GPIO_PIN(0, 1),
			},
		}
	},
};
