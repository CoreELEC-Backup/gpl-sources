/***************************************************************************

    ui/filesel.c

    MESS's clunky built-in file manager

    Copyright Nicola Salmoria and the MAME Team.
    Visit http://mamedev.org for licensing and usage restrictions.

    TODO
        - Support image creation arguments
        - Restrict empty slot if image required

***************************************************************************/

#include "emu.h"
#include "ui/ui.h"
#include "zippath.h"
#include "ui/menu.h"
#include "ui/filesel.h"
#include "ui/swlist.h"
#include "imagedev/floppy.h"


/***************************************************************************
    CONSTANTS
***************************************************************************/

// conditional compilation to enable chosing of image formats - this is not
// yet fully implemented
#define ENABLE_FORMATS          0

// time (in seconds) to display errors
#define ERROR_MESSAGE_TIME      5

// itemrefs for key menu items
#define ITEMREF_NEW_IMAGE_NAME  ((void *) 0x0001)
#define ITEMREF_CREATE          ((void *) 0x0002)
#define ITEMREF_FORMAT          ((void *) 0x0003)
#define ITEMREF_NO              ((void *) 0x0004)
#define ITEMREF_YES             ((void *) 0x0005)


/***************************************************************************
    MENU HELPERS
***************************************************************************/

//-------------------------------------------------
//  input_character - inputs a typed character
//  into a buffer
//-------------------------------------------------

static void input_character(char *buffer, size_t buffer_length, unicode_char unichar, int (*filter)(unicode_char))
{
	size_t buflen = strlen(buffer);

	if ((unichar == 8 || unichar == 0x7f) && (buflen > 0))
	{
		*(char *)utf8_previous_char(&buffer[buflen]) = 0;
	}
	else if ((unichar > ' ') && ((filter == NULL) || (*filter)(unichar)))
	{
		buflen += utf8_from_uchar(&buffer[buflen], buffer_length - buflen, unichar);
		buffer[buflen] = 0;
	}
}

//-------------------------------------------------
//  extra_text_draw_box - generically adds header
//  or footer text
//-------------------------------------------------

static void extra_text_draw_box(render_container *container, float origx1, float origx2, float origy, float yspan, const char *text, int direction)
{
	float text_width, text_height;
	float width, maxwidth;
	float x1, y1, x2, y2, temp;

	// get the size of the text
	container->manager().machine().ui().draw_text_full(container,text, 0.0f, 0.0f, 1.0f, JUSTIFY_LEFT, WRAP_WORD,
		DRAW_NONE, ARGB_WHITE, ARGB_BLACK, &text_width, &text_height);
	width = text_width + (2 * UI_BOX_LR_BORDER);
	maxwidth = MAX(width, origx2 - origx1);

	// compute our bounds
	x1 = 0.5f - 0.5f * maxwidth;
	x2 = x1 + maxwidth;
	y1 = origy + (yspan * direction);
	y2 = origy + (UI_BOX_TB_BORDER * direction);

	if (y1 > y2)
	{
		temp = y1;
		y1 = y2;
		y2 = temp;
	}

	// draw a box
	container->manager().machine().ui().draw_outlined_box(container,x1, y1, x2, y2, UI_BACKGROUND_COLOR);

	// take off the borders
	x1 += UI_BOX_LR_BORDER;
	x2 -= UI_BOX_LR_BORDER;
	y1 += UI_BOX_TB_BORDER;
	y2 -= UI_BOX_TB_BORDER;

	// draw the text within it
	container->manager().machine().ui().draw_text_full(container,text, x1, y1, text_width, JUSTIFY_LEFT, WRAP_WORD,
						DRAW_NORMAL, ARGB_WHITE, ARGB_BLACK, NULL, NULL);
}


//-------------------------------------------------
//  extra_text_render - generically adds header
//  and footer text
//-------------------------------------------------

void extra_text_render(render_container *container, float top, float bottom,
	float origx1, float origy1, float origx2, float origy2,
	const char *header, const char *footer)
{
	header = ((header != NULL) && (header[0] != '\0')) ? header : NULL;
	footer = ((footer != NULL) && (footer[0] != '\0')) ? footer : NULL;

	if (header != NULL)
		extra_text_draw_box(container, origx1, origx2, origy1, top, header, -1);
	if (footer != NULL)
		extra_text_draw_box(container, origx1, origx2, origy2, bottom, footer, +1);
}


