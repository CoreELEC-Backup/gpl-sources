/*
* UAE - The Un*x Amiga Emulator
*
* MC68000 emulation
*
* (c) 1995 Bernd Schmidt
*/

#define MOVEC_DEBUG 0
#define MMUOP_DEBUG 2
#define DEBUG_CD32CDTVIO 0

#include "main.h"
#include "compat.h"
#include "sysconfig.h"
#include "sysdeps.h"
#include "hatari-glue.h"
#include "options_cpu.h"
#include "events.h"
#include "custom.h"
#include "maccess.h"
#include "memory.h"
#include "newcpu.h"
#include "main.h"
#include "m68000.h"
#include "md-fpp.h"
#include "cpummu.h"
#include "cpummu030.h"
#include "cpu_prefetch.h"
#include "reset.h"
#include "cycInt.h"
#include "mfp.h"
#include "tos.h"
#include "vdi.h"
#include "cart.h"
#include "dialog.h"
#include "bios.h"
#include "xbios.h"
#include "screen.h"
#include "video.h"
#include "options.h"
#include "dsp.h"
#include "log.h"
#include "debugui.h"
#include "debugcpu.h"
#include "stMemory.h"
//#include "falcon_cycle030.h"


#ifdef JIT
#include "jit/compemu.h"
#include <signal.h>
#else
/* Need to have these somewhere */
// static void build_comp (void) {}
// bool check_prefs_changed_comp (void) { return false; }
#endif
/* For faster JIT cycles handling */
signed long pissoff = 0;

uaecptr rtarea_base = RTAREA_DEFAULT;

/* Opcode of faulting instruction */
static uae_u16 last_op_for_exception_3;
/* PC at fault time */
static uaecptr last_addr_for_exception_3;
/* Address that generated the exception */
static uaecptr last_fault_for_exception_3;
/* read (0) or write (1) access */
static int last_writeaccess_for_exception_3;
/* instruction (1) or data (0) access */
static int last_instructionaccess_for_exception_3;
unsigned long irqcycles[15];
int irqdelay[15];
int mmu_enabled, mmu_triggered;
int cpu_cycles;
static int baseclock;
int cpucycleunit;

const int areg_byteinc[] = { 1, 1, 1, 1, 1, 1, 1, 2 };
const int imm8_table[] = { 8, 1, 2, 3, 4, 5, 6, 7 };

int movem_index1[256];
int movem_index2[256];
int movem_next[256];

cpuop_func *cpufunctbl[65536];

int OpcodeFamily;
int BusCyclePenalty = 0;


/* Amiga's specific variables, required to compile until all Amiga stuffs are ignored */
int vpos;
int quit_program;  // declared as "int quit_program = 0;" in main.c


struct mmufixup mmufixup[2];

#define COUNT_INSTRS 0
#define MC68060_PCR   0x04300000
#define MC68EC060_PCR 0x04310000

uae_u64 srp_030, crp_030;
uae_u32 tt0_030, tt1_030, tc_030;
uae_u16 mmusr_030;

static struct cache020 caches020[CACHELINES020];
static struct cache030 icaches030[CACHELINES030];
static struct cache030 dcaches030[CACHELINES030];
static struct cache040 caches040[CACHESETS040];
static void InterruptAddJitter (int Level , int Pending);

static void m68k_disasm_2 (FILE *f, uaecptr addr, uaecptr *nextpc, int cnt, uae_u32 *seaddr, uae_u32 *deaddr, int safemode);


#if COUNT_INSTRS
static unsigned long int instrcount[65536];
static uae_u16 opcodenums[65536];

static int compfn (const void *el1, const void *el2)
{
	return instrcount[*(const uae_u16 *)el1] < instrcount[*(const uae_u16 *)el2];
}

static TCHAR *icountfilename (void)
{
	TCHAR *name = getenv ("INSNCOUNT");
	if (name)
		return name;
	return COUNT_INSTRS == 2 ? "frequent.68k" : "insncount";
}

void dump_counts (void)
{
	FILE *f = fopen (icountfilename (), "w");
	unsigned long int total;
	int i;

	write_log ("Writing instruction count file...\n");
	for (i = 0; i < 65536; i++) {
		opcodenums[i] = i;
		total += instrcount[i];
	}
	qsort (opcodenums, 65536, sizeof (uae_u16), compfn);

	fprintf (f, "Total: %lu\n", total);
	for (i=0; i < 65536; i++) {
		unsigned long int cnt = instrcount[opcodenums[i]];
		struct instr *dp;
		struct mnemolookup *lookup;
		if (!cnt)
			break;
		dp = table68k + opcodenums[i];
		for (lookup = lookuptab;lookup->mnemo != dp->mnemo; lookup++)
			;
		fprintf (f, "%04x: %lu %s\n", opcodenums[i], cnt, lookup->name);
	}
	fclose (f);
}
#else
void dump_counts (void)
{
}
#endif


uae_u32 (*x_prefetch)(int);
uae_u32 (*x_next_iword)(void);
uae_u32 (*x_next_ilong)(void);
uae_u32 (*x_get_long)(uaecptr);
uae_u32 (*x_get_word)(uaecptr);
uae_u32 (*x_get_byte)(uaecptr);
void (*x_put_long)(uaecptr,uae_u32);
void (*x_put_word)(uaecptr,uae_u32);
void (*x_put_byte)(uaecptr,uae_u32);

// shared memory access functions
static void set_x_funcs (void)
{
	if (currprefs.mmu_model && currprefs.cpu_model == 68030) {
		x_prefetch = get_iword_mmu030;
		x_next_iword = next_iword_mmu030;
		x_next_ilong = next_ilong_mmu030;
		x_put_long = put_long_mmu030;
		x_put_word = put_word_mmu030;
		x_put_byte = put_byte_mmu030;
		x_get_long = get_long_mmu030;
		x_get_word = get_word_mmu030;
		x_get_byte = get_byte_mmu030;
	} else if (currprefs.mmu_model) {
		x_prefetch = get_iword_mmu;
		x_next_iword = next_iword_mmu;
		x_next_ilong = next_ilong_mmu;
		x_put_long = put_long_mmu;
		x_put_word = put_word_mmu;
		x_put_byte = put_byte_mmu;
		x_get_long = get_long_mmu;
		x_get_word = get_word_mmu;
		x_get_byte = get_byte_mmu;
	} else if (!currprefs.cpu_cycle_exact) {
		x_prefetch = get_iword;
		x_next_iword = next_iword;
		x_next_ilong = next_ilong;
		x_put_long = put_long;
		x_put_word = put_word;
		x_put_byte = put_byte;
		x_get_long = get_long;
		x_get_word = get_word;
		x_get_byte = get_byte;
	} else if (currprefs.cpu_model < 68020) {
		x_prefetch = NULL;
		x_next_iword = NULL;
		x_next_ilong = NULL;
		x_put_long = put_long_ce;
		x_put_word = put_word_ce;
		x_put_byte = put_byte_ce;
		x_get_long = get_long_ce;
		x_get_word = get_word_ce;
		x_get_byte = get_byte_ce;
	} else if (currprefs.cpu_model == 68020) {
		x_prefetch = get_word_ce020_prefetch;
		x_next_iword = next_iword_020ce;
		x_next_ilong = next_ilong_020ce;
		x_put_long = put_long_ce020;
		x_put_word = put_word_ce020;
		x_put_byte = put_byte_ce020;
		x_get_long = get_long_ce020;
		x_get_word = get_word_ce020;
		x_get_byte = get_byte_ce020;
	} else {
		x_prefetch = get_word_ce030_prefetch;
		x_next_iword = next_iword_030ce;
		x_next_ilong = next_ilong_030ce;
		x_put_long = put_long_ce030;
		x_put_word = put_word_ce030;
		x_put_byte = put_byte_ce030;
		x_get_long = get_long_ce030;
		x_get_word = get_word_ce030;
		x_get_byte = get_byte_ce030;
	}

}

static void set_cpu_caches (void)
{
	int i;
	uae_u32 caar = regs.caar & 0xfc;

	for (i = 0; i < CPU_PIPELINE_MAX; i++)
		regs.prefetch020addr[i] = 0xffffffff;

#ifdef JIT
	if (currprefs.cachesize) {
		if (currprefs.cpu_model < 68040) {
			set_cache_state (regs.cacr & 1);
			if (regs.cacr & 0x08) {
				flush_icache (0, 3);
			}
		} else {
			set_cache_state ((regs.cacr & 0x8000) ? 1 : 0);
		}
	}
#endif
	if (currprefs.cpu_model == 68020) {
		if (regs.cacr & 0x08) { // clear instr cache
			for (i = 0; i < CACHELINES020; i++)
				caches020[i].valid = 0;
		}
		if (regs.cacr & 0x04) { // clear entry in instr cache
			caches020[(caar >> 2) & (CACHELINES020 - 1)].valid = 0;
			regs.cacr &= ~0x04;
		}
	} else if (currprefs.cpu_model == 68030) {
		//regs.cacr |= 0x100;
		if (regs.cacr & 0x08) { // clear instr cache
			for (i = 0; i < CACHELINES030; i++) {
				icaches030[i].valid[0] = 0;
				icaches030[i].valid[1] = 0;
				icaches030[i].valid[2] = 0;
				icaches030[i].valid[3] = 0;
			}
		}
		if (regs.cacr & 0x04) { // clear entry in instr cache
			icaches030[(caar >> 4) & (CACHELINES030 - 1)].valid[(caar >> 2) & 3] = 0;
			regs.cacr &= ~0x04;
		}
		if (regs.cacr & 0x800) { // clear data cache
			for (i = 0; i < CACHELINES030; i++) {
				dcaches030[i].valid[0] = 0;
				dcaches030[i].valid[1] = 0;
				dcaches030[i].valid[2] = 0;
				dcaches030[i].valid[3] = 0;
			}
			regs.cacr &= ~0x800;
		}
		if (regs.cacr & 0x400) { // clear entry in data cache
			dcaches030[(caar >> 4) & (CACHELINES030 - 1)].valid[(caar >> 2) & 3] = 0;
			regs.cacr &= ~0x400;
		}
	} else if (currprefs.cpu_model == 68040) {
		if (!(regs.cacr & 0x8000)) {
			for (i = 0; i < CACHESETS040; i++) {
				caches040[i].valid[0] = 0;
				caches040[i].valid[1] = 0;
				caches040[i].valid[2] = 0;
				caches040[i].valid[3] = 0;
			}
		}
	}
}

STATIC_INLINE void count_instr (unsigned int opcode)
{
}

static unsigned long REGPARAM3 op_illg_1 (uae_u32 opcode) REGPARAM;

static unsigned long REGPARAM2 op_illg_1 (uae_u32 opcode)
{
	op_illg (opcode);
	return 4;
}

void build_cpufunctbl (void)
{
	int i, opcnt;
	unsigned long opcode;
	const struct cputbl *tbl = 0;
	int lvl;

	switch (currprefs.cpu_model)
	{
#ifdef CPUEMU_0
#ifndef CPUEMU_68000_ONLY
	case 68060:
		lvl = 5;
		tbl = op_smalltbl_0_ff;
		if (currprefs.cpu_cycle_exact)
			tbl = op_smalltbl_21_ff;
		if (currprefs.mmu_model)
			tbl = op_smalltbl_31_ff;
		break;
	case 68040:
		lvl = 4;
		tbl = op_smalltbl_1_ff;
		if (currprefs.cpu_cycle_exact)
			tbl = op_smalltbl_22_ff;
		if (currprefs.mmu_model)
			tbl = op_smalltbl_31_ff;
		break;
	case 68030:
		lvl = 3;
		tbl = op_smalltbl_2_ff;
		if (currprefs.cpu_cycle_exact)
			tbl = op_smalltbl_23_ff;
		if (currprefs.mmu_model)
			tbl = op_smalltbl_32_ff;
		break;
	case 68020:
		lvl = 2;
		tbl = op_smalltbl_3_ff;
		if (currprefs.cpu_cycle_exact)
			tbl = op_smalltbl_20_ff;
		break;
	case 68010:
		lvl = 1;
		tbl = op_smalltbl_4_ff;
		break;
#endif
#endif
	default:
		changed_prefs.cpu_model = currprefs.cpu_model = 68000;
	case 68000:
		lvl = 0;
		tbl = op_smalltbl_5_ff;
#ifdef CPUEMU_11
		if (currprefs.cpu_compatible)
			tbl = op_smalltbl_11_ff; /* prefetch */
#endif
#ifdef CPUEMU_12
		if (currprefs.cpu_cycle_exact)
			tbl = op_smalltbl_12_ff; /* prefetch and cycle-exact */
#endif
		break;
	}

	if (tbl == 0) {
		write_log ("no CPU emulation cores available CPU=%d!", currprefs.cpu_model);
		abort ();
	}

	for (opcode = 0; opcode < 65536; opcode++)
		cpufunctbl[opcode] = op_illg_1;
	for (i = 0; tbl[i].handler != NULL; i++) {
		opcode = tbl[i].opcode;
		cpufunctbl[opcode] = tbl[i].handler;
	}

	/* hack fpu to 68000/68010 mode */
	if (currprefs.fpu_model && currprefs.cpu_model < 68020) {
		tbl = op_smalltbl_3_ff;
		for (i = 0; tbl[i].handler != NULL; i++) {
			if ((tbl[i].opcode & 0xfe00) == 0xf200)
				cpufunctbl[tbl[i].opcode] = tbl[i].handler;
		}
	}
	opcnt = 0;
	for (opcode = 0; opcode < 65536; opcode++) {
		cpuop_func *f;

		if (table68k[opcode].mnemo == i_ILLG)
			continue;
		if (currprefs.fpu_model && currprefs.cpu_model < 68020) {
			/* more hack fpu to 68000/68010 mode */
			if (table68k[opcode].clev > lvl && (opcode & 0xfe00) != 0xf200)
				continue;
		} else if (table68k[opcode].clev > lvl) {
			continue;
		}

		if (table68k[opcode].handler != -1) {
			int idx = table68k[opcode].handler;
			f = cpufunctbl[idx];
			if (f == op_illg_1)
				abort ();
			cpufunctbl[opcode] = f;
			opcnt++;
		}
	}
	write_log ("Building CPU, %d opcodes (%d %d %d)\n",
		opcnt, lvl,
		currprefs.cpu_cycle_exact ? -1 : currprefs.cpu_compatible ? 1 : 0, currprefs.address_space_24);
	write_log ("CPU=%d, FPU=%d, MMU=%d, JIT%s=%d.\n", currprefs.cpu_model,
		currprefs.fpu_model, currprefs.mmu_model,
		currprefs.cachesize ? (currprefs.compfpu ? "=CPU/FPU" : "=CPU") : "",
		currprefs.cachesize);
#ifdef JIT
	build_comp ();
#endif
	set_cpu_caches ();
	if (currprefs.mmu_model) {
		if (currprefs.cpu_model >= 68040) {
			mmu_reset ();
			mmu_set_tc (regs.tcr);
			mmu_set_super (regs.s != 0);
		}
		else {
			mmu030_reset (0);
		}
	}
}

void fill_prefetch_slow (void)
{
	if (currprefs.mmu_model)
		return;
	regs.ir = x_get_word (m68k_getpc ());
	regs.irc = x_get_word (m68k_getpc () + 2);
}

unsigned long cycles_mask, cycles_val;

static void update_68k_cycles (void)
{
	cycles_mask = 0;
	cycles_val = currprefs.m68k_speed;
	if (currprefs.m68k_speed < 1) {
		cycles_mask = 0xFFFFFFFF;
		cycles_val = 0;
	}
	currprefs.cpu_clock_multiplier = changed_prefs.cpu_clock_multiplier;
	currprefs.cpu_frequency = changed_prefs.cpu_frequency;

	baseclock = currprefs.ntscmode ? 28636360 : 28375160;
	cpucycleunit = CYCLE_UNIT / 2;
	if (currprefs.cpu_clock_multiplier) {
		if (currprefs.cpu_clock_multiplier >= 256) {
			cpucycleunit = CYCLE_UNIT / (currprefs.cpu_clock_multiplier >> 8);
		} else {
			cpucycleunit = CYCLE_UNIT * currprefs.cpu_clock_multiplier;
		}
	} else if (currprefs.cpu_frequency) {
		cpucycleunit = CYCLE_UNIT * baseclock / currprefs.cpu_frequency;
	}
	if (cpucycleunit < 1)
		cpucycleunit = 1;
	if (currprefs.cpu_cycle_exact)
		write_log ("CPU cycleunit: %d (%.3f)\n", cpucycleunit, (float)cpucycleunit / CYCLE_UNIT);
}

static void prefs_changed_cpu (void)
{
	fixup_cpu (&changed_prefs);
	currprefs.cpu_model = changed_prefs.cpu_model;
	currprefs.fpu_model = changed_prefs.fpu_model;
	currprefs.mmu_model = changed_prefs.mmu_model;
	currprefs.cpu_compatible = changed_prefs.cpu_compatible;
	currprefs.cpu_cycle_exact = changed_prefs.cpu_cycle_exact;
	currprefs.blitter_cycle_exact = changed_prefs.cpu_cycle_exact;
}

void check_prefs_changed_cpu (void)
{
	bool changed = 0;

#ifdef JIT
	changed = check_prefs_changed_comp ();
#endif
	if (changed
		|| currprefs.cpu_model != changed_prefs.cpu_model
		|| currprefs.fpu_model != changed_prefs.fpu_model
		|| currprefs.mmu_model != changed_prefs.mmu_model
		|| currprefs.cpu_compatible != changed_prefs.cpu_compatible
		|| currprefs.cpu_cycle_exact != changed_prefs.cpu_cycle_exact) {

			prefs_changed_cpu ();
			if (!currprefs.cpu_compatible && changed_prefs.cpu_compatible)
				fill_prefetch_slow ();
			build_cpufunctbl ();
			changed = 1;
	}
	if (changed
		|| currprefs.m68k_speed != changed_prefs.m68k_speed
		|| currprefs.cpu_clock_multiplier != changed_prefs.cpu_clock_multiplier
		|| currprefs.cpu_frequency != changed_prefs.cpu_frequency) {
			currprefs.m68k_speed = changed_prefs.m68k_speed;
			reset_frame_rate_hack ();
			update_68k_cycles ();
			changed = 1;
	}

	if (currprefs.cpu_idle != changed_prefs.cpu_idle) {
		currprefs.cpu_idle = changed_prefs.cpu_idle;
	}
	if (changed)
		set_special (SPCFLAG_BRK);

}

void init_m68k (void)
{
	int i;

	prefs_changed_cpu ();
	update_68k_cycles ();

	for (i = 0 ; i < 256 ; i++) {
		int j;
		for (j = 0 ; j < 8 ; j++) {
			if (i & (1 << j)) break;
		}
		movem_index1[i] = j;
		movem_index2[i] = 7-j;
		movem_next[i] = i & (~(1 << j));
	}

#if COUNT_INSTRS
	{
		FILE *f = fopen (icountfilename (), "r");
		memset (instrcount, 0, sizeof instrcount);
		if (f) {
			uae_u32 opcode, count, total;
			TCHAR name[20];
			write_log ("Reading instruction count file...\n");
			fscanf (f, "Total: %lu\n", &total);
			while (fscanf (f, "%lx: %lu %s\n", &opcode, &count, name) == 3) {
				instrcount[opcode] = count;
			}
			fclose (f);
		}
	}
#endif
	write_log ("Building CPU table for configuration: %d", currprefs.cpu_model);
	regs.address_space_mask = 0xffffffff;
//	if (currprefs.cpu_compatible) {
//		if (currprefs.address_space_24 && currprefs.cpu_model >= 68030)
//			currprefs.address_space_24 = false;
//	}
	if (currprefs.fpu_model > 0)
		write_log ("/%d", currprefs.fpu_model);
	if (currprefs.cpu_cycle_exact) {
		if (currprefs.cpu_model == 68000)
			write_log (" prefetch and cycle-exact");
		else
			write_log (" ~cycle-exact");
	} else if (currprefs.cpu_compatible)
		write_log (" prefetch");
	if (currprefs.address_space_24) {
		regs.address_space_mask = 0x00ffffff;
		write_log (" 24-bit");
	}
	write_log ("\n");

	read_table68k ();
	do_merges ();

	write_log ("%d CPU functions\n", nr_cpuop_funcs);

	build_cpufunctbl ();
	set_x_funcs ();

#ifdef JIT
	/* We need to check whether NATMEM settings have changed
	* before starting the CPU */
	check_prefs_changed_comp ();
#endif
}

struct regstruct regs, mmu_backup_regs;
struct flag_struct regflags;
static struct regstruct regs_backup[16];
static int backup_pointer = 0;
static long int m68kpc_offset;

#define get_ibyte_1(o) get_byte (regs.pc + (regs.pc_p - regs.pc_oldp) + (o) + 1)
#define get_iword_1(o) get_word (regs.pc + (regs.pc_p - regs.pc_oldp) + (o))
#define get_ilong_1(o) get_long (regs.pc + (regs.pc_p - regs.pc_oldp) + (o))

