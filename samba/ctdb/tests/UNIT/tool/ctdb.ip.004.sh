#!/bin/sh

. "${TEST_SCRIPTS_DIR}/unit.sh"

define_test "3 nodes, all ok, IP missing on node 0"

setup_ctdbd <<EOF
NODEMAP
0       192.168.20.41   0x0     CURRENT RECMASTER
1       192.168.20.42   0x0
2       192.168.20.43   0x0

IFACES
:Name:LinkStatus:References:
:eth2:1:2:
:eth1:1:4:

PUBLICIPS
10.0.0.31  0 0,1,2
10.0.0.32  1 0,1,2
10.0.0.33  2 1,2
EOF

required_result 0 <<EOF
Public IPs on node 0
10.0.0.31 0
10.0.0.32 1
EOF
simple_test
