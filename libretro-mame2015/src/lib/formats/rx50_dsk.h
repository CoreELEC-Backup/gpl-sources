/*********************************************************************

    formats/rx50_dsk.h

    Format for DEC RX50 floppy drive used e.g. by Rainbow 100 and 190

    Disk is PC MFM, 80 tracks, single sided, with 10 sectors per track

based on lib/formats/esq16_dsk.h

*********************************************************************/

#ifndef RX50_DSK_H_
#define RX50_DSK_H_

#include "flopimg.h"

class rx50img_format : public floppy_image_format_t
{
public:
	rx50img_format();

	virtual int identify(io_generic *io, UINT32 form_factor);
	virtual bool load(io_generic *io, UINT32 form_factor, floppy_image *image);
	virtual bool save(io_generic *io, floppy_image *image);

	virtual const char *name() const;
	virtual const char *description() const;
	virtual const char *extensions() const;
	virtual bool supports_save() const;

	static const desc_e rx50_10_desc[];

private:
	void find_size(io_generic *io, UINT8 &track_count, UINT8 &head_count, UINT8 &sector_count);
};

extern const floppy_format_type FLOPPY_RX50IMG_FORMAT;

#endif /* RX50_DSK_H_ */
