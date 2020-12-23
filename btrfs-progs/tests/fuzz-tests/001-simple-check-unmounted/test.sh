#!/bin/bash

# iterate over all fuzzed images and run 'btrfs check'

source $TOP/tests/common

setup_root_helper
check_prereq btrfs

# redefine the one provided by common
check_image() {
	local image

	image=$1
	run_mayfail $TOP/btrfs check "$image"
}

check_all_images $TOP/tests/fuzz-tests/images

exit 0
