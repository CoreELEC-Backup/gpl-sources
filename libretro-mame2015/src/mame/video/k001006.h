#pragma once
#ifndef __K001006_H__
#define __K001006_H__



class k001006_device : public device_t
{
public:
	k001006_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);
	~k001006_device() {}

	// static configuration
	static void set_gfx_region(device_t &device, const char *tag) { downcast<k001006_device &>(device).m_gfx_region = tag; }
	static void set_tex_layout(device_t &device, int layout) { downcast<k001006_device &>(device).m_tex_layout = layout; }

	UINT32 fetch_texel(int page, int pal_index, int u, int v);
	void preprocess_texture_data(UINT8 *dst, UINT8 *src, int length, int gticlub);

	DECLARE_READ32_MEMBER( read );
	DECLARE_WRITE32_MEMBER( write );

protected:
	// device-level overrides
	virtual void device_config_complete();
	virtual void device_start();
	virtual void device_reset();

private:
	// internal state
	UINT16 *     m_pal_ram;
	UINT16 *     m_unknown_ram;
	UINT32       m_addr;
	int          m_device_sel;

	UINT8 *      m_texrom;

	UINT32 *     m_palette;

	const char * m_gfx_region;
	UINT8 *      m_gfxrom;
	//int m_tex_width;
	//int m_tex_height;
	//int m_tex_mirror_x;
	//int m_tex_mirror_y;
	int m_tex_layout;
};


extern const device_type K001006;


#define MCFG_K001006_GFX_REGION(_tag) \
	k001006_device::set_gfx_region(*device, _tag);

#define MCFG_K001006_TEX_LAYOUT(x) \
	k001006_device::set_tex_layout(*device, x);

#endif
