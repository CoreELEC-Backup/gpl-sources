/*************************************************************************

    Glass

*************************************************************************/

class glass_state : public driver_device
{
public:
	glass_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_videoram(*this, "videoram"),
		m_vregs(*this, "vregs"),
		m_spriteram(*this, "spriteram"),
		m_mainram(*this, "mainram"),
		m_maincpu(*this, "maincpu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette") { }

	/* memory pointers */
	required_shared_ptr<UINT16> m_videoram;
	required_shared_ptr<UINT16> m_vregs;
	required_shared_ptr<UINT16> m_spriteram;
	required_shared_ptr<UINT16> m_mainram;
//      UINT16 *    m_paletteram;    // currently this uses generic palette handling

	/* video-related */
	tilemap_t     *m_pant[2];
	bitmap_ind16    *m_screen_bitmap;

	/* misc */
	int         m_current_bit;
	int         m_current_command;
	int         m_cause_interrupt;
	int         m_blitter_serial_buffer[5];
	DECLARE_WRITE16_MEMBER(clr_int_w);
	DECLARE_WRITE16_MEMBER(OKIM6295_bankswitch_w);
	DECLARE_WRITE16_MEMBER(glass_coin_w);
	DECLARE_WRITE16_MEMBER(glass_blitter_w);
	DECLARE_WRITE16_MEMBER(glass_vram_w);

	DECLARE_READ16_MEMBER( glass_mainram_r );
	DECLARE_WRITE16_MEMBER( glass_mainram_w );

	DECLARE_DRIVER_INIT(glass);
	TILE_GET_INFO_MEMBER(get_tile_info_glass_screen0);
	TILE_GET_INFO_MEMBER(get_tile_info_glass_screen1);
	virtual void machine_start();
	virtual void machine_reset();
	virtual void video_start();
	UINT32 screen_update_glass(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(glass_interrupt);
	void draw_sprites( bitmap_ind16 &bitmap, const rectangle &cliprect );
	void glass_ROM16_split_gfx( const char *src_reg, const char *dst_reg, int start, int length, int dest1, int dest2 );
	required_device<cpu_device> m_maincpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
};
