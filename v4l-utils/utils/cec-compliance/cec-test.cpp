// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2016 Cisco Systems, Inc. and/or its affiliates. All rights reserved.
 */

#include <cstdlib>
#include <cstring>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <config.h>
#include <sstream>
#include <vector>
#include <map>

#include "cec-compliance.h"

#define test_case(name, tags, subtests) {name, tags, subtests, ARRAY_SIZE(subtests)}
#define test_case_ext(name, tags, subtests) {name, tags, subtests, subtests##_size}

struct remote_test {
	const char *name;
	const unsigned tags;
	struct remote_subtest *subtests;
	unsigned num_subtests;
};


/* System Information */

int system_info_polling(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = { };

	cec_msg_init(&msg, me, la);
	fail_on_test(doioctl(node, CEC_TRANSMIT, &msg));
	if (node->remote_la_mask & (1 << la)) {
		if (!cec_msg_status_is_ok(&msg)) {
			fail("Polling a valid remote LA failed\n");
			return FAIL_CRITICAL;
		}
	} else {
		if (cec_msg_status_is_ok(&msg)) {
			fail("Polling an invalid remote LA was successful\n");
			return FAIL_CRITICAL;
		}
		return OK_NOT_SUPPORTED;
	}

	return 0;
}

int system_info_phys_addr(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = { };

	cec_msg_init(&msg, me, la);
	cec_msg_give_physical_addr(&msg, true);
	if (!transmit_timeout(node, &msg) || timed_out_or_abort(&msg)) {
		fail_or_warn(node, "Give Physical Addr timed out\n");
		return node->in_standby ? 0 : FAIL_CRITICAL;
	}
	fail_on_test(node->remote[la].phys_addr != ((msg.msg[2] << 8) | msg.msg[3]));
	fail_on_test(node->remote[la].prim_type != msg.msg[4]);
	return 0;
}

int system_info_version(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_get_cec_version(&msg, true);
	if (!transmit_timeout(node, &msg) || timed_out(&msg))
		return fail_or_warn(node, "Get CEC Version timed out\n");
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;

	/* This needs to be kept in sync with newer CEC versions */
	fail_on_test(msg.msg[2] < CEC_OP_CEC_VERSION_1_3A ||
		     msg.msg[2] > CEC_OP_CEC_VERSION_2_0);
	fail_on_test(node->remote[la].cec_version != msg.msg[2]);

	return 0;
}

int system_info_get_menu_lang(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};
	char language[4];

	cec_msg_init(&msg, me, la);
	cec_msg_get_menu_language(&msg, true);
	if (!transmit_timeout(node, &msg) || timed_out(&msg))
		return fail_or_warn(node, "Get Menu Languages timed out\n");

	/* Devices other than TVs shall send Feature Abort [Unregcognized Opcode]
	   in reply to Get Menu Language. */
	fail_on_test(!is_tv(la, node->remote[la].prim_type) && !unrecognized_op(&msg));

	if (unrecognized_op(&msg)) {
		if (is_tv(la, node->remote[la].prim_type))
			warn("TV did not respond to Get Menu Language.\n");
		return OK_NOT_SUPPORTED;
	}
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg))
		return OK_PRESUMED;
	cec_ops_set_menu_language(&msg, language);
	fail_on_test(strcmp(node->remote[la].language, language));

	return 0;
}

static int system_info_set_menu_lang(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_set_menu_language(&msg, "eng");
	fail_on_test(!transmit_timeout(node, &msg));
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;

	return OK_PRESUMED;
}

int system_info_give_features(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = { };

	cec_msg_init(&msg, me, la);
	cec_msg_give_features(&msg, true);
	if (!transmit_timeout(node, &msg) || timed_out(&msg))
		return fail_or_warn(node, "Give Features timed out\n");
	if (unrecognized_op(&msg)) {
		if (node->remote[la].cec_version < CEC_OP_CEC_VERSION_2_0)
			return OK_NOT_SUPPORTED;
		fail_on_test_v2(node->remote[la].cec_version, true);
	}
	if (refused(&msg))
		return OK_REFUSED;
	if (node->remote[la].cec_version < CEC_OP_CEC_VERSION_2_0)
		info("Device has CEC Version < 2.0 but supports Give Features.\n");

	/* RC Profile and Device Features are assumed to be 1 byte. As of CEC 2.0 only
	   1 byte is used, but this might be extended in future versions. */
	__u8 cec_version, all_device_types;
	const __u8 *rc_profile, *dev_features;

	cec_ops_report_features(&msg, &cec_version, &all_device_types, &rc_profile, &dev_features);
	fail_on_test(rc_profile == NULL || dev_features == NULL);
	info("All Device Types: \t\t%s\n", cec_all_dev_types2s(all_device_types).c_str());
	info("RC Profile: \t%s", cec_rc_src_prof2s(*rc_profile, "").c_str());
	info("Device Features: \t%s", cec_dev_feat2s(*dev_features, "").c_str());

	if (!(cec_has_playback(1 << la) || cec_has_record(1 << la) || cec_has_tuner(1 << la)) &&
	    (*dev_features & CEC_OP_FEAT_DEV_HAS_SET_AUDIO_RATE)) {
		return fail("Only Playback, Recording or Tuner devices shall set the Set Audio Rate bit\n");
	}
	if (!(cec_has_playback(1 << la) || cec_has_record(1 << la)) &&
	    (*dev_features & CEC_OP_FEAT_DEV_HAS_DECK_CONTROL))
		return fail("Only Playback and Recording devices shall set the Supports Deck Control bit\n");
	if (!cec_has_tv(1 << la) && node->remote[la].has_rec_tv)
		return fail("Only TVs shall set the Record TV Screen bit\n");
	if (cec_has_playback(1 << la) && (*dev_features & CEC_OP_FEAT_DEV_SINK_HAS_ARC_TX))
		return fail("A Playback device cannot set the Sink Supports ARC Tx bit\n");
	if (cec_has_tv(1 << la) && (*dev_features & CEC_OP_FEAT_DEV_SOURCE_HAS_ARC_RX))
		return fail("A TV cannot set the Source Supports ARC Rx bit\n");

	fail_on_test(cec_version != node->remote[la].cec_version);
	fail_on_test(node->remote[la].rc_profile != *rc_profile);
	fail_on_test(node->remote[la].dev_features != *dev_features);
	fail_on_test(node->remote[la].all_device_types != all_device_types);
	return 0;
}

static struct remote_subtest system_info_subtests[] = {
	{ "Polling Message", CEC_LOG_ADDR_MASK_ALL, system_info_polling },
	{ "Give Physical Address", CEC_LOG_ADDR_MASK_ALL, system_info_phys_addr },
	{ "Give CEC Version", CEC_LOG_ADDR_MASK_ALL, system_info_version },
	{ "Get Menu Language", CEC_LOG_ADDR_MASK_ALL, system_info_get_menu_lang },
	{ "Set Menu Language", CEC_LOG_ADDR_MASK_ALL, system_info_set_menu_lang },
	{ "Give Device Features", CEC_LOG_ADDR_MASK_ALL, system_info_give_features },
};