/***************************************************************************
    CONFIRM SAVE AS MENU
***************************************************************************/

//-------------------------------------------------
//  ctor
//-------------------------------------------------

ui_menu_confirm_save_as::ui_menu_confirm_save_as(running_machine &machine, render_container *container, bool *yes)
	: ui_menu(machine, container)
{
	m_yes = yes;
	*m_yes = false;
}


//-------------------------------------------------
//  dtor
//-------------------------------------------------

ui_menu_confirm_save_as::~ui_menu_confirm_save_as()
{
}


//-------------------------------------------------
//  populate
//-------------------------------------------------

void ui_menu_confirm_save_as::populate()
{
	item_append("File Already Exists - Override?", NULL, MENU_FLAG_DISABLE, NULL);
	item_append(MENU_SEPARATOR_ITEM, NULL, MENU_FLAG_DISABLE, NULL);
	item_append("No", NULL, 0, ITEMREF_NO);
	item_append("Yes", NULL, 0, ITEMREF_YES);
}

//-------------------------------------------------
//  handle - confirm save as menu
//-------------------------------------------------

void ui_menu_confirm_save_as::handle()
{
	// process the menu
	const ui_menu_event *event = process(0);

	// process the event
	if ((event != NULL) && (event->iptkey == IPT_UI_SELECT))
	{
		if (event->itemref == ITEMREF_YES)
			*m_yes = true;

		// no matter what, pop out
		ui_menu::stack_pop(machine());
	}
}



/***************************************************************************
    FILE CREATE MENU
***************************************************************************/

//-------------------------------------------------
//  is_valid_filename_char - tests to see if a
//  character is valid in a filename
//-------------------------------------------------

static int is_valid_filename_char(unicode_char unichar)
{
	// this should really be in the OSD layer
	static const char valid_filename_char[] =
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     // 00-0f
		0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,     // 10-1f
		1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0,     //  !"#$%&'()*+,-./
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,     // 0123456789:;<=>?
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,     // @ABCDEFGHIJKLMNO
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1,     // PQRSTUVWXYZ[\]^_
		0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,     // `abcdefghijklmno
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 0,     // pqrstuvwxyz{|}~
	};
	return (unichar < ARRAY_LENGTH(valid_filename_char)) && valid_filename_char[unichar];
}


//-------------------------------------------------
//  ctor
//-------------------------------------------------

ui_menu_file_create::ui_menu_file_create(running_machine &machine, render_container *container, device_image_interface *image, astring &current_directory, astring &current_file, bool *ok)
	: ui_menu(machine, container),
		m_current_directory(current_directory),
		m_current_file(current_file)
{
	m_image = image;
	m_ok = ok;
	*m_ok = true;
}


//-------------------------------------------------
//  dtor
//-------------------------------------------------

ui_menu_file_create::~ui_menu_file_create()
{
}


//-------------------------------------------------
//  custom_render - perform our special rendering
//-------------------------------------------------

void ui_menu_file_create::custom_render(void *selectedref, float top, float bottom, float origx1, float origy1, float origx2, float origy2)
{
	extra_text_render(container, top, bottom, origx1, origy1, origx2, origy2,
		m_current_directory,
		NULL);
}


//-------------------------------------------------
//  populate - populates the file creator menu
//-------------------------------------------------

void ui_menu_file_create::populate()
{
	astring buffer;
	const image_device_format *format;
	const char *new_image_name;

	// append the "New Image Name" item
	if (get_selection() == ITEMREF_NEW_IMAGE_NAME)
	{
		buffer.cat(m_filename_buffer).cat("_");
		new_image_name = buffer;
	}
	else
	{
		new_image_name = m_filename_buffer;
	}
	item_append("New Image Name:", new_image_name, 0, ITEMREF_NEW_IMAGE_NAME);

	// do we support multiple formats?
	format = m_image->formatlist();
	if (ENABLE_FORMATS && (format != NULL))
	{
		item_append("Image Format:", m_current_format->description(), 0, ITEMREF_FORMAT);
		m_current_format = format;
	}

	// finish up the menu
	item_append(MENU_SEPARATOR_ITEM, NULL, 0, NULL);
	item_append("Create", NULL, 0, ITEMREF_CREATE);

	customtop = machine().ui().get_line_height() + 3.0f * UI_BOX_TB_BORDER;
}


