#!/bin/sh

. "${TEST_SCRIPTS_DIR}/unit.sh"

define_test "all services available"

setup

ok_null

simple_test
