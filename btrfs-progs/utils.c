/*
 * Copyright (C) 2007 Oracle.  All rights reserved.
 * Copyright (C) 2008 Morey Roof.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License v2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 021110-1307, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <uuid/uuid.h>
#include <fcntl.h>
#include <unistd.h>
#include <mntent.h>
#include <ctype.h>
#include <linux/loop.h>
#include <linux/major.h>
#include <linux/kdev_t.h>
#include <limits.h>
#include <blkid/blkid.h>
#include <sys/vfs.h>
#include <sys/statfs.h>
#include <linux/magic.h>
#include <getopt.h>
#include <sys/utsname.h>
#include <linux/version.h>

#include "kerncompat.h"
#include "radix-tree.h"
#include "ctree.h"
#include "disk-io.h"
#include "transaction.h"
#include "crc32c.h"
#include "utils.h"
#include "volumes.h"
#include "ioctl.h"
#include "commands.h"

#ifndef BLKDISCARD
#define BLKDISCARD	_IO(0x12,119)
#endif

static int btrfs_scan_done = 0;

static char argv0_buf[ARGV0_BUF_SIZE] = "btrfs";

static int rand_seed_initlized = 0;
static unsigned short rand_seed[3];

const char *get_argv0_buf(void)
{
	return argv0_buf;
}

void fixup_argv0(char **argv, const char *token)
{
	int len = strlen(argv0_buf);

	snprintf(argv0_buf + len, sizeof(argv0_buf) - len, " %s", token);
	argv[0] = argv0_buf;
}

void set_argv0(char **argv)
{
	strncpy(argv0_buf, argv[0], sizeof(argv0_buf));
	argv0_buf[sizeof(argv0_buf) - 1] = 0;
}

int check_argc_exact(int nargs, int expected)
{
	if (nargs < expected)
		fprintf(stderr, "%s: too few arguments\n", argv0_buf);
	if (nargs > expected)
		fprintf(stderr, "%s: too many arguments\n", argv0_buf);

	return nargs != expected;
}

int check_argc_min(int nargs, int expected)
{
	if (nargs < expected) {
		fprintf(stderr, "%s: too few arguments\n", argv0_buf);
		return 1;
	}

	return 0;
}

int check_argc_max(int nargs, int expected)
{
	if (nargs > expected) {
		fprintf(stderr, "%s: too many arguments\n", argv0_buf);
		return 1;
	}

	return 0;
}


/*
 * Discard the given range in one go
 */
static int discard_range(int fd, u64 start, u64 len)
{
	u64 range[2] = { start, len };

	if (ioctl(fd, BLKDISCARD, &range) < 0)
		return errno;
	return 0;
}

/*
 * Discard blocks in the given range in 1G chunks, the process is interruptible
 */
static int discard_blocks(int fd, u64 start, u64 len)
{
	while (len > 0) {
		/* 1G granularity */
		u64 chunk_size = min_t(u64, len, 1*1024*1024*1024);
		int ret;

		ret = discard_range(fd, start, chunk_size);
		if (ret)
			return ret;
		len -= chunk_size;
		start += chunk_size;
	}

	return 0;
}

static u64 reference_root_table[] = {
	[1] =	BTRFS_ROOT_TREE_OBJECTID,
	[2] =	BTRFS_EXTENT_TREE_OBJECTID,
	[3] =	BTRFS_CHUNK_TREE_OBJECTID,
	[4] =	BTRFS_DEV_TREE_OBJECTID,
	[5] =	BTRFS_FS_TREE_OBJECTID,
	[6] =	BTRFS_CSUM_TREE_OBJECTID,
};

int test_uuid_unique(char *fs_uuid)
{
	int unique = 1;
	blkid_dev_iterate iter = NULL;
	blkid_dev dev = NULL;
	blkid_cache cache = NULL;

	if (blkid_get_cache(&cache, NULL) < 0) {
		printf("ERROR: lblkid cache get failed\n");
		return 1;
	}
	blkid_probe_all(cache);
	iter = blkid_dev_iterate_begin(cache);
	blkid_dev_set_search(iter, "UUID", fs_uuid);

	while (blkid_dev_next(iter, &dev) == 0) {
		dev = blkid_verify(cache, dev);
		if (dev) {
			unique = 0;
			break;
		}
	}

	blkid_dev_iterate_end(iter);
	blkid_put_cache(cache);

	return unique;
}

/*
 * Reserve space from free_tree.
 * The algorithm is very simple, find the first cache_extent with enough space
 * and allocate from its beginning.
 */
static int reserve_free_space(struct cache_tree *free_tree, u64 len,
			      u64 *ret_start)
{
	struct cache_extent *cache;
	int found = 0;

	ASSERT(ret_start != NULL);
	cache = first_cache_extent(free_tree);
	while (cache) {
		if (cache->size > len) {
			found = 1;
			*ret_start = cache->start;

			cache->size -= len;
			if (cache->size == 0) {
				remove_cache_extent(free_tree, cache);
				free(cache);
			} else {
				cache->start += len;
			}
			break;
		}
		cache = next_cache_extent(cache);
	}
	if (!found)
		return -ENOSPC;
	return 0;
}

static inline int write_temp_super(int fd, struct btrfs_super_block *sb,
				   u64 sb_bytenr)
{
	u32 crc = ~(u32)0;
	int ret;

	crc = btrfs_csum_data(NULL, (char *)sb + BTRFS_CSUM_SIZE, crc,
			      BTRFS_SUPER_INFO_SIZE - BTRFS_CSUM_SIZE);
	btrfs_csum_final(crc, &sb->csum[0]);
	ret = pwrite(fd, sb, BTRFS_SUPER_INFO_SIZE, sb_bytenr);
	if (ret < BTRFS_SUPER_INFO_SIZE)
		ret = (ret < 0 ? -errno : -EIO);
	else
		ret = 0;
	return ret;
}

/*
 * Setup temporary superblock at cfg->super_bynter
 * Needed info are extracted from cfg, and root_bytenr, chunk_bytenr
 *
 * For now sys chunk array will be empty and dev_item is empty too.
 * They will be re-initialized at temp chunk tree setup.
 *
 * The superblock signature is not valid, denotes a partially created
 * filesystem, needs to be finalized.
 */
static int setup_temp_super(int fd, struct btrfs_mkfs_config *cfg,
			    u64 root_bytenr, u64 chunk_bytenr)
{
	unsigned char chunk_uuid[BTRFS_UUID_SIZE];
	char super_buf[BTRFS_SUPER_INFO_SIZE];
	struct btrfs_super_block *super = (struct btrfs_super_block *)super_buf;
	int ret;

	memset(super_buf, 0, BTRFS_SUPER_INFO_SIZE);
	cfg->num_bytes = round_down(cfg->num_bytes, cfg->sectorsize);

	if (*cfg->fs_uuid) {
		if (uuid_parse(cfg->fs_uuid, super->fsid) != 0) {
			error("cound not parse UUID: %s", cfg->fs_uuid);
			ret = -EINVAL;
			goto out;
		}
		if (!test_uuid_unique(cfg->fs_uuid)) {
			error("non-unique UUID: %s", cfg->fs_uuid);
			ret = -EINVAL;
			goto out;
		}
	} else {
		uuid_generate(super->fsid);
		uuid_unparse(super->fsid, cfg->fs_uuid);
	}
	uuid_generate(chunk_uuid);
	uuid_unparse(chunk_uuid, cfg->chunk_uuid);

	btrfs_set_super_bytenr(super, cfg->super_bytenr);
	btrfs_set_super_num_devices(super, 1);
	btrfs_set_super_magic(super, BTRFS_MAGIC_PARTIAL);
	btrfs_set_super_generation(super, 1);
	btrfs_set_super_root(super, root_bytenr);
	btrfs_set_super_chunk_root(super, chunk_bytenr);
	btrfs_set_super_total_bytes(super, cfg->num_bytes);
	/*
	 * Temporary filesystem will only have 6 tree roots:
	 * chunk tree, root tree, extent_tree, device tree, fs tree
	 * and csum tree.
	 */
	btrfs_set_super_bytes_used(super, 6 * cfg->nodesize);
	btrfs_set_super_sectorsize(super, cfg->sectorsize);
	btrfs_set_super_leafsize(super, cfg->nodesize);
	btrfs_set_super_nodesize(super, cfg->nodesize);
	btrfs_set_super_stripesize(super, cfg->stripesize);
	btrfs_set_super_csum_type(super, BTRFS_CSUM_TYPE_CRC32);
	btrfs_set_super_chunk_root(super, chunk_bytenr);
	btrfs_set_super_cache_generation(super, -1);
	btrfs_set_super_incompat_flags(super, cfg->features);
	if (cfg->label)
		__strncpy_null(super->label, cfg->label, BTRFS_LABEL_SIZE - 1);

	/* Sys chunk array will be re-initialized at chunk tree init time */
	super->sys_chunk_array_size = 0;

	ret = write_temp_super(fd, super, cfg->super_bytenr);
out:
	return ret;
}

/*
 * Setup an extent buffer for tree block.
 */
static int setup_temp_extent_buffer(struct extent_buffer *buf,
				    struct btrfs_mkfs_config *cfg,
				    u64 bytenr, u64 owner)
{
	unsigned char fsid[BTRFS_FSID_SIZE];
	unsigned char chunk_uuid[BTRFS_UUID_SIZE];
	int ret;

	ret = uuid_parse(cfg->fs_uuid, fsid);
	if (ret)
		return -EINVAL;
	ret = uuid_parse(cfg->chunk_uuid, chunk_uuid);
	if (ret)
		return -EINVAL;

	memset(buf->data, 0, cfg->nodesize);
	buf->len = cfg->nodesize;
	btrfs_set_header_bytenr(buf, bytenr);
	btrfs_set_header_generation(buf, 1);
	btrfs_set_header_backref_rev(buf, BTRFS_MIXED_BACKREF_REV);
	btrfs_set_header_owner(buf, owner);
	btrfs_set_header_flags(buf, BTRFS_HEADER_FLAG_WRITTEN);
	write_extent_buffer(buf, chunk_uuid, btrfs_header_chunk_tree_uuid(buf),
			    BTRFS_UUID_SIZE);
	write_extent_buffer(buf, fsid, btrfs_header_fsid(), BTRFS_FSID_SIZE);
	return 0;
}

static inline int write_temp_extent_buffer(int fd, struct extent_buffer *buf,
					   u64 bytenr)
{
	int ret;

	csum_tree_block_size(buf, BTRFS_CRC32_SIZE, 0);

	/* Temporary extent buffer is always mapped 1:1 on disk */
	ret = pwrite(fd, buf->data, buf->len, bytenr);
	if (ret < buf->len)
		ret = (ret < 0 ? ret : -EIO);
	else
		ret = 0;
	return ret;
}

/*
 * Insert a root item for temporary tree root
 *
 * Only used in make_btrfs_v2().
 */
static void insert_temp_root_item(struct extent_buffer *buf,
				  struct btrfs_mkfs_config *cfg,
				  int *slot, u32 *itemoff, u64 objectid,
				  u64 bytenr)
{
	struct btrfs_root_item root_item;
	struct btrfs_inode_item *inode_item;
	struct btrfs_disk_key disk_key;

	btrfs_set_header_nritems(buf, *slot + 1);
	(*itemoff) -= sizeof(root_item);
	memset(&root_item, 0, sizeof(root_item));
	inode_item = &root_item.inode;
	btrfs_set_stack_inode_generation(inode_item, 1);
	btrfs_set_stack_inode_size(inode_item, 3);
	btrfs_set_stack_inode_nlink(inode_item, 1);
	btrfs_set_stack_inode_nbytes(inode_item, cfg->nodesize);
	btrfs_set_stack_inode_mode(inode_item, S_IFDIR | 0755);
	btrfs_set_root_refs(&root_item, 1);
	btrfs_set_root_used(&root_item, cfg->nodesize);
	btrfs_set_root_generation(&root_item, 1);
	btrfs_set_root_bytenr(&root_item, bytenr);

	memset(&disk_key, 0, sizeof(disk_key));
	btrfs_set_disk_key_type(&disk_key, BTRFS_ROOT_ITEM_KEY);
	btrfs_set_disk_key_objectid(&disk_key, objectid);
	btrfs_set_disk_key_offset(&disk_key, 0);

	btrfs_set_item_key(buf, &disk_key, *slot);
	btrfs_set_item_offset(buf, btrfs_item_nr(*slot), *itemoff);
	btrfs_set_item_size(buf, btrfs_item_nr(*slot), sizeof(root_item));
	write_extent_buffer(buf, &root_item,
			    btrfs_item_ptr_offset(buf, *slot),
			    sizeof(root_item));
	(*slot)++;
}

static int setup_temp_root_tree(int fd, struct btrfs_mkfs_config *cfg,
				u64 root_bytenr, u64 extent_bytenr,
				u64 dev_bytenr, u64 fs_bytenr, u64 csum_bytenr)
{
	struct extent_buffer *buf = NULL;
	u32 itemoff = __BTRFS_LEAF_DATA_SIZE(cfg->nodesize);
	int slot = 0;
	int ret;

	/*
	 * Provided bytenr must in ascending order, or tree root will have a
	 * bad key order.
	 */
	if (!(root_bytenr < extent_bytenr && extent_bytenr < dev_bytenr &&
	      dev_bytenr < fs_bytenr && fs_bytenr < csum_bytenr)) {
		error("bad tree bytenr order: "
				"root < extent %llu < %llu, "
				"extent < dev %llu < %llu, "
				"dev < fs %llu < %llu, "
				"fs < csum %llu < %llu",
				(unsigned long long)root_bytenr,
				(unsigned long long)extent_bytenr,
				(unsigned long long)extent_bytenr,
				(unsigned long long)dev_bytenr,
				(unsigned long long)dev_bytenr,
				(unsigned long long)fs_bytenr,
				(unsigned long long)fs_bytenr,
				(unsigned long long)csum_bytenr);
		return -EINVAL;
	}
	buf = malloc(sizeof(*buf) + cfg->nodesize);
	if (!buf)
		return -ENOMEM;

	ret = setup_temp_extent_buffer(buf, cfg, root_bytenr,
				       BTRFS_ROOT_TREE_OBJECTID);
	if (ret < 0)
		goto out;

	insert_temp_root_item(buf, cfg, &slot, &itemoff,
			      BTRFS_EXTENT_TREE_OBJECTID, extent_bytenr);
	insert_temp_root_item(buf, cfg, &slot, &itemoff,
			      BTRFS_DEV_TREE_OBJECTID, dev_bytenr);
	insert_temp_root_item(buf, cfg, &slot, &itemoff,
			      BTRFS_FS_TREE_OBJECTID, fs_bytenr);
	insert_temp_root_item(buf, cfg, &slot, &itemoff,
			      BTRFS_CSUM_TREE_OBJECTID, csum_bytenr);

	ret = write_temp_extent_buffer(fd, buf, root_bytenr);
out:
	free(buf);
	return ret;
}

static int insert_temp_dev_item(int fd, struct extent_buffer *buf,
				struct btrfs_mkfs_config *cfg,
				int *slot, u32 *itemoff)
{
	struct btrfs_disk_key disk_key;
	struct btrfs_dev_item *dev_item;
	char super_buf[BTRFS_SUPER_INFO_SIZE];
	unsigned char dev_uuid[BTRFS_UUID_SIZE];
	unsigned char fsid[BTRFS_FSID_SIZE];
	struct btrfs_super_block *super = (struct btrfs_super_block *)super_buf;
	int ret;

	ret = pread(fd, super_buf, BTRFS_SUPER_INFO_SIZE, cfg->super_bytenr);
	if (ret < BTRFS_SUPER_INFO_SIZE) {
		ret = (ret < 0 ? -errno : -EIO);
		goto out;
	}

	btrfs_set_header_nritems(buf, *slot + 1);
	(*itemoff) -= sizeof(*dev_item);
	/* setup device item 1, 0 is for replace case */
	btrfs_set_disk_key_type(&disk_key, BTRFS_DEV_ITEM_KEY);
	btrfs_set_disk_key_objectid(&disk_key, BTRFS_DEV_ITEMS_OBJECTID);
	btrfs_set_disk_key_offset(&disk_key, 1);
	btrfs_set_item_key(buf, &disk_key, *slot);
	btrfs_set_item_offset(buf, btrfs_item_nr(*slot), *itemoff);
	btrfs_set_item_size(buf, btrfs_item_nr(*slot), sizeof(*dev_item));

	dev_item = btrfs_item_ptr(buf, *slot, struct btrfs_dev_item);
	/* Generate device uuid */
	uuid_generate(dev_uuid);
	write_extent_buffer(buf, dev_uuid,
			(unsigned long)btrfs_device_uuid(dev_item),
			BTRFS_UUID_SIZE);
	uuid_parse(cfg->fs_uuid, fsid);
	write_extent_buffer(buf, fsid,
			(unsigned long)btrfs_device_fsid(dev_item),
			BTRFS_FSID_SIZE);
	btrfs_set_device_id(buf, dev_item, 1);
	btrfs_set_device_generation(buf, dev_item, 0);
	btrfs_set_device_total_bytes(buf, dev_item, cfg->num_bytes);
	/*
	 * The number must match the initial SYSTEM and META chunk size
	 */
	btrfs_set_device_bytes_used(buf, dev_item,
			BTRFS_MKFS_SYSTEM_GROUP_SIZE +
			BTRFS_CONVERT_META_GROUP_SIZE);
	btrfs_set_device_io_align(buf, dev_item, cfg->sectorsize);
	btrfs_set_device_io_width(buf, dev_item, cfg->sectorsize);
	btrfs_set_device_sector_size(buf, dev_item, cfg->sectorsize);
	btrfs_set_device_type(buf, dev_item, 0);

	/* Super dev_item is not complete, copy the complete one to sb */
	read_extent_buffer(buf, &super->dev_item, (unsigned long)dev_item,
			   sizeof(*dev_item));
	ret = write_temp_super(fd, super, cfg->super_bytenr);
	(*slot)++;
out:
	return ret;
}

