/*
 *
 *  Connection Manager
 *
 *  Copyright (C) 2013,2015  BMW Car IT GmbH.
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

#include <xtables.h>
#include <linux/netfilter_ipv4/ip_tables.h>

#include "connman.h"

#define CHAIN_PREFIX "connman-"

static const char *builtin_chains[] = {
	[NF_IP_PRE_ROUTING]	= "PREROUTING",
	[NF_IP_LOCAL_IN]	= "INPUT",
	[NF_IP_FORWARD]		= "FORWARD",
	[NF_IP_LOCAL_OUT]	= "OUTPUT",
	[NF_IP_POST_ROUTING]	= "POSTROUTING",
};

struct connman_managed_table {
	char *name;
	unsigned int chains[NF_INET_NUMHOOKS];
};

struct fw_rule {
	bool enabled;
	char *table;
	char *chain;
	char *rule_spec;
};

struct firewall_context {
	GList *rules;
};

static GSList *managed_tables;
static struct firewall_context *connmark_ctx;
static unsigned int connmark_ref;

static int chain_to_index(const char *chain_name)
{
	if (!g_strcmp0(builtin_chains[NF_IP_PRE_ROUTING], chain_name))
		return NF_IP_PRE_ROUTING;
	if (!g_strcmp0(builtin_chains[NF_IP_LOCAL_IN], chain_name))
		return NF_IP_LOCAL_IN;
	if (!g_strcmp0(builtin_chains[NF_IP_FORWARD], chain_name))
		return NF_IP_FORWARD;
	if (!g_strcmp0(builtin_chains[NF_IP_LOCAL_OUT], chain_name))
		return NF_IP_LOCAL_OUT;
	if (!g_strcmp0(builtin_chains[NF_IP_POST_ROUTING], chain_name))
		return NF_IP_POST_ROUTING;

	return -1;
}

static int managed_chain_to_index(const char *chain_name)
{
	if (!g_str_has_prefix(chain_name, CHAIN_PREFIX))
		return -1;

	return chain_to_index(chain_name + strlen(CHAIN_PREFIX));
}

static int insert_managed_chain(const char *table_name, int id)
{
	char *rule, *managed_chain;
	int err;

	managed_chain = g_strdup_printf("%s%s", CHAIN_PREFIX,
					builtin_chains[id]);

	err = __connman_iptables_new_chain(AF_INET, table_name, managed_chain);
	if (err < 0)
		goto out;

	rule = g_strdup_printf("-j %s", managed_chain);
	err = __connman_iptables_insert(AF_INET, table_name, builtin_chains[id],
						rule);
	g_free(rule);
	if (err < 0) {
		__connman_iptables_delete_chain(AF_INET, table_name,
							managed_chain);
		goto out;
	}

out:
	g_free(managed_chain);

	return err;
}

static int delete_managed_chain(const char *table_name, int id)
{
	char *rule, *managed_chain;
	int err;

	managed_chain = g_strdup_printf("%s%s", CHAIN_PREFIX,
					builtin_chains[id]);

	rule = g_strdup_printf("-j %s", managed_chain);
	err = __connman_iptables_delete(AF_INET, table_name, builtin_chains[id],
					rule);
	g_free(rule);

	if (err < 0)
		goto out;

	err =  __connman_iptables_delete_chain(AF_INET, table_name,
						managed_chain);

out:
	g_free(managed_chain);

	return err;
}

static int insert_managed_rule(const char *table_name,
				const char *chain_name,
				const char *rule_spec)
{
	struct connman_managed_table *mtable = NULL;
	GSList *list;
	char *chain;
	int id, err;

	id = chain_to_index(chain_name);
	if (id < 0) {
		/* This chain is not managed */
		chain = g_strdup(chain_name);
		goto out;
	}

	for (list = managed_tables; list; list = list->next) {
		mtable = list->data;

		if (g_strcmp0(mtable->name, table_name) == 0)
			break;

		mtable = NULL;
	}

	if (!mtable) {
		mtable = g_new0(struct connman_managed_table, 1);
		mtable->name = g_strdup(table_name);

		managed_tables = g_slist_prepend(managed_tables, mtable);
	}

	if (mtable->chains[id] == 0) {
		DBG("table %s add managed chain for %s",
			table_name, chain_name);

		err = insert_managed_chain(table_name, id);
		if (err < 0)
			return err;
	}

	mtable->chains[id]++;
	chain = g_strdup_printf("%s%s", CHAIN_PREFIX, chain_name);

out:
	err = __connman_iptables_append(AF_INET, table_name, chain, rule_spec);

	g_free(chain);

	return err;
 }

