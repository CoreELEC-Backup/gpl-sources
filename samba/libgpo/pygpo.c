/*
   Unix SMB/CIFS implementation.
   Copyright (C) Luke Morrison <luc785@hotmail.com> 2013

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Python.h>
#include "includes.h"
#include "version.h"
#include "param/pyparam.h"
#include "gpo.h"
#include "ads.h"
#include "secrets.h"
#include "../libds/common/flags.h"
#include "librpc/rpc/pyrpc_util.h"
#include "auth/credentials/pycredentials.h"
#include "libcli/util/pyerrors.h"
#include "python/py3compat.h"
#include "python/modules.h"

/* A Python C API module to use LIBGPO */

#define GPO_getter(ATTR) \
static PyObject* GPO_get_##ATTR(PyObject *self, void *closure) \
{ \
	struct GROUP_POLICY_OBJECT *gpo_ptr \
		= pytalloc_get_ptr(self); \
	\
	if (gpo_ptr->ATTR) \
		return PyUnicode_FromString(gpo_ptr->ATTR); \
	else \
		return Py_None; \
}
GPO_getter(ds_path)
GPO_getter(file_sys_path)
GPO_getter(display_name)
GPO_getter(name)
GPO_getter(link)
GPO_getter(user_extensions)
GPO_getter(machine_extensions)

static PyGetSetDef GPO_setters[] = {
	{discard_const_p(char, "ds_path"), (getter)GPO_get_ds_path, NULL, NULL,
		NULL},
	{discard_const_p(char, "file_sys_path"), (getter)GPO_get_file_sys_path,
		NULL, NULL, NULL},
	{discard_const_p(char, "display_name"), (getter)GPO_get_display_name,
		NULL, NULL, NULL},
	{discard_const_p(char, "name"), (getter)GPO_get_name, NULL, NULL,
		NULL},
	{discard_const_p(char, "link"), (getter)GPO_get_link, NULL, NULL,
		NULL},
	{discard_const_p(char, "user_extensions"),
		(getter)GPO_get_user_extensions,
		NULL, NULL, NULL},
	{discard_const_p(char, "machine_extensions"),
		(getter)GPO_get_machine_extensions, NULL, NULL, NULL},
	{0}
};

static PyObject *py_gpo_get_unix_path(PyObject *self, PyObject *args,
				      PyObject *kwds)
{
	NTSTATUS status;
	const char *cache_dir = NULL;
	PyObject *ret = NULL;
	char *unix_path = NULL;
	TALLOC_CTX *frame = NULL;
	static const char *kwlist[] = {"cache_dir", NULL};
	struct GROUP_POLICY_OBJECT *gpo_ptr \
		= (struct GROUP_POLICY_OBJECT *)pytalloc_get_ptr(self);

	frame = talloc_stackframe();

	if (!PyArg_ParseTupleAndKeywords(args, kwds, "|s",
					 discard_const_p(char *, kwlist),
					 &cache_dir)) {
		goto out;
	}

	if (!cache_dir) {
		cache_dir = cache_path(talloc_tos(), GPO_CACHE_DIR);
		if (!cache_dir) {
			PyErr_SetString(PyExc_MemoryError,
					"Failed to determine gpo cache dir");
			goto out;
		}
	}

	status = gpo_get_unix_path(frame, cache_dir, gpo_ptr, &unix_path);

	if (!NT_STATUS_IS_OK(status)) {
		PyErr_Format(PyExc_RuntimeError,
				"Failed to determine gpo unix path: %s",
				get_friendly_nt_error_msg(status));
		goto out;
	}

	ret = PyUnicode_FromString(unix_path);

out:
	TALLOC_FREE(frame);
	return ret;
}

static PyMethodDef GPO_methods[] = {
	{"get_unix_path", PY_DISCARD_FUNC_SIG(PyCFunction,
					      py_gpo_get_unix_path),
		METH_VARARGS | METH_KEYWORDS,
		NULL },
	{0}
};

static PyTypeObject GPOType = {
	PyVarObject_HEAD_INIT(NULL, 0)
	.tp_name = "gpo.GROUP_POLICY_OBJECT",
	.tp_doc = "GROUP_POLICY_OBJECT",
	.tp_getset = GPO_setters,
	.tp_methods = GPO_methods,
	.tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
};

typedef struct {
	PyObject_HEAD
	ADS_STRUCT *ads_ptr;
	PyObject *py_creds;
	struct cli_credentials *cli_creds;
} ADS;

static void py_ads_dealloc(ADS* self)
{
	ads_destroy(&(self->ads_ptr));
	Py_CLEAR(self->py_creds);
	Py_TYPE(self)->tp_free((PyObject*)self);
}

