/***************************************************************************

  video.c

  Functions to emulate the video hardware of the machine.

***************************************************************************/

#include "emu.h"
#include "includes/8080bw.h"


#define NUM_PENS    (8)


MACHINE_START_MEMBER(_8080bw_state,extra_8080bw_vh)
{
	save_item(NAME(m_flip_screen));
	save_item(NAME(m_color_map));
	save_item(NAME(m_screen_red));

	// These two only belong to schaser, but for simplicity's sake let's waste
	// two bytes in other drivers' .sta files.
	save_item(NAME(m_schaser_background_disable));
	save_item(NAME(m_schaser_background_select));
}


PALETTE_INIT_MEMBER(_8080bw_state,rollingc)
{
	// palette is 3bpp + intensity
	for (int i = 0; i < 8; i++)
	{
		palette.set_pen_color(i, pal1bit(i >> 2) >> 1, pal1bit(i >> 1) >> 1, pal1bit(i >> 0) >> 1);
		palette.set_pen_color(i | 8, pal1bit(i >> 2), pal1bit(i >> 1), pal1bit(i >> 0));
	}

	// but according to photos, pen 6 is clearly orange instead of dark-yellow, and pen 5 is less dark as well
	// pens 1, 2 and 4 are good though. Maybe we're missing a color prom?
	palette.set_pen_color(0x05, 0xff, 0x00, 0x80); // pink
	palette.set_pen_color(0x06, 0xff, 0x80, 0x00); // orange
}


void _8080bw_state::invadpt2_get_pens( pen_t *pens )
{
	offs_t i;

	for (i = 0; i < NUM_PENS; i++)
	{
		pens[i] = rgb_t(pal1bit(i >> 0), pal1bit(i >> 2), pal1bit(i >> 1));
	}
}


void _8080bw_state::sflush_get_pens( pen_t *pens )
{
	offs_t i;

	pens[0] = rgb_t(0x80, 0x80, 0xff);

	for (i = 1; i < NUM_PENS; i++)
	{
		pens[i] = rgb_t(pal1bit(i >> 0), pal1bit(i >> 2), pal1bit(i >> 1));
	}
}


void _8080bw_state::cosmo_get_pens( pen_t *pens )
{
	offs_t i;

	for (i = 0; i < NUM_PENS; i++)
	{
		pens[i] = rgb_t(pal1bit(i >> 0), pal1bit(i >> 1), pal1bit(i >> 2));
	}
}


inline void _8080bw_state::set_pixel( bitmap_rgb32 &bitmap, UINT8 y, UINT8 x, const pen_t *pens, UINT8 color )
{
	if (y >= MW8080BW_VCOUNTER_START_NO_VBLANK)
	{
		if (m_flip_screen)
			bitmap.pix32(MW8080BW_VBSTART - 1 - (y - MW8080BW_VCOUNTER_START_NO_VBLANK), MW8080BW_HPIXCOUNT - 1 - x) = pens[color];
		else
			bitmap.pix32(y - MW8080BW_VCOUNTER_START_NO_VBLANK, x) = pens[color];
	}
}


inline void _8080bw_state::set_8_pixels( bitmap_rgb32 &bitmap, UINT8 y, UINT8 x, UINT8 data, const pen_t *pens, UINT8 fore_color, UINT8 back_color )
{
	int i;

	for (i = 0; i < 8; i++)
	{
		set_pixel(bitmap, y, x, pens, (data & 0x01) ? fore_color : back_color);

		x = x + 1;
		data = data >> 1;
	}
}


/* this is needed as this driver doesn't emulate the shift register like mw8080bw does */
void _8080bw_state::clear_extra_columns( bitmap_rgb32 &bitmap, const pen_t *pens, UINT8 color )
{
	UINT8 x;

	for (x = 0; x < 4; x++)
	{
		UINT8 y;

		for (y = MW8080BW_VCOUNTER_START_NO_VBLANK; y != 0; y++)
		{
			if (m_flip_screen)
				bitmap.pix32(MW8080BW_VBSTART - 1 - (y - MW8080BW_VCOUNTER_START_NO_VBLANK), MW8080BW_HPIXCOUNT - 1 - (256 + x)) = pens[color];
			else
				bitmap.pix32(y - MW8080BW_VCOUNTER_START_NO_VBLANK, 256 + x) = pens[color];
		}
	}
}


UINT32 _8080bw_state::screen_update_invadpt2(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	pen_t pens[NUM_PENS];
	offs_t offs;
	UINT8 *prom;
	UINT8 *color_map_base;

	invadpt2_get_pens(pens);

	prom = memregion("proms")->base();
	color_map_base = m_color_map ? &prom[0x0400] : &prom[0x0000];

	for (offs = 0; offs < m_main_ram.bytes(); offs++)
	{
		UINT8 y = offs >> 5;
		UINT8 x = offs << 3;

		offs_t color_address = (offs >> 8 << 5) | (offs & 0x1f);

		UINT8 data = m_main_ram[offs];
		UINT8 fore_color = m_screen_red ? 1 : color_map_base[color_address] & 0x07;

		set_8_pixels(bitmap, y, x, data, pens, fore_color, 0);
	}

	clear_extra_columns(bitmap, pens, 0);

	return 0;
}


