/***************************************************************************

	Rohga Video emulation - Bryan McPhail, mish@tendril.co.uk

***************************************************************************/

#include "driver.h"
#include "vidhrdw/generic.h"
#include "deco16ic.h"

/******************************************************************************/

static int wizdfire_bank_callback(const int bank)
{
	return ((bank>>4)&0x3)<<12;
}

VIDEO_START( rohga )
{
	if (deco16_2_video_init(0))
		return 1;

	deco16_set_tilemap_bank_callback(0,wizdfire_bank_callback);
	deco16_set_tilemap_bank_callback(1,wizdfire_bank_callback);
	deco16_set_tilemap_bank_callback(2,wizdfire_bank_callback);
	deco16_set_tilemap_bank_callback(3,wizdfire_bank_callback);

	return 0;
}

VIDEO_START( wizdfire )
{
	if (deco16_2_video_init(0))
		return 1;

	deco16_set_tilemap_bank_callback(0,wizdfire_bank_callback);
	deco16_set_tilemap_bank_callback(1,wizdfire_bank_callback);
	deco16_set_tilemap_bank_callback(2,wizdfire_bank_callback);
	deco16_set_tilemap_bank_callback(3,wizdfire_bank_callback);

	deco16_pf1_rowscroll=deco16_pf2_rowscroll=0;

	alpha_set_level(0x80);

	return 0;
}

VIDEO_START( nitrobal )
{
	if (deco16_2_video_init(0))
		return 1;

	deco16_set_tilemap_bank_callback(0,wizdfire_bank_callback);
	deco16_set_tilemap_bank_callback(1,wizdfire_bank_callback);
	deco16_set_tilemap_bank_callback(2,wizdfire_bank_callback);
	deco16_set_tilemap_bank_callback(3,wizdfire_bank_callback);

	deco16_set_tilemap_colour_base(2,0);
	deco16_set_tilemap_colour_mask(2,0);
	deco16_set_tilemap_colour_base(3,0);
	deco16_set_tilemap_colour_mask(3,0);

	alpha_set_level(0x80);

	return 0;
}

/******************************************************************************/

static void rohga_drawsprites(struct mame_bitmap *bitmap, const data16_t *spriteptr)
{
	int offs;

	for (offs = 0x400-4;offs >= 0;offs -= 4)
	{
		int x,y,sprite,colour,multi,fx,fy,inc,flash,mult,pri=0;
		sprite = spriteptr[offs+1];
		if (!sprite) continue;

		x = spriteptr[offs+2];

		/* Sprite/playfield priority */
		switch (x&0xc000) {
		case 0x0000: pri=0; break;
		case 0x4000: pri=0xf0; break;
		case 0x8000: pri=0xf0|0xcc; break;
		case 0xc000: pri=0xf0|0xcc; break; /* Perhaps 0xf0|0xcc|0xaa (Sprite under bottom layer) */
		}
//todo - test above..
		y = spriteptr[offs];
		flash=y&0x1000;
		if (flash && (cpu_getcurrentframe() & 1)) continue;
		colour = (x >> 9) &0xf;

		fx = y & 0x2000;
		fy = y & 0x4000;
		multi = (1 << ((y & 0x0600) >> 9)) - 1;	/* 1x, 2x, 4x, 8x height */

		x = x & 0x01ff;
		y = y & 0x01ff;
		if (x >= 320) x -= 512;
		if (y >= 256) y -= 512;

		sprite &= ~multi;
		if (fy)
			inc = -1;
		else
		{
			sprite += multi;
			inc = 1;
		}

		if (flip_screen) {
			x=304-x;
			y=240-y;
			if (fx) fx=0; else fx=1;
			if (fy) fy=0; else fy=1;
			mult=-16;
		}
		else mult=+16;

		while (multi >= 0)
		{
			pdrawgfx(bitmap,Machine->gfx[3],
					sprite - multi * inc,
					colour,
					fx,fy,
					x,y + mult * multi,
					&Machine->visible_area,TRANSPARENCY_PEN,0,pri);

			multi--;
		}
	}
}