static uae_s32 ShowEA (FILE *f, uae_u16 opcode, int reg, amodes mode, wordsizes size, TCHAR *buf, uae_u32 *eaddr, int safemode)
{
	uae_u16 dp;
	uae_s8 disp8;
	uae_s16 disp16;
	int r;
	uae_u32 dispreg;
	uaecptr addr = 0;
	uae_s32 offset = 0;
	TCHAR buffer[80];

	switch (mode){
	case Dreg:
		_stprintf (buffer, "D%d", reg);
		break;
	case Areg:
		_stprintf (buffer, "A%d", reg);
		break;
	case Aind:
		_stprintf (buffer, "(A%d)", reg);
		addr = regs.regs[reg + 8];
		break;
	case Aipi:
		_stprintf (buffer, "(A%d)+", reg);
		addr = regs.regs[reg + 8];
		break;
	case Apdi:
		_stprintf (buffer, "-(A%d)", reg);
		addr = regs.regs[reg + 8];
		break;
	case Ad16:
		{
			TCHAR offtxt[80];
			disp16 = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
			if (disp16 < 0)
				_stprintf (offtxt, "-$%04x", -disp16);
			else
				_stprintf (offtxt, "$%04x", disp16);
			addr = m68k_areg (regs, reg) + disp16;
			_stprintf (buffer, "(A%d, %s) == $%08lx", reg, offtxt, (unsigned long)addr);
		}
		break;
	case Ad8r:
		dp = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
		disp8 = dp & 0xFF;
		r = (dp & 0x7000) >> 12;
		dispreg = dp & 0x8000 ? m68k_areg (regs, r) : m68k_dreg (regs, r);
		if (!(dp & 0x800)) dispreg = (uae_s32)(uae_s16)(dispreg);
		dispreg <<= (dp >> 9) & 3;

		if (dp & 0x100) {
			uae_s32 outer = 0, disp = 0;
			uae_s32 base = m68k_areg (regs, reg);
			TCHAR name[10];
			_stprintf (name, "A%d, ", reg);
			if (dp & 0x80) { base = 0; name[0] = 0; }
			if (dp & 0x40) dispreg = 0;
			if ((dp & 0x30) == 0x20) { disp = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
			if ((dp & 0x30) == 0x30) { disp = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }
			base += disp;

			if ((dp & 0x3) == 0x2) { outer = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
			if ((dp & 0x3) == 0x3) { outer = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }

			if (!(dp & 4)) base += dispreg;
			if ((dp & 3) && !safemode) base = get_long (base);
			if (dp & 4) base += dispreg;

			addr = base + outer;
			_stprintf (buffer, "(%s%c%d.%c*%d+%d)+%d == $%08lx", name,
				dp & 0x8000 ? 'A' : 'D', (int)r, dp & 0x800 ? 'L' : 'W',
				1 << ((dp >> 9) & 3),
				disp, outer,
				(unsigned long)addr);
		} else {
			addr = m68k_areg (regs, reg) + (uae_s32)((uae_s8)disp8) + dispreg;
			_stprintf (buffer, "(A%d, %c%d.%c*%d, $%02x) == $%08lx", reg,
				dp & 0x8000 ? 'A' : 'D', (int)r, dp & 0x800 ? 'L' : 'W',
				1 << ((dp >> 9) & 3), disp8,
				(unsigned long)addr);
		}
		break;
	case PC16:
		addr = m68k_getpc () + m68kpc_offset;
		disp16 = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
		addr += (uae_s16)disp16;
		_stprintf (buffer, "(PC,$%04x) == $%08lx", disp16 & 0xffff, (unsigned long)addr);
		break;
	case PC8r:
		addr = m68k_getpc () + m68kpc_offset;
		dp = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
		disp8 = dp & 0xFF;
		r = (dp & 0x7000) >> 12;
		dispreg = dp & 0x8000 ? m68k_areg (regs, r) : m68k_dreg (regs, r);
		if (!(dp & 0x800)) dispreg = (uae_s32)(uae_s16)(dispreg);
		dispreg <<= (dp >> 9) & 3;

		if (dp & 0x100) {
			uae_s32 outer = 0, disp = 0;
			uae_s32 base = addr;
			TCHAR name[10];
			_stprintf (name, "PC, ");
			if (dp & 0x80) { base = 0; name[0] = 0; }
			if (dp & 0x40) dispreg = 0;
			if ((dp & 0x30) == 0x20) { disp = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
			if ((dp & 0x30) == 0x30) { disp = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }
			base += disp;

			if ((dp & 0x3) == 0x2) { outer = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
			if ((dp & 0x3) == 0x3) { outer = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }

			if (!(dp & 4)) base += dispreg;
			if ((dp & 3) && !safemode) base = get_long (base);
			if (dp & 4) base += dispreg;

			addr = base + outer;
			_stprintf (buffer, "(%s%c%d.%c*%d+%d)+%d == $%08lx", name,
				dp & 0x8000 ? 'A' : 'D', (int)r, dp & 0x800 ? 'L' : 'W',
				1 << ((dp >> 9) & 3),
				disp, outer,
				(unsigned long)addr);
		} else {
			addr += (uae_s32)((uae_s8)disp8) + dispreg;
			_stprintf (buffer, "(PC, %c%d.%c*%d, $%02x) == $%08lx", dp & 0x8000 ? 'A' : 'D',
				(int)r, dp & 0x800 ? 'L' : 'W',  1 << ((dp >> 9) & 3),
				disp8, (unsigned long)addr);
		}
		break;
	case absw:
		addr = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset);
		_stprintf (buffer, "$%08lx", (unsigned long)addr);
		m68kpc_offset += 2;
		break;
	case absl:
		addr = get_ilong_1 (m68kpc_offset);
		_stprintf (buffer, "$%08lx", (unsigned long)addr);
		m68kpc_offset += 4;
		break;
	case imm:
		switch (size){
		case sz_byte:
			_stprintf (buffer, "#$%02x", (unsigned int)(get_iword_1 (m68kpc_offset) & 0xff));
			m68kpc_offset += 2;
			break;
		case sz_word:
			_stprintf (buffer, "#$%04x", (unsigned int)(get_iword_1 (m68kpc_offset) & 0xffff));
			m68kpc_offset += 2;
			break;
		case sz_long:
			_stprintf (buffer, "#$%08lx", (unsigned long)(get_ilong_1 (m68kpc_offset)));
			m68kpc_offset += 4;
			break;
		default:
			break;
		}
		break;
	case imm0:
		offset = (uae_s32)(uae_s8)get_iword_1 (m68kpc_offset);
		m68kpc_offset += 2;
		_stprintf (buffer, "#$%02x", (unsigned int)(offset & 0xff));
		break;
	case imm1:
		offset = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset);
		m68kpc_offset += 2;
		buffer[0] = 0;
		_stprintf (buffer, "#$%04x", (unsigned int)(offset & 0xffff));
		break;
	case imm2:
		offset = (uae_s32)get_ilong_1 (m68kpc_offset);
		m68kpc_offset += 4;
		_stprintf (buffer, "#$%08lx", (unsigned long)offset);
		break;
	case immi:
		offset = (uae_s32)(uae_s8)(reg & 0xff);
		_stprintf (buffer, "#$%08lx", (unsigned long)offset);
		break;
	default:
		break;
	}
	if (buf == 0)
		f_out (f, "%s", buffer);
	else
		_tcscat (buf, buffer);
	if (eaddr)
		*eaddr = addr;
	return offset;
}

#if 0
/* The plan is that this will take over the job of exception 3 handling -
* the CPU emulation functions will just do a longjmp to m68k_go whenever
* they hit an odd address. */
static int verify_ea (int reg, amodes mode, wordsizes size, uae_u32 *val)
{
	uae_u16 dp;
	uae_s8 disp8;
	uae_s16 disp16;
	int r;
	uae_u32 dispreg;
	uaecptr addr;
	uae_s32 offset = 0;

	switch (mode){
	case Dreg:
		*val = m68k_dreg (regs, reg);
		return 1;
	case Areg:
		*val = m68k_areg (regs, reg);
		return 1;

	case Aind:
	case Aipi:
		addr = m68k_areg (regs, reg);
		break;
	case Apdi:
		addr = m68k_areg (regs, reg);
		break;
	case Ad16:
		disp16 = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
		addr = m68k_areg (regs, reg) + (uae_s16)disp16;
		break;
	case Ad8r:
		addr = m68k_areg (regs, reg);
d8r_common:
		dp = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
		disp8 = dp & 0xFF;
		r = (dp & 0x7000) >> 12;
		dispreg = dp & 0x8000 ? m68k_areg (regs, r) : m68k_dreg (regs, r);
		if (!(dp & 0x800)) dispreg = (uae_s32)(uae_s16)(dispreg);
		dispreg <<= (dp >> 9) & 3;

		if (dp & 0x100) {
			uae_s32 outer = 0, disp = 0;
			uae_s32 base = addr;
			if (dp & 0x80) base = 0;
			if (dp & 0x40) dispreg = 0;
			if ((dp & 0x30) == 0x20) { disp = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
			if ((dp & 0x30) == 0x30) { disp = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }
			base += disp;

			if ((dp & 0x3) == 0x2) { outer = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset); m68kpc_offset += 2; }
			if ((dp & 0x3) == 0x3) { outer = get_ilong_1 (m68kpc_offset); m68kpc_offset += 4; }

			if (!(dp & 4)) base += dispreg;
			if (dp & 3) base = get_long (base);
			if (dp & 4) base += dispreg;

			addr = base + outer;
		} else {
			addr += (uae_s32)((uae_s8)disp8) + dispreg;
		}
		break;
	case PC16:
		addr = m68k_getpc () + m68kpc_offset;
		disp16 = get_iword_1 (m68kpc_offset); m68kpc_offset += 2;
		addr += (uae_s16)disp16;
		break;
	case PC8r:
		addr = m68k_getpc () + m68kpc_offset;
		goto d8r_common;
	case absw:
		addr = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset);
		m68kpc_offset += 2;
		break;
	case absl:
		addr = get_ilong_1 (m68kpc_offset);
		m68kpc_offset += 4;
		break;
	case imm:
		switch (size){
		case sz_byte:
			*val = get_iword_1 (m68kpc_offset) & 0xff;
			m68kpc_offset += 2;
			break;
		case sz_word:
			*val = get_iword_1 (m68kpc_offset) & 0xffff;
			m68kpc_offset += 2;
			break;
		case sz_long:
			*val = get_ilong_1 (m68kpc_offset);
			m68kpc_offset += 4;
			break;
		default:
			break;
		}
		return 1;
	case imm0:
		*val = (uae_s32)(uae_s8)get_iword_1 (m68kpc_offset);
		m68kpc_offset += 2;
		return 1;
	case imm1:
		*val = (uae_s32)(uae_s16)get_iword_1 (m68kpc_offset);
		m68kpc_offset += 2;
		return 1;
	case imm2:
		*val = get_ilong_1 (m68kpc_offset);
		m68kpc_offset += 4;
		return 1;
	case immi:
		*val = (uae_s32)(uae_s8)(reg & 0xff);
		return 1;
	default:
		addr = 0;
		break;
	}
	if ((addr & 1) == 0)
		return 1;

	last_addr_for_exception_3 = m68k_getpc () + m68kpc_offset;
	last_fault_for_exception_3 = addr;
	last_writeaccess_for_exception_3 = 0;
	last_instructionaccess_for_exception_3 = 0;
	return 0;
}
#endif

int get_cpu_model (void)
{
	return currprefs.cpu_model;
}

/*
* extract bitfield data from memory and return it in the MSBs
* bdata caches the unmodified data for put_bitfield()
*/
uae_u32 REGPARAM2 get_bitfield (uae_u32 src, uae_u32 bdata[2], uae_s32 offset, int width)
{
	uae_u32 tmp, res, mask;

	offset &= 7;
	mask = 0xffffffffu << (32 - width);
	switch ((offset + width + 7) >> 3) {
	case 1:
		tmp = get_byte (src);
		res = tmp << (24 + offset);
		bdata[0] = tmp & ~(mask >> (24 + offset));
		break;
	case 2:
		tmp = get_word (src);
		res = tmp << (16 + offset);
		bdata[0] = tmp & ~(mask >> (16 + offset));
		break;
	case 3:
		tmp = get_word (src);
		res = tmp << (16 + offset);
		bdata[0] = tmp & ~(mask >> (16 + offset));
		tmp = get_byte (src + 2);
		res |= tmp << (8 + offset);
		bdata[1] = tmp & ~(mask >> (8 + offset));
		break;
	case 4:
		tmp = get_long (src);
		res = tmp << offset;
		bdata[0] = tmp & ~(mask >> offset);
		break;
	case 5:
		tmp = get_long (src);
		res = tmp << offset;
		bdata[0] = tmp & ~(mask >> offset);
		tmp = get_byte (src + 4);
		res |= tmp >> (8 - offset);
		bdata[1] = tmp & ~(mask << (8 - offset));
		break;
	default:
		/* Panic? */
		res = 0;
		break;
	}
	return res;
}
/*
* write bitfield data (in the LSBs) back to memory, upper bits
* must be cleared already.
*/
void REGPARAM2 put_bitfield (uae_u32 dst, uae_u32 bdata[2], uae_u32 val, uae_s32 offset, int width)
{
	offset = (offset & 7) + width;
	switch ((offset + 7) >> 3) {
	case 1:
		put_byte (dst, bdata[0] | (val << (8 - offset)));
		break;
	case 2:
		put_word (dst, bdata[0] | (val << (16 - offset)));
		break;
	case 3:
		put_word (dst, bdata[0] | (val >> (offset - 16)));
		put_byte (dst + 2, bdata[1] | (val << (24 - offset)));
		break;
	case 4:
		put_long (dst, bdata[0] | (val << (32 - offset)));
		break;
	case 5:
		put_long (dst, bdata[0] | (val >> (offset - 32)));
		put_byte (dst + 4, bdata[1] | (val << (40 - offset)));
		break;
	}
}

uae_u32 REGPARAM2 x_get_bitfield (uae_u32 src, uae_u32 bdata[2], uae_s32 offset, int width)
{
	uae_u32 tmp, res, mask;

	offset &= 7;
	mask = 0xffffffffu << (32 - width);
	switch ((offset + width + 7) >> 3) {
	case 1:
		tmp = x_get_byte (src);
		res = tmp << (24 + offset);
		bdata[0] = tmp & ~(mask >> (24 + offset));
		break;
	case 2:
		tmp = x_get_word (src);
		res = tmp << (16 + offset);
		bdata[0] = tmp & ~(mask >> (16 + offset));
		break;
	case 3:
		tmp = x_get_word (src);
		res = tmp << (16 + offset);
		bdata[0] = tmp & ~(mask >> (16 + offset));
		tmp = x_get_byte (src + 2);
		res |= tmp << (8 + offset);
		bdata[1] = tmp & ~(mask >> (8 + offset));
		break;
	case 4:
		tmp = x_get_long (src);
		res = tmp << offset;
		bdata[0] = tmp & ~(mask >> offset);
		break;
	case 5:
		tmp = x_get_long (src);
		res = tmp << offset;
		bdata[0] = tmp & ~(mask >> offset);
		tmp = x_get_byte (src + 4);
		res |= tmp >> (8 - offset);
		bdata[1] = tmp & ~(mask << (8 - offset));
		break;
	default:
		/* Panic? */
		res = 0;
		break;
	}
	return res;
}

void REGPARAM2 x_put_bitfield (uae_u32 dst, uae_u32 bdata[2], uae_u32 val, uae_s32 offset, int width)
{
	offset = (offset & 7) + width;
	switch ((offset + 7) >> 3) {
	case 1:
		x_put_byte (dst, bdata[0] | (val << (8 - offset)));
		break;
	case 2:
		x_put_word (dst, bdata[0] | (val << (16 - offset)));
		break;
	case 3:
		x_put_word (dst, bdata[0] | (val >> (offset - 16)));
		x_put_byte (dst + 2, bdata[1] | (val << (24 - offset)));
		break;
	case 4:
		x_put_long (dst, bdata[0] | (val << (32 - offset)));
		break;
	case 5:
		x_put_long (dst, bdata[0] | (val >> (offset - 32)));
		x_put_byte (dst + 4, bdata[1] | (val << (40 - offset)));
		break;
	}
}

uae_u32 REGPARAM2 get_disp_ea_020 (uae_u32 base, uae_u32 dp)
{
	int reg = (dp >> 12) & 15;
	uae_s32 regd = regs.regs[reg];
	if ((dp & 0x800) == 0)
		regd = (uae_s32)(uae_s16)regd;
	regd <<= (dp >> 9) & 3;
	if (dp & 0x100) {
		uae_s32 outer = 0;
		if (dp & 0x80) base = 0;
		if (dp & 0x40) regd = 0;

		if ((dp & 0x30) == 0x20)
			base += (uae_s32)(uae_s16) next_iword ();
		if ((dp & 0x30) == 0x30)
			base += next_ilong ();

		if ((dp & 0x3) == 0x2)
			outer = (uae_s32)(uae_s16) next_iword ();
		if ((dp & 0x3) == 0x3)
			outer = next_ilong ();

		if ((dp & 0x4) == 0)
			base += regd;
		if (dp & 0x3)
			base = get_long (base);
		if (dp & 0x4)
			base += regd;

		return base + outer;
	} else {
		return base + (uae_s32)((uae_s8)dp) + regd;
	}
}

uae_u32 REGPARAM2 x_get_disp_ea_020 (uae_u32 base, uae_u32 dp)
{
	int reg = (dp >> 12) & 15;
	int cycles = 0;
	uae_u32 v;

	uae_s32 regd = regs.regs[reg];
	if ((dp & 0x800) == 0)
		regd = (uae_s32)(uae_s16)regd;
	regd <<= (dp >> 9) & 3;
	if (dp & 0x100) {
		uae_s32 outer = 0;
		if (dp & 0x80)
			base = 0;
		if (dp & 0x40)
			regd = 0;

		if ((dp & 0x30) == 0x20) {
			base += (uae_s32)(uae_s16) x_next_iword ();
			cycles++;
		}
		if ((dp & 0x30) == 0x30) {
			base += x_next_ilong ();
			cycles++;
		}

		if ((dp & 0x3) == 0x2) {
			outer = (uae_s32)(uae_s16) x_next_iword ();
			cycles++;
		}
		if ((dp & 0x3) == 0x3) {
			outer = x_next_ilong ();
			cycles++;
		}

		if ((dp & 0x4) == 0) {
			base += regd;
			cycles++;
		}
		if (dp & 0x3) {
			base = x_get_long (base);
			cycles++;
		}
		if (dp & 0x4) {
			base += regd;
			cycles++;
		}
		v = base + outer;
	} else {
		v = base + (uae_s32)((uae_s8)dp) + regd;
	}
	if (cycles)
		do_cycles_ce020 (cycles);
	return v;
}


uae_u32 REGPARAM3 get_disp_ea_000 (uae_u32 base, uae_u32 dp) REGPARAM
{
	int reg = (dp >> 12) & 15;
	uae_s32 regd = regs.regs[reg];
#if 1
	if ((dp & 0x800) == 0)
		regd = (uae_s32)(uae_s16)regd;
	return base + (uae_s8)dp + regd;
#else
	/* Branch-free code... benchmark this again now that
	* things are no longer inline.  */
	uae_s32 regd16;
	uae_u32 mask;
	mask = ((dp & 0x800) >> 11) - 1;
	regd16 = (uae_s32)(uae_s16)regd;
	regd16 &= mask;
	mask = ~mask;
	base += (uae_s8)dp;
	regd &= mask;
	regd |= regd16;
	return base + regd;
#endif
}

#if AMIGA_ONLY
STATIC_INLINE int in_rom (uaecptr pc)
{
	return (munge24 (pc) & 0xFFF80000) == 0xF80000;
}


STATIC_INLINE int in_rtarea (uaecptr pc)
{
	return (munge24 (pc) & 0xFFFF0000) == rtarea_base && uae_boot_rom;
}
#endif

void REGPARAM2 MakeSR (void)
{
	regs.sr = ((regs.t1 << 15) | (regs.t0 << 14)
		| (regs.s << 13) | (regs.m << 12) | (regs.intmask << 8)
		| (GET_XFLG () << 4) | (GET_NFLG () << 3)
		| (GET_ZFLG () << 2) | (GET_VFLG () << 1)
		|  GET_CFLG ());
}

void REGPARAM2 MakeFromSR (void)
{
	int oldm = regs.m;
	int olds = regs.s;

	if (currprefs.cpu_cycle_exact && currprefs.cpu_model >= 68020) {
		do_cycles_ce (6 * CYCLE_UNIT);
		regs.ce020memcycles = 0;
	}

	SET_XFLG ((regs.sr >> 4) & 1);
	SET_NFLG ((regs.sr >> 3) & 1);
	SET_ZFLG ((regs.sr >> 2) & 1);
	SET_VFLG ((regs.sr >> 1) & 1);
	SET_CFLG (regs.sr & 1);
	if (regs.t1 == ((regs.sr >> 15) & 1) &&
		regs.t0 == ((regs.sr >> 14) & 1) &&
		regs.s  == ((regs.sr >> 13) & 1) &&
		regs.m  == ((regs.sr >> 12) & 1) &&
		regs.intmask == ((regs.sr >> 8) & 7))
		return;
	regs.t1 = (regs.sr >> 15) & 1;
	regs.t0 = (regs.sr >> 14) & 1;
	regs.s  = (regs.sr >> 13) & 1;
	regs.m  = (regs.sr >> 12) & 1;
	regs.intmask = (regs.sr >> 8) & 7;
	if (currprefs.cpu_model >= 68020) {
		/* 68060 does not have MSP but does have M-bit.. */
		if (currprefs.cpu_model >= 68060)
			regs.msp = regs.isp;
		if (olds != regs.s) {
			if (olds) {
				if (oldm)
					regs.msp = m68k_areg (regs, 7);
				else
					regs.isp = m68k_areg (regs, 7);
				m68k_areg (regs, 7) = regs.usp;
			} else {
				regs.usp = m68k_areg (regs, 7);
				m68k_areg (regs, 7) = regs.m ? regs.msp : regs.isp;
			}
		} else if (olds && oldm != regs.m) {
			if (oldm) {
				regs.msp = m68k_areg (regs, 7);
				m68k_areg (regs, 7) = regs.isp;
			} else {
				regs.isp = m68k_areg (regs, 7);
				m68k_areg (regs, 7) = regs.msp;
			}
		}
		if (currprefs.cpu_model >= 68060)
			regs.t0 = 0;
	} else {
		regs.t0 = regs.m = 0;
		if (olds != regs.s) {
			if (olds) {
				regs.isp = m68k_areg (regs, 7);
				m68k_areg (regs, 7) = regs.usp;
			} else {
				regs.usp = m68k_areg (regs, 7);
				m68k_areg (regs, 7) = regs.isp;
			}
		}
	}
	if (currprefs.mmu_model)
		mmu_set_super (regs.s != 0);

	doint ();
	if (regs.t1 || regs.t0)
		set_special (SPCFLAG_TRACE);
	else
		/* Keep SPCFLAG_DOTRACE, we still want a trace exception for
		SR-modifying instructions (including STOP).  */
		unset_special (SPCFLAG_TRACE);
}

static void exception_trace (int nr)
{
	unset_special (SPCFLAG_TRACE | SPCFLAG_DOTRACE);
	if (regs.t1 && !regs.t0) {
		/* trace stays pending if exception is div by zero, chk,
		* trapv or trap #x
		*/
		if (nr == 5 || nr == 6 || nr == 7 || (nr >= 32 && nr <= 47))
			set_special (SPCFLAG_DOTRACE);
	}
	regs.t1 = regs.t0 = regs.m = 0;
}

static void exception_debug (int nr)
{
#ifdef DEBUGGER
	if (!exception_debugging)
		return;
	console_out_f ("Exception %d, PC=%08X\n", nr, M68K_GETPC);
#endif
	DebugUI_Exceptions(nr, M68K_GETPC);
}

#ifdef CPUEMU_12

/* cycle-exact exception handler, 68000 only */

/*

Address/Bus Error:

- 6 idle cycles
- write PC low word
- write SR
- write PC high word
- write instruction word
- write fault address low word
- write status code
- write fault address high word
- 2 idle cycles
- read exception address high word
- read exception address low word
- prefetch
- 2 idle cycles
- prefetch

Division by Zero:

- 6 idle cycles
- write PC low word
- write SR
- write PC high word
- read exception address high word
- read exception address low word
- prefetch
- 2 idle cycles
- prefetch

Traps:

- 2 idle cycles
- write PC low word
- write SR
- write PC high word
- read exception address high word
- read exception address low word
- prefetch
- 2 idle cycles
- prefetch

TrapV:

- write PC low word
- write SR
- write PC high word
- read exception address high word
- read exception address low word
- prefetch
- 2 idle cycles
- prefetch

CHK:

- 6 idle cycles
- write PC low word
- write SR
- write PC high word
- read exception address high word
- read exception address low word
- prefetch
- 2 idle cycles
- prefetch

Illegal Instruction:

- 2 idle cycles
- write PC low word
- write SR
- write PC high word
- read exception address high word
- read exception address low word
- prefetch
- 2 idle cycles
- prefetch

Interrupt cycle diagram:

- 6 idle cycles
- write PC low word
- read exception number byte from (0xfffff1 | (interrupt number << 1))
- 4 idle cycles
- write SR
- write PC high word
- read exception address high word
- read exception address low word
- prefetch
- 2 idle cycles
- prefetch

*/

static void Exception_ce000 (int nr, uaecptr oldpc)
{
	uae_u32 currpc = m68k_getpc (), newpc;
	int sv = regs.s;
	int start;

#if AMIGA_ONLY
	int interrupt;
	interrupt = nr >= 24 && nr < 24 + 8;
#endif

	start = 6;
	if (nr == 7) // TRAPV
		start = 0;
	else if (nr >= 32 && nr < 32 + 16) // TRAP #x
		start = 2;
	else if (nr == 4 || nr == 8) // ILLG & PRIVIL VIOL
		start = 2;

	if (start)
		do_cycles_ce000 (start);

	exception_debug (nr);
	MakeSR ();

	/* Handle Hatari GEM and BIOS traps */
	if (nr == 0x22) {
		/* Intercept VDI & AES exceptions (Trap #2) */
		if (bVdiAesIntercept && VDI_AES_Entry()) {
			/* Set 'PC' to address of 'VDI_OPCODE' illegal instruction.
			 * This will call OpCode_VDI() after completion of Trap call!
			 * This is used to modify specific VDI return vectors contents.
			*/
			VDI_OldPC = currpc;
			currpc = CART_VDI_OPCODE_ADDR;
		}
	}
	else if (nr == 0x2d) {
		/* Intercept BIOS (Trap #13) calls */
		if (Bios())  return;
	}
	else if (nr == 0x2e) {
		/* Intercept XBIOS (Trap #14) calls */
		if (XBios())  return;
	}

	if (!regs.s) {
		regs.usp = m68k_areg (regs, 7);
		m68k_areg (regs, 7) = regs.isp;
		regs.s = 1;
	}
	if (nr == 2 || nr == 3) { /* 2=bus error, 3=address error */
		uae_u16 mode = (sv ? 4 : 0) | (last_instructionaccess_for_exception_3 ? 2 : 1);
		mode |= last_writeaccess_for_exception_3 ? 0 : 16;
		m68k_areg (regs, 7) -= 14;
		/* fixme: bit3=I/N */
		put_word_ce (m68k_areg (regs, 7) + 12, last_addr_for_exception_3);
		put_word_ce (m68k_areg (regs, 7) + 8, regs.sr);
		put_word_ce (m68k_areg (regs, 7) + 10, last_addr_for_exception_3 >> 16);
		put_word_ce (m68k_areg (regs, 7) + 6, last_op_for_exception_3);
		put_word_ce (m68k_areg (regs, 7) + 4, last_fault_for_exception_3);
		put_word_ce (m68k_areg (regs, 7) + 0, mode);
		put_word_ce (m68k_areg (regs, 7) + 2, last_fault_for_exception_3 >> 16);
		do_cycles_ce000 (2);
		write_log ("Exception %d (%x) at %x -> %x!\n", nr, oldpc, currpc, STMemory_ReadLong(4 * nr));
		goto kludge_me_do;
	}
	m68k_areg (regs, 7) -= 6;
	put_word_ce (m68k_areg (regs, 7) + 4, currpc); // write low address
#if AMIGA_ONLY
	if (interrupt) {
		// fetch interrupt vector number
		nr = get_byte_ce (0x00fffff1 | ((nr - 24) << 1));
		do_cycles_ce000 (4);
	}
#endif
	put_word_ce (m68k_areg (regs, 7) + 0, regs.sr); // write SR
	put_word_ce (m68k_areg (regs, 7) + 2, currpc >> 16); // write high address
kludge_me_do:
	newpc = get_word_ce (4 * nr) << 16; // read high address
	newpc |= get_word_ce (4 * nr + 2); // read low address
	if (newpc & 1) {
		if (nr == 2 || nr == 3)
			Reset_Cold(); /* there is nothing else we can do.. */
		else
			exception3 (regs.ir, m68k_getpc (), newpc);
		return;
	}
	m68k_setpc (newpc);
	regs.ir = get_word_ce (m68k_getpc ()); // prefetch 1
	do_cycles_ce000 (2);
	regs.irc = get_word_ce (m68k_getpc () + 2); // prefetch 2
#ifdef JIT
	set_special (SPCFLAG_END_COMPILE);
#endif
	exception_trace (nr);
}
#endif

static void Exception_mmu (int nr, uaecptr oldpc)
{
	uae_u32 currpc = m68k_getpc (), newpc;
	int sv = regs.s;
	int i;

	exception_debug (nr);
	MakeSR ();

	if (!regs.s) {
		regs.usp = m68k_areg (regs, 7);
		if (currprefs.cpu_model >= 68020)
			m68k_areg (regs, 7) = regs.m ? regs.msp : regs.isp;
		else
			m68k_areg (regs, 7) = regs.isp;
		regs.s = 1;
		mmu_set_super (1);
	}

	if (nr == 2 && currprefs.cpu_model <= 68030) {
		// Bus error for 68030 mode
		// write_log ("Exception_mmu %08x %08x %08x\n", currpc, oldpc, regs.mmu_fault_addr);
		m68k_areg (regs, 7) -= 4;
		x_put_long (m68k_areg (regs, 7), 0);  // Internal register
		m68k_areg (regs, 7) -= 4;
		x_put_long (m68k_areg (regs, 7), regs.wb3_data);  // Data output buffer
		m68k_areg (regs, 7) -= 4;
		x_put_long (m68k_areg (regs, 7), 0);  // Internal register
		m68k_areg (regs, 7) -= 4;
		x_put_long (m68k_areg (regs, 7), regs.mmu_fault_addr);
		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), 0);  // Instr. pipe stage B
		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), 0);  // Instr. pipe stage C
		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), regs.mmu_ssw);
		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), 0);  // Internal register

		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), 0xa000 + nr * 4);
		m68k_areg (regs, 7) -= 4;
		x_put_long (m68k_areg (regs, 7), oldpc);
		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), regs.sr);
		goto kludge_me_do;

	} else if (nr == 2) {
		// Bus error / access error for 68040
		// write_log ("Exception_mmu %08x %08x %08x\n", currpc, oldpc, regs.mmu_fault_addr);
		for (i = 0 ; i < 7 ; i++) {
			m68k_areg (regs, 7) -= 4;
			x_put_long (m68k_areg (regs, 7), 0);
		}
		m68k_areg (regs, 7) -= 4;
		x_put_long (m68k_areg (regs, 7), regs.wb3_data);
		m68k_areg (regs, 7) -= 4;
		x_put_long (m68k_areg (regs, 7), regs.mmu_fault_addr);
		m68k_areg (regs, 7) -= 4;
		x_put_long (m68k_areg (regs, 7), regs.mmu_fault_addr);
		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), 0);
		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), 0);
		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), regs.wb3_status);
		regs.wb3_status = 0;
		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), regs.mmu_ssw);
		m68k_areg (regs, 7) -= 4;
		x_put_long (m68k_areg (regs, 7), regs.mmu_fault_addr);

		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), 0x7000 + nr * 4);
		m68k_areg (regs, 7) -= 4;
		x_put_long (m68k_areg (regs, 7), oldpc);
		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), regs.sr);
		goto kludge_me_do;

	} else if (nr == 3) {

		// address error
		uae_u16 ssw = (sv ? 4 : 0) | (last_instructionaccess_for_exception_3 ? 2 : 1);
		ssw |= last_writeaccess_for_exception_3 ? 0 : 0x40;
		ssw |= 0x20;
		for (i = 0 ; i < 36; i++) {
			m68k_areg (regs, 7) -= 2;
			x_put_word (m68k_areg (regs, 7), 0);
		}
		m68k_areg (regs, 7) -= 4;
		x_put_long (m68k_areg (regs, 7), last_fault_for_exception_3);
		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), 0);
		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), 0);
		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), 0);
		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), ssw);
		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), 0xb000 + nr * 4);
		write_log ("Exception %d (%x) at %x -> %x!\n", nr, oldpc, currpc, STMemory_ReadLong(regs.vbr + 4*nr));

	} else if (nr ==5 || nr == 6 || nr == 7 || nr == 9 || nr == 56) {

		m68k_areg (regs, 7) -= 4;
		x_put_long (m68k_areg (regs, 7), oldpc);
		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), 0x2000 + nr * 4);

	} else if (regs.m && nr >= 24 && nr < 32) { /* M + Interrupt */

		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), nr * 4);
		m68k_areg (regs, 7) -= 4;
		x_put_long (m68k_areg (regs, 7), currpc);
		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), regs.sr);
		regs.sr |= (1 << 13);
		regs.msp = m68k_areg (regs, 7);
		m68k_areg (regs, 7) = regs.isp;
		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), 0x1000 + nr * 4);

	} else {

		m68k_areg (regs, 7) -= 2;
		x_put_word (m68k_areg (regs, 7), nr * 4);

	}
	m68k_areg (regs, 7) -= 4;
	x_put_long (m68k_areg (regs, 7), currpc);
	m68k_areg (regs, 7) -= 2;
	x_put_word (m68k_areg (regs, 7), regs.sr);
