/*
 *  $Id: common.c,v 1.12 2010/07/19 13:49:24 schmirl Exp $
 */
 
#include <vdr/channels.h>
#include <iostream>

#include "common.h"
#include "tools/select.h"

using namespace std;

const char *VERSION = "0.6.1-git";

const char cMenuEditIpItem::IpCharacters[] = "0123456789.";

cMenuEditIpItem::cMenuEditIpItem(const char *Name, char *Value):
		cMenuEditItem(Name) {
	value = Value;
	curNum = -1;
	pos = -1;
	step = false;
	Set();
}

cMenuEditIpItem::~cMenuEditIpItem() {
}

void cMenuEditIpItem::Set(void) {
	char buf[1000];
	if (pos >= 0) {
		in_addr_t addr = inet_addr(value);
		if ((int)addr == -1)
			addr = 0;
		int p = 0;
		for (int i = 0; i < 4; ++i) {
			p += snprintf(buf + p, sizeof(buf) - p, pos == i ? "[%d]" : "%d", 
					pos == i ? curNum : (addr >> (i * 8)) & 0xff);
			if (i < 3)
				buf[p++] = '.';
		}
		SetValue(buf);
	} else
		SetValue(value);
}

eOSState cMenuEditIpItem::ProcessKey(eKeys Key) {
	in_addr addr;
	addr.s_addr = inet_addr(value);
	if ((int)addr.s_addr == -1)
		addr.s_addr = 0;

	switch (Key) {
	case kUp:
		if (pos >= 0) {
			if (curNum < 255) ++curNum;
		} else
			return cMenuEditItem::ProcessKey(Key);
		break;

	case kDown:
		if (pos >= 0) {
			if (curNum > 0) --curNum;
		} else
			return cMenuEditItem::ProcessKey(Key);
		break;

	case kOk:
		if (pos >= 0) {
			addr.s_addr = inet_addr(value);
			if ((int)addr.s_addr == -1)
				addr.s_addr = 0;
			addr.s_addr &= ~(0xff << (pos * 8));
			addr.s_addr |= curNum << (pos * 8);
			strcpy(value, inet_ntoa(addr));
		} else
			return cMenuEditItem::ProcessKey(Key);
		curNum = -1;
		pos = -1;
		break;
		
	case kRight:
		if (pos >= 0) {
			addr.s_addr = inet_addr(value);
			if ((int)addr.s_addr == -1)
				addr.s_addr = 0;
			addr.s_addr &= ~(0xff << (pos * 8));
			addr.s_addr |= curNum << (pos * 8);
			strcpy(value, inet_ntoa(addr));
		}

		if (pos == -1 || pos == 3)
			pos = 0;
		else
			++pos;

		curNum = (addr.s_addr >> (pos * 8)) & 0xff;
		step = true;
		break;

	case kLeft:
		if (pos >= 0) {
			addr.s_addr = inet_addr(value);
			if ((int)addr.s_addr == -1)
				addr.s_addr = 0;
			addr.s_addr &= ~(0xff << (pos * 8));
			addr.s_addr |= curNum << (pos * 8);
			strcpy(value, inet_ntoa(addr));
		}

		if (pos <= 0)
			pos = 3;
		else
			--pos;

		curNum = (addr.s_addr >> (pos * 8)) & 0xff;
		step = true;
		break;

	case k0 ... k9:
		if (pos == -1)
			pos = 0;

		if (curNum == -1 || step) {
			curNum = Key - k0;
			step = false;
		} else
			curNum = curNum * 10 + (Key - k0);

		if ((curNum * 10 > 255) || (curNum == 0)) {
			in_addr addr;
			addr.s_addr = inet_addr(value);
			if ((int)addr.s_addr == -1)
				addr.s_addr = 0;
			addr.s_addr &= ~(0xff << (pos * 8));
			addr.s_addr |= curNum << (pos * 8);
			strcpy(value, inet_ntoa(addr));
			if (++pos == 4)
				pos = 0;
			curNum = (addr.s_addr >> (pos * 8)) & 0xff;
			step = true;
		}
		break;

	default:
		return cMenuEditItem::ProcessKey(Key);
	}

	Set();
	return osContinue;
}

