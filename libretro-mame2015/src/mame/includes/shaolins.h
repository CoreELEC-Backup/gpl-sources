class shaolins_state : public driver_device
{
public:
	shaolins_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this,"maincpu"),
		m_spriteram(*this, "spriteram"),
		m_colorram(*this, "colorram"),
		m_videoram(*this, "videoram"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette") { }

	required_device<cpu_device> m_maincpu;
	required_shared_ptr<UINT8> m_spriteram;
	required_shared_ptr<UINT8> m_colorram;
	required_shared_ptr<UINT8> m_videoram;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	int m_palettebank;

	tilemap_t *m_bg_tilemap;
	UINT8 m_nmi_enable;

	DECLARE_WRITE8_MEMBER(shaolins_videoram_w);
	DECLARE_WRITE8_MEMBER(shaolins_colorram_w);
	DECLARE_WRITE8_MEMBER(shaolins_palettebank_w);
	DECLARE_WRITE8_MEMBER(shaolins_scroll_w);
	DECLARE_WRITE8_MEMBER(shaolins_nmi_w);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	virtual void video_start();
	DECLARE_PALETTE_INIT(shaolins);
	UINT32 screen_update_shaolins(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	TIMER_DEVICE_CALLBACK_MEMBER(shaolins_interrupt);
	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect);
};