static int insert_temp_chunk_item(int fd, struct extent_buffer *buf,
				  struct btrfs_mkfs_config *cfg,
				  int *slot, u32 *itemoff, u64 start, u64 len,
				  u64 type)
{
	struct btrfs_chunk *chunk;
	struct btrfs_disk_key disk_key;
	char super_buf[BTRFS_SUPER_INFO_SIZE];
	struct btrfs_super_block *sb = (struct btrfs_super_block *)super_buf;
	int ret = 0;

	ret = pread(fd, super_buf, BTRFS_SUPER_INFO_SIZE,
		    cfg->super_bytenr);
	if (ret < BTRFS_SUPER_INFO_SIZE) {
		ret = (ret < 0 ? ret : -EIO);
		return ret;
	}

	btrfs_set_header_nritems(buf, *slot + 1);
	(*itemoff) -= btrfs_chunk_item_size(1);
	btrfs_set_disk_key_type(&disk_key, BTRFS_CHUNK_ITEM_KEY);
	btrfs_set_disk_key_objectid(&disk_key, BTRFS_FIRST_CHUNK_TREE_OBJECTID);
	btrfs_set_disk_key_offset(&disk_key, start);
	btrfs_set_item_key(buf, &disk_key, *slot);
	btrfs_set_item_offset(buf, btrfs_item_nr(*slot), *itemoff);
	btrfs_set_item_size(buf, btrfs_item_nr(*slot),
			    btrfs_chunk_item_size(1));

	chunk = btrfs_item_ptr(buf, *slot, struct btrfs_chunk);
	btrfs_set_chunk_length(buf, chunk, len);
	btrfs_set_chunk_owner(buf, chunk, BTRFS_EXTENT_TREE_OBJECTID);
	btrfs_set_chunk_stripe_len(buf, chunk, 64 * 1024);
	btrfs_set_chunk_type(buf, chunk, type);
	btrfs_set_chunk_io_align(buf, chunk, cfg->sectorsize);
	btrfs_set_chunk_io_width(buf, chunk, cfg->sectorsize);
	btrfs_set_chunk_sector_size(buf, chunk, cfg->sectorsize);
	btrfs_set_chunk_num_stripes(buf, chunk, 1);
	/* TODO: Support DUP profile for system chunk */
	btrfs_set_stripe_devid_nr(buf, chunk, 0, 1);
	/* We are doing 1:1 mapping, so start is its dev offset */
	btrfs_set_stripe_offset_nr(buf, chunk, 0, start);
	write_extent_buffer(buf, &sb->dev_item.uuid,
			    (unsigned long)btrfs_stripe_dev_uuid_nr(chunk, 0),
			    BTRFS_UUID_SIZE);
	(*slot)++;

	/*
	 * If it's system chunk, also copy it to super block.
	 */
	if (type & BTRFS_BLOCK_GROUP_SYSTEM) {
		char *cur;

		cur = (char *)sb->sys_chunk_array + sb->sys_chunk_array_size;
		memcpy(cur, &disk_key, sizeof(disk_key));
		cur += sizeof(disk_key);
		read_extent_buffer(buf, cur, (unsigned long int)chunk,
				   btrfs_chunk_item_size(1));
		sb->sys_chunk_array_size += btrfs_chunk_item_size(1) +
					    sizeof(disk_key);

		ret = write_temp_super(fd, sb, cfg->super_bytenr);
	}
	return ret;
}

static int setup_temp_chunk_tree(int fd, struct btrfs_mkfs_config *cfg,
				 u64 sys_chunk_start, u64 meta_chunk_start,
				 u64 chunk_bytenr)
{
	struct extent_buffer *buf = NULL;
	u32 itemoff = __BTRFS_LEAF_DATA_SIZE(cfg->nodesize);
	int slot = 0;
	int ret;

	/* Must ensure SYS chunk starts before META chunk */
	if (meta_chunk_start < sys_chunk_start) {
		error("wrong chunk order: meta < system %llu < %llu",
				(unsigned long long)meta_chunk_start,
				(unsigned long long)sys_chunk_start);
		return -EINVAL;
	}
	buf = malloc(sizeof(*buf) + cfg->nodesize);
	if (!buf)
		return -ENOMEM;
	ret = setup_temp_extent_buffer(buf, cfg, chunk_bytenr,
				       BTRFS_CHUNK_TREE_OBJECTID);
	if (ret < 0)
		goto out;

	ret = insert_temp_dev_item(fd, buf, cfg, &slot, &itemoff);
	if (ret < 0)
		goto out;
	ret = insert_temp_chunk_item(fd, buf, cfg, &slot, &itemoff,
				     sys_chunk_start,
				     BTRFS_MKFS_SYSTEM_GROUP_SIZE,
				     BTRFS_BLOCK_GROUP_SYSTEM);
	if (ret < 0)
		goto out;
	ret = insert_temp_chunk_item(fd, buf, cfg, &slot, &itemoff,
				     meta_chunk_start,
				     BTRFS_CONVERT_META_GROUP_SIZE,
				     BTRFS_BLOCK_GROUP_METADATA);
	if (ret < 0)
		goto out;
	ret = write_temp_extent_buffer(fd, buf, chunk_bytenr);

out:
	free(buf);
	return ret;
}

static void insert_temp_dev_extent(struct extent_buffer *buf,
				   int *slot, u32 *itemoff, u64 start, u64 len)
{
	struct btrfs_dev_extent *dev_extent;
	struct btrfs_disk_key disk_key;

	btrfs_set_header_nritems(buf, *slot + 1);
	(*itemoff) -= sizeof(*dev_extent);
	btrfs_set_disk_key_type(&disk_key, BTRFS_DEV_EXTENT_KEY);
	btrfs_set_disk_key_objectid(&disk_key, 1);
	btrfs_set_disk_key_offset(&disk_key, start);
	btrfs_set_item_key(buf, &disk_key, *slot);
	btrfs_set_item_offset(buf, btrfs_item_nr(*slot), *itemoff);
	btrfs_set_item_size(buf, btrfs_item_nr(*slot), sizeof(*dev_extent));

	dev_extent = btrfs_item_ptr(buf, *slot, struct btrfs_dev_extent);
	btrfs_set_dev_extent_chunk_objectid(buf, dev_extent,
					    BTRFS_FIRST_CHUNK_TREE_OBJECTID);
	btrfs_set_dev_extent_length(buf, dev_extent, len);
	btrfs_set_dev_extent_chunk_offset(buf, dev_extent, start);
	btrfs_set_dev_extent_chunk_tree(buf, dev_extent,
					BTRFS_CHUNK_TREE_OBJECTID);
	(*slot)++;
}

static int setup_temp_dev_tree(int fd, struct btrfs_mkfs_config *cfg,
			       u64 sys_chunk_start, u64 meta_chunk_start,
			       u64 dev_bytenr)
{
	struct extent_buffer *buf = NULL;
	u32 itemoff = __BTRFS_LEAF_DATA_SIZE(cfg->nodesize);
	int slot = 0;
	int ret;

	/* Must ensure SYS chunk starts before META chunk */
	if (meta_chunk_start < sys_chunk_start) {
		error("wrong chunk order: meta < system %llu < %llu",
				(unsigned long long)meta_chunk_start,
				(unsigned long long)sys_chunk_start);
		return -EINVAL;
	}
	buf = malloc(sizeof(*buf) + cfg->nodesize);
	if (!buf)
		return -ENOMEM;
	ret = setup_temp_extent_buffer(buf, cfg, dev_bytenr,
				       BTRFS_DEV_TREE_OBJECTID);
	if (ret < 0)
		goto out;
	insert_temp_dev_extent(buf, &slot, &itemoff, sys_chunk_start,
			       BTRFS_MKFS_SYSTEM_GROUP_SIZE);
	insert_temp_dev_extent(buf, &slot, &itemoff, meta_chunk_start,
			       BTRFS_CONVERT_META_GROUP_SIZE);
	ret = write_temp_extent_buffer(fd, buf, dev_bytenr);
out:
	free(buf);
	return ret;
}

static int setup_temp_fs_tree(int fd, struct btrfs_mkfs_config *cfg,
			      u64 fs_bytenr)
{
	struct extent_buffer *buf = NULL;
	int ret;

	buf = malloc(sizeof(*buf) + cfg->nodesize);
	if (!buf)
		return -ENOMEM;
	ret = setup_temp_extent_buffer(buf, cfg, fs_bytenr,
				       BTRFS_FS_TREE_OBJECTID);
	if (ret < 0)
		goto out;
	/*
	 * Temporary fs tree is completely empty.
	 */
	ret = write_temp_extent_buffer(fd, buf, fs_bytenr);
out:
	free(buf);
	return ret;
}

static int setup_temp_csum_tree(int fd, struct btrfs_mkfs_config *cfg,
				u64 csum_bytenr)
{
	struct extent_buffer *buf = NULL;
	int ret;

	buf = malloc(sizeof(*buf) + cfg->nodesize);
	if (!buf)
		return -ENOMEM;
	ret = setup_temp_extent_buffer(buf, cfg, csum_bytenr,
				       BTRFS_CSUM_TREE_OBJECTID);
	if (ret < 0)
		goto out;
	/*
	 * Temporary csum tree is completely empty.
	 */
	ret = write_temp_extent_buffer(fd, buf, csum_bytenr);
out:
	free(buf);
	return ret;
}

/*
 * Insert one temporary extent item.
 *
 * NOTE: if skinny_metadata is not enabled, this function must be called
 * after all other trees are initialized.
 * Or fs without skinny-metadata will be screwed up.
 */
static int insert_temp_extent_item(int fd, struct extent_buffer *buf,
				   struct btrfs_mkfs_config *cfg,
				   int *slot, u32 *itemoff, u64 bytenr,
				   u64 ref_root)
{
	struct extent_buffer *tmp;
	struct btrfs_extent_item *ei;
	struct btrfs_extent_inline_ref *iref;
	struct btrfs_disk_key disk_key;
	struct btrfs_disk_key tree_info_key;
	struct btrfs_tree_block_info *info;
	int itemsize;
	int skinny_metadata = cfg->features &
			      BTRFS_FEATURE_INCOMPAT_SKINNY_METADATA;
	int ret;

	if (skinny_metadata)
		itemsize = sizeof(*ei) + sizeof(*iref);
	else
		itemsize = sizeof(*ei) + sizeof(*iref) +
			   sizeof(struct btrfs_tree_block_info);

	btrfs_set_header_nritems(buf, *slot + 1);
	*(itemoff) -= itemsize;

	if (skinny_metadata) {
		btrfs_set_disk_key_type(&disk_key, BTRFS_METADATA_ITEM_KEY);
		btrfs_set_disk_key_offset(&disk_key, 0);
	} else {
		btrfs_set_disk_key_type(&disk_key, BTRFS_EXTENT_ITEM_KEY);
		btrfs_set_disk_key_offset(&disk_key, cfg->nodesize);
	}
	btrfs_set_disk_key_objectid(&disk_key, bytenr);

	btrfs_set_item_key(buf, &disk_key, *slot);
	btrfs_set_item_offset(buf, btrfs_item_nr(*slot), *itemoff);
	btrfs_set_item_size(buf, btrfs_item_nr(*slot), itemsize);

	ei = btrfs_item_ptr(buf, *slot, struct btrfs_extent_item);
	btrfs_set_extent_refs(buf, ei, 1);
	btrfs_set_extent_generation(buf, ei, 1);
	btrfs_set_extent_flags(buf, ei, BTRFS_EXTENT_FLAG_TREE_BLOCK);

	if (skinny_metadata) {
		iref = (struct btrfs_extent_inline_ref *)(ei + 1);
	} else {
		info = (struct btrfs_tree_block_info *)(ei + 1);
		iref = (struct btrfs_extent_inline_ref *)(info + 1);
	}
	btrfs_set_extent_inline_ref_type(buf, iref,
					 BTRFS_TREE_BLOCK_REF_KEY);
	btrfs_set_extent_inline_ref_offset(buf, iref, ref_root);

	(*slot)++;
	if (skinny_metadata)
		return 0;

	/*
	 * Lastly, check the tree block key by read the tree block
	 * Since we do 1:1 mapping for convert case, we can directly
	 * read the bytenr from disk
	 */
	tmp = malloc(sizeof(*tmp) + cfg->nodesize);
	if (!tmp)
		return -ENOMEM;
	ret = setup_temp_extent_buffer(tmp, cfg, bytenr, ref_root);
	if (ret < 0)
		goto out;
	ret = pread(fd, tmp->data, cfg->nodesize, bytenr);
	if (ret < cfg->nodesize) {
		ret = (ret < 0 ? -errno : -EIO);
		goto out;
	}
	if (btrfs_header_nritems(tmp) == 0) {
		btrfs_set_disk_key_type(&tree_info_key, 0);
		btrfs_set_disk_key_objectid(&tree_info_key, 0);
		btrfs_set_disk_key_offset(&tree_info_key, 0);
	} else {
		btrfs_item_key(tmp, &tree_info_key, 0);
	}
	btrfs_set_tree_block_key(buf, info, &tree_info_key);

out:
	free(tmp);
	return ret;
}

static void insert_temp_block_group(struct extent_buffer *buf,
				   struct btrfs_mkfs_config *cfg,
				   int *slot, u32 *itemoff,
				   u64 bytenr, u64 len, u64 used, u64 flag)
{
	struct btrfs_block_group_item bgi;
	struct btrfs_disk_key disk_key;

	btrfs_set_header_nritems(buf, *slot + 1);
	(*itemoff) -= sizeof(bgi);
	btrfs_set_disk_key_type(&disk_key, BTRFS_BLOCK_GROUP_ITEM_KEY);
	btrfs_set_disk_key_objectid(&disk_key, bytenr);
	btrfs_set_disk_key_offset(&disk_key, len);
	btrfs_set_item_key(buf, &disk_key, *slot);
	btrfs_set_item_offset(buf, btrfs_item_nr(*slot), *itemoff);
	btrfs_set_item_size(buf, btrfs_item_nr(*slot), sizeof(bgi));

	btrfs_set_block_group_flags(&bgi, flag);
	btrfs_set_block_group_used(&bgi, used);
	btrfs_set_block_group_chunk_objectid(&bgi,
			BTRFS_FIRST_CHUNK_TREE_OBJECTID);
	write_extent_buffer(buf, &bgi, btrfs_item_ptr_offset(buf, *slot),
			    sizeof(bgi));
	(*slot)++;
}

static int setup_temp_extent_tree(int fd, struct btrfs_mkfs_config *cfg,
				  u64 chunk_bytenr, u64 root_bytenr,
				  u64 extent_bytenr, u64 dev_bytenr,
				  u64 fs_bytenr, u64 csum_bytenr)
{
	struct extent_buffer *buf = NULL;
	u32 itemoff = __BTRFS_LEAF_DATA_SIZE(cfg->nodesize);
	int slot = 0;
	int ret;

	/*
	 * We must ensure provided bytenr are in ascending order,
	 * or extent tree key order will be broken.
	 */
	if (!(chunk_bytenr < root_bytenr && root_bytenr < extent_bytenr &&
	      extent_bytenr < dev_bytenr && dev_bytenr < fs_bytenr &&
	      fs_bytenr < csum_bytenr)) {
		error("bad tree bytenr order: "
				"chunk < root %llu < %llu, "
				"root < extent %llu < %llu, "
				"extent < dev %llu < %llu, "
				"dev < fs %llu < %llu, "
				"fs < csum %llu < %llu",
				(unsigned long long)chunk_bytenr,
				(unsigned long long)root_bytenr,
				(unsigned long long)root_bytenr,
				(unsigned long long)extent_bytenr,
				(unsigned long long)extent_bytenr,
				(unsigned long long)dev_bytenr,
				(unsigned long long)dev_bytenr,
				(unsigned long long)fs_bytenr,
				(unsigned long long)fs_bytenr,
				(unsigned long long)csum_bytenr);
		return -EINVAL;
	}
	buf = malloc(sizeof(*buf) + cfg->nodesize);
	if (!buf)
		return -ENOMEM;

	ret = setup_temp_extent_buffer(buf, cfg, extent_bytenr,
				       BTRFS_EXTENT_TREE_OBJECTID);
	if (ret < 0)
		goto out;

	ret = insert_temp_extent_item(fd, buf, cfg, &slot, &itemoff,
			chunk_bytenr, BTRFS_CHUNK_TREE_OBJECTID);
	if (ret < 0)
		goto out;

	insert_temp_block_group(buf, cfg, &slot, &itemoff, chunk_bytenr,
			BTRFS_MKFS_SYSTEM_GROUP_SIZE, cfg->nodesize,
			BTRFS_BLOCK_GROUP_SYSTEM);

	ret = insert_temp_extent_item(fd, buf, cfg, &slot, &itemoff,
			root_bytenr, BTRFS_ROOT_TREE_OBJECTID);
	if (ret < 0)
		goto out;

	/* 5 tree block used, root, extent, dev, fs and csum*/
	insert_temp_block_group(buf, cfg, &slot, &itemoff, root_bytenr,
			BTRFS_CONVERT_META_GROUP_SIZE, cfg->nodesize * 5,
			BTRFS_BLOCK_GROUP_METADATA);

	ret = insert_temp_extent_item(fd, buf, cfg, &slot, &itemoff,
			extent_bytenr, BTRFS_EXTENT_TREE_OBJECTID);
	if (ret < 0)
		goto out;
	ret = insert_temp_extent_item(fd, buf, cfg, &slot, &itemoff,
			dev_bytenr, BTRFS_DEV_TREE_OBJECTID);
	if (ret < 0)
		goto out;
	ret = insert_temp_extent_item(fd, buf, cfg, &slot, &itemoff,
			fs_bytenr, BTRFS_FS_TREE_OBJECTID);
	if (ret < 0)
		goto out;
	ret = insert_temp_extent_item(fd, buf, cfg, &slot, &itemoff,
			csum_bytenr, BTRFS_CSUM_TREE_OBJECTID);
	if (ret < 0)
		goto out;

	ret = write_temp_extent_buffer(fd, buf, extent_bytenr);
out:
	free(buf);
	return ret;
}

