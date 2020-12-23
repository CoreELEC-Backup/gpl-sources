/*
  Hatari - dlgAbout.c

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.

  Show information about the program and its license.
*/
const char DlgAbout_fileid[] = "Hatari dlgAbout.c : " __DATE__ " " __TIME__;

#include "main.h"
#include "version.h"
#include "dialog.h"
#include "sdlgui.h"

#include "gui-retro.h"

#define DLGABOUT_EXIT       17

/* The "About"-dialog: */
static SGOBJ aboutdlg[] =
{
	{ SGBOX, 0, 0, 0,0, 40,25, NULL },
	{ SGTEXT, 0, 0, 13,1, 12,1, PROG_NAME },
	{ SGTEXT, 0, 0, 13,2, 12,1, "=============" },
	{ SGTEXT, 0, 0, 1,4, 38,1, "Written by Thomas Huth and many other" },
	{ SGTEXT, 0, 0, 1,5, 38,1, "people around the world." },
	{ SGTEXT, 0, 0, 2,7, 34,1, "Please see the docs for more info!" },
	{ SGTEXT, 0, 0, 1,9, 38,1, "This program is free software; you can" },
	{ SGTEXT, 0, 0, 1,10, 38,1, "redistribute it and/or modify it under" },
	{ SGTEXT, 0, 0, 1,11, 38,1, "the terms of the GNU General Public" },
	{ SGTEXT, 0, 0, 1,12, 38,1, "License as published by the Free Soft-" },
	{ SGTEXT, 0, 0, 1,13, 38,1, "ware Foundation; either version 2 of" },
	{ SGTEXT, 0, 0, 1,14, 38,1, "the License, or (at your option) any" },
	{ SGTEXT, 0, 0, 1,15, 38,1, "later version." },
	{ SGTEXT, 0, 0, 1,17, 38,1, "This program is distributed in the" },
	{ SGTEXT, 0, 0, 1,18, 38,1, "hope that it will be useful, but" },
	{ SGTEXT, 0, 0, 1,19, 38,1, "WITHOUT ANY WARRANTY. See the GNU Ge-" },
	{ SGTEXT, 0, 0, 1,20, 38,1, "neral Public License for more details." },
	{ SGBUTTON, SG_EXIT/*SG_DEFAULT*/, 0, 16,23, 8,1, "OK" },
	{ -1, 0, 0, 0,0, 0,0, NULL }
};


/*-----------------------------------------------------------------------*/
/**
 * Show the "about" dialog:
 */
void Dialog_AboutDlg(void)
{
	int but;

	/* Center PROG_NAME title string */
	aboutdlg[1].x = (aboutdlg[0].w - strlen(PROG_NAME)) / 2;

	SDLGui_CenterDlg(aboutdlg);
        do
	{
                but=SDLGui_DoDialog(aboutdlg, NULL);
                gui_poll_events();

        }
        while (but != DLGABOUT_EXIT && but != SDLGUI_QUIT
	       && but != SDLGUI_ERROR && !bQuitProgram);

}
