/*********************************************************************

    formats/m5_dsk.h

    sord m5 format

*********************************************************************/

#ifndef M5_DSK_H_
#define M5_DSK_H_

#include "upd765_dsk.h"

class m5_format : public upd765_format {
public:
	m5_format();

	virtual const char *name() const;
	virtual const char *description() const;
	virtual const char *extensions() const;

private:
	static const format formats[];
};

extern const floppy_format_type FLOPPY_M5_FORMAT;

#endif
