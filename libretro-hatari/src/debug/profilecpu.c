/*
 * Hatari - profilecpu.c
 * 
 * Copyright (C) 2010-2013 by Eero Tamminen
 *
 * This file is distributed under the GNU General Public License, version 2
 * or at your option any later version. Read the file gpl.txt for details.
 *
 * profilecpu.c - functions for profiling CPU and showing the results.
 */
const char Profilecpu_fileid[] = "Hatari profilecpu.c : " __DATE__ " " __TIME__;

#include <stdio.h>
#include <inttypes.h>
#include <assert.h>
#include "main.h"
#include "configuration.h"
#include "clocks_timings.h"
#include "debugInfo.h"
#include "dsp.h"
#include "m68000.h"
#include "68kDisass.h"
#include "profile.h"
#include "profile_priv.h"
#include "stMemory.h"
#include "symbols.h"
#include "tos.h"
#include "screen.h"
#include "video.h"


/* cartridge area */
#define CART_START	0xFA0000
#define CART_END	0xFC0000
#define CART_SIZE	(CART_END - CART_START)


/* if non-zero, output (more) warnings on suspicious:
 * - cycle/instruction counts
 * - PC switches
 * And drop to debugger on invalid PC addresses.
 */
#define DEBUG 0
#if DEBUG
#include "debugui.h"
static bool skip_assert;
#endif

static callinfo_t cpu_callinfo;

/* This is relevant with WinUAE CPU core:
 * - the default cycle exact variant needs this define to be non-zero
 * - non-cycle exact and MMU variants need this define to be 0
 *   for cycle counts to make any sense
 */
#define USE_CYCLES_COUNTER 1

#define MAX_CPU_PROFILE_VALUE 0xFFFFFFFF

typedef struct {
	Uint32 count;	/* how many times this address instrcution is executed */
	Uint32 cycles;	/* how many CPU cycles was taken at this address */
	Uint32 misses;  /* how many CPU cache misses happened at this address */
} cpu_profile_item_t;

#define MAX_MISS 4

static struct {
	counters_t all;       /* total counts for all areas */
	Uint32 miss_counts[MAX_MISS];  /* cache miss counts */
	cpu_profile_item_t *data; /* profile data items */
	Uint32 size;          /* number of allocated profile data items */
	profile_area_t ram;   /* normal RAM stats */
	profile_area_t rom;   /* cartridge ROM stats */
	profile_area_t tos;   /* ROM TOS stats */
	int active;           /* number of active data items in all areas */
	Uint32 *sort_arr;     /* data indexes used for sorting */
	int prev_family;      /* previous instruction opcode family */
	Uint32 prev_cycles;   /* previous instruction cycles counter */
	Uint32 prev_pc;       /* previous instruction address */
	Uint32 loop_start;    /* address of last loop start */
	Uint32 loop_end;      /* address of last loop end */
	Uint32 loop_count;    /* how many times it was looped */
	Uint32 disasm_addr;   /* 'addresses' command start address */
	bool processed;	      /* true when data is already processed */
	bool enabled;         /* true when profiling enabled */
} cpu_profile;

/* special hack for EmuTOS */
static Uint32 etos_switcher;


/* ------------------ CPU profile address mapping ----------------- */

/**
 * convert Atari memory address to sorting array profile data index.
 */
static inline Uint32 address2index(Uint32 pc)
{
	if (unlikely(pc & 1)) {
		fprintf(stderr, "WARNING: odd CPU profile instruction address 0x%x!\n", pc);
#if DEBUG
		skip_assert = true;
		DebugUI(REASON_CPU_EXCEPTION);
#endif
	}
	if (pc < STRamEnd) {
		/* most likely case, use RAM address as-is */

	} else if (pc >= TosAddress && pc < TosAddress + TosSize) {
		/* TOS, put it after RAM data */
		pc = pc - TosAddress + STRamEnd;
		if (TosAddress >= CART_END) {
			/* and after cartridge data as it's higher */
			pc += CART_SIZE;
		}
	} else if (pc >= CART_START && pc < CART_END) {
		/* ROM, put it after RAM data */
		pc = pc - CART_START + STRamEnd;
		if (TosAddress < CART_START) {
			/* and after TOS as it's higher */
			pc += TosSize;
		}
	} else {
		fprintf(stderr, "WARNING: 'invalid' CPU PC profile instruction address 0x%x!\n", pc);
		/* extra entry at end is reserved for invalid PC values */
		pc = STRamEnd + TosSize + 0x20000;
#if DEBUG
		skip_assert = true;
		DebugUI(REASON_CPU_EXCEPTION);
#endif
	}
	/* CPU instructions are at even addresses, save space by halving */
	return (pc >> 1);
}

