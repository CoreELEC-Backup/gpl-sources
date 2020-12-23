/*
* UAE - The Un*x Amiga Emulator
*
* MC68000 emulation generator
*
* This is a fairly stupid program that generates a lot of case labels that
* can be #included in a switch statement.
* As an alternative, it can generate functions that handle specific
* MC68000 instructions, plus a prototype header file and a function pointer
* array to look up the function for an opcode.
* Error checking is bad, an illegal table68k file will cause the program to
* call abort().
* The generated code is sometimes sub-optimal, an optimizing compiler should
* take care of this.
*
* The source for the insn timings is Markt & Technik's Amiga Magazin 8/1992.
*
* Copyright 1995, 1996, 1997, 1998, 1999, 2000 Bernd Schmidt
*/

#include "sysconfig.h"
#include "sysdeps.h"
#include <ctype.h>
#include <string.h>

#include "readcpu.h"
#include "falcon_cycle030.h"

#define BOOL_TYPE "int"
/* Define the minimal 680x0 where NV flags are not affected by xBCD instructions.  */
#define xBCD_KEEPS_NV_FLAGS 4

static FILE *headerfile;
static FILE *stblfile;

static int using_prefetch, using_indirect, using_mmu;
static int using_ce020;
static int using_exception_3;
static int using_ce;
static int cpu_level;
static int count_read, count_write, count_cycles, count_ncycles;
static int count_read_ea, count_write_ea, count_cycles_ea;
static const char *mmu_postfix;

static int optimized_flags;

long nCurInstrCycPos;  /* Hatari only : Stores where we have to patch in the current cycles value */

#define GF_APDI 1
#define GF_AD8R 2
#define GF_PC8R 4
#define GF_AA 7
#define GF_NOREFILL 8
#define GF_PREFETCH 16
#define GF_FC 32
#define GF_MOVE 64

/* For the current opcode, the next lower level that will have different code.
* Initialized to -1 for each opcode. If it remains unchanged, indicates we
* are done with that opcode.  */
static int next_cpu_level;

static int *opcode_map;
static int *opcode_next_clev;
static int *opcode_last_postfix;
static unsigned long *counts;
static int generate_stbl;
static int fixupcnt;

static int instr_index = 0;		/* Hatari: Added to give a unique index to each 68030 cycle exact instruction (CPU_EMU_21) */
static int instr_table;			/* Hatari: Used to know which cycle table to use (CPU_EMU_21) */
static int instr_table_index;		/* Hatari: Used when the cycle table is not the generic one to set the line of the table (CPU_EMU_21) */

/* used to disable the return cycles function call when there are more than one cycle to take into account for an instruction */
/* Falcon CPU only (CPUEMU_21)) */
static bool no_return_cycles;

/* All the following indexes are useful to get the correct cycle index in the cycle tables for 68030 cycle exact CPU (CPU_21) */
/* Hatari only code */
static int chk_w_index = 0;
static int chk_l_index = 0;
static int chk2_b_index = 0;
static int chk2_w_index = 0;
static int chk2_l_index = 0;
static int cas_b_index = 0;
static int cas_w_index = 0;
static int cas_l_index = 0;
static int divl_index = 0;

/* These defines describe the different tables used for 68030 Cycle exact (Hatari only) */
#define TABLE_FALCON_CYCLES			0
#define TABLE_FALCON_CYCLES_CHK2_BW		1
#define TABLE_FALCON_CYCLES_CHK2_L		2
#define TABLE_FALCON_CYCLES_CHK_L		3
#define TABLE_FALCON_CYCLES_CHK_W		4
#define TABLE_FALCON_CYCLES_CAS_BW		5
#define TABLE_FALCON_CYCLES_CAS_L		6
#define TABLE_FALCON_CYCLES_CAS2_W		7
#define TABLE_FALCON_CYCLES_CAS2_L		8
#define TABLE_FALCON_CYCLES_RTE			9
#define TABLE_FALCON_CYCLES_BCC			10
#define TABLE_FALCON_CYCLES_DBCC		11
#define TABLE_FALCON_CYCLES_TRAPCC		12
#define TABLE_FALCON_CYCLES_TRAPCC_W		13
#define TABLE_FALCON_CYCLES_TRAPCC_L		14
#define TABLE_FALCON_CYCLES_TRAPV		15
#define TABLE_FALCON_CYCLES_DIVU_L		16
#define TABLE_FALCON_CYCLES_DIVS_L		17
#define TABLE_FALCON_CYCLES_BFTST_MEM		18
#define TABLE_FALCON_CYCLES_BFEXTU_MEM		19
#define TABLE_FALCON_CYCLES_BFCHG_MEM		20
#define TABLE_FALCON_CYCLES_BFEXTS_MEM		21
#define TABLE_FALCON_CYCLES_BFCLR_MEM		22
#define TABLE_FALCON_CYCLES_BFFFO_MEM		23
#define TABLE_FALCON_CYCLES_BFSET_MEM		24
#define TABLE_FALCON_CYCLES_BFINS_MEM		25
#define TABLE_FALCON_CYCLES_LSD			26
#define TABLE_FALCON_CYCLES_ASR			27
#define TABLE_FALCON_CYCLES_MOVEC		28
#define TABLE_FALCON_CYCLES_MOVES_EA_RN		29
#define TABLE_FALCON_CYCLES_MOVES_RN_EA		30
#define TABLE_FALCON_CYCLES_68030_INTERNAL	31


#define GENA_GETV_NO_FETCH	0
#define GENA_GETV_FETCH		1
#define GENA_GETV_FETCH_ALIGN	2
#define GENA_MOVEM_DO_INC	0
#define GENA_MOVEM_NO_INC	1
#define GENA_MOVEM_MOVE16	2

/**
  Table_labels table for all case.
  */
static const char* falcon_cycles_tables[] = {
	"table_falcon_cycles",
	"table_falcon_cycles_CHK2_BW",
	"table_falcon_cycles_CHK2_L",
	"table_falcon_cycles_CHK_L",
	"table_falcon_cycles_CHK_W",
	"table_falcon_cycles_CAS_BW",
	"table_falcon_cycles_CAS_L",
	"table_falcon_cycles_CAS2_W",
	"table_falcon_cycles_CAS2_L",
	"table_falcon_cycles_RTE",
	"table_falcon_cycles_Bcc",
	"table_falcon_cycles_DBcc",
	"table_falcon_cycles_TRAPcc",
	"table_falcon_cycles_TRAPcc_W",
	"table_falcon_cycles_TRAPcc_L",
	"table_falcon_cycles_TRAPV",
	"table_falcon_cycles_DIVU_L",
	"table_falcon_cycles_DIVS_L",
	"table_falcon_cycles_BFTST_Mem",
	"table_falcon_cycles_BFEXTU_Mem",
	"table_falcon_cycles_BFCHG_Mem",
	"table_falcon_cycles_BFEXTS_Mem",
	"table_falcon_cycles_BFCLR_Mem",
	"table_falcon_cycles_BFFFO_Mem",
	"table_falcon_cycles_BFSET_Mem",
	"table_falcon_cycles_BFINS_Mem",
	"table_falcon_cycles_LSD",
	"table_falcon_cycles_ASR",
	"table_falcon_cycles_MOVEC",
	"table_falcon_cycles_MOVES_EA_RN",
	"table_falcon_cycles_MOVES_RN_EA",
	"table_falcon_cycles_68030_internal"
};

static void read_counts (void)
{
	FILE *file;
	unsigned long opcode, count, total;
	char name[20];
	int nr = 0;
	memset (counts, 0, 65536 * sizeof *counts);

	count = 0;
	file = fopen ("frequent.68k", "r");
	if (file) {
		if (fscanf (file, "Total: %lu\n", &total) == EOF) {
			perror("read_counts");
		}
		while (fscanf (file, "%lx: %lu %s\n", &opcode, &count, name) == 3) {
			opcode_next_clev[nr] = 5;
			opcode_last_postfix[nr] = -1;
			opcode_map[nr++] = opcode;
			counts[opcode] = count;
		}
		fclose (file);
	}
	if (nr == nr_cpuop_funcs)
		return;
	for (opcode = 0; opcode < 0x10000; opcode++) {
		if (table68k[opcode].handler == -1 && table68k[opcode].mnemo != i_ILLG
			&& counts[opcode] == 0)
		{
			opcode_next_clev[nr] = 5;
			opcode_last_postfix[nr] = -1;
			opcode_map[nr++] = opcode;
			counts[opcode] = count;
		}
	}
	if (nr != nr_cpuop_funcs)
		abort ();
}

static char endlabelstr[80];
static int endlabelno = 0;
static int need_endlabel;

static int n_braces, limit_braces;
static int m68k_pc_offset;
static int insn_n_cycles, insn_n_cycles020;

static void fpulimit (void)
{
	if (limit_braces)
		return;
	printf ("\n#ifdef FPUEMU\n");
	limit_braces = n_braces;
	n_braces = 0;
}

static void returncycles (const char *s, int cycles)
{
	if (using_ce)
		return;

	if (using_ce020 == 1)
		printf ("%sreturn;\n", s);
	else if (using_ce020 == 2) {
		if (no_return_cycles == true)
			return;
		// TODO: This first if statement should be removed when all the cycles are correctly generated for the Falcon 68030 CPU
		// The next "else" should be removed too and only the printf line should be keept.
		if (instr_table == TABLE_FALCON_CYCLES && table_falcon_cycles[instr_index].cache_cycles == 0) {
			// set a temporary false value for this instruction cycles timing.
			printf ("\tregs.ce030_instr_cycles = table_falcon_cycles[0];\n");
		}
		else {
			printf ("%s\tregs.ce030_instr_cycles = %s[%d];\n", s, falcon_cycles_tables[instr_table], instr_table_index);
		}

		printf ("%s\treturn;\n", s);
	}
	else
		printf ("%sreturn %d * CYCLE_UNIT / 2;\n", s, cycles);
}

/* Todo : remove this function when all Falcon cycles are coded */
static void addcycles_ce020 (int head, int tail, int cache_cycles, int noncache_cycles)
{
	if (!using_ce020)
		return;
	if (cache_cycles > 0 && noncache_cycles > 0) {
		//not needed anymore
	}
	count_cycles += noncache_cycles;
}

static void addcycles000 (int cycles)
{
	if (!using_ce)
		return;
	printf ("\tdo_cycles_ce000 (%d);\n", cycles);
	count_cycles += cycles;
}
static void addcycles000_2 (const char *s, int cycles)
{
	if (!using_ce)
		return;
	printf ("%sdo_cycles_ce000 (%d);\n", s, cycles);
	count_cycles += cycles;
}

static void addcycles000_3 (const char *s)
{
	if (!using_ce)
		return;
	printf ("%sif (cycles > 0) do_cycles_ce000 (cycles);\n", s);
	count_ncycles++;
}

static int isreg (amodes mode)
{
	if (mode == Dreg || mode == Areg)
		return 1;
	return 0;
}

static void start_brace (void)
{
	n_braces++;
	printf ("{");
}

static void close_brace (void)
{
	assert (n_braces > 0);
	n_braces--;
	printf ("}");
}

static void finish_braces (void)
{
	while (n_braces > 0)
		close_brace ();
}

static void pop_braces (int to)
{
	while (n_braces > to)
		close_brace ();
}

static int bit_size (int size)
{
	switch (size) {
	case sz_byte: return 8;
	case sz_word: return 16;
	case sz_long: return 32;
	default: abort ();
	}
	return 0;
}

static const char *bit_mask (int size)
{
	switch (size) {
	case sz_byte: return "0xff";
	case sz_word: return "0xffff";
	case sz_long: return "0xffffffff";
	default: abort ();
	}
	return 0;
}

static const char *srcl, *dstl;
static const char *srcw, *dstw;
static const char *srcb, *dstb;
static const char *ce020_prefetch_long;
static const char *ce020_prefetch_word;

static void gen_nextilong (const char *type, const char *name, int flags)
{
	int r = m68k_pc_offset;
	m68k_pc_offset += 4;

	if (using_ce020) {
		printf ("\t%s %s = %s (%d);\n", type, name, ce020_prefetch_long, r);
		count_read += 2;
	} else if (using_ce) {
		printf ("\t%s %s;\n", type, name);
		/* we must do this because execution order of (something | something2) is not defined */
		if (flags & GF_NOREFILL) {
			printf ("\t%s = get_word_ce_prefetch (%d) << 16;\n", name, r + 2);
			count_read++;
			printf ("\t%s |= regs.irc;\n", name);
		} else {
			printf ("\t%s = get_word_ce_prefetch (%d) << 16;\n", name, r + 2);
			count_read++;
			printf ("\t%s |= get_word_ce_prefetch (%d);\n", name, r + 4);
			count_read++;
		}
	} else {
		if (using_prefetch) {
			if (flags & GF_NOREFILL) {
				printf ("\t%s %s;\n", type, name);
				printf ("\t%s = get_word_prefetch (%d) << 16;\n", name, r + 2);
				count_read++;
				printf ("\t%s |= regs.irc;\n", name);
				insn_n_cycles += 4;
			} else {
				printf ("\t%s %s = get_long_prefetch (%d);\n", type, name, r + 2);
				count_read++;
				count_read++;
				insn_n_cycles += 8;
			}
		} else if (using_indirect) {
			insn_n_cycles += 8;
			printf ("\t%s %s = get_ilongi (%d);\n", type, name, r);
		} else if (using_mmu) {
			insn_n_cycles += 8;
			printf ("\t%s %s = get_ilong_mmu%s (%d);\n", type, name,
				mmu_postfix, r);
		} else {
			insn_n_cycles += 8;
			printf ("\t%s %s = get_ilong (%d);\n", type, name, r);
		}
	}
}

static const char *gen_nextiword (int flags)
{
	static char buffer[80];
	int r = m68k_pc_offset;
	m68k_pc_offset += 2;

	if (using_ce020) {
		sprintf (buffer, "%s (%d)", ce020_prefetch_word, r);
		count_read++;
	} else if (using_ce) {
		if (flags & GF_NOREFILL) {
			strcpy (buffer, "regs.irc");
		} else {
			sprintf (buffer, "get_word_ce_prefetch (%d)", r + 2);
			count_read++;
		}
	} else {
		if (using_prefetch) {
			if (flags & GF_NOREFILL) {
				sprintf (buffer, "regs.irc"/*, r*/);
			} else {
				sprintf (buffer, "get_word_prefetch (%d)", r + 2);
				count_read++;
				insn_n_cycles += 4;
			}
		} else if (using_indirect) {
			sprintf (buffer, "get_iwordi(%d)", r);
			insn_n_cycles += 4;
		} else if (using_mmu) {
			sprintf (buffer, "get_iword_mmu%s (%d)",
				 mmu_postfix, r);
			insn_n_cycles += 4;
		} else {
			sprintf (buffer, "get_iword (%d)", r);
			insn_n_cycles += 4;
		}
	}
	return buffer;
}

static const char *gen_nextibyte (int flags)
{
	static char buffer[80];
	int r = m68k_pc_offset;
	m68k_pc_offset += 2;

	if (using_ce020) {
		sprintf (buffer, "(uae_u8)%s (%d)", ce020_prefetch_word, r);
		count_read++;
	} else if (using_ce) {
		if (flags & GF_NOREFILL) {
			strcpy (buffer, "(uae_u8)regs.irc");
		} else {
			sprintf (buffer, "(uae_u8)get_word_ce_prefetch (%d)", r + 2);
			count_read++;
		}
	} else {
		insn_n_cycles += 4;
		if (using_prefetch) {
			if (flags & GF_NOREFILL) {
				sprintf (buffer, "(uae_u8)regs.irc"/*, r*/);
			} else {
				sprintf (buffer, "(uae_u8)get_word_prefetch (%d)", r + 2);
				insn_n_cycles += 4;
				count_read++;
			}
		} else if (using_indirect)  {
			sprintf (buffer, "get_ibytei (%d)", r);
			insn_n_cycles += 4;
		} else if (using_mmu)  {
			sprintf (buffer, "get_ibyte_mmu%s (%d)",
				 mmu_postfix, r);
			insn_n_cycles += 4;
		} else {
			sprintf (buffer, "get_ibyte (%d)", r);
			insn_n_cycles += 4;
		}
	}
	return buffer;
}

static void irc2ir (void)
{
	if (!using_prefetch)
		return;
	printf ("\tregs.ir = regs.irc;\n");
	if (using_ce)
		printf ("\tipl_fetch ();\n");
}

static int did_prefetch;

static void fill_prefetch_2 (void)
{
	if (!using_prefetch)
		return;
	if (using_ce)
		printf ("\tget_word_ce_prefetch (%d);\n", m68k_pc_offset + 2);
	else
		printf ("\tget_word_prefetch (%d);\n", m68k_pc_offset + 2);
	did_prefetch = 1;
	count_read++;
	insn_n_cycles += 4;
}

static void fill_prefetch_1 (int o)
{
	if (!using_prefetch)
		return;
	if (using_ce) {
		printf ("\tget_word_ce_prefetch (%d);\n", o);
	} else {
		printf ("\tget_word_prefetch (%d);\n", o);
	}
	did_prefetch = 1;
	count_read++;
	insn_n_cycles += 4;
}

static void fill_prefetch_full (void)
{
	fill_prefetch_1 (0);
	irc2ir ();
	fill_prefetch_1 (2);
}

static void fill_prefetch_0 (void)
{
	if (!using_prefetch)
		return;
	if (using_ce)
		printf ("\tget_word_ce_prefetch (0);\n");
	else
		printf ("\tget_word_prefetch (0);\n");
	did_prefetch = 1;
	count_read++;
	insn_n_cycles += 4;
}

static void dummy_prefetch (void)
{
	int o = m68k_pc_offset + 2;
	if (!using_prefetch)
		return;
	if (using_ce) {
		printf ("\tget_wordi_ce (m68k_getpc () + %d);\n", o);
	} else {
		printf ("\tget_wordi (m68k_getpc () + %d);\n", o);
	}
	count_read++;
	insn_n_cycles += 4;
}

static void fill_prefetch_next_1 (void)
{
	irc2ir ();
	fill_prefetch_1 (m68k_pc_offset + 2);
}

static void fill_prefetch_next (void)
{
	fill_prefetch_next_1 ();
}

#if 0
static void fill_prefetch_next_delay (int extracycles)
{
	if (!using_prefetch)
		return;
	if (using_ce) {
		if (extracycles > 0) {
			printf("\t{\n");
			fill_prefetch_next ();
			printf("\tif (%d > 0) do_cycles(%d * CYCLE_UNIT / 2);\n",
				extracycles, extracycles);
			printf("\t}\n");
		} else {
			fill_prefetch_next ();
		}
	} else {
		fill_prefetch_next ();
	}
}
#endif

static void fill_prefetch_finish (void)
{
	if (did_prefetch || !using_prefetch)
		return;
	fill_prefetch_1 (m68k_pc_offset);
}

static void setpc (const char *format, ...)
{
	va_list parms;
	char buffer[1000];

	va_start (parms, format);
	_vsnprintf (buffer, 1000 - 1, format, parms);
	va_end (parms);

	if (using_mmu)
		printf ("\tm68k_setpc_mmu (%s);\n", buffer);
	else
		printf ("\tm68k_setpc (%s);\n", buffer);
}

static void incpc (const char *format, ...)
{
	va_list parms;
	char buffer[1000];

	va_start (parms, format);
	_vsnprintf (buffer, 1000 - 1, format, parms);
	va_end (parms);

	if (using_mmu)
		printf ("\tm68k_incpci (%s);\n", buffer);
	else
		printf ("\tm68k_incpc (%s);\n", buffer);
}

static void sync_m68k_pc (void)
{
	if (m68k_pc_offset == 0)
		return;
	incpc ("%d", m68k_pc_offset);
	m68k_pc_offset = 0;
}

static void gen_set_fault_pc (void)
{
	if (!using_mmu)
		return;
	sync_m68k_pc ();
	printf ("\tregs.fault_pc = m68k_getpci ();\n");
	m68k_pc_offset = 0;
}

static void syncmovepc (int getv, int flags)
{
#if 0
	if (!(flags & GF_MOVE))
		return;
	if (getv == 1) {
		sync_m68k_pc ();
		//fill_prefetch_next ();
	}
#endif
}

/* getv == 1: fetch data; getv != 0: check for odd address. If movem != 0,
* the calling routine handles Apdi and Aipi modes.
* gb-- movem == 2 means the same thing but for a MOVE16 instruction */

/* fixup indicates if we want to fix up address registers in pre decrement
* or post increment mode now (0) or later (1). A value of 2 will then be
* used to do the actual fix up. This allows to do all memory readings
* before any register is modified, and so to rerun operation without
* side effect in case a bus fault is generated by any memory access.
* XJ - 2006/11/13 */