/*
 * Improved version of make_btrfs().
 *
 * This one will
 * 1) Do chunk allocation to avoid used data
 *    And after this function, extent type matches chunk type
 * 2) Better structured code
 *    No super long hand written codes to initialized all tree blocks
 *    Split into small blocks and reuse codes.
 *    TODO: Reuse tree operation facilities by introducing new flags
 */
static int make_convert_btrfs(int fd, struct btrfs_mkfs_config *cfg,
			      struct btrfs_convert_context *cctx)
{
	struct cache_tree *free = &cctx->free;
	struct cache_tree *used = &cctx->used;
	u64 sys_chunk_start;
	u64 meta_chunk_start;
	/* chunk tree bytenr, in system chunk */
	u64 chunk_bytenr;
	/* metadata trees bytenr, in metadata chunk */
	u64 root_bytenr;
	u64 extent_bytenr;
	u64 dev_bytenr;
	u64 fs_bytenr;
	u64 csum_bytenr;
	int ret;

	/* Shouldn't happen */
	BUG_ON(cache_tree_empty(used));

	/*
	 * reserve space for temporary superblock first
	 * Here we allocate a little larger space, to keep later
	 * free space will be STRIPE_LEN aligned
	 */
	ret = reserve_free_space(free, BTRFS_STRIPE_LEN,
				 &cfg->super_bytenr);
	if (ret < 0)
		goto out;

	/*
	 * Then reserve system chunk space
	 * TODO: Change system group size depending on cctx->total_bytes.
	 * If using current 4M, it can only handle less than one TB for
	 * worst case and then run out of sys space.
	 */
	ret = reserve_free_space(free, BTRFS_MKFS_SYSTEM_GROUP_SIZE,
				 &sys_chunk_start);
	if (ret < 0)
		goto out;
	ret = reserve_free_space(free, BTRFS_CONVERT_META_GROUP_SIZE,
				 &meta_chunk_start);
	if (ret < 0)
		goto out;

	/*
	 * Allocated meta/sys chunks will be mapped 1:1 with device offset.
	 *
	 * Inside the allocated metadata chunk, the layout will be:
	 *  | offset		| contents	|
	 *  -------------------------------------
	 *  | +0		| tree root	|
	 *  | +nodesize		| extent root	|
	 *  | +nodesize * 2	| device root	|
	 *  | +nodesize * 3	| fs tree	|
	 *  | +nodesize * 4	| csum tree	|
	 *  -------------------------------------
	 * Inside the allocated system chunk, the layout will be:
	 *  | offset		| contents	|
	 *  -------------------------------------
	 *  | +0		| chunk root	|
	 *  -------------------------------------
	 */
	chunk_bytenr = sys_chunk_start;
	root_bytenr = meta_chunk_start;
	extent_bytenr = meta_chunk_start + cfg->nodesize;
	dev_bytenr = meta_chunk_start + cfg->nodesize * 2;
	fs_bytenr = meta_chunk_start + cfg->nodesize * 3;
	csum_bytenr = meta_chunk_start + cfg->nodesize * 4;

	ret = setup_temp_super(fd, cfg, root_bytenr, chunk_bytenr);
	if (ret < 0)
		goto out;

	ret = setup_temp_root_tree(fd, cfg, root_bytenr, extent_bytenr,
				   dev_bytenr, fs_bytenr, csum_bytenr);
	if (ret < 0)
		goto out;
	ret = setup_temp_chunk_tree(fd, cfg, sys_chunk_start, meta_chunk_start,
				    chunk_bytenr);
	if (ret < 0)
		goto out;
	ret = setup_temp_dev_tree(fd, cfg, sys_chunk_start, meta_chunk_start,
				  dev_bytenr);
	if (ret < 0)
		goto out;
	ret = setup_temp_fs_tree(fd, cfg, fs_bytenr);
	if (ret < 0)
		goto out;
	ret = setup_temp_csum_tree(fd, cfg, csum_bytenr);
	if (ret < 0)
		goto out;
	/*
	 * Setup extent tree last, since it may need to read tree block key
	 * for non-skinny metadata case.
	 */
	ret = setup_temp_extent_tree(fd, cfg, chunk_bytenr, root_bytenr,
				     extent_bytenr, dev_bytenr, fs_bytenr,
				     csum_bytenr);
out:
	return ret;
}

/*
 * @fs_uuid - if NULL, generates a UUID, returns back the new filesystem UUID
 *
 * The superblock signature is not valid, denotes a partially created
 * filesystem, needs to be finalized.
 */
int make_btrfs(int fd, struct btrfs_mkfs_config *cfg,
		struct btrfs_convert_context *cctx)
{
	struct btrfs_super_block super;
	struct extent_buffer *buf;
	struct btrfs_root_item root_item;
	struct btrfs_disk_key disk_key;
	struct btrfs_extent_item *extent_item;
	struct btrfs_inode_item *inode_item;
	struct btrfs_chunk *chunk;
	struct btrfs_dev_item *dev_item;
	struct btrfs_dev_extent *dev_extent;
	u8 chunk_tree_uuid[BTRFS_UUID_SIZE];
	u8 *ptr;
	int i;
	int ret;
	u32 itemoff;
	u32 nritems = 0;
	u64 first_free;
	u64 ref_root;
	u32 array_size;
	u32 item_size;
	int skinny_metadata = !!(cfg->features &
				 BTRFS_FEATURE_INCOMPAT_SKINNY_METADATA);
	u64 num_bytes;

	if (cctx)
		return make_convert_btrfs(fd, cfg, cctx);
	buf = malloc(sizeof(*buf) + max(cfg->sectorsize, cfg->nodesize));
	if (!buf)
		return -ENOMEM;

	first_free = BTRFS_SUPER_INFO_OFFSET + cfg->sectorsize * 2 - 1;
	first_free &= ~((u64)cfg->sectorsize - 1);

	memset(&super, 0, sizeof(super));

	num_bytes = (cfg->num_bytes / cfg->sectorsize) * cfg->sectorsize;
	if (*cfg->fs_uuid) {
		if (uuid_parse(cfg->fs_uuid, super.fsid) != 0) {
			error("cannot not parse UUID: %s", cfg->fs_uuid);
			ret = -EINVAL;
			goto out;
		}
		if (!test_uuid_unique(cfg->fs_uuid)) {
			error("non-unique UUID: %s", cfg->fs_uuid);
			ret = -EBUSY;
			goto out;
		}
	} else {
		uuid_generate(super.fsid);
		uuid_unparse(super.fsid, cfg->fs_uuid);
	}
	uuid_generate(super.dev_item.uuid);
	uuid_generate(chunk_tree_uuid);

	btrfs_set_super_bytenr(&super, cfg->blocks[0]);
	btrfs_set_super_num_devices(&super, 1);
	btrfs_set_super_magic(&super, BTRFS_MAGIC_PARTIAL);
	btrfs_set_super_generation(&super, 1);
	btrfs_set_super_root(&super, cfg->blocks[1]);
	btrfs_set_super_chunk_root(&super, cfg->blocks[3]);
	btrfs_set_super_total_bytes(&super, num_bytes);
	btrfs_set_super_bytes_used(&super, 6 * cfg->nodesize);
	btrfs_set_super_sectorsize(&super, cfg->sectorsize);
	btrfs_set_super_leafsize(&super, cfg->nodesize);
	btrfs_set_super_nodesize(&super, cfg->nodesize);
	btrfs_set_super_stripesize(&super, cfg->stripesize);
	btrfs_set_super_csum_type(&super, BTRFS_CSUM_TYPE_CRC32);
	btrfs_set_super_chunk_root_generation(&super, 1);
	btrfs_set_super_cache_generation(&super, -1);
	btrfs_set_super_incompat_flags(&super, cfg->features);
	if (cfg->label)
		__strncpy_null(super.label, cfg->label, BTRFS_LABEL_SIZE - 1);

	/* create the tree of root objects */
	memset(buf->data, 0, cfg->nodesize);
	buf->len = cfg->nodesize;
	btrfs_set_header_bytenr(buf, cfg->blocks[1]);
	btrfs_set_header_nritems(buf, 4);
	btrfs_set_header_generation(buf, 1);
	btrfs_set_header_backref_rev(buf, BTRFS_MIXED_BACKREF_REV);
	btrfs_set_header_owner(buf, BTRFS_ROOT_TREE_OBJECTID);
	write_extent_buffer(buf, super.fsid, btrfs_header_fsid(),
			    BTRFS_FSID_SIZE);

	write_extent_buffer(buf, chunk_tree_uuid,
			    btrfs_header_chunk_tree_uuid(buf),
			    BTRFS_UUID_SIZE);

	/* create the items for the root tree */
	memset(&root_item, 0, sizeof(root_item));
	inode_item = &root_item.inode;
	btrfs_set_stack_inode_generation(inode_item, 1);
	btrfs_set_stack_inode_size(inode_item, 3);
	btrfs_set_stack_inode_nlink(inode_item, 1);
	btrfs_set_stack_inode_nbytes(inode_item, cfg->nodesize);
	btrfs_set_stack_inode_mode(inode_item, S_IFDIR | 0755);
	btrfs_set_root_refs(&root_item, 1);
	btrfs_set_root_used(&root_item, cfg->nodesize);
	btrfs_set_root_generation(&root_item, 1);

	memset(&disk_key, 0, sizeof(disk_key));
	btrfs_set_disk_key_type(&disk_key, BTRFS_ROOT_ITEM_KEY);
	btrfs_set_disk_key_offset(&disk_key, 0);
	nritems = 0;

	itemoff = __BTRFS_LEAF_DATA_SIZE(cfg->nodesize) - sizeof(root_item);
	btrfs_set_root_bytenr(&root_item, cfg->blocks[2]);
	btrfs_set_disk_key_objectid(&disk_key, BTRFS_EXTENT_TREE_OBJECTID);
	btrfs_set_item_key(buf, &disk_key, nritems);
	btrfs_set_item_offset(buf, btrfs_item_nr(nritems), itemoff);
	btrfs_set_item_size(buf, btrfs_item_nr(nritems),
			    sizeof(root_item));
	write_extent_buffer(buf, &root_item, btrfs_item_ptr_offset(buf,
			    nritems), sizeof(root_item));
	nritems++;

	itemoff = itemoff - sizeof(root_item);
	btrfs_set_root_bytenr(&root_item, cfg->blocks[4]);
	btrfs_set_disk_key_objectid(&disk_key, BTRFS_DEV_TREE_OBJECTID);
	btrfs_set_item_key(buf, &disk_key, nritems);
	btrfs_set_item_offset(buf, btrfs_item_nr(nritems), itemoff);
	btrfs_set_item_size(buf, btrfs_item_nr(nritems),
			    sizeof(root_item));
	write_extent_buffer(buf, &root_item,
			    btrfs_item_ptr_offset(buf, nritems),
			    sizeof(root_item));
	nritems++;

	itemoff = itemoff - sizeof(root_item);
	btrfs_set_root_bytenr(&root_item, cfg->blocks[5]);
	btrfs_set_disk_key_objectid(&disk_key, BTRFS_FS_TREE_OBJECTID);
	btrfs_set_item_key(buf, &disk_key, nritems);
	btrfs_set_item_offset(buf, btrfs_item_nr(nritems), itemoff);
	btrfs_set_item_size(buf, btrfs_item_nr(nritems),
			    sizeof(root_item));
	write_extent_buffer(buf, &root_item,
			    btrfs_item_ptr_offset(buf, nritems),
			    sizeof(root_item));
	nritems++;

	itemoff = itemoff - sizeof(root_item);
	btrfs_set_root_bytenr(&root_item, cfg->blocks[6]);
	btrfs_set_disk_key_objectid(&disk_key, BTRFS_CSUM_TREE_OBJECTID);
	btrfs_set_item_key(buf, &disk_key, nritems);
	btrfs_set_item_offset(buf, btrfs_item_nr(nritems), itemoff);
	btrfs_set_item_size(buf, btrfs_item_nr(nritems),
			    sizeof(root_item));
	write_extent_buffer(buf, &root_item,
			    btrfs_item_ptr_offset(buf, nritems),
			    sizeof(root_item));
	nritems++;


	csum_tree_block_size(buf, BTRFS_CRC32_SIZE, 0);
	ret = pwrite(fd, buf->data, cfg->nodesize, cfg->blocks[1]);
	if (ret != cfg->nodesize) {
		ret = (ret < 0 ? -errno : -EIO);
		goto out;
	}

	/* create the items for the extent tree */
	memset(buf->data + sizeof(struct btrfs_header), 0,
		cfg->nodesize - sizeof(struct btrfs_header));
	nritems = 0;
	itemoff = __BTRFS_LEAF_DATA_SIZE(cfg->nodesize);
	for (i = 1; i < 7; i++) {
		item_size = sizeof(struct btrfs_extent_item);
		if (!skinny_metadata)
			item_size += sizeof(struct btrfs_tree_block_info);

		if (cfg->blocks[i] < first_free) {
			error("block[%d] below first free: %llu < %llu",
					i, (unsigned long long)cfg->blocks[i],
					(unsigned long long)first_free);
			ret = -EINVAL;
			goto out;
		}
		if (cfg->blocks[i] < cfg->blocks[i - 1]) {
			error("blocks %d and %d in reverse order: %llu < %llu",
				i, i - 1,
				(unsigned long long)cfg->blocks[i],
				(unsigned long long)cfg->blocks[i - 1]);
			ret = -EINVAL;
			goto out;
		}

		/* create extent item */
		itemoff -= item_size;
		btrfs_set_disk_key_objectid(&disk_key, cfg->blocks[i]);
		if (skinny_metadata) {
			btrfs_set_disk_key_type(&disk_key,
						BTRFS_METADATA_ITEM_KEY);
			btrfs_set_disk_key_offset(&disk_key, 0);
		} else {
			btrfs_set_disk_key_type(&disk_key,
						BTRFS_EXTENT_ITEM_KEY);
			btrfs_set_disk_key_offset(&disk_key, cfg->nodesize);
		}
		btrfs_set_item_key(buf, &disk_key, nritems);
		btrfs_set_item_offset(buf, btrfs_item_nr(nritems),
				      itemoff);
		btrfs_set_item_size(buf, btrfs_item_nr(nritems),
				    item_size);
		extent_item = btrfs_item_ptr(buf, nritems,
					     struct btrfs_extent_item);
		btrfs_set_extent_refs(buf, extent_item, 1);
		btrfs_set_extent_generation(buf, extent_item, 1);
		btrfs_set_extent_flags(buf, extent_item,
				       BTRFS_EXTENT_FLAG_TREE_BLOCK);
		nritems++;

		/* create extent ref */
		ref_root = reference_root_table[i];
		btrfs_set_disk_key_objectid(&disk_key, cfg->blocks[i]);
		btrfs_set_disk_key_offset(&disk_key, ref_root);
		btrfs_set_disk_key_type(&disk_key, BTRFS_TREE_BLOCK_REF_KEY);
		btrfs_set_item_key(buf, &disk_key, nritems);
		btrfs_set_item_offset(buf, btrfs_item_nr(nritems),
				      itemoff);
		btrfs_set_item_size(buf, btrfs_item_nr(nritems), 0);
		nritems++;
	}
	btrfs_set_header_bytenr(buf, cfg->blocks[2]);
	btrfs_set_header_owner(buf, BTRFS_EXTENT_TREE_OBJECTID);
	btrfs_set_header_nritems(buf, nritems);
	csum_tree_block_size(buf, BTRFS_CRC32_SIZE, 0);
	ret = pwrite(fd, buf->data, cfg->nodesize, cfg->blocks[2]);
	if (ret != cfg->nodesize) {
		ret = (ret < 0 ? -errno : -EIO);
		goto out;
	}

	/* create the chunk tree */
	memset(buf->data + sizeof(struct btrfs_header), 0,
		cfg->nodesize - sizeof(struct btrfs_header));
	nritems = 0;
	item_size = sizeof(*dev_item);
	itemoff = __BTRFS_LEAF_DATA_SIZE(cfg->nodesize) - item_size;

	/* first device 1 (there is no device 0) */
	btrfs_set_disk_key_objectid(&disk_key, BTRFS_DEV_ITEMS_OBJECTID);
	btrfs_set_disk_key_offset(&disk_key, 1);
	btrfs_set_disk_key_type(&disk_key, BTRFS_DEV_ITEM_KEY);
	btrfs_set_item_key(buf, &disk_key, nritems);
	btrfs_set_item_offset(buf, btrfs_item_nr(nritems), itemoff);
	btrfs_set_item_size(buf, btrfs_item_nr(nritems), item_size);

