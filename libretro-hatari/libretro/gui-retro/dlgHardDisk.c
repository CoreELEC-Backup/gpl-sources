/*
  Hatari - dlgHardDisk.c

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.
*/
const char DlgHardDisk_fileid[] = "Hatari dlgHardDisk.c : " __DATE__ " " __TIME__;

#include <assert.h>
#include "main.h"
#include "configuration.h"
#include "dialog.h"
#include "sdlgui.h"
#include "file.h"

#include "gui-retro.h"

#define DISKDLG_ACSIEJECT          3
#define DISKDLG_ACSIBROWSE         4
#define DISKDLG_ACSINAME           5
#define DISKDLG_IDEMASTEREJECT     7
#define DISKDLG_IDEMASTERBROWSE    8
#define DISKDLG_IDEMASTERNAME      9
#define DISKDLG_IDESLAVEEJECT     11
#define DISKDLG_IDESLAVEBROWSE    12
#define DISKDLG_IDESLAVENAME      13
#define DISKDLG_GEMDOSEJECT       15
#define DISKDLG_GEMDOSBROWSE      16
#define DISKDLG_GEMDOSNAME        17
#define DISKDLG_DRIVESKIP         18
#define DISKDLG_PROTOFF           20
#define DISKDLG_PROTON            21
#define DISKDLG_PROTAUTO          22
#define DISKDLG_BOOTHD            23
#define DISKDLG_EXIT              24


/* The disks dialog: */
static SGOBJ diskdlg[] =
{
	{ SGBOX, 0, 0, 0,0, 64,19, NULL },
	{ SGTEXT, 0, 0, 27,1, 10,1, "Hard disks" },

	{ SGTEXT, 0, 0, 2,3, 14,1, "ACSI HD image:" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 46,3, 7,1, "Eject" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 54,3, 8,1, "Browse" },
	{ SGTEXT, 0, 0, 3,4, 58,1, NULL },

	{ SGTEXT, 0, 0, 2,5, 20,1, "IDE HD master image:" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 46,5, 7,1, "Eject" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 54,5, 8,1, "Browse" },
	{ SGTEXT, 0, 0, 3,6, 58,1, NULL },

	{ SGTEXT, 0, 0, 2,7, 19,1, "IDE HD slave image:" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 46,7, 7,1, "Eject" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 54,7, 8,1, "Browse" },
	{ SGTEXT, 0, 0, 3,8, 58,1, NULL },

	{ SGTEXT, 0, 0, 2,9, 13,1, "GEMDOS drive:" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 46,9, 7,1, "Eject" },
	{ SGBUTTON, SG_EXIT/*0*/, 0, 54,9, 8,1, "Browse" },
	{ SGTEXT, 0, 0, 3,10, 58,1, NULL },

	{ SGCHECKBOX, 0, 0, 8,11, 42,1, "After ACSI/IDE _partitions (experimental)" },

	{ SGTEXT, 0, 0, 8,12, 31,1, "Write protection:" },
	{ SGRADIOBUT, 0, 0, 26,12, 5,1, "Off" },
	{ SGRADIOBUT, 0, 0, 32,12, 4,1, "On" },
	{ SGRADIOBUT, 0, 0, 37,12, 6,1, "Auto" },

	{ SGCHECKBOX, 0, 0, 2,14, 14,1, "Boot from HD" },

	{ SGBUTTON, SG_EXIT/*SG_DEFAULT*/, 0, 22,17, 20,1, "Back to main menu" },
	{ -1, 0, 0, 0,0, 0,0, NULL }
};


/**
 * Let user browse given directory, set directory if one selected.
 * return false if none selected, otherwise return true.
 */
static bool DlgDisk_BrowseDir(char *dlgname, char *confname, int maxlen)
{
	char *str, *selname;

	selname = SDLGui_FileSelect("NULL",confname, NULL, false);
	if (selname)
	{
		strcpy(confname, selname);
		free(selname);

		str = strrchr(confname, PATHSEP);
		if (str != NULL)
			str[1] = 0;
		File_CleanFileName(confname);
		File_ShrinkName(dlgname, confname, maxlen);
		return true;
	}
	return false;
}


/**
 * Show and process the hard disk dialog.
 */
