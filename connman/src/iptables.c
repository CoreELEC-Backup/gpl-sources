/*
 *
 *  Connection Manager
 *
 *  Copyright (C) 2007-2013  Intel Corporation. All rights reserved.
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

#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <xtables.h>
#include <inttypes.h>
#include <setjmp.h>

#include <linux/netfilter_ipv4/ip_tables.h>
#include <linux/netfilter_ipv6/ip6_tables.h>

#include "connman.h"
#include "src/shared/util.h"

/*
 * Some comments on how the iptables API works (some of them from the
 * source code from iptables and the kernel):
 *
 * - valid_hooks: bit indicates valid IDs for hook_entry
 * - hook_entry[ID] offset to the chain start
 * - overflows should be end of entry chains, and uncodintional policy nodes.
 * - policy entry: last entry in a chain
 * - user chain: end of last builtin + policy entry
 * - final entry must be error node
 * - Underflows must be unconditional and use the STANDARD target with
 *   ACCEPT/DROP
 * - IPT_SO_GET_INFO and IPT_SO_GET_ENTRIES are used to read a table
 * - IPT_SO_GET_INFO: struct ipt_getinfo (note the lack of table content)
 * - IPT_SO_GET_ENTRIES: struct ipt_get_entries (contains only parts of the
 *   table header/meta info. The table is appended after the header. The entries
 *   are of the type struct ipt_entry.
 * - After the ipt_entry the matches are appended. After the matches
 *   the target is appended.
 * - ipt_entry->target_offset =  Size of ipt_entry + matches
 * - ipt_entry->next_offset =  Size of ipt_entry + matches + target
 * - IPT_SO_SET_REPLACE is used to write a table (contains the complete
 * - hook_entry and overflow mark the beginning and the end of a chain, e.g
 *     entry hook: pre/in/fwd/out/post -1/0/352/504/-1
 *     underflow:  pre/in/fwd/out/post -1/200/352/904/-1
 *   means that INPUT starts at offset 0 and ends at 200 (the start offset to
 *   the last element). FORWARD has one entry starting/ending at 352. The entry
 *   has a size of 152. 352 + 152 = 504 which is the start of the OUTPUT chain
 *   which then ends at 904. PREROUTING and POSTROUTING are invalid hooks in
 *   the filter table.
 * - 'iptables -t filter -A INPUT -m mark --mark 999 -j LOG'
 *   writing that table looks like this:
 *
 *   filter valid_hooks 0x0000000e  num_entries 5  size 856
 *   entry hook: pre/in/fwd/out/post -1/0/376/528/-1
 *   underflow:  pre/in/fwd/out/post -1/224/376/528/-1
 *   entry 0x699d30  offset 0  size 224
 *     RULE  match 0x699da0  target 0x699dd0
 *             match  mark match 0x3e7
 *             target  LOG flags 0 level 4
 *             src 0.0.0.0/0.0.0.0
 *             dst 0.0.0.0/0.0.0.0
 *   entry 0x699e10  offset 224  size 152
 *     RULE  match 0x699e80  target 0x699e80
 *             target ACCEPT
 *             src 0.0.0.0/0.0.0.0
 *             dst 0.0.0.0/0.0.0.0
 *   entry 0x699ea8  offset 376  size 152
 *     RULE  match 0x699f18  target 0x699f18
 *             target ACCEPT
 *             src 0.0.0.0/0.0.0.0
 *             dst 0.0.0.0/0.0.0.0
 *   entry 0x699f40  offset 528  size 152
 *     RULE  match 0x699fb0  target 0x699fb0
 *             target ACCEPT
 *             src 0.0.0.0/0.0.0.0
 *             dst 0.0.0.0/0.0.0.0
 *   entry 0x699fd8  offset 680  size 176
 *     USER CHAIN (ERROR)  match 0x69a048  target 0x69a048
 *
 *   Reading the filter table looks like this:
 *
 *   filter valid_hooks 0x0000000e  num_entries 5  size 856
 *   entry hook: pre/in/fwd/out/post -1/0/376/528/-1
 *   underflow:  pre/in/fwd/out/post -1/224/376/528/-1
 *   entry 0x25fec28  offset 0  size 224
 *     CHAIN (INPUT)  match 0x25fec98  target 0x25fecc8
 *             match  mark match 0x3e7
 *             target  LOG flags 0 level 4
 *             src 0.0.0.0/0.0.0.0
 *             dst 0.0.0.0/0.0.0.0
 *   entry 0x25fed08  offset 224  size 152
 *     RULE  match 0x25fed78  target 0x25fed78
 *             target ACCEPT
 *             src 0.0.0.0/0.0.0.0
 *             dst 0.0.0.0/0.0.0.0
 *   entry 0x25feda0  offset 376  size 152
 *     CHAIN (FORWARD)  match 0x25fee10  target 0x25fee10
 *             target ACCEPT
 *             src 0.0.0.0/0.0.0.0
 *             dst 0.0.0.0/0.0.0.0
 *   entry 0x25fee38  offset 528  size 152
 *     CHAIN (OUTPUT)  match 0x25feea8  target 0x25feea8
 *             target ACCEPT
 *             src 0.0.0.0/0.0.0.0
 *             dst 0.0.0.0/0.0.0.0
 *   entry 0x25feed0  offset 680  size 176
 *     End of CHAIN
 */

/*
 * Values for the index values used here  are defined as equal for both IPv4
 * and IPv6 (NF_IP_* and NF_IP6_*) in Netfilter headers.
 */
static const char *hooknames[] = {
	[NF_IP_PRE_ROUTING]	= "PREROUTING",
	[NF_IP_LOCAL_IN]	= "INPUT",
	[NF_IP_FORWARD]		= "FORWARD",
	[NF_IP_LOCAL_OUT]	= "OUTPUT",
	[NF_IP_POST_ROUTING]	= "POSTROUTING",
};

#define LABEL_ACCEPT  "ACCEPT"
#define LABEL_DROP    "DROP"
#define LABEL_QUEUE   "QUEUE"
#define LABEL_RETURN  "RETURN"

#define XT_OPTION_OFFSET_SCALE 256

struct connman_iptables_entry {
	int type;
	unsigned int offset;
	int builtin;
	int counter_idx;

	struct ipt_entry *entry;
	struct ip6t_entry *entry6;
};

struct connman_iptables {
	int type;
	char *name;
	int ipt_sock;

	struct ipt_getinfo *info;
	struct ipt_get_entries *blob_entries;
	struct ip6t_getinfo *info6;
	struct ip6t_get_entries *blob_entries6;

	unsigned int num_entries;
	unsigned int old_entries;
	unsigned int size;

	unsigned int underflow[NF_INET_NUMHOOKS];
	unsigned int hook_entry[NF_INET_NUMHOOKS];

	GList *entries;
};

static GHashTable *table_hash = NULL;
static GHashTable *table_hash_ipv6 = NULL;
static bool debug_enabled = false;

struct iptables_ip {
	int type;
	struct ipt_ip *ip;
	struct ip6t_ip6 *ip6;
};

struct iptables_replace {
	int type;
	struct ipt_replace *r;
	struct ip6t_replace *r6;
};

static jmp_buf env_state;
static bool jmp_set = false;

static void enable_jmp()
{
	jmp_set = true;
}

static void disable_jmp()
{
	jmp_set = false;
}

static bool can_jmp()
{
	DBG("%s", jmp_set ? "true" : "false");
	return jmp_set;
}

typedef int (*iterate_entries_cb_t)(struct connman_iptables_entry *entry,
					int builtin, unsigned int hook,
					size_t size, unsigned int offset,
					void *user_data);

static u_int16_t iptables_entry_get_next_offset(
					struct connman_iptables_entry *entry)
{
	if (!entry)
		return 0;

	switch (entry->type) {
	case AF_INET:
		return entry->entry ? entry->entry->next_offset : 0;
	case AF_INET6:
		return entry->entry6 ? entry->entry6->next_offset : 0;
	}

	return 0;
}

static u_int16_t iptables_entry_get_target_offset(
					struct connman_iptables_entry *entry)
{
	if (!entry)
		return 0;

	switch (entry->type) {
	case AF_INET:
		return entry->entry ? entry->entry->target_offset : 0;
	case AF_INET6:
		return entry->entry6 ? entry->entry6->target_offset : 0;
	}

	return 0;
}

static unsigned char *iptables_entry_get_elems(
					struct connman_iptables_entry *entry)
{
	if (!entry)
		return NULL;

	switch (entry->type) {
	case AF_INET:
		return entry->entry ? entry->entry->elems : NULL;
	case AF_INET6:
		return entry->entry6 ? entry->entry6->elems : NULL;
	}

	return NULL;
}

static struct xt_entry_target *iptables_entry_get_target(
					struct connman_iptables_entry *entry)
{
	if (!entry)
		return NULL;

	switch (entry->type) {
	case AF_INET:
		return entry->entry ? ipt_get_target(entry->entry) : NULL;
	case AF_INET6:
		return entry->entry6 ? ip6t_get_target(entry->entry6) : NULL;
	}

	return NULL;
}

static struct xt_counters *iptables_entry_get_counters(
					struct connman_iptables_entry *entry)
{
	if (!entry)
		return NULL;

	switch (entry->type) {
	case AF_INET:
		return entry->entry ? &entry->entry->counters : NULL;
	case AF_INET6:
		return entry->entry6 ? &entry->entry6->counters : NULL;
	}

	return NULL;
}

static void iptables_entry_free(struct connman_iptables_entry *entry)
{
	if (!entry)
		return;

	g_free(entry->entry);
	g_free(entry->entry6);
	g_free(entry);
}

static const char *iptables_table_get_info_name(struct connman_iptables* table)
{
	if (!table)
		return NULL;

	switch (table->type) {
	case AF_INET:
		return table->info->name;
	case AF_INET6:
		return table->info6->name;
	}

	return NULL;
}

static unsigned int iptables_table_get_info_num_entries(
					struct connman_iptables* table)
{
	if (!table)
		return 0;

	switch (table->type) {
	case AF_INET:
		return table->info->num_entries;
	case AF_INET6:
		return table->info6->num_entries;
	}

	return 0;
}

static unsigned int iptables_table_get_info_size(struct connman_iptables* table)
{
	if (!table)
		return 0;

	switch (table->type) {
	case AF_INET:
		return table->info->size;
	case AF_INET6:
		return table->info6->size;
	}

	return 0;
}

static unsigned int iptables_table_get_info_valid_hooks(
					struct connman_iptables* table)
{
	if (!table)
		return 0;

	switch (table->type) {
	case AF_INET:
		return table->info->valid_hooks;
	case AF_INET6:
		return table->info6->valid_hooks;
	}

	return 0;
}

static unsigned int *iptables_table_get_info_hook_entry(
					struct connman_iptables* table)
{
	if (!table)
		return NULL;

	switch (table->type) {
	case AF_INET:
		return table->info->hook_entry;
	case AF_INET6:
		return table->info6->hook_entry;
	}

	return NULL;
}

static unsigned int *iptables_table_get_info_underflow(
					struct connman_iptables* table)
{
	if (!table)
		return NULL;

	switch (table->type) {
	case AF_INET:
		return table->info->underflow;
	case AF_INET6:
		return table->info6->underflow;
	}

	return NULL;
}

static unsigned int iptables_table_get_entries_size(
					struct connman_iptables* table)
{
	if (!table)
		return 0;

	switch (table->type) {
	case AF_INET:
		return table->blob_entries->size;
	case AF_INET6:
		return table->blob_entries6->size;
	}

	return 0;
}

static const char *get_error_target(int type)
{
	switch (type) {
	case AF_INET:
		return IPT_ERROR_TARGET;
	case AF_INET6:
		return IP6T_ERROR_TARGET;
	default:
		return XT_ERROR_TARGET;
	}
}

static const char *get_standard_target(int type)
{
	switch (type) {
	case AF_INET:
		return IPT_STANDARD_TARGET;
	case AF_INET6:
		return IP6T_STANDARD_TARGET;
	default:
		return XT_STANDARD_TARGET;
	}
}

static struct connman_iptables *hash_table_lookup(int type,
					const char *table_name) {

	switch (type) {
	case AF_INET:
		return g_hash_table_lookup(table_hash, table_name);
	case AF_INET6:
		return g_hash_table_lookup(table_hash_ipv6, table_name);
	}
	
	return NULL;
}

static bool hash_table_replace(int type,
					char *table_name,
					struct connman_iptables *table) {

	switch (type) {
	case AF_INET:
		return g_hash_table_replace(table_hash, table_name, table);
	case AF_INET6:
		return g_hash_table_replace(table_hash_ipv6, table_name, table);
	}
	
	return false;
}

static bool hash_table_remove(int type, const char *table_name)
{
	switch (type) {
	case AF_INET:
		return g_hash_table_remove(table_hash, table_name);
	case AF_INET6:
		return g_hash_table_remove(table_hash_ipv6, table_name);
	}
	
	return false;
}

static unsigned int next_hook_entry_index(unsigned int *valid_hooks)
{
	unsigned int h;

	if (*valid_hooks == 0)
		return NF_INET_NUMHOOKS;

	h = __builtin_ffs(*valid_hooks) - 1;
	*valid_hooks ^= (1 << h);

	return h;
}

