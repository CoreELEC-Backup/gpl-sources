#include "sysconfig.h"
#include "sysdeps.h"
#include "options.h"
#include "memory_uae.h"
#include "custom.h"
#include "events.h"
#include "newcpu.h"
#include "machdep/m68kops.h"
#include "cpu_prefetch.h"
#include "cputbl.h"
#include "cpummu.h"
#define CPUFUNC(x) x##_ff
#define SET_CFLG_ALWAYS(x) SET_CFLG(x)
#define SET_NFLG_ALWAYS(x) SET_NFLG(x)
#ifdef NOFLAGS
#include "noflags.h"
#endif

#if !defined(PART_1) && !defined(PART_2) && !defined(PART_3) && !defined(PART_4) && !defined(PART_5) && !defined(PART_6) && !defined(PART_7) && !defined(PART_8)
#define PART_1 1
#define PART_2 1
#define PART_3 1
#define PART_4 1
#define PART_5 1
#define PART_6 1
#define PART_7 1
#define PART_8 1
#endif

#ifdef PART_1
/* OR.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0000_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0010_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0018_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0020_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 22 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0028_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0030_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0038_31)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0039_31)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* ORSR.B #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_003c_31)(uae_u32 opcode)
{
{	MakeSR ();
{	uae_s16 src = get_iword_mmu040 (2);
	src &= 0xFF;
	regs.sr |= src;
	MakeFromSR ();
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0040_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0050_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0058_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0060_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 18 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0068_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0070_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0078_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0079_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* ORSR.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_007c_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel10885; }
{	MakeSR ();
{	uae_s16 src = get_iword_mmu040 (2);
	regs.sr |= src;
	MakeFromSR ();
}}}	m68k_incpci (4);
endlabel10885: ;
return 8 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0080_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0090_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0098_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_00a0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 30 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_00a8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_00b0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 32 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_00b8_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_00b9_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (10);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 36 * CYCLE_UNIT / 2;
}

/* CHK2.B #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00d0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu040 (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu040 (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel10894; }
}
}}}	m68k_incpci (4);
endlabel10894: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.B #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00e8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu040 (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu040 (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel10895; }
}
}}}	m68k_incpci (6);
endlabel10895: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.B #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00f0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu040 (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu040 (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel10896; }
}
}}}}endlabel10896: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.B #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00f8_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu040 (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu040 (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel10897; }
}
}}}	m68k_incpci (6);
endlabel10897: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.B #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00f9_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu040 (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu040 (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel10898; }
}
}}}	m68k_incpci (8);
endlabel10898: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.B #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00fa_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu040 (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu040 (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel10899; }
}
}}}	m68k_incpci (6);
endlabel10899: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.B #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00fb_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu040 (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu040 (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel10900; }
}
}}}}endlabel10900: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BTST.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0100_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* MVPMR.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0108_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr memp = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_u16 val = (get_byte_mmu040 (memp) << 8) + get_byte_mmu040 (memp + 2);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0110_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0118_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0120_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0128_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0130_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0138_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0139_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_013a_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_getpc () + 2;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_013b_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,#<data>.B */
uae_u32 REGPARAM2 CPUFUNC(op_013c_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = get_ibyte_mmu040 (2);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* BCHG.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0140_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* MVPMR.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0148_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr memp = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_u32 val = (get_byte_mmu040 (memp) << 24) + (get_byte_mmu040 (memp + 2) << 16)
              + (get_byte_mmu040 (memp + 4) << 8) + get_byte_mmu040 (memp + 6);
	m68k_dreg (regs, dstreg) = (val);
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0150_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0158_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, dst);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0160_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, dst);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0168_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0170_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0178_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0179_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_017a_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_getpc () + 2;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_017b_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCLR.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0180_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* MVPRM.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0188_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	uaecptr memp = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	put_byte_mmu040 (memp, src >> 8);
	put_byte_mmu040 (memp + 2, src);
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0190_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0198_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, dst);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_01a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, dst);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_01a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_01b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_01b8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_01b9_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_01ba_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_getpc () + 2;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_01bb_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BSET.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_01c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* MVPRM.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_01c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
	uaecptr memp = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	put_byte_mmu040 (memp, src >> 24);
	put_byte_mmu040 (memp + 2, src >> 16);
	put_byte_mmu040 (memp + 4, src >> 8);
	put_byte_mmu040 (memp + 6, src);
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_01d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_01d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, dst);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_01e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, dst);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_01e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_01f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_01f8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_01f9_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_01fa_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_getpc () + 2;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_01fb_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0200_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0210_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0218_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0220_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 22 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0228_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0230_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0238_31)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0239_31)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* ANDSR.B #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_023c_31)(uae_u32 opcode)
{
{	MakeSR ();
{	uae_s16 src = get_iword_mmu040 (2);
	src |= 0xFF00;
	regs.sr &= src;
	MakeFromSR ();
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0240_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0250_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0258_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0260_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 18 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0268_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0270_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0278_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0279_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* ANDSR.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_027c_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel10963; }
{	MakeSR ();
{	uae_s16 src = get_iword_mmu040 (2);
	regs.sr &= src;
	MakeFromSR ();
}}}	m68k_incpci (4);
endlabel10963: ;
return 8 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0280_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0290_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0298_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_02a0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 30 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_02a8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_02b0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 32 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_02b8_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_02b9_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (10);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 36 * CYCLE_UNIT / 2;
}

