#ifndef _VIDEOARCH_H
#define _VIDEOARCH_H

#include "vice.h"
#include "types.h"

extern int machine_ui_done;

struct video_canvas_s {
    unsigned int initialized;
    unsigned int created;
    unsigned int width, height;
    unsigned int *screen;

    struct video_render_config_s *videoconfig;
    struct draw_buffer_s *draw_buffer;
    struct viewport_s *viewport;
    struct geometry_s *geometry;
    struct palette_s *palette;
    struct video_resource_chip_s *video_resource_chip;

    unsigned int depth;

    struct video_draw_buffer_callback_s *video_draw_buffer_callback;
};
typedef struct video_canvas_s video_canvas_t;

#endif
