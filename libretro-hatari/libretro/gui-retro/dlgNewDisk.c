/*
  Hatari - dlgNewDisk.c

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.
*/
const char DlgNewDisk_fileid[] = "Hatari dlgNewDisk.c : " __DATE__ " " __TIME__;

#include "main.h"
#include "configuration.h"
#include "createBlankImage.h"
#include "dialog.h"
#include "sdlgui.h"
#include "file.h"
#include "log.h"

#include "gui-retro.h"

#define DLGNEWDISK_DECTRACK   3
#define DLGNEWDISK_TRACKSTR   4
#define DLGNEWDISK_INCTRACK   5
#define DLGNEWDISK_SECTORS9   7
#define DLGNEWDISK_SECTORS10  8
#define DLGNEWDISK_SECTORS11  9
#define DLGNEWDISK_SECTORS18  10
#define DLGNEWDISK_SECTORS36  11
#define DLGNEWDISK_SIDES1     13
#define DLGNEWDISK_SIDES2     14
#define DLGNEWDISK_SAVE       15
#define DLGNEWDISK_EXIT       16

static char szTracks[3];
static int nTracks = 80;

/* The new disk image dialog: */
static SGOBJ newdiskdlg[] =
{
	{ SGBOX, 0, 0, 0,0, 29,14, NULL },
	{ SGTEXT, 0, 0, 6,1, 16,1, "New floppy image" },
	{ SGTEXT, 0, 0, 2,3, 7,1, "Tracks:" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 12,3, 1,1, SGARROWLEFT/*"\x04"*/ },   /* Left-arrow button  */
	{ SGTEXT, 0, 0, 14,3, 2,1, szTracks },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 17,3, 1,1, SGARROWRIGHT/*"\x03"*/ },   /* Right-arrow button */
	{ SGTEXT, 0, 0, 2,5, 8,1, "Sectors:" },
	{ SGRADIOBUT, 0, SG_SELECTED, 12,5, 4,1, " 9" },
	{ SGRADIOBUT, 0, 0, 17,5, 4,1, "10" },
	{ SGRADIOBUT, 0, 0, 22,5, 4,1, "11" },
	{ SGRADIOBUT, 0, 0, 12,6, 9,1, "18 (HD)" },
	{ SGRADIOBUT, 0, 0, 12,7, 9,1, "36 (ED)" },
	{ SGTEXT, 0, 0, 2,9, 6,1, "Sides:" },
	{ SGRADIOBUT, 0, 0, 12,9, 4,1, "1" },
	{ SGRADIOBUT, 0, SG_SELECTED, 17,9, 4,1, "2" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 4,12, 8,1, "Create" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 18,12, 6,1, "Back" },
	{ -1, 0, 0, 0,0, 0,0, NULL }
};

#define DEFAULT_DISK_NAME "new_disk.st"


/*-----------------------------------------------------------------------*/
/**
 * Handle creation of a the "new blank disk image".
 * return true if disk created, false otherwise.
 */
static bool DlgNewDisk_CreateDisk(const char *path)
{
	int nSectors, nSides;

	/* (potentially non-existing) filename? */
	if (File_DirExists(path))
	{
		Log_AlertDlg(LOG_ERROR, "ERROR: '%s' isn't a file!", path);
		return false;
	}

	/* Get number of sectors */
	if (newdiskdlg[DLGNEWDISK_SECTORS36].state & SG_SELECTED)
		nSectors = 36;
	else if (newdiskdlg[DLGNEWDISK_SECTORS18].state & SG_SELECTED)
		nSectors = 18;
	else if (newdiskdlg[DLGNEWDISK_SECTORS11].state & SG_SELECTED)
		nSectors = 11;
	else if (newdiskdlg[DLGNEWDISK_SECTORS10].state & SG_SELECTED)
		nSectors = 10;
	else
		nSectors = 9;

	/* Get number of sides */
	if (newdiskdlg[DLGNEWDISK_SIDES1].state & SG_SELECTED)
		nSides = 1;
	else
		nSides = 2;
	
	return CreateBlankImage_CreateFile(path, nTracks, nSectors, nSides);
}


/*-----------------------------------------------------------------------*/
/**
 * Show and process the "new blank disk image" dialog.
 * Return file name of last created diskimage or NULL if none created.
 * Caller needs to free the name.
 */
char *DlgNewDisk_Main(void)
{
	int but;
	char *szNewDiskName, *tmpname, *retname = NULL;
	sprintf(szTracks, "%i", nTracks);

 	SDLGui_CenterDlg(newdiskdlg);

	/* Initialize disk image name: */
	szNewDiskName = File_MakePath(ConfigureParams.DiskImage.szDiskImageDirectory, "new_disk.st", NULL);
	if (!szNewDiskName)
		return NULL;

	/* Draw and process the dialog */
	do
	{
		but = SDLGui_DoDialog(newdiskdlg, NULL);
		switch(but)
		{
		 case DLGNEWDISK_DECTRACK:
			if (nTracks > 40)
				nTracks -= 1;
			sprintf(szTracks, "%i", nTracks);
			break;
		 case DLGNEWDISK_INCTRACK:
			if (nTracks < 85)
				nTracks += 1;
			sprintf(szTracks, "%i", nTracks);
			break;
		 case DLGNEWDISK_SAVE:
			tmpname = SDLGui_FileSelect("NULL",szNewDiskName, NULL, true);
			if (tmpname)
			{
				if (DlgNewDisk_CreateDisk(tmpname))
				{
					if (retname)
						free(retname);
					retname = tmpname;
				}
				else
					free(tmpname);
			}
			break;
		}
                gui_poll_events();
	}
	while (but != DLGNEWDISK_EXIT && but != SDLGUI_QUIT
	       && but != SDLGUI_ERROR && !bQuitProgram);

	free(szNewDiskName);
	return retname;
}
