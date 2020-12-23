// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic
/***************************************************************************

    diimage.h

    Device image interfaces.

***************************************************************************/

#pragma once

#ifndef __EMU_H__
#error Dont include this file directly; include emu.h instead.
#endif

#ifndef __DIIMAGE_H__
#define __DIIMAGE_H__

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class software_list;

enum iodevice_t
{
	/* List of all supported devices.  Refer to the device by these names only */
	IO_UNKNOWN,
	IO_CARTSLOT,    /*  1 - Cartridge Port, as found on most console and on some computers */
	IO_FLOPPY,      /*  2 - Floppy Disk unit */
	IO_HARDDISK,    /*  3 - Hard Disk unit */
	IO_CYLINDER,    /*  4 - Magnetically-Coated Cylinder */
	IO_CASSETTE,    /*  5 - Cassette Recorder (common on early home computers) */
	IO_PUNCHCARD,   /*  6 - Card Puncher/Reader */
	IO_PUNCHTAPE,   /*  7 - Tape Puncher/Reader (reels instead of punchcards) */
	IO_PRINTER,     /*  8 - Printer device */
	IO_SERIAL,      /*  9 - Generic Serial Port */
	IO_PARALLEL,    /* 10 - Generic Parallel Port */
	IO_SNAPSHOT,    /* 11 - Complete 'snapshot' of the state of the computer */
	IO_QUICKLOAD,   /* 12 - Allow to load program/data into memory, without matching any actual device */
	IO_MEMCARD,     /* 13 - Memory card */
	IO_CDROM,       /* 14 - optical CD-ROM disc */
	IO_MAGTAPE,     /* 15 - Magentic tape */
	IO_ROM,         /* 16 - Individual ROM image - the Amstrad CPC has a few applications that were sold on 16kB ROMs */
	IO_MIDIIN,      /* 17 - MIDI In port */
	IO_MIDIOUT,     /* 18 - MIDI Out port */
	IO_COUNT        /* 19 - Total Number of IO_devices for searching */
};

enum image_error_t
{
	IMAGE_ERROR_SUCCESS,
	IMAGE_ERROR_INTERNAL,
	IMAGE_ERROR_UNSUPPORTED,
	IMAGE_ERROR_OUTOFMEMORY,
	IMAGE_ERROR_FILENOTFOUND,
	IMAGE_ERROR_INVALIDIMAGE,
	IMAGE_ERROR_ALREADYOPEN,
	IMAGE_ERROR_UNSPECIFIED
};

struct image_device_type_info
{
	iodevice_t m_type;
	const char *m_name;
	const char *m_shortname;
};

class image_device_format
{
	friend class simple_list<image_device_format>;

public:
	image_device_format(const char *name, const char *description, const char *extensions, const char *optspec)
		: m_next(NULL),
			m_name(name),
			m_description(description),
			m_extensions(extensions),
			m_optspec(optspec)  { }

	image_device_format *next() const { return m_next; }
	const char *name() const { return m_name; }
	const char *description() const { return m_description; }
	const char *extensions() const { return m_extensions; }
	const char *optspec() const { return m_optspec; }

private:
	image_device_format *m_next;
	astring m_name;
	astring m_description;
	astring m_extensions;
	astring m_optspec;
};


class device_image_interface;
struct feature_list;
class software_part;
class software_info;

// device image interface function types
typedef delegate<int (device_image_interface &)> device_image_load_delegate;
typedef delegate<void (device_image_interface &)> device_image_func_delegate;
// legacy
typedef void (*device_image_partialhash_func)(hash_collection &, const unsigned char *, unsigned long, const char *);

//**************************************************************************
//  MACROS
//**************************************************************************

#define IMAGE_INIT_PASS     FALSE
#define IMAGE_INIT_FAIL     TRUE
#define IMAGE_VERIFY_PASS   FALSE
#define IMAGE_VERIFY_FAIL   TRUE

#define DEVICE_IMAGE_LOAD_MEMBER_NAME(_name)           device_image_load_##_name
#define DEVICE_IMAGE_LOAD_NAME(_class,_name)           _class::DEVICE_IMAGE_LOAD_MEMBER_NAME(_name)
#define DECLARE_DEVICE_IMAGE_LOAD_MEMBER(_name)        int DEVICE_IMAGE_LOAD_MEMBER_NAME(_name)(device_image_interface &image)
#define DEVICE_IMAGE_LOAD_MEMBER(_class,_name)         int DEVICE_IMAGE_LOAD_NAME(_class,_name)(device_image_interface &image)
#define DEVICE_IMAGE_LOAD_DELEGATE(_class,_name)       device_image_load_delegate(&DEVICE_IMAGE_LOAD_NAME(_class,_name),#_class "::device_image_load_" #_name, downcast<_class *>(device->owner()))

