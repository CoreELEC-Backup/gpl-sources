/***************************************************************************

    Siemens PC-D

    license: MAME, GPL-2.0+
    copyright-holders: Dirk Best

    Skeleton driver

***************************************************************************/

#include "emu.h"
#include "cpu/i86/i186.h"
#include "machine/ram.h"
#include "machine/nvram.h"
#include "machine/pic8259.h"
#include "machine/mc2661.h"
#include "machine/wd_fdc.h"
#include "machine/mc146818.h"
#include "machine/pcd_kbd.h"
#include "sound/speaker.h"
#include "video/scn2674.h"
#include "formats/pc_dsk.h"
#include "bus/scsi/omti5100.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class pcd_state : public driver_device
{
public:
	pcd_state(const machine_config &mconfig, device_type type, const char *tag) :
	driver_device(mconfig, type, tag),
	m_maincpu(*this, "maincpu"),
	m_pic1(*this, "pic1"),
	m_pic2(*this, "pic2"),
	m_speaker(*this, "speaker"),
	m_fdc(*this, "fdc"),
	m_rtc(*this, "rtc"),
	m_crtc(*this, "crtc"),
	m_palette(*this, "palette"),
	m_gfxdecode(*this, "gfxdecode"),
	m_scsi(*this, "scsi"),
	m_scsi_data_out(*this, "scsi_data_out"),
	m_scsi_data_in(*this, "scsi_data_in"),
	m_vram(*this, "vram"),
	m_charram(8*1024)
	{ }

	DECLARE_READ8_MEMBER( irq_callback );
	TIMER_DEVICE_CALLBACK_MEMBER( timer0_tick );
	DECLARE_WRITE_LINE_MEMBER( i186_timer1_w );

	DECLARE_READ8_MEMBER( nmi_io_r );
	DECLARE_WRITE8_MEMBER( nmi_io_w );
	DECLARE_READ8_MEMBER( rtc_r );
	DECLARE_WRITE8_MEMBER( rtc_w );
	DECLARE_READ8_MEMBER( stat_r );
	DECLARE_WRITE8_MEMBER( stat_w );
	DECLARE_READ8_MEMBER( led_r );
	DECLARE_WRITE8_MEMBER( led_w );
	DECLARE_READ8_MEMBER( detect_r );
	DECLARE_WRITE8_MEMBER( detect_w );
	DECLARE_READ16_MEMBER( dskctl_r );
	DECLARE_WRITE16_MEMBER( dskctl_w );
	DECLARE_READ8_MEMBER( mcu_r );
	DECLARE_WRITE8_MEMBER( mcu_w );
	DECLARE_READ8_MEMBER( scsi_r );
	DECLARE_WRITE8_MEMBER( scsi_w );
	DECLARE_WRITE8_MEMBER( vram_sw_w );
	DECLARE_WRITE16_MEMBER( vram_w );
	SCN2674_DRAW_CHARACTER_MEMBER(display_pixels);
	DECLARE_FLOPPY_FORMATS( floppy_formats );
	DECLARE_WRITE_LINE_MEMBER(write_scsi_bsy);
	DECLARE_WRITE_LINE_MEMBER(write_scsi_cd);
	DECLARE_WRITE_LINE_MEMBER(write_scsi_io);
	DECLARE_WRITE_LINE_MEMBER(write_scsi_msg);
	DECLARE_WRITE_LINE_MEMBER(write_scsi_req);

protected:
	// driver_device overrides
	virtual void machine_start();
	virtual void machine_reset();
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);

private:
	required_device<i80186_cpu_device> m_maincpu;
	required_device<pic8259_device> m_pic1;
	required_device<pic8259_device> m_pic2;
	required_device<speaker_sound_device> m_speaker;
	required_device<wd2793_t> m_fdc;
	required_device<mc146818_device> m_rtc;
	required_device<scn2674_device> m_crtc;
	required_device<palette_device> m_palette;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<SCSI_PORT_DEVICE> m_scsi;
	required_device<output_latch_device> m_scsi_data_out;
	required_device<input_buffer_device> m_scsi_data_in;
	required_shared_ptr<UINT16> m_vram;
	dynamic_buffer m_charram;
	UINT8 m_stat, m_led, m_vram_sw;
	int m_msg, m_bsy, m_io, m_cd, m_req, m_rst;
	emu_timer *m_req_hack;
	UINT16 m_dskctl;
};


