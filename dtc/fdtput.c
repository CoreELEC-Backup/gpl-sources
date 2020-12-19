// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (c) 2011 The Chromium OS Authors. All rights reserved.
 */

#include <assert.h>
#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <byteswap.h>

#include <libfdt.h>

#include "util.h"

/* These are the operations we support */
enum oper_type {
	OPER_WRITE_PROP,		/* Write a property in a node */
	OPER_CREATE_NODE,		/* Create a new node */
	OPER_REMOVE_NODE,		/* Delete a node */
	OPER_DELETE_PROP,		/* Delete a property in a node */
};

struct display_info {
	enum oper_type oper;	/* operation to perform */
	int type;		/* data type (s/i/u/x or 0 for default) */
	int size;		/* data size (1/2/4) */
	int verbose;		/* verbose output */
	int auto_path;		/* automatically create all path components */
	char *amlogic_dt_id;	/* force amlogic-dt-it for Amlogic multi dtb */
};


/**
 * Report an error with a particular node.
 *
 * @param name		Node name to report error on
 * @param namelen	Length of node name, or -1 to use entire string
 * @param err		Error number to report (-FDT_ERR_...)
 */
static void report_error(const char *name, int namelen, int err)
{
	if (namelen == -1)
		namelen = strlen(name);
	fprintf(stderr, "Error at '%1.*s': %s\n", namelen, name,
		fdt_strerror(err));
}

/**
 * Encode a series of arguments in a property value.
 *
 * @param disp		Display information / options
 * @param arg		List of arguments from command line
 * @param arg_count	Number of arguments (may be 0)
 * @param valuep	Returns buffer containing value
 * @param value_len	Returns length of value encoded
 */
static int encode_value(struct display_info *disp, char **arg, int arg_count,
			char **valuep, int *value_len)
{
	char *value = NULL;	/* holding area for value */
	int value_size = 0;	/* size of holding area */
	char *ptr;		/* pointer to current value position */
	int len;		/* length of this cell/string/byte */
	int ival;
	int upto;	/* the number of bytes we have written to buf */
	char fmt[3];

	upto = 0;

	if (disp->verbose)
		fprintf(stderr, "Decoding value:\n");

	fmt[0] = '%';
	fmt[1] = disp->type ? disp->type : 'd';
	fmt[2] = '\0';
	for (; arg_count > 0; arg++, arg_count--, upto += len) {
		/* assume integer unless told otherwise */
		if (disp->type == 's')
			len = strlen(*arg) + 1;
		else
			len = disp->size == -1 ? 4 : disp->size;

		/* enlarge our value buffer by a suitable margin if needed */
		if (upto + len > value_size) {
			value_size = (upto + len) + 500;
			value = xrealloc(value, value_size);
		}

		ptr = value + upto;
		if (disp->type == 's') {
			memcpy(ptr, *arg, len);
			if (disp->verbose)
				fprintf(stderr, "\tstring: '%s'\n", ptr);
		} else {
			fdt32_t *iptr = (fdt32_t *)ptr;
			sscanf(*arg, fmt, &ival);
			if (len == 4)
				*iptr = cpu_to_fdt32(ival);
			else
				*ptr = (uint8_t)ival;
			if (disp->verbose) {
				fprintf(stderr, "\t%s: %d\n",
					disp->size == 1 ? "byte" :
					disp->size == 2 ? "short" : "int",
					ival);
			}
		}
	}
	*value_len = upto;
	*valuep = value;
	if (disp->verbose)
		fprintf(stderr, "Value size %d\n", upto);
	return 0;
}

#define ALIGN(x)		(((x) + (FDT_TAGSIZE) - 1) & ~((FDT_TAGSIZE) - 1))

static char *realloc_fdt(char *fdt, int delta)
{
	int new_sz = fdt_totalsize(fdt) + delta;
	fdt = xrealloc(fdt, new_sz);
	fdt_open_into(fdt, fdt, new_sz);
	return fdt;
}

static char *realloc_node(char *fdt, const char *name)
{
	int delta;
	/* FDT_BEGIN_NODE, node name in off_struct and FDT_END_NODE */
	delta = sizeof(struct fdt_node_header) + ALIGN(strlen(name) + 1)
			+ FDT_TAGSIZE;
	return realloc_fdt(fdt, delta);
}