	dev_item = btrfs_item_ptr(buf, nritems, struct btrfs_dev_item);
	btrfs_set_device_id(buf, dev_item, 1);
	btrfs_set_device_generation(buf, dev_item, 0);
	btrfs_set_device_total_bytes(buf, dev_item, num_bytes);
	btrfs_set_device_bytes_used(buf, dev_item,
				    BTRFS_MKFS_SYSTEM_GROUP_SIZE);
	btrfs_set_device_io_align(buf, dev_item, cfg->sectorsize);
	btrfs_set_device_io_width(buf, dev_item, cfg->sectorsize);
	btrfs_set_device_sector_size(buf, dev_item, cfg->sectorsize);
	btrfs_set_device_type(buf, dev_item, 0);

	write_extent_buffer(buf, super.dev_item.uuid,
			    (unsigned long)btrfs_device_uuid(dev_item),
			    BTRFS_UUID_SIZE);
	write_extent_buffer(buf, super.fsid,
			    (unsigned long)btrfs_device_fsid(dev_item),
			    BTRFS_UUID_SIZE);
	read_extent_buffer(buf, &super.dev_item, (unsigned long)dev_item,
			   sizeof(*dev_item));

	nritems++;
	item_size = btrfs_chunk_item_size(1);
	itemoff = itemoff - item_size;

	/* then we have chunk 0 */
	btrfs_set_disk_key_objectid(&disk_key, BTRFS_FIRST_CHUNK_TREE_OBJECTID);
	btrfs_set_disk_key_offset(&disk_key, 0);
	btrfs_set_disk_key_type(&disk_key, BTRFS_CHUNK_ITEM_KEY);
	btrfs_set_item_key(buf, &disk_key, nritems);
	btrfs_set_item_offset(buf, btrfs_item_nr(nritems), itemoff);
	btrfs_set_item_size(buf, btrfs_item_nr(nritems), item_size);

	chunk = btrfs_item_ptr(buf, nritems, struct btrfs_chunk);
	btrfs_set_chunk_length(buf, chunk, BTRFS_MKFS_SYSTEM_GROUP_SIZE);
	btrfs_set_chunk_owner(buf, chunk, BTRFS_EXTENT_TREE_OBJECTID);
	btrfs_set_chunk_stripe_len(buf, chunk, 64 * 1024);
	btrfs_set_chunk_type(buf, chunk, BTRFS_BLOCK_GROUP_SYSTEM);
	btrfs_set_chunk_io_align(buf, chunk, cfg->sectorsize);
	btrfs_set_chunk_io_width(buf, chunk, cfg->sectorsize);
	btrfs_set_chunk_sector_size(buf, chunk, cfg->sectorsize);
	btrfs_set_chunk_num_stripes(buf, chunk, 1);
	btrfs_set_stripe_devid_nr(buf, chunk, 0, 1);
	btrfs_set_stripe_offset_nr(buf, chunk, 0, 0);
	nritems++;

	write_extent_buffer(buf, super.dev_item.uuid,
			    (unsigned long)btrfs_stripe_dev_uuid(&chunk->stripe),
			    BTRFS_UUID_SIZE);

	/* copy the key for the chunk to the system array */
	ptr = super.sys_chunk_array;
	array_size = sizeof(disk_key);

	memcpy(ptr, &disk_key, sizeof(disk_key));
	ptr += sizeof(disk_key);

	/* copy the chunk to the system array */
	read_extent_buffer(buf, ptr, (unsigned long)chunk, item_size);
	array_size += item_size;
	ptr += item_size;
	btrfs_set_super_sys_array_size(&super, array_size);

	btrfs_set_header_bytenr(buf, cfg->blocks[3]);
	btrfs_set_header_owner(buf, BTRFS_CHUNK_TREE_OBJECTID);
	btrfs_set_header_nritems(buf, nritems);
	csum_tree_block_size(buf, BTRFS_CRC32_SIZE, 0);
	ret = pwrite(fd, buf->data, cfg->nodesize, cfg->blocks[3]);
	if (ret != cfg->nodesize) {
		ret = (ret < 0 ? -errno : -EIO);
		goto out;
	}

	/* create the device tree */
	memset(buf->data + sizeof(struct btrfs_header), 0,
		cfg->nodesize - sizeof(struct btrfs_header));
	nritems = 0;
	itemoff = __BTRFS_LEAF_DATA_SIZE(cfg->nodesize) -
		sizeof(struct btrfs_dev_extent);

	btrfs_set_disk_key_objectid(&disk_key, 1);
	btrfs_set_disk_key_offset(&disk_key, 0);
	btrfs_set_disk_key_type(&disk_key, BTRFS_DEV_EXTENT_KEY);
	btrfs_set_item_key(buf, &disk_key, nritems);
	btrfs_set_item_offset(buf, btrfs_item_nr(nritems), itemoff);
	btrfs_set_item_size(buf, btrfs_item_nr(nritems),
			    sizeof(struct btrfs_dev_extent));
	dev_extent = btrfs_item_ptr(buf, nritems, struct btrfs_dev_extent);
	btrfs_set_dev_extent_chunk_tree(buf, dev_extent,
					BTRFS_CHUNK_TREE_OBJECTID);
	btrfs_set_dev_extent_chunk_objectid(buf, dev_extent,
					BTRFS_FIRST_CHUNK_TREE_OBJECTID);
	btrfs_set_dev_extent_chunk_offset(buf, dev_extent, 0);

	write_extent_buffer(buf, chunk_tree_uuid,
		    (unsigned long)btrfs_dev_extent_chunk_tree_uuid(dev_extent),
		    BTRFS_UUID_SIZE);

	btrfs_set_dev_extent_length(buf, dev_extent,
				    BTRFS_MKFS_SYSTEM_GROUP_SIZE);
	nritems++;

	btrfs_set_header_bytenr(buf, cfg->blocks[4]);
	btrfs_set_header_owner(buf, BTRFS_DEV_TREE_OBJECTID);
	btrfs_set_header_nritems(buf, nritems);
	csum_tree_block_size(buf, BTRFS_CRC32_SIZE, 0);
	ret = pwrite(fd, buf->data, cfg->nodesize, cfg->blocks[4]);
	if (ret != cfg->nodesize) {
		ret = (ret < 0 ? -errno : -EIO);
		goto out;
	}

	/* create the FS root */
	memset(buf->data + sizeof(struct btrfs_header), 0,
		cfg->nodesize - sizeof(struct btrfs_header));
	btrfs_set_header_bytenr(buf, cfg->blocks[5]);
	btrfs_set_header_owner(buf, BTRFS_FS_TREE_OBJECTID);
	btrfs_set_header_nritems(buf, 0);
	csum_tree_block_size(buf, BTRFS_CRC32_SIZE, 0);
	ret = pwrite(fd, buf->data, cfg->nodesize, cfg->blocks[5]);
	if (ret != cfg->nodesize) {
		ret = (ret < 0 ? -errno : -EIO);
		goto out;
	}
	/* finally create the csum root */
	memset(buf->data + sizeof(struct btrfs_header), 0,
		cfg->nodesize - sizeof(struct btrfs_header));
	btrfs_set_header_bytenr(buf, cfg->blocks[6]);
	btrfs_set_header_owner(buf, BTRFS_CSUM_TREE_OBJECTID);
	btrfs_set_header_nritems(buf, 0);
	csum_tree_block_size(buf, BTRFS_CRC32_SIZE, 0);
	ret = pwrite(fd, buf->data, cfg->nodesize, cfg->blocks[6]);
	if (ret != cfg->nodesize) {
		ret = (ret < 0 ? -errno : -EIO);
		goto out;
	}

	/* and write out the super block */
	memset(buf->data, 0, BTRFS_SUPER_INFO_SIZE);
	memcpy(buf->data, &super, sizeof(super));
	buf->len = BTRFS_SUPER_INFO_SIZE;
	csum_tree_block_size(buf, BTRFS_CRC32_SIZE, 0);
	ret = pwrite(fd, buf->data, BTRFS_SUPER_INFO_SIZE, cfg->blocks[0]);
	if (ret != BTRFS_SUPER_INFO_SIZE) {
		ret = (ret < 0 ? -errno : -EIO);
		goto out;
	}

	ret = 0;

out:
	free(buf);
	return ret;
}

#define VERSION_TO_STRING3(a,b,c)	#a "." #b "." #c, KERNEL_VERSION(a,b,c)
#define VERSION_TO_STRING2(a,b)		#a "." #b, KERNEL_VERSION(a,b,0)

/*
 * Feature stability status and versions: compat <= safe <= default
 */
static const struct btrfs_fs_feature {
	const char *name;
	u64 flag;
	const char *sysfs_name;
	/*
	 * Compatibility with kernel of given version. Filesystem can be
	 * mounted.
	 */
	const char *compat_str;
	u32 compat_ver;
	/*
	 * Considered safe for use, but is not on by default, even if the
	 * kernel supports the feature.
	 */
	const char *safe_str;
	u32 safe_ver;
	/*
	 * Considered safe for use and will be turned on by default if
	 * supported by the running kernel.
	 */
	const char *default_str;
	u32 default_ver;
	const char *desc;
} mkfs_features[] = {
	{ "mixed-bg", BTRFS_FEATURE_INCOMPAT_MIXED_GROUPS,
		"mixed_groups",
		VERSION_TO_STRING3(2,6,37),
		VERSION_TO_STRING3(2,6,37),
		NULL, 0,
		"mixed data and metadata block groups" },
	{ "extref", BTRFS_FEATURE_INCOMPAT_EXTENDED_IREF,
		"extended_iref",
		VERSION_TO_STRING2(3,7),
		VERSION_TO_STRING2(3,12),
		VERSION_TO_STRING2(3,12),
		"increased hardlink limit per file to 65536" },
	{ "raid56", BTRFS_FEATURE_INCOMPAT_RAID56,
		"raid56",
		VERSION_TO_STRING2(3,9),
		NULL, 0,
		NULL, 0,
		"raid56 extended format" },
	{ "skinny-metadata", BTRFS_FEATURE_INCOMPAT_SKINNY_METADATA,
		"skinny_metadata",
		VERSION_TO_STRING2(3,10),
		VERSION_TO_STRING2(3,18),
		VERSION_TO_STRING2(3,18),
		"reduced-size metadata extent refs" },
	{ "no-holes", BTRFS_FEATURE_INCOMPAT_NO_HOLES,
		"no_holes",
		VERSION_TO_STRING2(3,14),
		VERSION_TO_STRING2(4,0),
		NULL, 0,
		"no explicit hole extents for files" },
	/* Keep this one last */
	{ "list-all", BTRFS_FEATURE_LIST_ALL, NULL }
};

static int parse_one_fs_feature(const char *name, u64 *flags)
{
	int i;
	int found = 0;

	for (i = 0; i < ARRAY_SIZE(mkfs_features); i++) {
		if (name[0] == '^' &&
			!strcmp(mkfs_features[i].name, name + 1)) {
			*flags &= ~ mkfs_features[i].flag;
			found = 1;
		} else if (!strcmp(mkfs_features[i].name, name)) {
			*flags |= mkfs_features[i].flag;
			found = 1;
		}
	}

	return !found;
}

void btrfs_parse_features_to_string(char *buf, u64 flags)
{
	int i;

	buf[0] = 0;

	for (i = 0; i < ARRAY_SIZE(mkfs_features); i++) {
		if (flags & mkfs_features[i].flag) {
			if (*buf)
				strcat(buf, ", ");
			strcat(buf, mkfs_features[i].name);
		}
	}
}

void btrfs_process_fs_features(u64 flags)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(mkfs_features); i++) {
		if (flags & mkfs_features[i].flag) {
			printf("Turning ON incompat feature '%s': %s\n",
				mkfs_features[i].name,
				mkfs_features[i].desc);
		}
	}
}

void btrfs_list_all_fs_features(u64 mask_disallowed)
{
	int i;

	fprintf(stderr, "Filesystem features available:\n");
	for (i = 0; i < ARRAY_SIZE(mkfs_features) - 1; i++) {
		const struct btrfs_fs_feature *feat = &mkfs_features[i];

		if (feat->flag & mask_disallowed)
			continue;
		fprintf(stderr, "%-20s- %s (0x%llx", feat->name, feat->desc,
				feat->flag);
		if (feat->compat_ver)
			fprintf(stderr, ", compat=%s", feat->compat_str);
		if (feat->safe_ver)
			fprintf(stderr, ", safe=%s", feat->safe_str);
		if (feat->default_ver)
			fprintf(stderr, ", default=%s", feat->default_str);
		fprintf(stderr, ")\n");
	}
}

/*
 * Return NULL if all features were parsed fine, otherwise return the name of
 * the first unparsed.
 */
char* btrfs_parse_fs_features(char *namelist, u64 *flags)
{
	char *this_char;
	char *save_ptr = NULL; /* Satisfy static checkers */

	for (this_char = strtok_r(namelist, ",", &save_ptr);
	     this_char != NULL;
	     this_char = strtok_r(NULL, ",", &save_ptr)) {
		if (parse_one_fs_feature(this_char, flags))
			return this_char;
	}

	return NULL;
}

void print_kernel_version(FILE *stream, u32 version)
{
	u32 v[3];

	v[0] = version & 0xFF;
	v[1] = (version >> 8) & 0xFF;
	v[2] = version >> 16;
	fprintf(stream, "%u.%u", v[2], v[1]);
	if (v[0])
		fprintf(stream, ".%u", v[0]);
}

u32 get_running_kernel_version(void)
{
	struct utsname utsbuf;
	char *tmp;
	char *saveptr = NULL;
	u32 version;

	uname(&utsbuf);
	if (strcmp(utsbuf.sysname, "Linux") != 0) {
		error("unsupported system: %s", utsbuf.sysname);
		exit(1);
	}
	/* 1.2.3-4-name */
	tmp = strchr(utsbuf.release, '-');
	if (tmp)
		*tmp = 0;

	tmp = strtok_r(utsbuf.release, ".", &saveptr);
	if (!string_is_numerical(tmp))
		return (u32)-1;
	version = atoi(tmp) << 16;
	tmp = strtok_r(NULL, ".", &saveptr);
	if (!string_is_numerical(tmp))
		return (u32)-1;
	version |= atoi(tmp) << 8;
	tmp = strtok_r(NULL, ".", &saveptr);
	if (tmp) {
		if (!string_is_numerical(tmp))
			return (u32)-1;
		version |= atoi(tmp);
	}

	return version;
}

u64 btrfs_device_size(int fd, struct stat *st)
{
	u64 size;
	if (S_ISREG(st->st_mode)) {
		return st->st_size;
	}
	if (!S_ISBLK(st->st_mode)) {
		return 0;
	}
	if (ioctl(fd, BLKGETSIZE64, &size) >= 0) {
		return size;
	}
	return 0;
}

static int zero_blocks(int fd, off_t start, size_t len)
{
	char *buf = malloc(len);
	int ret = 0;
	ssize_t written;

	if (!buf)
		return -ENOMEM;
	memset(buf, 0, len);
	written = pwrite(fd, buf, len, start);
	if (written != len)
		ret = -EIO;
	free(buf);
	return ret;
}

#define ZERO_DEV_BYTES (2 * 1024 * 1024)

/* don't write outside the device by clamping the region to the device size */
static int zero_dev_clamped(int fd, off_t start, ssize_t len, u64 dev_size)
{
	off_t end = max(start, start + len);

#ifdef __sparc__
	/* and don't overwrite the disk labels on sparc */
	start = max(start, 1024);
	end = max(end, 1024);
#endif

	start = min_t(u64, start, dev_size);
	end = min_t(u64, end, dev_size);

	return zero_blocks(fd, start, end - start);
}

int btrfs_add_to_fsid(struct btrfs_trans_handle *trans,
		      struct btrfs_root *root, int fd, const char *path,
		      u64 device_total_bytes, u32 io_width, u32 io_align,
		      u32 sectorsize)
{
	struct btrfs_super_block *disk_super;
	struct btrfs_super_block *super = root->fs_info->super_copy;
	struct btrfs_device *device;
	struct btrfs_dev_item *dev_item;
	char *buf = NULL;
	u64 fs_total_bytes;
	u64 num_devs;
	int ret;

	device_total_bytes = (device_total_bytes / sectorsize) * sectorsize;

	device = calloc(1, sizeof(*device));
	if (!device) {
		ret = -ENOMEM;
		goto out;
	}
	buf = calloc(1, sectorsize);
	if (!buf) {
		ret = -ENOMEM;
		goto out;
	}

	disk_super = (struct btrfs_super_block *)buf;
	dev_item = &disk_super->dev_item;

	uuid_generate(device->uuid);
	device->devid = 0;
	device->type = 0;
	device->io_width = io_width;
	device->io_align = io_align;
	device->sector_size = sectorsize;
	device->fd = fd;
	device->writeable = 1;
	device->total_bytes = device_total_bytes;
	device->bytes_used = 0;
	device->total_ios = 0;
	device->dev_root = root->fs_info->dev_root;
	device->name = strdup(path);
	if (!device->name) {
		ret = -ENOMEM;
		goto out;
	}

	INIT_LIST_HEAD(&device->dev_list);
	ret = btrfs_add_device(trans, root, device);
	if (ret)
		goto out;

	fs_total_bytes = btrfs_super_total_bytes(super) + device_total_bytes;
	btrfs_set_super_total_bytes(super, fs_total_bytes);

	num_devs = btrfs_super_num_devices(super) + 1;
	btrfs_set_super_num_devices(super, num_devs);

	memcpy(disk_super, super, sizeof(*disk_super));

	btrfs_set_super_bytenr(disk_super, BTRFS_SUPER_INFO_OFFSET);
	btrfs_set_stack_device_id(dev_item, device->devid);
	btrfs_set_stack_device_type(dev_item, device->type);
	btrfs_set_stack_device_io_align(dev_item, device->io_align);
	btrfs_set_stack_device_io_width(dev_item, device->io_width);
	btrfs_set_stack_device_sector_size(dev_item, device->sector_size);
	btrfs_set_stack_device_total_bytes(dev_item, device->total_bytes);
	btrfs_set_stack_device_bytes_used(dev_item, device->bytes_used);
	memcpy(&dev_item->uuid, device->uuid, BTRFS_UUID_SIZE);

	ret = pwrite(fd, buf, sectorsize, BTRFS_SUPER_INFO_OFFSET);
	BUG_ON(ret != sectorsize);

	free(buf);
	list_add(&device->dev_list, &root->fs_info->fs_devices->devices);
	device->fs_devices = root->fs_info->fs_devices;
	return 0;

out:
	free(device);
	free(buf);
	return ret;
}