//**************************************************************************
//  MACHINE EMULATION
//**************************************************************************

static const gfx_layout pcd_charlayout =
{
	8, 14,                   /* 8 x 14 characters */
	512,                    /* 512 characters */
	1,                  /* 1 bits per pixel */
	{ 0 },                  /* no bitplanes */
	/* x offsets */
	{ 0, 1, 2, 3, 4, 5, 6, 7 },
	/* y offsets */
	{ 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 8*8, 9*8, 10*8, 11*8, 12*8, 13*8, 14*8 },
	8*16
};

void pcd_state::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	// TODO: remove this hack
	if(m_req)
		m_maincpu->drq0_w(1);
}

void pcd_state::machine_start()
{
	m_gfxdecode->set_gfx(0, global_alloc(gfx_element(machine().device<palette_device>("palette"), pcd_charlayout, m_charram, 0, 1, 0)));
	m_req_hack = timer_alloc();
}

void pcd_state::machine_reset()
{
	m_stat = 0;
	m_led = 0;
	m_dskctl = 0;
	m_vram_sw = 1;
	m_rst = 0;
}

READ8_MEMBER( pcd_state::irq_callback )
{
	return (offset ? m_pic2 : m_pic1)->acknowledge();
}

TIMER_DEVICE_CALLBACK_MEMBER( pcd_state::timer0_tick )
{
	m_maincpu->tmrin0_w(0);
	m_maincpu->tmrin0_w(1);
}

WRITE_LINE_MEMBER( pcd_state::i186_timer1_w )
{
	m_speaker->level_w(state);
}

WRITE16_MEMBER( pcd_state::vram_w )
{
	if(m_vram_sw)
		COMBINE_DATA(&m_vram[offset]);
	else if(mem_mask & 0xff)
	{
		m_charram[offset & 0x1fff] = data;
		m_gfxdecode->m_gfx[0]->mark_dirty(offset/16);
	}
}

WRITE8_MEMBER( pcd_state::vram_sw_w )
{
	m_vram_sw = data & 1;
}

READ8_MEMBER( pcd_state::nmi_io_r )
{
	if(space.debugger_access())
		return 0;
	logerror("%s: unmapped %s %04x\n", machine().describe_context(), space.name(), offset);
	m_stat |= 8;
	m_maincpu->set_input_line(INPUT_LINE_NMI, PULSE_LINE);
	return 0;
}

WRITE8_MEMBER( pcd_state::nmi_io_w )
{
	if(space.debugger_access())
		return;
	logerror("%s: unmapped %s %04x\n", machine().describe_context(), space.name(), offset);
	m_stat |= 8;
	m_maincpu->set_input_line(INPUT_LINE_NMI, PULSE_LINE);
}

READ8_MEMBER( pcd_state::rtc_r )
{
	m_rtc->write(space, 0, offset);
	return m_rtc->read(space, 1);
}

WRITE8_MEMBER( pcd_state::rtc_w )
{
	m_rtc->write(space, 0, offset);
	m_rtc->write(space, 1, data);
}

READ8_MEMBER( pcd_state::stat_r )
{
	return m_stat;
}

WRITE8_MEMBER( pcd_state::stat_w )
{
	m_stat &= ~data;
}

READ8_MEMBER( pcd_state::detect_r )
{
	return 0;
}

WRITE8_MEMBER( pcd_state::detect_w )
{
}

READ8_MEMBER( pcd_state::mcu_r )
{
	return 0x20;
}

WRITE8_MEMBER( pcd_state::mcu_w )
{
}

READ16_MEMBER( pcd_state::dskctl_r )
{
	return m_dskctl;
}