/**
 * convert sorting array profile data index to Atari memory address.
 */
static Uint32 index2address(Uint32 idx)
{
	idx <<= 1;
	/* RAM */
	if (idx < STRamEnd) {
		return idx;
	}
	idx -= STRamEnd;
	/* TOS before cartridge area? */
	if (TosAddress < CART_START) {
		/* TOS */
		if (idx < TosSize) {
			return idx + TosAddress;
		}
		idx -= TosSize;
		/* ROM */
		return idx + CART_START;
	} else {
		/* ROM */
		if (idx < CART_SIZE) {
			return idx + CART_START;
		}
		idx -= CART_SIZE;
		/* TOS */
		return idx + TosAddress;
	}
}

/* ------------------ CPU profile results ----------------- */

/**
 * Get CPU cycles, count and count percentage for given address.
 * Return true if data was available and non-zero, false otherwise.
 */
bool Profile_CpuAddressData(Uint32 addr, float *percentage, Uint32 *count, Uint32 *cycles, Uint32 *misses)
{
	Uint32 idx;
	if (!cpu_profile.data) {
		return false;
	}
	idx = address2index(addr);
	*misses = cpu_profile.data[idx].misses;
	*cycles = cpu_profile.data[idx].cycles;
	*count = cpu_profile.data[idx].count;
	if (cpu_profile.all.count) {
		*percentage = 100.0*(*count)/cpu_profile.all.count;
	} else {
		*percentage = 0.0;
	}
	return (*count > 0);
}

/**
 * Helper to show statistics for specified CPU profile area.
 */
static void show_cpu_area_stats(profile_area_t *area)
{
	if (!area->active) {
		fprintf(stderr, "- no activity\n");
		return;
	}
	fprintf(stderr, "- active address range:\n  0x%06x-0x%06x\n",
		index2address(area->lowest),
		index2address(area->highest));
	fprintf(stderr, "- active instruction addresses:\n  %d (%.2f%% of all)\n",
		area->active,
		100.0 * area->active / cpu_profile.active);
	fprintf(stderr, "- executed instructions:\n  %"PRIu64" (%.2f%% of all)\n",
		area->counters.count,
		100.0 * area->counters.count / cpu_profile.all.count);
#if ENABLE_WINUAE_CPU
	if (cpu_profile.all.misses) {	/* CPU cache in use? */
		fprintf(stderr, "- instruction cache misses:\n  %"PRIu64" (%.2f%% of all)\n",
			area->counters.misses,
			100.0 * area->counters.misses / cpu_profile.all.misses);
	}
#endif
	fprintf(stderr, "- used cycles:\n  %"PRIu64" (%.2f%% of all)\n  = %.5fs\n",
		area->counters.cycles,
		100.0 * area->counters.cycles / cpu_profile.all.cycles,
		(double)area->counters.cycles / MachineClocks.CPU_Freq);
	if (area->overflow) {
		fprintf(stderr, "  *** COUNTER OVERFLOW! ***\n");
	}
}


/**
 * show CPU area (RAM, ROM, TOS) specific statistics.
 */
void Profile_CpuShowStats(void)
{
	fprintf(stderr, "Normal RAM (0-0x%X):\n", STRamEnd);
	show_cpu_area_stats(&cpu_profile.ram);

	fprintf(stderr, "ROM TOS (0x%X-0x%X):\n", TosAddress, TosAddress + TosSize);
	show_cpu_area_stats(&cpu_profile.tos);

	fprintf(stderr, "Cartridge ROM (0x%X-%X):\n", CART_START, CART_END);
	show_cpu_area_stats(&cpu_profile.rom);

	fprintf(stderr, "\n= %.5fs\n",
		(double)cpu_profile.all.cycles / MachineClocks.CPU_Freq);

#if ENABLE_WINUAE_CPU
	if (cpu_profile.all.misses) {	/* CPU cache in use? */
		int i;
		fprintf(stderr, "\nCache misses per instruction, number of occurrences:\n");
		for (i = 0; i < MAX_MISS; i++) {
			fprintf(stderr, "- %d: %d\n", i, cpu_profile.miss_counts[i]);
		}
	}
#endif
}

/**
 * Show CPU instructions which execution was profiled, in the address order,
 * starting from the given address.  Return next disassembly address.
 */
