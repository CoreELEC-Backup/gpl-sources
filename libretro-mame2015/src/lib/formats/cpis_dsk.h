/*********************************************************************

    formats/cpis_dsk.h

    Telenova Compis disk images

*********************************************************************/

#ifndef CPIS_DSK_H
#define CPIS_DSK_H

#include "flopimg.h"


/**************************************************************************/

LEGACY_FLOPPY_OPTIONS_EXTERN(compis);


#include "upd765_dsk.h"

class cpis_format : public upd765_format {
public:
	cpis_format();

	virtual const char *name() const;
	virtual const char *description() const;
	virtual const char *extensions() const;

private:
	static const format formats[];
};

extern const floppy_format_type FLOPPY_CPIS_FORMAT;

#endif /* CPIS_DSK_H */
