/*  Copyright 2006 Guillaume Duhamel
    Copyright 2005-2006 Fabien Coulon

    This file is part of Yabause.

    Yabause is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Yabause is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Yabause; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
*/

#ifndef YUI_VDP2_H
#define YUI_VDP2_H

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtktable.h>

#include "yuiwindow.h"

G_BEGIN_DECLS

#define YUI_VDP2_TYPE            (yui_vdp2_get_type ())
#define YUI_VDP2(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), YUI_VDP2_TYPE, YuiVdp2))
#define YUI_VDP2_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  YUI_VDP2_TYPE, YuiVdp2Class))
#define IS_YUI_VDP2(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), YUI_VDP2_TYPE))
#define IS_YUI_VDP2_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  YUI_VDP2_TYPE))

typedef struct _YuiVdp2       YuiVdp2;
typedef struct _YuiVdp2Class  YuiVdp2Class;

struct _YuiVdp2
{
  GtkWindow dialog;

  GtkWidget * image;
  GtkTextBuffer * buffer;
  GtkWidget * toolbar;

  GtkListStore * store;

  gint cursor;

  gulong paused_handler;
  gulong running_handler;

  YuiWindow * yui;
};

struct _YuiVdp2Class
{
  GtkWindowClass parent_class;
};

GType		yui_vdp2_get_type       (void);
GtkWidget *	yui_vdp2_new            (YuiWindow * yui);
void		yui_vdp2_fill		(YuiVdp2 * vdp2);
void		yui_vdp2_update		(YuiVdp2 * vdp2);
void		yui_vdp2_destroy	(YuiVdp2 * vdp2);

G_END_DECLS

#endif
