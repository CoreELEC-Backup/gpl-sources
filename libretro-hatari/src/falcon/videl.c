/*
  Hatari - videl.c

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.

  Falcon Videl emulation. The Videl is the graphics shifter chip of the Falcon.
  It supports free programmable resolutions with 1, 2, 4, 8 or 16 bits per
  pixel.

  This file originally came from the Aranym project and has been heavily
  modified to work for Hatari (but the kudos for the great Videl emulation
  code goes to the people from the Aranym project of course).

  Videl can run at 2 frequencies : 25.175 Mhz or 32 MHz
  
  Hardware I/O registers:

	$FFFF8006 (byte) : monitor type

	$FFFF8201 (byte) : VDL_VBH - Video Base Hi
	$FFFF8203 (byte) : VDL_VBM - Video Base Mi
	$FFFF8205 (byte) : VDL_VCH - Video Count Hi
	$FFFF8207 (byte) : VDL_VCM - Video Count Mi
	$FFFF8209 (byte) : VDL_VCL - Video Count Lo
	$FFFF820A (byte) : VDL_SYM - Sync mode
	$FFFF820D (byte) : VDL_VBL - Video Base Lo
	$FFFF820E (word) : VDL_LOF - Offset to next line
	$FFFF8210 (word) : VDL_LWD - Line Wide in Words
	
	$FFFF8240 (word) : VDL_STC - ST Palette Register 00
	.........
	$FFFF825E (word) : VDL_STC - ST Palette Register 15

	$FFFF8260 (byte) : ST shift mode
	$FFFF8264 (byte) : Horizontal scroll register shadow register
	$FFFF8265 (byte) : Horizontal scroll register
	$FFFF8266 (word) : Falcon shift mode
	
	$FFFF8280 (word) : HHC - Horizontal Hold Counter
	$FFFF8282 (word) : HHT - Horizontal Hold Timer
	$FFFF8284 (word) : HBB - Horizontal Border Begin
	$FFFF8286 (word) : HBE - Horizontal Border End
	$FFFF8288 (word) : HDB - Horizontal Display Begin
	$FFFF828A (word) : HDE - Horizontal Display End
	$FFFF828C (word) : HSS - Horizontal SS
	$FFFF828E (word) : HFS - Horizontal FS
	$FFFF8290 (word) : HEE - Horizontal EE
	
	$FFFF82A0 (word) : VFC - Vertical Frequency Counter
	$FFFF82A2 (word) : VFT - Vertical Frequency Timer
	$FFFF82A4 (word) : VBB - Vertical Border Begin
	$FFFF82A6 (word) : VBE - Vertical Border End
	$FFFF82A8 (word) : VDB - Vertical Display Begin
	$FFFF82AA (word) : VDE - Vertical Display End
	$FFFF82AC (word) : VSS - Vertical SS
	
	$FFFF82C0 (word) : VCO - Video control
	$FFFF82C2 (word) : VMD - Video mode

	$FFFF9800 (long) : VDL_PAL - Videl palette Register 000
	...........
	$FFFF98FC (long) : VDL_PAL - Videl palette Register 255
*/

const char VIDEL_fileid[] = "Hatari videl.c : " __DATE__ " " __TIME__;

#include <SDL_endian.h>
#include <SDL.h>
#include "main.h"
#include "configuration.h"
#include "memorySnapShot.h"
#include "ioMem.h"
#include "log.h"
#include "hostscreen.h"
#include "screen.h"
#include "stMemory.h"
#include "videl.h"
#include "video.h"				/* for bUseHighRes variable, maybe unuseful (Laurent) */
#include "vdi.h"				/* for bUseVDIRes variable,  maybe unuseful (Laurent) */

#define Atari2HostAddr(a) (&STRam[a])
#define VIDEL_COLOR_REGS_BEGIN	0xff9800


struct videl_s {
	bool   bUseSTShifter;			/* whether to use ST or Falcon palette */
	Uint8  reg_ffff8006_save;		/* save reg_ffff8006 as it's a read only register */
	Uint8  monitor_type;			/* 00 Monochrome (SM124) / 01 Color (SC1224) / 10 VGA Color / 11 Television ($FFFF8006) */
	Uint32 videoBaseAddr;			/* Video base address, refreshed after each VBL */

	Sint16 leftBorderSize;			/* Size of the left border */
	Sint16 rightBorderSize;			/* Size of the right border */
	Sint16 upperBorderSize;			/* Size of the upper border */
	Sint16 lowerBorderSize;			/* Size of the lower border */
	Uint16 XSize;				/* X size of the graphical area */
	Uint16 YSize;				/* Y size of the graphical area */

	Uint16 save_scrWidth;			/* save screen width to detect a change of X resolution */
	Uint16 save_scrHeight;			/* save screen height to detect a change of Y resolution */
	Uint16 save_scrBpp;			/* save screen Bpp to detect a change of bitplan mode */

	bool hostColorsSync;			/* Sync palette with host's */
};

struct videl_zoom_s {
	Uint16 zoomwidth;
	Uint16 prev_scrwidth;
	Uint16 zoomheight;
	Uint16 prev_scrheight;
	int *zoomxtable;
	int *zoomytable;
};

static struct videl_s videl;
static struct videl_zoom_s videl_zoom;

Uint16 vfc_counter;			/* counter for VFC register $ff82a0 (to be internalized when VIDEL emulation is complete) */

static void VIDEL_memset_uint32(Uint32 *addr, Uint32 color, int count);
static void VIDEL_memset_uint16(Uint16 *addr, Uint16 color, int count);
static void VIDEL_memset_uint8(Uint8 *addr, Uint8 color, int count);
static void Videl_ColorReg_WriteWord(void);


/**
 *  Called upon startup and when CPU encounters a RESET instruction.
 */
void VIDEL_reset(void)
{
	videl.bUseSTShifter = false;				/* Use Falcon color palette by default */
	videl.reg_ffff8006_save = IoMem_ReadByte(0xff8006);
	videl.monitor_type = videl.reg_ffff8006_save & 0xc0;
	
	videl.hostColorsSync = false; 

	vfc_counter = 0;
	
	/* Autozoom */
	videl_zoom.zoomwidth = 0;
	videl_zoom.prev_scrwidth = 0;
	videl_zoom.zoomheight = 0;
	videl_zoom.prev_scrheight = 0;
	videl_zoom.zoomxtable = NULL;
	videl_zoom.zoomytable = NULL;

	/* Default resolution to boot with */
	videl.save_scrWidth = 640;
	videl.save_scrHeight = 480;
	videl.save_scrBpp = ConfigureParams.Screen.nForceBpp;
	HostScreen_setWindowSize(videl.save_scrWidth, videl.save_scrHeight, videl.save_scrBpp);

	/* Reset IO register (some are not initialized by TOS) */
	IoMem_WriteWord(0xff820e, 0);    /* Line offset */
	IoMem_WriteWord(0xff8264, 0);    /* Horizontal scroll */

	/* Init synch mode register */
	 VIDEL_SyncMode_WriteByte();
}

/**
 * Save/Restore snapshot of local variables ('MemorySnapShot_Store' handles type)
 */
void VIDEL_MemorySnapShot_Capture(bool bSave)
{
	/* Save/Restore details */
	MemorySnapShot_Store(&videl, sizeof(videl));
	MemorySnapShot_Store(&vfc_counter, sizeof(vfc_counter));
}

/**
 * Monitor write access to Falcon color palette registers
 */
void VIDEL_FalconColorRegsWrite(void)
{
	uint32_t color = IoMem_ReadLong(IoAccessBaseAddress & ~3);
	color &= 0xfcfc00fc;	/* Unused bits have to be set to 0 */
	IoMem_WriteLong(IoAccessBaseAddress & ~3, color);
	videl.hostColorsSync = false;
}

/**
 * VIDEL_Monitor_WriteByte : Contains memory and monitor configuration. 
 *                           This register is read only.
 */
void VIDEL_Monitor_WriteByte(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff8006 Monitor and memory conf write (Read only)\n");
	/* Restore hardware value */

	IoMem_WriteByte(0xff8006, videl.reg_ffff8006_save);
}

/**
 * VIDEL_SyncMode_WriteByte : Videl synchronization mode. 
 *             $FFFF820A [R/W] _______0  .................................. SYNC-MODE
                                     ||
                                     |+--Synchronisation [ 0:internal / 1:external ]
                                     +---Vertical frequency [ Read-only bit ]
                                         [ Monochrome monitor:0 / Colour monitor:1 ]
 */
void VIDEL_SyncMode_WriteByte(void)
{
	Uint8 syncMode = IoMem_ReadByte(0xff820a);
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff820a Sync Mode write: 0x%02x\n", syncMode);

	if (videl.monitor_type == FALCON_MONITOR_MONO)
		syncMode &= 0xfd;
	else
		syncMode |= 0x2;
	
	IoMem_WriteByte(0xff820a, syncMode);
}

/**
 * Read video address counter and update ff8205/07/09
 */
void VIDEL_ScreenCounter_ReadByte(void)
{
//	Uint32 addr;	// To be used
	Uint32 addr = 0; // To be removed

	// addr = Videl_CalculateAddress();		/* TODO: get current video address */
	IoMem[0xff8205] = ( addr >> 16 ) & 0xff;
	IoMem[0xff8207] = ( addr >> 8 ) & 0xff;
	IoMem[0xff8209] = addr & 0xff;

	LOG_TRACE(TRACE_VIDEL, "Videl : $ff8205/07/09 Sync Mode read: 0x%08x\n", addr);
}

/**
 * Write video address counter
 */
void VIDEL_ScreenCounter_WriteByte(void)
{
	Uint32 addr_new = 0;
	Uint8 AddrByte;

	AddrByte = IoMem[ IoAccessCurrentAddress ];

	/* Compute the new video address with one modified byte */
	if ( IoAccessCurrentAddress == 0xff8205 )
		addr_new = ( addr_new & 0x00ffff ) | ( AddrByte << 16 );
	else if ( IoAccessCurrentAddress == 0xff8207 )
		addr_new = ( addr_new & 0xff00ff ) | ( AddrByte << 8 );
	else if ( IoAccessCurrentAddress == 0xff8209 )
		addr_new = ( addr_new & 0xffff00 ) | ( AddrByte );

	// TODO: save the value in a table for the final rendering 
}

/**
 * VIDEL_LineOffset_WriteWord: $FFFF820E [R/W] W _______876543210  Line Offset
 * How many words are added to the end of display line, i.e. how many words are
 * 'behind' the display.
 */
