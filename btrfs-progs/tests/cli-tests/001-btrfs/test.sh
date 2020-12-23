#!/bin/bash
# test commands of btrfs

source $TOP/tests/common

check_prereq btrfs

# returns 1
run_mayfail $TOP/btrfs || true
run_check $TOP/btrfs version
run_check $TOP/btrfs version --
run_check $TOP/btrfs help
run_check $TOP/btrfs help --
run_check $TOP/btrfs --help
run_check $TOP/btrfs --help --full