static char *realloc_property(char *fdt, int nodeoffset,
		const char *name, int newlen)
{
	int delta = 0;
	int oldlen = 0;

	if (!fdt_get_property(fdt, nodeoffset, name, &oldlen))
		/* strings + property header */
		delta = sizeof(struct fdt_property) + strlen(name) + 1;

	if (newlen > oldlen)
		/* actual value in off_struct */
		delta += ALIGN(newlen) - ALIGN(oldlen);

	return realloc_fdt(fdt, delta);
}

static int store_key_value(char **blob, const char *node_name,
		const char *property, const char *buf, int len)
{
	int node;
	int err;

	node = fdt_path_offset(*blob, node_name);
	if (node < 0) {
		report_error(node_name, -1, node);
		return -1;
	}

	err = fdt_setprop(*blob, node, property, buf, len);
	if (err == -FDT_ERR_NOSPACE) {
		*blob = realloc_property(*blob, node, property, len);
		err = fdt_setprop(*blob, node, property, buf, len);
	}
	if (err) {
		report_error(property, -1, err);
		return -1;
	}
	return 0;
}

/**
 * Create paths as needed for all components of a path
 *
 * Any components of the path that do not exist are created. Errors are
 * reported.
 *
 * @param blob		FDT blob to write into
 * @param in_path	Path to process
 * @return 0 if ok, -1 on error
 */
static int create_paths(char **blob, const char *in_path)
{
	const char *path = in_path;
	const char *sep;
	int node, offset = 0;

	/* skip leading '/' */
	while (*path == '/')
		path++;

	for (sep = path; *sep; path = sep + 1, offset = node) {
		/* equivalent to strchrnul(), but it requires _GNU_SOURCE */
		sep = strchr(path, '/');
		if (!sep)
			sep = path + strlen(path);

		node = fdt_subnode_offset_namelen(*blob, offset, path,
				sep - path);
		if (node == -FDT_ERR_NOTFOUND) {
			*blob = realloc_node(*blob, path);
			node = fdt_add_subnode_namelen(*blob, offset, path,
						       sep - path);
		}
		if (node < 0) {
			report_error(path, sep - path, node);
			return -1;
		}
	}

	return 0;
}

/**
 * Create a new node in the fdt.
 *
 * This will overwrite the node_name string. Any error is reported.
 *
 * TODO: Perhaps create fdt_path_offset_namelen() so we don't need to do this.
 *
 * @param blob		FDT blob to write into
 * @param node_name	Name of node to create
 * @return new node offset if found, or -1 on failure
 */
static int create_node(char **blob, const char *node_name)
{
	int node = 0;
	char *p;

	p = strrchr(node_name, '/');
	if (!p) {
		report_error(node_name, -1, -FDT_ERR_BADPATH);
		return -1;
	}
	*p = '\0';

	*blob = realloc_node(*blob, p + 1);

	if (p > node_name) {
		node = fdt_path_offset(*blob, node_name);
		if (node < 0) {
			report_error(node_name, -1, node);
			return -1;
		}
	}

	node = fdt_add_subnode(*blob, node, p + 1);
	if (node < 0) {
		report_error(p + 1, -1, node);
		return -1;
	}

	return 0;
}

/**
 * Delete a property of a node in the fdt.
 *
 * @param blob		FDT blob to write into
 * @param node_name	Path to node containing the property to delete
 * @param prop_name	Name of property to delete
 * @return 0 on success, or -1 on failure
 */
static int delete_prop(char *blob, const char *node_name, const char *prop_name)
{
	int node = 0;

	node = fdt_path_offset(blob, node_name);
	if (node < 0) {
		report_error(node_name, -1, node);
		return -1;
	}

	node = fdt_delprop(blob, node, prop_name);
	if (node < 0) {
		report_error(node_name, -1, node);
		return -1;
	}

	return 0;
}

/**
 * Delete a node in the fdt.
 *
 * @param blob		FDT blob to write into
 * @param node_name	Name of node to delete
 * @return 0 on success, or -1 on failure
 */