Uint32 Profile_CpuShowAddresses(Uint32 lower, Uint32 upper, FILE *out)
{
	int oldcols[DISASM_COLUMNS], newcols[DISASM_COLUMNS];
	int show, shown, active;
	const char *symbol;
	cpu_profile_item_t *data;
	Uint32 idx, end, size;
	uaecptr nextpc, addr;

	data = cpu_profile.data;
	if (!data) {
		fprintf(stderr, "ERROR: no CPU profiling data available!\n");
		return 0;
	}

	size = cpu_profile.size;
	active = cpu_profile.active;
	if (upper) {
		end = address2index(upper);
		show = active;
		if (end > size) {
			end = size;
		}
	} else {
		end = size;
		show = ConfigureParams.Debugger.nDisasmLines;
		if (!show || show > active) {
			show = active;
		}
	}

	/* get/change columns */
	Disasm_GetColumns(oldcols);
	Disasm_DisableColumn(DISASM_COLUMN_HEXDUMP, oldcols, newcols);
	Disasm_SetColumns(newcols);

	fputs("# disassembly with profile data: <instructions percentage>% (<sum of instructions>, <sum of cycles>, <sum of i-cache misses>)\n", out);

	nextpc = 0;
	idx = address2index(lower);
	for (shown = 0; shown < show && idx < end; idx++) {
		if (!data[idx].count) {
			continue;
		}
		addr = index2address(idx);
		if (addr != nextpc && nextpc) {
			fprintf(out, "[...]\n");
		}
		symbol = Symbols_GetByCpuAddress(addr);
		if (symbol) {
			fprintf(out, "%s:\n", symbol);
		}
		/* NOTE: column setup works only with 68kDisass disasm engine! */
		Disasm(out, addr, &nextpc, 1);
		shown++;
	}
	printf("Disassembled %d (of active %d) CPU addresses.\n", shown, active);

	/* restore disassembly columns */
	Disasm_SetColumns(oldcols);
	return nextpc;
}

/**
 * remove all disassembly columns except instruction ones.
 * data needed to restore columns is stored to "oldcols"
 */
static void leave_instruction_column(int *oldcols)
{
	int i, newcols[DISASM_COLUMNS];

	Disasm_GetColumns(oldcols);
	for (i = 0; i < DISASM_COLUMNS; i++) {
		if (i == DISASM_COLUMN_OPCODE || i == DISASM_COLUMN_OPERAND) {
			continue;
		}
		Disasm_DisableColumn(i, oldcols, newcols);
		oldcols = newcols;
	}
	Disasm_SetColumns(newcols);
}

#if ENABLE_WINUAE_CPU
/**
 * compare function for qsort() to sort CPU profile data by instruction cache misses.
 */
static int cmp_cpu_misses(const void *p1, const void *p2)
{
	Uint32 count1 = cpu_profile.data[*(const Uint32*)p1].misses;
	Uint32 count2 = cpu_profile.data[*(const Uint32*)p2].misses;
	if (count1 > count2) {
		return -1;
	}
	if (count1 < count2) {
		return 1;
	}
	return 0;
}

/**
 * Sort CPU profile data addresses by instruction cache misses and show the results.
 */
void Profile_CpuShowMisses(int show)
{
	int active;
	int oldcols[DISASM_COLUMNS];
	Uint32 *sort_arr, *end, addr, nextpc;
	cpu_profile_item_t *data = cpu_profile.data;
	float percentage;
	Uint32 count;

	if (!cpu_profile.all.misses) {
		fprintf(stderr, "No CPU cache miss information available.\n");
		return;
	}

	active = cpu_profile.active;
	sort_arr = cpu_profile.sort_arr;
	qsort(sort_arr, active, sizeof(*sort_arr), cmp_cpu_misses);

	leave_instruction_column(oldcols);

	printf("addr:\t\tmisses:\n");
	show = (show < active ? show : active);
	for (end = sort_arr + show; sort_arr < end; sort_arr++) {
		addr = index2address(*sort_arr);
		count = data[*sort_arr].misses;
		percentage = 100.0*count/cpu_profile.all.misses;
		printf("0x%06x\t%5.2f%%\t%d%s\t", addr, percentage, count,
		       count == MAX_CPU_PROFILE_VALUE ? " (OVERFLOW)" : "");
		Disasm(stdout, addr, &nextpc, 1);
	}
	printf("%d CPU addresses listed.\n", show);

	Disasm_SetColumns(oldcols);
}
#else
void Profile_CpuShowMisses(int show) {
	fprintf(stderr, "Cache misses are recorded only with WinUAE CPU.\n");
}
#endif


/**
 * compare function for qsort() to sort CPU profile data by cycles counts.
 */
