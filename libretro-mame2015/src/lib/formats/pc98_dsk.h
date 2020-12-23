/*********************************************************************

    formats/pc98_dsk.h

    PC disk images

*********************************************************************/

#ifndef PC98_DSK_H
#define PC98_DSK_H

#include "flopimg.h"
#include "upd765_dsk.h"

/**************************************************************************/


class pc98_format : public upd765_format
{
public:
	pc98_format();

	virtual const char *name() const;
	virtual const char *description() const;
	virtual const char *extensions() const;

private:
	static const format formats[];
};

extern const floppy_format_type FLOPPY_PC98_FORMAT;

#endif /* PC_DSK_H */