static int btrfs_wipe_existing_sb(int fd)
{
	const char *off = NULL;
	size_t len = 0;
	loff_t offset;
	char buf[BUFSIZ];
	int ret = 0;
	blkid_probe pr = NULL;

	pr = blkid_new_probe();
	if (!pr)
		return -1;

	if (blkid_probe_set_device(pr, fd, 0, 0)) {
		ret = -1;
		goto out;
	}

	ret = blkid_probe_lookup_value(pr, "SBMAGIC_OFFSET", &off, NULL);
	if (!ret)
		ret = blkid_probe_lookup_value(pr, "SBMAGIC", NULL, &len);

	if (ret || len == 0 || off == NULL) {
		/*
		 * If lookup fails, the probe did not find any values, eg. for
		 * a file image or a loop device. Soft error.
		 */
		ret = 1;
		goto out;
	}

	offset = strtoll(off, NULL, 10);
	if (len > sizeof(buf))
		len = sizeof(buf);

	memset(buf, 0, len);
	ret = pwrite(fd, buf, len, offset);
	if (ret < 0) {
		error("cannot wipe existing superblock: %s", strerror(errno));
		ret = -1;
	} else if (ret != len) {
		error("cannot wipe existing superblock: wrote %d of %zd", ret, len);
		ret = -1;
	}
	fsync(fd);

out:
	blkid_free_probe(pr);
	return ret;
}

int btrfs_prepare_device(int fd, const char *file, u64 *block_count_ret,
		u64 max_block_count, unsigned opflags)
{
	u64 block_count;
	struct stat st;
	int i, ret;

	ret = fstat(fd, &st);
	if (ret < 0) {
		error("unable to stat %s: %s", file, strerror(errno));
		return 1;
	}

	block_count = btrfs_device_size(fd, &st);
	if (block_count == 0) {
		error("unable to determine size of %s", file);
		return 1;
	}
	if (max_block_count)
		block_count = min(block_count, max_block_count);

	if (opflags & PREP_DEVICE_DISCARD) {
		/*
		 * We intentionally ignore errors from the discard ioctl.  It
		 * is not necessary for the mkfs functionality but just an
		 * optimization.
		 */
		if (discard_range(fd, 0, 0) == 0) {
			if (opflags & PREP_DEVICE_VERBOSE)
				printf("Performing full device TRIM (%s) ...\n",
						pretty_size(block_count));
			discard_blocks(fd, 0, block_count);
		}
	}

	ret = zero_dev_clamped(fd, 0, ZERO_DEV_BYTES, block_count);
	for (i = 0 ; !ret && i < BTRFS_SUPER_MIRROR_MAX; i++)
		ret = zero_dev_clamped(fd, btrfs_sb_offset(i),
				       BTRFS_SUPER_INFO_SIZE, block_count);
	if (!ret && (opflags & PREP_DEVICE_ZERO_END))
		ret = zero_dev_clamped(fd, block_count - ZERO_DEV_BYTES,
				       ZERO_DEV_BYTES, block_count);

	if (ret < 0) {
		error("failed to zero device '%s': %s", file, strerror(-ret));
		return 1;
	}

	ret = btrfs_wipe_existing_sb(fd);
	if (ret < 0) {
		error("cannot wipe superblocks on %s", file);
		return 1;
	}

	*block_count_ret = block_count;
	return 0;
}

int btrfs_make_root_dir(struct btrfs_trans_handle *trans,
			struct btrfs_root *root, u64 objectid)
{
	int ret;
	struct btrfs_inode_item inode_item;
	time_t now = time(NULL);

	memset(&inode_item, 0, sizeof(inode_item));
	btrfs_set_stack_inode_generation(&inode_item, trans->transid);
	btrfs_set_stack_inode_size(&inode_item, 0);
	btrfs_set_stack_inode_nlink(&inode_item, 1);
	btrfs_set_stack_inode_nbytes(&inode_item, root->nodesize);
	btrfs_set_stack_inode_mode(&inode_item, S_IFDIR | 0755);
	btrfs_set_stack_timespec_sec(&inode_item.atime, now);
	btrfs_set_stack_timespec_nsec(&inode_item.atime, 0);
	btrfs_set_stack_timespec_sec(&inode_item.ctime, now);
	btrfs_set_stack_timespec_nsec(&inode_item.ctime, 0);
	btrfs_set_stack_timespec_sec(&inode_item.mtime, now);
	btrfs_set_stack_timespec_nsec(&inode_item.mtime, 0);
	btrfs_set_stack_timespec_sec(&inode_item.otime, now);
	btrfs_set_stack_timespec_nsec(&inode_item.otime, 0);

	if (root->fs_info->tree_root == root)
		btrfs_set_super_root_dir(root->fs_info->super_copy, objectid);

	ret = btrfs_insert_inode(trans, root, objectid, &inode_item);
	if (ret)
		goto error;

	ret = btrfs_insert_inode_ref(trans, root, "..", 2, objectid, objectid, 0);
	if (ret)
		goto error;

	btrfs_set_root_dirid(&root->root_item, objectid);
	ret = 0;
error:
	return ret;
}

/*
 * checks if a path is a block device node
 * Returns negative errno on failure, otherwise
 * returns 1 for blockdev, 0 for not-blockdev
 */
int is_block_device(const char *path)
{
	struct stat statbuf;

	if (stat(path, &statbuf) < 0)
		return -errno;

	return !!S_ISBLK(statbuf.st_mode);
}

/*
 * check if given path is a mount point
 * return 1 if yes. 0 if no. -1 for error
 */
int is_mount_point(const char *path)
{
	FILE *f;
	struct mntent *mnt;
	int ret = 0;

	f = setmntent("/proc/self/mounts", "r");
	if (f == NULL)
		return -1;

	while ((mnt = getmntent(f)) != NULL) {
		if (strcmp(mnt->mnt_dir, path))
			continue;
		ret = 1;
		break;
	}
	endmntent(f);
	return ret;
}

static int is_reg_file(const char *path)
{
	struct stat statbuf;

	if (stat(path, &statbuf) < 0)
		return -errno;
	return S_ISREG(statbuf.st_mode);
}

/*
 * This function checks if the given input parameter is
 * an uuid or a path
 * return <0 : some error in the given input
 * return BTRFS_ARG_UNKNOWN:	unknown input
 * return BTRFS_ARG_UUID:	given input is uuid
 * return BTRFS_ARG_MNTPOINT:	given input is path
 * return BTRFS_ARG_REG:	given input is regular file
 * return BTRFS_ARG_BLKDEV:	given input is block device
 */
int check_arg_type(const char *input)
{
	uuid_t uuid;
	char path[PATH_MAX];

	if (!input)
		return -EINVAL;

	if (realpath(input, path)) {
		if (is_block_device(path) == 1)
			return BTRFS_ARG_BLKDEV;

		if (is_mount_point(path) == 1)
			return BTRFS_ARG_MNTPOINT;

		if (is_reg_file(path))
			return BTRFS_ARG_REG;

		return BTRFS_ARG_UNKNOWN;
	}

	if (strlen(input) == (BTRFS_UUID_UNPARSED_SIZE - 1) &&
		!uuid_parse(input, uuid))
		return BTRFS_ARG_UUID;

	return BTRFS_ARG_UNKNOWN;
}

/*
 * Find the mount point for a mounted device.
 * On success, returns 0 with mountpoint in *mp.
 * On failure, returns -errno (not mounted yields -EINVAL)
 * Is noisy on failures, expects to be given a mounted device.
 */
int get_btrfs_mount(const char *dev, char *mp, size_t mp_size)
{
	int ret;
	int fd = -1;

	ret = is_block_device(dev);
	if (ret <= 0) {
		if (!ret) {
			error("not a block device: %s", dev);
			ret = -EINVAL;
		} else {
			error("cannot check %s: %s", dev, strerror(-ret));
		}
		goto out;
	}

	fd = open(dev, O_RDONLY);
	if (fd < 0) {
		ret = -errno;
		error("cannot open %s: %s", dev, strerror(errno));
		goto out;
	}

	ret = check_mounted_where(fd, dev, mp, mp_size, NULL);
	if (!ret) {
		ret = -EINVAL;
	} else { /* mounted, all good */
		ret = 0;
	}
out:
	if (fd != -1)
		close(fd);
	return ret;
}

/*
 * Given a pathname, return a filehandle to:
 * 	the original pathname or,
 * 	if the pathname is a mounted btrfs device, to its mountpoint.
 *
 * On error, return -1, errno should be set.
 */
int open_path_or_dev_mnt(const char *path, DIR **dirstream, int verbose)
{
	char mp[PATH_MAX];
	int ret;

	if (is_block_device(path)) {
		ret = get_btrfs_mount(path, mp, sizeof(mp));
		if (ret < 0) {
			/* not a mounted btrfs dev */
			error_on(verbose, "'%s' is not a mounted btrfs device",
				 path);
			errno = EINVAL;
			return -1;
		}
		ret = open_file_or_dir(mp, dirstream);
		error_on(verbose && ret < 0, "can't access '%s': %s",
			 path, strerror(errno));
	} else {
		ret = btrfs_open_dir(path, dirstream, 1);
	}

	return ret;
}

/*
 * Do the following checks before calling open_file_or_dir():
 * 1: path is in a btrfs filesystem
 * 2: path is a directory
 */
int btrfs_open_dir(const char *path, DIR **dirstream, int verbose)
{
	struct statfs stfs;
	struct stat st;
	int ret;

	if (statfs(path, &stfs) != 0) {
		error_on(verbose, "cannot access '%s': %s", path,
				strerror(errno));
		return -1;
	}

	if (stfs.f_type != BTRFS_SUPER_MAGIC) {
		error_on(verbose, "not a btrfs filesystem: %s", path);
		return -2;
	}

	if (stat(path, &st) != 0) {
		error_on(verbose, "cannot access '%s': %s", path,
				strerror(errno));
		return -1;
	}

	if (!S_ISDIR(st.st_mode)) {
		error_on(verbose, "not a directory: %s", path);
		return -3;
	}

	ret = open_file_or_dir(path, dirstream);
	if (ret < 0) {
		error_on(verbose, "cannot access '%s': %s", path,
				strerror(errno));
	}

	return ret;
}

/* checks if a device is a loop device */
static int is_loop_device (const char* device) {
	struct stat statbuf;

	if(stat(device, &statbuf) < 0)
		return -errno;

	return (S_ISBLK(statbuf.st_mode) &&
		MAJOR(statbuf.st_rdev) == LOOP_MAJOR);
}

/*
 * Takes a loop device path (e.g. /dev/loop0) and returns
 * the associated file (e.g. /images/my_btrfs.img) using
 * loopdev API
 */
static int resolve_loop_device_with_loopdev(const char* loop_dev, char* loop_file)
{
	int fd;
	int ret;
	struct loop_info64 lo64;

	fd = open(loop_dev, O_RDONLY | O_NONBLOCK);
	if (fd < 0)
		return -errno;
	ret = ioctl(fd, LOOP_GET_STATUS64, &lo64);
	if (ret < 0) {
		ret = -errno;
		goto out;
	}

	memcpy(loop_file, lo64.lo_file_name, sizeof(lo64.lo_file_name));
	loop_file[sizeof(lo64.lo_file_name)] = 0;

out:
	close(fd);

	return ret;
}

/* Takes a loop device path (e.g. /dev/loop0) and returns
 * the associated file (e.g. /images/my_btrfs.img) */
static int resolve_loop_device(const char* loop_dev, char* loop_file,
		int max_len)
{
	int ret;
	FILE *f;
	char fmt[20];
	char p[PATH_MAX];
	char real_loop_dev[PATH_MAX];

	if (!realpath(loop_dev, real_loop_dev))
		return -errno;
	snprintf(p, PATH_MAX, "/sys/block/%s/loop/backing_file", strrchr(real_loop_dev, '/'));
	if (!(f = fopen(p, "r"))) {
		if (errno == ENOENT)
			/*
			 * It's possibly a partitioned loop device, which is
			 * resolvable with loopdev API.
			 */
			return resolve_loop_device_with_loopdev(loop_dev, loop_file);
		return -errno;
	}

	snprintf(fmt, 20, "%%%i[^\n]", max_len-1);
	ret = fscanf(f, fmt, loop_file);
	fclose(f);
	if (ret == EOF)
		return -errno;

	return 0;
}

/*
 * Checks whether a and b are identical or device
 * files associated with the same block device
 */
static int is_same_blk_file(const char* a, const char* b)
{
	struct stat st_buf_a, st_buf_b;
	char real_a[PATH_MAX];
	char real_b[PATH_MAX];

	if (!realpath(a, real_a))
		strncpy_null(real_a, a);

	if (!realpath(b, real_b))
		strncpy_null(real_b, b);

	/* Identical path? */
	if (strcmp(real_a, real_b) == 0)
		return 1;

	if (stat(a, &st_buf_a) < 0 || stat(b, &st_buf_b) < 0) {
		if (errno == ENOENT)
			return 0;
		return -errno;
	}

	/* Same blockdevice? */
	if (S_ISBLK(st_buf_a.st_mode) && S_ISBLK(st_buf_b.st_mode) &&
	    st_buf_a.st_rdev == st_buf_b.st_rdev) {
		return 1;
	}

	/* Hardlink? */
	if (st_buf_a.st_dev == st_buf_b.st_dev &&
	    st_buf_a.st_ino == st_buf_b.st_ino) {
		return 1;
	}

	return 0;
}

/* checks if a and b are identical or device
 * files associated with the same block device or
 * if one file is a loop device that uses the other
 * file.
 */
static int is_same_loop_file(const char* a, const char* b)
{
	char res_a[PATH_MAX];
	char res_b[PATH_MAX];
	const char* final_a = NULL;
	const char* final_b = NULL;
	int ret;

	/* Resolve a if it is a loop device */
	if((ret = is_loop_device(a)) < 0) {
		if (ret == -ENOENT)
			return 0;
		return ret;
	} else if (ret) {
		ret = resolve_loop_device(a, res_a, sizeof(res_a));
		if (ret < 0) {
			if (errno != EPERM)
				return ret;
		} else {
			final_a = res_a;
		}
	} else {
		final_a = a;
	}

	/* Resolve b if it is a loop device */
	if ((ret = is_loop_device(b)) < 0) {
		if (ret == -ENOENT)
			return 0;
		return ret;
	} else if (ret) {
		ret = resolve_loop_device(b, res_b, sizeof(res_b));
		if (ret < 0) {
			if (errno != EPERM)
				return ret;
		} else {
			final_b = res_b;
		}
	} else {
		final_b = b;
	}

	return is_same_blk_file(final_a, final_b);
}

/* Checks if a file exists and is a block or regular file*/
static int is_existing_blk_or_reg_file(const char* filename)
{
	struct stat st_buf;

	if(stat(filename, &st_buf) < 0) {
		if(errno == ENOENT)
			return 0;
		else
			return -errno;
	}

	return (S_ISBLK(st_buf.st_mode) || S_ISREG(st_buf.st_mode));
}

/* Checks if a file is used (directly or indirectly via a loop device)
 * by a device in fs_devices
 */
static int blk_file_in_dev_list(struct btrfs_fs_devices* fs_devices,
		const char* file)
{
	int ret;
	struct list_head *head;
	struct list_head *cur;
	struct btrfs_device *device;

	head = &fs_devices->devices;
	list_for_each(cur, head) {
		device = list_entry(cur, struct btrfs_device, dev_list);

		if((ret = is_same_loop_file(device->name, file)))
			return ret;
	}

	return 0;
}

/*
 * Resolve a pathname to a device mapper node to /dev/mapper/<name>
 * Returns NULL on invalid input or malloc failure; Other failures
 * will be handled by the caller using the input pathame.
 */
char *canonicalize_dm_name(const char *ptname)
{
	FILE	*f;
	size_t	sz;
	char	path[PATH_MAX], name[PATH_MAX], *res = NULL;

	if (!ptname || !*ptname)
		return NULL;

	snprintf(path, sizeof(path), "/sys/block/%s/dm/name", ptname);
	if (!(f = fopen(path, "r")))
		return NULL;

	/* read <name>\n from sysfs */
	if (fgets(name, sizeof(name), f) && (sz = strlen(name)) > 1) {
		name[sz - 1] = '\0';
		snprintf(path, sizeof(path), "/dev/mapper/%s", name);

		if (access(path, F_OK) == 0)
			res = strdup(path);
	}
	fclose(f);
	return res;
}

