/*********************************************************************

    formats/trd_dsk.h

    TRD disk images

*********************************************************************/

#ifndef TRD_DSK_H
#define TRD_DSK_H

#include "wd177x_dsk.h"

class trd_format : public wd177x_format {
public:
	trd_format();

	virtual const char *name() const;
	virtual const char *description() const;
	virtual const char *extensions() const;

private:
	static const format formats[];
};

extern const floppy_format_type FLOPPY_TRD_FORMAT;

#endif /* TRD_DSK_H */
