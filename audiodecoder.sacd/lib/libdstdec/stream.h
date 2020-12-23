/*
* Direct Stream Transfer (DST) codec
* ISO/IEC 14496-3 Part 3 Subpart 10: Technical description of lossless coding of oversampled audio
*/

#ifndef STREAM_H
#define STREAM_H

#include <stdint.h>
#include <algorithm>
#include "common.h"
#include "consts.h"

using std::min;

namespace dst
{

class stream_t {
	const uint8_t* m_data;
	int            m_size;
	int            m_offset;
public:
	stream_t() {
		m_data = nullptr;
		m_size = 0;
		m_offset = 0;
	}

	void set_data(const uint8_t* data, size_t size) {
		m_data = data;
		m_size = size;
		m_offset = 0;
	}

	int get_offset() {
		return m_offset;
	}

	int get_bit() {
		if (m_offset + 1 > 8 * m_size) {
			kodiLog(ADDON_LOG_ERROR, "ERROR: read after end of stream");
			return 0;
		}
		uint32_t value = m_data[m_offset / 8];
		value >>= 7 - m_offset % 8;
		value &= 1;
		m_offset++;
		return value;
	}

	int get_sint(int length) {
		int value = (int)get_uint(length);
		if (value >= (1 << (length - 1))) {
			value -= (1 << length);
		}
		return value;
	}

	uint32_t get_uint(int length) {
		if (m_offset + length > 8 * m_size) {
			kodiLog(ADDON_LOG_ERROR, "ERROR: read after end of stream");
			return 0;
		}
		uint32_t value = 0;
		for (auto i = 0; i < (m_offset % 8 + length + 7) / 8; i++) {
			value = (value << 8) | m_data[m_offset / 8 + i];
		}
		value >>= 7 - (m_offset + length - 1) % 8;
		value &= (1 << length) - 1;
		m_offset += length;
		return value;
	}

};

}

#endif