WRITE16_MEMBER( pcd_state::dskctl_w )
{
	floppy_image_device *floppy0 = m_fdc->subdevice<floppy_connector>("0")->get_device();
	floppy_image_device *floppy1 = m_fdc->subdevice<floppy_connector>("1")->get_device();

	COMBINE_DATA(&m_dskctl);

	if((m_dskctl & 1) && floppy0)
		m_fdc->set_floppy(floppy0);
	if((m_dskctl & 2) && floppy1)
		m_fdc->set_floppy(floppy1);

	if(floppy0)
	{
		floppy0->mon_w(!(m_dskctl & 4));
		floppy0->ss_w((m_dskctl & 8) != 0);
	}
	if(floppy1)
	{
		floppy1->mon_w(!(m_dskctl & 4));
		floppy1->ss_w((m_dskctl & 8) != 0);
	}
}

READ8_MEMBER( pcd_state::led_r )
{
	// DIPs?
	// 0x01 no mmu
	// 0x10 enter monitor after post
	// 0x20 enter monitor before post
	return 0x01;
}

WRITE8_MEMBER( pcd_state::led_w )
{
	for(int i = 0; i < 6; i++)
		logerror("%c", (data & (1 << i)) ? '-' : '*');
	logerror("\n");
	m_led = data;
}

SCN2674_DRAW_CHARACTER_MEMBER(pcd_state::display_pixels)
{
	if(lg)
	{
		UINT16 data = m_vram[address];
		data = (data >> 8) | (data << 8);
		for(int i = 0; i < 16; i++)
			bitmap.pix32(y, x + i) = m_palette->pen((data & (1 << (15 - i))) ? 1 : 0);
	}
	else
	{
		UINT8 data = m_charram[(m_vram[address] & 0xff) * 16 + linecount];
		if(cursor && blink)
			data = 0xff;
		for(int i = 0; i < 8; i++)
			bitmap.pix32(y, x + i) = m_palette->pen((data & (1 << (7 - i))) ? 1 : 0);
	}
}

READ8_MEMBER(pcd_state::scsi_r)
{
	UINT8 ret = 0;

	switch(offset)
	{
		case 0:
		case 2:
			ret = m_scsi_data_in->read();
			m_scsi->write_ack(1);
			if(!offset)
				m_maincpu->drq0_w(0);
			break;

		case 1:
			ret = (m_cd << 7) | (m_req << 5) | (m_bsy << 4);
			break;
	}

	return ret;
}

WRITE8_MEMBER(pcd_state::scsi_w)
{
	switch(offset)
	{
		case 0:
			m_scsi_data_out->write(data);
			m_scsi->write_ack(1);
			if(m_cd)
			{
				m_maincpu->drq0_w(0);
				m_req_hack->adjust(attotime::never);
			}
			break;
		case 1:
			if(data & 4)
			{
				m_rst = 1;
				m_scsi->write_rst(1);
				break;
			}
			if(m_rst)
			{
				m_rst = 0;
				m_scsi->write_rst(0);
				break;
			}

			if(!m_bsy)
			{
				m_scsi_data_out->write(0);
				m_scsi->write_sel(1);
			}
			break;
	}
}

WRITE_LINE_MEMBER(pcd_state::write_scsi_bsy)
{
	m_bsy = state ? 1 : 0;
	m_scsi->write_sel(0);
}
WRITE_LINE_MEMBER(pcd_state::write_scsi_cd)
{
	m_cd = state ? 1 : 0;
}
WRITE_LINE_MEMBER(pcd_state::write_scsi_io)
{
	m_io = state ? 1 : 0;
}
WRITE_LINE_MEMBER(pcd_state::write_scsi_msg)
{
	m_msg = state ? 1 : 0;
}