#define DEVICE_IMAGE_UNLOAD_MEMBER_NAME(_name)          device_image_unload_##_name
#define DEVICE_IMAGE_UNLOAD_NAME(_class,_name)          _class::DEVICE_IMAGE_UNLOAD_MEMBER_NAME(_name)
#define DECLARE_DEVICE_IMAGE_UNLOAD_MEMBER(_name)       void DEVICE_IMAGE_UNLOAD_MEMBER_NAME(_name)(device_image_interface &image)
#define DEVICE_IMAGE_UNLOAD_MEMBER(_class,_name)        void DEVICE_IMAGE_UNLOAD_NAME(_class,_name)(device_image_interface &image)
#define DEVICE_IMAGE_UNLOAD_DELEGATE(_class,_name)      device_image_func_delegate(&DEVICE_IMAGE_UNLOAD_NAME(_class,_name),#_class "::device_image_unload_" #_name, downcast<_class *>(device->owner()))


// ======================> device_image_interface

// class representing interface-specific live image
class device_image_interface : public device_interface
{
public:
	// construction/destruction
	device_image_interface(const machine_config &mconfig, device_t &device);
	virtual ~device_image_interface();

	static const char *device_typename(iodevice_t type);
	static const char *device_brieftypename(iodevice_t type);
	static iodevice_t device_typeid(const char *name);

	virtual void device_compute_hash(hash_collection &hashes, const void *data, size_t length, const char *types) const;

	virtual bool call_load() { return FALSE; }
	virtual bool call_softlist_load(software_list_device &swlist, const char *swname, const rom_entry *start_entry) { return FALSE; }
	virtual bool call_create(int format_type, option_resolution *format_options) { return FALSE; }
	virtual void call_unload() { }
	virtual void call_display() { }
	virtual device_image_partialhash_func get_partial_hash() const { return NULL; }
	virtual bool core_opens_image_file() const { return TRUE; }
	virtual iodevice_t image_type()  const = 0;
	virtual bool is_readable()  const = 0;
	virtual bool is_writeable() const = 0;
	virtual bool is_creatable() const = 0;
	virtual bool must_be_loaded() const = 0;
	virtual bool is_reset_on_load() const = 0;
	virtual const char *image_interface() const { return NULL; }
	virtual const char *file_extensions() const = 0;
	virtual const option_guide *create_option_guide() const = 0;

	virtual ui_menu *get_selection_menu(running_machine &machine, class render_container *container);

	const image_device_format *device_get_indexed_creatable_format(int index) { return m_formatlist.find(index); }
	const image_device_format *device_get_named_creatable_format(const char *format_name);
	const option_guide *device_get_creation_option_guide() { return create_option_guide(); }

	const char *error();
	void seterror(image_error_t err, const char *message);
	void message(const char *format, ...) ATTR_PRINTF(2,3);

	bool exists() { return m_image_name; }
	const char *filename() { if (!m_image_name) return NULL; else return m_image_name; }
	const char *basename() { if (!m_basename) return NULL; else return m_basename; }
	const char *basename_noext()  { if (!m_basename_noext) return NULL; else return m_basename_noext; }
	const char *filetype()  { if (!m_filetype) return NULL; else return m_filetype; }
	core_file *image_core_file() { return m_file; }
	UINT64 length() { check_for_file(); return core_fsize(m_file); }
	bool is_readonly() { return m_readonly; }
	bool has_been_created() { return m_created; }
	void make_readonly() { m_readonly = true; }
	UINT32 fread(void *buffer, UINT32 length) { check_for_file(); return core_fread(m_file, buffer, length); }
	UINT32 fread(optional_shared_ptr<UINT8> &ptr, UINT32 length) { ptr.allocate(length); return fread(ptr.target(), length); }
	UINT32 fread(optional_shared_ptr<UINT8> &ptr, UINT32 length, offs_t offset) { ptr.allocate(length); return fread(ptr + offset, length - offset); }
	UINT32 fwrite(const void *buffer, UINT32 length) { check_for_file(); return core_fwrite(m_file, buffer, length); }
	int fseek(INT64 offset, int whence) { check_for_file(); return core_fseek(m_file, offset, whence); }
	UINT64 ftell() { check_for_file(); return core_ftell(m_file); }
	int fgetc() { char ch; if (fread(&ch, 1) != 1) ch = '\0'; return ch; }
	char *fgets(char *buffer, UINT32 length) { check_for_file(); return core_fgets(buffer, length, m_file); }
	int image_feof() { check_for_file(); return core_feof(m_file); }
	void *ptr() {check_for_file(); return (void *) core_fbuffer(m_file); }
	// configuration access
	void set_init_phase() { m_init_phase = TRUE; }