/* Core behavior */

int core_unknown(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = { };
	const __u8 unknown_opcode = 0xfe;

	/* Unknown opcodes should be responded to with Feature Abort, with abort
	   reason Unknown Opcode.

	   For CEC 2.0 and before, 0xfe is an unused opcode. The test possibly
	   needs to be updated for future CEC versions. */
	cec_msg_init(&msg, me, la);
	msg.len = 2;
	msg.msg[1] = unknown_opcode;
	if (!transmit_timeout(node, &msg) || timed_out(&msg))
		return fail_or_warn(node, "Unknown Opcode timed out\n");
	fail_on_test(!cec_msg_status_is_abort(&msg));

	__u8 abort_msg, reason;

	cec_ops_feature_abort(&msg, &abort_msg, &reason);
	fail_on_test(reason != CEC_OP_ABORT_UNRECOGNIZED_OP);
	fail_on_test(abort_msg != 0xfe);

	/* Unknown opcodes that are broadcast should be ignored */
	cec_msg_init(&msg, me, CEC_LOG_ADDR_BROADCAST);
	msg.len = 2;
	msg.msg[1] = unknown_opcode;
	fail_on_test(!transmit_timeout(node, &msg));
	fail_on_test(!timed_out(&msg));

	return 0;
}

int core_abort(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};

	/* The Abort message should always be responded to with Feature Abort
	   (with any abort reason) */
	cec_msg_init(&msg, me, la);
	cec_msg_abort(&msg);
	if (!transmit_timeout(node, &msg) || timed_out(&msg))
		return fail_or_warn(node, "Abort timed out\n");
	fail_on_test(!cec_msg_status_is_abort(&msg));
	return 0;
}

static struct remote_subtest core_subtests[] = {
	{ "Feature aborts unknown messages", CEC_LOG_ADDR_MASK_ALL, core_unknown },
	{ "Feature aborts Abort message", CEC_LOG_ADDR_MASK_ALL, core_abort },
};


/* Vendor Specific Commands */

int vendor_specific_commands_id(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_give_device_vendor_id(&msg, true);
	if (!transmit(node, &msg))
		return fail_or_warn(node, "Give Device Vendor ID timed out\n");
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg))
		return OK_PRESUMED;
	fail_on_test(node->remote[la].vendor_id !=
		     (__u32)((msg.msg[2] << 16) | (msg.msg[3] << 8) | msg.msg[4]));

	return 0;
}

static struct remote_subtest vendor_specific_subtests[] = {
	{ "Give Device Vendor ID", CEC_LOG_ADDR_MASK_ALL, vendor_specific_commands_id },
};


/* Device OSD Transfer */

static int device_osd_transfer_set(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = { };

	cec_msg_init(&msg, me, la);
	cec_msg_set_osd_name(&msg, "Whatever");
	fail_on_test(!transmit_timeout(node, &msg));
	if (unrecognized_op(&msg)) {
		if (is_tv(la, node->remote[la].prim_type) &&
		    node->remote[la].cec_version >= CEC_OP_CEC_VERSION_2_0)
			warn("TV feature aborted Set OSD Name\n");
		return OK_NOT_SUPPORTED;
	}
	if (refused(&msg))
		return OK_REFUSED;

	return OK_PRESUMED;
}

int device_osd_transfer_give(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = { };

	/* Todo: CEC 2.0: devices with several logical addresses shall report
	   the same for each logical address. */
	cec_msg_init(&msg, me, la);
	cec_msg_give_osd_name(&msg, true);
	if (!transmit_timeout(node, &msg) || timed_out(&msg))
		return fail_or_warn(node, "Give OSD Name timed out\n");
	fail_on_test(!is_tv(la, node->remote[la].prim_type) && unrecognized_op(&msg));
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg))
		return OK_PRESUMED;
	char osd_name[15];
	cec_ops_set_osd_name(&msg, osd_name);
	fail_on_test(!osd_name[0]);
	fail_on_test(strcmp(node->remote[la].osd_name, osd_name));
	fail_on_test(msg.len != strlen(osd_name) + 2);

	return 0;
}

static struct remote_subtest device_osd_transfer_subtests[] = {
	{ "Set OSD Name", CEC_LOG_ADDR_MASK_ALL, device_osd_transfer_set },
	{ "Give OSD Name", CEC_LOG_ADDR_MASK_ALL, device_osd_transfer_give },
};


/* OSD Display */

static int osd_string_set_default(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = { };
	char osd[14];
	bool unsuitable = false;

	sprintf(osd, "Rept %x from %x", la, me);

	interactive_info(true, "You should see \"%s\" appear on the screen", osd);
	cec_msg_init(&msg, me, la);
	cec_msg_set_osd_string(&msg, CEC_OP_DISP_CTL_DEFAULT, osd);
	fail_on_test(!transmit_timeout(node, &msg));
	/* In CEC 2.0 it is mandatory for a TV to support this if it reports so
	   in its Device Features. */
	fail_on_test_v2(node->remote[la].cec_version,
			unrecognized_op(&msg) &&
			(node->remote[la].dev_features & CEC_OP_FEAT_DEV_HAS_SET_OSD_STRING));
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg)) {
		warn("The device is in an unsuitable state or cannot display the complete message.\n");
		unsuitable = true;
	}
	node->remote[la].has_osd = true;
	if (!interactive)
		return OK_PRESUMED;

	/* The CEC 1.4b CTS specifies that one should wait at least 20 seconds for the
	   string to be cleared on the remote device */
	interactive_info(true, "Waiting 20s for OSD string to be cleared on the remote device");
	sleep(20);
	fail_on_test(!unsuitable && interactive && !question("Did the string appear and then disappear?"));

	return 0;
}

static int osd_string_set_until_clear(struct node *node, unsigned me, unsigned la, bool interactive)
{
	if (!node->remote[la].has_osd)
		return NOTAPPLICABLE;

	struct cec_msg msg = { };
	char osd[14];
	bool unsuitable = false;

	strcpy(osd, "Appears 1 sec");
	// Make sure the string is the maximum possible length
	fail_on_test(strlen(osd) != 13);

	interactive_info(true, "You should see \"%s\" appear on the screen for approximately three seconds.", osd);
	cec_msg_init(&msg, me, la);
	cec_msg_set_osd_string(&msg, CEC_OP_DISP_CTL_UNTIL_CLEARED, osd);
	fail_on_test(!transmit(node, &msg));
	if (cec_msg_status_is_abort(&msg) && !unrecognized_op(&msg)) {
		warn("The device is in an unsuitable state or cannot display the complete message.\n");
		unsuitable = true;
	}
	sleep(3);

	cec_msg_init(&msg, me, la);
	cec_msg_set_osd_string(&msg, CEC_OP_DISP_CTL_CLEAR, "");
	fail_on_test(!transmit_timeout(node, &msg, 250));
	fail_on_test(cec_msg_status_is_abort(&msg));
	fail_on_test(!unsuitable && interactive && !question("Did the string appear?"));

	if (interactive)
		return 0;

	return OK_PRESUMED;
}