static int cmp_cpu_cycles(const void *p1, const void *p2)
{
	Uint32 count1 = cpu_profile.data[*(const Uint32*)p1].cycles;
	Uint32 count2 = cpu_profile.data[*(const Uint32*)p2].cycles;
	if (count1 > count2) {
		return -1;
	}
	if (count1 < count2) {
		return 1;
	}
	return 0;
}

/**
 * Sort CPU profile data addresses by cycle counts and show the results.
 */
void Profile_CpuShowCycles(int show)
{
	int active;
	int oldcols[DISASM_COLUMNS];
	Uint32 *sort_arr, *end, addr, nextpc;
	cpu_profile_item_t *data = cpu_profile.data;
	float percentage;
	Uint32 count;

	if (!data) {
		fprintf(stderr, "ERROR: no CPU profiling data available!\n");
		return;
	}

	active = cpu_profile.active;
	sort_arr = cpu_profile.sort_arr;
	qsort(sort_arr, active, sizeof(*sort_arr), cmp_cpu_cycles);

	leave_instruction_column(oldcols);

	printf("addr:\t\tcycles:\n");
	show = (show < active ? show : active);
	for (end = sort_arr + show; sort_arr < end; sort_arr++) {
		addr = index2address(*sort_arr);
		count = data[*sort_arr].cycles;
		percentage = 100.0*count/cpu_profile.all.cycles;
		printf("0x%06x\t%5.2f%%\t%d%s\t", addr, percentage, count,
		       count == MAX_CPU_PROFILE_VALUE ? " (OVERFLOW)" : "");
		Disasm(stdout, addr, &nextpc, 1);
	}
	printf("%d CPU addresses listed.\n", show);

	Disasm_SetColumns(oldcols);
}

/**
 * compare function for qsort() to sort CPU profile data by descending
 * address access counts.
 */
static int cmp_cpu_count(const void *p1, const void *p2)
{
	Uint32 count1 = cpu_profile.data[*(const Uint32*)p1].count;
	Uint32 count2 = cpu_profile.data[*(const Uint32*)p2].count;
	if (count1 > count2) {
		return -1;
	}
	if (count1 < count2) {
		return 1;
	}
	return 0;
}

/**
 * Sort CPU profile data addresses by call counts and show the results.
 * If symbols are requested and symbols are loaded, show (only) addresses
 * matching a symbol.
 */
void Profile_CpuShowCounts(int show, bool only_symbols)
{
	cpu_profile_item_t *data = cpu_profile.data;
	int symbols, matched, active;
	int oldcols[DISASM_COLUMNS];
	Uint32 *sort_arr, *end, addr, nextpc;
	const char *name;
	float percentage;
	Uint32 count;

	if (!data) {
		fprintf(stderr, "ERROR: no CPU profiling data available!\n");
		return;
	}
	active = cpu_profile.active;
	show = (show < active ? show : active);

	sort_arr = cpu_profile.sort_arr;
	qsort(sort_arr, active, sizeof(*sort_arr), cmp_cpu_count);

	if (!only_symbols) {
		leave_instruction_column(oldcols);
		printf("addr:\t\tcount:\n");
		for (end = sort_arr + show; sort_arr < end; sort_arr++) {
			addr = index2address(*sort_arr);
			count = data[*sort_arr].count;
			percentage = 100.0*count/cpu_profile.all.count;
			printf("0x%06x\t%5.2f%%\t%d%s\t",
			       addr, percentage, count,
			       count == MAX_CPU_PROFILE_VALUE ? " (OVERFLOW)" : "");
			Disasm(stdout, addr, &nextpc, 1);
		}
		printf("%d CPU addresses listed.\n", show);
		Disasm_SetColumns(oldcols);
		return;
	}

	symbols = Symbols_CpuCount();
	if (!symbols) {
		fprintf(stderr, "ERROR: no CPU symbols loaded!\n");
		return;
	}
	matched = 0;	

	leave_instruction_column(oldcols);

	printf("addr:\t\tcount:\t\tsymbol:\n");
	for (end = sort_arr + active; sort_arr < end; sort_arr++) {

		addr = index2address(*sort_arr);
		name = Symbols_GetByCpuAddress(addr);
		if (!name) {
			continue;
		}
		count = data[*sort_arr].count;
		percentage = 100.0*count/cpu_profile.all.count;
		printf("0x%06x\t%5.2f%%\t%d\t%s%s\t",
		       addr, percentage, count, name,
		       count == MAX_CPU_PROFILE_VALUE ? " (OVERFLOW)" : "");
		Disasm(stdout, addr, &nextpc, 1);

		matched++;
		if (matched >= show || matched >= symbols) {
			break;
		}
	}
	printf("%d CPU symbols listed.\n", matched);

	Disasm_SetColumns(oldcols);
}