kludge_me_do:
	newpc = x_get_long (regs.vbr + 4 * nr);
	if (newpc & 1) {
		if (nr == 2 || nr == 3)
			Reset_Cold();  /* there is nothing else we can do.. */
		else
			exception3 (regs.ir, m68k_getpc (), newpc);
		return;
	}
	m68k_setpc (newpc);
#ifdef JIT
	set_special (SPCFLAG_END_COMPILE);
#endif
	fill_prefetch_slow ();
	exception_trace (nr);
}

/* Handle exceptions - non-MMU mode */
static void Exception_normal (int nr, uaecptr oldpc, int ExceptionSource)
{
	uae_u32 currpc = m68k_getpc (), newpc;
	int sv = regs.s;

	if (ExceptionSource == M68000_EXC_SRC_CPU) {
		if (nr == 0x22) {
			/* Intercept VDI & AES exceptions (Trap #2) */
			if (bVdiAesIntercept && VDI_AES_Entry()) {
				/* Set 'PC' to address of 'VDI_OPCODE' illegal instruction.
				 * This will call OpCode_VDI() after completion of Trap call!
				 * This is used to modify specific VDI return vectors contents.
				*/
				VDI_OldPC = currpc;
				currpc = CART_VDI_OPCODE_ADDR;
			}
		}
		else if (nr == 0x2d) {
			/* Intercept BIOS (Trap #13) calls */
			if (Bios())  return;
		}
		else if (nr == 0x2e) {
			/* Intercept XBIOS (Trap #14) calls */
			if (XBios())  return;
		}
	}

#if AMIGA_ONLY
	if (nr >= 24 && nr < 24 + 8 && currprefs.cpu_model <= 68010)
		nr = x_get_byte (0x00fffff1 | (nr << 1));
#endif

	exception_debug (nr);
	MakeSR ();

	/* Change to supervisor mode if necessary */
	if (!regs.s) {
		regs.usp = m68k_areg (regs, 7);
		if (currprefs.cpu_model >= 68020)
			m68k_areg (regs, 7) = regs.m ? regs.msp : regs.isp;
		else
			m68k_areg (regs, 7) = regs.isp;
		regs.s = 1;
		if (currprefs.mmu_model)
			mmu_set_super (regs.s != 0);
	}
	if (currprefs.cpu_model > 68000) {
		/* Build additional exception stack frame for 68010 and higher */
		/* (special case for MFP) */
		if (ExceptionSource == M68000_EXC_SRC_INT_MFP || ExceptionSource == M68000_EXC_SRC_INT_DSP) {
			m68k_areg(regs, 7) -= 2;
			put_word (m68k_areg(regs, 7), nr * 4);	/* MFP interrupt, 'nr' can be in a different range depending on $fffa17 */
		}
		else if (nr == 2 || nr == 3) {
			int i;
			if (currprefs.cpu_model >= 68040) {
				if (nr == 2) {
					// bus error
					if (currprefs.mmu_model) {

						for (i = 0 ; i < 7 ; i++) {
							m68k_areg (regs, 7) -= 4;
							x_put_long (m68k_areg (regs, 7), 0);
						}
						m68k_areg (regs, 7) -= 4;
						x_put_long (m68k_areg (regs, 7), regs.wb3_data);
						m68k_areg (regs, 7) -= 4;
						x_put_long (m68k_areg (regs, 7), regs.mmu_fault_addr);
						m68k_areg (regs, 7) -= 4;
						x_put_long (m68k_areg (regs, 7), regs.mmu_fault_addr);
						m68k_areg (regs, 7) -= 2;
						x_put_word (m68k_areg (regs, 7), 0);
						m68k_areg (regs, 7) -= 2;
						x_put_word (m68k_areg (regs, 7), 0);
						m68k_areg (regs, 7) -= 2;
						x_put_word (m68k_areg (regs, 7), regs.wb3_status);
						regs.wb3_status = 0;
						m68k_areg (regs, 7) -= 2;
						x_put_word (m68k_areg (regs, 7), regs.mmu_ssw);
						m68k_areg (regs, 7) -= 4;
						x_put_long (m68k_areg (regs, 7), regs.mmu_fault_addr);

						m68k_areg (regs, 7) -= 2;
						x_put_word (m68k_areg (regs, 7), 0x7000 + nr * 4);
						m68k_areg (regs, 7) -= 4;
						x_put_long (m68k_areg (regs, 7), oldpc);
						m68k_areg (regs, 7) -= 2;
						x_put_word (m68k_areg (regs, 7), regs.sr);
						newpc = x_get_long (regs.vbr + 4 * nr);
						if (newpc & 1) {
							if (nr == 2 || nr == 3)
								uae_reset (1); /* there is nothing else we can do.. */
							else
								exception3 (regs.ir, m68k_getpc (), newpc);
							return;
						}
						m68k_setpc (newpc);
#ifdef JIT
						set_special (SPCFLAG_END_COMPILE);
#endif
						exception_trace (nr);
						return;

					} else {

						for (i = 0 ; i < 18 ; i++) {
							m68k_areg (regs, 7) -= 2;
							x_put_word (m68k_areg (regs, 7), 0);
						}
						m68k_areg (regs, 7) -= 4;
						x_put_long (m68k_areg (regs, 7), last_fault_for_exception_3);
						m68k_areg (regs, 7) -= 2;
						x_put_word (m68k_areg (regs, 7), 0);
						m68k_areg (regs, 7) -= 2;
						x_put_word (m68k_areg (regs, 7), 0);
						m68k_areg (regs, 7) -= 2;
						x_put_word (m68k_areg (regs, 7), 0);
						m68k_areg (regs, 7) -= 2;
						x_put_word (m68k_areg (regs, 7), 0x0140 | (sv ? 6 : 2)); /* SSW */
						m68k_areg (regs, 7) -= 4;
						x_put_long (m68k_areg (regs, 7), last_addr_for_exception_3);
						m68k_areg (regs, 7) -= 2;
						x_put_word (m68k_areg (regs, 7), 0x7000 + nr * 4);
						m68k_areg (regs, 7) -= 4;
						x_put_long (m68k_areg (regs, 7), oldpc);
						m68k_areg (regs, 7) -= 2;
						x_put_word (m68k_areg (regs, 7), regs.sr);
						goto kludge_me_do;

					}

				} else {
					m68k_areg (regs, 7) -= 4;
					x_put_long (m68k_areg (regs, 7), last_fault_for_exception_3);
					m68k_areg (regs, 7) -= 2;
					x_put_word (m68k_areg (regs, 7), 0x2000 + nr * 4);
				}
			} else {
				// address error
				uae_u16 ssw = (sv ? 4 : 0) | (last_instructionaccess_for_exception_3 ? 2 : 1);
				ssw |= last_writeaccess_for_exception_3 ? 0 : 0x40;
				ssw |= 0x20;
				for (i = 0 ; i < 36; i++) {
					m68k_areg (regs, 7) -= 2;
					x_put_word (m68k_areg (regs, 7), 0);
				}
				m68k_areg (regs, 7) -= 4;
				x_put_long (m68k_areg (regs, 7), last_fault_for_exception_3);
				m68k_areg (regs, 7) -= 2;
				x_put_word (m68k_areg (regs, 7), 0);
				m68k_areg (regs, 7) -= 2;
				x_put_word (m68k_areg (regs, 7), 0);
				m68k_areg (regs, 7) -= 2;
				x_put_word (m68k_areg (regs, 7), 0);
				m68k_areg (regs, 7) -= 2;
				x_put_word (m68k_areg (regs, 7), ssw);
				m68k_areg (regs, 7) -= 2;
				x_put_word (m68k_areg (regs, 7), 0xb000 + nr * 4);
			}
			write_log ("Exception %d (%x) at %x -> %x!\n", nr, oldpc, currpc, STMemory_ReadLong(regs.vbr + 4*nr));
		} else if (nr ==5 || nr == 6 || nr == 7 || nr == 9) {
			m68k_areg (regs, 7) -= 4;
			x_put_long (m68k_areg (regs, 7), oldpc);
			m68k_areg (regs, 7) -= 2;
			x_put_word (m68k_areg (regs, 7), 0x2000 + nr * 4);
		} else if (regs.m && nr >= 24 && nr < 32) { /* M + Interrupt */
			m68k_areg (regs, 7) -= 2;
			x_put_word (m68k_areg (regs, 7), nr * 4);
			m68k_areg (regs, 7) -= 4;
			x_put_long (m68k_areg (regs, 7), currpc);
			m68k_areg (regs, 7) -= 2;
			x_put_word (m68k_areg (regs, 7), regs.sr);
			regs.sr |= (1 << 13);
			regs.msp = m68k_areg (regs, 7);
			m68k_areg (regs, 7) = regs.isp;
			m68k_areg (regs, 7) -= 2;
			x_put_word (m68k_areg (regs, 7), 0x1000 + nr * 4);
		} else {
			m68k_areg (regs, 7) -= 2;
			x_put_word (m68k_areg (regs, 7), nr * 4);
		}
	} else if (nr == 2 || nr == 3) {
		uae_u16 mode = (sv ? 4 : 0) | (last_instructionaccess_for_exception_3 ? 2 : 1);
		mode |= last_writeaccess_for_exception_3 ? 0 : 16;
		m68k_areg (regs, 7) -= 14;
		/* fixme: bit3=I/N */
		x_put_word (m68k_areg (regs, 7) + 0, mode);
		x_put_long (m68k_areg (regs, 7) + 2, last_fault_for_exception_3);
		x_put_word (m68k_areg (regs, 7) + 6, last_op_for_exception_3);
		x_put_word (m68k_areg (regs, 7) + 8, regs.sr);
		x_put_long (m68k_areg (regs, 7) + 10, last_addr_for_exception_3);
		write_log ("Exception %d (%x) at %x -> %x!\n", nr, oldpc, currpc, STMemory_ReadLong(regs.vbr + 4*nr));
		goto kludge_me_do;
	}

	/* Push PC on stack: */
	m68k_areg (regs, 7) -= 4;
	x_put_long (m68k_areg (regs, 7), currpc);
	/* Push SR on stack: */
 	m68k_areg (regs, 7) -= 2;
	x_put_word (m68k_areg (regs, 7), regs.sr);
kludge_me_do:
	newpc = x_get_long (regs.vbr + 4 * nr);
	if (newpc & 1) {
		if (nr == 2 || nr == 3)
			uae_reset (1); /* there is nothing else we can do.. */
		else
			exception3 (regs.ir, m68k_getpc (), newpc);
		return;
	}
	m68k_setpc (newpc);
#ifdef JIT
	set_special (SPCFLAG_END_COMPILE);
#endif
	fill_prefetch_slow ();
	exception_trace (nr);

	/* Handle exception cycles (special case for MFP) */
	if (ExceptionSource == M68000_EXC_SRC_INT_MFP) {
		M68000_AddCycles(44+12-CPU_IACK_CYCLES_MFP);	/* MFP interrupt, 'nr' can be in a different range depending on $fffa17 */
	}
	else if (nr >= 24 && nr <= 31) {
		if ( nr == 26 )					/* HBL */
			M68000_AddCycles(44+12-CPU_IACK_CYCLES_VIDEO);	/* Video Interrupt */
		else if ( nr == 28 ) 				/* VBL */
			M68000_AddCycles(44+12-CPU_IACK_CYCLES_VIDEO);	/* Video Interrupt */
		else
			M68000_AddCycles(44+4);			/* Other Interrupts */
		}
	else if(nr >= 32 && nr <= 47) {
		M68000_AddCycles(34-4);				/* Trap (total is 34, but cpuemu.c already adds 4) */
	}
	else switch(nr) {
		case 2: M68000_AddCycles(50); break;		/* Bus error */
		case 3: M68000_AddCycles(50); break;		/* Address error */
		case 4: M68000_AddCycles(34); break;		/* Illegal instruction */
		case 5: M68000_AddCycles(38); break;		/* Div by zero */
		case 6: M68000_AddCycles(40); break;		/* CHK */
		case 7: M68000_AddCycles(34); break;		/* TRAPV */
		case 8: M68000_AddCycles(34); break;		/* Privilege violation */
		case 9: M68000_AddCycles(34); break;		/* Trace */
		case 10: M68000_AddCycles(34); break;		/* Line-A - probably wrong */
		case 11: M68000_AddCycles(34); break;		/* Line-F - probably wrong */
		default:
		/* FIXME: Add right cycles value for MFP interrupts and copro exceptions ... */
		if(nr < 64)
			M68000_AddCycles(4);			/* Coprocessor and unassigned exceptions (???) */
		else
			M68000_AddCycles(44+12);		/* Must be a MFP or DSP interrupt */
		break;
	}
}


