// license:BSD-3-Clause
// copyright-holders:Curt Coder
/*********************************************************************

    formats/victor9k_dsk.h

    Victor 9000 sector disk image format

*********************************************************************/

#ifndef VICTOR9K_DSK_H_
#define VICTOR9K_DSK_H_

#include "flopimg.h"

class victor9k_format : public floppy_image_format_t {
public:
	struct format {
		UINT32 form_factor;      // See floppy_image for possible values
		UINT32 variant;          // See floppy_image for possible values

		UINT16 sector_count;
		UINT8 track_count;
		UINT8 head_count;
		UINT16 sector_base_size;
	};

	victor9k_format();

	virtual const char *name() const;
	virtual const char *description() const;
	virtual const char *extensions() const;

	int find_size(io_generic *io, UINT32 form_factor);
	virtual int identify(io_generic *io, UINT32 form_factor);
	void log_boot_sector(UINT8 *data);
	floppy_image_format_t::desc_e* get_sector_desc(const format &f, int &current_size, int sector_count);
	void build_sector_description(const format &f, UINT8 *sectdata, UINT32 sect_offs, desc_s *sectors, int sector_count) const;
	virtual bool load(io_generic *io, UINT32 form_factor, floppy_image *image);
	virtual bool supports_save() const;

	static int get_rpm(int head, int track);

protected:
	static const format formats[];

	static const UINT32 cell_size[9];
	static const int sectors_per_track[2][80];
	static const int speed_zone[2][80];
	static const int rpm[9];
};

extern const floppy_format_type FLOPPY_VICTOR_9000_FORMAT;


FLOPPY_IDENTIFY( victor9k_dsk_identify );

FLOPPY_CONSTRUCT( victor9k_dsk_construct );

#endif
