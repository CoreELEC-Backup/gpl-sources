/*********************************************************************

    formats/iq151_dsk.h

    iq151 format

*********************************************************************/

#ifndef IQ151_DSK_H_
#define IQ151_DSK_H_

#include "upd765_dsk.h"

class iq151_format : public upd765_format {
public:
	iq151_format();

	virtual const char *name() const;
	virtual const char *description() const;
	virtual const char *extensions() const;

private:
	static const format formats[];
};

extern const floppy_format_type FLOPPY_IQ151_FORMAT;

#endif