/* Handle exceptions. We need a special case to handle MFP exceptions */
/* on Atari ST, because it's possible to change the MFP's vector base */
/* and get a conflict with 'normal' cpu exceptions. */
void REGPARAM2 Exception (int nr, uaecptr oldpc, int ExceptionSource)
{
	/* Pending bits / vector number can change before the end of the IACK sequence. */
	/* We need to handle MFP and HBL/VBL cases for this. */
	if (ExceptionSource == M68000_EXC_SRC_INT_MFP)
	{
		M68000_AddCycles(CPU_IACK_CYCLES_MFP);
		CPU_IACK = true;
		while (PendingInterruptCount <= 0 && PendingInterruptFunction)
			CALL_VAR(PendingInterruptFunction);
		nr = MFP_ProcessIACK(nr);
		CPU_IACK = false;
	}
	else if (ExceptionSource == M68000_EXC_SRC_AUTOVEC && (nr == 26 || nr == 28))
	{
		M68000_AddCycles(CPU_IACK_CYCLES_VIDEO);
		CPU_IACK = true;
		while (PendingInterruptCount <= 0 && PendingInterruptFunction)
			CALL_VAR(PendingInterruptFunction);
		if (MFP_UpdateNeeded == true)
			MFP_UpdateIRQ(0);			/* update MFP's state if some internal timers related to MFP expired */
		pendingInterrupts &= ~(1 << (nr - 24));		/* clear HBL or VBL pending bit */
		CPU_IACK = false;
	}

#ifdef CPUEMU_12
	if (currprefs.cpu_cycle_exact && currprefs.cpu_model == 68000)
		Exception_ce000 (nr, oldpc);
	else
#endif
		if (currprefs.mmu_model)
			Exception_mmu (nr, oldpc); // Todo: add ExceptionSource
		else
			Exception_normal (nr, oldpc, ExceptionSource);

#if AMIGA_ONLY
	if (debug_illegal && !in_rom (M68K_GETPC)) {
		int v = nr;
		if (nr <= 63 && (debug_illegal_mask & ((uae_u64)1 << nr))) {
			write_log ("Exception %d breakpoint\n", nr);
			activate_debugger ();
		}
	}
#endif
}

STATIC_INLINE void do_interrupt (int nr, int Pending)
{
#if AMIGA_ONLY
	if (debug_dma)
		record_dma_event (DMA_EVENT_CPUIRQ, current_hpos (), vpos);
#endif

	regs.stopped = 0;
	unset_special (SPCFLAG_STOP);
	assert (nr < 8 && nr >= 0);

	/* On Hatari, only video ints are using SPCFLAG_INT (see m68000.c) */
	Exception (nr + 24, 0, M68000_EXC_SRC_AUTOVEC);

	regs.intmask = nr;
	doint ();

	set_special (SPCFLAG_INT);
	/* Handle Atari ST's specific jitter for hbl/vbl */
	InterruptAddJitter (nr , Pending);
}


void NMI (void)
{
	do_interrupt (7, false);
}

#ifndef CPUEMU_68000_ONLY

int movec_illg (int regno)
{
	int regno2 = regno & 0x7ff;

	if (currprefs.cpu_model == 68060) {
		if (regno <= 8)
			return 0;
		if (regno == 0x800 || regno == 0x801 ||
			regno == 0x806 || regno == 0x807 || regno == 0x808)
			return 0;
		return 1;
	} else if (currprefs.cpu_model == 68010) {
		if (regno2 < 2)
			return 0;
		return 1;
	} else if (currprefs.cpu_model == 68020) {
		if (regno == 3)
			return 1; /* 68040/060 only */
		/* 4 is >=68040, but 0x804 is in 68020 */
		if (regno2 < 4 || regno == 0x804)
			return 0;
		return 1;
	} else if (currprefs.cpu_model == 68030) {
		if (regno2 <= 2)
			return 0;
		if (regno == 0x803 || regno == 0x804)
			return 0;
		return 1;
	} else if (currprefs.cpu_model == 68040) {
		if (regno == 0x802)
			return 1; /* 68020 only */
		if (regno2 < 8) return 0;
		return 1;
	}
	return 1;
}

int m68k_move2c (int regno, uae_u32 *regp)
{
#if MOVEC_DEBUG > 0
	write_log ("move2c %04X <- %08X PC=%x\n", regno, *regp, M68K_GETPC);
#endif
	if (movec_illg (regno)) {
		op_illg (0x4E7B);
		return 0;
	} else {
		switch (regno) {
		case 0: regs.sfc = *regp & 7; break;
		case 1: regs.dfc = *regp & 7; break;
		case 2:
			{
				uae_u32 cacr_mask = 0;
				if (currprefs.cpu_model == 68020)
					cacr_mask = 0x0000000f;
				else if (currprefs.cpu_model == 68030)
					cacr_mask = 0x00003f1f;
				else if (currprefs.cpu_model == 68040)
					cacr_mask = 0x80008000;
				else if (currprefs.cpu_model == 68060)
					cacr_mask = 0xf8e0e000;
				regs.cacr = *regp & cacr_mask;
				set_cpu_caches ();
			}
			break;
			/* 68040/060 only */
		case 3:
			regs.tcr = *regp & (currprefs.cpu_model == 68060 ? 0xfffe : 0xc000);
			if (currprefs.mmu_model)
				mmu_set_tc (regs.tcr);
			break;

			/* no differences between 68040 and 68060 */
		case 4: regs.itt0 = *regp & 0xffffe364; break;
		case 5: regs.itt1 = *regp & 0xffffe364; break;
		case 6: regs.dtt0 = *regp & 0xffffe364; break;
		case 7: regs.dtt1 = *regp & 0xffffe364; break;
			/* 68060 only */
		case 8: regs.buscr = *regp & 0xf0000000; break;

		case 0x800: regs.usp = *regp; break;
		case 0x801: regs.vbr = *regp; break;
		case 0x802: regs.caar = *regp; break;
		case 0x803: regs.msp = *regp; if (regs.m == 1) m68k_areg (regs, 7) = regs.msp; break;
		case 0x804: regs.isp = *regp; if (regs.m == 0) m68k_areg (regs, 7) = regs.isp; break;
			/* 68040 only */
		case 0x805: regs.mmusr = *regp; break;
			/* 68040/060 */
		case 0x806: regs.urp = *regp & 0xfffffe00; break;
		case 0x807: regs.srp = *regp & 0xfffffe00; break;
			/* 68060 only */
		case 0x808:
			{
				uae_u32 opcr = regs.pcr;
				regs.pcr &= ~(0x40 | 2 | 1);
				regs.pcr |= (*regp) & (0x40 | 2 | 1);
				if (((opcr ^ regs.pcr) & 2) == 2) {
					write_log ("68060 FPU state: %s\n", regs.pcr & 2 ? "disabled" : "enabled");
					/* flush possible already translated FPU instructions */
					flush_icache (0, 3);
				}
			}
			break;
		default:
			op_illg (0x4E7B);
			return 0;
		}
	}
	return 1;
}

int m68k_movec2 (int regno, uae_u32 *regp)
{
#if MOVEC_DEBUG > 0
	write_log ("movec2 %04X PC=%x\n", regno, M68K_GETPC);
#endif
	if (movec_illg (regno)) {
		op_illg (0x4E7A);
		return 0;
	} else {
		switch (regno) {
		case 0: *regp = regs.sfc; break;
		case 1: *regp = regs.dfc; break;
		case 2:
			{
				uae_u32 v = regs.cacr;
				uae_u32 cacr_mask = 0;
				if (currprefs.cpu_model == 68020)
					cacr_mask = 0x00000003;
				else if (currprefs.cpu_model == 68030)
					cacr_mask = 0x00003313;
				else if (currprefs.cpu_model == 68040)
					cacr_mask = 0x80008000;
				else if (currprefs.cpu_model == 68060)
					cacr_mask = 0xf880e000;
				*regp = v & cacr_mask;
			}
			break;
		case 3: *regp = regs.tcr; break;
		case 4: *regp = regs.itt0; break;
		case 5: *regp = regs.itt1; break;
		case 6: *regp = regs.dtt0; break;
		case 7: *regp = regs.dtt1; break;
		case 8: *regp = regs.buscr; break;

		case 0x800: *regp = regs.usp; break;
		case 0x801: *regp = regs.vbr; break;
		case 0x802: *regp = regs.caar; break;
		case 0x803: *regp = regs.m == 1 ? m68k_areg (regs, 7) : regs.msp; break;
		case 0x804: *regp = regs.m == 0 ? m68k_areg (regs, 7) : regs.isp; break;
		case 0x805: *regp = regs.mmusr; break;
		case 0x806: *regp = regs.urp; break;
		case 0x807: *regp = regs.srp; break;
		case 0x808: *regp = regs.pcr; break;

		default:
			op_illg (0x4E7A);
			return 0;
		}
	}
#if MOVEC_DEBUG > 0
	write_log ("-> %08X\n", *regp);
#endif
	return 1;
}

STATIC_INLINE int div_unsigned (uae_u32 src_hi, uae_u32 src_lo, uae_u32 div, uae_u32 *quot, uae_u32 *rem)
{
	uae_u32 q = 0, cbit = 0;
	int i;

	if (div <= src_hi) {
		return 1;
	}
	for (i = 0 ; i < 32 ; i++) {
		cbit = src_hi & 0x80000000ul;
		src_hi <<= 1;
		if (src_lo & 0x80000000ul) src_hi++;
		src_lo <<= 1;
		q = q << 1;
		if (cbit || div <= src_hi) {
			q |= 1;
			src_hi -= div;
		}
	}
	*quot = q;
	*rem = src_hi;
	return 0;
}

void m68k_divl (uae_u32 opcode, uae_u32 src, uae_u16 extra, uaecptr oldpc)
{
#if defined (uae_s64)
	if (src == 0) {
		Exception (5, oldpc, M68000_EXC_SRC_CPU);
		return;
	}
	if (extra & 0x800) {
		/* signed variant */
		uae_s64 a = (uae_s64)(uae_s32)m68k_dreg (regs, (extra >> 12) & 7);
		uae_s64 quot, rem;

		if (extra & 0x400) {
			a &= 0xffffffffu;
			a |= (uae_s64)m68k_dreg (regs, extra & 7) << 32;
		}
		rem = a % (uae_s64)(uae_s32)src;
		quot = a / (uae_s64)(uae_s32)src;
		if ((quot & UVAL64 (0xffffffff80000000)) != 0
			&& (quot & UVAL64 (0xffffffff80000000)) != UVAL64 (0xffffffff80000000))
		{
			SET_VFLG (1);
			SET_NFLG (1);
			SET_CFLG (0);
		} else {
			if (((uae_s32)rem < 0) != ((uae_s64)a < 0)) rem = -rem;
			SET_VFLG (0);
			SET_CFLG (0);
			SET_ZFLG (((uae_s32)quot) == 0);
			SET_NFLG (((uae_s32)quot) < 0);
			m68k_dreg (regs, extra & 7) = (uae_u32)rem;
			m68k_dreg (regs, (extra >> 12) & 7) = (uae_u32)quot;
		}
	} else {
		/* unsigned */
		uae_u64 a = (uae_u64)(uae_u32)m68k_dreg (regs, (extra >> 12) & 7);
		uae_u64 quot, rem;

		if (extra & 0x400) {
			a &= 0xffffffffu;
			a |= (uae_u64)m68k_dreg (regs, extra & 7) << 32;
		}
		rem = a % (uae_u64)src;
		quot = a / (uae_u64)src;
		if (quot > 0xffffffffu) {
			SET_VFLG (1);
			SET_NFLG (1);
			SET_CFLG (0);
		} else {
			SET_VFLG (0);
			SET_CFLG (0);
			SET_ZFLG (((uae_s32)quot) == 0);
			SET_NFLG (((uae_s32)quot) < 0);
			m68k_dreg (regs, extra & 7) = (uae_u32)rem;
			m68k_dreg (regs, (extra >> 12) & 7) = (uae_u32)quot;
		}
	}
#else
	if (src == 0) {
		Exception (5, oldpc, M68000_EXC_SRC_CPU);
		return;
	}
	if (extra & 0x800) {
		/* signed variant */
		uae_s32 lo = (uae_s32)m68k_dreg (regs, (extra >> 12) & 7);
		uae_s32 hi = lo < 0 ? -1 : 0;
		uae_s32 save_high;
		uae_u32 quot, rem;
		uae_u32 sign;

		if (extra & 0x400) {
			hi = (uae_s32)m68k_dreg (regs, extra & 7);
		}
		save_high = hi;
		sign = (hi ^ src);
		if (hi < 0) {
			hi = ~hi;
			lo = -lo;
			if (lo == 0) hi++;
		}
		if ((uae_s32)src < 0) src = -src;
		if (div_unsigned (hi, lo, src, &quot, &rem) ||
			(sign & 0x80000000) ? quot > 0x80000000 : quot > 0x7fffffff) {
				SET_VFLG (1);
				SET_NFLG (1);
				SET_CFLG (0);
		} else {
			if (sign & 0x80000000) quot = -quot;
			if (((uae_s32)rem < 0) != (save_high < 0)) rem = -rem;
			SET_VFLG (0);
			SET_CFLG (0);
			SET_ZFLG (((uae_s32)quot) == 0);
			SET_NFLG (((uae_s32)quot) < 0);
			m68k_dreg (regs, extra & 7) = rem;
			m68k_dreg (regs, (extra >> 12) & 7) = quot;
		}
	} else {
		/* unsigned */
		uae_u32 lo = (uae_u32)m68k_dreg (regs, (extra >> 12) & 7);
		uae_u32 hi = 0;
		uae_u32 quot, rem;

		if (extra & 0x400) {
			hi = (uae_u32)m68k_dreg (regs, extra & 7);
		}
		if (div_unsigned (hi, lo, src, &quot, &rem)) {
			SET_VFLG (1);
			SET_NFLG (1);
			SET_CFLG (0);
		} else {
			SET_VFLG (0);
			SET_CFLG (0);
			SET_ZFLG (((uae_s32)quot) == 0);
			SET_NFLG (((uae_s32)quot) < 0);
			m68k_dreg (regs, extra & 7) = rem;
			m68k_dreg (regs, (extra >> 12) & 7) = quot;
		}
	}
#endif
}

STATIC_INLINE void mul_unsigned (uae_u32 src1, uae_u32 src2, uae_u32 *dst_hi, uae_u32 *dst_lo)
{
	uae_u32 r0 = (src1 & 0xffff) * (src2 & 0xffff);
	uae_u32 r1 = ((src1 >> 16) & 0xffff) * (src2 & 0xffff);
	uae_u32 r2 = (src1 & 0xffff) * ((src2 >> 16) & 0xffff);
	uae_u32 r3 = ((src1 >> 16) & 0xffff) * ((src2 >> 16) & 0xffff);
	uae_u32 lo;

	lo = r0 + ((r1 << 16) & 0xffff0000ul);
	if (lo < r0) r3++;
	r0 = lo;
	lo = r0 + ((r2 << 16) & 0xffff0000ul);
	if (lo < r0) r3++;
	r3 += ((r1 >> 16) & 0xffff) + ((r2 >> 16) & 0xffff);
	*dst_lo = lo;
	*dst_hi = r3;
}

void m68k_mull (uae_u32 opcode, uae_u32 src, uae_u16 extra)
{
#if defined (uae_s64)
	if (extra & 0x800) {
		/* signed variant */
		uae_s64 a = (uae_s64)(uae_s32)m68k_dreg (regs, (extra >> 12) & 7);

		a *= (uae_s64)(uae_s32)src;
		SET_VFLG (0);
		SET_CFLG (0);
		SET_ZFLG (a == 0);
		SET_NFLG (a < 0);
		if (extra & 0x400)
			m68k_dreg (regs, extra & 7) = (uae_u32)(a >> 32);
		else if ((a & UVAL64 (0xffffffff80000000)) != 0
			&& (a & UVAL64 (0xffffffff80000000)) != UVAL64 (0xffffffff80000000))
		{
			SET_VFLG (1);
		}
		m68k_dreg (regs, (extra >> 12) & 7) = (uae_u32)a;
	} else {
		/* unsigned */
		uae_u64 a = (uae_u64)(uae_u32)m68k_dreg (regs, (extra >> 12) & 7);

		a *= (uae_u64)src;
		SET_VFLG (0);
		SET_CFLG (0);
		SET_ZFLG (a == 0);
		SET_NFLG (((uae_s64)a) < 0);
		if (extra & 0x400)
			m68k_dreg (regs, extra & 7) = (uae_u32)(a >> 32);
		else if ((a & UVAL64 (0xffffffff00000000)) != 0) {
			SET_VFLG (1);
		}
		m68k_dreg (regs, (extra >> 12) & 7) = (uae_u32)a;
	}
#else
	if (extra & 0x800) {
		/* signed variant */
		uae_s32 src1, src2;
		uae_u32 dst_lo, dst_hi;
		uae_u32 sign;

		src1 = (uae_s32)src;
		src2 = (uae_s32)m68k_dreg (regs, (extra >> 12) & 7);
		sign = (src1 ^ src2);
		if (src1 < 0) src1 = -src1;
		if (src2 < 0) src2 = -src2;
		mul_unsigned ((uae_u32)src1, (uae_u32)src2, &dst_hi, &dst_lo);
		if (sign & 0x80000000) {
			dst_hi = ~dst_hi;
			dst_lo = -dst_lo;
			if (dst_lo == 0) dst_hi++;
		}
		SET_VFLG (0);
		SET_CFLG (0);
		SET_ZFLG (dst_hi == 0 && dst_lo == 0);
		SET_NFLG (((uae_s32)dst_hi) < 0);
		if (extra & 0x400)
			m68k_dreg (regs, extra & 7) = dst_hi;
		else if ((dst_hi != 0 || (dst_lo & 0x80000000) != 0)
			&& ((dst_hi & 0xffffffff) != 0xffffffff
			|| (dst_lo & 0x80000000) != 0x80000000))
		{
			SET_VFLG (1);
		}
		m68k_dreg (regs, (extra >> 12) & 7) = dst_lo;
	} else {
		/* unsigned */
		uae_u32 dst_lo, dst_hi;

		mul_unsigned (src, (uae_u32)m68k_dreg (regs, (extra >> 12) & 7), &dst_hi, &dst_lo);

		SET_VFLG (0);
		SET_CFLG (0);
		SET_ZFLG (dst_hi == 0 && dst_lo == 0);
		SET_NFLG (((uae_s32)dst_hi) < 0);
		if (extra & 0x400)
			m68k_dreg (regs, extra & 7) = dst_hi;
		else if (dst_hi != 0) {
			SET_VFLG (1);
		}
		m68k_dreg (regs, (extra >> 12) & 7) = dst_lo;
	}
#endif
}

#endif

void m68k_reset (int hardreset)
{
	regs.spcflags &= SPCFLAG_MODE_CHANGE | SPCFLAG_BRK;
	regs.ipl = regs.ipl_pin = 0;
#ifdef SAVESTATE
	if (savestate_state == STATE_RESTORE || savestate_state == STATE_REWIND) {
		m68k_setpc (regs.pc);
		SET_XFLG ((regs.sr >> 4) & 1);
		SET_NFLG ((regs.sr >> 3) & 1);
		SET_ZFLG ((regs.sr >> 2) & 1);
		SET_VFLG ((regs.sr >> 1) & 1);
		SET_CFLG (regs.sr & 1);
		regs.t1 = (regs.sr >> 15) & 1;
		regs.t0 = (regs.sr >> 14) & 1;
		regs.s  = (regs.sr >> 13) & 1;
		regs.m  = (regs.sr >> 12) & 1;
		regs.intmask = (regs.sr >> 8) & 7;
		/* set stack pointer */
		if (regs.s)
			m68k_areg (regs, 7) = regs.isp;
		else
			m68k_areg (regs, 7) = regs.usp;
		return;
	}
#endif
	regs.s = 1;
	regs.m = 0;
	regs.stopped = 0;
	regs.t1 = 0;
	regs.t0 = 0;
	SET_ZFLG (0);
	SET_XFLG (0);
	SET_CFLG (0);
	SET_VFLG (0);
	SET_NFLG (0);
	regs.intmask = 7;
	regs.vbr = regs.sfc = regs.dfc = 0;
	regs.irc = 0xffff;

	m68k_areg (regs, 7) = get_long (0);
	m68k_setpc (get_long (4));

#ifdef FPUEMU
	fpu_reset ();
#endif
	regs.caar = regs.cacr = 0;
	regs.itt0 = regs.itt1 = regs.dtt0 = regs.dtt1 = 0;
	regs.tcr = regs.mmusr = regs.urp = regs.srp = regs.buscr = 0;
	if (currprefs.cpu_model == 68020) {
		regs.cacr |= 8;
		set_cpu_caches ();
	}

	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
	if (currprefs.mmu_model) {
		if (currprefs.cpu_model >= 68040) {
			mmu_reset ();
			mmu_set_tc (regs.tcr);
			mmu_set_super (regs.s != 0);
		}
		else {
			mmu030_reset (hardreset);
		}
	}

	/* 68060 FPU is not compatible with 68040,
	* 68060 accelerators' boot ROM disables the FPU
	*/
	regs.pcr = 0;
	if (currprefs.cpu_model == 68060) {
		regs.pcr = currprefs.fpu_model == 68060 ? MC68060_PCR : MC68EC060_PCR;
		regs.pcr |= (currprefs.cpu060_revision & 0xff) << 8;
	}
	fill_prefetch_slow ();
}