void DlgHardDisk_Main(void)
{
	int but, i;
	char dlgname_gdos[64], dlgname_acsi[64];
	char dlgname_ide_master[64], dlgname_ide_slave[64];

	SDLGui_CenterDlg(diskdlg);

	/* Set up dialog to actual values: */
printf("skip: %d\n", ConfigureParams.HardDisk.nHardDiskDrive);
	/* Skip ACSI/IDE partitions? */
	if (ConfigureParams.HardDisk.nHardDiskDrive == DRIVE_SKIP)
		diskdlg[DISKDLG_DRIVESKIP].state |= SG_SELECTED;
	else
		diskdlg[DISKDLG_DRIVESKIP].state &= ~SG_SELECTED;

	/* Boot from harddisk? */
	if (ConfigureParams.HardDisk.bBootFromHardDisk)
		diskdlg[DISKDLG_BOOTHD].state |= SG_SELECTED;
	else
		diskdlg[DISKDLG_BOOTHD].state &= ~SG_SELECTED;

	/* ACSI hard disk image: */
	if (ConfigureParams.Acsi[0].bUseDevice)
		File_ShrinkName(dlgname_acsi, ConfigureParams.Acsi[0].sDeviceFile,
		                diskdlg[DISKDLG_ACSINAME].w);
	else
		dlgname_acsi[0] = '\0';
	diskdlg[DISKDLG_ACSINAME].txt = dlgname_acsi;

	/* IDE master hard disk image: */
	if (ConfigureParams.HardDisk.bUseIdeMasterHardDiskImage)
		File_ShrinkName(dlgname_ide_master, ConfigureParams.HardDisk.szIdeMasterHardDiskImage,
		                diskdlg[DISKDLG_IDEMASTERNAME].w);
	else
		dlgname_ide_master[0] = '\0';
	diskdlg[DISKDLG_IDEMASTERNAME].txt = dlgname_ide_master;

	/* IDE slave hard disk image: */
	if (ConfigureParams.HardDisk.bUseIdeSlaveHardDiskImage)
		File_ShrinkName(dlgname_ide_slave, ConfigureParams.HardDisk.szIdeSlaveHardDiskImage,
		                diskdlg[DISKDLG_IDESLAVENAME].w);
	else
		dlgname_ide_slave[0] = '\0';
	diskdlg[DISKDLG_IDESLAVENAME].txt = dlgname_ide_slave;

	/* GEMDOS hard disk directory: */
	if (ConfigureParams.HardDisk.bUseHardDiskDirectories)
		File_ShrinkName(dlgname_gdos, ConfigureParams.HardDisk.szHardDiskDirectories[0],
		                diskdlg[DISKDLG_GEMDOSNAME].w);
	else
		dlgname_gdos[0] = '\0';
	diskdlg[DISKDLG_GEMDOSNAME].txt = dlgname_gdos;

	/* Write protection */
	for (i = DISKDLG_PROTOFF; i <= DISKDLG_PROTAUTO; i++)
	{
		diskdlg[i].state &= ~SG_SELECTED;
	}
	diskdlg[DISKDLG_PROTOFF+ConfigureParams.HardDisk.nWriteProtection].state |= SG_SELECTED;

	/* Draw and process the dialog */
	do
	{
		but = SDLGui_DoDialog(diskdlg, NULL);
		switch (but)
		{
		 case DISKDLG_ACSIEJECT:
			ConfigureParams.Acsi[0].bUseDevice = false;
			dlgname_acsi[0] = '\0';
			break;
		 case DISKDLG_ACSIBROWSE:
			if (SDLGui_FileConfSelect("NULL",dlgname_acsi,
			                          ConfigureParams.Acsi[0].sDeviceFile,
			                          diskdlg[DISKDLG_ACSINAME].w, false))
				ConfigureParams.Acsi[0].bUseDevice = true;
			break;
		 case DISKDLG_IDEMASTEREJECT:
			ConfigureParams.HardDisk.bUseIdeMasterHardDiskImage = false;
			dlgname_ide_master[0] = '\0';
			break;
		 case DISKDLG_IDEMASTERBROWSE:
			if (SDLGui_FileConfSelect("NULL",dlgname_ide_master,
			                          ConfigureParams.HardDisk.szIdeMasterHardDiskImage,
			                          diskdlg[DISKDLG_IDEMASTERNAME].w, false))
				ConfigureParams.HardDisk.bUseIdeMasterHardDiskImage = true;
			break;
		 case DISKDLG_IDESLAVEEJECT:
			ConfigureParams.HardDisk.bUseIdeSlaveHardDiskImage = false;
			dlgname_ide_slave[0] = '\0';
			break;
		 case DISKDLG_IDESLAVEBROWSE:
			if (SDLGui_FileConfSelect("NULL",dlgname_ide_slave,
			                          ConfigureParams.HardDisk.szIdeSlaveHardDiskImage,
			                          diskdlg[DISKDLG_IDESLAVENAME].w, false))
				ConfigureParams.HardDisk.bUseIdeSlaveHardDiskImage = true;
			break;
		 case DISKDLG_GEMDOSEJECT:
			ConfigureParams.HardDisk.bUseHardDiskDirectories = false;
			dlgname_gdos[0] = '\0';
			break;
		 case DISKDLG_GEMDOSBROWSE:
			if (DlgDisk_BrowseDir(dlgname_gdos,
			                     ConfigureParams.HardDisk.szHardDiskDirectories[0],
			                     diskdlg[DISKDLG_GEMDOSNAME].w))
				ConfigureParams.HardDisk.bUseHardDiskDirectories = true;
			break;
		}
                gui_poll_events();
	}
	while (but != DISKDLG_EXIT && but != SDLGUI_QUIT
	        && but != SDLGUI_ERROR && !bQuitProgram);

	/* Read values from dialog: */
	for (i = DISKDLG_PROTOFF; i <= DISKDLG_PROTAUTO; i++)
	{
		if (diskdlg[i].state & SG_SELECTED)
		{
			ConfigureParams.HardDisk.nWriteProtection = i-DISKDLG_PROTOFF;
			break;
		}
	}
	ConfigureParams.HardDisk.bBootFromHardDisk = (diskdlg[DISKDLG_BOOTHD].state & SG_SELECTED);

	if (diskdlg[DISKDLG_DRIVESKIP].state & SG_SELECTED)
		ConfigureParams.HardDisk.nHardDiskDrive = DRIVE_SKIP;
	else if (ConfigureParams.HardDisk.nHardDiskDrive == DRIVE_SKIP)
		ConfigureParams.HardDisk.nHardDiskDrive = DRIVE_C;
}