static void genamode2 (amodes mode, const char *reg, wordsizes size, const char *name, int getv, int movem, int flags, int fixup)
{
	char namea[100];
	int m68k_pc_offset_last = m68k_pc_offset;

	sprintf (namea, "%sa", name);

	start_brace ();
	switch (mode) {
	case Dreg:
		if (movem)
			abort ();
		if (getv == 1)
			switch (size) {
			case sz_byte:
#ifdef USE_DUBIOUS_BIGENDIAN_OPTIMIZATION
				/* This causes the target compiler to generate better code on few systems */
				printf ("\tuae_s8 %s = ((uae_u8*)&m68k_dreg (regs, %s))[3];\n", name, reg);
#else
				printf ("\tuae_s8 %s = m68k_dreg (regs, %s);\n", name, reg);
#endif
				break;
			case sz_word:
#ifdef USE_DUBIOUS_BIGENDIAN_OPTIMIZATION
				printf ("\tuae_s16 %s = ((uae_s16*)&m68k_dreg (regs, %s))[1];\n", name, reg);
#else
				printf ("\tuae_s16 %s = m68k_dreg (regs, %s);\n", name, reg);
#endif
				break;
			case sz_long:
				printf ("\tuae_s32 %s = m68k_dreg (regs, %s);\n", name, reg);
				break;
			default:
				abort ();
		}
		syncmovepc (getv, flags);
		return;
	case Areg:
		if (movem)
			abort ();
		if (getv == 1)
			switch (size) {
			case sz_word:
				printf ("\tuae_s16 %s = m68k_areg (regs, %s);\n", name, reg);
				break;
			case sz_long:
				printf ("\tuae_s32 %s = m68k_areg (regs, %s);\n", name, reg);
				break;
			default:
				abort ();
		}
		syncmovepc (getv, flags);
		return;
	case Aind: // (An)
		printf ("\tuaecptr %sa = m68k_areg (regs, %s);\n", name, reg);
		break;
	case Aipi: // (An)+
		printf ("\tuaecptr %sa = m68k_areg (regs, %s);\n", name, reg);
		break;
	case Apdi: // -(An)
		printf ("\tuaecptr %sa;\n", name);
		switch (size) {
		case sz_byte:
			if (movem)
				printf ("\t%sa = m68k_areg (regs, %s);\n", name, reg);
			else
				printf ("\t%sa = m68k_areg (regs, %s) - areg_byteinc[%s];\n", name, reg, reg);
			break;
		case sz_word:
			printf ("\t%sa = m68k_areg (regs, %s) - %d;\n", name, reg, movem ? 0 : 2);
			break;
		case sz_long:
			printf ("\t%sa = m68k_areg (regs, %s) - %d;\n", name, reg, movem ? 0 : 4);
			break;
		default:
			abort ();
		}
		if (!(flags & GF_APDI)) {
			addcycles000 (2);
			insn_n_cycles += 2;
			count_cycles_ea += 2;
		}
		break;
	case Ad16: // (d16,An)
		printf ("\tuaecptr %sa = m68k_areg (regs, %s) + (uae_s32)(uae_s16)%s;\n", name, reg, gen_nextiword (flags));
		count_read_ea++;
		break;
	case Ad8r: // (d8,An,Xn)
		printf ("\tuaecptr %sa;\n", name);
		if (cpu_level > 1) {
			if (next_cpu_level < 1)
				next_cpu_level = 1;
			sync_m68k_pc ();
			start_brace ();
			/* This would ordinarily be done in gen_nextiword, which we bypass.  */
			insn_n_cycles += 4;
			if (using_ce020 || using_mmu)
				printf ("\t%sa = x_get_disp_ea_020 (m68k_areg (regs, %s), x_next_iword ());\n", name, reg);
			else
				printf ("\t%sa = get_disp_ea_020 (m68k_areg (regs, %s), next_iword ());\n", name, reg);
		} else {
			if (!(flags & GF_AD8R)) {
				addcycles000 (2);
				insn_n_cycles += 2;
				count_cycles_ea += 2;
			}
			printf ("\t%sa = get_disp_ea_000 (m68k_areg (regs, %s), %s);\n", name, reg, gen_nextiword (flags));
			count_read_ea++;
		}
		break;
	case PC16: // (d16,PC,Xn)
		printf ("\tuaecptr %sa = m68k_getpc () + %d;\n", name, m68k_pc_offset);
		printf ("\t%sa += (uae_s32)(uae_s16)%s;\n", name, gen_nextiword (flags));
		break;
	case PC8r: // (d8,PC,Xn)
		printf ("\tuaecptr tmppc;\n");
		printf ("\tuaecptr %sa;\n", name);
		if (cpu_level > 1) {
			if (next_cpu_level < 1)
				next_cpu_level = 1;
			sync_m68k_pc ();
			start_brace ();
			/* This would ordinarily be done in gen_nextiword, which we bypass.  */
			insn_n_cycles += 4;
			printf ("\ttmppc = m68k_getpc ();\n");
			if (using_ce020 || using_mmu)
				printf ("\t%sa = x_get_disp_ea_020 (tmppc, x_next_iword ());\n", name);
			else
				printf ("\t%sa = get_disp_ea_020 (tmppc, next_iword ());\n", name);
		} else {
			printf ("\ttmppc = m68k_getpc () + %d;\n", m68k_pc_offset);
			if (!(flags & GF_PC8R)) {
				addcycles000 (2);
				insn_n_cycles += 2;
				count_cycles_ea += 2;
			}
			printf ("\t%sa = get_disp_ea_000 (tmppc, %s);\n", name, gen_nextiword (flags));
		}

		break;
	case absw:
		printf ("\tuaecptr %sa = (uae_s32)(uae_s16)%s;\n", name, gen_nextiword (flags));
		break;
	case absl:
		gen_nextilong ("uaecptr", namea, flags);
		count_read_ea += 2;
		break;
	case imm:
		if (getv != 1)
			abort ();
		insn_n_cycles020++;
		switch (size) {
		case sz_byte:
			printf ("\tuae_s8 %s = %s;\n", name, gen_nextibyte (flags));
			count_read_ea++;
			break;
		case sz_word:
			printf ("\tuae_s16 %s = %s;\n", name, gen_nextiword (flags));
			count_read_ea++;
			break;
		case sz_long:
			gen_nextilong ("uae_s32", name, flags);
			count_read_ea += 2;
			break;
		default:
			abort ();
		}
		syncmovepc (getv, flags);
		return;
	case imm0:
		if (getv != 1)
			abort ();
		printf ("\tuae_s8 %s = %s;\n", name, gen_nextibyte (flags));
		count_read_ea++;
		syncmovepc (getv, flags);
		return;
	case imm1:
		if (getv != 1)
			abort ();
		printf ("\tuae_s16 %s = %s;\n", name, gen_nextiword (flags));
		count_read_ea++;
		syncmovepc (getv, flags);
		return;
	case imm2:
		if (getv != 1)
			abort ();
		gen_nextilong ("uae_s32", name, flags);
		count_read_ea += 2;
		syncmovepc (getv, flags);
		return;
	case immi:
		if (getv != 1)
			abort ();
		printf ("\tuae_u32 %s = %s;\n", name, reg);
		syncmovepc (getv, flags);
		return;
	default:
		abort ();
	}

	syncmovepc (getv, flags);

	/* We get here for all non-reg non-immediate addressing modes to
	* actually fetch the value. */

	if ((using_prefetch || using_ce) && using_exception_3 && getv != 0 && size != sz_byte) {
		int offset = 0;
		if (flags & GF_MOVE) {
			offset = m68k_pc_offset;
			if (getv == 2)
				offset += 2;
		} else {
			offset = m68k_pc_offset_last;
		}
		printf ("\tif (%sa & 1) {\n", name);
		printf ("\t\texception3 (opcode, m68k_getpc () + %d, %sa);\n",
			offset, name);
		printf ("\t\tgoto %s;\n", endlabelstr);
		printf ("\t}\n");
		need_endlabel = 1;
		start_brace ();
	}

	if (flags & GF_PREFETCH)
		fill_prefetch_next ();

	if (getv == 1) {
		start_brace ();
		if (using_ce020) {
			switch (size) {
			case sz_byte: insn_n_cycles += 4; printf ("\tuae_s8 %s = %s (%sa);\n", name, srcb, name); count_read++; break;
			case sz_word: insn_n_cycles += 4; printf ("\tuae_s16 %s = %s (%sa);\n", name, srcw, name); count_read++; break;
			case sz_long: insn_n_cycles += 8; printf ("\tuae_s32 %s = %s (%sa);\n", name, srcl, name); count_read += 2; break;
			default: abort ();
			}
		} else if (using_ce) {
			switch (size) {
			case sz_byte: printf ("\tuae_s8 %s = get_byte_ce (%sa);\n", name, name); count_read++; break;
			case sz_word: printf ("\tuae_s16 %s = get_word_ce (%sa);\n", name, name); count_read++; break;
			case sz_long: printf ("\tuae_s32 %s = get_word_ce (%sa) << 16; %s |= get_word_ce (%sa + 2);\n", name, name, name, name); count_read += 2; break;
			default: abort ();
			}
		} else if (using_mmu) {
			if (flags & GF_FC) {
				switch (size) {
				case sz_byte: insn_n_cycles += 4; printf ("\tuae_s8 %s = sfc%s_get_byte (%sa);\n", name, mmu_postfix, name); break;
				case sz_word: insn_n_cycles += 4; printf ("\tuae_s16 %s = sfc%s_get_word (%sa);\n", name, mmu_postfix, name); break;
				case sz_long: insn_n_cycles += 8; printf ("\tuae_s32 %s = sfc%s_get_long (%sa);\n", name, mmu_postfix, name); break;
				default: abort ();
				}
			} else {
				switch (size) {
				case sz_byte: insn_n_cycles += 4; printf ("\tuae_s8 %s = get_byte_mmu%s (%sa);\n", name, mmu_postfix, name); break;
				case sz_word: insn_n_cycles += 4; printf ("\tuae_s16 %s = get_word_mmu%s (%sa);\n", name, mmu_postfix, name); break;
				case sz_long: insn_n_cycles += 8; printf ("\tuae_s32 %s = get_long_mmu%s (%sa);\n", name, mmu_postfix, name); break;
				default: abort ();
				}
			}
		} else {
			switch (size) {
			case sz_byte: insn_n_cycles += 4; printf ("\tuae_s8 %s = get_byte (%sa);\n", name, name); count_read++; break;
			case sz_word: insn_n_cycles += 4; printf ("\tuae_s16 %s = get_word (%sa);\n", name, name); count_read++; break;
			case sz_long: insn_n_cycles += 8; printf ("\tuae_s32 %s = get_long (%sa);\n", name, name); count_read += 2; break;
			default: abort ();
			}
		}
	}

	/* We now might have to fix up the register for pre-dec or post-inc
	* addressing modes. */
	if (!movem)
		switch (mode) {
		case Aipi:
			if (fixup == 1) {
				printf ("\tmmufixup[%d].reg = %s;\n", fixupcnt, reg);
				printf ("\tmmufixup[%d].value = m68k_areg (regs, %s);\n", fixupcnt, reg);
			}
			switch (size) {
			case sz_byte:
				printf ("\tm68k_areg (regs, %s) += areg_byteinc[%s];\n", reg, reg);
				break;
			case sz_word:
				printf ("\tm68k_areg (regs, %s) += 2;\n", reg);
				break;
			case sz_long:
				printf ("\tm68k_areg (regs, %s) += 4;\n", reg);
				break;
			default:
				abort ();
			}
			break;
		case Apdi:
			if (fixup == 1) {
				printf ("\tmmufixup[%d].reg = %s;\n", fixupcnt, reg);
				printf ("\tmmufixup[%d].value = m68k_areg (regs, %s);\n", fixupcnt, reg);
			}
			printf ("\tm68k_areg (regs, %s) = %sa;\n", reg, name);
			break;
		default:
			break;
	}
}

static void genamode_fixup (amodes mode, const char *reg, wordsizes size, const char *name, int getv, int movem, int flags, int fixup)
{
	if (fixup != 2) {
		genamode2 (mode, reg, size, name, getv, movem, flags, fixup);
	} else {
		if (!movem) {
			switch ((int)mode)
			{
			case Aipi:
			case Apdi:
				printf ("\tmmufixup[0].reg = -1;\n"/*, fixupcnt*/);
				break;
			}
		}
	}
}

static void genamode (amodes mode, const char *reg, wordsizes size, const char *name, int getv, int movem, int flags)
{
	genamode2 (mode, reg, size, name, getv, movem, flags, 0);
}
static void genamode_pre (amodes mode, const char *reg, wordsizes size, const char *name, int getv, int movem, int flags)
{
	genamode_fixup (mode, reg, size, name, getv, movem, flags, using_mmu ? 1 : 0);
}
static void genamode_post (amodes mode, const char *reg, wordsizes size, const char *name, int getv, int movem, int flags)
{
	if (using_mmu)
		genamode_fixup (mode, reg, size, name, getv, movem, flags, 2);
}

static void genastore_2 (const char *from, amodes mode, const char *reg, wordsizes size, const char *to, int store_dir, int flags)
{
	switch (mode) {
	case Dreg:
		switch (size) {
		case sz_byte:
			printf ("\tm68k_dreg (regs, %s) = (m68k_dreg (regs, %s) & ~0xff) | ((%s) & 0xff);\n", reg, reg, from);
			break;
		case sz_word:
			printf ("\tm68k_dreg (regs, %s) = (m68k_dreg (regs, %s) & ~0xffff) | ((%s) & 0xffff);\n", reg, reg, from);
			break;
		case sz_long:
			printf ("\tm68k_dreg (regs, %s) = (%s);\n", reg, from);
			break;
		default:
			abort ();
		}
		break;
	case Areg:
		switch (size) {
		case sz_word:
			printf ("\tm68k_areg (regs, %s) = (uae_s32)(uae_s16)(%s);\n", reg, from);
			break;
		case sz_long:
			printf ("\tm68k_areg (regs, %s) = (%s);\n", reg, from);
			break;
		default:
			abort ();
		}
		break;
	case Aind:
	case Aipi:
	case Apdi:
	case Ad16:
	case Ad8r:
	case absw:
	case absl:
	case PC16:
	case PC8r:
		gen_set_fault_pc ();
		if (using_ce020) {
			switch (size) {
			case sz_byte:
				printf ("\t%s (%sa, %s);\n", dstb, to, from);
				count_write++;
				break;
			case sz_word:
				if (cpu_level < 2 && (mode == PC16 || mode == PC8r))
					abort ();
				printf ("\t%s (%sa, %s);\n", dstw, to, from);
				count_write++;
				break;
			case sz_long:
				if (cpu_level < 2 && (mode == PC16 || mode == PC8r))
					abort ();
				printf ("\t%s (%sa, %s);\n", dstl, to, from);
				count_write += 2;
				break;
			default:
				abort ();
			}
		} else if (using_ce) {
			switch (size) {
			case sz_byte:
				printf ("\tput_byte_ce (%sa, %s);\n", to, from);
				count_write++;
				break;
			case sz_word:
				if (cpu_level < 2 && (mode == PC16 || mode == PC8r))
					abort ();
				printf ("\tput_word_ce (%sa, %s);\n", to, from);
				count_write++;
				break;
			case sz_long:
				if (cpu_level < 2 && (mode == PC16 || mode == PC8r))
					abort ();
				if (store_dir)
					printf ("\tput_word_ce (%sa + 2, %s); put_word_ce (%sa, %s >> 16);\n", to, from, to, from);
				else
					printf ("\tput_word_ce (%sa, %s >> 16); put_word_ce (%sa + 2, %s);\n", to, from, to, from);
				count_write += 2;
				break;
			default:
				abort ();
			}
		} else if (using_mmu) {
			switch (size) {
			case sz_byte:
				insn_n_cycles += 4;
				if (flags & GF_FC)
					printf ("\tdfc%s_put_byte (%sa, %s);\n", mmu_postfix, to, from);
				else
					printf ("\tput_byte_mmu%s (%sa, %s);\n", mmu_postfix, to, from);
				break;
			case sz_word:
				insn_n_cycles += 4;
				if (cpu_level < 2 && (mode == PC16 || mode == PC8r))
					abort ();
				if (flags & GF_FC)
					printf ("\tdfc%s_put_word (%sa, %s);\n", mmu_postfix, to, from);
				else
					printf ("\tput_word_mmu%s (%sa, %s);\n", mmu_postfix, to, from);
				break;
			case sz_long:
				insn_n_cycles += 8;
				if (cpu_level < 2 && (mode == PC16 || mode == PC8r))
					abort ();
				if (flags & GF_FC)
					printf ("\tdfc%s_put_long (%sa, %s);\n", mmu_postfix, to, from);
				else
					printf ("\tput_long_mmu%s (%sa, %s);\n", mmu_postfix, to, from);
				break;
			default:
				abort ();
			}
		} else {
			switch (size) {
			case sz_byte:
				insn_n_cycles += 4;
				printf ("\tput_byte (%sa, %s);\n", to, from);
				count_write++;
				break;
			case sz_word:
				insn_n_cycles += 4;
				if (cpu_level < 2 && (mode == PC16 || mode == PC8r))
					abort ();
				printf ("\tput_word (%sa, %s);\n", to, from);
				count_write++;
				break;
			case sz_long:
				insn_n_cycles += 8;
				if (cpu_level < 2 && (mode == PC16 || mode == PC8r))
					abort ();
				printf ("\tput_long (%sa, %s);\n", to, from);
				count_write += 2;
				break;
			default:
				abort ();
			}
		}
		break;
	case imm:
	case imm0:
	case imm1:
	case imm2:
	case immi:
		abort ();
		break;
	default:
		abort ();
	}
}

static void genastore (const char *from, amodes mode, const char *reg, wordsizes size, const char *to)
{
	genastore_2 (from, mode, reg, size, to, 0, 0);
}
static void genastore_rev (const char *from, amodes mode, const char *reg, wordsizes size, const char *to)
{
	genastore_2 (from, mode, reg, size, to, 1, 0);
}
static void genastore_fc (const char *from, amodes mode, const char *reg, wordsizes size, const char *to)
{
	genastore_2 (from, mode, reg, size, to, 1, GF_FC);
}

static void genmovemel (uae_u16 opcode)
{
	char getcode[100];
	int size = table68k[opcode].size == sz_long ? 4 : 2;

	if (table68k[opcode].size == sz_long) {
		sprintf (getcode, "%s (srca)", srcl);
	} else {
		sprintf (getcode, "(uae_s32)(uae_s16)%s (srca)", srcw);
	}
	count_read += table68k[opcode].size == sz_long ? 2 : 1;
	printf ("\tuae_u16 mask = %s;\n", gen_nextiword (0));
	printf ("\tunsigned int dmask = mask & 0xff, amask = (mask >> 8) & 0xff;\n");
	printf ("\tuae_u16 regs_number = 0;\n");
	genamode (table68k[opcode].dmode, "dstreg", table68k[opcode].size, "src", 2, 1, 0);
	start_brace ();
	printf ("\twhile (dmask) { m68k_dreg (regs, movem_index1[dmask]) = %s; srca += %d; dmask = movem_next[dmask]; regs_number++; }\n",
		getcode, size);
	printf ("\twhile (amask) { m68k_areg (regs, movem_index1[amask]) = %s; srca += %d; amask = movem_next[amask]; regs_number++; }\n",
		getcode, size);

	if (table68k[opcode].dmode == Aipi) {
		printf ("\tm68k_areg (regs, dstreg) = srca;\n");
		count_read++;
	}
	count_ncycles++;

	if (using_ce020 == 2) {
		if (table68k[opcode].size == sz_long)
			printf ("\tregs.ce030_instr_addcycles = regs_number * 8;\n");
		else
			printf ("\tregs.ce030_instr_addcycles = regs_number * 4;\n");
	}

	fill_prefetch_next ();
}

static void genmovemel_ce (uae_u16 opcode)
{
	int size = table68k[opcode].size == sz_long ? 4 : 2;
	printf ("\tuae_u16 mask = %s;\n", gen_nextiword (0));
	printf ("\tunsigned int dmask = mask & 0xff, amask = (mask >> 8) & 0xff;\n");
	printf ("\tuae_u32 v;\n");
	genamode (table68k[opcode].dmode, "dstreg", table68k[opcode].size, "src", 2, 1, GF_AA);
	if (table68k[opcode].dmode == Ad8r || table68k[opcode].dmode == PC8r)
		addcycles000 (2);
	start_brace ();
	if (table68k[opcode].size == sz_long) {
		printf ("\twhile (dmask) { v = get_word_ce(srca) << 16; v |= get_word_ce(srca + 2); m68k_dreg (regs, movem_index1[dmask]) = v; srca += %d; dmask = movem_next[dmask]; }\n",
			size);
		printf ("\twhile (amask) { v = get_word_ce(srca) << 16; v |= get_word_ce(srca + 2); m68k_areg (regs, movem_index1[amask]) = v; srca += %d; amask = movem_next[amask]; }\n",
			size);
	} else {
		printf ("\twhile (dmask) { m68k_dreg (regs, movem_index1[dmask]) = (uae_s32)(uae_s16)get_word_ce(srca); srca += %d; dmask = movem_next[dmask]; }\n",
			size);
		printf ("\twhile (amask) { m68k_areg (regs, movem_index1[amask]) = (uae_s32)(uae_s16)get_word_ce(srca); srca += %d; amask = movem_next[amask]; }\n",
			size);
	}
	printf ("\tget_word_ce (srca);\n"); // and final extra word fetch that goes nowhere..
	count_read++;
	if (table68k[opcode].dmode == Aipi)
		printf ("\tm68k_areg (regs, dstreg) = srca;\n");

	count_ncycles++;
	fill_prefetch_next ();
}

