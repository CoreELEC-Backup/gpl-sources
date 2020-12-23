#ifndef _INCLUDES_CDI_H_
#define _INCLUDES_CDI_H_

#include "machine/cdi070.h"
#include "machine/cdislave.h"
#include "machine/cdicdic.h"
#include "sound/dmadac.h"
#include "video/mcd212.h"

/*----------- driver state -----------*/

#define CLOCK_A XTAL_30MHz
#define CLOCK_B XTAL_19_6608MHz

class cdi_state : public driver_device
{
public:
	cdi_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_planea(*this, "planea"),
		m_planeb(*this, "planeb"),
		m_input1(*this, "INPUT1"),
		m_input2(*this, "INPUT2"),
		m_mousex(*this, "MOUSEX"),
		m_mousey(*this, "MOUSEY"),
		m_mousebtn(*this, "MOUSEBTN"),
		m_slave(*this, "slave"),
		m_scc(*this, "scc68070"),
		m_cdic(*this, "cdic"),
		m_cdda(*this, "cdda"),
		m_mcd212(*this, "mcd212"){ }

	required_device<cpu_device> m_maincpu;
	required_shared_ptr<UINT16> m_planea;
	required_shared_ptr<UINT16> m_planeb;
	optional_ioport m_input1;
	optional_ioport m_input2;
	required_ioport m_mousex;
	required_ioport m_mousey;
	required_ioport m_mousebtn;
	required_device<cdislave_device> m_slave;
	required_device<cdi68070_device> m_scc;
	required_device<cdicdic_device> m_cdic;
	required_device<cdda_device> m_cdda;
	required_device<mcd212_device> m_mcd212;

	dmadac_sound_device *m_dmadac[2];

	INTERRUPT_GEN_MEMBER( mcu_frame );

	UINT8 m_timer_set;
	emu_timer *m_test_timer;
	bitmap_rgb32 m_lcdbitmap;
	DECLARE_INPUT_CHANGED_MEMBER(mcu_input);
	virtual void machine_start();
	virtual void video_start();
	DECLARE_MACHINE_RESET(cdi);
	DECLARE_MACHINE_RESET(quizrd12);
	DECLARE_MACHINE_RESET(quizrd17);
	DECLARE_MACHINE_RESET(quizrd18);
	DECLARE_MACHINE_RESET(quizrd22);
	DECLARE_MACHINE_RESET(quizrd23);
	DECLARE_MACHINE_RESET(quizrd32);
	DECLARE_MACHINE_RESET(quizrd34);
	DECLARE_MACHINE_RESET(quizrr40);
	DECLARE_MACHINE_RESET(quizrr41);
	DECLARE_MACHINE_RESET(quizrr42);
	UINT32 screen_update_cdimono1(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	UINT32 screen_update_cdimono1_lcd(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
};

/*----------- debug defines -----------*/

#define VERBOSE_LEVEL   (0)

#define ENABLE_VERBOSE_LOG (0)

#define ENABLE_UART_PRINTING (0)

#endif // _INCLUDES_CDI_H_
