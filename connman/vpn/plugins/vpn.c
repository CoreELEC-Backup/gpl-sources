/*
 *
 *  ConnMan VPN daemon
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

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/if_tun.h>
#include <net/if.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include <dbus/dbus.h>

#include <glib/gprintf.h>

#include <connman/log.h>
#include <connman/rtnl.h>
#include <connman/task.h>
#include <connman/inet.h>

#include "../vpn-rtnl.h"
#include "../vpn-provider.h"

#include "vpn.h"
#include "../vpn.h"

struct vpn_data {
	struct vpn_provider *provider;
	char *if_name;
	unsigned flags;
	unsigned int watch;
	enum vpn_state state;
	struct connman_task *task;
	int tun_flags;
};

struct vpn_driver_data {
	const char *name;
	const char *program;
	const struct vpn_driver *vpn_driver;
	struct vpn_provider_driver provider_driver;
};

GHashTable *driver_hash = NULL;

static int stop_vpn(struct vpn_provider *provider)
{
	struct vpn_data *data = vpn_provider_get_data(provider);
	struct vpn_driver_data *vpn_driver_data;
	const char *name;
	struct ifreq ifr;
	int fd, err;

	if (!data)
		return -EINVAL;

	name = vpn_provider_get_driver_name(provider);
	if (!name)
		return -EINVAL;

	vpn_driver_data = g_hash_table_lookup(driver_hash, name);

	if (vpn_driver_data && vpn_driver_data->vpn_driver &&
			vpn_driver_data->vpn_driver->flags & VPN_FLAG_NO_TUN) {
		vpn_driver_data->vpn_driver->disconnect(data->provider);
		return 0;
	}

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = data->tun_flags | IFF_NO_PI;
	sprintf(ifr.ifr_name, "%s", data->if_name);

	fd = open("/dev/net/tun", O_RDWR | O_CLOEXEC);
	if (fd < 0) {
		err = -errno;
		connman_error("Failed to open /dev/net/tun to device %s: %s",
			      data->if_name, strerror(errno));
		return err;
	}

	if (ioctl(fd, TUNSETIFF, (void *)&ifr)) {
		err = -errno;
		connman_error("Failed to TUNSETIFF for device %s to it: %s",
			      data->if_name, strerror(errno));
		close(fd);
		return err;
	}

	if (ioctl(fd, TUNSETPERSIST, 0)) {
		err = -errno;
		connman_error("Failed to set tun device %s nonpersistent: %s",
			      data->if_name, strerror(errno));
		close(fd);
		return err;
	}
	close(fd);
	DBG("Killed tun device %s", data->if_name);
	return 0;
}

void vpn_died(struct connman_task *task, int exit_code, void *user_data)
{
	struct vpn_provider *provider = user_data;
	struct vpn_data *data = vpn_provider_get_data(provider);
	int state = VPN_STATE_FAILURE;
	enum vpn_provider_error ret;

	DBG("provider %p data %p", provider, data);

	if (!data)
		goto vpn_exit;

	/* The task may die after we have already started the new one */
	if (data->task != task)
		goto done;

	state = data->state;

	stop_vpn(provider);
	vpn_provider_set_data(provider, NULL);

	if (data->watch != 0) {
		vpn_rtnl_remove_watch(data->watch);
		data->watch = 0;
		vpn_provider_unref(provider);
	}

vpn_exit:
	if (state != VPN_STATE_READY && state != VPN_STATE_DISCONNECT) {
		const char *name;
		struct vpn_driver_data *vpn_data = NULL;

		name = vpn_provider_get_driver_name(provider);
		if (name)
			vpn_data = g_hash_table_lookup(driver_hash, name);

		if (vpn_data &&
				vpn_data->vpn_driver->error_code)
			ret = vpn_data->vpn_driver->error_code(provider,
					exit_code);
		else
			ret = VPN_PROVIDER_ERROR_UNKNOWN;

		vpn_provider_indicate_error(provider, ret);
	} else
		vpn_provider_set_state(provider, VPN_PROVIDER_STATE_IDLE);

	vpn_provider_set_index(provider, -1);

	if (data) {
		vpn_provider_unref(data->provider);
		g_free(data->if_name);
		g_free(data);
	}

done:
	connman_task_destroy(task);
}

int vpn_set_ifname(struct vpn_provider *provider, const char *ifname)
{
	struct vpn_data *data = vpn_provider_get_data(provider);
	int index;

	if (!ifname || !data)
		return  -EIO;

	index = connman_inet_ifindex(ifname);
	if (index < 0)
		return  -EIO;

	if (data->if_name)
		g_free(data->if_name);

	data->if_name = (char *)g_strdup(ifname);
	vpn_provider_set_index(provider, index);

	return 0;
}