/*
 * Resolve a pathname to a canonical device node, e.g. /dev/sda1 or
 * to a device mapper pathname.
 * Returns NULL on invalid input or malloc failure; Other failures
 * will be handled by the caller using the input pathame.
 */
char *canonicalize_path(const char *path)
{
	char *canonical, *p;

	if (!path || !*path)
		return NULL;

	canonical = realpath(path, NULL);
	if (!canonical)
		return strdup(path);
	p = strrchr(canonical, '/');
	if (p && strncmp(p, "/dm-", 4) == 0 && isdigit(*(p + 4))) {
		char *dm = canonicalize_dm_name(p + 1);

		if (dm) {
			free(canonical);
			return dm;
		}
	}
	return canonical;
}

/*
 * returns 1 if the device was mounted, < 0 on error or 0 if everything
 * is safe to continue.
 */
int check_mounted(const char* file)
{
	int fd;
	int ret;

	fd = open(file, O_RDONLY);
	if (fd < 0) {
		error("mount check: cannot open %s: %s", file,
				strerror(errno));
		return -errno;
	}

	ret =  check_mounted_where(fd, file, NULL, 0, NULL);
	close(fd);

	return ret;
}

int check_mounted_where(int fd, const char *file, char *where, int size,
			struct btrfs_fs_devices **fs_dev_ret)
{
	int ret;
	u64 total_devs = 1;
	int is_btrfs;
	struct btrfs_fs_devices *fs_devices_mnt = NULL;
	FILE *f;
	struct mntent *mnt;

	/* scan the initial device */
	ret = btrfs_scan_one_device(fd, file, &fs_devices_mnt,
		    &total_devs, BTRFS_SUPER_INFO_OFFSET, SBREAD_DEFAULT);
	is_btrfs = (ret >= 0);

	/* scan other devices */
	if (is_btrfs && total_devs > 1) {
		ret = btrfs_scan_devices();
		if (ret)
			return ret;
	}

	/* iterate over the list of currently mounted filesystems */
	if ((f = setmntent ("/proc/self/mounts", "r")) == NULL)
		return -errno;

	while ((mnt = getmntent (f)) != NULL) {
		if(is_btrfs) {
			if(strcmp(mnt->mnt_type, "btrfs") != 0)
				continue;

			ret = blk_file_in_dev_list(fs_devices_mnt, mnt->mnt_fsname);
		} else {
			/* ignore entries in the mount table that are not
			   associated with a file*/
			if((ret = is_existing_blk_or_reg_file(mnt->mnt_fsname)) < 0)
				goto out_mntloop_err;
			else if(!ret)
				continue;

			ret = is_same_loop_file(file, mnt->mnt_fsname);
		}

		if(ret < 0)
			goto out_mntloop_err;
		else if(ret)
			break;
	}

	/* Did we find an entry in mnt table? */
	if (mnt && size && where) {
		strncpy(where, mnt->mnt_dir, size);
		where[size-1] = 0;
	}
	if (fs_dev_ret)
		*fs_dev_ret = fs_devices_mnt;

	ret = (mnt != NULL);

out_mntloop_err:
	endmntent (f);

	return ret;
}

struct pending_dir {
	struct list_head list;
	char name[PATH_MAX];
};

int btrfs_register_one_device(const char *fname)
{
	struct btrfs_ioctl_vol_args args;
	int fd;
	int ret;

	fd = open("/dev/btrfs-control", O_RDWR);
	if (fd < 0) {
		warning(
	"failed to open /dev/btrfs-control, skipping device registration: %s",
			strerror(errno));
		return -errno;
	}
	memset(&args, 0, sizeof(args));
	strncpy_null(args.name, fname);
	ret = ioctl(fd, BTRFS_IOC_SCAN_DEV, &args);
	if (ret < 0) {
		error("device scan failed on '%s': %s", fname,
				strerror(errno));
		ret = -errno;
	}
	close(fd);
	return ret;
}

/*
 * Register all devices in the fs_uuid list created in the user
 * space. Ensure btrfs_scan_devices() is called before this func.
 */
int btrfs_register_all_devices(void)
{
	int err = 0;
	int ret = 0;
	struct btrfs_fs_devices *fs_devices;
	struct btrfs_device *device;
	struct list_head *all_uuids;

	all_uuids = btrfs_scanned_uuids();

	list_for_each_entry(fs_devices, all_uuids, list) {
		list_for_each_entry(device, &fs_devices->devices, dev_list) {
			if (*device->name)
				err = btrfs_register_one_device(device->name);

			if (err)
				ret++;
		}
	}

	return ret;
}

int btrfs_device_already_in_root(struct btrfs_root *root, int fd,
				 int super_offset)
{
	struct btrfs_super_block *disk_super;
	char *buf;
	int ret = 0;

	buf = malloc(BTRFS_SUPER_INFO_SIZE);
	if (!buf) {
		ret = -ENOMEM;
		goto out;
	}
	ret = pread(fd, buf, BTRFS_SUPER_INFO_SIZE, super_offset);
	if (ret != BTRFS_SUPER_INFO_SIZE)
		goto brelse;

	ret = 0;
	disk_super = (struct btrfs_super_block *)buf;
	/*
	 * Accept devices from the same filesystem, allow partially created
	 * structures.
	 */
	if (btrfs_super_magic(disk_super) != BTRFS_MAGIC &&
			btrfs_super_magic(disk_super) != BTRFS_MAGIC_PARTIAL)
		goto brelse;

	if (!memcmp(disk_super->fsid, root->fs_info->super_copy->fsid,
		    BTRFS_FSID_SIZE))
		ret = 1;
brelse:
	free(buf);
out:
	return ret;
}

/*
 * Note: this function uses a static per-thread buffer. Do not call this
 * function more than 10 times within one argument list!
 */
const char *pretty_size_mode(u64 size, unsigned mode)
{
	static __thread int ps_index = 0;
	static __thread char ps_array[10][32];
	char *ret;

	ret = ps_array[ps_index];
	ps_index++;
	ps_index %= 10;
	(void)pretty_size_snprintf(size, ret, 32, mode);

	return ret;
}

static const char* unit_suffix_binary[] =
	{ "B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB"};
static const char* unit_suffix_decimal[] =
	{ "B", "kB", "MB", "GB", "TB", "PB", "EB"};

int pretty_size_snprintf(u64 size, char *str, size_t str_size, unsigned unit_mode)
{
	int num_divs;
	float fraction;
	u64 base = 0;
	int mult = 0;
	const char** suffix = NULL;
	u64 last_size;

	if (str_size == 0)
		return 0;

	if ((unit_mode & ~UNITS_MODE_MASK) == UNITS_RAW) {
		snprintf(str, str_size, "%llu", size);
		return 0;
	}

	if ((unit_mode & ~UNITS_MODE_MASK) == UNITS_BINARY) {
		base = 1024;
		mult = 1024;
		suffix = unit_suffix_binary;
	} else if ((unit_mode & ~UNITS_MODE_MASK) == UNITS_DECIMAL) {
		base = 1000;
		mult = 1000;
		suffix = unit_suffix_decimal;
	}

	/* Unknown mode */
	if (!base) {
		fprintf(stderr, "INTERNAL ERROR: unknown unit base, mode %d\n",
				unit_mode);
		assert(0);
		return -1;
	}

	num_divs = 0;
	last_size = size;
	switch (unit_mode & UNITS_MODE_MASK) {
	case UNITS_TBYTES: base *= mult; num_divs++;
	case UNITS_GBYTES: base *= mult; num_divs++;
	case UNITS_MBYTES: base *= mult; num_divs++;
	case UNITS_KBYTES: num_divs++;
			   break;
	case UNITS_BYTES:
			   base = 1;
			   num_divs = 0;
			   break;
	default:
		while (size >= mult) {
			last_size = size;
			size /= mult;
			num_divs++;
		}
		/*
		 * If the value is smaller than base, we didn't do any
		 * division, in that case, base should be 1, not original
		 * base, or the unit will be wrong
		 */
		if (num_divs == 0)
			base = 1;
	}

	if (num_divs >= ARRAY_SIZE(unit_suffix_binary)) {
		str[0] = '\0';
		printf("INTERNAL ERROR: unsupported unit suffix, index %d\n",
				num_divs);
		assert(0);
		return -1;
	}
	fraction = (float)last_size / base;

	return snprintf(str, str_size, "%.2f%s", fraction, suffix[num_divs]);
}

/*
 * __strncpy_null - strncpy with null termination
 * @dest:	the target array
 * @src:	the source string
 * @n:		maximum bytes to copy (size of *dest)
 *
 * Like strncpy, but ensures destination is null-terminated.
 *
 * Copies the string pointed to by src, including the terminating null
 * byte ('\0'), to the buffer pointed to by dest, up to a maximum
 * of n bytes.  Then ensure that dest is null-terminated.
 */
char *__strncpy_null(char *dest, const char *src, size_t n)
{
	strncpy(dest, src, n);
	if (n > 0)
		dest[n - 1] = '\0';
	return dest;
}

/*
 * Checks to make sure that the label matches our requirements.
 * Returns:
       0    if everything is safe and usable
      -1    if the label is too long
 */
static int check_label(const char *input)
{
       int len = strlen(input);

       if (len > BTRFS_LABEL_SIZE - 1) {
		error("label %s is too long (max %d)", input,
				BTRFS_LABEL_SIZE - 1);
               return -1;
       }

       return 0;
}

static int set_label_unmounted(const char *dev, const char *label)
{
	struct btrfs_trans_handle *trans;
	struct btrfs_root *root;
	int ret;

	ret = check_mounted(dev);
	if (ret < 0) {
	       error("checking mount status of %s failed: %d", dev, ret);
	       return -1;
	}
	if (ret > 0) {
		error("device %s is mounted, use mount point", dev);
		return -1;
	}

	/* Open the super_block at the default location
	 * and as read-write.
	 */
	root = open_ctree(dev, 0, OPEN_CTREE_WRITES);
	if (!root) /* errors are printed by open_ctree() */
		return -1;

	trans = btrfs_start_transaction(root, 1);
	__strncpy_null(root->fs_info->super_copy->label, label, BTRFS_LABEL_SIZE - 1);

	btrfs_commit_transaction(trans, root);

	/* Now we close it since we are done. */
	close_ctree(root);
	return 0;
}

static int set_label_mounted(const char *mount_path, const char *labelp)
{
	int fd;
	char label[BTRFS_LABEL_SIZE];

	fd = open(mount_path, O_RDONLY | O_NOATIME);
	if (fd < 0) {
		error("unable to access %s: %s", mount_path, strerror(errno));
		return -1;
	}

	memset(label, 0, sizeof(label));
	__strncpy_null(label, labelp, BTRFS_LABEL_SIZE - 1);
	if (ioctl(fd, BTRFS_IOC_SET_FSLABEL, label) < 0) {
		error("unable to set label of %s: %s", mount_path,
				strerror(errno));
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}

int get_label_unmounted(const char *dev, char *label)
{
	struct btrfs_root *root;
	int ret;

	ret = check_mounted(dev);
	if (ret < 0) {
	       error("checking mount status of %s failed: %d", dev, ret);
	       return -1;
	}

	/* Open the super_block at the default location
	 * and as read-only.
	 */
	root = open_ctree(dev, 0, 0);
	if(!root)
		return -1;

	__strncpy_null(label, root->fs_info->super_copy->label,
			BTRFS_LABEL_SIZE - 1);

	/* Now we close it since we are done. */
	close_ctree(root);
	return 0;
}

/*
 * If a partition is mounted, try to get the filesystem label via its
 * mounted path rather than device.  Return the corresponding error
 * the user specified the device path.
 */
int get_label_mounted(const char *mount_path, char *labelp)
{
	char label[BTRFS_LABEL_SIZE];
	int fd;
	int ret;

	fd = open(mount_path, O_RDONLY | O_NOATIME);
	if (fd < 0) {
		error("unable to access %s: %s", mount_path, strerror(errno));
		return -1;
	}

	memset(label, '\0', sizeof(label));
	ret = ioctl(fd, BTRFS_IOC_GET_FSLABEL, label);
	if (ret < 0) {
		if (errno != ENOTTY)
			error("unable to get label of %s: %s", mount_path,
					strerror(errno));
		ret = -errno;
		close(fd);
		return ret;
	}

	__strncpy_null(labelp, label, BTRFS_LABEL_SIZE - 1);
	close(fd);
	return 0;
}

int get_label(const char *btrfs_dev, char *label)
{
	int ret;

	ret = is_existing_blk_or_reg_file(btrfs_dev);
	if (!ret)
		ret = get_label_mounted(btrfs_dev, label);
	else if (ret > 0)
		ret = get_label_unmounted(btrfs_dev, label);

	return ret;
}

int set_label(const char *btrfs_dev, const char *label)
{
	int ret;

	if (check_label(label))
		return -1;

	ret = is_existing_blk_or_reg_file(btrfs_dev);
	if (!ret)
		ret = set_label_mounted(btrfs_dev, label);
	else if (ret > 0)
		ret = set_label_unmounted(btrfs_dev, label);

	return ret;
}

/*
 * A not-so-good version fls64. No fascinating optimization since
 * no one except parse_size use it
 */
static int fls64(u64 x)
{
	int i;

	for (i = 0; i <64; i++)
		if (x << i & (1ULL << 63))
			return 64 - i;
	return 64 - i;
}

u64 parse_size(char *s)
{
	char c;
	char *endptr;
	u64 mult = 1;
	u64 ret;

	if (!s) {
		error("size value is empty");
		exit(1);
	}
	if (s[0] == '-') {
		error("size value '%s' is less equal than 0", s);
		exit(1);
	}
	ret = strtoull(s, &endptr, 10);
	if (endptr == s) {
		error("size value '%s' is invalid", s);
		exit(1);
	}
	if (endptr[0] && endptr[1]) {
		error("illegal suffix contains character '%c' in wrong position",
			endptr[1]);
		exit(1);
	}
	/*
	 * strtoll returns LLONG_MAX when overflow, if this happens,
	 * need to call strtoull to get the real size
	 */
	if (errno == ERANGE && ret == ULLONG_MAX) {
		error("size value '%s' is too large for u64", s);
		exit(1);
	}
	if (endptr[0]) {
		c = tolower(endptr[0]);
		switch (c) {
		case 'e':
			mult *= 1024;
			/* fallthrough */
		case 'p':
			mult *= 1024;
			/* fallthrough */
		case 't':
			mult *= 1024;
			/* fallthrough */
		case 'g':
			mult *= 1024;
			/* fallthrough */
		case 'm':
			mult *= 1024;
			/* fallthrough */
		case 'k':
			mult *= 1024;
			/* fallthrough */
		case 'b':
			break;
		default:
			error("unknown size descriptor '%c'", c);
			exit(1);
		}
	}
	/* Check whether ret * mult overflow */
	if (fls64(ret) + fls64(mult) - 1 > 64) {
		error("size value '%s' is too large for u64", s);
		exit(1);
	}
	ret *= mult;
	return ret;
}

u64 parse_qgroupid(const char *p)
{
	char *s = strchr(p, '/');
	const char *ptr_src_end = p + strlen(p);
	char *ptr_parse_end = NULL;
	u64 level;
	u64 id;
	int fd;
	int ret = 0;

	if (p[0] == '/')
		goto path;

	/* Numeric format like '0/257' is the primary case */
	if (!s) {
		id = strtoull(p, &ptr_parse_end, 10);
		if (ptr_parse_end != ptr_src_end)
			goto path;
		return id;
	}
	level = strtoull(p, &ptr_parse_end, 10);
	if (ptr_parse_end != s)
		goto path;

	id = strtoull(s + 1, &ptr_parse_end, 10);
	if (ptr_parse_end != ptr_src_end)
		goto  path;

	return (level << BTRFS_QGROUP_LEVEL_SHIFT) | id;

path:
	/* Path format like subv at 'my_subvol' is the fallback case */
	ret = test_issubvolume(p);
	if (ret < 0 || !ret)
		goto err;
	fd = open(p, O_RDONLY);
	if (fd < 0)
		goto err;
	ret = lookup_path_rootid(fd, &id);
	if (ret)
		error("failed to lookup root id: %s", strerror(-ret));
	close(fd);
	if (ret < 0)
		goto err;
	return id;

err:
	error("invalid qgroupid or subvolume path: %s", p);
	exit(-1);
}

int open_file_or_dir3(const char *fname, DIR **dirstream, int open_flags)
{
	int ret;
	struct stat st;
	int fd;

	ret = stat(fname, &st);
	if (ret < 0) {
		return -1;
	}
	if (S_ISDIR(st.st_mode)) {
		*dirstream = opendir(fname);
		if (!*dirstream)
			return -1;
		fd = dirfd(*dirstream);
	} else if (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)) {
		fd = open(fname, open_flags);
	} else {
		/*
		 * we set this on purpose, in case the caller output
		 * strerror(errno) as success
		 */
		errno = EINVAL;
		return -1;
	}
	if (fd < 0) {
		fd = -1;
		if (*dirstream) {
			closedir(*dirstream);
			*dirstream = NULL;
		}
	}
	return fd;
}

int open_file_or_dir(const char *fname, DIR **dirstream)
{
	return open_file_or_dir3(fname, dirstream, O_RDWR);
}

void close_file_or_dir(int fd, DIR *dirstream)
{
	if (dirstream)
		closedir(dirstream);
	else if (fd >= 0)
		close(fd);
}

