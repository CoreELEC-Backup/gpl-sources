#include "cpu/z80/z80.h"
#include "machine/i8255.h"

class system1_state : public driver_device
{
public:
	system1_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_ppi8255(*this, "ppi8255"),
		m_ram(*this, "ram"),
		m_spriteram(*this, "spriteram"),
		m_nob_mcu_latch(*this, "nob_mcu_latch"),
		m_nob_mcu_status(*this, "nob_mcu_status"),
		m_maincpu(*this, "maincpu"),
		m_soundcpu(*this, "soundcpu"),
		m_mcu(*this, "mcu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_screen(*this, "screen"),
		m_palette(*this, "palette"),
		m_generic_paletteram_8(*this, "paletteram") { }

	optional_device<i8255_device>  m_ppi8255;
	required_shared_ptr<UINT8> m_ram;
	required_shared_ptr<UINT8> m_spriteram;
	optional_shared_ptr<UINT8> m_nob_mcu_latch;
	optional_shared_ptr<UINT8> m_nob_mcu_status;

	UINT8 *m_videoram;
	void (system1_state::*m_videomode_custom)(UINT8 data, UINT8 prevdata);
	UINT8 m_mute_xor;
	UINT8 m_dakkochn_mux_data;
	UINT8 m_videomode_prev;
	UINT8 m_mcu_control;
	UINT8 m_nob_maincpu_latch;
	int m_nobb_inport23_step;
	UINT8 *m_mix_collide;
	UINT8 m_mix_collide_summary;
	UINT8 *m_sprite_collide;
	UINT8 m_sprite_collide_summary;
	bitmap_ind16 m_sprite_bitmap;
	UINT8 m_video_mode;
	UINT8 m_videoram_bank;
	tilemap_t *m_tilemap_page[8];
	UINT8 m_tilemap_pages;

	DECLARE_WRITE8_MEMBER(videomode_w);
	DECLARE_READ8_MEMBER(sound_data_r);
	DECLARE_WRITE8_MEMBER(soundport_w);
	DECLARE_WRITE8_MEMBER(mcu_control_w);
	DECLARE_WRITE8_MEMBER(mcu_io_w);
	DECLARE_READ8_MEMBER(mcu_io_r);
	DECLARE_WRITE8_MEMBER(nob_mcu_control_p2_w);
	DECLARE_READ8_MEMBER(nob_maincpu_latch_r);
	DECLARE_WRITE8_MEMBER(nob_maincpu_latch_w);
	DECLARE_READ8_MEMBER(nob_mcu_status_r);
	DECLARE_READ8_MEMBER(nobb_inport1c_r);
	DECLARE_READ8_MEMBER(nobb_inport22_r);
	DECLARE_READ8_MEMBER(nobb_inport23_r);
	DECLARE_WRITE8_MEMBER(nobb_outport24_w);
	DECLARE_READ8_MEMBER(nob_start_r);
	DECLARE_READ8_MEMBER(shtngmst_gunx_r);
	DECLARE_WRITE8_MEMBER(system1_videomode_w);
	DECLARE_READ8_MEMBER(system1_mixer_collision_r);
	DECLARE_WRITE8_MEMBER(system1_mixer_collision_w);
	DECLARE_WRITE8_MEMBER(system1_mixer_collision_reset_w);
	DECLARE_READ8_MEMBER(system1_sprite_collision_r);
	DECLARE_WRITE8_MEMBER(system1_sprite_collision_w);
	DECLARE_WRITE8_MEMBER(system1_sprite_collision_reset_w);
	DECLARE_READ8_MEMBER(system1_videoram_r);
	DECLARE_WRITE8_MEMBER(system1_videoram_w);
	DECLARE_WRITE8_MEMBER(system1_paletteram_w);
	DECLARE_CUSTOM_INPUT_MEMBER(dakkochn_mux_data_r);
	DECLARE_CUSTOM_INPUT_MEMBER(dakkochn_mux_status_r);
	DECLARE_WRITE8_MEMBER(sound_control_w);

	DECLARE_DRIVER_INIT(nobb);
	DECLARE_DRIVER_INIT(wboy2);
	DECLARE_DRIVER_INIT(imsorry);
	DECLARE_DRIVER_INIT(pitfall2);
	DECLARE_DRIVER_INIT(dakkochn);
	DECLARE_DRIVER_INIT(bootleg);
	DECLARE_DRIVER_INIT(wboysys2);
	DECLARE_DRIVER_INIT(shtngmst);
	DECLARE_DRIVER_INIT(wboyo);
	DECLARE_DRIVER_INIT(swat);
	DECLARE_DRIVER_INIT(regulus);
	DECLARE_DRIVER_INIT(bank0c);
	DECLARE_DRIVER_INIT(blockgal);
	DECLARE_DRIVER_INIT(nob);
	DECLARE_DRIVER_INIT(mrviking);
	DECLARE_DRIVER_INIT(teddybb);
	DECLARE_DRIVER_INIT(flicky);
	DECLARE_DRIVER_INIT(bank44);
	DECLARE_DRIVER_INIT(myherok);
	DECLARE_DRIVER_INIT(wmatch);
	DECLARE_DRIVER_INIT(bank00);
	DECLARE_DRIVER_INIT(myheroj);
	DECLARE_DRIVER_INIT(ufosensi);
	DECLARE_DRIVER_INIT(nprinces);
	DECLARE_DRIVER_INIT(wbml);
	DECLARE_DRIVER_INIT(bootsys2);
	DECLARE_DRIVER_INIT(bullfgtj);
	DECLARE_DRIVER_INIT(wboy);
	DECLARE_DRIVER_INIT(hvymetal);
	DECLARE_DRIVER_INIT(gardiab);
	DECLARE_DRIVER_INIT(4dwarrio);
	DECLARE_DRIVER_INIT(choplift);
	DECLARE_DRIVER_INIT(seganinj);
	DECLARE_DRIVER_INIT(gardia);
	DECLARE_DRIVER_INIT(spatter);
	TILE_GET_INFO_MEMBER(tile_get_info);
	virtual void machine_start();
	virtual void machine_reset();
	virtual void video_start();
	DECLARE_MACHINE_START(system2);
	DECLARE_VIDEO_START(system2);
	UINT32 screen_update_system1(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	UINT32 screen_update_system2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	UINT32 screen_update_system2_rowscroll(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(mcu_irq_assert);
	TIMER_DEVICE_CALLBACK_MEMBER(soundirq_gen);
	TIMER_DEVICE_CALLBACK_MEMBER(mcu_t0_callback);
	DECLARE_WRITE8_MEMBER(system1_videoram_bank_w);
	void video_start_common(int pagecount);
	inline void videoram_wait_states(cpu_device *cpu);
	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect, int xoffset);
	void video_update_common(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, bitmap_ind16 &fgpixmap, bitmap_ind16 **bgpixmaps, const int *bgrowscroll, int bgyscroll, int spritexoffs);
	void bank44_custom_w(UINT8 data, UINT8 prevdata);
	void bank0c_custom_w(UINT8 data, UINT8 prevdata);
	void dakkochn_custom_w(UINT8 data, UINT8 prevdata);
	required_device<z80_device> m_maincpu;
	required_device<cpu_device> m_soundcpu;
	optional_device<cpu_device> m_mcu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;
	required_shared_ptr<UINT8> m_generic_paletteram_8;
};