static const char * addr2name(Uint32 addr, Uint64 *total)
{
	Uint32 idx = address2index(addr);
	*total = cpu_profile.data[idx].count;
	return Symbols_GetByCpuAddress(addr);
}

/**
 * Output CPU callers info to given file.
 */
void Profile_CpuShowCallers(FILE *fp)
{
	Profile_ShowCallers(fp, cpu_callinfo.sites, cpu_callinfo.site, addr2name);
}

/**
 * Save CPU profile information to given file.
 */
void Profile_CpuSave(FILE *out)
{
	Uint32 text;
	fputs("Field names:\tExecuted instructions, Used cycles, Instruction cache misses\n", out);
	/* (Python) pegexp that matches address and all describled fields from disassembly:
	 * $<hex>  :  <ASM>  <percentage>% (<count>, <cycles>, <misses>)
	 * $e5af38 :   rts           0.00% (12, 0, 12)
	 */
	fputs("Field regexp:\t^\\$([0-9a-f]+) :.*% \\((.*)\\)$\n", out);
	/* some information for interpreting the addresses */
	fprintf(out, "ROM_TOS:\t0x%06x-0x%06x\n", TosAddress, TosAddress + TosSize);
	text = DebugInfo_GetTEXT();
	if (text < TosAddress) {
		fprintf(out, "PROGRAM_TEXT:\t0x%06x-0x%06x\n", text, DebugInfo_GetTEXTEnd());
	}
	fprintf(out, "CARTRIDGE:\t0x%06x-0x%06x\n", CART_START, CART_END);
	Profile_CpuShowAddresses(0, CART_END-2, out);
	Profile_CpuShowCallers(out);
}

/* ------------------ CPU profile control ----------------- */

/**
 * Initialize CPU profiling when necessary.  Return true if profiling.
 */
bool Profile_CpuStart(void)
{
	int size;

	Profile_FreeCallinfo(&(cpu_callinfo));
	if (cpu_profile.sort_arr) {
		/* remove previous results */
		free(cpu_profile.sort_arr);
		free(cpu_profile.data);
		cpu_profile.sort_arr = NULL;
		cpu_profile.data = NULL;
		printf("Freed previous CPU profile buffers.\n");
	}
	if (!cpu_profile.enabled) {
		return false;
	}
	/* zero everything */
	memset(&cpu_profile, 0, sizeof(cpu_profile));

	/* Shouldn't change within same debug session */
	size = (STRamEnd + 0x20000 + TosSize) / 2;

	/* Add one entry for catching invalid PC values */
	cpu_profile.data = calloc(size + 1, sizeof(*cpu_profile.data));
	if (!cpu_profile.data) {
		perror("ERROR, new CPU profile buffer alloc failed");
		return false;
	}
	printf("Allocated CPU profile buffer (%d MB).\n",
	       (int)sizeof(*cpu_profile.data)*size/(1024*1024));
	cpu_profile.size = size;

	Profile_AllocCallinfo(&(cpu_callinfo), Symbols_CpuCount(), "CPU");

	/* special hack for EmuTOS */
	etos_switcher = PC_UNDEFINED;
	if (cpu_callinfo.sites && bIsEmuTOS &&
	    (!Symbols_GetCpuAddress(SYMTYPE_TEXT, "_switchto", &etos_switcher) || etos_switcher < TosAddress)) {
		etos_switcher = PC_UNDEFINED;
	}

	cpu_profile.prev_cycles = Cycles_GetCounter(CYCLES_COUNTER_CPU);
	cpu_profile.prev_family = OpcodeFamily;
	cpu_profile.prev_pc = M68000_GetPC() & 0xffffff;

	cpu_profile.loop_start = PC_UNDEFINED;
	cpu_profile.loop_end = PC_UNDEFINED;
	cpu_profile.loop_count = 0;
	Profile_LoopReset();

	cpu_profile.disasm_addr = 0;
	cpu_profile.processed = false;
	cpu_profile.enabled = true;
	return cpu_profile.enabled;
}

/**
 * return true if pc could be next instruction for previous pc
 */
static bool is_prev_instr(Uint32 prev_pc, Uint32 pc)
{
	/* just moved to next instruction (1-2 words)? */
	if (prev_pc < pc && (pc - prev_pc) <= 10) {
		return true;
	}
	return false;
}

/**
 * return caller instruction type classification
 */
