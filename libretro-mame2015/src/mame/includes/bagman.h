
class bagman_state : public driver_device
{
public:
	bagman_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_videoram(*this, "videoram"),
		m_colorram(*this, "colorram"),
		m_video_enable(*this, "video_enable"),
		m_spriteram(*this, "spriteram"),
		m_maincpu(*this, "maincpu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette")  { }

	UINT8 m_ls259_buf[8];
	UINT8 m_p1_res;
	UINT8 m_p1_old_val;
	UINT8 m_p2_res;
	UINT8 m_p2_old_val;
	required_shared_ptr<UINT8> m_videoram;
	required_shared_ptr<UINT8> m_colorram;
	required_shared_ptr<UINT8> m_video_enable;

	/*table holds outputs of all ANDs (after AND map)*/
	UINT8 m_andmap[64];

	/*table holds inputs (ie. not x, x, not q, q) to the AND map*/
	UINT8 m_columnvalue[32];

	/*8 output pins (actually 6 output and 2 input/output)*/
	UINT8 m_outvalue[8];

	tilemap_t *m_bg_tilemap;
	required_shared_ptr<UINT8> m_spriteram;

	UINT8 m_irq_mask;
	DECLARE_WRITE8_MEMBER(bagman_coin_counter_w);
	DECLARE_WRITE8_MEMBER(irq_mask_w);
	DECLARE_WRITE8_MEMBER(bagman_pal16r6_w);
	DECLARE_READ8_MEMBER(bagman_pal16r6_r);
	void update_pal();
	DECLARE_WRITE8_MEMBER(bagman_videoram_w);
	DECLARE_WRITE8_MEMBER(bagman_colorram_w);
	DECLARE_WRITE8_MEMBER(bagman_flipscreen_w);
	DECLARE_WRITE8_MEMBER(bagman_ls259_w);
	DECLARE_READ8_MEMBER(dial_input_p1_r);
	DECLARE_READ8_MEMBER(dial_input_p2_r);
	DECLARE_DRIVER_INIT(bagman);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	DECLARE_MACHINE_RESET(bagman);
	DECLARE_VIDEO_START(bagman);
	DECLARE_PALETTE_INIT(bagman);
	UINT32 screen_update_bagman(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(vblank_irq);
	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect);
	required_device<cpu_device> m_maincpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
};

/*----------- timings -----------*/

#define BAGMAN_MAIN_CLOCK   XTAL_18_432MHz
#define BAGMAN_HCLK         (BAGMAN_MAIN_CLOCK / 3)
#define BAGMAN_H0           (BAGMAN_HCLK / 2)
#define BAGMAN_H1           (BAGMAN_H0   / 2)
#define HTOTAL              ((0x100-0x40)*2)
#define HBEND               (0x00)
#define HBSTART             (0x100)
#define VTOTAL              ((0x100-0x7c)*2)

/* the following VBEND/VBSTART are used for compsync
 * #define VBEND                (0x08)
 * #define VBSTART              (0x100)
 *
 * However VBSYQ (and INTQ) is generated using the following values:
 */
#define VBEND               (0x10)
#define VBSTART             (0xf0)
