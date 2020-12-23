/***************************************************************************

There are three IRQ sources:
- IRQ0
- IRQ1 = IRQA from the video PIA
- IRQ2 = IRQA from the IEEE488 PIA

Interrupt handling on the Osborne-1 is a bit awkward. When an interrupt is
taken by the Z80 the ROMMODE is enabled on each fetch of an instruction
byte. During execution of an instruction the previous ROMMODE setting seems
to be used. Side effect of this is that when an interrupt is taken and the
stack pointer is pointing to 0000-3FFF then the return address will still
be written to RAM if RAM was switched in.

***************************************************************************/

#include "includes/osborne1.h"

#define RAMMODE     (0x01)


WRITE8_MEMBER( osborne1_state::osborne1_0000_w )
{
	/* Check whether regular RAM is enabled */
	if ( ! m_bank2_enabled || ( m_in_irq_handler && m_bankswitch == RAMMODE ) )
	{
		m_ram->pointer()[ offset ] = data;
	}
}


WRITE8_MEMBER( osborne1_state::osborne1_1000_w )
{
	/* Check whether regular RAM is enabled */
	if ( ! m_bank2_enabled || ( m_in_irq_handler && m_bankswitch == RAMMODE ) )
	{
		m_ram->pointer()[ 0x1000 + offset ] = data;
	}
}


READ8_MEMBER( osborne1_state::osborne1_2000_r )
{
	UINT8   data = 0xFF;

	/* Check whether regular RAM is enabled */
	if ( ! m_bank2_enabled )
	{
		data = m_ram->pointer()[ 0x2000 + offset ];
	}
	else
	{
		switch( offset & 0x0F00 )
		{
		case 0x100: /* Floppy */
			data = m_fdc->read( space, offset & 0x03 );
			break;
		case 0x200: /* Keyboard */
			/* Row 0 */
			if ( offset & 0x01 )    data &= m_row0->read();
			/* Row 1 */
			if ( offset & 0x02 )    data &= m_row1->read();
			/* Row 2 */
			if ( offset & 0x04 )    data &= m_row3->read();
			/* Row 3 */
			if ( offset & 0x08 )    data &= m_row4->read();
			/* Row 4 */
			if ( offset & 0x10 )    data &= m_row5->read();
			/* Row 5 */
			if ( offset & 0x20 )    data &= m_row2->read();
			/* Row 6 */
			if ( offset & 0x40 )    data &= m_row6->read();
			/* Row 7 */
			if ( offset & 0x80 )    data &= m_row7->read();
			break;
		case 0x900: /* IEEE488 PIA */
			data = m_pia0->read(space, offset & 0x03 );
			break;
		case 0xA00: /* Serial */
			break;
		case 0xC00: /* Video PIA */
			data = m_pia1->read(space, offset & 0x03 );
			break;
		}
	}
	return data;
}


WRITE8_MEMBER( osborne1_state::osborne1_2000_w )
{
	/* Check whether regular RAM is enabled */
	if ( ! m_bank2_enabled )
	{
		m_ram->pointer()[ 0x2000 + offset ] = data;
	}
	else
	{
		if ( m_in_irq_handler && m_bankswitch == RAMMODE )
		{
			m_ram->pointer()[ 0x2000 + offset ] = data;
		}
		/* Handle writes to the I/O area */
		switch( offset & 0x0F00 )
		{
		case 0x100: /* Floppy */
			m_fdc->write(space, offset & 0x03, data );
			break;
		case 0x900: /* IEEE488 PIA */
			m_pia0->write(space, offset & 0x03, data );
			break;
		case 0xA00: /* Serial */
			break;
		case 0xC00: /* Video PIA */
			m_pia1->write(space, offset & 0x03, data );
			break;
		}
	}
}


WRITE8_MEMBER( osborne1_state::osborne1_3000_w )
{
	/* Check whether regular RAM is enabled */
	if ( ! m_bank2_enabled || ( m_in_irq_handler && m_bankswitch == RAMMODE ) )
	{
		m_ram->pointer()[ 0x3000 + offset ] = data;
	}
}


WRITE8_MEMBER( osborne1_state::osborne1_videoram_w )
{
	/* Check whether the video attribute section is enabled */
	if ( m_bank3_enabled )
		data |= 0x7F;

	m_bank4_ptr[offset] = data;
}


WRITE8_MEMBER( osborne1_state::osborne1_bankswitch_w )
{
	switch( offset )
	{
	case 0x00:
		m_bank2_enabled = 1;
		m_bank3_enabled = 0;
		break;
	case 0x01:
		m_bank2_enabled = 0;
		m_bank3_enabled = 0;
		break;
	case 0x02:
		m_bank2_enabled = 1;
		m_bank3_enabled = 1;
		break;
	case 0x03:
		m_bank2_enabled = 1;
		m_bank3_enabled = 0;
		break;
	}
	if ( m_bank2_enabled )
	{
		m_bank1->set_base(m_region_maincpu->base() );
		m_bank2->set_base(m_empty_4K );
		m_bank3->set_base(m_empty_4K );
	}
	else
	{
		m_bank1->set_base(m_ram->pointer() );
		m_bank2->set_base(m_ram->pointer() + 0x1000 );
		m_bank3->set_base(m_ram->pointer() + 0x3000 );
	}
	m_bank4_ptr = m_ram->pointer() + ( ( m_bank3_enabled ) ? 0x10000 : 0xF000 );
	m_bank4->set_base(m_bank4_ptr );
	m_bankswitch = offset;
	m_in_irq_handler = 0;
}


