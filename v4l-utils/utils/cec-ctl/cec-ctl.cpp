// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright 2016 Cisco Systems, Inc. and/or its affiliates. All rights reserved.
 */

#include <cstring>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <dirent.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <ctime>
#include <cerrno>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <linux/cec-funcs.h>
#include "cec-htng-funcs.h"
#include "cec-log.h"
#include "cec-parse.h"

#ifdef __ANDROID__
#include <android-config.h>
#else
#include <config.h>
#endif

#include "cec-ctl.h"
#include "compiler.h"

static struct timespec start_monotonic;
static struct timeval start_timeofday;
static bool ignore_la[16];

#define POLL_FAKE_OPCODE 256
static unsigned short ignore_opcode[257];

static char options[512];

#define VENDOR_EXTRA \
	"  --vendor-command payload=<byte>[:<byte>]*\n" \
	"                                  Send VENDOR_COMMAND message (" xstr(CEC_MSG_VENDOR_COMMAND) ")\n" \
	"  --vendor-command-with-id vendor-id=<val>,cmd=<byte>[:<byte>]*\n" \
	"                                  Send VENDOR_COMMAND_WITH_ID message (" xstr(CEC_MSG_VENDOR_COMMAND_WITH_ID) ")\n" \
	"  --vendor-remote-button-down rc-code=<byte>[:<byte>]*\n" \
	"                                  Send VENDOR_REMOTE_BUTTON_DOWN message (" xstr(CEC_MSG_VENDOR_REMOTE_BUTTON_DOWN) ")\n"

/* Short option list

   Please keep in alphabetical order.
   That makes it easier to see which short options are still free.

   In general the lower case is used to set something and the upper
   case is used to retrieve a setting. */
enum Option {
	OptSetAdapter = 'a',
	OptClear = 'C',
	OptSetDevice = 'd',
	OptSetDriver = 'D',
	OptPhysAddrFromEDID = 'e',
	OptPhysAddrFromEDIDPoll = 'E',
	OptFrom = 'f',
	OptHelp = 'h',
	OptLogicalAddress = 'l',
	OptLogicalAddresses = 'L',
	OptMonitor = 'm',
	OptMonitorAll = 'M',
	OptToggleNoReply = 'n',
	OptNonBlocking = 'N',
	OptOsdName = 'o',
	OptPhysAddr = 'p',
	OptPoll = 'P',
	OptShowRaw = 'r',
	OptSkipInfo = 's',
	OptShowTopology = 'S',
	OptTo = 't',
	OptTrace = 'T',
	OptVerbose = 'v',
	OptVendorID = 'V',
	OptWallClock = 'w',
	OptWaitForMsgs = 'W',

	OptTV = 128,
	OptRecord,
	OptTuner,
	OptPlayback,
	OptAudio,
	OptProcessor,
	OptSwitch,
	OptCDCOnly,
	OptUnregistered,
	OptCECVersion1_4,
	OptAllowUnregFallback,
	OptNoRC,
	OptReplyToFollowers,
	OptRawMsg,
	OptListDevices,
	OptTimeout,
	OptMonitorTime,
	OptMonitorPin,
	OptIgnore,
	OptStorePin,
	OptAnalyzePin,
	OptRcTVProfile1,
	OptRcTVProfile2,
	OptRcTVProfile3,
	OptRcTVProfile4,
	OptRcSrcDevRoot,
	OptRcSrcDevSetup,
	OptRcSrcContents,
	OptRcSrcMediaTop,
	OptRcSrcMediaContext,
	OptFeatRecordTVScreen,
	OptFeatSetOSDString,
	OptFeatDeckControl,
	OptFeatSetAudioRate,
	OptFeatSinkHasARCTx,
	OptFeatSourceHasARCRx,
	OptStressTestPowerCycle,
	OptTestPowerCycle,
	OptVendorCommand = 508,
	OptVendorCommandWithID,
	OptVendorRemoteButtonDown,
	OptCustomCommand,
};

struct node {
	int fd;
	const char *device;
	unsigned caps;
	unsigned available_log_addrs;
	unsigned num_log_addrs;
	__u16 log_addr_mask;
	__u16 phys_addr;
	__u8 log_addr[CEC_MAX_LOG_ADDRS];
};

#define doioctl(n, r, p) cec_named_ioctl((n)->fd, #r, r, p)

bool verbose;

typedef std::vector<cec_msg> msg_vec;

static struct option long_options[] = {
	{ "device", required_argument, 0, OptSetDevice },
	{ "adapter", required_argument, 0, OptSetAdapter },
	{ "driver", required_argument, 0, OptSetDriver },
	{ "help", no_argument, 0, OptHelp },
	{ "trace", no_argument, 0, OptTrace },
	{ "verbose", no_argument, 0, OptVerbose },
	{ "wall-clock", no_argument, 0, OptWallClock },
	{ "osd-name", required_argument, 0, OptOsdName },
	{ "phys-addr-from-edid-poll", required_argument, 0, OptPhysAddrFromEDIDPoll },
	{ "phys-addr-from-edid", required_argument, 0, OptPhysAddrFromEDID },
	{ "phys-addr", required_argument, 0, OptPhysAddr },
	{ "vendor-id", required_argument, 0, OptVendorID },
	{ "cec-version-1.4", no_argument, 0, OptCECVersion1_4 },
	{ "allow-unreg-fallback", no_argument, 0, OptAllowUnregFallback },
	{ "no-rc-passthrough", no_argument, 0, OptNoRC },
	{ "reply-to-followers", no_argument, 0, OptReplyToFollowers },
	{ "raw-msg", no_argument, 0, OptRawMsg },
	{ "timeout", required_argument, 0, OptTimeout },
	{ "clear", no_argument, 0, OptClear },
	{ "wait-for-msgs", no_argument, 0, OptWaitForMsgs },
	{ "monitor", no_argument, 0, OptMonitor },
	{ "monitor-all", no_argument, 0, OptMonitorAll },
	{ "monitor-pin", no_argument, 0, OptMonitorPin },
	{ "monitor-time", required_argument, 0, OptMonitorTime },
	{ "ignore", required_argument, 0, OptIgnore },
	{ "store-pin", required_argument, 0, OptStorePin },
	{ "analyze-pin", required_argument, 0, OptAnalyzePin },
	{ "no-reply", no_argument, 0, OptToggleNoReply },
	{ "non-blocking", no_argument, 0, OptNonBlocking },
	{ "logical-address", no_argument, 0, OptLogicalAddress },
	{ "logical-addresses", no_argument, 0, OptLogicalAddresses },
	{ "to", required_argument, 0, OptTo },
	{ "from", required_argument, 0, OptFrom },
	{ "skip-info", no_argument, 0, OptSkipInfo },
	{ "show-raw", no_argument, 0, OptShowRaw },
	{ "show-topology", no_argument, 0, OptShowTopology },
	{ "list-devices", no_argument, 0, OptListDevices },
	{ "poll", no_argument, 0, OptPoll },
	{ "rc-tv-profile-1", no_argument, 0, OptRcTVProfile1 },
	{ "rc-tv-profile-2", no_argument, 0, OptRcTVProfile2 },
	{ "rc-tv-profile-3", no_argument, 0, OptRcTVProfile3 },
	{ "rc-tv-profile-4", no_argument, 0, OptRcTVProfile4 },
	{ "rc-src-dev-root", no_argument, 0, OptRcSrcDevRoot },
	{ "rc-src-dev-setup", no_argument, 0, OptRcSrcDevSetup },
	{ "rc-src-contents", no_argument, 0, OptRcSrcContents },
	{ "rc-src-media-top", no_argument, 0, OptRcSrcMediaTop },
	{ "rc-src-media-context", no_argument, 0, OptRcSrcMediaContext },
	{ "feat-record-tv-screen", no_argument, 0, OptFeatRecordTVScreen },
	{ "feat-set-osd-string", no_argument, 0, OptFeatSetOSDString },
	{ "feat-deck-control", no_argument, 0, OptFeatDeckControl },
	{ "feat-set-audio-rate", no_argument, 0, OptFeatSetAudioRate },
	{ "feat-sink-has-arc-tx", no_argument, 0, OptFeatSinkHasARCTx },
	{ "feat-source-has-arc-rx", no_argument, 0, OptFeatSourceHasARCRx },

	{ "tv", no_argument, 0, OptTV },
	{ "record", no_argument, 0, OptRecord },
	{ "tuner", no_argument, 0, OptTuner },
	{ "playback", no_argument, 0, OptPlayback },
	{ "audio", no_argument, 0, OptAudio },
	{ "processor", no_argument, 0, OptProcessor },
	{ "switch", no_argument, 0, OptSwitch },
	{ "cdc-only", no_argument, 0, OptCDCOnly },
	{ "unregistered", no_argument, 0, OptUnregistered },
	{ "help-all", no_argument, 0, OptHelpAll },

	CEC_PARSE_LONG_OPTS

	{ "vendor-remote-button-down", required_argument, 0, OptVendorRemoteButtonDown }, \
	{ "vendor-command-with-id", required_argument, 0, OptVendorCommandWithID }, \
	{ "vendor-command", required_argument, 0, OptVendorCommand }, \
	{ "custom-command", required_argument, 0, OptCustomCommand }, \

	{ "test-power-cycle", optional_argument, 0, OptTestPowerCycle }, \
	{ "stress-test-power-cycle", required_argument, 0, OptStressTestPowerCycle }, \

	{ 0, 0, 0, 0 }
};