static PyObject* py_ads_connect(ADS *self, PyObject *Py_UNUSED(ignored));
static int py_ads_init(ADS *self, PyObject *args, PyObject *kwds)
{
	const char *realm = NULL;
	const char *workgroup = NULL;
	const char *ldap_server = NULL;
	PyObject *lp_obj = NULL;
	PyObject *py_creds = NULL;
	struct loadparm_context *lp_ctx = NULL;
	bool ok = false;

	static const char *kwlist[] = {
		"ldap_server", "loadparm_context", "credentials", NULL
	};
	if (!PyArg_ParseTupleAndKeywords(args, kwds, "sO|O",
					 discard_const_p(char *, kwlist),
					 &ldap_server, &lp_obj, &py_creds)) {
		return -1;
	}
	/* keep reference to the credentials. Clear any earlier ones */
	Py_CLEAR(self->py_creds);
	self->cli_creds = NULL;
	self->py_creds = py_creds;
	Py_XINCREF(self->py_creds);

	if (self->py_creds) {
		ok = py_check_dcerpc_type(self->py_creds, "samba.credentials",
					  "Credentials");
		if (!ok) {
			return -1;
		}
		self->cli_creds
			= PyCredentials_AsCliCredentials(self->py_creds);
	}

	ok = py_check_dcerpc_type(lp_obj, "samba.param", "LoadParm");
	if (!ok) {
		return -1;
	}
	lp_ctx = pytalloc_get_type(lp_obj, struct loadparm_context);
	if (lp_ctx == NULL) {
		return -1;
	}
	ok = lp_load_initial_only(lp_ctx->szConfigFile);
	if (!ok) {
		PyErr_Format(PyExc_RuntimeError, "Could not load config file '%s'",
				lp_ctx->szConfigFile);
		return -1;
	}

	if (self->cli_creds) {
		realm = cli_credentials_get_realm(self->cli_creds);
		workgroup = cli_credentials_get_domain(self->cli_creds);
	} else {
		realm = lp_realm();
		workgroup = lp_workgroup();
	}

	/* in case __init__ is called more than once */
	if (self->ads_ptr) {
		ads_destroy(&self->ads_ptr);
		self->ads_ptr = NULL;
	}
	/* always succeeds or crashes */
	self->ads_ptr = ads_init(realm, workgroup, ldap_server, ADS_SASL_PLAIN);
	
	return 0;
}

/* connect.  Failure to connect results in an Exception */
static PyObject* py_ads_connect(ADS *self,
		PyObject *Py_UNUSED(ignored))
{
	ADS_STATUS status;
	TALLOC_CTX *frame = talloc_stackframe();
	if (!self->ads_ptr) {
		PyErr_SetString(PyExc_RuntimeError, "Uninitialized");
		return NULL;
	}
	SAFE_FREE(self->ads_ptr->auth.user_name);
	SAFE_FREE(self->ads_ptr->auth.password);
	SAFE_FREE(self->ads_ptr->auth.realm);
	if (self->cli_creds) {
		self->ads_ptr->auth.user_name =
			SMB_STRDUP(cli_credentials_get_username(self->cli_creds));
		self->ads_ptr->auth.password =
			SMB_STRDUP(cli_credentials_get_password(self->cli_creds));
		self->ads_ptr->auth.realm =
			SMB_STRDUP(cli_credentials_get_realm(self->cli_creds));
		self->ads_ptr->auth.flags |= ADS_AUTH_USER_CREDS;
		status = ads_connect_user_creds(self->ads_ptr);
	} else {
		char *passwd = NULL;
		int ret;
		if (!secrets_init()) {
			PyErr_SetString(PyExc_RuntimeError,
					"secrets_init() failed");
			goto err;
		}

		passwd = secrets_fetch_machine_password(self->ads_ptr->server.workgroup,
							NULL, NULL);
		if (passwd == NULL) {
			PyErr_SetString(PyExc_RuntimeError,
					"Failed to fetch the machine account "
					"password");
			goto err;
		}
		ret = asprintf(&(self->ads_ptr->auth.user_name), "%s$",
				   lp_netbios_name());
		if (ret == -1) {
			SAFE_FREE(passwd);
			PyErr_NoMemory();
			goto err;
		}
		self->ads_ptr->auth.password = passwd; /* take ownership of this data */
		self->ads_ptr->auth.realm =
			SMB_STRDUP(self->ads_ptr->server.realm);
		if (!strupper_m(self->ads_ptr->auth.realm)) {
			PyErr_SetString(PyExc_RuntimeError, "Failed to strupper");
			goto err;
		}
		self->ads_ptr->auth.flags |= ADS_AUTH_USER_CREDS;
		status = ads_connect(self->ads_ptr);
	}
	if (!ADS_ERR_OK(status)) {
		PyErr_Format(PyExc_RuntimeError,
				"ads_connect() failed: %s",
				ads_errstr(status));
		goto err;
	}

	TALLOC_FREE(frame);
	Py_RETURN_TRUE;

err:
	TALLOC_FREE(frame);
	return NULL;
}