void VIDEL_LineOffset_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff820e Line Offset write: 0x%04x\n",
	          IoMem_ReadWord(0xff820e));
}

/**
 * VIDEL_Line_Width_WriteWord: $FFFF8210 [R/W] W ______9876543210 Line Width (VWRAP)
 * Length of display line in words.Or, how many words should be added to
 * vram counter after every display line.
 */
void VIDEL_Line_Width_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff8210 Line Width write: 0x%04x\n",
	          IoMem_ReadWord(0xff8210));
}

/**
 * Write to video address base high, med and low register (0xff8201/03/0d).
 * On Falcon, when a program writes to high or med registers, base low register
 * is reset to zero.
 */
void VIDEL_ScreenBase_WriteByte(void)
{
	if ((IoAccessCurrentAddress == 0xff8201) || (IoAccessCurrentAddress == 0xff8203)) {
		/* Reset screen base low register */
		IoMem[0xff820d] = 0;
	}

	LOG_TRACE(TRACE_VIDEL, "Videl : $%04x Screen base write: 0x%02x\t (screen: 0x%04x)\n",
	          IoAccessCurrentAddress, IoMem[IoAccessCurrentAddress],
	          (IoMem[0xff8201]<<16) + (IoMem[0xff8203]<<8) + IoMem[0xff820d]);
}

/**
    VIDEL_ST_ShiftModeWriteByte : 
	$FFFF8260 [R/W] B  ______10  ST Shift Mode
	                         ||
	                         ||                           others   vga
	                         ||                  $FF8210 $FF82C2 $FF82C2
	                         00--4BP/320 Pixels=> $0050   $0000   $0005
	                         01--2BP/640 Pixels=> $0050   $0004   $0009
	                         10--1BP/640 Pixels=> $0028   $0006   $0008
	                         11--???/320 Pixels=> $0050   $0000   $0000

	Writing to this register does the following things:
		- activate STE palette
		- sets line width ($ffff8210)
		- sets video mode in $ffff82c2 (double lines/interlace & cycles/pixel)
 */
void VIDEL_ST_ShiftModeWriteByte(void)
{
	Uint16 line_width, video_mode;
	Uint8 st_shiftMode;

	st_shiftMode = IoMem_ReadByte(0xff8260);
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff8260 ST Shift Mode (STSHIFT) write: 0x%02x\n", st_shiftMode);

	/* Bits 2-7 are set to 0 */
	IoMem_WriteByte(0xff8260, st_shiftMode & 3); 

	/* Activate STE palette */
	videl.bUseSTShifter = true;

	/*  Compute line width and video mode */
	switch (st_shiftMode & 0x3) {
		case 0:	/* 4BP/320 Pixels */
			line_width = 0x50;
			/* half pixels + double lines vs. no scaling */
			video_mode = videl.monitor_type == FALCON_MONITOR_VGA ? 0x5 : 0x0;
			break;
		case 1:	/* 2BP/640 Pixels */
			line_width = 0x50;
			/* quarter pixels + double lines vs. half pixels */
			video_mode = videl.monitor_type == FALCON_MONITOR_VGA ? 0x9 : 0x4;
			break;
		case 2:	/* 1BP/640 Pixels */
			line_width = 0x28;
			if (videl.monitor_type == FALCON_MONITOR_MONO) {
				video_mode = 0x0;
				break;
			}
			/* quarter pixels vs. half pixels + interlace */
			video_mode = videl.monitor_type == FALCON_MONITOR_VGA ? 0x8 : 0x6;
			break;
		case 3:	/* ???/320 Pixels */
		default:
			line_width = 0x50;
			video_mode = 0x0;
			break;
	}

	/* Set line width ($FFFF8210) */
	IoMem_WriteWord(0xff8210, line_width); 
	
	/* Set video mode ($FFFF82C2) */
	IoMem_WriteWord(0xff82c2, video_mode); 
}

/**
    VIDEL_HorScroll64_WriteByte : Horizontal scroll register (0-15)
		$FFFF8264 [R/W] ________  ................................ H-SCROLL HI
				    ||||  [ Shadow register for $FFFF8265 ]
				    ++++--Pixel shift [ 0:normal / 1..15:Left shift ]
					[ Change in line-width NOT required ]
 */
void VIDEL_HorScroll64_WriteByte(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff8264 Horizontal scroll 64 write: 0x%02x\n",
	          IoMem_ReadByte(0xff8264));
}

/**
    VIDEL_HorScroll65_WriteByte : Horizontal scroll register (0-15)
		$FFFF8265 [R/W] ____3210  .................................H-SCROLL LO
				    ||||
				    ++++--Pixel [ 0:normal / 1..15:Left shift ]
					[ Change in line-width NOT required ]
 */
void VIDEL_HorScroll65_WriteByte(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff8265 Horizontal scroll 65 write: 0x%02x\n",
	          IoMem_ReadByte(0xff8265));
}

/**
    VIDEL_Falcon_ShiftMode_WriteWord :
	$FFFF8266 [R/W] W  _____A98_6543210  Falcon Shift Mode (SPSHIFT)
	                        ||| |||||||
	                        ||| |||++++- 0..15: Colourbank choice from 256-colour table in 16 colour multiples
	                        ||| ||+----- 8 Bitplanes mode (256 Colors) [0:off / 1:on]
	                        ||| |+------ Vertical Sync [0: internal / 1: external]
	                        ||| +------- Horizontal Sync [0: internal / 1: external]
	                        ||+--------- True-Color-Mode [0:off / 1:on]
	                        |+---------- Overlay-Mode [0:off / 1:on]
	                        +----------- 0: 2-Color-Mode [0:off / 1:on]

	Writing to this register does the following things:
		- activate Falcon palette
		- if you set Bits A/8/4 == 0, it selects 16-Color-Falcon-Mode (NOT the
		  same as ST LOW since Falcon palette is used!)
		- $8260 register is ignored, you don't need to write here anything

	Note: 4-Color-Mode isn't realisable with Falcon palette.
 */
void VIDEL_Falcon_ShiftMode_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff8266 Falcon Shift Mode (SPSHIFT) write: 0x%04x\n",
	          IoMem_ReadWord(0xff8266));

	videl.bUseSTShifter = false;
}

/**
 *  Write Horizontal Hold Counter (HHC)
 */
void VIDEL_HHC_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff8280 Horizontal Hold Counter (HHC) write: 0x%04x\n",
	          IoMem_ReadWord(0xff8280));
}

/**
 *  Write Horizontal Hold Timer (HHT)
 */
void VIDEL_HHT_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff8282 Horizontal Hold Timer (HHT) write: 0x%04x\n",
	          IoMem_ReadWord(0xff8282));
}

/**
 *  Write Horizontal Border Begin (HBB)
 */
void VIDEL_HBB_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff8284 Horizontal Border Begin (HBB) write: 0x%04x\n",
	          IoMem_ReadWord(0xff8284));
}

/**
 *  Write Horizontal Border End (HBE)
 */
void VIDEL_HBE_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff8286 Horizontal Border End (HBE) write: 0x%04x\n",
	          IoMem_ReadWord(0xff8286));
}

/**
 *  Write Horizontal Display Begin (HDB)
	$FFFF8288 [R/W] W ______9876543210  Horizontal Display Begin (HDB)
				|
				+---------- Display will start in [0: 1st halfline / 1: 2nd halfline]
 */
void VIDEL_HDB_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff8288 Horizontal Display Begin (HDB) write: 0x%04x\n",
	          IoMem_ReadWord(0xff8288));
}

/**
 *  Write Horizontal Display End (HDE)
 */
void VIDEL_HDE_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff828a Horizontal Display End (HDE) write: 0x%04x\n",
	          IoMem_ReadWord(0xff828a));
}

/**
 *  Write Horizontal SS (HSS)
 */
void VIDEL_HSS_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff828c Horizontal SS (HSS) write: 0x%04x\n",
	          IoMem_ReadWord(0xff828c));
}

/**
 *  Write Horizontal FS (HFS)
 */
void VIDEL_HFS_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff828e Horizontal FS (HFS) write: 0x%04x\n",
	          IoMem_ReadWord(0xff828e));
}

/**
 *  Write Horizontal EE (HEE)
 */
void VIDEL_HEE_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff8290 Horizontal EE (HEE) write: 0x%04x\n",
	          IoMem_ReadWord(0xff8290));
}

/**
 *  Write Vertical Frequency Counter (VFC)
 */
void VIDEL_VFC_ReadWord(void)
{
	IoMem_WriteWord(0xff82a0, vfc_counter);
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff82a0 Vertical Frequency Counter (VFC) read: 0x%04x\n", vfc_counter);
}

/**
 *  Write Vertical Frequency Timer (VFT)
 */
void VIDEL_VFT_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff82a2 Vertical Frequency Timer (VFT) write: 0x%04x\n",
	          IoMem_ReadWord(0xff82a2));
}

/**
 *  Write Vertical Border Begin (VBB)
 */
void VIDEL_VBB_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff82a4 Vertical Border Begin (VBB) write: 0x%04x\n",
	          IoMem_ReadWord(0xff82a4));
}

/**
 *  Write Vertical Border End (VBE)
 */
void VIDEL_VBE_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff82a6 Vertical Border End (VBE) write: 0x%04x\n",
	          IoMem_ReadWord(0xff82a6));
}

/**
 *  Write Vertical Display Begin (VDB)
 */
void VIDEL_VDB_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff82a8 Vertical Display Begin (VDB) write: 0x%04x\n",
	          IoMem_ReadWord(0xff82a8));
}

/**
 *  Write Vertical Display End (VDE)
 */
void VIDEL_VDE_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff82aa Vertical Display End (VDE) write: 0x%04x\n",
	          IoMem_ReadWord(0xff82aa));
}

/**
 *  Write Vertical SS (VSS)
 */
void VIDEL_VSS_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff82ac Vertical SS (VSS) write: 0x%04x\n",
	          IoMem_ReadWord(0xff82ac));
}

/**
 *  Write Video Control (VCO)
 */
void VIDEL_VCO_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff82c0 Video control (VCO) write: 0x%04x\n",
	          IoMem_ReadWord(0xff82c0));
}

/**
 *  Write Video Mode (VDM)
 */
