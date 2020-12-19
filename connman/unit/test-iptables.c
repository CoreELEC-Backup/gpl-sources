/*
 *  ConnMan firewall unit tests
 *
 *  Copyright (C) 2019 Jolla Ltd. All rights reserved.
 *  Contact: jussi.laakkonen@jolla.com
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>
#include <gdbus.h>
#include <unistd.h>
#include <stdlib.h>
#include <xtables.h>
#include <errno.h>
#include <sys/wait.h>

#include "src/connman.h"

enum configtype {
	TEST_CONFIG_PASS =			0x0001,
	TEST_CONFIG_INIT_FAIL =			0x0002,
	TEST_CONFIG_FIND_MATCH_FAIL =		0x0004,
	TEST_CONFIG_FIND_TARGET_FAIL =		0x0008,
	TEST_CONFIG_PARSE_PROTOCOL_FAIL =	0x0010,
	TEST_CONFIG_MFCALL_FAIL =		0x0020,
	TEST_CONFIG_TFCALL_FAIL =		0x0040,
	TEST_CONFIG_MPCALL_FAIL =		0x0080,
	TEST_CONFIG_TPCALL_FAIL =		0x0100,
	TEST_CONFIG_INSMOD_FAIL =		0x0200,
	TEST_CONFIG_COMPATIBLE_REV_FAIL =	0x0400,
	TEST_CONFIG_OPTIONS_XFRM_FAIL =		0x0800,
	TEST_CONFIG_MERGE_OPTIONS_FAIL =	0x1000,
};

enum configtype test_config_type = TEST_CONFIG_PASS;

static void set_test_config(enum configtype type)
{
	test_config_type = type;
}

/* Start of dummies */

/* xtables dummies */

/* From /usr/include/linux/netfilter_ipv4/ip_tables.h */
#define IPT_BASE_CTL			64
#define IPT_SO_SET_REPLACE		(IPT_BASE_CTL)
#define IPT_SO_SET_ADD_COUNTERS		(IPT_BASE_CTL + 1)
#define IPT_SO_GET_INFO			(IPT_BASE_CTL)
#define IPT_SO_GET_ENTRIES		(IPT_BASE_CTL + 1)

/* From /usr/include/linux/netfilter_ipv6/ip6_tables.h */
#define IP6T_BASE_CTL			64
#define IP6T_SO_SET_REPLACE		(IP6T_BASE_CTL)
#define IP6T_SO_SET_ADD_COUNTERS	(IP6T_BASE_CTL + 1)
#define IP6T_SO_GET_INFO		(IP6T_BASE_CTL)
#define IP6T_SO_GET_ENTRIES		(IP6T_BASE_CTL + 1)

int xt_match_parse(int c, char **argv, int invert, unsigned int *flags,
			const void *entry, struct xt_entry_match **match)
{
	return 0;
}

int xt_target_parse(int c, char **argv, int invert, unsigned int *flags,
			const void *entry, struct xt_entry_target **targetinfo)
{
	return 0;
}

static void xt_x6_parse(struct xt_option_call *opt) {
	return;
}

static void xt_x6_fcheck(struct xt_fcheck_call *call) {
	return;
}

static struct xtables_match xt_match = {
	.version = "1",
	.next = NULL,
	.name = "tcp",
	.real_name = "tcp",
	.revision = 1,
	.ext_flags = 0,
	.family = AF_INET,
	.size = XT_ALIGN(sizeof(struct xtables_match)),
	.userspacesize = XT_ALIGN(sizeof(struct xtables_match)),
	.parse = xt_match_parse,
	.extra_opts = NULL,
	.x6_parse = xt_x6_parse,
	.x6_fcheck = xt_x6_fcheck,
	.x6_options = NULL,
	.udata_size = XT_ALIGN(sizeof(struct xtables_match)),
	.udata = NULL,
	.option_offset = 32,
	.m = NULL,
	.mflags = 0,
	.loaded = 1,
};