UINT32 _8080bw_state::screen_update_ballbomb(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	pen_t pens[NUM_PENS];
	offs_t offs;
	UINT8 *color_map_base;
	UINT8 *prom;

	invadpt2_get_pens(pens);

	prom = memregion("proms")->base();
	color_map_base = m_color_map ? &prom[0x0400] : &prom[0x0000];

	for (offs = 0; offs < m_main_ram.bytes(); offs++)
	{
		UINT8 y = offs >> 5;
		UINT8 x = offs << 3;

		offs_t color_address = (offs >> 8 << 5) | (offs & 0x1f);

		UINT8 data = m_main_ram[offs];
		UINT8 fore_color = m_screen_red ? 1 : color_map_base[color_address] & 0x07;

		/* blue background */
		set_8_pixels(bitmap, y, x, data, pens, fore_color, 2);
	}

	clear_extra_columns(bitmap, pens, 2);

	return 0;
}


UINT32 _8080bw_state::screen_update_schaser(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	pen_t pens[NUM_PENS];
	offs_t offs;
	UINT8 *background_map_base;

	invadpt2_get_pens(pens);

	background_map_base = memregion("proms")->base();

	for (offs = 0; offs < m_main_ram.bytes(); offs++)
	{
		UINT8 back_color = 0;

		UINT8 y = offs >> 5;
		UINT8 x = offs << 3;

		UINT8 data = m_main_ram[offs];
		UINT8 fore_color = m_colorram[offs & 0x1f9f] & 0x07;

		if (!m_schaser_background_disable)
		{
			offs_t back_address = (offs >> 8 << 5) | (offs & 0x1f);

			UINT8 back_data = background_map_base[back_address];

			/* the equations derived from the schematics don't appear to produce
			   the right colors, but this one does, at least for this PROM */
			back_color = (((back_data & 0x0c) == 0x0c) && m_schaser_background_select) ? 4 : 2;
		}

		set_8_pixels(bitmap, y, x, data, pens, fore_color, back_color);
	}

	clear_extra_columns(bitmap, pens, m_schaser_background_disable ? 0 : 2);

	return 0;
}


UINT32 _8080bw_state::screen_update_schasercv(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	pen_t pens[NUM_PENS];
	offs_t offs;

	invadpt2_get_pens(pens);

	for (offs = 0; offs < m_main_ram.bytes(); offs++)
	{
		UINT8 y = offs >> 5;
		UINT8 x = offs << 3;

		UINT8 data = m_main_ram[offs];
		UINT8 fore_color = m_colorram[offs & 0x1f9f] & 0x07;

		/* blue background */
		set_8_pixels(bitmap, y, x, data, pens, fore_color, 2);
	}

	clear_extra_columns(bitmap, pens, 2);

	return 0;
}


UINT32 _8080bw_state::screen_update_rollingc(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	offs_t offs;

	for (offs = 0; offs < m_main_ram.bytes(); offs++)
	{
		UINT8 y = offs >> 5;
		UINT8 x = offs << 3;

		UINT8 data = m_main_ram[offs];
		UINT8 fore_color = m_colorram[offs & 0x1f1f] & 0x0f;
		UINT8 back_color = m_colorram2[offs & 0x1f1f] & 0x0f;

		set_8_pixels(bitmap, y, x, data, m_palette->pens(), fore_color, back_color);
	}

	clear_extra_columns(bitmap, m_palette->pens(), 0);

	return 0;
}


UINT32 _8080bw_state::screen_update_polaris(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	pen_t pens[NUM_PENS];
	offs_t offs;
	UINT8 *color_map_base;
	UINT8 *cloud_gfx;

	invadpt2_get_pens(pens);

	color_map_base = memregion("proms")->base();
	cloud_gfx = memregion("user1")->base();

	for (offs = 0; offs < m_main_ram.bytes(); offs++)
	{
		UINT8 y = offs >> 5;
		UINT8 x = offs << 3;

		UINT8 data = m_main_ram[offs];

		offs_t color_address = (offs >> 8 << 5) | (offs & 0x1f);

		/* for the background color, bit 0 of the map PROM is connected to green gun.
		   red is 0 and blue is 1, giving cyan and blue for the background.  This
		   is different from what the schematics shows, but it's supported
		   by screenshots.  Bit 3 is connected to cloud enable, while
		   bits 1 and 2 are marked 'not use' (sic) */

		UINT8 back_color = (color_map_base[color_address] & 0x01) ? 6 : 2;
		UINT8 fore_color = ~m_colorram[offs & 0x1f9f] & 0x07;

		UINT8 cloud_y = y - m_polaris_cloud_pos;

		if ((color_map_base[color_address] & 0x08) || (cloud_y >= 64))
		{
			set_8_pixels(bitmap, y, x, data, pens, fore_color, back_color);
		}
		else
		{
			/* cloud appears in this part of the screen */
			int i;

			for (i = 0; i < 8; i++)
			{
				UINT8 color;

				if (data & 0x01)
				{
					color = fore_color;
				}
				else
				{
					int bit = 1 << (~x & 0x03);
					offs_t cloud_gfx_offs = ((x >> 2) & 0x03) | ((~cloud_y & 0x3f) << 2);

					color = (cloud_gfx[cloud_gfx_offs] & bit) ? 7 : back_color;
				}

				set_pixel(bitmap, y, x, pens, color);

				x = x + 1;
				data = data >> 1;
			}
		}
	}

	clear_extra_columns(bitmap, pens, 6);

	return 0;
}