//-------------------------------------------------
//  handle - file creator menu
//-------------------------------------------------

void ui_menu_file_create::handle()
{
	// process the menu
	const ui_menu_event *event = process(0);

	// process the event
	if (event != NULL)
	{
		// handle selections
		switch(event->iptkey)
		{
			case IPT_UI_SELECT:
				if ((event->itemref == ITEMREF_CREATE) || (event->itemref == ITEMREF_NEW_IMAGE_NAME))
				{
					astring tmp_file(m_filename_buffer);
					if (tmp_file.find(".") != -1 && tmp_file.find(".") < tmp_file.len() - 1)
					{
						m_current_file.cpy(m_filename_buffer);
						ui_menu::stack_pop(machine());
					}
					else
						machine().ui().popup_time(1, "Please enter a file extension too");
				}
				break;

			case IPT_SPECIAL:
				if (get_selection() == ITEMREF_NEW_IMAGE_NAME)
				{
					input_character(
						m_filename_buffer,
						ARRAY_LENGTH(m_filename_buffer),
						event->unichar,
						is_valid_filename_char);
					reset(UI_MENU_RESET_REMEMBER_POSITION);
				}
				break;
			case IPT_UI_CANCEL:
				*m_ok = false;
				break;
		}
	}
}

/***************************************************************************
    FILE SELECTOR MENU
***************************************************************************/

//-------------------------------------------------
//  ctor
//-------------------------------------------------

ui_menu_file_selector::ui_menu_file_selector(running_machine &machine, render_container *container, device_image_interface *image, astring &current_directory, astring &current_file, bool has_empty, bool has_softlist, bool has_create, int *result)
	: ui_menu(machine, container),
		m_current_directory(current_directory),
		m_current_file(current_file)
{
	m_image = image;
	m_has_empty = has_empty;
	m_has_softlist = has_softlist;
	m_has_create = has_create;
	m_result = result;
}


//-------------------------------------------------
//  dtor
//-------------------------------------------------

ui_menu_file_selector::~ui_menu_file_selector()
{
}


//-------------------------------------------------
//  custom_render - perform our special rendering
//-------------------------------------------------

void ui_menu_file_selector::custom_render(void *selectedref, float top, float bottom, float origx1, float origy1, float origx2, float origy2)
{
	extra_text_render(container, top, bottom,
		origx1, origy1, origx2, origy2,
		m_current_directory,
		NULL);
}



//-------------------------------------------------
//  compare_file_selector_entries - sorting proc
//  for file selector entries
//-------------------------------------------------

int ui_menu_file_selector::compare_entries(const file_selector_entry *e1, const file_selector_entry *e2)
{
	int result;
	const char *e1_basename = (e1->basename != NULL) ? e1->basename : "";
	const char *e2_basename = (e2->basename != NULL) ? e2->basename : "";

	if (e1->type < e2->type)
	{
		result = -1;
	}
	else if (e1->type > e2->type)
	{
		result = 1;
	}
	else
	{
		result = core_stricmp(e1_basename, e2_basename);
		if (result == 0)
		{
			result = strcmp(e1_basename, e2_basename);
			if (result == 0)
			{
				if (e1 < e2)
					result = -1;
				else if (e1 > e2)
					result = 1;
			}
		}
	}

	return result;
}


//-------------------------------------------------
//  append_entry - appends a new
//  file selector entry to an entry list
//-------------------------------------------------

