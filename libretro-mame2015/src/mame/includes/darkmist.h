#include "audio/t5182.h"

class darkmist_state : public driver_device
{
public:
	darkmist_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_t5182(*this, "t5182"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette"),
		m_spritebank(*this, "spritebank"),
		m_scroll(*this, "scroll"),
		m_videoram(*this, "videoram"),
		m_workram(*this, "workram"),
		m_spriteram(*this, "spriteram") { }

	required_device<cpu_device> m_maincpu;
	required_device<t5182_device> m_t5182;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;

	required_shared_ptr<UINT8> m_spritebank;
	required_shared_ptr<UINT8> m_scroll;
	required_shared_ptr<UINT8> m_videoram;
	required_shared_ptr<UINT8> m_workram;
	required_shared_ptr<UINT8> m_spriteram;

	int m_hw;
	tilemap_t *m_bgtilemap;
	tilemap_t *m_fgtilemap;
	tilemap_t *m_txtilemap;

	DECLARE_WRITE8_MEMBER(hw_w);

	TILE_GET_INFO_MEMBER(get_bgtile_info);
	TILE_GET_INFO_MEMBER(get_fgtile_info);
	TILE_GET_INFO_MEMBER(get_txttile_info);

	DECLARE_DRIVER_INIT(darkmist);
	virtual void video_start();
	DECLARE_PALETTE_INIT(darkmist);

	UINT32 screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void decrypt_gfx();
	void decrypt_snd();

	TIMER_DEVICE_CALLBACK_MEMBER(scanline);
};