void VIDEL_VMD_WriteWord(void)
{
	LOG_TRACE(TRACE_VIDEL, "Videl : $ff82c2 Video Mode (VDM) write: 0x%04x\n",
	          IoMem_ReadWord(0xff82c2));
}


/**
 *  VIDEL_getVideoramAddress: returns the video RAM address.
 *  On Falcon, video address must be a multiple of four in bitplane modes.
 */
static Uint32 VIDEL_getVideoramAddress(void)
{
	Uint32 videoBase;
	
	videoBase  = (Uint32) IoMem_ReadByte(0xff8201) << 16;
	videoBase |= (Uint32) IoMem_ReadByte(0xff8203) << 8;
	videoBase |= IoMem_ReadByte(0xff820d) & ~3;
	
	return videoBase;
}

static Uint16 VIDEL_getScreenBpp(void)
{
	Uint16 f_shift = IoMem_ReadWord(0xff8266);
	Uint16 bits_per_pixel;
	Uint8  st_shift = IoMem_ReadByte(0xff8260);

	/* to get bpp, we must examine f_shift and st_shift.
	 * f_shift is valid if any of bits no. 10, 8 or 4 is set. 
	 * Priority in f_shift is: 10 ">" 8 ">" 4, i.e.
	 * if bit 10 set then bit 8 and bit 4 don't care...
	 * If all these bits are 0 and ST shifter is written
	 * after Falcon one, get display depth from st_shift
	 * (as for ST and STE)
	 */
	if (f_shift & 0x400)		/* Falcon: 2 colors */
		bits_per_pixel = 1;
	else if (f_shift & 0x100)	/* Falcon: hicolor */
		bits_per_pixel = 16;
	else if (f_shift & 0x010)	/* Falcon: 8 bitplanes */
		bits_per_pixel = 8;
	else if (!videl.bUseSTShifter)	/* Falcon: 4 bitplanes */
		bits_per_pixel = 4;
	else if (st_shift == 0)
		bits_per_pixel = 4;
	else if (st_shift == 0x01)
		bits_per_pixel = 2;
	else /* if (st_shift == 0x02) */
		bits_per_pixel = 1;

/*	LOG_TRACE(TRACE_VIDEL, "Videl works in %d bpp, f_shift=%04x, st_shift=%d", bits_per_pixel, f_shift, st_shift); */

	return bits_per_pixel;
}

/**
 *  VIDEL_getScreenWidth : returns the visible X resolution
 *	left border + graphic area + right border
 *	left border  : hdb - hbe-offset
 *	right border : hbb - hde-offset
 *	Graphics display : starts at cycle HDB and ends at cycle HDE. 
 */
static int VIDEL_getScreenWidth(void)
{
	Uint16 hbb, hbe, hdb, hde, vdm, hht;
	Uint16 cycPerPixel, divider;
	Sint16 hdb_offset, hde_offset;
	Sint16 leftBorder, rightBorder;
	Uint16 bpp = VIDEL_getScreenBpp();

	/* X Size of the Display area */
	videl.XSize = (IoMem_ReadWord(0xff8210) & 0x03ff) * 16 / bpp;

	/* If the user disabled the borders display from the gui, we suppress them */
	if (ConfigureParams.Screen.bAllowOverscan == 0) {
		videl.leftBorderSize = 0;
		videl.rightBorderSize = 0;
		return videl.XSize;
	}

	/* According to Aura and Animal Mine doc about Videl, if a monochrome monitor is connected,
	 * HDB and HDE have no significance and no border is displayed.
	 */
	if (videl.monitor_type == FALCON_MONITOR_MONO) {
		videl.leftBorderSize = 0;
		videl.rightBorderSize = 0;
		return videl.XSize;
	}

	hbb = IoMem_ReadWord(0xff8284) & 0x01ff;
	hbe = IoMem_ReadWord(0xff8286) & 0x01ff;
	hdb = IoMem_ReadWord(0xff8288) & 0x01ff;
	hde = IoMem_ReadWord(0xff828a) & 0x01ff;
	vdm = IoMem_ReadWord(0xff82c2) & 0xc;
	hht = IoMem_ReadWord(0xff8282) & 0x1ff;

	/* Compute cycles per pixel */
	if (vdm == 0)
		cycPerPixel = 4;
	else if (vdm == 4)
		cycPerPixel = 2;
	else
		cycPerPixel = 1;

	/* Compute the divider */
	if (videl.monitor_type == FALCON_MONITOR_VGA) {
		if (cycPerPixel == 4)
			divider = 4;
		else
			divider = 2;
	}
	else if (videl.bUseSTShifter == true) {
		divider = 16;
	}
	else {
		divider = cycPerPixel;
	}

	/* Compute hdb_offset and hde_offset */
	if (videl.bUseSTShifter == false) {
		if (bpp < 16) {
			/* falcon mode bpp */
			hdb_offset = ((64+(128/bpp + 16 + 2) * cycPerPixel) / divider ) + 1;
			hde_offset = ((128/bpp + 2) * cycPerPixel) / divider;
		}
		else {
			/* falcon mode true color */
			hdb_offset = ((64 + 16 * cycPerPixel) / divider ) + 1;
			hde_offset = 0;
		}
	}
	else {
		/* ST bitplan mode */
		hdb_offset = ((128+(128/bpp + 2) * cycPerPixel) / divider ) + 1;
		hde_offset = ((128/bpp + 2) * cycPerPixel) / divider;
	}

	LOG_TRACE(TRACE_VIDEL, "hdb_offset=%04x,    hde_offset=%04x\n", hdb_offset, hde_offset);

	/* Compute left border size in cycles */
	if (IoMem_ReadWord(0xff8288) & 0x0200)
		leftBorder = hdb - hbe + hdb_offset - hht - 2;
	else
		leftBorder = hdb - hbe + hdb_offset;

	/* Compute right border size in cycles */
	rightBorder = hbb - hde_offset - hde;

	videl.leftBorderSize = leftBorder / cycPerPixel;
	videl.rightBorderSize = rightBorder / cycPerPixel;
	LOG_TRACE(TRACE_VIDEL, "left border size=%04x,    right border size=%04x\n", videl.leftBorderSize, videl.rightBorderSize);

	if (videl.leftBorderSize < 0) {
//		fprintf(stderr, "BORDER LEFT < 0   %d\n", videl.leftBorderSize);
		videl.leftBorderSize = 0;
	}
	if (videl.rightBorderSize < 0) {
//		fprintf(stderr, "BORDER RIGHT < 0   %d\n", videl.rightBorderSize);
		videl.rightBorderSize = 0;
	}

	return videl.leftBorderSize + videl.XSize + videl.rightBorderSize;
}

/**
 *  VIDEL_getScreenHeight : returns the visible Y resolution
 *	upper border + graphic area + lower border
 *	upper border : vdb - vbe
 *	lower border : vbb - vde
 *	Graphics display : starts at line VDB and ends at line VDE. 
 *	If interlace mode off unit of VC-registers is half lines, else lines.
 */
static int VIDEL_getScreenHeight(void)
{
	Uint16 vbb = IoMem_ReadWord(0xff82a4) & 0x07ff;
	Uint16 vbe = IoMem_ReadWord(0xff82a6) & 0x07ff;  
	Uint16 vdb = IoMem_ReadWord(0xff82a8) & 0x07ff;
	Uint16 vde = IoMem_ReadWord(0xff82aa) & 0x07ff;
	Uint16 vmode = IoMem_ReadWord(0xff82c2);

	/* According to Aura and Animal Mine doc about Videl, if a monochrome monitor is connected,
	 * VDB and VDE have no significance and no border is displayed.
	 */
	if (videl.monitor_type == FALCON_MONITOR_MONO) {
		videl.upperBorderSize = 0;
		videl.lowerBorderSize = 0;
	}
	else {
		/* We must take the positive value only, as a program like AceTracker starts the */
		/* graphical area 1 line before the end of the upper border */
		videl.upperBorderSize = vdb - vbe > 0 ? vdb - vbe : 0;
		videl.lowerBorderSize = vbb - vde > 0 ? vbb - vde : 0;
	}

	/* Y Size of the Display area */
	if (vde >= vdb) {
		videl.YSize = vde - vdb;
	}
	else {
		LOG_TRACE(TRACE_VIDEL, "WARNING: vde=0x%x is less than vdb=0x%x\n",
		          vde, vdb);
	}

	/* If the user disabled the borders display from the gui, we suppress them */
	if (ConfigureParams.Screen.bAllowOverscan == 0) {
		videl.upperBorderSize = 0;
		videl.lowerBorderSize = 0;
	}

	if (!(vmode & 0x02)){		/* interlace */
		videl.YSize >>= 1;
		videl.upperBorderSize >>= 1;
		videl.lowerBorderSize >>= 1;
	}
	
	if (vmode & 0x01) {		/* double */
		videl.YSize >>= 1;
		videl.upperBorderSize >>= 1;
		videl.lowerBorderSize >>= 1;
	}

	return videl.upperBorderSize + videl.YSize + videl.lowerBorderSize;
}

#if 0
/* this is easier & more robustly done in hostscreen.c just by
 * comparing requested screen width & height to each other.
 */
static void VIDEL_getMonitorScale(int *sx, int *sy)
{
	/* Videl video mode register bits and resulting desktop resolution:
	 * 
	 * quarter, half, interlace, double:   pixels: -> zoom:
	 * rgb:
	 *    0       0       0         0      320x200 -> 2 x 2
	 *    0       0       1         0      320x400 -> 2 x 1
	 *    0       1       0         0      640x200 -> 1 x 2 !
	 *    0       1       1         0      640x400 -> 1 x 1
	 * vga:
	 *    0       0       0         1      (just double ?)
	 *    0       0       1         1      (double & interlace ???)
	 *    0       1       0         0      320x480 -> 2 x 1 !
	 *    0       1       0         1      320x240 -> 2 x 2
	 *    0       1       1         1      (double + interlace ???)
	 *    1       0       0         0      640x480 -> 1 x 1
	 *    1       0       0         1      640x240 -> 1 x 2
	 */
	int vmode = IoMem_ReadWord(0xff82c2);

	/* half pixel seems to have opposite meaning on
	 * VGA and RGB monitor, so they need to handled separately
	 */
	if (videl.monitor_type) == FALCON_MONITOR_VGA) {
		if (vmode & 0x08) {  /* quarter pixel */
			*sx = 1;
		} else {
			*sx = 2;
		}
		if (vmode & 0x01) {  /* double line */
			*sy = 2;
		} else {
			*sy = 1;
		}
	} else {
		if (vmode & 0x04) {  /* half pixel */
			*sx = 1;
		} else {
			*sx = 2;
		}
		if (vmode & 0x02) {  /* interlace used only on RGB ? */
			*sy = 1;
		} else {
			*sy = 2;
		}
	}
}
#endif