unsigned long REGPARAM2 op_illg (uae_u32 opcode)
{
	uaecptr pc = m68k_getpc ();
	static int warned;

#if AMIGA_ONLY
	int inrom = in_rom (pc);
	int inrt = in_rtarea (pc);

	if (cloanto_rom && (opcode & 0xF100) == 0x7100) {
		m68k_dreg (regs, (opcode >> 9) & 7) = (uae_s8)(opcode & 0xFF);
		m68k_incpc (2);
		fill_prefetch_slow ();
		return 4;
	}

	if (opcode == 0x4E7B && inrom && get_long (0x10) == 0) {
		notify_user (NUMSG_KS68020);
		uae_restart (-1, NULL);
	}
#endif

#ifdef AUTOCONFIG
	if (opcode == 0xFF0D) {
		if (inrom) {
			/* This is from the dummy Kickstart replacement */
			uae_u16 arg = get_iword (2);
			m68k_incpc (4);
			ersatz_perform (arg);
			fill_prefetch_slow ();
			return 4;
		} else if (inrt) {
			/* User-mode STOP replacement */
			m68k_setstopped ();
			return 4;
		}
	}

	if ((opcode & 0xF000) == 0xA000 && inrt) {
		/* Calltrap. */
		m68k_incpc (2);
		m68k_handle_trap (opcode & 0xFFF);
		fill_prefetch_slow ();
		return 4;
	}
#endif

	if ((opcode & 0xF000) == 0xF000) {
		if (warned < 20) {
			write_log ("B-Trap %x at %x (%p)\n", opcode, pc, regs.pc_p);
			warned++;
		}
		Exception (0xB, 0, M68000_EXC_SRC_CPU);
		//activate_debugger ();
		return 4;
	}
	if ((opcode & 0xF000) == 0xA000) {
		if (warned < 20) {
			write_log ("A-Trap %x at %x (%p)\n", opcode, pc, regs.pc_p);
			warned++;
		}
		Exception (0xA, 0, M68000_EXC_SRC_CPU);
		//activate_debugger();
		return 4;
	}
	if (warned < 20) {
		write_log ("Illegal instruction: %04x at %08X -> %08X\n", opcode, pc, STMemory_ReadLong(regs.vbr + 0x10));
		warned++;
		//activate_debugger();
	}

	Exception (4, 0, M68000_EXC_SRC_CPU);
	return 4;
}

#ifdef CPUEMU_0

void mmu_op30 (uaecptr pc, uae_u32 opcode, uae_u16 extra, uaecptr extraa)
{
	if (currprefs.cpu_model != 68030) {
		m68k_setpc (pc);
		op_illg (opcode);
		return;
	}

	if (extra & 0x8000)
		mmu_op30_ptest (pc, opcode, extra, extraa);
	else if ((extra & 0xFC00) == 0x2000)
		mmu_op30_pload (pc, opcode, extra, extraa);
	else if ((extra & 0xE000) == 0x2000)
		mmu_op30_pflush (pc, opcode, extra, extraa);
	else
		mmu_op30_pmove (pc, opcode, extra, extraa);
}

void mmu_op (uae_u32 opcode, uae_u32 extra)
{
	if (currprefs.cpu_model) {
		mmu_op_real (opcode, extra);
		return;
	}
#if MMUOP_DEBUG > 1
	write_log ("mmu_op %04X PC=%08X\n", opcode, m68k_getpc ());
#endif
	if ((opcode & 0xFE0) == 0x0500) {
		/* PFLUSH */
		regs.mmusr = 0;
#if MMUOP_DEBUG > 0
		write_log ("PFLUSH\n");
#endif
		return;
	} else if ((opcode & 0x0FD8) == 0x548) {
		if (currprefs.cpu_model < 68060) { /* PTEST not in 68060 */
			/* PTEST */
#if MMUOP_DEBUG > 0
			write_log ("PTEST\n");
#endif
			return;
		}
	} else if ((opcode & 0x0FB8) == 0x588) {
		/* PLPA */
		if (currprefs.cpu_model == 68060) {
#if MMUOP_DEBUG > 0
			write_log ("PLPA\n");
#endif
			return;
		}
	}
#if MMUOP_DEBUG > 0
	write_log ("Unknown MMU OP %04X\n", opcode);
#endif
	m68k_setpc (m68k_getpc () - 2);
	op_illg (opcode);
}

#endif

static uaecptr last_trace_ad = 0;

static void do_trace (void)
{
	if (regs.t0 && currprefs.cpu_model >= 68020) {
		uae_u16 opcode;
		/* should also include TRAP, CHK, SR modification FPcc */
		/* probably never used so why bother */
		/* We can afford this to be inefficient... */
		m68k_setpc (m68k_getpc ());
		fill_prefetch_slow ();
		opcode = x_get_word (regs.pc);
		if (opcode == 0x4e73 			/* RTE */
			|| opcode == 0x4e74 		/* RTD */
			|| opcode == 0x4e75 		/* RTS */
			|| opcode == 0x4e77 		/* RTR */
			|| opcode == 0x4e76 		/* TRAPV */
			|| (opcode & 0xffc0) == 0x4e80 	/* JSR */
			|| (opcode & 0xffc0) == 0x4ec0 	/* JMP */
			|| (opcode & 0xff00) == 0x6100	/* BSR */
			|| ((opcode & 0xf000) == 0x6000	/* Bcc */
			&& cctrue ((opcode >> 8) & 0xf))
			|| ((opcode & 0xf0f0) == 0x5050	/* DBcc */
			&& !cctrue ((opcode >> 8) & 0xf)
			&& (uae_s16)m68k_dreg (regs, opcode & 7) != 0))
		{
			last_trace_ad = m68k_getpc ();
			unset_special (SPCFLAG_TRACE);
			set_special (SPCFLAG_DOTRACE);
		}
	} else if (regs.t1) {
		last_trace_ad = m68k_getpc ();
		unset_special (SPCFLAG_TRACE);
		set_special (SPCFLAG_DOTRACE);
	}
}


// handle interrupt delay (few cycles)
STATIC_INLINE int time_for_interrupt (void)
{
	if (regs.ipl > regs.intmask || regs.ipl == 7) {
#if 0
		if (regs.ipl == 3 && current_hpos () < 11) {
			write_log ("%d\n", current_hpos ());
			activate_debugger ();
		}
#endif
		return 1;
	}
	return 0;
}

void doint (void)
{
	if (currprefs.cpu_cycle_exact) {
		regs.ipl_pin = intlev ();
		set_special (SPCFLAG_INT);
		return;
	}
	if (currprefs.cpu_compatible)
		set_special (SPCFLAG_INT);
	else
		set_special (SPCFLAG_DOINT);
}

#define IDLETIME (currprefs.cpu_idle * sleep_resolution / 700)

/*
 * Compute the number of jitter cycles to add when a video interrupt occurs
 * (this is specific to the Atari ST)
 */
STATIC_INLINE void InterruptAddJitter (int Level , int Pending)
{
	int cycles = 0;

	if ( Level == 2 )				/* HBL */
	{
		if ( Pending )
			cycles = HblJitterArrayPending[ HblJitterIndex ];
		else
			cycles = HblJitterArray[ HblJitterIndex ];
	}
	else if ( Level == 4 )			/* VBL */
	{
		if ( Pending )
			cycles = VblJitterArrayPending[ VblJitterIndex ];
		else
			cycles = VblJitterArray[ VblJitterIndex ];
	}

	//fprintf ( stderr , "jitter %d\n" , cycles );
	//cycles=0;
	if ( cycles > 0 )				/* no need to call M68000_AddCycles if cycles == 0 */
		M68000_AddCycles ( cycles );
}


/*
 * Handle special flags
 */

static bool do_specialties_interrupt (int Pending)
{
#if ENABLE_DSP_EMU
    /* Check for DSP int first (if enabled) (level 6) */
    if (regs.spcflags & SPCFLAG_DSP) {
       if (DSP_ProcessIRQ() == true)
         return true;
    }
#endif

    /* Check for MFP ints (level 6) */
    if (regs.spcflags & SPCFLAG_MFP) {
       if (MFP_ProcessIRQ() == true)
         return true;					/* MFP exception was generated, no higher interrupt can happen */
    }

    /* No MFP int, check for VBL/HBL ints (levels 4/2) */
    if (regs.spcflags & (SPCFLAG_INT | SPCFLAG_DOINT)) {
	int intr = intlev ();
	/* SPCFLAG_DOINT will be enabled again in MakeFromSR to handle pending interrupts! */
//	unset_special (SPCFLAG_DOINT);
	unset_special (SPCFLAG_INT | SPCFLAG_DOINT);
	if (intr != -1 && intr > regs.intmask) {
	    do_interrupt (intr , Pending);			/* process the interrupt and add pending jitter if necessary */
	    return true;
	}
    }

    return false;					/* no interrupt was found */
}

STATIC_INLINE int do_specialties (int cycles)
{
#ifdef JIT
	unset_special (SPCFLAG_END_COMPILE);   /* has done its job */
#endif

#if AMIGA_ONLY
	while ((regs.spcflags & SPCFLAG_BLTNASTY) && dmaen (DMA_BLITTER) && cycles > 0 && !currprefs.blitter_cycle_exact) {
		/* Laurent : I don't know if our blitter code should be called here ! */
		int c = blitnasty ();
		if (c > 0) {
			cycles -= c * CYCLE_UNIT * 2;
			if (cycles < CYCLE_UNIT)
				cycles = 0;
		} else
			c = 4;
		do_cycles (c * CYCLE_UNIT);

		if (regs.spcflags & SPCFLAG_COPPER)
			do_copper ();
	}
#endif

	if (regs.spcflags & SPCFLAG_BUSERROR) {
		/* We can not execute bus errors directly in the memory handler
		* functions since the PC should point to the address of the next
		* instruction, so we're executing the bus errors here: */
		unset_special(SPCFLAG_BUSERROR);
		Exception(2, BusErrorPC, M68000_EXC_SRC_CPU);
	}

	if(regs.spcflags & SPCFLAG_EXTRA_CYCLES) {
		/* Add some extra cycles to simulate a wait state */
		unset_special(SPCFLAG_EXTRA_CYCLES);
		M68000_AddCycles(nWaitStateCycles);
		nWaitStateCycles = 0;
	}

	if (regs.spcflags & SPCFLAG_DOTRACE)
		Exception (9, last_trace_ad, M68000_EXC_SRC_CPU);

	if (regs.spcflags & SPCFLAG_TRAP) {
		unset_special (SPCFLAG_TRAP);
		Exception (3, 0, M68000_EXC_SRC_CPU);
	}

	/* Handle the STOP instruction */
	if ( regs.spcflags & SPCFLAG_STOP ) {
	    /* We first test if there's a pending interrupt that would */
	    /* allow to immediately leave the STOP state */
	    if ( do_specialties_interrupt(true) ) {		/* test if there's an interrupt and add pending jitter */
	        regs.stopped = 0;
	        unset_special (SPCFLAG_STOP);
	    }

	    while (regs.spcflags & SPCFLAG_STOP) {
		do_cycles (currprefs.cpu_cycle_exact ? 2 * CYCLE_UNIT : 4 * CYCLE_UNIT);
		M68000_AddCycles(4);

		/* It is possible one or more ints happen at the same time */
		/* We must process them during the same cpu cycle then choose the highest priority one */
		while ( ( PendingInterruptCount <= 0 ) && ( PendingInterruptFunction ) )
		    CALL_VAR(PendingInterruptFunction);
		if ( MFP_UpdateNeeded == true )
		    MFP_UpdateIRQ ( 0 );

		/* Check is there's an interrupt to process (could be a delayed MFP interrupt) */
		if ( do_specialties_interrupt(false) ) {	/* test if there's an interrupt and add non pending jitter */
		    regs.stopped = 0;
		    unset_special (SPCFLAG_STOP);
		}

#if AMIGA_ONLY
		if (regs.spcflags & SPCFLAG_COPPER)
			do_copper ();
#endif

		if (currprefs.cpu_cycle_exact) {
			ipl_fetch ();
			if (time_for_interrupt ()) {
				do_interrupt (regs.ipl, true);
			}
		} else {
#if 0
		if (regs.spcflags & (SPCFLAG_INT | SPCFLAG_DOINT)) {
			int intr = intlev ();
			unset_special (SPCFLAG_INT | SPCFLAG_DOINT);
			if (intr > 0 && intr > regs.intmask)
				do_interrupt (intr, true);
		}
#endif
		}
		if ((regs.spcflags & (SPCFLAG_BRK | SPCFLAG_MODE_CHANGE))) {
			unset_special (SPCFLAG_BRK | SPCFLAG_MODE_CHANGE);
			// SPCFLAG_BRK breaks STOP condition, need to prefetch
			m68k_resumestopped ();
			return 1;
		}

		if (currprefs.cpu_idle && currprefs.m68k_speed != 0 && ((regs.spcflags & SPCFLAG_STOP)) == SPCFLAG_STOP) {
			/* sleep 1ms if STOP-instruction is executed */
			if (1) {
				static int sleepcnt, lvpos, zerocnt;
					if (vpos != lvpos) {
						sleepcnt--;
#ifdef JIT
					if (pissoff == 0 && currprefs.cachesize && --zerocnt < 0) {
						sleepcnt = -1;
						zerocnt = IDLETIME / 4;
					}
#endif
					lvpos = vpos;
					if (sleepcnt < 0) {
						/*sleepcnt = IDLETIME / 2; */  /* Laurent : badly removed for now */
						sleep_millis (1);
					}
				}
			}
		}
	    }
	}

	if (regs.spcflags & SPCFLAG_TRACE)
		do_trace ();

	if (currprefs.cpu_cycle_exact) {
		if (time_for_interrupt ()) {
			do_interrupt (regs.ipl, true);
		}
	} else {
		if (regs.spcflags & SPCFLAG_INT) {
			int intr = intlev ();
			unset_special (SPCFLAG_INT | SPCFLAG_DOINT);
			if (intr > 0 && (intr > regs.intmask || intr == 7))
				do_interrupt (intr, false);		/* call do_interrupt() with Pending=false, not necessarily true but harmless */
		}
	}

	if (regs.spcflags & SPCFLAG_DOINT) {
		unset_special (SPCFLAG_DOINT);
		set_special (SPCFLAG_INT);
	}

	if ( do_specialties_interrupt(false) ) {	/* test if there's an interrupt and add non pending jitter */
		/* TODO: Always do do_specialties_interrupt() in m68k_run_x instead? */
		regs.stopped = 0;
	}

	if (regs.spcflags & SPCFLAG_DEBUGGER)
		DebugCpu_Check();

	if ((regs.spcflags & (SPCFLAG_BRK | SPCFLAG_MODE_CHANGE))) {
		unset_special(SPCFLAG_BRK | SPCFLAG_MODE_CHANGE);
		return 1;
	}
	return 0;
}

//static uae_u32 pcs[1000];

#if DEBUG_CD32CDTVIO

static uae_u32 cd32nextpc, cd32request;

static void out_cd32io2 (void)
{
	uae_u32 request = cd32request;
	write_log ("%08x returned\n", request);
	//write_log ("ACTUAL=%d ERROR=%d\n", get_long (request + 32), STMemory_ReadByte(request + 31));
	cd32nextpc = 0;
	cd32request = 0;
}

static void out_cd32io (uae_u32 pc)
{
	TCHAR out[100];
	int ioreq = 0;
	uae_u32 request = m68k_areg (regs, 1);

	if (pc == cd32nextpc) {
		out_cd32io2 ();
		return;
	}
	out[0] = 0;
	switch (pc)
	{
	case 0xe57cc0:
	case 0xf04c34:
		_stprintf (out, "opendevice");
		break;
	case 0xe57ce6:
	case 0xf04c56:
		_stprintf (out, "closedevice");
		break;
	case 0xe57e44:
	case 0xf04f2c:
		_stprintf (out, "beginio");
		ioreq = 1;
		break;
	case 0xe57ef2:
	case 0xf0500e:
		_stprintf (out, "abortio");
		ioreq = -1;
		break;
	}
	if (out[0] == 0)
		return;
	if (cd32request)
		write_log ("old request still not returned!\n");
	cd32request = request;
	cd32nextpc = get_long (m68k_areg (regs, 7));
	write_log ("%s A1=%08X\n", out, request);
	if (ioreq) {
		static int cnt = 0;
		int cmd = get_word (request + 28);
#if 0
		if (cmd == 37) {
			cnt--;
			if (cnt <= 0)
				activate_debugger ();
		}
#endif
		write_log ("CMD=%d DATA=%08X LEN=%d %OFF=%d PC=%x\n", cmd,
			   STMemory_ReadLong(request + 40),
			   STMemory_ReadLong(request + 36),
			   STMemory_ReadLong(request + 44), M68K_GETPC);
	}
	if (ioreq < 0)
		;//activate_debugger ();
}

#endif /* DEBUG_CD32CDTVIO */

#ifndef CPUEMU_11

static void m68k_run_1 (void)
{
}

#else

/* It's really sad to have two almost identical functions for this, but we
do it all for performance... :(
This version emulates 68000's prefetch "cache" */
static void m68k_run_1 (void)
{
	struct regstruct *r = &regs;

	for (;;) {
		uae_u32 opcode = r->ir;

		count_instr (opcode);

		/*m68k_dumpstate(stderr, NULL);*/
		if (LOG_TRACE_LEVEL(TRACE_CPU_DISASM))
		{
			int FrameCycles, HblCounterVideo, LineCycles;

			Video_GetPosition ( &FrameCycles , &HblCounterVideo , &LineCycles );

			LOG_TRACE_PRINT ( "cpu video_cyc=%6d %3d@%3d : " , FrameCycles, LineCycles, HblCounterVideo );
			m68k_disasm(stderr, m68k_getpc (), NULL, 1);
		}

#if DEBUG_CD32CDTVIO
		out_cd32io (m68k_getpc ());
#endif

#if 0
		int pc = m68k_getpc ();
		if (pc == 0xdff002)
			write_log ("hip\n");
		if (pc != pcs[0] && (pc < 0xd00000 || pc > 0x1000000)) {
			memmove (pcs + 1, pcs, 998 * 4);
			pcs[0] = pc;
			//write_log ("%08X-%04X ", pc, opcode);
		}
#endif
		/* In case of a Bus Error, we need the PC of the instruction
		 * that caused  the error to build the exception stack frame */
		BusErrorPC = m68k_getpc();

		do_cycles (cpu_cycles);
		cpu_cycles = (*cpufunctbl[opcode])(opcode);
		cpu_cycles &= cycles_mask;
		cpu_cycles |= cycles_val;

		M68000_AddCyclesWithPairing(cpu_cycles * 2 / CYCLE_UNIT);

		/* We can have several interrupts at the same time before the next CPU instruction */
		/* We must check for pending interrupt and call do_specialties_interrupt() only */
		/* if the cpu is not in the STOP state. Else, the int could be acknowledged now */
		/* and prevent exiting the STOP state when calling do_specialties() after. */
		/* For performance, we first test PendingInterruptCount, then regs.spcflags */
	        if ( PendingInterruptCount <= 0 )
		{
			while ( ( PendingInterruptCount <= 0 ) && ( PendingInterruptFunction ) && ( ( regs.spcflags & SPCFLAG_STOP ) == 0 ) )
				CALL_VAR(PendingInterruptFunction);		/* call the interrupt handler */
			if ( MFP_UpdateNeeded == true )
				MFP_UpdateIRQ ( 0 );
		}

		if (r->spcflags) {
			do_specialties_interrupt(false);		/* test if there's an mfp/video interrupt and add non pending jitter */
			if (do_specialties (cpu_cycles / CYCLE_UNIT))
				return;
		}
		regs.ipl = regs.ipl_pin;
		if (!currprefs.cpu_compatible || (currprefs.cpu_cycle_exact && currprefs.cpu_model == 68000))
			return;
	}
}

#endif /* CPUEMU_11 */

#ifndef CPUEMU_12

static void m68k_run_1_ce (void)
{
}

#else

/* cycle-exact m68k_run () */

static void m68k_run_1_ce (void)
{
	struct regstruct *r = &regs;

	ipl_fetch ();
	for (;;) {
		uae_u32 opcode = r->ir;

		/*m68k_dumpstate(stderr, NULL);*/
		if (LOG_TRACE_LEVEL(TRACE_CPU_DISASM))
		{
			int FrameCycles, HblCounterVideo, LineCycles;
			Video_GetPosition ( &FrameCycles , &HblCounterVideo , &LineCycles );
			LOG_TRACE_PRINT ( "cpu video_cyc=%6d %3d@%3d : " , FrameCycles, LineCycles, HblCounterVideo );
			m68k_disasm(stderr, m68k_getpc (), NULL, 1);
		}

		/* In case of a Bus Error, we need the PC of the instruction
		 * that caused  the error to build the exception stack frame */
		BusErrorPC = m68k_getpc();

		currcycle = 0;
		(*cpufunctbl[opcode])(opcode);

		/* HACK for Hatari: Adding cycles should of course not be done
		 * here in CE mode (so this should be removed later), but until
		 * we're really there, this helps to get this mode running
		 * at least to a basic extend! */
		M68000_AddCyclesWithPairing(currcycle * 2 / CYCLE_UNIT);
	        if ( PendingInterruptCount <= 0 )
		{
			while ( ( PendingInterruptCount <= 0 ) && ( PendingInterruptFunction ) && ( ( regs.spcflags & SPCFLAG_STOP ) == 0 ) )
				CALL_VAR(PendingInterruptFunction);		/* call the interrupt handler */
			if ( MFP_UpdateNeeded == true )
				MFP_UpdateIRQ ( 0 );
		}

		if (r->spcflags) {
			do_specialties_interrupt(false);		/* test if there's an mfp/video interrupt and add non pending jitter */
			if (do_specialties (0))
				return;
		}
		if (!currprefs.cpu_cycle_exact || currprefs.cpu_model > 68000)
			return;
	}
}
#endif

#ifdef JIT  /* Completely different run_2 replacement */

void do_nothing (void)
{
	/* What did you expect this to do? */
	do_cycles (0);
	/* I bet you didn't expect *that* ;-) */
}

