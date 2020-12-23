/***************************************************************************

    Bally Astrocade-based hardware

***************************************************************************/
#include "sound/astrocde.h"
#include "sound/samples.h"
#include "sound/votrax.h"

#define ASTROCADE_CLOCK     (XTAL_14_31818MHz/2)

#define AC_SOUND_PRESENT    (0x01)
#define AC_LIGHTPEN_INTS    (0x02)
#define AC_STARS            (0x04)
#define AC_MONITOR_BW       (0x08)

#define USE_FAKE_VOTRAX     (1)


class astrocde_state : public driver_device
{
public:
	enum
	{
		TIMER_INTERRUPT_OFF,
		TIMER_SCANLINE
	};

	astrocde_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_subcpu(*this, "sub"),
		m_samples(*this, "samples"),
		m_votrax(*this, "votrax"),
		m_astrocade_sound1(*this, "astrocade1"),
		m_videoram(*this, "videoram"),
		m_protected_ram(*this, "protected_ram"),
		m_screen(*this, "screen") { }

	required_device<cpu_device> m_maincpu;
	optional_device<cpu_device> m_subcpu;
	optional_device<samples_device> m_samples;
	optional_device<votrax_sc01_device> m_votrax;
	optional_device<astrocade_device> m_astrocade_sound1;
	optional_shared_ptr<UINT8> m_videoram;
	optional_shared_ptr<UINT8> m_protected_ram;
	required_device<screen_device> m_screen;