/** map the correct colortable into the correct pixel format
 */
static void VIDEL_updateColors(void)
{
	int i, r, g, b, colors = 1 << videl.save_scrBpp;

#define F_COLORS(i) IoMem_ReadByte(VIDEL_COLOR_REGS_BEGIN + (i))
#define STE_COLORS(i)	IoMem_ReadByte(0xff8240 + (i))

	if (!videl.bUseSTShifter) {
		for (i = 0; i < colors; i++) {
			int offset = i << 2;
			r = F_COLORS(offset) & 0xfc;
			r |= r>>6;
			g = F_COLORS(offset + 1) & 0xfc;
			g |= g>>6;
			b = F_COLORS(offset + 3) & 0xfc;
			b |= b>>6;
			HostScreen_setPaletteColor(i, r,g,b);
		}
		HostScreen_updatePalette(colors);
	} else {
		for (i = 0; i < colors; i++) {
			int offset = i << 1;
			r = STE_COLORS(offset) & 0x0f;
			r = ((r & 7)<<1)|(r>>3);
			r |= r<<4;
			g = (STE_COLORS(offset + 1)>>4) & 0x0f;
			g = ((g & 7)<<1)|(g>>3);
			g |= g<<4;
			b = STE_COLORS(offset + 1) & 0x0f;
			b = ((b & 7)<<1)|(b>>3);
			b |= b<<4;
			HostScreen_setPaletteColor(i, r,g,b);
		}
		HostScreen_updatePalette(colors);
	}

	videl.hostColorsSync = true;
}


void VIDEL_ZoomModeChanged(void)
{
	/* User selected another zoom mode, so set a new screen resolution now */
	HostScreen_setWindowSize(videl.save_scrWidth, videl.save_scrHeight, videl.save_scrBpp == 16 ? 16 : ConfigureParams.Screen.nForceBpp);
}


bool VIDEL_renderScreen(void)
{
	/* Atari screen infos */
	int vw	 = VIDEL_getScreenWidth();
	int vh	 = VIDEL_getScreenHeight();
	int vbpp = VIDEL_getScreenBpp();

	int lineoffset = IoMem_ReadWord(0xff820e) & 0x01ff; /* 9 bits */
	int linewidth = IoMem_ReadWord(0xff8210) & 0x03ff;  /* 10 bits */
	int nextline;

	bool change = false;

	videl.videoBaseAddr = VIDEL_getVideoramAddress(); // Todo: to be removed when all code is in Videl

	if (vw > 0 && vw != videl.save_scrWidth) {
		LOG_TRACE(TRACE_VIDEL, "Videl : width change from %d to %d\n", videl.save_scrWidth, vw);
		videl.save_scrWidth = vw;
		change = true;
	}
	if (vh > 0 && vh != videl.save_scrHeight) {
		LOG_TRACE(TRACE_VIDEL, "Videl : height change from %d to %d\n", videl.save_scrHeight, vh);
		videl.save_scrHeight = vh;
		change = true;
	}
	if (vbpp != videl.save_scrBpp) {
		LOG_TRACE(TRACE_VIDEL, "Videl : bpp change from %d to %d\n", videl.save_scrBpp, vbpp);
		videl.save_scrBpp = vbpp;
		change = true;
	}
	if (change) {
		LOG_TRACE(TRACE_VIDEL, "Videl : video mode change to %dx%d@%d\n", videl.save_scrWidth, videl.save_scrHeight, videl.save_scrBpp);
		HostScreen_setWindowSize(videl.save_scrWidth, videl.save_scrHeight, videl.save_scrBpp == 16 ? 16 : ConfigureParams.Screen.nForceBpp);
	}

	if (!HostScreen_renderBegin())
		return false;

	/* 
	   I think this implementation is naive: 
	   indeed, I suspect that we should instead skip lineoffset
	   words each time we have read "more" than linewidth words
	   (possibly "more" because of the number of bit planes).
	   Moreover, the 1 bit plane mode is particular;
	   while doing some experiments on my Falcon, it seems to
	   behave like the 4 bit planes mode.
	   At last, we have also to take into account the 4 bits register
	   located at the word $ffff8264 (bit offset). This register makes
	   the semantics of the lineoffset register change a little.
	   int bitoffset = IoMem_ReadWord(0xff8264) & 0x000f;
	   The meaning of this register in True Color mode is not clear
	   for me at the moment (and my experiments on the Falcon don't help
	   me).
	*/
	nextline = linewidth + lineoffset;

	if ((vw<32) || (vh<32))
		return false;

	if (videl.save_scrBpp < 16 && videl.hostColorsSync == 0)
		VIDEL_updateColors();

	if (nScreenZoomX * nScreenZoomY != 1) {
		VIDEL_ConvertScreenZoom(vw, vh, videl.save_scrBpp, nextline);
	} else {
		VIDEL_ConvertScreenNoZoom(vw, vh, videl.save_scrBpp, nextline);
	}

	HostScreen_update1(HostScreen_renderEnd(), false);

	return true;
}


/**
 * Performs conversion from the TOS's bitplane word order (big endian) data
 * into the native chunky color index.
 */
static void VIDEL_bitplaneToChunky(Uint16 *atariBitplaneData, Uint16 bpp,
                                   Uint8 colorValues[16])
{
	Uint32 a, b, c, d, x;

	/* Obviously the different cases can be broken out in various
	 * ways to lessen the amount of work needed for <8 bit modes.
	 * It's doubtful if the usage of those modes warrants it, though.
	 * The branches below should be ~100% correctly predicted and
	 * thus be more or less for free.
	 * Getting the palette values inline does not seem to help
	 * enough to worry about. The palette lookup is much slower than
	 * this code, though, so it would be nice to do something about it.
	 */
	if (bpp >= 4) {
		d = *(Uint32 *)&atariBitplaneData[0];
		c = *(Uint32 *)&atariBitplaneData[2];
		if (bpp == 4) {
			a = b = 0;
		} else {
			b = *(Uint32 *)&atariBitplaneData[4];
			a = *(Uint32 *)&atariBitplaneData[6];
		}
	} else {
		a = b = c = 0;
		if (bpp == 2) {
			d = *(Uint32 *)&atariBitplaneData[0];
		} else {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
			d = atariBitplaneData[0]<<16;
#else
			d = atariBitplaneData[0];
#endif
		}
	}

	x = a;
	a =  (a & 0xf0f0f0f0)       | ((c & 0xf0f0f0f0) >> 4);
	c = ((x & 0x0f0f0f0f) << 4) |  (c & 0x0f0f0f0f);
	x = b;
	b =  (b & 0xf0f0f0f0)       | ((d & 0xf0f0f0f0) >> 4);
	d = ((x & 0x0f0f0f0f) << 4) |  (d & 0x0f0f0f0f);

	x = a;
	a =  (a & 0xcccccccc)       | ((b & 0xcccccccc) >> 2);
	b = ((x & 0x33333333) << 2) |  (b & 0x33333333);
	x = c;
	c =  (c & 0xcccccccc)       | ((d & 0xcccccccc) >> 2);
	d = ((x & 0x33333333) << 2) |  (d & 0x33333333);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	a = (a & 0x5555aaaa) | ((a & 0x00005555) << 17) | ((a & 0xaaaa0000) >> 17);
	b = (b & 0x5555aaaa) | ((b & 0x00005555) << 17) | ((b & 0xaaaa0000) >> 17);
	c = (c & 0x5555aaaa) | ((c & 0x00005555) << 17) | ((c & 0xaaaa0000) >> 17);
	d = (d & 0x5555aaaa) | ((d & 0x00005555) << 17) | ((d & 0xaaaa0000) >> 17);

	colorValues[ 8] = a;
	a >>= 8;
	colorValues[ 0] = a;
	a >>= 8;
	colorValues[ 9] = a;
	a >>= 8;
	colorValues[ 1] = a;

	colorValues[10] = b;
	b >>= 8;
	colorValues[ 2] = b;
	b >>= 8;
	colorValues[11] = b;
	b >>= 8;
	colorValues[ 3] = b;

	colorValues[12] = c;
	c >>= 8;
	colorValues[ 4] = c;
	c >>= 8;
	colorValues[13] = c;
	c >>= 8;
	colorValues[ 5] = c;

	colorValues[14] = d;
	d >>= 8;
	colorValues[ 6] = d;
	d >>= 8;
	colorValues[15] = d;
	d >>= 8;
	colorValues[ 7] = d;
#else
	a = (a & 0xaaaa5555) | ((a & 0x0000aaaa) << 15) | ((a & 0x55550000) >> 15);
	b = (b & 0xaaaa5555) | ((b & 0x0000aaaa) << 15) | ((b & 0x55550000) >> 15);
	c = (c & 0xaaaa5555) | ((c & 0x0000aaaa) << 15) | ((c & 0x55550000) >> 15);
	d = (d & 0xaaaa5555) | ((d & 0x0000aaaa) << 15) | ((d & 0x55550000) >> 15);

	colorValues[ 1] = a;
	a >>= 8;
	colorValues[ 9] = a;
	a >>= 8;
	colorValues[ 0] = a;
	a >>= 8;
	colorValues[ 8] = a;

	colorValues[ 3] = b;
	b >>= 8;
	colorValues[11] = b;
	b >>= 8;
	colorValues[ 2] = b;
	b >>= 8;
	colorValues[10] = b;

	colorValues[ 5] = c;
	c >>= 8;
	colorValues[13] = c;
	c >>= 8;
	colorValues[ 4] = c;
	c >>= 8;
	colorValues[12] = c;

	colorValues[ 7] = d;
	d >>= 8;
	colorValues[15] = d;
	d >>= 8;
	colorValues[ 6] = d;
	d >>= 8;
	colorValues[14] = d;
#endif
}