static void usage()
{
	printf("Usage:\n"
	       "  -d, --device <dev>       Use device <dev> instead of /dev/cec0\n"
	       "                           If <dev> starts with a digit, then /dev/cec<dev> is used.\n"
	       "  -D, --driver <driver>    Use a cec device with this driver name\n"
	       "  -a, --adapter <adapter>  Use a cec device with this adapter name\n"
	       "  -p, --phys-addr <addr>   Use this physical address\n"
	       "  -e, --phys-addr-from-edid <path>\n"
	       "                           Set physical address from this EDID file\n"
	       "  -E, --phys-addr-from-edid-poll <path>\n"
	       "                           Continuously poll the EDID file for changes, and update the\n"
	       "                           physical address whenever there is a change\n"
	       "  -o, --osd-name <name>    Use this OSD name\n"
	       "  -V, --vendor-id <id>     Use this vendor ID\n"
	       "  -l, --logical-address    Show first configured logical address\n"
	       "  -L, --logical-addresses  Show all configured logical addresses\n"
	       "  -C, --clear              Clear all logical addresses\n"
	       "  -n, --no-reply           Toggle 'don't wait for a reply'\n"
	       "  -N, --non-blocking       Transmit messages in non-blocking mode\n"
	       "  -t, --to <la>            Send message to the given logical address\n"
	       "  -f, --from <la>          Send message from the given logical address\n"
	       "                           By default use the first assigned logical address\n"
	       "  -r, --show-raw           Show the raw CEC message (hex values)\n"
	       "  -s, --skip-info          Skip Driver Info output\n"
	       "  -S, --show-topology      Show the CEC topology\n"
	       "  -P, --poll               Send poll message\n"
	       "  -h, --help               Display this help message\n"
	       "  --help-all               Show all help messages\n"
	       "  -T, --trace              Trace all called ioctls\n"
	       "  -v, --verbose            Turn on verbose reporting\n"
	       "  -w, --wall-clock         Show timestamps as wall-clock time (implies -v)\n"
	       "  -W, --wait-for-msgs      Wait for messages and events for up to --monitor-time secs.\n"
	       "  --cec-version-1.4        Use CEC Version 1.4 instead of 2.0\n"
	       "  --allow-unreg-fallback   Allow fallback to Unregistered\n"
	       "  --no-rc-passthrough      Disable the RC passthrough\n"
	       "  --reply-to-followers     The reply will be sent to followers as well\n"
	       "  --raw-msg                Transmit the message without validating it (must be root)\n"
	       "  --timeout <ms>           Set the reply timeout in milliseconds (default is 1000 ms)\n"
	       "  --list-devices           List all cec devices\n"
	       "\n"
	       "  --tv                     This is a TV\n"
	       "  --record                 This is a recording and playback device\n"
	       "  --tuner                  This is a tuner device\n"
	       "  --playback               This is a playback device\n"
	       "  --audio                  This is an audio system device\n"
	       "  --processor              This is a processor device\n"
	       "  --switch                 This is a pure CEC switch\n"
	       "  --cdc-only               This is a CDC-only device\n"
	       "  --unregistered           This is an unregistered device\n"
	       "\n"
	       "  --feat-record-tv-screen  Signal the Record TV Screen feature\n"
	       "  --feat-set-osd-string    Signal the Set OSD String feature\n"
	       "  --feat-deck-control      Signal the Deck Control feature\n"
	       "  --feat-set-audio-rate    Signal the Set Audio Rate feature\n"
	       "  --feat-sink-has-arc-tx   Signal the sink ARC Tx feature\n"
	       "  --feat-source-has-arc-rx Signal the source ARC Rx feature\n"
	       "\n"
	       "  --rc-tv-profile-1        Signal RC TV Profile 1\n"
	       "  --rc-tv-profile-2        Signal RC TV Profile 2\n"
	       "  --rc-tv-profile-3        Signal RC TV Profile 3\n"
	       "  --rc-tv-profile-4        Signal RC TV Profile 4\n"
	       "\n"
	       "  --rc-src-dev-root        Signal that the RC source has a Dev Root Menu\n"
	       "  --rc-src-dev-setup       Signal that the RC source has a Dev Setup Menu\n"
	       "  --rc-src-contents        Signal that the RC source has a Contents Menu\n"
	       "  --rc-src-media-top       Signal that the RC source has a Media Top Menu\n"
	       "  --rc-src-media-context   Signal that the RC source has a Media Context Menu\n"
	       "\n"
	       "  -m, --monitor            Monitor CEC traffic\n"
	       "  -M, --monitor-all        Monitor all CEC traffic\n"
	       "  --monitor-pin            Monitor low-level CEC pin\n"
	       "  --monitor-time <secs>    Monitor for <secs> seconds (default is forever)\n"
	       "  --ignore <la>,<opcode>   Ignore messages from logical address <la> and opcode\n"
	       "                           <opcode> when monitoring. 'all' can be used for <la>\n"
	       "                           or <opcode> to match all logical addresses or opcodes.\n"
	       "                           To ignore poll messages use 'poll' as <opcode>.\n"
	       "  --store-pin <to>         Store the low-level CEC pin changes to the file <to>.\n"
	       "                           Use - for stdout.\n"
	       "  --analyze-pin <from>     Analyze the low-level CEC pin changes from the file <from>.\n"
	       "                           Use - for stdin.\n"
	       "  --test-power-cycle [polls=<n>][,sleep=<secs>]\n"
	       "                           Test power cycle behavior of the display. It polls up to\n"
	       "                           <n> times (default 15), waiting for a state change. If\n"
	       "                           that fails it waits <secs> seconds (default 10) before\n"
	       "                           retrying this.\n"
	       "  --stress-test-power-cycle cnt=<count>[,polls=<n>][,max-sleep=<maxsecs>][,min-sleep=<minsecs>][,seed=<seed>][,repeats=<reps>]\n"
	       "                            [,sleep-before-on=<secs1>][,sleep-before-off=<secs2>]\n"
	       "                           Power cycle display <count> times. If 0, then never stop.\n"
	       "                           It polls up to <n> times (default 30), waiting for a state change.\n"
	       "                           If <maxsecs> is non-zero (0 is the default), then sleep for\n"
	       "                           a random number of seconds between <minsecs> (0 is the default) and <maxsecs>\n"
	       "                           before each <Standby> or <Image View On> message.\n"
	       "                           If <seed> is specified, then set the randomizer seed to\n"
	       "                           that value instead of using the current time as seed.\n"
	       "                           If <reps> is specified, then repeat the <Image View On> and\n"
	       "                           <Standby> up to <reps> times. Note: should not be needed!\n"
	       "                           If <secs1> is specified, then sleep for <secs1> seconds\n"
	       "                           before transmitting <Image View On>.\n"
	       "                           If <secs2> is specified, then sleep for <secs2> seconds\n"
	       "                           before transmitting <Standby>.\n"
	       "\n"
	       CEC_PARSE_USAGE
	       "\n"
	       "  --custom-command cmd=<byte>[,payload=<byte>[:<byte>]*]\n"
	       "                                      Send custom message\n"
	       );
}

static const char *power_status2s(__u8 power_status)
{
	switch (power_status) {
	case CEC_OP_POWER_STATUS_ON:
		return "On";
	case CEC_OP_POWER_STATUS_STANDBY:
		return "Standby";
	case CEC_OP_POWER_STATUS_TO_ON:
		return "In transition Standby to On";
	case CEC_OP_POWER_STATUS_TO_STANDBY:
		return "In transition On to Standby";
	default:
		return "Unknown";
	}
}

std::string ts2s(__u64 ts)
{
	std::string s;
	struct timeval sub;
	struct timeval res;
	__u64 diff;
	char buf[64];
	time_t t;

	if (!options[OptWallClock]) {
		sprintf(buf, "%llu.%03llus", ts / 1000000000, (ts % 1000000000) / 1000000);
		return buf;
	}
	diff = ts - start_monotonic.tv_sec * 1000000000ULL - start_monotonic.tv_nsec;
	sub.tv_sec = diff / 1000000000ULL;
	sub.tv_usec = (diff % 1000000000ULL) / 1000;
	timeradd(&start_timeofday, &sub, &res);
	t = res.tv_sec;
	s = ctime(&t);
	s = s.substr(0, s.length() - 6);
	sprintf(buf, "%03lu", res.tv_usec / 1000);
	return s + "." + buf;
}

std::string ts2s(double ts)
{
	if (!options[OptWallClock]) {
		char buf[64];

		sprintf(buf, "%10.06f", ts);
		return buf;
	}
	return ts2s(static_cast<__u64>(ts * 1000000000.0));
}