/* CHK2.W #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02d0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu040 (dsta); upper = (uae_s32)(uae_s16)get_word_mmu040 (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel10972; }
}
}}}	m68k_incpci (4);
endlabel10972: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.W #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02e8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu040 (dsta); upper = (uae_s32)(uae_s16)get_word_mmu040 (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel10973; }
}
}}}	m68k_incpci (6);
endlabel10973: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.W #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02f0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu040 (dsta); upper = (uae_s32)(uae_s16)get_word_mmu040 (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel10974; }
}
}}}}endlabel10974: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.W #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02f8_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu040 (dsta); upper = (uae_s32)(uae_s16)get_word_mmu040 (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel10975; }
}
}}}	m68k_incpci (6);
endlabel10975: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.W #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02f9_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu040 (dsta); upper = (uae_s32)(uae_s16)get_word_mmu040 (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel10976; }
}
}}}	m68k_incpci (8);
endlabel10976: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.W #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02fa_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu040 (dsta); upper = (uae_s32)(uae_s16)get_word_mmu040 (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel10977; }
}
}}}	m68k_incpci (6);
endlabel10977: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.W #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02fb_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu040 (dsta); upper = (uae_s32)(uae_s16)get_word_mmu040 (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel10978; }
}
}}}}endlabel10978: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* SUB.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0400_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0410_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0418_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0420_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 22 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0428_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0430_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0438_31)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0439_31)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 28 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0440_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0450_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0458_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0460_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, newv);
}}}}}}return 18 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0468_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0470_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0478_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0479_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0480_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0490_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 28 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0498_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, newv);
}}}}}}return 28 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_04a0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, newv);
}}}}}}return 30 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_04a8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 32 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_04b0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}}return 32 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_04b8_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 32 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_04b9_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (10);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 36 * CYCLE_UNIT / 2;
}

/* CHK2.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04d0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu040 (dsta); upper = get_long_mmu040 (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel11003; }
}
}}}	m68k_incpci (4);
endlabel11003: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04e8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu040 (dsta); upper = get_long_mmu040 (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel11004; }
}
}}}	m68k_incpci (6);
endlabel11004: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04f0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu040 (dsta); upper = get_long_mmu040 (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel11005; }
}
}}}}endlabel11005: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04f8_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu040 (dsta); upper = get_long_mmu040 (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel11006; }
}
}}}	m68k_incpci (6);
endlabel11006: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04f9_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu040 (dsta); upper = get_long_mmu040 (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel11007; }
}
}}}	m68k_incpci (8);
endlabel11007: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04fa_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu040 (dsta); upper = get_long_mmu040 (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel11008; }
}
}}}	m68k_incpci (6);
endlabel11008: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04fb_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu040 (dsta); upper = get_long_mmu040 (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel11009; }
}
}}}}endlabel11009: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* ADD.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0600_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0610_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0618_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0620_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 22 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0628_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0630_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0638_31)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0639_31)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 28 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0640_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0650_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0658_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0660_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, newv);
}}}}}}return 18 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0668_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0670_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0678_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0679_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0680_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0690_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 28 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0698_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, newv);
}}}}}}return 28 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_06a0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, newv);
}}}}}}return 30 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_06a8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 32 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_06b0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}}return 32 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_06b8_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 32 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_06b9_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (10);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 36 * CYCLE_UNIT / 2;
}

/* RTM.L Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* RTM.L An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06f8_31)(uae_u32 opcode)
{
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06f9_31)(uae_u32 opcode)
{
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06fa_31)(uae_u32 opcode)
{
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06fb_31)(uae_u32 opcode)
{
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* BTST.L #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0800_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0810_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0818_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0820_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0828_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0830_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0838_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0839_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (8);
return 20 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_083a_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_083b_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,#<data>.B */
uae_u32 REGPARAM2 CPUFUNC(op_083c_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s8 dst = get_ibyte_mmu040 (4);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* BCHG.L #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0840_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0850_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0858_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0860_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, dst);
}}}}return 18 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0868_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0870_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0878_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0879_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_087a_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_087b_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* BCLR.L #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0880_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0890_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0898_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_08a0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, dst);
}}}}return 18 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_08a8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_08b0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_08b8_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_08b9_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_08ba_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_08bb_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* BSET.L #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_08c0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_08d0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_08d8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_08e0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, dst);
}}}}return 18 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_08e8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_08f0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_08f8_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_08f9_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_08fa_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_08fb_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0a00_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a10_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0a18_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a20_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 22 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a28_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0a30_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0a38_31)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0a39_31)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* EORSR.B #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_0a3c_31)(uae_u32 opcode)
{
{	MakeSR ();
{	uae_s16 src = get_iword_mmu040 (2);
	src &= 0xFF;
	regs.sr ^= src;
	MakeFromSR ();
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0a40_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a50_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0a58_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a60_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 18 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a68_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0a70_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0a78_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0a79_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* EORSR.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_0a7c_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11101; }
{	MakeSR ();
{	uae_s16 src = get_iword_mmu040 (2);
	regs.sr ^= src;
	MakeFromSR ();
}}}	m68k_incpci (4);
endlabel11101: ;
return 8 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0a80_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif

#ifdef PART_2
/* EOR.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a90_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0a98_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0aa0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 30 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0aa8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0ab0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 32 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0ab8_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0ab9_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (10);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 36 * CYCLE_UNIT / 2;
}

/* CAS.B #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ad0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_lrmw_byte_mmu040 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_byte_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* CAS.B #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ad8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_lrmw_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_byte_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* CAS.B #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ae0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_lrmw_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_byte_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}return 22 * CYCLE_UNIT / 2;
}

#endif
/* CAS.B #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ae8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_lrmw_byte_mmu040 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_byte_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

#endif
/* CAS.B #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0af0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_lrmw_byte_mmu040 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_byte_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

#endif
/* CAS.B #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0af8_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_lrmw_byte_mmu040 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_byte_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

#endif
/* CAS.B #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0af9_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s8 dst = get_lrmw_byte_mmu040 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_byte_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}return 28 * CYCLE_UNIT / 2;
}

#endif
/* CMP.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0c00_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c10_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0c18_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c20_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c28_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0c30_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0c38_31)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0c39_31)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0c3a_31)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

#endif
/* CMP.B #<data>.B,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0c3b_31)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* CMP.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0c40_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c50_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0c58_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c60_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c68_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0c70_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu040 (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0c78_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0c79_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (8);
return 20 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0c7a_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s16 dst = get_word_mmu040 (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CMP.W #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0c7b_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 dst = get_word_mmu040 (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* CMP.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0c80_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c90_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0c98_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0ca0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0ca8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0cb0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu040 (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0cb8_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0cb9_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (10);
return 28 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cba_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 6;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (6);
{	uae_s32 dst = get_long_mmu040 (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

#endif
/* CMP.L #<data>.L,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cbb_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (6);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 dst = get_long_mmu040 (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 24 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cd0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_lrmw_word_mmu040 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	if (GET_ZFLG ()){
		put_lrmw_word_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_word_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cd8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_lrmw_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	if (GET_ZFLG ()){
		put_lrmw_word_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_word_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ce0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_lrmw_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	if (GET_ZFLG ()){
		put_lrmw_word_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_word_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}return 22 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ce8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s16 dst = get_lrmw_word_mmu040 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	if (GET_ZFLG ()){
		put_lrmw_word_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_word_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cf0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_lrmw_word_mmu040 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	if (GET_ZFLG ()){
		put_lrmw_word_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_word_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cf8_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s16 dst = get_lrmw_word_mmu040 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	if (GET_ZFLG ()){
		put_lrmw_word_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_word_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cf9_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s16 dst = get_lrmw_word_mmu040 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	if (GET_ZFLG ()){
		put_lrmw_word_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_word_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}return 28 * CYCLE_UNIT / 2;
}

#endif
/* CAS2.W #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cfc_31)(uae_u32 opcode)
{
{{	uae_s32 extra;
	extra = get_ilong_mmu040 (2);
	uae_u32 rn1 = regs.regs[(extra >> 28) & 15];
	uae_u32 rn2 = regs.regs[(extra >> 12) & 15];
	uae_u16 dst1 = get_lrmw_word_mmu040 (rn1), dst2 = get_lrmw_word_mmu040 (rn2);
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, (extra >> 16) & 7)), (uae_s16)(dst1));
	if (GET_ZFLG ()) {
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, extra & 7)), (uae_s16)(dst2));
	if (GET_ZFLG ()) {
	put_lrmw_word_mmu040 (rn1, m68k_dreg (regs, (extra >> 22) & 7));
	put_lrmw_word_mmu040 (rn2, m68k_dreg (regs, (extra >> 6) & 7));
	}}
	if (! GET_ZFLG ()) {
	m68k_dreg (regs, (extra >> 6) & 7) = (m68k_dreg (regs, (extra >> 6) & 7) & ~0xffff) | (dst2 & 0xffff);
	m68k_dreg (regs, (extra >> 22) & 7) = (m68k_dreg (regs, (extra >> 22) & 7) & ~0xffff) | (dst1 & 0xffff);
	}
}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e10_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel11155; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	dfc040_put_byte (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_s8 src = sfc040_get_byte (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
	m68k_incpci (4);
}}}}}}endlabel11155: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e18_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel11156; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	dfc040_put_byte (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_s8 src = sfc040_get_byte (srca);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
	m68k_incpci (4);
}}}}}}endlabel11156: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e20_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel11157; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	dfc040_put_byte (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 src = sfc040_get_byte (srca);
	m68k_areg (regs, dstreg) = srca;
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
	m68k_incpci (4);
}}}}}}endlabel11157: ;
return 20 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e28_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel11158; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	dfc040_put_byte (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 src = sfc040_get_byte (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
	m68k_incpci (6);
}}}}}}endlabel11158: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e30_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel11159; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	dfc040_put_byte (dsta, src);
}}}else{{	uaecptr srca;
	m68k_incpci (4);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 1);
{	uae_s8 src = sfc040_get_byte (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
}}}}}}}endlabel11159: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e38_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11160; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	dfc040_put_byte (dsta, src);
}}else{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s8 src = sfc040_get_byte (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
	m68k_incpci (6);
}}}}}}endlabel11160: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e39_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11161; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	dfc040_put_byte (dsta, src);
}}else{{	uaecptr srca;
	srca = get_ilong_mmu040 (4);
{	uae_s8 src = sfc040_get_byte (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
	m68k_incpci (8);
}}}}}}endlabel11161: ;
return 32 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e50_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel11162; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	dfc040_put_word (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_s16 src = sfc040_get_word (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
	m68k_incpci (4);
}}}}}}endlabel11162: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e58_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel11163; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	dfc040_put_word (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_s16 src = sfc040_get_word (srca);
	m68k_areg (regs, dstreg) += 2;
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
	m68k_incpci (4);
}}}}}}endlabel11163: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e60_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel11164; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	dfc040_put_word (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) - 2;
{	uae_s16 src = sfc040_get_word (srca);
	m68k_areg (regs, dstreg) = srca;
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
	m68k_incpci (4);
}}}}}}endlabel11164: ;
return 20 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e68_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel11165; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	dfc040_put_word (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s16 src = sfc040_get_word (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
	m68k_incpci (6);
}}}}}}endlabel11165: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e70_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel11166; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	dfc040_put_word (dsta, src);
}}}else{{	uaecptr srca;
	m68k_incpci (4);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 1);
{	uae_s16 src = sfc040_get_word (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
}}}}}}}endlabel11166: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e78_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11167; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	dfc040_put_word (dsta, src);
}}else{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s16 src = sfc040_get_word (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
	m68k_incpci (6);
}}}}}}endlabel11167: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e79_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11168; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	dfc040_put_word (dsta, src);
}}else{{	uaecptr srca;
	srca = get_ilong_mmu040 (4);
{	uae_s16 src = sfc040_get_word (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
	m68k_incpci (8);
}}}}}}endlabel11168: ;
return 32 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e90_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel11169; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	dfc040_put_long (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_s32 src = sfc040_get_long (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
	m68k_incpci (4);
}}}}}}endlabel11169: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e98_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel11170; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	dfc040_put_long (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_s32 src = sfc040_get_long (srca);
	m68k_areg (regs, dstreg) += 4;
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
	m68k_incpci (4);
}}}}}}endlabel11170: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ea0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel11171; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	dfc040_put_long (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) - 4;
{	uae_s32 src = sfc040_get_long (srca);
	m68k_areg (regs, dstreg) = srca;
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
	m68k_incpci (4);
}}}}}}endlabel11171: ;
return 28 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ea8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel11172; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	dfc040_put_long (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s32 src = sfc040_get_long (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
	m68k_incpci (6);
}}}}}}endlabel11172: ;
return 32 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0eb0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel11173; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	dfc040_put_long (dsta, src);
}}}else{{	uaecptr srca;
	m68k_incpci (4);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 1);
{	uae_s32 src = sfc040_get_long (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
}}}}}}}endlabel11173: ;
return 32 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0eb8_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11174; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	dfc040_put_long (dsta, src);
}}else{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s32 src = sfc040_get_long (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
	m68k_incpci (6);
}}}}}}endlabel11174: ;
return 32 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0eb9_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11175; }
{{	uae_s16 extra = get_iword_mmu040 (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	dfc040_put_long (dsta, src);
}}else{{	uaecptr srca;
	srca = get_ilong_mmu040 (4);
{	uae_s32 src = sfc040_get_long (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
	m68k_incpci (8);
}}}}}}endlabel11175: ;
return 40 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ed0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_lrmw_long_mmu040 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	if (GET_ZFLG ()){
		put_lrmw_long_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_long_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = dst;
}}}}}}return 32 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ed8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_lrmw_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	if (GET_ZFLG ()){
		put_lrmw_long_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_long_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = dst;
}}}}}}return 32 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ee0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_lrmw_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	if (GET_ZFLG ()){
		put_lrmw_long_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_long_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = dst;
}}}}}}return 34 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ee8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s32 dst = get_lrmw_long_mmu040 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	if (GET_ZFLG ()){
		put_lrmw_long_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_long_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = dst;
}}}}}}return 36 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ef0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_lrmw_long_mmu040 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	if (GET_ZFLG ()){
		put_lrmw_long_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_long_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = dst;
}}}}}}}return 36 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ef8_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s32 dst = get_lrmw_long_mmu040 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	if (GET_ZFLG ()){
		put_lrmw_long_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_long_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = dst;
}}}}}}return 36 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ef9_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s32 dst = get_lrmw_long_mmu040 (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	if (GET_ZFLG ()){
		put_lrmw_long_mmu040 (dsta, (m68k_dreg (regs, ru)));
	}else{
		put_lrmw_long_mmu040 (dsta, dst);
		m68k_dreg(regs, rc) = dst;
}}}}}}return 40 * CYCLE_UNIT / 2;
}

#endif
/* CAS2.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0efc_31)(uae_u32 opcode)
{
{{	uae_s32 extra;
	extra = get_ilong_mmu040 (2);
	uae_u32 rn1 = regs.regs[(extra >> 28) & 15];
	uae_u32 rn2 = regs.regs[(extra >> 12) & 15];
	uae_u32 dst1 = get_lrmw_long_mmu040 (rn1), dst2 = get_lrmw_long_mmu040 (rn2);
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, (extra >> 16) & 7)), (uae_s32)(dst1));
	if (GET_ZFLG ()) {
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, extra & 7)), (uae_s32)(dst2));
	if (GET_ZFLG ()) {
	put_lrmw_long_mmu040 (rn1, m68k_dreg (regs, (extra >> 22) & 7));
	put_lrmw_long_mmu040 (rn2, m68k_dreg (regs, (extra >> 6) & 7));
	}}
	if (! GET_ZFLG ()) {
	m68k_dreg (regs, (extra >> 6) & 7) = dst2;
	m68k_dreg (regs, (extra >> 22) & 7) = dst1;
	}
}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* MOVE.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1000_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1010_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (2);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1018_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	mmufixup[0].reg = -1;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (2);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1020_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	mmufixup[0].reg = -1;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (2);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1028_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1030_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1038_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1039_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (6);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_103a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_103b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_103c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (4);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1080_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1090_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1098_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = -1;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = -1;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10b8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10b9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10ba_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10bb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10bc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	mmufixup[0].reg = -1;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	mmufixup[0].reg = -1;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10f8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10f9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10fa_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10fb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10fc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1100_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1110_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1118_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	m68k_areg (regs, dstreg) = dsta;
	mmufixup[0].reg = -1;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1120_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	m68k_areg (regs, dstreg) = dsta;
	mmufixup[0].reg = -1;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1128_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1130_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1138_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1139_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_113a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_113b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_113c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1140_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1150_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1158_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	mmufixup[0].reg = -1;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1160_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	mmufixup[0].reg = -1;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 18 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1168_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1170_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (0);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1178_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1179_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (6);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_117a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_117b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (0);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_117c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_1180_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_1190_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_1198_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	mmufixup[0].reg = -1;
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	mmufixup[0].reg = -1;
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 18 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 1);
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11b8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11b9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11ba_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11bb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 1);
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11bc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	mmufixup[0].reg = -1;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	mmufixup[0].reg = -1;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 18 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (0);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11f8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11f9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (6);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11fa_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11fb_31)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (0);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11fc_31)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
	mmufixup[0].reg = -1;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
	mmufixup[0].reg = -1;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 22 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (0);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13f8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13f9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (6);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (10);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13fa_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13fb_31)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (0);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13fc_31)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
	optflag_testb ((uae_s8)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2000_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVE.L An,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2008_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2010_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2018_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	mmufixup[0].reg = -1;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2020_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	mmufixup[0].reg = -1;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2028_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2030_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2038_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2039_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_203a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_203b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_203c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (6);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.L Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_2040_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVEA.L An,An */
uae_u32 REGPARAM2 CPUFUNC(op_2048_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVEA.L (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_2050_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.L (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_2058_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	mmufixup[0].reg = -1;
	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.L -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_2060_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	mmufixup[0].reg = -1;
	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* MOVEA.L (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_2068_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEA.L (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_2070_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	m68k_areg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEA.L (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_2078_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEA.L (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_2079_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVEA.L (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_207a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEA.L (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_207b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	m68k_areg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEA.L #<data>.L,An */
uae_u32 REGPARAM2 CPUFUNC(op_207c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (6);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2080_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L An,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2088_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2090_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2098_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = -1;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = -1;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 22 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20b8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20b9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20ba_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20bb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20bc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L An,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	mmufixup[0].reg = -1;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	mmufixup[0].reg = -1;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 22 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20f8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20f9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20fa_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20fb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20fc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2100_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L An,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2108_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2110_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2118_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	m68k_areg (regs, dstreg) = dsta;
	mmufixup[0].reg = -1;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2120_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	m68k_areg (regs, dstreg) = dsta;
	mmufixup[0].reg = -1;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 22 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2128_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2130_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2138_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2139_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_213a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_213b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_213c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2140_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L An,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2148_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2150_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2158_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	mmufixup[0].reg = -1;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2160_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	mmufixup[0].reg = -1;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 26 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2168_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

#endif

#ifdef PART_3
/* MOVE.L (d8,An,Xn),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2170_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (0);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2178_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2179_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (6);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_217a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_217b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (0);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_217c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (6);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_2180_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L An,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_2188_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_2190_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_2198_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	mmufixup[0].reg = -1;
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	mmufixup[0].reg = -1;
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 26 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 1);
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21b8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21b9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21ba_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21bb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 1);
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21bc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L An,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	mmufixup[0].reg = -1;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	mmufixup[0].reg = -1;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 26 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (0);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21f8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21f9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (6);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21fa_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21fb_31)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (0);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21fc_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (6);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L An,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
	mmufixup[0].reg = -1;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
	mmufixup[0].reg = -1;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 30 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (0);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23f8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23f9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (6);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (10);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 36 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23fa_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23fb_31)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (0);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23fc_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (6);
	optflag_testl ((uae_s32)(src));
	m68k_incpci (10);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3000_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVE.W An,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3008_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3010_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (2);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3018_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	mmufixup[0].reg = -1;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (2);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3020_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	mmufixup[0].reg = -1;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (2);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3028_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3030_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3038_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3039_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (6);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_303a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_303b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_303c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (4);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVEA.W Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_3040_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVEA.W An,An */
uae_u32 REGPARAM2 CPUFUNC(op_3048_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVEA.W (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_3050_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (2);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVEA.W (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_3058_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	mmufixup[0].reg = -1;
	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (2);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVEA.W -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_3060_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	mmufixup[0].reg = -1;
	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (2);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* MOVEA.W (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_3068_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.W (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_3070_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.W (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_3078_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.W (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_3079_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (6);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEA.W (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_307a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.W (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_307b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.W #<data>.W,An */
uae_u32 REGPARAM2 CPUFUNC(op_307c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (4);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3080_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W An,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3088_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3090_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3098_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = -1;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = -1;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30b8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30b9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30ba_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30bb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30bc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W An,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	mmufixup[0].reg = -1;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	mmufixup[0].reg = -1;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30f8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30f9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30fa_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30fb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30fc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3100_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W An,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3108_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3110_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3118_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	m68k_areg (regs, dstreg) = dsta;
	mmufixup[0].reg = -1;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3120_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	m68k_areg (regs, dstreg) = dsta;
	mmufixup[0].reg = -1;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3128_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3130_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3138_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3139_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_313a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_313b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_313c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3140_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W An,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3148_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3150_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3158_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	mmufixup[0].reg = -1;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3160_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	mmufixup[0].reg = -1;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 18 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3168_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3170_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (0);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3178_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3179_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (6);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_317a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_317b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (0);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_317c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_3180_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W An,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_3188_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_3190_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_3198_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	mmufixup[0].reg = -1;
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	mmufixup[0].reg = -1;
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 18 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 1);
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31b8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31b9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31ba_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31bb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 1);
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31bc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W An,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	mmufixup[0].reg = -1;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	mmufixup[0].reg = -1;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 18 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (0);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31f8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31f9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (6);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31fa_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31fb_31)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (0);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31fc_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W An,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
	mmufixup[0].reg = -1;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
	mmufixup[0].reg = -1;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 22 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (0);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33f8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33f9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (6);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (10);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33fa_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33fb_31)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (0);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33fc_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
	optflag_testw ((uae_s16)(src));
	m68k_incpci (8);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}return 20 * CYCLE_UNIT / 2;
}

/* NEGX.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4000_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((newv) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* NEGX.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4010_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, newv);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* NEGX.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4018_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, newv);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* NEGX.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4020_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, newv);
}}}}}return 14 * CYCLE_UNIT / 2;
}

/* NEGX.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4028_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, newv);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEGX.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4030_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEGX.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4038_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, newv);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEGX.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4039_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, newv);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* NEGX.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4040_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((newv) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* NEGX.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4050_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, newv);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* NEGX.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4058_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (srca, newv);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* NEGX.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4060_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (srca, newv);
}}}}}return 14 * CYCLE_UNIT / 2;
}

/* NEGX.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4068_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, newv);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEGX.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4070_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEGX.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4078_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, newv);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEGX.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4079_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, newv);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* NEGX.L Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4080_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	m68k_dreg (regs, srcreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* NEGX.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4090_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, newv);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* NEGX.L (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4098_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (srca, newv);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* NEGX.L -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_40a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (srca, newv);
}}}}}return 22 * CYCLE_UNIT / 2;
}

/* NEGX.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_40a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, newv);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* NEGX.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_40b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, newv);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* NEGX.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_40b8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, newv);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* NEGX.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_40b9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, newv);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MVSR2.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_40c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel11512; }
{{	MakeSR ();
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((regs.sr) & 0xffff);
}}}	m68k_incpci (2);
endlabel11512: ;
return 4 * CYCLE_UNIT / 2;
}

/* MVSR2.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_40d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel11513; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	MakeSR ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, regs.sr);
}}}endlabel11513: ;
return 8 * CYCLE_UNIT / 2;
}

/* MVSR2.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_40d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel11514; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
	MakeSR ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (srca, regs.sr);
}}}endlabel11514: ;
return 8 * CYCLE_UNIT / 2;
}

/* MVSR2.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_40e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel11515; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	MakeSR ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (srca, regs.sr);
}}}endlabel11515: ;
return 10 * CYCLE_UNIT / 2;
}

/* MVSR2.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_40e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel11516; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	MakeSR ();
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, regs.sr);
}}}endlabel11516: ;
return 12 * CYCLE_UNIT / 2;
}

/* MVSR2.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_40f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel11517; }
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
	MakeSR ();
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, regs.sr);
}}}}endlabel11517: ;
return 12 * CYCLE_UNIT / 2;
}

/* MVSR2.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_40f8_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11518; }
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	MakeSR ();
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, regs.sr);
}}}endlabel11518: ;
return 12 * CYCLE_UNIT / 2;
}

/* MVSR2.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_40f9_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11519; }
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
	MakeSR ();
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, regs.sr);
}}}endlabel11519: ;
return 16 * CYCLE_UNIT / 2;
}

/* CHK.L Dn,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4100_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11520;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11520;
	}
}}}endlabel11520: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (An),Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4110_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11521;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11521;
	}
}}}}endlabel11521: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (An)+,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4118_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11522;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11522;
	}
}}}}endlabel11522: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L -(An),Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4120_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11523;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11523;
	}
}}}}endlabel11523: ;
return 14 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (d16,An),Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4128_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11524;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11524;
	}
}}}}endlabel11524: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (d8,An,Xn),Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4130_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11525;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11525;
	}
}}}}}endlabel11525: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (xxx).W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4138_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11526;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11526;
	}
}}}}endlabel11526: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (xxx).L,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4139_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (6);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11527;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11527;
	}
}}}}endlabel11527: ;
return 20 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (d16,PC),Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_413a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11528;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11528;
	}
}}}}endlabel11528: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (d8,PC,Xn),Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_413b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11529;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11529;
	}
}}}}}endlabel11529: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L #<data>.L,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_413c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (6);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11530;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11530;
	}
}}}endlabel11530: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4180_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11531;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11531;
	}
}}}endlabel11531: ;
return 4 * CYCLE_UNIT / 2;
}

/* CHK.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4190_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11532;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11532;
	}
}}}}endlabel11532: ;
return 8 * CYCLE_UNIT / 2;
}

/* CHK.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4198_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11533;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11533;
	}
}}}}endlabel11533: ;
return 8 * CYCLE_UNIT / 2;
}

/* CHK.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11534;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11534;
	}
}}}}endlabel11534: ;
return 10 * CYCLE_UNIT / 2;
}

/* CHK.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11535;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11535;
	}
}}}}endlabel11535: ;
return 12 * CYCLE_UNIT / 2;
}

/* CHK.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11536;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11536;
	}
}}}}}endlabel11536: ;
return 12 * CYCLE_UNIT / 2;
}

/* CHK.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41b8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11537;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11537;
	}
}}}}endlabel11537: ;
return 12 * CYCLE_UNIT / 2;
}

/* CHK.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41b9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (6);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11538;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11538;
	}
}}}}endlabel11538: ;
return 16 * CYCLE_UNIT / 2;
}

/* CHK.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41ba_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11539;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11539;
	}
}}}}endlabel11539: ;
return 12 * CYCLE_UNIT / 2;
}

/* CHK.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41bb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11540;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11540;
	}
}}}}}endlabel11540: ;
return 12 * CYCLE_UNIT / 2;
}

/* CHK.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41bc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel11541;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel11541;
	}
}}}endlabel11541: ;
return 8 * CYCLE_UNIT / 2;
}

/* LEA.L (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_41d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LEA.L (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_41e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* LEA.L (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_41f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	m68k_areg (regs, dstreg) = (srca);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* LEA.L (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_41f8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* LEA.L (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_41f9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* LEA.L (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_41fa_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* LEA.L (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_41fb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	m68k_areg (regs, dstreg) = (srca);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* CLR.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4200_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	SET_CZNV (FLAGVAL_Z);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((0) & 0xff);
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CLR.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4210_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	SET_CZNV (FLAGVAL_Z);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, 0);
}}return 8 * CYCLE_UNIT / 2;
}

/* CLR.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4218_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
	SET_CZNV (FLAGVAL_Z);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, 0);
}}return 8 * CYCLE_UNIT / 2;
}

/* CLR.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4220_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	SET_CZNV (FLAGVAL_Z);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, 0);
}}return 10 * CYCLE_UNIT / 2;
}

/* CLR.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4228_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	SET_CZNV (FLAGVAL_Z);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, 0);
}}return 12 * CYCLE_UNIT / 2;
}

/* CLR.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4230_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
	SET_CZNV (FLAGVAL_Z);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, 0);
}}}return 12 * CYCLE_UNIT / 2;
}

/* CLR.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4238_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	SET_CZNV (FLAGVAL_Z);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, 0);
}}return 12 * CYCLE_UNIT / 2;
}

/* CLR.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4239_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
	SET_CZNV (FLAGVAL_Z);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, 0);
}}return 16 * CYCLE_UNIT / 2;
}

/* CLR.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4240_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	SET_CZNV (FLAGVAL_Z);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((0) & 0xffff);
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CLR.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4250_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	SET_CZNV (FLAGVAL_Z);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, 0);
}}return 8 * CYCLE_UNIT / 2;
}

/* CLR.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4258_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
	SET_CZNV (FLAGVAL_Z);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (srca, 0);
}}return 8 * CYCLE_UNIT / 2;
}

/* CLR.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4260_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	SET_CZNV (FLAGVAL_Z);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (srca, 0);
}}return 10 * CYCLE_UNIT / 2;
}

/* CLR.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4268_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	SET_CZNV (FLAGVAL_Z);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, 0);
}}return 12 * CYCLE_UNIT / 2;
}

/* CLR.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4270_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
	SET_CZNV (FLAGVAL_Z);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, 0);
}}}return 12 * CYCLE_UNIT / 2;
}

/* CLR.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4278_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	SET_CZNV (FLAGVAL_Z);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, 0);
}}return 12 * CYCLE_UNIT / 2;
}

/* CLR.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4279_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
	SET_CZNV (FLAGVAL_Z);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, 0);
}}return 16 * CYCLE_UNIT / 2;
}

/* CLR.L Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4280_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	SET_CZNV (FLAGVAL_Z);
	m68k_dreg (regs, srcreg) = (0);
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CLR.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4290_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	SET_CZNV (FLAGVAL_Z);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, 0);
}}return 12 * CYCLE_UNIT / 2;
}

/* CLR.L (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4298_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
	SET_CZNV (FLAGVAL_Z);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (srca, 0);
}}return 12 * CYCLE_UNIT / 2;
}

/* CLR.L -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_42a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	SET_CZNV (FLAGVAL_Z);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (srca, 0);
}}return 14 * CYCLE_UNIT / 2;
}

/* CLR.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_42a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	SET_CZNV (FLAGVAL_Z);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, 0);
}}return 16 * CYCLE_UNIT / 2;
}

/* CLR.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_42b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
	SET_CZNV (FLAGVAL_Z);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, 0);
}}}return 16 * CYCLE_UNIT / 2;
}

/* CLR.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_42b8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	SET_CZNV (FLAGVAL_Z);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, 0);
}}return 16 * CYCLE_UNIT / 2;
}

/* CLR.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_42b9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
	SET_CZNV (FLAGVAL_Z);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, 0);
}}return 20 * CYCLE_UNIT / 2;
}

/* MVSR2.B Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	MakeSR ();
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((regs.sr & 0xff) & 0xffff);
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

#endif
#endif

#ifdef PART_4
/* MVSR2.B (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	MakeSR ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, regs.sr & 0xff);
}}return 8 * CYCLE_UNIT / 2;
}

#endif
/* MVSR2.B (An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
	MakeSR ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (srca, regs.sr & 0xff);
}}return 8 * CYCLE_UNIT / 2;
}

#endif
/* MVSR2.B -(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	MakeSR ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (srca, regs.sr & 0xff);
}}return 10 * CYCLE_UNIT / 2;
}

#endif
/* MVSR2.B (d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	MakeSR ();
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, regs.sr & 0xff);
}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* MVSR2.B (d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
	MakeSR ();
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, regs.sr & 0xff);
}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* MVSR2.B (xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42f8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	MakeSR ();
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, regs.sr & 0xff);
}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* MVSR2.B (xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42f9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
	MakeSR ();
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, regs.sr & 0xff);
}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* NEG.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4400_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((dst) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* NEG.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4410_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, dst);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* NEG.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4418_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, dst);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* NEG.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4420_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, dst);
}}}}}return 14 * CYCLE_UNIT / 2;
}

/* NEG.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4428_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEG.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4430_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, dst);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEG.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4438_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEG.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4439_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* NEG.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4440_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((dst) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* NEG.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4450_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, dst);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* NEG.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4458_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (srca, dst);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* NEG.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4460_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (srca, dst);
}}}}}return 14 * CYCLE_UNIT / 2;
}

/* NEG.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4468_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEG.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4470_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, dst);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEG.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4478_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEG.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4479_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* NEG.L Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4480_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	m68k_dreg (regs, srcreg) = (dst);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* NEG.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4490_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* NEG.L (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4498_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (srca, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* NEG.L -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_44a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (srca, dst);
}}}}}return 22 * CYCLE_UNIT / 2;
}

/* NEG.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_44a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, dst);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* NEG.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_44b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, dst);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* NEG.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_44b8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, dst);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* NEG.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_44b9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, dst);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MV2SR.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_44c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* MV2SR.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_44d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* MV2SR.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_44d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MV2SR.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_44e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* MV2SR.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_44e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_44f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_44f8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_44f9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* MV2SR.B (d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_44fa_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.B (d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_44fb_31)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.B #<data>.B */
uae_u32 REGPARAM2 CPUFUNC(op_44fc_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* NOT.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4600_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((dst) & 0xff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* NOT.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4610_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, dst);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* NOT.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4618_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, dst);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* NOT.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4620_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, dst);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* NOT.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4628_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* NOT.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4630_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NOT.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4638_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* NOT.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4639_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, dst);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* NOT.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4640_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((dst) & 0xffff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* NOT.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4650_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, dst);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* NOT.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4658_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (srca, dst);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* NOT.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4660_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (srca, dst);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* NOT.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4668_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* NOT.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4670_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NOT.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4678_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, dst);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* NOT.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4679_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (srca, dst);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* NOT.L Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4680_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	m68k_dreg (regs, srcreg) = (dst);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* NOT.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4690_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, dst);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* NOT.L (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4698_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (srca, dst);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* NOT.L -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_46a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (srca, dst);
}}}}return 22 * CYCLE_UNIT / 2;
}

/* NOT.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_46a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, dst);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* NOT.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_46b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, dst);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* NOT.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_46b8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, dst);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* NOT.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_46b9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (srca, dst);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MV2SR.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_46c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel11640; }
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	regs.sr = src;
	MakeFromSR ();
}}}	m68k_incpci (2);
endlabel11640: ;
return 4 * CYCLE_UNIT / 2;
}

/* MV2SR.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_46d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel11641; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (2);
endlabel11641: ;
return 8 * CYCLE_UNIT / 2;
}

/* MV2SR.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_46d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel11642; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (2);
endlabel11642: ;
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MV2SR.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_46e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel11643; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (2);
endlabel11643: ;
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* MV2SR.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_46e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel11644; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (4);
endlabel11644: ;
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_46f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel11645; }
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}}endlabel11645: ;
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_46f8_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11646; }
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (4);
endlabel11646: ;
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_46f9_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11647; }
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (6);
endlabel11647: ;
return 16 * CYCLE_UNIT / 2;
}

/* MV2SR.W (d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_46fa_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11648; }
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (4);
endlabel11648: ;
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.W (d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_46fb_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11649; }
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}}endlabel11649: ;
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_46fc_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11650; }
{{	uae_s16 src = get_iword_mmu040 (2);
	regs.sr = src;
	MakeFromSR ();
}}}	m68k_incpci (4);
endlabel11650: ;
return 8 * CYCLE_UNIT / 2;
}

/* NBCD.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4800_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_u16 newv_lo = - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	int cflg, tmp_newv;
	if (newv_lo > 9) { newv_lo -= 6; }
	tmp_newv = newv = newv_hi + newv_lo;
	cflg = (newv & 0x1F0) > 0x90;
	if (cflg) newv -= 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((newv) & 0xff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LINK.L An,#<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4808_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
{	uae_s32 offs;
	offs = get_ilong_mmu040 (2);
	mmufixup[0].reg = -1;
{	uaecptr olda;
	olda = m68k_areg (regs, 7) - 4;
	m68k_areg (regs, 7) = olda;
{	uae_s32 src = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = (m68k_areg(regs, 7));
	m68k_areg(regs, 7) += offs;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (olda, src);
}}}}return 22 * CYCLE_UNIT / 2;
}

#endif
/* NBCD.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4810_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_u16 newv_lo = - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	int cflg, tmp_newv;
	if (newv_lo > 9) { newv_lo -= 6; }
	tmp_newv = newv = newv_hi + newv_lo;
	cflg = (newv & 0x1F0) > 0x90;
	if (cflg) newv -= 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, newv);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* NBCD.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4818_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_u16 newv_lo = - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	int cflg, tmp_newv;
	if (newv_lo > 9) { newv_lo -= 6; }
	tmp_newv = newv = newv_hi + newv_lo;
	cflg = (newv & 0x1F0) > 0x90;
	if (cflg) newv -= 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, newv);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* NBCD.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4820_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_u16 newv_lo = - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	int cflg, tmp_newv;
	if (newv_lo > 9) { newv_lo -= 6; }
	tmp_newv = newv = newv_hi + newv_lo;
	cflg = (newv & 0x1F0) > 0x90;
	if (cflg) newv -= 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, newv);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* NBCD.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4828_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_u16 newv_lo = - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	int cflg, tmp_newv;
	if (newv_lo > 9) { newv_lo -= 6; }
	tmp_newv = newv = newv_hi + newv_lo;
	cflg = (newv & 0x1F0) > 0x90;
	if (cflg) newv -= 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, newv);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* NBCD.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4830_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_u16 newv_lo = - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	int cflg, tmp_newv;
	if (newv_lo > 9) { newv_lo -= 6; }
	tmp_newv = newv = newv_hi + newv_lo;
	cflg = (newv & 0x1F0) > 0x90;
	if (cflg) newv -= 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, newv);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NBCD.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4838_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_u16 newv_lo = - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	int cflg, tmp_newv;
	if (newv_lo > 9) { newv_lo -= 6; }
	tmp_newv = newv = newv_hi + newv_lo;
	cflg = (newv & 0x1F0) > 0x90;
	if (cflg) newv -= 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, newv);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* NBCD.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4839_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_u16 newv_lo = - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = - (src & 0xF0);
	uae_u16 newv;
	int cflg, tmp_newv;
	if (newv_lo > 9) { newv_lo -= 6; }
	tmp_newv = newv = newv_hi + newv_lo;
	cflg = (newv & 0x1F0) > 0x90;
	if (cflg) newv -= 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, newv);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* SWAP.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4840_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_u32 dst = ((src >> 16)&0xFFFF) | ((src&0xFFFF)<<16);
	optflag_testl ((uae_s32)(dst));
	m68k_dreg (regs, srcreg) = (dst);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* BKPTQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4848_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* PEA.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4850_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, srca);
}}}return 12 * CYCLE_UNIT / 2;
}

/* PEA.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4868_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, srca);
}}}return 16 * CYCLE_UNIT / 2;
}

/* PEA.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4870_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, srca);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* PEA.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4878_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, srca);
}}}return 16 * CYCLE_UNIT / 2;
}

/* PEA.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4879_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, srca);
}}}return 20 * CYCLE_UNIT / 2;
}

/* PEA.L (d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_487a_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, srca);
}}}return 16 * CYCLE_UNIT / 2;
}

/* PEA.L (d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_487b_31)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, srca);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* EXT.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4880_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_u16 dst = (uae_s16)(uae_s8)src;
	optflag_testw ((uae_s16)(dst));
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((dst) & 0xffff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* MVMLE.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4890_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu040 (2);
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = m68k_areg (regs, dstreg);
}{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		put_word_mmu040 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_word_mmu040 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 2;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* MVMLE.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_48a0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu040 (2);
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = m68k_areg (regs, dstreg) - 0;
}{	uae_u16 amask = mask & 0xff, dmask = (mask >> 8) & 0xff;
	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (amask) {
		srca -= 2;
		put_word_mmu040 (srca, m68k_areg (regs, movem_index2[amask]));
		amask = movem_next[amask];
	}
	while (dmask) {
		srca -= 2;
		put_word_mmu040 (srca, m68k_dreg (regs, movem_index2[dmask]));
		dmask = movem_next[dmask];
	}
	m68k_areg (regs, dstreg) = srca;
	mmu040_movem = 0;
}}}	m68k_incpci (4);
return 10 * CYCLE_UNIT / 2;
}

/* MVMLE.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_48a8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu040 (2);
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
}{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		put_word_mmu040 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_word_mmu040 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 2;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMLE.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_48b0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu040 (2);
{	uaecptr srca;
	m68k_incpci (4);
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{{	srca = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
}{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		put_word_mmu040 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_word_mmu040 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 2;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MVMLE.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_48b8_31)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu040 (2);
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = (uae_s32)(uae_s16)get_iword_mmu040 (4);
}{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		put_word_mmu040 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_word_mmu040 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 2;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMLE.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_48b9_31)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu040 (2);
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = get_ilong_mmu040 (4);
}{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		put_word_mmu040 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_word_mmu040 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 2;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

/* EXT.L Dn */
uae_u32 REGPARAM2 CPUFUNC(op_48c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_u32 dst = (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(dst));
	m68k_dreg (regs, srcreg) = (dst);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* MVMLE.L #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_48d0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu040 (2);
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = m68k_areg (regs, dstreg);
}{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		put_long_mmu040 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_long_mmu040 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 4;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* MVMLE.L #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_48e0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu040 (2);
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = m68k_areg (regs, dstreg) - 0;
}{	uae_u16 amask = mask & 0xff, dmask = (mask >> 8) & 0xff;
	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (amask) {
		srca -= 4;
		put_long_mmu040 (srca, m68k_areg (regs, movem_index2[amask]));
		amask = movem_next[amask];
	}
	while (dmask) {
		srca -= 4;
		put_long_mmu040 (srca, m68k_dreg (regs, movem_index2[dmask]));
		dmask = movem_next[dmask];
	}
	m68k_areg (regs, dstreg) = srca;
	mmu040_movem = 0;
}}}	m68k_incpci (4);
return 10 * CYCLE_UNIT / 2;
}

/* MVMLE.L #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_48e8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu040 (2);
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
}{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		put_long_mmu040 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_long_mmu040 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 4;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMLE.L #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_48f0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu040 (2);
{	uaecptr srca;
	m68k_incpci (4);
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{{	srca = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
}{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		put_long_mmu040 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_long_mmu040 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 4;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MVMLE.L #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_48f8_31)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu040 (2);
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = (uae_s32)(uae_s16)get_iword_mmu040 (4);
}{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		put_long_mmu040 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_long_mmu040 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 4;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMLE.L #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_48f9_31)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu040 (2);
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = get_ilong_mmu040 (4);
}{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		put_long_mmu040 (srca, m68k_dreg (regs, movem_index1[dmask]));
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		put_long_mmu040 (srca, m68k_areg (regs, movem_index1[amask]));
		srca += 4;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

/* EXT.B Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_49c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_u32 dst = (uae_s32)(uae_s8)src;
	optflag_testl ((uae_s32)(dst));
	m68k_dreg (regs, srcreg) = (dst);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

#endif
/* TST.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4a00_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
	optflag_testb ((uae_s8)(src));
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* TST.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a10_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* TST.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4a18_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* TST.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a20_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* TST.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a28_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* TST.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4a30_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu040 (srca);
	optflag_testb ((uae_s8)(src));
}}}}return 12 * CYCLE_UNIT / 2;
}

/* TST.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4a38_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* TST.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4a39_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TST.B (d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4a3a_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TST.B (d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4a3b_31)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu040 (srca);
	optflag_testb ((uae_s8)(src));
}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* TST.B #<data>.B */
uae_u32 REGPARAM2 CPUFUNC(op_4a3c_31)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu040 (2);
	optflag_testb ((uae_s8)(src));
}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* TST.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4a40_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	optflag_testw ((uae_s16)(src));
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* TST.W An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4a48_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_areg (regs, srcreg);
	optflag_testw ((uae_s16)(src));
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

#endif
/* TST.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a50_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* TST.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4a58_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* TST.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a60_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* TST.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a68_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* TST.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4a70_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
	optflag_testw ((uae_s16)(src));
}}}}return 12 * CYCLE_UNIT / 2;
}

/* TST.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4a78_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* TST.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4a79_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TST.W (d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4a7a_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TST.W (d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4a7b_31)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
	optflag_testw ((uae_s16)(src));
}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* TST.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_4a7c_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
	optflag_testw ((uae_s16)(src));
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* TST.L Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4a80_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
	optflag_testl ((uae_s32)(src));
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* TST.L An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4a88_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg (regs, srcreg);
	optflag_testl ((uae_s32)(src));
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

#endif
/* TST.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a90_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* TST.L (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4a98_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* TST.L -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4aa0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* TST.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4aa8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* TST.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4ab0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
	optflag_testl ((uae_s32)(src));
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TST.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4ab8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* TST.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4ab9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* TST.L (d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4aba_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* TST.L (d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4abb_31)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
	optflag_testl ((uae_s32)(src));
}}}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* TST.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_4abc_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
	optflag_testl ((uae_s32)(src));
}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* TAS.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4ac0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((src) & 0xff);
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* TAS.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ad0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_lrmw_byte_mmu040 (srca);
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_lrmw_byte_mmu040 (srca, src);
}}}return 12 * CYCLE_UNIT / 2;
}

/* TAS.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4ad8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_lrmw_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_lrmw_byte_mmu040 (srca, src);
}}}return 12 * CYCLE_UNIT / 2;
}

/* TAS.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ae0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_lrmw_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_lrmw_byte_mmu040 (srca, src);
}}}return 14 * CYCLE_UNIT / 2;
}

/* TAS.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ae8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_lrmw_byte_mmu040 (srca);
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_lrmw_byte_mmu040 (srca, src);
}}}return 16 * CYCLE_UNIT / 2;
}

/* TAS.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4af0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_lrmw_byte_mmu040 (srca);
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_lrmw_byte_mmu040 (srca, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TAS.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4af8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_lrmw_byte_mmu040 (srca);
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_lrmw_byte_mmu040 (srca, src);
}}}return 16 * CYCLE_UNIT / 2;
}

/* TAS.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4af9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_lrmw_byte_mmu040 (srca);
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_lrmw_byte_mmu040 (srca, src);
}}}return 20 * CYCLE_UNIT / 2;
}

/* MULL.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c00_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	m68k_mull(opcode, dst, extra);
}}}return 8 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c10_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	m68k_incpci (4);
	m68k_mull(opcode, dst, extra);
}}}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c18_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	m68k_incpci (4);
	m68k_mull(opcode, dst, extra);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c20_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	m68k_incpci (4);
	m68k_mull(opcode, dst, extra);
}}}}	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c28_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s32 dst = get_long_mmu040 (dsta);
	m68k_incpci (6);
	m68k_mull(opcode, dst, extra);
}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c30_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu040 (dsta);
	m68k_mull(opcode, dst, extra);
}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c38_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s32 dst = get_long_mmu040 (dsta);
	m68k_incpci (6);
	m68k_mull(opcode, dst, extra);
}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c39_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s32 dst = get_long_mmu040 (dsta);
	m68k_incpci (8);
	m68k_mull(opcode, dst, extra);
}}}}return 24 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c3a_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s32 dst = get_long_mmu040 (dsta);
	m68k_incpci (6);
	m68k_mull(opcode, dst, extra);
}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c3b_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 dst = get_long_mmu040 (dsta);
	m68k_mull(opcode, dst, extra);
}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,#<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c3c_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uae_s32 dst;
	dst = get_ilong_mmu040 (4);
	m68k_incpci (8);
	m68k_mull(opcode, dst, extra);
}}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c40_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	m68k_divl(opcode, dst, extra);
}}}return 8 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c50_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	m68k_incpci (4);
	m68k_divl(opcode, dst, extra);
}}}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c58_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	m68k_incpci (4);
	m68k_divl(opcode, dst, extra);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c60_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	m68k_incpci (4);
	m68k_divl(opcode, dst, extra);
}}}}	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c68_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s32 dst = get_long_mmu040 (dsta);
	m68k_incpci (6);
	m68k_divl(opcode, dst, extra);
}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c70_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu040 (dsta);
	m68k_divl(opcode, dst, extra);
}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c78_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s32 dst = get_long_mmu040 (dsta);
	m68k_incpci (6);
	m68k_divl(opcode, dst, extra);
}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c79_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_s32 dst = get_long_mmu040 (dsta);
	m68k_incpci (8);
	m68k_divl(opcode, dst, extra);
}}}}return 24 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c7a_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_s32 dst = get_long_mmu040 (dsta);
	m68k_incpci (6);
	m68k_divl(opcode, dst, extra);
}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c7b_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 dst = get_long_mmu040 (dsta);
	m68k_divl(opcode, dst, extra);
}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,#<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c7c_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uae_s32 dst;
	dst = get_ilong_mmu040 (4);
	m68k_incpci (8);
	m68k_divl(opcode, dst, extra);
}}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* MVMEL.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4c90_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu040 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = m68k_areg (regs, dstreg);
}{	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		m68k_dreg (regs, movem_index1[dmask]) = (uae_s32)(uae_s16)get_word_mmu040 (srca);
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		m68k_areg (regs, movem_index1[amask]) = (uae_s32)(uae_s16)get_word_mmu040 (srca);
		srca += 2;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4c98_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu040 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = m68k_areg (regs, dstreg);
}{	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		m68k_dreg (regs, movem_index1[dmask]) = (uae_s32)(uae_s16)get_word_mmu040 (srca);
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		m68k_areg (regs, movem_index1[amask]) = (uae_s32)(uae_s16)get_word_mmu040 (srca);
		srca += 2;
		amask = movem_next[amask];
	}
	m68k_areg (regs, dstreg) = srca;
	mmu040_movem = 0;
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ca8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu040 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
}{	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		m68k_dreg (regs, movem_index1[dmask]) = (uae_s32)(uae_s16)get_word_mmu040 (srca);
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		m68k_areg (regs, movem_index1[amask]) = (uae_s32)(uae_s16)get_word_mmu040 (srca);
		srca += 2;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4cb0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu040 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	m68k_incpci (4);
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{{	srca = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
}{	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		m68k_dreg (regs, movem_index1[dmask]) = (uae_s32)(uae_s16)get_word_mmu040 (srca);
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		m68k_areg (regs, movem_index1[amask]) = (uae_s32)(uae_s16)get_word_mmu040 (srca);
		srca += 2;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4cb8_31)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu040 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = (uae_s32)(uae_s16)get_iword_mmu040 (4);
}{	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		m68k_dreg (regs, movem_index1[dmask]) = (uae_s32)(uae_s16)get_word_mmu040 (srca);
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		m68k_areg (regs, movem_index1[amask]) = (uae_s32)(uae_s16)get_word_mmu040 (srca);
		srca += 2;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4cb9_31)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu040 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = get_ilong_mmu040 (4);
}{	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		m68k_dreg (regs, movem_index1[dmask]) = (uae_s32)(uae_s16)get_word_mmu040 (srca);
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		m68k_areg (regs, movem_index1[amask]) = (uae_s32)(uae_s16)get_word_mmu040 (srca);
		srca += 2;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_4cba_31)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu040 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = m68k_getpc () + 4;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (4);
}{	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		m68k_dreg (regs, movem_index1[dmask]) = (uae_s32)(uae_s16)get_word_mmu040 (srca);
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		m68k_areg (regs, movem_index1[amask]) = (uae_s32)(uae_s16)get_word_mmu040 (srca);
		srca += 2;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4cbb_31)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu040 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (4);
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
}{	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		m68k_dreg (regs, movem_index1[dmask]) = (uae_s32)(uae_s16)get_word_mmu040 (srca);
		srca += 2;
		dmask = movem_next[dmask];
	}
	while (amask) {
		m68k_areg (regs, movem_index1[amask]) = (uae_s32)(uae_s16)get_word_mmu040 (srca);
		srca += 2;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4cd0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu040 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = m68k_areg (regs, dstreg);
}{	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		m68k_dreg (regs, movem_index1[dmask]) = get_long_mmu040 (srca);
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		m68k_areg (regs, movem_index1[amask]) = get_long_mmu040 (srca);
		srca += 4;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4cd8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu040 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = m68k_areg (regs, dstreg);
}{	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		m68k_dreg (regs, movem_index1[dmask]) = get_long_mmu040 (srca);
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		m68k_areg (regs, movem_index1[amask]) = get_long_mmu040 (srca);
		srca += 4;
		amask = movem_next[amask];
	}
	m68k_areg (regs, dstreg) = srca;
	mmu040_movem = 0;
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ce8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu040 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
}{	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		m68k_dreg (regs, movem_index1[dmask]) = get_long_mmu040 (srca);
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		m68k_areg (regs, movem_index1[amask]) = get_long_mmu040 (srca);
		srca += 4;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4cf0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu040 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	m68k_incpci (4);
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{{	srca = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
}{	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		m68k_dreg (regs, movem_index1[dmask]) = get_long_mmu040 (srca);
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		m68k_areg (regs, movem_index1[amask]) = get_long_mmu040 (srca);
		srca += 4;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4cf8_31)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu040 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = (uae_s32)(uae_s16)get_iword_mmu040 (4);
}{	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		m68k_dreg (regs, movem_index1[dmask]) = get_long_mmu040 (srca);
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		m68k_areg (regs, movem_index1[amask]) = get_long_mmu040 (srca);
		srca += 4;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4cf9_31)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu040 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = get_ilong_mmu040 (4);
}{	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		m68k_dreg (regs, movem_index1[dmask]) = get_long_mmu040 (srca);
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		m68k_areg (regs, movem_index1[amask]) = get_long_mmu040 (srca);
		srca += 4;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_4cfa_31)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu040 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{	srca = m68k_getpc () + 4;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (4);
}{	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		m68k_dreg (regs, movem_index1[dmask]) = get_long_mmu040 (srca);
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		m68k_areg (regs, movem_index1[amask]) = get_long_mmu040 (srca);
		srca += 4;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4cfb_31)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu040 (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (4);
	if (mmu040_movem) {
		srca = mmu040_movem_ea;
	} else
{{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
}{	mmu040_movem = 1;
	mmu040_movem_ea = srca;
	while (dmask) {
		m68k_dreg (regs, movem_index1[dmask]) = get_long_mmu040 (srca);
		srca += 4;
		dmask = movem_next[dmask];
	}
	while (amask) {
		m68k_areg (regs, movem_index1[amask]) = get_long_mmu040 (srca);
		srca += 4;
		amask = movem_next[amask];
	}
	mmu040_movem = 0;
}}}}return 12 * CYCLE_UNIT / 2;
}

/* TRAPQ.L #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_4e40_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 15);
{{	uae_u32 src = srcreg;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	Exception (src + 32);
}}return 4 * CYCLE_UNIT / 2;
}

/* LINK.W An,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_4e50_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu040 (2);
	mmufixup[0].reg = -1;
{	uaecptr olda;
	olda = m68k_areg (regs, 7) - 4;
	m68k_areg (regs, 7) = olda;
{	uae_s32 src = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = (m68k_areg(regs, 7));
	m68k_areg(regs, 7) += offs;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (olda, src);
}}}}return 18 * CYCLE_UNIT / 2;
}

/* UNLK.L An */
uae_u32 REGPARAM2 CPUFUNC(op_4e58_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg (regs, srcreg);
	uae_s32 old = get_long_mmu040 (src);
	m68k_areg (regs, 7) = src + 4;
	m68k_areg (regs, srcreg) = old;
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* MVR2USP.L An */
uae_u32 REGPARAM2 CPUFUNC(op_4e60_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel11768; }
{{	uae_s32 src = m68k_areg (regs, srcreg);
	regs.usp = src;
}}}	m68k_incpci (2);
endlabel11768: ;
return 4 * CYCLE_UNIT / 2;
}

/* MVUSP2R.L An */
uae_u32 REGPARAM2 CPUFUNC(op_4e68_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel11769; }
{{	m68k_areg (regs, srcreg) = (regs.usp);
}}}	m68k_incpci (2);
endlabel11769: ;
return 4 * CYCLE_UNIT / 2;
}

