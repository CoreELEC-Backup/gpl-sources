/*
 *  megatools - Mega.nz client library and tools
 *  Copyright (C) 2013  Ondřej Jirman <megous@megous.com>
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
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "tools.h"

static gchar *opt_name;
static gchar *opt_email;
static gchar *opt_password;
static gboolean opt_register;
static gchar *opt_verify;
static gboolean opt_script;

static GOptionEntry entries[] = {
	{ "name", 'n', 0, G_OPTION_ARG_STRING, &opt_name, "Your real name", "NAME" },
	{ "email", 'e', 0, G_OPTION_ARG_STRING, &opt_email, "Your email (will be your username)", "EMAIL" },
	{ "password", 'p', 0, G_OPTION_ARG_STRING, &opt_password, "Your password", "PASSWORD" },
	{ "register", '\0', 0, G_OPTION_ARG_NONE, &opt_register, "Perform registration", NULL },
	{ "verify", '\0', 0, G_OPTION_ARG_STRING, &opt_verify, "Finish registration (pass verification link)",
	  "STATE" },
	{ "scripted", '\0', 0, G_OPTION_ARG_NONE, &opt_script, "Return script friendly output from --register", NULL },
	{ NULL }
};

static gchar *serialize_reg_state(struct mega_reg_state *state)
{
	gc_free gchar *pk = g_base64_encode(state->password_key, 16);
	gc_free gchar *ch = g_base64_encode(state->challenge, 16);

	return g_strdup_printf("%s:%s:%s", pk, ch, state->user_handle);
}

static struct mega_reg_state *unserialize_reg_state(const gchar *str)
{
	gc_match_info_unref GMatchInfo *m = NULL;
	gc_regex_unref GRegex *r = g_regex_new("^([a-z0-9/+=]{24}):([a-z0-9/+=]{24}):(.*)$", G_REGEX_CASELESS, 0, NULL);
	gsize len;

	if (!r)
		return NULL;

	if (!g_regex_match(r, str, 0, &m))
		return NULL;

	gc_free gchar *pk = g_match_info_fetch(m, 1);
	gc_free gchar *ch = g_match_info_fetch(m, 2);
	gc_free guchar *decoded_ch = NULL;
	gc_free guchar *decoded_pk = NULL;

	struct mega_reg_state *state = g_new0(struct mega_reg_state, 1);
	state->user_handle = g_match_info_fetch(m, 3);
	;

	decoded_pk = g_base64_decode(pk, &len);
	if (!decoded_pk)
		goto err;

	if (len != 16)
		goto err;

	memcpy(state->password_key, decoded_pk, 16);

	decoded_ch = g_base64_decode(ch, &len);
	if (!decoded_ch)
		goto err;

	if (len != 16)
		goto err;

	memcpy(state->challenge, decoded_ch, 16);

	return state;

err:
	g_free(state->user_handle);
	g_free(state);
	return NULL;
}

int main(int ac, char *av[])
{
	gc_error_free GError *local_err = NULL;
	struct mega_reg_state *state = NULL;
	gc_free gchar *signup_key = NULL;
	struct mega_session *s;

	tool_init(&ac, &av, "LINK - register a new mega.nz account", entries, 0);

	if (opt_verify && opt_register) {
		g_printerr("ERROR: You must specify either --register or --verify option\n");
		return 1;
	}

	if (opt_register) {
		if (!opt_name) {
			g_printerr("ERROR: You must specify name for your new mega.nz account\n");
			return 1;
		}

		if (!opt_email) {
			g_printerr("ERROR: You must specify email for your new mega.nz account\n");
			return 1;
		}

		if (!opt_password) {
			g_printerr("ERROR: You must specify password for your new mega.nz account\n");
			return 1;
		}

	} else if (opt_verify) {
		if (ac != 2) {
			g_printerr("ERROR: You must specify signup key and a link from the verification email\n");
			return 1;
		}

		gc_regex_unref GRegex *r = g_regex_new(
			"^(?:https?://mega(?:\\.co)?\\.nz/#confirm)?([a-z0-9_-]{80,512})$", G_REGEX_CASELESS, 0, NULL);
		gc_match_info_unref GMatchInfo *m = NULL;

		g_assert(r != NULL);

		if (!g_regex_match(r, av[1], 0, &m)) {
			g_printerr("ERROR: Invalid verification link or key: '%s'\n", av[1]);
			return 1;
		}

		signup_key = g_match_info_fetch(m, 1);
		state = unserialize_reg_state(opt_verify);
		if (!state) {
			g_printerr(
				"ERROR: Failed to decode registration state parameter, make sure you copied it correctly\n");
			return 1;
		}
	} else {
		g_printerr("ERROR: You must specify either --register or --verify option\n");
		return 1;
	}

	s = tool_start_session(0);
	if (!s) {
		tool_fini(NULL);
		return 1;
	}

	if (opt_register) {
		if (!mega_session_register(s, opt_email, opt_password, opt_name, &state, &local_err)) {
			g_printerr("ERROR: Registration failed: %s\n",
				   local_err ? local_err->message : "Unknown error");
			goto err;
		}

		gc_free gchar *serialized_state = serialize_reg_state(state);

		if (opt_script)
			g_print("%s --verify %s @LINK@\n", av[0], serialized_state);
		else
			g_print("Registration email was sent to %s. To complete registration, you must run:\n\n"
				"  %s --verify %s @LINK@\n\n"
				"(Where @LINK@ is registration link from the 'MEGA Signup' email)\n",
				opt_email, g_get_prgname(), serialized_state);
	}

	if (opt_verify) {
		if (!mega_session_register_verify(s, state, signup_key, &local_err)) {
			g_printerr("ERROR: Verification failed: %s\n",
				   local_err ? local_err->message : "Unknown error");
			goto err;
		}

		if (!opt_script)
			g_print("Account registered successfully!\n");
	}

	tool_fini(s);
	return 0;

err:
	tool_fini(s);
	return 1;
}
