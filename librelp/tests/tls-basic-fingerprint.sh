#!/bin/bash
. ${srcdir}/test-framework.sh
startup_receiver -T -a "fingerprint" -x ${srcdir}/tls-certs/ca.pem -y ${srcdir}/tls-certs/cert.pem -z ${srcdir}/tls-certs/key.pem -P 'SHA1:5C:C6:62:D5:9D:25:9F:BC:F3:CB:61:FA:D2:B3:8B:61:88:D7:06:C3'

echo 'Send Message...'
./send -t 127.0.0.1 -p $TESTPORT -m "testmessage" -T -a "fingerprint" -x ${srcdir}/tls-certs/ca.pem -y ${srcdir}/tls-certs/cert.pem -z ${srcdir}/tls-certs/key.pem -P 'SHA1:5C:C6:62:D5:9D:25:9F:BC:F3:CB:61:FA:D2:B3:8B:61:88:D7:06:C3' $OPT_VERBOSE 1>>librelp.out.log 2>&1

stop_receiver
check_output "testmessage"
terminate