int get_device_info(int fd, u64 devid,
		struct btrfs_ioctl_dev_info_args *di_args)
{
	int ret;

	di_args->devid = devid;
	memset(&di_args->uuid, '\0', sizeof(di_args->uuid));

	ret = ioctl(fd, BTRFS_IOC_DEV_INFO, di_args);
	return ret < 0 ? -errno : 0;
}

static u64 find_max_device_id(struct btrfs_ioctl_search_args *search_args,
			      int nr_items)
{
	struct btrfs_dev_item *dev_item;
	char *buf = search_args->buf;

	buf += (nr_items - 1) * (sizeof(struct btrfs_ioctl_search_header)
				       + sizeof(struct btrfs_dev_item));
	buf += sizeof(struct btrfs_ioctl_search_header);

	dev_item = (struct btrfs_dev_item *)buf;

	return btrfs_stack_device_id(dev_item);
}

static int search_chunk_tree_for_fs_info(int fd,
				struct btrfs_ioctl_fs_info_args *fi_args)
{
	int ret;
	int max_items;
	u64 start_devid = 1;
	struct btrfs_ioctl_search_args search_args;
	struct btrfs_ioctl_search_key *search_key = &search_args.key;

	fi_args->num_devices = 0;

	max_items = BTRFS_SEARCH_ARGS_BUFSIZE
	       / (sizeof(struct btrfs_ioctl_search_header)
			       + sizeof(struct btrfs_dev_item));

	search_key->tree_id = BTRFS_CHUNK_TREE_OBJECTID;
	search_key->min_objectid = BTRFS_DEV_ITEMS_OBJECTID;
	search_key->max_objectid = BTRFS_DEV_ITEMS_OBJECTID;
	search_key->min_type = BTRFS_DEV_ITEM_KEY;
	search_key->max_type = BTRFS_DEV_ITEM_KEY;
	search_key->min_transid = 0;
	search_key->max_transid = (u64)-1;
	search_key->nr_items = max_items;
	search_key->max_offset = (u64)-1;

again:
	search_key->min_offset = start_devid;

	ret = ioctl(fd, BTRFS_IOC_TREE_SEARCH, &search_args);
	if (ret < 0)
		return -errno;

	fi_args->num_devices += (u64)search_key->nr_items;

	if (search_key->nr_items == max_items) {
		start_devid = find_max_device_id(&search_args,
					search_key->nr_items) + 1;
		goto again;
	}

	/* get the lastest max_id to stay consistent with the num_devices */
	if (search_key->nr_items == 0)
		/*
		 * last tree_search returns an empty buf, use the devid of
		 * the last dev_item of the previous tree_search
		 */
		fi_args->max_id = start_devid - 1;
	else
		fi_args->max_id = find_max_device_id(&search_args,
						search_key->nr_items);

	return 0;
}

/*
 * For a given path, fill in the ioctl fs_ and info_ args.
 * If the path is a btrfs mountpoint, fill info for all devices.
 * If the path is a btrfs device, fill in only that device.
 *
 * The path provided must be either on a mounted btrfs fs,
 * or be a mounted btrfs device.
 *
 * Returns 0 on success, or a negative errno.
 */
int get_fs_info(const char *path, struct btrfs_ioctl_fs_info_args *fi_args,
		struct btrfs_ioctl_dev_info_args **di_ret)
{
	int fd = -1;
	int ret = 0;
	int ndevs = 0;
	u64 last_devid = 0;
	int replacing = 0;
	struct btrfs_fs_devices *fs_devices_mnt = NULL;
	struct btrfs_ioctl_dev_info_args *di_args;
	struct btrfs_ioctl_dev_info_args tmp;
	char mp[PATH_MAX];
	DIR *dirstream = NULL;

	memset(fi_args, 0, sizeof(*fi_args));

	if (is_block_device(path) == 1) {
		struct btrfs_super_block *disk_super;
		char buf[BTRFS_SUPER_INFO_SIZE];

		/* Ensure it's mounted, then set path to the mountpoint */
		fd = open(path, O_RDONLY);
		if (fd < 0) {
			ret = -errno;
			error("cannot open %s: %s", path, strerror(errno));
			goto out;
		}
		ret = check_mounted_where(fd, path, mp, sizeof(mp),
					  &fs_devices_mnt);
		if (!ret) {
			ret = -EINVAL;
			goto out;
		}
		if (ret < 0)
			goto out;
		path = mp;
		/* Only fill in this one device */
		fi_args->num_devices = 1;

		disk_super = (struct btrfs_super_block *)buf;
		ret = btrfs_read_dev_super(fd, disk_super,
					   BTRFS_SUPER_INFO_OFFSET, 0);
		if (ret < 0) {
			ret = -EIO;
			goto out;
		}
		last_devid = btrfs_stack_device_id(&disk_super->dev_item);
		fi_args->max_id = last_devid;

		memcpy(fi_args->fsid, fs_devices_mnt->fsid, BTRFS_FSID_SIZE);
		close(fd);
	}

	/* at this point path must not be for a block device */
	fd = open_file_or_dir(path, &dirstream);
	if (fd < 0) {
		ret = -errno;
		goto out;
	}

	/* fill in fi_args if not just a single device */
	if (fi_args->num_devices != 1) {
		ret = ioctl(fd, BTRFS_IOC_FS_INFO, fi_args);
		if (ret < 0) {
			ret = -errno;
			goto out;
		}

		/*
		 * The fs_args->num_devices does not include seed devices
		 */
		ret = search_chunk_tree_for_fs_info(fd, fi_args);
		if (ret)
			goto out;

		/*
		 * search_chunk_tree_for_fs_info() will lacks the devid 0
		 * so manual probe for it here.
		 */
		ret = get_device_info(fd, 0, &tmp);
		if (!ret) {
			fi_args->num_devices++;
			ndevs++;
			replacing = 1;
			if (last_devid == 0)
				last_devid++;
		}
	}

	if (!fi_args->num_devices)
		goto out;

	di_args = *di_ret = malloc((fi_args->num_devices) * sizeof(*di_args));
	if (!di_args) {
		ret = -errno;
		goto out;
	}

	if (replacing)
		memcpy(di_args, &tmp, sizeof(tmp));
	for (; last_devid <= fi_args->max_id; last_devid++) {
		ret = get_device_info(fd, last_devid, &di_args[ndevs]);
		if (ret == -ENODEV)
			continue;
		if (ret)
			goto out;
		ndevs++;
	}

	/*
	* only when the only dev we wanted to find is not there then
	* let any error be returned
	*/
	if (fi_args->num_devices != 1) {
		BUG_ON(ndevs == 0);
		ret = 0;
	}

out:
	close_file_or_dir(fd, dirstream);
	return ret;
}

#define isoctal(c)	(((c) & ~7) == '0')

static inline void translate(char *f, char *t)
{
	while (*f != '\0') {
		if (*f == '\\' &&
		    isoctal(f[1]) && isoctal(f[2]) && isoctal(f[3])) {
			*t++ = 64*(f[1] & 7) + 8*(f[2] & 7) + (f[3] & 7);
			f += 4;
		} else
			*t++ = *f++;
	}
	*t = '\0';
	return;
}

/*
 * Checks if the swap device.
 * Returns 1 if swap device, < 0 on error or 0 if not swap device.
 */
static int is_swap_device(const char *file)
{
	FILE	*f;
	struct stat	st_buf;
	dev_t	dev;
	ino_t	ino = 0;
	char	tmp[PATH_MAX];
	char	buf[PATH_MAX];
	char	*cp;
	int	ret = 0;

	if (stat(file, &st_buf) < 0)
		return -errno;
	if (S_ISBLK(st_buf.st_mode))
		dev = st_buf.st_rdev;
	else if (S_ISREG(st_buf.st_mode)) {
		dev = st_buf.st_dev;
		ino = st_buf.st_ino;
	} else
		return 0;

	if ((f = fopen("/proc/swaps", "r")) == NULL)
		return 0;

	/* skip the first line */
	if (fgets(tmp, sizeof(tmp), f) == NULL)
		goto out;

	while (fgets(tmp, sizeof(tmp), f) != NULL) {
		if ((cp = strchr(tmp, ' ')) != NULL)
			*cp = '\0';
		if ((cp = strchr(tmp, '\t')) != NULL)
			*cp = '\0';
		translate(tmp, buf);
		if (stat(buf, &st_buf) != 0)
			continue;
		if (S_ISBLK(st_buf.st_mode)) {
			if (dev == st_buf.st_rdev) {
				ret = 1;
				break;
			}
		} else if (S_ISREG(st_buf.st_mode)) {
			if (dev == st_buf.st_dev && ino == st_buf.st_ino) {
				ret = 1;
				break;
			}
		}
	}

out:
	fclose(f);

	return ret;
}

/*
 * Check for existing filesystem or partition table on device.
 * Returns:
 *	 1 for existing fs or partition
 *	 0 for nothing found
 *	-1 for internal error
 */
static int check_overwrite(const char *device)
{
	const char	*type;
	blkid_probe	pr = NULL;
	int		ret;
	blkid_loff_t	size;

	if (!device || !*device)
		return 0;

	ret = -1; /* will reset on success of all setup calls */

	pr = blkid_new_probe_from_filename(device);
	if (!pr)
		goto out;

	size = blkid_probe_get_size(pr);
	if (size < 0)
		goto out;

	/* nothing to overwrite on a 0-length device */
	if (size == 0) {
		ret = 0;
		goto out;
	}

	ret = blkid_probe_enable_partitions(pr, 1);
	if (ret < 0)
		goto out;

	ret = blkid_do_fullprobe(pr);
	if (ret < 0)
		goto out;

	/*
	 * Blkid returns 1 for nothing found and 0 when it finds a signature,
	 * but we want the exact opposite, so reverse the return value here.
	 *
	 * In addition print some useful diagnostics about what actually is
	 * on the device.
	 */
	if (ret) {
		ret = 0;
		goto out;
	}

	if (!blkid_probe_lookup_value(pr, "TYPE", &type, NULL)) {
		fprintf(stderr,
			"%s appears to contain an existing "
			"filesystem (%s).\n", device, type);
	} else if (!blkid_probe_lookup_value(pr, "PTTYPE", &type, NULL)) {
		fprintf(stderr,
			"%s appears to contain a partition "
			"table (%s).\n", device, type);
	} else {
		fprintf(stderr,
			"%s appears to contain something weird "
			"according to blkid\n", device);
	}
	ret = 1;

out:
	if (pr)
		blkid_free_probe(pr);
	if (ret == -1)
		fprintf(stderr,
			"probe of %s failed, cannot detect "
			  "existing filesystem.\n", device);
	return ret;
}

static int group_profile_devs_min(u64 flag)
{
	switch (flag & BTRFS_BLOCK_GROUP_PROFILE_MASK) {
	case 0: /* single */
	case BTRFS_BLOCK_GROUP_DUP:
		return 1;
	case BTRFS_BLOCK_GROUP_RAID0:
	case BTRFS_BLOCK_GROUP_RAID1:
	case BTRFS_BLOCK_GROUP_RAID5:
		return 2;
	case BTRFS_BLOCK_GROUP_RAID6:
		return 3;
	case BTRFS_BLOCK_GROUP_RAID10:
		return 4;
	default:
		return -1;
	}
}

int test_num_disk_vs_raid(u64 metadata_profile, u64 data_profile,
	u64 dev_cnt, int mixed, int ssd)
{
	u64 allowed = 0;
	u64 profile = metadata_profile | data_profile;

	switch (dev_cnt) {
	default:
	case 4:
		allowed |= BTRFS_BLOCK_GROUP_RAID10;
	case 3:
		allowed |= BTRFS_BLOCK_GROUP_RAID6;
	case 2:
		allowed |= BTRFS_BLOCK_GROUP_RAID0 | BTRFS_BLOCK_GROUP_RAID1 |
			BTRFS_BLOCK_GROUP_RAID5;
	case 1:
		allowed |= BTRFS_BLOCK_GROUP_DUP;
	}

	if (dev_cnt > 1 && profile & BTRFS_BLOCK_GROUP_DUP) {
		warning("DUP is not recommended on filesystem with multiple devices");
	}
	if (metadata_profile & ~allowed) {
		fprintf(stderr,
			"ERROR: unable to create FS with metadata profile %s "
			"(have %llu devices but %d devices are required)\n",
			btrfs_group_profile_str(metadata_profile), dev_cnt,
			group_profile_devs_min(metadata_profile));
		return 1;
	}
	if (data_profile & ~allowed) {
		fprintf(stderr,
			"ERROR: unable to create FS with data profile %s "
			"(have %llu devices but %d devices are required)\n",
			btrfs_group_profile_str(data_profile), dev_cnt,
			group_profile_devs_min(data_profile));
		return 1;
	}

	if (dev_cnt == 3 && profile & BTRFS_BLOCK_GROUP_RAID6) {
		warning("RAID6 is not recommended on filesystem with 3 devices only");
	}
	if (dev_cnt == 2 && profile & BTRFS_BLOCK_GROUP_RAID5) {
		warning("RAID5 is not recommended on filesystem with 2 devices only");
	}
	warning_on(!mixed && (data_profile & BTRFS_BLOCK_GROUP_DUP) && ssd,
		   "DUP may not actually lead to 2 copies on the device, see manual page");

	return 0;
}

int group_profile_max_safe_loss(u64 flags)
{
	switch (flags & BTRFS_BLOCK_GROUP_PROFILE_MASK) {
	case 0: /* single */
	case BTRFS_BLOCK_GROUP_DUP:
	case BTRFS_BLOCK_GROUP_RAID0:
		return 0;
	case BTRFS_BLOCK_GROUP_RAID1:
	case BTRFS_BLOCK_GROUP_RAID5:
	case BTRFS_BLOCK_GROUP_RAID10:
		return 1;
	case BTRFS_BLOCK_GROUP_RAID6:
		return 2;
	default:
		return -1;
	}
}

/*
 * Check if a device is suitable for btrfs
 * returns:
 *  1: something is wrong, an error is printed
 *  0: all is fine
 */
int test_dev_for_mkfs(const char *file, int force_overwrite)
{
	int ret, fd;
	struct stat st;

	ret = is_swap_device(file);
	if (ret < 0) {
		error("checking status of %s: %s", file, strerror(-ret));
		return 1;
	}
	if (ret == 1) {
		error("%s is a swap device", file);
		return 1;
	}
	if (!force_overwrite) {
		if (check_overwrite(file)) {
			error("use the -f option to force overwrite of %s",
					file);
			return 1;
		}
	}
	ret = check_mounted(file);
	if (ret < 0) {
		error("cannot check mount status of %s: %s", file,
				strerror(-ret));
		return 1;
	}
	if (ret == 1) {
		error("%s is mounted", file);
		return 1;
	}
	/* check if the device is busy */
	fd = open(file, O_RDWR|O_EXCL);
	if (fd < 0) {
		error("unable to open %s: %s", file, strerror(errno));
		return 1;
	}
	if (fstat(fd, &st)) {
		error("unable to stat %s: %s", file, strerror(errno));
		close(fd);
		return 1;
	}
	if (!S_ISBLK(st.st_mode)) {
		error("%s is not a block device", file);
		close(fd);
		return 1;
	}
	close(fd);
	return 0;
}

int btrfs_scan_devices(void)
{
	int fd = -1;
	int ret;
	u64 num_devices;
	struct btrfs_fs_devices *tmp_devices;
	blkid_dev_iterate iter = NULL;
	blkid_dev dev = NULL;
	blkid_cache cache = NULL;
	char path[PATH_MAX];

	if (btrfs_scan_done)
		return 0;

	if (blkid_get_cache(&cache, NULL) < 0) {
		error("blkid cache get failed");
		return 1;
	}
	blkid_probe_all(cache);
	iter = blkid_dev_iterate_begin(cache);
	blkid_dev_set_search(iter, "TYPE", "btrfs");
	while (blkid_dev_next(iter, &dev) == 0) {
		dev = blkid_verify(cache, dev);
		if (!dev)
			continue;
		/* if we are here its definitely a btrfs disk*/
		strncpy_null(path, blkid_dev_devname(dev));

		fd = open(path, O_RDONLY);
		if (fd < 0) {
			error("cannot open %s: %s", path, strerror(errno));
			continue;
		}
		ret = btrfs_scan_one_device(fd, path, &tmp_devices,
				&num_devices, BTRFS_SUPER_INFO_OFFSET,
				SBREAD_DEFAULT);
		if (ret) {
			error("cannot scan %s: %s", path, strerror(-ret));
			close (fd);
			continue;
		}

		close(fd);
	}
	blkid_dev_iterate_end(iter);
	blkid_put_cache(cache);

	btrfs_scan_done = 1;

	return 0;
}

int is_vol_small(const char *file)
{
	int fd = -1;
	int e;
	struct stat st;
	u64 size;

	fd = open(file, O_RDONLY);
	if (fd < 0)
		return -errno;
	if (fstat(fd, &st) < 0) {
		e = -errno;
		close(fd);
		return e;
	}
	size = btrfs_device_size(fd, &st);
	if (size == 0) {
		close(fd);
		return -1;
	}
	if (size < BTRFS_MKFS_SMALL_VOLUME_SIZE) {
		close(fd);
		return 1;
	} else {
		close(fd);
		return 0;
	}
}

/*
 * This reads a line from the stdin and only returns non-zero if the
 * first whitespace delimited token is a case insensitive match with yes
 * or y.
 */