static int iterate_entries(struct connman_iptables_entry *entries,
				unsigned int valid_hooks,
				unsigned int *hook_entry,
				unsigned int *underflow,
				size_t size, iterate_entries_cb_t cb,
				void *user_data)
{
	unsigned int offset, h, hook;
	int builtin, err;
	struct connman_iptables_entry entry;

	if (!entries)
		return -EINVAL;

	switch (entries->type) {
	case AF_INET:
		if (!entries->entry)
			return -EINVAL;

		break;
	case AF_INET6:
		if (!entries->entry6)
			return -EINVAL;

		break;
	default:
		return -EINVAL;
	}

	h = next_hook_entry_index(&valid_hooks);
	hook = h;

	entry.type = entries->type;
	entry.entry = entries->entry;
	entry.entry6 = entries->entry6;

	for (offset = 0; offset < size;
			offset += iptables_entry_get_next_offset(&entry)) {
		builtin = -1;
		
		switch (entries->type) {
		case AF_INET:
			entry.entry = (void* )entries->entry + offset;
			break;
		case AF_INET6:
			entry.entry6 = (void* )entries->entry6 + offset;
			break;
		}

		/*
		 * Updating builtin, hook and h is very tricky.
		 * The rules are:
		 * - builtin is only set to the current hook number
		 *   if the current entry is the hook entry (aka chain
		 *   head). And only for builtin chains, never for
		 *   the user chains.
		 * - hook is the current hook number. If we
		 *   look at user chains it needs to be NF_INET_NETNUMHOOKS.
		 * - h is the next hook entry. Thous we need to be carefully
		 *   not to access the table when h is NF_INET_NETNUMHOOKS.
		 */
		if (h < NF_INET_NUMHOOKS && hook_entry[h] == offset) {
			builtin = h;
			hook = h;
		}

		if (h == NF_INET_NUMHOOKS)
			hook = h;

		if (h < NF_INET_NUMHOOKS && underflow[h] <= offset)
			h = next_hook_entry_index(&valid_hooks);

		err = cb(&entry, builtin, hook, size, offset, user_data);
		if (err < 0)
			return err;
	}

	return 0;
}

static int print_entry(struct connman_iptables_entry *entry, int builtin,
					unsigned int hook, size_t size,
					unsigned int offset, void *user_data)
{
	iterate_entries_cb_t cb;
	struct xt_counters *counters;

	cb = user_data;
	counters = iptables_entry_get_counters(entry);

	DBG("entry %p  hook %u  offset %u  size %u  packets %"PRIu64"  "
		"bytes %"PRIu64, entry, hook, offset,
			iptables_entry_get_next_offset(entry),
			(uint64_t) counters->pcnt, (uint64_t) counters->bcnt);

	return cb(entry, builtin, hook, size, offset, NULL);
}

static int target_to_verdict(const char *target_name)
{
	if (!g_strcmp0(target_name, LABEL_ACCEPT))
		return -NF_ACCEPT - 1;

	if (!g_strcmp0(target_name, LABEL_DROP))
		return -NF_DROP - 1;

	if (!g_strcmp0(target_name, LABEL_QUEUE))
		return -NF_QUEUE - 1;

	if (!g_strcmp0(target_name, LABEL_RETURN))
		return XT_RETURN;

	return 0;
}

static bool is_builtin_target(const char *target_name)
{
	if (!g_strcmp0(target_name, LABEL_ACCEPT) ||
		!g_strcmp0(target_name, LABEL_DROP) ||
		!g_strcmp0(target_name, LABEL_QUEUE) ||
		!g_strcmp0(target_name, LABEL_RETURN))
		return true;

	return false;
}

static bool is_jump(struct connman_iptables_entry *e)
{
	struct xt_entry_target *target;

	target = iptables_entry_get_target(e);

	if (!target)
		return false;

	if (!g_strcmp0(target->u.user.name, get_standard_target(e->type))) {
		struct xt_standard_target *t;

		t = (struct xt_standard_target *)target;

		switch (t->verdict) {
		case XT_RETURN:
		case -NF_ACCEPT - 1:
		case -NF_DROP - 1:
		case -NF_QUEUE - 1:
		case -NF_STOP - 1:
			return false;

		default:
			return true;
		}
	}

	return false;
}

static bool is_fallthrough(struct connman_iptables_entry *e)
{
	struct xt_entry_target *target;

	target = iptables_entry_get_target(e);

	if (!target)
		return false;

	if (!g_strcmp0(target->u.user.name, get_standard_target(e->type))) {
		struct xt_standard_target *t;

		t = (struct xt_standard_target *)target;
		if (t->verdict == 0)
			return true;
	}
	return false;
}

static bool is_chain(struct connman_iptables *table,
				struct connman_iptables_entry *e)
{
	struct xt_entry_target *target;

	if (!e)
		return false;

	if (e->builtin >= 0)
		return true;

	target = iptables_entry_get_target(e);
	
	if (!target)
		return false;

	if (!g_strcmp0(target->u.user.name, get_error_target(e->type)))
		return true;

	return false;
}

static GList *find_chain_head(struct connman_iptables *table,
				const char *chain_name)
{
	GList *list;
	struct connman_iptables_entry *head;
	struct xt_entry_target *target;
	int builtin;
	
	switch (table->type) {
	case AF_INET:
	case AF_INET6:
		break;
	default:
		return NULL;
	}

	for (list = table->entries; list; list = list->next) {
		head = list->data;

		/* Buit-in chain */
		builtin = head->builtin;

		if (builtin >= 0 && !g_strcmp0(hooknames[builtin], chain_name))
			break;

		/* User defined chain */
		target = iptables_entry_get_target(head);
		
		if (!target)
			continue;

		if (!g_strcmp0(target->u.user.name,
			get_error_target(table->type)) &&
			!g_strcmp0((char *)target->data, chain_name))
			break;
	}

	return list;
}

static GList *find_chain_tail(struct connman_iptables *table,
				const char *chain_name)
{
	struct connman_iptables_entry *tail;
	GList *chain_head, *list;

	chain_head = find_chain_head(table, chain_name);
	if (!chain_head)
		return NULL;

	/* Then we look for the next chain */
	for (list = chain_head->next; list; list = list->next) {
		tail = list->data;

		if (is_chain(table, tail))
			return list;
	}

	/* Nothing found, we return the table end */
	return g_list_last(table->entries);
}

static void update_offsets(struct connman_iptables *table)
{
	GList *list, *prev;
	struct connman_iptables_entry *entry, *prev_entry;

	for (list = table->entries; list; list = list->next) {
		entry = list->data;

		if (list == table->entries) {
			entry->offset = 0;

			continue;
		}

		prev = list->prev;
		prev_entry = prev->data;

		entry->offset = prev_entry->offset +
					iptables_entry_get_next_offset(
						prev_entry);
	}
}

static void update_targets_reference(struct connman_iptables *table,
				struct connman_iptables_entry *entry_before,
				struct connman_iptables_entry *modified_entry,
				bool is_removing)
{
	struct connman_iptables_entry *tmp;
	struct xt_standard_target *t;
	GList *list;
	unsigned int offset;

	offset = iptables_entry_get_next_offset(modified_entry);

	for (list = table->entries; list; list = list->next) {
		tmp = list->data;

		if (!is_jump(tmp))
			continue;

		t = (struct xt_standard_target *)
			iptables_entry_get_target(tmp);

		if (!t)
			continue;

		if (is_removing) {
			if (t->verdict >= entry_before->offset)
				t->verdict -= offset;
		} else {
			if (t->verdict > entry_before->offset)
				t->verdict += offset;
		}
	}

	if (is_fallthrough(modified_entry)) {
		t = (struct xt_standard_target *)
			iptables_entry_get_target(modified_entry);
		
		if (!t)
			return;

		t->verdict = entry_before->offset +
			iptables_entry_get_target_offset(modified_entry) +
			XT_ALIGN(sizeof(struct xt_standard_target));
		t->target.u.target_size =
			XT_ALIGN(sizeof(struct xt_standard_target));
	}
}

static int iptables_add_entry(struct connman_iptables *table,
				struct connman_iptables_entry *entry,
				GList *before, int builtin, int counter_idx)
{
	struct connman_iptables_entry *e, *entry_before;

	if (!table) {
		return -EINVAL;
	}

	e = g_try_malloc0(sizeof(struct connman_iptables_entry));
	if (!e)
		return -ENOMEM;

	switch (table->type) {
	case AF_INET:
		e->entry = entry->entry;
		break;
	case AF_INET6:
		e->entry6 = entry->entry6;
		break;
	default:
		g_free(e);
		return -EINVAL;
	}

	e->type = entry->type;
	e->builtin = builtin;
	e->counter_idx = counter_idx;

	table->entries = g_list_insert_before(table->entries, before, e);
	table->num_entries++;
	table->size += iptables_entry_get_next_offset(e);

	if (!before) {
		e->offset = table->size -
				iptables_entry_get_next_offset(e);
		return 0;
	}

	entry_before = before->data;

	/*
	 * We've just appended/inserted a new entry. All references
	 * should be bumped accordingly.
	 */
	update_targets_reference(table, entry_before, e, false);

	update_offsets(table);

	return 0;
}

static int remove_table_entry(struct connman_iptables *table,
				struct connman_iptables_entry *entry)
{
	int removed = 0;
	u_int16_t next_offset;

	next_offset = iptables_entry_get_next_offset(entry);
	table->num_entries--;

	table->size -= next_offset;
	removed = next_offset;

	table->entries = g_list_remove(table->entries, entry);

	iptables_entry_free(entry);

	return removed;
}

static void delete_update_hooks(struct connman_iptables *table,
				int builtin, GList *chain_head,
				int removed)
{
	struct connman_iptables_entry *e;
	GList *list;

	e = chain_head->data;
	e->builtin = builtin;

	table->underflow[builtin] -= removed;

	for (list = chain_head->next; list; list = list->next) {
		e = list->data;

		if (e->builtin < 0)
			continue;

		table->hook_entry[e->builtin] -= removed;
		table->underflow[e->builtin] -= removed;
	}
}

static int iptables_flush_chain(struct connman_iptables *table,
						const char *name)
{
	GList *chain_head, *chain_tail, *list, *next;
	struct connman_iptables_entry *entry;
	int builtin, removed = 0;

	DBG("table %s chain %s", table->name, name);

	chain_head = find_chain_head(table, name);
	if (!chain_head)
		return -EINVAL;

	chain_tail = find_chain_tail(table, name);
	if (!chain_tail)
		return -EINVAL;

	entry = chain_head->data;
	builtin = entry->builtin;

	if (builtin >= 0)
		list = chain_head;
	else
		list = chain_head->next;

	if (list == chain_tail->prev)
		return 0;

	while (list != chain_tail->prev) {
		entry = list->data;
		next = g_list_next(list);

		removed += remove_table_entry(table, entry);

		list = next;
	}

	if (builtin >= 0)
		delete_update_hooks(table, builtin, chain_tail->prev, removed);

	update_offsets(table);

	return 0;
}

static int iptables_add_chain(struct connman_iptables *table,
				const char *name)
{
	GList *last;
	struct ipt_entry *entry_head = NULL;
	struct ipt_entry *entry_return = NULL;
	struct ip6t_entry *entry6_head = NULL;
	struct ip6t_entry *entry6_return = NULL;
	struct connman_iptables_entry entry = { 0 };
	struct xt_error_target *error = NULL;
	struct ipt_standard_target *standard = NULL;
	u_int16_t entry_head_size, entry_return_size;
	size_t entry_struct_size = 0;
	size_t xt_error_target_size = 0;
	size_t standard_target_size = 0;

	DBG("table %s chain %s", table->name, name);

	entry.type = table->type;

	/* Do not allow to add duplicate chains */
	if (find_chain_head(table, name))
		return -EEXIST;

	last = g_list_last(table->entries);

	xt_error_target_size = XT_ALIGN(sizeof(struct xt_error_target));

	/*
	 * An empty chain is composed of:
	 * - A head entry, with no match and an error target.
	 *   The error target data is the chain name.
	 * - A tail entry, with no match and a standard target.
	 *   The standard target verdict is XT_RETURN (return to the
	 *   caller).
	 */

	/* head entry */
	switch (entry.type) {
	case AF_INET:
		entry_struct_size = XT_ALIGN(sizeof(struct ipt_entry));
		entry_head_size = entry_struct_size + xt_error_target_size;

		entry_head = g_try_malloc0(entry_head_size);
		if (!entry_head)
			goto err_head;

		entry_head->target_offset = entry_struct_size;
		entry_head->next_offset = entry_head_size;

		error = (struct xt_error_target *) entry_head->elems;

		entry.entry = entry_head;
		break;
	case AF_INET6:
		entry_struct_size = XT_ALIGN(sizeof(struct ip6t_entry));
		entry_head_size = entry_struct_size + xt_error_target_size;

		entry6_head = g_try_malloc0(entry_head_size);
		if (!entry6_head)
			goto err_head;

		entry6_head->target_offset = entry_struct_size;
		entry6_head->next_offset = entry_head_size;

		error = (struct xt_error_target *) entry6_head->elems;

		entry.entry6 = entry6_head;
		break;
	default:
		return -EINVAL;
	}

	g_stpcpy(error->target.u.user.name, get_error_target(entry.type));
	error->target.u.user.target_size = xt_error_target_size;
	g_stpcpy(error->errorname, name);

	if (iptables_add_entry(table, &entry, last, -1, -1) < 0)
		goto err_head;

	standard_target_size = XT_ALIGN(sizeof(struct ipt_standard_target));
	entry_return_size = entry_struct_size + standard_target_size;

	/* tail entry */
	switch (entry.type) {
	case AF_INET:
		entry_return = g_try_malloc0(entry_return_size);
		if (!entry_return)
			goto err;

		entry_return->target_offset = entry_struct_size;
		entry_return->next_offset = entry_return_size;

		standard = (struct ipt_standard_target *) entry_return->elems;

		entry.entry = entry_return;
		break;
	case AF_INET6:
		entry6_return = g_try_malloc0(entry_return_size);
		if (!entry6_return)
			goto err;

		entry6_return->target_offset = entry_struct_size;
		entry6_return->next_offset = entry_return_size;

		standard = (struct ipt_standard_target *) entry6_return->elems;

		entry.entry6 = entry6_return;
		break;
	}

	standard->target.u.user.target_size = standard_target_size;
	standard->verdict = XT_RETURN;

	if (iptables_add_entry(table, &entry, last, -1, -1) < 0)
		goto err;

	return 0;

err:
	g_free(entry_return);
	g_free(entry6_return);
err_head:
	g_free(entry_head);
	g_free(entry6_head);

	return -ENOMEM;
}

