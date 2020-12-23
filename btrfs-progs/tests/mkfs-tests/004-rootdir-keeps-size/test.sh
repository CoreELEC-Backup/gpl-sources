#!/bin/bash
# make sure that mkfs.btrfs --rootsize does not change size of the image

source $TOP/tests/common

check_prereq mkfs.btrfs

prepare_test_dev

test_mkfs_with_size() {
	local size
	local imgsize
	local tmp

	size="$1"
	run_check truncate -s$size $TEST_DEV
	imgsize=$(run_check_stdout stat --format=%s $TEST_DEV)
	run_check $SUDO_HELPER $TOP/mkfs.btrfs -f \
		--rootdir $TOP/Documentation \
		$TEST_DEV
	tmp=$(run_check_stdout stat --format=%s $TEST_DEV)
	if ! [ "$imgsize" = "$tmp" ]; then
		_fail "image size changed from $imgsize to $tmp"
	fi
}

test_mkfs_with_size 128M
test_mkfs_with_size 256M
test_mkfs_with_size 512M
test_mkfs_with_size 1G
test_mkfs_with_size 2G