static int delete_managed_rule(const char *table_name,
				const char *chain_name,
				const char *rule_spec)
 {
	struct connman_managed_table *mtable = NULL;
	GSList *list;
	int id, err;
	char *managed_chain;

	id = chain_to_index(chain_name);
	if (id < 0) {
		/* This chain is not managed */
		return __connman_iptables_delete(AF_INET, table_name,
							chain_name, rule_spec);
	}

	managed_chain = g_strdup_printf("%s%s", CHAIN_PREFIX, chain_name);

	err = __connman_iptables_delete(AF_INET, table_name, managed_chain,
						rule_spec);

	for (list = managed_tables; list; list = list->next) {
		mtable = list->data;

		if (g_strcmp0(mtable->name, table_name) == 0)
			break;

		mtable = NULL;
	}

	if (!mtable) {
		err = -ENOENT;
		goto out;
	}

	mtable->chains[id]--;
	if (mtable->chains[id] > 0)
		goto out;

	DBG("table %s remove managed chain for %s",
			table_name, chain_name);

	err = delete_managed_chain(table_name, id);

 out:
	g_free(managed_chain);

	return err;
}

static void cleanup_managed_table(gpointer user_data)
{
	struct connman_managed_table *table = user_data;

	g_free(table->name);
	g_free(table);
}

static void cleanup_fw_rule(gpointer user_data)
{
	struct fw_rule *rule = user_data;

	g_free(rule->rule_spec);
	g_free(rule->chain);
	g_free(rule->table);
	g_free(rule);
}

struct firewall_context *__connman_firewall_create(void)
{
	struct firewall_context *ctx;

	ctx = g_new0(struct firewall_context, 1);

	return ctx;
}

void __connman_firewall_destroy(struct firewall_context *ctx)
{
	g_list_free_full(ctx->rules, cleanup_fw_rule);
	g_free(ctx);
}

static int enable_rule(struct fw_rule *rule)
{
	int err;

	if (rule->enabled)
		return -EALREADY;

	DBG("%s %s %s", rule->table, rule->chain, rule->rule_spec);

	err = insert_managed_rule(rule->table, rule->chain, rule->rule_spec);
	if (err < 0)
		return err;

	err = __connman_iptables_commit(AF_INET, rule->table);
	if (err < 0)
		return err;

	rule->enabled = true;

	return 0;
}

static int disable_rule(struct fw_rule *rule)
{
	int err;

	if (!rule->enabled)
		return -EALREADY;

	err = delete_managed_rule(rule->table, rule->chain, rule->rule_spec);
	if (err < 0) {
		connman_error("Cannot remove previously installed "
			"iptables rules: %s", strerror(-err));
		return err;
	}

	err = __connman_iptables_commit(AF_INET, rule->table);
	if (err < 0) {
		connman_error("Cannot remove previously installed "
			"iptables rules: %s", strerror(-err));
		return err;
	}

	rule->enabled = false;

	return 0;
}

static void firewall_add_rule(struct firewall_context *ctx,
				const char *table,
				const char *chain,
				const char *rule_fmt, ...)
{
	va_list args;
	char *rule_spec;
	struct fw_rule *rule;

	va_start(args, rule_fmt);

	rule_spec = g_strdup_vprintf(rule_fmt, args);

	va_end(args);

	rule = g_new0(struct fw_rule, 1);

	rule->enabled = false;
	rule->table = g_strdup(table);
	rule->chain = g_strdup(chain);
	rule->rule_spec = rule_spec;

	ctx->rules = g_list_append(ctx->rules, rule);
}

static void firewall_remove_rules(struct firewall_context *ctx)
{
	g_list_free_full(ctx->rules, cleanup_fw_rule);
	ctx->rules = NULL;
}

static int firewall_enable_rules(struct firewall_context *ctx)
{
	struct fw_rule *rule;
	GList *list;
	int err = -ENOENT;

	for (list = g_list_first(ctx->rules); list; list = g_list_next(list)) {
		rule = list->data;

		err = enable_rule(rule);
		if (err < 0)
			break;
	}

	return err;
}

static int firewall_disable_rules(struct firewall_context *ctx)
{
	struct fw_rule *rule;
	GList *list;
	int e;
	int err = -ENOENT;

	for (list = g_list_last(ctx->rules); list;
			list = g_list_previous(list)) {
		rule = list->data;

		e = disable_rule(rule);

		/* Report last error back */
		if (e == 0 && err == -ENOENT)
			err = 0;
		else if (e < 0)
			err = e;
	}

	return err;
}

int __connman_firewall_enable_nat(struct firewall_context *ctx,
				char *address, unsigned char prefixlen,
				char *interface)
{
	int err;

	firewall_add_rule(ctx, "nat", "POSTROUTING",
				"-s %s/%d -o %s -j MASQUERADE",
				address, prefixlen, interface);

	err = firewall_enable_rules(ctx);
	if (err)
		firewall_remove_rules(ctx);
	return err;
}

int __connman_firewall_disable_nat(struct firewall_context *ctx)
{
	int err;

	err = firewall_disable_rules(ctx);
	if (err < 0) {
		DBG("could not disable NAT rule");
		return err;
	}

	firewall_remove_rules(ctx);
	return 0;
}

int __connman_firewall_enable_snat(struct firewall_context *ctx,
				int index, const char *ifname,
				const char *addr)
{
	int err;

	firewall_add_rule(ctx, "nat", "POSTROUTING",
				"-o %s -j SNAT --to-source %s",
				ifname, addr);

	err = firewall_enable_rules(ctx);
	if (err)
		firewall_remove_rules(ctx);
	return err;
}