void VIDEL_ConvertScreenNoZoom(int vw, int vh, int vbpp, int nextline)
{
	int scrpitch = HostScreen_getPitch();
	int h, w, j;

	Uint16 *fvram = (Uint16 *) Atari2HostAddr(videl.videoBaseAddr);
	Uint16 *fvram_line;
	Uint8 *hvram = HostScreen_getVideoramAddress();
	SDL_PixelFormat *scrfmt = HostScreen_getFormat();

	Uint16 lowBorderSize, rightBorderSize;
	int scrwidth, scrheight;
	int vw_clip, vh_clip;

	int hscrolloffset = IoMem_ReadByte(0xff8265) & 0x0f;

	/* Horizontal scroll register set? */
	if (hscrolloffset) {
		/* Yes, so we need to adjust offset to next line: */
		nextline += vbpp;
	}

	/* If emulated computer is the TT, we use the same rendering for display, but without the borders */
	if (ConfigureParams.System.nMachineType == MACHINE_TT) {
		videl.leftBorderSize = 0;
		videl.rightBorderSize = 0;
		videl.upperBorderSize = 0;
		videl.lowerBorderSize = 0;
		fvram = (Uint16 *) Atari2HostAddr(VIDEL_getVideoramAddress());
	} else {
		bTTSampleHold = false;
	}

	/* Clip to SDL_Surface dimensions */
	scrwidth = HostScreen_getWidth();
	scrheight = HostScreen_getHeight();
	vw_clip = vw;
	vh_clip = vh;
	if (vw>scrwidth) vw_clip = scrwidth;
	if (vh>scrheight) vh_clip = scrheight;	

	/* If emulated computer is the FALCON, we must take :
	 * vw = X area display size and not all the X screen with the borders into account
	 * vh = Y area display size and not all the Y screen with the borders into account
	 */
	if (ConfigureParams.System.nMachineType == MACHINE_FALCON) {
		vw = videl.XSize;
		vh = videl.YSize;
	}

	/* If there's not enough space to display the left border, just return */
	if (vw_clip < videl.leftBorderSize)
		return;
	/* If there's not enough space for the left border + the graphic area, we clip */ 
	if (vw_clip < vw + videl.leftBorderSize) {
		vw = vw_clip - videl.leftBorderSize;
		rightBorderSize = 0;
	}
	/* if there's not enough space for the left border + the graphic area + the right border, we clip the border */
	else if (vw_clip < vw + videl.leftBorderSize + videl.rightBorderSize)
		rightBorderSize = vw_clip - videl.leftBorderSize - vw;
	else
		rightBorderSize = videl.rightBorderSize;

	/* If there's not enough space to display the upper border, just return */
	if (vh_clip < videl.upperBorderSize)
		return;

	/* If there's not enough space for the upper border + the graphic area, we clip */ 
	if (vh_clip < vh + videl.upperBorderSize) {
		vh = vh_clip - videl.upperBorderSize;
		lowBorderSize = 0;
	}
	/* if there's not enough space for the upper border + the graphic area + the lower border, we clip the border */
	else if (vh_clip < vh + videl.upperBorderSize + videl.lowerBorderSize)
		lowBorderSize = vh_clip - videl.upperBorderSize - vh;
	else
		lowBorderSize = videl.lowerBorderSize;

	/* Center screen */
	hvram += ((scrheight-vh_clip)>>1)*scrpitch;
	hvram += ((scrwidth-vw_clip)>>1)*HostScreen_getBpp();

	fvram_line = fvram;
	scrwidth = videl.leftBorderSize + vw + videl.rightBorderSize;

	/* render the graphic area */
	if (vbpp < 16) {
		/* Bitplanes modes */

		/* The SDL colors blitting... */
		Uint8 color[16];

		/* FIXME: The byte swap could be done here by enrolling the loop into 2 each by 8 pixels */
		switch ( HostScreen_getBpp() ) {
			case 1:
			{
				Uint8 *hvram_line = hvram;

				/* Render the upper border */
				for (h = 0; h < videl.upperBorderSize; h++) {
					VIDEL_memset_uint8 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch;
				}

				/* Render the graphical area */
				for (h = 0; h < vh; h++) {
					Uint16 *fvram_column = fvram_line;
					Uint8 *hvram_column = hvram_line;

					/* Left border first */
					VIDEL_memset_uint8 (hvram_column, HostScreen_getPaletteColor(0), videl.leftBorderSize);
					hvram_column += videl.leftBorderSize;
				
					/* First 16 pixels */
					VIDEL_bitplaneToChunky(fvram_column, vbpp, color);
					memcpy(hvram_column, color+hscrolloffset, 16-hscrolloffset);
					hvram_column += 16-hscrolloffset;
					fvram_column += vbpp;
					/* Now the main part of the line */
					for (w = 1; w < (vw+15)>>4; w++) {
						VIDEL_bitplaneToChunky( fvram_column, vbpp, color );
						memcpy(hvram_column, color, 16);
						hvram_column += 16;
						fvram_column += vbpp;
					}
					/* Last pixels of the line for fine scrolling */
					if (hscrolloffset) {
						VIDEL_bitplaneToChunky(fvram_column, vbpp, color);
						memcpy(hvram_column, color, hscrolloffset);
					}
					/* Right border */
					VIDEL_memset_uint8 (hvram_column, HostScreen_getPaletteColor(0), rightBorderSize);

					if (bTTSampleHold) {
						Uint8 TMPPixel = 0;
						for (w=0; w < (vw); w++) {
							if (hvram_line[w] == 0) {
								hvram_line[w] = TMPPixel;
							} else {
								TMPPixel = hvram_line[w];
							}
						}
					}

					fvram_line += nextline;
					hvram_line += scrpitch;
				}

				/* Render the lower border */
				for (h = 0; h < lowBorderSize; h++) {
					VIDEL_memset_uint8 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch;
				}
			}
			break;
			case 2:
			{
				Uint16 *hvram_line = (Uint16 *)hvram;

				/* Render the upper border */
				for (h = 0; h < videl.upperBorderSize; h++) {
					VIDEL_memset_uint16 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch>>1;
				}

				/* Render the graphical area */
				for (h = 0; h < vh; h++) {
					Uint16 *fvram_column = fvram_line;
					Uint16 *hvram_column = hvram_line;

					/* Left border first */
					VIDEL_memset_uint16 (hvram_column, HostScreen_getPaletteColor(0), videl.leftBorderSize);
					hvram_column += videl.leftBorderSize;
				
					/* First 16 pixels */
					VIDEL_bitplaneToChunky(fvram_column, vbpp, color);
					for (j = 0; j < 16 - hscrolloffset; j++) {
						*hvram_column++ = HostScreen_getPaletteColor(color[j+hscrolloffset]);
					}
					fvram_column += vbpp;
					/* Now the main part of the line */
					for (w = 1; w < (vw+15)>>4; w++) {
						VIDEL_bitplaneToChunky( fvram_column, vbpp, color );
						for (j=0; j<16; j++) {
							*hvram_column++ = HostScreen_getPaletteColor( color[j] );
						}
						fvram_column += vbpp;
					}
					/* Last pixels of the line for fine scrolling */
					if (hscrolloffset) {
						VIDEL_bitplaneToChunky(fvram_column, vbpp, color);
						for (j = 0; j < hscrolloffset; j++) {
							*hvram_column++ = HostScreen_getPaletteColor(color[j]);
						}
					}
					/* Right border */
					VIDEL_memset_uint16 (hvram_column, HostScreen_getPaletteColor(0), rightBorderSize);

					fvram_line += nextline;
					hvram_line += scrpitch>>1;
				}

				/* Render the lower border */
				for (h = 0; h < lowBorderSize; h++) {
					VIDEL_memset_uint16 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch>>1;
				}
			}
			break;
			case 4:
			{
				Uint32 *hvram_line = (Uint32 *)hvram;

				/* Render the upper border */
				for (h = 0; h < videl.upperBorderSize; h++) {
					VIDEL_memset_uint32 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch>>2;
				}

				/* Render the graphical area */
				for (h = 0; h < vh; h++) {
					Uint16 *fvram_column = fvram_line;
					Uint32 *hvram_column = hvram_line;

					/* Left border first */
					VIDEL_memset_uint32 (hvram_column, HostScreen_getPaletteColor(0), videl.leftBorderSize);
					hvram_column += videl.leftBorderSize;
				
					/* First 16 pixels */
					VIDEL_bitplaneToChunky(fvram_column, vbpp, color);
					for (j = 0; j < 16 - hscrolloffset; j++) {
						*hvram_column++ = HostScreen_getPaletteColor(color[j+hscrolloffset]);
					}
					fvram_column += vbpp;
					/* Now the main part of the line */
					for (w = 1; w < (vw+15)>>4; w++) {
						VIDEL_bitplaneToChunky( fvram_column, vbpp, color );
						for (j=0; j<16; j++) {
							*hvram_column++ = HostScreen_getPaletteColor( color[j] );
						}
						fvram_column += vbpp;
					}
					/* Last pixels of the line for fine scrolling */
					if (hscrolloffset) {
						VIDEL_bitplaneToChunky(fvram_column, vbpp, color);
						for (j = 0; j < hscrolloffset; j++) {
							*hvram_column++ = HostScreen_getPaletteColor(color[j]);
						}
					}
					/* Right border */
					VIDEL_memset_uint32 (hvram_column, HostScreen_getPaletteColor(0), rightBorderSize);

					fvram_line += nextline;
					hvram_line += scrpitch>>2;
				}

				/* Render the lower border */
				for (h = 0; h < lowBorderSize; h++) {
					VIDEL_memset_uint32 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch>>2;
				}
			}
			break;
		}

	} else {

		/* Falcon TC (High Color) */
		switch ( HostScreen_getBpp() )  {
			case 1:
			{
				/* FIXME: when Videl switches to 16bpp, set the palette to 3:3:2 */
				Uint8 *hvram_line = hvram;

				/* Render the upper border */
				for (h = 0; h < videl.upperBorderSize; h++) {
					VIDEL_memset_uint8 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch;
				}

				/* Render the graphical area */
				for (h = 0; h < vh; h++) {
					Uint16 *fvram_column = fvram_line;
					Uint8 *hvram_column = hvram_line;
					int tmp;

					/* Left border first */
					VIDEL_memset_uint8 (hvram_column, HostScreen_getPaletteColor(0), videl.leftBorderSize);
					hvram_column += videl.leftBorderSize;

					/* Graphical area */
					for (w = 0; w < vw; w++) {
						tmp = SDL_SwapBE16(*fvram_column++);
						*hvram_column++ = (((tmp>>13) & 7) << 5) + (((tmp>>8) & 7) << 2) + (((tmp>>2) & 3));
					}

					/* Right border */
					VIDEL_memset_uint8 (hvram_column, HostScreen_getPaletteColor(0), rightBorderSize);

					fvram_line += nextline;
					hvram_line += scrpitch;
				}
				/* Render the bottom border */
				for (h = 0; h < lowBorderSize; h++) {
					VIDEL_memset_uint8 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch;
				}
			}
			break;
			case 2:
			{
				Uint16 *hvram_line = (Uint16 *)hvram;

				/* Render the upper border */
				for (h = 0; h < videl.upperBorderSize; h++) {
					VIDEL_memset_uint16 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch>>1;
				}

				/* Render the graphical area */
				for (h = 0; h < vh; h++) {
					Uint16 *hvram_column = hvram_line;
#if SDL_BYTEORDER != SDL_BIG_ENDIAN
					Uint16 *fvram_column;
#endif
					/* Left border first */
					VIDEL_memset_uint16 (hvram_column, HostScreen_getPaletteColor(0), videl.leftBorderSize);
					hvram_column += videl.leftBorderSize;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
					/* FIXME: here might be a runtime little/big video endian switch like:
						if ( " videocard memory in Motorola endian format " false)
					*/
					memcpy(hvram_column, fvram_line, vw<<1);
					hvram_column += vw<<1;
#else
					fvram_column = fvram_line;
					/* Graphical area */
					for (w = 0; w < vw; w++)
						*hvram_column ++ = SDL_SwapBE16(*fvram_column++);
#endif /* SDL_BYTEORDER == SDL_BIG_ENDIAN */

					/* Right border */
					VIDEL_memset_uint16 (hvram_column, HostScreen_getPaletteColor(0), rightBorderSize);

					fvram_line += nextline;
					hvram_line += scrpitch>>1;
				}

				/* Render the bottom border */
				for (h = 0; h < lowBorderSize; h++) {
					VIDEL_memset_uint16 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch>>1;
				}
			}
			break;
			case 4:
			{
				Uint32 *hvram_line = (Uint32 *)hvram;

				/* Render the upper border */
				for (h = 0; h < videl.upperBorderSize; h++) {
					VIDEL_memset_uint32 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch>>2;
				}

				/* Render the graphical area */
				for (h = 0; h < vh; h++) {
					Uint16 *fvram_column = fvram_line;
					Uint32 *hvram_column = hvram_line;

					/* Left border first */
					VIDEL_memset_uint32 (hvram_column, HostScreen_getPaletteColor(0), videl.leftBorderSize);
					hvram_column += videl.leftBorderSize;

					/* Graphical area */
					for (w = 0; w < vw; w++) {
						Uint16 srcword = *fvram_column++;
						*hvram_column ++ = SDL_MapRGB(scrfmt, (srcword & 0xf8), (((srcword & 0x07) << 5) | ((srcword >> 11) & 0x3c)), ((srcword >> 5) & 0xf8));
					}

					/* Right border */
					VIDEL_memset_uint32 (hvram_column, HostScreen_getPaletteColor(0), rightBorderSize);
				}

				fvram_line += nextline;
				hvram_line += scrpitch>>2;

				/* Render the bottom border */
				for (h = 0; h < lowBorderSize; h++) {
					VIDEL_memset_uint32 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch>>2;
				}
			}
			break;
		}
	}
}