static int osd_string_invalid(struct node *node, unsigned me, unsigned la, bool interactive)
{
	if (!node->remote[la].has_osd)
		return NOTAPPLICABLE;

	struct cec_msg msg = { };

	/* Send Set OSD String with an Display Control operand. A Feature Abort is
	   expected in reply. */
	interactive_info(true, "You should observe no change on the on screen display");
	cec_msg_init(&msg, me, la);
	cec_msg_set_osd_string(&msg, 0xff, "");
	fail_on_test(!transmit_timeout(node, &msg));
	fail_on_test(timed_out(&msg));
	fail_on_test(!cec_msg_status_is_abort(&msg));
	fail_on_test(interactive && question("Did the display change?"));

	return 0;
}

static struct remote_subtest osd_string_subtests[] = {
	{ "Set OSD String with default timeout", CEC_LOG_ADDR_MASK_TV, osd_string_set_default },
	{ "Set OSD String with no timeout", CEC_LOG_ADDR_MASK_TV, osd_string_set_until_clear },
	{ "Set OSD String with invalid operand", CEC_LOG_ADDR_MASK_TV, osd_string_invalid },
};


/* Routing Control */

static int routing_control_inactive_source(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};
	int response;

	interactive_info(true, "Please make sure that the TV is currently viewing this source.");
	mode_set_follower(node);
	cec_msg_init(&msg, me, la);
	cec_msg_inactive_source(&msg, node->phys_addr);
	fail_on_test(!transmit(node, &msg));
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	// It may take a bit of time for the Inactive Source message to take
	// effect, so sleep a bit.
	response = util_receive(node, CEC_LOG_ADDR_TV, 3000, &msg,
				CEC_MSG_INACTIVE_SOURCE,
				CEC_MSG_ACTIVE_SOURCE, CEC_MSG_SET_STREAM_PATH);
	if (me == CEC_LOG_ADDR_TV) {
		// Inactive Source should be ignored by all other devices
		if (response >= 0)
			return fail("Unexpected reply to Inactive Source\n");
		fail_on_test(response >= 0);
	} else {
		if (response < 0)
			warn("Expected Active Source or Set Stream Path reply to Inactive Source\n");
		fail_on_test(interactive && !question("Did the TV switch away from or stop showing this source?"));
	}

	return 0;
}

static int routing_control_active_source(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};

	interactive_info(true, "Please switch the TV to another source.");
	cec_msg_init(&msg, me, la);
	cec_msg_active_source(&msg, node->phys_addr);
	fail_on_test(!transmit_timeout(node, &msg));
	fail_on_test(interactive && !question("Did the TV switch to this source?"));

	if (interactive)
		return 0;

	return OK_PRESUMED;
}

static int routing_control_req_active_source(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};

	/* We have now said that we are active source, so receiving a reply to
	   Request Active Source should fail the test. */
	cec_msg_init(&msg, me, la);
	cec_msg_request_active_source(&msg, true);
	fail_on_test(!transmit_timeout(node, &msg));
	fail_on_test(!timed_out(&msg));

	return 0;
}

static int routing_control_set_stream_path(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};
	__u16 phys_addr;

	/* Send Set Stream Path with the remote physical address. We expect the
	   source to eventually send Active Source. The timeout of long_timeout
	   seconds is necessary because the device might have to wake up from standby.

	   In CEC 2.0 it is mandatory for sources to send Active Source. */
	if (is_tv(la, node->remote[la].prim_type))
		interactive_info(true, "Please ensure that the device is in standby.");
	announce("Sending Set Stream Path and waiting for reply. This may take up to %llu s.", (long long)long_timeout);
	cec_msg_init(&msg, me, la);
	cec_msg_set_stream_path(&msg, node->remote[la].phys_addr);
	msg.reply = CEC_MSG_ACTIVE_SOURCE;
	fail_on_test(!transmit_timeout(node, &msg, long_timeout * 1000));
	if (timed_out(&msg) && is_tv(la, node->remote[la].prim_type))
		return OK_NOT_SUPPORTED;
	if (timed_out(&msg) && node->remote[la].cec_version < CEC_OP_CEC_VERSION_2_0) {
		warn("Device did not respond to Set Stream Path.\n");
		return OK_NOT_SUPPORTED;
	}
	fail_on_test_v2(node->remote[la].cec_version, timed_out(&msg));
	cec_ops_active_source(&msg, &phys_addr);
	fail_on_test(phys_addr != node->remote[la].phys_addr);
	if (is_tv(la, node->remote[la].prim_type))
		fail_on_test(interactive && !question("Did the device go out of standby?"));

	if (interactive || node->remote[la].cec_version >= CEC_OP_CEC_VERSION_2_0)
		return 0;

	return OK_PRESUMED;
}

static struct remote_subtest routing_control_subtests[] = {
	{ "Active Source", CEC_LOG_ADDR_MASK_TV, routing_control_active_source },
	{ "Request Active Source", CEC_LOG_ADDR_MASK_ALL, routing_control_req_active_source },
	{ "Inactive Source", CEC_LOG_ADDR_MASK_TV, routing_control_inactive_source },
	{ "Set Stream Path", CEC_LOG_ADDR_MASK_ALL, routing_control_set_stream_path },
};


/* Remote Control Passthrough */

static int rc_passthrough_user_ctrl_pressed(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};
	struct cec_op_ui_command rc_press;

	cec_msg_init(&msg, me, la);
	rc_press.ui_cmd = CEC_OP_UI_CMD_VOLUME_UP; // Volume up key (the key is not crucial here)
	cec_msg_user_control_pressed(&msg, &rc_press);
	fail_on_test(!transmit_timeout(node, &msg));
	/* Mandatory for all except devices which have taken logical address 15 */
	fail_on_test_v2(node->remote[la].cec_version,
			unrecognized_op(&msg) && !(cec_is_unregistered(1 << la)));
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;

	return OK_PRESUMED;
}

static int rc_passthrough_user_ctrl_released(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_user_control_released(&msg);
	fail_on_test(!transmit_timeout(node, &msg));
	fail_on_test_v2(node->remote[la].cec_version,
			cec_msg_status_is_abort(&msg) && !(la & CEC_LOG_ADDR_MASK_UNREGISTERED));
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	node->remote[la].has_remote_control_passthrough = true;

	return OK_PRESUMED;
}

static struct remote_subtest rc_passthrough_subtests[] = {
	{ "User Control Pressed", CEC_LOG_ADDR_MASK_ALL, rc_passthrough_user_ctrl_pressed },
	{ "User Control Released", CEC_LOG_ADDR_MASK_ALL, rc_passthrough_user_ctrl_released },
};


/* Device Menu Control */

/*
  TODO: These are very rudimentary tests which should be expanded.
 */