UINT32 _8080bw_state::screen_update_lupin3(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	pen_t pens[NUM_PENS];
	offs_t offs;

	invadpt2_get_pens(pens);

	for (offs = 0; offs < m_main_ram.bytes(); offs++)
	{
		UINT8 y = offs >> 5;
		UINT8 x = offs << 3;

		UINT8 data = m_main_ram[offs];
		UINT8 fore_color = ~m_colorram[offs & 0x1f9f] & 0x07;

		set_8_pixels(bitmap, y, x, data, pens, fore_color, 0);
	}

	clear_extra_columns(bitmap, pens, 0);

	return 0;
}


UINT32 _8080bw_state::screen_update_cosmo(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	pen_t pens[NUM_PENS];
	offs_t offs;

	cosmo_get_pens(pens);

	for (offs = 0; offs < m_main_ram.bytes(); offs++)
	{
		UINT8 y = offs >> 5;
		UINT8 x = offs << 3;

		offs_t color_address = (offs >> 8 << 5) | (offs & 0x1f);

		UINT8 data = m_main_ram[offs];
		UINT8 fore_color = m_colorram[color_address] & 0x07;

		set_8_pixels(bitmap, y, x, data, pens, fore_color, 0);
	}

	clear_extra_columns(bitmap, pens, 0);

	return 0;
}


UINT32 _8080bw_state::screen_update_indianbt(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	pen_t pens[NUM_PENS];
	offs_t offs;
	UINT8 *color_map_base;
	UINT8 *prom;

	cosmo_get_pens(pens);

	prom = memregion("proms")->base();
	color_map_base = m_color_map ? &prom[0x0400] : &prom[0x0000];

	for (offs = 0; offs < m_main_ram.bytes(); offs++)
	{
		UINT8 y = offs >> 5;
		UINT8 x = offs << 3;

		offs_t color_address = (offs >> 8 << 5) | (offs & 0x1f);

		UINT8 data = m_main_ram[offs];
		UINT8 fore_color = color_map_base[color_address] & 0x07;

		set_8_pixels(bitmap, y, x, data, pens, fore_color, 0);
	}

	clear_extra_columns(bitmap, pens, 0);

	return 0;
}


UINT32 _8080bw_state::screen_update_sflush(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	pen_t pens[NUM_PENS];
	offs_t offs;

	sflush_get_pens(pens);

	for (offs = 0; offs < m_main_ram.bytes(); offs++)
	{
		UINT8 y = offs >> 5;
		UINT8 x = offs << 3;

		UINT8 data = m_main_ram[offs];
		UINT8 fore_color = m_colorram[offs & 0x1f9f] & 0x07;

		set_8_pixels(bitmap, y, x, data, pens, fore_color, 0);
	}

	clear_extra_columns(bitmap, pens, 0);

	return 0;
}


UINT32 _8080bw_state::screen_update_shuttlei(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	pen_t pens[2] = { rgb_t::black, rgb_t::white };
	offs_t offs;

	for (offs = 0; offs < m_main_ram.bytes(); offs++)
	{
		int i;

		UINT8 y = offs >> 5;
		UINT8 x = offs << 3;

		UINT8 data = m_main_ram[offs];

		for (i = 0; i < 8; i++)
		{
			if (m_flip_screen)
				bitmap.pix32(191-y, 255-(x|i)) = pens[BIT(data, 7)];
			else
				bitmap.pix32(y, x|i) = pens[BIT(data, 7)];
			data <<= 1;
		}
	}

	return 0;
}


UINT32 _8080bw_state::screen_update_spacecom(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	pen_t pens[2] = { rgb_t::black, rgb_t::white };
	offs_t offs;

	for (offs = 0; offs < 0x1c00; offs++)
	{
		int i;

		UINT8 y = offs >> 5;
		UINT8 x = offs << 3;
		UINT8 flipx = m_flip_screen ? 7 : 0;

		UINT8 data = m_main_ram[offs+0x400];

		for (i = 0; i < 8; i++)
		{
			bitmap.pix32(y, x | (i^flipx)) = pens[data & 1];
			data >>= 1;
		}
	}

	return 0;
}