static void genmovemle (uae_u16 opcode)
{
	char putcode[100];
	int size = table68k[opcode].size == sz_long ? 4 : 2;

	if (table68k[opcode].size == sz_long) {
		sprintf (putcode, "%s (srca", dstl);
	} else {
		sprintf (putcode, "%s (srca", dstw);
	}
	count_write += table68k[opcode].size == sz_long ? 2 : 1;

	printf ("\tuae_u16 mask = %s;\n", gen_nextiword (0));
	genamode (table68k[opcode].dmode, "dstreg", table68k[opcode].size, "src", 2, 1, 0);
	start_brace ();
	printf ("\tuae_u16 regs_number = 0;\n");
	if (table68k[opcode].dmode == Apdi) {
		printf ("\tuae_u16 amask = mask & 0xff, dmask = (mask >> 8) & 0xff;\n");
		if (!using_mmu)
			printf ("\tint type = get_cpu_model() >= 68020;\n");
		printf ("\twhile (amask) {\n");
		printf ("\t\tsrca -= %d;\n", size);
		if (!using_mmu)
			printf ("\t\tif (type) m68k_areg (regs, dstreg) = srca;\n");
		printf ("\t\t%s, m68k_areg (regs, movem_index2[amask]));\n", putcode);
		printf ("\t\tamask = movem_next[amask];\n");
		printf ("\t\tregs_number++;\n");
		printf ("\t}\n");
		printf ("\twhile (dmask) { srca -= %d; %s, m68k_dreg (regs, movem_index2[dmask])); dmask = movem_next[dmask]; regs_number++; }\n",
			size, putcode);
		printf ("\tm68k_areg (regs, dstreg) = srca;\n");
	} else {
		printf ("\tuae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;\n");
		printf ("\twhile (dmask) { %s, m68k_dreg (regs, movem_index1[dmask])); srca += %d; dmask = movem_next[dmask]; regs_number++; }\n",
			putcode, size);
		printf ("\twhile (amask) { %s, m68k_areg (regs, movem_index1[amask])); srca += %d; amask = movem_next[amask]; regs_number++; }\n",
			putcode, size);
	}

	count_ncycles++;
	if (using_ce020 == 2) {
		if (table68k[opcode].size == sz_long)
			printf ("\tregs.ce030_instr_addcycles = regs_number * 8;\n");
		else
			printf ("\tregs.ce030_instr_addcycles = regs_number * 4;\n");
	}
	fill_prefetch_next ();
}

static void genmovemle_ce (uae_u16 opcode)
{
	int size = table68k[opcode].size == sz_long ? 4 : 2;
	printf ("\tuae_u16 mask = %s;\n", gen_nextiword (0));
	genamode (table68k[opcode].dmode, "dstreg", table68k[opcode].size, "src", 2, 1, GF_AA);

	if (table68k[opcode].dmode == Ad8r || table68k[opcode].dmode == PC8r) {
		addcycles000 (2);
	}
	start_brace ();
	if (table68k[opcode].size == sz_long) {
		if (table68k[opcode].dmode == Apdi) {
			printf ("\tuae_u16 amask = mask & 0xff, dmask = (mask >> 8) & 0xff;\n");
			printf ("\twhile (amask) { srca -= %d; put_word_ce (srca, m68k_areg (regs, movem_index2[amask]) >> 16); put_word_ce (srca + 2, m68k_areg (regs, movem_index2[amask])); amask = movem_next[amask]; }\n",
				size);
			printf ("\twhile (dmask) { srca -= %d; put_word_ce (srca, m68k_dreg (regs, movem_index2[dmask]) >> 16); put_word_ce (srca + 2, m68k_dreg (regs, movem_index2[dmask])); dmask = movem_next[dmask];}\n",
				size);
			printf ("\tm68k_areg (regs, dstreg) = srca;\n");
		} else {
			printf ("\tuae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;\n");
			printf ("\twhile (dmask) { put_word_ce (srca, m68k_dreg (regs, movem_index1[dmask]) >> 16); put_word_ce (srca + 2, m68k_dreg (regs, movem_index1[dmask])); srca += %d; dmask = movem_next[dmask]; }\n",
				size);
			printf ("\twhile (amask) { put_word_ce (srca, m68k_areg (regs, movem_index1[amask]) >> 16); put_word_ce (srca + 2, m68k_areg (regs, movem_index1[amask])); srca += %d; amask = movem_next[amask];}\n",
				size);
		}
	} else {
		if (table68k[opcode].dmode == Apdi) {
			printf ("\tuae_u16 amask = mask & 0xff, dmask = (mask >> 8) & 0xff;\n");
			printf ("\twhile (amask) { srca -= %d; put_word_ce (srca, m68k_areg (regs, movem_index2[amask])); amask = movem_next[amask]; }\n",
				size);
			printf ("\twhile (dmask) { srca -= %d; put_word_ce (srca, m68k_dreg (regs, movem_index2[dmask])); dmask = movem_next[dmask]; }\n",
				size);
			printf ("\tm68k_areg (regs, dstreg) = srca;\n");
		} else {
			printf ("\tuae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;\n");
			printf ("\twhile (dmask) { put_word_ce (srca, m68k_dreg (regs, movem_index1[dmask])); srca += %d; dmask = movem_next[dmask]; }\n",
				size);
			printf ("\twhile (amask) { put_word_ce (srca, m68k_areg (regs, movem_index1[amask])); srca += %d; amask = movem_next[amask]; }\n",
				size);
		}
	}

	count_ncycles++;
	fill_prefetch_next ();
}

static void duplicate_carry (int n)
{
	int i;
	for (i = 0; i <= n; i++)
		printf ("\t");
	printf ("COPY_CARRY ();\n");
}

typedef enum
{
	flag_logical_noclobber, flag_logical, flag_add, flag_sub, flag_cmp, flag_addx, flag_subx, flag_z, flag_zn,
	flag_av, flag_sv
}
flagtypes;

static void genflags_normal (flagtypes type, wordsizes size, const char *value, const char *src, const char *dst)
{
	char vstr[100], sstr[100], dstr[100];
	char usstr[100], udstr[100];
	char unsstr[100], undstr[100];

	switch (size) {
	case sz_byte:
		strcpy (vstr, "((uae_s8)(");
		strcpy (usstr, "((uae_u8)(");
		break;
	case sz_word:
		strcpy (vstr, "((uae_s16)(");
		strcpy (usstr, "((uae_u16)(");
		break;
	case sz_long:
		strcpy (vstr, "((uae_s32)(");
		strcpy (usstr, "((uae_u32)(");
		break;
	default:
		abort ();
	}
	strcpy (unsstr, usstr);

	strcpy (sstr, vstr);
	strcpy (dstr, vstr);
	strcat (vstr, value);
	strcat (vstr, "))");
	strcat (dstr, dst);
	strcat (dstr, "))");
	strcat (sstr, src);
	strcat (sstr, "))");

	strcpy (udstr, usstr);
	strcat (udstr, dst);
	strcat (udstr, "))");
	strcat (usstr, src);
	strcat (usstr, "))");

	strcpy (undstr, unsstr);
	strcat (unsstr, "-");
	strcat (undstr, "~");
	strcat (undstr, dst);
	strcat (undstr, "))");
	strcat (unsstr, src);
	strcat (unsstr, "))");

	switch (type) {
	case flag_logical_noclobber:
	case flag_logical:
	case flag_z:
	case flag_zn:
	case flag_av:
	case flag_sv:
	case flag_addx:
	case flag_subx:
		break;

	case flag_add:
		start_brace ();
		printf ("uae_u32 %s = %s + %s;\n", value, dstr, sstr);
		break;
	case flag_sub:
	case flag_cmp:
		start_brace ();
		printf ("uae_u32 %s = %s - %s;\n", value, dstr, sstr);
		break;
	}

	switch ((int)type) {
	case flag_logical_noclobber:
	case flag_logical:
	case flag_zn:
		break;

	case flag_add:
	case flag_sub:
	case flag_addx:
	case flag_subx:
	case flag_cmp:
	case flag_av:
	case flag_sv:
		start_brace ();
		printf ("\t" BOOL_TYPE " flgs = %s < 0;\n", sstr);
		printf ("\t" BOOL_TYPE " flgo = %s < 0;\n", dstr);
		printf ("\t" BOOL_TYPE " flgn = %s < 0;\n", vstr);
		break;
	}

	switch (type) {
	case flag_logical:
		printf ("\tCLEAR_CZNV ();\n");
		printf ("\tSET_ZFLG   (%s == 0);\n", vstr);
		printf ("\tSET_NFLG   (%s < 0);\n", vstr);
		break;
	case flag_logical_noclobber:
		printf ("\tSET_ZFLG (%s == 0);\n", vstr);
		printf ("\tSET_NFLG (%s < 0);\n", vstr);
		break;
	case flag_av:
		printf ("\tSET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));\n");
		break;
	case flag_sv:
		printf ("\tSET_VFLG ((flgs ^ flgo) & (flgn ^ flgo));\n");
		break;
	case flag_z:
		printf ("\tSET_ZFLG (GET_ZFLG () & (%s == 0));\n", vstr);
		break;
	case flag_zn:
		printf ("\tSET_ZFLG (GET_ZFLG () & (%s == 0));\n", vstr);
		printf ("\tSET_NFLG (%s < 0);\n", vstr);
		break;
	case flag_add:
		printf ("\tSET_ZFLG (%s == 0);\n", vstr);
		printf ("\tSET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));\n");
		printf ("\tSET_CFLG (%s < %s);\n", undstr, usstr);
		duplicate_carry (0);
		printf ("\tSET_NFLG (flgn != 0);\n");
		break;
	case flag_sub:
		printf ("\tSET_ZFLG (%s == 0);\n", vstr);
		printf ("\tSET_VFLG ((flgs ^ flgo) & (flgn ^ flgo));\n");
		printf ("\tSET_CFLG (%s > %s);\n", usstr, udstr);
		duplicate_carry (0);
		printf ("\tSET_NFLG (flgn != 0);\n");
		break;
	case flag_addx:
		printf ("\tSET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));\n"); /* minterm SON: 0x42 */
		printf ("\tSET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));\n"); /* minterm SON: 0xD4 */
		duplicate_carry (0);
		break;
	case flag_subx:
		printf ("\tSET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));\n"); /* minterm SON: 0x24 */
		printf ("\tSET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));\n"); /* minterm SON: 0xB2 */
		duplicate_carry (0);
		break;
	case flag_cmp:
		printf ("\tSET_ZFLG (%s == 0);\n", vstr);
		printf ("\tSET_VFLG ((flgs != flgo) && (flgn != flgo));\n");
		printf ("\tSET_CFLG (%s > %s);\n", usstr, udstr);
		printf ("\tSET_NFLG (flgn != 0);\n");
		break;
	}
}

static void genflags (flagtypes type, wordsizes size, const char *value, const char *src, const char *dst)
{
	/* Temporarily deleted 68k/ARM flag optimizations.  I'd prefer to have
	them in the appropriate m68k.h files and use just one copy of this
	code here.  The API can be changed if necessary.  */
	if (optimized_flags) {
		switch (type) {
		case flag_add:
		case flag_sub:
			start_brace ();
			printf ("\tuae_u32 %s;\n", value);
			break;

		default:
			break;
		}

		/* At least some of those casts are fairly important! */
		switch (type) {
		case flag_logical_noclobber:
			printf ("\t{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);\n");
			if (strcmp (value, "0") == 0) {
				printf ("\tSET_CZNV (olcznv | FLAGVAL_Z);\n");
			} else {
				switch (size) {
				case sz_byte: printf ("\toptflag_testb (regs, (uae_s8)(%s));\n", value); break;
				case sz_word: printf ("\toptflag_testw (regs, (uae_s16)(%s));\n", value); break;
				case sz_long: printf ("\toptflag_testl (regs, (uae_s32)(%s));\n", value); break;
				}
				printf ("\tIOR_CZNV (oldcznv);\n");
			}
			printf ("\t}\n");
			return;
		case flag_logical:
			if (strcmp (value, "0") == 0) {
				printf ("\tSET_CZNV (FLAGVAL_Z);\n");
			} else {
				switch (size) {
				case sz_byte: printf ("\toptflag_testb (regs, (uae_s8)(%s));\n", value); break;
				case sz_word: printf ("\toptflag_testw (regs, (uae_s16)(%s));\n", value); break;
				case sz_long: printf ("\toptflag_testl (regs, (uae_s32)(%s));\n", value); break;
				}
			}
			return;

		case flag_add:
			switch (size) {
			case sz_byte: printf ("\toptflag_addb (regs, %s, (uae_s8)(%s), (uae_s8)(%s));\n", value, src, dst); break;
			case sz_word: printf ("\toptflag_addw (regs, %s, (uae_s16)(%s), (uae_s16)(%s));\n", value, src, dst); break;
			case sz_long: printf ("\toptflag_addl (regs, %s, (uae_s32)(%s), (uae_s32)(%s));\n", value, src, dst); break;
			}
			return;

		case flag_sub:
			switch (size) {
			case sz_byte: printf ("\toptflag_subb (regs, %s, (uae_s8)(%s), (uae_s8)(%s));\n", value, src, dst); break;
			case sz_word: printf ("\toptflag_subw (regs, %s, (uae_s16)(%s), (uae_s16)(%s));\n", value, src, dst); break;
			case sz_long: printf ("\toptflag_subl (regs, %s, (uae_s32)(%s), (uae_s32)(%s));\n", value, src, dst); break;
			}
			return;

		case flag_cmp:
			switch (size) {
			case sz_byte: printf ("\toptflag_cmpb (regs, (uae_s8)(%s), (uae_s8)(%s));\n", src, dst); break;
			case sz_word: printf ("\toptflag_cmpw (regs, (uae_s16)(%s), (uae_s16)(%s));\n", src, dst); break;
			case sz_long: printf ("\toptflag_cmpl (regs, (uae_s32)(%s), (uae_s32)(%s));\n", src, dst); break;
			}
			return;

		default:
			break;
		}
	}

	genflags_normal (type, size, value, src, dst);
}

static void force_range_for_rox (const char *var, wordsizes size)
{
	/* Could do a modulo operation here... which one is faster? */
	switch (size) {
	case sz_long:
		printf ("\tif (%s >= 33) %s -= 33;\n", var, var);
		break;
	case sz_word:
		printf ("\tif (%s >= 34) %s -= 34;\n", var, var);
		printf ("\tif (%s >= 17) %s -= 17;\n", var, var);
		break;
	case sz_byte:
		printf ("\tif (%s >= 36) %s -= 36;\n", var, var);
		printf ("\tif (%s >= 18) %s -= 18;\n", var, var);
		printf ("\tif (%s >= 9) %s -= 9;\n", var, var);
		break;
	}
}

static const char *cmask (wordsizes size)
{
	switch (size) {
	case sz_byte: return "0x80";
	case sz_word: return "0x8000";
	case sz_long: return "0x80000000";
	default: abort ();
	}
}

static int source_is_imm1_8 (struct instr *i)
{
	return i->stype == 3;
}

static void shift_ce (amodes dmode, int size)
{
	if (using_ce && isreg (dmode)) {
		int c = size == sz_long ? 4 : 2;
		printf ("\t{\n");
		printf ("\t\tint cycles = %d;\n", c);
		printf ("\t\tcycles += 2 * ccnt;\n");
		addcycles000_3 ("\t\t");
		printf ("\t}\n");
		count_cycles += c;
	}
}

// BCHG/BSET/BCLR Dx,Dx or #xx,Dx adds 2 cycles if bit number > 15
static void bsetcycles (struct instr *curi)
{
	if (curi->size == sz_byte) {
		printf ("\tsrc &= 7;\n");
	} else {
		printf ("\tsrc &= 31;\n");
		if (isreg (curi->dmode)) {
			addcycles000 (2);
			if (curi->mnemo != i_BTST && using_ce) {
				printf ("\tif (src > 15) do_cycles_ce000 (2);\n");
				count_ncycles++;
			}
		}
	}
}

static int islongimm (struct instr *curi)
{
	return (curi->size == sz_long && (curi->smode == Dreg || curi->smode == imm));
}

