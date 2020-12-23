/* test_dvr.c - Test recording a TS from the dvr device.
 * usage: test_dvr PID [more PIDs...]
 *
 * Copyright (C) 2003 convergence GmbH
 * Johannes Stezenbach <js@convergence.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

//hm, I haven't tested writing large files yet... maybe it works
#define _LARGEFILE64_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <inttypes.h>

#include <linux/dvb/dmx.h>

static unsigned long BUF_SIZE = 64 * 1024;
static unsigned long long total_bytes;

static void usage(void)
{
	fprintf(stderr, "usage: test_dvb file PID [more PIDs...]\n"
			"       record a partial TS stream consisting of TS packets\n"
			"       with the selected PIDs to the given file.\n"
			"       Use PID 0x2000 to record the full TS stream (if\n"
			"       the hardware supports it).\n"
			"       The demux and dvr devices used can be changed by setting\n"
			"       the environment variables DEMUX and DVR.\n"
			"       You can override the input buffer size by setting BUF_SIZE to\n"
			"       the number of bytes wanted.\n"
			"       Note: There is no output buffering, so writing to stdout is\n"
			"       not really supported, but you can try something like:\n"
			"       BUF_SIZE=188 ./test_dvr /dev/stdout 0 2>/dev/null | xxd\n"
			"       ./test_dvr /dev/stdout 0x100 0x110 2>/dev/null| xine stdin://mpeg2\n"
			"\n");
	exit(1);
}


static void process_data(int dvrfd, int tsfd)
{
	uint8_t buf[BUF_SIZE];
	int bytes, b2;

	bytes = read(dvrfd, buf, sizeof(buf));
	if (bytes < 0) {
		perror("read");
		if (errno == EOVERFLOW)
			return;
		fprintf(stderr, "exiting due to read() error\n");
		exit(1);
	}
	total_bytes += bytes;
	b2 = write(tsfd, buf, bytes);
	if (b2 == -1) {
		perror("write");
		exit(1);
	} else if (b2 < bytes)
		fprintf(stderr, "warning: read %d, but wrote only %d bytes\n", bytes, b2);
	else
		fprintf(stderr, "got %d bytes (%llu total)\n", bytes, total_bytes);
}

static int add_filter(unsigned int pid, const char* dmxdev)
{
	int fd;
	struct dmx_pes_filter_params f;

	fd = open(dmxdev, O_RDONLY);
	if (fd == -1) {
		perror("cannot open demux device");
		return 1;
	}

	memset(&f, 0, sizeof(f));
	f.pid = (uint16_t) pid;
	f.input = DMX_IN_FRONTEND;
	f.output = DMX_OUT_TS_TAP;
	f.pes_type = DMX_PES_OTHER;
	f.flags   = DMX_IMMEDIATE_START;

	if (ioctl(fd, DMX_SET_PES_FILTER, &f) == -1) {
		perror("DMX_SET_PES_FILTER");
		return 1;
	}
	fprintf(stderr, "set filter for PID 0x%04x on fd %d\n", pid, fd);

	//FIXME: don't leak the fd
	return 0;
}

int main(int argc, char *argv[])
{
	int dvrfd, tsfd;
	unsigned int pid;
	char *dmxdev = "/dev/dvb/adapter0/demux0";
	char *dvrdev = "/dev/dvb/adapter0/dvr0";
	int i;
	char *chkp;

	if (argc < 3)
		usage();

	if (getenv("DEMUX"))
		dmxdev = getenv("DEMUX");
	if (getenv("DVR"))
		dvrdev = getenv("DVR");

	fprintf(stderr, "using '%s' and '%s'\n"
		"writing to '%s'\n", dmxdev, dvrdev, argv[1]);
	tsfd = open(argv[1], O_WRONLY | O_CREAT | O_TRUNC | O_LARGEFILE, 0664);
	if (tsfd == -1) {
		perror("cannot write output file");
		return 1;
	}

	dvrfd = open(dvrdev, O_RDONLY);
	if (dvrfd == -1) {
		perror("cannot open dvr device");
		return 1;
	}

	if (getenv("BUF_SIZE") && ((BUF_SIZE = strtoul(getenv("BUF_SIZE"), NULL, 0)) > 0))
		fprintf(stderr, "BUF_SIZE = %lu\n", BUF_SIZE);

	for (i = 2; i < argc; i++) {
		pid = strtoul(argv[i], &chkp, 0);
		if (pid > 0x2000 || chkp == argv[i])
			usage();
		fprintf(stderr, "adding filter for PID 0x%04x\n", pid);
		//FIXME: stop & close filters later...
		if (add_filter(pid, dmxdev) != 0)
			return 1;
	}

	for (;;) {
		process_data(dvrfd, tsfd);
	}

	close(dvrfd);
	return 0;
}