void exec_nostats (void)
{
	struct regstruct *r = &regs;

	for (;;)
	{
		uae_u16 opcode = get_iword (0);

		cpu_cycles = (*cpufunctbl[opcode])(opcode);

		cpu_cycles &= cycles_mask;
		cpu_cycles |= cycles_val;

		do_cycles (cpu_cycles);

		if (end_block (opcode) || r->spcflags || uae_int_requested)
			return; /* We will deal with the spcflags in the caller */
	}
}

static int triggered;

void execute_normal (void)
{
	struct regstruct *r = &regs;
	int blocklen;
	cpu_history pc_hist[MAXRUN];
	int total_cycles;

	if (check_for_cache_miss ())
		return;

	total_cycles = 0;
	blocklen = 0;
	start_pc_p = r->pc_oldp;
	start_pc = r->pc;
	for (;;) {
		/* Take note: This is the do-it-normal loop */
		uae_u16 opcode = get_iword (0);

		special_mem = DISTRUST_CONSISTENT_MEM;
		pc_hist[blocklen].location = (uae_u16*)r->pc_p;

		cpu_cycles = (*cpufunctbl[opcode])(opcode);

		cpu_cycles &= cycles_mask;
		cpu_cycles |= cycles_val;
		do_cycles (cpu_cycles);
		total_cycles += cpu_cycles;
		pc_hist[blocklen].specmem = special_mem;
		blocklen++;
		if (end_block (opcode) || blocklen >= MAXRUN || r->spcflags || uae_int_requested) {
			compile_block (pc_hist, blocklen, total_cycles);
			return; /* We will deal with the spcflags in the caller */
		}
		/* No need to check regs.spcflags, because if they were set,
		we'd have ended up inside that "if" */
	}
}

typedef void compiled_handler (void);

static void m68k_run_jit (void)
{
	for (;;) {

		/*m68k_dumpstate(stderr, NULL);*/
		if (LOG_TRACE_LEVEL(TRACE_CPU_DISASM))
		{
			int FrameCycles, HblCounterVideo, LineCycles;
			Video_GetPosition ( &FrameCycles , &HblCounterVideo , &LineCycles );
			LOG_TRACE_PRINT ( "cpu video_cyc=%6d %3d@%3d : " , FrameCycles, LineCycles, HblCounterVideo );
			m68k_disasm(stderr, m68k_getpc (), NULL, 1);
		}

		((compiled_handler*)(pushall_call_handler))();
		/* Whenever we return from that, we should check spcflags */
		if (uae_int_requested) {
			INTREQ_f (0x8008);
			set_special (SPCFLAG_INT);
		}
		if (regs.spcflags) {
			if (do_specialties (0)) {
				return;
			}
		}
	}
}
#endif /* JIT */

#ifndef CPUEMU_0

static void m68k_run_2 (void)
{
}

#else

#if 0
static void opcodedebug (uae_u32 pc, uae_u16 opcode)
{
	struct mnemolookup *lookup;
	struct instr *dp;
	uae_u32 addr;
	int fault;

	if (cpufunctbl[opcode] == op_illg_1)
		opcode = 0x4AFC;
	dp = table68k + opcode;
	for (lookup = lookuptab;lookup->mnemo != dp->mnemo; lookup++)
		;
	fault = 0;
	TRY(prb) {
		addr = mmu_translate (pc, (regs.mmu_ssw & 4) ? 1 : 0, 0, 0);
	} CATCH (prb) {
		fault = 1;
	} ENDTRY
	if (!fault) {
		write_log ("mmufixup=%d %04x %04x\n", mmufixup[0].reg, regs.wb3_status, regs.mmu_ssw);
		m68k_disasm_2 (stdout, addr, NULL, 1, NULL, NULL, 0);
		write_log ("%s\n", buf);
		m68k_dumpstate (stdout, NULL);
	}
}
#endif

static uaecptr oldpc;

/* Aranym MMU 68040  */
static void m68k_run_mmu040 (void)
{
	uae_u32 opcode = 0;
	uaecptr pc = 0;
	uaecptr fault = 0;
	m68k_exception save_except;

	for (;;) {
	TRY (prb) {
		for (;;) {
			if (LOG_TRACE_LEVEL(TRACE_CPU_DISASM))
			{
				int FrameCycles, HblCounterVideo, LineCycles;
				Video_GetPosition ( &FrameCycles , &HblCounterVideo , &LineCycles );
				LOG_TRACE_PRINT ( "cpu video_cyc=%6d %3d@%3d : " , FrameCycles, LineCycles, HblCounterVideo );
				m68k_disasm(stderr, m68k_getpc (), NULL, 1);
			}

			BusErrorPC = pc = oldpc = regs.fault_pc = m68k_getpc ();
#if 0
			static int done;
			if (pc == 0x16AF94) {
//				write_log ("D0=%d A7=%08x\n", regs.regs[0], regs.regs[15]);
				if (regs.regs[0] == 360) {
					done = 1;
					activate_debugger ();
				}
			}
/*
			if (pc == 0x16B01A) {
				write_log ("-> ERR\n");
			}
			if (pc == 0x16B018) {
				write_log ("->\n");
			}
*/
			if (pc == 0x17967C || pc == 0x13b5e2 - 4) {
				if (done) {
					write_log ("*\n");
					mmu_dump_tables ();
					activate_debugger ();
				}
			}
#endif
			opcode = x_prefetch (0);
			count_instr (opcode);
			do_cycles (cpu_cycles);
			cpu_cycles = (*cpufunctbl[opcode])(opcode);
			cpu_cycles &= cycles_mask;
			cpu_cycles |= cycles_val;


			M68000_AddCycles(cpu_cycles  / CYCLE_UNIT);

			if (regs.spcflags & SPCFLAG_EXTRA_CYCLES) {
				/* Add some extra cycles to simulate a wait state */
				unset_special(SPCFLAG_EXTRA_CYCLES);
				M68000_AddCycles(nWaitStateCycles);
				nWaitStateCycles = 0;
			}

			/* We can have several interrupts at the same time before the next CPU instruction */
			/* We must check for pending interrupt and call do_specialties_interrupt() only */
			/* if the cpu is not in the STOP state. Else, the int could be acknowledged now */
			/* and prevent exiting the STOP state when calling do_specialties() after. */
			/* For performance, we first test PendingInterruptCount, then regs.spcflags */
	        	if ( PendingInterruptCount <= 0 )
			{
				while ( ( PendingInterruptCount <= 0 ) && ( PendingInterruptFunction ) && ( ( regs.spcflags & SPCFLAG_STOP ) == 0 ) )
					CALL_VAR(PendingInterruptFunction);		/* call the interrupt handler */
				if ( MFP_UpdateNeeded == true )
					MFP_UpdateIRQ ( 0 );
			}

			if (regs.spcflags) {
				do_specialties_interrupt(false);		/* test if there's an mfp/video interrupt and add non pending jitter */
				if (do_specialties (cpu_cycles / CYCLE_UNIT))
					return;
			}

			/* Run DSP 56k code if necessary */
			if (bDspEnabled) {
				DSP_Run(cpu_cycles* 2 / CYCLE_UNIT);
			}
		} /* End of for;; */
	} CATCH (prb) {
		save_except = __exvalue;
		if (currprefs.mmu_model == 68060) {
			regs.fault_pc = pc;
			if (mmufixup[1].reg >= 0) {
				m68k_areg (regs, mmufixup[1].reg) = mmufixup[1].value;
				mmufixup[1].reg = -1;
			}
		} else {
			if (regs.wb3_status & 0x80) {
				// movem to memory?
				if ((opcode & 0xff80) == 0x4880) {
					regs.mmu_ssw |= MMU_SSW_CM;
					write_log ("MMU_SSW_CM\n");
				}
			}
		}

		//opcodedebug (regs.fault_pc, opcode);

		if (mmufixup[0].reg >= 0) {
			m68k_areg (regs, mmufixup[0].reg) = mmufixup[0].value;
			mmufixup[0].reg = -1;
		}

		Exception_mmu (save_except, oldpc);
	} ENDTRY
	} /* end for ;; */
}

/* "cycle exact" 68020/030  */
#define MAX68020CYCLES 4
static void m68k_run_2ce (void)
{
	struct regstruct *r = &regs;
	int curr_cycles = 0;

	struct falcon_cycles_t falcon_instr_cycle;

	ipl_fetch ();

	for (;;) {
		/*m68k_dumpstate(stderr, NULL);*/
		if (LOG_TRACE_LEVEL(TRACE_CPU_DISASM))
		{
			int FrameCycles, HblCounterVideo, LineCycles;
			Video_GetPosition ( &FrameCycles , &HblCounterVideo , &LineCycles );
			LOG_TRACE_PRINT ( "cpu video_cyc=%6d %3d@%3d : " , FrameCycles, LineCycles, HblCounterVideo );
			m68k_disasm(stderr, m68k_getpc (), NULL, 1);
		}

		/* clear add_cycles for instructions like movem */
		regs.ce030_instr_addcycles = 0;

		/* Clear M68000 cycle counter */
		if (bDspEnabled)
			Cycles_SetCounter(CYCLES_COUNTER_CPU, 0);	/* to measure the total number of cycles spent in the cpu */

		/* In case of a Bus Error, we need the PC of the instruction
		 * that caused  the error to build the exception stack frame */
		BusErrorPC = m68k_getpc();

		uae_u32 opcode = x_prefetch (0);
		(*cpufunctbl[opcode])(opcode);

		/* Laurent : if 68030 instr cache is on, not frozen and nohitcache miss, cycles are computed with head / tail / and cache_cycles
		 *           else, cycles are equal to non cache cycles.
		 */
		falcon_instr_cycle = regs.ce030_instr_cycles;

		if ((currprefs.cpu_model == 68030) && ((r->cacr & 3) == 1) && (CpuInstruction.iCacheMisses == 0)) { // not frozen and enabled
			if (falcon_instr_cycle.head < CpuInstruction.iSave_instr_tail)
				curr_cycles = (falcon_instr_cycle.cache_cycles - falcon_instr_cycle.head);
			else
				curr_cycles = (falcon_instr_cycle.cache_cycles - CpuInstruction.iSave_instr_tail);

			CpuInstruction.iSave_instr_tail = falcon_instr_cycle.tail;
		}
		else {
			curr_cycles = falcon_instr_cycle.noncache_cycles;
		}

		curr_cycles += regs.ce030_instr_addcycles;

		M68000_AddCycles(curr_cycles);

		if (regs.spcflags & SPCFLAG_EXTRA_CYCLES) {
			/* Add some extra cycles to simulate a wait state */
			unset_special(SPCFLAG_EXTRA_CYCLES);
			M68000_AddCycles(nWaitStateCycles);
			nWaitStateCycles = 0;
		}

		/* We can have several interrupts at the same time before the next CPU instruction */
		/* We must check for pending interrupt and call do_specialties_interrupt() only */
		/* if the cpu is not in the STOP state. Else, the int could be acknowledged now */
		/* and prevent exiting the STOP state when calling do_specialties() after. */
		/* For performance, we first test PendingInterruptCount, then regs.spcflags */
	        if ( PendingInterruptCount <= 0 )
		{
			while ( ( PendingInterruptCount <= 0 ) && ( PendingInterruptFunction ) && ( ( regs.spcflags & SPCFLAG_STOP ) == 0 ) )
				CALL_VAR(PendingInterruptFunction);		/* call the interrupt handler */
			if ( MFP_UpdateNeeded == true )
				MFP_UpdateIRQ ( 0 );
		}

		if (r->spcflags) {
			do_specialties_interrupt(false);		/* test if there's an mfp/video interrupt and add non pending jitter */
			if (do_specialties (0))
				return;
		}

		/* Run DSP 56k code if necessary */
		if (bDspEnabled) {
			DSP_Run(Cycles_GetCounter(CYCLES_COUNTER_CPU) * 2);
		}
	}
}

/* emulate simple prefetch  */
static void m68k_run_2p (void)
{
	uae_u32 prefetch, prefetch_pc;
	struct regstruct *r = &regs;

	prefetch_pc = m68k_getpc ();
	prefetch = get_longi (prefetch_pc);
	for (;;) {
		uae_u32 opcode;
		uae_u32 pc = m68k_getpc ();

		/*m68k_dumpstate(stderr, NULL);*/
		if (LOG_TRACE_LEVEL(TRACE_CPU_DISASM))
		{
			int FrameCycles, HblCounterVideo, LineCycles;
			Video_GetPosition ( &FrameCycles , &HblCounterVideo , &LineCycles );
			LOG_TRACE_PRINT ( "cpu video_cyc=%6d %3d@%3d : " , FrameCycles, LineCycles, HblCounterVideo );
			m68k_disasm(stderr, m68k_getpc (), NULL, 1);
		}

#if DEBUG_CD32CDTVIO
		out_cd32io (m68k_getpc ());
#endif

		do_cycles (cpu_cycles);

		if (pc == prefetch_pc)
			opcode = prefetch >> 16;
		else if (pc == prefetch_pc + 2)
			opcode = prefetch & 0xffff;
		else
			opcode = get_wordi (pc);

		count_instr (opcode);

		/* In case of a Bus Error, we need the PC of the instruction
		 * that caused  the error to build the exception stack frame */
		BusErrorPC = m68k_getpc();

		prefetch_pc = m68k_getpc () + 2;
		prefetch = get_longi (prefetch_pc);
		cpu_cycles = (*cpufunctbl[opcode])(opcode);
		cpu_cycles &= cycles_mask;
		cpu_cycles |= cycles_val;

		M68000_AddCycles(cpu_cycles  / CYCLE_UNIT);

		if (regs.spcflags & SPCFLAG_EXTRA_CYCLES) {
			/* Add some extra cycles to simulate a wait state */
			unset_special(SPCFLAG_EXTRA_CYCLES);
			M68000_AddCycles(nWaitStateCycles);
			nWaitStateCycles = 0;
		}

		/* We can have several interrupts at the same time before the next CPU instruction */
		/* We must check for pending interrupt and call do_specialties_interrupt() only */
		/* if the cpu is not in the STOP state. Else, the int could be acknowledged now */
		/* and prevent exiting the STOP state when calling do_specialties() after. */
		/* For performance, we first test PendingInterruptCount, then regs.spcflags */
	        if ( PendingInterruptCount <= 0 )
		{
			while ( ( PendingInterruptCount <= 0 ) && ( PendingInterruptFunction ) && ( ( regs.spcflags & SPCFLAG_STOP ) == 0 ) )
				CALL_VAR(PendingInterruptFunction);		/* call the interrupt handler */
			if ( MFP_UpdateNeeded == true )
				MFP_UpdateIRQ ( 0 );
		}

		if (r->spcflags) {
			do_specialties_interrupt(false);		/* test if there's an mfp/video interrupt and add non pending jitter */
			if (do_specialties (cpu_cycles / CYCLE_UNIT))
				return;
		}

		/* Run DSP 56k code if necessary */
		if (bDspEnabled) {
			DSP_Run(cpu_cycles*2/ CYCLE_UNIT);
		}
	}
}


//static int used[65536];

/* Same thing, but don't use prefetch to get opcode.  */
static void m68k_run_2 (void)
{
	struct regstruct *r = &regs;

	for (;;) {
		uae_u32 opcode = get_iword (0);
		count_instr (opcode);

		/*m68k_dumpstate(stderr, NULL);*/
		if (LOG_TRACE_LEVEL(TRACE_CPU_DISASM))
		{
			int FrameCycles, HblCounterVideo, LineCycles;
			Video_GetPosition ( &FrameCycles , &HblCounterVideo , &LineCycles );
			LOG_TRACE_PRINT ( "cpu video_cyc=%6d %3d@%3d : " , FrameCycles, LineCycles, HblCounterVideo );
			m68k_disasm(stderr, m68k_getpc (), NULL, 1);
		}

#if 0
		if (!used[opcode]) {
			write_log ("%04X ", opcode);
			used[opcode] = 1;
		}
#endif
		/* In case of a Bus Error, we need the PC of the instruction
		 * that caused  the error to build the exception stack frame */
		BusErrorPC = m68k_getpc();

		do_cycles (cpu_cycles);
		cpu_cycles = (*cpufunctbl[opcode])(opcode);
		cpu_cycles &= cycles_mask;
		cpu_cycles |= cycles_val;

		M68000_AddCycles(cpu_cycles  / CYCLE_UNIT);

		if (regs.spcflags & SPCFLAG_EXTRA_CYCLES) {
			/* Add some extra cycles to simulate a wait state */
			unset_special(SPCFLAG_EXTRA_CYCLES);
			M68000_AddCycles(nWaitStateCycles);
			nWaitStateCycles = 0;
		}

		/* We can have several interrupts at the same time before the next CPU instruction */
		/* We must check for pending interrupt and call do_specialties_interrupt() only */
		/* if the cpu is not in the STOP state. Else, the int could be acknowledged now */
		/* and prevent exiting the STOP state when calling do_specialties() after. */
		/* For performance, we first test PendingInterruptCount, then regs.spcflags */
	        if ( PendingInterruptCount <= 0 )
		{
			while ( ( PendingInterruptCount <= 0 ) && ( PendingInterruptFunction ) && ( ( regs.spcflags & SPCFLAG_STOP ) == 0 ) )
				CALL_VAR(PendingInterruptFunction);		/* call the interrupt handler */
			if ( MFP_UpdateNeeded == true )
				MFP_UpdateIRQ ( 0 );
		}

		if (r->spcflags) {
			do_specialties_interrupt(false);		/* test if there's an mfp/video interrupt and add non pending jitter */
			if (do_specialties (cpu_cycles / CYCLE_UNIT))
				return;
		}

		/* Run DSP 56k code if necessary */
		if (bDspEnabled) {
			DSP_Run(cpu_cycles* 2 / CYCLE_UNIT);
		}
	}
}


/* fake MMU 68k  */
static void m68k_run_mmu (void)
{
	for (;;) {

		/*m68k_dumpstate(stderr, NULL);*/
		if (LOG_TRACE_LEVEL(TRACE_CPU_DISASM))
		{
			int FrameCycles, HblCounterVideo, LineCycles;
			Video_GetPosition ( &FrameCycles , &HblCounterVideo , &LineCycles );
			LOG_TRACE_PRINT ( "cpu video_cyc=%6d %3d@%3d : " , FrameCycles, LineCycles, HblCounterVideo );
			m68k_disasm(stderr, m68k_getpc (), NULL, 1);
		}

		uae_u32 opcode = get_iword (0);
		do_cycles (cpu_cycles);
		mmu_backup_regs = regs;
		cpu_cycles = (*cpufunctbl[opcode])(opcode);
		cpu_cycles &= cycles_mask;
		cpu_cycles |= cycles_val;
		if (mmu_triggered)
			mmu_do_hit ();
		if (regs.spcflags) {
			if (do_specialties (cpu_cycles))
				return;
		}
	}
}

#endif /* CPUEMU_0 */

int in_m68k_go = 0;

static void exception2_handle (uaecptr addr, uaecptr fault)
{
	last_addr_for_exception_3 = addr;
	last_fault_for_exception_3 = fault;
	last_writeaccess_for_exception_3 = 0;
	last_instructionaccess_for_exception_3 = 0;
	Exception (2, m68k_getpc (), true);
}

void m68k_go (int may_quit)
{
	if (in_m68k_go || !may_quit) {
		write_log ("Bug! m68k_go is not reentrant.\n");
		abort ();
	}

	reset_frame_rate_hack ();
	update_68k_cycles ();

	in_m68k_go++;
	for (;;) {
		void (*run_func)(void);

		/* Exit hatari ? */
		if (bQuitProgram == true)
			break;

#ifdef DEBUGGER
		if (debugging)
			debug ();
#endif
		if (regs.panic) {
			regs.panic = 0;
			/* program jumped to non-existing memory and cpu was >= 68020 */
			get_real_address (regs.isp); /* stack in no one's land? -> reboot */
			if (regs.isp & 1)
				regs.panic = 1;
			if (!regs.panic)
				exception2_handle (regs.panic_pc, regs.panic_addr);
			if (regs.panic) {
				/* system is very badly confused */
				write_log ("double bus error or corrupted stack, forcing reboot..\n");
				regs.panic = 0;
				uae_reset (1);
			}
		}

#if 0 /* what was the meaning of this? this breaks trace emulation if debugger is used */
		if (regs.spcflags) {
			uae_u32 of = regs.spcflags;
			regs.spcflags &= ~(SPCFLAG_BRK | SPCFLAG_MODE_CHANGE);
			do_specialties (0);
			regs.spcflags |= of & (SPCFLAG_BRK | SPCFLAG_MODE_CHANGE);
		}
#endif

		set_x_funcs ();
		if (mmu_enabled && !currprefs.cachesize) {
			run_func = m68k_run_mmu;
		} else {
			run_func = currprefs.cpu_cycle_exact && currprefs.cpu_model == 68000 ? m68k_run_1_ce :
				currprefs.cpu_compatible && currprefs.cpu_model == 68000 ? m68k_run_1 :
#ifdef JIT
				currprefs.cpu_model >= 68020 && currprefs.cachesize ? m68k_run_jit :
#endif
				currprefs.cpu_model >= 68030 && currprefs.mmu_model ? m68k_run_mmu040 :
				currprefs.cpu_model >= 68020 && currprefs.cpu_cycle_exact ? m68k_run_2ce :
				currprefs.cpu_compatible ? m68k_run_2p : m68k_run_2;
		}
		run_func ();
	}
	in_m68k_go--;
}

#if 0
static void m68k_verify (uaecptr addr, uaecptr *nextpc)
{
	uae_u32 opcode, val;
	struct instr *dp;

	opcode = get_iword_1 (0);
	last_op_for_exception_3 = opcode;
	m68kpc_offset = 2;

	if (cpufunctbl[opcode] == op_illg_1) {
		opcode = 0x4AFC;
	}
	dp = table68k + opcode;

	if (dp->suse) {
		if (!verify_ea (dp->sreg, dp->smode, dp->size, &val)) {
			Exception (3, 0);
			return;
		}
	}
	if (dp->duse) {
		if (!verify_ea (dp->dreg, dp->dmode, dp->size, &val)) {
			Exception (3, 0);
			return;
		}
	}
}
#endif

static const TCHAR *ccnames[] =
{ "T ","F ","HI","LS","CC","CS","NE","EQ",
"VC","VS","PL","MI","GE","LT","GT","LE" };

