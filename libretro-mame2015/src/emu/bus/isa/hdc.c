/**********************************************************************

    ISA 8 bit XT Hard Disk Controller

**********************************************************************/

#include "emu.h"
#include "hdc.h"

#define LOG_HDC_STATUS      0
#define LOG_HDC_CALL        0
#define LOG_HDC_DATA        0

#define CMD_TESTREADY   0x00
#define CMD_RECALIBRATE 0x01
#define CMD_SENSE       0x03
#define CMD_FORMATDRV   0x04
#define CMD_VERIFY      0x05
#define CMD_FORMATTRK   0x06
#define CMD_FORMATBAD   0x07
#define CMD_READ        0x08
#define CMD_WRITE       0x0a
#define CMD_SEEK        0x0b

#define CMD_SETPARAM    0x0c
#define CMD_GETECC      0x0d

#define CMD_READSBUFF   0x0e
#define CMD_WRITESBUFF  0x0f

#define CMD_RAMDIAG     0xe0
#define CMD_DRIVEDIAG   0xe3
#define CMD_INTERNDIAG  0xe4
#define CMD_READLONG    0xe5
#define CMD_WRITELONG   0xe6

/* Bits for command status byte */
#define CSB_ERROR       0x02
#define CSB_LUN         0x20

/* XT hard disk controller status bits */
#define STA_READY       0x01
#define STA_INPUT       0x02
#define STA_COMMAND     0x04
#define STA_SELECT      0x08
#define STA_REQUEST     0x10
#define STA_INTERRUPT   0x20

/* XT hard disk controller control bits */
#define CTL_PIO         0x00
#define CTL_DMA         0x01

static const char *const hdc_command_names[] =
{
	"CMD_TESTREADY",        /* 0x00 */
	"CMD_RECALIBRATE",      /* 0x01 */
	NULL,                   /* 0x02 */
	"CMD_SENSE",            /* 0x03 */
	"CMD_FORMATDRV",        /* 0x04 */
	"CMD_VERIFY",           /* 0x05 */
	"CMD_FORMATTRK",        /* 0x06 */
	"CMD_FORMATBAD",        /* 0x07 */
	"CMD_READ",             /* 0x08 */
	NULL,                   /* 0x09 */
	"CMD_WRITE",            /* 0x0A */
	"CMD_SEEK",             /* 0x0B */
	"CMD_SETPARAM",         /* 0x0C */
	"CMD_GETECC",           /* 0x0D */
	"CMD_READSBUFF",        /* 0x0E */
	"CMD_WRITESBUFF",       /* 0x0F */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x10-0x17 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x18-0x1F */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x20-0x27 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x28-0x2F */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x30-0x37 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x38-0x3F */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x40-0x47 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x48-0x4F */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x50-0x57 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x58-0x5F */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x60-0x67 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x68-0x6F */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x70-0x77 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x78-0x7F */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x80-0x87 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x88-0x8F */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x90-0x97 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0x98-0x9F */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0xA0-0xA7 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0xA8-0xAF */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0xB0-0xB7 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0xB8-0xBF */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0xC0-0xC7 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0xC8-0xCF */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0xD0-0xD7 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0xD8-0xDF */

	"CMD_RAMDIAG",          /* 0xE0 */
	NULL,                   /* 0xE1 */
	NULL,                   /* 0xE2 */
	"CMD_DRIVEDIAG",        /* 0xE3 */
	"CMD_INTERNDIAG",       /* 0xE4 */
	"CMD_READLONG",         /* 0xE5 */
	"CMD_WRITELONG",        /* 0xE6 */
	NULL,                   /* 0xE7 */

	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0xE8-0xEF */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, /* 0xF0-0xF7 */
	NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL  /* 0xF8-0xFF */
};

static MACHINE_CONFIG_FRAGMENT( hdc_config )
	MCFG_HARDDISK_ADD("primary")
	MCFG_HARDDISK_ADD("slave")
MACHINE_CONFIG_END

ROM_START( hdc )
	ROM_REGION(0x02000,"hdc", 0)
	// Bios taken from WD1002A-WX1
	ROM_LOAD("wdbios.rom",  0x00000, 0x02000, CRC(8e9e2bd4) SHA1(601d7ceab282394ebab50763c267e915a6a2166a)) /* WDC IDE Superbios 2.0 (06/28/89) Expansion Rom C8000-C9FFF  */
ROM_END

