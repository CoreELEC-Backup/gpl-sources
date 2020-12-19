#!/bin/bash

. ../../../prepare.inc.sh
. ../../../toolbox.inc.sh


# ---- do the actual testing ----

result=PASS
if [ $skip_install_required -eq 1 ]
then
    echo "++++ SKIPPING TEST" >$OUTPUTFILE
    marker "SKIP BECAUSE TEST REQUIRES FULL INSTALL (for /sbin/request-key)"
    toolbox_report_result $TEST PASS
    exit 0
else
    echo "++++ BEGINNING TEST" >$OUTPUTFILE
fi

set_gc_delay 10

# create a pair of keyrings to play in
marker "CREATE KEYRINGS"
create_keyring "sandbox" @s
expect_keyid keyringid

# check that we can't yet request a non-existent key
marker "CHECK REQUEST FAILS"
request_key --fail user lizard $keyringid
expect_error ENOKEY

# add a user key to the first keyring
marker "ADD USER KEY"
create_key user lizard gizzard $keyringid
expect_keyid keyid

# request the key
marker "REQUEST KEY"
request_key user lizard
expect_keyid keyid2 $keyid

# remove the key from the keyring
marker "DETACH KEY FROM KEYRING"
unlink_key $keyid $keyringid

# request a key from /sbin/request-key to the session keyring
marker "PIPED CALL OUT REQUEST KEY TO SESSION"
prequest_key_callout gizzard user debug:lizard
expect_keyid keyid

# should have appeared in the session keyring
marker "CHECK ATTACHMENT TO SESSION KEYRING"
list_keyring @s
expect_keyring_rlist rlist $keyid

# rerequesting should pick up that key again
marker "REDO PIPED CALL OUT REQUEST KEY TO SESSION"
prequest_key_callout gizzard user debug:lizard
expect_keyid keyid2 $keyid

# remove the key from the session
# - it was installed twice
#   - once by request_key's keyring arg
#   - once from the instantiation call
#   but it will only have one link
marker "DETACH KEY FROM SESSION"
unlink_key --wait $keyid @s
unlink_key --fail $keyid @s
expect_error ENOKEY

# request a key from /sbin/request-key to the keyring we made
marker "PIPED CALL OUT REQUEST KEY TO KEYRING"
prequest_key_callout gizzard user debug:lizard $keyringid
expect_keyid keyid

# should have appeared once each in the sandbox and session keyrings
marker "CHECK ATTACHMENT TO KEYRING"
list_keyring $keyringid
expect_keyring_rlist rlist $keyid

marker "CHECK ATTACHMENT TO SESSION"
list_keyring @s
expect_keyring_rlist rlist $keyid

# rerequesting should pick up that key again
marker "REDO PIPED CALL OUT REQUEST KEY TO KEYRING"
prequest_key_callout gizzard user debug:lizard $keyringid
expect_keyid keyid2 $keyid

# remove the key from the session
marker "DETACH KEY"
unlink_key $keyid $keyringid
unlink_key --wait $keyid @s
unlink_key --fail $keyid @s
expect_error ENOKEY

# remove the keyrings we added
marker "UNLINK KEYRINGS"
unlink_key $keyringid @s

set_gc_delay $orig_gc_delay

echo "++++ FINISHED TEST: $result" >>$OUTPUTFILE

# --- then report the results in the database ---
toolbox_report_result $TEST $result
