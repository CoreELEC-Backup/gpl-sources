/*********************************************************************

    msx_dsk.h

    MSX disk images

*********************************************************************/

#ifndef MSX_DSK_H
#define MSX_DSK_H

#include "flopimg.h"

/**************************************************************************/

LEGACY_FLOPPY_OPTIONS_EXTERN(msx);

/**************************************************************************/

#include "wd177x_dsk.h"
#include "upd765_dsk.h"

//class msx_format : public wd177x_format {
class msx_format: public upd765_format {
public:
	msx_format();

	virtual const char *name() const;
	virtual const char *description() const;
	virtual const char *extensions() const;

private:
	static const format formats[];
};

extern const floppy_format_type FLOPPY_MSX_FORMAT;


#endif /* MSX_DSK_H */