static int dev_menu_ctl_request(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_menu_request(&msg, true, CEC_OP_MENU_REQUEST_QUERY);
	fail_on_test(!transmit_timeout(node, &msg));
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg))
		return OK_PRESUMED;
	if (node->remote[la].cec_version >= CEC_OP_CEC_VERSION_2_0)
		warn("The Device Menu Control feature is deprecated in CEC 2.0\n");

	return 0;
}

static struct remote_subtest dev_menu_ctl_subtests[] = {
	{ "Menu Request", static_cast<__u16>(~CEC_LOG_ADDR_MASK_TV), dev_menu_ctl_request },
	{ "User Control Pressed", CEC_LOG_ADDR_MASK_ALL, rc_passthrough_user_ctrl_pressed },
	{ "User Control Released", CEC_LOG_ADDR_MASK_ALL, rc_passthrough_user_ctrl_released },
};


/* Deck Control */

/*
  TODO: These are very rudimentary tests which should be expanded.
 */

static int deck_ctl_give_status(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_give_deck_status(&msg, true, CEC_OP_STATUS_REQ_ONCE);
	fail_on_test(!transmit_timeout(node, &msg));
	fail_on_test(timed_out(&msg));
	if (is_playback_or_rec(la)) {
		fail_on_test_v2(node->remote[la].cec_version,
				node->remote[la].has_deck_ctl && cec_msg_status_is_abort(&msg));
		fail_on_test_v2(node->remote[la].cec_version,
				!node->remote[la].has_deck_ctl && !unrecognized_op(&msg));
	}
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg))
		return OK_PRESUMED;

	return 0;
}

static int deck_ctl_deck_status(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_deck_status(&msg, CEC_OP_DECK_INFO_STOP);
	fail_on_test(!transmit_timeout(node, &msg));
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg))
		return OK_PRESUMED;

	return 0;
}

static int deck_ctl_deck_ctl(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_deck_control(&msg, CEC_OP_DECK_CTL_MODE_STOP);
	fail_on_test(!transmit_timeout(node, &msg));
	if (is_playback_or_rec(la)) {
		fail_on_test_v2(node->remote[la].cec_version,
				node->remote[la].has_deck_ctl && unrecognized_op(&msg));
		fail_on_test_v2(node->remote[la].cec_version,
				!node->remote[la].has_deck_ctl && !unrecognized_op(&msg));
	}
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg))
		return OK_PRESUMED;

	return OK_PRESUMED;
}

static int deck_ctl_play(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_play(&msg, CEC_OP_PLAY_MODE_PLAY_STILL);
	fail_on_test(!transmit_timeout(node, &msg));
	if (is_playback_or_rec(la)) {
		fail_on_test_v2(node->remote[la].cec_version,
				node->remote[la].has_deck_ctl && unrecognized_op(&msg));
		fail_on_test_v2(node->remote[la].cec_version,
				!node->remote[la].has_deck_ctl && !unrecognized_op(&msg));
	}
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg))
		return OK_PRESUMED;

	return OK_PRESUMED;
}

static struct remote_subtest deck_ctl_subtests[] = {
	{ "Give Deck Status",
	  CEC_LOG_ADDR_MASK_PLAYBACK | CEC_LOG_ADDR_MASK_RECORD,
	  deck_ctl_give_status },
	{ "Deck Status",
	  CEC_LOG_ADDR_MASK_ALL,
	  deck_ctl_deck_status },
	{ "Deck Control",
	  CEC_LOG_ADDR_MASK_PLAYBACK | CEC_LOG_ADDR_MASK_RECORD,
	  deck_ctl_deck_ctl },
	{ "Play",
	  CEC_LOG_ADDR_MASK_PLAYBACK | CEC_LOG_ADDR_MASK_RECORD,
	  deck_ctl_play },
};


/* Tuner Control */

static const char *bcast_type2s(__u8 bcast_type)
{
	switch (bcast_type) {
	case CEC_OP_ANA_BCAST_TYPE_CABLE:
		return "Cable";
	case CEC_OP_ANA_BCAST_TYPE_SATELLITE:
		return "Satellite";
	case CEC_OP_ANA_BCAST_TYPE_TERRESTRIAL:
		return "Terrestrial";
	default:
		return "Future use";
	}
}

static int log_tuner_service(const struct cec_op_tuner_device_info &info,
			     const char *prefix = "")
{
	printf("\t\t%s", prefix);

	if (info.is_analog) {
		double freq_mhz = (info.analog.ana_freq * 625) / 10000.0;

		printf("Analog Channel %.2f MHz (%s, %s)\n", freq_mhz,
		       bcast_system2s(info.analog.bcast_system),
		       bcast_type2s(info.analog.ana_bcast_type));

		switch (info.analog.bcast_system) {
		case CEC_OP_BCAST_SYSTEM_PAL_BG:
		case CEC_OP_BCAST_SYSTEM_SECAM_LQ:
		case CEC_OP_BCAST_SYSTEM_PAL_M:
		case CEC_OP_BCAST_SYSTEM_NTSC_M:
		case CEC_OP_BCAST_SYSTEM_PAL_I:
		case CEC_OP_BCAST_SYSTEM_SECAM_DK:
		case CEC_OP_BCAST_SYSTEM_SECAM_BG:
		case CEC_OP_BCAST_SYSTEM_SECAM_L:
		case CEC_OP_BCAST_SYSTEM_PAL_DK:
			break;
		default:
			return fail("invalid analog bcast_system %u", info.analog.bcast_system);
		}
		if (info.analog.ana_bcast_type > CEC_OP_ANA_BCAST_TYPE_TERRESTRIAL)
			return fail("invalid analog bcast_type %u\n", info.analog.ana_bcast_type);
		fail_on_test(!info.analog.ana_freq);
		return 0;
	}

	__u8 system = info.digital.dig_bcast_system;

	printf("%s Channel ", dig_bcast_system2s(system));
	if (info.digital.service_id_method) {
		__u16 major = info.digital.channel.major;
		__u16 minor = info.digital.channel.minor;

		switch (info.digital.channel.channel_number_fmt) {
		case CEC_OP_CHANNEL_NUMBER_FMT_2_PART:
			printf("%u.%u\n", major, minor);
			break;
		case CEC_OP_CHANNEL_NUMBER_FMT_1_PART:
			printf("%u\n", minor);
			break;
		default:
			printf("%u.%u\n", major, minor);
			return fail("invalid service ID method\n");
		}
		return 0;
	}


	switch (system) {
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_GEN:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_BS:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_CS:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_T: {
		__u16 tsid = info.digital.arib.transport_id;
		__u16 sid = info.digital.arib.service_id;
		__u16 onid = info.digital.arib.orig_network_id;

		printf("TSID: %u, SID: %u, ONID: %u\n", tsid, sid, onid);
		break;
	}
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_GEN:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_SAT:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_CABLE:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_T: {
		__u16 tsid = info.digital.atsc.transport_id;
		__u16 pn = info.digital.atsc.program_number;

		printf("TSID: %u, Program Number: %u\n", tsid, pn);
		break;
	}
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_GEN:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_S:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_S2:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_C:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_T: {
		__u16 tsid = info.digital.dvb.transport_id;
		__u16 sid = info.digital.dvb.service_id;
		__u16 onid = info.digital.dvb.orig_network_id;

		printf("TSID: %u, SID: %u, ONID: %u\n", tsid, sid, onid);
		break;
	}
	default:
		break;
	}

	switch (system) {
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_GEN:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_GEN:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_GEN:
		warn_once("generic digital broadcast systems should not be used");
		break;
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_BS:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_CS:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ARIB_T:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_CABLE:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_SAT:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_ATSC_T:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_C:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_S:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_S2:
	case CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_T:
		break;
	default:
		return fail("invalid digital broadcast system %u", system);
	}

	if (info.digital.service_id_method > CEC_OP_SERVICE_ID_METHOD_BY_CHANNEL)
		return fail("invalid service ID method %u\n", info.digital.service_id_method);

	return 0;
}