static int iptables_delete_chain(struct connman_iptables *table,
					const char *name)
{
	struct connman_iptables_entry *entry;
	GList *chain_head, *chain_tail;

	DBG("table %s chain %s", table->name, name);

	chain_head = find_chain_head(table, name);
	if (!chain_head)
		return -EINVAL;

	entry = chain_head->data;

	/* We cannot remove builtin chain */
	if (entry->builtin >= 0)
		return -EINVAL;

	chain_tail = find_chain_tail(table, name);
	if (!chain_tail)
		return -EINVAL;

	/* Chain must be flushed */
	if (chain_head->next != chain_tail->prev)
		return -EINVAL;

	remove_table_entry(table, entry);

	entry = chain_tail->prev->data;
	remove_table_entry(table, entry);

	update_offsets(table);

	return 0;
}

static struct connman_iptables_entry *new_rule(struct iptables_ip *ip,
		const char *target_name, struct xtables_target *xt_t,
		struct xtables_rule_match *xt_rm)
{
	struct xtables_rule_match *tmp_xt_rm;
	struct connman_iptables_entry *new_entry;
	size_t match_size, target_size;

	new_entry = g_try_malloc0(sizeof(struct connman_iptables_entry));

	if (!new_entry)
		return NULL;

	new_entry->type = ip->type;

	match_size = 0;
	for (tmp_xt_rm = xt_rm; tmp_xt_rm; tmp_xt_rm = tmp_xt_rm->next)
		match_size += tmp_xt_rm->match->m->u.match_size;

	if (xt_t)
		target_size = xt_t->t->u.target_size;
	else
		target_size = XT_ALIGN(sizeof(struct xt_standard_target));

	switch (ip->type) {
	case AF_INET:
		new_entry->entry = g_try_malloc0(
					XT_ALIGN(sizeof(struct ipt_entry)) +
					target_size + match_size);
		if (!new_entry->entry)
			goto err;

		memcpy(&new_entry->entry->ip, ip->ip, sizeof(struct ipt_ip));

		new_entry->entry->target_offset =
					XT_ALIGN(sizeof(struct ipt_entry)) +
					match_size;
		new_entry->entry->next_offset =
					XT_ALIGN(sizeof(struct ipt_entry)) +
					target_size + match_size;
		break;
	case AF_INET6:
		new_entry->entry6 = g_try_malloc0(
					XT_ALIGN(sizeof(struct ip6t_entry)) +
					target_size + match_size);
		if (!new_entry->entry6)
			goto err;

		memcpy(&new_entry->entry6->ipv6, ip->ip6,
						sizeof(struct ip6t_ip6));

		new_entry->entry6->target_offset =
					XT_ALIGN(sizeof(struct ip6t_entry)) +
					match_size;
		new_entry->entry6->next_offset =
					XT_ALIGN(sizeof(struct ip6t_entry)) +
					target_size + match_size;
		break;
	default:
		goto err;
	}

	match_size = 0;
	for (tmp_xt_rm = xt_rm; tmp_xt_rm;
				tmp_xt_rm = tmp_xt_rm->next) {

		switch (new_entry->type) {
		case AF_INET:
			memcpy(new_entry->entry->elems + match_size,
					tmp_xt_rm->match->m,
					tmp_xt_rm->match->m->u.match_size);
			break;
		case AF_INET6:
			memcpy(new_entry->entry6->elems + match_size,
					tmp_xt_rm->match->m,
					tmp_xt_rm->match->m->u.match_size);
			break;
		}
		match_size += tmp_xt_rm->match->m->u.match_size;
	}

	if (xt_t) {
		struct xt_entry_target *entry_target;

		entry_target = iptables_entry_get_target(new_entry);
		memcpy(entry_target, xt_t->t, target_size);
	}

	return new_entry;

err:
	g_free(new_entry);

	return NULL;
}

static void update_hooks(struct connman_iptables *table, GList *chain_head,
				struct connman_iptables_entry *entry)
{
	GList *list;
	struct connman_iptables_entry *head, *e;
	int builtin;
	u_int16_t next_offset;

	if (!table || !chain_head)
		return;

	head = chain_head->data;

	builtin = head->builtin;
	if (builtin < 0)
		return;

	next_offset = iptables_entry_get_next_offset(entry);

	table->underflow[builtin] += next_offset;

	for (list = chain_head->next; list; list = list->next) {
		e = list->data;

		builtin = e->builtin;
		if (builtin < 0)
			continue;

		table->hook_entry[builtin] += next_offset;
		table->underflow[builtin] += next_offset;
	}
}

static struct connman_iptables_entry *prepare_rule_inclusion(
				struct connman_iptables *table,
				struct iptables_ip *ip,
				const char *chain_name,
				const char *target_name,
				struct xtables_target *xt_t,
				int *builtin,
				struct xtables_rule_match *xt_rm,
				bool insert)
{
	GList *chain_tail, *chain_head;
	struct connman_iptables_entry *head;
	struct connman_iptables_entry *new_entry;

	chain_head = find_chain_head(table, chain_name);
	if (!chain_head)
		return NULL;

	chain_tail = find_chain_tail(table, chain_name);
	if (!chain_tail)
		return NULL;

	new_entry = new_rule(ip, target_name, xt_t, xt_rm);

	switch (new_entry->type) {
	case AF_INET:
		if (new_entry->entry)
			break;
	case AF_INET6:
		if (new_entry->entry6)
			break;
	default:
		goto err;
	}

	update_hooks(table, chain_head, new_entry);

	/*
	 * If the chain is builtin, and does not have any rule,
	 * then the one that we're inserting is becoming the head
	 * and thus needs the builtin flag.
	 */
	head = chain_head->data;
	if (head->builtin < 0)
		*builtin = -1;
	else if (insert || chain_head == chain_tail->prev) {
		*builtin = head->builtin;
		head->builtin = -1;
	}

	return new_entry;

err:
	g_free(new_entry);

	return NULL;
}

static int iptables_append_rule(struct connman_iptables *table,
				struct iptables_ip *ip,
				const char *chain_name,
				const char *target_name,
				struct xtables_target *xt_t,
				struct xtables_rule_match *xt_rm)
{
	struct connman_iptables_entry *new_entry;
	int builtin = -1, ret;
	GList *chain_tail;

	DBG("table %s chain %s", table->name, chain_name);

	chain_tail = find_chain_tail(table, chain_name);
	if (!chain_tail)
		return -EINVAL;

	new_entry = prepare_rule_inclusion(table, ip, chain_name, target_name,
					xt_t, &builtin, xt_rm, false);

	if (!new_entry)
		return -EINVAL;

	switch (new_entry->type) {
	case AF_INET:
		if (new_entry->entry)
			break;
	case AF_INET6:
		if (new_entry->entry6)
			break;
	default:
		ret = -EINVAL;
		goto err;
	}

	ret = iptables_add_entry(table, new_entry, chain_tail->prev,
					builtin, -1);
	if (ret < 0)
		goto err;

	/*
	 * Free only the container, not the content  iptables_add_entry()
	 * allocates new containers for entries.
	 */
	g_free(new_entry);

	return ret;

err:
	iptables_entry_free(new_entry);

	return ret;
}

static int iptables_insert_rule(struct connman_iptables *table,
				struct iptables_ip *ip,
				const char *chain_name,
				const char *target_name,
				struct xtables_target *xt_t,
				struct xtables_rule_match *xt_rm)
{
	struct connman_iptables_entry *new_entry;
	int builtin = -1, ret;
	GList *chain_head;

	DBG("table %s chain %s", table->name, chain_name);

	chain_head = find_chain_head(table, chain_name);
	if (!chain_head)
		return -EINVAL;

	new_entry = prepare_rule_inclusion(table, ip, chain_name, target_name,
					xt_t, &builtin, xt_rm, true);

	if (!new_entry)
		return -EINVAL;

	switch (new_entry->type) {
	case AF_INET:
		if (new_entry->entry)
			break;
	case AF_INET6:
		if (new_entry->entry6)
			break;
	default:
		ret = -EINVAL;
		goto err;
	}

	if (builtin == -1)
		chain_head = chain_head->next;

	ret = iptables_add_entry(table, new_entry, chain_head, builtin, -1);
	if (ret < 0)
		goto err;

	/*
	 * Free only the container, not the content  iptables_add_entry()
	 * allocates new containers for entries.
	 */
	g_free(new_entry);

	return ret;

err:
	iptables_entry_free(new_entry);

	return ret;
}

static bool is_same_ipt_entry(struct ipt_entry *i_e1,
					struct ipt_entry *i_e2)
{
	if (memcmp(&i_e1->ip, &i_e2->ip, sizeof(struct ipt_ip)) != 0)
		return false;

	if (i_e1->target_offset != i_e2->target_offset)
		return false;

	if (i_e1->next_offset != i_e2->next_offset)
		return false;

	return true;
}

/* A copy of is_same_ipt_entry with IPv6 structures */
static bool is_same_ip6t_entry(struct ip6t_entry *i_e1,
					struct ip6t_entry *i_e2)
{
	if (memcmp(&i_e1->ipv6, &i_e2->ipv6, sizeof(struct ip6t_ip6)) != 0)
		return false;

	if (i_e1->target_offset != i_e2->target_offset)
		return false;

	if (i_e1->next_offset != i_e2->next_offset)
		return false;

	return true;
}

static bool is_same_iptables_entry(struct connman_iptables_entry *e1,
						struct connman_iptables_entry *e2)
{
	if (e1->type != e2->type)
		return false;

	switch (e1->type) {
	case AF_INET:
		return is_same_ipt_entry(e1->entry, e2->entry);
	case AF_INET6:
		return is_same_ip6t_entry(e1->entry6, e2->entry6);
	}

	return false;
}

static bool is_same_target(struct xt_entry_target *xt_e_t1,
					struct xt_entry_target *xt_e_t2)
{
	unsigned int i;

	if (!xt_e_t1 || !xt_e_t2)
		return false;

	if (g_strcmp0(xt_e_t1->u.user.name, "") == 0 &&
			g_strcmp0(xt_e_t2->u.user.name, "") == 0) {
		/* fallthrough */
		return true;

	/*
	 * IPT_STANDARD_TARGET and IP6T_STANDARD_TARGET are defined by
	 * XT_STANDARD_TARGET
	 */
	} else if (g_strcmp0(xt_e_t1->u.user.name, XT_STANDARD_TARGET) == 0) {
		struct xt_standard_target *xt_s_t1;
		struct xt_standard_target *xt_s_t2;

		xt_s_t1 = (struct xt_standard_target *) xt_e_t1;
		xt_s_t2 = (struct xt_standard_target *) xt_e_t2;

		if (xt_s_t1->verdict != xt_s_t2->verdict)
			return false;
	} else {
		if (xt_e_t1->u.target_size != xt_e_t2->u.target_size)
			return false;

		if (g_strcmp0(xt_e_t1->u.user.name, xt_e_t2->u.user.name) != 0)
			return false;

		for (i = 0; i < xt_e_t1->u.target_size -
				sizeof(struct xt_standard_target); i++) {
			if ((xt_e_t1->data[i] ^ xt_e_t2->data[i]) != 0)
				return false;
		}
	}

	return true;
}

static bool is_same_match(struct xt_entry_match *xt_e_m1,
				struct xt_entry_match *xt_e_m2)
{
	unsigned int i;

	if (!xt_e_m1 || !xt_e_m2)
		return false;

	if (xt_e_m1->u.match_size != xt_e_m2->u.match_size)
		return false;