static int vpn_set_state(struct vpn_provider *provider,
						enum vpn_provider_state state)
{
	struct vpn_data *data = vpn_provider_get_data(provider);
	if (!data)
		return -EINVAL;

	switch (state) {
	case VPN_PROVIDER_STATE_UNKNOWN:
		return -EINVAL;
	case VPN_PROVIDER_STATE_IDLE:
		data->state = VPN_STATE_IDLE;
		break;
	case VPN_PROVIDER_STATE_CONNECT:
	case VPN_PROVIDER_STATE_READY:
		data->state = VPN_STATE_CONNECT;
		break;
	case VPN_PROVIDER_STATE_DISCONNECT:
		data->state = VPN_STATE_DISCONNECT;
		break;
	case VPN_PROVIDER_STATE_FAILURE:
		data->state = VPN_STATE_FAILURE;
		break;
	}

	return 0;
}

static void vpn_newlink(unsigned flags, unsigned change, void *user_data)
{
	struct vpn_provider *provider = user_data;
	struct vpn_data *data = vpn_provider_get_data(provider);

	if ((data->flags & IFF_UP) != (flags & IFF_UP)) {
		if (flags & IFF_UP) {
			data->state = VPN_STATE_READY;
			vpn_provider_set_state(provider,
					VPN_PROVIDER_STATE_READY);
		}
	}
	data->flags = flags;
}

static DBusMessage *vpn_notify(struct connman_task *task,
			DBusMessage *msg, void *user_data)
{
	struct vpn_provider *provider = user_data;
	struct vpn_data *data;
	struct vpn_driver_data *vpn_driver_data;
	const char *name;
	int state, index, err;

	data = vpn_provider_get_data(provider);

	name = vpn_provider_get_driver_name(provider);

	if (!name) {
		DBG("Cannot find VPN driver for provider %p", provider);
		vpn_provider_set_state(provider, VPN_PROVIDER_STATE_FAILURE);
		return NULL;
	}

	vpn_driver_data = g_hash_table_lookup(driver_hash, name);
	if (!vpn_driver_data) {
		DBG("Cannot find VPN driver data for name %s", name);
		vpn_provider_set_state(provider, VPN_PROVIDER_STATE_FAILURE);
		return NULL;
	}

	state = vpn_driver_data->vpn_driver->notify(msg, provider);

	DBG("provider %p driver %s state %d", provider, name, state);

	switch (state) {
	case VPN_STATE_CONNECT:
	case VPN_STATE_READY:
		if (data->state == VPN_STATE_READY) {
			/*
			 * This is the restart case, in which case we must
			 * just set the IP address.
			 *
			 * We need to remove first the old address, just
			 * replacing the old address will not work as expected
			 * because the old address will linger in the interface
			 * and not disappear so the clearing is needed here.
			 *
			 * Also the state must change, otherwise the routes
			 * will not be set properly.
			 */
			vpn_provider_set_state(provider,
						VPN_PROVIDER_STATE_CONNECT);

			vpn_provider_clear_address(provider, AF_INET);
			vpn_provider_clear_address(provider, AF_INET6);

			vpn_provider_change_address(provider);
			vpn_provider_set_state(provider,
						VPN_PROVIDER_STATE_READY);
			break;
		}

		index = vpn_provider_get_index(provider);
		vpn_provider_ref(provider);
		data->watch = vpn_rtnl_add_newlink_watch(index,
						     vpn_newlink, provider);
		err = connman_inet_ifup(index);
		if (err < 0) {
			if (err == -EALREADY) {
				/*
				 * So the interface is up already, that is just
				 * great. Unfortunately in this case the
				 * newlink watch might not have been called at
				 * all. We must manually call it here so that
				 * the provider can go to ready state and the
				 * routes are setup properly. Also reset flags
				 * so vpn_newlink() can handle the change.
				 */
				data->flags = 0;
				vpn_newlink(IFF_UP, 0, provider);
			} else {
				DBG("Cannot take interface %d up err %d/%s",
					index, -err, strerror(-err));
			}
		}
		break;

	case VPN_STATE_UNKNOWN:
	case VPN_STATE_IDLE:
	case VPN_STATE_DISCONNECT:
	case VPN_STATE_FAILURE:
		vpn_provider_set_state(provider,
					VPN_PROVIDER_STATE_DISCONNECT);
		break;

	case VPN_STATE_AUTH_FAILURE:
		vpn_provider_indicate_error(provider,
					VPN_PROVIDER_ERROR_AUTH_FAILED);
		break;
	}

	return NULL;
}