void VIDEL_ConvertScreenZoom(int vw, int vh, int vbpp, int nextline)
{
	int i, j, w, h, cursrcline;

	Uint16 *fvram = (Uint16 *) Atari2HostAddr(videl.videoBaseAddr);
	Uint16 *fvram_line;
	Uint16 scrIdx = 0;

	int coefx = 1;
	int coefy = 1;
	int scrpitch, scrwidth, scrheight, scrbpp, hscrolloffset;
	Uint8 *hvram;
	SDL_PixelFormat *scrfmt;

	/* If emulated computer is the TT, we use the same rendering for display, but without the borders */
	if (ConfigureParams.System.nMachineType == MACHINE_TT) {
		videl.leftBorderSize = 0;
		videl.rightBorderSize = 0;
		videl.upperBorderSize = 0;
		videl.lowerBorderSize = 0;
		videl.XSize = vw;
		videl.YSize = vh;
		fvram = (Uint16 *) Atari2HostAddr(VIDEL_getVideoramAddress());
	} else {
		bTTSampleHold = false;
	}

	/* Host screen infos */
	scrpitch = HostScreen_getPitch();
	scrwidth = HostScreen_getWidth();
	scrheight = HostScreen_getHeight();
	scrbpp = HostScreen_getBpp();
	scrfmt = HostScreen_getFormat();
	hvram = (Uint8 *) HostScreen_getVideoramAddress();

	hscrolloffset = IoMem_ReadByte(0xff8265) & 0x0f;

	/* Horizontal scroll register set? */
	if (hscrolloffset) {
		/* Yes, so we need to adjust offset to next line: */
		nextline += vbpp;
	}

	/* Integer zoom coef ? */
	if (/*(bx_options.autozoom.integercoefs) &&*/ (scrwidth>=vw) && (scrheight>=vh)) {
		coefx = scrwidth/vw;
		coefy = scrheight/vh;

		scrwidth = vw * coefx;
		scrheight = vh * coefy;

		/* Center screen */
		hvram += ((HostScreen_getHeight()-scrheight)>>1)*scrpitch;
		hvram += ((HostScreen_getWidth()-scrwidth)>>1)*scrbpp;
	}

	/* New zoom ? */
	if ((videl_zoom.zoomwidth != vw) || (scrwidth != videl_zoom.prev_scrwidth)) {
		if (videl_zoom.zoomxtable) {
			free(videl_zoom.zoomxtable);
		}
		videl_zoom.zoomxtable = malloc(sizeof(int)*scrwidth);
		for (i=0; i<scrwidth; i++) {
			videl_zoom.zoomxtable[i] = (vw*i)/scrwidth;
		}
		videl_zoom.zoomwidth = vw;
		videl_zoom.prev_scrwidth = scrwidth;
	}
	if ((videl_zoom.zoomheight != vh) || (scrheight != videl_zoom.prev_scrheight)) {
		if (videl_zoom.zoomytable) {
			free(videl_zoom.zoomytable);
		}
		videl_zoom.zoomytable = malloc(sizeof(int)*scrheight);
		for (i=0; i<scrheight; i++) {
			videl_zoom.zoomytable[i] = (vh*i)/scrheight;
		}
		videl_zoom.zoomheight = vh;
		videl_zoom.prev_scrheight = scrheight;
	}

	cursrcline = -1;

	/* We reuse the following values to compute the display area size in zoom mode */
	/* scrwidth must not change */
	if (ConfigureParams.System.nMachineType == MACHINE_FALCON) {
		vw = videl.XSize;
		vh = videl.YSize;
		scrheight = vh * coefy;
	}

	if (vbpp<16) {
		Uint8 color[16];

		/* Bitplanes modes */
		switch(scrbpp) {
			case 1:
			{
				/* One complete 16-pixel aligned planar 2 chunky line */
				Uint8 *p2cline = malloc(sizeof(Uint8) * ((vw+15) & ~15));
				Uint8 *hvram_line = hvram;
				Uint8 *hvram_column = p2cline;

				/* Render the upper border */
				for (h = 0; h < videl.upperBorderSize * coefy; h++) {
					VIDEL_memset_uint8 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch;
				}

				/* Render the graphical area */
				for (h = 0; h < scrheight; h++) {

					fvram_line = fvram + (videl_zoom.zoomytable[scrIdx] * nextline);
					scrIdx ++;

					/* Recopy the same line ? */
					if (videl_zoom.zoomytable[h] == cursrcline) {
						memcpy(hvram_line, hvram_line-scrpitch, scrwidth*scrbpp);
					} else {
						Uint16 *fvram_column = fvram_line;
						hvram_column = p2cline;

						/* First 16 pixels of a new line */
						VIDEL_bitplaneToChunky(fvram_column, vbpp, color);
						memcpy(hvram_column, color+hscrolloffset, 16-hscrolloffset);
						hvram_column += 16-hscrolloffset;
						fvram_column += vbpp;
						/* Convert main part of the new line */
						for (w=1; w < (vw+15)>>4; w++) {
							VIDEL_bitplaneToChunky( fvram_column, vbpp, color );
							memcpy(hvram_column, color, 16);
							hvram_column += 16;
							fvram_column += vbpp;
						}
						/* Last pixels of the line for fine scrolling */
						if (hscrolloffset) {
							VIDEL_bitplaneToChunky(fvram_column, vbpp, color);
							memcpy(hvram_column, color, hscrolloffset);
						}

						hvram_column = hvram_line;

						/* Display the Left border */
						VIDEL_memset_uint8 (hvram_column, HostScreen_getPaletteColor(0), videl.leftBorderSize * coefx);
						hvram_column += videl.leftBorderSize * coefx;

						/* Display the Graphical area */
						for (w=0; w<(vw*coefx); w++)
							hvram_column[w] = p2cline[videl_zoom.zoomxtable[w - videl.leftBorderSize * coefx]];
						hvram_column += vw * coefx;

						/* Display the Right border */
						VIDEL_memset_uint8 (hvram_column, HostScreen_getPaletteColor(0), videl.rightBorderSize * coefx);
						hvram_column += videl.rightBorderSize * coefx;

						if (bTTSampleHold) {
							Uint8 TMPPixel = 0;
							for (w=0; w < (vw*coefx); w++) {
								if (hvram_line[w] == 0) {
									hvram_line[w] = TMPPixel;
								} else {
									TMPPixel = hvram_line[w];
								}
							}
						}

					}

					hvram_line += scrpitch;
					cursrcline = videl_zoom.zoomytable[h];
				}

				/* Render the lower border */
				for (h = 0; h < videl.lowerBorderSize * coefy; h++) {
					VIDEL_memset_uint8 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch;
				}

				free(p2cline);
			}
			break;
			case 2:
			{
				/* One complete 16-pixel aligned planar 2 chunky line */
				Uint16 *p2cline = malloc(sizeof(Uint16) * ((vw+15) & ~15));
				Uint16 *hvram_line = (Uint16 *)hvram;
				Uint16 *hvram_column = p2cline;

				/* Render the upper border */
				for (h = 0; h < videl.upperBorderSize * coefy; h++) {
					VIDEL_memset_uint16 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch>>1;
				}

				/* Render the graphical area */
				for (h = 0; h < scrheight; h++) {
					fvram_line = fvram + (videl_zoom.zoomytable[scrIdx] * nextline);
					scrIdx ++;

					/* Recopy the same line ? */
					if (videl_zoom.zoomytable[h] == cursrcline) {
						memcpy(hvram_line, hvram_line-(scrpitch>>1), scrwidth*scrbpp);
					} else {
						Uint16 *fvram_column = fvram_line;
						hvram_column = p2cline;

						/* First 16 pixels of a new line */
						VIDEL_bitplaneToChunky(fvram_column, vbpp, color);
						for (j = 0; j < 16 - hscrolloffset; j++) {
							*hvram_column++ = HostScreen_getPaletteColor(color[j+hscrolloffset]);
						}
						fvram_column += vbpp;
						/* Convert the main part of the new line */
						for (w = 1; w < (vw+15)>>4; w++) {
							VIDEL_bitplaneToChunky( fvram_column, vbpp, color );
							for (j=0; j<16; j++) {
								*hvram_column++ = HostScreen_getPaletteColor( color[j] );
							}
							fvram_column += vbpp;
						}
						/* Last pixels of the new line for fine scrolling */
						if (hscrolloffset) {
							VIDEL_bitplaneToChunky(fvram_column, vbpp, color);
							for (j = 0; j < hscrolloffset; j++) {
								*hvram_column++ = HostScreen_getPaletteColor(color[j]);
							}
						}

						hvram_column = hvram_line;

						/* Display the Left border */
						VIDEL_memset_uint16 (hvram_column, HostScreen_getPaletteColor(0), videl.leftBorderSize * coefx);
						hvram_column += videl.leftBorderSize * coefx;

						/* Display the Graphical area */
						for (w=0; w<(vw*coefx); w++)
							hvram_column[w] = p2cline[videl_zoom.zoomxtable[w]];
						hvram_column += vw * coefx;

						/* Display the Right border */
						VIDEL_memset_uint16 (hvram_column, HostScreen_getPaletteColor(0), videl.rightBorderSize * coefx);
						hvram_column += videl.rightBorderSize * coefx;
					}

					hvram_line += scrpitch>>1;
					cursrcline = videl_zoom.zoomytable[h];
				}

				/* Render the lower border */
				for (h = 0; h < videl.lowerBorderSize * coefy; h++) {
					VIDEL_memset_uint16 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch>>1;
				}

				free(p2cline);
			}
			break;
			case 4:
			{
				/* One complete 16-pixel aligned planar 2 chunky line */
				Uint32 *p2cline = malloc(sizeof(Uint32) * ((vw+15) & ~15));
				Uint32 *hvram_line = (Uint32 *)hvram;
				Uint32 *hvram_column = p2cline;

				/* Render the upper border */
				for (h = 0; h < videl.upperBorderSize * coefy; h++) {
					VIDEL_memset_uint32 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch>>2;
				}

				/* Render the graphical area */
				for (h = 0; h < scrheight; h++) {
					fvram_line = fvram + (videl_zoom.zoomytable[scrIdx] * nextline);
					scrIdx ++;
					/* Recopy the same line ? */
					if (videl_zoom.zoomytable[h] == cursrcline) {
						memcpy(hvram_line, hvram_line-(scrpitch>>2), scrwidth*scrbpp);
					} else {
						Uint16 *fvram_column = fvram_line;
						hvram_column = p2cline;

						/* First 16 pixels of a new line */
						VIDEL_bitplaneToChunky(fvram_column, vbpp, color);
						for (j = 0; j < 16 - hscrolloffset; j++) {
							*hvram_column++ = HostScreen_getPaletteColor(color[j+hscrolloffset]);
						}
						fvram_column += vbpp;
						/* Convert the main part of the new line */
						for (w = 1; w < (vw+15)>>4; w++) {
							VIDEL_bitplaneToChunky( fvram_column, vbpp, color );
							for (j=0; j<16; j++) {
								*hvram_column++ = HostScreen_getPaletteColor( color[j] );
							}
							fvram_column += vbpp;
						}
						/* Last pixels of the new line for fine scrolling */
						if (hscrolloffset) {
							VIDEL_bitplaneToChunky(fvram_column, vbpp, color);
							for (j = 0; j < hscrolloffset; j++) {
								*hvram_column++ = HostScreen_getPaletteColor(color[j]);
							}
						}

						hvram_column = hvram_line;
						/* Display the Left border */
						VIDEL_memset_uint32 (hvram_column, HostScreen_getPaletteColor(0), videl.leftBorderSize * coefx);
						hvram_column += videl.leftBorderSize * coefx;

						/* Display the Graphical area */
						for (w=0; w<(vw*coefx); w++) {
							hvram_column[w] = p2cline[videl_zoom.zoomxtable[w]];
						}
						hvram_column += vw * coefx;

						/* Display the Right border */
						VIDEL_memset_uint32 (hvram_column, HostScreen_getPaletteColor(0), videl.rightBorderSize * coefx);
						hvram_column += videl.rightBorderSize * coefx;
					}

					hvram_line += scrpitch>>2;
					cursrcline = videl_zoom.zoomytable[h];
				}

				/* Render the lower border */
				for (h = 0; h < videl.lowerBorderSize * coefy; h++) {
					VIDEL_memset_uint32 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch>>2;
				}

				free(p2cline);
			}
			break;
		}
	} else {
		/* Falcon high-color (16-bit) mode */

		switch(scrbpp) {
			case 1:
			{
				/* FIXME: when Videl switches to 16bpp, set the palette to 3:3:2 */
				Uint8 *hvram_line = hvram;
				Uint8 *hvram_column = hvram_line;

				/* Render the upper border */
				for (h = 0; h < videl.upperBorderSize * coefy; h++) {
					VIDEL_memset_uint8 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch;
				}

				/* Render the graphical area */
				for (h = 0; h < scrheight; h++) {
					Uint16 *fvram_column;

					fvram_line = fvram + (videl_zoom.zoomytable[scrIdx] * nextline);
					scrIdx ++;

					fvram_column = fvram_line;

					/* Recopy the same line ? */
					if (videl_zoom.zoomytable[h] == cursrcline) {
						memcpy(hvram_line, hvram_line-scrpitch, scrwidth*scrbpp);
					} else {

						hvram_column = hvram_line;
						/* Display the Left border */
						VIDEL_memset_uint8 (hvram_column, HostScreen_getPaletteColor(0), videl.leftBorderSize * coefx);
						hvram_column += videl.leftBorderSize * coefx;

						/* Display the Graphical area */
						for (w = 0; w<(vw*coefx); w++) {
							Uint16 srcword;
							Uint8 dstbyte;
							
							srcword = SDL_SwapBE16(fvram_column[videl_zoom.zoomxtable[w - videl.leftBorderSize * coefx]]);

							dstbyte = ((srcword>>13) & 7) << 5;
							dstbyte |= ((srcword>>8) & 7) << 2;
							dstbyte |= ((srcword>>2) & 3);
							*hvram_column++ = dstbyte;
						}

						/* Display the Right border */
						VIDEL_memset_uint8 (hvram_column, HostScreen_getPaletteColor(0), videl.rightBorderSize * coefx);
						hvram_column += videl.rightBorderSize * coefx;
					}

					hvram_line += scrpitch;
					cursrcline = videl_zoom.zoomytable[h];
				}

				/* Render the lower border */
				for (h = 0; h < videl.lowerBorderSize * coefy; h++) {
					VIDEL_memset_uint8 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch;
				}
			}
			break;
			case 2:
			{
				Uint16 *hvram_line = (Uint16 *)hvram;
				Uint16 *hvram_column = hvram_line;

				/* Render the upper border */
				for (h = 0; h < videl.upperBorderSize * coefy; h++) {
					VIDEL_memset_uint16 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch>>1;
				}

				/* Render the graphical area */
				for (h = 0; h < scrheight; h++) {
					Uint16 *fvram_column;

					fvram_line = fvram + (videl_zoom.zoomytable[scrIdx] * nextline);
					scrIdx ++;

					fvram_column = fvram_line;

					/* Recopy the same line ? */
					if (videl_zoom.zoomytable[h] == cursrcline) {
						memcpy(hvram_line, hvram_line-(scrpitch>>1), scrwidth*scrbpp);
					} else {

						hvram_column = hvram_line;

						/* Display the Left border */
						VIDEL_memset_uint16 (hvram_column, HostScreen_getPaletteColor(0), videl.leftBorderSize * coefx);
						hvram_column += videl.leftBorderSize * coefx;

						/* Display the Graphical area */
						for (w=0; w<(vw*coefx); w++)
							*hvram_column++ = SDL_SwapBE16(fvram_column[videl_zoom.zoomxtable[w]]);


						/* Display the Right border */
						VIDEL_memset_uint16 (hvram_column, HostScreen_getPaletteColor(0), videl.rightBorderSize * coefx);
						hvram_column += videl.rightBorderSize * coefx;
					}

					hvram_line += scrpitch>>1;
					cursrcline = videl_zoom.zoomytable[h];
				}

				/* Render the lower border */
				for (h = 0; h < videl.lowerBorderSize * coefy; h++) {
					VIDEL_memset_uint16 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch>>1;
				}
			}
			break;
			case 4:
			{
				Uint32 *hvram_line = (Uint32 *)hvram;
				Uint32 *hvram_column = hvram_line;

				/* Render the upper border */
				for (h = 0; h < videl.upperBorderSize * coefy; h++) {
					VIDEL_memset_uint32 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
				}

				/* Render the graphical area */
				for (h = 0; h < scrheight; h++) {
					Uint16 *fvram_column;

					fvram_line = fvram + (videl_zoom.zoomytable[scrIdx] * nextline);
					scrIdx ++;
					fvram_column = fvram_line;

					/* Recopy the same line ? */
					if (videl_zoom.zoomytable[h] == cursrcline) {
						memcpy(hvram_line, hvram_line-(scrpitch>>2), scrwidth*scrbpp);
						hvram_line += scrpitch>>2;
					} else {

						hvram_column = hvram_line;

						/* Display the Left border */
						VIDEL_memset_uint32 (hvram_column, HostScreen_getPaletteColor(0), videl.leftBorderSize * coefx);
						hvram_column += videl.leftBorderSize * coefx;

						/* Display the Graphical area */
						for (w = 0; w<(vw*coefx); w++) {
							Uint16 srcword;

							srcword = fvram_column[videl_zoom.zoomxtable[w]];
							*hvram_column++ = SDL_MapRGB(scrfmt, (srcword & 0xf8), (((srcword & 0x07) << 5) | ((srcword >> 11) & 0x3c)), ((srcword >> 5) & 0xf8));
						}

						/* Display the Right border */
						VIDEL_memset_uint32 (hvram_column, HostScreen_getPaletteColor(0), videl.rightBorderSize * coefx);
						hvram_column += videl.rightBorderSize * coefx;
					}

					hvram_line += scrpitch>>2;
					cursrcline = videl_zoom.zoomytable[h];
				}

				/* Render the lower border */
				for (h = 0; h < videl.lowerBorderSize * coefy; h++) {
					VIDEL_memset_uint32 (hvram_line, HostScreen_getPaletteColor(0), scrwidth);
					hvram_line += scrpitch>>2;
				}
			}
			break;
		}
	}
}