static calltype_t cpu_opcode_type(int family, Uint32 prev_pc, Uint32 pc)
{
	switch (family) {

	case i_JSR:
	case i_BSR:
		return CALL_SUBROUTINE;

	case i_RTS:
	case i_RTR:
	case i_RTD:
		return CALL_SUBRETURN;

	case i_JMP:	/* often used also for "inlined" function calls... */
	case i_Bcc:	/* both BRA & BCC */
	case i_FBcc:
	case i_DBcc:
	case i_FDBcc:
		return CALL_BRANCH;

	case i_TRAP:
	case i_TRAPV:
	case i_TRAPcc:
	case i_FTRAPcc:
	case i_STOP:
	case i_ILLG:
	case i_CHK:
	case i_CHK2:
	case i_BKPT:
		return CALL_EXCEPTION;

	case i_RTE:
		return CALL_EXCRETURN;
	}
	/* just moved to next instruction? */
	if (is_prev_instr(prev_pc, pc)) {
		return CALL_NEXT;
	}
	return CALL_UNKNOWN;
}

/**
 * If call tracking is enabled (there are symbols), collect
 * information about subroutine and other calls, and their costs.
 * 
 * Like with profile data, caller info checks need to be for previous
 * instruction, that's why "pc" argument for this function actually
 * needs to be previous PC.
 */
static void collect_calls(Uint32 pc, counters_t *counters)
{
	calltype_t flag;
	int idx, family;
	Uint32 prev_pc, caller_pc;

	family = cpu_profile.prev_family;
	cpu_profile.prev_family = OpcodeFamily;

	prev_pc = cpu_callinfo.prev_pc;
	cpu_callinfo.prev_pc = pc;
	caller_pc = PC_UNDEFINED;

	/* address is return address for last subroutine call? */
	if (unlikely(pc == cpu_callinfo.return_pc) && likely(cpu_callinfo.depth)) {

		flag = cpu_opcode_type(family, prev_pc, pc);
		/* previous address can be exception return (e.g. RTE) instead of RTS,
		 * if exception occurred right after returning from subroutine call.
		 */
		if (likely(flag == CALL_SUBRETURN || flag == CALL_EXCRETURN)) {
			caller_pc = Profile_CallEnd(&cpu_callinfo, counters);
		} else {
#if DEBUG
			/* although at return address, it didn't return yet,
			 * e.g. because there was a jsr or jump to return address
			 */
			Uint32 nextpc;
			fprintf(stderr, "WARNING: subroutine call returned 0x%x -> 0x%x, not through RTS!\n", prev_pc, pc);
			Disasm(stderr, prev_pc, &nextpc, 1);
#endif
		}
		/* next address might be another symbol, so need to fall through */
	}

	/* address is one which we're tracking? */
	idx = Symbols_GetCpuAddressIndex(pc);
	if (unlikely(idx >= 0)) {

		flag = cpu_opcode_type(family, prev_pc, pc);
		if (flag == CALL_SUBROUTINE || flag == CALL_EXCEPTION) {
			/* special HACK for for EmuTOS AES switcher which
			 * changes stack content to remove itself from call
			 * stack and uses RTS for subroutine *calls*, not
			 * for returning from them.
			 *
			 * It wouldn't be reliable to detect calls from it,
			 * so I'm making call *to* it show up as branch, to
			 * keep callstack depth correct.
			 */
			if (unlikely(pc == etos_switcher)) {
				flag = CALL_BRANCH;
			} else if (unlikely(prev_pc == PC_UNDEFINED)) {
				/* if first profiled instruction
				 * is subroutine call, it doesn't have
				 * valid prev_pc value stored
				 */
				cpu_callinfo.return_pc = PC_UNDEFINED;
				fprintf(stderr, "WARNING: previous PC from callinfo for 0x%d is undefined!\n", pc);
#if DEBUG
				skip_assert = true;
				DebugUI(REASON_CPU_EXCEPTION);
#endif
			} else {
				/* slow! */
				cpu_callinfo.return_pc = Disasm_GetNextPC(prev_pc);
			}
		} else if (caller_pc != PC_UNDEFINED) {
			/* returned from function to first instruction of another symbol:
			 *	0xf384	jsr some_function
			 *	other_symbol:
			 *	0f3x8a	some_instruction
			 * -> change return instruction address to
			 *    address of what did the returned call.
			 */
			prev_pc = caller_pc;
			assert(is_prev_instr(prev_pc, pc));
			flag = CALL_NEXT;
		}
		Profile_CallStart(idx, &cpu_callinfo, prev_pc, flag, pc, counters);
	}
}

/**
 * log last loop info, if there's suitable data for one
 */
