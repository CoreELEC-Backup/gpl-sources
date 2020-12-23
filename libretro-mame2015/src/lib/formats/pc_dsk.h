/*********************************************************************

    formats/pc_dsk.h

    PC disk images

*********************************************************************/

#ifndef PC_DSK_H
#define PC_DSK_H

#include "flopimg.h"
#include "upd765_dsk.h"

/**************************************************************************/

LEGACY_FLOPPY_OPTIONS_EXTERN(pc);


class pc_format : public upd765_format
{
public:
	pc_format();

	virtual const char *name() const;
	virtual const char *description() const;
	virtual const char *extensions() const;

private:
	static const format formats[];
};

extern const floppy_format_type FLOPPY_PC_FORMAT;

#endif /* PC_DSK_H */
