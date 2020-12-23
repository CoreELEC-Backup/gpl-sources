/*********************************************************************

    formats/kc85_dsk.h

    kc85 format

*********************************************************************/

#ifndef KC85_DSK_H_
#define KC85_DSK_H_

#include "upd765_dsk.h"

class kc85_format : public upd765_format {
public:
	kc85_format();

	virtual const char *name() const;
	virtual const char *description() const;
	virtual const char *extensions() const;

private:
	static const format formats[];
};

extern const floppy_format_type FLOPPY_KC85_FORMAT;

#endif
