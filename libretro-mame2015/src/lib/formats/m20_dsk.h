/*********************************************************************

    formats/m20_dsk.c

    Olivetti M20 floppy-disk images

*********************************************************************/

#ifndef M20_DSK_H
#define M20_DSK_H

#include "flopimg.h"

/**************************************************************************/

LEGACY_FLOPPY_OPTIONS_EXTERN(m20);


#include "wd177x_dsk.h"

class m20_format : public floppy_image_format_t {
public:
	m20_format();

	virtual int identify(io_generic *io, UINT32 form_factor);
	virtual bool load(io_generic *io, UINT32 form_factor, floppy_image *image);
	virtual bool save(io_generic *io, floppy_image *image);

	virtual const char *name() const;
	virtual const char *description() const;
	virtual const char *extensions() const;
	virtual bool supports_save() const;
};

extern const floppy_format_type FLOPPY_M20_FORMAT;

#endif /* M20_DSK_H */