static int tuner_ctl_test(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};
	struct cec_op_tuner_device_info info = {};
	std::vector<struct cec_op_tuner_device_info> info_vec;
	bool has_tuner = (1 << la) & (CEC_LOG_ADDR_MASK_TV | CEC_LOG_ADDR_MASK_TUNER);
	int ret;

	cec_msg_init(&msg, me, la);
	cec_msg_give_tuner_device_status(&msg, true, CEC_OP_STATUS_REQ_ONCE);
	fail_on_test(!transmit_timeout(node, &msg));
	fail_on_test(!has_tuner && !timed_out_or_abort(&msg));
	if (!has_tuner)
		return OK_NOT_SUPPORTED;
	if (timed_out(&msg) || unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (cec_msg_status_is_abort(&msg))
		return OK_REFUSED;

	printf("\t    Start Channel Scan\n");
	cec_ops_tuner_device_status(&msg, &info);
	info_vec.push_back(info);
	ret = log_tuner_service(info);
	if (ret)
		return ret;

	while (true) {
		cec_msg_init(&msg, me, la);
		cec_msg_tuner_step_increment(&msg);
		fail_on_test(!transmit(node, &msg));
		fail_on_test(cec_msg_status_is_abort(&msg));
		if (cec_msg_status_is_abort(&msg)) {
			fail_on_test(abort_reason(&msg) == CEC_OP_ABORT_UNRECOGNIZED_OP);
			if (abort_reason(&msg) == CEC_OP_ABORT_REFUSED) {
				warn("Tuner step increment does not wrap.\n");
				break;
			}

			warn("Tuner at end of service list did not receive feature abort refused.\n");
			break;
		}
		cec_msg_init(&msg, me, la);
		cec_msg_give_tuner_device_status(&msg, true, CEC_OP_STATUS_REQ_ONCE);
		fail_on_test(!transmit_timeout(node, &msg));
		fail_on_test(timed_out_or_abort(&msg));
		memset(&info, 0, sizeof(info));
		cec_ops_tuner_device_status(&msg, &info);
		if (!memcmp(&info, &info_vec[0], sizeof(info)))
			break;
		ret = log_tuner_service(info);
		if (ret)
			return ret;
		info_vec.push_back(info);
	}
	printf("\t    Finished Channel Scan\n");

	printf("\t    Start Channel Test\n");
	for (std::vector<struct cec_op_tuner_device_info>::iterator iter = info_vec.begin();
			iter != info_vec.end(); iter++) {
		cec_msg_init(&msg, me, la);
		log_tuner_service(*iter, "Select ");
		if (iter->is_analog)
			cec_msg_select_analogue_service(&msg, iter->analog.ana_bcast_type,
				iter->analog.ana_freq, iter->analog.bcast_system);
		else
			cec_msg_select_digital_service(&msg, &iter->digital);
		fail_on_test(!transmit(node, &msg));
		fail_on_test(cec_msg_status_is_abort(&msg));
		cec_msg_init(&msg, me, la);
		cec_msg_give_tuner_device_status(&msg, true, CEC_OP_STATUS_REQ_ONCE);
		fail_on_test(!transmit_timeout(node, &msg));
		fail_on_test(timed_out_or_abort(&msg));
		memset(&info, 0, sizeof(info));
		cec_ops_tuner_device_status(&msg, &info);
		if (memcmp(&info, &(*iter), sizeof(info))) {
			log_tuner_service(info);
			log_tuner_service(*iter);
		}
		fail_on_test(memcmp(&info, &(*iter), sizeof(info)));
	}
	printf("\t    Finished Channel Test\n");

	cec_msg_init(&msg, me, la);
	cec_msg_select_analogue_service(&msg, 3, 16000, 9);
	printf("\t\tSelect invalid analog channel\n");
	fail_on_test(!transmit_timeout(node, &msg));
	fail_on_test(!cec_msg_status_is_abort(&msg));
	fail_on_test(abort_reason(&msg) != CEC_OP_ABORT_INVALID_OP);
	cec_msg_init(&msg, me, la);
	info.digital.service_id_method = CEC_OP_SERVICE_ID_METHOD_BY_DIG_ID;
	info.digital.dig_bcast_system = CEC_OP_DIG_SERVICE_BCAST_SYSTEM_DVB_S2;
	info.digital.dvb.transport_id = 0;
	info.digital.dvb.service_id = 0;
	info.digital.dvb.orig_network_id = 0;
	cec_msg_select_digital_service(&msg, &info.digital);
	printf("\t\tSelect invalid digital channel\n");
	fail_on_test(!transmit_timeout(node, &msg));
	fail_on_test(!cec_msg_status_is_abort(&msg));
	fail_on_test(abort_reason(&msg) != CEC_OP_ABORT_INVALID_OP);

	return 0;
}

static struct remote_subtest tuner_ctl_subtests[] = {
	{ "Tuner Control", CEC_LOG_ADDR_MASK_TUNER | CEC_LOG_ADDR_MASK_TV, tuner_ctl_test },
};


/* One Touch Record */

/*
  TODO: These are very rudimentary tests which should be expanded.

  - The HDMI CEC 1.4b spec details that Standby shall not be acted upon while the
    device is recording, but it should remember that it received Standby.
 */

static int one_touch_rec_tv_screen(struct node *node, unsigned me, unsigned la, bool interactive)
{
	/*
	  TODO:
	  - Page 36 in HDMI CEC 1.4b spec lists additional behaviors that should be
	    checked for.
	  - The TV should ignore this message when received from other LA than Recording or
	    Reserved.
	 */
	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_record_tv_screen(&msg, true);
	fail_on_test(!transmit_timeout(node, &msg));
	fail_on_test_v2(node->remote[la].cec_version,
			node->remote[la].has_rec_tv && unrecognized_op(&msg));
	fail_on_test_v2(node->remote[la].cec_version,
			!node->remote[la].has_rec_tv && !unrecognized_op(&msg));
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg))
		return OK_PRESUMED;

	return 0;
}