	if (xt_e_m1->u.user.revision != xt_e_m2->u.user.revision)
		return false;

	if (g_strcmp0(xt_e_m1->u.user.name, xt_e_m2->u.user.name) != 0)
		return false;

	for (i = 0; i < xt_e_m1->u.match_size - sizeof(struct xt_entry_match);
			i++) {
		if ((xt_e_m1->data[i] ^ xt_e_m2->data[i]) != 0)
			return false;
	}

	return true;
}

static GList *find_existing_rule(struct connman_iptables *table,
				struct iptables_ip *ip,
				const char *chain_name,
				const char *target_name,
				struct xtables_target *xt_t,
				GList *matches,
				struct xtables_rule_match *xt_rm)
{
	GList *chain_tail, *chain_head, *list;
	struct xt_entry_target *xt_e_t = NULL;
	struct xt_entry_match *xt_e_m = NULL;
	struct connman_iptables_entry *entry;
	struct connman_iptables_entry *entry_test;
	int builtin;

	chain_head = find_chain_head(table, chain_name);
	if (!chain_head)
		return NULL;

	chain_tail = find_chain_tail(table, chain_name);
	if (!chain_tail)
		return NULL;

	if (!xt_t && !matches)
		return NULL;

	entry_test = new_rule(ip, target_name, xt_t, xt_rm);

	switch (entry_test->type) {
	case AF_INET:
		if (!entry_test->entry)
			return NULL;
		break;
	case AF_INET6:
		if (!entry_test->entry6)
			return NULL;
		break;
	default:
		return NULL;
	}

	if (xt_t)
		xt_e_t = iptables_entry_get_target(entry_test);
	if (matches)
		xt_e_m = (struct xt_entry_match *)
				iptables_entry_get_elems(entry_test);

	entry = chain_head->data;
	builtin = entry->builtin;

	if (builtin >= 0)
		list = chain_head;
	else
		list = chain_head->next;

	for (; list != chain_tail->prev; list = list->next) {
		struct connman_iptables_entry *tmp;

		tmp = list->data;

		if (!is_same_iptables_entry(entry_test, tmp))
			continue;

		if (xt_t) {
			struct xt_entry_target *tmp_xt_e_t = NULL;

			tmp_xt_e_t = iptables_entry_get_target(tmp);

			if (!is_same_target(tmp_xt_e_t, xt_e_t))
				continue;
		}

		if (matches) {
			struct xt_entry_match *tmp_xt_e_m;

			tmp_xt_e_m = (struct xt_entry_match *)
					iptables_entry_get_elems(tmp);

			if (!is_same_match(tmp_xt_e_m, xt_e_m))
				continue;
		}

		break;
	}

	iptables_entry_free(entry_test);

	if (list != chain_tail->prev)
		return list;

	return NULL;
}

static int iptables_delete_rule(struct connman_iptables *table,
				struct iptables_ip *ip,
				const char *chain_name,
				const char *target_name,
				struct xtables_target *xt_t,
				GList *matches,
				struct xtables_rule_match *xt_rm)
{
	struct connman_iptables_entry *entry;
	GList *chain_head, *chain_tail, *list;
	int builtin, removed;

	DBG("table %s chain %s", table->name, chain_name);

	removed = 0;

	chain_head = find_chain_head(table, chain_name);
	if (!chain_head)
		return -EINVAL;

	chain_tail = find_chain_tail(table, chain_name);
	if (!chain_tail)
		return -EINVAL;

	list = find_existing_rule(table, ip, chain_name, target_name,
						xt_t, matches, xt_rm);

	if (!list)
		return -EINVAL;

	entry = chain_head->data;
	builtin = entry->builtin;

	if (builtin >= 0 && list == chain_head) {
		/*
		 * We are about to remove the first rule in the
		 * chain. In this case we need to store the builtin
		 * value to the new chain_head.
		 *
		 * Note, for builtin chains, chain_head->next is
		 * always valid. A builtin chain has always a policy
		 * rule at the end.
		 */
		chain_head = chain_head->next;

		entry = chain_head->data;
		entry->builtin = builtin;
	}

	entry = list->data;
	if (!entry)
		return -EINVAL;

	/* We have deleted a rule,
	 * all references should be bumped accordingly */
	if (list->next)
		update_targets_reference(table, list->next->data,
						list->data, true);

	removed += remove_table_entry(table, entry);

	if (builtin >= 0)
		delete_update_hooks(table, builtin, chain_head, removed);

	update_offsets(table);

	return 0;
}

static int iptables_change_policy(struct connman_iptables *table,
				const char *chain_name, const char *policy)
{
	GList *chain_head, *chain_tail;
	struct connman_iptables_entry *entry;
	struct xt_entry_target *target;
	struct xt_standard_target *t;
	int verdict;

	DBG("table %s chain %s policy %s", table->name, chain_name, policy);

	verdict = target_to_verdict(policy);
	switch (verdict) {
	case -NF_ACCEPT - 1:
	case -NF_DROP - 1:
		break;
	default:
		return -EINVAL;
	}

	chain_head = find_chain_head(table, chain_name);
	if (!chain_head)
		return -EINVAL;

	entry = chain_head->data;
	if (entry->builtin < 0)
		return -EINVAL;

	chain_tail = find_chain_tail(table, chain_name);
	if (!chain_tail)
		return -EINVAL;

	entry = chain_tail->prev->data;

	target = iptables_entry_get_target(entry);

	if (!target)
		return -EINVAL;

	t = (struct xt_standard_target *)target;
	if (t->verdict != verdict)
		entry->counter_idx = -1;
	t->verdict = verdict;

	return 0;
}

static struct ipt_replace *iptables_blob(struct connman_iptables *table)
{
	struct ipt_replace *r;
	GList *list;
	struct connman_iptables_entry *e;
	unsigned char *entry_index;

	r = g_try_malloc0(sizeof(struct ipt_replace) + table->size);
	if (!r)
		return NULL;

	memset(r, 0, sizeof(*r) + table->size);

	r->counters = g_try_malloc0(sizeof(struct xt_counters)
				* table->old_entries);
	if (!r->counters) {
		g_free(r);
		return NULL;
	}

	g_stpcpy(r->name, table->info->name);
	r->num_entries = table->num_entries;
	r->size = table->size;

	r->num_counters = table->old_entries;
	r->valid_hooks  = table->info->valid_hooks;

	memcpy(r->hook_entry, table->hook_entry, sizeof(table->hook_entry));
	memcpy(r->underflow, table->underflow, sizeof(table->underflow));

	entry_index = (unsigned char *)r->entries;
	for (list = table->entries; list; list = list->next) {
		e = list->data;

		memcpy(entry_index, e->entry, e->entry->next_offset);
		entry_index += e->entry->next_offset;
	}

	return r;
}

/* A copy of iptables_blob() with IPv6 structures */
static struct ip6t_replace *ip6tables_blob(struct connman_iptables *table)
{
	struct ip6t_replace *r;
	GList *list;
	struct connman_iptables_entry *e;
	unsigned char *entry_index;

	r = g_try_malloc0(sizeof(struct ip6t_replace) + table->size);
	if (!r)
		return NULL;

	memset(r, 0, sizeof(*r) + table->size);

	r->counters = g_try_malloc0(sizeof(struct xt_counters)
				* table->old_entries);
	if (!r->counters) {
		g_free(r);
		return NULL;
	}

	g_stpcpy(r->name, table->info6->name);
	r->num_entries = table->num_entries;
	r->size = table->size;

	r->num_counters = table->old_entries;
	r->valid_hooks  = table->info6->valid_hooks;

	memcpy(r->hook_entry, table->hook_entry, sizeof(table->hook_entry));
	memcpy(r->underflow, table->underflow, sizeof(table->underflow));

	entry_index = (unsigned char *)r->entries;
	for (list = table->entries; list; list = list->next) {
		e = list->data;

		memcpy(entry_index, e->entry6, e->entry6->next_offset);
		entry_index += e->entry6->next_offset;
	}

	return r;
}

static void dump_ip(struct connman_iptables_entry *entry)
{
	char *iniface, *outiface;
	char ip_string[INET6_ADDRSTRLEN];
	char ip_mask[INET6_ADDRSTRLEN];

	switch (entry->type) {
	case AF_INET:
		iniface = entry->entry->ip.iniface;
		outiface = entry->entry->ip.outiface;
		break;
	case AF_INET6:
		iniface = entry->entry6->ipv6.iniface;
		outiface = entry->entry6->ipv6.outiface;
		break;
	default:
		return;
	}

	if (strlen(iniface))
		DBG("\tin %s", iniface);

	if (strlen(outiface))
		DBG("\tout %s", outiface);

	if (entry->type == AF_INET) {
		if (inet_ntop(entry->type, &entry->entry->ip.src, ip_string,
				INET6_ADDRSTRLEN) && inet_ntop(entry->type,
				&entry->entry->ip.smsk, ip_mask,
				INET6_ADDRSTRLEN))
			DBG("\tsrc %s/%s", ip_string, ip_mask);

		if (inet_ntop(entry->type, &entry->entry->ip.dst, ip_string,
				INET6_ADDRSTRLEN) && inet_ntop(entry->type,
				&entry->entry->ip.dmsk, ip_mask,
				INET6_ADDRSTRLEN))
			DBG("\tdst %s/%s", ip_string, ip_mask);
	}

	if (entry->type == AF_INET6) {
		if (inet_ntop(entry->type, &entry->entry6->ipv6.src, ip_string,
				INET6_ADDRSTRLEN) && inet_ntop(entry->type,
				&entry->entry6->ipv6.smsk, ip_mask,
				INET6_ADDRSTRLEN))
			DBG("\tsrc %s/%s", ip_string, ip_mask);

		if (inet_ntop(entry->type, &entry->entry6->ipv6.dst, ip_string,
				INET6_ADDRSTRLEN) && inet_ntop(entry->type,
				&entry->entry6->ipv6.dmsk, ip_mask,
				INET6_ADDRSTRLEN))
			DBG("\tdst %s/%s", ip_string, ip_mask);
	}
}

static void dump_target(struct connman_iptables_entry *entry)
{
	struct xtables_target *xt_t;
	struct xt_entry_target *target;
	int err;

	target = iptables_entry_get_target(entry);

	if (!target)
		return;

	if (!g_strcmp0(target->u.user.name, get_standard_target(entry->type))) {
		struct xt_standard_target *t;

		t = (struct xt_standard_target *)target;

		switch (t->verdict) {
		case XT_RETURN:
			DBG("\ttarget RETURN");
			break;

		case -NF_ACCEPT - 1:
			DBG("\ttarget ACCEPT");
			break;

		case -NF_DROP - 1:
			DBG("\ttarget DROP");
			break;

		case -NF_QUEUE - 1:
			DBG("\ttarget QUEUE");
			break;

		case -NF_STOP - 1:
			DBG("\ttarget STOP");
			break;

		default:
			DBG("\tJUMP %u", t->verdict);
			break;
		}

		enable_jmp();

		if ((err = setjmp(env_state)) != 0) {
			DBG("setjmp() called by longjmp() with value %d", err);
			disable_jmp();
			return;
		}

		xt_t = xtables_find_target(get_standard_target(entry->type),
						XTF_LOAD_MUST_SUCCEED);

		disable_jmp();

		if (xt_t->print)
			xt_t->print(NULL, target, 1);
	} else {
		enable_jmp();

		if ((err = setjmp(env_state)) != 0) {
			DBG("setjmp() called by longjmp() with value %d", err);
			disable_jmp();
			return;
		}

		xt_t = xtables_find_target(target->u.user.name, XTF_TRY_LOAD);

		disable_jmp();

		if (!xt_t) {
			DBG("\ttarget %s", target->u.user.name);
			return;
		}

		if (xt_t->print) {
			DBG("\ttarget ");
			xt_t->print(NULL, target, 1);
		}
	}

	if (xt_t == xt_t->next)
		free(xt_t);
}

static void dump_match(struct connman_iptables_entry *entry)
{
	struct xtables_match *xt_m;
	struct xt_entry_match *match;
	u_int16_t target_offset;
	int err;

	target_offset = iptables_entry_get_target_offset(entry);

	switch (entry->type) {
	case AF_INET:
		if (entry->entry->elems == (unsigned char *)entry->entry +
				target_offset)
			return;
		break;
	case AF_INET6:
		if (entry->entry6->elems == (unsigned char *)entry->entry6 +
				target_offset)
			return;
		break;
	default:
		return;
	}

	match = (struct xt_entry_match *) iptables_entry_get_elems(entry);

	if (!strlen(match->u.user.name))
		return;

	enable_jmp();

	if ((err = setjmp(env_state)) != 0) {
		DBG("setjmp() called by longjmp() with value %d", err);
		disable_jmp();
		return;
	}

	xt_m = xtables_find_match(match->u.user.name, XTF_TRY_LOAD, NULL);

	disable_jmp();

	if (!xt_m)
		goto out;

	if (xt_m->print) {
		DBG("\tmatch ");
		xt_m->print(NULL, match, 1);

		return;
	}
	if (xt_m == xt_m->next)
		free(xt_m);

out:
	DBG("\tmatch %s", match->u.user.name);

}

