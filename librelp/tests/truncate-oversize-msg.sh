#!/bin/bash
. ${srcdir}/test-framework.sh
startup_receiver -o truncate -m 144

echo 'Send Message...'
./send -t 127.0.0.1 -p $TESTPORT -m "testmessage" -d 154 1>>client.err.log 2>&1

stop_receiver
# ^-sign symbolizes the beginning of the message and $-sign the expected end.
check_output "^testmessage0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012$"
check_output "error.*frame too long" librelp.out.log
cat librelp.out.log
terminate
