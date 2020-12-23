class bigstrkb_state : public driver_device
{
public:
	bigstrkb_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_videoram2(*this, "videoram2"),
		m_videoram3(*this, "videoram3"),
		m_videoram(*this, "videoram"),
		m_spriteram(*this, "spriteram"),
		m_vidreg1(*this, "vidreg1"),
		m_vidreg2(*this, "vidreg2"),
		m_maincpu(*this, "maincpu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette") { }

	tilemap_t *m_tilemap;
	tilemap_t *m_tilemap2;
	tilemap_t *m_tilemap3;

	required_shared_ptr<UINT16> m_videoram2;
	required_shared_ptr<UINT16> m_videoram3;
	required_shared_ptr<UINT16> m_videoram;
	required_shared_ptr<UINT16> m_spriteram;

	required_shared_ptr<UINT16> m_vidreg1;
	required_shared_ptr<UINT16> m_vidreg2;

	DECLARE_WRITE16_MEMBER(bsb_videoram_w);
	DECLARE_WRITE16_MEMBER(bsb_videoram2_w);
	DECLARE_WRITE16_MEMBER(bsb_videoram3_w);
	TILEMAP_MAPPER_MEMBER(bsb_bg_scan);
	TILE_GET_INFO_MEMBER(get_bsb_tile_info);
	TILE_GET_INFO_MEMBER(get_bsb_tile2_info);
	TILE_GET_INFO_MEMBER(get_bsb_tile3_info);
	virtual void video_start();
	UINT32 screen_update_bigstrkb(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect );
	required_device<cpu_device> m_maincpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
};