static void gen_opcode (unsigned long int opcode)
{
	struct instr *curi = table68k + opcode;

	insn_n_cycles = using_prefetch ? 0 : 4;

	/* Store the family of the instruction (used to check for pairing on ST,
	 * for non-CPU cycles calculation and profiling)
	 */
	printf ("\tOpcodeFamily = %d;\n", curi->mnemo);
	/* leave some space for patching in the current cycles later */
	if (!using_ce020) {
		printf("\tCurrentInstrCycles =     \n");
		nCurInstrCycPos = ftell(stdout) - 5;
	}
	else if (using_ce020 == 2) {
		/* Initialize the cycle table to the general table by default */
		instr_table = TABLE_FALCON_CYCLES;
		instr_table_index = instr_index;
		no_return_cycles = false;
	}

	if (using_ce020) {
		if (using_ce020 == 2) {
			ce020_prefetch_long = "get_long_ce030_prefetch";
			ce020_prefetch_word = "get_word_ce030_prefetch";
			srcl = "get_long_ce030";
			dstl = "put_long_ce030";
			srcw = "get_word_ce030";
			dstw = "put_word_ce030";
			srcb = "get_byte_ce030";
			dstb = "put_byte_ce030";
		} else {
			ce020_prefetch_long = "get_long_ce020_prefetch";
			ce020_prefetch_word = "get_word_ce020_prefetch";
			srcl = "get_long_ce020";
			dstl = "put_long_ce020";
			srcw = "get_word_ce020";
			dstw = "put_word_ce020";
			srcb = "get_byte_ce020";
			dstb = "put_byte_ce020";
		}
	} else if (using_mmu && cpu_level == 3) {
		srcl = "get_long_mmu030";
		dstl = "put_long_mmu030";
		srcw = "get_word_mmu030";
		dstw = "put_word_mmu030";
		srcb = "get_byte_mmu030";
		dstb = "put_byte_mmu030";
	} else if (using_mmu) {
		srcl = "get_long_mmu";
		dstl = "put_long_mmu";
		srcw = "get_word_mmu";
		dstw = "put_word_mmu";
		srcb = "get_byte_mmu";
		dstb = "put_byte_mmu";
	} else if (using_ce) {
		srcl = "get_long_ce";
		dstl = "put_long_ce";
		srcw = "get_word_ce";
		dstw = "put_word_ce";
		srcb = "get_byte_ce";
		dstb = "put_byte_ce";
	} else {
		srcl = "get_long";
		dstl = "put_long";
		srcw = "get_word";
		dstw = "put_word";
		srcb = "get_byte";
		dstb = "put_byte";
	}

	insn_n_cycles020 = 0;

	start_brace ();
	m68k_pc_offset = 2;
	switch (curi->plev) {
	case 0: /* not privileged */
		break;
	case 1: /* unprivileged only on 68000 */
		if (cpu_level == 0)
			break;
		if (next_cpu_level < 0)
			next_cpu_level = 0;

		/* fall through */
	case 2: /* priviledged */
		printf ("if (!regs.s) { Exception (8, 0 ,M68000_EXC_SRC_CPU); goto %s; }\n", endlabelstr);
		need_endlabel = 1;
		start_brace ();
		break;
	case 3: /* privileged if size == word */
		if (curi->size == sz_byte)
			break;
		printf ("if (!regs.s) { Exception (8, 0 ,M68000_EXC_SRC_CPU); goto %s; }\n", endlabelstr);
		need_endlabel = 1;
		start_brace ();
		break;
	}
	switch (curi->mnemo) {
	case i_OR:
	case i_AND:
	case i_EOR:
	{
		int c = 0;
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "dst", 1, 0, 0);
		printf ("\tsrc %c= dst;\n", curi->mnemo == i_OR ? '|' : curi->mnemo == i_AND ? '&' : '^');
		genflags (flag_logical, curi->size, "src", "", "");
		if (curi->dmode == Dreg && curi->size == sz_long) {
			c += 2;
			if (curi->smode == imm || curi->smode == Dreg)
				c += 2;
		}
		fill_prefetch_next ();
		if (c > 0)
			addcycles000 (c);
		genastore_rev ("src", curi->dmode, "dstreg", curi->size, "dst");
		break;
	}
	// all SR/CCR modifications have dummy read access
	case i_ORSR:
	case i_EORSR:
		printf ("\tMakeSR ();\n");
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		dummy_prefetch ();
		if (curi->size == sz_byte) {
			printf ("\tsrc &= 0xFF;\n");
		}
		addcycles000 (8);
		fill_prefetch_next ();
		printf ("\tregs.sr %c= src;\n", curi->mnemo == i_EORSR ? '^' : '|');
		printf ("\tMakeFromSR ();\n");
		break;
	case i_ANDSR:
		printf ("\tMakeSR ();\n");
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		dummy_prefetch ();
		if (curi->size == sz_byte) {
			printf ("\tsrc |= 0xFF00;\n");
		}
		addcycles000 (8);
		fill_prefetch_next ();
		printf ("\tregs.sr &= src;\n");
		printf ("\tMakeFromSR ();\n");
		break;
	case i_SUB:
	{
		int c = 0;
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "dst", 1, 0, 0);
		if (curi->dmode == Dreg) {
			if (curi->size == sz_long) {
				c += 2;
				if (curi->smode == imm || curi->smode == immi || curi->smode == Dreg)
					c += 2;
			}
		}
		fill_prefetch_next ();
		if (c > 0)
			addcycles000 (c);
		start_brace ();
		genflags (flag_sub, curi->size, "newv", "src", "dst");
		genastore_rev ("newv", curi->dmode, "dstreg", curi->size, "dst");
		break;
	}
	case i_SUBA:
	{
		int c = 0;
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		genamode (curi->dmode, "dstreg", sz_long, "dst", 1, 0, 0);
		if (curi->smode == immi) {
			// SUBAQ.x is always 8 cycles
			c += 4;
		} else {
			c = curi->size == sz_long ? 2 : 4;
			if (islongimm (curi))
				c += 2;
		}
		fill_prefetch_next ();
		if (c > 0)
			addcycles000 (c);
		start_brace ();
		printf ("\tuae_u32 newv = dst - src;\n");
		genastore ("newv", curi->dmode, "dstreg", sz_long, "dst");
		break;
	}
	case i_SUBX:
		if (!isreg (curi->smode))
			addcycles000 (2);
		genamode_pre (curi->smode, "srcreg", curi->size, "src", 1, 0, GF_AA);
		genamode (curi->dmode, "dstreg", curi->size, "dst", 1, 0, GF_AA);
		genamode_post (curi->smode, "srcreg", curi->size, "src", 1, 0, GF_AA);
		fill_prefetch_next ();
		if (curi->size == sz_long && isreg (curi->smode))
			addcycles000 (4);
		start_brace ();
		printf ("\tuae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);\n");
		genflags (flag_subx, curi->size, "newv", "src", "dst");
		genflags (flag_zn, curi->size, "newv", "", "");
		genastore ("newv", curi->dmode, "dstreg", curi->size, "dst");
		break;
	case i_SBCD:
		if (!isreg (curi->smode))
			addcycles000 (2);
		genamode_pre (curi->smode, "srcreg", curi->size, "src", 1, 0, GF_AA);
		genamode (curi->dmode, "dstreg", curi->size, "dst", 1, 0, GF_AA);
		genamode_post (curi->smode, "srcreg", curi->size, "src", 1, 0, GF_AA);
		fill_prefetch_next ();
		start_brace ();
		printf ("\tuae_u16 newv_lo = (dst & 0xF) - (src & 0xF) - (GET_XFLG () ? 1 : 0);\n");
		printf ("\tuae_u16 newv_hi = (dst & 0xF0) - (src & 0xF0);\n");
		printf ("\tuae_u16 newv, tmp_newv;\n");
		printf ("\tint bcd = 0;\n");
		printf ("\tnewv = tmp_newv = newv_hi + newv_lo;\n");
		printf ("\tif (newv_lo & 0xF0) { newv -= 6; bcd = 6; };\n");
		printf ("\tif ((((dst & 0xFF) - (src & 0xFF) - (GET_XFLG () ? 1 : 0)) & 0x100) > 0xFF) { newv -= 0x60; }\n");
		printf ("\tSET_CFLG ((((dst & 0xFF) - (src & 0xFF) - bcd - (GET_XFLG () ? 1 : 0)) & 0x300) > 0xFF);\n");
		duplicate_carry (0);
		/* Manual says bits NV are undefined though a real 68040/060 don't change them */
		if (cpu_level >= xBCD_KEEPS_NV_FLAGS) {
			if (next_cpu_level < xBCD_KEEPS_NV_FLAGS)
				next_cpu_level = xBCD_KEEPS_NV_FLAGS - 1;
			genflags (flag_z, curi->size, "newv", "", "");
		} else {
			genflags (flag_zn, curi->size, "newv", "", "");
			printf ("\tSET_VFLG ((tmp_newv & 0x80) != 0 && (newv & 0x80) == 0);\n");
		}
		if (isreg (curi->smode))
			addcycles000 (2);
		genastore ("newv", curi->dmode, "dstreg", curi->size, "dst");
		break;
	case i_ADD:
	{
		int c = 0;
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "dst", 1, 0, 0);
		if (curi->dmode == Dreg) {
			if (curi->size == sz_long) {
				c += 2;
				if (curi->smode == imm || curi->smode == immi || curi->smode == Dreg)
					c += 2;
			}
		}
		fill_prefetch_next ();
		if (c > 0)
			addcycles000 (c);
		start_brace ();
		genflags (flag_add, curi->size, "newv", "src", "dst");
		genastore_rev ("newv", curi->dmode, "dstreg", curi->size, "dst");
		break;
	}
	case i_ADDA:
	{
		int c = 0;
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		genamode (curi->dmode, "dstreg", sz_long, "dst", 1, 0, 0);
		if (curi->smode == immi) {
			// ADDAQ.x is always 8 cycles
			c += 4;
		} else {
			c = curi->size == sz_long ? 2 : 4;
			if (islongimm (curi))
				c += 2;
		}
		fill_prefetch_next ();
		if (c > 0)
			addcycles000 (c);
		start_brace ();
		printf ("\tuae_u32 newv = dst + src;\n");
		genastore ("newv", curi->dmode, "dstreg", sz_long, "dst");
		break;
	}
	case i_ADDX:
		if (!isreg (curi->smode))
			addcycles000 (2);
		genamode_pre (curi->smode, "srcreg", curi->size, "src", 1, 0, GF_AA);
		genamode (curi->dmode, "dstreg", curi->size, "dst", 1, 0, GF_AA);
		genamode_post (curi->smode, "srcreg", curi->size, "src", 1, 0, GF_AA);
		fill_prefetch_next ();
		if (curi->size == sz_long && isreg (curi->smode))
			addcycles000 (4);
		start_brace ();
		printf ("\tuae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);\n");
		genflags (flag_addx, curi->size, "newv", "src", "dst");
		genflags (flag_zn, curi->size, "newv", "", "");
		genastore ("newv", curi->dmode, "dstreg", curi->size, "dst");
		break;
	case i_ABCD:
		if (!isreg (curi->smode))
			addcycles000 (2);
		genamode_pre (curi->smode, "srcreg", curi->size, "src", 1, 0, GF_AA);
		genamode (curi->dmode, "dstreg", curi->size, "dst", 1, 0, GF_AA);
		genamode_post (curi->smode, "srcreg", curi->size, "src", 1, 0, GF_AA);
		fill_prefetch_next ();
		start_brace ();
		printf ("\tuae_u16 newv_lo = (src & 0xF) + (dst & 0xF) + (GET_XFLG () ? 1 : 0);\n");
		printf ("\tuae_u16 newv_hi = (src & 0xF0) + (dst & 0xF0);\n");
		printf ("\tuae_u16 newv, tmp_newv;\n");
		printf ("\tint cflg;\n");
		printf ("\tnewv = tmp_newv = newv_hi + newv_lo;");
		printf ("\tif (newv_lo > 9) { newv += 6; }\n");
		printf ("\tcflg = (newv & 0x3F0) > 0x90;\n");
		printf ("\tif (cflg) newv += 0x60;\n");
		printf ("\tSET_CFLG (cflg);\n");
		duplicate_carry (0);
		/* Manual says bits NV are undefined though a real 68040 don't change them */
		if (cpu_level >= xBCD_KEEPS_NV_FLAGS) {
			if (next_cpu_level < xBCD_KEEPS_NV_FLAGS)
				next_cpu_level = xBCD_KEEPS_NV_FLAGS - 1;
			genflags (flag_z, curi->size, "newv", "", "");
		}
		else {
			genflags (flag_zn, curi->size, "newv", "", "");
			printf ("\tSET_VFLG ((tmp_newv & 0x80) == 0 && (newv & 0x80) != 0);\n");
		}
		if (isreg (curi->smode))
			addcycles000 (2);
		genastore ("newv", curi->dmode, "dstreg", curi->size, "dst");
		break;
	case i_NEG:
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		fill_prefetch_next ();
		if (isreg (curi->smode) && curi->size == sz_long)
			addcycles000 (2);
		start_brace ();
		genflags (flag_sub, curi->size, "dst", "src", "0");
		genastore_rev ("dst", curi->smode, "srcreg", curi->size, "src");
		break;
	case i_NEGX:
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		fill_prefetch_next ();
		if (isreg (curi->smode) && curi->size == sz_long)
			addcycles000 (2);
		start_brace ();
		printf ("\tuae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);\n");
		genflags (flag_subx, curi->size, "newv", "src", "0");
		genflags (flag_zn, curi->size, "newv", "", "");
		genastore_rev ("newv", curi->smode, "srcreg", curi->size, "src");
		break;
	case i_NBCD:
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		if (isreg (curi->smode))
			addcycles000 (2);
		fill_prefetch_next ();
		start_brace ();
		printf ("\tuae_u16 newv_lo = - (src & 0xF) - (GET_XFLG () ? 1 : 0);\n");
		printf ("\tuae_u16 newv_hi = - (src & 0xF0);\n");
		printf ("\tuae_u16 newv;\n");
		printf ("\tint cflg;\n");
		printf ("\tif (newv_lo > 9) { newv_lo -= 6; }\n");
		printf ("\tnewv = newv_hi + newv_lo;");
		printf ("\tcflg = (newv & 0x1F0) > 0x90;\n");
		printf ("\tif (cflg) newv -= 0x60;\n");
		printf ("\tSET_CFLG (cflg);\n");
		duplicate_carry(0);
		/* Manual says bits NV are undefined though a real 68040 don't change them */
		if (cpu_level >= xBCD_KEEPS_NV_FLAGS) {
			if (next_cpu_level < xBCD_KEEPS_NV_FLAGS)
				next_cpu_level = xBCD_KEEPS_NV_FLAGS - 1;
			genflags (flag_z, curi->size, "newv", "", "");
		}
		else {
			genflags (flag_zn, curi->size, "newv", "", "");
		}
		genastore ("newv", curi->smode, "srcreg", curi->size, "src");
		break;
	case i_CLR:
		genamode (curi->smode, "srcreg", curi->size, "src", cpu_level == 0 ? 1 : 2, 0, 0);
		fill_prefetch_next ();
		if (isreg (curi->smode) && curi->size == sz_long)
			addcycles000 (2);
		genflags (flag_logical, curi->size, "0", "", "");
		genastore_rev ("0", curi->smode, "srcreg", curi->size, "src");
		break;
	case i_NOT:
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		fill_prefetch_next ();
		if (isreg (curi->smode) && curi->size == sz_long)
			addcycles000 (2);
		start_brace ();
		printf ("\tuae_u32 dst = ~src;\n");
		genflags (flag_logical, curi->size, "dst", "", "");
		genastore_rev ("dst", curi->smode, "srcreg", curi->size, "src");
		break;
	case i_TST:
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		fill_prefetch_next ();
		genflags (flag_logical, curi->size, "src", "", "");
		break;
	case i_BTST:
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "dst", 1, 0, 0);
		fill_prefetch_next ();
		bsetcycles (curi);
		printf ("\tSET_ZFLG (1 ^ ((dst >> src) & 1));\n");
		break;
	case i_BCHG:
	case i_BCLR:
	case i_BSET:
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "dst", 1, 0, 0);
		fill_prefetch_next ();
		bsetcycles (curi);
		// bclr needs 1 extra cycle
		if (curi->mnemo == i_BCLR && curi->dmode == Dreg)
			addcycles000 (2);
		if (curi->mnemo == i_BCHG) {
			printf ("\tdst ^= (1 << src);\n");
			printf ("\tSET_ZFLG (((uae_u32)dst & (1 << src)) >> src);\n");
		} else if (curi->mnemo == i_BCLR) {
			printf ("\tSET_ZFLG (1 ^ ((dst >> src) & 1));\n");
			printf ("\tdst &= ~(1 << src);\n");
		} else if (curi->mnemo == i_BSET) {
			printf ("\tSET_ZFLG (1 ^ ((dst >> src) & 1));\n");
			printf ("\tdst |= (1 << src);\n");
		}
		genastore ("dst", curi->dmode, "dstreg", curi->size, "dst");
		break;
	case i_CMPM:
		// confirmed
		genamode_pre (curi->smode, "srcreg", curi->size, "src", 1, 0, GF_AA);
		genamode (curi->dmode, "dstreg", curi->size, "dst", 1, 0, GF_AA);
		genamode_post (curi->smode, "srcreg", curi->size, "src", 1, 0, GF_AA);
		fill_prefetch_next ();
		start_brace ();
		genflags (flag_cmp, curi->size, "newv", "src", "dst");
		break;
	case i_CMP:
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "dst", 1, 0, 0);
		fill_prefetch_next ();
		if (curi->dmode == Dreg && curi->size == sz_long)
			addcycles000 (2);
		start_brace ();
		genflags (flag_cmp, curi->size, "newv", "src", "dst");
		break;
	case i_CMPA:
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		genamode (curi->dmode, "dstreg", sz_long, "dst", 1, 0, 0);
		fill_prefetch_next ();
		addcycles000 (2);
		start_brace ();
		genflags (flag_cmp, sz_long, "newv", "src", "dst");
		break;
		/* The next two are coded a little unconventional, but they are doing
		* weird things... */
	case i_MVPRM: // MOVEP R->M
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		printf ("\tuaecptr memp = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)%s;\n", gen_nextiword (0));
		/* [NP] Use MovepByteNbr to keep track of each access inside a movep */
		if (curi->size == sz_word) {
			printf ("\tMovepByteNbr=1; %s (memp, src >> 8); MovepByteNbr=2; %s (memp + 2, src);\n", dstb, dstb);
			count_write += 2;
		} else {
			printf ("\tMovepByteNbr=1; %s (memp, src >> 24); MovepByteNbr=2; %s (memp + 2, src >> 16);\n", dstb, dstb);
			printf ("\tMovepByteNbr=3; %s (memp + 4, src >> 8); MovepByteNbr=4; %s (memp + 6, src);\n", dstb, dstb);
			count_write += 4;
		}
		printf ("\tMovepByteNbr=0;\n");
		fill_prefetch_next ();
		break;
	case i_MVPMR: // MOVEP M->R
		printf ("\tuaecptr memp = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)%s;\n", gen_nextiword (0));
		genamode (curi->dmode, "dstreg", curi->size, "dst", 2, 0, 0);
		/* [NP] Use MovepByteNbr to keep track of each access inside a movep */
		if (curi->size == sz_word) {
			//printf ("\tuae_u16 val = (%s (memp) << 8) + %s (memp + 2);\n", srcb, srcb);
			printf ("\tuae_u16 val;\n");
			printf ("\tMovepByteNbr=1; val = (%s (memp) << 8);\n", srcb);
			printf ("\tMovepByteNbr=2; val += %s (memp + 2);\n", srcb);
			count_read += 2;
		} else {
			//printf ("\tuae_u32 val = (%s (memp) << 24) + (%s (memp + 2) << 16)\n", srcb, srcb);
			//printf ("              + (%s (memp + 4) << 8) + %s (memp + 6);\n", srcb, srcb);
			printf ("\tuae_u32 val;\n");
			printf ("\tMovepByteNbr=1; val = (%s (memp) << 24);\n", srcb);
			printf ("\tMovepByteNbr=2; val += (%s (memp + 2) << 16);\n", srcb);
			printf ("\tMovepByteNbr=2; val += (%s (memp + 4) << 8);\n", srcb);
			printf ("\tMovepByteNbr=4; val += %s (memp + 6);\n", srcb);
			count_read += 4;
		}
		printf ("\tMovepByteNbr=0;\n");
		fill_prefetch_next ();
		genastore ("val", curi->dmode, "dstreg", curi->size, "dst");
		break;
	case i_MOVE:
	case i_MOVEA:
		{
			/* 2 MOVE instruction variants have special prefetch sequence:
			* - MOVE <ea>,-(An) = prefetch is before writes (Apdi)
			* - MOVE memory,(xxx).L = 2 prefetches after write
			* - all others = prefetch is done after writes
			*
			* - move.x xxx,[at least 1 extension word here] = fetch 1 extension word before (xxx)
			*
			*/
			int prefetch_done = 0, flags;
			int dualprefetch = curi->dmode == absl && (curi->smode != Dreg && curi->smode != Areg && curi->smode != imm);
			genamode_pre (curi->smode, "srcreg", curi->size, "src", 1, 0, GF_MOVE);
			flags = 1 | (dualprefetch ? GF_NOREFILL : 0);
			genamode2 (curi->dmode, "dstreg", curi->size, "dst", 2, 0, flags | GF_MOVE, 0);
			genamode_post (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
			if (curi->mnemo == i_MOVEA && curi->size == sz_word)
				printf ("\tsrc = (uae_s32)(uae_s16)src;\n");
			if (curi->dmode == Apdi) {
				fill_prefetch_next ();
				prefetch_done = 1;
			}
			if (curi->mnemo == i_MOVE)
				genflags (flag_logical, curi->size, "src", "", "");
			genastore ("src", curi->dmode, "dstreg", curi->size, "dst");
			sync_m68k_pc ();
			if (dualprefetch) {
				fill_prefetch_full ();
				prefetch_done = 1;
			}
			if (!prefetch_done)
				fill_prefetch_next ();
		}
		break;
	case i_MVSR2: // MOVE FROM SR
		genamode (curi->smode, "srcreg", sz_word, "src", 2, 0, 0);
		fill_prefetch_next ();
		if (isreg (curi->smode))
			addcycles000 (2);
		printf ("\tMakeSR ();\n");
		if (curi->size == sz_byte)
			genastore ("regs.sr & 0xff", curi->smode, "srcreg", sz_word, "src");
		else
			genastore ("regs.sr", curi->smode, "srcreg", sz_word, "src");
		break;
	case i_MV2SR: // MOVE TO SR
		genamode (curi->smode, "srcreg", sz_word, "src", 1, 0, 0);
		if (curi->size == sz_byte) {
			// MOVE TO CCR
			dummy_prefetch ();
			addcycles000 (4);
			printf ("\tMakeSR ();\n\tregs.sr &= 0xFF00;\n\tregs.sr |= src & 0xFF;\n");
		} else {
			// MOVE TO SR
			dummy_prefetch ();
			addcycles000 (4);
			printf ("\tregs.sr = src;\n");
		}
		printf ("\tMakeFromSR ();\n");
		fill_prefetch_next ();
		break;
	case i_SWAP:
		genamode (curi->smode, "srcreg", sz_long, "src", 1, 0, 0);
		fill_prefetch_next ();
		start_brace ();
		printf ("\tuae_u32 dst = ((src >> 16)&0xFFFF) | ((src&0xFFFF)<<16);\n");
		genflags (flag_logical, sz_long, "dst", "", "");
		genastore ("dst", curi->smode, "srcreg", sz_long, "src");
		break;
	case i_EXG:
		// confirmed
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "dst", 1, 0, 0);
		fill_prefetch_next ();
		addcycles000 (2);
		genastore ("dst", curi->smode, "srcreg", curi->size, "src");
		genastore ("src", curi->dmode, "dstreg", curi->size, "dst");
		break;
	case i_EXT:
		// confirmed
		genamode (curi->smode, "srcreg", sz_long, "src", 1, 0, 0);
		fill_prefetch_next ();
		start_brace ();
		switch (curi->size) {
		case sz_byte: printf ("\tuae_u32 dst = (uae_s32)(uae_s8)src;\n"); break;
		case sz_word: printf ("\tuae_u16 dst = (uae_s16)(uae_s8)src;\n"); break;
		case sz_long: printf ("\tuae_u32 dst = (uae_s32)(uae_s16)src;\n"); break;
		default: abort ();
		}
		genflags (flag_logical,
			curi->size == sz_word ? sz_word : sz_long, "dst", "", "");
		genastore ("dst", curi->smode, "srcreg",
			curi->size == sz_word ? sz_word : sz_long, "src");
		break;
	case i_MVMEL:
		// confirmed
		if (using_ce)
			genmovemel_ce (opcode);
		else
			genmovemel (opcode);
		break;
	case i_MVMLE:
		// confirmed
		if (using_ce)
			genmovemle_ce (opcode);
		else
			genmovemle (opcode);
		break;
	case i_TRAP:
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		gen_set_fault_pc ();
		sync_m68k_pc ();
		printf ("\tException (src + 32, 0 ,M68000_EXC_SRC_CPU);\n");
		did_prefetch = 1;
		m68k_pc_offset = 0;
		break;
	case i_MVR2USP:
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		fill_prefetch_next ();
		printf ("\tregs.usp = src;\n");
		break;
	case i_MVUSP2R:
		genamode (curi->smode, "srcreg", curi->size, "src", 2, 0, 0);
		fill_prefetch_next ();
		genastore ("regs.usp", curi->smode, "srcreg", curi->size, "src");
		break;
	case i_RESET:
		fill_prefetch_next ();
		printf ("\tcpureset ();\n");
		addcycles000 (128);
		if (using_prefetch)
			printf ("\tregs.irc = get_iword (4);\n");
		break;
	case i_NOP:
		fill_prefetch_next ();
		break;
	case i_STOP:
		if (using_prefetch) {
			printf ("\tregs.sr = regs.irc;\n");
			m68k_pc_offset += 2;
		} else {
			genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
			printf ("\tregs.sr = src;\n");
		}
		printf ("\tMakeFromSR ();\n");
		printf ("\tm68k_setstopped ();\n");
		sync_m68k_pc ();
		// STOP does not prefetch anything
		did_prefetch = -1;
		break;
	case i_LPSTOP: /* 68060 */
		printf ("\tuae_u16 sw = get_iword (2);\n");
		printf ("\tuae_u16 sr;\n");
		printf ("\tif (sw != (0x100|0x80|0x40)) { Exception (4, 0 ,M68000_EXC_SRC_CPU); goto %s; }\n", endlabelstr);
		printf ("\tsr = get_iword (4);\n");
		printf ("\tif (!(sr & 0x8000)) { Exception (8, 0 ,M68000_EXC_SRC_CPU); goto %s; }\n", endlabelstr);
		printf ("\tregs.sr = sr;\n");
		printf ("\tMakeFromSR ();\n");
		printf ("\tm68k_setstopped();\n");
		m68k_pc_offset += 4;
		sync_m68k_pc ();
		fill_prefetch_full ();
		break;
	case i_RTE:
		if (cpu_level == 0) {
			genamode (Aipi, "7", sz_word, "sr", 1, 0, GF_NOREFILL);
			genamode (Aipi, "7", sz_long, "pc", 1, 0, GF_NOREFILL);
			printf ("\tregs.sr = sr;\n");
			setpc ("pc");
			printf ("\tMakeFromSR ();\n");
		} else {
		    int old_brace_level = n_braces;
		    if (next_cpu_level < 0)
				next_cpu_level = 0;
		    printf ("\tuae_u16 newsr; uae_u32 newpc;\n");
			printf ("\tfor (;;) {\n");
			printf ("\t\tuaecptr a = m68k_areg (regs, 7);\n");
			printf ("\t\tuae_u16 sr = %s (a);\n", srcw);
			printf ("\t\tuae_u32 pc = %s (a + 2);\n", srcl);
			printf ("\t\tuae_u16 format = %s (a + 2 + 4);\n", srcw);
			printf ("\t\tint frame = format >> 12;\n");
			printf ("\t\tint offset = 8;\n");
			printf ("\t\tnewsr = sr; newpc = pc;\n");
		    printf ("\t\tif (frame == 0x0) { m68k_areg (regs, 7) += offset; break; }\n");
		    printf ("\t\telse if (frame == 0x1) { m68k_areg (regs, 7) += offset; }\n");
		    printf ("\t\telse if (frame == 0x2) { m68k_areg (regs, 7) += offset + 4; break; }\n");
		    printf ("\t\telse if (frame == 0x4) { m68k_areg (regs, 7) += offset + 8; break; }\n");
			if (using_mmu)
			printf ("\t\telse if (frame == 0x7) { m68k_do_rte_mmu%s (a); m68k_areg (regs, 7) += offset + 52; break; }\n", mmu_postfix);
		    printf ("\t\telse if (frame == 0x8) { m68k_areg (regs, 7) += offset + 50; break; }\n");
		    printf ("\t\telse if (frame == 0x9) { m68k_areg (regs, 7) += offset + 12; break; }\n");
		    printf ("\t\telse if (frame == 0xa) { m68k_areg (regs, 7) += offset + 24; break; }\n");
		    printf ("\t\telse if (frame == 0xb) { m68k_areg (regs, 7) += offset + 84; break; }\n");
		    printf ("\t\telse { m68k_areg (regs, 7) += offset; Exception (14, 0 ,M68000_EXC_SRC_CPU); goto %s; }\n", endlabelstr);
		    printf ("\t\tregs.sr = newsr; MakeFromSR ();\n}\n");
		    pop_braces (old_brace_level);
		    printf ("\tregs.sr = newsr; MakeFromSR ();\n");
		    printf ("\tif (newpc & 1) {\n");
		    printf ("\t\texception3i (0x%04lX, m68k_getpc (), newpc);\n", opcode);
			printf ("\t\tgoto %s;\n", endlabelstr);
			printf ("\t}\n");
		    printf ("\t\tm68k_setpc (newpc);\n");
			printf ("\tipl_fetch ();\n");
		    need_endlabel = 1;
		}
		instr_table = TABLE_FALCON_CYCLES_RTE;
		instr_table_index = 0;
		/* PC is set and prefetch filled. */
		m68k_pc_offset = 0;
		fill_prefetch_full ();
		break;
	case i_RTD:
		if (using_mmu) {
			genamode (curi->smode, "srcreg", curi->size, "offs", GENA_GETV_FETCH, GENA_MOVEM_DO_INC, 0);
			genamode (Aipi, "7", sz_long, "pc", GENA_GETV_FETCH, GENA_MOVEM_DO_INC, 0);
			printf ("\tm68k_areg(regs, 7) += offs;\n");
		} else {
			genamode (Aipi, "7", sz_long, "pc", 1, 0, 0);
			genamode (curi->smode, "srcreg", curi->size, "offs", 1, 0, 0);
			printf ("\tm68k_areg (regs, 7) += offs;\n");
			printf ("\tif (pc & 1) {\n");
			printf ("\t\texception3i (0x%04lX, m68k_getpc (), pc);\n", opcode);
			printf ("\t\tgoto %s;\n", endlabelstr);
			printf ("\t}\n");
		}
	    printf ("\tif (pc & 1) {\n");
	    printf ("\t\texception3i (0x%04lX, m68k_getpc(), pc);\n", opcode);
		printf ("\t\tgoto %s;\n", endlabelstr);
		printf ("\t}\n");
		setpc ("pc");
		/* PC is set and prefetch filled. */
		m68k_pc_offset = 0;
		fill_prefetch_full ();
	    need_endlabel = 1;
		break;
	case i_LINK:
		// ce confirmed
		if (using_mmu) {
			genamode (curi->dmode, "dstreg", curi->size, "offs", GENA_GETV_FETCH, GENA_MOVEM_DO_INC, 0);
			genamode (Apdi, "7", sz_long, "old", GENA_GETV_FETCH_ALIGN, GENA_MOVEM_DO_INC, 0);
			genamode (curi->smode, "srcreg", sz_long, "src", GENA_GETV_FETCH, GENA_MOVEM_DO_INC, 0);
			genastore ("m68k_areg(regs, 7)", curi->smode, "srcreg", sz_long, "src");
			printf ("\tm68k_areg(regs, 7) += offs;\n");
			genastore ("src", Apdi, "7", sz_long, "old");
		} else {
			genamode (Apdi, "7", sz_long, "old", 2, 0, GF_AA);
			genamode (curi->smode, "srcreg", sz_long, "src", 1, 0, GF_AA);
			genamode (curi->dmode, "dstreg", curi->size, "offs", 1, 0, 0);
			genastore ("src", Apdi, "7", sz_long, "old");
			genastore ("m68k_areg (regs, 7)", curi->smode, "srcreg", sz_long, "src");
			printf ("\tm68k_areg (regs, 7) += offs;\n");
			fill_prefetch_next ();
		}
		break;
	case i_UNLK:
		// ce confirmed
		if (using_mmu) {
			genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
			printf ("\tuae_s32 old = get_long_mmu%s (src);\n", mmu_postfix);
			printf ("\tm68k_areg (regs, 7) = src + 4;\n");
			printf ("\tm68k_areg (regs, srcreg) = old;\n");
		} else {
			genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
			printf ("\tm68k_areg (regs, 7) = src;\n");
			genamode (Aipi, "7", sz_long, "old", 1, 0, 0);
			fill_prefetch_next ();
			genastore ("old", curi->smode, "srcreg", curi->size, "src");
		}
		break;
	case i_RTS:
		printf ("\tuaecptr pc = m68k_getpc ();\n");
		if (using_ce020 == 1)
			printf ("\tm68k_do_rts_ce020 ();\n");
		else if (using_ce020 == 2)
			printf ("\tm68k_do_rts_ce030 ();\n");
		else if (using_ce)
			printf ("\tm68k_do_rts_ce ();\n");
		else if (using_indirect)
			printf ("\tm68k_do_rtsi ();\n");
		else if (using_mmu)
			printf ("\tm68k_do_rts_mmu%s ();\n", mmu_postfix);
		else
			printf ("\tm68k_do_rts ();\n");
	    printf ("\tif (m68k_getpc () & 1) {\n");
		printf ("\t\tuaecptr faultpc = m68k_getpc ();\n");
		printf ("\t\tm68k_setpc (pc);\n");
		printf ("\t\texception3i (0x%04lX, pc, faultpc);\n", opcode);
		printf ("\t\tgoto %s;\n", endlabelstr);
		printf ("\t}\n");
		count_read += 2;
		m68k_pc_offset = 0;
		fill_prefetch_full ();
	    need_endlabel = 1;
		break;
	case i_TRAPV:
		sync_m68k_pc ();
		fill_prefetch_next ();
		printf ("\tif (GET_VFLG ()) {\n");
		printf ("\t\tException (7, m68k_getpc () ,M68000_EXC_SRC_CPU);\n");
		printf ("\t\tgoto %s;\n", endlabelstr);
		printf ("\t}\n");
		need_endlabel = 1;
		break;
	case i_RTR:
		printf ("\tMakeSR ();\n");
		genamode_pre (Aipi, "7", sz_word, "sr", 1, 0, 0);
		genamode (Aipi, "7", sz_long, "pc", 1, 0, 0);
		genamode_post (Aipi, "7", sz_word, "sr", 1, 0, 0);
		printf ("\tregs.sr &= 0xFF00; sr &= 0xFF;\n");
		printf ("\tregs.sr |= sr;\n");
		setpc ("pc");
		printf ("\tMakeFromSR ();\n");
		m68k_pc_offset = 0;
		fill_prefetch_full ();
		break;
	case i_JSR: // TODO: check stack write order
		genamode (curi->smode, "srcreg", curi->size, "src", 0, 0, GF_AA|GF_NOREFILL);
		start_brace ();
		printf ("\tuaecptr oldpc = m68k_getpc () + %d;\n", m68k_pc_offset);
		if (using_exception_3) {
			printf ("\tif (srca & 1) {\n");
			printf ("\t\texception3i (opcode, oldpc, srca);\n");
			printf ("\t\tgoto %s;\n", endlabelstr);
			printf ("\t}\n");
			need_endlabel = 1;
		}
		if (using_mmu) {
			printf ("\t%s (m68k_areg (regs, 7) - 4, oldpc);\n", dstl);
			printf ("\tm68k_areg (regs, 7) -= 4;\n");
			setpc ("srca");
			m68k_pc_offset = 0;
		} else {
			if (curi->smode == Ad16 || curi->smode == absw || curi->smode == PC16)
				addcycles000 (2);
			setpc ("srca");
			m68k_pc_offset = 0;
			fill_prefetch_1 (0);
			if (curi->smode == Ad8r || curi->smode == PC8r)
				addcycles000 (6);
			printf ("\tm68k_areg (regs, 7) -= 4;\n");
			if (using_ce) {
				printf ("\tput_word_ce (m68k_areg (regs, 7), oldpc >> 16);\n");
				printf ("\tput_word_ce (m68k_areg (regs, 7) + 2, oldpc);\n");
			} else {
				printf ("\t%s (m68k_areg (regs, 7), oldpc);\n", dstl);
			}
		}
		count_write += 2;
		fill_prefetch_next ();
		break;
	case i_JMP:
		genamode (curi->smode, "srcreg", curi->size, "src", 0, 0, GF_AA | ((curi->smode == Ad8r || curi->smode == PC8r) ? 0 : GF_NOREFILL));
		if (using_exception_3) {
			printf ("\tif (srca & 1) {\n");
			printf ("\t\texception3i (opcode, m68k_getpc () + 6, srca);\n");
			printf ("\t\tgoto %s;\n", endlabelstr);
			printf ("\t}\n");
			need_endlabel = 1;
		}
		if (curi->smode == Ad16 || curi->smode == Ad8r || curi->smode == absw || curi->smode == PC16 || curi->smode == PC8r)
			addcycles000 (2);
		setpc ("srca");
		m68k_pc_offset = 0;
		fill_prefetch_full ();
		break;
	case i_BSR:
		// .b and .w confirmed
		printf ("\tuae_s32 s;\n");
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, GF_AA|GF_NOREFILL);
		printf ("\ts = (uae_s32)src + 2;\n");
		if (using_exception_3) {
			printf ("\tif (src & 1) {\n");
			printf ("\t\texception3i (opcode, m68k_getpc () + 2, m68k_getpc () + s);\n");
			printf ("\t\tgoto %s;\n", endlabelstr);
			printf ("\t}\n");
			need_endlabel = 1;
		}
		addcycles000 (2);
		if (using_ce020 == 1) {
			printf ("\tm68k_do_bsr_ce020 (m68k_getpc () + %d, s);\n", m68k_pc_offset);
		} else if (using_ce020 == 2) {
			printf ("\tm68k_do_bsr_ce030 (m68k_getpc () + %d, s);\n", m68k_pc_offset);
		} else if (using_ce) {
			printf ("\tm68k_do_bsr_ce (m68k_getpc () + %d, s);\n", m68k_pc_offset);
		} else if (using_indirect) {
			printf ("\tm68k_do_bsri (m68k_getpc () + %d, s);\n", m68k_pc_offset);
		} else if (using_mmu) {
			printf ("\tm68k_do_bsr_mmu%s (m68k_getpc () + %d, s);\n", mmu_postfix, m68k_pc_offset);
		} else {
			printf ("\tm68k_do_bsr (m68k_getpc () + %d, s);\n", m68k_pc_offset);
		}
		count_write += 2;
		m68k_pc_offset = 0;
		fill_prefetch_full ();
		break;
	case i_Bcc:
		// bcc.b branch: idle cycle, prefetch, prefetch
		// bcc.b not branch: 2 idle cycles, prefetch

		instr_table = TABLE_FALCON_CYCLES_BCC;

		if (curi->size == sz_long) {
			if (cpu_level < 2) {
				addcycles000 (2);
				printf ("\tif (cctrue (%d)) {\n", curi->cc);
				printf ("\t\texception3i (opcode, m68k_getpc () + 2, m68k_getpc () + 1);\n");
				printf ("\t\tgoto %s;\n", endlabelstr);
				printf ("\t}\n");
				sync_m68k_pc ();
				irc2ir ();
				fill_prefetch_2 ();
				printf ("\tgoto %s;\n", endlabelstr);
				need_endlabel = 1;
			} else {
				if (next_cpu_level < 1)
					next_cpu_level = 1;
			}
		}
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, GF_AA | GF_NOREFILL);
		addcycles000 (2);
		printf ("\tif (!cctrue (%d)) goto didnt_jump;\n", curi->cc);
		if (using_exception_3) {
			printf ("\tif (src & 1) {\n");
			printf ("\t\texception3i (opcode, m68k_getpc () + 2, m68k_getpc () + 2 + (uae_s32)src);\n");
			printf ("\t\tgoto %s;\n", endlabelstr);
			printf ("\t}\n");
			need_endlabel = 1;
		}
		if (using_prefetch) {
			incpc ("(uae_s32)src + 2");
			fill_prefetch_full ();
			if (using_ce)
				printf ("\treturn;\n");
			else
				printf ("\treturn 10 * CYCLE_UNIT / 2;\n");
		} else {
			incpc ("(uae_s32)src + 2");
			instr_table_index = 0;
			returncycles ("", 10);
		}
		printf ("didnt_jump:;\n");
		need_endlabel = 1;
		sync_m68k_pc ();
		if (curi->size == sz_byte) {
			addcycles000 (2);
			irc2ir ();
			fill_prefetch_2 ();
		} else if (curi->size == sz_word) {
			addcycles000 (2);
			fill_prefetch_full ();
		} else {
			fill_prefetch_full ();
		}
		if (using_ce020 == 2) { /* For Falcon cycle exact timings only (CPU_EMU_21) */
			if (curi->size == sz_byte)
				instr_table_index = 1;
			else if (curi->size == sz_word)
				instr_table_index = 2;
			else
				instr_table_index = 3;
		}
		insn_n_cycles = curi->size == sz_byte ? 8 : 12;
		break;
	case i_LEA:
		genamode (curi->smode, "srcreg", curi->size, "src", 0, 0, GF_AA);
		genamode (curi->dmode, "dstreg", curi->size, "dst", 2, 0, GF_AA);
		if (curi->smode == Ad8r || curi->smode == PC8r)
			addcycles000 (2);
		fill_prefetch_next ();
		if (curi->smode == Ad8r || curi->smode == PC8r)
			addcycles000 (2);
		genastore ("srca", curi->dmode, "dstreg", curi->size, "dst");
		break;
	case i_PEA:
		genamode (curi->smode, "srcreg", curi->size, "src", 0, 0, GF_AA);
		genamode (Apdi, "7", sz_long, "dst", 2, 0, GF_AA);
		if (curi->smode == Ad8r || curi->smode == PC8r)
			addcycles000 (2);
		if (!(curi->smode == absw || curi->smode == absl))
			fill_prefetch_next ();
		if (curi->smode == Ad8r || curi->smode == PC8r)
			addcycles000 (2);
		genastore ("srca", Apdi, "7", sz_long, "dst");
		if ((curi->smode == absw || curi->smode == absl))
			fill_prefetch_next ();
		break;
	case i_DBcc:
		no_return_cycles = true;
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, GF_AA | GF_NOREFILL);
		genamode (curi->dmode, "dstreg", curi->size, "offs", 1, 0, GF_AA | GF_NOREFILL);
		printf ("\tuaecptr oldpc = m68k_getpc ();\n");
		addcycles000 (2);
		printf ("\tif (!cctrue (%d)) {\n", curi->cc);
		printf ("\t");incpc ("(uae_s32)offs + 2");
		fill_prefetch_1 (0);
		printf ("\t");genastore ("(src - 1)", curi->smode, "srcreg", curi->size, "src");

		printf ("\t\tif (src) {\n");
		if (using_ce020 == 2)
			printf ("\t\t\tregs.ce030_instr_cycles = table_falcon_cycles_DBcc[0];\n");

		if (using_exception_3) {
			printf ("\t\t\tif (offs & 1) {\n");
			printf ("\t\t\t\texception3i (opcode, m68k_getpc () + 2, m68k_getpc () + 2 + (uae_s32)offs + 2);\n");
			printf ("\t\t\t\tgoto %s;\n", endlabelstr);
			printf ("\t\t\t}\n");
			need_endlabel = 1;
		}
		irc2ir ();
		fill_prefetch_1 (2);
		returncycles ("\t\t\t", 12);
		if (using_ce || using_ce020 == 2)
			printf ("\t\t\treturn;\n");
		printf ("\t\t}\n");
		if (using_ce020 == 2)
			printf ("\t\tregs.ce030_instr_cycles = table_falcon_cycles_DBcc[1];\n");
		printf ("\t} else {\n");
		if (using_ce020 == 2)
			printf ("\t\tregs.ce030_instr_cycles = table_falcon_cycles_DBcc[2];\n");

		addcycles000_2 ("\t\t", 2);
		printf ("\t}\n");
		setpc ("oldpc + %d", m68k_pc_offset);
		m68k_pc_offset = 0;
		fill_prefetch_full ();
		insn_n_cycles = 12;
		need_endlabel = 1;
		break;
	case i_Scc:
		// confirmed
		genamode (curi->smode, "srcreg", curi->size, "src", cpu_level == 0 ? 1 : 2, 0, 0);
		start_brace ();
		fill_prefetch_next();
		start_brace ();
		printf ("\tint val = cctrue (%d) ? 0xff : 0;\n", curi->cc);
		if (using_ce) {
			printf ("\tint cycles = 0;\n");
			if (isreg (curi->smode))
				printf ("\tif (val) cycles += 2;\n");
			addcycles000_3 ("\t");
		}
		genastore ("val", curi->smode, "srcreg", curi->size, "src");
		break;
	case i_DIVU:
		printf ("\tuaecptr oldpc = m68k_getpc ();\n");
		genamode (curi->smode, "srcreg", sz_word, "src", 1, 0, 0);
		genamode (curi->dmode, "dstreg", sz_long, "dst", 1, 0, 0);
		printf ("\tCLEAR_CZNV ();\n");
		printf ("\tif (src == 0) {\n");
		if (cpu_level > 0) {
			/* 68020 sets V when dividing by zero and N if dst is negative
			* 68000 clears both
			*/
			printf("\t\tSET_VFLG (1);\n");
			printf("\t\tif (dst < 0) SET_NFLG (1);\n");
		}
		incpc ("%d", m68k_pc_offset);
		printf ("\t\tException (5, oldpc ,M68000_EXC_SRC_CPU);\n");
		printf ("\t\tgoto %s;\n", endlabelstr);
		printf ("\t} else {\n");
		printf ("\t\tuae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;\n");
		printf ("\t\tuae_u32 rem = (uae_u32)dst %% (uae_u32)(uae_u16)src;\n");
		fill_prefetch_next ();
		if (using_ce) {
			start_brace ();
			printf ("\t\tint cycles = (getDivu68kCycles((uae_u32)dst, (uae_u16)src));\n");
			addcycles000_3 ("\t\t");
		}
		/* The N flag appears to be set each time there is an overflow.
		* Weird. but 68020 only sets N when dst is negative.. */
		printf ("\t\tif (newv > 0xffff) {\n");
		printf ("\t\t\tSET_VFLG (1);\n");