static int dump_entry(struct connman_iptables_entry *entry, int builtin,
			unsigned int hook, size_t size, unsigned int offset,
			void *user_data)
{
	struct xt_entry_target *target;
	char *char_entry;

	target = iptables_entry_get_target(entry);

	if (!target)
		return -EINVAL;

	if (offset + iptables_entry_get_next_offset(entry) == size) {
		DBG("\tEnd of CHAIN");
		return 0;
	}

	switch (entry->type) {
	case AF_INET:
		char_entry = (char *)entry->entry;
		break;
	case AF_INET6:
		char_entry = (char *)entry->entry6;
		break;
	default:
		return 0;
	}

	if (!g_strcmp0(target->u.user.name, IPT_ERROR_TARGET)) {
		DBG("\tUSER CHAIN (%s) match %p  target %p",
			target->data, iptables_entry_get_elems(entry),
			char_entry + iptables_entry_get_target_offset(entry));

		return 0;
	} else if (builtin >= 0) {
		DBG("\tCHAIN (%s) match %p  target %p",
			hooknames[builtin], iptables_entry_get_elems(entry),
			char_entry + iptables_entry_get_target_offset(entry));
	} else {
		DBG("\tRULE  match %p  target %p",
			iptables_entry_get_elems(entry),
			char_entry + iptables_entry_get_target_offset(entry));
	}

	dump_match(entry);
	dump_target(entry);
	dump_ip(entry);

	return 0;
}

static void dump_table(struct connman_iptables *table)
{
	struct connman_iptables_entry entry = { 0 };
	unsigned int *hook_entry;
	unsigned int *underflow;
	unsigned int valid_hooks;
	unsigned int size;

	hook_entry = iptables_table_get_info_hook_entry(table);
	underflow = iptables_table_get_info_underflow(table);
	valid_hooks = iptables_table_get_info_valid_hooks(table);
	size = iptables_table_get_info_size(table);
	
	DBG("%s valid_hooks=0x%08x, num_entries=%u, size=%u",
		iptables_table_get_info_name(table),
		valid_hooks,
		iptables_table_get_info_num_entries(table),
		size);

	DBG("entry hook: pre/in/fwd/out/post %d/%d/%d/%d/%d",
		hook_entry[NF_IP_PRE_ROUTING],
		hook_entry[NF_IP_LOCAL_IN],
		hook_entry[NF_IP_FORWARD],
		hook_entry[NF_IP_LOCAL_OUT],
		hook_entry[NF_IP_POST_ROUTING]);
	DBG("underflow:  pre/in/fwd/out/post %d/%d/%d/%d/%d",
		underflow[NF_IP_PRE_ROUTING],
		underflow[NF_IP_LOCAL_IN],
		underflow[NF_IP_FORWARD],
		underflow[NF_IP_LOCAL_OUT],
		underflow[NF_IP_POST_ROUTING]);

	entry.type = table->type;

	switch (table->type) {
	case AF_INET:
		entry.entry = table->blob_entries->entrytable;
		break;
	case AF_INET6:
		entry.entry6 = table->blob_entries6->entrytable;
	}

	iterate_entries(&entry,
		valid_hooks,
		hook_entry,
		underflow,
		size,
		print_entry, dump_entry);
}

static const char *iptables_replace_get_name(struct iptables_replace *replace)
{
	if (!replace)
		return NULL;

	switch (replace->type) {
	case AF_INET:
		return replace->r->name;
	case AF_INET6:
		return replace->r6->name;
	}

	return NULL;
}

static unsigned int iptables_replace_get_valid_hooks(
					struct iptables_replace *replace)
{
	if (!replace)
		return 0;

	switch (replace->type) {
	case AF_INET:
		return replace->r->valid_hooks;
	case AF_INET6:
		return replace->r6->valid_hooks;
	}

	return 0;
}

static unsigned int iptables_replace_get_num_entries(
					struct iptables_replace *replace)
{
	if (!replace)
		return 0;

	switch (replace->type) {
	case AF_INET:
		return replace->r->num_entries;
	case AF_INET6:
		return replace->r6->num_entries;
	}

	return 0;
}

static unsigned int *iptables_replace_get_hook_entry(
					struct iptables_replace *replace)
{
	if (!replace)
		return NULL;

	switch (replace->type) {
	case AF_INET:
		return replace->r->hook_entry;
	case AF_INET6:
		return replace->r6->hook_entry;
	}

	return NULL;
}

static unsigned int *iptables_replace_get_underflow(
					struct iptables_replace *replace)
{
	if (!replace)
		return NULL;

	switch (replace->type) {
	case AF_INET:
		return replace->r->underflow;
	case AF_INET6:
		return replace->r6->underflow;
	}

	return NULL;
}

static unsigned int iptables_replace_get_size(struct iptables_replace *replace)
{
	if (!replace)
		return 0;

	switch (replace->type) {
	case AF_INET:
		return replace->r->size;
	case AF_INET6:
		return replace->r6->size;
	}

	return 0;
}

static void dump_replace(struct iptables_replace *repl)
{
	struct connman_iptables_entry entry = { 0 };
	unsigned int *hook_entry;
	unsigned int *underflow;
	unsigned int valid_hooks;
	unsigned int size;
	
	hook_entry = iptables_replace_get_hook_entry(repl);
	underflow = iptables_replace_get_underflow(repl);
	valid_hooks = iptables_replace_get_valid_hooks(repl);
	size = iptables_replace_get_size(repl);

	switch (repl->type) {
	case AF_INET:
		entry.entry = repl->r->entries;
		break;
	case AF_INET6:
		entry.entry6 = repl->r6->entries;
		break;
	default:
		return;
	}

	DBG("%s valid_hooks 0x%08x  num_entries %u  size %u",
			iptables_replace_get_name(repl),
			valid_hooks,
			iptables_replace_get_num_entries(repl), size);

	DBG("entry hook: pre/in/fwd/out/post %d/%d/%d/%d/%d",
		hook_entry[NF_IP_PRE_ROUTING],
		hook_entry[NF_IP_LOCAL_IN],
		hook_entry[NF_IP_FORWARD],
		hook_entry[NF_IP_LOCAL_OUT],
		hook_entry[NF_IP_POST_ROUTING]);
	DBG("underflow:  pre/in/fwd/out/post %d/%d/%d/%d/%d",
		underflow[NF_IP_PRE_ROUTING],
		underflow[NF_IP_LOCAL_IN],
		underflow[NF_IP_FORWARD],
		underflow[NF_IP_LOCAL_OUT],
		underflow[NF_IP_POST_ROUTING]);

	iterate_entries(&entry, valid_hooks, hook_entry, underflow,
			size, print_entry, dump_entry);
}

static int iptables_get_entries(struct connman_iptables *table)
{
	socklen_t entry_size;
	int err;

	switch (table->type) {
	case AF_INET:
		entry_size = sizeof(struct ipt_get_entries) + table->info->size;

		err = getsockopt(table->ipt_sock, IPPROTO_IP,
				IPT_SO_GET_ENTRIES, table->blob_entries,
				&entry_size);
		break;
	case AF_INET6:
		entry_size = sizeof(struct ip6t_get_entries) +
					table->info6->size;

		err = getsockopt(table->ipt_sock, IPPROTO_IPV6,
				IP6T_SO_GET_ENTRIES, table->blob_entries6,
				&entry_size);
		break;
	default:
		return -EINVAL;
	}

	if (err < 0)
		return -errno;

	return 0;
}

static int iptables_replace(struct connman_iptables *table,
					struct iptables_replace *r)
{
	int err;

	switch (r->type) {
	case AF_INET:
		if (!r->r)
			return -EINVAL;

		err = setsockopt(table->ipt_sock, IPPROTO_IP,
				IPT_SO_SET_REPLACE, r->r,
				sizeof(*r->r) + r->r->size);
		break;
	case AF_INET6:
		if (!r->r6)
			return -EINVAL;

		err = setsockopt(table->ipt_sock, IPPROTO_IPV6,
				IP6T_SO_SET_REPLACE, r->r6,
				sizeof(*r->r6) + r->r6->size);
		break;
	default:
		return -EINVAL;
	}

	if (err < 0)
		return -errno;

	return 0;
}

static int iptables_add_counters(struct connman_iptables *table,
		struct xt_counters_info *c)
{
	int err;
	int level;
	int optname;

	switch (table->type) {
	case AF_INET:
		level = IPPROTO_IP;
		optname = IPT_SO_SET_ADD_COUNTERS;
		break;
	case AF_INET6:
		level = IPPROTO_IPV6;
		optname = IP6T_SO_SET_ADD_COUNTERS;
		break;
	default:
		return -EINVAL;
	}

	err = setsockopt(table->ipt_sock, level, optname, c,
		sizeof(*c) + sizeof(struct xt_counters) * c->num_counters);

	if (err < 0)
		return -errno;

	return 0;
}

static int add_entry(struct connman_iptables_entry *entry, int builtin,
			unsigned int hook, size_t size, unsigned offset,
			void *user_data)
{
	struct connman_iptables *table = user_data;
	struct connman_iptables_entry new_entry = { 0 };
	u_int16_t next_offset;
	
	new_entry.type = entry->type;
	next_offset = iptables_entry_get_next_offset(entry);

	switch (entry->type) {
	case AF_INET:
		new_entry.entry = g_try_malloc0(next_offset);
		if (!new_entry.entry)
			return -ENOMEM;

		memcpy(new_entry.entry, entry->entry, next_offset);
		break;
	case AF_INET6:
		new_entry.entry6 = g_try_malloc0(next_offset);
		if (!new_entry.entry6)
			return -ENOMEM;

		memcpy(new_entry.entry6, entry->entry6, next_offset);
		break;
	default:
		return -EINVAL;
	}
	
	return iptables_add_entry(table, &new_entry, NULL, builtin,
				table->num_entries);
}

static void table_cleanup(struct connman_iptables *table)
{
	GList *list;
	struct connman_iptables_entry *entry;

	if (!table)
		return;

	if (table->ipt_sock >= 0)
		close(table->ipt_sock);

	for (list = table->entries; list; list = list->next) {
		entry = list->data;

		iptables_entry_free(entry);
	}

	g_list_free(table->entries);
	g_free(table->name);
	
	if (table->type == AF_INET) {
		g_free(table->info);
		g_free(table->blob_entries);
	}

	if (table->type == AF_INET6) {
		g_free(table->info6);
		g_free(table->blob_entries6);
	}

	g_free(table);
}

static int setup_xtables(int type);
static void reset_xtables();

static struct connman_iptables *iptables_init(int type, const char *table_name)
{
	struct connman_iptables *table = NULL;
	struct connman_iptables_entry entry = { 0 };
	char *iptables_mod = NULL;
	char *module = NULL;
	socklen_t s;

	switch(type) {
	case AF_INET:
		iptables_mod = g_strdup("ip_tables");
		module = g_strconcat("iptable_", table_name, NULL);
		break;
	case AF_INET6:
		iptables_mod = g_strdup("ip6_tables");
		module = g_strconcat("ip6table_", table_name, NULL);
		break;
	default:
		return NULL;
	}

	DBG("%d %s", type, table_name);

	if (setup_xtables(type))
		return NULL;

	if (xtables_insmod(iptables_mod, NULL, TRUE) != 0)
		DBG("%s module loading gives error but trying anyway",
				iptables_mod);

	g_free(iptables_mod);

	if (xtables_insmod(module, NULL, TRUE) != 0)
		DBG("%s module loading gives error but trying anyway", module);

	g_free(module);

	table = g_try_new0(struct connman_iptables, 1);
	if (!table)
		return NULL;

	table->type = entry.type = type;

	table->ipt_sock = socket(type, SOCK_RAW | SOCK_CLOEXEC, IPPROTO_RAW);
	if (table->ipt_sock < 0)
		goto err;

	switch (type) {
	case AF_INET:
		table->info = g_try_new0(struct ipt_getinfo, 1);
		if (!table->info)
			goto err;

		s = sizeof(*table->info);
		g_stpcpy(table->info->name, table_name);

		if (getsockopt(table->ipt_sock, IPPROTO_IP, IPT_SO_GET_INFO,
							table->info, &s) < 0) {
			connman_error("iptables support missing error %d (%s)",
							errno, strerror(errno));
			goto err;
		}

		table->blob_entries = g_try_malloc0(
					sizeof(struct ipt_get_entries) +
					table->info->size);
		if (!table->blob_entries)
			goto err;

		g_stpcpy(table->blob_entries->name, table_name);
		table->blob_entries->size = table->info->size;

		break;
	case AF_INET6:
		table->info6 = g_try_new0(struct ip6t_getinfo, 1);
		if (!table->info6)
			goto err;

		s = sizeof(*table->info6);
		g_stpcpy(table->info6->name, table_name);

		if (getsockopt(table->ipt_sock, IPPROTO_IPV6, IP6T_SO_GET_INFO,
						table->info6, &s) < 0) {
			connman_error("ip6tables support missing error %d (%s)",
					errno, strerror(errno));
			goto err;
		}

		table->blob_entries6 = g_try_malloc0(
					sizeof(struct ip6t_get_entries) +
					table->info6->size);
		if (!table->blob_entries6)
			goto err;

		g_stpcpy(table->blob_entries6->name, table_name);
		table->blob_entries6->size = table->info6->size;

		break;
	}

