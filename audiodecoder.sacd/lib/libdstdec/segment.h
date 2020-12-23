/*
* Direct Stream Transfer (DST) codec
* ISO/IEC 14496-3 Part 3 Subpart 10: Technical description of lossless coding of oversampled audio
*/

#ifndef SEGMENT_H
#define SEGMENT_H

#include <stdint.h>
#include <array>
#include <vector>
#include "consts.h"

using std::array;
using std::vector;

namespace dst
{

class segment_t {
public:
	int Resolution;                                 // Resolution for segments
	vector<array<int, MAXNROF_SEGS>> SegmentLength; // SegmentLength[ChNr][SegmentNr]
	vector<int> NrOfSegments;                       // NrOfSegments[ChNr]
	vector<array<int, MAXNROF_SEGS>> Table4Segment; // Table4Segment[ChNr][SegmentNr]
public:
	void init(int channels) {
		SegmentLength.resize(channels);
		NrOfSegments.resize(channels);
		Table4Segment.resize(channels);
	}
};

}

#endif