static int one_touch_rec_on(struct node *node, unsigned me, unsigned la, bool interactive)
{
	/*
	  TODO: Page 36 in HDMI CEC 1.4b spec lists additional behaviors that should be
	  checked for.
	 */
	struct cec_msg msg = {};
	struct cec_op_record_src rec_src = {};

	rec_src.type = CEC_OP_RECORD_SRC_OWN;
	cec_msg_init(&msg, me, la);
	cec_msg_record_on(&msg, true, &rec_src);
	fail_on_test(!transmit_timeout(node, &msg));
	fail_on_test(timed_out(&msg));
	fail_on_test(cec_has_record(1 << la) && unrecognized_op(&msg));
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg))
		return OK_PRESUMED;

	return 0;
}

static int one_touch_rec_off(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_record_off(&msg, false);
	fail_on_test(!transmit_timeout(node, &msg));
	fail_on_test(cec_has_record(1 << la) && unrecognized_op(&msg));
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg))
		return OK_PRESUMED;
	if (timed_out(&msg))
		return OK_PRESUMED;

	return 0;
}

static int one_touch_rec_status(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_record_status(&msg, CEC_OP_RECORD_STATUS_DIG_SERVICE);
	fail_on_test(!transmit_timeout(node, &msg));
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg))
		return OK_PRESUMED;

	return 0;
}

static struct remote_subtest one_touch_rec_subtests[] = {
	{ "Record TV Screen", CEC_LOG_ADDR_MASK_TV, one_touch_rec_tv_screen },
	{ "Record On", CEC_LOG_ADDR_MASK_RECORD, one_touch_rec_on },
	{ "Record Off", CEC_LOG_ADDR_MASK_RECORD, one_touch_rec_off },
	{ "Record Status", CEC_LOG_ADDR_MASK_ALL, one_touch_rec_status },
};


/* Timer Programming */

/*
  TODO: These are very rudimentary tests which should be expanded.
 */

static int timer_prog_set_analog_timer(struct node *node, unsigned me, unsigned la, bool interactive)
{
	/* TODO: Check the timer status for possible errors, etc. */

	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_set_analogue_timer(&msg, true, 1, 1, 0, 0, 1, 0, CEC_OP_REC_SEQ_ONCE_ONLY,
				     CEC_OP_ANA_BCAST_TYPE_CABLE,
				     7668, // 479.25 MHz
				     node->remote[la].bcast_sys);
	fail_on_test(!transmit_timeout(node, &msg, 10000));
	if (timed_out(&msg)) {
		warn("Timed out waiting for Timer Status. Assuming timer was set.\n");
		return OK_PRESUMED;
	}
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg))
		return OK_PRESUMED;

	return 0;
}

static int timer_prog_set_digital_timer(struct node *node, unsigned me, unsigned la, bool interactive)
{
	/* TODO: Check the timer status for possible errors, etc. */

	struct cec_msg msg = {};
	struct cec_op_digital_service_id digital_service_id = {};

	digital_service_id.service_id_method = CEC_OP_SERVICE_ID_METHOD_BY_CHANNEL;
	digital_service_id.channel.channel_number_fmt = CEC_OP_CHANNEL_NUMBER_FMT_1_PART;
	digital_service_id.channel.minor = 1;
	digital_service_id.dig_bcast_system = node->remote[la].dig_bcast_sys;
	cec_msg_init(&msg, me, la);
	cec_msg_set_digital_timer(&msg, true, 1, 1, 0, 0, 1, 0, CEC_OP_REC_SEQ_ONCE_ONLY,
				    &digital_service_id);
	fail_on_test(!transmit_timeout(node, &msg, 10000));
	if (timed_out(&msg)) {
		warn("Timed out waiting for Timer Status. Assuming timer was set.\n");
		return OK_PRESUMED;
	}
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg))
		return OK_PRESUMED;

	return 0;
}

static int timer_prog_set_ext_timer(struct node *node, unsigned me, unsigned la, bool interactive)
{
	/* TODO: Check the timer status. */

	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_set_ext_timer(&msg, true, 1, 1, 0, 0, 1, 0, CEC_OP_REC_SEQ_ONCE_ONLY,
			      CEC_OP_EXT_SRC_PHYS_ADDR, 0, node->phys_addr);
	fail_on_test(!transmit_timeout(node, &msg, 10000));
	if (timed_out(&msg)) {
		warn("Timed out waiting for Timer Status. Assuming timer was set.\n");
		return OK_PRESUMED;
	}
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg))
		return OK_PRESUMED;

	return 0;
}

static int timer_prog_clear_analog_timer(struct node *node, unsigned me, unsigned la, bool interactive)
{
	/* TODO: Check the timer cleared status. */

	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_clear_analogue_timer(&msg, true, 1, 1, 0, 0, 1, 0, CEC_OP_REC_SEQ_ONCE_ONLY,
				     CEC_OP_ANA_BCAST_TYPE_CABLE,
				     7668, // 479.25 MHz
				     node->remote[la].bcast_sys);
	fail_on_test(!transmit_timeout(node, &msg, 10000));
	if (timed_out(&msg)) {
		warn("Timed out waiting for Timer Cleared Status. Assuming timer was cleared.\n");
		return OK_PRESUMED;
	}
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg))
		return OK_PRESUMED;

	return 0;
}

static int timer_prog_clear_digital_timer(struct node *node, unsigned me, unsigned la, bool interactive)
{
	/* TODO: Check the timer cleared status. */

	struct cec_msg msg = {};
	struct cec_op_digital_service_id digital_service_id = {};

	digital_service_id.service_id_method = CEC_OP_SERVICE_ID_METHOD_BY_CHANNEL;
	digital_service_id.channel.channel_number_fmt = CEC_OP_CHANNEL_NUMBER_FMT_1_PART;
	digital_service_id.channel.minor = 1;
	digital_service_id.dig_bcast_system = node->remote[la].dig_bcast_sys;
	cec_msg_init(&msg, me, la);
	cec_msg_clear_digital_timer(&msg, true, 1, 1, 0, 0, 1, 0, CEC_OP_REC_SEQ_ONCE_ONLY,
				    &digital_service_id);
	fail_on_test(!transmit_timeout(node, &msg, 10000));
	if (timed_out(&msg)) {
		warn("Timed out waiting for Timer Cleared Status. Assuming timer was cleared.\n");
		return OK_PRESUMED;
	}
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg))
		return OK_PRESUMED;

	return 0;
}

static int timer_prog_clear_ext_timer(struct node *node, unsigned me, unsigned la, bool interactive)
{
	/* TODO: Check the timer cleared status. */

	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_clear_ext_timer(&msg, true, 1, 1, 0, 0, 1, 0, CEC_OP_REC_SEQ_ONCE_ONLY,
				CEC_OP_EXT_SRC_PHYS_ADDR, 0, node->phys_addr);
	fail_on_test(!transmit_timeout(node, &msg, 10000));
	if (timed_out(&msg)) {
		warn("Timed out waiting for Timer Cleared Status. Assuming timer was cleared.\n");
		return OK_PRESUMED;
	}
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;
	if (cec_msg_status_is_abort(&msg))
		return OK_PRESUMED;

	return 0;
}