static struct xtables_target xt_target = {
	.version = "1",
	.next = NULL,
	.name = "ACCEPT",
	.real_name = "ACCEPT",
	.revision = 1,
	.ext_flags = 0,
	.family = AF_INET,
	.size = XT_ALIGN(sizeof(struct xtables_match)),
	.userspacesize = XT_ALIGN(sizeof(struct xtables_match)),
	.parse = xt_target_parse,
	.extra_opts = NULL,
	.x6_parse = xt_x6_parse,
	.x6_fcheck = xt_x6_fcheck,
	.x6_options = NULL,
	.udata_size = XT_ALIGN(sizeof(struct xtables_match)),
	.udata = NULL,
	.option_offset = 32,
	.t = NULL,
	.tflags = 0,
	.used = 0,
	.loaded = 1,
};

struct xtables_globals *xt_params = NULL;

struct xtables_match *xtables_matches = NULL;
struct xtables_target *xtables_targets = NULL;

static void call_error(const char *src)
{
	g_assert(xt_params);

	DBG("%s", src);

	xt_params->exit_err(PARAMETER_PROBLEM, "longjmp() %s", src);
}

int xtables_init_all(struct xtables_globals *xtp, uint8_t nfproto)
{
	DBG("%d", nfproto);

	if (test_config_type & TEST_CONFIG_INIT_FAIL)
		call_error("xtables_init_all");

	xt_params = xtp;

	return 0;
}

struct xtables_match *xtables_find_match(const char *name,
			enum xtables_tryload tryload,
			struct xtables_rule_match **matches)
{
	DBG("name %s type %d", name, tryload);

	if (test_config_type & TEST_CONFIG_FIND_MATCH_FAIL)
		call_error("xtables_find_match");

	*matches = g_try_new0(struct xtables_rule_match, 1);
	(*matches)->next = NULL;
	(*matches)->match = &xt_match;
	(*matches)->completed = 0;

	return &xt_match;
}

struct xtables_target *xtables_find_target(const char *name,
			enum xtables_tryload tryload)
{
	DBG("name %s type %d", name, tryload);

	if (test_config_type & TEST_CONFIG_FIND_TARGET_FAIL)
		call_error("xtables_find_target");

	return &xt_target;
}

uint16_t xtables_parse_protocol(const char *s)
{
	DBG("protocol %s", s);

	if (test_config_type & TEST_CONFIG_PARSE_PROTOCOL_FAIL)
		call_error("xtables_parse_protocol");

	if (!g_strcmp0(s, "tcp"))
		return 6;

	return 0;
}

void xtables_option_mfcall(struct xtables_match *m)
{
	DBG("");

	if (test_config_type & TEST_CONFIG_MFCALL_FAIL)
		call_error("xtables_option_mfcall");

	m = &xt_match;

	return;
}

void xtables_option_tfcall(struct xtables_target *t)
{
	DBG("");

	if (test_config_type & TEST_CONFIG_TFCALL_FAIL)
		call_error("xtables_option_tfcall");

	t = &xt_target;

	return;
}

void xtables_option_mpcall(unsigned int c, char **argv, bool invert,
			struct xtables_match *m, void *fw)
{
	DBG("");

	if (test_config_type & TEST_CONFIG_MPCALL_FAIL)
		call_error("xtables_option_mpcall");

	m = &xt_match;

	return;
}

void xtables_option_tpcall(unsigned int c, char **argv, bool invert,
			struct xtables_target *t, void *fw)
{
	DBG("");

	if (test_config_type & TEST_CONFIG_TPCALL_FAIL)
		call_error("xtables_option_tpcall");

	t = &xt_target;

	return;
}

int xtables_insmod(const char *modname, const char *modprobe, bool quiet)
{
	DBG("mod %s modprobe %s quiet %s", modname, modprobe,
				quiet ? "true" : "false");

	if (test_config_type & TEST_CONFIG_INSMOD_FAIL)
		call_error("xtables_insmod");

	return 0;
}

int xtables_compatible_revision(const char *name, uint8_t revision, int opt)
{
	DBG("name %s rev %d opt %d", name, revision, opt);

	if (test_config_type & TEST_CONFIG_COMPATIBLE_REV_FAIL)
		call_error("xtables_compatible_revision");

	return 1;
}

struct option *xtables_options_xfrm(struct option *opt1, struct option *opt2,
					const struct xt_option_entry *entry,
					unsigned int *dummy)
{
	if (test_config_type & TEST_CONFIG_OPTIONS_XFRM_FAIL)
		call_error("xtables_options_xfrm");

	return opt1;
}