static void log_last_loop(void)
{
	unsigned len = cpu_profile.loop_end - cpu_profile.loop_start;
	if (cpu_profile.loop_count > 1 && (len < profile_loop.cpu_limit || !profile_loop.cpu_limit)) {
		fprintf(profile_loop.fp, "CPU %d 0x%06x %d %d\n", nVBLs,
			cpu_profile.loop_start, len, cpu_profile.loop_count);
	}
}

/**
 * Update CPU cycle and count statistics for PC address.
 *
 * This gets called after instruction has executed and PC
 * has advanced to next instruction.
 */
void Profile_CpuUpdate(void)
{
	counters_t *counters = &(cpu_profile.all);
	Uint32 pc, prev_pc, idx, cycles, misses;
	cpu_profile_item_t *prev;

	prev_pc = cpu_profile.prev_pc;
	/* PC may have extra bits, they need to be masked away as
	 * emulation itself does that too when PC value is used
	 */
	cpu_profile.prev_pc = pc = M68000_GetPC() & 0xffffff;

	if (unlikely(profile_loop.fp)) {
		if (pc < prev_pc) {
			if (pc == cpu_profile.loop_start && prev_pc == cpu_profile.loop_end) {
				cpu_profile.loop_count++;
			} else {
				cpu_profile.loop_start = pc;
				cpu_profile.loop_end = prev_pc;
				cpu_profile.loop_count = 1;
			}
		} else {
			if (pc > cpu_profile.loop_end) {
				log_last_loop();
				cpu_profile.loop_end = 0xffffffff;			
				cpu_profile.loop_count = 0;
			}
		}
	}

	idx = address2index(prev_pc);
	assert(idx <= cpu_profile.size);
	prev = cpu_profile.data + idx;

	if (likely(prev->count < MAX_CPU_PROFILE_VALUE)) {
		prev->count++;
	}

#if USE_CYCLES_COUNTER
	/* Confusingly, with DSP enabled, cycle counter is for this instruction,
	 * without DSP enabled, it's a monotonically increasing counter.
	 */
	if (bDspEnabled) {
		cycles = Cycles_GetCounter(CYCLES_COUNTER_CPU);
	} else {
		Uint32 newcycles = Cycles_GetCounter(CYCLES_COUNTER_CPU);
		cycles = newcycles - cpu_profile.prev_cycles;
		cpu_profile.prev_cycles = newcycles;
	}
#else
	cycles = CurrentInstrCycles + nWaitStateCycles;
#endif
	/* cycles are based on 8Mhz clock, change them to correct one */
	cycles <<= nCpuFreqShift;

	if (likely(prev->cycles < MAX_CPU_PROFILE_VALUE - cycles)) {
		prev->cycles += cycles;
	} else {
		prev->cycles = MAX_CPU_PROFILE_VALUE;
	}

#if ENABLE_WINUAE_CPU
	misses = CpuInstruction.iCacheMisses;
	assert(misses < MAX_MISS);
	cpu_profile.miss_counts[misses]++;
	if (likely(prev->misses < MAX_CPU_PROFILE_VALUE - misses)) {
		prev->misses += misses;
	} else {
		prev->misses = MAX_CPU_PROFILE_VALUE;
	}
#else
	misses = 0;
#endif
	if (cpu_callinfo.sites) {
		collect_calls(prev_pc, counters);
	}
	/* counters are increased after caller info is processed,
	 * otherwise cost for the instruction calling the callee
	 * doesn't get accounted to caller (but callee).
	 */
	counters->misses += misses;
	counters->cycles += cycles;
	counters->count++;

#if DEBUG
	if (unlikely(OpcodeFamily == 0)) {
		Uint32 nextpc;
		fputs("WARNING: instruction opcode family is zero (=i_ILLG) for instruction:\n", stderr);
		Disasm(stderr, prev_pc, &nextpc, 1);
	}
	/* catch too large (and negative) cycles for other than STOP instruction */
	if (unlikely(cycles > 512 && OpcodeFamily != i_STOP)) {
		Uint32 nextpc;
		fprintf(stderr, "WARNING: cycles %d > 512:\n", cycles);
		Disasm(stderr, prev_pc, &nextpc, 1);
	}
	if (unlikely(cycles == 0)) {
		Uint32 nextpc;
		fputs("WARNING: Zero cycles for an opcode:\n", stderr);
		Disasm(stderr, prev_pc, &nextpc, 1);
	}
#endif
}


/**
 * Helper for accounting CPU profile area item.
 */