	UINT8 m_video_config;
	UINT8 m_sparkle[4];
	char m_totalword[256];
	char *m_totalword_ptr;
	char m_oldword[256];
	int m_plural;
	UINT8 m_port_1_last;
	UINT8 m_port_2_last;
	UINT8 m_ram_write_enable;
	UINT8 m_input_select;
	UINT8 m_profpac_bank;
	UINT8 *m_sparklestar;
	UINT8 m_interrupt_enabl;
	UINT8 m_interrupt_vector;
	UINT8 m_interrupt_scanline;
	UINT8 m_vertical_feedback;
	UINT8 m_horizontal_feedback;
	emu_timer *m_scanline_timer;
	emu_timer *m_intoff_timer;
	UINT8 m_colors[8];
	UINT8 m_colorsplit;
	UINT8 m_bgdata;
	UINT8 m_vblank;
	UINT8 m_video_mode;
	UINT8 m_funcgen_expand_color[2];
	UINT8 m_funcgen_control;
	UINT8 m_funcgen_expand_count;
	UINT8 m_funcgen_rotate_count;
	UINT8 m_funcgen_rotate_data[4];
	UINT8 m_funcgen_shift_prev_data;
	UINT8 m_funcgen_intercept;
	UINT16 m_pattern_source;
	UINT8 m_pattern_mode;
	UINT16 m_pattern_dest;
	UINT8 m_pattern_skip;
	UINT8 m_pattern_width;
	UINT8 m_pattern_height;
	UINT16 *m_profpac_videoram;
	UINT16 m_profpac_palette[16];
	UINT8 m_profpac_colormap[4];
	UINT8 m_profpac_intercept;
	UINT8 m_profpac_vispage;
	UINT8 m_profpac_readpage;
	UINT8 m_profpac_readshift;
	UINT8 m_profpac_writepage;
	UINT8 m_profpac_writemode;
	UINT16 m_profpac_writemask;
	UINT8 m_profpac_vw;
	DECLARE_WRITE8_MEMBER(protected_ram_enable_w);
	DECLARE_READ8_MEMBER(protected_ram_r);
	DECLARE_WRITE8_MEMBER(protected_ram_w);
	DECLARE_WRITE8_MEMBER(seawolf2_lamps_w);
	DECLARE_WRITE8_MEMBER(seawolf2_sound_1_w);
	DECLARE_WRITE8_MEMBER(seawolf2_sound_2_w);
	DECLARE_WRITE8_MEMBER(ebases_trackball_select_w);
	DECLARE_WRITE8_MEMBER(ebases_coin_w);
	DECLARE_READ8_MEMBER(spacezap_io_r);
	DECLARE_READ8_MEMBER(wow_io_r);
	DECLARE_READ8_MEMBER(gorf_io_1_r);
	DECLARE_READ8_MEMBER(gorf_io_2_r);
	DECLARE_READ8_MEMBER(robby_io_r);
	DECLARE_READ8_MEMBER(profpac_io_1_r);
	DECLARE_READ8_MEMBER(profpac_io_2_r);
	DECLARE_WRITE8_MEMBER(profpac_banksw_w);
	DECLARE_READ8_MEMBER(demndrgn_io_r);
	DECLARE_WRITE8_MEMBER(demndrgn_sound_w);
	DECLARE_WRITE8_MEMBER(tenpindx_sound_w);
	DECLARE_WRITE8_MEMBER(tenpindx_lamp_w);
	DECLARE_WRITE8_MEMBER(tenpindx_counter_w);
	DECLARE_WRITE8_MEMBER(tenpindx_lights_w);
	DECLARE_READ8_MEMBER(astrocade_data_chip_register_r);
	DECLARE_WRITE8_MEMBER(astrocade_data_chip_register_w);
	DECLARE_WRITE8_MEMBER(astrocade_funcgen_w);
	DECLARE_WRITE8_MEMBER(astrocade_pattern_board_w);
	DECLARE_WRITE8_MEMBER(profpac_page_select_w);
	DECLARE_READ8_MEMBER(profpac_intercept_r);
	DECLARE_WRITE8_MEMBER(profpac_screenram_ctrl_w);
	DECLARE_READ8_MEMBER(profpac_videoram_r);
	DECLARE_WRITE8_MEMBER(profpac_videoram_w);
	DECLARE_CUSTOM_INPUT_MEMBER(ebases_trackball_r);
	DECLARE_CUSTOM_INPUT_MEMBER(demndragn_joystick_r);
	DECLARE_INPUT_CHANGED_MEMBER(spacezap_monitor);
	DECLARE_DRIVER_INIT(profpac);
	DECLARE_DRIVER_INIT(spacezap);
	DECLARE_DRIVER_INIT(robby);
	DECLARE_DRIVER_INIT(wow);
	DECLARE_DRIVER_INIT(tenpindx);
	DECLARE_DRIVER_INIT(seawolf2);
	DECLARE_DRIVER_INIT(demndrgn);
	DECLARE_DRIVER_INIT(ebases);
	DECLARE_DRIVER_INIT(gorf);
	DECLARE_DRIVER_INIT(astrocde);
	virtual void video_start();
	DECLARE_PALETTE_INIT(astrocde);
	DECLARE_VIDEO_START(profpac);
	DECLARE_PALETTE_INIT(profpac);
	UINT32 screen_update_astrocde(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	UINT32 screen_update_profpac(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	TIMER_CALLBACK_MEMBER(scanline_callback);
	void profbank_banksw_restore();
	inline int mame_vpos_to_astrocade_vpos(int scanline);
	void init_savestate();
	void astrocade_trigger_lightpen(UINT8 vfeedback, UINT8 hfeedback);
	inline void increment_source(UINT8 curwidth, UINT8 *u13ff);
	inline void increment_dest(UINT8 curwidth);
	void execute_blit(address_space &space);
	void init_sparklestar();
	virtual void machine_start();

	/*----------- defined in audio/wow.c -----------*/
	DECLARE_READ8_MEMBER( wow_speech_r );
	CUSTOM_INPUT_MEMBER( wow_speech_status_r );

	/*----------- defined in audio/gorf.c -----------*/
	DECLARE_READ8_MEMBER( gorf_speech_r );
	CUSTOM_INPUT_MEMBER( gorf_speech_status_r );

protected:
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);
};

/*----------- defined in audio/wow.c -----------*/

extern const char *const wow_sample_names[];

/*----------- defined in audio/gorf.c -----------*/

extern const char *const gorf_sample_names[];