struct option *xtables_merge_options(struct option *orig_opts,
			struct option *oldopts, const struct option *newopts,
			unsigned int *option_offset)
{
	if (test_config_type & TEST_CONFIG_MERGE_OPTIONS_FAIL)
		call_error("xtables_merge_options");

	return orig_opts;
}

/* End of xtables dummies */

/* socket dummies */

int global_sockfd = 1000;

int socket(int domain, int type, int protocol)
{
	DBG("domain %d type %d protocol %d", domain, type, protocol);

	return global_sockfd;
}

int getsockopt(int sockfd, int level, int optname, void *optval,
			socklen_t *optlen)
{
	struct ipt_getinfo *info = NULL;
	struct ipt_get_entries *entries = NULL;
	struct ip6t_getinfo *info6 = NULL;
	struct ip6t_get_entries *entries6 = NULL;

	DBG("");

	g_assert_cmpint(global_sockfd, ==, sockfd);

	switch (level) {
	case IPPROTO_IP:
		DBG("IPPROTO_IP");

		switch (optname) {
		case IPT_SO_GET_ENTRIES:
			DBG("IPT_SO_GET_ENTRIES");
			optval = entries;
			break;
		case IPT_SO_GET_INFO:
			DBG("IPT_SO_GET_INFO");
			optval = info;
			break;
		default:
			DBG("optname %d", optname);
			return -1;
		}

		break;
	case IPPROTO_IPV6:
		DBG("IPPROTO_IPV6");
		switch (optname) {
		case IP6T_SO_GET_ENTRIES:
			DBG("IP6T_SO_GET_ENTRIES");
			optval = entries6;
			break;
		case IP6T_SO_GET_INFO:
			DBG("IP6T_SO_GET_INFO");
			optval = info6;
			break;
		default:
			DBG("optname %d", optname);
			return -1;
		}

		break;
	default:
		return -1;
	}

	*optlen = 0;
	return 0;
}

int setsockopt(int sockfd, int level, int optname, const void *optval,
			socklen_t optlen)
{
	DBG("");

	g_assert_cmpint(global_sockfd, ==, sockfd);

	switch (level) {
	case IPPROTO_IP:
		DBG("IPPROTO_IP");
		switch (optname) {
		case IPT_SO_SET_REPLACE:
			DBG("IPT_SO_SET_REPLACE");
			return 0;
		case IPT_SO_SET_ADD_COUNTERS:
			DBG("IPT_SO_SET_ADD_COUNTERS");
			return 0;
		default:
			DBG("optname %d", optname);
			return -1;
		}

		break;
	case IPPROTO_IPV6:
		DBG("IPPROTO_IPV6");

		switch (optname) {
		case IP6T_SO_SET_REPLACE:
			DBG("IP6T_SO_SET_REPLACE");
			return 0;
		case IP6T_SO_SET_ADD_COUNTERS:
			DBG("IP6T_SO_SET_ADD_COUNTERS");
			return 0;
		default:
			DBG("optname %d", optname);
			return -1;
		}

		break;
	default:
		return -1;
	}
}

/* End of socket dummies */

/* End of dummies */

static void iptables_test_basic0()
{
	set_test_config(TEST_CONFIG_PASS);

	__connman_iptables_init();

	g_assert(!__connman_iptables_new_chain(AF_INET, "filter", "INPUT"));
	g_assert_cmpint(__connman_iptables_insert(AF_INET, "filter", "INPUT",
				"-p tcp -m tcp --dport 42 -j ACCEPT"), ==, 0);

	__connman_iptables_cleanup();
}

/*
 * These ok0...ok6 tests test the error handling. The setjmp() position is set
 * properly for the functions that will trigger it and as a result, depending on
 * iptables.c, there will be an error or no error at all. Each of these should
 * return gracefully without calling exit().
 */

static void iptables_test_jmp_ok0()
{
	set_test_config(TEST_CONFIG_FIND_MATCH_FAIL);

	__connman_iptables_init();

	g_assert(!__connman_iptables_new_chain(AF_INET, "filter", "INPUT"));
	g_assert_cmpint(__connman_iptables_insert(AF_INET, "filter", "INPUT",
				"-p tcp -m tcp -j ACCEPT"), ==, -EINVAL);

	__connman_iptables_cleanup();
}