static int delete_node(char *blob, const char *node_name)
{
	int node = 0;

	node = fdt_path_offset(blob, node_name);
	if (node < 0) {
		report_error(node_name, -1, node);
		return -1;
	}

	node = fdt_del_node(blob, node);
	if (node < 0) {
		report_error(node_name, -1, node);
		return -1;
	}

	return 0;
}

static void padSpaces(uint8_t *s, int sz)
{
	--sz;
	while ( sz >= 0 && s[sz] == 0 )
	{
		s[sz] = 0x20;
		--sz;
	}
}

static void create_multidtb(char **blob, uint32_t dt_total, uint32_t page_size, char **fdts)
{
	uint32_t totalsize = page_size;
	char *multidtb = xmalloc(totalsize);
	uint32_t fdt_size = page_size;
	uint32_t version = AML_DT_VERSION;
	uint32_t offset = 0;
	int i, len;
	uint32_t blob_old_size;

	if (*(const uint32_t *)(*blob) == AML_DT_HEADER_MAGIC)
		blob_old_size = utilfdt_get_multidtb_size(*blob);
	else
		blob_old_size = fdt_totalsize(*blob);

	memset(multidtb, 0, totalsize);
	memcpy(multidtb + offset, AML_DT_MAGIC, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(multidtb + offset, &version, sizeof(uint32_t));
	offset += sizeof(uint32_t);
	memcpy(multidtb + offset, &dt_total, sizeof(uint32_t));
	offset += sizeof(uint32_t);


	for (i = 0; i < dt_total; i++) {
		int tmp_node;
		const struct fdt_property *dt_id = NULL;

		if (!fdts[i])
			continue;

		tmp_node = fdt_path_offset(fdts[i], "/");
		if (tmp_node >= 0)
			dt_id = fdt_getprop(fdts[i], tmp_node, "amlogic-dt-id", &len);

		if (dt_id) {
			uint8_t data[3][INFO_ENTRY_SIZE + 1] = { {0} };
			if (sscanf((const char*)dt_id, "%" INFO_ENTRY_SIZE_S "[^_]_%" INFO_ENTRY_SIZE_S "[^_]_%" INFO_ENTRY_SIZE_S "[^_\"]\"",
				data[0], data[1], data[2]) == 3) {
					int a;
					for (a = 0; a < 3; a++){
						int b;
						padSpaces(data[a], INFO_ENTRY_SIZE);
						for (b = 0; b < INFO_ENTRY_SIZE/sizeof(uint32_t); ++b) {
							uint32_t val = __bswap_32(*(uint32_t *)(&data[a][b * sizeof(uint32_t)]));
							memcpy(multidtb + offset, &val, sizeof(uint32_t));
							offset += sizeof(uint32_t);
						}
					}
			}
			else
				continue;

			memcpy(multidtb + offset, &totalsize, sizeof(uint32_t));
			offset += sizeof(uint32_t);
			fdt_size = fdt_totalsize(fdts[i]);
			fdt_size += page_size - (fdt_size % page_size);
			memcpy(multidtb + offset, &fdt_size, sizeof(uint32_t));
			offset += sizeof(uint32_t);

			totalsize += fdt_size;
			multidtb = xrealloc(multidtb, totalsize);
			memset(multidtb + (totalsize - fdt_size), 0, fdt_size);
			memcpy(multidtb + (totalsize - fdt_size), fdts[i], fdt_totalsize(fdts[i]));
		}
	}

	if (blob_old_size < totalsize)
		*blob = xrealloc(*blob, totalsize);

	memcpy(*blob, multidtb, totalsize);
	free(multidtb);
}

static int do_fdtput(struct display_info *disp, const char *filename,
		    char **arg, int arg_count)
{
	char *value = NULL;
	char *blob;
	char *node;
	int i, len, ret = 0;
	struct stat sb;
	uint32_t dt_total = 0;
	uint32_t page_size = 0;
	uint32_t fdt_addr = 0;
	int fdt_count = 0;
	char **fdts = NULL;
	char *fdt;
	char amlogic_dt_id[256] = {0};

	blob = utilfdt_read(filename, NULL);
	if (!blob)
		return -1;

	if (*(uint32_t *)blob == AML_DT_HEADER_MAGIC) {
		/* get correct dtb in Amlogic multi-dtb by amlogic-dt-id */
		if (disp->amlogic_dt_id || stat(amlogic_dt_id_path, &sb) == 0) {
			uint32_t tmp_fdt_addr = 0;
			uint32_t tmp_fdt_size = 0;
			uint32_t aml_dtb_offset_offset;
			uint32_t aml_dtb_header_size;

			if (utilfdt_get_amlogic_dt_id(disp->amlogic_dt_id, amlogic_dt_id) < 0) {
				free(blob);
				return -1;
			}

			utilfdt_get_multidtb_data(blob, &dt_total, &aml_dtb_header_size, &aml_dtb_offset_offset);

			if (disp->verbose)
				fprintf(stderr, "Using amlogic-dt-id: %s, total number of dtbs in multidtb: %d\n", amlogic_dt_id, dt_total);

			fdts = xmalloc(dt_total * sizeof(char *));
			memset(fdts, 0, dt_total * sizeof(char *));

			// check all dtbs in multidtb
			for (i = 0; i < dt_total; i++) {
				tmp_fdt_addr = *(uint32_t *)(blob + AML_DT_FIRST_DTB_OFFSET + i * aml_dtb_header_size + aml_dtb_offset_offset);
				tmp_fdt_size = fdt_totalsize(blob + tmp_fdt_addr);

				if (!page_size)
					page_size = tmp_fdt_addr;

				if (*(uint32_t *)(blob + tmp_fdt_addr) == DT_HEADER_MAGIC) {
					int tmp_node;
					const struct fdt_property *dt_id = NULL;
					tmp_node = fdt_path_offset(blob + tmp_fdt_addr, "/");
					if (tmp_node >= 0)
						dt_id = fdt_getprop(blob + tmp_fdt_addr, tmp_node, "amlogic-dt-id", &len);

					// check if correct dtb in multidtb got found
					if (dt_id &&
							(len >= strlen(amlogic_dt_id)) &&
							(!strncmp((const char *)dt_id, amlogic_dt_id, strlen(amlogic_dt_id)))) {
						fdt_addr = tmp_fdt_addr;
					}
					// make a backup and leave the dtb untouched
					else if (fdts) {
						fdts[fdt_count] = xmalloc(tmp_fdt_size);
						memcpy(fdts[fdt_count], blob + tmp_fdt_addr, tmp_fdt_size);
						fdt_count++;
					}
				}
			}

			if (fdt_addr == 0) {
				fprintf(stderr, "No matching amlogic-dt-id found for '%s': %d\n", amlogic_dt_id, -ENOENT);
				if (fdts) {
					for (i = 0; i < (dt_total - 1); i++)
						free(fdts[i]);
					free(fdts);
				}
				free(blob);
				return -1;
			}
		}
		else {
			fprintf(stderr, "Amlogic-dt-id needed for Amlogic multi-dtb: %d\n", -ENOENT);
			free(blob);
			return -1;
		}
	}

	// allocate new memory for currently used blob
	fdt = blob;
	uint32_t fdt_size = fdt_totalsize(blob + fdt_addr);
	blob = xmalloc(fdt_size);
	memcpy(blob, fdt + fdt_addr, fdt_size);

	switch (disp->oper) {
	case OPER_WRITE_PROP:
		/*
		 * Convert the arguments into a single binary value, then
		 * store them into the property.
		 */
		assert(arg_count >= 2);
		if (disp->auto_path && create_paths(&blob, *arg))
			return -1;
		if (encode_value(disp, arg + 2, arg_count - 2, &value, &len) ||
			store_key_value(&blob, *arg, arg[1], value, len))
			ret = -1;
		break;
	case OPER_CREATE_NODE:
		for (; ret >= 0 && arg_count--; arg++) {
			if (disp->auto_path)
				ret = create_paths(&blob, *arg);
			else
				ret = create_node(&blob, *arg);
		}
		break;
	case OPER_REMOVE_NODE:
		for (; ret >= 0 && arg_count--; arg++)
			ret = delete_node(blob, *arg);
		break;
	case OPER_DELETE_PROP:
		node = *arg;
		for (arg++; ret >= 0 && arg_count-- > 1; arg++)
			ret = delete_prop(blob, node, *arg);
		break;
	}
	if (ret >= 0) {
		fdt_pack(blob);

		// add single dtb to multidtb before save to file
		// free memory allocated before
		fdt_size = fdt_totalsize(blob);
		if (fdts) {
			fdts[fdt_count] = xmalloc(fdt_size);
			memcpy(fdts[fdt_count], blob, fdt_size);
			free(blob);
			blob = fdt;
			create_multidtb(&blob, dt_total, page_size, fdts);
		}
		else {
			memcpy(fdt, blob, fdt_size);
			free(blob);
			blob = fdt;
		}
		ret = utilfdt_write(filename, blob);
	}

	if (fdts) {
		for (i = 0; i < (dt_total - 1); i++)
			free(fdts[i]);
		free(fdts);
	}
	free(blob);

	if (value) {
		free(value);
	}

	return ret;
}

/* Usage related data. */
static const char usage_synopsis[] =
	"write a property value to a device tree\n"
	"	fdtput <options> <dt file> <node> <property> [<value>...]\n"
	"	fdtput -c <options> <dt file> [<node>...]\n"
	"	fdtput -r <options> <dt file> [<node>...]\n"
	"	fdtput -d <options> <dt file> <node> [<property>...]\n"
	"\n"
	"The command line arguments are joined together into a single value.\n"
	USAGE_TYPE_MSG;
static const char usage_short_opts[] = "crdpt:va:" USAGE_COMMON_SHORT_OPTS;
static struct option const usage_long_opts[] = {
	{"create",           no_argument, NULL, 'c'},
	{"remove",	     no_argument, NULL, 'r'},
	{"delete",	     no_argument, NULL, 'd'},
	{"auto-path",        no_argument, NULL, 'p'},
	{"type",              a_argument, NULL, 't'},
	{"verbose",          no_argument, NULL, 'v'},
	{"amlogic-dt-id",     a_argument, NULL, 'a'},
	USAGE_COMMON_LONG_OPTS,
};
static const char * const usage_opts_help[] = {
	"Create nodes if they don't already exist",
	"Delete nodes (and any subnodes) if they already exist",
	"Delete properties if they already exist",
	"Automatically create nodes as needed for the node path",
	"Type of data",
	"Display each value decoded from command line",
	"Forced amlogic-dt-id to be used for multi-dtb",
	USAGE_COMMON_OPTS_HELP
};

int main(int argc, char *argv[])
{
	int opt;
	struct display_info disp;
	char *filename = NULL;

	memset(&disp, '\0', sizeof(disp));
	disp.size = -1;
	disp.oper = OPER_WRITE_PROP;
	while ((opt = util_getopt_long()) != EOF) {
		/*
		 * TODO: add options to:
		 * - rename node
		 * - pack fdt before writing
		 * - set amount of free space when writing
		 */
		switch (opt) {
		case_USAGE_COMMON_FLAGS

		case 'a':
			disp.amlogic_dt_id = optarg;
			break;
		case 'c':
			disp.oper = OPER_CREATE_NODE;
			break;
		case 'r':
			disp.oper = OPER_REMOVE_NODE;
			break;
		case 'd':
			disp.oper = OPER_DELETE_PROP;
			break;
		case 'p':
			disp.auto_path = 1;
			break;
		case 't':
			if (utilfdt_decode_type(optarg, &disp.type,
					&disp.size))
				usage("Invalid type string");
			break;

		case 'v':
			disp.verbose = 1;
			break;
		}
	}

	if (optind < argc)
		filename = argv[optind++];
	if (!filename)
		usage("missing filename");

	argv += optind;
	argc -= optind;

	if (disp.oper == OPER_WRITE_PROP) {
		if (argc < 1)
			usage("missing node");
		if (argc < 2)
			usage("missing property");
	}

	if (disp.oper == OPER_DELETE_PROP)
		if (argc < 1)
			usage("missing node");

	if (do_fdtput(&disp, filename, argv, argc))
		return 1;
	return 0;
}