static int timer_prog_set_prog_title(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_set_timer_program_title(&msg, "Super-Hans II");
	fail_on_test(!transmit_timeout(node, &msg));
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;

	return OK_PRESUMED;
}

static int timer_prog_timer_status(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_timer_status(&msg, CEC_OP_TIMER_OVERLAP_WARNING_NO_OVERLAP,
			     CEC_OP_MEDIA_INFO_NO_MEDIA,
			     CEC_OP_PROG_INFO_ENOUGH_SPACE,
			     0, 0, 0);
	fail_on_test(!transmit_timeout(node, &msg));
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;

	return OK_PRESUMED;
}

static int timer_prog_timer_clear_status(struct node *node, unsigned me, unsigned la, bool interactive)
{
	struct cec_msg msg = {};

	cec_msg_init(&msg, me, la);
	cec_msg_timer_cleared_status(&msg, CEC_OP_TIMER_CLR_STAT_CLEARED);
	fail_on_test(!transmit_timeout(node, &msg));
	if (unrecognized_op(&msg))
		return OK_NOT_SUPPORTED;
	if (refused(&msg))
		return OK_REFUSED;

	return OK_PRESUMED;
}

static struct remote_subtest timer_prog_subtests[] = {
	{ "Set Analogue Timer", CEC_LOG_ADDR_MASK_RECORD, timer_prog_set_analog_timer },
	{ "Set Digital Timer", CEC_LOG_ADDR_MASK_RECORD, timer_prog_set_digital_timer },
	{ "Set Timer Program Title", CEC_LOG_ADDR_MASK_RECORD, timer_prog_set_prog_title },
	{ "Set External Timer", CEC_LOG_ADDR_MASK_RECORD, timer_prog_set_ext_timer },
	{ "Clear Analogue Timer", CEC_LOG_ADDR_MASK_RECORD, timer_prog_clear_analog_timer },
	{ "Clear Digital Timer", CEC_LOG_ADDR_MASK_RECORD, timer_prog_clear_digital_timer },
	{ "Clear External Timer", CEC_LOG_ADDR_MASK_RECORD, timer_prog_clear_ext_timer },
	{ "Timer Status", CEC_LOG_ADDR_MASK_RECORD, timer_prog_timer_status },
	{ "Timer Cleared Status", CEC_LOG_ADDR_MASK_RECORD, timer_prog_timer_clear_status },
};

static int cdc_hec_discover(struct node *node, unsigned me, unsigned la, bool print)
{
	/* TODO: For future use cases, it might be necessary to store the results
	   from the HEC discovery to know which HECs are possible to form, etc. */
	struct cec_msg msg = {};
	__u32 mode = CEC_MODE_INITIATOR | CEC_MODE_FOLLOWER;
	bool has_cdc = false;

	doioctl(node, CEC_S_MODE, &mode);
	cec_msg_init(&msg, me, la);
	cec_msg_cdc_hec_discover(&msg);
	fail_on_test(!transmit(node, &msg));

	/* The spec describes that we shall wait for messages
	   up to 1 second, and extend the deadline for every received
	   message. The maximum time to wait for incoming state reports
	   is 5 seconds. */
	unsigned ts_start = get_ts_ms();
	while (get_ts_ms() - ts_start < 5000) {
		__u8 from;

		memset(&msg, 0, sizeof(msg));
		msg.timeout = 1000;
		if (doioctl(node, CEC_RECEIVE, &msg))
			break;
		from = cec_msg_initiator(&msg);
		if (msg.msg[1] == CEC_MSG_FEATURE_ABORT) {
			if (from == la)
				return fail("Device replied Feature Abort to broadcast message\n");

			warn("Device %d replied Feature Abort to broadcast message\n", cec_msg_initiator(&msg));
		}
		if (msg.msg[1] != CEC_MSG_CDC_MESSAGE)
			continue;
		if (msg.msg[4] != CEC_MSG_CDC_HEC_REPORT_STATE)
			continue;

		__u16 phys_addr, target_phys_addr, hec_field;
		__u8 hec_func_state, host_func_state, enc_func_state, cdc_errcode, has_field;

		cec_ops_cdc_hec_report_state(&msg, &phys_addr, &target_phys_addr,
					     &hec_func_state, &host_func_state,
					     &enc_func_state, &cdc_errcode,
					     &has_field, &hec_field);

		if (target_phys_addr != node->phys_addr)
			continue;
		if (phys_addr == node->remote[la].phys_addr)
			has_cdc = true;
		if (!print)
			continue;

		from = cec_msg_initiator(&msg);
		info("Received CDC HEC State report from device %d (%s):\n", from, cec_la2s(from));
		info("Physical address                 : %x.%x.%x.%x\n",
		     cec_phys_addr_exp(phys_addr));
		info("Target physical address          : %x.%x.%x.%x\n",
		     cec_phys_addr_exp(target_phys_addr));
		info("HEC Functionality State          : %s\n", hec_func_state2s(hec_func_state));
		info("Host Functionality State         : %s\n", host_func_state2s(host_func_state));
		info("ENC Functionality State          : %s\n", enc_func_state2s(enc_func_state));
		info("CDC Error Code                   : %s\n", cdc_errcode2s(cdc_errcode));

		if (has_field) {
			std::ostringstream oss;

			/* Bit 14 indicates whether or not the device's HDMI
			   output has HEC support/is active. */
			if (!hec_field)
				oss << "None";
			else {
				if (hec_field & (1 << 14))
					oss << "out, ";
				for (int i = 13; i >= 0; i--) {
					if (hec_field & (1 << i))
						oss << "in" << (14 - i) << ", ";
				}
				oss << "\b\b ";
			}
			info("HEC Support Field    : %s\n", oss.str().c_str());
		}
	}

	mode = CEC_MODE_INITIATOR;
	doioctl(node, CEC_S_MODE, &mode);

	if (has_cdc)
		return 0;
	return OK_NOT_SUPPORTED;
}

static struct remote_subtest cdc_subtests[] = {
	{ "CDC_HEC_Discover", CEC_LOG_ADDR_MASK_ALL, cdc_hec_discover },
};


/* Post-test checks */

static int post_test_check_recognized(struct node *node, unsigned me, unsigned la, bool interactive)
{
	bool fail = false;

	for (unsigned i = 0; i < 256; i++) {
		if (node->remote[la].recognized_op[i] && node->remote[la].unrecognized_op[i]) {
			struct cec_msg msg = {};
			msg.msg[1] = i;
			fail("Opcode %s has been both recognized by and has been replied\n", opcode2s(&msg).c_str());
			fail("Feature Abort [Unrecognized Opcode] to by the device.\n");
			fail = true;
		}
	}
	fail_on_test(fail);

	return 0;
}

static struct remote_subtest post_test_subtests[] = {
	{ "Recognized/unrecognized message consistency", CEC_LOG_ADDR_MASK_ALL, post_test_check_recognized },
};