static void wizdfire_drawsprites(struct mame_bitmap *bitmap, data16_t *spriteptr, int mode, int bank)
{
	int offs;

	for (offs = 0;offs < 0x400;offs += 4)
	{
		int x,y,sprite,colour,multi,fx,fy,inc,flash,mult;
		int trans=TRANSPARENCY_PEN;

		sprite = spriteptr[offs+1];
		if (!sprite) continue;

		x = spriteptr[offs+2];

		/*
		Sprite/playfield priority - we can't use pdrawgfx because we need alpha'd sprites overlaid
		over non-alpha'd sprites, plus sprites underneath and above an alpha'd background layer.

		Hence, we rely on the hardware sorting everything correctly and not relying on any orthoganality
		effects (it doesn't seem to), and instead draw seperate passes for each sprite priority.  :(
		*/
		switch (mode) {
		case 4:
			if ((x&0xc000)!=0xc000)
				continue;
			break;
		case 3:
			if ((x&0xc000)!=0x8000)
				continue;
			break;
		case 2:
			if ((x&0x8000)!=0x8000)
				continue;
			break;
		case 1:
		case 0:
		default:
			if ((x&0x8000)!=0)
				continue;
			break;
		}

		y = spriteptr[offs];
		flash=y&0x1000;
		if (flash && (cpu_getcurrentframe() & 1)) continue;
		colour = (x >> 9) &0x1f;

		if (bank==4 && colour&0x10) {
			trans=TRANSPARENCY_ALPHA;
			colour&=0xf;
		}

		fx = y & 0x2000;
		fy = y & 0x4000;
		multi = (1 << ((y & 0x0600) >> 9)) - 1;	/* 1x, 2x, 4x, 8x height */

		x = x & 0x01ff;
		y = y & 0x01ff;
		if (x >= 320) x -= 512;
		if (y >= 256) y -= 512;

		sprite &= ~multi;
		if (fy)
			inc = -1;
		else
		{
			sprite += multi;
			inc = 1;
		}

		if (flip_screen) {
			x=304-x;
			y=240-y;
			if (fx) fx=0; else fx=1;
			if (fy) fy=0; else fy=1;
			mult=-16;
		}
		else
			mult=+16;

		if (fx) fx=0; else fx=1;
		if (fy) fy=0; else fy=1;

		while (multi >= 0)
		{
			drawgfx(bitmap,Machine->gfx[bank],
					sprite - multi * inc,
					colour,
					fx,fy,
					x,y + mult * multi,
					&Machine->visible_area,trans,0);

			multi--;
		}
	}
}

