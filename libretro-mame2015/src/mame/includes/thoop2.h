class thoop2_state : public driver_device
{
public:
	thoop2_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_videoram(*this, "videoram"),
		m_vregs(*this, "vregs"),
		m_spriteram(*this, "spriteram"),
		m_maincpu(*this, "maincpu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette") { }

	required_shared_ptr<UINT16> m_videoram;
	required_shared_ptr<UINT16> m_vregs;
	required_shared_ptr<UINT16> m_spriteram;
	int m_sprite_count[5];
	int *m_sprite_table[5];
	tilemap_t *m_pant[2];
	DECLARE_WRITE16_MEMBER(OKIM6295_bankswitch_w);
	DECLARE_WRITE16_MEMBER(thoop2_coin_w);
	DECLARE_READ16_MEMBER(DS5002FP_R);
	DECLARE_WRITE16_MEMBER(thoop2_vram_w);
	TILE_GET_INFO_MEMBER(get_tile_info_thoop2_screen0);
	TILE_GET_INFO_MEMBER(get_tile_info_thoop2_screen1);
	virtual void video_start();
	UINT32 screen_update_thoop2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void thoop2_sort_sprites();
	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect, int pri);
	required_device<cpu_device> m_maincpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
};