WRITE_LINE_MEMBER(pcd_state::write_scsi_req)
{
	m_req = state ? 1 : 0;
	if(state)
	{
		if(!m_cd)
		{
			m_maincpu->drq0_w(1);
			m_req_hack->adjust(attotime::from_msec(10)); // poke the dmac
		}
		else if(m_msg)
		{
			m_scsi_data_in->read();
			m_scsi->write_ack(1);
		}
	}
	else
		m_scsi->write_ack(0);
}
//**************************************************************************
//  ADDRESS MAPS
//**************************************************************************

static ADDRESS_MAP_START( pcd_map, AS_PROGRAM, 16, pcd_state )
	AM_RANGE(0x00000, 0x7ffff) AM_RAM // fixed 512k for now
	AM_RANGE(0xf0000, 0xf7fff) AM_READONLY AM_WRITE(vram_w) AM_SHARE("vram")
	AM_RANGE(0xfc000, 0xfffff) AM_ROM AM_REGION("bios", 0)
	AM_RANGE(0x00000, 0xfffff) AM_READWRITE8(nmi_io_r, nmi_io_w, 0xffff)
ADDRESS_MAP_END

static ADDRESS_MAP_START( pcd_io, AS_IO, 16, pcd_state )
	ADDRESS_MAP_UNMAP_HIGH
	AM_RANGE(0x0000, 0xefff) AM_READWRITE8(nmi_io_r, nmi_io_w, 0xffff)
	AM_RANGE(0xf000, 0xf7ff) AM_RAM AM_SHARE("nvram")
	AM_RANGE(0xf800, 0xf801) AM_DEVREADWRITE8("pic1", pic8259_device, read, write, 0xffff)
	AM_RANGE(0xf820, 0xf821) AM_DEVREADWRITE8("pic2", pic8259_device, read, write, 0xffff)
	AM_RANGE(0xf840, 0xf841) AM_READWRITE8(stat_r, stat_w, 0x00ff)
	AM_RANGE(0xf840, 0xf841) AM_READWRITE8(led_r, led_w, 0xff00)
	AM_RANGE(0xf880, 0xf8bf) AM_READWRITE8(rtc_r, rtc_w, 0xffff)
	AM_RANGE(0xf900, 0xf903) AM_DEVREADWRITE8("fdc", wd2793_t, read, write, 0xffff)
	AM_RANGE(0xf904, 0xf905) AM_READWRITE(dskctl_r, dskctl_w)
	AM_RANGE(0xf940, 0xf943) AM_READWRITE8(scsi_r, scsi_w, 0xffff)
	AM_RANGE(0xf980, 0xf98f) AM_DEVWRITE8("crtc", scn2674_device, write, 0x00ff)
	AM_RANGE(0xf980, 0xf98f) AM_DEVREAD8("crtc", scn2674_device, read, 0xff00)
	AM_RANGE(0xf9a0, 0xf9a1) AM_WRITE8(vram_sw_w, 0x00ff)
	AM_RANGE(0xf9b0, 0xf9b3) AM_READWRITE8(mcu_r, mcu_w, 0x00ff) // 8741 comms
	AM_RANGE(0xf9c0, 0xf9c3) AM_DEVREADWRITE8("usart1",mc2661_device,read,write,0xffff)  // UARTs
	AM_RANGE(0xf9d0, 0xf9d3) AM_DEVREADWRITE8("usart2",mc2661_device,read,write,0xffff)
	AM_RANGE(0xf9e0, 0xf9e3) AM_DEVREADWRITE8("usart3",mc2661_device,read,write,0xffff)
//  AM_RANGE(0xfa00, 0xfa7f) // pcs4-n (peripheral chip select)
	AM_RANGE(0xfb00, 0xfb01) AM_READWRITE8(detect_r, detect_w, 0xff00) // expansion card detection?
	AM_RANGE(0xfb00, 0xffff) AM_READWRITE8(nmi_io_r, nmi_io_w, 0xffff)
ADDRESS_MAP_END


//**************************************************************************
//  MACHINE DRIVERS
//**************************************************************************