static INPUT_PORTS_START( isa_hdc )
	PORT_START("HDD")
	PORT_BIT(     0xb0, 0xb0, IPT_UNUSED )
	PORT_DIPNAME( 0x40, 0x40, "IRQ level")
	PORT_DIPSETTING(    0x40, "5" )
	PORT_DIPSETTING(    0x00, "2" )
	PORT_DIPNAME( 0x0c, 0x0c, "Type of 1st drive")
	PORT_DIPSETTING(    0x0c, "0" )
	PORT_DIPSETTING(    0x08, "1" )
	PORT_DIPSETTING(    0x04, "2" )
	PORT_DIPSETTING(    0x00, "3" )
	PORT_DIPNAME( 0x03, 0x03, "Type of 2nd drive")
	PORT_DIPSETTING(    0x03, "0" )
	PORT_DIPSETTING(    0x02, "1" )
	PORT_DIPSETTING(    0x01, "2" )
	PORT_DIPSETTING(    0x00, "3" )

	PORT_START("ROM")
	PORT_DIPNAME( 0x01, 0x01, "Install ROM?")
	PORT_DIPSETTING(    0x01, DEF_STR(Yes) )
	PORT_DIPSETTING(    0x00, DEF_STR(No) )
INPUT_PORTS_END

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

const device_type ISA8_HDC = &device_creator<isa8_hdc_device>;
const device_type ISA8_HDC_EC1841 = &device_creator<isa8_hdc_ec1841_device>;

//-------------------------------------------------
//  machine_config_additions - device-specific
//  machine configurations
//-------------------------------------------------

machine_config_constructor isa8_hdc_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( hdc_config );
}

//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const rom_entry *isa8_hdc_device::device_rom_region() const
{
	return ROM_NAME( hdc );
}

//-------------------------------------------------
//  input_ports - device-specific input ports
//-------------------------------------------------

ioport_constructor isa8_hdc_device::device_input_ports() const
{
	return INPUT_PORTS_NAME( isa_hdc );
}

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  isa8_hdc_device - constructor
//-------------------------------------------------

isa8_hdc_device::isa8_hdc_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
		device_t(mconfig, ISA8_HDC, "Fixed Disk Controller Card", tag, owner, clock, "hdc", __FILE__),
		device_isa8_card_interface(mconfig, *this)
{
	m_type = STANDARD;
}

isa8_hdc_device::isa8_hdc_device(const machine_config &mconfig, device_type type, const char *name, const char *tag, device_t *owner, UINT32 clock, const char *shortname, const char *source) :
		device_t(mconfig, type, name, tag, owner, clock, shortname, source),
		device_isa8_card_interface(mconfig, *this)
{
}

