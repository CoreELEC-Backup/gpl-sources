/*********************************************************************

    video/apple2.h  - Video handling for 8-bit Apple IIs

*********************************************************************/

#ifndef __A2_VIDEO__
#define __A2_VIDEO__

#include "emu.h"

class a2_video_device :
	public device_t
{
public:
	// construction/destruction
	a2_video_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	DECLARE_PALETTE_INIT(apple2);

	bool m_page2;
	bool m_flash;
	bool m_mix;
	bool m_graphics;
	bool m_hires;
	bool m_dhires;
	bool m_80col;
	bool m_altcharset;
	UINT16 *m_hires_artifact_map;
	UINT16 *m_dhires_artifact_map;

	UINT8 *m_ram_ptr, *m_aux_ptr, *m_char_ptr;
	int m_char_size;

	int m_sysconfig;

	void text_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int beginrow, int endrow);
	void text_update_orig(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int beginrow, int endrow);
	void lores_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int beginrow, int endrow);
	void dlores_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int beginrow, int endrow);
	void hgr_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int beginrow, int endrow);
	void hgr_update_tk2000(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int beginrow, int endrow);
	void dhgr_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int beginrow, int endrow);

protected:
	virtual void device_reset();
	virtual void device_start();

private:
	void plot_text_character(bitmap_ind16 &bitmap, int xpos, int ypos, int xscale, UINT32 code, const UINT8 *textgfx_data, UINT32 textgfx_datalen, int fg, int bg);
	void plot_text_character_orig(bitmap_ind16 &bitmap, int xpos, int ypos, int xscale, UINT32 code, const UINT8 *textgfx_data, UINT32 textgfx_datalen, int fg, int bg);
};

// device type definition
extern const device_type APPLE2_VIDEO;

#endif