static int vpn_create_tun(struct vpn_provider *provider, int flags)
{
	struct vpn_data *data = vpn_provider_get_data(provider);
	struct ifreq ifr;
	int i, fd, index;
	int ret = 0;

	if (!data)
		return -EISCONN;

	fd = open("/dev/net/tun", O_RDWR | O_CLOEXEC);
	if (fd < 0) {
		i = -errno;
		connman_error("Failed to open /dev/net/tun: %s",
			      strerror(errno));
		ret = i;
		goto exist_err;
	}

	memset(&ifr, 0, sizeof(ifr));
	ifr.ifr_flags = flags | IFF_NO_PI;

	for (i = 0; i < 256; i++) {
		sprintf(ifr.ifr_name, "vpn%d", i);

		if (!ioctl(fd, TUNSETIFF, (void *)&ifr))
			break;
	}

	if (i == 256) {
		connman_error("Failed to find available tun device");
		close(fd);
		ret = -ENODEV;
		goto exist_err;
	}

	data->tun_flags = flags;
	g_free(data->if_name);
	data->if_name = (char *)g_strdup(ifr.ifr_name);
	if (!data->if_name) {
		connman_error("Failed to allocate memory");
		close(fd);
		ret = -ENOMEM;
		goto exist_err;
	}

	if (ioctl(fd, TUNSETPERSIST, 1)) {
		i = -errno;
		connman_error("Failed to set tun persistent: %s",
			      strerror(errno));
		close(fd);
		ret = i;
		goto exist_err;
	}

	close(fd);

	index = connman_inet_ifindex(data->if_name);
	if (index < 0) {
		connman_error("Failed to get tun ifindex");
		stop_vpn(provider);
		ret = -EIO;
		goto exist_err;
	}
	vpn_provider_set_index(provider, index);

	return 0;

exist_err:
	return ret;
}

static gid_t get_gid(const char *group_name)
{
	struct group *grp;

	grp = vpn_util_get_group(group_name);
	if (grp)
		return grp->gr_gid;

	return -1;
}

static uid_t get_uid(const char *user_name)
{
	struct passwd *pw;

	pw = vpn_util_get_passwd(user_name);
	if (pw)
		return pw->pw_uid;

	return -1;
}

static gint get_supplementary_gids(gchar **groups, gid_t **gid_list)
{
	gint group_count = 0;
	gid_t *list = NULL;
	int i;

	if (groups) {
		for(i = 0; groups[i]; i++) {
			group_count++;

			list = (gid_t*)g_try_realloc(list,
						sizeof(gid_t) * group_count);

			if (!list) {
				DBG("cannot allocate supplementary group list");
				break;
			}

			list[i] = get_gid(groups[i]);
		}
	}

	*gid_list = list;

	return group_count;
}

static void vpn_task_setup(gpointer user_data)
{
	struct vpn_plugin_data *data;
	uid_t uid;
	gid_t gid;
	gid_t *gid_list = NULL;
	size_t gid_list_size;
	const gchar *user;
	const gchar *group;
	gchar **suppl_groups;

	data = user_data;
	user = vpn_settings_get_binary_user(data);
	group = vpn_settings_get_binary_group(data);
	suppl_groups = vpn_settings_get_binary_supplementary_groups(data);

	uid = get_uid(user);
	gid = get_gid(group);
	gid_list_size = get_supplementary_gids(suppl_groups, &gid_list);

	DBG("vpn_task_setup uid:%d gid:%d supplementary group list size:%zu",
					uid, gid, gid_list_size);


	/* Change group if proper group name was set, requires CAP_SETGID.*/
	if (gid > 0 && setgid(gid))
		connman_error("error setting gid %d %s", gid, strerror(errno));

	/* Set the supplementary groups if list exists, requires CAP_SETGID. */
	if (gid_list_size && gid_list && setgroups(gid_list_size, gid_list))
		connman_error("error setting gid list %s", strerror(errno));

	/* Change user for the task if set, requires CAP_SETUID */
	if (uid > 0 && setuid(uid))
		connman_error("error setting uid %d %s", uid, strerror(errno));
}


static gboolean update_provider_state(gpointer data)
{
	struct vpn_provider *provider = data;
	struct vpn_data *vpn_data;
	int index;

	DBG("");

	vpn_data = vpn_provider_get_data(provider);

	index = vpn_provider_get_index(provider);
	DBG("index to watch %d", index);
	vpn_provider_ref(provider);
	vpn_data->watch = vpn_rtnl_add_newlink_watch(index,
						vpn_newlink, provider);
	connman_inet_ifup(index);

	return FALSE;
}