	if (iptables_get_entries(table) < 0)
		goto err;

	table->num_entries = 0;
	table->size = 0;

	switch (type) {
	case AF_INET:
		table->old_entries = table->info->num_entries;

		memcpy(table->underflow, table->info->underflow,
					sizeof(table->info->underflow));
		memcpy(table->hook_entry, table->info->hook_entry,
					sizeof(table->info->hook_entry));

		entry.entry = table->blob_entries->entrytable;
		break;
	case AF_INET6:
		table->old_entries = table->info6->num_entries;

		memcpy(table->underflow, table->info6->underflow,
				sizeof(table->info6->underflow));
		memcpy(table->hook_entry, table->info6->hook_entry,
					sizeof(table->info6->hook_entry));

		entry.entry6 = table->blob_entries6->entrytable;
		break;
	}

	iterate_entries(&entry,
			iptables_table_get_info_valid_hooks(table),
			iptables_table_get_info_hook_entry(table),
			iptables_table_get_info_underflow(table),
			iptables_table_get_entries_size(table),
			add_entry,
			table);

	if (debug_enabled)
		dump_table(table);

	reset_xtables();

	return table;

err:
	table_cleanup(table);
	reset_xtables();

	return NULL;
}

static struct option iptables_opts[] = {
	{.name = "append",        .has_arg = 1, .val = 'A'},
	{.name = "compare",       .has_arg = 1, .val = 'C'},
	{.name = "delete",        .has_arg = 1, .val = 'D'},
	{.name = "flush-chain",   .has_arg = 1, .val = 'F'},
	{.name = "insert",        .has_arg = 1, .val = 'I'},
	{.name = "list",          .has_arg = 2, .val = 'L'},
	{.name = "new-chain",     .has_arg = 1, .val = 'N'},
	{.name = "policy",        .has_arg = 1, .val = 'P'},
	{.name = "delete-chain",  .has_arg = 1, .val = 'X'},
	{.name = "destination",   .has_arg = 1, .val = 'd'},
	{.name = "in-interface",  .has_arg = 1, .val = 'i'},
	{.name = "jump",          .has_arg = 1, .val = 'j'},
	{.name = "match",         .has_arg = 1, .val = 'm'},
	{.name = "out-interface", .has_arg = 1, .val = 'o'},
	{.name = "source",        .has_arg = 1, .val = 's'},
	{.name = "table",         .has_arg = 1, .val = 't'},
	{.name = "protocol",      .has_arg = 1, .val = 'p'},
	{NULL},
};

void iptables_exit(enum xtables_exittype status, const char *msg, ...)
			__attribute__((noreturn, format(printf,2,3)));

void iptables_exit(enum xtables_exittype status, const char *msg, ...)
{
	va_list args;
	gchar str[256] = { 0 };

	switch (status) {
	case OTHER_PROBLEM:
		DBG("OTHER_PROBLEM");
		break;
	case PARAMETER_PROBLEM:
		DBG("PARAMETER_PROBLEM");
		break;
	case VERSION_PROBLEM:
		DBG("VERSION_PROBLEM");
		break;
	case RESOURCE_PROBLEM:
		DBG("RESOURCE_PROBLEM");
		break;
	case XTF_ONLY_ONCE:
		DBG("XTF_ONLY_ONCE");
		break;
	case XTF_NO_INVERT:
		DBG("XTF_NO_INVERT");
		break;
	case XTF_BAD_VALUE:
		DBG("XTF_BAD_VALUE");
		break;
	case XTF_ONE_ACTION:
		DBG("XTF_ONE_ACTION");
		break;
	}

	va_start(args, msg);
	vsnprintf(str, 256, msg, args);
	va_end(args);

	connman_error("iptables rule error: %s", str);

	if (can_jmp()) {
		DBG("calling longjmp()");
		 /* enum xtables_exittype begins from 1 */
		longjmp(env_state, status);
	}

	connman_error("exit because of iptables error");

	exit(status);
}

struct xtables_globals iptables_globals = {
	.option_offset = 0,
	.opts = iptables_opts,
	.orig_opts = iptables_opts,
	.exit_err = iptables_exit,
#if XTABLES_VERSION_CODE > 10
	.compat_rev = xtables_compatible_revision,
#endif
};

struct xtables_globals ip6tables_globals = {
	.option_offset = 0,
	.opts = iptables_opts,
	.orig_opts = iptables_opts,
	.exit_err = iptables_exit,
#if XTABLES_VERSION_CODE > 10
	.compat_rev = xtables_compatible_revision,
#endif
};

static struct xtables_target *prepare_target(struct connman_iptables *table,
							const char *target_name)
{
	struct xtables_target *xt_t = NULL;
	bool is_builtin, is_user_defined;
	GList *chain_head = NULL;
	size_t target_size;
	int err;

	is_builtin = false;
	is_user_defined = false;
	
	DBG("target %s", target_name);
	
	if (!table)
		return NULL;

	if (is_builtin_target(target_name))
		is_builtin = true;
	else {
		chain_head = find_chain_head(table, target_name);
		if (chain_head && chain_head->next)
			is_user_defined = true;
	}

	enable_jmp();

	if ((err = setjmp(env_state)) != 0) {
		DBG("setjmp() called by longjmp() with value %d", err);
		disable_jmp();
		return NULL;
	}

	if (is_builtin || is_user_defined)
		xt_t = xtables_find_target(get_standard_target(table->type),
						XTF_LOAD_MUST_SUCCEED);
	else 
		xt_t = xtables_find_target(target_name, XTF_TRY_LOAD);

	disable_jmp();

	if (!xt_t)
		return NULL;

	switch (table->type) {
	case AF_INET:
		target_size = XT_ALIGN(sizeof(struct ipt_entry_target)) +
					xt_t->size;
		break;
	case AF_INET6:
		target_size = XT_ALIGN(sizeof(struct ip6t_entry_target)) +
					xt_t->size;
		break;
	default:
		return NULL;
	}

	xt_t->t = g_try_malloc0(target_size);
	if (!xt_t->t)
		return NULL;

	xt_t->t->u.target_size = target_size;

	if (is_builtin || is_user_defined) {
		struct xt_standard_target *target;

		target = (struct xt_standard_target *)(xt_t->t);
		g_stpcpy(target->target.u.user.name,
				get_standard_target(table->type));

		if (is_builtin)
			target->verdict = target_to_verdict(target_name);
		else if (is_user_defined) {
			struct connman_iptables_entry *target_rule;

			target_rule = chain_head->next->data;
			target->verdict = target_rule->offset;
		}
	} else {
		g_stpcpy(xt_t->t->u.user.name, target_name);
		xt_t->t->u.user.revision = xt_t->revision;
		if (xt_t->init)
			xt_t->init(xt_t->t);
	}

	switch (table->type) {
	case AF_INET:
		if (xt_t->x6_options)
			iptables_globals.opts =
				xtables_options_xfrm(
					iptables_globals.orig_opts,
					iptables_globals.opts,
					xt_t->x6_options,
					&xt_t->option_offset);
		else
			iptables_globals.opts =
				xtables_merge_options(
					iptables_globals.orig_opts,
					iptables_globals.opts,
					xt_t->extra_opts,
					&xt_t->option_offset);

		if (!iptables_globals.opts) {
			g_free(xt_t->t);
			xt_t = NULL;
		}

		break;
	case AF_INET6:
		if (xt_t->x6_options)
			ip6tables_globals.opts =
				xtables_options_xfrm(
					ip6tables_globals.orig_opts,
					ip6tables_globals.opts,
					xt_t->x6_options,
					&xt_t->option_offset);
		else
			ip6tables_globals.opts =
				xtables_merge_options(
					ip6tables_globals.orig_opts,
					ip6tables_globals.opts,
					xt_t->extra_opts,
					&xt_t->option_offset);

		if (!ip6tables_globals.opts) {
			g_free(xt_t->t);
			xt_t = NULL;
		}

		break;
	}

	return xt_t;
}

static struct xtables_match *prepare_matches(struct connman_iptables *table,
					struct xtables_rule_match **xt_rm,
					const char *match_name)
{
	struct xtables_match *xt_m;
	size_t match_size;
	int err;

	if (!table || !match_name)
		return NULL;

	enable_jmp();

	if ((err = setjmp(env_state)) != 0) {
		DBG("setjmp() called by longjmp() with value %d", err);
		disable_jmp();
		return NULL;
	}

	xt_m = xtables_find_match(match_name, XTF_LOAD_MUST_SUCCEED, xt_rm);

	disable_jmp();

	switch (table->type) {
	case AF_INET:
		match_size = XT_ALIGN(sizeof(struct ipt_entry_match)) +
						xt_m->size;
		break;
	case AF_INET6:
		match_size = XT_ALIGN(sizeof(struct ip6t_entry_match)) +
						xt_m->size;
		break;
	default:
		return NULL;
	}

	xt_m->m = g_try_malloc0(match_size);
	if (!xt_m->m)
		return NULL;

	xt_m->m->u.match_size = match_size;
	g_stpcpy(xt_m->m->u.user.name, xt_m->name);
	xt_m->m->u.user.revision = xt_m->revision;

	if (xt_m->init)
		xt_m->init(xt_m->m);

	switch (table->type) {
	case AF_INET:
		if (xt_m->x6_options)
			iptables_globals.opts =
				xtables_options_xfrm(
					iptables_globals.orig_opts,
					iptables_globals.opts,
					xt_m->x6_options,
					&xt_m->option_offset);
		else
			iptables_globals.opts =
				xtables_merge_options(
					iptables_globals.orig_opts,
					iptables_globals.opts,
					xt_m->extra_opts,
					&xt_m->option_offset);

		if (!iptables_globals.opts) {
			g_free(xt_m->m);

			if (xt_m == xt_m->next)
				free(xt_m);

			xt_m = NULL;
		}

		break;
	case AF_INET6:
		if (xt_m->x6_options)
			ip6tables_globals.opts =
				xtables_options_xfrm(
					ip6tables_globals.orig_opts,
					ip6tables_globals.opts,
					xt_m->x6_options,
					&xt_m->option_offset);
		else
			ip6tables_globals.opts =
				xtables_merge_options(
					ip6tables_globals.orig_opts,
					ip6tables_globals.opts,
					xt_m->extra_opts,
					&xt_m->option_offset);

		if (!ip6tables_globals.opts) {
			g_free(xt_m->m);

			if (xt_m == xt_m->next)
				free(xt_m);

			xt_m = NULL;
		}

		break;
	}

	return xt_m;
}

static int parse_ip_and_mask(const char *str, struct in_addr *ip,
				struct in_addr *mask)
{
	char **tokens;
	uint32_t prefixlength;
	uint32_t tmp;
	int err;

	tokens = g_strsplit(str, "/", 2);
	if (!tokens)
		return -1;

	if (!inet_pton(AF_INET, tokens[0], ip)) {
		err = -1;
		goto out;
	}

	if (tokens[1]) {
		prefixlength = strtol(tokens[1], NULL, 10);
		if (prefixlength > 32) {
			err = -1;
			goto out;
		} else if (prefixlength == 32) {
			tmp = 0xffffffff;
		} else {
			tmp = ~(0xffffffff >> prefixlength);
		}
	} else {
		tmp = 0xffffffff;
	}

	mask->s_addr = htonl(tmp);
	ip->s_addr = ip->s_addr & mask->s_addr;
	err = 0;
out:
	g_strfreev(tokens);

	return err;
}

static int parse_ipv6_and_mask(const char *str, struct in6_addr *ip,
				struct in6_addr *mask)
{
	char **tokens;
	uint32_t prefixlength;
	struct in6_addr in6;
	int i, j;
	int err;

	tokens = g_strsplit(str, "/", 2);
	if (!tokens)
		return -1;

	if (!inet_pton(AF_INET6, tokens[0], ip)) {
		err = -1;
		goto out;
	}

	if (tokens[1]) {
		prefixlength = strtol(tokens[1], NULL, 10);
		if (prefixlength > 128) {
			err = -1;
			goto out;
		}
	} else {
		prefixlength = 128;
	}

	/*
	 * This part was adapted from (no need to re-invent the wheel):
	 * https://gitlab.com/ipcalc/ipcalc/blob/master/ipcalc.c#L733
	 */
	memset(&in6, 0, sizeof(struct in6_addr));

	for (i = prefixlength, j = 0; i > 0; i -= 8, j++) {
		if (i >= 8)
			in6.s6_addr[j] = 0xff;
		else
			in6.s6_addr[j] = (unsigned long)(0xffU << (8 - i));
	}

	memcpy(mask, &in6, sizeof(struct in6_addr));

	for (i = 0; i < 16 ; i++)
		ip->s6_addr[i] = ip->s6_addr[i] & mask->s6_addr[i];

	err = 0;
out:
	g_strfreev(tokens);

	return err;
}

static struct connman_iptables *get_table(int type, const char *table_name)
{
	struct connman_iptables *table = NULL;

	if (!table_name)
		table_name = "filter";

	table = hash_table_lookup(type, table_name);