/* Parameter mapping and functions for the GP_EXT struct */
void initgpo(void);

/* Global methods aka do not need a special pyobject type */
static PyObject *py_gpo_get_sysvol_gpt_version(PyObject * self,
					       PyObject * args)
{
	TALLOC_CTX *tmp_ctx = NULL;
	char *unix_path;
	char *display_name = NULL;
	uint32_t sysvol_version = 0;
	PyObject *result;
	NTSTATUS status;

	if (!PyArg_ParseTuple(args, "s", &unix_path)) {
		return NULL;
	}
	tmp_ctx = talloc_new(NULL);
	if (!tmp_ctx) {
		return PyErr_NoMemory();
	}
	status = gpo_get_sysvol_gpt_version(tmp_ctx, unix_path,
					    &sysvol_version,
					    &display_name);
	if (!NT_STATUS_IS_OK(status)) {
		PyErr_SetNTSTATUS(status);
		TALLOC_FREE(tmp_ctx);
		return NULL;
	}

	result = Py_BuildValue("[s,i]", display_name, sysvol_version);
	talloc_free(tmp_ctx);
	return result;
}

#ifdef HAVE_ADS
static ADS_STATUS find_samaccount(ADS_STRUCT *ads, TALLOC_CTX *mem_ctx,
				  const char *samaccountname,
				  uint32_t *uac_ret, const char **dn_ret)
{
	ADS_STATUS status;
	const char *attrs[] = { "userAccountControl", NULL };
	const char *filter;
	LDAPMessage *res = NULL;
	char *dn = NULL;
	uint32_t uac = 0;

	filter = talloc_asprintf(mem_ctx, "(sAMAccountName=%s)",
				 samaccountname);
	if (filter == NULL) {
		status = ADS_ERROR_NT(NT_STATUS_NO_MEMORY);
		goto out;
	}

	status = ads_do_search_all(ads, ads->config.bind_path,
				   LDAP_SCOPE_SUBTREE, filter, attrs, &res);

	if (!ADS_ERR_OK(status)) {
		goto out;
	}

	if (ads_count_replies(ads, res) != 1) {
		status = ADS_ERROR(LDAP_NO_RESULTS_RETURNED);
		goto out;
	}

	dn = ads_get_dn(ads, talloc_tos(), res);
	if (dn == NULL) {
		status = ADS_ERROR(LDAP_NO_MEMORY);
		goto out;
	}

	if (!ads_pull_uint32(ads, res, "userAccountControl", &uac)) {
		status = ADS_ERROR(LDAP_NO_SUCH_ATTRIBUTE);
		goto out;
	}

	if (uac_ret) {
		*uac_ret = uac;
	}

	if (dn_ret) {
		*dn_ret = talloc_strdup(mem_ctx, dn);
		if (*dn_ret == NULL) {
			status = ADS_ERROR(LDAP_NO_MEMORY);
			goto out;
		}
	}
out:
	TALLOC_FREE(dn);
	ads_msgfree(ads, res);

	return status;
}