static void nitrobal_drawsprites(struct mame_bitmap *bitmap, const struct rectangle *cliprect, const data16_t *spriteptr, int gfxbank)
{
	int offs,end,inc;

	/*
		Alternate format from most 16 bit games - same as Captain America and Mutant Fighter

		Word 0:
			0x8000:	Y flip
			0x4000: X flip
			0x2000:	Flash (Sprite toggles on/off every frame)
			0x1fff:	Y value
		Word 1:
			0xffff: X value
		Word 2:
			0xf000:	Block height
			0x0f00: Block width
			0x00e0: Unused?

sprites1:
	bit 0x40 set for under PF2
	bit 0x20 set on others...
	bit 0x80 set in game pf3 (split)

Sprites 2:
	bit 0x10 set on alpha sprites..
	bit 0x80 set is above pf2 else under pf2

			0x001f: Colour
		Word 3:
			0xffff:	Sprite value
	*/

	offs=0x3fc;
	end=-4;
	inc=-4;

	while (offs!=end)
	{
		int x,y,sprite,colour,fx,fy,w,h,sx,sy,x_mult,y_mult,tilemap_pri,sprite_pri;
		int trans=TRANSPARENCY_PEN;

		sprite = spriteptr[offs+3];
		if (!sprite) {
			offs+=inc;
			continue;
		}

		sx = spriteptr[offs+1];

		h = (spriteptr[offs+2]&0xf000)>>12;
		w = (spriteptr[offs+2]&0x0f00)>> 8;

		sy = spriteptr[offs];
		if ((sy&0x2000) && (cpu_getcurrentframe() & 1)) {
			offs+=inc;
			continue;
		}

		colour = (spriteptr[offs+2] >>0) & 0x1f;

		// PRIORITIES - TODO
		if (gfxbank==3) {
			/* Sprite chip 1 */
			switch (spriteptr[offs+2]&0xe0) {
//			case 0xc0: colour=rand()%0xff; tilemap_pri=256; break; //todo
			case 0xc0: tilemap_pri=8; break; //? under other sprites
			case 0x80: tilemap_pri=32; break; //? under other sprites
			case 0x20: tilemap_pri=32; break; /* Over pf2 and under other sprite chip */
			case 0x40: tilemap_pri=8; break; /* Under pf2 and under other sprite chip */
			case 0xa0: tilemap_pri=32; break;
			case 0:
				tilemap_pri=128; break;
			default:
				tilemap_pri=128; 
				break; 
			}

/*
Intro:
	0x40 is under pf2 and other sprites
	0x20 is under pf2

Level 1
	0xa0 means under other sprites and pf2?
  
Level 2 (tank scene)

	Chip 1:

  0x20 set means under pf2 else above??
  0x80 set means under other sprites else above

Level 3:
	0xc0 means under other sprites and pf2		check??
	0x40 means under pf2
	0xa0 means under pf2..  

	always over other sprites..?


PRI MODE 2:  (Level 4)
sprite 1:
	mode 0xa0 is under pf2 (sprites unknown)
	mode 0x40 is under pf2 (sprites unknown)

sprite 2:

	mode 0x40 is under pf2
	mode 0 is under pf2 


Level 5 (Space, pri mode 1)

sprite 1:
	mode 0x80 is over pf2 and over other sprites



sprite 2:

	mode 0 is over pf2

  */

			sprite_pri=1;
		} else {
			/* Sprite chip 2 (with alpha blending) */

			/* Sprite above playfield 2, but still below other sprite chip */
//			if (spriteptr[offs+2]&0x80)
				tilemap_pri=64; 
//			else
//				tilemap_pri=8; 

			if (deco16_priority)
				tilemap_pri=8;
			else
				tilemap_pri=64;

			sprite_pri=2;
		}

		if (gfxbank==4 && colour&0x10) {
			trans=TRANSPARENCY_ALPHA;
			colour&=0xf;
		}

		fx = (spriteptr[offs+0]&0x4000);
		fy = (spriteptr[offs+0]&0x8000);

		if (!flip_screen) { /* Inverted from Mutant Fighter! */
			if (fx) fx=0; else fx=1;
			if (fy) fy=0; else fy=1;

			sx = sx & 0x01ff;
			sy = sy & 0x01ff;
			if (sx>0x180) sx=-(0x200 - sx);
			if (sy>0x180) sy=-(0x200 - sy);

			if (fx) { x_mult=-16; sx+=16*w; } else { x_mult=16; sx-=16; }
			if (fy) { y_mult=-16; sy+=16*h; } else { y_mult=16; sy-=16; }
		} else {
			sx = sx & 0x01ff;
			sy = sy & 0x01ff;
			if (sx&0x100) sx=-(0x100 - (sx&0xff));
			if (sy&0x100) sy=-(0x100 - (sy&0xff));
			sx = 304 - sx;
			sy = 240 - sy;
			if (sx >= 432) sx -= 512;
			if (sy >= 384) sy -= 512;
			if (fx) { x_mult=-16; sx+=16; } else { x_mult=16; sx-=16*w; }
			if (fy) { y_mult=-16; sy+=16; } else { y_mult=16; sy-=16*h; }
		}

		for (x=0; x<w; x++) {
			for (y=0; y<h; y++) {
				deco16_pdrawgfx(bitmap,Machine->gfx[gfxbank],
						sprite + y + h * x,
						colour,
						fx,fy,
						sx + x_mult * (w-x),sy + y_mult * (h-y),
						&Machine->visible_area,trans,0,tilemap_pri,sprite_pri,0);
			}
		}

		offs+=inc;
	}
}

/******************************************************************************/