/* RESET.L  */
uae_u32 REGPARAM2 CPUFUNC(op_4e70_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11770; }
{	cpureset ();
	m68k_incpci (2);
}}endlabel11770: ;
return 4 * CYCLE_UNIT / 2;
}

/* NOP.L  */
uae_u32 REGPARAM2 CPUFUNC(op_4e71_31)(uae_u32 opcode)
{
{}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* STOP.L #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_4e72_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11772; }
{{	uae_s16 src = get_iword_mmu040 (2);
	regs.sr = src;
	MakeFromSR ();
	m68k_setstopped ();
	m68k_incpci (4);
}}}endlabel11772: ;
return 8 * CYCLE_UNIT / 2;
}

/* RTE.L  */
uae_u32 REGPARAM2 CPUFUNC(op_4e73_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11773; }
{	uae_u16 newsr; uae_u32 newpc;
	for (;;) {
		uaecptr a = m68k_areg (regs, 7);
		uae_u16 sr = get_word_mmu040 (a);
		uae_u32 pc = get_long_mmu040 (a + 2);
		uae_u16 format = get_word_mmu040 (a + 2 + 4);
		int frame = format >> 12;
		int offset = 8;
		newsr = sr; newpc = pc;
		if (frame == 0x0) { m68k_areg (regs, 7) += offset; break; }
		else if (frame == 0x1) { m68k_areg (regs, 7) += offset; }
		else if (frame == 0x2) { m68k_areg (regs, 7) += offset + 4; break; }
		else if (frame == 0x4) { m68k_areg (regs, 7) += offset + 8; break; }
		else if (frame == 0x8) { m68k_areg (regs, 7) += offset + 50; break; }
		else if (frame == 0x7) { m68k_do_rte_mmu040 (a); m68k_areg (regs, 7) += offset + 52; break; }
		else if (frame == 0x9) { m68k_areg (regs, 7) += offset + 12; break; }
		else if (frame == 0xa) { m68k_areg (regs, 7) += offset + 24; break; }
		else if (frame == 0xb) { m68k_areg (regs, 7) += offset + 84; break; }
		else { m68k_areg (regs, 7) += offset; Exception (14); goto endlabel11773; }
		regs.sr = newsr; MakeFromSR ();
}
	regs.sr = newsr; MakeFromSR ();
	if (newpc & 1) {
		exception3i (0x4E73, newpc);
		goto endlabel11773;
	}
	m68k_setpc_mmu (newpc);
	ipl_fetch ();
}}endlabel11773: ;
return 4 * CYCLE_UNIT / 2;
}

