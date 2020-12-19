/*
 *
 *  Connection Manager
 *
 *  Copyright (C) 2007-2012  Intel Corporation. All rights reserved.
 *  Copyright (C) 2012-2014  BMW Car IT GmbH.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "connman.h"

static char *default_interface;
static GHashTable *nat_hash;

struct connman_nat {
	char *address;
	unsigned char prefixlen;
	struct firewall_context *fw;

	char *interface;
};

static int enable_ip_forward(bool enable)
{
	static char value = 0;
	int f, err = 0;

	if ((f = open("/proc/sys/net/ipv4/ip_forward", O_CLOEXEC | O_RDWR)) < 0)
		return -errno;

	if (!value) {
		if (read(f, &value, sizeof(value)) < 0)
			value = 0;

		if (lseek(f, 0, SEEK_SET) < 0) {
			close(f);
			return -errno;
		}
	}

	if (enable) {
		char allow = '1';

		if (write (f, &allow, sizeof(allow)) < 0)
			err = -errno;
	} else {
		char deny = '0';

		if (value)
			deny = value;

		if (write(f, &deny, sizeof(deny)) < 0)
			err = -errno;

		value = 0;
	}

	close(f);

	return err;
}

static int enable_nat(struct connman_nat *nat)
{
	g_free(nat->interface);
	nat->interface = g_strdup(default_interface);

	if (!nat->interface)
		return 0;

	return __connman_firewall_enable_nat(nat->fw, nat->address,
					nat->prefixlen,	nat->interface);
}

static void disable_nat(struct connman_nat *nat)
{
	if (!nat->interface)
		return;

	__connman_firewall_disable_nat(nat->fw);
}

int __connman_nat_enable(const char *name, const char *address,
				unsigned char prefixlen)
{
	struct connman_nat *nat;
	int err;

	if (g_hash_table_size(nat_hash) == 0) {
		err = enable_ip_forward(true);
		if (err < 0)
			return err;
	}

	nat = g_try_new0(struct connman_nat, 1);
	if (!nat)
		goto err;

	nat->fw = __connman_firewall_create();
	if (!nat->fw)
		goto err;

	nat->address = g_strdup(address);
	nat->prefixlen = prefixlen;

	g_hash_table_replace(nat_hash, g_strdup(name), nat);

	return enable_nat(nat);

err:
	if (nat) {
		if (nat->fw)
			__connman_firewall_destroy(nat->fw);
		g_free(nat);
	}

	if (g_hash_table_size(nat_hash) == 0)
		enable_ip_forward(false);

	return -ENOMEM;
}

void __connman_nat_disable(const char *name)
{
	struct connman_nat *nat;

	nat = g_hash_table_lookup(nat_hash, name);
	if (!nat)
		return;

	disable_nat(nat);

	g_hash_table_remove(nat_hash, name);

	if (g_hash_table_size(nat_hash) == 0)
		enable_ip_forward(false);
}

static void update_default_interface(struct connman_service *service)
{
	GHashTableIter iter;
	gpointer key, value;
	char *interface;
	int err;

	interface = connman_service_get_interface(service);

	DBG("interface %s", interface);

	g_free(default_interface);
	default_interface = interface;

	g_hash_table_iter_init(&iter, nat_hash);

	while (g_hash_table_iter_next(&iter, &key, &value)) {
		const char *name = key;
		struct connman_nat *nat = value;

		disable_nat(nat);
		err = enable_nat(nat);
		if (err < 0)
			DBG("Failed to enable nat for %s", name);
	}
}

static void shutdown_nat(gpointer key, gpointer value, gpointer user_data)
{
	const char *name = key;

	__connman_nat_disable(name);
}

static void cleanup_nat(gpointer data)
{
	struct connman_nat *nat = data;

	__connman_firewall_destroy(nat->fw);
	g_free(nat->address);
	g_free(nat->interface);
	g_free(nat);
}

static const struct connman_notifier nat_notifier = {
	.name			= "nat",
	.default_changed	= update_default_interface,
};

int __connman_nat_init(void)
{
	int err;

	DBG("");

	err = connman_notifier_register(&nat_notifier);
	if (err < 0)
		return err;

	nat_hash = g_hash_table_new_full(g_str_hash, g_str_equal,
						g_free, cleanup_nat);

	return 0;
}

void __connman_nat_cleanup(void)
{
	DBG("");

	g_hash_table_foreach(nat_hash, shutdown_nat, NULL);
	g_hash_table_destroy(nat_hash);
	nat_hash = NULL;

	connman_notifier_unregister(&nat_notifier);
}