static void VIDEL_memset_uint32(Uint32 *addr, Uint32 color, int count)
{
	while (count-- > 0) {
		*addr++ = color;
	}
}

static void VIDEL_memset_uint16(Uint16 *addr, Uint16 color, int count)
{
	while (count-- > 0) {
		*addr++ = color;
	}
}

static void VIDEL_memset_uint8(Uint8 *addr, Uint8 color, int count)
{
	memset(addr, color, count);
}



/**
 * Write to videl ST palette registers (0xff8240-0xff825e)
 *
 * [Laurent]: The following note should be verified on Falcon before being applied.
 * 
 * Note that there's a special "strange" case when writing only to the upper byte
 * of the color reg (instead of writing 16 bits at once with .W/.L).
 * In that case, the byte written to address x is automatically written
 * to address x+1 too (but we shouldn't copy x in x+1 after masking x ; we apply the mask at the end)
 * Similarly, when writing a byte to address x+1, it's also written to address x
 * So :	move.w #0,$ff8240	-> color 0 is now $000
 *	move.b #7,$ff8240	-> color 0 is now $707 !
 *	move.b #$55,$ff8241	-> color 0 is now $555 !
 *	move.b #$71,$ff8240	-> color 0 is now $171 (bytes are first copied, then masked)
 */
