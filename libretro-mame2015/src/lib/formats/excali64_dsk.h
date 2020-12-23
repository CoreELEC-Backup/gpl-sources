/*********************************************************************

    formats/excali64_dsk.h

    Excalibur 64 disk image format

*********************************************************************/

#ifndef EXCALI64_DSK_H_
#define EXCALI64_DSK_H_

#include "wd177x_dsk.h"

class excali64_format : public wd177x_format {
public:
	excali64_format();

	virtual const char *name() const;
	virtual const char *description() const;
	virtual const char *extensions() const;

private:
	static const format formats[];
};

extern const floppy_format_type FLOPPY_EXCALI64_FORMAT;

#endif
