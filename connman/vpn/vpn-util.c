/*
 *  ConnMan VPN daemon utils
 *
 *  Copyright (C) 2020  Jolla Ltd. All rights reserved.
 *  Copyright (C) 2020  Open Mobile Platform LLC.
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
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include <glib/gstdio.h>

#include <connman/log.h>

#include "vpn.h"

static bool is_string_digits(const char *str)
{
	int i;

	if (!str || !*str)
		return false;

	for (i = 0; str[i]; i++) {
		if (!g_ascii_isdigit(str[i]))
			return false;
	}

	return true;
}

static uid_t get_str_id(const char *username)
{
	if (!username || !*username)
		return 0;

	return (uid_t)g_ascii_strtoull(username, NULL, 10);
}

struct passwd *vpn_util_get_passwd(const char *username)
{
	struct passwd *pwd;
	uid_t uid;

	if (!username || !*username)
		return NULL;

	if (is_string_digits(username)) {
		uid = get_str_id(username);
		pwd = getpwuid(uid);
	} else {
		pwd = getpwnam(username);
	}

	return pwd;
}

struct group *vpn_util_get_group(const char *groupname)
{
	struct group *grp;
	gid_t gid;

	if (!groupname || !*groupname)
		return NULL;

	if (is_string_digits(groupname)) {
		gid = get_str_id(groupname);
		grp = getgrgid(gid);
	} else {
		grp = getgrnam(groupname);
	}

	return grp;
}

/*
 * These prefixes are used for checking if the requested path for
 * vpn_util_create_path() is acceptable. Allow only prefixes meant for run-time
 * or temporary use to limit the access to any system resources.
 *
 * VPN core and plugins would need to create only temporary dirs for the
 * run-time use. The requested dirs can be created for a specific user when
 * running a VPN plugin as a different user and thus, user specific run dir is
 * allowed and limitation to access any other system dir is restricted.
 */
static const char *allowed_prefixes[] = { "/var/run/connman-vpn/",
					"/var/run/user/", "/tmp/", NULL };

static int is_path_allowed(const char *path)
{
	int err = -EPERM;
	int i;

	if (!path || !*path || !g_path_is_absolute(path))
		return -EINVAL;

	if (g_strrstr(path, "..") || g_strrstr(path, "./"))
		return -EPERM;

	for (i = 0; allowed_prefixes[i]; i++) {
		if (g_str_has_prefix(path, allowed_prefixes[i])) {
			const char *suffix = path +
						strlen(allowed_prefixes[i]);

			/*
			 * Don't allow plain prefixes, an additional dir must
			 * be included after the prexix in the requested path.
			 */
			if (suffix && *suffix != G_DIR_SEPARATOR &&
						g_strrstr(suffix,
							G_DIR_SEPARATOR_S)) {
				DBG("allowed %s, has suffix %s", path, suffix);
				err = 0;
			}

			break;
		}
	}

	return err;
}

int vpn_util_create_path(const char *path, uid_t uid, gid_t grp, int mode)
{
	mode_t old_umask;
	char *dir_p;
	int err;

	err = is_path_allowed(path);
	if (err)
		return err;

	dir_p = g_path_get_dirname(path);
	if (!dir_p)
		return -ENOMEM;

	err = g_unlink(dir_p);
	if (err)
		err = -errno;

	switch (err) {
	case 0:
		/* Removed */
	case -ENOENT:
		/* Did not exist */
		break;
	case -EACCES:
		/*
		 * Cannot get write access to the containing directory, check
		 * if the path exists.
		 */
		if (!g_file_test(dir_p, G_FILE_TEST_EXISTS))
			goto out;

		/* If the dir does not exist new one cannot be created */
		if (!g_file_test(dir_p, G_FILE_TEST_IS_DIR))
			goto out;

		/* Do a chmod as the dir exists */
		/* fallthrough */
	case -EISDIR:
		/* Exists as dir, just chmod and change owner */
		err = g_chmod(dir_p, mode);
		if (err) {
			connman_warn("chmod %s failed, err %d", dir_p, err);
			err = -errno;
		}

		goto chown;
	default:
		/* Any other error that is not handled here */
		connman_warn("remove %s failed, err %d", dir_p, err);
		goto out;
	}

	/* Set dir creation mask to correspond to the mode */
	old_umask = umask(~mode & 0777);

	DBG("mkdir %s", dir_p);
	err = g_mkdir_with_parents(dir_p, mode);

	umask(old_umask);

	if (err) {
		connman_warn("mkdir %s failed, err %d", dir_p, err);
		err = -errno;
		goto out;
	}

chown:
	if (uid && grp) {
		err = chown(dir_p, uid, grp);
		if (err) {
			err = -errno;
			connman_warn("chown %s failed for %d/%d, err %d",
							dir_p, uid, grp, err);
		}
	}

out:
	g_free(dir_p);

	return err;
}