ui_menu_file_selector::file_selector_entry *ui_menu_file_selector::append_entry(
	file_selector_entry_type entry_type, const char *entry_basename, const char *entry_fullpath)
{
	file_selector_entry *entry;
	file_selector_entry **entryptr;

	// allocate a new entry
	entry = (file_selector_entry *) m_pool_alloc(sizeof(*entry));
	memset(entry, 0, sizeof(*entry));
	entry->type = entry_type;
	entry->basename = (entry_basename != NULL) ? pool_strdup(entry_basename) : entry_basename;
	entry->fullpath = (entry_fullpath != NULL) ? pool_strdup(entry_fullpath) : entry_fullpath;

	// find the end of the list
	entryptr = &m_entrylist;
	while ((*entryptr != NULL) && (compare_entries(entry, *entryptr) >= 0))
		entryptr = &(*entryptr)->next;

	// insert the entry
	entry->next = *entryptr;
	*entryptr = entry;
	return entry;
}


//-------------------------------------------------
//  append_entry_menu_item - appends
//  a menu item for a file selector entry
//-------------------------------------------------

ui_menu_file_selector::file_selector_entry *ui_menu_file_selector::append_dirent_entry(const osd_directory_entry *dirent)
{
	astring buffer;
	file_selector_entry_type entry_type;
	file_selector_entry *entry;

	switch(dirent->type)
	{
		case ENTTYPE_FILE:
			entry_type = SELECTOR_ENTRY_TYPE_FILE;
			break;

		case ENTTYPE_DIR:
			entry_type = SELECTOR_ENTRY_TYPE_DIRECTORY;
			break;

		default:
			// exceptional case; do not add a menu item
			return NULL;
	}

	// determine the full path
	zippath_combine(buffer, m_current_directory, dirent->name);

	// create the file selector entry
	entry = append_entry(
		entry_type,
		dirent->name,
		buffer);

	return entry;
}


//-------------------------------------------------
//  append_entry_menu_item - appends
//  a menu item for a file selector entry
//-------------------------------------------------

void ui_menu_file_selector::append_entry_menu_item(const file_selector_entry *entry)
{
	const char *text = NULL;
	const char *subtext = NULL;

	switch(entry->type)
	{
		case SELECTOR_ENTRY_TYPE_EMPTY:
			text = "[empty slot]";
			break;

		case SELECTOR_ENTRY_TYPE_CREATE:
			text = "[create]";
			break;

		case SELECTOR_ENTRY_TYPE_SOFTWARE_LIST:
			text = "[software list]";
			break;

		case SELECTOR_ENTRY_TYPE_DRIVE:
			text = entry->basename;
			subtext = "[DRIVE]";
			break;

		case SELECTOR_ENTRY_TYPE_DIRECTORY:
			text = entry->basename;
			subtext = "[DIR]";
			break;

		case SELECTOR_ENTRY_TYPE_FILE:
			text = entry->basename;
			subtext = "[FILE]";
			break;
	}
	item_append(text, subtext, 0, (void *) entry);
}


//-------------------------------------------------
//  populate
//-------------------------------------------------

void ui_menu_file_selector::populate()
{
	zippath_directory *directory = NULL;
	file_error err = FILERR_NONE;
	const osd_directory_entry *dirent;
	const file_selector_entry *entry;
	const file_selector_entry *selected_entry = NULL;
	int i;
	const char *volume_name;
	const char *path = m_current_directory;

	// open the directory
	err = zippath_opendir(path, &directory);

	// clear out the menu entries
	m_entrylist = NULL;

	if (m_has_empty)
	{
		// add the "[empty slot]" entry
		append_entry(SELECTOR_ENTRY_TYPE_EMPTY, NULL, NULL);
	}

	if (m_has_create)
	{
		// add the "[create]" entry
		append_entry(SELECTOR_ENTRY_TYPE_CREATE, NULL, NULL);
	}

	if (m_has_softlist)
	{
		// add the "[software list]" entry
		append_entry(SELECTOR_ENTRY_TYPE_SOFTWARE_LIST, NULL, NULL);
	}

	// add the drives
	i = 0;
	while((volume_name = osd_get_volume_name(i))!=NULL)
	{
		append_entry(SELECTOR_ENTRY_TYPE_DRIVE,
			volume_name, volume_name);
		i++;
	}

	// build the menu for each item
	if (err == FILERR_NONE)
	{
		while((dirent = zippath_readdir(directory)) != NULL)
		{
			// append a dirent entry
			entry = append_dirent_entry(dirent);

			if (entry != NULL)
			{
				// set the selected item to be the first non-parent directory or file
				if ((selected_entry == NULL) && strcmp(dirent->name, ".."))
					selected_entry = entry;

				// do we have to select this file?
				if (!core_stricmp(m_current_file, dirent->name))
					selected_entry = entry;
			}
		}
	}

	// append all of the menu entries
	for (entry = m_entrylist; entry != NULL; entry = entry->next)
		append_entry_menu_item(entry);

	// set the selection (if we have one)
	if (selected_entry != NULL)
		set_selection((void *) selected_entry);

	// set up custom render proc
	customtop = machine().ui().get_line_height() + 3.0f * UI_BOX_TB_BORDER;

	if (directory != NULL)
		zippath_closedir(directory);
}


