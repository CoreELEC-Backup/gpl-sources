// license:MAME|LGPL-2.1+
// copyright-holders:Michael Zapf
/****************************************************************************

    TI-99 Speech Synthesizer
    See spchsyn.c for documentation

    Michael Zapf, October 2010
    February 2012: Rewritten as class

*****************************************************************************/

#ifndef __TISPEECH__
#define __TISPEECH__

#include "emu.h"
#include "peribox.h"
#include "sound/tms5220.h"

extern const device_type TI99_SPEECH;

class ti_speech_synthesizer_device : public ti_expansion_card_device
{
public:
	ti_speech_synthesizer_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	DECLARE_READ8Z_MEMBER(readz);
	DECLARE_WRITE8_MEMBER(write);
	DECLARE_SETADDRESS_DBIN_MEMBER(setaddress_dbin);

	DECLARE_READ8Z_MEMBER(crureadz) { };
	DECLARE_WRITE8_MEMBER(cruwrite) { };

	DECLARE_WRITE_LINE_MEMBER( speech_ready );

	DECLARE_READ8_MEMBER( spchrom_read );
	DECLARE_WRITE8_MEMBER( spchrom_load_address );
	DECLARE_WRITE8_MEMBER( spchrom_read_and_branch );

protected:
	virtual void            device_start();
	virtual void            device_reset(void);
	virtual const rom_entry *device_rom_region() const;
	virtual machine_config_constructor device_mconfig_additions() const;
	virtual void            device_config_complete();

private:
	cd2501e_device *m_vsp;
	bool            m_read_mode;
};

#endif