static int vpn_connect(struct vpn_provider *provider,
			vpn_provider_connect_cb_t cb,
			const char *dbus_sender, void *user_data)
{
	struct vpn_data *data = vpn_provider_get_data(provider);
	struct vpn_driver_data *vpn_driver_data;
	struct vpn_plugin_data *vpn_plugin_data;
	const char *name;
	int ret = 0, tun_flags = IFF_TUN;
	enum vpn_state state = VPN_STATE_UNKNOWN;

	if (data)
		state = data->state;

	DBG("data %p state %d", data, state);

	switch (state) {
	case VPN_STATE_UNKNOWN:
		data = g_try_new0(struct vpn_data, 1);
		if (!data)
			return -ENOMEM;

		data->provider = vpn_provider_ref(provider);
		data->watch = 0;
		data->flags = 0;
		data->task = NULL;

		vpn_provider_set_data(provider, data);
		/* fall through */

	case VPN_STATE_DISCONNECT:
	case VPN_STATE_IDLE:
	case VPN_STATE_FAILURE:
	case VPN_STATE_AUTH_FAILURE:
		data->state = VPN_STATE_IDLE;
		break;

	case VPN_STATE_CONNECT:
		return -EINPROGRESS;

	case VPN_STATE_READY:
		return -EISCONN;
	}

	name = vpn_provider_get_driver_name(provider);
	if (!name)
		return -EINVAL;

	vpn_driver_data = g_hash_table_lookup(driver_hash, name);

	if (!vpn_driver_data || !vpn_driver_data->vpn_driver) {
		ret = -EINVAL;
		goto exist_err;
	}

	if (!(vpn_driver_data->vpn_driver->flags & VPN_FLAG_NO_TUN)) {
		if (vpn_driver_data->vpn_driver->device_flags) {
			tun_flags = vpn_driver_data->vpn_driver->device_flags(provider);
		}
		ret = vpn_create_tun(provider, tun_flags);
		if (ret < 0)
			goto exist_err;
	}


	if (vpn_driver_data && vpn_driver_data->vpn_driver &&
			vpn_driver_data->vpn_driver->flags & VPN_FLAG_NO_DAEMON) {

		ret = vpn_driver_data->vpn_driver->connect(provider,
					NULL, NULL, cb, dbus_sender, user_data);
		if (ret) {
			stop_vpn(provider);
			goto exist_err;
		}

		DBG("%s started with dev %s",
			vpn_driver_data->provider_driver.name, data->if_name);

		data->state = VPN_STATE_CONNECT;

		g_timeout_add(1, update_provider_state, provider);
		return -EINPROGRESS;
	}

	vpn_plugin_data =
		vpn_settings_get_vpn_plugin_config(vpn_driver_data->name);
	data->task = connman_task_create(vpn_driver_data->program,
					vpn_task_setup, vpn_plugin_data);

	if (!data->task) {
		ret = -ENOMEM;
		stop_vpn(provider);
		goto exist_err;
	}

	if (connman_task_set_notify(data->task, "notify",
					vpn_notify, provider)) {
		ret = -ENOMEM;
		stop_vpn(provider);
		connman_task_destroy(data->task);
		data->task = NULL;
		goto exist_err;
	}

	ret = vpn_driver_data->vpn_driver->connect(provider, data->task,
						data->if_name, cb, dbus_sender,
						user_data);
	if (ret < 0 && ret != -EINPROGRESS) {
		stop_vpn(provider);
		connman_task_destroy(data->task);
		data->task = NULL;
		goto exist_err;
	}

	DBG("%s started with dev %s",
		vpn_driver_data->provider_driver.name, data->if_name);

	data->state = VPN_STATE_CONNECT;

	return -EINPROGRESS;

exist_err:
	vpn_provider_set_index(provider, -1);
	vpn_provider_set_data(provider, NULL);
	vpn_provider_unref(data->provider);
	g_free(data->if_name);
	g_free(data);

	return ret;
}

static int vpn_probe(struct vpn_provider *provider)
{
	return 0;
}