//-------------------------------------------------
//  handle
//-------------------------------------------------

void ui_menu_file_selector::handle()
{
	file_error err;
	const file_selector_entry *entry;
	const file_selector_entry *selected_entry = NULL;
	int bestmatch = 0;

	// process the menu
	const ui_menu_event *event = process(0);
	if (event != NULL && event->itemref != NULL)
	{
		// handle selections
		if (event->iptkey == IPT_UI_SELECT)
		{
			entry = (const file_selector_entry *) event->itemref;
			switch(entry->type)
			{
				case SELECTOR_ENTRY_TYPE_EMPTY:
					// empty slot - unload
					*m_result = R_EMPTY;
					ui_menu::stack_pop(machine());
					break;

				case SELECTOR_ENTRY_TYPE_CREATE:
					// create
					*m_result = R_CREATE;
					ui_menu::stack_pop(machine());
					break;

				case SELECTOR_ENTRY_TYPE_SOFTWARE_LIST:
					*m_result = R_SOFTLIST;
					ui_menu::stack_pop(machine());
					break;

				case SELECTOR_ENTRY_TYPE_DRIVE:
				case SELECTOR_ENTRY_TYPE_DIRECTORY:
					// drive/directory - first check the path
					err = zippath_opendir(entry->fullpath, NULL);
					if (err != FILERR_NONE)
					{
						// this path is problematic; present the user with an error and bail
						machine().ui().popup_time(1, "Error accessing %s", entry->fullpath);
						break;
					}
					m_current_directory.cpy(entry->fullpath);
					reset((ui_menu_reset_options)0);
					break;

				case SELECTOR_ENTRY_TYPE_FILE:
					// file
					m_current_file.cpy(entry->fullpath);
					*m_result = R_FILE;
					ui_menu::stack_pop(machine());
					break;
			}

			// reset the char buffer when pressing IPT_UI_SELECT
			if (m_filename_buffer[0] != '\0')
				memset(m_filename_buffer, '\0', ARRAY_LENGTH(m_filename_buffer));
		}
		else if (event->iptkey == IPT_SPECIAL)
		{
			int buflen = strlen(m_filename_buffer);
			bool update_selected = FALSE;

			// if it's a backspace and we can handle it, do so
			if ((event->unichar == 8 || event->unichar == 0x7f) && buflen > 0)
			{
				*(char *)utf8_previous_char(&m_filename_buffer[buflen]) = 0;
				update_selected = TRUE;

				if (ARRAY_LENGTH(m_filename_buffer) > 0)
					machine().ui().popup_time(ERROR_MESSAGE_TIME, "%s", m_filename_buffer);
			}
			// if it's any other key and we're not maxed out, update
			else if (event->unichar >= ' ' && event->unichar < 0x7f)
			{
				buflen += utf8_from_uchar(&m_filename_buffer[buflen], ARRAY_LENGTH(m_filename_buffer) - buflen, event->unichar);
				m_filename_buffer[buflen] = 0;
				update_selected = TRUE;

				if (ARRAY_LENGTH(m_filename_buffer) > 0)
					machine().ui().popup_time(ERROR_MESSAGE_TIME, "%s", m_filename_buffer);
			}

			if (update_selected)
			{
				const file_selector_entry *cur_selected = (const file_selector_entry *)get_selection();

				// check for entries which matches our m_filename_buffer:
				// from current entry to the end
				for (entry = cur_selected; entry != NULL; entry = entry->next)
				{
					if (entry->basename != NULL && m_filename_buffer != NULL)
					{
						int match = 0;
						for (int i = 0; i < ARRAY_LENGTH(m_filename_buffer); i++)
						{
							if (core_strnicmp(entry->basename, m_filename_buffer, i) == 0)
								match = i;
						}

						if (match > bestmatch)
						{
							bestmatch = match;
							selected_entry = entry;
						}
					}
				}
				// and from the first entry to current one
				for (entry = m_entrylist; entry != cur_selected; entry = entry->next)
				{
					if (entry->basename != NULL && m_filename_buffer != NULL)
					{
						int match = 0;
						for (int i = 0; i < ARRAY_LENGTH(m_filename_buffer); i++)
						{
							if (core_strnicmp(entry->basename, m_filename_buffer, i) == 0)
								match = i;
						}

						if (match > bestmatch)
						{
							bestmatch = match;
							selected_entry = entry;
						}
					}
				}

				if (selected_entry != NULL && selected_entry != cur_selected)
					set_selection((void *) selected_entry);
			}
		}
		else if (event->iptkey == IPT_UI_CANCEL)
		{
			// reset the char buffer also in this case
			if (m_filename_buffer[0] != '\0')
				memset(m_filename_buffer, '\0', ARRAY_LENGTH(m_filename_buffer));
		}
	}
}