int __connman_firewall_disable_snat(struct firewall_context *ctx)
{
	int err;

	err = firewall_disable_rules(ctx);
	if (err < 0) {
		DBG("could not disable SNAT rule");
		return err;
	}

	firewall_remove_rules(ctx);
	return 0;
}

static int firewall_enable_connmark(void)
{
	int err;

	if (connmark_ref > 0) {
		connmark_ref++;
		return 0;
	}

	connmark_ctx = __connman_firewall_create();

	firewall_add_rule(connmark_ctx, "mangle", "INPUT",
					"-j CONNMARK --restore-mark");
	firewall_add_rule(connmark_ctx, "mangle", "POSTROUTING",
					"-j CONNMARK --save-mark");
	err = firewall_enable_rules(connmark_ctx);
	if (err) {
		__connman_firewall_destroy(connmark_ctx);
		connmark_ctx = NULL;
		return err;
	}
	connmark_ref++;
	return 0;
}

static void firewall_disable_connmark(void)
{
	connmark_ref--;
	if (connmark_ref > 0)
		return;

	firewall_disable_rules(connmark_ctx);
	__connman_firewall_destroy(connmark_ctx);
	connmark_ctx = NULL;
}

int __connman_firewall_enable_marking(struct firewall_context *ctx,
					enum connman_session_id_type id_type,
					char *id, const char *src_ip,
					uint32_t mark)
{
	int err;

	err = firewall_enable_connmark();
	if (err)
		return err;

	switch (id_type) {
	case CONNMAN_SESSION_ID_TYPE_UID:
		firewall_add_rule(ctx, "mangle", "OUTPUT",
				"-m owner --uid-owner %s -j MARK --set-mark %d",
					id, mark);
		break;
	case CONNMAN_SESSION_ID_TYPE_GID:
		firewall_add_rule(ctx, "mangle", "OUTPUT",
				"-m owner --gid-owner %s -j MARK --set-mark %d",
					id, mark);
		break;
	case CONNMAN_SESSION_ID_TYPE_UNKNOWN:
		break;
	case CONNMAN_SESSION_ID_TYPE_LSM:
	default:
		return -EINVAL;
	}

	if (src_ip) {
		firewall_add_rule(ctx, "mangle", "OUTPUT",
				"-s %s -j MARK --set-mark %d",
					src_ip, mark);
	}

	return firewall_enable_rules(ctx);
}

int __connman_firewall_disable_marking(struct firewall_context *ctx)
{
	firewall_disable_connmark();
	return firewall_disable_rules(ctx);
}

static void iterate_chains_cb(const char *chain_name, void *user_data)
{
	GSList **chains = user_data;
	int id;

	id = managed_chain_to_index(chain_name);
	if (id < 0)
		return;

	*chains = g_slist_prepend(*chains, GINT_TO_POINTER(id));
}

static void flush_table(const char *table_name)
{
	GSList *chains = NULL, *list;
	char *rule, *managed_chain;
	int id, err;

	__connman_iptables_iterate_chains(AF_INET, table_name,
						iterate_chains_cb, &chains);

	for (list = chains; list; list = list->next) {
		id = GPOINTER_TO_INT(list->data);

		managed_chain = g_strdup_printf("%s%s", CHAIN_PREFIX,
						builtin_chains[id]);

		rule = g_strdup_printf("-j %s", managed_chain);
		err = __connman_iptables_delete(AF_INET, table_name,
						builtin_chains[id], rule);
		if (err < 0) {
			connman_warn("Failed to delete jump rule '%s': %s",
				rule, strerror(-err));
		}
		g_free(rule);

		err = __connman_iptables_flush_chain(AF_INET, table_name,
							managed_chain);
		if (err < 0) {
			connman_warn("Failed to flush chain '%s': %s",
				managed_chain, strerror(-err));
		}
		err = __connman_iptables_delete_chain(AF_INET, table_name,
							managed_chain);
		if (err < 0) {
			connman_warn("Failed to delete chain '%s': %s",
				managed_chain, strerror(-err));
		}

		g_free(managed_chain);
	}

	err = __connman_iptables_commit(AF_INET, table_name);
	if (err < 0) {
		connman_warn("Failed to flush table '%s': %s",
			table_name, strerror(-err));
	}

	g_slist_free(chains);
}

static void flush_all_tables(void)
{
	/* Flush the tables ConnMan might have modified
	 * But do so if only ConnMan has done something with
	 * iptables */

	if (!g_file_test("/proc/net/ip_tables_names",
			G_FILE_TEST_EXISTS | G_FILE_TEST_IS_REGULAR)) {
		return;
	}

	flush_table("filter");
	flush_table("mangle");
	flush_table("nat");
}

int __connman_firewall_init(void)
{
	DBG("");

	__connman_iptables_init();
	flush_all_tables();

	return 0;
}

void __connman_firewall_cleanup(void)
{
	DBG("");

	g_slist_free_full(managed_tables, cleanup_managed_table);
	__connman_iptables_cleanup();
}
