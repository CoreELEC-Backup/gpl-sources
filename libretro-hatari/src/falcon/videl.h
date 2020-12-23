/*
  Hatari - videl.h

  This file is distributed under the GNU General Public License, version 2
  or at your option any later version. Read the file gpl.txt for details.
*/

#ifndef HATARI_VIDEL_H
#define HATARI_VIDEL_H

/* To be removed when Videl emulation is complete */
extern Uint16 vfc_counter;			/* counter for VFC register $ff82a0 */

extern int nFrameSkips;

extern bool VIDEL_renderScreen(void);

extern void VIDEL_reset(void);

extern void VIDEL_ZoomModeChanged(void);
extern void VIDEL_ConvertScreenNoZoom(int vw, int vh, int bpp, int nextline);
extern void VIDEL_ConvertScreenZoom(int vw, int vh, int bpp, int nextline);

/* Called from ioMemTabFalcon.c */
extern void VIDEL_Monitor_WriteByte(void);
extern void VIDEL_SyncMode_WriteByte(void);
extern void VIDEL_ScreenBase_WriteByte(void);
extern void VIDEL_ScreenCounter_ReadByte(void);
extern void VIDEL_ScreenCounter_WriteByte(void);
extern void VIDEL_StColorRegsWrite(void);
extern void VIDEL_FalconColorRegsWrite(void);
extern void VIDEL_LineOffset_WriteWord(void);
extern void VIDEL_Line_Width_WriteWord(void);
extern void VIDEL_HorScroll64_WriteByte(void);
extern void VIDEL_HorScroll65_WriteByte(void);
extern void VIDEL_ST_ShiftModeWriteByte(void);
extern void VIDEL_Falcon_ShiftMode_WriteWord(void);
extern void VIDEL_HHC_WriteWord(void);
extern void VIDEL_HHT_WriteWord(void);
extern void VIDEL_HBB_WriteWord(void);
extern void VIDEL_HBE_WriteWord(void);
extern void VIDEL_HDB_WriteWord(void);
extern void VIDEL_HDE_WriteWord(void);
extern void VIDEL_HSS_WriteWord(void);
extern void VIDEL_HFS_WriteWord(void);
extern void VIDEL_HEE_WriteWord(void);
extern void VIDEL_VFC_ReadWord(void);
extern void VIDEL_VFT_WriteWord(void);
extern void VIDEL_VBB_WriteWord(void);
extern void VIDEL_VBE_WriteWord(void);
extern void VIDEL_VDB_WriteWord(void);
extern void VIDEL_VDE_WriteWord(void);
extern void VIDEL_VSS_WriteWord(void);
extern void VIDEL_VCO_WriteWord(void);
extern void VIDEL_VMD_WriteWord(void);

extern void Videl_Color0_WriteWord(void);
extern void Videl_Color1_WriteWord(void);
extern void Videl_Color2_WriteWord(void);
extern void Videl_Color3_WriteWord(void);
extern void Videl_Color4_WriteWord(void);
extern void Videl_Color5_WriteWord(void);
extern void Videl_Color6_WriteWord(void);
extern void Videl_Color7_WriteWord(void);
extern void Videl_Color8_WriteWord(void);
extern void Videl_Color9_WriteWord(void);
extern void Videl_Color10_WriteWord(void);
extern void Videl_Color11_WriteWord(void);
extern void Videl_Color12_WriteWord(void);
extern void Videl_Color13_WriteWord(void);
extern void Videl_Color14_WriteWord(void);
extern void Videl_Color15_WriteWord(void);


/* Called from cycint.c */
extern void VIDEL_InterruptHandler_HalfLine(void);

/* Called from memorySnapShot.c */
extern void VIDEL_MemorySnapShot_Capture(bool bSave);

extern void Videl_Info(Uint32 dummy);

#endif /* _VIDEL_H */