#ifdef UNDEF68020
		if (cpu_level >= 2)
			printf ("\t\t\tif (currprefs.cpu_level == 0 || dst < 0) SET_NFLG (&regs, 1);\n");
		else /* ??? some 68000 revisions may not set NFLG when overflow happens.. */
#endif
			printf ("\t\t\tSET_NFLG (1);\n");
		printf ("\t\t} else {\n");
		printf ("\t\t"); genflags (flag_logical, sz_word, "newv", "", "");
		printf ("\t\t\tnewv = (newv & 0xffff) | ((uae_u32)rem << 16);\n");
		printf ("\t\t"); genastore ("newv", curi->dmode, "dstreg", sz_long, "dst");
		printf ("\t\t}\n");
		sync_m68k_pc ();
		printf ("\t}\n");
		count_ncycles++;
		insn_n_cycles += 136 - (136 - 76) / 2; /* average */
		need_endlabel = 1;
		break;
	case i_DIVS:
		printf ("\tuaecptr oldpc = m68k_getpc ();\n");
		genamode (curi->smode, "srcreg", sz_word, "src", 1, 0, 0);
		genamode (curi->dmode, "dstreg", sz_long, "dst", 1, 0, 0);
		printf ("\tCLEAR_CZNV ();\n");
		printf ("\tif (src == 0) {\n");
		if (cpu_level > 0) {
			/* 68020 sets V when dividing by zero. Z is also set.
			* 68000 clears both
			*/
			printf("\t\tSET_VFLG (1);\n");
			printf("\t\tSET_ZFLG (1);\n");
		}
		incpc ("%d", m68k_pc_offset);
		printf ("\t\tException (5, oldpc ,M68000_EXC_SRC_CPU);\n");
		printf ("\t\tgoto %s;\n", endlabelstr);
		printf ("\t} else {\n");
		printf ("\t\tuae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;\n");
		printf ("\t\tuae_u16 rem = (uae_s32)dst %% (uae_s32)(uae_s16)src;\n");
		fill_prefetch_next ();
		if (using_ce) {
			start_brace ();
			printf ("\t\tint cycles = (getDivs68kCycles((uae_s32)dst, (uae_s16)src));\n");
			addcycles000_3 ("\t\t");
		}
		printf ("\t\tif ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {\n");
		printf ("\t\t\tSET_VFLG (1);\n");