	if (table)
		return table;

	table = iptables_init(type, table_name);

	if (!table)
		return NULL;

	if (table->name)
		g_free(table->name);

	table->name = g_strdup(table_name);
	
	hash_table_replace(type, table->name, table);

	return table;
}

struct parse_context {
	int type;
	int argc;
	char **argv;
	struct ipt_ip *ip;
	struct ip6t_ip6 *ipv6;
	struct xtables_target *xt_t;
	GList *xt_m;
	struct xtables_rule_match *xt_rm;
	uint16_t proto;
};

static int prepare_getopt_args(const char *str, struct parse_context *ctx)
{
	char **tokens;
	int i;

	tokens = g_strsplit_set(str, " ", -1);

	i = g_strv_length(tokens);

	/* Add space for the argv[0] value */
	ctx->argc = i + 1;

	/* Don't forget the last NULL entry */
	ctx->argv = g_try_malloc0((ctx->argc + 1) * sizeof(char *));
	if (!ctx->argv) {
		g_strfreev(tokens);
		return -ENOMEM;
	}

	/*
	 * getopt_long() jumps over the first token; we need to add some
	 * random argv[0] entry.
	 */
	ctx->argv[0] = g_strdup("argh");
	for (i = 1; i < ctx->argc; i++)
		ctx->argv[i] = tokens[i - 1];

	g_free(tokens);

	return 0;
}

static int parse_xt_modules(int c, bool invert,
				struct parse_context *ctx)
{
	struct xtables_match *m;
	struct xtables_rule_match *rm;
	struct ipt_entry fw;
	struct ip6t_entry fw6;
	int err;

	switch (ctx->type) {
	case AF_INET:
		memset(&fw, 0, sizeof(fw));

		/* The SNAT parser wants to know the protocol. */
		if (ctx->proto == 0)
			ctx->proto = IPPROTO_IP;

		fw.ip.proto = ctx->proto;
		break;
	case AF_INET6:
		memset(&fw6, 0, sizeof(fw6));

		if (ctx->proto == 0)
			ctx->proto = IPPROTO_IPV6;

		fw6.ipv6.proto = ctx->proto;

		/* Flags must be set for IPv6 if protocol is set. */
		fw6.ipv6.flags |= IP6T_F_PROTO;

		break;
	default:
		return 0;
	}

	for (rm = ctx->xt_rm; rm; rm = rm->next) {
		if (rm->completed != 0)
			continue;

		m = rm->match;

		if (!m->x6_parse && !m->parse)
			continue;

		if (c < (int) m->option_offset ||
				c >= (int) m->option_offset
					+ XT_OPTION_OFFSET_SCALE)
			continue;

		enable_jmp();

		if ((err = setjmp(env_state)) != 0) {
			DBG("setjmp() called by longjmp() with value %d", err);
			disable_jmp();
			return -EINVAL;
		}

		switch (ctx->type) {
		case AF_INET:
			xtables_option_mpcall(c, ctx->argv, invert, m, &fw);
			break;
		case AF_INET6:
			xtables_option_mpcall(c, ctx->argv, invert, m, &fw6);
			break;
		}

		disable_jmp();
	}

	if (!ctx->xt_t)
		return 0;

	if (!ctx->xt_t->x6_parse && !ctx->xt_t->parse)
		return 0;

	if (c < (int) ctx->xt_t->option_offset ||
			c >= (int) ctx->xt_t->option_offset
					+ XT_OPTION_OFFSET_SCALE)
		return 0;

	enable_jmp();

	if ((err = setjmp(env_state)) != 0) {
		DBG("setjmp() called by longjmp() with value %d", err);
		disable_jmp();
		return -EINVAL;
	}

	switch (ctx->type) {
	case AF_INET:
		xtables_option_tpcall(c, ctx->argv, invert, ctx->xt_t, &fw);
		break;
	case AF_INET6:
		xtables_option_tpcall(c, ctx->argv, invert, ctx->xt_t, &fw6);
		break;
	}

	disable_jmp();

	return 0;
}

static int final_check_xt_modules(struct parse_context *ctx)
{
	struct xtables_rule_match *rm;
	int err;

	for (rm = ctx->xt_rm; rm; rm = rm->next) {
		enable_jmp();

		if ((err = setjmp(env_state)) != 0) {
			DBG("setjmp() called by longjmp() with value %d", err);
			disable_jmp();
			return -EINVAL;
		}

		xtables_option_mfcall(rm->match);

		disable_jmp();
	}

	enable_jmp();

	if ((err = setjmp(env_state)) != 0) {
		DBG("setjmp() called by longjmp() with value %d", err);
		disable_jmp();
		return -EINVAL;
	}

	if (ctx->xt_t)
		xtables_option_tfcall(ctx->xt_t);

	disable_jmp();

	return 0;
}

static int parse_rule_spec(struct connman_iptables *table,
				struct parse_context *ctx)
{
	/*
	 * How the parser works:
	 *
	 *  - If getopt finds 's', 'd', 'i', 'o'.
	 *    just extract the information.
	 *  - if '!' is found, set the invert flag to true and
	 *    removes the '!' from the optarg string and jumps
	 *    back to getopt to reparse the current optarg string.
	 *    After reparsing the invert flag is reset to false.
	 *  - If 'm' or 'j' is found then call either
	 *    prepare_matches() or prepare_target(). Those function
	 *    will modify (extend) the longopts for getopt_long.
	 *    That means getopt will change its matching context according
	 *    the loaded target.
	 *
	 *    Here an example with iptables-test
	 *
	 *    argv[0] = ./tools/iptables-test
	 *    argv[1] = -t
	 *    argv[2] = filter
	 *    argv[3] = -A
	 *    argv[4] = INPUT
	 *    argv[5] = -m
	 *    argv[6] = mark
	 *    argv[7] = --mark
	 *    argv[8] = 999
	 *    argv[9] = -j
	 *    argv[10] = LOG
	 *
	 *    getopt found 'm' then the optarg is "mark" and optind 7
	 *    The longopts array containts before hitting the `case 'm'`
	 *
	 *    val A has_arg 1 name append
	 *    val C has_arg 1 name compare
	 *    val D has_arg 1 name delete
	 *    val F has_arg 1 name flush-chain
	 *    val I has_arg 1 name insert
	 *    val L has_arg 2 name list
	 *    val N has_arg 1 name new-chain
	 *    val P has_arg 1 name policy
	 *    val X has_arg 1 name delete-chain
	 *    val d has_arg 1 name destination
	 *    val i has_arg 1 name in-interface
	 *    val j has_arg 1 name jump
	 *    val m has_arg 1 name match
	 *    val o has_arg 1 name out-interface
	 *    val s has_arg 1 name source
	 *    val t has_arg 1 name table
	 *
	 *    After executing the `case 'm'` block longopts is
	 *
	 *    val A has_arg 1 name append
	 *    val C has_arg 1 name compare
	 *    val D has_arg 1 name delete
	 *    val F has_arg 1 name flush-chain
	 *    val I has_arg 1 name insert
	 *    val L has_arg 2 name list
	 *    val N has_arg 1 name new-chain
	 *    val P has_arg 1 name policy
	 *    val X has_arg 1 name delete-chain
	 *    val d has_arg 1 name destination
	 *    val i has_arg 1 name in-interface
	 *    val j has_arg 1 name jump
	 *    val m has_arg 1 name match
	 *    val o has_arg 1 name out-interface
	 *    val s has_arg 1 name source
	 *    val t has_arg 1 name table
	 *    val   has_arg 1 name mark
	 *
	 *    So the 'mark' matcher has added the 'mark' options
	 *    and getopt will then return c '256' optarg "999" optind 9
	 *    And we will hit the 'default' statement which then
	 *    will call the matchers parser (xt_m->parser() or
	 *    xtables_option_mpcall() depending on which version
	 *    of libxtables is found.
	 */
	struct xtables_match *xt_m;
	bool invert = false;
	int len, c, err;

	if (ctx->type != table->type) {
		DBG("ctx->type %d does not match table->type %d", ctx->type,
				table->type);
		return -EINVAL;
	}

	switch (ctx->type) {
	case AF_INET:
		ctx->ip = g_try_new0(struct ipt_ip, 1);
		if (!ctx->ip)
			return -ENOMEM;

		break;
	case AF_INET6:
		ctx->ipv6 = g_try_new0(struct ip6t_ip6, 1);
		if (!ctx->ipv6)
			return -ENOMEM;

		break;
	default:
		return -EINVAL;
	}

	/*
	 * Tell getopt_long not to generate error messages for unknown
	 * options and also reset optind back to 0.
	 */
	opterr = 0;
	optind = 0;

	while ((c = getopt_long(ctx->argc, ctx->argv,
					"-:d:i:o:s:m:j:p:",
					ctx->type == AF_INET ?
						iptables_globals.opts :
						ip6tables_globals.opts,
					NULL)) != -1) {
		switch (c) {
		case 's':
			if (ctx->type == AF_INET) {
				/* Source specification */
				if (!parse_ip_and_mask(optarg,
							&ctx->ip->src,
							&ctx->ip->smsk))
					break;

				if (invert)
					ctx->ip->invflags |= IPT_INV_SRCIP;
			}

			if (ctx->type == AF_INET6) {
				if (!parse_ipv6_and_mask(optarg,
							&ctx->ipv6->src,
							&ctx->ipv6->smsk))
					break;

				if (invert)
					ctx->ipv6->invflags |= IP6T_INV_SRCIP;
			}

			break;
		case 'd':
			if (ctx->type == AF_INET) {
				/* Destination specification */
				if (!parse_ip_and_mask(optarg,
							&ctx->ip->dst,
							&ctx->ip->dmsk))
					break;

				if (invert)
					ctx->ip->invflags |= IPT_INV_DSTIP;
			}

			if (ctx->type == AF_INET6) {
				/* Destination specification */
				if (!parse_ipv6_and_mask(optarg,
							&ctx->ipv6->dst,
							&ctx->ipv6->dmsk))
					break;

				if (invert)
					ctx->ipv6->invflags |= IP6T_INV_DSTIP;
			}
			
			break;
		case 'i':
			/* In interface specification */
			len = strlen(optarg);

			if (len + 1 > IFNAMSIZ)
				break;

			if (ctx->type == AF_INET) {
				g_stpcpy(ctx->ip->iniface, optarg);
				memset(ctx->ip->iniface_mask, 0xff, len + 1);

				if (invert)
					ctx->ip->invflags |= IPT_INV_VIA_IN;
			}
			
			if (ctx->type == AF_INET6) {
				g_stpcpy(ctx->ipv6->iniface, optarg);
				memset(ctx->ipv6->iniface_mask, 0xff, len + 1);

				if (invert)
					ctx->ipv6->invflags |= IP6T_INV_VIA_IN;
			}

			break;
		case 'o':
			/* Out interface specification */
			len = strlen(optarg);

			if (len + 1 > IFNAMSIZ)
				break;

			if (ctx->type == AF_INET) {
				g_stpcpy(ctx->ip->outiface, optarg);
				memset(ctx->ip->outiface_mask, 0xff, len + 1);

				if (invert)
					ctx->ip->invflags |= IPT_INV_VIA_OUT;
			}

			if (ctx->type == AF_INET6) {
				g_stpcpy(ctx->ipv6->outiface, optarg);
				memset(ctx->ipv6->outiface_mask, 0xff, len + 1);

				if (invert)
					ctx->ipv6->invflags |= IP6T_INV_VIA_OUT;
			}

			break;
		case 'm':
			/* Matches */
			xt_m = prepare_matches(table, &ctx->xt_rm, optarg);
			if (!xt_m) {
				err = -EINVAL;
				goto out;
			}
			ctx->xt_m = g_list_append(ctx->xt_m, xt_m);

			break;
		case 'p':
			enable_jmp();

			if ((err = setjmp(env_state)) != 0) {
				DBG("setjmp() called by longjmp() with value "
							"%d", err);
				disable_jmp();

				/* Errors from parse_rule_spec are negative */
				err = -EINVAL;
				goto out;
			}

			ctx->proto = xtables_parse_protocol(optarg);

			disable_jmp();

			/*
			 * If protocol was set add it to ipt_ip.
			 * xtables_parse_protocol() returns 0 or
			 * UINT16_MAX (-1) on error
			 */
			if (ctx->proto > 0 && ctx->proto < UINT16_MAX) {
				if (ctx->type == AF_INET)
					ctx->ip->proto = ctx->proto;

				if (ctx->type == AF_INET6) {
					ctx->ipv6->proto = ctx->proto;

					/*
					 * Flags must be set for IPv6 if
					 * protocol is set.
					 */
					ctx->ipv6->flags |= IP6T_F_PROTO;
				}
			}
			break;
		case 'j':
			/* Target */
			ctx->xt_t = prepare_target(table, optarg);
			if (!ctx->xt_t) {
				err = -EINVAL;
				goto out;
			}

			break;
		case 1:
			if (optarg[0] == '!' && optarg[1] == '\0') {
				invert = true;

				/* Remove the '!' from the optarg */
				optarg[0] = '\0';

				/*
				 * And recall getopt_long without resetting
				 * invert.
				 */
				continue;
			}

			break;
		default:
			err = parse_xt_modules(c, invert, ctx);
			if (err == 1)
				continue;
			else if (err == -EINVAL)
				goto out;

			break;
		}

		invert = false;
	}