/* RTD.L #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_4e74_31)(uae_u32 opcode)
{
{{	uae_s16 offs = get_iword_mmu040 (2);
{	uaecptr pca;
	pca = m68k_areg (regs, 7);
{	uae_s32 pc = get_long_mmu040 (pca);
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) += 4;
	m68k_areg(regs, 7) += offs;
	if (pc & 1) {
		exception3i (0x4E74, pc);
		goto endlabel11774;
	}
	m68k_setpc_mmu (pc);
}}}}endlabel11774: ;
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* RTS.L  */
uae_u32 REGPARAM2 CPUFUNC(op_4e75_31)(uae_u32 opcode)
{
{	uaecptr pc = m68k_getpc ();
	m68k_do_rts_mmu040 ();
	if (m68k_getpc () & 1) {
		uaecptr faultpc = m68k_getpc ();
	m68k_setpc_mmu (pc);
		exception3i (0x4E75, faultpc);
	}
}return 4 * CYCLE_UNIT / 2;
}

/* TRAPV.L  */
uae_u32 REGPARAM2 CPUFUNC(op_4e76_31)(uae_u32 opcode)
{
{	m68k_incpci (2);
	if (GET_VFLG ()) {
		Exception (7);
		goto endlabel11776;
	}
}endlabel11776: ;
return 4 * CYCLE_UNIT / 2;
}

/* RTR.L  */
uae_u32 REGPARAM2 CPUFUNC(op_4e77_31)(uae_u32 opcode)
{
{	uaecptr oldpc = m68k_getpc ();
	MakeSR ();
{	uaecptr sra;
	sra = m68k_areg (regs, 7);
{	uae_s16 sr = get_word_mmu040 (sra);
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) += 2;
{	uaecptr pca;
	pca = m68k_areg (regs, 7);
{	uae_s32 pc = get_long_mmu040 (pca);
	m68k_areg (regs, 7) += 4;
	mmufixup[0].reg = -1;
	regs.sr &= 0xFF00; sr &= 0xFF;
	regs.sr |= sr;
	m68k_setpc_mmu (pc);
	MakeFromSR ();
	if (m68k_getpc () & 1) {
		uaecptr faultpc = m68k_getpc ();
	m68k_setpc_mmu (oldpc);
		exception3i (0x4E77, faultpc);
	}
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEC2.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4e7a_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11778; }
{{	uae_s16 src = get_iword_mmu040 (2);
{	int regno = (src >> 12) & 15;
	uae_u32 *regp = regs.regs + regno;
	if (! m68k_movec2(src & 0xFFF, regp)) goto endlabel11778;
}}}}	m68k_incpci (4);
endlabel11778: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* MOVE2C.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4e7b_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel11779; }
{{	uae_s16 src = get_iword_mmu040 (2);
{	int regno = (src >> 12) & 15;
	uae_u32 *regp = regs.regs + regno;
	if (! m68k_move2c(src & 0xFFF, regp)) goto endlabel11779;
}}}}	m68k_incpci (4);
endlabel11779: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* JSR.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4e90_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uaecptr oldpc = m68k_getpc () + 2;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel11780;
	}
	put_long_mmu040 (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}endlabel11780: ;
return 4 * CYCLE_UNIT / 2;
}

/* JSR.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ea8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uaecptr oldpc = m68k_getpc () + 4;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel11781;
	}
	put_long_mmu040 (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}endlabel11781: ;
return 8 * CYCLE_UNIT / 2;
}

/* JSR.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4eb0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uaecptr oldpc = m68k_getpc () + 0;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel11782;
	}
	put_long_mmu040 (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}}endlabel11782: ;
return 8 * CYCLE_UNIT / 2;
}

/* JSR.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4eb8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uaecptr oldpc = m68k_getpc () + 4;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel11783;
	}
	put_long_mmu040 (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}endlabel11783: ;
return 8 * CYCLE_UNIT / 2;
}

/* JSR.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4eb9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uaecptr oldpc = m68k_getpc () + 6;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel11784;
	}
	put_long_mmu040 (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}endlabel11784: ;
return 12 * CYCLE_UNIT / 2;
}

/* JSR.L (d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_4eba_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uaecptr oldpc = m68k_getpc () + 4;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel11785;
	}
	put_long_mmu040 (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}endlabel11785: ;
return 8 * CYCLE_UNIT / 2;
}

/* JSR.L (d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4ebb_31)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uaecptr oldpc = m68k_getpc () + 0;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel11786;
	}
	put_long_mmu040 (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}}endlabel11786: ;
return 8 * CYCLE_UNIT / 2;
}

/* JMP.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ed0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel11787;
	}
	m68k_setpc_mmu (srca);
}}endlabel11787: ;
return 4 * CYCLE_UNIT / 2;
}

/* JMP.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ee8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel11788;
	}
	m68k_setpc_mmu (srca);
}}endlabel11788: ;
return 8 * CYCLE_UNIT / 2;
}

/* JMP.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4ef0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel11789;
	}
	m68k_setpc_mmu (srca);
}}}endlabel11789: ;
return 8 * CYCLE_UNIT / 2;
}

/* JMP.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4ef8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel11790;
	}
	m68k_setpc_mmu (srca);
}}endlabel11790: ;
return 8 * CYCLE_UNIT / 2;
}

/* JMP.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4ef9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel11791;
	}
	m68k_setpc_mmu (srca);
}}endlabel11791: ;
return 12 * CYCLE_UNIT / 2;
}

/* JMP.L (d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_4efa_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel11792;
	}
	m68k_setpc_mmu (srca);
}}endlabel11792: ;
return 8 * CYCLE_UNIT / 2;
}

/* JMP.L (d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4efb_31)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel11793;
	}
	m68k_setpc_mmu (srca);
}}}endlabel11793: ;
return 8 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5000_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5010_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5018_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5020_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 14 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5028_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5030_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5038_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5039_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5040_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDAQ.W #<data>,An */
uae_u32 REGPARAM2 CPUFUNC(op_5048_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5050_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5058_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5060_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, newv);
}}}}}}return 14 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5068_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5070_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

#endif

#ifdef PART_5
/* ADDQ.W #<data>,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5078_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5079_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5080_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDAQ.L #<data>,An */
uae_u32 REGPARAM2 CPUFUNC(op_5088_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5090_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5098_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_50a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, newv);
}}}}}}return 22 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_50a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_50b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_50b8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_50b9_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 28 * CYCLE_UNIT / 2;
}

/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_50c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (0) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_50c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu040 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (0)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel11821;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel11821: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_50d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (0) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_50d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (0) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_50e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (0) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_50e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (0) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_50f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (0) ? 0xff : 0;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_50f8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (0) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_50f9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{{	int val = cctrue (0) ? 0xff : 0;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_50fa_31)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu040 (2);
	if (cctrue (0)) { Exception (7); goto endlabel11829; }
}}	m68k_incpci (4);
endlabel11829: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_50fb_31)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu040 (2);
	if (cctrue (0)) { Exception (7); goto endlabel11830; }
}}	m68k_incpci (6);
endlabel11830: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_50fc_31)(uae_u32 opcode)
{
{	if (cctrue (0)) { Exception (7); goto endlabel11831; }
}	m68k_incpci (2);
endlabel11831: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* SUBQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5100_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5110_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5118_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5120_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 14 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5128_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5130_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5138_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5139_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5140_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBAQ.W #<data>,An */
uae_u32 REGPARAM2 CPUFUNC(op_5148_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5150_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5158_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5160_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, newv);
}}}}}}return 14 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5168_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5170_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5178_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5179_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5180_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBAQ.L #<data>,An */
uae_u32 REGPARAM2 CPUFUNC(op_5188_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5190_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5198_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_51a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, newv);
}}}}}}return 22 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_51a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_51b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_51b8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_51b9_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 28 * CYCLE_UNIT / 2;
}

