/***************************************************************************

    coco_vhd.h

    Color Computer Virtual Hard Drives

***************************************************************************/

#ifndef COCOVHD_H
#define COCOVHD_H


/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

// ======================> coco_vhd_image_device

class coco_vhd_image_device :   public device_t,
								public device_image_interface
{
public:
	// construction/destruction
	coco_vhd_image_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	virtual ~coco_vhd_image_device();

	// image-level overrides
	virtual bool call_load();

	virtual iodevice_t image_type() const { return IO_HARDDISK; }

	virtual bool is_readable()  const { return 1; }
	virtual bool is_writeable() const { return 1; }
	virtual bool is_creatable() const { return 1; }
	virtual bool must_be_loaded() const { return 0; }
	virtual bool is_reset_on_load() const { return 0; }
	virtual const char *image_interface() const { return NULL; }
	virtual const char *file_extensions() const { return "vhd"; }
	virtual const option_guide *create_option_guide() const { return NULL; }

	// specific implementation
	DECLARE_READ8_MEMBER(read) { return read(offset); }
	DECLARE_WRITE8_MEMBER(write) { write(offset, data); }
	UINT8 read(offs_t offset);
	void write(offs_t offset, UINT8 data);

protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();

	void coco_vhd_readwrite(UINT8 data);

private:
	cpu_device *            m_cpu;
	address_space *         m_cpu_space;
	UINT32                  m_logical_record_number;
	UINT32                  m_buffer_address;
	UINT8                   m_status;
};

// device type definition
extern const device_type COCO_VHD;

/***************************************************************************
    DEVICE CONFIGURATION MACROS
***************************************************************************/

#define MCFG_COCO_VHD_ADD(_tag) \
	MCFG_DEVICE_ADD(_tag, COCO_VHD, 0)
#endif /* COCOVHD_H */