static void addmovemreg (TCHAR *out, int *prevreg, int *lastreg, int *first, int reg)
{
	TCHAR *p = out + _tcslen (out);
	if (*prevreg < 0) {
		*prevreg = reg;
		*lastreg = reg;
		return;
	}
	if ((*prevreg) + 1 != reg || (reg & 8) != ((*prevreg & 8))) {
		_stprintf (p, "%s%c%d", (*first) ? "" : "/", (*lastreg) < 8 ? 'D' : 'A', (*lastreg) & 7);
		p = p + _tcslen (p);
		if ((*lastreg) + 2 == reg) {
			_stprintf (p, "/%c%d", (*prevreg) < 8 ? 'D' : 'A', (*prevreg) & 7);
		} else if ((*lastreg) != (*prevreg)) {
			_stprintf (p, "-%c%d", (*prevreg) < 8 ? 'D' : 'A', (*prevreg) & 7);
		}
		*lastreg = reg;
		*first = 0;
	}
	*prevreg = reg;
}

static void movemout (TCHAR *out, uae_u16 mask, int mode)
{
	unsigned int dmask, amask;
	int prevreg = -1, lastreg = -1, first = 1;

	if (mode == Apdi) {
		int i;
		uae_u8 dmask2 = (mask >> 8) & 0xff;
		uae_u8 amask2 = mask & 0xff;
		dmask = 0;
		amask = 0;
		for (i = 0; i < 8; i++) {
			if (dmask2 & (1 << i))
				dmask |= 1 << (7 - i);
			if (amask2 & (1 << i))
				amask |= 1 << (7 - i);
		}
	} else {
		dmask = mask & 0xff;
		amask = (mask >> 8) & 0xff;
	}
	while (dmask) { addmovemreg (out, &prevreg, &lastreg, &first, movem_index1[dmask]); dmask = movem_next[dmask]; }
	while (amask) { addmovemreg (out, &prevreg, &lastreg, &first, movem_index1[amask] + 8); amask = movem_next[amask]; }
	addmovemreg (out, &prevreg, &lastreg, &first, -1);
}

static void disasm_size (TCHAR *instrname, struct instr *dp)
{
#if 0
	int i, size;
	uae_u16 mnemo = dp->mnemo;

	size = dp->size;
	for (i = 0; i < 65536; i++) {
		struct instr *in = &table68k[i];
		if (in->mnemo == mnemo && in != dp) {
			if (size != in->size)
				break;
		}
	}
	if (i == 65536)
		size = -1;
#endif
	switch (dp->size)
	{
	case sz_byte:
		_tcscat (instrname, ".B ");
		break;
	case sz_word:
		_tcscat (instrname, ".W ");
		break;
	case sz_long:
		_tcscat (instrname, ".L ");
		break;
	default:
		_tcscat (instrname, "   ");
		break;
	}
}

static void m68k_disasm_2 (FILE *f, uaecptr addr, uaecptr *nextpc, int cnt, uae_u32 *seaddr, uae_u32 *deaddr, int safemode)
{
	uaecptr newpc = 0;
	m68kpc_offset = addr - m68k_getpc ();

	if (!table68k)
		return;
	while (cnt-- > 0) {
		TCHAR instrname[100], *ccpt;
		int i;
		uae_u32 opcode;
		struct mnemolookup *lookup;
		struct instr *dp;
		int oldpc;

		oldpc = m68kpc_offset;
		opcode = get_iword_1 (m68kpc_offset);
		if (cpufunctbl[opcode] == op_illg_1) {
			opcode = 0x4AFC;
		}
		dp = table68k + opcode;
		for (lookup = lookuptab;lookup->mnemo != dp->mnemo; lookup++)
			;

		fprintf(f, "%08lX ", m68k_getpc () + m68kpc_offset);
		m68kpc_offset += 2;

		if (strcmp(lookup->friendlyname, ""))
			_tcscpy (instrname, lookup->friendlyname);
		else
			_tcscpy (instrname, lookup->name);
		ccpt = _tcsstr (instrname, "cc");
		if (ccpt != 0) {
			_tcsncpy (ccpt, ccnames[dp->cc], 2);
		}
		disasm_size (instrname, dp);

		if (lookup->mnemo == i_MOVEC2 || lookup->mnemo == i_MOVE2C) {
			uae_u16 imm = get_iword_1 (m68kpc_offset);
			uae_u16 creg = imm & 0x0fff;
			uae_u16 r = imm >> 12;
			TCHAR regs[16];
			const TCHAR *cname = "?";
			int i;
			for (i = 0; m2cregs[i].regname; i++) {
				if (m2cregs[i].regno == creg)
					break;
			}
			_stprintf (regs, "%c%d", r >= 8 ? 'A' : 'D', r >= 8 ? r - 8 : r);
			if (m2cregs[i].regname)
				cname = m2cregs[i].regname;
			if (lookup->mnemo == i_MOVE2C) {
				_tcscat (instrname, regs);
				_tcscat (instrname, ",");
				_tcscat (instrname, cname);
			} else {
				_tcscat (instrname, cname);
				_tcscat (instrname, ",");
				_tcscat (instrname, regs);
			}
			m68kpc_offset += 2;
		} else if (lookup->mnemo == i_MVMEL) {
			newpc = m68k_getpc () + m68kpc_offset;
			m68kpc_offset += 2;
			newpc += ShowEA (0, opcode, dp->dreg, dp->dmode, dp->size, instrname, deaddr, safemode);
			_tcscat (instrname, ",");
			movemout (instrname, get_iword_1 (oldpc + 2), dp->dmode);
		} else if (lookup->mnemo == i_MVMLE) {
			m68kpc_offset += 2;
			movemout (instrname, get_iword_1 (oldpc + 2), dp->dmode);
			_tcscat (instrname, ",");
			newpc = m68k_getpc () + m68kpc_offset;
			newpc += ShowEA (0, opcode, dp->dreg, dp->dmode, dp->size, instrname, deaddr, safemode);
		} else {
			if (dp->suse) {
				newpc = m68k_getpc () + m68kpc_offset;
				newpc += ShowEA (0, opcode, dp->sreg, dp->smode, dp->size, instrname, seaddr, safemode);
			}
			if (dp->suse && dp->duse)
				_tcscat (instrname, ",");
			if (dp->duse) {
				newpc = m68k_getpc () + m68kpc_offset;
				newpc += ShowEA (0, opcode, dp->dreg, dp->dmode, dp->size, instrname, deaddr, safemode);
			}
		}

		for (i = 0; i < (m68kpc_offset - oldpc) / 2; i++) {
			fprintf(f, "%04x ", get_iword_1 (oldpc + i * 2));
		}

		while (i++ < 5)
			fprintf(f, "%s", "     ");

		fprintf(f, "%s", instrname);

		if (ccpt != 0) {
			if (deaddr)
				*deaddr = newpc;
			if (cctrue (dp->cc))
				fprintf(f, " == $%08X (T)", newpc);
			else
				fprintf(f, " == $%08X (F)", newpc);
		} else if ((opcode & 0xff00) == 0x6100) { /* BSR */
			if (deaddr)
				*deaddr = newpc;
			fprintf(f, " == $%08X", newpc);
		}
		fprintf(f, "%s", "\n");
	}
	if (nextpc)
		*nextpc = m68k_getpc () + m68kpc_offset;
}

void m68k_disasm_ea (FILE *f, uaecptr addr, uaecptr *nextpc, int cnt, uae_u32 *seaddr, uae_u32 *deaddr)
{
	m68k_disasm_2 (f, addr, nextpc, cnt, seaddr, deaddr, 1);
}
void m68k_disasm (FILE *f, uaecptr addr, uaecptr *nextpc, int cnt)
{
	m68k_disasm_2 (f, addr, nextpc, cnt, NULL, NULL, 0);
}

/*************************************************************
Disasm the m68kcode at the given address into instrname
and instrcode
*************************************************************/
void sm68k_disasm (TCHAR *instrname, TCHAR *instrcode, uaecptr addr, uaecptr *nextpc)
{
	TCHAR *ccpt;
	uae_u32 opcode;
	struct mnemolookup *lookup;
	struct instr *dp;
	int oldpc;

	uaecptr newpc = 0;

	m68kpc_offset = addr - m68k_getpc ();

	oldpc = m68kpc_offset;
	opcode = get_iword_1 (m68kpc_offset);
	if (cpufunctbl[opcode] == op_illg_1) {
		opcode = 0x4AFC;
	}
	dp = table68k + opcode;
	for (lookup = lookuptab;lookup->mnemo != dp->mnemo; lookup++);

	m68kpc_offset += 2;

	_tcscpy (instrname, lookup->name);
	ccpt = _tcsstr (instrname, "cc");
	if (ccpt != 0) {
		_tcsncpy (ccpt, ccnames[dp->cc], 2);
	}
	switch (dp->size){
	case sz_byte: _tcscat (instrname, ".B "); break;
	case sz_word: _tcscat (instrname, ".W "); break;
	case sz_long: _tcscat (instrname, ".L "); break;
	default: _tcscat (instrname, "   "); break;
	}

	if (dp->suse) {
		newpc = m68k_getpc () + m68kpc_offset;
		newpc += ShowEA (0, opcode, dp->sreg, dp->smode, dp->size, instrname, NULL, 0);
	}
	if (dp->suse && dp->duse)
		_tcscat (instrname, ",");
	if (dp->duse) {
		newpc = m68k_getpc () + m68kpc_offset;
		newpc += ShowEA (0, opcode, dp->dreg, dp->dmode, dp->size, instrname, NULL, 0);
	}

	if (instrcode)
	{
		int i;
		for (i = 0; i < (m68kpc_offset - oldpc) / 2; i++)
		{
			_stprintf (instrcode, "%04x ", get_iword_1 (oldpc + i * 2));
			instrcode += _tcslen (instrcode);
		}
	}

	if (nextpc)
		*nextpc = m68k_getpc () + m68kpc_offset;
}

struct cpum2c m2cregs[] = {
	{ 0, "SFC" },
	{ 1, "DFC" },
	{ 2, "CACR" },
	{ 3, "TC" },
	{ 4, "ITT0" },
	{ 5, "ITT1" },
	{ 6, "DTT0" },
	{ 7, "DTT1" },
	{ 8, "BUSC" },
	{ 0x800, "USP" },
	{ 0x801, "VBR" },
	{ 0x802, "CAAR" },
	{ 0x803, "MSP" },
	{ 0x804, "ISP" },
	{ 0x805, "MMUS" },
	{ 0x806, "URP" },
	{ 0x807, "SRP" },
	{ 0x808, "PCR" },
	{ -1, NULL }
};

void val_move2c2 (int regno, uae_u32 val)
{
	switch (regno) {
	case 0: regs.sfc = val; break;
	case 1: regs.dfc = val; break;
	case 2: regs.cacr = val; break;
	case 3: regs.tcr = val; break;
	case 4: regs.itt0 = val; break;
	case 5: regs.itt1 = val; break;
	case 6: regs.dtt0 = val; break;
	case 7: regs.dtt1 = val; break;
	case 8: regs.buscr = val; break;
	case 0x800: regs.usp = val; break;
	case 0x801: regs.vbr = val; break;
	case 0x802: regs.caar = val; break;
	case 0x803: regs.msp = val; break;
	case 0x804: regs.isp = val; break;
	case 0x805: regs.mmusr = val; break;
	case 0x806: regs.urp = val; break;
	case 0x807: regs.srp = val; break;
	case 0x808: regs.pcr = val; break;
	}
}

uae_u32 val_move2c (int regno)
{
	switch (regno) {
	case 0: return regs.sfc;
	case 1: return regs.dfc;
	case 2: return regs.cacr;
	case 3: return regs.tcr;
	case 4: return regs.itt0;
	case 5: return regs.itt1;
	case 6: return regs.dtt0;
	case 7: return regs.dtt1;
	case 8: return regs.buscr;
	case 0x800: return regs.usp;
	case 0x801: return regs.vbr;
	case 0x802: return regs.caar;
	case 0x803: return regs.msp;
	case 0x804: return regs.isp;
	case 0x805: return regs.mmusr;
	case 0x806: return regs.urp;
	case 0x807: return regs.srp;
	case 0x808: return regs.pcr;
	default: return 0;
	}
}

void m68k_dumpstate (FILE *f, uaecptr *nextpc)
{
	int i, j;

	for (i = 0; i < 8; i++){
		f_out (f, "  D%d %08X ", i, m68k_dreg (regs, i));
		if ((i & 3) == 3) f_out (f, "\n");
	}
	for (i = 0; i < 8; i++){
		f_out (f, "  A%d %08X ", i, m68k_areg (regs, i));
		if ((i & 3) == 3) f_out (f, "\n");
	}
	if (regs.s == 0)
		regs.usp = m68k_areg (regs, 7);
	if (regs.s && regs.m)
		regs.msp = m68k_areg (regs, 7);
	if (regs.s && regs.m == 0)
		regs.isp = m68k_areg (regs, 7);
	j = 2;
	f_out (f, "USP  %08X ISP  %08X ", regs.usp, regs.isp);
	for (i = 0; m2cregs[i].regno>= 0; i++) {
		if (!movec_illg (m2cregs[i].regno)) {
			if (!_tcscmp (m2cregs[i].regname, "USP") || !_tcscmp (m2cregs[i].regname, "ISP"))
				continue;
			if (j > 0 && (j % 4) == 0)
				f_out (f, "\n");
			f_out (f, "%-4s %08X ", m2cregs[i].regname, val_move2c (m2cregs[i].regno));
			j++;
		}
	}
	if (j > 0)
		f_out (f, "\n");
	f_out (f, "T=%d%d S=%d M=%d X=%d N=%d Z=%d V=%d C=%d IMASK=%d STP=%d\n",
		regs.t1, regs.t0, regs.s, regs.m,
		GET_XFLG (), GET_NFLG (), GET_ZFLG (),
		GET_VFLG (), GET_CFLG (),
		regs.intmask, regs.stopped);
#ifdef FPUEMU
	if (currprefs.fpu_model) {
		uae_u32 fpsr;
		for (i = 0; i < 8; i++){
			f_out (f, "FP%d: %g ", i, regs.fp[i]);
			if ((i & 3) == 3)
				f_out (f, "\n");
		}
		fpsr = get_fpsr ();
		f_out (f, "N=%d Z=%d I=%d NAN=%d\n",
			(fpsr & 0x8000000) != 0,
			(fpsr & 0x4000000) != 0,
			(fpsr & 0x2000000) != 0,
			(fpsr & 0x1000000) != 0);
	}
#endif
	if (currprefs.cpu_compatible && currprefs.cpu_model == 68000) {
		struct instr *dp;
		struct mnemolookup *lookup1, *lookup2;
		dp = table68k + regs.irc;
		for (lookup1 = lookuptab; lookup1->mnemo != dp->mnemo; lookup1++);
		dp = table68k + regs.ir;
		for (lookup2 = lookuptab; lookup2->mnemo != dp->mnemo; lookup2++);
		f_out (f, "Prefetch %04x (%s) %04x (%s)\n", regs.irc, lookup1->name, regs.ir, lookup2->name);
	}

	m68k_disasm (f, m68k_getpc (), nextpc, 1);
	if (nextpc)
		f_out (f, "Next PC: %08x\n", *nextpc);
}

#ifdef SAVESTATE

/* CPU save/restore code */

#define CPUTYPE_EC 1
#define CPUMODE_HALT 1



uae_u8 *restore_cpu (uae_u8 *src)
{
	int i, flags, model;
	uae_u32 l;

	changed_prefs.cpu_model = model = restore_u32 ();
	flags = restore_u32 ();
	changed_prefs.address_space_24 = 0;
	if (flags & CPUTYPE_EC)
		changed_prefs.address_space_24 = 1;
	if (model > 68020)
		changed_prefs.cpu_compatible = 0;
	currprefs.address_space_24 = changed_prefs.address_space_24;
	currprefs.cpu_compatible = changed_prefs.cpu_compatible;
	currprefs.cpu_cycle_exact = changed_prefs.cpu_cycle_exact;
	currprefs.blitter_cycle_exact = changed_prefs.blitter_cycle_exact;
	currprefs.cpu_frequency = changed_prefs.cpu_frequency = 0;
	currprefs.cpu_clock_multiplier = changed_prefs.cpu_clock_multiplier = 0;
	for (i = 0; i < 15; i++)
		regs.regs[i] = restore_u32 ();
	regs.pc = restore_u32 ();
	regs.irc = restore_u16 ();
	regs.ir = restore_u16 ();
	regs.usp = restore_u32 ();
	regs.isp = restore_u32 ();
	regs.sr = restore_u16 ();
	l = restore_u32 ();
	if (l & CPUMODE_HALT) {
		regs.stopped = 1;
	} else {
		regs.stopped = 0;
	}
	if (model >= 68010) {
		regs.dfc = restore_u32 ();
		regs.sfc = restore_u32 ();
		regs.vbr = restore_u32 ();
	}
	if (model >= 68020) {
		regs.caar = restore_u32 ();
		regs.cacr = restore_u32 ();
		regs.msp = restore_u32 ();
		/* A500 speed in 68020 mode isn't too logical.. */
		if (changed_prefs.m68k_speed == 0 && !(currprefs.cpu_cycle_exact))
			currprefs.m68k_speed = changed_prefs.m68k_speed = -1;
	}
	if (model >= 68030) {
		crp_030 = restore_u64 ();
		srp_030 = restore_u64 ();
		tt0_030 =restore_u32 ();
		tt1_030 = restore_u32 ();
		tc_030 = restore_u32 ();
		mmusr_030 = restore_u16 ();
	}
	if (model >= 68040) {
		regs.itt0 = restore_u32 ();
		regs.itt1 = restore_u32 ();
		regs.dtt0 = restore_u32 ();
		regs.dtt1 = restore_u32 ();
		regs.tcr = restore_u32 ();
		regs.urp = restore_u32 ();
		regs.srp = restore_u32 ();
	}
	if (model >= 68060) {
		regs.buscr = restore_u32 ();
		regs.pcr = restore_u32 ();
	}
	if (flags & 0x80000000) {
		int khz = restore_u32 ();
		restore_u32 ();
		if (khz > 0 && khz < 800000)
			currprefs.m68k_speed = changed_prefs.m68k_speed = 0;
	}
	write_log ("CPU: %d%s%03d, PC=%08X\n",
		model / 1000, flags & 1 ? "EC" : "", model % 1000, regs.pc);

	return src;
}

void restore_cpu_finish (void)
{
	init_m68k ();
	m68k_setpc (regs.pc);
	set_cpu_caches ();
	doint ();
	if (regs.stopped)
		set_special (SPCFLAG_STOP);
	//activate_debugger ();
}

uae_u8 *restore_cpu_extra (uae_u8 *src)
{
	restore_u32 ();
	uae_u32 flags = restore_u32 ();


	currprefs.cpu_cycle_exact = changed_prefs.cpu_cycle_exact = (flags & 1) ? true : false;
	currprefs.blitter_cycle_exact = changed_prefs.blitter_cycle_exact = currprefs.cpu_cycle_exact;
	currprefs.cpu_compatible = changed_prefs.cpu_compatible = (flags & 2) ? true : false;
	currprefs.cpu_frequency = changed_prefs.cpu_frequency = restore_u32 ();
	currprefs.cpu_clock_multiplier = changed_prefs.cpu_clock_multiplier = restore_u32 ();
	currprefs.cachesize = changed_prefs.cachesize = (flags & 8) ? 8192 : 0;

	currprefs.m68k_speed = changed_prefs.m68k_speed = 0;
	if (flags & 4)
		currprefs.m68k_speed = changed_prefs.m68k_speed = -1;

	currprefs.cpu060_revision = changed_prefs.cpu060_revision = restore_u8 ();
	currprefs.fpu_revision = changed_prefs.fpu_revision = restore_u8 ();

	return src;
}

uae_u8 *save_cpu_extra (int *len, uae_u8 *dstptr)
{
	uae_u8 *dstbak, *dst;
	uae_u32 flags;

	if (dstptr)
		dstbak = dst = dstptr;
	else
		dstbak = dst = xmalloc (uae_u8, 1000);
	save_u32 (0); // version
	flags = 0;
	flags |= currprefs.cpu_cycle_exact ? 1 : 0;
	flags |= currprefs.cpu_compatible ? 2 : 0;
	flags |= currprefs.m68k_speed < 0 ? 4 : 0;
	flags |= currprefs.cachesize > 0 ? 8 : 0;
	save_u32 (flags);
	save_u32 (currprefs.cpu_frequency);
	save_u32 (currprefs.cpu_clock_multiplier);
	save_u8 (currprefs.cpu060_revision);
	save_u8 (currprefs.fpu_revision);
	*len = dst - dstbak;
	return dstbak;
}

uae_u8 *save_cpu (int *len, uae_u8 *dstptr)
{
	uae_u8 *dstbak, *dst;
	int model, i, khz;

	if (dstptr)
		dstbak = dst = dstptr;
	else
		dstbak = dst = xmalloc (uae_u8, 1000);
	model = currprefs.cpu_model;
	save_u32 (model);					/* MODEL */
	save_u32 (0x80000000 | (currprefs.address_space_24 ? 1 : 0)); /* FLAGS */
	for (i = 0;i < 15; i++)
		save_u32 (regs.regs[i]);		/* D0-D7 A0-A6 */
	save_u32 (m68k_getpc ());			/* PC */
	save_u16 (regs.irc);				/* prefetch */
	save_u16 (regs.ir);					/* instruction prefetch */
	MakeSR ();
	save_u32 (!regs.s ? regs.regs[15] : regs.usp);	/* USP */
	save_u32 (regs.s ? regs.regs[15] : regs.isp);	/* ISP */
	save_u16 (regs.sr);								/* SR/CCR */
	save_u32 (regs.stopped ? CPUMODE_HALT : 0);		/* flags */
	if (model >= 68010) {
		save_u32 (regs.dfc);			/* DFC */
		save_u32 (regs.sfc);			/* SFC */
		save_u32 (regs.vbr);			/* VBR */
	}
	if (model >= 68020) {
		save_u32 (regs.caar);			/* CAAR */
		save_u32 (regs.cacr);			/* CACR */
		save_u32 (regs.msp);			/* MSP */
	}
	if (model >= 68030) {
		save_u64 (crp_030);				/* CRP */
		save_u64 (srp_030);				/* SRP */
		save_u32 (tt0_030);				/* TT0/AC0 */
		save_u32 (tt1_030);				/* TT1/AC1 */
		save_u32 (tc_030);				/* TCR */
		save_u16 (mmusr_030);			/* MMUSR/ACUSR */
	}
	if (model >= 68040) {
		save_u32 (regs.itt0);			/* ITT0 */
		save_u32 (regs.itt1);			/* ITT1 */
		save_u32 (regs.dtt0);			/* DTT0 */
		save_u32 (regs.dtt1);			/* DTT1 */
		save_u32 (regs.tcr);			/* TCR */
		save_u32 (regs.urp);			/* URP */
		save_u32 (regs.srp);			/* SRP */
	}
	if (model >= 68060) {
		save_u32 (regs.buscr);			/* BUSCR */
		save_u32 (regs.pcr);			/* PCR */
	}
	khz = -1;
	if (currprefs.m68k_speed == 0) {
		khz = currprefs.ntscmode ? 715909 : 709379;
		if (currprefs.cpu_model >= 68020)
			khz *= 2;
	}
	save_u32 (khz); // clock rate in KHz: -1 = fastest possible
	save_u32 (0); // spare
	*len = dst - dstbak;
	return dstbak;
}

