/*********************************************************************

    formats/a5105_dsk.h

    a5105 format

*********************************************************************/

#ifndef A5105_DSK_H_
#define A5105_DSK_H_

#include "upd765_dsk.h"

class a5105_format : public upd765_format {
public:
	a5105_format();

	virtual const char *name() const;
	virtual const char *description() const;
	virtual const char *extensions() const;

private:
	static const format formats[];
};

extern const floppy_format_type FLOPPY_A5105_FORMAT;

#endif