isa8_hdc_ec1841_device::isa8_hdc_ec1841_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
		isa8_hdc_device( mconfig, ISA8_HDC_EC1841, "EC1841 HDC", tag, owner, clock, "hdc_ec1841", __FILE__)
{
	m_type = EC1841;
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void isa8_hdc_device::device_start()
{
	set_isa_device();
	m_isa->install_device(0x0320, 0x0323, 0, 0, read8_delegate( FUNC(isa8_hdc_device::pc_hdc_r), this ), write8_delegate( FUNC(isa8_hdc_device::pc_hdc_w), this ) );
	m_isa->set_dma_channel(3, this, FALSE);
	buffer.resize(17*4*512);
	timer = timer_alloc();
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void isa8_hdc_device::device_reset()
{
	drv = 0;
	data_cnt = 0;
	buffer_ptr = NULL;
	hdc_control = 0;
	for (int i = 0; i < 2; i++)
	{
		cylinders[i] = 612;
		rwc[i] = 613;
		wp[i] = 613;
		heads[i] = 4;
		ecc[i] = 11;

		/* indexes */
		cylinder[i] = 0;
		head[i] = 0;
		sector[i] = 0;
		sector_cnt[i] = 0;
		control[i] = 0;
	}

	csb = 0;
	status = 0;
	error = 0;
	dip = ioport("HDD")->read();

	if (ioport("ROM")->read() == 1)
		m_isa->install_rom(this, 0xc8000, 0xc9fff, 0, 0, "hdc", "hdc");
}

hard_disk_file *isa8_hdc_device::pc_hdc_file(int id)
{
	harddisk_image_device *img = NULL;
	astring tempstring;
	switch( id )
	{
	case 0:
		img = dynamic_cast<harddisk_image_device *>(machine().device(subtag(tempstring,"primary")));
		break;
	case 1:
		img = dynamic_cast<harddisk_image_device *>(machine().device(subtag(tempstring,"slave")));
		break;
	}
	if ( img == NULL )
		return NULL;

	if (!img->exists())
		return NULL;

	return img->get_hard_disk_file();
}

void isa8_hdc_device::pc_hdc_result(int set_error_info)
{
	if ( ( hdc_control & 0x02 )) {
		// dip switch selected IRQ 5 or 2
		if (BIT(dip, 6))
			m_isa->irq5_w(1);
		else
			m_isa->irq2_w(1);
	}

	if (LOG_HDC_STATUS)
		logerror("pc_hdc_result(): $%02x to $%04x\n", csb, data_cnt);

	buffer[data_cnt++] = csb;

	if (set_error_info && ( csb & CSB_ERROR ) )
	{
		buffer[data_cnt++] = error;
		if (error & 0x80)
		{
			buffer[data_cnt++] = (drv << 5) | head[drv];
			buffer[data_cnt++] = ((cylinder[drv] >> 2) & 0xc0) | sector[drv];
			buffer[data_cnt++] = cylinder[drv] & 0xff;

			if (LOG_HDC_STATUS)
			{
				logerror("pc_hdc_result(): result [%02x %02x %02x %02x]\n",
					buffer[data_cnt-4], buffer[data_cnt-3], buffer[data_cnt-2], buffer[data_cnt-1]);
			}
		}
		else
		{
			if (LOG_HDC_STATUS)
				logerror("pc_hdc_result(): result [%02x]\n", buffer[data_cnt-1]);
		}
	}
	status |= STA_INTERRUPT | STA_INPUT | STA_REQUEST | STA_COMMAND | STA_READY;
}



int isa8_hdc_device::no_dma(void)
{
	return (hdc_control & CTL_DMA) == 0;
}



int isa8_hdc_device::get_lbasector()
{
	hard_disk_info *info;
	hard_disk_file *file;
	int lbasector;

	file = pc_hdc_file(drv);
	info = hard_disk_get_info(file);

	lbasector = cylinder[drv];
	lbasector *= info->heads;
	lbasector += head[drv];
	lbasector *= info->sectors;
	lbasector += sector[drv];
	return lbasector;
}



/********************************************************************
 *
 * Read a number of sectors to the address set up for DMA chan #3
 *
 ********************************************************************/

/* the following crap is an abomination; it is a relic of the old crappy DMA
 * implementation that threw the idea of "emulating the hardware" to the wind
 */

int isa8_hdc_device::pc_hdc_dack_r()
{
	UINT8 result;
	hard_disk_info *info;
	hard_disk_file *file;

	file = pc_hdc_file(drv);
	if (!file)
		return 0;
	info = hard_disk_get_info(file);

	if (hdcdma_read == 0)
	{
		hard_disk_read(file, get_lbasector(), hdcdma_data);
		hdcdma_read = 512;
		hdcdma_size -= 512;
		hdcdma_src = hdcdma_data;
		sector[drv]++;
	}

	result = *(hdcdma_src++);

	if( --hdcdma_read == 0 )
	{
		/* end of cylinder ? */
		if (sector[drv] >= info->sectors)
		{
			sector[drv] = 0;
			if (++head[drv] >= info->heads)     /* beyond heads? */
			{
				head[drv] = 0;                  /* reset head */
				cylinder[drv]++;                /* next cylinder */
			}
		}
	}

	if (!no_dma())
	{
		m_isa->drq3_w((hdcdma_read || hdcdma_size ) ? 1 : 0);
		if(!(hdcdma_read || hdcdma_size)) pc_hdc_result(0);
	}

	return result;
}



void isa8_hdc_device::pc_hdc_dack_w(int data)
{
	hard_disk_info *info;
	hard_disk_file *file;

	file = pc_hdc_file(drv);
	if (!file)
		return;
	info = hard_disk_get_info(file);

	*(hdcdma_dst++) = data;

	if( --hdcdma_write == 0 )
	{
		hard_disk_write(file, get_lbasector(), hdcdma_data);
		hdcdma_write = 512;
		hdcdma_size -= 512;

		/* end of cylinder ? */
		if( ++sector[drv] >= info->sectors )
		{
			sector[drv] = 0;
			if (++head[drv] >= info->heads)     /* beyond heads? */
			{
				head[drv] = 0;                  /* reset head */
				cylinder[drv]++;                /* next cylinder */
			}
		}
		hdcdma_dst = hdcdma_data;
	}

	if (!no_dma())
	{
		m_isa->drq3_w(hdcdma_size ? 1 : 0);
		if(!hdcdma_size) pc_hdc_result(1);
	}
}



void isa8_hdc_device::pc_hdc_dack_ws(int data)
{
	*(hdcdma_dst++) = data;

	if( --hdcdma_write == 0 )
	{
		hdcdma_write = 512;
		hdcdma_size -= 512;
		hdcdma_dst = hdcdma_data;
	}

	if (!no_dma())
	{
		m_isa->drq3_w(hdcdma_size ? 1 : 0);
		if(!hdcdma_size) pc_hdc_result(1);
	}
}



void isa8_hdc_device::execute_read()
{
	hard_disk_file *disk = NULL;
	int size = sector_cnt[drv] * 512;
	int read_ = 0;

	disk = pc_hdc_file(drv);
	if (!disk)
		return;

	hdcdma_src = hdcdma_data;
	hdcdma_read = read_;
	hdcdma_size = size;

	if (no_dma())
	{
		do
		{
			buffer[data_cnt++] = pc_hdc_dack_r();
		} while (hdcdma_read || hdcdma_size);
	}
	else
	{
		m_isa->drq3_w(1);
	}
}



void isa8_hdc_device::execute_write()
{
	hard_disk_file *disk = NULL;
	int size = sector_cnt[drv] * 512;
	int write_ = 512;

	disk = pc_hdc_file(drv);
	if (!disk)
		return;

	hdcdma_dst = hdcdma_data;
	hdcdma_write = write_;
	hdcdma_size = size;

	if (no_dma())
	{
		do
		{
			pc_hdc_dack_w(buffer[data_cnt++]);
		}
		while (hdcdma_write || hdcdma_size);
	}
	else
	{
		m_isa->drq3_w(1);
	}
}



void isa8_hdc_device::execute_writesbuff()
{
	hdcdma_dst = hdcdma_data;
	hdcdma_write = 512;
	hdcdma_size = 512;

	if (no_dma())
	{
		do
		{
			pc_hdc_dack_ws(buffer[data_cnt++]);
		}
		while (hdcdma_write || hdcdma_size);
	}
	else
	{
		m_isa->drq3_w(1);
	}
}



void isa8_hdc_device::get_drive()
{
	drv = (buffer[1] >> 5) & 1;
	csb = (drv) ? CSB_LUN : 0x00;
}



void isa8_hdc_device::get_chsn()
{
	head[drv] = buffer[1] & 0x1f;
	sector[drv] = buffer[2] & 0x3f;
	cylinder[drv] = (buffer[2] & 0xc0) << 2;
	cylinder[drv] |= buffer[3];
	sector_cnt[drv] = buffer[4];
	control[drv] = buffer[5];   /* 7: no retry, 6: no ecc retry, 210: step rate */

	error = 0x80;   /* a potential error has C/H/S/N info */
}

int isa8_hdc_device::test_ready()
{
	if( !pc_hdc_file(drv) )
	{
		csb |= CSB_ERROR;
		error |= 0x04;  /* drive not ready */
		return 0;
	}
	return 1;
}

void isa8_hdc_device::hdc_command()
{
	int set_error_info = 1;
	int old_error = error;          /* Previous error data is needed for CMD_SENSE */
	UINT8 cmd;
	const char *command_name;

	csb = 0x00;
	error = 0;

	buffer_ptr = buffer;
	cmd = buffer[0];

	get_drive();

	if (LOG_HDC_STATUS)
	{
		command_name = hdc_command_names[cmd] ? hdc_command_names[cmd] : "Unknown";
		logerror("%s pc_hdc_command(): Executing command; cmd=0x%02x (%s) drv=%d\n",
			machine().describe_context(), cmd, command_name, drv);
	}

	switch (cmd)
	{
		case CMD_TESTREADY:
			set_error_info = 0;
			test_ready();
			break;
		case CMD_SENSE:
			/* Perform error code translation. This may need to be expanded in the future. */
			buffer[data_cnt++] = ( old_error & 0xC0 ) | ( ( old_error & 0x04 ) ? 0x04 : 0x00 ) ;
			buffer[data_cnt++] = (drv << 5) | head[drv];
			buffer[data_cnt++] = ((cylinder[drv] >> 2) & 0xc0) | sector[drv];
			buffer[data_cnt++] = cylinder[drv] & 0xff;
			set_error_info = 0;
			break;
		case CMD_RECALIBRATE:
			get_chsn();
			break;

		case CMD_FORMATDRV:
		case CMD_VERIFY:
		case CMD_FORMATTRK:
		case CMD_FORMATBAD:
		case CMD_SEEK:
		case CMD_DRIVEDIAG:
			get_chsn();
			test_ready();
			break;

		case CMD_READ:
		case CMD_READLONG:
			get_chsn();

			if (LOG_HDC_STATUS)
			{
				logerror("%s hdc read D:%d C:%d H:%d S:%d N:%d CTL:$%02x\n",
					machine().describe_context(), drv, cylinder[drv], head[drv], sector[drv], sector_cnt[drv], control[drv]);
			}

			if (test_ready())
				execute_read();
			set_error_info = 0;
			break;

		case CMD_WRITE:
		case CMD_WRITELONG:
			get_chsn();

			if (LOG_HDC_STATUS)
			{
				logerror("%s hdc write  D:%d C:%d H:%d S:%d N:%d CTL:$%02x\n",
					machine().describe_context(), drv, cylinder[drv], head[drv], sector[drv], sector_cnt[drv], control[drv]);
			}

			if (test_ready())
				execute_write();
			break;

		case CMD_WRITESBUFF:
			if (LOG_HDC_STATUS)
			{
				logerror("%s hdc write sector buffer\n", machine().describe_context());
			}

			execute_writesbuff();
			break;

		case CMD_SETPARAM:
			get_chsn();
			cylinders[drv] = ((buffer[6]&3)<<8) | buffer[7];
			heads[drv] = buffer[8] & 0x1f;
			rwc[drv] = ((buffer[9]&3)<<8) | buffer[10];
			wp[drv] = ((buffer[11]&3)<<8) | buffer[12];
			ecc[drv] = buffer[13];
			break;

		case CMD_GETECC:
			buffer[data_cnt++] = ecc[drv];
			break;

		case CMD_READSBUFF:
		case CMD_RAMDIAG:
		case CMD_INTERNDIAG:
			break;

	}
	if(no_dma()) pc_hdc_result(set_error_info);
}


void isa8_hdc_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	hdc_command();
}


/*  Command format
 *  Bits     Description
 *  7      0
 *  xxxxxxxx command
 *  dddhhhhh drive / head
 *  ccssssss cylinder h / sector
 *  cccccccc cylinder l
 *  nnnnnnnn count
 *  xxxxxxxx control
 *
 *  Command format extra for set drive characteristics
 *  000000cc cylinders h
 *  cccccccc cylinders l
 *  000hhhhh heads
 *  000000cc reduced write h
 *  cccccccc reduced write l
 *  000000cc write precomp h
 *  cccccccc write precomp l
 *  eeeeeeee ecc
 */
void isa8_hdc_device::pc_hdc_data_w(int data)
{
	if( data_cnt == 0 )
	{
		if (LOG_HDC_DATA)
			logerror("hdc_data_w $%02x: ", data);

		buffer_ptr = buffer;
		data_cnt = 6;   /* expect 6 bytes including this one */
		status &= ~STA_READY;
		status &= ~STA_INPUT;
		switch (data)
		{
			case CMD_SETPARAM:
				data_cnt += 8;
				break;

			case CMD_TESTREADY:
			case CMD_RECALIBRATE:
			case CMD_SENSE:
			case CMD_FORMATDRV:
			case CMD_VERIFY:
			case CMD_FORMATTRK:
			case CMD_FORMATBAD:
			case CMD_READ:
			case CMD_WRITE:
			case CMD_SEEK:
			case CMD_GETECC:
			case CMD_READSBUFF:
			case CMD_WRITESBUFF:
			case CMD_RAMDIAG:
			case CMD_DRIVEDIAG:
			case CMD_INTERNDIAG:
			case CMD_READLONG:
			case CMD_WRITELONG:
				break;

			default:
				data_cnt = 0;
				status |= STA_INPUT;
				csb |= CSB_ERROR | 0x20; /* unknown command */
				pc_hdc_result(1);
				break;
		}
		if( data_cnt )
			status |= STA_REQUEST;
	}

	if (data_cnt)
	{
		if (LOG_HDC_DATA)
			logerror("hdc_data_w $%02x\n", data);

		*buffer_ptr++ = data;
		// XXX ec1841 wants this
		if (buffer[0] == CMD_SETPARAM && data_cnt == 9 && (m_type == EC1841)) {
			status &= ~STA_READY;
		} else {
			status |= STA_READY;
		}
		if (--data_cnt == 0)
		{
			if (LOG_HDC_STATUS)
				logerror("%s pc_hdc_data_w(): Launching command\n", machine().describe_context());

			status &= ~STA_COMMAND;
			status &= ~STA_REQUEST;
			status &= ~STA_READY;
			status &= ~STA_INPUT;
			timer->adjust(attotime::from_msec(1),0);
		}
	}
}



void isa8_hdc_device::pc_hdc_reset_w(int data)
{
	cylinder[0] = cylinder[1] = 0;
	head[0] = head[1] = 0;
	sector[0] = sector[1] = 0;
	csb = 0;
	status = STA_COMMAND | STA_READY;
	memset(buffer, 0, buffer.count());
	buffer_ptr = buffer;
	data_cnt = 0;
}



void isa8_hdc_device::pc_hdc_select_w(int data)
{
	status &= ~STA_INTERRUPT;
	status |= STA_SELECT;
}



void isa8_hdc_device::pc_hdc_control_w(int data)
{
	if (LOG_HDC_STATUS)
		logerror("%s: pc_hdc_control_w(): control write %d\n", machine().describe_context(), data);

	hdc_control = data;

	if (!(hdc_control & 0x02))
	{
		if (BIT(dip, 6))
			m_isa->irq5_w(0);
		else
			m_isa->irq2_w(0);
	}
}



UINT8 isa8_hdc_device::pc_hdc_data_r()
{
	UINT8 data = 0xff;
	if( data_cnt )
	{
		data = *buffer_ptr++;
		status &= ~STA_INTERRUPT;
		if( --data_cnt == 0 )
		{
			status &= ~STA_INPUT;
			status &= ~STA_REQUEST;
			status &= ~STA_SELECT;
			status |= STA_COMMAND;
		}
	}
	return data;
}



UINT8 isa8_hdc_device::pc_hdc_status_r()
{
	return status;
}



/*
    Dipswitch configuration


    Tandon/Western Digital Fixed Disk Controller
    bit0-1 : Determine disk size(?)
        Causes geometry data to be read from c8043, c8053, c8063, c8073 (?)
        00 - 40 Mbytes
        01 - 30 Mbytes
        10 - 10 Mbytes
        11 - 20 Mbytes
    bit2-7 : unknown

 */

UINT8 isa8_hdc_device::pc_hdc_dipswitch_r()
{
	status |= STA_READY; // XXX
	if (LOG_HDC_STATUS)
		logerror("%s: pc_hdc_dipswitch_r: status $%02X\n", machine().describe_context(), status);
	return dip;
}



/*************************************************************************
 *
 *      HDC
 *      hard disk controller
 *
 *************************************************************************/
READ8_MEMBER( isa8_hdc_device::pc_hdc_r )
{
	UINT8 data = 0xff;

	switch( offset )
	{
		case 0: data = pc_hdc_data_r();     break;
		case 1: data = pc_hdc_status_r();   break;
		case 2: data = pc_hdc_dipswitch_r(); break;
		case 3: break;
	}

	if (LOG_HDC_CALL)
		logerror("%s pc_hdc_r(): offs=%d result=0x%02x\n", machine().describe_context(), offset, data);

	return data;
}

WRITE8_MEMBER( isa8_hdc_device::pc_hdc_w )
{
	if (LOG_HDC_CALL)
		logerror("%s pc_hdc_w(): offs=%d data=0x%02x\n", machine().describe_context(), offset, data);

	switch( offset )
	{
		case 0: pc_hdc_data_w(data);    break;
		case 1: pc_hdc_reset_w(data);   break;
		case 2: pc_hdc_select_w(data);  break;
		case 3: pc_hdc_control_w(data); break;
	}
}


UINT8 isa8_hdc_device::dack_r(int line)
{
	return pc_hdc_dack_r();
}

void isa8_hdc_device::dack_w(int line,UINT8 data)
{
	if (buffer[0] == CMD_WRITESBUFF)
		pc_hdc_dack_ws(data);
	else
		pc_hdc_dack_w(data);
}