static SLOT_INTERFACE_START( pcd_floppies )
	SLOT_INTERFACE("55f", TEAC_FD_55F)
	SLOT_INTERFACE("55g", TEAC_FD_55G)
	SLOT_INTERFACE("525dsqd", FLOPPY_525_QD) // the devices above cause a crash in floppy_image_format_t::generate_track_from_levels
SLOT_INTERFACE_END

FLOPPY_FORMATS_MEMBER( pcd_state::floppy_formats )
	FLOPPY_PC_FORMAT
FLOPPY_FORMATS_END

static MACHINE_CONFIG_START( pcd, pcd_state )
	MCFG_CPU_ADD("maincpu", I80186, XTAL_16MHz)
	MCFG_CPU_PROGRAM_MAP(pcd_map)
	MCFG_CPU_IO_MAP(pcd_io)
	MCFG_80186_TMROUT1_HANDLER(WRITELINE(pcd_state, i186_timer1_w))
	MCFG_80186_IRQ_SLAVE_ACK(READ8(pcd_state, irq_callback))

	MCFG_CPU_ADD("graphics", I8741, XTAL_16MHz/2)
	MCFG_DEVICE_DISABLE()

	MCFG_TIMER_DRIVER_ADD_PERIODIC("timer0_tick", pcd_state, timer0_tick, attotime::from_hz(XTAL_16MHz / 24)) // adjusted to pass post

	MCFG_PIC8259_ADD("pic1", DEVWRITELINE("maincpu", i80186_cpu_device, int0_w), VCC, NULL)
	MCFG_PIC8259_ADD("pic2", DEVWRITELINE("maincpu", i80186_cpu_device, int1_w), VCC, NULL)

#if 0
	MCFG_RAM_ADD(RAM_TAG)
	MCFG_RAM_DEFAULT_SIZE("256K")
	MCFG_RAM_EXTRA_OPTIONS("512K,1M")