static void update_area_item(profile_area_t *area, Uint32 addr, cpu_profile_item_t *item)
{
	Uint32 cycles = item->cycles;
	Uint32 count = item->count;

	if (!count) {
		return;
	}
	area->counters.count += count;
	area->counters.misses += item->misses;
	area->counters.cycles += cycles;

	if (cycles == MAX_CPU_PROFILE_VALUE) {
		area->overflow = true;
	}
	if (addr < area->lowest) {
		area->lowest = addr;
	}
	area->highest = addr;

	area->active++;
}

/**
 * Helper for collecting CPU profile area statistics.
 */
static Uint32 update_area(profile_area_t *area, Uint32 start, Uint32 end)
{
	cpu_profile_item_t *item;
	Uint32 addr;

	memset(area, 0, sizeof(profile_area_t));
	area->lowest = cpu_profile.size;

	item = &(cpu_profile.data[start]);
	for (addr = start; addr < end; addr++, item++) {
		update_area_item(area, addr, item);
	}
	return addr;
}

/**
 * Helper for initializing CPU profile area sorting indexes.
 */
static Uint32* index_area(profile_area_t *area, Uint32 *sort_arr)
{
	cpu_profile_item_t *item;
	Uint32 addr;

	item = &(cpu_profile.data[area->lowest]);
	for (addr = area->lowest; addr <= area->highest; addr++, item++) {
		if (item->count) {
			*sort_arr++ = addr;
		}
	}
	return sort_arr;
}

/**
 * Stop and process the CPU profiling data; collect stats and
 * prepare for more optimal sorting.
 */
void Profile_CpuStop(void)
{
	Uint32 *sort_arr, next;
	int active;

	if (cpu_profile.processed || !cpu_profile.enabled) {
		return;
	}

	log_last_loop();
	if (profile_loop.fp) {
		fflush(profile_loop.fp);
	}

	/* user didn't change RAM or TOS size in the meanwhile? */
	assert(cpu_profile.size == (STRamEnd + 0x20000 + TosSize) / 2);

	Profile_FinalizeCalls(&(cpu_callinfo), &(cpu_profile.all), Symbols_GetByCpuAddress);

	/* find lowest and highest addresses executed etc */
	next = update_area(&cpu_profile.ram, 0, STRamEnd/2);
	next = update_area(&cpu_profile.tos, next, (STRamEnd + TosSize)/2);
	next = update_area(&cpu_profile.rom, next, cpu_profile.size);
	assert(next == cpu_profile.size);

#if DEBUG
	if (skip_assert) {
		skip_assert = false;
	} else
#endif
	{
		assert(cpu_profile.all.misses == cpu_profile.ram.counters.misses + cpu_profile.tos.counters.misses + cpu_profile.rom.counters.misses);
		assert(cpu_profile.all.cycles == cpu_profile.ram.counters.cycles + cpu_profile.tos.counters.cycles + cpu_profile.rom.counters.cycles);
		assert(cpu_profile.all.count == cpu_profile.ram.counters.count + cpu_profile.tos.counters.count + cpu_profile.rom.counters.count);
	}

	/* allocate address array for sorting */
	active = cpu_profile.ram.active + cpu_profile.rom.active + cpu_profile.tos.active;
	sort_arr = calloc(active, sizeof(*sort_arr));

	if (!sort_arr) {
		perror("ERROR: allocating CPU profile address data");
		free(cpu_profile.data);
		cpu_profile.data = NULL;
		return;
	}
	printf("Allocated CPU profile address buffer (%d KB).\n",
	       (int)sizeof(*sort_arr)*(active+512)/1024);
	cpu_profile.sort_arr = sort_arr;
	cpu_profile.active = active;

	/* and fill addresses for used instructions... */
	sort_arr = index_area(&cpu_profile.ram, sort_arr);
	sort_arr = index_area(&cpu_profile.tos, sort_arr);
	sort_arr = index_area(&cpu_profile.rom, sort_arr);
	assert(sort_arr == cpu_profile.sort_arr + cpu_profile.active);
	//printf("%d/%d/%d\n", area->active, sort_arr-cpu_profile.sort_arr, active);

	Profile_CpuShowStats();
	cpu_profile.processed = true;
}

/**
 * Get pointers to CPU profile enabling and disasm address variables
 * for updating them (in parser).
 */
void Profile_CpuGetPointers(bool **enabled, Uint32 **disasm_addr)
{
	*disasm_addr = &cpu_profile.disasm_addr;
	*enabled = &cpu_profile.enabled;
}

/**
 * Get callinfo & symbol search pointers for stack walking.
 */
void Profile_CpuGetCallinfo(callinfo_t **callinfo, const char* (**get_symbol)(Uint32))
{
	*callinfo = &(cpu_callinfo);
	*get_symbol = Symbols_GetByCpuAddress;
}