static __u64 current_ts()
{
	struct timespec ts;

	clock_gettime(CLOCK_MONOTONIC, &ts);
	return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

int cec_named_ioctl(int fd, const char *name,
		    unsigned long int request, void *parm)
{
	int retval = ioctl(fd, request, parm);
	int e;

	e = retval == 0 ? 0 : errno;
	if (options[OptTrace])
		printf("\t\t%s returned %d (%s)\n",
			name, retval, strerror(e));

	return retval == -1 ? e : (retval ? -1 : 0);
}

static void print_bytes(const __u8 *bytes, unsigned len)
{
	for (unsigned i = 0; i < len; i++)
		printf(" 0x%02x", bytes[i]);
	printf(" (");
	for (unsigned i = 0; i < len; i++)
		if (bytes[i] >= 32 && bytes[i] <= 127)
		    printf("%c", bytes[i]);
		else
		    printf(" ");
	printf(")");
}

static void log_raw_msg(const struct cec_msg *msg)
{
	printf("\tRaw:");
	print_bytes(msg->msg, msg->len);
	printf("\n");
}

static const char *event2s(__u32 event)
{
	switch (event) {
	case CEC_EVENT_STATE_CHANGE:
		return "State Change";
	case CEC_EVENT_LOST_MSGS:
		return "Lost Messages";
	case CEC_EVENT_PIN_CEC_LOW:
		return "CEC Pin Low";
	case CEC_EVENT_PIN_CEC_HIGH:
		return "CEC Pin High";
	case CEC_EVENT_PIN_HPD_LOW:
		return "HPD Pin Low";
	case CEC_EVENT_PIN_HPD_HIGH:
		return "HPD Pin High";
	case CEC_EVENT_PIN_5V_LOW:
		return "5V Pin Low";
	case CEC_EVENT_PIN_5V_HIGH:
		return "5V Pin High";
	default:
		return "Unknown";
	}
}

static void log_event(struct cec_event &ev, bool show)
{
	bool is_high = ev.event == CEC_EVENT_PIN_CEC_HIGH;
	__u16 pa;

	if (ev.event != CEC_EVENT_PIN_CEC_LOW && ev.event != CEC_EVENT_PIN_CEC_HIGH &&
	    ev.event != CEC_EVENT_PIN_HPD_LOW && ev.event != CEC_EVENT_PIN_HPD_HIGH &&
	    ev.event != CEC_EVENT_PIN_5V_LOW && ev.event != CEC_EVENT_PIN_5V_HIGH)
		printf("\n");
	if ((ev.flags & CEC_EVENT_FL_DROPPED_EVENTS) && show)
		printf("(warn: %s events were lost)\n", event2s(ev.event));
	if ((ev.flags & CEC_EVENT_FL_INITIAL_STATE) && show)
		printf("Initial ");
	switch (ev.event) {
	case CEC_EVENT_STATE_CHANGE:
		pa = ev.state_change.phys_addr;
		if (show)
			printf("Event: State Change: PA: %x.%x.%x.%x, LA mask: 0x%04x, Conn Info: %s\n",
			       cec_phys_addr_exp(pa),
			       ev.state_change.log_addr_mask,
			       ev.state_change.have_conn_info ? "yes" : "no");
		break;
	case CEC_EVENT_LOST_MSGS:
		if (show)
			printf("Event: Lost %d messages\n",
			       ev.lost_msgs.lost_msgs);
		break;
	case CEC_EVENT_PIN_CEC_LOW:
	case CEC_EVENT_PIN_CEC_HIGH:
		if ((ev.flags & CEC_EVENT_FL_INITIAL_STATE) && show)
			printf("Event: CEC Pin %s\n", is_high ? "High" : "Low");

		log_event_pin(is_high, ev.ts, show);
		return;
	case CEC_EVENT_PIN_HPD_LOW:
	case CEC_EVENT_PIN_HPD_HIGH:
		if (show)
			printf("Event: HPD Pin %s\n",
			       ev.event == CEC_EVENT_PIN_HPD_HIGH ? "High" : "Low");
		break;
	case CEC_EVENT_PIN_5V_LOW:
	case CEC_EVENT_PIN_5V_HIGH:
		if (show)
			printf("Event: 5V Pin %s\n",
			       ev.event == CEC_EVENT_PIN_5V_HIGH ? "High" : "Low");
		break;
	default:
		if (show)
			printf("Event: Unknown (0x%x)\n", ev.event);
		break;
	}
	if (verbose && show)
		printf("\tTimestamp: %s\n", ts2s(ev.ts).c_str());
}

/*
 * Bits 23-8 contain the physical address, bits 0-3 the logical address
 * (equal to the index).
 */
static __u32 phys_addrs[16];

static int showTopologyDevice(struct node *node, unsigned i, unsigned la)
{
	struct cec_msg msg;
	char osd_name[15];

	printf("\tSystem Information for device %d (%s) from device %d (%s):\n",
	       i, cec_la2s(i), la & 0xf, cec_la2s(la));

	cec_msg_init(&msg, la, i);
	cec_msg_get_cec_version(&msg, true);
	doioctl(node, CEC_TRANSMIT, &msg);
	printf("\t\tCEC Version                : %s\n",
	       (!cec_msg_status_is_ok(&msg)) ? cec_status2s(msg).c_str() : cec_version2s(msg.msg[2]));

	cec_msg_init(&msg, la, i);
	cec_msg_give_physical_addr(&msg, true);
	doioctl(node, CEC_TRANSMIT, &msg);
	printf("\t\tPhysical Address           : ");
	if (!cec_msg_status_is_ok(&msg)) {
		printf("%s\n", cec_status2s(msg).c_str());
	} else {
		__u16 phys_addr = (msg.msg[2] << 8) | msg.msg[3];

		printf("%x.%x.%x.%x\n", cec_phys_addr_exp(phys_addr));
		printf("\t\tPrimary Device Type        : %s\n",
		       cec_prim_type2s(msg.msg[4]));
		phys_addrs[i] = (phys_addr << 8) | i;
	}

	cec_msg_init(&msg, la, i);
	cec_msg_give_device_vendor_id(&msg, true);
	doioctl(node, CEC_TRANSMIT, &msg);
	printf("\t\tVendor ID                  : ");
	if (!cec_msg_status_is_ok(&msg))
		printf("%s\n", cec_status2s(msg).c_str());
	else
		printf("0x%02x%02x%02x %s\n",
		       msg.msg[2], msg.msg[3], msg.msg[4],
		       cec_vendor2s(msg.msg[2] << 16 | msg.msg[3] << 8 | msg.msg[4]));

	cec_msg_init(&msg, la, i);
	cec_msg_give_osd_name(&msg, true);
	doioctl(node, CEC_TRANSMIT, &msg);
	cec_ops_set_osd_name(&msg, osd_name);
	printf("\t\tOSD Name                   : ");
	if (cec_msg_status_is_ok(&msg))
		printf("'%s'\n", osd_name);
	else
		printf("%s\n", cec_status2s(msg).c_str());

	cec_msg_init(&msg, la, i);
	cec_msg_get_menu_language(&msg, true);
	doioctl(node, CEC_TRANSMIT, &msg);
	if (cec_msg_status_is_ok(&msg)) {
		char language[4];

		cec_ops_set_menu_language(&msg, language);
		language[3] = 0;
		printf("\t\tMenu Language              : %s\n", language);
	}

	cec_msg_init(&msg, la, i);
	cec_msg_give_device_power_status(&msg, true);
	doioctl(node, CEC_TRANSMIT, &msg);
	if (cec_msg_status_is_ok(&msg)) {
		__u8 pwr;

		cec_ops_report_power_status(&msg, &pwr);
		printf("\t\tPower Status               : %s\n",
		       power_status2s(pwr));
	}

	cec_msg_init(&msg, la, i);
	cec_msg_give_features(&msg, true);
	doioctl(node, CEC_TRANSMIT, &msg);
	if (cec_msg_status_is_ok(&msg)) {
		__u8 vers, all_dev_types;
		const __u8 *rc, *feat;

		cec_ops_report_features(&msg, &vers, &all_dev_types, &rc, &feat);

		printf("\t\tFeatures                   :\n");
		printf("\t\t    CEC Version            : %s\n", cec_version2s(vers));
		printf("\t\t    All Device Types       : %s\n",
		       cec_all_dev_types2s(all_dev_types).c_str());
		while (rc) {
			if (*rc & 0x40) {
				printf("\t\t    RC Source Profile      :\n%s",
				       cec_rc_src_prof2s(*rc, "\t").c_str());
			} else {
				const char *s = "Reserved";

				switch (*rc & 0xf) {
				case 0:
					s = "None";
					break;
				case 2:
					s = "RC Profile 1";
					break;
				case 6:
					s = "RC Profile 2";
					break;
				case 10:
					s = "RC Profile 3";
					break;
				case 14:
					s = "RC Profile 4";
					break;
				}
				printf("\t\t    RC TV Profile          : %s\n", s);
			}
			if (!(*rc++ & CEC_OP_FEAT_EXT))
				break;
		}

		while (feat) {
			printf("\t\t    Device Features        :\n%s",
				       cec_dev_feat2s(*feat, "\t").c_str());
			if (!(*feat++ & CEC_OP_FEAT_EXT))
				break;
		}
	}
	return 0;
}

static __u16 calc_mask(__u16 pa)
{
	if (pa & 0xf)
		return 0xffff;
	if (pa & 0xff)
		return 0xfff0;
	if (pa & 0xfff)
		return 0xff00;
	if (pa & 0xffff)
		return 0xf000;
	return 0;
}

static int showTopology(struct node *node)
{
	struct cec_msg msg = { };
	struct cec_log_addrs laddrs = { };

	if (!(node->caps & CEC_CAP_TRANSMIT))
		return -ENOTTY;

	doioctl(node, CEC_ADAP_G_LOG_ADDRS, &laddrs);

	if (!laddrs.num_log_addrs)
		return 0;

	for (unsigned i = 0; i < 15; i++) {
		int ret;

		cec_msg_init(&msg, laddrs.log_addr[0], i);
		ret = doioctl(node, CEC_TRANSMIT, &msg);

		if (ret)
			continue;

		if (msg.tx_status & CEC_TX_STATUS_OK)
			showTopologyDevice(node, i, laddrs.log_addr[0]);
		else if (verbose && !(msg.tx_status & CEC_TX_STATUS_MAX_RETRIES))
			printf("\t\t%s for addr %d\n", cec_status2s(msg).c_str(), i);
	}

	__u32 pas[16];

	memcpy(pas, phys_addrs, sizeof(pas));
	std::sort(pas, pas + 16);
	unsigned level = 0;
	unsigned last_pa_mask = 0;

	if ((pas[0] >> 8) == 0xffff)
		return 0;

	printf("\n\tTopology:\n\n");
	for (unsigned i = 0; i < 16; i++) {
		__u16 pa = pas[i] >> 8;
		__u8 la = pas[i] & 0xf;

		if (pa == 0xffff)
			break;

		__u16 pa_mask = calc_mask(pa);

		while (last_pa_mask < pa_mask) {
			last_pa_mask = (last_pa_mask >> 4) | 0xf000;
			level++;
		}
		while (last_pa_mask > pa_mask) {
			last_pa_mask <<= 4;
			level--;
		}
		printf("\t");
		for (unsigned j = 0; j < level; j++)
			printf("    ");
		printf("%x.%x.%x.%x: %s\n", cec_phys_addr_exp(pa),
		       cec_la2s(la));
	}
	return 0;
}

static inline unsigned response_time_ms(const struct cec_msg &msg)
{
	unsigned ms = (msg.rx_ts - msg.tx_ts) / 1000000;

	// Compensate for the time it took (approx.) to receive the
	// message.
	if (ms >= msg.len * 24)
		return ms - msg.len * 24;
	return 0;
}

static void generate_eob_event(__u64 ts, FILE *fstore)
{
	if (!eob_ts || eob_ts_max >= ts)
		return;

	struct cec_event ev_eob = {
		eob_ts,
		CEC_EVENT_PIN_CEC_HIGH
	};

	if (fstore) {
		fprintf(fstore, "%llu.%09llu %d\n",
			ev_eob.ts / 1000000000, ev_eob.ts % 1000000000,
			ev_eob.event - CEC_EVENT_PIN_CEC_LOW);
		fflush(fstore);
	}
	log_event(ev_eob, fstore != stdout);
}

static void show_msg(const cec_msg &msg)
{
	__u8 from = cec_msg_initiator(&msg);
	__u8 to = cec_msg_destination(&msg);

	if (ignore_la[from])
		return;
	if ((msg.len == 1 && (ignore_opcode[POLL_FAKE_OPCODE] & (1 << from))) ||
	    (msg.len > 1 && (ignore_opcode[msg.msg[1]] & (1 << from))))
		return;

	bool transmitted = msg.tx_status != 0;
	printf("%s %s to %s (%d to %d): ",
	       transmitted ? "Transmitted by" : "Received from",
	       cec_la2s(from), to == 0xf ? "all" : cec_la2s(to), from, to);
	cec_log_msg(&msg);
	if (options[OptShowRaw])
		log_raw_msg(&msg);
	std::string status;
	if ((msg.tx_status & ~CEC_TX_STATUS_OK) ||
	    (msg.rx_status & ~CEC_RX_STATUS_OK))
		status = std::string(" ") + cec_status2s(msg);
	if (verbose && transmitted)
		printf("\tSequence: %u Tx Timestamp: %s%s\n",
		       msg.sequence, ts2s(msg.tx_ts).c_str(),
		       status.c_str());
	else if (verbose && !transmitted)
		printf("\tSequence: %u Rx Timestamp: %s%s\n",
		       msg.sequence, ts2s(msg.rx_ts).c_str(),
		       status.c_str());
}

static void wait_for_msgs(struct node &node, __u32 monitor_time)
{
	fd_set rd_fds;
	fd_set ex_fds;
	int fd = node.fd;
	time_t t;

	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
	t = time(NULL) + monitor_time;

	while (!monitor_time || time(NULL) < t) {
		struct timeval tv = { 1, 0 };
		int res;

		fflush(stdout);
		FD_ZERO(&rd_fds);
		FD_ZERO(&ex_fds);
		FD_SET(fd, &rd_fds);
		FD_SET(fd, &ex_fds);
		res = select(fd + 1, &rd_fds, NULL, &ex_fds, &tv);
		if (res < 0)
			break;
		if (FD_ISSET(fd, &ex_fds)) {
			struct cec_event ev;

			if (doioctl(&node, CEC_DQEVENT, &ev))
				continue;
			log_event(ev, true);
		}
		if (FD_ISSET(fd, &rd_fds)) {
			struct cec_msg msg = { };

			res = doioctl(&node, CEC_RECEIVE, &msg);
			if (res == ENODEV) {
				fprintf(stderr, "Device was disconnected.\n");
				break;
			}
			if (!res)
				show_msg(msg);
		}
	}
}

#define MONITOR_FL_DROPPED_EVENTS     (1 << 16)

static void monitor(struct node &node, __u32 monitor_time, const char *store_pin)
{
	__u32 monitor = CEC_MODE_MONITOR;
	fd_set rd_fds;
	fd_set ex_fds;
	int fd = node.fd;
	FILE *fstore = NULL;
	time_t t;

	if (options[OptMonitorAll])
		monitor = CEC_MODE_MONITOR_ALL;
	else if (options[OptMonitorPin] || options[OptStorePin])
		monitor = CEC_MODE_MONITOR_PIN;

	if (!(node.caps & CEC_CAP_MONITOR_ALL) && monitor == CEC_MODE_MONITOR_ALL) {
		fprintf(stderr, "Monitor All mode is not supported, falling back to regular monitoring\n");
		monitor = CEC_MODE_MONITOR;
	}
	if (!(node.caps & CEC_CAP_MONITOR_PIN) && monitor == CEC_MODE_MONITOR_PIN) {
		fprintf(stderr, "Monitor Pin mode is not supported\n");
		usage();
		std::exit(EXIT_FAILURE);
	}

	if (doioctl(&node, CEC_S_MODE, &monitor)) {
		fprintf(stderr, "Selecting monitor mode failed, you may have to run this as root.\n");
		return;
	}

	if (monitor == CEC_MODE_MONITOR_PIN) {
		struct cec_log_addrs laddrs = { };

		doioctl(&node, CEC_ADAP_G_LOG_ADDRS, &laddrs);
		if (laddrs.log_addr_mask && !options[OptSkipInfo]) {
			fprintf(stderr, "note: this CEC adapter is configured. This may cause inaccurate event\n");
			fprintf(stderr, "      timestamps. It is recommended to unconfigure the adapter (cec-ctl -C)\n");
		}
	}

	if (store_pin) {
		if (!strcmp(store_pin, "-"))
			fstore = stdout;
		else
			fstore = fopen(store_pin, "w+");
		if (fstore == NULL) {
			fprintf(stderr, "Failed to open %s: %s\n", store_pin,
				strerror(errno));
			std::exit(EXIT_FAILURE);
		}
		fprintf(fstore, "# cec-ctl --store-pin\n");
		fprintf(fstore, "# version 1\n");
		fprintf(fstore, "# start_monotonic %lu.%09lu\n",
			start_monotonic.tv_sec, start_monotonic.tv_nsec);
		fprintf(fstore, "# start_timeofday %lu.%06lu\n",
			start_timeofday.tv_sec, start_timeofday.tv_usec);
		fprintf(fstore, "# log_addr_mask 0x%04x\n", node.log_addr_mask);
		fprintf(fstore, "# phys_addr %x.%x.%x.%x\n",
			cec_phys_addr_exp(node.phys_addr));
	}

	if (fstore != stdout)
		printf("\n");

	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
	t = time(NULL) + monitor_time;

	while (!monitor_time || time(NULL) < t) {
		struct timeval tv = { 1, 0 };
		bool pin_event = false;
		int res;

		fflush(stdout);
		FD_ZERO(&rd_fds);
		FD_ZERO(&ex_fds);
		FD_SET(fd, &rd_fds);
		FD_SET(fd, &ex_fds);
		res = select(fd + 1, &rd_fds, NULL, &ex_fds, &tv);
		if (res < 0)
			break;
		if (FD_ISSET(fd, &rd_fds)) {
			struct cec_msg msg = { };

			res = doioctl(&node, CEC_RECEIVE, &msg);
			if (res == ENODEV) {
				fprintf(stderr, "Device was disconnected.\n");
				break;
			}
			if (!res && fstore != stdout)
				show_msg(msg);
		}
		if (FD_ISSET(fd, &ex_fds)) {
			struct cec_event ev;

			if (doioctl(&node, CEC_DQEVENT, &ev))
				continue;
			if (ev.event == CEC_EVENT_PIN_CEC_LOW ||
			    ev.event == CEC_EVENT_PIN_CEC_HIGH ||
			    ev.event == CEC_EVENT_PIN_HPD_LOW ||
			    ev.event == CEC_EVENT_PIN_HPD_HIGH ||
			    ev.event == CEC_EVENT_PIN_5V_LOW ||
			    ev.event == CEC_EVENT_PIN_5V_HIGH)
				pin_event = true;
			generate_eob_event(ev.ts, fstore);
			if (pin_event && fstore) {
				unsigned int v = ev.event - CEC_EVENT_PIN_CEC_LOW;

				if (ev.flags & CEC_EVENT_FL_DROPPED_EVENTS)
					v |= MONITOR_FL_DROPPED_EVENTS;
				fprintf(fstore, "%llu.%09llu %d\n",
					ev.ts / 1000000000, ev.ts % 1000000000, v);
				fflush(fstore);
			}
			if (!pin_event || options[OptMonitorPin])
				log_event(ev, fstore != stdout);
		}
		if (!res && eob_ts) {
			struct timespec ts;
			__u64 ts64;

			clock_gettime(CLOCK_MONOTONIC, &ts);
			ts64 = ts.tv_sec * 1000000000ULL + ts.tv_nsec;
			generate_eob_event(ts64, fstore);
		}
	}
	if (fstore && fstore != stdout)
		fclose(fstore);
}

static void analyze(const char *analyze_pin)
{
	FILE *fanalyze;
	struct cec_event ev = { };
	unsigned long tv_sec, tv_nsec, tv_usec;
	unsigned version;
	unsigned log_addr_mask;
	unsigned pa1, pa2, pa3, pa4;
	unsigned line = 1;
	char s[100];

	if (!strcmp(analyze_pin, "-"))
		fanalyze = stdin;
	else
		fanalyze = fopen(analyze_pin, "r");
	if (fanalyze == NULL) {
		fprintf(stderr, "Failed to open %s: %s\n", analyze_pin,
			strerror(errno));
		std::exit(EXIT_FAILURE);
	}
	if (!fgets(s, sizeof(s), fanalyze) ||
	    strcmp(s, "# cec-ctl --store-pin\n"))
		goto err;
	line++;
	if (!fgets(s, sizeof(s), fanalyze) ||
	    sscanf(s, "# version %u\n", &version) != 1 ||
	    version != 1)
		goto err;
	line++;
	if (!fgets(s, sizeof(s), fanalyze) ||
	    sscanf(s, "# start_monotonic %lu.%09lu\n", &tv_sec, &tv_nsec) != 2 ||
	    tv_nsec >= 1000000000)
		goto err;
	start_monotonic.tv_sec = tv_sec;
	start_monotonic.tv_nsec = tv_nsec;
	line++;
	if (!fgets(s, sizeof(s), fanalyze) ||
	    sscanf(s, "# start_timeofday %lu.%06lu\n", &tv_sec, &tv_usec) != 2 ||
	    tv_usec >= 1000000)
		goto err;
	start_timeofday.tv_sec = tv_sec;
	start_timeofday.tv_usec = tv_usec;
	line++;
	if (!fgets(s, sizeof(s), fanalyze) ||
	    sscanf(s, "# log_addr_mask 0x%04x\n", &log_addr_mask) != 1)
		goto err;
	line++;
	if (!fgets(s, sizeof(s), fanalyze) ||
	    sscanf(s, "# phys_addr %x.%x.%x.%x\n", &pa1, &pa2, &pa3, &pa4) != 4)
		goto err;
	line++;

	printf("Physical Address:     %x.%x.%x.%x\n", pa1, pa2, pa3, pa4);
	printf("Logical Address Mask: 0x%04x\n\n", log_addr_mask);

	while (fgets(s, sizeof(s), fanalyze)) {
		unsigned event;

		if (s[0] == '#' || s[0] == '\n') {
			line++;
			continue;
		}
		if (sscanf(s, "%lu.%09lu %d\n", &tv_sec, &tv_nsec, &event) != 3 ||
		    (event & ~MONITOR_FL_DROPPED_EVENTS) > 5) {
			fprintf(stderr, "malformed data at line %d\n", line);
			break;
		}
		ev.ts = tv_sec * 1000000000ULL + tv_nsec;
		ev.flags = 0;
		if (event & MONITOR_FL_DROPPED_EVENTS) {
			event &= ~MONITOR_FL_DROPPED_EVENTS;
			ev.flags = CEC_EVENT_FL_DROPPED_EVENTS;
		}
		ev.event = event + CEC_EVENT_PIN_CEC_LOW;
		log_event(ev, true);
		line++;
	}

	if (eob_ts) {
		ev.event = CEC_EVENT_PIN_CEC_HIGH;
		ev.ts = eob_ts;
		log_event(ev, true);
	}

	if (fanalyze != stdin)
		fclose(fanalyze);
	return;

err:
	fprintf(stderr, "Not a pin store file: malformed data at line %d\n", line);
	std::exit(EXIT_FAILURE);
}

static bool wait_for_pwr_state(struct node &node, unsigned from,
			       unsigned &hpd_is_low_cnt, bool on)
{
	struct cec_msg msg;
	__u8 pwr;
	int ret;

	cec_msg_init(&msg, from, CEC_LOG_ADDR_TV);
	cec_msg_give_device_power_status(&msg, true);
	ret = doioctl(&node, CEC_TRANSMIT, &msg);
	if (ret == ENONET) {
		printf("X");
		fflush(stdout);
		hpd_is_low_cnt++;
		return hpd_is_low_cnt <= 2 ? false : !on;
	}
	hpd_is_low_cnt = 0;
	if (ret) {
		fprintf(stderr, "Give Device Power Status Transmit failed: %s\n",
			strerror(ret));
		std::exit(EXIT_FAILURE);
	}
	if ((msg.rx_status & CEC_RX_STATUS_OK) &&
	    (msg.rx_status & CEC_RX_STATUS_FEATURE_ABORT)) {
		printf("A");
		fflush(stdout);
		return false;
	}
	if (!(msg.rx_status & CEC_RX_STATUS_OK)) {
		if (msg.tx_status & CEC_TX_STATUS_OK)
			printf("T");
		else
			printf("N");
		fflush(stdout);
		return false;
	}

	cec_ops_report_power_status(&msg, &pwr);
	switch (pwr) {
	case CEC_OP_POWER_STATUS_ON:
		printf("+");
		break;
	case CEC_OP_POWER_STATUS_STANDBY:
		printf("-");
		break;
	case CEC_OP_POWER_STATUS_TO_ON:
		printf("%c", on ? '/' : '|');
		break;
	case CEC_OP_POWER_STATUS_TO_STANDBY:
		printf("%c", on ? '|' : '\\');
		break;
	default:
		printf(" %d ", pwr);
		break;
	}
	fflush(stdout);
	return pwr == (on ? CEC_OP_POWER_STATUS_ON : CEC_OP_POWER_STATUS_STANDBY);
}

static bool wait_for_power_on(struct node &node, unsigned from)
{
	unsigned hpd_is_low_cnt = 0;
	return wait_for_pwr_state(node, from, hpd_is_low_cnt, true);
}

static bool wait_for_power_off(struct node &node, unsigned from, unsigned &hpd_is_low_cnt)
{
	return wait_for_pwr_state(node, from, hpd_is_low_cnt, false);
}

static int transmit_msg_retry(struct node &node, struct cec_msg &msg)
{
	bool from_unreg = cec_msg_initiator(&msg) == CEC_LOG_ADDR_UNREGISTERED;
	unsigned cnt = 0;
	bool repeat;
	int ret;

	// Can happen if wakeup_la == 15 and CEC just started configuring
	do {
		ret = doioctl(&node, CEC_TRANSMIT, &msg);
		repeat = ret == ENONET || (ret == EINVAL && from_unreg);
		if (repeat)
			usleep(100000);
	} while (repeat && cnt++ < 10);
	return ret;
}

static int init_power_cycle_test(struct node &node, unsigned repeats, unsigned max_tries)
{
	struct cec_msg msg;
	unsigned from;
	unsigned tries;
	__u16 pa;
	int ret;

	printf("Legend:\n\n"
	       "X   No LA claimed (HPD is likely pulled low)\n"
	       "N   Give Device Power Status was Nacked\n"
	       "T   Time out waiting for Report Power Status reply\n"
	       "A   Feature Abort of Give Device Power Status \n"
	       "+   Reported On\n"
	       "-   Reported In Standby\n"
	       "/   Reported Transitioning to On\n"
	       "\\   Reported Transitioning to Standby\n"
	       "|   Reported Transitioning to On when 'to Standby' was expected or vice versa\n\n");

	struct cec_log_addrs laddrs = { };
	doioctl(&node, CEC_ADAP_G_LOG_ADDRS, &laddrs);
	if (laddrs.log_addr[0] != CEC_LOG_ADDR_INVALID) {
		from = laddrs.log_addr[0];
	} else {
		switch (laddrs.log_addr_type[0]) {
		case CEC_LOG_ADDR_TYPE_TV:
			fprintf(stderr, "A TV can't run the power cycle test.\n");
			std::exit(EXIT_FAILURE);
		case CEC_LOG_ADDR_TYPE_RECORD:
			from = CEC_LOG_ADDR_RECORD_1;
			break;
		case CEC_LOG_ADDR_TYPE_TUNER:
			from = CEC_LOG_ADDR_TUNER_1;
			break;
		case CEC_LOG_ADDR_TYPE_PLAYBACK:
			from = CEC_LOG_ADDR_PLAYBACK_1;
			break;
		case CEC_LOG_ADDR_TYPE_AUDIOSYSTEM:
			from = CEC_LOG_ADDR_AUDIOSYSTEM;
			break;
		case CEC_LOG_ADDR_TYPE_SPECIFIC:
			from = CEC_LOG_ADDR_SPECIFIC;
			break;
		case CEC_LOG_ADDR_TYPE_UNREGISTERED:
		default:
			from = CEC_LOG_ADDR_UNREGISTERED;
			break;
		}
	}

	if (laddrs.log_addr[0] == CEC_LOG_ADDR_INVALID) {
		printf("No Logical Addresses claimed, assume TV is already in Standby\n\n");
	} else {
		doioctl(&node, CEC_ADAP_G_PHYS_ADDR, &pa);
		for (unsigned repeat = 0; repeat <= repeats; repeat++) {
			/*
			 * Some displays only accept Standby from the Active Source.
			 * So make us the Active Source before sending Standby.
			 */
			printf("%s: ", ts2s(current_ts()).c_str());
			printf("Transmit Active Source to TV: ");
			fflush(stdout);
			cec_msg_init(&msg, from, CEC_LOG_ADDR_TV);
			cec_msg_active_source(&msg, pa);
			ret = transmit_msg_retry(node, msg);
			if (ret) {
				printf("FAIL: %s\n", strerror(ret));
				std::exit(EXIT_FAILURE);
			}
			printf("OK\n");
			printf("%s: ", ts2s(current_ts()).c_str());
			printf("Transmit Standby to TV: ");
			fflush(stdout);
			cec_msg_init(&msg, from, CEC_LOG_ADDR_TV);
			cec_msg_standby(&msg);

			tries = 0;
			unsigned hpd_is_low_cnt = 0;
			for (;;) {
				doioctl(&node, CEC_ADAP_G_LOG_ADDRS, &laddrs);
				if (laddrs.log_addr[0] == CEC_LOG_ADDR_INVALID)
					break;

				ret = transmit_msg_retry(node, msg);
				if (ret) {
					printf("FAIL: %s\n", strerror(ret));
					std::exit(EXIT_FAILURE);
				}

				if (wait_for_power_off(node, from, hpd_is_low_cnt))
					break;
				if (++tries > max_tries) {
					if (repeat == repeats) {
						printf(" FAIL: never went into standby\n");
						std::exit(EXIT_FAILURE);
					}
					break;
				}
				sleep(1);
			}
			if (tries <= max_tries)
				break;

			printf(" WARN: never went into standby during attempt %u\n", repeat + 1);
		}
		printf(" OK\n");
		printf("%s: ", ts2s(current_ts()).c_str());
		printf("TV is in Standby\n");
		sleep(5);
	}
	doioctl(&node, CEC_ADAP_G_PHYS_ADDR, &pa);
	printf("%s: ", ts2s(current_ts()).c_str());
	printf("Physical Address: %x.%x.%x.%x\n\n",
	       cec_phys_addr_exp(pa));
	return from;
}

static void test_power_cycle(struct node &node, unsigned int max_tries,
			     unsigned int retry_sleep)
{
	struct cec_log_addrs laddrs = { };
	struct cec_msg msg;
	unsigned failures = 0;
	unsigned from;
	unsigned tries;
	unsigned secs;
	__u16 pa, prev_pa;
	__u16 display_pa = CEC_PHYS_ADDR_INVALID;
	__u8 wakeup_la;
	int ret;

	from = init_power_cycle_test(node, 2, max_tries);

	doioctl(&node, CEC_ADAP_G_LOG_ADDRS, &laddrs);
	if (laddrs.log_addr[0] != CEC_LOG_ADDR_INVALID)
		wakeup_la = from = laddrs.log_addr[0];
	else
		wakeup_la = CEC_LOG_ADDR_UNREGISTERED;

	bool hpd_is_low = wakeup_la == CEC_LOG_ADDR_UNREGISTERED;

	printf("The Hotplug Detect pin %s when in Standby\n\n",
	       hpd_is_low ? "is pulled low" : "remains high");

	for (unsigned iter = 0; iter <= 2 * 12; iter++) {
		unsigned i = iter / 2;

		/*
		 * For sleep values 0-5 run the test twice,
		 * after that run it only once.
		 */
		if (i > 5 && (iter & 1))
			continue;

		printf("%s: ", ts2s(current_ts()).c_str());
		printf("Wake up TV using Image View On from LA %s: ", cec_la2s(wakeup_la));
		fflush(stdout);
		cec_msg_init(&msg, wakeup_la, CEC_LOG_ADDR_TV);
		cec_msg_image_view_on(&msg);
		ret = transmit_msg_retry(node, msg);
		if (ret) {
			printf("FAIL: %s\n", strerror(ret));
			std::exit(EXIT_FAILURE);
		}
		tries = 0;
		for (;;) {
			doioctl(&node, CEC_ADAP_G_LOG_ADDRS, &laddrs);
			if (laddrs.log_addr[0] != CEC_LOG_ADDR_INVALID)
				wakeup_la = from = laddrs.log_addr[0];
			if (wait_for_power_on(node, from))
				break;
			if (++tries > max_tries)
				break;
			sleep(1);
		}

		if (tries > max_tries) {
			wakeup_la = from;
			printf("\nFAIL: never woke up, sleep %u secs, then retry\n",
			       retry_sleep);
			failures++;
			fflush(stdout);
			sleep(retry_sleep);
			printf("%s: ", ts2s(current_ts()).c_str());
			printf("Wake up TV using Image View On from LA %s: ", cec_la2s(wakeup_la));
			fflush(stdout);
			cec_msg_init(&msg, wakeup_la, CEC_LOG_ADDR_TV);
			cec_msg_image_view_on(&msg);
			ret = doioctl(&node, CEC_TRANSMIT, &msg);
			if (ret == ENONET) {
				printf("(ENONET) ");
			} else if (ret == EINVAL && wakeup_la == CEC_LOG_ADDR_UNREGISTERED) {
				printf("(EINVAL) ");
			} else if (ret) {
				printf("FAIL: %s\n", strerror(ret));
				std::exit(EXIT_FAILURE);
			}
			tries = 0;
			for (;;) {
				doioctl(&node, CEC_ADAP_G_LOG_ADDRS, &laddrs);
				if (laddrs.log_addr[0] != CEC_LOG_ADDR_INVALID)
					wakeup_la = from = laddrs.log_addr[0];
				if (wait_for_power_on(node, from))
					break;
				if (++tries > max_tries) {
					printf("\nFAIL: never woke up\n");
					failures++;
					break;
				}
				sleep(1);
			}
		}
		printf(" %d second%s\n", tries, tries == 1 ? "" : "s");

		doioctl(&node, CEC_ADAP_G_LOG_ADDRS, &laddrs);
		if (laddrs.log_addr[0] == CEC_LOG_ADDR_INVALID) {
			printf("FAIL: invalid logical address\n");
			std::exit(EXIT_FAILURE);
		}
		from = laddrs.log_addr[0];
		doioctl(&node, CEC_ADAP_G_PHYS_ADDR, &pa);
		printf("%s: ", ts2s(current_ts()).c_str());
		printf("Physical Address: %x.%x.%x.%x LA: %s\n",
		       cec_phys_addr_exp(pa), cec_la2s(from));
		if (pa == CEC_PHYS_ADDR_INVALID || !pa) {
			printf("FAIL: invalid physical address\n");
			std::exit(EXIT_FAILURE);
		}
		prev_pa = pa;
		if (display_pa == CEC_PHYS_ADDR_INVALID)
			display_pa = pa;
		if (pa != display_pa) {
			printf("FAIL: physical address changed from %x.%x.%x.%x to %x.%x.%x.%x\n",
			       cec_phys_addr_exp(display_pa), cec_phys_addr_exp(pa));
			std::exit(EXIT_FAILURE);
		}
		secs = i <= 10 ? i : 10 + 10 * (i - 10);
		printf("%s: ", ts2s(current_ts()).c_str());
		printf("Sleep %u second%s\n", secs, secs == 1 ? "" : "s");
		sleep(secs);
		doioctl(&node, CEC_ADAP_G_PHYS_ADDR, &pa);
		if (pa != prev_pa) {
			printf("\tFAIL: PA is now %x.%x.%x.%x\n\n",
			       cec_phys_addr_exp(pa));
			std::exit(EXIT_FAILURE);
		}

		printf("\n%s: ", ts2s(current_ts()).c_str());
		printf("Put TV in standby from LA %s: ", cec_la2s(from));
		fflush(stdout);
		/*
		 * Some displays only accept Standby from the Active Source.
		 * So make us the Active Source before sending Standby.
		 */
		cec_msg_init(&msg, from, CEC_LOG_ADDR_TV);
		cec_msg_active_source(&msg, pa);
		ret = transmit_msg_retry(node, msg);
		if (ret) {
			printf("FAIL: Active Source Transmit failed: %s\n", strerror(ret));
			std::exit(EXIT_FAILURE);
		}
		cec_msg_init(&msg, from, CEC_LOG_ADDR_TV);
		cec_msg_standby(&msg);
		ret = transmit_msg_retry(node, msg);
		if (ret) {
			printf("FAIL: %s\n", strerror(ret));
			std::exit(EXIT_FAILURE);
		}

		tries = 0;
		bool first_standby = true;
		unsigned hpd_is_low_cnt = 0;
		for (;;) {
			doioctl(&node, CEC_ADAP_G_LOG_ADDRS, &laddrs);
			if (laddrs.log_addr[0] == CEC_LOG_ADDR_INVALID)
				break;
			if (!hpd_is_low)
				hpd_is_low_cnt = 0;
			if (wait_for_power_off(node, from, hpd_is_low_cnt))
				break;
			if (++tries > max_tries) {
				if (first_standby) {
					printf("\nFAIL: never went into standby, sleep %u secs, then retry\n",
					       retry_sleep);
					failures++;
					fflush(stdout);
					sleep(retry_sleep);
					printf("%s: ", ts2s(current_ts()).c_str());
					printf("Put TV in standby from LA %s: ", cec_la2s(from));
					fflush(stdout);
					first_standby = false;
					tries = 0;
					cec_msg_init(&msg, from, CEC_LOG_ADDR_TV);
					cec_msg_standby(&msg);
					ret = doioctl(&node, CEC_TRANSMIT, &msg);
					if (!ret)
						continue;
					printf("FAIL: %s\n", strerror(ret));
					std::exit(EXIT_FAILURE);
				}
				printf("\nFAIL: never went into standby\n");
				failures++;
				break;
			}
			sleep(1);
		}
		printf(" %d second%s\n", tries, tries == 1 ? "" : "s");
		doioctl(&node, CEC_ADAP_G_PHYS_ADDR, &pa);
		printf("%s: ", ts2s(current_ts()).c_str());
		printf("Physical Address: %x.%x.%x.%x\n",
		       cec_phys_addr_exp(pa));
		prev_pa = pa;
		printf("%s: ", ts2s(current_ts()).c_str());
		printf("Sleep %d second%s\n", secs, secs == 1 ? "" : "s");
		sleep(secs);
		doioctl(&node, CEC_ADAP_G_PHYS_ADDR, &pa);
		if (pa != prev_pa) {
			printf("%s: ", ts2s(current_ts()).c_str());
			printf("Physical Address: %x.%x.%x.%x\n",
			       cec_phys_addr_exp(pa));
		}
		if (pa != CEC_PHYS_ADDR_INVALID && pa != display_pa) {
			printf("FAIL: physical address changed from %x.%x.%x.%x to %x.%x.%x.%x\n",
			       cec_phys_addr_exp(display_pa), cec_phys_addr_exp(pa));
			std::exit(EXIT_FAILURE);
		}
		printf("\n");
	}
	if (failures)
		printf("Test had %u failure%s\n", failures, failures == 1 ? "" : "s");
}

static void stress_test_power_cycle(struct node &node, unsigned cnt,
				    unsigned min_sleep, unsigned max_sleep, unsigned max_tries,
				    bool has_seed, unsigned seed, unsigned repeats,
				    double sleep_before_on, double sleep_before_off)
{
	struct cec_log_addrs laddrs = { };
	struct cec_msg msg;
	unsigned tries = 0;
	unsigned iter = 0;
	unsigned min_usleep = 1000000 * (max_sleep ? min_sleep : 0);
	unsigned mod_usleep = 0;
	unsigned wakeup_la;
	__u16 pa, prev_pa;
	__u16 display_pa = CEC_PHYS_ADDR_INVALID;
	int ret;

	if (max_sleep)
		mod_usleep = 1000000 * (max_sleep - min_sleep) + 1;

	if (!has_seed)
		seed = time(NULL);

	if (mod_usleep)
		printf("Randomizer seed: %u\n\n", seed);

	unsigned from = init_power_cycle_test(node, repeats, max_tries);

	doioctl(&node, CEC_ADAP_G_LOG_ADDRS, &laddrs);
	if (laddrs.log_addr[0] != CEC_LOG_ADDR_INVALID)
		wakeup_la = from = laddrs.log_addr[0];
	else
		wakeup_la = CEC_LOG_ADDR_UNREGISTERED;

	bool hpd_is_low = wakeup_la == CEC_LOG_ADDR_UNREGISTERED;

	printf("The Hotplug Detect pin %s when in Standby\n\n",
	       hpd_is_low ? "is pulled low" : "remains high");

	srandom(seed);

	for (;;) {
		unsigned usecs1 = mod_usleep ? random() % mod_usleep : static_cast<unsigned>(sleep_before_on * 1000000);
		unsigned usecs2 = mod_usleep ? random() % mod_usleep : static_cast<unsigned>(sleep_before_off * 1000000);

		usecs1 += min_usleep;
		usecs2 += min_usleep;

		iter++;

		if (usecs1)
			printf("%s: Sleep %.2fs\n", ts2s(current_ts()).c_str(),
			       usecs1 / 1000000.0);
		fflush(stdout);
		usleep(usecs1);
		for (unsigned repeat = 0; repeat <= repeats; repeat++) {
			printf("%s: ", ts2s(current_ts()).c_str());
			printf("Transmit Image View On from LA %s (iteration %u): ", cec_la2s(wakeup_la), iter);

			tries = 0;
			cec_msg_init(&msg, wakeup_la, CEC_LOG_ADDR_TV);
			cec_msg_image_view_on(&msg);
			ret = transmit_msg_retry(node, msg);
			if (ret == EINVAL && wakeup_la == CEC_LOG_ADDR_UNREGISTERED) {
				// Can happen if wakeup_la == 15 and CEC just started configuring
				printf("(EINVAL) ");
			} else if (ret) {
				printf("FAIL: %s\n", strerror(ret));
				std::exit(EXIT_FAILURE);
			}

			for (;;) {
				doioctl(&node, CEC_ADAP_G_LOG_ADDRS, &laddrs);
				if (laddrs.log_addr[0] != CEC_LOG_ADDR_INVALID)
					wakeup_la = from = laddrs.log_addr[0];
				if (wait_for_power_on(node, from))
					break;
				if (++tries > max_tries) {
					if (repeat == repeats) {
						printf("\nFAIL: never woke up\n");
						std::exit(EXIT_FAILURE);
					}
					break;
				}
				sleep(1);
			}
			if (tries <= max_tries)
				break;
			printf("\nWARN: never woke up during attempt %u\n", repeat + 1);
		}
		printf(" %d second%s\n", tries, tries == 1 ? "" : "s");
		doioctl(&node, CEC_ADAP_G_LOG_ADDRS, &laddrs);
		if (laddrs.log_addr[0] == CEC_LOG_ADDR_INVALID) {
			printf("FAIL: invalid logical address\n");
			std::exit(EXIT_FAILURE);
		}
		from = laddrs.log_addr[0];
		doioctl(&node, CEC_ADAP_G_PHYS_ADDR, &pa);
		prev_pa = pa;
		printf("%s: ", ts2s(current_ts()).c_str());
		printf("Physical Address: %x.%x.%x.%x LA: %s\n",
		       cec_phys_addr_exp(pa), cec_la2s(from));
		if (pa == CEC_PHYS_ADDR_INVALID || !pa) {
			printf("FAIL: invalid physical address\n");
			std::exit(EXIT_FAILURE);
		}
		if (display_pa == CEC_PHYS_ADDR_INVALID)
			display_pa = pa;
		if (pa != display_pa) {
			printf("FAIL: physical address changed from %x.%x.%x.%x to %x.%x.%x.%x\n",
			       cec_phys_addr_exp(display_pa), cec_phys_addr_exp(pa));
			std::exit(EXIT_FAILURE);
		}

		if (cnt && iter == cnt)
			break;

		if (usecs2)
			printf("%s: Sleep %.2fs\n", ts2s(current_ts()).c_str(),
			       usecs2 / 1000000.0);
		fflush(stdout);
		usleep(usecs2);
		for (unsigned repeat = 0; repeat <= repeats; repeat++) {
			printf("%s: ", ts2s(current_ts()).c_str());
			printf("Transmit Standby (iteration %u): ", iter);
			doioctl(&node, CEC_ADAP_G_PHYS_ADDR, &pa);
			if (pa != prev_pa) {
				printf("\tFAIL: PA is now %x.%x.%x.%x\n\n",
				       cec_phys_addr_exp(pa));
				std::exit(EXIT_FAILURE);
			}
			if (pa != CEC_PHYS_ADDR_INVALID && pa != display_pa) {
				printf("FAIL: physical address changed from %x.%x.%x.%x to %x.%x.%x.%x\n",
				       cec_phys_addr_exp(display_pa), cec_phys_addr_exp(pa));
				std::exit(EXIT_FAILURE);
			}

			cec_msg_init(&msg, from, CEC_LOG_ADDR_TV);
			/*
			 * Some displays only accept Standby from the Active Source.
			 * So make us the Active Source before sending Standby.
			 */
			cec_msg_active_source(&msg, pa);
			ret = transmit_msg_retry(node, msg);
			if (ret) {
				printf("FAIL: Active Source Transmit failed: %s\n", strerror(ret));
				std::exit(EXIT_FAILURE);
			}
			cec_msg_standby(&msg);
			ret = transmit_msg_retry(node, msg);
			if (ret) {
				printf("FAIL: %s\n", strerror(ret));
				std::exit(EXIT_FAILURE);
			}

			tries = 0;
			unsigned hpd_is_low_cnt = 0;
			for (;;) {
				doioctl(&node, CEC_ADAP_G_LOG_ADDRS, &laddrs);
				if (laddrs.log_addr[0] == CEC_LOG_ADDR_INVALID)
					break;
				if (!hpd_is_low)
					hpd_is_low_cnt = 0;
				if (wait_for_power_off(node, from, hpd_is_low_cnt))
					break;
				if (++tries > max_tries) {
					if (repeat == repeats) {
						printf("\nFAIL: never went into standby\n");
						std::exit(EXIT_FAILURE);
					}
					break;
				}
				sleep(1);
			}
			if (tries <= max_tries)
				break;
			printf("\nWARN: never went into standby during attempt %u\n", repeat + 1);
		}
		printf(" %d second%s\n", tries, tries == 1 ? "" : "s");
	}
}

static int calc_node_val(const char *s)
{
	s = std::strrchr(s, '/') + 1;

	if (!memcmp(s, "cec", 3))
		return atol(s + 3);
	return 0;
}

static bool sort_on_device_name(const std::string &s1, const std::string &s2)
{
	int n1 = calc_node_val(s1.c_str());
	int n2 = calc_node_val(s2.c_str());

	return n1 < n2;
}

static unsigned int cec_get_edid_spa_location(const __u8 *edid,
					      unsigned int size)
{
	unsigned int blocks = size / 128;
	unsigned int block;
	__u8 d;

	/* Sanity check: at least 2 blocks and a multiple of the block size */
	if (blocks < 2 || size % 128)
		return 0;

	/*
	 * If there are fewer extension blocks than the size, then update
	 * 'blocks'. It is allowed to have more extension blocks than the size,
	 * since some hardware can only read e.g. 256 bytes of the EDID, even
	 * though more blocks are present. The first CEA-861 extension block
	 * should normally be in block 1 anyway.
	 */
	if (edid[0x7e] + 1U < blocks)
		blocks = edid[0x7e] + 1;

	for (block = 1; block < blocks; block++) {
		unsigned int offset = block * 128;

		/* Skip any non-CEA-861 extension blocks */
		if (edid[offset] != 0x02 || edid[offset + 1] != 0x03)
			continue;

		/* search Vendor Specific Data Block (tag 3) */
		d = edid[offset + 2] & 0x7f;
		/* Check if there are Data Blocks */
		if (d <= 4)
			continue;
		if (d > 4) {
			unsigned int i = offset + 4;
			unsigned int end = offset + d;

			/* Note: 'end' is always < 'size' */
			do {
				__u8 tag = edid[i] >> 5;
				__u8 len = edid[i] & 0x1f;

				if (tag == 3 && len >= 5 && i + len <= end &&
				    edid[i + 1] == 0x03 &&
				    edid[i + 2] == 0x0c &&
				    edid[i + 3] == 0x00)
					return i + 4;
				i += len + 1;
			} while (i < end);
		}
	}
	return 0;
}

static __u16 parse_phys_addr_from_edid(const char *edid_path)
{
	FILE *f = fopen(edid_path, "r");
	__u16 pa = CEC_PHYS_ADDR_INVALID;
	__u8 edid[256];

	if (f == NULL)
		return pa;
	if (fread(edid, sizeof(edid), 1, f) == 1) {
		unsigned int loc = cec_get_edid_spa_location(edid, sizeof(edid));

		if (loc)
			pa = (edid[loc] << 8) | edid[loc + 1];
	}
	fclose(f);
	return pa;
}

typedef std::vector<std::string> dev_vec;
typedef std::map<std::string, std::string> dev_map;

static void list_devices()
{
	DIR *dp;
	struct dirent *ep;
	dev_vec files;
	dev_map links;
	dev_map cards;
	struct cec_caps caps;

	dp = opendir("/dev");
	if (dp == NULL) {
		perror ("Couldn't open the directory");
		return;
	}
	while ((ep = readdir(dp)))
		if (!memcmp(ep->d_name, "cec", 3) && isdigit(ep->d_name[3]))
			files.push_back(std::string("/dev/") + ep->d_name);
	closedir(dp);

	/* Find device nodes which are links to other device nodes */
	for (dev_vec::iterator iter = files.begin();
			iter != files.end(); ) {
		char link[64+1];
		int link_len;
		std::string target;

		link_len = readlink(iter->c_str(), link, 64);
		if (link_len < 0) {	/* Not a link or error */
			iter++;
			continue;
		}
		link[link_len] = '\0';

		/* Only remove from files list if target itself is in list */
		if (link[0] != '/')	/* Relative link */
			target = std::string("/dev/");
		target += link;
		if (find(files.begin(), files.end(), target) == files.end()) {
			iter++;
			continue;
		}

		/* Move the device node from files to links */
		if (links[target].empty())
			links[target] = *iter;
		else
			links[target] += ", " + *iter;
		iter = files.erase(iter);
	}

	std::sort(files.begin(), files.end(), sort_on_device_name);

	for (dev_vec::iterator iter = files.begin();
			iter != files.end(); ++iter) {
		int fd = open(iter->c_str(), O_RDWR);
		std::string cec_info;

		if (fd < 0)
			continue;
		int err = ioctl(fd, CEC_ADAP_G_CAPS, &caps);
		close(fd);
		if (err)
			continue;
		cec_info = std::string(caps.driver) + " (" + caps.name + ")";
		if (cards[cec_info].empty())
			cards[cec_info] += cec_info + ":\n";
		cards[cec_info] += "\t" + (*iter);
		if (!(links[*iter].empty()))
			cards[cec_info] += " <- " + links[*iter];
		cards[cec_info] += "\n";
	}
	for (dev_map::iterator iter = cards.begin();
			iter != cards.end(); ++iter) {
		printf("%s\n", iter->second.c_str());
	}
}

int main(int argc, char **argv)
{
	std::string device;
	const char *driver = NULL;
	const char *adapter = NULL;
	const struct cec_msg_args *opt;
	msg_vec msgs;
	char short_options[26 * 2 * 2 + 1];
	__u32 timeout = 1000;
	__u32 monitor_time = 0;
	__u32 vendor_id = 0x000c03; /* HDMI LLC vendor ID */
	unsigned int stress_test_pwr_cycle_cnt = 0;
	unsigned int stress_test_pwr_cycle_min_sleep = 0;
	unsigned int stress_test_pwr_cycle_max_sleep = 0;
	unsigned int stress_test_pwr_cycle_polls = 30;
	bool stress_test_pwr_cycle_has_seed = false;
	unsigned int stress_test_pwr_cycle_seed = 0;
	unsigned int stress_test_pwr_cycle_repeats = 0;
	double stress_test_pwr_cycle_sleep_before_on = 0;
	double stress_test_pwr_cycle_sleep_before_off = 0;
	unsigned int test_pwr_cycle_polls = 15;
	unsigned int test_pwr_cycle_sleep = 10;
	bool warn_if_unconfigured = false;
	__u16 phys_addr;
	__u8 from = 0, to = 0, first_to = 0xff;
	__u8 dev_features = 0;
	__u8 rc_tv = 0;
	__u8 rc_src = 0;
	const char *osd_name = "";
	const char *edid_path = NULL;
	const char *store_pin = NULL;
	const char *analyze_pin = NULL;
	bool reply = true;
	int idx = 0;
	int fd = -1;
	int ch;
	int i;

	memset(phys_addrs, 0xff, sizeof(phys_addrs));

	for (i = 0; long_options[i].name; i++) {
		if (!isalpha(long_options[i].val))
			continue;
		short_options[idx++] = long_options[i].val;
		if (long_options[i].has_arg == required_argument)
			short_options[idx++] = ':';
	}
	while (true) {
		int option_index = 0;
		struct cec_msg msg;

		short_options[idx] = 0;
		ch = getopt_long(argc, argv, short_options,
				 long_options, &option_index);
		if (ch == -1)
			break;

		cec_msg_init(&msg, 0, 0);
		msg.msg[0] = options[OptTo] ? to : 0xf0;
		options[ch] = 1;

		switch (ch) {
		case OptHelp:
			usage();
			return 0;
		case OptSetDevice:
			device = optarg;
			if (device[0] >= '0' && device[0] <= '9' && device.length() <= 3) {
				static char newdev[20];

				sprintf(newdev, "/dev/cec%s", optarg);
				device = newdev;
			}
			break;
		case OptSetDriver:
			driver = optarg;
			break;
		case OptSetAdapter:
			adapter = optarg;
			break;
		case OptVerbose:
			verbose = true;
			break;
		case OptFrom:
			from = strtoul(optarg, NULL, 0) & 0xf;
			break;
		case OptTo:
			to = strtoul(optarg, NULL, 0) & 0xf;
			if (first_to == 0xff)
				first_to = to;
			break;
		case OptTimeout:
			timeout = strtoul(optarg, NULL, 0);
			break;
		case OptMonitorTime:
			monitor_time = strtoul(optarg, NULL, 0);
			break;
		case OptIgnore: {
			bool all_la = !strncmp(optarg, "all", 3);
			bool all_opcodes = true;
			const char *sep = std::strchr(optarg, ',');
			unsigned la_mask = 0xffff, opcode, la = 0;

			if (sep)
				all_opcodes = !strncmp(sep + 1, "all", 3);
			if (!all_la) {
				la = strtoul(optarg, NULL, 0);

				if (la > 15) {
					fprintf(stderr, "invalid logical address (> 15)\n");
					usage();
					return 1;
				}
				la_mask = 1 << la;
			}
			if (!all_opcodes) {
				if (!strncmp(sep + 1, "poll", 4)) {
					opcode = POLL_FAKE_OPCODE;
				} else {
					opcode = strtoul(sep + 1, NULL, 0);
					if (opcode > 255) {
						fprintf(stderr, "invalid opcode (> 255)\n");
						usage();
						return 1;
					}
				}
				ignore_opcode[opcode] |= la_mask;
				break;
			}
			if (all_la && all_opcodes) {
				fprintf(stderr, "all,all is invalid\n");
				usage();
				return 1;
			}
			ignore_la[la] = true;
			break;
		}
		case OptStorePin:
			store_pin = optarg;
			break;
		case OptAnalyzePin:
			analyze_pin = optarg;
			break;
		case OptToggleNoReply:
			reply = !reply;
			break;
		case OptPhysAddr:
			phys_addr = cec_parse_phys_addr(optarg);
			break;
		case OptPhysAddrFromEDIDPoll:
			edid_path = optarg;
			fallthrough;
		case OptPhysAddrFromEDID:
			phys_addr = parse_phys_addr_from_edid(optarg);
			break;
		case OptOsdName:
			osd_name = optarg;
			break;
		case OptVendorID:
			vendor_id = strtoul(optarg, NULL, 0) & 0x00ffffff;
			break;
		case OptRcTVProfile1:
			rc_tv = CEC_OP_FEAT_RC_TV_PROFILE_1;
			break;
		case OptRcTVProfile2:
			rc_tv = CEC_OP_FEAT_RC_TV_PROFILE_2;
			break;
		case OptRcTVProfile3:
			rc_tv = CEC_OP_FEAT_RC_TV_PROFILE_3;
			break;
		case OptRcTVProfile4:
			rc_tv = CEC_OP_FEAT_RC_TV_PROFILE_4;
			break;
		case OptRcSrcDevRoot:
			rc_src |= CEC_OP_FEAT_RC_SRC_HAS_DEV_ROOT_MENU;
			break;
		case OptRcSrcDevSetup:
			rc_src |= CEC_OP_FEAT_RC_SRC_HAS_DEV_SETUP_MENU;
			break;
		case OptRcSrcContents:
			rc_src |= CEC_OP_FEAT_RC_SRC_HAS_CONTENTS_MENU;
			break;
		case OptRcSrcMediaTop:
			rc_src |= CEC_OP_FEAT_RC_SRC_HAS_MEDIA_TOP_MENU;
			break;
		case OptRcSrcMediaContext:
			rc_src |= CEC_OP_FEAT_RC_SRC_HAS_MEDIA_CONTEXT_MENU;
			break;
		case OptFeatRecordTVScreen:
			dev_features |= CEC_OP_FEAT_DEV_HAS_RECORD_TV_SCREEN;
			break;
		case OptFeatSetOSDString:
			dev_features |= CEC_OP_FEAT_DEV_HAS_SET_OSD_STRING;
			break;
		case OptFeatDeckControl:
			dev_features |= CEC_OP_FEAT_DEV_HAS_DECK_CONTROL;
			break;
		case OptFeatSetAudioRate:
			dev_features |= CEC_OP_FEAT_DEV_HAS_SET_AUDIO_RATE;
			break;
		case OptFeatSinkHasARCTx:
			dev_features |= CEC_OP_FEAT_DEV_SINK_HAS_ARC_TX;
			break;
		case OptFeatSourceHasARCRx:
			dev_features |= CEC_OP_FEAT_DEV_SOURCE_HAS_ARC_RX;
			break;
		case OptPlayback:
		case OptRecord:
			if (options[OptPlayback] && options[OptRecord]) {
				fprintf(stderr, "--playback and --record cannot be combined.\n\n");
				usage();
				return 1;
			}
			break;
		case OptSwitch:
		case OptCDCOnly:
		case OptUnregistered:
			if (options[OptCDCOnly] + options[OptUnregistered] + options[OptSwitch] > 1) {
				fprintf(stderr, "--switch, --cdc-only and --unregistered cannot be combined.\n\n");
				usage();
				return 1;
			}
			break;
		case ':':
			fprintf(stderr, "Option '%s' requires a value\n\n",
				argv[optind]);
			usage();
			return 1;
		case '?':
			if (argv[optind])
				fprintf(stderr, "Unknown argument '%s'\n\n", argv[optind]);
			usage();
			return 1;
		case OptVendorCommand: {
			static const char *arg_names[] = {
				"payload",
				NULL
			};
			char *value, *endptr, *subs = optarg;
			__u8 size = 0;
			__u8 bytes[14];

			while (*subs != '\0') {
				switch (cec_parse_subopt(&subs, arg_names, &value)) {
				case 0:
					while (size < sizeof(bytes)) {
						bytes[size++] = strtol(value, &endptr, 0L);
						if (endptr == value) {
							size--;
							break;
						}
						value = std::strchr(value, ':');
						if (value == NULL)
							break;
						value++;
					}
					break;
				default:
					std::exit(EXIT_FAILURE);
				}
			}
			if (size) {
				cec_msg_vendor_command(&msg, size, bytes);
				msgs.push_back(msg);
			}
			break;
		}
		case OptCustomCommand: {
			static const char *arg_names[] = {
				"cmd",
				"payload",
				NULL
			};
			char *value, *endptr, *subs = optarg;
			bool have_cmd = false;
			__u8 cmd = 0;
			__u8 size = 0;
			__u8 bytes[14];

			while (*subs != '\0') {
				switch (cec_parse_subopt(&subs, arg_names, &value)) {
				case 0:
					cmd = strtol(value, &endptr, 0L);
					have_cmd = true;
					break;
				case 1:
					while (size < sizeof(bytes)) {
						bytes[size++] = strtol(value, &endptr, 0L);
						if (endptr == value) {
							size--;
							break;
						}
						value = std::strchr(value, ':');
						if (value == NULL)
							break;
						value++;
					}
					break;
				default:
					std::exit(EXIT_FAILURE);
				}
			}
			if (have_cmd) {
				msg.len = 2 + size;
				msg.msg[1] = cmd;
				memcpy(msg.msg + 2, bytes, size);
				msgs.push_back(msg);
			}
			break;
		}
		case OptVendorCommandWithID: {
			static const char *arg_names[] = {
				"vendor-id",
				"cmd",
				NULL
			};
			char *value, *endptr, *subs = optarg;
			__u32 vendor_id = 0;
			__u8 size = 0;
			__u8 bytes[11];

			while (*subs != '\0') {
				switch (cec_parse_subopt(&subs, arg_names, &value)) {
				case 0:
					vendor_id = strtol(value, 0L, 0);
					break;
				case 1:
					while (size < sizeof(bytes)) {
						bytes[size++] = strtol(value, &endptr, 0L);
						if (endptr == value) {
							size--;
							break;
						}
						value = std::strchr(value, ':');
						if (value == NULL)
							break;
						value++;
					}
					break;
				default:
					std::exit(EXIT_FAILURE);
				}
			}
			if (size) {
				cec_msg_vendor_command_with_id(&msg, vendor_id, size, bytes);
				msgs.push_back(msg);
			}
			break;
		}
		case OptVendorRemoteButtonDown: {
			static const char *arg_names[] = {
				"rc-code",
				NULL
			};
			char *value, *endptr, *subs = optarg;
			__u8 size = 0;
			__u8 bytes[14];

			while (*subs != '\0') {
				switch (cec_parse_subopt(&subs, arg_names, &value)) {
				case 0:
					while (size < sizeof(bytes)) {
						bytes[size++] = strtol(value, &endptr, 0L);
						if (endptr == value) {
							size--;
							break;
						}
						value = std::strchr(value, ':');
						if (value == NULL)
							break;
						value++;
					}
					break;
				default:
					std::exit(EXIT_FAILURE);
				}
			}
			if (size) {
				cec_msg_vendor_remote_button_down(&msg, size, bytes);
				msgs.push_back(msg);
			}
			break;
		}
		case OptPoll:
			msgs.push_back(msg);
			break;

		case OptListDevices:
			list_devices();
			break;

		case OptTestPowerCycle: {
			static const char *arg_names[] = {
				"polls",
				"sleep",
				NULL
			};
			char *value, *subs = optarg;

			warn_if_unconfigured = true;
			if (!optarg)
				break;

			while (*subs != '\0') {
				switch (cec_parse_subopt(&subs, arg_names, &value)) {
				case 0:
					test_pwr_cycle_polls = strtoul(value, 0L, 0);
					break;
				case 1:
					test_pwr_cycle_sleep = strtoul(value, 0L, 0);
					break;
				default:
					std::exit(EXIT_FAILURE);
				}
			}
			break;
		}

		case OptStressTestPowerCycle: {
			static const char *arg_names[] = {
				"cnt",
				"min-sleep",
				"max-sleep",
				"seed",
				"repeats",
				"sleep-before-on",
				"sleep-before-off",
				"polls",
				NULL
			};
			char *value, *subs = optarg;

			while (*subs != '\0') {
				switch (cec_parse_subopt(&subs, arg_names, &value)) {
				case 0:
					stress_test_pwr_cycle_cnt = strtoul(value, 0L, 0);
					break;
				case 1:
					stress_test_pwr_cycle_min_sleep = strtoul(value, 0L, 0);
					break;
				case 2:
					stress_test_pwr_cycle_max_sleep = strtoul(value, 0L, 0);
					break;
				case 3:
					stress_test_pwr_cycle_has_seed = true;
					stress_test_pwr_cycle_seed = strtoul(value, 0L, 0);
					break;
				case 4:
					stress_test_pwr_cycle_repeats = strtoul(value, 0L, 0);
					break;
				case 5:
					stress_test_pwr_cycle_sleep_before_on = strtod(value, NULL);
					break;
				case 6:
					stress_test_pwr_cycle_sleep_before_off = strtod(value, NULL);
					break;
				case 7:
					stress_test_pwr_cycle_polls = strtoul(value, 0L, 0);
					break;
				default:
					std::exit(EXIT_FAILURE);
				}
			}
			if (stress_test_pwr_cycle_min_sleep > stress_test_pwr_cycle_max_sleep) {
				fprintf(stderr, "min-sleep > max-sleep\n");
				std::exit(EXIT_FAILURE);
			}
			warn_if_unconfigured = true;
			break;
		}

		default:
			if (ch >= OptHelpAll) {
				cec_parse_usage_options(options);
				std::exit(EXIT_SUCCESS);
			}
			if (ch <= OptMessages)
				break;
			opt = cec_log_msg_args(ch - OptMessages - 1);
			cec_parse_msg_args(msg, reply, opt, ch);
			msgs.push_back(msg);
			break;
		}
	}
	if (optind < argc) {
		fprintf(stderr, "unknown arguments: ");
		while (optind < argc)
			fprintf(stderr, "%s ", argv[optind++]);
		fprintf(stderr, "\n");
		usage();
		return 1;
	}

	if (!msgs.empty())
		warn_if_unconfigured = true;

	if (store_pin && analyze_pin) {
		fprintf(stderr, "--store-pin and --analyze-pin options cannot be combined.\n\n");
		usage();
		return 1;
	}

	if (analyze_pin && options[OptSetDevice]) {
		fprintf(stderr, "--device and --analyze-pin options cannot be combined.\n\n");
		usage();
		return 1;
	}

	if (analyze_pin) {
		analyze(analyze_pin);
		return 0;
	}

	if (options[OptWallClock] && !options[OptMonitorPin])
		verbose = true;

	if (store_pin && !strcmp(store_pin, "-"))
		options[OptSkipInfo] = 1;

	if (rc_tv && rc_src) {
		fprintf(stderr, "--rc-tv- and --rc-src- options cannot be combined.\n\n");
		usage();
		return 1;
	}

	if (rc_tv && !options[OptTV]) {
		fprintf(stderr, "--rc-tv- can only be used in combination with --tv.\n\n");
		usage();
		return 1;
	}

	if (rc_src && options[OptTV]) {
		fprintf(stderr, "--rc-src- can't be used in combination with --tv.\n\n");
		usage();
		return 1;
	}

	if ((dev_features & (CEC_OP_FEAT_DEV_SOURCE_HAS_ARC_RX |
			     CEC_OP_FEAT_DEV_HAS_DECK_CONTROL |
			     CEC_OP_FEAT_DEV_HAS_SET_AUDIO_RATE)) && options[OptTV]) {
		fprintf(stderr, "--feat-deck-control, --feat-set-audio-rate and --feat-source-has-arc-rx cannot be used in combination with --tv.\n\n");
		usage();
		return 1;
	}

	if ((dev_features & (CEC_OP_FEAT_DEV_HAS_RECORD_TV_SCREEN |
			     CEC_OP_FEAT_DEV_HAS_SET_OSD_STRING)) && !options[OptTV]) {
		fprintf(stderr, "--feat-set-osd-string and --feat-record-tv-screen can only be used in combination with --tv.\n\n");
		usage();
		return 1;
	}

	if (device.empty() && (driver || adapter)) {
		device = cec_device_find(driver, adapter);
		if (device.empty()) {
			fprintf(stderr,
				"Could not find a CEC device for the given driver/adapter combination\n");
			std::exit(EXIT_FAILURE);
		}
	}
	if (device.empty())
		device = "/dev/cec0";

	clock_gettime(CLOCK_MONOTONIC, &start_monotonic);
	gettimeofday(&start_timeofday, NULL);

	if ((fd = open(device.c_str(), O_RDWR)) < 0) {
		fprintf(stderr, "Failed to open %s: %s\n", device.c_str(),
			strerror(errno));
		std::exit(EXIT_FAILURE);
	}

	struct node node;
	struct cec_caps caps = { };

	node.fd = fd;
	node.device = device.c_str();
	doioctl(&node, CEC_ADAP_G_CAPS, &caps);
	node.caps = caps.capabilities;
	node.available_log_addrs = caps.available_log_addrs;

	bool set_phys_addr = options[OptPhysAddr] || options[OptPhysAddrFromEDID] ||
			     options[OptPhysAddrFromEDIDPoll];

	if (set_phys_addr && !(node.caps & CEC_CAP_PHYS_ADDR))
		fprintf(stderr, "The CEC adapter doesn't allow setting the physical address manually, ignore this option.\n\n");

	unsigned flags = 0;

	if (options[OptOsdName])
		;
	else if (options[OptTV])
		osd_name = "TV";
	else if (options[OptRecord])
		osd_name = "Record";
	else if (options[OptPlayback])
		osd_name = "Playback";
	else if (options[OptTuner])
		osd_name = "Tuner";
	else if (options[OptAudio])
		osd_name = "Audio System";
	else if (options[OptProcessor])
		osd_name = "Processor";
	else if (options[OptSwitch] || options[OptCDCOnly] || options[OptUnregistered])
		osd_name = "";
	else
		osd_name = "TV";

	if (options[OptTV])
		flags |= 1 << CEC_OP_PRIM_DEVTYPE_TV;
	if (options[OptRecord])
		flags |= 1 << CEC_OP_PRIM_DEVTYPE_RECORD;
	if (options[OptTuner])
		flags |= 1 << CEC_OP_PRIM_DEVTYPE_TUNER;
	if (options[OptPlayback])
		flags |= 1 << CEC_OP_PRIM_DEVTYPE_PLAYBACK;
	if (options[OptAudio])
		flags |= 1 << CEC_OP_PRIM_DEVTYPE_AUDIOSYSTEM;
	if (options[OptProcessor])
		flags |= 1 << CEC_OP_PRIM_DEVTYPE_PROCESSOR;
	if (options[OptSwitch] || options[OptCDCOnly] || options[OptUnregistered])
		flags |= 1 << CEC_OP_PRIM_DEVTYPE_SWITCH;

	if (flags == 0 && (rc_tv || rc_src || dev_features)) {
		fprintf(stderr, "The --feat- and --rc- options can only be used in combination with selecting the device type.\n\n");
		usage();
		return 1;
	}

	bool set_log_addrs = (node.caps & CEC_CAP_LOG_ADDRS) && flags;
	bool clear_log_addrs = (node.caps & CEC_CAP_LOG_ADDRS) && options[OptClear];

	// When setting both PA and LA it is best to clear the LAs first.
	if (clear_log_addrs || (set_phys_addr && set_log_addrs)) {
		struct cec_log_addrs laddrs = { };

		doioctl(&node, CEC_ADAP_S_LOG_ADDRS, &laddrs);
	}

	if (set_phys_addr)
		doioctl(&node, CEC_ADAP_S_PHYS_ADDR, &phys_addr);

	doioctl(&node, CEC_ADAP_G_PHYS_ADDR, &phys_addr);

	if (set_log_addrs) {
		struct cec_log_addrs laddrs = {};
		__u8 all_dev_types = 0;
		__u8 prim_type = 0xff;

		doioctl(&node, CEC_ADAP_S_LOG_ADDRS, &laddrs);
		memset(&laddrs, 0, sizeof(laddrs));

		laddrs.cec_version = options[OptCECVersion1_4] ?
			CEC_OP_CEC_VERSION_1_4 : CEC_OP_CEC_VERSION_2_0;
		strncpy(laddrs.osd_name, osd_name, sizeof(laddrs.osd_name));
		laddrs.osd_name[sizeof(laddrs.osd_name) - 1] = 0;
		laddrs.vendor_id = vendor_id;
		if (options[OptAllowUnregFallback])
			laddrs.flags |= CEC_LOG_ADDRS_FL_ALLOW_UNREG_FALLBACK;
		if (!options[OptNoRC] && flags != (1 << CEC_OP_PRIM_DEVTYPE_SWITCH))
			laddrs.flags |= CEC_LOG_ADDRS_FL_ALLOW_RC_PASSTHRU;
		if (options[OptCDCOnly])
			laddrs.flags |= CEC_LOG_ADDRS_FL_CDC_ONLY;

		for (unsigned i = 0; i < 8; i++) {
			unsigned la_type;

			if (!(flags & (1 << i)))
				continue;
			if (prim_type == 0xff)
				prim_type = i;
			if (laddrs.num_log_addrs == node.available_log_addrs) {
				fprintf(stderr, "Attempt to define too many logical addresses\n");
				std::exit(EXIT_FAILURE);
			}
			switch (i) {
			case CEC_OP_PRIM_DEVTYPE_TV:
				la_type = CEC_LOG_ADDR_TYPE_TV;
				all_dev_types |= CEC_OP_ALL_DEVTYPE_TV;
				prim_type = i;
				break;
			case CEC_OP_PRIM_DEVTYPE_RECORD:
				la_type = CEC_LOG_ADDR_TYPE_RECORD;
				all_dev_types |= CEC_OP_ALL_DEVTYPE_RECORD;
				break;
			case CEC_OP_PRIM_DEVTYPE_TUNER:
				la_type = CEC_LOG_ADDR_TYPE_TUNER;
				all_dev_types |= CEC_OP_ALL_DEVTYPE_TUNER;
				break;
			case CEC_OP_PRIM_DEVTYPE_PLAYBACK:
				la_type = CEC_LOG_ADDR_TYPE_PLAYBACK;
				all_dev_types |= CEC_OP_ALL_DEVTYPE_PLAYBACK;
				break;
			case CEC_OP_PRIM_DEVTYPE_AUDIOSYSTEM:
				la_type = CEC_LOG_ADDR_TYPE_AUDIOSYSTEM;
				all_dev_types |= CEC_OP_ALL_DEVTYPE_AUDIOSYSTEM;
				if (prim_type != CEC_OP_PRIM_DEVTYPE_TV)
					prim_type = i;
				break;
			case CEC_OP_PRIM_DEVTYPE_PROCESSOR:
				la_type = CEC_LOG_ADDR_TYPE_SPECIFIC;
				all_dev_types |= CEC_OP_ALL_DEVTYPE_SWITCH;
				break;
			case CEC_OP_PRIM_DEVTYPE_SWITCH:
			default:
				la_type = CEC_LOG_ADDR_TYPE_UNREGISTERED;
				all_dev_types |= CEC_OP_ALL_DEVTYPE_SWITCH;
				break;
			}
			laddrs.log_addr_type[laddrs.num_log_addrs++] = la_type;
		}
		for (unsigned i = 0; i < laddrs.num_log_addrs; i++) {
			laddrs.primary_device_type[i] = prim_type;
			laddrs.all_device_types[i] = all_dev_types;
			laddrs.features[i][0] = rc_tv | rc_src;
			laddrs.features[i][1] = dev_features;
		}

		doioctl(&node, CEC_ADAP_S_LOG_ADDRS, &laddrs);
	}

	struct cec_log_addrs laddrs = { };
	doioctl(&node, CEC_ADAP_G_LOG_ADDRS, &laddrs);
	node.num_log_addrs = laddrs.num_log_addrs;
	node.log_addr_mask = laddrs.log_addr_mask;
	node.phys_addr = phys_addr;

	for (i = 0; i < laddrs.num_log_addrs; i++) {
		__u8 la = laddrs.log_addr[i];

		if (la != CEC_LOG_ADDR_INVALID)
			phys_addrs[la] = (phys_addr << 8) | la;
	}

	if (!options[OptSkipInfo]) {
		struct cec_connector_info conn_info = {};

		doioctl(&node, CEC_ADAP_G_CONNECTOR_INFO, &conn_info);

		cec_driver_info(caps, laddrs, phys_addr, conn_info);
	}

	if (node.num_log_addrs == 0) {
		if (options[OptMonitor] || options[OptMonitorAll] ||
		    options[OptMonitorPin] || options[OptStorePin] ||
		    options[OptPhysAddrFromEDIDPoll])
			goto skip_la;
		if (warn_if_unconfigured)
			fprintf(stderr, "\nAdapter is unconfigured, please configure it first.\n");
		return 0;
	}
	if (!options[OptSkipInfo])
		printf("\n");

	if (!options[OptFrom])
		from = laddrs.log_addr[0] & 0xf;

	if (options[OptShowTopology])
		showTopology(&node);

	if (options[OptLogicalAddress])
		printf("%d\n", laddrs.log_addr[0] & 0xf);
	if (options[OptLogicalAddresses]) {
		for (i = 0; i < laddrs.num_log_addrs; i++)
			printf("%d ", laddrs.log_addr[i] & 0xf);
		printf("\n");
	}

	if (options[OptWaitForMsgs]) {
		__u32 monitor = CEC_MODE_INITIATOR | CEC_MODE_FOLLOWER;

		if (doioctl(&node, CEC_S_MODE, &monitor)) {
			fprintf(stderr, "Selecting follower mode failed.\n");
			return 1;
		}
	}
	if (options[OptNonBlocking])
		fcntl(node.fd, F_SETFL, fcntl(node.fd, F_GETFL) | O_NONBLOCK);

	for (msg_vec::iterator iter = msgs.begin(); iter != msgs.end(); ++iter) {
		struct cec_msg msg = *iter;

		fflush(stdout);
		if (!cec_msg_is_broadcast(&msg) && !options[OptTo]) {
			fprintf(stderr, "attempting to send message without --to\n");
			std::exit(EXIT_FAILURE);
		}
		if (msg.msg[0] == 0xf0)
			msg.msg[0] = first_to;
		msg.msg[0] &= 0x0f;
		msg.msg[0] |= from << 4;
		to = msg.msg[0] & 0xf;
		printf("\nTransmit from %s to %s (%d to %d):\n",
		       cec_la2s(from), to == 0xf ? "all" : cec_la2s(to), from, to);
		msg.flags = options[OptReplyToFollowers] ? CEC_MSG_FL_REPLY_TO_FOLLOWERS : 0;
		msg.flags |= options[OptRawMsg] ? CEC_MSG_FL_RAW : 0;
		msg.timeout = msg.reply ? timeout : 0;
		cec_log_msg(&msg);
		if (doioctl(&node, CEC_TRANSMIT, &msg))
			continue;
		if (msg.rx_status & (CEC_RX_STATUS_OK | CEC_RX_STATUS_FEATURE_ABORT)) {
			printf("    Received from %s (%d):\n    ", cec_la2s(cec_msg_initiator(&msg)),
			       cec_msg_initiator(&msg));
			cec_log_msg(&msg);
			if (options[OptShowRaw])
				log_raw_msg(&msg);
		}
		printf("\tSequence: %u Tx Timestamp: %s",
			msg.sequence, ts2s(msg.tx_ts).c_str());
		if (msg.rx_ts)
			printf(" Rx Timestamp: %s\n\tApproximate response time: %u ms",
				ts2s(msg.rx_ts).c_str(),
				response_time_ms(msg));
		printf("\n");
		if (!cec_msg_status_is_ok(&msg) || verbose)
			printf("\t%s\n", cec_status2s(msg).c_str());
	}
	fflush(stdout);

	if (options[OptNonBlocking])
		fcntl(node.fd, F_SETFL, fcntl(node.fd, F_GETFL) & ~O_NONBLOCK);

	if (options[OptTestPowerCycle])
		test_power_cycle(node, test_pwr_cycle_polls, test_pwr_cycle_sleep);
	if (options[OptStressTestPowerCycle])
		stress_test_power_cycle(node, stress_test_pwr_cycle_cnt,
					stress_test_pwr_cycle_min_sleep,
					stress_test_pwr_cycle_max_sleep,
					stress_test_pwr_cycle_polls,
					stress_test_pwr_cycle_has_seed,
					stress_test_pwr_cycle_seed,
					stress_test_pwr_cycle_repeats,
					stress_test_pwr_cycle_sleep_before_on,
					stress_test_pwr_cycle_sleep_before_off);

skip_la:
	if (options[OptPhysAddrFromEDIDPoll]) {
		bool has_edid;
		char dummy;
		int fd;

		fd = open(edid_path, O_RDONLY);
		if (fd < 0)
			std::exit(EXIT_FAILURE);
		lseek(fd, 0, SEEK_SET);
		has_edid = read(fd, &dummy, 1) > 0;

		if (!has_edid)
			phys_addr = CEC_PHYS_ADDR_INVALID;
		else
			phys_addr = parse_phys_addr_from_edid(edid_path);
		doioctl(&node, CEC_ADAP_S_PHYS_ADDR, &phys_addr);
		printf("Physical Address: %x.%x.%x.%x\n", cec_phys_addr_exp(phys_addr));

		for (;;) {
			bool edid;

			/* Poll every 100 ms */
			usleep(100000);
			lseek(fd, 0, SEEK_SET);
			edid = read(fd, &dummy, 1) > 0;
			if (has_edid != edid) {
				has_edid = edid;
				if (!edid)
					phys_addr = CEC_PHYS_ADDR_INVALID;
				else
					phys_addr = parse_phys_addr_from_edid(edid_path);
				doioctl(&node, CEC_ADAP_S_PHYS_ADDR, &phys_addr);
				printf("Physical Address: %x.%x.%x.%x\n",
				       cec_phys_addr_exp(phys_addr));
			}
		}
	}

	if (options[OptMonitor] || options[OptMonitorAll] ||
	    options[OptMonitorPin] || options[OptStorePin])
		monitor(node, monitor_time, store_pin);
	else if (options[OptWaitForMsgs])
		wait_for_msgs(node, monitor_time);
	fflush(stdout);
	close(fd);
	return 0;
}