#endif

	// nvram
	MCFG_NVRAM_ADD_1FILL("nvram")

	// floppy disk controller
	MCFG_WD2793x_ADD("fdc", XTAL_16MHz/8/2)
	MCFG_WD_FDC_INTRQ_CALLBACK(DEVWRITELINE("pic1", pic8259_device, ir6_w))
	MCFG_WD_FDC_DRQ_CALLBACK(DEVWRITELINE("maincpu", i80186_cpu_device, drq1_w))

	// floppy drives
	MCFG_FLOPPY_DRIVE_ADD("fdc:0", pcd_floppies, "525dsqd", pcd_state::floppy_formats)
	MCFG_FLOPPY_DRIVE_ADD("fdc:1", pcd_floppies, "525dsqd", pcd_state::floppy_formats)

	// usart
	MCFG_DEVICE_ADD("usart1", MC2661, XTAL_4_9152MHz)
	MCFG_MC2661_RXRDY_HANDLER(DEVWRITELINE("pic1", pic8259_device, ir3_w))
	MCFG_MC2661_TXRDY_HANDLER(DEVWRITELINE("pic1", pic8259_device, ir3_w))
	MCFG_DEVICE_ADD("usart2", MC2661, XTAL_4_9152MHz)
	MCFG_MC2661_RXRDY_HANDLER(DEVWRITELINE("pic1", pic8259_device, ir2_w))
	//MCFG_MC2661_TXRDY_HANDLER(DEVWRITELINE("pic1", pic8259_device, ir2_w)) // this gets stuck high causing the keyboard to not work
	MCFG_MC2661_TXD_HANDLER(DEVWRITELINE("keyboard", pcd_keyboard_device, t0_w))
	MCFG_DEVICE_ADD("usart3", MC2661, XTAL_4_9152MHz)
	MCFG_MC2661_RXRDY_HANDLER(DEVWRITELINE("pic1", pic8259_device, ir4_w))
	MCFG_MC2661_TXRDY_HANDLER(DEVWRITELINE("pic1", pic8259_device, ir4_w))

	// sound hardware
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SOUND_ADD("speaker", SPEAKER_SOUND, 0)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)

	// video hardware
	MCFG_SCREEN_ADD("screen", RASTER)
	MCFG_SCREEN_SIZE(640, 350)
	MCFG_SCREEN_VISIBLE_AREA(0, 639, 0, 349)
	MCFG_SCREEN_REFRESH_RATE(50)
	MCFG_SCREEN_UPDATE_DEVICE("crtc", scn2674_device, screen_update)

	MCFG_GFXDECODE_ADD("gfxdecode", "palette", empty)
	MCFG_PALETTE_ADD_BLACK_AND_WHITE("palette")

	MCFG_SCN2674_VIDEO_ADD("crtc", 0, NULL);
	MCFG_SCN2674_TEXT_CHARACTER_WIDTH(8)
	MCFG_SCN2674_GFX_CHARACTER_WIDTH(16)
	MCFG_SCN2674_DRAW_CHARACTER_CALLBACK_OWNER(pcd_state, display_pixels)

	// rtc
	MCFG_MC146818_ADD("rtc", XTAL_32_768kHz)
	MCFG_MC146818_IRQ_HANDLER(DEVWRITELINE("pic1", pic8259_device, ir7_w))

	MCFG_DEVICE_ADD("keyboard", PCD_KEYBOARD, 0)
	MCFG_PCD_KEYBOARD_OUT_TX_HANDLER(DEVWRITELINE("usart2", mc2661_device, rx_w))

	MCFG_DEVICE_ADD("scsi", SCSI_PORT, 0)
	MCFG_SCSI_DATA_INPUT_BUFFER("scsi_data_in")
	MCFG_SCSI_MSG_HANDLER(WRITELINE(pcd_state, write_scsi_msg))
	MCFG_SCSI_BSY_HANDLER(WRITELINE(pcd_state, write_scsi_bsy))
	MCFG_SCSI_IO_HANDLER(WRITELINE(pcd_state, write_scsi_io))
	MCFG_SCSI_CD_HANDLER(WRITELINE(pcd_state, write_scsi_cd))
	MCFG_SCSI_REQ_HANDLER(WRITELINE(pcd_state, write_scsi_req))

	MCFG_SCSI_OUTPUT_LATCH_ADD("scsi_data_out", "scsi")
	MCFG_DEVICE_ADD("scsi_data_in", INPUT_BUFFER, 0)
	MCFG_SCSIDEV_ADD("scsi:1", "harddisk", OMTI5100, SCSI_ID_0)
MACHINE_CONFIG_END


//**************************************************************************
//  ROM DEFINITIONS
//**************************************************************************

ROM_START( pcd )
	ROM_REGION(0x4000, "bios", 0)
	ROM_LOAD16_BYTE("s26361-d359.d42", 0x0001, 0x2000, CRC(e20244dd) SHA1(0ebc5ddb93baacd9106f1917380de58aac64fe73))
	ROM_LOAD16_BYTE("s26361-d359.d43", 0x0000, 0x2000, CRC(e03db2ec) SHA1(fcae8b0c9e7543706817b0a53872826633361fda))
	ROM_FILL(0xb64, 1, 0xe2)  // post expects 0xd0 fdc command to be instant, give it a delay
	ROM_FILL(0xb65, 1, 0xfe)
	ROM_FILL(0xb35, 1, 0x90)  // fdc delay_register_commit is too long
	ROM_FILL(0xb36, 1, 0x90)
	ROM_FILL(0x3ffe, 1, 0xf4)  // fix csum
	ROM_FILL(0x3fff, 1, 0x3d)

	// gfx card (scn2674 with 8741), to be moved
	ROM_REGION(0x400, "graphics", 0)
	ROM_LOAD("s36361-d321-v1.bin", 0x000, 0x400, CRC(69baeb2a) SHA1(98b9cd0f38c51b4988a3aed0efcf004bedd115ff))
ROM_END


//**************************************************************************
//  GAME DRIVERS
//**************************************************************************

COMP( 1984, pcd, 0, 0, pcd, 0, driver_device, 0, "Siemens", "PC-D", GAME_NOT_WORKING )