static struct remote_test tests[] = {
	test_case("Core",
		  TAG_CORE,
		  core_subtests),
	test_case_ext("Give Device Power Status feature",
		      TAG_POWER_STATUS,
		      power_status_subtests),
	test_case("System Information feature",
		  TAG_SYSTEM_INFORMATION,
		  system_info_subtests),
	test_case("Vendor Specific Commands feature",
		  TAG_VENDOR_SPECIFIC_COMMANDS,
		  vendor_specific_subtests),
	test_case("Device OSD Transfer feature",
		  TAG_DEVICE_OSD_TRANSFER,
		  device_osd_transfer_subtests),
	test_case("OSD String feature",
		  TAG_OSD_DISPLAY,
		  osd_string_subtests),
	test_case("Remote Control Passthrough feature",
		  TAG_REMOTE_CONTROL_PASSTHROUGH,
		  rc_passthrough_subtests),
	test_case("Device Menu Control feature",
		  TAG_DEVICE_MENU_CONTROL,
		  dev_menu_ctl_subtests),
	test_case("Deck Control feature",
		  TAG_DECK_CONTROL,
		  deck_ctl_subtests),
	test_case("Tuner Control feature",
		  TAG_TUNER_CONTROL,
		  tuner_ctl_subtests),
	test_case("One Touch Record feature",
		  TAG_ONE_TOUCH_RECORD,
		  one_touch_rec_subtests),
	test_case("Timer Programming feature",
		  TAG_TIMER_PROGRAMMING,
		  timer_prog_subtests),
	test_case("Capability Discovery and Control feature",
		  TAG_CAP_DISCOVERY_CONTROL,
		  cdc_subtests),
	test_case_ext("Dynamic Auto Lipsync feature",
		      TAG_DYNAMIC_AUTO_LIPSYNC,
		      dal_subtests),
	test_case_ext("Audio Return Channel feature",
		      TAG_ARC_CONTROL,
		      arc_subtests),
	test_case_ext("System Audio Control feature",
		      TAG_SYSTEM_AUDIO_CONTROL,
		      sac_subtests),
	test_case_ext("Audio Rate Control feature",
		      TAG_AUDIO_RATE_CONTROL,
		      audio_rate_ctl_subtests),
	test_case_ext("One Touch Play feature",
		      TAG_ONE_TOUCH_PLAY,
		      one_touch_play_subtests),
	test_case("Routing Control feature",
		  TAG_ROUTING_CONTROL,
		  routing_control_subtests),
	test_case_ext("Standby/Resume and Power Status",
		      TAG_POWER_STATUS | TAG_STANDBY_RESUME,
		      standby_resume_subtests),
	test_case("Post-test checks",
		  TAG_CORE,
		  post_test_subtests),
};

static const unsigned num_tests = sizeof(tests) / sizeof(struct remote_test);

static std::map<std::string, int> mapTests;
static std::map<std::string, bool> mapTestsNoWarnings;

void collectTests()
{
	std::map<std::string, __u64> mapTestFuncs;

	for (unsigned i = 0; i < num_tests; i++) {
		for (unsigned j = 0; j < tests[i].num_subtests; j++) {
			std::string name = safename(tests[i].subtests[j].name);
			__u64 func = (__u64)tests[i].subtests[j].test_fn;

			if (mapTestFuncs.find(name) != mapTestFuncs.end() &&
			    mapTestFuncs[name] != func) {
				fprintf(stderr, "Duplicate subtest name, but different tests: %s\n",
					tests[i].subtests[j].name);
				std::exit(EXIT_FAILURE);
			}
			mapTestFuncs[name] = func;
			mapTests[name] = DONT_CARE;
			mapTestsNoWarnings[name] = false;
		}
	}
}

void listTests()
{
	for (unsigned i = 0; i < num_tests; i++) {
		printf("%s:\n", tests[i].name);
		for (unsigned j = 0; j < tests[i].num_subtests; j++) {
			std::string name = safename(tests[i].subtests[j].name);

			printf("\t%s\n", name.c_str());
		}
	}
}

int setExpectedResult(char *optarg, bool no_warnings)
{
	char *equal = std::strchr(optarg, '=');

	if (!equal || equal == optarg || !isdigit(equal[1]))
		return 1;
	*equal = 0;
	std::string name = safename(optarg);
	if (mapTests.find(name) == mapTests.end())
		return 1;
	mapTests[name] = strtoul(equal + 1, NULL, 0);
	mapTestsNoWarnings[name] = no_warnings;
	return 0;
}

void testRemote(struct node *node, unsigned me, unsigned la, unsigned test_tags,
		bool interactive)
{
	printf("testing CEC local LA %d (%s) to remote LA %d (%s):\n",
	       me, cec_la2s(me), la, cec_la2s(la));

	if (!util_interactive_ensure_power_state(node, me, la, interactive, CEC_OP_POWER_STATUS_ON))
		return;
	if (node->remote[la].in_standby && !interactive) {
		announce("The remote device is in standby. It should be powered on when testing. Aborting.");
		return;
	}
	if (!node->remote[la].has_power_status) {
		announce("The device didn't support Give Device Power Status.");
		announce("Assuming that the device is powered on.");
	}

	int ret = 0;

	for (unsigned i = 0; i < num_tests; i++) {
		if ((tests[i].tags & test_tags) != tests[i].tags)
			continue;

		printf("\t%s:\n", tests[i].name);
		for (unsigned j = 0; j < tests[i].num_subtests; j++) {
			const char *name = tests[i].subtests[j].name;

			if (tests[i].subtests[j].for_cec20 &&
			    (node->remote[la].cec_version < CEC_OP_CEC_VERSION_2_0 ||
			     !node->has_cec20))
				continue;

			if (tests[i].subtests[j].in_standby) {
				struct cec_log_addrs laddrs = { };
				doioctl(node, CEC_ADAP_G_LOG_ADDRS, &laddrs);

				if (!laddrs.log_addr_mask)
					continue;
			}
			node->in_standby = tests[i].subtests[j].in_standby;
			mode_set_initiator(node);
			unsigned old_warnings = warnings;
			ret = tests[i].subtests[j].test_fn(node, me, la, interactive);
			bool has_warnings = old_warnings < warnings;
			if (!(tests[i].subtests[j].la_mask & (1 << la)) && !ret)
				ret = OK_UNEXPECTED;

			if (mapTests[safename(name)] != DONT_CARE) {
				if (ret != mapTests[safename(name)])
					printf("\t    %s: %s (Expected '%s', got '%s')\n",
					       name, ok(FAIL),
					       result_name(mapTests[safename(name)], false),
					       result_name(ret, false));
				else if (has_warnings && mapTestsNoWarnings[safename(name)])
					printf("\t    %s: %s (Expected no warnings, got %d warnings)\n",
					       name, ok(FAIL), warnings - old_warnings);
				else if (ret == FAIL)
					printf("\t    %s: %s\n", name, ok(OK_EXPECTED_FAIL));
				else
					printf("\t    %s: %s\n", name, ok(ret));
			} else if (ret != NOTAPPLICABLE)
				printf("\t    %s: %s\n", name, ok(ret));
			if (ret == FAIL_CRITICAL)
				return;
		}
		printf("\n");
	}
}
