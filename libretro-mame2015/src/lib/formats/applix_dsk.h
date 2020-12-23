/*********************************************************************

    formats/applix_dsk.h

    Applix disk image format

*********************************************************************/

#ifndef APPLIX_DSK_H_
#define APPLIX_DSK_H_

#include "wd177x_dsk.h"

class applix_format : public wd177x_format {
public:
	applix_format();

	virtual const char *name() const;
	virtual const char *description() const;
	virtual const char *extensions() const;

private:
	static const format formats[];
};

extern const floppy_format_type FLOPPY_APPLIX_FORMAT;

#endif