#ifdef UNDEF68020
		if (cpu_level > 0)
			printf ("\t\t\tif (currprefs.cpu_level == 0) SET_NFLG (&regs, 1);\n");
		else
#endif
			printf ("\t\t\tSET_NFLG (1);\n");
		printf ("\t\t} else {\n");
		printf ("\t\t\tif (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;\n");
		genflags (flag_logical, sz_word, "newv", "", "");
		printf ("\t\t\tnewv = (newv & 0xffff) | ((uae_u32)rem << 16);\n");
		printf ("\t\t"); genastore ("newv", curi->dmode, "dstreg", sz_long, "dst");
		printf ("\t\t}\n");
		sync_m68k_pc ();
		printf ("\t}\n");
		count_ncycles++;
		insn_n_cycles += 156 - (156 - 120) / 2; /* average */
		need_endlabel = 1;
		break;
	case i_MULU:
		genamode (curi->smode, "srcreg", sz_word, "src", 1, 0, 0);
		genamode (curi->dmode, "dstreg", sz_word, "dst", 1, 0, 0);
		fill_prefetch_next();
		start_brace ();
		printf ("\tuae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;\n");
		if (using_ce)
			printf ("\tint cycles = 38 - 4, bits;\n");
		genflags (flag_logical, sz_long, "newv", "", "");
		if (using_ce) {
			printf ("\tfor(bits = 0; bits < 16 && src; bits++, src >>= 1)\n");
			printf ("\t\tif (src & 1) cycles += 2;\n");
			addcycles000_3 ("\t");
		}
		genastore ("newv", curi->dmode, "dstreg", sz_long, "dst");
		sync_m68k_pc ();
		count_cycles += 38 - 4;
		count_ncycles++;
		insn_n_cycles += (70 - 38) / 2 + 38; /* average */
		break;
	case i_MULS:
		genamode (curi->smode, "srcreg", sz_word, "src", 1, 0, 0);
		genamode (curi->dmode, "dstreg", sz_word, "dst", 1, 0, 0);
		fill_prefetch_next();
		start_brace ();
		printf ("\tuae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;\n");
		if (using_ce) {
			printf ("\tint cycles = 38 - 4, bits;\n");
			printf ("\tuae_u32 usrc;\n");
		}
		genflags (flag_logical, sz_long, "newv", "", "");
		if (using_ce) {
			printf ("\tusrc = ((uae_u32)src) << 1;\n");
			printf ("\tfor(bits = 0; bits < 16 && usrc; bits++, usrc >>= 1)\n");
			printf ("\t\tif ((usrc & 3) == 1 || (usrc & 3) == 2) cycles += 2;\n");
			addcycles000_3 ("\t");
		}
		genastore ("newv", curi->dmode, "dstreg", sz_long, "dst");
		count_cycles += 38 - 4;
		count_ncycles++;
		insn_n_cycles += (70 - 38) / 2 + 38; /* average */
		break;
	case i_CHK:
		printf ("\tuaecptr oldpc = m68k_getpc ();\n");
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "dst", 1, 0, 0);
		sync_m68k_pc ();
		addcycles000 (4);
		printf ("\tif (dst > src) {\n");
		printf ("\t\tSET_NFLG (0);\n");
		printf ("\t\tException (6, oldpc ,M68000_EXC_SRC_CPU);\n");
		printf ("\t\tgoto %s;\n", endlabelstr);
		printf ("\t}\n");
		addcycles000 (2);
		printf ("\tif ((uae_s32)dst < 0) {\n");
		printf ("\t\tSET_NFLG (1);\n");
		printf ("\t\tException (6, oldpc ,M68000_EXC_SRC_CPU);\n");
		printf ("\t\tgoto %s;\n", endlabelstr);
		printf ("\t}\n");
		fill_prefetch_next ();
		need_endlabel = 1;
		break;
	case i_CHK2:
		printf ("\tuaecptr oldpc = m68k_getpc ();\n");
		genamode (curi->smode, "srcreg", curi->size, "extra", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "dst", 2, 0, 0);
		fill_prefetch_0 ();
		printf ("\t{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];\n");
		switch (curi->size) {
		case sz_byte:
			printf ("\tlower = (uae_s32)(uae_s8)%s (dsta); upper = (uae_s32)(uae_s8)%s (dsta + 1);\n", srcb, srcb);
			printf ("\tif ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;\n");
			if (using_ce020 == 2) {
				instr_table = TABLE_FALCON_CYCLES_CHK2_BW;
				instr_table_index = chk2_b_index * 2;
				chk2_b_index ++;
			}
			break;
		case sz_word:
			printf ("\tlower = (uae_s32)(uae_s16)%s (dsta); upper = (uae_s32)(uae_s16)%s (dsta + 2);\n", srcw, srcw);
			printf ("\tif ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;\n");
			if (using_ce020 == 2) {
				instr_table = TABLE_FALCON_CYCLES_CHK2_BW;
				instr_table_index = chk2_w_index * 2;
				chk2_w_index ++;
			}
			break;
		case sz_long:
			printf ("\tlower = %s (dsta); upper = %s (dsta + 4);\n", srcl, srcl);
			if (using_ce020 == 2) {
				instr_table = TABLE_FALCON_CYCLES_CHK2_L;
				instr_table_index = chk2_l_index * 2;
				chk2_l_index ++;
			}
			break;
		default:
			abort ();
		}
		printf ("\tSET_ZFLG (upper == reg || lower == reg);\n");
		printf ("\tSET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);\n");
		printf ("\tif ((extra & 0x800) && GET_CFLG ()) {\n");
		printf ("\t\tException (6, oldpc ,M68000_EXC_SRC_CPU);\n");
		instr_table_index ++;	/* take the Exception cycle value in the CHK2 table */
		returncycles ("\t", 0);
		printf ("\t}\n}\n");
		instr_table_index --;	/* take the No Exception cycle value in the CHK2 table */
		break;

	case i_ASR:
		genamode (curi->smode, "srcreg", curi->size, "cnt", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "data", 1, 0, 0);
		fill_prefetch_next();
		start_brace ();
		switch (curi->size) {
		case sz_byte: printf ("\tuae_u32 val = (uae_u8)data;\n"); break;
		case sz_word: printf ("\tuae_u32 val = (uae_u16)data;\n"); break;
		case sz_long: printf ("\tuae_u32 val = data;\n"); break;
		default: abort ();
		}
		printf ("\tuae_u32 sign = (%s & val) >> %d;\n", cmask (curi->size), bit_size (curi->size) - 1);
		printf ("\tint ccnt = cnt & 63;\n");
		printf ("\tcnt &= 63;\n");
		printf ("\tCLEAR_CZNV ();\n");
		printf ("\tif (cnt >= %d) {\n", bit_size (curi->size));
		printf ("\t\tval = %s & (uae_u32)-sign;\n", bit_mask (curi->size));
		printf ("\t\tSET_CFLG (sign);\n");
		duplicate_carry (1);
		if (source_is_imm1_8 (curi))
			printf ("\t} else {\n");
		else {
			if (using_ce020 == 2){
				no_return_cycles = true;
				printf ("\t\tregs.ce030_instr_cycles = table_falcon_cycles_ASR[1];\n");
			}
			printf ("\t} else if (cnt > 0) {\n");
			if (using_ce020 == 2)
				printf ("\t\tregs.ce030_instr_cycles = table_falcon_cycles_ASR[0];\n");
		}
		printf ("\t\tval >>= cnt - 1;\n");
		printf ("\t\tSET_CFLG (val & 1);\n");
		duplicate_carry (1);
		printf ("\t\tval >>= 1;\n");
		printf ("\t\tval |= (%s << (%d - cnt)) & (uae_u32)-sign;\n",
			bit_mask (curi->size),
			bit_size (curi->size));
		printf ("\t\tval &= %s;\n", bit_mask (curi->size));
		printf ("\t}\n");
		genflags (flag_logical_noclobber, curi->size, "val", "", "");
		shift_ce (curi->dmode, curi->size);
		genastore ("val", curi->dmode, "dstreg", curi->size, "data");
		break;
	case i_ASL:
		genamode (curi->smode, "srcreg", curi->size, "cnt", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "data", 1, 0, 0);
		fill_prefetch_next();
		start_brace ();
		switch (curi->size) {
		case sz_byte: printf ("\tuae_u32 val = (uae_u8)data;\n"); break;
		case sz_word: printf ("\tuae_u32 val = (uae_u16)data;\n"); break;
		case sz_long: printf ("\tuae_u32 val = data;\n"); break;
		default: abort ();
		}
		printf ("\tint ccnt = cnt & 63;\n");
		printf ("\tcnt &= 63;\n");
		printf ("\tCLEAR_CZNV ();\n");
		printf ("\tif (cnt >= %d) {\n", bit_size (curi->size));
		printf ("\t\tSET_VFLG (val != 0);\n");
		printf ("\t\tSET_CFLG (cnt == %d ? val & 1 : 0);\n",
			bit_size (curi->size));
		duplicate_carry (1);
		printf ("\t\tval = 0;\n");
		if (source_is_imm1_8 (curi))
			printf ("\t} else {\n");
		else
			printf ("\t} else if (cnt > 0) {\n");
		printf ("\t\tuae_u32 mask = (%s << (%d - cnt)) & %s;\n",
			bit_mask (curi->size),
			bit_size (curi->size) - 1,
			bit_mask (curi->size));
		printf ("\t\tSET_VFLG ((val & mask) != mask && (val & mask) != 0);\n");
		printf ("\t\tval <<= cnt - 1;\n");
		printf ("\t\tSET_CFLG ((val & %s) >> %d);\n", cmask (curi->size), bit_size (curi->size) - 1);
		duplicate_carry (1);
		printf ("\t\tval <<= 1;\n");
		printf ("\t\tval &= %s;\n", bit_mask (curi->size));
		printf ("\t}\n");
		genflags (flag_logical_noclobber, curi->size, "val", "", "");
		shift_ce (curi->dmode, curi->size);
		genastore ("val", curi->dmode, "dstreg", curi->size, "data");
		break;
	case i_LSR:
		genamode (curi->smode, "srcreg", curi->size, "cnt", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "data", 1, 0, 0);
		fill_prefetch_next();
		start_brace ();
		switch (curi->size) {
		case sz_byte: printf ("\tuae_u32 val = (uae_u8)data;\n"); break;
		case sz_word: printf ("\tuae_u32 val = (uae_u16)data;\n"); break;
		case sz_long: printf ("\tuae_u32 val = data;\n"); break;
		default: abort ();
		}
		printf ("\tint ccnt = cnt & 63;\n");
		printf ("\tcnt &= 63;\n");
		printf ("\tCLEAR_CZNV ();\n");
		printf ("\tif (cnt >= %d) {\n", bit_size (curi->size));
		printf ("\t\tSET_CFLG ((cnt == %d) & (val >> %d));\n",
			bit_size (curi->size), bit_size (curi->size) - 1);
		duplicate_carry (1);
		printf ("\t\tval = 0;\n");
		if (source_is_imm1_8 (curi))
			printf ("\t} else {\n");
		else {
			if (using_ce020 == 2){
				no_return_cycles = true;
				printf ("\t\tregs.ce030_instr_cycles = table_falcon_cycles_LSD[1];\n");
			}
			printf ("\t} else if (cnt > 0) {\n");
			if (using_ce020 == 2)
				printf ("\t\tregs.ce030_instr_cycles = table_falcon_cycles_LSD[0];\n");
		}
		printf ("\t\tval >>= cnt - 1;\n");
		printf ("\t\tSET_CFLG (val & 1);\n");
		duplicate_carry (1);
		printf ("\t\tval >>= 1;\n");
		printf ("\t}\n");
		genflags (flag_logical_noclobber, curi->size, "val", "", "");
		shift_ce (curi->dmode, curi->size);
		genastore ("val", curi->dmode, "dstreg", curi->size, "data");
		break;
	case i_LSL:
		genamode (curi->smode, "srcreg", curi->size, "cnt", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "data", 1, 0, 0);
		fill_prefetch_next();
		start_brace ();
		switch (curi->size) {
		case sz_byte: printf ("\tuae_u32 val = (uae_u8)data;\n"); break;
		case sz_word: printf ("\tuae_u32 val = (uae_u16)data;\n"); break;
		case sz_long: printf ("\tuae_u32 val = data;\n"); break;
		default: abort ();
		}
		printf ("\tint ccnt = cnt & 63;\n");
		printf ("\tcnt &= 63;\n");
		printf ("\tCLEAR_CZNV ();\n");
		printf ("\tif (cnt >= %d) {\n", bit_size (curi->size));
		printf ("\t\tSET_CFLG (cnt == %d ? val & 1 : 0);\n", bit_size (curi->size));
		duplicate_carry (1);
		printf ("\t\tval = 0;\n");
		if (source_is_imm1_8 (curi))
			printf ("\t} else {\n");
		else {
			if (using_ce020 == 2){
				no_return_cycles = true;
				printf ("\t\tregs.ce030_instr_cycles = table_falcon_cycles_LSD[1];\n");
			}
			printf ("\t} else if (cnt > 0) {\n");
			if (using_ce020 == 2)
				printf ("\t\tregs.ce030_instr_cycles = table_falcon_cycles_LSD[0];\n");
		}
		printf ("\t\tval <<= (cnt - 1);\n");
		printf ("\t\tSET_CFLG ((val & %s) >> %d);\n", cmask (curi->size), bit_size (curi->size) - 1);
		duplicate_carry (1);
		printf ("\t\tval <<= 1;\n");
		printf ("\tval &= %s;\n", bit_mask (curi->size));
		printf ("\t}\n");
		genflags (flag_logical_noclobber, curi->size, "val", "", "");
		shift_ce (curi->dmode, curi->size);
		genastore ("val", curi->dmode, "dstreg", curi->size, "data");
		break;
	case i_ROL:
		genamode (curi->smode, "srcreg", curi->size, "cnt", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "data", 1, 0, 0);
		fill_prefetch_next ();
		start_brace ();
		switch (curi->size) {
		case sz_byte: printf ("\tuae_u32 val = (uae_u8)data;\n"); break;
		case sz_word: printf ("\tuae_u32 val = (uae_u16)data;\n"); break;
		case sz_long: printf ("\tuae_u32 val = data;\n"); break;
		default: abort ();
		}
		printf ("\tint ccnt = cnt & 63;\n");
		printf ("\tcnt &= 63;\n");
		printf ("\tCLEAR_CZNV ();\n");
		if (source_is_imm1_8 (curi))
			printf ("{");
		else
			printf ("\tif (cnt > 0) {\n");
		printf ("\tuae_u32 loval;\n");
		printf ("\tcnt &= %d;\n", bit_size (curi->size) - 1);
		printf ("\tloval = val >> (%d - cnt);\n", bit_size (curi->size));
		printf ("\tval <<= cnt;\n");
		printf ("\tval |= loval;\n");
		printf ("\tval &= %s;\n", bit_mask (curi->size));
		printf ("\tSET_CFLG (val & 1);\n");
		printf ("}\n");
		genflags (flag_logical_noclobber, curi->size, "val", "", "");
		shift_ce (curi->dmode, curi->size);
		genastore ("val", curi->dmode, "dstreg", curi->size, "data");
		break;
	case i_ROR:
		genamode (curi->smode, "srcreg", curi->size, "cnt", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "data", 1, 0, 0);
		fill_prefetch_next ();
		start_brace ();
		switch (curi->size) {
		case sz_byte: printf ("\tuae_u32 val = (uae_u8)data;\n"); break;
		case sz_word: printf ("\tuae_u32 val = (uae_u16)data;\n"); break;
		case sz_long: printf ("\tuae_u32 val = data;\n"); break;
		default: abort ();
		}
		printf ("\tint ccnt = cnt & 63;\n");
		printf ("\tcnt &= 63;\n");
		printf ("\tCLEAR_CZNV ();\n");
		if (source_is_imm1_8 (curi))
			printf ("{");
		else
			printf ("\tif (cnt > 0) {");
		printf ("\tuae_u32 hival;\n");
		printf ("\tcnt &= %d;\n", bit_size (curi->size) - 1);
		printf ("\thival = val << (%d - cnt);\n", bit_size (curi->size));
		printf ("\tval >>= cnt;\n");
		printf ("\tval |= hival;\n");
		printf ("\tval &= %s;\n", bit_mask (curi->size));
		printf ("\tSET_CFLG ((val & %s) >> %d);\n", cmask (curi->size), bit_size (curi->size) - 1);
		printf ("\t}\n");
		genflags (flag_logical_noclobber, curi->size, "val", "", "");
		shift_ce (curi->dmode, curi->size);
		genastore ("val", curi->dmode, "dstreg", curi->size, "data");
		break;
	case i_ROXL:
		genamode (curi->smode, "srcreg", curi->size, "cnt", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "data", 1, 0, 0);
		fill_prefetch_next ();
		start_brace ();
		switch (curi->size) {
		case sz_byte: printf ("\tuae_u32 val = (uae_u8)data;\n"); break;
		case sz_word: printf ("\tuae_u32 val = (uae_u16)data;\n"); break;
		case sz_long: printf ("\tuae_u32 val = data;\n"); break;
		default: abort ();
		}
		printf ("\tint ccnt = cnt & 63;\n");
		printf ("\tcnt &= 63;\n");
		printf ("\tCLEAR_CZNV ();\n");
		if (source_is_imm1_8 (curi))
			printf ("{");
		else {
			force_range_for_rox ("cnt", curi->size);
			printf ("\tif (cnt > 0) {\n");
		}
		printf ("\tcnt--;\n");
		printf ("\t{\n\tuae_u32 carry;\n");
		printf ("\tuae_u32 loval = val >> (%d - cnt);\n", bit_size (curi->size) - 1);
		printf ("\tcarry = loval & 1;\n");
		printf ("\tval = (((val << 1) | GET_XFLG ()) << cnt) | (loval >> 1);\n");
		printf ("\tSET_XFLG (carry);\n");
		printf ("\tval &= %s;\n", bit_mask (curi->size));
		printf ("\t} }\n");
		printf ("\tSET_CFLG (GET_XFLG ());\n");
		genflags (flag_logical_noclobber, curi->size, "val", "", "");
		shift_ce (curi->dmode, curi->size);
		genastore ("val", curi->dmode, "dstreg", curi->size, "data");
		break;
	case i_ROXR:
		genamode (curi->smode, "srcreg", curi->size, "cnt", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "data", 1, 0, 0);
		fill_prefetch_next ();
		start_brace ();
		switch (curi->size) {
		case sz_byte: printf ("\tuae_u32 val = (uae_u8)data;\n"); break;
		case sz_word: printf ("\tuae_u32 val = (uae_u16)data;\n"); break;
		case sz_long: printf ("\tuae_u32 val = data;\n"); break;
		default: abort ();
		}
		printf ("\tint ccnt = cnt & 63;\n");
		printf ("\tcnt &= 63;\n");
		printf ("\tCLEAR_CZNV ();\n");
		if (source_is_imm1_8 (curi))
			printf ("{");
		else {
			force_range_for_rox ("cnt", curi->size);
			printf ("\tif (cnt > 0) {\n");
		}
		printf ("\tcnt--;\n");
		printf ("\t{\n\tuae_u32 carry;\n");
		printf ("\tuae_u32 hival = (val << 1) | GET_XFLG ();\n");
		printf ("\thival <<= (%d - cnt);\n", bit_size (curi->size) - 1);
		printf ("\tval >>= cnt;\n");
		printf ("\tcarry = val & 1;\n");
		printf ("\tval >>= 1;\n");
		printf ("\tval |= hival;\n");
		printf ("\tSET_XFLG (carry);\n");
		printf ("\tval &= %s;\n", bit_mask (curi->size));
		printf ("\t} }\n");
		printf ("\tSET_CFLG (GET_XFLG ());\n");
		genflags (flag_logical_noclobber, curi->size, "val", "", "");
		shift_ce (curi->dmode, curi->size);
		genastore ("val", curi->dmode, "dstreg", curi->size, "data");
		break;
	case i_ASRW:
		genamode (curi->smode, "srcreg", curi->size, "data", 1, 0, 0);
		fill_prefetch_next ();
		start_brace ();
		switch (curi->size) {
		case sz_byte: printf ("\tuae_u32 val = (uae_u8)data;\n"); break;
		case sz_word: printf ("\tuae_u32 val = (uae_u16)data;\n"); break;
		case sz_long: printf ("\tuae_u32 val = data;\n"); break;
		default: abort ();
		}
		printf ("\tuae_u32 sign = %s & val;\n", cmask (curi->size));
		printf ("\tuae_u32 cflg = val & 1;\n");
		printf ("\tval = (val >> 1) | sign;\n");
		genflags (flag_logical, curi->size, "val", "", "");
		printf ("\tSET_CFLG (cflg);\n");
		duplicate_carry (0);
		genastore ("val", curi->smode, "srcreg", curi->size, "data");
		break;
	case i_ASLW:
		genamode (curi->smode, "srcreg", curi->size, "data", 1, 0, 0);
		fill_prefetch_next ();
		start_brace ();
		switch (curi->size) {
		case sz_byte: printf ("\tuae_u32 val = (uae_u8)data;\n"); break;
		case sz_word: printf ("\tuae_u32 val = (uae_u16)data;\n"); break;
		case sz_long: printf ("\tuae_u32 val = data;\n"); break;
		default: abort ();
		}
		printf ("\tuae_u32 sign = %s & val;\n", cmask (curi->size));
		printf ("\tuae_u32 sign2;\n");
		printf ("\tval <<= 1;\n");
		genflags (flag_logical, curi->size, "val", "", "");
		printf ("\tsign2 = %s & val;\n", cmask (curi->size));
		printf ("\tSET_CFLG (sign != 0);\n");
		duplicate_carry (0);

		printf ("\tSET_VFLG (GET_VFLG () | (sign2 != sign));\n");
		genastore ("val", curi->smode, "srcreg", curi->size, "data");
		break;
	case i_LSRW:
		genamode (curi->smode, "srcreg", curi->size, "data", 1, 0, 0);
		fill_prefetch_next ();
		start_brace ();
		switch (curi->size) {
		case sz_byte: printf ("\tuae_u32 val = (uae_u8)data;\n"); break;
		case sz_word: printf ("\tuae_u32 val = (uae_u16)data;\n"); break;
		case sz_long: printf ("\tuae_u32 val = data;\n"); break;
		default: abort ();
		}
		printf ("\tuae_u32 carry = val & 1;\n");
		printf ("\tval >>= 1;\n");
		genflags (flag_logical, curi->size, "val", "", "");
		printf ("\tSET_CFLG (carry);\n");
		duplicate_carry (0);
		genastore ("val", curi->smode, "srcreg", curi->size, "data");
		break;
	case i_LSLW:
		genamode (curi->smode, "srcreg", curi->size, "data", 1, 0, 0);
		fill_prefetch_next ();
		start_brace ();
		switch (curi->size) {
		case sz_byte: printf ("\tuae_u8 val = data;\n"); break;
		case sz_word: printf ("\tuae_u16 val = data;\n"); break;
		case sz_long: printf ("\tuae_u32 val = data;\n"); break;
		default: abort ();
		}
		printf ("\tuae_u32 carry = val & %s;\n", cmask (curi->size));
		printf ("\tval <<= 1;\n");
		genflags (flag_logical, curi->size, "val", "", "");
		printf ("\tSET_CFLG (carry >> %d);\n", bit_size (curi->size) - 1);
		duplicate_carry (0);
		genastore ("val", curi->smode, "srcreg", curi->size, "data");
		break;
	case i_ROLW:
		genamode (curi->smode, "srcreg", curi->size, "data", 1, 0, 0);
		fill_prefetch_next ();
		start_brace ();
		switch (curi->size) {
		case sz_byte: printf ("\tuae_u8 val = data;\n"); break;
		case sz_word: printf ("\tuae_u16 val = data;\n"); break;
		case sz_long: printf ("\tuae_u32 val = data;\n"); break;
		default: abort ();
		}
		printf ("\tuae_u32 carry = val & %s;\n", cmask (curi->size));
		printf ("\tval <<= 1;\n");
		printf ("\tif (carry)  val |= 1;\n");
		genflags (flag_logical, curi->size, "val", "", "");
		printf ("\tSET_CFLG (carry >> %d);\n", bit_size (curi->size) - 1);
		genastore ("val", curi->smode, "srcreg", curi->size, "data");
		break;
	case i_RORW:
		genamode (curi->smode, "srcreg", curi->size, "data", 1, 0, 0);
		fill_prefetch_next ();
		start_brace ();
		switch (curi->size) {
		case sz_byte: printf ("\tuae_u8 val = data;\n"); break;
		case sz_word: printf ("\tuae_u16 val = data;\n"); break;
		case sz_long: printf ("\tuae_u32 val = data;\n"); break;
		default: abort ();
		}
		printf ("\tuae_u32 carry = val & 1;\n");
		printf ("\tval >>= 1;\n");
		printf ("\tif (carry) val |= %s;\n", cmask (curi->size));
		genflags (flag_logical, curi->size, "val", "", "");
		printf ("\tSET_CFLG (carry);\n");
		genastore ("val", curi->smode, "srcreg", curi->size, "data");
		break;
	case i_ROXLW:
		genamode (curi->smode, "srcreg", curi->size, "data", 1, 0, 0);
		fill_prefetch_next ();
		start_brace ();
		switch (curi->size) {
		case sz_byte: printf ("\tuae_u8 val = data;\n"); break;
		case sz_word: printf ("\tuae_u16 val = data;\n"); break;
		case sz_long: printf ("\tuae_u32 val = data;\n"); break;
		default: abort ();
		}
		printf ("\tuae_u32 carry = val & %s;\n", cmask (curi->size));
		printf ("\tval <<= 1;\n");
		printf ("\tif (GET_XFLG ()) val |= 1;\n");
		genflags (flag_logical, curi->size, "val", "", "");
		printf ("\tSET_CFLG (carry >> %d);\n", bit_size (curi->size) - 1);
		duplicate_carry (0);
		genastore ("val", curi->smode, "srcreg", curi->size, "data");
		break;
	case i_ROXRW:
		genamode (curi->smode, "srcreg", curi->size, "data", 1, 0, 0);
		fill_prefetch_next ();
		start_brace ();
		switch (curi->size) {
		case sz_byte: printf ("\tuae_u8 val = data;\n"); break;
		case sz_word: printf ("\tuae_u16 val = data;\n"); break;
		case sz_long: printf ("\tuae_u32 val = data;\n"); break;
		default: abort ();
		}
		printf ("\tuae_u32 carry = val & 1;\n");
		printf ("\tval >>= 1;\n");
		printf ("\tif (GET_XFLG ()) val |= %s;\n", cmask (curi->size));
		genflags (flag_logical, curi->size, "val", "", "");
		printf ("\tSET_CFLG (carry);\n");
		duplicate_carry (0);
		genastore ("val", curi->smode, "srcreg", curi->size, "data");
		break;
	case i_MOVEC2:
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		fill_prefetch_next ();
		start_brace ();
		printf ("\tint regno = (src >> 12) & 15;\n");
		printf ("\tuae_u32 *regp = regs.regs + regno;\n");
		printf ("\tif (! m68k_movec2(src & 0xFFF, regp)) goto %s;\n", endlabelstr);
		break;
	case i_MOVE2C:
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		fill_prefetch_next ();
		start_brace ();
		printf ("\tint regno = (src >> 12) & 15;\n");
		printf ("\tuae_u32 *regp = regs.regs + regno;\n");
		printf ("\tif (! m68k_move2c(src & 0xFFF, regp)) goto %s;\n", endlabelstr);
		if (using_ce020 == 2) {
			no_return_cycles = true;
			printf ("\tif((src & 0xFFF) < 3)\n");
			printf ("\t\tregs.ce030_instr_cycles = table_falcon_cycles_MOVEC[1];\n");
			printf ("\telse\n");
			printf ("\t\tregs.ce030_instr_cycles = table_falcon_cycles_MOVEC[0];\n");
		}
		break;
	case i_CAS:
		{
			if (using_ce020 == 2) {
				no_return_cycles = true;
				if (curi->size == sz_byte) {
					instr_table = TABLE_FALCON_CYCLES_CAS_BW;
					instr_table_index = cas_b_index * 2;
					cas_b_index ++;
				}
				else if (curi->size == sz_word) {
					instr_table = TABLE_FALCON_CYCLES_CAS_BW;
					instr_table_index = cas_w_index * 2;
					cas_w_index ++;
				}
				else {
					instr_table = TABLE_FALCON_CYCLES_CAS_L;
					instr_table_index = cas_l_index * 2;
					cas_l_index ++;
				}
			}
			int old_brace_level;
			genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
			genamode (curi->dmode, "dstreg", curi->size, "dst", 1, 0, 0);
			fill_prefetch_0 ();
			start_brace ();
			printf ("\tint ru = (src >> 6) & 7;\n");
			printf ("\tint rc = src & 7;\n");
			genflags (flag_cmp, curi->size, "newv", "m68k_dreg (regs, rc)", "dst");
			printf ("\tif (GET_ZFLG ())");
			old_brace_level = n_braces;
			start_brace ();
			genastore ("(m68k_dreg (regs, ru))", curi->dmode, "dstreg", curi->size, "dst");
			if (using_ce020 == 2)
				printf ("\tregs.ce030_instr_cycles = %s[%d];\n", falcon_cycles_tables[instr_table], instr_table_index);
			pop_braces (old_brace_level);
			printf ("else");
			start_brace ();
			printf ("m68k_dreg (regs, rc) = dst;\n");
			if (using_ce020 == 2)
				printf ("\tregs.ce030_instr_cycles = %s[%d];\n", falcon_cycles_tables[instr_table], instr_table_index + 1);
			pop_braces (old_brace_level);
		}
		break;
	case i_CAS2:
		genamode (curi->smode, "srcreg", curi->size, "extra", 1, 0, 0);
		printf ("\tuae_u32 rn1 = regs.regs[(extra >> 28) & 15];\n");
		printf ("\tuae_u32 rn2 = regs.regs[(extra >> 12) & 15];\n");
		if (curi->size == sz_word) {
			int old_brace_level = n_braces;
			printf ("\tuae_u16 dst1 = %s (rn1), dst2 = %s (rn2);\n", srcw, srcw);
			genflags (flag_cmp, curi->size, "newv", "m68k_dreg (regs, (extra >> 16) & 7)", "dst1");
			printf ("\tif (GET_ZFLG ()) {\n");
			genflags (flag_cmp, curi->size, "newv", "m68k_dreg (regs, extra & 7)", "dst2");
			printf ("\tif (GET_ZFLG ()) {\n");
			printf ("\t%s (rn1, m68k_dreg (regs, (extra >> 22) & 7));\n", dstw);
			printf ("\t%s (rn1, m68k_dreg (regs, (extra >> 6) & 7));\n", dstw);
			printf ("\t}}\n");
			pop_braces (old_brace_level);
			printf ("\tif (! GET_ZFLG ()) {\n");
			printf ("\tm68k_dreg (regs, (extra >> 22) & 7) = (m68k_dreg (regs, (extra >> 22) & 7) & ~0xffff) | (dst1 & 0xffff);\n");
			printf ("\tm68k_dreg (regs, (extra >> 6) & 7) = (m68k_dreg (regs, (extra >> 6) & 7) & ~0xffff) | (dst2 & 0xffff);\n");
			printf ("\t}\n");
		} else {
			int old_brace_level = n_braces;
			printf ("\tuae_u32 dst1 = %s (rn1), dst2 = %s (rn2);\n", srcl, srcl);
			genflags (flag_cmp, curi->size, "newv", "m68k_dreg (regs, (extra >> 16) & 7)", "dst1");
			printf ("\tif (GET_ZFLG ()) {\n");
			genflags (flag_cmp, curi->size, "newv", "m68k_dreg (regs, extra & 7)", "dst2");
			printf ("\tif (GET_ZFLG ()) {\n");
			printf ("\t%s (rn1, m68k_dreg (regs, (extra >> 22) & 7));\n", dstl);
			printf ("\t%s (rn1, m68k_dreg (regs, (extra >> 6) & 7));\n", dstl);
			printf ("\t}}\n");
			pop_braces (old_brace_level);
			printf ("\tif (! GET_ZFLG ()) {\n");
			printf ("\tm68k_dreg (regs, (extra >> 22) & 7) = dst1;\n");
			printf ("\tm68k_dreg (regs, (extra >> 6) & 7) = dst2;\n");
			printf ("\t}\n");
		}
		break;
	case i_MOVES: /* ignore DFC and SFC when using_mmu == false */
		{
			int old_brace_level;
			genamode (curi->smode, "srcreg", curi->size, "extra", 1, 0, 0);
			printf ("\tif (extra & 0x800)\n");
			{
				int old_m68k_pc_offset = m68k_pc_offset;
				old_brace_level = n_braces;
				start_brace ();
				printf ("\tuae_u32 src = regs.regs[(extra >> 12) & 15];\n");
				genamode (curi->dmode, "dstreg", curi->size, "dst", 2, 0, 0);
				genastore_fc ("src", curi->dmode, "dstreg", curi->size, "dst");
				pop_braces (old_brace_level);
				m68k_pc_offset = old_m68k_pc_offset;
			}
			printf ("else");
			{
				start_brace ();
				genamode (curi->dmode, "dstreg", curi->size, "src", 1, 0, GF_FC);
				printf ("\tif (extra & 0x8000) {\n");
				switch (curi->size) {
				case sz_byte: printf ("\tm68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;\n"); break;
				case sz_word: printf ("\tm68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;\n"); break;
				case sz_long: printf ("\tm68k_areg (regs, (extra >> 12) & 7) = src;\n"); break;
				default: abort ();
				}
				printf ("\t} else {\n");
				genastore ("src", Dreg, "(extra >> 12) & 7", curi->size, "");
				printf ("\t}\n");
				sync_m68k_pc ();
				pop_braces (old_brace_level);
			}
		}
		break;
	case i_BKPT:		/* only needed for hardware emulators */
		sync_m68k_pc ();
		printf ("\top_illg (opcode);\n");
		break;
	case i_CALLM:		/* not present in 68030 */
		sync_m68k_pc ();
		printf ("\top_illg (opcode);\n");
		break;
	case i_RTM:		/* not present in 68030 */
		sync_m68k_pc ();
		printf ("\top_illg (opcode);\n");
		break;
	case i_TRAPcc:
		if (curi->smode != am_unknown && curi->smode != am_illg)
			genamode (curi->smode, "srcreg", curi->size, "dummy", 1, 0, 0);
		fill_prefetch_0 ();
		printf ("\tif (cctrue (%d)) { Exception (7, m68k_getpc () ,M68000_EXC_SRC_CPU); goto %s; }\n", curi->cc, endlabelstr);
		need_endlabel = 1;
		break;
	case i_DIVL:
		printf ("\tuaecptr oldpc = m68k_getpc ();\n");
		genamode (curi->smode, "srcreg", curi->size, "extra", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "dst", 1, 0, 0);
		sync_m68k_pc ();
		printf ("\tm68k_divl(opcode, dst, extra, oldpc);\n");
		if (using_ce020 == 2) {
			no_return_cycles = true;
			printf ("\tif (extra & 0x800)\n");
			printf ("\t\tregs.ce030_instr_cycles = table_falcon_cycles_DIVS_L[%d];\n", divl_index);
			printf ("\telse\n");
			printf ("\t\tregs.ce030_instr_cycles = table_falcon_cycles_DIVU_L[%d];\n", divl_index);
			divl_index ++;
		}
		break;
	case i_MULL:
		genamode (curi->smode, "srcreg", curi->size, "extra", 1, 0, 0);
		genamode (curi->dmode, "dstreg", curi->size, "dst", 1, 0, 0);
		sync_m68k_pc ();
		printf ("\tm68k_mull(opcode, dst, extra);\n");
		break;
	case i_BFTST:
	case i_BFEXTU:
	case i_BFCHG:
	case i_BFEXTS:
	case i_BFCLR:
	case i_BFFFO:
	case i_BFSET:
	case i_BFINS:
		{
			const char *getb, *putb;

			if (using_mmu || using_ce020) {
				getb = "x_get_bitfield";
				putb = "x_put_bitfield";
			} else {
				getb = "get_bitfield";
				putb = "put_bitfield";
			}

			genamode (curi->smode, "srcreg", curi->size, "extra", 1, 0, 0);
			genamode (curi->dmode, "dstreg", sz_long, "dst", 2, 0, 0);
			start_brace ();
			printf ("\tuae_u32 bdata[2];\n");
			printf ("\tuae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;\n");
			printf ("\tint width = (((extra & 0x20 ? m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;\n");
			if (curi->dmode == Dreg) {
				printf ("\tuae_u32 tmp = m68k_dreg(regs, dstreg);\n");
				printf ("\toffset &= 0x1f;\n");
				printf ("\ttmp = (tmp << offset) | (tmp >> (32 - offset));\n");
				printf ("\tbdata[0] = tmp & ((1 << (32 - width)) - 1);\n");
			} else {
				printf ("\tuae_u32 tmp;\n");
				printf ("\tdsta += offset >> 3;\n");
				printf ("\ttmp = %s (dsta, bdata, offset, width);\n", getb);
			}
			printf ("\tSET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);\n");
			if (curi->mnemo == i_BFEXTS)
				printf ("\ttmp = (uae_s32)tmp >> (32 - width);\n");
			else
				printf ("\ttmp >>= (32 - width);\n");
			printf ("\tSET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);\n");
			switch (curi->mnemo) {
			case i_BFTST:
				break;
			case i_BFEXTU:
			case i_BFEXTS:
				printf ("\tm68k_dreg (regs, (extra >> 12) & 7) = tmp;\n");
				break;
			case i_BFCHG:
				printf ("\ttmp = tmp ^ (0xffffffffu >> (32 - width));\n");
				break;
			case i_BFCLR:
				printf ("\ttmp = 0;\n");
				break;
			case i_BFFFO:
				printf ("\t{ uae_u32 mask = 1 << (width - 1);\n");
				printf ("\twhile (mask) { if (tmp & mask) break; mask >>= 1; offset++; }}\n");
				printf ("\tm68k_dreg (regs, (extra >> 12) & 7) = offset;\n");
				break;
			case i_BFSET:
				printf ("\ttmp = 0xffffffffu >> (32 - width);\n");
				break;
			case i_BFINS:
				printf ("\ttmp = m68k_dreg (regs, (extra >> 12) & 7);\n");
				printf ("\ttmp = tmp & (0xffffffffu >> (32 - width));\n");
				printf ("\tSET_NFLG (tmp & (1 << (width - 1)) ? 1 : 0);\n");
				printf ("\tSET_ZFLG (tmp == 0);\n");
				break;
			default:
				break;
			}
			if (curi->mnemo == i_BFCHG
				|| curi->mnemo == i_BFCLR
				|| curi->mnemo == i_BFSET
				|| curi->mnemo == i_BFINS) {
					if (curi->dmode == Dreg) {
						printf ("\ttmp = bdata[0] | (tmp << (32 - width));\n");
						printf ("\tm68k_dreg(regs, dstreg) = (tmp >> offset) | (tmp << (32 - offset));\n");
					} else {
						printf ("\t%s(dsta, bdata, tmp, offset, width);\n", putb);
					}
			}
		}
		break;
	case i_PACK:
		if (curi->smode == Dreg) {
			printf ("\tuae_u16 val = m68k_dreg (regs, srcreg) + %s;\n", gen_nextiword (0));
			printf ("\tm68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & 0xffffff00) | ((val >> 4) & 0xf0) | (val & 0xf);\n");
		} else {
			printf ("\tuae_u16 val;\n");
			printf ("\tm68k_areg (regs, srcreg) -= areg_byteinc[srcreg];\n");
			printf ("\tval = (uae_u16)%s (m68k_areg (regs, srcreg));\n", srcb);
			printf ("\tm68k_areg (regs, srcreg) -= areg_byteinc[srcreg];\n");
			printf ("\tval = (val | ((uae_u16)%s (m68k_areg (regs, srcreg)) << 8)) + %s;\n", srcb, gen_nextiword (0));
			printf ("\tm68k_areg (regs, dstreg) -= areg_byteinc[dstreg];\n");
			gen_set_fault_pc ();
			printf ("\t%s (m68k_areg (regs, dstreg),((val >> 4) & 0xf0) | (val & 0xf));\n", dstb);
		}
		break;
	case i_UNPK:
		if (curi->smode == Dreg) {
			printf ("\tuae_u16 val = m68k_dreg (regs, srcreg);\n");
			printf ("\tval = (((val << 4) & 0xf00) | (val & 0xf)) + %s;\n", gen_nextiword (0));
			printf ("\tm68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & 0xffff0000) | (val & 0xffff);\n");
		} else {
			printf ("\tuae_u16 val;\n");
			printf ("\tm68k_areg (regs, srcreg) -= areg_byteinc[srcreg];\n");
			printf ("\tval = (uae_u16)%s (m68k_areg (regs, srcreg));\n", srcb);
			printf ("\tval = (((val << 4) & 0xf00) | (val & 0xf)) + %s;\n", gen_nextiword (0));
			if (cpu_level >= 2) {
				printf ("\tm68k_areg (regs, dstreg) -= 2 * areg_byteinc[dstreg];\n");
				printf ("\t%s (m68k_areg (regs, dstreg) + areg_byteinc[dstreg], val);\n", dstb);
				printf ("\t%s (m68k_areg (regs, dstreg), val >> 8);\n", dstb);
			} else {
				printf ("\tm68k_areg (regs, dstreg) -= areg_byteinc[dstreg];\n");
				printf ("\t%s (m68k_areg (regs, dstreg),val);\n", dstb);
				printf ("\tm68k_areg (regs, dstreg) -= areg_byteinc[dstreg];\n");
				gen_set_fault_pc ();
				printf ("\t%s (m68k_areg (regs, dstreg),val >> 8);\n", dstb);
			}
		}
		break;
	case i_TAS:
		genamode (curi->smode, "srcreg", curi->size, "src", 1, 0, 0);
		genflags (flag_logical, curi->size, "src", "", "");
		if (!isreg (curi->smode))
			addcycles000 (2);
		fill_prefetch_next ();
		printf ("\tsrc |= 0x80;\n");
		if (cpu_level >= 2 || curi->smode == Dreg || !using_ce) {
			if (next_cpu_level < 2)
				next_cpu_level = 2 - 1;
			genastore ("src", curi->smode, "srcreg", curi->size, "src");
		} else {
			printf ("\tif (!is_cycle_ce ()) {\n");
			genastore ("src", curi->smode, "srcreg", curi->size, "src");
			printf ("\t} else {\n");
			printf ("\t\tdo_cycles_ce000 (4);\n");
			printf ("\t}\n");
		}
		break;
	case i_FPP:
		fpulimit();
		genamode (curi->smode, "srcreg", curi->size, "extra", 1, 0, 0);
		sync_m68k_pc ();
		printf ("\tfpuop_arithmetic(opcode, extra);\n");
		break;
	case i_FDBcc:
		fpulimit();
		genamode (curi->smode, "srcreg", curi->size, "extra", 1, 0, 0);
		sync_m68k_pc ();
		printf ("\tfpuop_dbcc (opcode, extra);\n");
		break;
	case i_FScc:
		fpulimit();
		genamode (curi->smode, "srcreg", curi->size, "extra", 1, 0, 0);
		sync_m68k_pc ();
		printf ("\tfpuop_scc (opcode, extra);\n");
		break;
	case i_FTRAPcc:
		fpulimit();
		printf ("\tuaecptr oldpc = m68k_getpc ();\n");
		printf ("\tuae_u16 extra = %s;\n", gen_nextiword (0));
		if (curi->smode != am_unknown && curi->smode != am_illg)
			genamode (curi->smode, "srcreg", curi->size, "dummy", 1, 0, 0);
		sync_m68k_pc ();
		printf ("\tfpuop_trapcc (opcode, oldpc, extra);\n");
		break;
	case i_FBcc:
		fpulimit();
		sync_m68k_pc ();
		start_brace ();
		printf ("\tuaecptr pc = m68k_getpc ();\n");
		genamode (curi->dmode, "srcreg", curi->size, "extra", 1, 0, 0);
		sync_m68k_pc ();
		printf ("\tfpuop_bcc (opcode, pc,extra);\n");
		break;
	case i_FSAVE:
		fpulimit();
		sync_m68k_pc ();
		printf ("\tfpuop_save (opcode);\n");
		break;
	case i_FRESTORE:
		fpulimit();
		sync_m68k_pc ();
		printf ("\tfpuop_restore (opcode);\n");
		break;

	case i_CINVL:
	case i_CINVP:
	case i_CINVA:
	case i_CPUSHL:
	case i_CPUSHP:
	case i_CPUSHA:
		if (using_mmu)
			printf ("\tflush_mmu%s(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);\n", mmu_postfix);
		printf ("\tif (opcode & 0x80)\n");
		printf ("\t\tflush_icache(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);\n");
		break;

	case i_MOVE16:
		{
			if ((opcode & 0xfff8) == 0xf620) {
				/* MOVE16 (Ax)+,(Ay)+ */
				printf ("\tuae_u32 v1, v2, v3, v4;\n");
				printf ("\tuaecptr mems = m68k_areg (regs, srcreg) & ~15, memd;\n");
				printf ("\tdstreg = (%s >> 12) & 7;\n", gen_nextiword (0));
				printf ("\tmemd = m68k_areg (regs, dstreg) & ~15;\n");
				printf ("\tv1 = %s (mems);\n", srcl);
				printf ("\tv2 = %s (mems + 4);\n", srcl);
				printf ("\tv3 = %s (mems + 8);\n", srcl);
				printf ("\tv4 = %s (mems + 12);\n", srcl);
				printf ("\t%s (memd , v1);\n", dstl);
				printf ("\t%s (memd + 4, v2);\n", dstl);
				printf ("\t%s (memd + 8, v3);\n", dstl);
				printf ("\t%s (memd + 12, v4);\n", dstl);
				printf ("\tif (srcreg != dstreg)\n");
				printf ("\t\tm68k_areg (regs, srcreg) += 16;\n");
				printf ("\tm68k_areg (regs, dstreg) += 16;\n");
			} else {
				/* Other variants */
				printf ("\tuae_u32 v1, v2, v3, v4;\n");
				genamode (curi->smode, "srcreg", curi->size, "mems", 0, 2, 0);
				genamode (curi->dmode, "dstreg", curi->size, "memd", 0, 2, 0);
				printf ("\tmemsa &= ~15;\n");
				printf ("\tmemda &= ~15;\n");
				printf ("\tv1 = %s (memsa);\n", srcl);
				printf ("\tv2 = %s (memsa + 4);\n", srcl);
				printf ("\tv3 = %s (memsa + 8);\n", srcl);
				printf ("\tv4 = %s (memsa + 12);\n", srcl);
				printf ("\t%s (memda , v1);\n", dstl);
				printf ("\t%s (memda + 4, v2);\n", dstl);
				printf ("\t%s (memda + 8, v3);\n", dstl);
				printf ("\t%s (memda + 12, v4);\n", dstl);
				if ((opcode & 0xfff8) == 0xf600)
					printf ("\tm68k_areg (regs, srcreg) += 16;\n");
				else if ((opcode & 0xfff8) == 0xf608)
					printf ("\tm68k_areg (regs, dstreg) += 16;\n");
			}
		}
		break;

	case i_PFLUSHN:
	case i_PFLUSH:
	case i_PFLUSHAN:
	case i_PFLUSHA:
	case i_PLPAR:
	case i_PLPAW:
	case i_PTESTR:
	case i_PTESTW:
		sync_m68k_pc ();
		printf ("\tmmu_op (opcode, 0);\n");
		break;
	case i_MMUOP030:
		printf ("\tuaecptr pc = m68k_getpc ();\n");
		printf ("\tuae_u16 extra = x_get_word (pc + 2);\n");
		m68k_pc_offset += 2;
		sync_m68k_pc ();
		if (curi->smode == Areg || curi->smode == Dreg)
			printf("\tuae_u16 extraa = 0;\n");
		else
			genamode (curi->smode, "srcreg", curi->size, "extra", 0, 0, 0);
		sync_m68k_pc ();
		printf ("\tmmu_op30 (pc, opcode, extra, extraa);\n");
		break;
	default:
		abort ();
		break;
	}
	finish_braces ();
	if (limit_braces) {
		printf ("\n#endif\n");
		n_braces = limit_braces;
		limit_braces = 0;
		finish_braces ();
	}
	if (did_prefetch >= 0)
		fill_prefetch_finish ();
	if (!count_cycles)
		addcycles_ce020 (0, 0, 2, 2);
	sync_m68k_pc ();
	did_prefetch = 0;
}

