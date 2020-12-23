/*************************************************************************

    Run and Gun / Slam Dunk

*************************************************************************/
#include "sound/k054539.h"
#include "machine/k053252.h"
#include "video/k053246_k053247_k055673.h"
#include "video/k053936.h"
#include "video/konami_helper.h"

class rungun_state : public driver_device
{
public:
	rungun_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_soundcpu(*this, "soundcpu"),
		m_k054539_1(*this, "k054539_1"),
		m_k054539_2(*this, "k054539_2"),
		m_k053936(*this, "k053936"),
		m_k055673(*this, "k055673"),
		m_k053252(*this, "k053252"),
		m_sysreg(*this, "sysreg"),
		m_936_videoram(*this, "936_videoram"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette") { }

	/* devices */
	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_soundcpu;
	required_device<k054539_device> m_k054539_1;
	required_device<k054539_device> m_k054539_2;
	required_device<k053936_device> m_k053936;
	required_device<k055673_device> m_k055673;
	required_device<k053252_device> m_k053252;

	/* memory pointers */
	required_shared_ptr<UINT16> m_sysreg;
	required_shared_ptr<UINT16> m_936_videoram;

	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;

	/* video-related */
	tilemap_t   *m_ttl_tilemap;
	tilemap_t   *m_936_tilemap;
	UINT16      m_ttl_vram[0x1000];
	int         m_ttl_gfx_index;
	int         m_sprite_colorbase;

	/* sound */
	UINT8       m_sound_ctrl;
	UINT8       m_sound_status;
	UINT8       m_sound_nmi_clk;

	DECLARE_READ16_MEMBER(rng_sysregs_r);
	DECLARE_WRITE16_MEMBER(rng_sysregs_w);
	DECLARE_WRITE16_MEMBER(sound_cmd1_w);
	DECLARE_WRITE16_MEMBER(sound_cmd2_w);
	DECLARE_WRITE16_MEMBER(sound_irq_w);
	DECLARE_READ16_MEMBER(sound_status_msb_r);
	DECLARE_WRITE8_MEMBER(sound_status_w);
	DECLARE_WRITE8_MEMBER(sound_ctrl_w);
	DECLARE_READ16_MEMBER(rng_ttl_ram_r);
	DECLARE_WRITE16_MEMBER(rng_ttl_ram_w);
	DECLARE_WRITE16_MEMBER(rng_936_videoram_w);
	TILE_GET_INFO_MEMBER(ttl_get_tile_info);
	TILE_GET_INFO_MEMBER(get_rng_936_tile_info);
	DECLARE_WRITE_LINE_MEMBER(k054539_nmi_gen);
	K055673_CB_MEMBER(sprite_callback);

	virtual void machine_start();
	virtual void machine_reset();
	virtual void video_start();
	UINT32 screen_update_rng(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(rng_interrupt);
	INTERRUPT_GEN_MEMBER(audio_interrupt);
};
