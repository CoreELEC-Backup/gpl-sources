/***************************************************************************

  ISA 8 bit Creative Labs Game Blaster Sound Card

***************************************************************************/

#include "emu.h"
#include "gblaster.h"
#include "sound/speaker.h"

/*
  creative labs game blaster (CMS creative music system)
  2 x saa1099 chips
  also on sound blaster 1.0
  option on sound blaster 1.5

  jumperable? normally 0x220
*/
static MACHINE_CONFIG_FRAGMENT( game_blaster_config )
	MCFG_SPEAKER_STANDARD_MONO("mono")
	MCFG_SAA1099_ADD("saa1099.1", 4772720)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
	MCFG_SAA1099_ADD("saa1099.2", 4772720)
	MCFG_SOUND_ROUTE(ALL_OUTPUTS, "mono", 0.50)
MACHINE_CONFIG_END

READ8_MEMBER( isa8_gblaster_device::saa1099_16_r )
{
	return 0xff;
}

WRITE8_MEMBER( isa8_gblaster_device::saa1099_1_16_w )
{
	switch(offset)
	{
		case 0 : m_saa1099_1->control_w( space, offset, data ); break;
		case 1 : m_saa1099_1->data_w( space, offset, data ); break;
	}
}

WRITE8_MEMBER( isa8_gblaster_device::saa1099_2_16_w )
{
	switch(offset)
	{
		case 0 : m_saa1099_2->control_w( space, offset, data ); break;
		case 1 : m_saa1099_2->data_w( space, offset, data ); break;
	}
}

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

const device_type ISA8_GAME_BLASTER = &device_creator<isa8_gblaster_device>;

//-------------------------------------------------
//  machine_config_additions - device-specific
//  machine configurations
//-------------------------------------------------

machine_config_constructor isa8_gblaster_device::device_mconfig_additions() const
{
	return MACHINE_CONFIG_NAME( game_blaster_config );
}

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  isa8_gblaster_device - constructor
//-------------------------------------------------

isa8_gblaster_device::isa8_gblaster_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock) :
		device_t(mconfig, ISA8_GAME_BLASTER, "Game Blaster Sound Card", tag, owner, clock, "isa_gblaster", __FILE__),
		device_isa8_card_interface(mconfig, *this),
		m_saa1099_1(*this, "saa1099.1"),
		m_saa1099_2(*this, "saa1099.2")
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void isa8_gblaster_device::device_start()
{
	set_isa_device();
	m_isa->install_device(0x0220, 0x0221, 0, 0, read8_delegate( FUNC(isa8_gblaster_device::saa1099_16_r), this ), write8_delegate( FUNC(isa8_gblaster_device::saa1099_1_16_w), this ) );
	m_isa->install_device(0x0222, 0x0223, 0, 0, read8_delegate( FUNC(isa8_gblaster_device::saa1099_16_r), this ), write8_delegate( FUNC(isa8_gblaster_device::saa1099_2_16_w), this ) );
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void isa8_gblaster_device::device_reset()
{
}