static void generate_includes (FILE * f)
{
	fprintf (f, "#include \"main.h\"\n");
	fprintf (f, "#include \"sysdeps.h\"\n");
	fprintf (f, "#include \"hatari-glue.h\"\n");
	fprintf (f, "#include \"maccess.h\"\n");
	fprintf (f, "#include \"memory.h\"\n");
	fprintf (f, "#include \"custom.h\"\n");
	fprintf (f, "#include \"newcpu.h\"\n");
	fprintf (f, "#include \"cpu_prefetch.h\"\n");
	fprintf (f, "#include \"cputbl.h\"\n");

	fprintf (f, "#define CPUFUNC(x) x##_ff\n"
		"#define SET_CFLG_ALWAYS(x) SET_CFLG(x)\n"
		"#define SET_NFLG_ALWAYS(x) SET_NFLG(x)\n"
		"#ifdef NOFLAGS\n"
		"#include \"noflags.h\"\n"
		"#endif\n");
}

static int postfix;


static char *decodeEA (amodes mode, wordsizes size)
{
	static char buffer[80];

	buffer[0] = 0;
	switch (mode){
	case Dreg:
		strcpy (buffer,"Dn");
		break;
	case Areg:
		strcpy (buffer,"An");
		break;
	case Aind:
		strcpy (buffer,"(An)");
		break;
	case Aipi:
		strcpy (buffer,"(An)+");
		break;
	case Apdi:
		strcpy (buffer,"-(An)");
		break;
	case Ad16:
		strcpy (buffer,"(d16,An)");
		break;
	case Ad8r:
		strcpy (buffer,"(d8,An,Xn)");
		break;
	case PC16:
		strcpy (buffer,"(d16,PC)");
		break;
	case PC8r:
		strcpy (buffer,"(d8,PC,Xn)");
		break;
	case absw:
		strcpy (buffer,"(xxx).W");
		break;
	case absl:
		strcpy (buffer,"(xxx).L");
		break;
	case imm:
		switch (size){
		case sz_byte:
			strcpy (buffer,"#<data>.B");
			break;
		case sz_word:
			strcpy (buffer,"#<data>.W");
			break;
		case sz_long:
			strcpy (buffer,"#<data>.L");
			break;
		default:
			break;
		}
		break;
	case imm0:
		strcpy (buffer,"#<data>.B");
		break;
	case imm1:
		strcpy (buffer,"#<data>.W");
		break;
	case imm2:
		strcpy (buffer,"#<data>.L");
		break;
	case immi:
		strcpy (buffer,"#<data>");
		break;

	default:
		break;
	}
	return buffer;
}