static PyObject *py_ads_get_gpo_list(ADS *self, PyObject *args, PyObject *kwds)
{
	TALLOC_CTX *frame = NULL;
	struct GROUP_POLICY_OBJECT *gpo = NULL, *gpo_list = NULL;
	ADS_STATUS status;
	const char *samaccountname = NULL;
	const char *dn = NULL;
	uint32_t uac = 0;
	uint32_t flags = 0;
	struct security_token *token = NULL;
	PyObject *ret = NULL;
	TALLOC_CTX *gpo_ctx = NULL;
	size_t list_size;
	size_t i;

	static const char *kwlist[] = {"samaccountname", NULL};
	if (!PyArg_ParseTupleAndKeywords(args, kwds, "s",
					 discard_const_p(char *, kwlist),
					 &samaccountname)) {
		return NULL;
	}
	if (!self->ads_ptr) {
		PyErr_SetString(PyExc_RuntimeError, "Uninitialized");
		return NULL;
	}

	frame = talloc_stackframe();

	status = find_samaccount(self->ads_ptr, frame,
				 samaccountname, &uac, &dn);
	if (!ADS_ERR_OK(status)) {
		PyErr_Format(PyExc_RuntimeError,
				"Failed to find samAccountName '%s': %s",
				samaccountname, ads_errstr(status));
		goto out;
	}

	if (uac & UF_WORKSTATION_TRUST_ACCOUNT ||
	    uac & UF_SERVER_TRUST_ACCOUNT) {
		flags |= GPO_LIST_FLAG_MACHINE;
		status = gp_get_machine_token(self->ads_ptr, frame, dn,
					      &token);
		if (!ADS_ERR_OK(status)) {
			PyErr_Format(PyExc_RuntimeError,
				"Failed to get machine token for '%s'(%s): %s",
				samaccountname, dn, ads_errstr(status));
			goto out;
		}
	} else {
		status = ads_get_sid_token(self->ads_ptr, frame, dn, &token);
		if (!ADS_ERR_OK(status)) {
			PyErr_Format(PyExc_RuntimeError,
				"Failed to get sid token for '%s'(%s): %s",
				samaccountname, dn, ads_errstr(status));
			goto out;
		}
	}

	gpo_ctx = talloc_new(frame);
	if (!gpo_ctx) {
		PyErr_NoMemory();
		goto out;
	}
	status = ads_get_gpo_list(self->ads_ptr, gpo_ctx, dn, flags, token,
				  &gpo_list);
	if (!ADS_ERR_OK(status)) {
		PyErr_Format(PyExc_RuntimeError,
			"Failed to fetch GPO list: %s",
			ads_errstr(status));
		goto out;
	}

	/* Convert the C linked list into a python list */
	list_size = 0;
	for (gpo = gpo_list; gpo != NULL; gpo = gpo->next) {
		list_size++;
	}

	i = 0;
	ret = PyList_New(list_size);
	if (ret == NULL) {
		goto out;
	}

	for (gpo = gpo_list; gpo != NULL; gpo = gpo->next) {
		PyObject *obj = pytalloc_reference_ex(&GPOType,
						      gpo_ctx, gpo);
		if (obj == NULL) {
			Py_CLEAR(ret);
			goto out;
		}

		PyList_SetItem(ret, i, obj);
		i++;
	}

out:
	TALLOC_FREE(gpo_ctx);
	TALLOC_FREE(frame);
	return ret;
}

#endif

static PyMethodDef ADS_methods[] = {
	{ "connect", (PyCFunction)py_ads_connect, METH_NOARGS,
		"Connect to the LDAP server" },
#ifdef HAVE_ADS
	{ "get_gpo_list", PY_DISCARD_FUNC_SIG(PyCFunction, py_ads_get_gpo_list),
		METH_VARARGS | METH_KEYWORDS,
		NULL },
#endif
	{0}
};

static PyTypeObject ads_ADSType = {
	.tp_name = "gpo.ADS_STRUCT",
	.tp_basicsize = sizeof(ADS),
	.tp_dealloc = (destructor)py_ads_dealloc,
	.tp_flags = Py_TPFLAGS_DEFAULT,
	.tp_doc = "ADS struct",
	.tp_methods = ADS_methods,
	.tp_init = (initproc)py_ads_init,
};

static PyMethodDef py_gpo_methods[] = {
	{"gpo_get_sysvol_gpt_version",
		(PyCFunction)py_gpo_get_sysvol_gpt_version,
		METH_VARARGS, NULL},
	{0}
};

static struct PyModuleDef moduledef = {
	PyModuleDef_HEAD_INIT,
	.m_name = "gpo",
	.m_doc = "libgpo python bindings",
	.m_size = -1,
	.m_methods = py_gpo_methods,
};

/* Will be called by python when loading this module */
void initgpo(void);

MODULE_INIT_FUNC(gpo)
{
	PyObject *m;

	debug_setup_talloc_log();

	/* Instantiate the types */
	m = PyModule_Create(&moduledef);
	if (m == NULL) {
		goto err;
	}

	if (PyModule_AddObject(m, "version",
			   PyUnicode_FromString(SAMBA_VERSION_STRING)) ) {
		goto err;
	}

	ads_ADSType.tp_new = PyType_GenericNew;
	if (PyType_Ready(&ads_ADSType) < 0) {
		goto err;
	}

	Py_INCREF(&ads_ADSType);
	if (PyModule_AddObject(m, "ADS_STRUCT", (PyObject *)&ads_ADSType)) {
		goto err;
	}

	if (pytalloc_BaseObject_PyType_Ready(&GPOType) < 0) {
		goto err;
	}

	Py_INCREF((PyObject *)(void *)&GPOType);
	if (PyModule_AddObject(m, "GROUP_POLICY_OBJECT",
			   (PyObject *)&GPOType)) {
		goto err;
	}
	return m;

err:
	Py_CLEAR(m);
	return NULL;
}