/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_51c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (1) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_51c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu040 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (1)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel11859;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel11859: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_51d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (1) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_51d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (1) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_51e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (1) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_51e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (1) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_51f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (1) ? 0xff : 0;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_51f8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (1) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_51f9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{{	int val = cctrue (1) ? 0xff : 0;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_51fa_31)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu040 (2);
	if (cctrue (1)) { Exception (7); goto endlabel11867; }
}}	m68k_incpci (4);
endlabel11867: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_51fb_31)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu040 (2);
	if (cctrue (1)) { Exception (7); goto endlabel11868; }
}}	m68k_incpci (6);
endlabel11868: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_51fc_31)(uae_u32 opcode)
{
{	if (cctrue (1)) { Exception (7); goto endlabel11869; }
}	m68k_incpci (2);
endlabel11869: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_52c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (2) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_52c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu040 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (2)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel11871;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel11871: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_52d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (2) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_52d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (2) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_52e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (2) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_52e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (2) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_52f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (2) ? 0xff : 0;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_52f8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (2) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_52f9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{{	int val = cctrue (2) ? 0xff : 0;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_52fa_31)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu040 (2);
	if (cctrue (2)) { Exception (7); goto endlabel11879; }
}}	m68k_incpci (4);
endlabel11879: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_52fb_31)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu040 (2);
	if (cctrue (2)) { Exception (7); goto endlabel11880; }
}}	m68k_incpci (6);
endlabel11880: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_52fc_31)(uae_u32 opcode)
{
{	if (cctrue (2)) { Exception (7); goto endlabel11881; }
}	m68k_incpci (2);
endlabel11881: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_53c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (3) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_53c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu040 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (3)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel11883;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel11883: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_53d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (3) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_53d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (3) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_53e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (3) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_53e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (3) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_53f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (3) ? 0xff : 0;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_53f8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (3) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_53f9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{{	int val = cctrue (3) ? 0xff : 0;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_53fa_31)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu040 (2);
	if (cctrue (3)) { Exception (7); goto endlabel11891; }
}}	m68k_incpci (4);
endlabel11891: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_53fb_31)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu040 (2);
	if (cctrue (3)) { Exception (7); goto endlabel11892; }
}}	m68k_incpci (6);
endlabel11892: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_53fc_31)(uae_u32 opcode)
{
{	if (cctrue (3)) { Exception (7); goto endlabel11893; }
}	m68k_incpci (2);
endlabel11893: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_54c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (4) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_54c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu040 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (4)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel11895;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel11895: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_54d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (4) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_54d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (4) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_54e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (4) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_54e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (4) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_54f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (4) ? 0xff : 0;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_54f8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (4) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_54f9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{{	int val = cctrue (4) ? 0xff : 0;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_54fa_31)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu040 (2);
	if (cctrue (4)) { Exception (7); goto endlabel11903; }
}}	m68k_incpci (4);
endlabel11903: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_54fb_31)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu040 (2);
	if (cctrue (4)) { Exception (7); goto endlabel11904; }
}}	m68k_incpci (6);
endlabel11904: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_54fc_31)(uae_u32 opcode)
{
{	if (cctrue (4)) { Exception (7); goto endlabel11905; }
}	m68k_incpci (2);
endlabel11905: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_55c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (5) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_55c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu040 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (5)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel11907;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel11907: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_55d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (5) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_55d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (5) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_55e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (5) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_55e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (5) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_55f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (5) ? 0xff : 0;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_55f8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (5) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_55f9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{{	int val = cctrue (5) ? 0xff : 0;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_55fa_31)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu040 (2);
	if (cctrue (5)) { Exception (7); goto endlabel11915; }
}}	m68k_incpci (4);
endlabel11915: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_55fb_31)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu040 (2);
	if (cctrue (5)) { Exception (7); goto endlabel11916; }
}}	m68k_incpci (6);
endlabel11916: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_55fc_31)(uae_u32 opcode)
{
{	if (cctrue (5)) { Exception (7); goto endlabel11917; }
}	m68k_incpci (2);
endlabel11917: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_56c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (6) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_56c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu040 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (6)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel11919;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel11919: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_56d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (6) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_56d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (6) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_56e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (6) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_56e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (6) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_56f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (6) ? 0xff : 0;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_56f8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (6) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_56f9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{{	int val = cctrue (6) ? 0xff : 0;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_56fa_31)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu040 (2);
	if (cctrue (6)) { Exception (7); goto endlabel11927; }
}}	m68k_incpci (4);
endlabel11927: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_56fb_31)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu040 (2);
	if (cctrue (6)) { Exception (7); goto endlabel11928; }
}}	m68k_incpci (6);
endlabel11928: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_56fc_31)(uae_u32 opcode)
{
{	if (cctrue (6)) { Exception (7); goto endlabel11929; }
}	m68k_incpci (2);
endlabel11929: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_57c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (7) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_57c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu040 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (7)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel11931;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel11931: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_57d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (7) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_57d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (7) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_57e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (7) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_57e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (7) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_57f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (7) ? 0xff : 0;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_57f8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (7) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_57f9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{{	int val = cctrue (7) ? 0xff : 0;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_57fa_31)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu040 (2);
	if (cctrue (7)) { Exception (7); goto endlabel11939; }
}}	m68k_incpci (4);
endlabel11939: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_57fb_31)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu040 (2);
	if (cctrue (7)) { Exception (7); goto endlabel11940; }
}}	m68k_incpci (6);
endlabel11940: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_57fc_31)(uae_u32 opcode)
{
{	if (cctrue (7)) { Exception (7); goto endlabel11941; }
}	m68k_incpci (2);
endlabel11941: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_58c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (8) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_58c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu040 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (8)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel11943;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel11943: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_58d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (8) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_58d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (8) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_58e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (8) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_58e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (8) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_58f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (8) ? 0xff : 0;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_58f8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (8) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_58f9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{{	int val = cctrue (8) ? 0xff : 0;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_58fa_31)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu040 (2);
	if (cctrue (8)) { Exception (7); goto endlabel11951; }
}}	m68k_incpci (4);
endlabel11951: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_58fb_31)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu040 (2);
	if (cctrue (8)) { Exception (7); goto endlabel11952; }
}}	m68k_incpci (6);
endlabel11952: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_58fc_31)(uae_u32 opcode)
{
{	if (cctrue (8)) { Exception (7); goto endlabel11953; }
}	m68k_incpci (2);
endlabel11953: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_59c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (9) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_59c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu040 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (9)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel11955;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel11955: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_59d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (9) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_59d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (9) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_59e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (9) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_59e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (9) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_59f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (9) ? 0xff : 0;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_59f8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (9) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_59f9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{{	int val = cctrue (9) ? 0xff : 0;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_59fa_31)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu040 (2);
	if (cctrue (9)) { Exception (7); goto endlabel11963; }
}}	m68k_incpci (4);
endlabel11963: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_59fb_31)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu040 (2);
	if (cctrue (9)) { Exception (7); goto endlabel11964; }
}}	m68k_incpci (6);
endlabel11964: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_59fc_31)(uae_u32 opcode)
{
{	if (cctrue (9)) { Exception (7); goto endlabel11965; }
}	m68k_incpci (2);
endlabel11965: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5ac0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (10) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_5ac8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu040 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (10)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel11967;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel11967: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ad0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (10) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5ad8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (10) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ae0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (10) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ae8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (10) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5af0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (10) ? 0xff : 0;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5af8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (10) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5af9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{{	int val = cctrue (10) ? 0xff : 0;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5afa_31)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu040 (2);
	if (cctrue (10)) { Exception (7); goto endlabel11975; }
}}	m68k_incpci (4);
endlabel11975: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5afb_31)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu040 (2);
	if (cctrue (10)) { Exception (7); goto endlabel11976; }
}}	m68k_incpci (6);
endlabel11976: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5afc_31)(uae_u32 opcode)
{
{	if (cctrue (10)) { Exception (7); goto endlabel11977; }
}	m68k_incpci (2);
endlabel11977: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5bc0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (11) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_5bc8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu040 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (11)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel11979;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel11979: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_5bd0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (11) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5bd8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (11) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5be0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (11) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5be8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (11) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5bf0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (11) ? 0xff : 0;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5bf8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (11) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5bf9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{{	int val = cctrue (11) ? 0xff : 0;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5bfa_31)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu040 (2);
	if (cctrue (11)) { Exception (7); goto endlabel11987; }
}}	m68k_incpci (4);
endlabel11987: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5bfb_31)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu040 (2);
	if (cctrue (11)) { Exception (7); goto endlabel11988; }
}}	m68k_incpci (6);
endlabel11988: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5bfc_31)(uae_u32 opcode)
{
{	if (cctrue (11)) { Exception (7); goto endlabel11989; }
}	m68k_incpci (2);
endlabel11989: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5cc0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (12) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_5cc8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu040 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (12)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel11991;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel11991: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_5cd0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (12) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5cd8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (12) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ce0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (12) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ce8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (12) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5cf0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (12) ? 0xff : 0;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5cf8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (12) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5cf9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{{	int val = cctrue (12) ? 0xff : 0;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5cfa_31)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu040 (2);
	if (cctrue (12)) { Exception (7); goto endlabel11999; }
}}	m68k_incpci (4);
endlabel11999: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5cfb_31)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu040 (2);
	if (cctrue (12)) { Exception (7); goto endlabel12000; }
}}	m68k_incpci (6);
endlabel12000: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5cfc_31)(uae_u32 opcode)
{
{	if (cctrue (12)) { Exception (7); goto endlabel12001; }
}	m68k_incpci (2);
endlabel12001: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5dc0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (13) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_5dc8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu040 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (13)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel12003;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel12003: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_5dd0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (13) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5dd8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (13) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5de0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (13) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5de8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (13) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5df0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (13) ? 0xff : 0;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5df8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (13) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5df9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{{	int val = cctrue (13) ? 0xff : 0;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5dfa_31)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu040 (2);
	if (cctrue (13)) { Exception (7); goto endlabel12011; }
}}	m68k_incpci (4);
endlabel12011: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5dfb_31)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu040 (2);
	if (cctrue (13)) { Exception (7); goto endlabel12012; }
}}	m68k_incpci (6);
endlabel12012: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5dfc_31)(uae_u32 opcode)
{
{	if (cctrue (13)) { Exception (7); goto endlabel12013; }
}	m68k_incpci (2);
endlabel12013: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5ec0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (14) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_5ec8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu040 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (14)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel12015;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel12015: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ed0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (14) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5ed8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (14) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ee0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (14) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ee8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (14) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5ef0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (14) ? 0xff : 0;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5ef8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (14) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5ef9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{{	int val = cctrue (14) ? 0xff : 0;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5efa_31)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu040 (2);
	if (cctrue (14)) { Exception (7); goto endlabel12023; }
}}	m68k_incpci (4);
endlabel12023: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5efb_31)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu040 (2);
	if (cctrue (14)) { Exception (7); goto endlabel12024; }
}}	m68k_incpci (6);
endlabel12024: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5efc_31)(uae_u32 opcode)
{
{	if (cctrue (14)) { Exception (7); goto endlabel12025; }
}	m68k_incpci (2);
endlabel12025: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5fc0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (15) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_5fc8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu040 (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (15)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel12027;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel12027: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_5fd0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (15) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5fd8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (15) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5fe0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (15) ? 0xff : 0;
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (srca, val);
}}}}return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5fe8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (15) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5ff0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (15) ? 0xff : 0;
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5ff8_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{{	int val = cctrue (15) ? 0xff : 0;
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5ff9_31)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{{	int val = cctrue (15) ? 0xff : 0;
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (srca, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5ffa_31)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu040 (2);
	if (cctrue (15)) { Exception (7); goto endlabel12035; }
}}	m68k_incpci (4);
endlabel12035: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5ffb_31)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu040 (2);
	if (cctrue (15)) { Exception (7); goto endlabel12036; }
}}	m68k_incpci (6);
endlabel12036: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5ffc_31)(uae_u32 opcode)
{
{	if (cctrue (15)) { Exception (7); goto endlabel12037; }
}	m68k_incpci (2);
endlabel12037: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6000_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
	if (!cctrue (0)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12038;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel12038: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6001_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (0)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12039;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel12039: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_60ff_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
	if (!cctrue (0)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12040;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel12040: ;
return 12 * CYCLE_UNIT / 2;
}

/* BSR.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6100_31)(uae_u32 opcode)
{
{	uae_s32 s;
{	uae_s16 src = get_iword_mmu040 (2);
	s = (uae_s32)src + 2;
	if (src & 1) {
		exception3pc (opcode, m68k_getpc () + s, 0, 1, m68k_getpc () + s);
		goto endlabel12041;
	}
	m68k_do_bsr_mmu040 (m68k_getpc () + 4, s);
}}endlabel12041: ;
return 8 * CYCLE_UNIT / 2;
}

/* BSRQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6101_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	uae_s32 s;
{	uae_u32 src = srcreg;
	s = (uae_s32)src + 2;
	if (src & 1) {
		exception3pc (opcode, m68k_getpc () + s, 0, 1, m68k_getpc () + s);
		goto endlabel12042;
	}
	m68k_do_bsr_mmu040 (m68k_getpc () + 2, s);
}}endlabel12042: ;
return 4 * CYCLE_UNIT / 2;
}

/* BSR.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_61ff_31)(uae_u32 opcode)
{
{	uae_s32 s;
{	uae_s32 src;
	src = get_ilong_mmu040 (2);
	s = (uae_s32)src + 2;
	if (src & 1) {
		exception3pc (opcode, m68k_getpc () + s, 0, 1, m68k_getpc () + s);
		goto endlabel12043;
	}
	m68k_do_bsr_mmu040 (m68k_getpc () + 6, s);
}}endlabel12043: ;
return 12 * CYCLE_UNIT / 2;
}

#endif

#ifdef PART_6
/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6200_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
	if (!cctrue (2)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12044;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel12044: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6201_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (2)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12045;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel12045: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_62ff_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
	if (!cctrue (2)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12046;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel12046: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6300_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
	if (!cctrue (3)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12047;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel12047: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6301_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (3)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12048;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel12048: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_63ff_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
	if (!cctrue (3)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12049;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel12049: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6400_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
	if (!cctrue (4)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12050;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel12050: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6401_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (4)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12051;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel12051: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_64ff_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
	if (!cctrue (4)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12052;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel12052: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6500_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
	if (!cctrue (5)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12053;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel12053: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6501_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (5)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12054;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel12054: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_65ff_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
	if (!cctrue (5)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12055;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel12055: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6600_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
	if (!cctrue (6)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12056;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel12056: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6601_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (6)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12057;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel12057: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_66ff_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
	if (!cctrue (6)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12058;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel12058: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6700_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
	if (!cctrue (7)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12059;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel12059: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6701_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (7)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12060;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel12060: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_67ff_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
	if (!cctrue (7)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12061;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel12061: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6800_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
	if (!cctrue (8)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12062;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel12062: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6801_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (8)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12063;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel12063: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_68ff_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
	if (!cctrue (8)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12064;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel12064: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6900_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
	if (!cctrue (9)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12065;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel12065: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6901_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (9)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12066;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel12066: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_69ff_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
	if (!cctrue (9)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12067;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel12067: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6a00_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
	if (!cctrue (10)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12068;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel12068: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6a01_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (10)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12069;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel12069: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_6aff_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
	if (!cctrue (10)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12070;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel12070: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6b00_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
	if (!cctrue (11)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12071;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel12071: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6b01_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (11)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12072;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel12072: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_6bff_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
	if (!cctrue (11)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12073;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel12073: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6c00_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
	if (!cctrue (12)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12074;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel12074: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6c01_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (12)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12075;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel12075: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_6cff_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
	if (!cctrue (12)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12076;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel12076: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6d00_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
	if (!cctrue (13)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12077;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel12077: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6d01_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (13)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12078;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel12078: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_6dff_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
	if (!cctrue (13)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12079;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel12079: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6e00_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
	if (!cctrue (14)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12080;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel12080: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6e01_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (14)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12081;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel12081: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_6eff_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
	if (!cctrue (14)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12082;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel12082: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6f00_31)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu040 (2);
	if (!cctrue (15)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12083;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel12083: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6f01_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (15)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12084;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel12084: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_6fff_31)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
	if (!cctrue (15)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel12085;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel12085: ;
return 12 * CYCLE_UNIT / 2;
}

/* MOVEQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_7000_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_u32 src = srcreg;
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* OR.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8000_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* OR.B (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8010_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* OR.B (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8018_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* OR.B -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8020_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (2);
return 10 * CYCLE_UNIT / 2;
}

/* OR.B (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8028_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.B (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8030_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* OR.B (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8038_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.B (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8039_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* OR.B (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_803a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.B (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_803b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_803c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8040_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* OR.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8050_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* OR.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8058_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* OR.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8060_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (2);
return 10 * CYCLE_UNIT / 2;
}

/* OR.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8068_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8070_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* OR.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8078_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8079_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* OR.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_807a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_807b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_807c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* OR.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8080_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* OR.L (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8090_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* OR.L (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8098_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* OR.L -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (2);
return 14 * CYCLE_UNIT / 2;
}

/* OR.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.L (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.L (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80b8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.L (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80b9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* OR.L (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80ba_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.L (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80bb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80bc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* DIVU.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel12120;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpci (2);
	}
}}}endlabel12120: ;
return 110 * CYCLE_UNIT / 2;
}

/* DIVU.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel12121;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpci (2);
	}
}}}}endlabel12121: ;
return 114 * CYCLE_UNIT / 2;
}

/* DIVU.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel12122;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpci (2);
	}
}}}}endlabel12122: ;
return 114 * CYCLE_UNIT / 2;
}

/* DIVU.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel12123;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpci (2);
	}
}}}}endlabel12123: ;
return 116 * CYCLE_UNIT / 2;
}

/* DIVU.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel12124;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpci (4);
	}
}}}}endlabel12124: ;
return 118 * CYCLE_UNIT / 2;
}

/* DIVU.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (0);
		Exception (5);
		goto endlabel12125;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
}}}}}endlabel12125: ;
return 118 * CYCLE_UNIT / 2;
}

/* DIVU.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80f8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel12126;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpci (4);
	}
}}}}endlabel12126: ;
return 118 * CYCLE_UNIT / 2;
}

/* DIVU.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80f9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (6);
		Exception (5);
		goto endlabel12127;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpci (6);
	}
}}}}endlabel12127: ;
return 122 * CYCLE_UNIT / 2;
}

/* DIVU.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80fa_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel12128;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpci (4);
	}
}}}}endlabel12128: ;
return 118 * CYCLE_UNIT / 2;
}

/* DIVU.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80fb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (0);
		Exception (5);
		goto endlabel12129;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
}}}}}endlabel12129: ;
return 118 * CYCLE_UNIT / 2;
}

/* DIVU.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80fc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel12130;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpci (4);
	}
}}}endlabel12130: ;
return 114 * CYCLE_UNIT / 2;
}

/* SBCD.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8100_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	uae_u16 newv_lo = (dst & 0xF) - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = (dst & 0xF0) - (src & 0xF0);
	uae_u16 newv, tmp_newv;
	int bcd = 0;
	newv = tmp_newv = newv_hi + newv_lo;
	if (newv_lo & 0xF0) { newv -= 6; bcd = 6; };
	if ((((dst & 0xFF) - (src & 0xFF) - (GET_XFLG () ? 1 : 0)) & 0x100) > 0xFF) { newv -= 0x60; }
	SET_CFLG ((((dst & 0xFF) - (src & 0xFF) - bcd - (GET_XFLG () ? 1 : 0)) & 0x300) > 0xFF);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SBCD.B -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_8108_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	m68k_areg (regs, dstreg) = dsta;
	mmufixup[0].reg = -1;
{	uae_u16 newv_lo = (dst & 0xF) - (src & 0xF) - (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = (dst & 0xF0) - (src & 0xF0);
	uae_u16 newv, tmp_newv;
	int bcd = 0;
	newv = tmp_newv = newv_hi + newv_lo;
	if (newv_lo & 0xF0) { newv -= 6; bcd = 6; };
	if ((((dst & 0xFF) - (src & 0xFF) - (GET_XFLG () ? 1 : 0)) & 0x100) > 0xFF) { newv -= 0x60; }
	SET_CFLG ((((dst & 0xFF) - (src & 0xFF) - bcd - (GET_XFLG () ? 1 : 0)) & 0x300) > 0xFF);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_8110_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* OR.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_8118_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* OR.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_8120_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* OR.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_8128_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_8130_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_8138_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_8139_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* PACK.L Dn,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_8140_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uae_u16 val = m68k_dreg (regs, srcreg) + get_iword_mmu040 (2);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & 0xffffff00) | ((val >> 4) & 0xf0) | (val & 0xf);
}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* PACK.L -(An),-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_8148_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uae_u16 val;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) -= areg_byteinc[srcreg];
	val = (uae_u16)get_byte_mmu040 (m68k_areg (regs, srcreg));
	m68k_areg (regs, srcreg) -= areg_byteinc[srcreg];
	val = (val | ((uae_u16)get_byte_mmu040 (m68k_areg (regs, srcreg)) << 8)) + get_iword_mmu040 (2);
	m68k_areg (regs, dstreg) -= areg_byteinc[dstreg];
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (m68k_areg (regs, dstreg),((val >> 4) & 0xf0) | (val & 0xf));
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* OR.W Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_8150_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* OR.W Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_8158_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* OR.W Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_8160_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* OR.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_8168_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.W Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_8170_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.W Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_8178_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.W Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_8179_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* UNPK.L Dn,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_8180_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uae_u16 val = m68k_dreg (regs, srcreg);
	val = (((val << 4) & 0xf00) | (val & 0xf)) + get_iword_mmu040 (2);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & 0xffff0000) | (val & 0xffff);
}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* UNPK.L -(An),-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_8188_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uae_u16 val;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) -= areg_byteinc[srcreg];
	val = (uae_u16)get_byte_mmu040 (m68k_areg (regs, srcreg));
	val = (((val << 4) & 0xf00) | (val & 0xf)) + get_iword_mmu040 (2);
	m68k_areg (regs, dstreg) -= 2 * areg_byteinc[dstreg];
	put_byte_mmu040 (m68k_areg (regs, dstreg) + areg_byteinc[dstreg], val);
	put_byte_mmu040 (m68k_areg (regs, dstreg), val >> 8);
}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* OR.L Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_8190_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* OR.L Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_8198_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* OR.L Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_81a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 22 * CYCLE_UNIT / 2;
}

/* OR.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_81a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* OR.L Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_81b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* OR.L Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_81b8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* OR.L Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_81b9_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* DIVS.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel12158;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
	m68k_incpci (2);
}}}endlabel12158: ;
return 142 * CYCLE_UNIT / 2;
}

/* DIVS.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel12159;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
	m68k_incpci (2);
}}}}endlabel12159: ;
return 146 * CYCLE_UNIT / 2;
}

/* DIVS.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel12160;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
	m68k_incpci (2);
}}}}endlabel12160: ;
return 146 * CYCLE_UNIT / 2;
}

/* DIVS.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel12161;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
	m68k_incpci (2);
}}}}endlabel12161: ;
return 148 * CYCLE_UNIT / 2;
}

/* DIVS.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel12162;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
	m68k_incpci (4);
}}}}endlabel12162: ;
return 150 * CYCLE_UNIT / 2;
}

/* DIVS.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (0);
		Exception (5);
		goto endlabel12163;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
}}}}}endlabel12163: ;
return 150 * CYCLE_UNIT / 2;
}

/* DIVS.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81f8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel12164;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
	m68k_incpci (4);
}}}}endlabel12164: ;
return 150 * CYCLE_UNIT / 2;
}

/* DIVS.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81f9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (6);
		Exception (5);
		goto endlabel12165;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
	m68k_incpci (6);
}}}}endlabel12165: ;
return 154 * CYCLE_UNIT / 2;
}

/* DIVS.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81fa_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel12166;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
	m68k_incpci (4);
}}}}endlabel12166: ;
return 150 * CYCLE_UNIT / 2;
}

/* DIVS.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81fb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (0);
		Exception (5);
		goto endlabel12167;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
}}}}}endlabel12167: ;
return 150 * CYCLE_UNIT / 2;
}

/* DIVS.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81fc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel12168;
	}
	CLEAR_CZNV ();
	if (dst == 0x80000000 && src == -1) {
		SET_VFLG (1);
		SET_NFLG (1);
	} else {
		uae_s32 newv = (uae_s32)dst / (uae_s32)(uae_s16)src;
		uae_u16 rem = (uae_s32)dst % (uae_s32)(uae_s16)src;
		if ((newv & 0xffff8000) != 0 && (newv & 0xffff8000) != 0xffff8000) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			if (((uae_s16)rem < 0) != ((uae_s32)dst < 0)) rem = -rem;
	optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	}
	m68k_incpci (4);
}}}endlabel12168: ;
return 146 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9000_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUB.B (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9010_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* SUB.B (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9018_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* SUB.B -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9020_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (2);
return 10 * CYCLE_UNIT / 2;
}

/* SUB.B (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9028_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.B (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9030_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUB.B (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9038_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.B (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9039_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.B (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_903a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.B (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_903b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_903c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9040_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUB.W An,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9048_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUB.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9050_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* SUB.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9058_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* SUB.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9060_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (2);
return 10 * CYCLE_UNIT / 2;
}

/* SUB.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9068_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9070_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUB.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9078_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9079_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_907a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_907b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_907c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9080_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUB.L An,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9088_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUB.L (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9090_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.L (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9098_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.L -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (2);
return 14 * CYCLE_UNIT / 2;
}

/* SUB.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.L (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.L (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90b8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.L (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90b9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SUB.L (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90ba_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.L (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90bb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90bc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* SUBA.W Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_90c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBA.W An,An */
uae_u32 REGPARAM2 CPUFUNC(op_90c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBA.W (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_90d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* SUBA.W (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_90d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	mmufixup[0].reg = -1;
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* SUBA.W -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_90e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	mmufixup[0].reg = -1;
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 10 * CYCLE_UNIT / 2;
}

/* SUBA.W (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_90e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUBA.W (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_90f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUBA.W (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_90f8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUBA.W (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_90f9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* SUBA.W (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_90fa_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUBA.W (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_90fb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUBA.W #<data>.W,An */
uae_u32 REGPARAM2 CPUFUNC(op_90fc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* SUBX.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9100_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBX.B -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9108_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	m68k_areg (regs, dstreg) = dsta;
	mmufixup[0].reg = -1;
{	uae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9110_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_9118_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9120_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 14 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_9128_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_9130_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_9138_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_9139_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* SUBX.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9140_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBX.W -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9148_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu040 (dsta);
	m68k_areg (regs, dstreg) = dsta;
	mmufixup[0].reg = -1;
{	uae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9150_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_9158_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9160_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, newv);
}}}}}}return 14 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_9168_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_9170_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_9178_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_9179_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* SUBX.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9180_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBX.L -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9188_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu040 (dsta);
	m68k_areg (regs, dstreg) = dsta;
	mmufixup[0].reg = -1;
{	uae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}}return 28 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9190_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_9198_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_91a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, newv);
}}}}}}return 22 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_91a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_91b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_91b8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_91b9_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 28 * CYCLE_UNIT / 2;
}