uae_u8 *save_mmu (int *len, uae_u8 *dstptr)
{
	uae_u8 *dstbak, *dst;
	int model;

	model = currprefs.mmu_model;
	if (model != 68040 && model != 68060)
		return NULL;
	if (dstptr)
		dstbak = dst = dstptr;
	else
		dstbak = dst = xmalloc (uae_u8, 1000);
	save_u32 (model);	/* MODEL */
	save_u32 (0);	/* FLAGS */
	*len = dst - dstbak;
	return dstbak;
}

uae_u8 *restore_mmu (uae_u8 *src)
{
	int flags, model;

	changed_prefs.mmu_model = model = restore_u32 ();
	flags = restore_u32 ();
	write_log ("MMU: %d\n", model);
	return src;
}

#endif /* SAVESTATE */

static void exception3f (uae_u32 opcode, uaecptr addr, uaecptr fault, int writeaccess, int instructionaccess)
{
	if (currprefs.cpu_model >= 68040)
		addr &= ~1;
	last_addr_for_exception_3 = addr;
	last_fault_for_exception_3 = fault;
	last_op_for_exception_3 = opcode;
	last_writeaccess_for_exception_3 = writeaccess;
	last_instructionaccess_for_exception_3 = instructionaccess;
	Exception (3, fault, true);
}

void exception3 (uae_u32 opcode, uaecptr addr, uaecptr fault)
{
	exception3f (opcode, addr, fault, 0, 0);
}

void exception3i (uae_u32 opcode, uaecptr addr, uaecptr fault)
{
	exception3f (opcode, addr, fault, 0, 1);
}

void exception2 (uaecptr addr, uaecptr fault)
{
	write_log ("delayed exception2!\n");
	regs.panic_pc = m68k_getpc ();
	regs.panic_addr = addr;
	regs.panic = 2;
	set_special (SPCFLAG_BRK);
	m68k_setpc (0xf80000);
#ifdef JIT
	set_special (SPCFLAG_END_COMPILE);
#endif
	fill_prefetch_slow ();
}

void cpureset (void)
{
	uaecptr pc;
	uaecptr ksboot = 0xf80002 - 2; /* -2 = RESET hasn't increased PC yet */
	uae_u16 ins;

	if (currprefs.cpu_compatible || currprefs.cpu_cycle_exact) {
//		customreset (0);
		customreset ();
		return;
	}
	pc = m68k_getpc ();
	if (pc >= currprefs.chipmem_size) {
		addrbank *b = &get_mem_bank (pc);
		if (b->check (pc, 2 + 2)) {
			/* We have memory, hope for the best.. */
//			customreset (0);
			customreset ();
			return;
		}
		write_log ("M68K RESET PC=%x, rebooting..\n", pc);
//		customreset (0);
		customreset ();
		m68k_setpc (ksboot);
		return;
	}
	/* panic, RAM is going to disappear under PC */
	ins = get_word (pc + 2);
	if ((ins & ~7) == 0x4ed0) {
		int reg = ins & 7;
		uae_u32 addr = m68k_areg (regs, reg);
		write_log ("reset/jmp (ax) combination emulated -> %x\n", addr);
//		customreset (0);
		customreset ();
		if (addr < 0x80000)
			addr += 0xf80000;
		m68k_setpc (addr - 2);
		return;
	}
	write_log ("M68K RESET PC=%x, rebooting..\n", pc);
//	customreset (0);
	customreset ();
	m68k_setpc (ksboot);
}


void m68k_setstopped (void)
{
	regs.stopped = 1;
	/* A traced STOP instruction drops through immediately without
	actually stopping.  */
	if ((regs.spcflags & SPCFLAG_DOTRACE) == 0)
		set_special (SPCFLAG_STOP);
	else
		m68k_resumestopped ();
}

void m68k_resumestopped (void)
{
	if (!regs.stopped)
		return;
	regs.stopped = 0;
	if (currprefs.cpu_cycle_exact) {
		if (currprefs.cpu_model == 68000)
			do_cycles_ce000 (6);
	}
	fill_prefetch_slow ();
	unset_special (SPCFLAG_STOP);
}

/*
* Compute exact number of CPU cycles taken
* by DIVU and DIVS on a 68000 processor.
*
* Copyright (c) 2005 by Jorge Cwik, pasti@fxatari.com
*
* This is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This software is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this software; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*
*/


/*

The routines below take dividend and divisor as parameters.
They return 0 if division by zero, or exact number of cycles otherwise.

The number of cycles returned assumes a register operand.
Effective address time must be added if memory operand.

For 68000 only (not 68010, 68012, 68020, etc).
Probably valid for 68008 after adding the extra prefetch cycle.


Best and worst cases for register operand:
(Note the difference with the documented range.)


DIVU:

Overflow (always): 10 cycles.
Worst case: 136 cycles.
Best case: 76 cycles.


DIVS:

Absolute overflow: 16-18 cycles.
Signed overflow is not detected prematurely.

Worst case: 156 cycles.
Best case without signed overflow: 122 cycles.
Best case with signed overflow: 120 cycles


*/

int getDivu68kCycles (uae_u32 dividend, uae_u16 divisor)
{
	int mcycles;
	uae_u32 hdivisor;
	int i;

	if (divisor == 0)
		return 0;

	// Overflow
	if ((dividend >> 16) >= divisor)
		return (mcycles = 5) * 2;

	mcycles = 38;
	hdivisor = divisor << 16;

	for (i = 0; i < 15; i++) {
		uae_u32 temp;
		temp = dividend;

		dividend <<= 1;

		// If carry from shift
		if ((uae_s32)temp < 0)
			dividend -= hdivisor;
		else {
			mcycles += 2;
			if (dividend >= hdivisor) {
				dividend -= hdivisor;
				mcycles--;
			}
		}
	}
	return mcycles * 2;
}

int getDivs68kCycles (uae_s32 dividend, uae_s16 divisor)
{
	int mcycles;
	uae_u32 aquot;
	int i;

	if (divisor == 0)
		return 0;

	mcycles = 6;

	if (dividend < 0)
		mcycles++;

	// Check for absolute overflow
	if (((uae_u32)abs (dividend) >> 16) >= (uae_u16)abs (divisor))
		return (mcycles + 2) * 2;

	// Absolute quotient
	aquot = (uae_u32) abs (dividend) / (uae_u16)abs (divisor);

	mcycles += 55;

	if (divisor >= 0) {
		if (dividend >= 0)
			mcycles--;
		else
			mcycles++;
	}

	// Count 15 msbits in absolute of quotient

	for (i = 0; i < 15; i++) {
		if ((uae_s16)aquot >= 0)
			mcycles++;
		aquot <<= 1;
	}

	return mcycles * 2;
}

STATIC_INLINE void fill_cache040 (uae_u32 addr)
{
	int index, i, lws;
	uae_u32 tag;
	uae_u32 data;
	struct cache040 *c;
	static int linecnt;

	addr &= ~15;
	index = (addr >> 4) & (CACHESETS040 - 1);
	tag = regs.s | (addr & ~((CACHESETS040 << 4) - 1));
	lws = (addr >> 2) & 3;
	c = &caches040[index];
	for (i = 0; i < CACHELINES040; i++) {
		if (c->valid[i] && c->tag[i] == tag) {
			// cache hit
			regs.prefetch020addr[0] = addr;
			regs.prefetch020data[0] = c->data[i][lws];
			return;
		}
	}
	// cache miss
	data = mem_access_delay_longi_read_ce020 (addr);
	int line = linecnt;
	for (i = 0; i < CACHELINES040; i++) {
		int line = (linecnt + i) & (CACHELINES040 - 1);
		if (c->tag[i] != tag || c->valid[i] == false) {
			c->tag[i] = tag;
			c->valid[i] = true;
			c->data[i][0] = data;
		}
	}
	regs.prefetch020addr[0] = addr;
	regs.prefetch020data[0] = data;
}

// this one is really simple and easy
STATIC_INLINE void fill_icache020 (uae_u32 addr, int idx)
{
	int index;
	uae_u32 tag;
	uae_u32 data;
	struct cache020 *c;

	addr &= ~3;
	index = (addr >> 2) & (CACHELINES020 - 1);
	tag = regs.s | (addr & ~((CACHELINES020 << 2) - 1));
	c = &caches020[index];
	if (c->valid && c->tag == tag) {
		// cache hit
		regs.prefetch020addr[idx] = addr;
		regs.prefetch020data[idx] = c->data;
		return;
	}
	// cache miss
	CpuInstruction.iCacheMisses++;
	data = mem_access_delay_longi_read_ce020 (addr);
	if (!(regs.cacr & 2)) {
		c->tag = tag;
		c->valid = !!(regs.cacr & 1);
		c->data = data;
	}
	regs.prefetch020addr[idx] = addr;
	regs.prefetch020data[idx] = data;
}

uae_u32 get_word_ce020_prefetch (int o)
{
	int i;
	uae_u32 pc = m68k_getpc () + o;

	CpuInstruction.iCacheMisses = 0;
	for (;;) {
		for (i = 0; i < 2; i++) {
			if (pc == regs.prefetch020addr[0]) {
				uae_u32 v = regs.prefetch020data[0] >> 16;
				fill_icache020 (regs.prefetch020addr[0] + 4, 1);
				return v;
			}
			if (pc == regs.prefetch020addr[0] + 2) {
				uae_u32 v = regs.prefetch020data[0] & 0xffff;
				if (regs.prefetch020addr[1] == regs.prefetch020addr[0] + 4) {
					regs.prefetch020addr[0] = regs.prefetch020addr[1];
					regs.prefetch020data[0] = regs.prefetch020data[1];
					fill_icache020 (regs.prefetch020addr[0] + 4, 1);
				} else {
					fill_icache020 (pc + 4, 0);
					fill_icache020 (regs.prefetch020addr[0] + 4, 1);
				}
				return v;
			}
			regs.prefetch020addr[0] = regs.prefetch020addr[1];
			regs.prefetch020data[0] = regs.prefetch020data[1];
		}
		fill_icache020 (pc + 0, 0);
		fill_icache020 (pc + 4, 1);
	}
}

// 68030 caches aren't so simple as 68020 cache..
STATIC_INLINE struct cache030 *getcache030 (struct cache030 *cp, uaecptr addr, uae_u32 *tagp, int *lwsp)
{
	int index, lws;
	uae_u32 tag;
	struct cache030 *c;

	addr &= ~3;
	index = (addr >> 4) & (CACHELINES030 - 1);
	tag = regs.s | (addr & ~((CACHELINES030 << 4) - 1));
	lws = (addr >> 2) & 3;
	c = &cp[index];
	*tagp = tag;
	*lwsp = lws;
	return c;
}

STATIC_INLINE void update_cache030 (struct cache030 *c, uae_u32 val, uae_u32 tag, int lws)
{
	if (c->tag != tag)
		c->valid[0] = c->valid[1] = c->valid[2] = c->valid[3] = false;
	c->tag = tag;
	c->valid[lws] = true;
	c->data[lws] = val;
}

STATIC_INLINE void fill_icache030 (uae_u32 addr, int idx)
{
	int lws;
	uae_u32 tag;
	uae_u32 data;
	struct cache030 *c;

	addr &= ~3;
	c = getcache030 (icaches030, addr, &tag, &lws);
	if (c->valid[lws] && c->tag == tag) {
		// cache hit
		regs.prefetch020addr[idx] = addr;
		regs.prefetch020data[idx] = c->data[lws];
		return;
	}
	// cache miss
	CpuInstruction.iCacheMisses++;
	data = mem_access_delay_longi_read_ce020 (addr);
	if ((regs.cacr & 3) == 1) { // not frozen and enabled
		update_cache030 (c, data, tag, lws);
#if 0
		if ((regs.cacr & 0x11) == 0x11 && lws == 0 && !c->valid[0] && !c->valid[1] && !c->valid[2] && !c->valid[3] && ce_banktype[addr >> 16] == CE_MEMBANK_FAST) {
			// do burst fetch if cache enabled, not frozen, all slots invalid, no chip ram
			c->data[1] = mem_access_delay_long_read_ce020 (addr + 4);
			c->data[2] = mem_access_delay_long_read_ce020 (addr + 8);
			c->data[3] = mem_access_delay_long_read_ce020 (addr + 12);
			c->valid[1] = c->valid[2] = c->valid[3] = true;
		}
#endif
	}
	regs.prefetch020addr[idx] = addr;
	regs.prefetch020data[idx] = data;
}

STATIC_INLINE bool cancache030 (uaecptr addr)
{
	return ce_cachable[addr >> 16] != 0;
}

// and finally the worst part, 68030 data cache..
void write_dcache030 (uaecptr addr, uae_u32 val, int size)
{
	struct cache030 *c1, *c2;
	int lws1, lws2;
	uae_u32 tag1, tag2;
	int aligned = addr & 3;

	if (!(regs.cacr & 0x100) || currprefs.cpu_model == 68040) // data cache disabled? 68040 shares this too.
		return;
	if (!cancache030 (addr))
		return;

	c1 = getcache030 (dcaches030, addr, &tag1, &lws1);
	if (!(regs.cacr & 0x2000)) { // write allocate
		if (c1->tag != tag1 || c1->valid[lws1] == false)
			return;
	}

#if 0
	uaecptr a = 0x1db0c;
	if (addr - (1 << size) + 1 <= a && addr + (1 << size) >= a) {
		write_log ("%08x %d %d %08x %08x %d\n", addr, aligned, size, val, tag1, lws1);
		if (aligned == 2)
			write_log ("*\n");
	}
#endif

	// easy one
	if (size == 2 && aligned == 0) {
		update_cache030 (c1, val, tag1, lws1);
#if 0
		if ((regs.cacr & 0x1100) == 0x1100 && lws1 == 0 && !c1->valid[0] && !c1->valid[1] && !c1->valid[2] && !c1->valid[3] && ce_banktype[addr >> 16] == CE_MEMBANK_FAST) {
			// do burst fetch if cache enabled, not frozen, all slots invalid, no chip ram
			c1->data[1] = mem_access_delay_long_read_ce020 (addr + 4);
			c1->data[2] = mem_access_delay_long_read_ce020 (addr + 8);
			c1->data[3] = mem_access_delay_long_read_ce020 (addr + 12);
			c1->valid[1] = c1->valid[2] = c1->valid[3] = true;
		}
#endif
		return;
	}
	// argh!! merge partial write
	c2 = getcache030 (dcaches030, addr + 4, &tag2, &lws2);
	if (size == 2) {
		if (c1->valid[lws1] && c1->tag == tag1) {
			c1->data[lws1] &= ~(0xffffffff >> (aligned * 8));
			c1->data[lws1] |= val >> (aligned * 8);
		}
		if (c2->valid[lws2] && c2->tag == tag2) {
			c2->data[lws2] &= 0xffffffff >> ((4 - aligned) * 8);
			c2->data[lws2] |= val << ((4 - aligned) * 8);
		}
	} else if (size == 1) {
		val <<= 16;
		if (c1->valid[lws1] && c1->tag == tag1) {
			c1->data[lws1] &= ~(0xffff0000 >> (aligned * 8));
			c1->data[lws1] |= val >> (aligned * 8);
		}
		if (c2->valid[lws2] && c2->tag == tag2 && aligned == 3) {
			c2->data[lws2] &= 0x00ffffff;
			c2->data[lws2] |= val << 8;
		}
	} else if (size == 0) {
		val <<= 24;
		if (c1->valid[lws1] && c1->tag == tag1) {
			c1->data[lws1] &= ~(0xff000000 >> (aligned * 8));
			c1->data[lws1] |= val >> (aligned * 8);
		}
	}
}

uae_u32 read_dcache030 (uaecptr addr, int size)
{
	struct cache030 *c1, *c2;
	int lws1, lws2;
	uae_u32 tag1, tag2;
	int aligned = addr & 3;
	int len = (1 << size) * 8;
	uae_u32 v1, v2;

	if (!(regs.cacr & 0x100) || currprefs.cpu_model == 68040 || !cancache030 (addr)) { // data cache disabled? shared with 68040 "ce"
		if (size == 2)
			return mem_access_delay_long_read_ce020 (addr);
		else if (size == 1)
			return mem_access_delay_word_read_ce020 (addr);
		else
			return mem_access_delay_byte_read_ce020 (addr);
	}

	c1 = getcache030 (dcaches030, addr, &tag1, &lws1);
	addr &= ~3;
	if (!c1->valid[lws1] || c1->tag != tag1) {
		v1 = mem_access_delay_long_read_ce020 (addr);
		update_cache030 (c1, v1, tag1, lws1);
	} else {
		v1 = c1->data[lws1];
		if (get_long (addr) != v1) {
			write_log ("data cache mismatch %d %d %08x %08x != %08x %08x %d PC=%08x\n",
				size, aligned, addr, STMemory_ReadLong(addr), v1, tag1, lws1, M68K_GETPC);
			v1 = get_long (addr);
		}
	}
	// only one long fetch needed?
	if (size == 0) {
		v1 >>= (3 - aligned) * 8;
		return v1;
	} else if (size == 1 && aligned <= 2) {
		v1 >>= (2 - aligned) * 8;
		return v1;
	} else if (size == 2 && aligned == 0) {
		return v1;
	}
	// need two longs
	addr += 4;
	c2 = getcache030 (dcaches030, addr, &tag2, &lws2);
	if (!c2->valid[lws2] || c2->tag != tag2) {
		v2 = mem_access_delay_long_read_ce020 (addr);
		update_cache030 (c2, v2, tag2, lws2);
	} else {
		v2 = c2->data[lws2];
		if (get_long (addr) != v2) {
			write_log ("data cache mismatch %d %d %08x %08x != %08x %08x %d PC=%08x\n",
				size, aligned, addr, STMemory_ReadLong(addr), v2, tag2, lws2, M68K_GETPC);
			v2 = get_long (addr);
		}
	}
	if (size == 1 && aligned == 3)
		return (v1 << 8) | (v2 >> 24);
	else if (size == 2 && aligned == 1)
		return (v1 << 8) | (v2 >> 24);
	else if (size == 2 && aligned == 2)
		return (v1 << 16) | (v2 >> 16);
	else if (size == 2 && aligned == 3)
		return (v1 << 24) | (v2 >> 8);

	write_log ("dcache030 weirdness!?\n");
	return 0;
}

uae_u32 get_word_ce030_prefetch (int o)
{
	int i;
	uae_u32 pc = m68k_getpc () + o;

	CpuInstruction.iCacheMisses = 0;
	for (;;) {
		for (i = 0; i < 2; i++) {
			if (pc == regs.prefetch020addr[0]) {
				uae_u32 v = regs.prefetch020data[0] >> 16;
				fill_icache030 (regs.prefetch020addr[0] + 4, 1);
				return v;
			}
			if (pc == regs.prefetch020addr[0] + 2) {
				uae_u32 v = regs.prefetch020data[0] & 0xffff;
				if (regs.prefetch020addr[1] == regs.prefetch020addr[0] + 4) {
					regs.prefetch020addr[0] = regs.prefetch020addr[1];
					regs.prefetch020data[0] = regs.prefetch020data[1];
					fill_icache030 (regs.prefetch020addr[0] + 4, 1);
				} else {
					fill_icache030 (pc + 4, 0);
					fill_icache030 (regs.prefetch020addr[0] + 4, 1);
				}
				return v;
			}
			regs.prefetch020addr[0] = regs.prefetch020addr[1];
			regs.prefetch020data[0] = regs.prefetch020data[1];
		}
		fill_icache030 (pc + 0, 0);
		fill_icache030 (pc + 4, 1);
	}
}


void flush_dcache (uaecptr addr, int size)
{
	int i;
	if (!currprefs.cpu_cycle_exact)
		return;
	if (currprefs.cpu_model >= 68030) {
		for (i = 0; i < CACHELINES030; i++) {
			dcaches030[i].valid[0] = 0;
			dcaches030[i].valid[1] = 0;
			dcaches030[i].valid[2] = 0;
			dcaches030[i].valid[3] = 0;
		}
	}
}

void do_cycles_ce020 (int clocks)
{
	do_cycles_ce (clocks * cpucycleunit);
}
void do_cycles_ce020_mem (int clocks)
{
	regs.ce020memcycles -= clocks * cpucycleunit;
	do_cycles_ce (clocks * cpucycleunit);
}

void do_cycles_ce000 (int clocks)
{
	do_cycles_ce (clocks * cpucycleunit);
}

void m68k_do_rte_mmu (uaecptr a7)
{
	uae_u16 ssr = get_word_mmu (a7 + 8 + 4);
	if (ssr & MMU_SSW_CT) {
		uaecptr src_a7 = a7 + 8 - 8;
		uaecptr dst_a7 = a7 + 8 + 52;
		put_word_mmu (dst_a7 + 0, get_word_mmu (src_a7 + 0));
		put_long_mmu (dst_a7 + 2, get_long_mmu (src_a7 + 2));
		// skip this word
		put_long_mmu (dst_a7 + 8, get_long_mmu (src_a7 + 8));
	}
}

void flush_mmu (uaecptr addr, int n)
{
}

void m68k_do_rts_mmu (void)
{
	m68k_setpc (get_long_mmu (m68k_areg (regs, 7)));
	m68k_areg (regs, 7) += 4;
}

void m68k_do_bsr_mmu (uaecptr oldpc, uae_s32 offset)
{
	put_long_mmu (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_incpci (offset);
}
