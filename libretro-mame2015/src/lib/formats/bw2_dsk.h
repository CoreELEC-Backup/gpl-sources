/*********************************************************************

    formats/bw2_dsk.h

    Bondwell 2 format

*********************************************************************/

#ifndef BW2_DSK_H_
#define BW2_DSK_H_

#include "upd765_dsk.h"

class bw2_format : public upd765_format {
public:
	bw2_format();

	virtual const char *name() const;
	virtual const char *description() const;
	virtual const char *extensions() const;

private:
	static const format formats[];
};

extern const floppy_format_type FLOPPY_BW2_FORMAT;

#endif