static char *outopcode (int opcode)
{
	static char out[100];
	struct instr *ins;
	int i;

	ins = &table68k[opcode];
	for (i = 0; lookuptab[i].name[0]; i++) {
		if (ins->mnemo == lookuptab[i].mnemo)
			break;
	}
	{
//		char *s = ua (lookuptab[i].name);
		const char *s = lookuptab[i].name;
		strcpy (out, s);
		//xfree (s);
	}
	if (ins->smode == immi)
		strcat (out, "Q");
	if (ins->size == sz_byte)
		strcat (out,".B");
	if (ins->size == sz_word)
		strcat (out,".W");
	if (ins->size == sz_long)
		strcat (out,".L");
	strcat (out," ");
	if (ins->suse)
		strcat (out, decodeEA (ins->smode, ins->size));
	if (ins->duse) {
		if (ins->suse) strcat (out,",");
		strcat (out, decodeEA (ins->dmode, ins->size));
	}
	return out;
}

static void generate_one_opcode (int rp)
{
	int idx;
	uae_u16 smsk, dmsk;
	long int opcode = opcode_map[rp];
	int i68000 = table68k[opcode].clev > 0;

	if (table68k[opcode].mnemo == i_ILLG
		|| table68k[opcode].clev > cpu_level)
		return;

	for (idx = 0; lookuptab[idx].name[0]; idx++) {
		if (table68k[opcode].mnemo == lookuptab[idx].mnemo)
			break;
	}

	if (table68k[opcode].handler != -1)
		return;

	if (opcode_next_clev[rp] != cpu_level) {
		//char *name = ua (lookuptab[idx].name);
		const char *name = lookuptab[idx].name;
		if (generate_stbl)
			fprintf (stblfile, "{ %sCPUFUNC(op_%04lx_%d), %ld }, /* %s */\n",
			(using_ce || using_ce020) ? "(cpuop_func*)" : "",
			opcode, opcode_last_postfix[rp],
			opcode, name);
		//xfree (name);
		return;
	}
	fprintf (headerfile, "extern %s op_%04lx_%d_nf;\n",
		(using_ce || using_ce020) ? "cpuop_func_ce" : "cpuop_func", opcode, postfix);
	fprintf (headerfile, "extern %s op_%04lx_%d_ff;\n",
		(using_ce || using_ce020) ? "cpuop_func_ce" : "cpuop_func", opcode, postfix);
	printf ("/* %s */\n", outopcode (opcode));
	if (i68000)
		printf("#ifndef CPUEMU_68000_ONLY\n");
	printf ("%s REGPARAM2 CPUFUNC(op_%04lx_%d)(uae_u32 opcode)\n{\n", (using_ce || using_ce020) ? "void" : "unsigned long", opcode, postfix);

	switch (table68k[opcode].stype) {
	case 0: smsk = 7; break;
	case 1: smsk = 255; break;
	case 2: smsk = 15; break;
	case 3: smsk = 7; break;
	case 4: smsk = 7; break;
	case 5: smsk = 63; break;
	case 7: smsk = 3; break;
	default: abort ();
	}
	dmsk = 7;

	next_cpu_level = -1;
	if (table68k[opcode].suse
		&& table68k[opcode].smode != imm && table68k[opcode].smode != imm0
		&& table68k[opcode].smode != imm1 && table68k[opcode].smode != imm2
		&& table68k[opcode].smode != absw && table68k[opcode].smode != absl
		&& table68k[opcode].smode != PC8r && table68k[opcode].smode != PC16)
	{
		if (table68k[opcode].spos == -1) {
			if (((int) table68k[opcode].sreg) >= 128)
				printf ("\tuae_u32 srcreg = (uae_s32)(uae_s8)%d;\n", (int) table68k[opcode].sreg);
			else
				printf ("\tuae_u32 srcreg = %d;\n", (int) table68k[opcode].sreg);
		} else {
			char source[100];
			int pos = table68k[opcode].spos;

			if (pos)
				sprintf (source, "((opcode >> %d) & %d)", pos, smsk);
			else
				sprintf (source, "(opcode & %d)", smsk);

			if (table68k[opcode].stype == 3)
				printf ("\tuae_u32 srcreg = imm8_table[%s];\n", source);
			else if (table68k[opcode].stype == 1)
				printf ("\tuae_u32 srcreg = (uae_s32)(uae_s8)%s;\n", source);
			else
				printf ("\tuae_u32 srcreg = %s;\n", source);
		}
	}
	if (table68k[opcode].duse
		/* Yes, the dmode can be imm, in case of LINK or DBcc */
		&& table68k[opcode].dmode != imm && table68k[opcode].dmode != imm0
		&& table68k[opcode].dmode != imm1 && table68k[opcode].dmode != imm2
		&& table68k[opcode].dmode != absw && table68k[opcode].dmode != absl)
	{
		if (table68k[opcode].dpos == -1) {
			if (((int) table68k[opcode].dreg) >= 128)
				printf ("\tuae_u32 dstreg = (uae_s32)(uae_s8)%d;\n", (int) table68k[opcode].dreg);
			else
				printf ("\tuae_u32 dstreg = %d;\n", (int) table68k[opcode].dreg);
		} else {
			int pos = table68k[opcode].dpos;
			if (pos)
				printf ("\tuae_u32 dstreg = (opcode >> %d) & %d;\n",
				pos, dmsk);
			else
				printf ("\tuae_u32 dstreg = opcode & %d;\n", dmsk);
		}
	}
	need_endlabel = 0;
	endlabelno++;
	sprintf (endlabelstr, "endlabel%d", endlabelno);
	count_read = count_write = count_ncycles = count_cycles = 0;
	count_read_ea = count_write_ea = count_cycles_ea = 0;
	gen_opcode (opcode);
	if (need_endlabel)
		printf ("%s: ;\n", endlabelstr);

	returncycles ("", insn_n_cycles);

	printf ("}");
	if (using_ce || using_prefetch) {
		if (count_read + count_write + count_cycles == 0)
			count_cycles = 4;
		printf (" /* %d%s (%d/%d)",
			(count_read + count_write) * 4 + count_cycles, count_ncycles ? "+" : "", count_read, count_write);
		printf (" */\n");
	} else {
		printf("\n");
	}
	printf ("\n");
	if (i68000)
		printf("#endif\n");
	opcode_next_clev[rp] = next_cpu_level;
	opcode_last_postfix[rp] = postfix;

	/* Hatari only : Now patch in the instruction cycles at the beginning of the function: */
	if (!using_ce020) {
		fseek(stdout, nCurInstrCycPos, SEEK_SET);
		printf("%d;", insn_n_cycles);
		fseek(stdout, 0, SEEK_END);
	}

	/* Hatari only : inc instruction index for cycle exact 68030 cpu */
	if (using_ce020 == 2)
		instr_index += 1;

	if (generate_stbl) {
//		char *name = ua (lookuptab[idx].name);
		const char *name = lookuptab[idx].name;
		if (i68000)
			fprintf (stblfile, "#ifndef CPUEMU_68000_ONLY\n");
		fprintf (stblfile, "{ %sCPUFUNC(op_%04lx_%d), %ld }, /* %s */\n",
			(using_ce || using_ce020) ? "(cpuop_func*)" : "",
			opcode, postfix, opcode, name);
		if (i68000)
			fprintf (stblfile, "#endif\n");
		//xfree (name);
	}
}

static void generate_func (void)
{
	int j, rp;

	/* sam: this is for people with low memory (eg. me :)) */
	printf ("\n"
		"#if !defined(PART_1) && !defined(PART_2) && "
		"!defined(PART_3) && !defined(PART_4) && "
		"!defined(PART_5) && !defined(PART_6) && "
		"!defined(PART_7) && !defined(PART_8)"
		"\n"
		"#define PART_1 1\n"
		"#define PART_2 1\n"
		"#define PART_3 1\n"
		"#define PART_4 1\n"
		"#define PART_5 1\n"
		"#define PART_6 1\n"
		"#define PART_7 1\n"
		"#define PART_8 1\n"
		"#endif\n\n");

	rp = 0;
	for(j = 1; j <= 8; ++j) {
		int k = (j * nr_cpuop_funcs) / 8;
		printf ("#ifdef PART_%d\n",j);
		for (; rp < k; rp++)
			generate_one_opcode (rp);
		printf ("#endif\n\n");
	}

	if (generate_stbl)
		fprintf (stblfile, "{ 0, 0 }};\n");
}

int main (int argc, char **argv)
{
	int i, rp, postfix2;
	char fname[100];

	read_table68k ();
	do_merges ();

	opcode_map =  xmalloc (int, nr_cpuop_funcs);
	opcode_last_postfix = xmalloc (int, nr_cpuop_funcs);
	opcode_next_clev = xmalloc (int, nr_cpuop_funcs);
	counts = xmalloc (unsigned long, 65536);
	read_counts ();

	/* It would be a lot nicer to put all in one file (we'd also get rid of
	* cputbl.h that way), but cpuopti can't cope.  That could be fixed, but
	* I don't dare to touch the 68k version.  */

	headerfile = fopen ("cputbl.h", "wb");

	stblfile = fopen ("cpustbl.c", "wb");
	generate_includes (stblfile);

	using_prefetch = 0;
	using_indirect = 0;
	using_exception_3 = 1;
	using_ce = 0;

	postfix2 = -1;
	for (i = 0; i <= 32; i++) {
		postfix = i;
		if ((i >= 6 && i < 11) || (i > 12 && i < 20) || (i > 23 && i < 31))
			continue;
		generate_stbl = 1;
		if (i == 0 || i == 11 || i == 12 || i == 20 || i == 21
		    || i == 31 || i == 32) {
			if (generate_stbl)
				fprintf (stblfile, "#ifdef CPUEMU_%d\n", postfix);
			postfix2 = postfix;
			sprintf (fname, "cpuemu_%d.c", postfix);
			if (freopen (fname, "wb", stdout) == NULL) {
				perror(fname);
				return -1;
			}
			generate_includes (stdout);
		}
		using_mmu = 0;
		using_prefetch = 0;
		using_ce = 0;
		using_ce020 = 0;
		using_mmu = 0;
		cpu_level = 5 - i;
		if (i == 11 || i == 12) {
			cpu_level = 0;
			using_prefetch = 1;
			using_exception_3 = 1;
			if (i == 12)
				using_ce = 1;
			for (rp = 0; rp < nr_cpuop_funcs; rp++)
				opcode_next_clev[rp] = 0;
		} else if (i == 20) {
			cpu_level = 2;
			using_ce020 = 1;
			read_counts ();
			for (rp = 0; rp < nr_cpuop_funcs; rp++)
				opcode_next_clev[rp] = cpu_level;
		} else if (i == 21 || i == 22 || i == 23) {
			cpu_level = 3 + (23 - i);
			using_ce020 = 2;
			if (i == 21) {
				read_counts ();
				for (rp = 0; rp < nr_cpuop_funcs; rp++)
					opcode_next_clev[rp] = cpu_level;
			}
		} else if (i >= 31 && i < 40) {
			cpu_level = 3 + 32 - i;
			using_mmu = 1;
			if (cpu_level == 3) {
				fprintf (stdout, "#include \"cpummu030.h\"\n");
				mmu_postfix = "030";
			}
			else {
				fprintf (stdout, "#include \"cpummu.h\"\n");
				mmu_postfix = "";
			}

			if (i == 31)
				read_counts ();
			for (rp = 0; rp < nr_cpuop_funcs; rp++)
				opcode_next_clev[rp] = cpu_level;
		}

		if (generate_stbl) {
			if ((i > 0 && i < 10) || (i >= 20))
				fprintf (stblfile, "#ifndef CPUEMU_68000_ONLY\n");
			fprintf (stblfile, "const struct cputbl CPUFUNC(op_smalltbl_%d)[] = {\n", postfix);
		}

		generate_func ();
		if (generate_stbl) {
			if ((i > 0 && i < 10) || (i >= 20))
				fprintf (stblfile, "#endif /* CPUEMU_68000_ONLY */\n");
			if (postfix2 >= 0)
				fprintf (stblfile, "#endif /* CPUEMU_%d */\n", postfix2);
		}
		postfix2 = -1;
	}

	free (table68k);
	return 0;
}

void write_log (const TCHAR *format,...)
{
}
