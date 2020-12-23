/***************************************************************************

    ui/videoopt.h

    Internal menus for video options

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

***************************************************************************/

#pragma once

#ifndef __UI_VIDEOOPT_H__
#define __UI_VIDEOOPT_H__


class ui_menu_video_targets : public ui_menu {
public:
	ui_menu_video_targets(running_machine &machine, render_container *container);
	virtual ~ui_menu_video_targets();
	virtual void populate();
	virtual void handle();
};

class ui_menu_video_options : public ui_menu {
public:
	ui_menu_video_options(running_machine &machine, render_container *container, render_target *target);
	virtual ~ui_menu_video_options();
	virtual void populate();
	virtual void handle();

private:
	enum {
		VIDEO_ITEM_ROTATE = 0x80000000,
		VIDEO_ITEM_BACKDROPS,
		VIDEO_ITEM_OVERLAYS,
		VIDEO_ITEM_BEZELS,
		VIDEO_ITEM_CPANELS,
		VIDEO_ITEM_MARQUEES,
		VIDEO_ITEM_ZOOM,
		VIDEO_ITEM_VIEW
	};

	render_target *target;
};


#endif  /* __UI_VIDEOOPT_H__ */
