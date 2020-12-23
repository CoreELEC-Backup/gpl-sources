//
// MEMORY.H: Header file
//
// All Jaguar related memory and I/O locations are contained in this file
//

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t jagMemSpace[];

extern uint8_t * jaguarMainRAM;
extern uint8_t * jaguarMainROM;
extern uint8_t * gpuRAM;
extern uint8_t * dspRAM;

extern uint32_t * butch, * dscntrl;
extern uint16_t * ds_data;
extern uint32_t * i2cntrl, * sbcntrl, * subdata, * subdatb, * sb_time, * fifo_data, * i2sdat2, * unknown;

extern uint16_t * memcon1, * memcon2, * hc, * vc, * lph, * lpv;
extern uint64_t * obData;
extern uint32_t * olp;
extern uint16_t * obf, * vmode, * bord1, * bord2, * hp, * hbb, * hbe, * hs,
	* hvs, * hdb1, * hdb2, * hde, * vp, * vbb, * vbe, * vs, * vdb, * vde,
	* veb, * vee, * vi, * pit0, * pit1, * heq;
extern uint32_t * bg;
extern uint16_t * int1, * int2;
extern uint8_t * clut, * lbuf;
extern uint32_t * g_flags, * g_mtxc, * g_mtxa, * g_end, * g_pc, * g_ctrl,
	* g_hidata, * g_divctrl;
extern uint32_t g_remain;
extern uint32_t * a1_base, * a1_flags, * a1_clip, * a1_pixel, * a1_step,
	* a1_fstep, * a1_fpixel, * a1_inc, * a1_finc, * a2_base, * a2_flags,
	* a2_mask, * a2_pixel, * a2_step, * b_cmd, * b_count;
extern uint64_t * b_srcd, * b_dstd, * b_dstz, * b_srcz1, * b_srcz2, * b_patd;
extern uint32_t * b_iinc, * b_zinc, * b_stop, * b_i3, * b_i2, * b_i1, * b_i0, * b_z3,
	* b_z2, * b_z1, * b_z0;
extern uint16_t * jpit1, * jpit2, * jpit3, * jpit4, * clk1, * clk2, * clk3, * j_int,
	* asidata, * asictrl;
extern uint16_t asistat;
extern uint16_t * asiclk, * joystick, * joybuts;
extern uint32_t * d_flags, * d_mtxc, * d_mtxa, * d_end, * d_pc, * d_ctrl,
	* d_mod, * d_divctrl;
extern uint32_t d_remain;
extern uint32_t * d_machi;
extern uint16_t * ltxd, lrxd, * rtxd, rrxd;
extern uint8_t * sclk, sstat;
extern uint32_t * smode;

// Read/write tracing enumeration

enum { UNKNOWN, JAGUAR, DSP, GPU, TOM, JERRY, M68K, BLITTER, OP, DEBUG };
extern const char * whoName[10];

// BIOS identification enum

// Some handy macros to help converting native endian to big endian (jaguar native)
// & vice versa

#define SET64(r, a, v) 	r[(a)] = ((v) & 0xFF00000000000000) >> 56, r[(a)+1] = ((v) & 0x00FF000000000000) >> 48, \
						r[(a)+2] = ((v) & 0x0000FF0000000000) >> 40, r[(a)+3] = ((v) & 0x000000FF00000000) >> 32, \
						r[(a)+4] = ((v) & 0xFF000000) >> 24, r[(a)+5] = ((v) & 0x00FF0000) >> 16, \
						r[(a)+6] = ((v) & 0x0000FF00) >> 8, r[(a)+7] = (v) & 0x000000FF
#define GET64(r, a)		(((uint64_t)r[(a)] << 56) | ((uint64_t)r[(a)+1] << 48) | \
						((uint64_t)r[(a)+2] << 40) | ((uint64_t)r[(a)+3] << 32) | \
						((uint64_t)r[(a)+4] << 24) | ((uint64_t)r[(a)+5] << 16) | \
						((uint64_t)r[(a)+6] << 8) | (uint64_t)r[(a)+7])
#define SET32(r, a, v)	r[(a)] = ((v) & 0xFF000000) >> 24, r[(a)+1] = ((v) & 0x00FF0000) >> 16, \
						r[(a)+2] = ((v) & 0x0000FF00) >> 8, r[(a)+3] = (v) & 0x000000FF
#define GET32(r, a)		((r[(a)] << 24) | (r[(a)+1] << 16) | (r[(a)+2] << 8) | r[(a)+3])
#define SET16(r, a, v)	r[(a)] = ((v) & 0xFF00) >> 8, r[(a)+1] = (v) & 0xFF
#define GET16(r, a)		((r[(a)] << 8) | r[(a)+1])

#ifdef __cplusplus
}
#endif

#endif	// __MEMORY_H__