	err = final_check_xt_modules(ctx);

out:
	return err;
}

static int current_type = -1;

static int setup_xtables(int type)
{
	int err;

	DBG("%d", type);

	if (type == current_type)
		return 0;

	if (current_type != -1)
		reset_xtables();

	switch (type) {
	case AF_INET:
		err = xtables_init_all(&iptables_globals, NFPROTO_IPV4);
		break;
	case AF_INET6:
		err = xtables_init_all(&ip6tables_globals, NFPROTO_IPV6);
		break;
	default:
		return -1;
	}
	
	if (!err) {
		current_type = type;
	} else {
		connman_error("error initializing xtables");
		current_type = -1;
		reset_xtables();
	}

	return err;
}

static void reset_xtables(void)
{
	struct xtables_match *xt_m;
	struct xtables_target *xt_t;

	/*
	 * As side effect parsing a rule sets some global flags
	 * which will be evaluated/verified. Let's reset them
	 * to ensure we can parse more than one rule.
	 *
	 * Clear all flags because the flags are only valid
	 * for one rule.
	 */
	for (xt_m = xtables_matches; xt_m; xt_m = xt_m->next)
		xt_m->mflags = 0;

	for (xt_t = xtables_targets; xt_t; xt_t = xt_t->next) {
		xt_t->tflags = 0;
		xt_t->used = 0;
	}

	/*
	 * We need also to free the memory implicitly allocated
	 * during parsing (see xtables_options_xfrm()).
	 * Note xt_params is actually iptables_globals.
	 */
	if (xt_params->opts != xt_params->orig_opts) {
		g_free(xt_params->opts);
		xt_params->opts = xt_params->orig_opts;
	}
	xt_params->option_offset = 0;
}

static void cleanup_parse_context(struct parse_context *ctx)
{
	struct xtables_rule_match *rm, *tmp;
	GList *list;

	g_strfreev(ctx->argv);

	g_free(ctx->ip);
	g_free(ctx->ipv6);

	if (ctx->xt_t) {
		g_free(ctx->xt_t->t);
		ctx->xt_t->t = NULL;
	}

	for (list = ctx->xt_m; list; list = list->next) {
		struct xtables_match *xt_m = list->data;

		g_free(xt_m->m);

		if (xt_m != xt_m->next)
			continue;

		g_free(xt_m);
	}
	g_list_free(ctx->xt_m);

	for (tmp = NULL, rm = ctx->xt_rm; rm; rm = rm->next) {
		if (tmp)
			g_free(tmp);
		tmp = rm;
	}
	g_free(tmp);

	g_free(ctx);
}

int __connman_iptables_dump(int type, const char *table_name)
{
	struct connman_iptables *table;

	DBG("%d -t %s -L", type, table_name);

	table = get_table(type, table_name);
	if (!table)
		return -EINVAL;

	dump_table(table);

	return 0;
}

int __connman_iptables_new_chain(int type,
					const char *table_name,
					const char *chain)
{
	struct connman_iptables *table;

	DBG("%d -t %s -N %s", type, table_name, chain);

	table = get_table(type, table_name);
	if (!table) {
		return -EINVAL;
	}

	switch (type) {
	case AF_INET:
	case AF_INET6:
		return iptables_add_chain(table, chain);
	}

	return -EINVAL;
}

int __connman_iptables_delete_chain(int type,
					const char *table_name,
					const char *chain)
{
	struct connman_iptables *table;

	DBG("%d -t %s -X %s", type, table_name, chain);

	table = get_table(type, table_name);
	if (!table)
		return -EINVAL;

	return iptables_delete_chain(table, chain);
}

int __connman_iptables_flush_chain(int type,
					const char *table_name,
					const char *chain)
{
	struct connman_iptables *table;

	DBG("%d -t %s -F %s", type, table_name, chain);

	table = get_table(type, table_name);
	if (!table)
		return -EINVAL;

	return iptables_flush_chain(table, chain);
}

int __connman_iptables_find_chain(int type,
					const char *table_name,
					const char *chain)
{
	struct connman_iptables *table;

	DBG("%d -t %s -F %s", type, table_name, chain);

	table = get_table(type, table_name);
	if (!table)
		return -EINVAL;

	if(!find_chain_head(table, chain))
		return -ENOENT; // Not Found
	
	return 0; // Found
}

int __connman_iptables_change_policy(int type,
					const char *table_name,
					const char *chain,
					const char *policy)
{
	struct connman_iptables *table;

	DBG("%d -t %s -F %s", type, table_name, chain);

	table = get_table(type, table_name);
	if (!table)
		return -EINVAL;

	return iptables_change_policy(table, chain, policy);
}

static void iptables_ip_setup(struct iptables_ip *ip, struct parse_context *ctx)
{
	if (!ip || !ctx)
		return;

	ip->type = ctx->type;
	ip->ip = ctx->ip;
	ip->ip6 = ctx->ipv6;
}

int __connman_iptables_append(int type,
				const char *table_name,
				const char *chain,
				const char *rule_spec)
{
	struct connman_iptables *table;
	struct parse_context *ctx;
	struct iptables_ip ip = { 0 };
	const char *target_name;
	int err;

	err = setup_xtables(type);
	
	if (err < 0)
		return err;

	ctx = g_try_new0(struct parse_context, 1);
	if (!ctx)
		return -ENOMEM;

	ctx->type = type;

	DBG("%d -t %s -A %s %s", type, table_name, chain, rule_spec);

	err = prepare_getopt_args(rule_spec, ctx);
	if (err < 0)
		goto out;

	table = get_table(type, table_name);
	if (!table) {
		err = -EINVAL;
		goto out;
	}

	err = parse_rule_spec(table, ctx);
	if (err < 0)
		goto out;

	if (!ctx->xt_t)
		target_name = NULL;
	else
		target_name = ctx->xt_t->name;

	iptables_ip_setup(&ip, ctx);

	err = iptables_append_rule(table, &ip, chain, target_name, ctx->xt_t,
								ctx->xt_rm);
out:
	cleanup_parse_context(ctx);
	reset_xtables();

	return err;
}

int __connman_iptables_insert(int type,
				const char *table_name,
				const char *chain,
				const char *rule_spec)
{
	struct connman_iptables *table;
	struct parse_context *ctx;
	struct iptables_ip ip = { 0 };
	const char *target_name;
	int err;

	err = setup_xtables(type);
	
	if (err < 0)
		return err;

	ctx = g_try_new0(struct parse_context, 1);
	if (!ctx)
		return -ENOMEM;
	
	ctx->type = type;

	DBG("%d -t %s -I %s %s", type, table_name, chain, rule_spec);

	err = prepare_getopt_args(rule_spec, ctx);
	if (err < 0)
		goto out;

	table = get_table(type, table_name);
	if (!table) {
		err = -EINVAL;
		goto out;
	}

	err = parse_rule_spec(table, ctx);
	if (err < 0)
		goto out;

	if (!ctx->xt_t)
		target_name = NULL;
	else
		target_name = ctx->xt_t->name;

	iptables_ip_setup(&ip, ctx);

	err = iptables_insert_rule(table, &ip, chain, target_name, ctx->xt_t,
								ctx->xt_rm);
out:
	cleanup_parse_context(ctx);
	reset_xtables();

	return err;
}

int __connman_iptables_delete(int type,
				const char *table_name,
				const char *chain,
				const char *rule_spec)
{
	struct connman_iptables *table;
	struct parse_context *ctx;
	struct iptables_ip ip = { 0 };
	const char *target_name;
	int err;

	err = setup_xtables(type);
	
	if (err < 0)
		return err;

	ctx = g_try_new0(struct parse_context, 1);
	if (!ctx)
		return -ENOMEM;
	
	ctx->type = type;

	DBG("%d -t %s -D %s %s", type, table_name, chain, rule_spec);

	err = prepare_getopt_args(rule_spec, ctx);
	if (err < 0)
		goto out;

	table = get_table(type, table_name);
	if (!table) {
		err = -EINVAL;
		goto out;
	}

	err = parse_rule_spec(table, ctx);
	if (err < 0)
		goto out;

	if (!ctx->xt_t)
		target_name = NULL;
	else
		target_name = ctx->xt_t->name;

	iptables_ip_setup(&ip, ctx);

	err = iptables_delete_rule(table, &ip, chain, target_name, ctx->xt_t,
				ctx->xt_m, ctx->xt_rm);
out:
	cleanup_parse_context(ctx);
	reset_xtables();

	return err;
}

int __connman_iptables_commit(int type, const char *table_name)
{
	struct connman_iptables *table;
	struct iptables_replace repl = { 0 };
	int err;
	struct xt_counters_info *counters;
	struct connman_iptables_entry *e;
	GList *list;
	unsigned int cnt;

	err = setup_xtables(type);
	
	if (err < 0)
		return err;

	DBG("%d %s", type, table_name);

	repl.type = type;

	table = hash_table_lookup(type, table_name);
	if (!table)
		return -EINVAL;

	switch (type) {
	case AF_INET:
		repl.r = iptables_blob(table);
		if (!repl.r)
			return -ENOMEM;
		
		break;
	case AF_INET6:
		repl.r6 = ip6tables_blob(table);
		if (!repl.r6)
			return -ENOMEM;
	}

	if (debug_enabled)
		dump_replace(&repl);

	err = iptables_replace(table, &repl);

	if (err < 0)
		goto out_free;

	counters = g_try_malloc0(sizeof(*counters) +
			sizeof(struct xt_counters) * table->num_entries);
	if (!counters) {
		err = -ENOMEM;
		goto out_hash_remove;
	}
	g_stpcpy(counters->name, iptables_table_get_info_name(table));
	counters->num_counters = table->num_entries;
	for (list = table->entries, cnt = 0; list; list = list->next, cnt++) {
		e = list->data;
		if (e->counter_idx >= 0) {
		
			switch (type) {
			case AF_INET:
				counters->counters[cnt] =
					repl.r->counters[e->counter_idx];
				break;
			case AF_INET6:
				counters->counters[cnt] =
					repl.r6->counters[e->counter_idx];
				break;
			}
		}
	}
	err = iptables_add_counters(table, counters);
	g_free(counters);

	if (err < 0)
		goto out_hash_remove;

	err = 0;

out_hash_remove:
	hash_table_remove(type, table_name);
out_free:
	if (type == AF_INET && repl.r)
		g_free(repl.r->counters);

	if (type == AF_INET6 && repl.r6)
		g_free(repl.r6->counters);

	g_free(repl.r);
	g_free(repl.r6);
	
	reset_xtables();

	return err;
}

static void remove_table(gpointer user_data)
{
	struct connman_iptables *table = user_data;

	table_cleanup(table);
}

static int iterate_chains_cb(struct connman_iptables_entry *entry, int builtin,
				unsigned int hook, size_t size,
				unsigned int offset, void *user_data)
{
	struct cb_data *cbd = user_data;
	connman_iptables_iterate_chains_cb_t cb = cbd->cb;
	struct xt_entry_target *target;

	if (offset + iptables_entry_get_next_offset(entry) == size)
		return 0;

	target = iptables_entry_get_target(entry);

	if (!g_strcmp0(target->u.user.name, get_error_target(entry->type))) {
		(*cb)((const char *)target->data, cbd->user_data);
	} else if (builtin >= 0) {
		(*cb)(hooknames[builtin], cbd->user_data);
	}

	return 0;
}

int __connman_iptables_iterate_chains(int type, const char *table_name,
				connman_iptables_iterate_chains_cb_t cb,
				void *user_data)
{
	struct cb_data *cbd = cb_data_new(cb, user_data);
	struct connman_iptables *table;
	struct connman_iptables_entry entry = { 0 };
	int err;

	err = setup_xtables(type);
	
	if (err < 0)
		return err;

	table = get_table(type, table_name);
	if (!table) {
		g_free(cbd);
		return -EINVAL;
	}

	entry.type = type;

	if (type == AF_INET)
		entry.entry = table->blob_entries->entrytable;

	if (type == AF_INET6)
		entry.entry6 = table->blob_entries6->entrytable;

	iterate_entries(&entry,
			iptables_table_get_info_valid_hooks(table),
			iptables_table_get_info_hook_entry(table),
			iptables_table_get_info_underflow(table),
			iptables_table_get_entries_size(table),
			iterate_chains_cb, cbd);

	g_free(cbd);
	
	reset_xtables();

	return 0;
}

int __connman_iptables_init(void)
{
	DBG("");

	if (getenv("CONNMAN_IPTABLES_DEBUG"))
		debug_enabled = true;

	table_hash = g_hash_table_new_full(g_str_hash, g_str_equal,
						NULL, remove_table);

	table_hash_ipv6 = g_hash_table_new_full(g_str_hash, g_str_equal,
						NULL, remove_table);

	return 0;
}

void __connman_iptables_cleanup(void)
{
	DBG("");

	g_hash_table_destroy(table_hash);
	g_hash_table_destroy(table_hash_ipv6);
}

