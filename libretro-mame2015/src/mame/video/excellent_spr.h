
class excellent_spr_device : public device_t,
						public device_video_interface
{
public:
	excellent_spr_device(const machine_config &mconfig, const char *tag, device_t *owner, UINT32 clock);

	void draw_sprites(screen_device &screen, gfxdecode_device *gfxdecode, const rectangle &cliprect);

	DECLARE_READ8_MEMBER(read);
	DECLARE_WRITE8_MEMBER(write);

	void aquarium_draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect, gfxdecode_device *gfxdecode, int y_offs);
	void gcpinbal_draw_sprites(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, gfxdecode_device *gfxdecode, int y_offs, int priority);

protected:
	UINT8* m_ram;

	virtual void device_start();
	virtual void device_reset();
private:
};

extern const device_type EXCELLENT_SPRITE;
