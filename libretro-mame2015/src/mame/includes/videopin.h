/*************************************************************************

    Atari Video Pinball hardware

*************************************************************************/

#include "sound/discrete.h"

/* Discrete Sound Input Nodes */
#define VIDEOPIN_OCTAVE_DATA    NODE_01
#define VIDEOPIN_NOTE_DATA      NODE_02
#define VIDEOPIN_BELL_EN        NODE_03
#define VIDEOPIN_BONG_EN        NODE_04
#define VIDEOPIN_ATTRACT_EN     NODE_05
#define VIDEOPIN_VOL_DATA       NODE_06


class videopin_state : public driver_device
{
public:
	enum
	{
		TIMER_INTERRUPT
	};

	videopin_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_video_ram(*this, "video_ram"),
		m_maincpu(*this, "maincpu"),
		m_discrete(*this, "discrete"),
		m_gfxdecode(*this, "gfxdecode"),
		m_screen(*this, "screen"),
		m_palette(*this, "palette") { }

	attotime m_time_pushed;
	attotime m_time_released;
	UINT8 m_prev;
	UINT8 m_mask;
	required_shared_ptr<UINT8> m_video_ram;
	int m_ball_x;
	int m_ball_y;
	tilemap_t* m_bg_tilemap;
	DECLARE_READ8_MEMBER(videopin_misc_r);
	DECLARE_WRITE8_MEMBER(videopin_led_w);
	DECLARE_WRITE8_MEMBER(videopin_ball_w);
	DECLARE_WRITE8_MEMBER(videopin_video_ram_w);
	DECLARE_WRITE8_MEMBER(videopin_out1_w);
	DECLARE_WRITE8_MEMBER(videopin_out2_w);
	DECLARE_WRITE8_MEMBER(videopin_note_dvsr_w);
	TILEMAP_MAPPER_MEMBER(get_memory_offset);
	TILE_GET_INFO_MEMBER(get_tile_info);
	virtual void machine_reset();
	virtual void video_start();
	UINT32 screen_update_videopin(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	TIMER_CALLBACK_MEMBER(interrupt_callback);
	void update_plunger();
	double calc_plunger_pos();
	required_device<cpu_device> m_maincpu;
	required_device<discrete_device> m_discrete;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;

protected:
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr);
};

/*----------- defined in audio/videopin.c -----------*/
DISCRETE_SOUND_EXTERN( videopin );