void Videl_ColorReg_WriteWord(void)
{
	Uint16 col;
	Uint32 addr = IoAccessCurrentAddress;

	videl.hostColorsSync = false;

	if (bUseHighRes || bUseVDIRes)               /* Don't store if hi-res or VDI resolution */
		return;

	/* Note from laurent: The following special case should be verified on the real Falcon before uncommenting */
	/* Handle special case when writing only to the upper byte of the color reg */
//	if ( ( nIoMemAccessSize == SIZE_BYTE ) && ( ( IoAccessCurrentAddress & 1 ) == 0 ) )
//		col = ( IoMem_ReadByte(addr) << 8 ) + IoMem_ReadByte(addr);		/* copy upper byte into lower byte */
	/* Same when writing only to the lower byte of the color reg */
//	else if ( ( nIoMemAccessSize == SIZE_BYTE ) && ( ( IoAccessCurrentAddress & 1 ) == 1 ) )
//		col = ( IoMem_ReadByte(addr) << 8 ) + IoMem_ReadByte(addr);		/* copy lower byte into upper byte */
	/* Usual case, writing a word or a long (2 words) */
//	else
		col = IoMem_ReadWord(addr);

	col &= 0xfff;				/* Mask off to 4096 palette */

	addr &= 0xfffffffe;			/* Ensure addr is even to store the 16 bit color */

	IoMem_WriteWord(addr, col);
}

/*
 * [NP] TODO : due to how .L accesses are handled in ioMem.c, we can't call directly
 * Video_ColorReg_WriteWord from ioMemTabFalcon.c, we must use an intermediate
 * function, else .L accesses will not change 2 .W color regs, but only one.
 * This should be changed in ioMem.c to do 2 separate .W accesses, as would do a real 68000
 */

void Videl_Color0_WriteWord(void)
{
	Videl_ColorReg_WriteWord();
}

void Videl_Color1_WriteWord(void)
{
	Videl_ColorReg_WriteWord();
}

void Videl_Color2_WriteWord(void)
{
	Videl_ColorReg_WriteWord();
}

void Videl_Color3_WriteWord(void)
{
	Videl_ColorReg_WriteWord();
}

void Videl_Color4_WriteWord(void)
{
	Videl_ColorReg_WriteWord();
}

void Videl_Color5_WriteWord(void)
{
	Videl_ColorReg_WriteWord();
}

void Videl_Color6_WriteWord(void)
{
	Videl_ColorReg_WriteWord();
}

void Videl_Color7_WriteWord(void)
{
	Videl_ColorReg_WriteWord();
}

void Videl_Color8_WriteWord(void)
{
	Videl_ColorReg_WriteWord();
}

void Videl_Color9_WriteWord(void)
{
	Videl_ColorReg_WriteWord();
}

void Videl_Color10_WriteWord(void)
{
	Videl_ColorReg_WriteWord();
}

void Videl_Color11_WriteWord(void)
{
	Videl_ColorReg_WriteWord();
}

void Videl_Color12_WriteWord(void)
{
	Videl_ColorReg_WriteWord();
}

void Videl_Color13_WriteWord(void)
{
	Videl_ColorReg_WriteWord();
}

void Videl_Color14_WriteWord(void)
{
	Videl_ColorReg_WriteWord();
}

void Videl_Color15_WriteWord(void)
{
	Videl_ColorReg_WriteWord();
}

/**
 * display Videl registers values (for debugger info command)
 */
void Videl_Info(Uint32 dummy)
{
	if (ConfigureParams.System.nMachineType != MACHINE_FALCON) {
		fprintf(stderr, "Not Falcon - no Videl!\n");
		return;
	}

	fprintf(stderr, "$FF8006.b : monitor type                     : %02x\n", IoMem_ReadByte(0xff8006));
	fprintf(stderr, "$FF8201.b : Video Base Hi                    : %02x\n", IoMem_ReadByte(0xff8201));
	fprintf(stderr, "$FF8203.b : Video Base Mi                    : %02x\n", IoMem_ReadByte(0xff8203));
	fprintf(stderr, "$FF8205.b : Video Count Hi                   : %02x\n", IoMem_ReadByte(0xff8205));
	fprintf(stderr, "$FF8207.b : Video Count Mi                   : %02x\n", IoMem_ReadByte(0xff8207));
	fprintf(stderr, "$FF8209.b : Video Count Lo                   : %02x\n", IoMem_ReadByte(0xff8209));
	fprintf(stderr, "$FF820A.b : Sync mode                        : %02x\n", IoMem_ReadByte(0xff820a));
	fprintf(stderr, "$FF820D.b : Video Base Lo                    : %02x\n", IoMem_ReadByte(0xff820d));
	fprintf(stderr, "$FF820E.w : offset to next line              : %04x\n", IoMem_ReadWord(0xff820e));
	fprintf(stderr, "$FF8210.w : VWRAP - line width               : %04x\n", IoMem_ReadWord(0xff8210));
	fprintf(stderr, "$FF8260.b : ST shift mode                    : %02x\n", IoMem_ReadByte(0xff8260));
	fprintf(stderr, "$FF8264.w : Horizontal scroll register       : %04x\n", IoMem_ReadWord(0xff8264));
	fprintf(stderr, "$FF8266.w : Falcon shift mode                : %04x\n", IoMem_ReadWord(0xff8266));
	fprintf(stderr, "\n");
	fprintf(stderr, "$FF8280.w : HHC - Horizontal Hold Counter    : %04x\n", IoMem_ReadWord(0xff8280));
	fprintf(stderr, "$FF8282.w : HHT - Horizontal Hold Timer      : %04x\n", IoMem_ReadWord(0xff8282));
	fprintf(stderr, "$FF8284.w : HBB - Horizontal Border Begin    : %04x\n", IoMem_ReadWord(0xff8284));
	fprintf(stderr, "$FF8286.w : HBE - Horizontal Border End      : %04x\n", IoMem_ReadWord(0xff8286));
	fprintf(stderr, "$FF8288.w : HDB - Horizontal Display Begin   : %04x\n", IoMem_ReadWord(0xff8288));
	fprintf(stderr, "$FF828A.w : HDE - Horizontal Display End     : %04x\n", IoMem_ReadWord(0xff828a));
	fprintf(stderr, "$FF828C.w : HSS - Horizontal SS              : %04x\n", IoMem_ReadWord(0xff828c));
	fprintf(stderr, "$FF828E.w : HFS - Horizontal FS              : %04x\n", IoMem_ReadWord(0xff828e));
	fprintf(stderr, "$FF8290.w : HEE - Horizontal EE              : %04x\n", IoMem_ReadWord(0xff8290));
	fprintf(stderr, "\n");
	fprintf(stderr, "$FF82A0.w : VFC - Vertical Frequency Counter : %04x\n", IoMem_ReadWord(0xff82a0));
	fprintf(stderr, "$FF82A2.w : VFT - Vertical Frequency Timer   : %04x\n", IoMem_ReadWord(0xff82a2));
	fprintf(stderr, "$FF82A4.w : VBB - Vertical Border Begin      : %04x\n", IoMem_ReadWord(0xff82a4));
	fprintf(stderr, "$FF82A6.w : VBE - Vertical Border End        : %04x\n", IoMem_ReadWord(0xff82a6));
	fprintf(stderr, "$FF82A8.w : VDB - Vertical Display Begin     : %04x\n", IoMem_ReadWord(0xff82a8));
	fprintf(stderr, "$FF82AA.w : VDE - Vertical Display End       : %04x\n", IoMem_ReadWord(0xff82aa));
	fprintf(stderr, "$FF82AC.w : VSS - Vertical SS                : %04x\n", IoMem_ReadWord(0xff82ac));
	fprintf(stderr, "\n");
	fprintf(stderr, "$FF82C0.w : VCO - Video control              : %04x\n", IoMem_ReadWord(0xff82c0));
	fprintf(stderr, "$FF82C2.w : VMD - Video mode                 : %04x\n", IoMem_ReadWord(0xff82c2));
	fprintf(stderr, "\n-------------------------\n");

	fprintf(stderr, "Video base  : %08x\n",
		(IoMem_ReadByte(0xff8201)<<16) + 
		(IoMem_ReadByte(0xff8203)<<8)  + 
		 IoMem_ReadByte(0xff820d));
	fprintf(stderr, "Video count : %08x\n",
		(IoMem_ReadByte(0xff8205)<<16) + 
		(IoMem_ReadByte(0xff8207)<<8)  + 
		 IoMem_ReadByte(0xff8209));
}