/* SUBA.L Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_91c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBA.L An,An */
uae_u32 REGPARAM2 CPUFUNC(op_91c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SUBA.L (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_91d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* SUBA.L (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_91d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	mmufixup[0].reg = -1;
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* SUBA.L -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_91e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	mmufixup[0].reg = -1;
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 14 * CYCLE_UNIT / 2;
}

/* SUBA.L (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_91e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUBA.L (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_91f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUBA.L (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_91f8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUBA.L (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_91f9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SUBA.L (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_91fa_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUBA.L (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_91fb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUBA.L #<data>.L,An */
uae_u32 REGPARAM2 CPUFUNC(op_91fc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b000_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMP.B (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b010_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* CMP.B (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b018_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* CMP.B -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b020_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (2);
return 10 * CYCLE_UNIT / 2;
}

/* CMP.B (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b028_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.B (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b030_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* CMP.B (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b038_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.B (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b039_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.B (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b03a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.B (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b03b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b03c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b040_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMP.W An,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b048_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMP.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b050_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* CMP.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b058_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* CMP.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b060_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (2);
return 10 * CYCLE_UNIT / 2;
}

/* CMP.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b068_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b070_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* CMP.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b078_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b079_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b07a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b07b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b07c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* CMP.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b080_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMP.L An,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b088_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

#endif

#ifdef PART_7
/* CMP.L (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b090_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.L (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b098_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.L -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
return 14 * CYCLE_UNIT / 2;
}

/* CMP.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.L (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* CMP.L (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0b8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.L (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0b9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* CMP.L (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0ba_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.L (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0bb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0bc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* CMPA.W Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_b0c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMPA.W An,An */
uae_u32 REGPARAM2 CPUFUNC(op_b0c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMPA.W (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_b0d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* CMPA.W (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_b0d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	mmufixup[0].reg = -1;
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* CMPA.W -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_b0e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	mmufixup[0].reg = -1;
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
return 10 * CYCLE_UNIT / 2;
}

/* CMPA.W (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_b0e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMPA.W (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_b0f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* CMPA.W (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_b0f8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMPA.W (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_b0f9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CMPA.W (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_b0fa_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMPA.W (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_b0fb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* CMPA.W #<data>.W,An */
uae_u32 REGPARAM2 CPUFUNC(op_b0fc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b100_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMPM.B (An)+,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_b108_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	mmufixup[0].reg = -1;
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_b110_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_b118_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_b120_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_b128_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_b130_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_b138_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_b139_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b140_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMPM.W (An)+,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_b148_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	m68k_areg (regs, dstreg) += 2;
	mmufixup[0].reg = -1;
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_b150_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_b158_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_b160_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_b168_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_b170_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_b178_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_b179_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b180_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMPM.L (An)+,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_b188_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	m68k_areg (regs, dstreg) += 4;
	mmufixup[0].reg = -1;
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_b190_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_b198_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_b1a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 22 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_b1a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_b1b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_b1b8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_b1b9_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* CMPA.L Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_b1c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMPA.L An,An */
uae_u32 REGPARAM2 CPUFUNC(op_b1c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CMPA.L (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_b1d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* CMPA.L (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_b1d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	mmufixup[0].reg = -1;
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* CMPA.L -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_b1e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	mmufixup[0].reg = -1;
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
return 14 * CYCLE_UNIT / 2;
}

/* CMPA.L (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_b1e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMPA.L (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_b1f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* CMPA.L (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_b1f8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMPA.L (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_b1f9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* CMPA.L (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_b1fa_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMPA.L (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_b1fb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* CMPA.L #<data>.L,An */
uae_u32 REGPARAM2 CPUFUNC(op_b1fc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* AND.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c000_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* AND.B (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c010_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* AND.B (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c018_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* AND.B -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c020_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (2);
return 10 * CYCLE_UNIT / 2;
}

/* AND.B (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c028_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.B (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c030_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* AND.B (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c038_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.B (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c039_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* AND.B (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c03a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.B (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c03b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c03c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c040_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* AND.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c050_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* AND.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c058_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* AND.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c060_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (2);
return 10 * CYCLE_UNIT / 2;
}

/* AND.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c068_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c070_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* AND.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c078_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c079_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* AND.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c07a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c07b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c07c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* AND.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c080_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* AND.L (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c090_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* AND.L (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c098_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* AND.L -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (2);
return 14 * CYCLE_UNIT / 2;
}

/* AND.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.L (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.L (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0b8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.L (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0b9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* AND.L (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0ba_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.L (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0bb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0bc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MULU.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (2);
}}}}return 58 * CYCLE_UNIT / 2;
}

/* MULU.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (2);
}}}}}return 62 * CYCLE_UNIT / 2;
}

/* MULU.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (2);
}}}}}return 62 * CYCLE_UNIT / 2;
}

/* MULU.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (2);
}}}}}return 64 * CYCLE_UNIT / 2;
}

/* MULU.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (4);
}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULU.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULU.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0f8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (4);
}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULU.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0f9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (6);
}}}}}return 70 * CYCLE_UNIT / 2;
}

/* MULU.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0fa_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (4);
}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULU.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0fb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULU.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0fc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (4);
}}}}return 62 * CYCLE_UNIT / 2;
}

/* ABCD.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c100_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	uae_u16 newv_lo = (src & 0xF) + (dst & 0xF) + (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = (src & 0xF0) + (dst & 0xF0);
	uae_u16 newv, tmp_newv;
	int cflg;
	newv = tmp_newv = newv_hi + newv_lo;	if (newv_lo > 9) { newv += 6; }
	cflg = (newv & 0x3F0) > 0x90;
	if (cflg) newv += 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ABCD.B -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c108_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	m68k_areg (regs, dstreg) = dsta;
	mmufixup[0].reg = -1;
{	uae_u16 newv_lo = (src & 0xF) + (dst & 0xF) + (GET_XFLG () ? 1 : 0);
	uae_u16 newv_hi = (src & 0xF0) + (dst & 0xF0);
	uae_u16 newv, tmp_newv;
	int cflg;
	newv = tmp_newv = newv_hi + newv_lo;	if (newv_lo > 9) { newv += 6; }
	cflg = (newv & 0x3F0) > 0x90;
	if (cflg) newv += 0x60;
	SET_CFLG (cflg);
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c110_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* AND.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_c118_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* AND.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c120_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, src);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* AND.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_c128_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_c130_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_c138_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_c139_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* EXG.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c140_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_dreg (regs, srcreg) = (dst);
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* EXG.L An,An */
uae_u32 REGPARAM2 CPUFUNC(op_c148_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	m68k_areg (regs, srcreg) = (dst);
	m68k_areg (regs, dstreg) = (src);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* AND.W Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c150_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* AND.W Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_c158_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* AND.W Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c160_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, src);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* AND.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_c168_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.W Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_c170_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.W Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_c178_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.W Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_c179_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* EXG.L Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_c188_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	m68k_dreg (regs, srcreg) = (dst);
	m68k_areg (regs, dstreg) = (src);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* AND.L Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c190_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* AND.L Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_c198_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* AND.L Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c1a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, src);
}}}}return 22 * CYCLE_UNIT / 2;
}

/* AND.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_c1a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* AND.L Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_c1b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* AND.L Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_c1b8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* AND.L Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_c1b9_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, src);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MULS.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 58 * CYCLE_UNIT / 2;
}

/* MULS.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 62 * CYCLE_UNIT / 2;
}

/* MULS.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 62 * CYCLE_UNIT / 2;
}

/* MULS.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 64 * CYCLE_UNIT / 2;
}

/* MULS.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 66 * CYCLE_UNIT / 2;
}

/* MULS.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULS.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1f8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 66 * CYCLE_UNIT / 2;
}

/* MULS.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1f9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 70 * CYCLE_UNIT / 2;
}

/* MULS.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1fa_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 66 * CYCLE_UNIT / 2;
}

/* MULS.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1fb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULS.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1fc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}	m68k_incpci (4);
return 62 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d000_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADD.B (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d010_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* ADD.B (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d018_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* ADD.B -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d020_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (2);
return 10 * CYCLE_UNIT / 2;
}

/* ADD.B (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d028_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.B (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d030_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADD.B (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d038_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.B (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d039_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.B (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d03a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.B (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d03b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s8 src = get_byte_mmu040 (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d03c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu040 (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d040_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADD.W An,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d048_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADD.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d050_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* ADD.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d058_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* ADD.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d060_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (2);
return 10 * CYCLE_UNIT / 2;
}

/* ADD.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d068_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d070_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADD.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d078_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d079_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d07a_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d07b_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d07c_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d080_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADD.L An,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d088_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADD.L (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d090_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.L (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d098_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.L -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	mmufixup[0].reg = -1;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (2);
return 14 * CYCLE_UNIT / 2;
}

/* ADD.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.L (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.L (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0b8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.L (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0b9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ADD.L (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0ba_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.L (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0bb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0bc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* ADDA.W Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_d0c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDA.W An,An */
uae_u32 REGPARAM2 CPUFUNC(op_d0c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDA.W (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_d0d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* ADDA.W (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_d0d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	mmufixup[0].reg = -1;
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* ADDA.W -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_d0e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	mmufixup[0].reg = -1;
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 10 * CYCLE_UNIT / 2;
}

/* ADDA.W (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_d0e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADDA.W (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_d0f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADDA.W (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_d0f8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADDA.W (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_d0f9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* ADDA.W (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_d0fa_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADDA.W (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_d0fb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s16 src = get_word_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADDA.W #<data>.W,An */
uae_u32 REGPARAM2 CPUFUNC(op_d0fc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu040 (2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* ADDX.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d100_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDX.B -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d108_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	m68k_areg (regs, dstreg) = dsta;
	mmufixup[0].reg = -1;
{	uae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d110_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_d118_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d120_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 14 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_d128_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_d130_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_d138_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_d139_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s8 dst = get_byte_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_byte_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* ADDX.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d140_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDX.W -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d148_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu040 (dsta);
	m68k_areg (regs, dstreg) = dsta;
	mmufixup[0].reg = -1;
{	uae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d150_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_d158_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d160_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dsta, newv);
}}}}}}return 14 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_d168_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_d170_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_d178_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_d179_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s16 dst = get_word_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* ADDX.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d180_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDX.L -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d188_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu040 (dsta);
	m68k_areg (regs, dstreg) = dsta;
	mmufixup[0].reg = -1;
{	uae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}}return 28 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d190_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_d198_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, newv);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d1a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu040 (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_long_mmu040 (dsta, newv);
}}}}}}return 22 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_d1a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_d1b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_d1b8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_d1b9_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (2);
{	uae_s32 dst = get_long_mmu040 (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_long_mmu040 (dsta, newv);
}}}}}}return 28 * CYCLE_UNIT / 2;
}