	const char* longname() { return m_longname; }
	const char* manufacturer() { return m_manufacturer; }
	const char* year() { return m_year; }
	UINT32 supported() { return m_supported; }

	const software_info *software_entry() { return m_software_info_ptr; }
	const software_part *part_entry() { return m_software_part_ptr; }
	const char *software_list_name() { return m_software_list_name; }

	void set_working_directory(const char *working_directory) { m_working_directory = working_directory; }
	const char * working_directory();

	UINT8 *get_software_region(const char *tag);
	UINT32 get_software_region_length(const char *tag);
	const char *get_feature(const char *feature_name);
	bool load_software_region(const char *tag, optional_shared_ptr<UINT8> &ptr);

	UINT32 crc();
	hash_collection& hash() { return m_hash; }

	void battery_load(void *buffer, int length, int fill);
	void battery_load(void *buffer, int length, void *def_buffer);
	void battery_save(const void *buffer, int length);

	const char *image_type_name()  const { return device_typename(image_type()); }



	const char *instance_name() const { return m_instance_name; }
	const char *brief_instance_name() const { return m_brief_instance_name; }
	bool uses_file_extension(const char *file_extension) const;
	image_device_format *formatlist() const { return m_formatlist.first(); }

	bool load(const char *path);
	bool open_image_file(emu_options &options);
	bool finish_load();
	void unload();
	bool create(const char *path, const image_device_format *create_format, option_resolution *create_args);
	bool load_software(software_list_device &swlist, const char *swname, const rom_entry *entry);
	int reopen_for_write(const char *path);

	static void software_name_split(const char *swlist_swname, astring &swlist_name, astring &swname, astring &swpart);

protected:
	bool load_internal(const char *path, bool is_create, int create_format, option_resolution *create_args, bool just_load);
	void determine_open_plan(int is_create, UINT32 *open_plan);
	image_error_t load_image_by_path(UINT32 open_flags, const char *path);
	void clear();
	bool is_loaded();

	image_error_t set_image_filename(const char *filename);

	void clear_error();

	void check_for_file() { assert_always(m_file != NULL, "Illegal operation on unmounted image"); }

	void setup_working_directory();
	bool try_change_working_directory(const char *subdir);

	int read_hash_config(const char *sysname);
	void run_hash(void (*partialhash)(hash_collection &, const unsigned char *, unsigned long, const char *), hash_collection &hashes, const char *types);
	void image_checkhash();
	void update_names(const device_type device_type = NULL, const char *inst = NULL, const char *brief = NULL);

	software_part *find_software_item(const char *path, bool restrict_to_interface);
	bool load_software_part(const char *path, software_part *&swpart);
	void software_get_default_slot(astring &result, const char *default_card_slot);

	// derived class overrides

	// configuration
	static const image_device_type_info *find_device_type(iodevice_t type);
	static const image_device_type_info m_device_info_array[];

	/* error related info */
	image_error_t m_err;
	astring m_err_message;

	/* variables that are only non-zero when an image is mounted */
	core_file *m_file;
	emu_file *m_mame_file;
	astring m_image_name;
	astring m_basename;
	astring m_basename_noext;
	astring m_filetype;

	/* working directory; persists across mounts */
	astring m_working_directory;

	/* Software information */
	astring m_full_software_name;
	software_info *m_software_info_ptr;
	software_part *m_software_part_ptr;
	astring m_software_list_name;

	/* info read from the hash file/software list */
	astring m_longname;
	astring m_manufacturer;
	astring m_year;
	UINT32  m_supported;

	/* flags */
	bool m_readonly;
	bool m_created;
	bool m_init_phase;
	bool m_from_swlist;

	/* special - used when creating */
	int m_create_format;
	option_resolution *m_create_args;

	hash_collection m_hash;

	astring m_brief_instance_name;
	astring m_instance_name;

	/* creation info */
	simple_list<image_device_format> m_formatlist;

	bool m_is_loading;
};

// iterator
typedef device_interface_iterator<device_image_interface> image_interface_iterator;

#endif  /* __DIIMAGE_H__ */