/***************************************************************************
    SELECT FORMAT MENU
***************************************************************************/

//-------------------------------------------------
//  ctor
//-------------------------------------------------

ui_menu_select_format::ui_menu_select_format(running_machine &machine, render_container *container, floppy_image_format_t **formats, int ext_match, int total_usable, int *result)
	: ui_menu(machine, container)
{
	m_formats = formats;
	m_ext_match = ext_match;
	m_total_usable = total_usable;
	m_result = result;
}


//-------------------------------------------------
//  dtor
//-------------------------------------------------

ui_menu_select_format::~ui_menu_select_format()
{
}


//-------------------------------------------------
//  populate
//-------------------------------------------------

void ui_menu_select_format::populate()
{
	item_append("Select image format", NULL, MENU_FLAG_DISABLE, NULL);
	for (int i = 0; i < m_total_usable; i++)
	{
		const floppy_image_format_t *fmt = m_formats[i];

		if (i && i == m_ext_match)
			item_append(MENU_SEPARATOR_ITEM, NULL, 0, NULL);
		item_append(fmt->description(), fmt->name(), 0, (void *)(FPTR)i);
	}
}


//-------------------------------------------------
//  handle
//-------------------------------------------------

void ui_menu_select_format::handle()
{
	// process the menu
	const ui_menu_event *event = process(0);
	if (event != NULL && event->iptkey == IPT_UI_SELECT)
	{
		*m_result = int(FPTR(event->itemref));
		ui_menu::stack_pop(machine());
	}
}


/***************************************************************************
    SELECT RW
***************************************************************************/

//-------------------------------------------------
//  ctor
//-------------------------------------------------

ui_menu_select_rw::ui_menu_select_rw(running_machine &machine, render_container *container,
										bool can_in_place, int *result)
	: ui_menu(machine, container)
{
	m_can_in_place = can_in_place;
	m_result = result;
}


//-------------------------------------------------
//  dtor
//-------------------------------------------------

ui_menu_select_rw::~ui_menu_select_rw()
{
}


//-------------------------------------------------
//  populate
//-------------------------------------------------

void ui_menu_select_rw::populate()
{
	item_append("Select access mode", NULL, MENU_FLAG_DISABLE, NULL);
	item_append("Read-only", 0, 0, (void *)READONLY);
	if (m_can_in_place)
		item_append("Read-write", 0, 0, (void *)READWRITE);
	item_append("Read this image, write to another image", 0, 0, (void *)WRITE_OTHER);
	item_append("Read this image, write to diff", 0, 0, (void *)WRITE_DIFF);
}


//-------------------------------------------------
//  handle
//-------------------------------------------------

void ui_menu_select_rw::handle()
{
	// process the menu
	const ui_menu_event *event = process(0);
	if (event != NULL && event->iptkey == IPT_UI_SELECT)
	{
		*m_result = int(FPTR(event->itemref));
		ui_menu::stack_pop(machine());
	}
}