static void iptables_test_jmp_ok1()
{
	set_test_config(TEST_CONFIG_FIND_TARGET_FAIL);

	__connman_iptables_init();

	g_assert(!__connman_iptables_new_chain(AF_INET, "filter", "INPUT"));
	g_assert_cmpint(__connman_iptables_insert(AF_INET, "filter", "INPUT",
				"-p tcp -m tcp -j ACCEPT"), ==, -EINVAL);

	__connman_iptables_cleanup();
}

static void iptables_test_jmp_ok2()
{
	set_test_config(TEST_CONFIG_PARSE_PROTOCOL_FAIL);

	__connman_iptables_init();

	g_assert(!__connman_iptables_new_chain(AF_INET, "filter", "INPUT"));
	g_assert_cmpint(__connman_iptables_insert(AF_INET, "filter", "INPUT",
				"-p tcp -m tcp --dport 42 -j ACCEPT"), ==,
				-EINVAL);

	__connman_iptables_cleanup();
}

static void iptables_test_jmp_ok3()
{
	set_test_config(TEST_CONFIG_TFCALL_FAIL);

	__connman_iptables_init();

	g_assert(!__connman_iptables_new_chain(AF_INET, "filter", "INPUT"));
	g_assert_cmpint(__connman_iptables_insert(AF_INET, "filter", "INPUT",
				"-p tcp -m tcp --dport 42 -j ACCEPT"), ==,
				-EINVAL);

	__connman_iptables_cleanup();
}

static void iptables_test_jmp_ok4()
{
	set_test_config(TEST_CONFIG_MFCALL_FAIL);

	__connman_iptables_init();

	g_assert(!__connman_iptables_new_chain(AF_INET, "filter", "INPUT"));

	g_assert_cmpint(__connman_iptables_insert(AF_INET, "filter", "INPUT",
				"-p tcp -m tcp --dport 42 -j ACCEPT"), ==,
				-EINVAL);

	__connman_iptables_cleanup();
}

static void iptables_test_jmp_ok5()
{
	set_test_config(TEST_CONFIG_TPCALL_FAIL);

	__connman_iptables_init();

	g_assert(!__connman_iptables_new_chain(AF_INET, "filter", "INPUT"));

	g_assert_cmpint(__connman_iptables_insert(AF_INET, "filter", "INPUT",
				"-p tcp -m tcp --dport 42 -j ACCEPT "
				"--comment test"), ==, -EINVAL);

	__connman_iptables_cleanup();
}

static void iptables_test_jmp_ok6()
{
	set_test_config(TEST_CONFIG_MPCALL_FAIL);

	__connman_iptables_init();

	g_assert(!__connman_iptables_new_chain(AF_INET, "filter", "INPUT"));

	g_assert_cmpint(__connman_iptables_insert(AF_INET, "filter", "INPUT",
				"-p tcp -m tcp --dport 42 -j ACCEPT"), ==,
				-EINVAL);

	__connman_iptables_cleanup();
}

/*
 * These exit0...exit2 tests invoke longjmp() via xtables exit_err() without
 * having env saved with setjmp(). All of these will result calling exit(), thus
 * forking is required.
 */

static void iptables_test_jmp_exit0()
{
	pid_t cpid = 0;
	int cstatus = 0;

	/*
	 * Should work as normal but fork() is needed as exit() is called
	 * when longjmp() is not allowed. At xtables_init_all() exit_err() is
	 * not normally called.
	 */
	set_test_config(TEST_CONFIG_INIT_FAIL);

	/* Child, run iptables test */
	if (fork() == 0) {
		__connman_iptables_init();

		/*
		 * Address family must be different from previous use because
		 * otherwise xtables_init_all() is not called.
		 */
		g_assert(!__connman_iptables_new_chain(AF_INET6, "filter",
					"INPUT"));

		__connman_iptables_cleanup();
		exit(0);
	} else {
		cpid = wait(&cstatus); /* Wait for child */
	}

	DBG("parent %d child %d exit %d", getpid(), cpid, WEXITSTATUS(cstatus));

	g_assert(WIFEXITED(cstatus));
	g_assert_cmpint(WEXITSTATUS(cstatus), ==, PARAMETER_PROBLEM);
}

