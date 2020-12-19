/*
 *
 *  BlueZ - Bluetooth protocol stack for Linux
 *
 *  Copyright (C) 2000-2001  Qualcomm Incorporated
 *  Copyright (C) 2002-2003  Maxim Krasnyansky <maxk@qualcomm.com>
 *  Copyright (C) 2002-2010  Marcel Holtmann <marcel@holtmann.org>
 *
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

typedef enum {
	BT_MODE_DUAL,
	BT_MODE_BREDR,
	BT_MODE_LE,
} bt_mode_t;

typedef enum {
	BT_GATT_CACHE_ALWAYS,
	BT_GATT_CACHE_YES,
	BT_GATT_CACHE_NO,
} bt_gatt_cache_t;

enum jw_repairing_t {
	JW_REPAIRING_NEVER,
	JW_REPAIRING_CONFIRM,
	JW_REPAIRING_ALWAYS,
};

enum mps_mode_t {
	MPS_OFF,
	MPS_SINGLE,
	MPS_MULTIPLE,
};

struct main_opts {
	char		*name;
	uint32_t	class;
	gboolean	pairable;
	uint32_t	pairto;
	uint32_t	discovto;
	uint32_t	tmpto;
	uint8_t		privacy;

	struct {
		uint16_t	num_entries;

		uint16_t	br_page_scan_type;
		uint16_t	br_page_scan_interval;
		uint16_t	br_page_scan_win;

		uint16_t	br_scan_type;
		uint16_t	br_scan_interval;
		uint16_t	br_scan_win;

		uint16_t	br_link_supervision_timeout;
		uint16_t	br_page_timeout;

		uint16_t	br_min_sniff_interval;
		uint16_t	br_max_sniff_interval;

		uint16_t	le_min_adv_interval;
		uint16_t	le_max_adv_interval;
		uint16_t	le_multi_adv_rotation_interval;

		uint16_t	le_scan_interval_autoconnect;
		uint16_t	le_scan_win_autoconnect;
		uint16_t	le_scan_interval_suspend;
		uint16_t	le_scan_win_suspend;
		uint16_t	le_scan_interval_discovery;
		uint16_t	le_scan_win_discovery;
		uint16_t	le_scan_interval_adv_monitor;
		uint16_t	le_scan_win_adv_monitor;
		uint16_t	le_scan_interval_connect;
		uint16_t	le_scan_win_connect;

		uint16_t	le_min_conn_interval;
		uint16_t	le_max_conn_interval;
		uint16_t	le_conn_latency;
		uint16_t	le_conn_lsto;
		uint16_t	le_autoconnect_timeout;
	} default_params;


	gboolean	reverse_discovery;
	gboolean	name_resolv;
	gboolean	debug_keys;
	gboolean	fast_conn;
	gboolean	refresh_discovery;

	uint16_t	did_source;
	uint16_t	did_vendor;
	uint16_t	did_product;
	uint16_t	did_version;

	bt_mode_t	mode;
	bt_gatt_cache_t gatt_cache;
	uint16_t	gatt_mtu;
	uint8_t		gatt_channels;
	enum mps_mode_t	mps;

	uint8_t		key_size;

	enum jw_repairing_t jw_repairing;
};

extern struct main_opts main_opts;

gboolean plugin_init(const char *enable, const char *disable);
void plugin_cleanup(void);

void rfkill_init(void);
void rfkill_exit(void);

GKeyFile *btd_get_main_conf(void);

void btd_exit(void);