int ask_user(const char *question)
{
	char buf[30] = {0,};
	char *saveptr = NULL;
	char *answer;

	printf("%s [y/N]: ", question);

	return fgets(buf, sizeof(buf) - 1, stdin) &&
	       (answer = strtok_r(buf, " \t\n\r", &saveptr)) &&
	       (!strcasecmp(answer, "yes") || !strcasecmp(answer, "y"));
}

/*
 * return 0 if a btrfs mount point is found
 * return 1 if a mount point is found but not btrfs
 * return <0 if something goes wrong
 */
int find_mount_root(const char *path, char **mount_root)
{
	FILE *mnttab;
	int fd;
	struct mntent *ent;
	int len;
	int ret;
	int not_btrfs = 1;
	int longest_matchlen = 0;
	char *longest_match = NULL;

	fd = open(path, O_RDONLY | O_NOATIME);
	if (fd < 0)
		return -errno;
	close(fd);

	mnttab = setmntent("/proc/self/mounts", "r");
	if (!mnttab)
		return -errno;

	while ((ent = getmntent(mnttab))) {
		len = strlen(ent->mnt_dir);
		if (strncmp(ent->mnt_dir, path, len) == 0) {
			/* match found and use the latest match */
			if (longest_matchlen <= len) {
				free(longest_match);
				longest_matchlen = len;
				longest_match = strdup(ent->mnt_dir);
				not_btrfs = strcmp(ent->mnt_type, "btrfs");
			}
		}
	}
	endmntent(mnttab);

	if (!longest_match)
		return -ENOENT;
	if (not_btrfs) {
		free(longest_match);
		return 1;
	}

	ret = 0;
	*mount_root = realpath(longest_match, NULL);
	if (!*mount_root)
		ret = -errno;

	free(longest_match);
	return ret;
}

int test_minimum_size(const char *file, u32 nodesize)
{
	int fd;
	struct stat statbuf;

	fd = open(file, O_RDONLY);
	if (fd < 0)
		return -errno;
	if (stat(file, &statbuf) < 0) {
		close(fd);
		return -errno;
	}
	if (btrfs_device_size(fd, &statbuf) < btrfs_min_dev_size(nodesize)) {
		close(fd);
		return 1;
	}
	close(fd);
	return 0;
}


/*
 * Test if path is a directory
 * Returns:
 *   0 - path exists but it is not a directory
 *   1 - path exists and it is a directory
 * < 0 - error
 */
int test_isdir(const char *path)
{
	struct stat st;
	int ret;

	ret = stat(path, &st);
	if (ret < 0)
		return -errno;

	return !!S_ISDIR(st.st_mode);
}

void units_set_mode(unsigned *units, unsigned mode)
{
	unsigned base = *units & UNITS_MODE_MASK;

	*units = base | mode;
}

void units_set_base(unsigned *units, unsigned base)
{
	unsigned mode = *units & ~UNITS_MODE_MASK;

	*units = base | mode;
}

int find_next_key(struct btrfs_path *path, struct btrfs_key *key)
{
	int level;

	for (level = 0; level < BTRFS_MAX_LEVEL; level++) {
		if (!path->nodes[level])
			break;
		if (path->slots[level] + 1 >=
		    btrfs_header_nritems(path->nodes[level]))
			continue;
		if (level == 0)
			btrfs_item_key_to_cpu(path->nodes[level], key,
					      path->slots[level] + 1);
		else
			btrfs_node_key_to_cpu(path->nodes[level], key,
					      path->slots[level] + 1);
		return 0;
	}
	return 1;
}

const char* btrfs_group_type_str(u64 flag)
{
	u64 mask = BTRFS_BLOCK_GROUP_TYPE_MASK |
		BTRFS_SPACE_INFO_GLOBAL_RSV;

	switch (flag & mask) {
	case BTRFS_BLOCK_GROUP_DATA:
		return "Data";
	case BTRFS_BLOCK_GROUP_SYSTEM:
		return "System";
	case BTRFS_BLOCK_GROUP_METADATA:
		return "Metadata";
	case BTRFS_BLOCK_GROUP_DATA|BTRFS_BLOCK_GROUP_METADATA:
		return "Data+Metadata";
	case BTRFS_SPACE_INFO_GLOBAL_RSV:
		return "GlobalReserve";
	default:
		return "unknown";
	}
}

const char* btrfs_group_profile_str(u64 flag)
{
	switch (flag & BTRFS_BLOCK_GROUP_PROFILE_MASK) {
	case 0:
		return "single";
	case BTRFS_BLOCK_GROUP_RAID0:
		return "RAID0";
	case BTRFS_BLOCK_GROUP_RAID1:
		return "RAID1";
	case BTRFS_BLOCK_GROUP_RAID5:
		return "RAID5";
	case BTRFS_BLOCK_GROUP_RAID6:
		return "RAID6";
	case BTRFS_BLOCK_GROUP_DUP:
		return "DUP";
	case BTRFS_BLOCK_GROUP_RAID10:
		return "RAID10";
	default:
		return "unknown";
	}
}

u64 disk_size(const char *path)
{
	struct statfs sfs;

	if (statfs(path, &sfs) < 0)
		return 0;
	else
		return sfs.f_bsize * sfs.f_blocks;
}

u64 get_partition_size(const char *dev)
{
	u64 result;
	int fd = open(dev, O_RDONLY);

	if (fd < 0)
		return 0;
	if (ioctl(fd, BLKGETSIZE64, &result) < 0) {
		close(fd);
		return 0;
	}
	close(fd);

	return result;
}

/*
 * Check if the BTRFS_IOC_TREE_SEARCH_V2 ioctl is supported on a given
 * filesystem, opened at fd
 */
int btrfs_tree_search2_ioctl_supported(int fd)
{
	struct btrfs_ioctl_search_args_v2 *args2;
	struct btrfs_ioctl_search_key *sk;
	int args2_size = 1024;
	char args2_buf[args2_size];
	int ret;

	args2 = (struct btrfs_ioctl_search_args_v2 *)args2_buf;
	sk = &(args2->key);

	/*
	 * Search for the extent tree item in the root tree.
	 */
	sk->tree_id = BTRFS_ROOT_TREE_OBJECTID;
	sk->min_objectid = BTRFS_EXTENT_TREE_OBJECTID;
	sk->max_objectid = BTRFS_EXTENT_TREE_OBJECTID;
	sk->min_type = BTRFS_ROOT_ITEM_KEY;
	sk->max_type = BTRFS_ROOT_ITEM_KEY;
	sk->min_offset = 0;
	sk->max_offset = (u64)-1;
	sk->min_transid = 0;
	sk->max_transid = (u64)-1;
	sk->nr_items = 1;
	args2->buf_size = args2_size - sizeof(struct btrfs_ioctl_search_args_v2);
	ret = ioctl(fd, BTRFS_IOC_TREE_SEARCH_V2, args2);
	if (ret == -EOPNOTSUPP)
		return 0;
	else if (ret == 0)
		return 1;
	return ret;
}

int btrfs_check_nodesize(u32 nodesize, u32 sectorsize, u64 features)
{
	if (nodesize < sectorsize) {
		error("illegal nodesize %u (smaller than %u)",
				nodesize, sectorsize);
		return -1;
	} else if (nodesize > BTRFS_MAX_METADATA_BLOCKSIZE) {
		error("illegal nodesize %u (larger than %u)",
			nodesize, BTRFS_MAX_METADATA_BLOCKSIZE);
		return -1;
	} else if (nodesize & (sectorsize - 1)) {
		error("illegal nodesize %u (not aligned to %u)",
			nodesize, sectorsize);
		return -1;
	} else if (features & BTRFS_FEATURE_INCOMPAT_MIXED_GROUPS &&
		   nodesize != sectorsize) {
		error("illegal nodesize %u (not equal to %u for mixed block group)",
			nodesize, sectorsize);
		return -1;
	}
	return 0;
}

/*
 * Copy a path argument from SRC to DEST and check the SRC length if it's at
 * most PATH_MAX and fits into DEST. DESTLEN is supposed to be exact size of
 * the buffer.
 * The destination buffer is zero terminated.
 * Return < 0 for error, 0 otherwise.
 */
int arg_copy_path(char *dest, const char *src, int destlen)
{
	size_t len = strlen(src);

	if (len >= PATH_MAX || len >= destlen)
		return -ENAMETOOLONG;

	__strncpy_null(dest, src, destlen);

	return 0;
}

unsigned int get_unit_mode_from_arg(int *argc, char *argv[], int df_mode)
{
	unsigned int unit_mode = UNITS_DEFAULT;
	int arg_i;
	int arg_end;

	for (arg_i = 0; arg_i < *argc; arg_i++) {
		if (!strcmp(argv[arg_i], "--"))
			break;

		if (!strcmp(argv[arg_i], "--raw")) {
			unit_mode = UNITS_RAW;
			argv[arg_i] = NULL;
			continue;
		}
		if (!strcmp(argv[arg_i], "--human-readable")) {
			unit_mode = UNITS_HUMAN_BINARY;
			argv[arg_i] = NULL;
			continue;
		}

		if (!strcmp(argv[arg_i], "--iec")) {
			units_set_mode(&unit_mode, UNITS_BINARY);
			argv[arg_i] = NULL;
			continue;
		}
		if (!strcmp(argv[arg_i], "--si")) {
			units_set_mode(&unit_mode, UNITS_DECIMAL);
			argv[arg_i] = NULL;
			continue;
		}

		if (!strcmp(argv[arg_i], "--kbytes")) {
			units_set_base(&unit_mode, UNITS_KBYTES);
			argv[arg_i] = NULL;
			continue;
		}
		if (!strcmp(argv[arg_i], "--mbytes")) {
			units_set_base(&unit_mode, UNITS_MBYTES);
			argv[arg_i] = NULL;
			continue;
		}
		if (!strcmp(argv[arg_i], "--gbytes")) {
			units_set_base(&unit_mode, UNITS_GBYTES);
			argv[arg_i] = NULL;
			continue;
		}
		if (!strcmp(argv[arg_i], "--tbytes")) {
			units_set_base(&unit_mode, UNITS_TBYTES);
			argv[arg_i] = NULL;
			continue;
		}

		if (!df_mode)
			continue;

		if (!strcmp(argv[arg_i], "-b")) {
			unit_mode = UNITS_RAW;
			argv[arg_i] = NULL;
			continue;
		}
		if (!strcmp(argv[arg_i], "-h")) {
			unit_mode = UNITS_HUMAN_BINARY;
			argv[arg_i] = NULL;
			continue;
		}
		if (!strcmp(argv[arg_i], "-H")) {
			unit_mode = UNITS_HUMAN_DECIMAL;
			argv[arg_i] = NULL;
			continue;
		}
		if (!strcmp(argv[arg_i], "-k")) {
			units_set_base(&unit_mode, UNITS_KBYTES);
			argv[arg_i] = NULL;
			continue;
		}
		if (!strcmp(argv[arg_i], "-m")) {
			units_set_base(&unit_mode, UNITS_MBYTES);
			argv[arg_i] = NULL;
			continue;
		}
		if (!strcmp(argv[arg_i], "-g")) {
			units_set_base(&unit_mode, UNITS_GBYTES);
			argv[arg_i] = NULL;
			continue;
		}
		if (!strcmp(argv[arg_i], "-t")) {
			units_set_base(&unit_mode, UNITS_TBYTES);
			argv[arg_i] = NULL;
			continue;
		}
	}

	for (arg_i = 0, arg_end = 0; arg_i < *argc; arg_i++) {
		if (!argv[arg_i])
			continue;
		argv[arg_end] = argv[arg_i];
		arg_end++;
	}

	*argc = arg_end;

	return unit_mode;
}

int string_is_numerical(const char *str)
{
	if (!str)
		return 0;
	if (!(*str >= '0' && *str <= '9'))
		return 0;
	while (*str >= '0' && *str <= '9')
		str++;
	if (*str != '\0')
		return 0;
	return 1;
}

/*
 * Preprocess @argv with getopt_long to reorder options and consume the "--"
 * option separator.
 * Unknown short and long options are reported, optionally the @usage is printed
 * before exit.
 */
void clean_args_no_options(int argc, char *argv[], const char * const *usagestr)
{
	static const struct option long_options[] = {
		{NULL, 0, NULL, 0}
	};

	while (1) {
		int c = getopt_long(argc, argv, "", long_options, NULL);

		if (c < 0)
			break;

		switch (c) {
		default:
			if (usagestr)
				usage(usagestr);
		}
	}
}

/*
 * Same as clean_args_no_options but pass through arguments that could look
 * like short options. Eg. reisze which takes a negative resize argument like
 * '-123M' .
 *
 * This accepts only two forms:
 * - "-- option1 option2 ..."
 * - "option1 option2 ..."
 */
void clean_args_no_options_relaxed(int argc, char *argv[], const char * const *usagestr)
{
	if (argc <= 1)
		return;

	if (strcmp(argv[1], "--") == 0)
		optind = 2;
}

/* Subvolume helper functions */
/*
 * test if name is a correct subvolume name
 * this function return
 * 0-> name is not a correct subvolume name
 * 1-> name is a correct subvolume name
 */
int test_issubvolname(const char *name)
{
	return name[0] != '\0' && !strchr(name, '/') &&
		strcmp(name, ".") && strcmp(name, "..");
}

/*
 * Test if path is a subvolume
 * Returns:
 *   0 - path exists but it is not a subvolume
 *   1 - path exists and it is  a subvolume
 * < 0 - error
 */
int test_issubvolume(const char *path)
{
	struct stat	st;
	struct statfs stfs;
	int		res;

	res = stat(path, &st);
	if (res < 0)
		return -errno;

	if (st.st_ino != BTRFS_FIRST_FREE_OBJECTID || !S_ISDIR(st.st_mode))
		return 0;

	res = statfs(path, &stfs);
	if (res < 0)
		return -errno;

	return (int)stfs.f_type == BTRFS_SUPER_MAGIC;
}

const char *subvol_strip_mountpoint(const char *mnt, const char *full_path)
{
	int len = strlen(mnt);
	if (!len)
		return full_path;

	if (mnt[len - 1] != '/')
		len += 1;

	return full_path + len;
}

/*
 * Returns
 * <0: Std error
 * 0: All fine
 * 1: Error; and error info printed to the terminal. Fixme.
 * 2: If the fullpath is root tree instead of subvol tree
 */
int get_subvol_info(const char *fullpath, struct root_info *get_ri)
{
	u64 sv_id;
	int ret = 1;
	int fd = -1;
	int mntfd = -1;
	char *mnt = NULL;
	const char *svpath = NULL;
	DIR *dirstream1 = NULL;
	DIR *dirstream2 = NULL;

	ret = test_issubvolume(fullpath);
	if (ret < 0)
		return ret;
	if (!ret) {
		error("not a subvolume: %s", fullpath);
		return 1;
	}

	ret = find_mount_root(fullpath, &mnt);
	if (ret < 0)
		return ret;
	if (ret > 0) {
		error("%s doesn't belong to btrfs mount point", fullpath);
		return 1;
	}
	ret = 1;
	svpath = subvol_strip_mountpoint(mnt, fullpath);

	fd = btrfs_open_dir(fullpath, &dirstream1, 1);
	if (fd < 0)
		goto out;

	ret = btrfs_list_get_path_rootid(fd, &sv_id);
	if (ret)
		goto out;

	mntfd = btrfs_open_dir(mnt, &dirstream2, 1);
	if (mntfd < 0)
		goto out;

	memset(get_ri, 0, sizeof(*get_ri));
	get_ri->root_id = sv_id;

	if (sv_id == BTRFS_FS_TREE_OBJECTID)
		ret = btrfs_get_toplevel_subvol(mntfd, get_ri);
	else
		ret = btrfs_get_subvol(mntfd, get_ri);
	if (ret)
		error("can't find '%s': %d", svpath, ret);

out:
	close_file_or_dir(mntfd, dirstream2);
	close_file_or_dir(fd, dirstream1);
	free(mnt);

	return ret;
}

void init_rand_seed(u64 seed)
{
	int i;

	/* only use the last 48 bits */
	for (i = 0; i < 3; i++) {
		rand_seed[i] = (unsigned short)(seed ^ (unsigned short)(-1));
		seed >>= 16;
	}
	rand_seed_initlized = 1;
}

static void __init_seed(void)
{
	struct timeval tv;
	int ret;
	int fd;

	if(rand_seed_initlized)
		return;
	/* Use urandom as primary seed source. */
	fd = open("/dev/urandom", O_RDONLY);
	if (fd >= 0) {
		ret = read(fd, rand_seed, sizeof(rand_seed));
		close(fd);
		if (ret < sizeof(rand_seed))
			goto fallback;
	} else {
fallback:
		/* Use time and pid as fallback seed */
		warning("failed to read /dev/urandom, use time and pid as random seed");
		gettimeofday(&tv, 0);
		rand_seed[0] = getpid() ^ (tv.tv_sec & 0xFFFF);
		rand_seed[1] = getppid() ^ (tv.tv_usec & 0xFFFF);
		rand_seed[2] = (tv.tv_sec ^ tv.tv_usec) >> 16;
	}
	rand_seed_initlized = 1;
}

u32 rand_u32(void)
{
	__init_seed();
	/*
	 * Don't use nrand48, its range is [0,2^31) The highest bit will alwasy
	 * be 0.  Use jrand48 to include the highest bit.
	 */
	return (u32)jrand48(rand_seed);
}

unsigned int rand_range(unsigned int upper)
{
	__init_seed();
	/*
	 * Use the full 48bits to mod, which would be more uniformly
	 * distributed
	 */
	return (unsigned int)(jrand48(rand_seed) % upper);
}