static void iptables_test_jmp_exit1()
{
	pid_t cpid = 0;
	int cstatus = 0;

	/*
	 * Should work as normal but fork() is needed as exit() is called
	 * when longjmp() is not allowed. At xtables_insmod() exit_err() is not
	 * normally called.
	 */
	set_test_config(TEST_CONFIG_INSMOD_FAIL);

	if (fork() == 0) {
		__connman_iptables_init();

		g_assert(!__connman_iptables_new_chain(AF_INET, "filter",
					"INPUT"));

		__connman_iptables_cleanup();
		exit(0);
	} else {
		cpid = wait(&cstatus);
	}

	DBG("parent %d child %d exit %d", getpid(), cpid, WEXITSTATUS(cstatus));

	g_assert(WIFEXITED(cstatus));
	g_assert_cmpint(WEXITSTATUS(cstatus), ==, PARAMETER_PROBLEM);
}

static void iptables_test_jmp_exit2()
{
	pid_t cpid = 0;
	int cstatus = 0;

	set_test_config(TEST_CONFIG_OPTIONS_XFRM_FAIL|
				TEST_CONFIG_MERGE_OPTIONS_FAIL|
				TEST_CONFIG_COMPATIBLE_REV_FAIL);

	if (fork() == 0) {
		__connman_iptables_init();

		g_assert(!__connman_iptables_new_chain(AF_INET, "filter",
					"INPUT"));
		g_assert_cmpint(__connman_iptables_insert(AF_INET, "filter",
					"INPUT", "-p tcp -m tcp --dport 42 "
					"-j ACCEPT --comment test"), ==, 0);

		__connman_iptables_cleanup();
		exit(0);
	} else {
		cpid = wait(&cstatus);
	}

	DBG("parent %d child %d exit %d", getpid(), cpid, WEXITSTATUS(cstatus));

	g_assert(WIFEXITED(cstatus));
	g_assert_cmpint(WEXITSTATUS(cstatus), ==, PARAMETER_PROBLEM);
}

static gchar *option_debug = NULL;

static bool parse_debug(const char *key, const char *value,
					gpointer user_data, GError **error)
{
	if (value)
		option_debug = g_strdup(value);
	else
		option_debug = g_strdup("*");

	return true;
}

static GOptionEntry options[] = {
	{ "debug", 'd', G_OPTION_FLAG_OPTIONAL_ARG,
				G_OPTION_ARG_CALLBACK, parse_debug,
				"Specify debug options to enable", "DEBUG" },
	{ NULL },
};

int main (int argc, char *argv[])
{
	GOptionContext *context;
	GError *error = NULL;

	g_test_init(&argc, &argv, NULL);

	context = g_option_context_new(NULL);
	g_option_context_add_main_entries(context, options, NULL);

	if (!g_option_context_parse(context, &argc, &argv, &error)) {
		if (error) {
			g_printerr("%s\n", error->message);
			g_error_free(error);
		} else
			g_printerr("An unknown error occurred\n");
		return 1;
	}

	g_option_context_free(context);

	__connman_log_init(argv[0], option_debug, false, false,
			"Unit Tests Connection Manager", VERSION);

	g_test_add_func("/iptables/test_basic0", iptables_test_basic0);
	g_test_add_func("/iptables/test_jmp_ok0", iptables_test_jmp_ok0);
	g_test_add_func("/iptables/test_jmp_ok1", iptables_test_jmp_ok1);
	g_test_add_func("/iptables/test_jmp_ok2", iptables_test_jmp_ok2);
	g_test_add_func("/iptables/test_jmp_ok3", iptables_test_jmp_ok3);
	g_test_add_func("/iptables/test_jmp_ok4", iptables_test_jmp_ok4);
	g_test_add_func("/iptables/test_jmp_ok5", iptables_test_jmp_ok5);
	g_test_add_func("/iptables/test_jmp_ok6", iptables_test_jmp_ok6);
	g_test_add_func("/iptables/test_jmp_exit0", iptables_test_jmp_exit0);
	g_test_add_func("/iptables/test_jmp_exit1", iptables_test_jmp_exit1);
	g_test_add_func("/iptables/test_jmp_exit2", iptables_test_jmp_exit2);

	return g_test_run();
}

/*
 * Local Variables:
 * mode: C
 * c-basic-offset: 8
 * indent-tabs-mode: t
 * End:
 */