VIDEO_UPDATE( rohga )
{
	/* Update playfields */
//	flip_screen_set( deco16_pf12_control[0]&0x80 );
	deco16_pf12_update(deco16_pf1_rowscroll,deco16_pf2_rowscroll);
	deco16_pf34_update(deco16_pf3_rowscroll,deco16_pf4_rowscroll);

	/* Draw playfields */
	fillbitmap(priority_bitmap,0,cliprect);
	fillbitmap(bitmap,Machine->pens[512],cliprect);

	if (!keyboard_pressed(KEYCODE_Z))
	deco16_tilemap_4_draw(bitmap,cliprect,TILEMAP_IGNORE_TRANSPARENCY,1);
	if (!keyboard_pressed(KEYCODE_X))
	deco16_tilemap_3_draw(bitmap,cliprect,0,2);
	if (!keyboard_pressed(KEYCODE_C))
	deco16_tilemap_2_draw(bitmap,cliprect,0,4);

	rohga_drawsprites(bitmap,spriteram16);
	deco16_tilemap_1_draw(bitmap,cliprect,0,0);

//	deco16_print_debug_info();
}

VIDEO_UPDATE( wizdfire )
{
	/* Update playfields */
	flip_screen_set( deco16_pf12_control[0]&0x80 );
	deco16_pf12_update(deco16_pf1_rowscroll,deco16_pf2_rowscroll);
	deco16_pf34_update(deco16_pf3_rowscroll,deco16_pf4_rowscroll);

	/* Draw playfields - Palette of 2nd playfield chip visible if playfields turned off */
	fillbitmap(bitmap,Machine->pens[512],&Machine->visible_area);

	deco16_tilemap_4_draw(bitmap,cliprect,TILEMAP_IGNORE_TRANSPARENCY,0);
	wizdfire_drawsprites(bitmap,buffered_spriteram16,4,3);
	deco16_tilemap_2_draw(bitmap,cliprect,0,0);
	wizdfire_drawsprites(bitmap,buffered_spriteram16,3,3);

	if ((deco16_priority&0x1f)==0x1f) /* Wizdfire has bit 0x40 always set, Dark Seal 2 doesn't?! */
		deco16_tilemap_3_draw(bitmap,cliprect,TILEMAP_ALPHA,0);
	else
		deco16_tilemap_3_draw(bitmap,cliprect,0,0);

	/* See notes in wizdfire_drawsprites about this */
	wizdfire_drawsprites(bitmap,buffered_spriteram16,0,3);
	wizdfire_drawsprites(bitmap,buffered_spriteram16_2,2,4);
	wizdfire_drawsprites(bitmap,buffered_spriteram16_2,1,4);

	deco16_tilemap_1_draw(bitmap,cliprect,0,0);
}

VIDEO_UPDATE( nitrobal )
{
	/* Update playfields */
	flip_screen_set( deco16_pf12_control[0]&0x80 );
	deco16_pf12_update(deco16_pf1_rowscroll,deco16_pf2_rowscroll);
	deco16_pf34_update(deco16_pf3_rowscroll,deco16_pf4_rowscroll);

	/* Draw playfields - Palette of 2nd playfield chip visible if playfields turned off */
	fillbitmap(bitmap,Machine->pens[512],&Machine->visible_area);
	fillbitmap(priority_bitmap,0,NULL);
	deco16_clear_sprite_priority_bitmap(); 
	deco16_tilemap_4_draw(bitmap,cliprect,TILEMAP_IGNORE_TRANSPARENCY,0);

	/* We don't draw pf3 because it's always the same data as pf4 - the 4bpp outputs
	are combined into a single 8bpp bitmap.  We can precompute the 8bpp tiles to avoid
	combining them at runtime (see also Robocop 2) */

	deco16_tilemap_2_draw(bitmap,cliprect,0,16);
	nitrobal_drawsprites(bitmap,cliprect,buffered_spriteram16,3);
	nitrobal_drawsprites(bitmap,cliprect,buffered_spriteram16_2,4);

	deco16_tilemap_1_draw(bitmap,cliprect,0,0);
}