DIRECT_UPDATE_MEMBER(osborne1_state::osborne1_opbase)
{
	if ( ( address & 0xF000 ) == 0x2000 )
	{
		if ( ! m_bank2_enabled )
		{
			direct.explicit_configure(0x2000, 0x2fff, 0x0fff, m_ram->pointer() + 0x2000);
			return ~0;
		}
	}
	return address;
}


WRITE_LINE_MEMBER( osborne1_state::ieee_pia_irq_a_func )
{
	m_pia_0_irq_state = state;
	m_maincpu->set_input_line(0, ( m_pia_1_irq_state ) ? ASSERT_LINE : CLEAR_LINE);
}


READ8_MEMBER( osborne1_state::ieee_pia_pb_r )
{
	/*

	    bit     description

	    0
	    1
	    2
	    3       EOI
	    4
	    5       DAV
	    6       NDAC
	    7       NRFD

	*/

	UINT8 data = 0;

	data |= m_ieee->eoi_r() << 3;
	data |= m_ieee->dav_r() << 5;
	data |= m_ieee->ndac_r() << 6;
	data |= m_ieee->nrfd_r() << 7;

	return data;
}


WRITE8_MEMBER( osborne1_state::ieee_pia_pb_w )
{
	/*

	    bit     description

	    0
	    1
	    2
	    3       EOI
	    4       ATN
	    5       DAV
	    6       NDAC
	    7       NRFD

	*/

	m_ieee->eoi_w(BIT(data, 3));
	m_ieee->atn_w(BIT(data, 4));
	m_ieee->dav_w(BIT(data, 5));
	m_ieee->ndac_w(BIT(data, 6));
	m_ieee->nrfd_w(BIT(data, 7));
}


WRITE_LINE_MEMBER( osborne1_state::video_pia_out_cb2_dummy )
{
}


WRITE8_MEMBER( osborne1_state::video_pia_port_a_w )
{
	m_fdc->dden_w(BIT(data, 0));

	data -= 0xea; // remove bias

	m_new_start_x = (data >> 1);
	if (m_new_start_x)
		m_new_start_x--;

	//logerror("Video pia port a write: %02X, density set to %s\n", data, data & 1 ? "FM" : "MFM" );
}


WRITE8_MEMBER( osborne1_state::video_pia_port_b_w )
{
	m_new_start_y = data & 0x1F;
	m_beep_state = BIT(data, 5);

	if (BIT(data, 6))
	{
		m_fdc->set_floppy(m_floppy0);
		m_floppy0->mon_w(0);
	}
	else if (BIT(data, 7))
	{
		m_fdc->set_floppy(m_floppy1);
		m_floppy1->mon_w(0);
	}
	else
	{
		m_fdc->set_floppy(NULL);
	}

	//logerror("Video pia port b write: %02X\n", data );
}


WRITE_LINE_MEMBER( osborne1_state::video_pia_irq_a_func )
{
	m_pia_1_irq_state = state;
	m_maincpu->set_input_line(0, ( m_pia_1_irq_state ) ? ASSERT_LINE : CLEAR_LINE);
}


//static const struct aica6850_interface osborne1_6850_config =
//{
//  10, /* tx_clock */
//  10, /* rx_clock */
//  NULL,   /* rx_pin */
//  NULL,   /* tx_pin */
//  NULL,   /* cts_pin */
//  NULL,   /* rts_pin */
//  NULL,   /* dcd_pin */
//  NULL    /* int_callback */
//};


void osborne1_state::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	switch (id)
	{
	case TIMER_VIDEO:
		osborne1_video_callback(ptr, param);
		break;
	case TIMER_SETUP:
		setup_osborne1(ptr, param);
		break;
	default:
		assert_always(FALSE, "Unknown id in osborne1_state::device_timer");
	}
}