static int vpn_disconnect(struct vpn_provider *provider)
{
	struct vpn_data *data = vpn_provider_get_data(provider);
	struct vpn_driver_data *vpn_driver_data;
	const char *name;

	DBG("disconnect provider %p:", provider);

	if (!data)
		return 0;

	name = vpn_provider_get_driver_name(provider);
	if (!name)
		return 0;

	vpn_driver_data = g_hash_table_lookup(driver_hash, name);
	if (vpn_driver_data->vpn_driver->disconnect)
		vpn_driver_data->vpn_driver->disconnect(provider);

	if (data->watch != 0) {
		vpn_provider_unref(provider);
		vpn_rtnl_remove_watch(data->watch);
		data->watch = 0;
	}

	data->state = VPN_STATE_DISCONNECT;

	if (!vpn_driver_data->vpn_driver->disconnect) {
		DBG("Driver has no disconnect() implementation, set provider "
					"state to disconnect.");
		vpn_provider_set_state(provider, VPN_PROVIDER_STATE_DISCONNECT);
	}

	if (data->task)
		connman_task_stop(data->task);

	return 0;
}

static int vpn_remove(struct vpn_provider *provider)
{
	struct vpn_data *data;
	struct vpn_driver_data *driver_data;
	const char *name;
	int err = 0;

	data = vpn_provider_get_data(provider);
	name = vpn_provider_get_driver_name(provider);

	if (!data)
		goto call_remove;

	if (data->watch != 0) {
		vpn_provider_unref(provider);
		vpn_rtnl_remove_watch(data->watch);
		data->watch = 0;
	}

	if (data->task)
		connman_task_stop(data->task);

	g_usleep(G_USEC_PER_SEC);
	stop_vpn(provider);

call_remove:
	if (!name)
		return 0;

	driver_data = g_hash_table_lookup(driver_hash, name);

	if (driver_data && driver_data->vpn_driver->remove)
		err = driver_data->vpn_driver->remove(provider);

	if (err)
		DBG("%p vpn_driver->remove() returned %d", provider, err);

	return err;
}

static int vpn_save(struct vpn_provider *provider, GKeyFile *keyfile)
{
	struct vpn_driver_data *vpn_driver_data;
	const char *name;

	name = vpn_provider_get_driver_name(provider);
	vpn_driver_data = g_hash_table_lookup(driver_hash, name);
	if (vpn_driver_data &&
			vpn_driver_data->vpn_driver->save)
		return vpn_driver_data->vpn_driver->save(provider, keyfile);

	return 0;
}

static int vpn_route_env_parse(struct vpn_provider *provider, const char *key,
			int *family, unsigned long *idx,
			enum vpn_provider_route_type *type)
{
	struct vpn_driver_data *vpn_driver_data = NULL;
	const char *name = NULL;

	if (!provider)
		return -EINVAL;

	name = vpn_provider_get_driver_name(provider);
	vpn_driver_data = g_hash_table_lookup(driver_hash, name);

	if (vpn_driver_data && vpn_driver_data->vpn_driver->route_env_parse)
		return vpn_driver_data->vpn_driver->route_env_parse(provider, key,
			family, idx, type);

	return 0;
}

int vpn_register(const char *name, const struct vpn_driver *vpn_driver,
			const char *program)
{
	struct vpn_driver_data *data;

	data = g_try_new0(struct vpn_driver_data, 1);
	if (!data)
		return -ENOMEM;

	data->name = name;
	data->program = program;

	if (vpn_settings_parse_vpn_plugin_config(data->name) != 0)
		DBG("No configuration provided for VPN plugin %s", data->name);

	data->vpn_driver = vpn_driver;

	data->provider_driver.name = name;
	data->provider_driver.disconnect = vpn_disconnect;
	data->provider_driver.connect = vpn_connect;
	data->provider_driver.probe = vpn_probe;
	data->provider_driver.remove = vpn_remove;
	data->provider_driver.save = vpn_save;
	data->provider_driver.set_state = vpn_set_state;
	data->provider_driver.route_env_parse = vpn_route_env_parse;

	if (!driver_hash)
		driver_hash = g_hash_table_new_full(g_str_hash,
							g_str_equal,
							NULL, g_free);

	if (!driver_hash) {
		connman_error("driver_hash not initialized for %s", name);
		g_free(data);
		return -ENOMEM;
	}

	g_hash_table_replace(driver_hash, (char *)name, data);

	vpn_provider_driver_register(&data->provider_driver);

	return 0;
}

void vpn_unregister(const char *name)
{
	struct vpn_driver_data *data;

	data = g_hash_table_lookup(driver_hash, name);
	if (!data)
		return;

	vpn_provider_driver_unregister(&data->provider_driver);
	vpn_settings_delete_vpn_plugin_config(name);

	g_hash_table_remove(driver_hash, name);

	if (g_hash_table_size(driver_hash) == 0)
		g_hash_table_destroy(driver_hash);
}