/* ADDA.L Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_d1c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDA.L An,An */
uae_u32 REGPARAM2 CPUFUNC(op_d1c8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ADDA.L (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_d1d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ADDA.L (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_d1d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	mmufixup[0].reg = -1;
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ADDA.L -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_d1e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu040 (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	mmufixup[0].reg = -1;
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 14 * CYCLE_UNIT / 2;
}

/* ADDA.L (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_d1e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADDA.L (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_d1f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADDA.L (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_d1f8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADDA.L (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_d1f9_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ADDA.L (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_d1fa_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADDA.L (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_d1fb_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = x_get_disp_ea_020 (tmppc, 0);
{	uae_s32 src = get_long_mmu040 (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADDA.L #<data>.L,An */
uae_u32 REGPARAM2 CPUFUNC(op_d1fc_31)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu040 (2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* ASRQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e000_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	uae_u32 sign = (0x80 & val) >> 7;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 8) {
		val = 0xff & (uae_u32)-sign;
		SET_CFLG (sign);
		COPY_CARRY ();
	} else {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
		val |= (0xff << (8 - cnt)) & (uae_u32)-sign;
		val &= 0xff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSRQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e008_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 8) {
		SET_CFLG ((cnt == 8) & (val >> 7));
		COPY_CARRY ();
		val = 0;
	} else {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXRQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e010_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
{	cnt--;
	{
	uae_u32 carry;
	uae_u32 hival = (val << 1) | GET_XFLG ();
	hival <<= (7 - cnt);
	val >>= cnt;
	carry = val & 1;
	val >>= 1;
	val |= hival;
	SET_XFLG (carry);
	val &= 0xff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* RORQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e018_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
{	uae_u32 hival;
	cnt &= 7;
	hival = val << (8 - cnt);
	val >>= cnt;
	val |= hival;
	val &= 0xff;
	SET_CFLG ((val & 0x80) >> 7);
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASR.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e020_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	uae_u32 sign = (0x80 & val) >> 7;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 8) {
		val = 0xff & (uae_u32)-sign;
		SET_CFLG (sign);
		COPY_CARRY ();
	} else if (cnt > 0) {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
		val |= (0xff << (8 - cnt)) & (uae_u32)-sign;
		val &= 0xff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSR.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e028_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 8) {
		SET_CFLG ((cnt == 8) & (val >> 7));
		COPY_CARRY ();
		val = 0;
	} else if (cnt > 0) {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXR.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e030_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 36) cnt -= 36;
	if (cnt >= 18) cnt -= 18;
	if (cnt >= 9) cnt -= 9;
	if (cnt > 0) {
	cnt--;
	{
	uae_u32 carry;
	uae_u32 hival = (val << 1) | GET_XFLG ();
	hival <<= (7 - cnt);
	val >>= cnt;
	carry = val & 1;
	val >>= 1;
	val |= hival;
	SET_XFLG (carry);
	val &= 0xff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

#endif

#ifdef PART_8
/* ROR.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e038_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt > 0) {	uae_u32 hival;
	cnt &= 7;
	hival = val << (8 - cnt);
	val >>= cnt;
	val |= hival;
	val &= 0xff;
	SET_CFLG ((val & 0x80) >> 7);
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASRQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e040_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = (0x8000 & val) >> 15;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 16) {
		val = 0xffff & (uae_u32)-sign;
		SET_CFLG (sign);
		COPY_CARRY ();
	} else {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
		val |= (0xffff << (16 - cnt)) & (uae_u32)-sign;
		val &= 0xffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSRQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e048_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 16) {
		SET_CFLG ((cnt == 16) & (val >> 15));
		COPY_CARRY ();
		val = 0;
	} else {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXRQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e050_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
{	cnt--;
	{
	uae_u32 carry;
	uae_u32 hival = (val << 1) | GET_XFLG ();
	hival <<= (15 - cnt);
	val >>= cnt;
	carry = val & 1;
	val >>= 1;
	val |= hival;
	SET_XFLG (carry);
	val &= 0xffff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* RORQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e058_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
{	uae_u32 hival;
	cnt &= 15;
	hival = val << (16 - cnt);
	val >>= cnt;
	val |= hival;
	val &= 0xffff;
	SET_CFLG ((val & 0x8000) >> 15);
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASR.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e060_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = (0x8000 & val) >> 15;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 16) {
		val = 0xffff & (uae_u32)-sign;
		SET_CFLG (sign);
		COPY_CARRY ();
	} else if (cnt > 0) {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
		val |= (0xffff << (16 - cnt)) & (uae_u32)-sign;
		val &= 0xffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSR.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e068_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 16) {
		SET_CFLG ((cnt == 16) & (val >> 15));
		COPY_CARRY ();
		val = 0;
	} else if (cnt > 0) {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXR.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e070_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 34) cnt -= 34;
	if (cnt >= 17) cnt -= 17;
	if (cnt > 0) {
	cnt--;
	{
	uae_u32 carry;
	uae_u32 hival = (val << 1) | GET_XFLG ();
	hival <<= (15 - cnt);
	val >>= cnt;
	carry = val & 1;
	val >>= 1;
	val |= hival;
	SET_XFLG (carry);
	val &= 0xffff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROR.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e078_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt > 0) {	uae_u32 hival;
	cnt &= 15;
	hival = val << (16 - cnt);
	val >>= cnt;
	val |= hival;
	val &= 0xffff;
	SET_CFLG ((val & 0x8000) >> 15);
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASRQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e080_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	uae_u32 sign = (0x80000000 & val) >> 31;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 32) {
		val = 0xffffffff & (uae_u32)-sign;
		SET_CFLG (sign);
		COPY_CARRY ();
	} else {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
		val |= (0xffffffff << (32 - cnt)) & (uae_u32)-sign;
		val &= 0xffffffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSRQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e088_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 32) {
		SET_CFLG ((cnt == 32) & (val >> 31));
		COPY_CARRY ();
		val = 0;
	} else {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXRQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e090_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
{	cnt--;
	{
	uae_u32 carry;
	uae_u32 hival = (val << 1) | GET_XFLG ();
	hival <<= (31 - cnt);
	val >>= cnt;
	carry = val & 1;
	val >>= 1;
	val |= hival;
	SET_XFLG (carry);
	val &= 0xffffffff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* RORQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e098_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
{	uae_u32 hival;
	cnt &= 31;
	hival = val << (32 - cnt);
	val >>= cnt;
	val |= hival;
	val &= 0xffffffff;
	SET_CFLG ((val & 0x80000000) >> 31);
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASR.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e0a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	uae_u32 sign = (0x80000000 & val) >> 31;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 32) {
		val = 0xffffffff & (uae_u32)-sign;
		SET_CFLG (sign);
		COPY_CARRY ();
	} else if (cnt > 0) {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
		val |= (0xffffffff << (32 - cnt)) & (uae_u32)-sign;
		val &= 0xffffffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSR.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e0a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 32) {
		SET_CFLG ((cnt == 32) & (val >> 31));
		COPY_CARRY ();
		val = 0;
	} else if (cnt > 0) {
		val >>= cnt - 1;
		SET_CFLG (val & 1);
		COPY_CARRY ();
		val >>= 1;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXR.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e0b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 33) cnt -= 33;
	if (cnt > 0) {
	cnt--;
	{
	uae_u32 carry;
	uae_u32 hival = (val << 1) | GET_XFLG ();
	hival <<= (31 - cnt);
	val >>= cnt;
	carry = val & 1;
	val >>= 1;
	val |= hival;
	SET_XFLG (carry);
	val &= 0xffffffff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROR.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e0b8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt > 0) {	uae_u32 hival;
	cnt &= 31;
	hival = val << (32 - cnt);
	val >>= cnt;
	val |= hival;
	val &= 0xffffffff;
	SET_CFLG ((val & 0x80000000) >> 31);
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASRW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e0d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* ASRW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e0d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu040 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dataa, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* ASRW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e0e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_word_mmu040 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dataa, val);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* ASRW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e0e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* ASRW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e0f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ASRW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e0f8_31)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* ASRW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e0f9_31)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* ASLQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e100_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 8) {
		SET_VFLG (val != 0);
		SET_CFLG (cnt == 8 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else {
		uae_u32 mask = (0xff << (7 - cnt)) & 0xff;
		SET_VFLG ((val & mask) != mask && (val & mask) != 0);
		val <<= cnt - 1;
		SET_CFLG ((val & 0x80) >> 7);
		COPY_CARRY ();
		val <<= 1;
		val &= 0xff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSLQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e108_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 8) {
		SET_CFLG (cnt == 8 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else {
		val <<= (cnt - 1);
		SET_CFLG ((val & 0x80) >> 7);
		COPY_CARRY ();
		val <<= 1;
	val &= 0xff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXLQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e110_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
{	cnt--;
	{
	uae_u32 carry;
	uae_u32 loval = val >> (7 - cnt);
	carry = loval & 1;
	val = (((val << 1) | GET_XFLG ()) << cnt) | (loval >> 1);
	SET_XFLG (carry);
	val &= 0xff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROLQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e118_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
{	uae_u32 loval;
	cnt &= 7;
	loval = val >> (8 - cnt);
	val <<= cnt;
	val |= loval;
	val &= 0xff;
	SET_CFLG (val & 1);
}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASL.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e120_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 8) {
		SET_VFLG (val != 0);
		SET_CFLG (cnt == 8 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else if (cnt > 0) {
		uae_u32 mask = (0xff << (7 - cnt)) & 0xff;
		SET_VFLG ((val & mask) != mask && (val & mask) != 0);
		val <<= cnt - 1;
		SET_CFLG ((val & 0x80) >> 7);
		COPY_CARRY ();
		val <<= 1;
		val &= 0xff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSL.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e128_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 8) {
		SET_CFLG (cnt == 8 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else if (cnt > 0) {
		val <<= (cnt - 1);
		SET_CFLG ((val & 0x80) >> 7);
		COPY_CARRY ();
		val <<= 1;
	val &= 0xff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXL.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e130_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 36) cnt -= 36;
	if (cnt >= 18) cnt -= 18;
	if (cnt >= 9) cnt -= 9;
	if (cnt > 0) {
	cnt--;
	{
	uae_u32 carry;
	uae_u32 loval = val >> (7 - cnt);
	carry = loval & 1;
	val = (((val << 1) | GET_XFLG ()) << cnt) | (loval >> 1);
	SET_XFLG (carry);
	val &= 0xff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROL.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e138_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u8)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt > 0) {
	uae_u32 loval;
	cnt &= 7;
	loval = val >> (8 - cnt);
	val <<= cnt;
	val |= loval;
	val &= 0xff;
	SET_CFLG (val & 1);
}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testb ((uae_s8)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASLQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e140_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 16) {
		SET_VFLG (val != 0);
		SET_CFLG (cnt == 16 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else {
		uae_u32 mask = (0xffff << (15 - cnt)) & 0xffff;
		SET_VFLG ((val & mask) != mask && (val & mask) != 0);
		val <<= cnt - 1;
		SET_CFLG ((val & 0x8000) >> 15);
		COPY_CARRY ();
		val <<= 1;
		val &= 0xffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSLQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e148_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 16) {
		SET_CFLG (cnt == 16 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else {
		val <<= (cnt - 1);
		SET_CFLG ((val & 0x8000) >> 15);
		COPY_CARRY ();
		val <<= 1;
	val &= 0xffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXLQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e150_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
{	cnt--;
	{
	uae_u32 carry;
	uae_u32 loval = val >> (15 - cnt);
	carry = loval & 1;
	val = (((val << 1) | GET_XFLG ()) << cnt) | (loval >> 1);
	SET_XFLG (carry);
	val &= 0xffff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROLQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e158_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
{	uae_u32 loval;
	cnt &= 15;
	loval = val >> (16 - cnt);
	val <<= cnt;
	val |= loval;
	val &= 0xffff;
	SET_CFLG (val & 1);
}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASL.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e160_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 16) {
		SET_VFLG (val != 0);
		SET_CFLG (cnt == 16 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else if (cnt > 0) {
		uae_u32 mask = (0xffff << (15 - cnt)) & 0xffff;
		SET_VFLG ((val & mask) != mask && (val & mask) != 0);
		val <<= cnt - 1;
		SET_CFLG ((val & 0x8000) >> 15);
		COPY_CARRY ();
		val <<= 1;
		val &= 0xffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSL.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e168_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 16) {
		SET_CFLG (cnt == 16 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else if (cnt > 0) {
		val <<= (cnt - 1);
		SET_CFLG ((val & 0x8000) >> 15);
		COPY_CARRY ();
		val <<= 1;
	val &= 0xffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXL.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e170_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 34) cnt -= 34;
	if (cnt >= 17) cnt -= 17;
	if (cnt > 0) {
	cnt--;
	{
	uae_u32 carry;
	uae_u32 loval = val >> (15 - cnt);
	carry = loval & 1;
	val = (((val << 1) | GET_XFLG ()) << cnt) | (loval >> 1);
	SET_XFLG (carry);
	val &= 0xffff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROL.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e178_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = (uae_u16)data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt > 0) {
	uae_u32 loval;
	cnt &= 15;
	loval = val >> (16 - cnt);
	val <<= cnt;
	val |= loval;
	val &= 0xffff;
	SET_CFLG (val & 1);
}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testw ((uae_s16)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASLQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e180_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 32) {
		SET_VFLG (val != 0);
		SET_CFLG (cnt == 32 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else {
		uae_u32 mask = (0xffffffff << (31 - cnt)) & 0xffffffff;
		SET_VFLG ((val & mask) != mask && (val & mask) != 0);
		val <<= cnt - 1;
		SET_CFLG ((val & 0x80000000) >> 31);
		COPY_CARRY ();
		val <<= 1;
		val &= 0xffffffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSLQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e188_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 32) {
		SET_CFLG (cnt == 32 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else {
		val <<= (cnt - 1);
		SET_CFLG ((val & 0x80000000) >> 31);
		COPY_CARRY ();
		val <<= 1;
	val &= 0xffffffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXLQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e190_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
{	cnt--;
	{
	uae_u32 carry;
	uae_u32 loval = val >> (31 - cnt);
	carry = loval & 1;
	val = (((val << 1) | GET_XFLG ()) << cnt) | (loval >> 1);
	SET_XFLG (carry);
	val &= 0xffffffff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROLQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e198_31)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
{	uae_u32 loval;
	cnt &= 31;
	loval = val >> (32 - cnt);
	val <<= cnt;
	val |= loval;
	val &= 0xffffffff;
	SET_CFLG (val & 1);
}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASL.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e1a0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 32) {
		SET_VFLG (val != 0);
		SET_CFLG (cnt == 32 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else if (cnt > 0) {
		uae_u32 mask = (0xffffffff << (31 - cnt)) & 0xffffffff;
		SET_VFLG ((val & mask) != mask && (val & mask) != 0);
		val <<= cnt - 1;
		SET_CFLG ((val & 0x80000000) >> 31);
		COPY_CARRY ();
		val <<= 1;
		val &= 0xffffffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LSL.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e1a8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 32) {
		SET_CFLG (cnt == 32 ? val & 1 : 0);
		COPY_CARRY ();
		val = 0;
	} else if (cnt > 0) {
		val <<= (cnt - 1);
		SET_CFLG ((val & 0x80000000) >> 31);
		COPY_CARRY ();
		val <<= 1;
	val &= 0xffffffff;
	}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROXL.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e1b0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt >= 33) cnt -= 33;
	if (cnt > 0) {
	cnt--;
	{
	uae_u32 carry;
	uae_u32 loval = val >> (31 - cnt);
	carry = loval & 1;
	val = (((val << 1) | GET_XFLG ()) << cnt) | (loval >> 1);
	SET_XFLG (carry);
	val &= 0xffffffff;
	} }
	SET_CFLG (GET_XFLG ());
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ROL.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e1b8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
{	uae_u32 val = data;
	cnt &= 63;
	CLEAR_CZNV ();
	if (cnt > 0) {
	uae_u32 loval;
	cnt &= 31;
	loval = val >> (32 - cnt);
	val <<= cnt;
	val |= loval;
	val &= 0xffffffff;
	SET_CFLG (val & 1);
}
	{uae_u32 oldcznv = GET_CZNV & ~(FLAGVAL_Z | FLAGVAL_N);
	optflag_testl ((uae_s32)(val));
	IOR_CZNV (oldcznv);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ASLW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e1d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* ASLW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e1d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu040 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dataa, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* ASLW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e1e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_word_mmu040 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dataa, val);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* ASLW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e1e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* ASLW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e1f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ASLW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e1f8_31)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* ASLW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e1f9_31)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* LSRW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e2d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* LSRW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e2d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu040 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dataa, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* LSRW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e2e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_word_mmu040 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dataa, val);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* LSRW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e2e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* LSRW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e2f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* LSRW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e2f8_31)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* LSRW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e2f9_31)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* LSLW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e3d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* LSLW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e3d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu040 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dataa, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* LSLW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e3e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_word_mmu040 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dataa, val);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* LSLW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e3e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* LSLW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e3f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* LSLW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e3f8_31)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* LSLW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e3f9_31)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* ROXRW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e4d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* ROXRW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e4d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu040 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dataa, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* ROXRW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e4e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_word_mmu040 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dataa, val);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* ROXRW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e4e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* ROXRW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e4f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ROXRW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e4f8_31)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* ROXRW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e4f9_31)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* ROXLW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e5d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* ROXLW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e5d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu040 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dataa, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* ROXLW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e5e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_word_mmu040 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dataa, val);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* ROXLW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e5e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* ROXLW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e5f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ROXLW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e5f8_31)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* ROXLW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e5f9_31)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* RORW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e6d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* RORW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e6d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu040 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dataa, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* RORW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e6e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_word_mmu040 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dataa, val);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* RORW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e6e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* RORW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e6f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* RORW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e6f8_31)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* RORW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e6f9_31)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* ROLW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e7d0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* ROLW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e7d8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu040 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dataa, val);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* ROLW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e7e0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_word_mmu040 (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	m68k_incpci (2);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	mmufixup[0].reg = -1;
	put_word_mmu040 (dataa, val);
}}}}return 14 * CYCLE_UNIT / 2;
}

/* ROLW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e7e8_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* ROLW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e7f0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ROLW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e7f8_31)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	m68k_incpci (4);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* ROLW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e7f9_31)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu040 (2);
{	uae_s16 data = get_word_mmu040 (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	m68k_incpci (6);
	regs.instruction_pc = m68k_getpci ();
	mmu_restart = false;
	put_word_mmu040 (dataa, val);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* BFTST.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e8c0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{{	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp = m68k_dreg(regs, dstreg);
	offset &= 0x1f;
	tmp = (tmp << offset) | (tmp >> (32 - offset));
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFTST.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e8d0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFTST.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e8e8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFTST.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e8f0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFTST.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e8f8_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFTST.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e8f9_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
}}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* BFTST.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e8fa_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFTST.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e8fb_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTU.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e9c0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{{	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp = m68k_dreg(regs, dstreg);
	offset &= 0x1f;
	tmp = (tmp << offset) | (tmp >> (32 - offset));
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTU.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e9d0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTU.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e9e8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTU.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e9f0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTU.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e9f8_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTU.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e9f9_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTU.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e9fa_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTU.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e9fb_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFCHG.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eac0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp = m68k_dreg(regs, dstreg);
	offset &= 0x1f;
	tmp = (tmp << offset) | (tmp >> (32 - offset));
	bdata[0] = tmp & ((1 << (32 - width)) - 1);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = tmp ^ (0xffffffffu >> (32 - width));
	tmp = bdata[0] | (tmp << (32 - width));
	m68k_dreg(regs, dstreg) = (tmp >> offset) | (tmp << (32 - offset));
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFCHG.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ead0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = tmp ^ (0xffffffffu >> (32 - width));
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFCHG.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eae8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = tmp ^ (0xffffffffu >> (32 - width));
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFCHG.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eaf0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = tmp ^ (0xffffffffu >> (32 - width));
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFCHG.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eaf8_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = tmp ^ (0xffffffffu >> (32 - width));
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFCHG.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eaf9_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = tmp ^ (0xffffffffu >> (32 - width));
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTS.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ebc0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{{	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp = m68k_dreg(regs, dstreg);
	offset &= 0x1f;
	tmp = (tmp << offset) | (tmp >> (32 - offset));
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp = (uae_s32)tmp >> (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTS.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ebd0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp = (uae_s32)tmp >> (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTS.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ebe8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp = (uae_s32)tmp >> (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTS.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ebf0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp = (uae_s32)tmp >> (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTS.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ebf8_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp = (uae_s32)tmp >> (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTS.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ebf9_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp = (uae_s32)tmp >> (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTS.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ebfa_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp = (uae_s32)tmp >> (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFEXTS.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ebfb_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp = (uae_s32)tmp >> (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	m68k_dreg (regs, (extra >> 12) & 7) = tmp;
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFCLR.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ecc0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp = m68k_dreg(regs, dstreg);
	offset &= 0x1f;
	tmp = (tmp << offset) | (tmp >> (32 - offset));
	bdata[0] = tmp & ((1 << (32 - width)) - 1);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0;
	tmp = bdata[0] | (tmp << (32 - width));
	m68k_dreg(regs, dstreg) = (tmp >> offset) | (tmp << (32 - offset));
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFCLR.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ecd0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0;
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFCLR.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ece8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0;
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFCLR.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ecf0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0;
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFCLR.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ecf8_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0;
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFCLR.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ecf9_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0;
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* BFFFO.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_edc0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{{	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp = m68k_dreg(regs, dstreg);
	offset &= 0x1f;
	tmp = (tmp << offset) | (tmp >> (32 - offset));
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	{ uae_u32 mask = 1 << (width - 1);
	while (mask) { if (tmp & mask) break; mask >>= 1; offset++; }}
	m68k_dreg (regs, (extra >> 12) & 7) = offset;
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFFFO.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_edd0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	{ uae_u32 mask = 1 << (width - 1);
	while (mask) { if (tmp & mask) break; mask >>= 1; offset++; }}
	m68k_dreg (regs, (extra >> 12) & 7) = offset;
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFFFO.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_ede8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	{ uae_u32 mask = 1 << (width - 1);
	while (mask) { if (tmp & mask) break; mask >>= 1; offset++; }}
	m68k_dreg (regs, (extra >> 12) & 7) = offset;
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFFFO.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_edf0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	{ uae_u32 mask = 1 << (width - 1);
	while (mask) { if (tmp & mask) break; mask >>= 1; offset++; }}
	m68k_dreg (regs, (extra >> 12) & 7) = offset;
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFFFO.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_edf8_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	{ uae_u32 mask = 1 << (width - 1);
	while (mask) { if (tmp & mask) break; mask >>= 1; offset++; }}
	m68k_dreg (regs, (extra >> 12) & 7) = offset;
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFFFO.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_edf9_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	{ uae_u32 mask = 1 << (width - 1);
	while (mask) { if (tmp & mask) break; mask >>= 1; offset++; }}
	m68k_dreg (regs, (extra >> 12) & 7) = offset;
}}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* BFFFO.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_edfa_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	{ uae_u32 mask = 1 << (width - 1);
	while (mask) { if (tmp & mask) break; mask >>= 1; offset++; }}
	m68k_dreg (regs, (extra >> 12) & 7) = offset;
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFFFO.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_edfb_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = x_get_disp_ea_020 (tmppc, 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	{ uae_u32 mask = 1 << (width - 1);
	while (mask) { if (tmp & mask) break; mask >>= 1; offset++; }}
	m68k_dreg (regs, (extra >> 12) & 7) = offset;
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFSET.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eec0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp = m68k_dreg(regs, dstreg);
	offset &= 0x1f;
	tmp = (tmp << offset) | (tmp >> (32 - offset));
	bdata[0] = tmp & ((1 << (32 - width)) - 1);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0xffffffffu >> (32 - width);
	tmp = bdata[0] | (tmp << (32 - width));
	m68k_dreg(regs, dstreg) = (tmp >> offset) | (tmp << (32 - offset));
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFSET.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eed0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0xffffffffu >> (32 - width);
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFSET.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eee8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0xffffffffu >> (32 - width);
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFSET.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eef0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0xffffffffu >> (32 - width);
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFSET.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eef8_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0xffffffffu >> (32 - width);
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFSET.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eef9_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = 0xffffffffu >> (32 - width);
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* BFINS.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_efc0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp = m68k_dreg(regs, dstreg);
	offset &= 0x1f;
	tmp = (tmp << offset) | (tmp >> (32 - offset));
	bdata[0] = tmp & ((1 << (32 - width)) - 1);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = m68k_dreg (regs, (extra >> 12) & 7);
	tmp = tmp & (0xffffffffu >> (32 - width));
	SET_NFLG (tmp & (1 << (width - 1)) ? 1 : 0);
	SET_ZFLG (tmp == 0);
	tmp = bdata[0] | (tmp << (32 - width));
	m68k_dreg(regs, dstreg) = (tmp >> offset) | (tmp << (32 - offset));
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFINS.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_efd0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = m68k_dreg (regs, (extra >> 12) & 7);
	tmp = tmp & (0xffffffffu >> (32 - width));
	SET_NFLG (tmp & (1 << (width - 1)) ? 1 : 0);
	SET_ZFLG (tmp == 0);
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* BFINS.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_efe8_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = m68k_dreg (regs, (extra >> 12) & 7);
	tmp = tmp & (0xffffffffu >> (32 - width));
	SET_NFLG (tmp & (1 << (width - 1)) ? 1 : 0);
	SET_ZFLG (tmp == 0);
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFINS.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eff0_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = x_get_disp_ea_020 (m68k_areg (regs, dstreg), 0);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = m68k_dreg (regs, (extra >> 12) & 7);
	tmp = tmp & (0xffffffffu >> (32 - width));
	SET_NFLG (tmp & (1 << (width - 1)) ? 1 : 0);
	SET_ZFLG (tmp == 0);
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFINS.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eff8_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = m68k_dreg (regs, (extra >> 12) & 7);
	tmp = tmp & (0xffffffffu >> (32 - width));
	SET_NFLG (tmp & (1 << (width - 1)) ? 1 : 0);
	SET_ZFLG (tmp == 0);
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BFINS.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_eff9_31)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu040 (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu040 (4);
{	uae_u32 bdata[2];
	uae_s32 offset = extra & 0x800 ? m68k_dreg(regs, (extra >> 6) & 7) : (extra >> 6) & 0x1f;
	int width = (((extra & 0x20 ? (int)m68k_dreg(regs, extra & 7) : extra) -1) & 0x1f) +1;
	uae_u32 tmp;
	dsta += offset >> 3;
	tmp = x_get_bitfield (dsta, bdata, offset, width);
	SET_NFLG_ALWAYS (((uae_s32)tmp) < 0 ? 1 : 0);
	tmp >>= (32 - width);
	SET_ZFLG (tmp == 0); SET_VFLG (0); SET_CFLG (0);
	tmp = m68k_dreg (regs, (extra >> 12) & 7);
	tmp = tmp & (0xffffffffu >> (32 - width));
	SET_NFLG (tmp & (1 << (width - 1)) ? 1 : 0);
	SET_ZFLG (tmp == 0);
	x_put_bitfield(dsta, bdata, tmp, offset, width);
}}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L Dn,#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f000_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12668; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	uae_u16 extraa = 0;
	mmu_op30 (pc, opcode, extra, extraa);
}}endlabel12668: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L An,#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f008_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12669; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	uae_u16 extraa = 0;
	mmu_op30 (pc, opcode, extra, extraa);
}}endlabel12669: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L (An),#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f010_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12670; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
{	uaecptr extraa;
	extraa = m68k_areg (regs, srcreg);
	mmu_op30 (pc, opcode, extra, extraa);
}}}endlabel12670: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L (An)+,#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f018_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12671; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
{	uaecptr extraa;
	extraa = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
	mmu_op30 (pc, opcode, extra, extraa);
}}}endlabel12671: ;
	mmufixup[0].reg = -1;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L -(An),#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f020_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12672; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
{	uaecptr extraa;
	extraa = m68k_areg (regs, srcreg) - 4;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = extraa;
	mmu_op30 (pc, opcode, extra, extraa);
}}}endlabel12672: ;
	mmufixup[0].reg = -1;
return 6 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L (d16,An),#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f028_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12673; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
{	uaecptr extraa;
	extraa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu040 (0);
	m68k_incpci (2);
	mmu_op30 (pc, opcode, extra, extraa);
}}}endlabel12673: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L (d8,An,Xn),#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f030_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12674; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
{	uaecptr extraa;
{	extraa = x_get_disp_ea_020 (m68k_areg (regs, srcreg), 0);
	mmu_op30 (pc, opcode, extra, extraa);
}}}}endlabel12674: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L (xxx).W,#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f038_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel12675; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
{	uaecptr extraa;
	extraa = (uae_s32)(uae_s16)get_iword_mmu040 (0);
	m68k_incpci (2);
	mmu_op30 (pc, opcode, extra, extraa);
}}}endlabel12675: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L (xxx).L,#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f039_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel12676; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
{	uaecptr extraa;
	extraa = get_ilong_mmu040 (0);
	m68k_incpci (4);
	mmu_op30 (pc, opcode, extra, extraa);
}}}endlabel12676: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f200_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f208_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f210_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f218_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f220_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f228_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f230_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f238_31)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f239_31)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f23a_31)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f23b_31)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,#<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f23c_31)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f240_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FDBcc.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f248_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_dbcc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f250_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f258_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f260_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f268_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f270_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f278_31)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f279_31)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FTRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f27a_31)(uae_u32 opcode)
{
{
#ifdef FPUEMU
	uaecptr oldpc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu040 (2);
{	uae_s16 dummy = get_iword_mmu040 (4);
	m68k_incpci (6);
	fpuop_trapcc (opcode, oldpc, extra);
}
#endif
}return 12 * CYCLE_UNIT / 2;
}

#endif
/* FTRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f27b_31)(uae_u32 opcode)
{
{
#ifdef FPUEMU
	uaecptr oldpc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu040 (2);
{	uae_s32 dummy;
	dummy = get_ilong_mmu040 (4);
	m68k_incpci (8);
	fpuop_trapcc (opcode, oldpc, extra);
}
#endif
}return 16 * CYCLE_UNIT / 2;
}

#endif
/* FTRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f27c_31)(uae_u32 opcode)
{
{
#ifdef FPUEMU
	uaecptr oldpc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu040 (2);
	m68k_incpci (4);
	fpuop_trapcc (opcode, oldpc, extra);

#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FBccQ.L #<data>,#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f280_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 63);
{
#ifdef FPUEMU
	m68k_incpci (2);
{	uaecptr pc = m68k_getpc ();
{	uae_s16 extra = get_iword_mmu040 (0);
	m68k_incpci (2);
	fpuop_bcc (opcode, pc,extra);
}}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FBccQ.L #<data>,#<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f2c0_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 63);
{
#ifdef FPUEMU
	m68k_incpci (2);
{	uaecptr pc = m68k_getpc ();
{	uae_s32 extra;
	extra = get_ilong_mmu040 (0);
	m68k_incpci (4);
	fpuop_bcc (opcode, pc,extra);
}}
#endif
}return 12 * CYCLE_UNIT / 2;
}

#endif
/* FSAVE.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f310_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12703; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_save (opcode);

#endif
}}endlabel12703: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FSAVE.L -(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f320_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12704; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_save (opcode);

#endif
}}endlabel12704: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FSAVE.L (d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f328_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12705; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_save (opcode);

#endif
}}endlabel12705: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FSAVE.L (d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f330_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12706; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_save (opcode);

#endif
}}endlabel12706: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FSAVE.L (xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f338_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel12707; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_save (opcode);

#endif
}}endlabel12707: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FSAVE.L (xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f339_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel12708; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_save (opcode);

#endif
}}endlabel12708: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f350_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12709; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel12709: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f358_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12710; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel12710: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f368_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12711; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel12711: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f370_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12712; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel12712: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f378_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel12713; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel12713: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f379_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel12714; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel12714: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f37a_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel12715; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel12715: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f37b_31)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel12716; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel12716: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVLQ.L #<data>,An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f408_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel12717; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12717: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVPQ.L #<data>,An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f410_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel12718; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12718: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f418_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel12719; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12719: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f419_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel12720; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12720: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f41a_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel12721; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12721: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f41b_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel12722; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12722: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f41c_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel12723; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12723: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f41d_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel12724; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12724: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f41e_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel12725; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12725: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CINVAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f41f_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel12726; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12726: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHLQ.L #<data>,An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f428_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel12727; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12727: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHPQ.L #<data>,An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f430_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel12728; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12728: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f438_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel12729; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12729: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f439_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel12730; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12730: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f43a_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel12731; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12731: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f43b_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel12732; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12732: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f43c_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel12733; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12733: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f43d_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel12734; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12734: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f43e_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel12735; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12735: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CPUSHAQ.L #<data> */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f43f_31)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 6) & 3);
{if (!regs.s) { Exception (8); goto endlabel12736; }
{	flush_mmu040(m68k_areg (regs, opcode & 3), (opcode >> 6) & 3);
}}	m68k_incpci (2);
endlabel12736: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* PFLUSHN.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f500_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12737; }
{	m68k_incpci (2);
	mmu_op (opcode, 0);
}}endlabel12737: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* PFLUSH.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f508_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12738; }
{	m68k_incpci (2);
	mmu_op (opcode, 0);
}}endlabel12738: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* PFLUSHAN.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f510_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12739; }
{	m68k_incpci (2);
	mmu_op (opcode, 0);
}}endlabel12739: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* PFLUSHA.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f518_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12740; }
{	m68k_incpci (2);
	mmu_op (opcode, 0);
}}endlabel12740: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* PTESTW.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f548_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12741; }
{	m68k_incpci (2);
	mmu_op (opcode, 0);
}}endlabel12741: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* PTESTR.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f568_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel12742; }
{	m68k_incpci (2);
	mmu_op (opcode, 0);
}}endlabel12742: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* MOVE16.L (An)+,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f600_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	uae_u32 v[4];
{	uaecptr memsa;
	memsa = m68k_areg (regs, srcreg);
{	uaecptr memda;
	memda = get_ilong_mmu040 (2);
	memsa &= ~15;
	memda &= ~15;
	get_move16_mmu (memsa, v);
	put_move16_mmu (memda, v);
	m68k_areg (regs, srcreg) += 16;
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* MOVE16.L (xxx).L,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f608_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u32 v[4];
{	uaecptr memsa;
	memsa = get_ilong_mmu040 (2);
{	uaecptr memda;
	memda = m68k_areg (regs, dstreg);
	memsa &= ~15;
	memda &= ~15;
	get_move16_mmu (memsa, v);
	put_move16_mmu (memda, v);
	m68k_areg (regs, dstreg) += 16;
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* MOVE16.L (An),(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f610_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	uae_u32 v[4];
{	uaecptr memsa;
	memsa = m68k_areg (regs, srcreg);
{	uaecptr memda;
	memda = get_ilong_mmu040 (2);
	memsa &= ~15;
	memda &= ~15;
	get_move16_mmu (memsa, v);
	put_move16_mmu (memda, v);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* MOVE16.L (xxx).L,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f618_31)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u32 v[4];
{	uaecptr memsa;
	memsa = get_ilong_mmu040 (2);
{	uaecptr memda;
	memda = m68k_areg (regs, dstreg);
	memsa &= ~15;
	memda &= ~15;
	get_move16_mmu (memsa, v);
	put_move16_mmu (memda, v);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* MOVE16.L (An)+,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f620_31)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = 0;
{	uae_u32 v[4];
	uaecptr mems = m68k_areg (regs, srcreg) & ~15, memd;
	dstreg = (get_iword_mmu040 (2) >> 12) & 7;
	memd = m68k_areg (regs, dstreg) & ~15;
	get_move16_mmu (mems, v);
	put_move16_mmu (memd, v);
	if (srcreg != dstreg)
		m68k_areg (regs, srcreg) += 16;
	m68k_areg (regs, dstreg) += 16;
}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
#endif