TIMER_CALLBACK_MEMBER(osborne1_state::osborne1_video_callback)
{
	int y = machine().first_screen()->vpos();
	UINT8 ra=0,chr,gfx,dim;
	UINT16 x,ma;

	/* Check for start of frame */
	if ( y == 0 )
	{
		/* Clear CA1 on video PIA */
		m_pia1->ca1_w(0);
	}
	if ( y == 240 )
	{
		/* Set CA1 on video PIA */
		m_pia1->ca1_w(1);
	}
	if ( y < 240 )
	{
		ra = y % 10;
		/* Draw a line of the display */
		ma = (m_new_start_y + (y/10)) * 128 + m_new_start_x;
		UINT16 *p = &m_bitmap.pix16(y);

		for ( x = 0; x < 52; x++ )
		{
			chr = m_ram->pointer()[ 0xF000 + ( (ma+x) & 0xFFF ) ];
			dim = m_ram->pointer()[ 0x10000 + ( (ma+x) & 0xFFF ) ] & 0x80;

			if ( (chr & 0x80) && (ra == 9) )
				gfx = 0xFF;
			else
				gfx = m_p_chargen[ (ra << 7) | ( chr & 0x7F ) ];

			/* Display a scanline of a character */
			*p++ = BIT(gfx, 7) ? ( dim ? 1 : 2 ) : 0;
			*p++ = BIT(gfx, 6) ? ( dim ? 1 : 2 ) : 0;
			*p++ = BIT(gfx, 5) ? ( dim ? 1 : 2 ) : 0;
			*p++ = BIT(gfx, 4) ? ( dim ? 1 : 2 ) : 0;
			*p++ = BIT(gfx, 3) ? ( dim ? 1 : 2 ) : 0;
			*p++ = BIT(gfx, 2) ? ( dim ? 1 : 2 ) : 0;
			*p++ = BIT(gfx, 1) ? ( dim ? 1 : 2 ) : 0;
			*p++ = BIT(gfx, 0) ? ( dim ? 1 : 2 ) : 0;
		}
	}

	if ( (ra==2) || (ra== 6) )
	{
		m_beep->set_state( m_beep_state );
	}
	else
	{
		m_beep->set_state( 0 );
	}

	m_video_timer->adjust(machine().first_screen()->time_until_pos(y + 1, 0 ));
}

TIMER_CALLBACK_MEMBER(osborne1_state::setup_osborne1)
{
	m_beep->set_state( 0 );
	m_beep->set_frequency( 300 /* 60 * 240 / 2 */ );
	m_pia1->ca1_w(0);
}

void osborne1_state::machine_reset()
{
	address_space& space = m_maincpu->space(AS_PROGRAM);
	/* Initialize memory configuration */
	osborne1_bankswitch_w( space, 0x00, 0 );

	m_pia_0_irq_state = FALSE;
	m_pia_1_irq_state = FALSE;
	m_in_irq_handler = 0;

	m_p_chargen = memregion( "chargen" )->base();

	memset( m_ram->pointer() + 0x10000, 0xFF, 0x1000 );

	space.set_direct_update_handler(direct_update_delegate(FUNC(osborne1_state::osborne1_opbase), this));
}


DRIVER_INIT_MEMBER(osborne1_state,osborne1)
{
	m_empty_4K = auto_alloc_array(machine(), UINT8, 0x1000 );
	memset( m_empty_4K, 0xFF, 0x1000 );

	/* Configure the 6850 ACIA */
//  acia6850_config( 0, &osborne1_6850_config );
	m_video_timer = timer_alloc(TIMER_VIDEO);
	m_video_timer->adjust(machine().first_screen()->time_until_pos(1, 0 ));

	timer_set(attotime::zero, TIMER_SETUP);
}


void osborne1_state::video_start()
{
	machine().first_screen()->register_screen_bitmap(m_bitmap);
}

UINT32 osborne1_state::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	copybitmap(bitmap, m_bitmap, 0, 0, 0, 0, cliprect);
	return 0;
}

/****************************************************************
    Osborne1 specific daisy chain code
****************************************************************/

const device_type OSBORNE1_DAISY = &device_creator<osborne1_daisy_device>;

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  z80ctc_device - constructor
//-------------------------------------------------
osborne1_daisy_device::osborne1_daisy_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock)
	: device_t(mconfig, OSBORNE1_DAISY, "Osborne 1 daisy", tag, owner, clock, "osborne1_daisy", __FILE__),
		device_z80daisy_interface(mconfig, *this)
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void osborne1_daisy_device::device_start()
{
}

//**************************************************************************
//  DAISY CHAIN INTERFACE
//**************************************************************************

//-------------------------------------------------
//  z80daisy_irq_state - return the overall IRQ
//  state for this device
//-------------------------------------------------

int osborne1_daisy_device::z80daisy_irq_state()
{
	osborne1_state *state = machine().driver_data<osborne1_state>();
	return ( state->m_pia_1_irq_state ? Z80_DAISY_INT : 0 );
}


//-------------------------------------------------
//  z80daisy_irq_ack - acknowledge an IRQ and
//  return the appropriate vector
//-------------------------------------------------

int osborne1_daisy_device::z80daisy_irq_ack()
{
	osborne1_state *state = machine().driver_data<osborne1_state>();
	/* Enable ROM and I/O when IRQ is acknowledged */
	UINT8 old_bankswitch = state->m_bankswitch;
	address_space& space = state->m_maincpu->space(AS_PROGRAM);

	state->osborne1_bankswitch_w( space, 0, 0 );
	state->m_bankswitch = old_bankswitch;
	state->m_in_irq_handler = 1;
	return 0xF8;
}

//-------------------------------------------------
//  z80daisy_irq_reti - clear the interrupt
//  pending state to allow other interrupts through
//-------------------------------------------------

void osborne1_daisy_device::z80daisy_irq_reti()
{
}
