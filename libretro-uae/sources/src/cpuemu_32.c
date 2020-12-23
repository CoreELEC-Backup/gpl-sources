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
#include "cpummu030.h"
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
uae_u32 REGPARAM2 CPUFUNC(op_0000_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0010_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 20 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0018_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0020_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0028_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0030_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0038_32)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0039_32)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (8);
return 28 * CYCLE_UNIT / 2;
}

/* ORSR.B #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_003c_32)(uae_u32 opcode)
{
{	MakeSR ();
{	uae_s16 src = get_iword_mmu030_state (2);
	src &= 0xFF;
	regs.sr |= src;
	MakeFromSR ();
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0040_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0050_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0058_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0060_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0068_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0070_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0078_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0079_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* ORSR.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_007c_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel12765; }
{	MakeSR ();
{	uae_s16 src = get_iword_mmu030_state (2);
	regs.sr |= src;
	MakeFromSR ();
}}}	m68k_incpci (4);
endlabel12765: ;
return 8 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0080_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0090_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0098_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_00a0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 30 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_00a8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_00b0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}return 32 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_00b8_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_00b9_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (10);
return 36 * CYCLE_UNIT / 2;
}

/* CHK2.B #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00d0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu030_state (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu030_state (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12774; }
}
}}}	m68k_incpci (4);
endlabel12774: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.B #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00e8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu030_state (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu030_state (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12775; }
}
}}}	m68k_incpci (6);
endlabel12775: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.B #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00f0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu030_state (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu030_state (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12776; }
}
}}}}endlabel12776: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.B #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00f8_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu030_state (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu030_state (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12777; }
}
}}}	m68k_incpci (6);
endlabel12777: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.B #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00f9_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu030_state (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu030_state (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12778; }
}
}}}	m68k_incpci (8);
endlabel12778: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.B #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00fa_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu030_state (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu030_state (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12779; }
}
}}}	m68k_incpci (6);
endlabel12779: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.B #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_00fb_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s8)get_byte_mmu030_state (dsta); upper = (uae_s32)(uae_s8)get_byte_mmu030_state (dsta + 1);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s8)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12780; }
}
}}}}endlabel12780: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* BTST.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0100_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_0108_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr memp = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_u16 val = (get_byte_mmu030_state (memp) << 8) + get_byte_mmu030_state (memp + 2);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0110_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0118_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
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
uae_u32 REGPARAM2 CPUFUNC(op_0120_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
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
uae_u32 REGPARAM2 CPUFUNC(op_0128_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0130_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0138_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0139_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_013a_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_getpc () + 2;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_013b_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* BTST.B Dn,#<data>.B */
uae_u32 REGPARAM2 CPUFUNC(op_013c_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = get_ibyte_mmu030_state (2);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* BCHG.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0140_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_0148_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr memp = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_u32 val = (get_byte_mmu030_state (memp) << 24) + (get_byte_mmu030_state (memp + 2) << 16)
              + (get_byte_mmu030_state (memp + 4) << 8) + get_byte_mmu030_state (memp + 6);
	m68k_dreg (regs, dstreg) = (val);
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0150_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0158_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0160_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0168_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0170_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_byte_mmu030_state (dsta, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0178_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0179_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_017a_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_getpc () + 2;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BCHG.B Dn,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_017b_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_byte_mmu030_state (dsta, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCLR.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0180_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_0188_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	uaecptr memp = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	put_byte_mmu030_state (memp, src >> 8);
	put_byte_mmu030_state (memp + 2, src);
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0190_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0198_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_01a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_01a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_01b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_01b8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_01b9_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_01ba_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_getpc () + 2;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BCLR.B Dn,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_01bb_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BSET.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_01c0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_01c8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
	uaecptr memp = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	put_byte_mmu030_state (memp, src >> 24);
	put_byte_mmu030_state (memp + 2, src >> 16);
	put_byte_mmu030_state (memp + 4, src >> 8);
	put_byte_mmu030_state (memp + 6, src);
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_01d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_01d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_01e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_01e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_01f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_01f8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_01f9_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_01fa_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_getpc () + 2;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BSET.B Dn,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_01fb_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0200_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0210_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 20 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0218_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0220_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0228_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0230_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0238_32)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0239_32)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (8);
return 28 * CYCLE_UNIT / 2;
}

/* ANDSR.B #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_023c_32)(uae_u32 opcode)
{
{	MakeSR ();
{	uae_s16 src = get_iword_mmu030_state (2);
	src |= 0xFF00;
	regs.sr &= src;
	MakeFromSR ();
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0240_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0250_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0258_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0260_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0268_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0270_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0278_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0279_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* ANDSR.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_027c_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel12843; }
{	MakeSR ();
{	uae_s16 src = get_iword_mmu030_state (2);
	regs.sr &= src;
	MakeFromSR ();
}}}	m68k_incpci (4);
endlabel12843: ;
return 8 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0280_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0290_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0298_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_02a0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 30 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_02a8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_02b0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}return 32 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_02b8_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_02b9_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (10);
return 36 * CYCLE_UNIT / 2;
}

/* CHK2.W #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02d0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu030_state (dsta); upper = (uae_s32)(uae_s16)get_word_mmu030_state (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12852; }
}
}}}	m68k_incpci (4);
endlabel12852: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.W #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02e8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu030_state (dsta); upper = (uae_s32)(uae_s16)get_word_mmu030_state (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12853; }
}
}}}	m68k_incpci (6);
endlabel12853: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.W #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02f0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu030_state (dsta); upper = (uae_s32)(uae_s16)get_word_mmu030_state (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12854; }
}
}}}}endlabel12854: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.W #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02f8_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu030_state (dsta); upper = (uae_s32)(uae_s16)get_word_mmu030_state (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12855; }
}
}}}	m68k_incpci (6);
endlabel12855: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.W #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02f9_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu030_state (dsta); upper = (uae_s32)(uae_s16)get_word_mmu030_state (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12856; }
}
}}}	m68k_incpci (8);
endlabel12856: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.W #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02fa_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu030_state (dsta); upper = (uae_s32)(uae_s16)get_word_mmu030_state (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12857; }
}
}}}	m68k_incpci (6);
endlabel12857: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.W #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_02fb_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = (uae_s32)(uae_s16)get_word_mmu030_state (dsta); upper = (uae_s32)(uae_s16)get_word_mmu030_state (dsta + 2);
	if ((extra & 0x8000) == 0) reg = (uae_s32)(uae_s16)reg;
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12858; }
}
}}}}endlabel12858: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* SUB.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0400_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0410_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 20 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0418_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0420_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0428_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0430_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0438_32)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0439_32)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (8);
return 28 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0440_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0450_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0458_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0460_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0468_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0470_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0478_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0479_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0480_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0490_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0498_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_04a0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 30 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_04a8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_04b0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}}return 32 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_04b8_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_04b9_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (10);
return 36 * CYCLE_UNIT / 2;
}

/* CHK2.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04d0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu030_state (dsta); upper = get_long_mmu030_state (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12883; }
}
}}}	m68k_incpci (4);
endlabel12883: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04e8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu030_state (dsta); upper = get_long_mmu030_state (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12884; }
}
}}}	m68k_incpci (6);
endlabel12884: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04f0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu030_state (dsta); upper = get_long_mmu030_state (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12885; }
}
}}}}endlabel12885: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04f8_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu030_state (dsta); upper = get_long_mmu030_state (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12886; }
}
}}}	m68k_incpci (6);
endlabel12886: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04f9_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu030_state (dsta); upper = get_long_mmu030_state (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12887; }
}
}}}	m68k_incpci (8);
endlabel12887: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04fa_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu030_state (dsta); upper = get_long_mmu030_state (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12888; }
}
}}}	m68k_incpci (6);
endlabel12888: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK2.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_04fb_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
	{uae_s32 upper,lower,reg = regs.regs[(extra >> 12) & 15];
	lower = get_long_mmu030_state (dsta); upper = get_long_mmu030_state (dsta + 4);
	SET_ZFLG (upper == reg || lower == reg);
	SET_CFLG_ALWAYS (lower <= upper ? reg < lower || reg > upper : reg > upper || reg < lower);
	if ((extra & 0x800) && GET_CFLG ()) { Exception (6); goto endlabel12889; }
}
}}}}endlabel12889: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* ADD.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0600_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0610_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 20 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0618_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0620_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0628_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0630_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0638_32)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0639_32)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (8);
return 28 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0640_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0650_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0658_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0660_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0668_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0670_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0678_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0679_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0680_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0690_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0698_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_06a0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 30 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_06a8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_06b0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}}return 32 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_06b8_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_06b9_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (10);
return 36 * CYCLE_UNIT / 2;
}

/* RTM.L Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* RTM.L An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06c8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06f8_32)(uae_u32 opcode)
{
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06f9_32)(uae_u32 opcode)
{
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06fa_32)(uae_u32 opcode)
{
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* CALLM.L (d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_06fb_32)(uae_u32 opcode)
{
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* BTST.L #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0800_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0810_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0818_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
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
uae_u32 REGPARAM2 CPUFUNC(op_0820_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
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
uae_u32 REGPARAM2 CPUFUNC(op_0828_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0830_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0838_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0839_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (8);
return 20 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_083a_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_083b_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* BTST.B #<data>.W,#<data>.B */
uae_u32 REGPARAM2 CPUFUNC(op_083c_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s8 dst = get_ibyte_mmu030_state (4);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* BCHG.L #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0840_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0850_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0858_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0860_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0868_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0870_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_byte_mmu030_state (dsta, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0878_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0879_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_087a_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BCHG.B #<data>.W,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_087b_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	put_byte_mmu030_state (dsta, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* BCLR.L #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0880_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0890_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0898_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_08a0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_08a8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_08b0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_08b8_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_08b9_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_08ba_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BCLR.B #<data>.W,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_08bb_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* BSET.L #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_08c0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= 31;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_08d0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_08d8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_08e0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_08e8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_08f0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_08f8_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_08f9_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_08fa_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BSET.B #<data>.W,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_08fb_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	put_byte_mmu030_state (dsta, dst);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0a00_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a10_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 20 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0a18_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a20_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a28_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0a30_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0a38_32)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 24 * CYCLE_UNIT / 2;
}

/* EOR.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0a39_32)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (8);
return 28 * CYCLE_UNIT / 2;
}

/* EORSR.B #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_0a3c_32)(uae_u32 opcode)
{
{	MakeSR ();
{	uae_s16 src = get_iword_mmu030_state (2);
	src &= 0xFF;
	regs.sr ^= src;
	MakeFromSR ();
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0a40_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a50_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0a58_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a60_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0a68_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0a70_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0a78_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* EOR.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0a79_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* EORSR.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_0a7c_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel12981; }
{	MakeSR ();
{	uae_s16 src = get_iword_mmu030_state (2);
	regs.sr ^= src;
	MakeFromSR ();
}}}	m68k_incpci (4);
endlabel12981: ;
return 8 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0a80_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
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
uae_u32 REGPARAM2 CPUFUNC(op_0a90_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0a98_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0aa0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 30 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0aa8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0ab0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}return 32 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0ab8_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

/* EOR.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0ab9_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (10);
return 36 * CYCLE_UNIT / 2;
}

/* CAS.B #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ad0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_lrmw_byte_mmu030_state (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CAS.B #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ad8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_lrmw_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CAS.B #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ae0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_lrmw_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

#endif
/* CAS.B #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ae8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_lrmw_byte_mmu030_state (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

#endif
/* CAS.B #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0af0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_lrmw_byte_mmu030_state (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* CAS.B #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0af8_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_lrmw_byte_mmu030_state (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

#endif
/* CAS.B #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0af9_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s8 dst = get_lrmw_byte_mmu030_state (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpb ((uae_s8)(m68k_dreg (regs, rc)), (uae_s8)(dst));
	if (GET_ZFLG ()){
		put_lrmw_byte_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xff) | (dst & 0xff);
}}}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

#endif
/* CMP.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0c00_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c10_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0c18_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c20_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c28_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0c30_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0c38_32)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0c39_32)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0c3a_32)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

#endif
/* CMP.B #<data>.B,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0c3b_32)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* CMP.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0c40_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c50_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0c58_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c60_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c68_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0c70_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0c78_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0c79_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (8);
return 20 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0c7a_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CMP.W #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0c7b_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* CMP.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_0c80_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0c90_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_0c98_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_0ca0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_0ca8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_0cb0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_0cb8_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_0cb9_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (10);
return 28 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cba_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 6;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (6);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

#endif
/* CMP.L #<data>.L,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cbb_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (6);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 24 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cd0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_lrmw_word_mmu030_state (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	if (GET_ZFLG ()){
		put_lrmw_word_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cd8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_lrmw_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	if (GET_ZFLG ()){
		put_lrmw_word_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ce0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_lrmw_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	if (GET_ZFLG ()){
		put_lrmw_word_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ce8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s16 dst = get_lrmw_word_mmu030_state (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	if (GET_ZFLG ()){
		put_lrmw_word_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cf0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_lrmw_word_mmu030_state (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	if (GET_ZFLG ()){
		put_lrmw_word_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cf8_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s16 dst = get_lrmw_word_mmu030_state (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	if (GET_ZFLG ()){
		put_lrmw_word_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

#endif
/* CAS.W #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cf9_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s16 dst = get_lrmw_word_mmu030_state (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, rc)), (uae_s16)(dst));
	if (GET_ZFLG ()){
		put_lrmw_word_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = (m68k_dreg(regs, rc) & ~0xffff) | (dst & 0xffff);
}}}}}}	m68k_incpci (8);
return 24 * CYCLE_UNIT / 2;
}

#endif
/* CAS2.W #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0cfc_32)(uae_u32 opcode)
{
{{	uae_s32 extra;
	extra = get_ilong_mmu030_state (2);
	uae_u32 rn1 = regs.regs[(extra >> 28) & 15];
	uae_u32 rn2 = regs.regs[(extra >> 12) & 15];
	uae_u16 dst1 = get_lrmw_word_mmu030_state (rn1), dst2 = get_lrmw_word_mmu030_state (rn2);
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, (extra >> 16) & 7)), (uae_s16)(dst1));
	if (GET_ZFLG ()) {
	optflag_cmpw ((uae_s16)(m68k_dreg (regs, extra & 7)), (uae_s16)(dst2));
	if (GET_ZFLG ()) {
	put_lrmw_word_mmu030_state (rn1, m68k_dreg (regs, (extra >> 22) & 7));
	put_lrmw_word_mmu030_state (rn2, m68k_dreg (regs, (extra >> 6) & 7));
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
uae_u32 REGPARAM2 CPUFUNC(op_0e10_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel13035; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	dfc030_put_byte (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_s8 src = sfc030_get_byte (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
}}}}}}	m68k_incpci (4);
endlabel13035: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e18_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel13036; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	dfc030_put_byte (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_s8 src = sfc030_get_byte (srca);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
}}}}}}	m68k_incpci (4);
endlabel13036: ;
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e20_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel13037; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	dfc030_put_byte (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 src = sfc030_get_byte (srca);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = srca;
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
}}}}}}	m68k_incpci (4);
endlabel13037: ;
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e28_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel13038; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	dfc030_put_byte (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 src = sfc030_get_byte (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
}}}}}}	m68k_incpci (6);
endlabel13038: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e30_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel13039; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	dfc030_put_byte (dsta, src);
}}}else{{	uaecptr srca;
	m68k_incpci (4);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 1);
{	uae_s8 src = sfc030_get_byte (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
}}}}}}}endlabel13039: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e38_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel13040; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	dfc030_put_byte (dsta, src);
}}else{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s8 src = sfc030_get_byte (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
}}}}}}	m68k_incpci (6);
endlabel13040: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.B #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e39_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel13041; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
	dfc030_put_byte (dsta, src);
}}else{{	uaecptr srca;
	srca = get_ilong_mmu030_state (4);
{	uae_s8 src = sfc030_get_byte (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s8)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xff) | ((src) & 0xff);
	}
}}}}}}	m68k_incpci (8);
endlabel13041: ;
return 32 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e50_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel13042; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	dfc030_put_word (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_s16 src = sfc030_get_word (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
}}}}}}	m68k_incpci (4);
endlabel13042: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e58_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel13043; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	dfc030_put_word (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_s16 src = sfc030_get_word (srca);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
}}}}}}	m68k_incpci (4);
endlabel13043: ;
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e60_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel13044; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	dfc030_put_word (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) - 2;
{	uae_s16 src = sfc030_get_word (srca);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = srca;
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
}}}}}}	m68k_incpci (4);
endlabel13044: ;
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e68_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel13045; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	dfc030_put_word (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s16 src = sfc030_get_word (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
}}}}}}	m68k_incpci (6);
endlabel13045: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e70_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel13046; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	dfc030_put_word (dsta, src);
}}}else{{	uaecptr srca;
	m68k_incpci (4);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 1);
{	uae_s16 src = sfc030_get_word (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
}}}}}}}endlabel13046: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e78_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel13047; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	dfc030_put_word (dsta, src);
}}else{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s16 src = sfc030_get_word (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
}}}}}}	m68k_incpci (6);
endlabel13047: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.W #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e79_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel13048; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
	dfc030_put_word (dsta, src);
}}else{{	uaecptr srca;
	srca = get_ilong_mmu030_state (4);
{	uae_s16 src = sfc030_get_word (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = (uae_s32)(uae_s16)src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (m68k_dreg (regs, (extra >> 12) & 7) & ~0xffff) | ((src) & 0xffff);
	}
}}}}}}	m68k_incpci (8);
endlabel13048: ;
return 32 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e90_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel13049; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	dfc030_put_long (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_s32 src = sfc030_get_long (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
}}}}}}	m68k_incpci (4);
endlabel13049: ;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0e98_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel13050; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	dfc030_put_long (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_s32 src = sfc030_get_long (srca);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
}}}}}}	m68k_incpci (4);
endlabel13050: ;
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ea0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel13051; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	dfc030_put_long (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) - 4;
{	uae_s32 src = sfc030_get_long (srca);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = srca;
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
}}}}}}	m68k_incpci (4);
endlabel13051: ;
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ea8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel13052; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	dfc030_put_long (dsta, src);
}}else{{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s32 src = sfc030_get_long (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
}}}}}}	m68k_incpci (6);
endlabel13052: ;
return 32 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0eb0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{if (!regs.s) { Exception (8); goto endlabel13053; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	dfc030_put_long (dsta, src);
}}}else{{	uaecptr srca;
	m68k_incpci (4);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 1);
{	uae_s32 src = sfc030_get_long (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
}}}}}}}endlabel13053: ;
return 32 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0eb8_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel13054; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	dfc030_put_long (dsta, src);
}}else{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s32 src = sfc030_get_long (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
}}}}}}	m68k_incpci (6);
endlabel13054: ;
return 32 * CYCLE_UNIT / 2;
}

#endif
/* MOVES.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0eb9_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel13055; }
{{	uae_s16 extra = get_iword_mmu030_state (2);
	if (extra & 0x800)
{	uae_u32 src = regs.regs[(extra >> 12) & 15];
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
	dfc030_put_long (dsta, src);
}}else{{	uaecptr srca;
	srca = get_ilong_mmu030_state (4);
{	uae_s32 src = sfc030_get_long (srca);
	if (extra & 0x8000) {
	m68k_areg (regs, (extra >> 12) & 7) = src;
	} else {
	m68k_dreg (regs, (extra >> 12) & 7) = (src);
	}
}}}}}}	m68k_incpci (8);
endlabel13055: ;
return 40 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ed0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_lrmw_long_mmu030_state (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	if (GET_ZFLG ()){
		put_lrmw_long_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = dst;
}}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ed8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_lrmw_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	if (GET_ZFLG ()){
		put_lrmw_long_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = dst;
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ee0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_lrmw_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	if (GET_ZFLG ()){
		put_lrmw_long_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = dst;
}}}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 26 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ee8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s32 dst = get_lrmw_long_mmu030_state (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	if (GET_ZFLG ()){
		put_lrmw_long_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = dst;
}}}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ef0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_lrmw_long_mmu030_state (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	if (GET_ZFLG ()){
		put_lrmw_long_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = dst;
}}}}}}}return 28 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ef8_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s32 dst = get_lrmw_long_mmu030_state (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	if (GET_ZFLG ()){
		put_lrmw_long_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = dst;
}}}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

#endif
/* CAS.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0ef9_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s32 dst = get_lrmw_long_mmu030_state (dsta);
{	int ru = (src >> 6) & 7;
	int rc = src & 7;
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, rc)), (uae_s32)(dst));
	if (GET_ZFLG ()){
		put_lrmw_long_mmu030_state (dsta, (m68k_dreg (regs, ru)));
	}else{
		m68k_dreg(regs, rc) = dst;
}}}}}}	m68k_incpci (8);
return 32 * CYCLE_UNIT / 2;
}

#endif
/* CAS2.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_0efc_32)(uae_u32 opcode)
{
{{	uae_s32 extra;
	extra = get_ilong_mmu030_state (2);
	uae_u32 rn1 = regs.regs[(extra >> 28) & 15];
	uae_u32 rn2 = regs.regs[(extra >> 12) & 15];
	uae_u32 dst1 = get_lrmw_long_mmu030_state (rn1), dst2 = get_lrmw_long_mmu030_state (rn2);
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, (extra >> 16) & 7)), (uae_s32)(dst1));
	if (GET_ZFLG ()) {
	optflag_cmpl ((uae_s32)(m68k_dreg (regs, extra & 7)), (uae_s32)(dst2));
	if (GET_ZFLG ()) {
	put_lrmw_long_mmu030_state (rn1, m68k_dreg (regs, (extra >> 22) & 7));
	put_lrmw_long_mmu030_state (rn2, m68k_dreg (regs, (extra >> 6) & 7));
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
uae_u32 REGPARAM2 CPUFUNC(op_1000_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_1010_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (2);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1018_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1020_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1028_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1030_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1038_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_1039_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (6);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_103a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_103b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_103c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpci (4);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1080_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1090_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1098_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10b8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10b9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10ba_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10bb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_10bc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10c0_32)(uae_u32 opcode)
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
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10f8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10f9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10fa_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10fb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_10fc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1100_32)(uae_u32 opcode)
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
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1110_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1118_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1120_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1128_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1130_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1138_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_1139_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_113a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_113b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_113c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1140_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1150_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1158_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1160_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1168_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1170_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1178_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_1179_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (6);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_117a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_117b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_117c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_1180_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_1190_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_1198_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 1);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11b8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11b9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11ba_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11bb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 1);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_11bc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11f8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11f9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (6);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11fa_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11fb_32)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_11fc_32)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.B (An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B (An)+,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.B -(An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,An,Xn),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13f8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (xxx).L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13f9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (6);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (10);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.B (d16,PC),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13fa_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B (d8,PC,Xn),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13fb_32)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (0);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.B #<data>.B,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_13fc_32)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2000_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_2008_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_2010_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2018_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2020_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2028_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2030_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2038_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_2039_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_203a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_203b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_203c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpci (6);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.L Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_2040_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVEA.L An,An */
uae_u32 REGPARAM2 CPUFUNC(op_2048_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}return 4 * CYCLE_UNIT / 2;
}

/* MOVEA.L (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_2050_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.L (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_2058_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.L -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_2060_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* MOVEA.L (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_2068_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEA.L (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_2070_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	m68k_areg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEA.L (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_2078_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEA.L (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_2079_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVEA.L (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_207a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEA.L (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_207b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	m68k_areg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEA.L #<data>.L,An */
uae_u32 REGPARAM2 CPUFUNC(op_207c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpci (6);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2080_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L An,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2088_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2090_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2098_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20b8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20b9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20ba_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20bb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_20bc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20c0_32)(uae_u32 opcode)
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
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L An,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20c8_32)(uae_u32 opcode)
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
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20f8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20f9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20fa_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20fb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_20fc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2100_32)(uae_u32 opcode)
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
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L An,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2108_32)(uae_u32 opcode)
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
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2110_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2118_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2120_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2128_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2130_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2138_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_2139_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_213a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_213b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_213c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2140_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L An,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2148_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2150_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2158_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2160_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 26 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2168_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 28 * CYCLE_UNIT / 2;
}

#endif

#ifdef PART_3
/* MOVE.L (d8,An,Xn),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2170_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2178_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_2179_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (6);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_217a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_217b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_217c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (6);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_2180_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L An,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_2188_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_2190_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_2198_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 26 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 1);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21b8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21b9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21ba_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21bb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 1);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_21bc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L An,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21c8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 26 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21f8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21f9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (6);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21fa_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21fb_32)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_21fc_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (6);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.L Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L An,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23c8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.L (An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L (An)+,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* MOVE.L -(An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 30 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,An,Xn),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23f8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (xxx).L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23f9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (6);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (10);
}}}}return 36 * CYCLE_UNIT / 2;
}

/* MOVE.L (d16,PC),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23fa_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L (d8,PC,Xn),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23fb_32)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (0);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}}return 32 * CYCLE_UNIT / 2;
}

/* MOVE.L #<data>.L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_23fc_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (6);
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
	m68k_incpci (10);
}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3000_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_3008_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_3010_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (2);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3018_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3020_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3028_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3030_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3038_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_3039_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (6);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_303a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_303b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_303c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpci (4);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVEA.W Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_3040_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_3048_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_3050_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (2);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVEA.W (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_3058_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MOVEA.W -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_3060_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* MOVEA.W (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_3068_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.W (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_3070_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.W (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_3078_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.W (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_3079_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (6);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVEA.W (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_307a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (4);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.W (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_307b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVEA.W #<data>.W,An */
uae_u32 REGPARAM2 CPUFUNC(op_307c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpci (4);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3080_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W An,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3088_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3090_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3098_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30b8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30b9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30ba_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30bb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_30bc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30c0_32)(uae_u32 opcode)
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
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W An,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30c8_32)(uae_u32 opcode)
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
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30f8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30f9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30fa_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30fb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_30fc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3100_32)(uae_u32 opcode)
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
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W An,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3108_32)(uae_u32 opcode)
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
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3110_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3118_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3120_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3128_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3130_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3138_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_3139_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_313a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_313b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_313c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3140_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W An,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3148_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3150_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3158_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3160_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3168_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3170_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3178_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_3179_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (6);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_317a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_317b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_317c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_3180_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W An,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_3188_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_3190_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_3198_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 1);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31b8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31b9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	m68k_incpci (6);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31ba_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31bb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 1);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_31bc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W An,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31c8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}return 12 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}	mmufixup[0].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31f8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31f9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (6);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31fa_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31fb_32)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (2);
}}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_31fc_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W An,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33c8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}return 16 * CYCLE_UNIT / 2;
}

/* MOVE.W (An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W (An)+,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* MOVE.W -(An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (6);
}}}}	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,An),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,An,Xn),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33f8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (xxx).L,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33f9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (6);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (10);
}}}}return 28 * CYCLE_UNIT / 2;
}

/* MOVE.W (d16,PC),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33fa_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W (d8,PC,Xn),(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33fb_32)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (0);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (4);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* MOVE.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_33fc_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
	m68k_incpci (8);
}}}return 20 * CYCLE_UNIT / 2;
}

/* NEGX.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4000_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_4010_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	put_byte_mmu030_state (srca, newv);
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* NEGX.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4018_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
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
	put_byte_mmu030_state (srca, newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* NEGX.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4020_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
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
	put_byte_mmu030_state (srca, newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* NEGX.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4028_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	put_byte_mmu030_state (srca, newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NEGX.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4030_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	put_byte_mmu030_state (srca, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEGX.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4038_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	put_byte_mmu030_state (srca, newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NEGX.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4039_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	put_byte_mmu030_state (srca, newv);
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* NEGX.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4040_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_4050_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	put_word_mmu030_state (srca, newv);
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* NEGX.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4058_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
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
	put_word_mmu030_state (srca, newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* NEGX.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4060_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
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
	put_word_mmu030_state (srca, newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* NEGX.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4068_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	put_word_mmu030_state (srca, newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NEGX.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4070_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	put_word_mmu030_state (srca, newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEGX.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4078_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	put_word_mmu030_state (srca, newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NEGX.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4079_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	put_word_mmu030_state (srca, newv);
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* NEGX.L Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4080_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_4090_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	put_long_mmu030_state (srca, newv);
}}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* NEGX.L (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4098_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
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
	put_long_mmu030_state (srca, newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* NEGX.L -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_40a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
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
	put_long_mmu030_state (srca, newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* NEGX.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_40a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	put_long_mmu030_state (srca, newv);
}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* NEGX.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_40b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	put_long_mmu030_state (srca, newv);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* NEGX.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_40b8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	put_long_mmu030_state (srca, newv);
}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* NEGX.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_40b9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	put_long_mmu030_state (srca, newv);
}}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* MVSR2.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_40c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel13392; }
{{	MakeSR ();
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((regs.sr) & 0xffff);
}}}	m68k_incpci (2);
endlabel13392: ;
return 4 * CYCLE_UNIT / 2;
}

/* MVSR2.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_40d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel13393; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	MakeSR ();
	put_word_mmu030_state (srca, regs.sr);
}}}	m68k_incpci (2);
endlabel13393: ;
return 8 * CYCLE_UNIT / 2;
}

/* MVSR2.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_40d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel13394; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
	MakeSR ();
	put_word_mmu030_state (srca, regs.sr);
}}}	m68k_incpci (2);
endlabel13394: ;
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MVSR2.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_40e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel13395; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	MakeSR ();
	put_word_mmu030_state (srca, regs.sr);
}}}	m68k_incpci (2);
endlabel13395: ;
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* MVSR2.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_40e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel13396; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	MakeSR ();
	put_word_mmu030_state (srca, regs.sr);
}}}	m68k_incpci (4);
endlabel13396: ;
return 12 * CYCLE_UNIT / 2;
}

/* MVSR2.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_40f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel13397; }
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
	MakeSR ();
	put_word_mmu030_state (srca, regs.sr);
}}}}endlabel13397: ;
return 12 * CYCLE_UNIT / 2;
}

/* MVSR2.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_40f8_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel13398; }
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	MakeSR ();
	put_word_mmu030_state (srca, regs.sr);
}}}	m68k_incpci (4);
endlabel13398: ;
return 12 * CYCLE_UNIT / 2;
}

/* MVSR2.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_40f9_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel13399; }
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
	MakeSR ();
	put_word_mmu030_state (srca, regs.sr);
}}}	m68k_incpci (6);
endlabel13399: ;
return 16 * CYCLE_UNIT / 2;
}

/* CHK.L Dn,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4100_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13400;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13400;
	}
}}}endlabel13400: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (An),Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4110_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13401;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13401;
	}
}}}}endlabel13401: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (An)+,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4118_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13402;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13402;
	}
}}}}endlabel13402: ;
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L -(An),Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4120_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13403;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13403;
	}
}}}}endlabel13403: ;
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (d16,An),Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4128_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13404;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13404;
	}
}}}}endlabel13404: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (d8,An,Xn),Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4130_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13405;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13405;
	}
}}}}}endlabel13405: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (xxx).W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4138_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13406;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13406;
	}
}}}}endlabel13406: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (xxx).L,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4139_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (6);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13407;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13407;
	}
}}}}endlabel13407: ;
return 20 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (d16,PC),Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_413a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13408;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13408;
	}
}}}}endlabel13408: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L (d8,PC,Xn),Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_413b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13409;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13409;
	}
}}}}}endlabel13409: ;
return 16 * CYCLE_UNIT / 2;
}

#endif
/* CHK.L #<data>.L,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_413c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (6);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13410;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13410;
	}
}}}endlabel13410: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* CHK.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4180_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13411;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13411;
	}
}}}endlabel13411: ;
return 4 * CYCLE_UNIT / 2;
}

/* CHK.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4190_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13412;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13412;
	}
}}}}endlabel13412: ;
return 8 * CYCLE_UNIT / 2;
}

/* CHK.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4198_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13413;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13413;
	}
}}}}endlabel13413: ;
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* CHK.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (2);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13414;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13414;
	}
}}}}endlabel13414: ;
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* CHK.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13415;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13415;
	}
}}}}endlabel13415: ;
return 12 * CYCLE_UNIT / 2;
}

/* CHK.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13416;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13416;
	}
}}}}}endlabel13416: ;
return 12 * CYCLE_UNIT / 2;
}

/* CHK.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41b8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13417;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13417;
	}
}}}}endlabel13417: ;
return 12 * CYCLE_UNIT / 2;
}

/* CHK.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41b9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (6);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13418;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13418;
	}
}}}}endlabel13418: ;
return 16 * CYCLE_UNIT / 2;
}

/* CHK.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41ba_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13419;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13419;
	}
}}}}endlabel13419: ;
return 12 * CYCLE_UNIT / 2;
}

/* CHK.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41bb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13420;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13420;
	}
}}}}}endlabel13420: ;
return 12 * CYCLE_UNIT / 2;
}

/* CHK.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_41bc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel13421;
	}
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel13421;
	}
}}}endlabel13421: ;
return 8 * CYCLE_UNIT / 2;
}

/* LEA.L (An),An */
uae_u32 REGPARAM2 CPUFUNC(op_41d0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_41e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* LEA.L (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_41f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	m68k_areg (regs, dstreg) = (srca);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* LEA.L (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_41f8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* LEA.L (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_41f9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* LEA.L (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_41fa_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* LEA.L (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_41fb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	m68k_areg (regs, dstreg) = (srca);
}}}}return 8 * CYCLE_UNIT / 2;
}

/* CLR.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4200_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	SET_CZNV (FLAGVAL_Z);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((0) & 0xff);
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CLR.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4210_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	SET_CZNV (FLAGVAL_Z);
	put_byte_mmu030_state (srca, 0);
}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* CLR.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4218_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
	SET_CZNV (FLAGVAL_Z);
	put_byte_mmu030_state (srca, 0);
}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* CLR.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4220_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	SET_CZNV (FLAGVAL_Z);
	put_byte_mmu030_state (srca, 0);
}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* CLR.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4228_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	SET_CZNV (FLAGVAL_Z);
	put_byte_mmu030_state (srca, 0);
}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CLR.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4230_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
	SET_CZNV (FLAGVAL_Z);
	put_byte_mmu030_state (srca, 0);
}}}return 12 * CYCLE_UNIT / 2;
}

/* CLR.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4238_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	SET_CZNV (FLAGVAL_Z);
	put_byte_mmu030_state (srca, 0);
}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CLR.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4239_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
	SET_CZNV (FLAGVAL_Z);
	put_byte_mmu030_state (srca, 0);
}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CLR.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4240_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	SET_CZNV (FLAGVAL_Z);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((0) & 0xffff);
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CLR.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4250_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	SET_CZNV (FLAGVAL_Z);
	put_word_mmu030_state (srca, 0);
}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* CLR.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4258_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
	SET_CZNV (FLAGVAL_Z);
	put_word_mmu030_state (srca, 0);
}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* CLR.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4260_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	SET_CZNV (FLAGVAL_Z);
	put_word_mmu030_state (srca, 0);
}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* CLR.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4268_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	SET_CZNV (FLAGVAL_Z);
	put_word_mmu030_state (srca, 0);
}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CLR.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4270_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
	SET_CZNV (FLAGVAL_Z);
	put_word_mmu030_state (srca, 0);
}}}return 12 * CYCLE_UNIT / 2;
}

/* CLR.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4278_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	SET_CZNV (FLAGVAL_Z);
	put_word_mmu030_state (srca, 0);
}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CLR.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4279_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
	SET_CZNV (FLAGVAL_Z);
	put_word_mmu030_state (srca, 0);
}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CLR.L Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4280_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	SET_CZNV (FLAGVAL_Z);
	m68k_dreg (regs, srcreg) = (0);
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* CLR.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4290_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	SET_CZNV (FLAGVAL_Z);
	put_long_mmu030_state (srca, 0);
}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* CLR.L (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4298_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
	SET_CZNV (FLAGVAL_Z);
	put_long_mmu030_state (srca, 0);
}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* CLR.L -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_42a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	SET_CZNV (FLAGVAL_Z);
	put_long_mmu030_state (srca, 0);
}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* CLR.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_42a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	SET_CZNV (FLAGVAL_Z);
	put_long_mmu030_state (srca, 0);
}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CLR.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_42b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
	SET_CZNV (FLAGVAL_Z);
	put_long_mmu030_state (srca, 0);
}}}return 16 * CYCLE_UNIT / 2;
}

/* CLR.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_42b8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	SET_CZNV (FLAGVAL_Z);
	put_long_mmu030_state (srca, 0);
}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CLR.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_42b9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
	SET_CZNV (FLAGVAL_Z);
	put_long_mmu030_state (srca, 0);
}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* MVSR2.B Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42c0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_42d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	MakeSR ();
	put_word_mmu030_state (srca, regs.sr & 0xff);
}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* MVSR2.B (An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
	MakeSR ();
	put_word_mmu030_state (srca, regs.sr & 0xff);
}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* MVSR2.B -(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	MakeSR ();
	put_word_mmu030_state (srca, regs.sr & 0xff);
}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

#endif
/* MVSR2.B (d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	MakeSR ();
	put_word_mmu030_state (srca, regs.sr & 0xff);
}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* MVSR2.B (d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
	MakeSR ();
	put_word_mmu030_state (srca, regs.sr & 0xff);
}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* MVSR2.B (xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42f8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	MakeSR ();
	put_word_mmu030_state (srca, regs.sr & 0xff);
}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* MVSR2.B (xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_42f9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
	MakeSR ();
	put_word_mmu030_state (srca, regs.sr & 0xff);
}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* NEG.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4400_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_4410_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	put_byte_mmu030_state (srca, dst);
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* NEG.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4418_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	put_byte_mmu030_state (srca, dst);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* NEG.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4420_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	put_byte_mmu030_state (srca, dst);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* NEG.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4428_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	put_byte_mmu030_state (srca, dst);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NEG.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4430_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	put_byte_mmu030_state (srca, dst);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEG.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4438_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	put_byte_mmu030_state (srca, dst);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NEG.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4439_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	put_byte_mmu030_state (srca, dst);
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* NEG.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4440_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_4450_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	put_word_mmu030_state (srca, dst);
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* NEG.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4458_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	put_word_mmu030_state (srca, dst);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* NEG.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4460_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	put_word_mmu030_state (srca, dst);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* NEG.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4468_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	put_word_mmu030_state (srca, dst);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NEG.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4470_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	put_word_mmu030_state (srca, dst);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NEG.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4478_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	put_word_mmu030_state (srca, dst);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NEG.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4479_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	put_word_mmu030_state (srca, dst);
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* NEG.L Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4480_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_4490_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	put_long_mmu030_state (srca, dst);
}}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* NEG.L (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4498_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	put_long_mmu030_state (srca, dst);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* NEG.L -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_44a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	put_long_mmu030_state (srca, dst);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* NEG.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_44a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	put_long_mmu030_state (srca, dst);
}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* NEG.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_44b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	put_long_mmu030_state (srca, dst);
}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* NEG.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_44b8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	put_long_mmu030_state (srca, dst);
}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* NEG.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_44b9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	put_long_mmu030_state (srca, dst);
}}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* MV2SR.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_44c0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_44d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* MV2SR.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_44d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
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
uae_u32 REGPARAM2 CPUFUNC(op_44e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
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
uae_u32 REGPARAM2 CPUFUNC(op_44e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_44f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_44f8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_44f9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* MV2SR.B (d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_44fa_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.B (d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_44fb_32)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.B #<data>.B */
uae_u32 REGPARAM2 CPUFUNC(op_44fc_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* NOT.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4600_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_4610_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	put_byte_mmu030_state (srca, dst);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* NOT.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4618_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	put_byte_mmu030_state (srca, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* NOT.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4620_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	put_byte_mmu030_state (srca, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* NOT.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4628_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	put_byte_mmu030_state (srca, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NOT.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4630_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	put_byte_mmu030_state (srca, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NOT.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4638_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	put_byte_mmu030_state (srca, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NOT.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4639_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	put_byte_mmu030_state (srca, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* NOT.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4640_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_4650_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	put_word_mmu030_state (srca, dst);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* NOT.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4658_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	put_word_mmu030_state (srca, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* NOT.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4660_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	put_word_mmu030_state (srca, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* NOT.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4668_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	put_word_mmu030_state (srca, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NOT.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4670_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	put_word_mmu030_state (srca, dst);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NOT.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4678_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	put_word_mmu030_state (srca, dst);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NOT.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4679_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	put_word_mmu030_state (srca, dst);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* NOT.L Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4680_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_4690_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	put_long_mmu030_state (srca, dst);
}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* NOT.L (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4698_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	put_long_mmu030_state (srca, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* NOT.L -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_46a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	put_long_mmu030_state (srca, dst);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* NOT.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_46a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	put_long_mmu030_state (srca, dst);
}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* NOT.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_46b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	put_long_mmu030_state (srca, dst);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* NOT.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_46b8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	put_long_mmu030_state (srca, dst);
}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* NOT.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_46b9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	put_long_mmu030_state (srca, dst);
}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* MV2SR.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_46c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel13520; }
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	regs.sr = src;
	MakeFromSR ();
}}}	m68k_incpci (2);
endlabel13520: ;
return 4 * CYCLE_UNIT / 2;
}

/* MV2SR.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_46d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel13521; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (2);
endlabel13521: ;
return 8 * CYCLE_UNIT / 2;
}

/* MV2SR.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_46d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel13522; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (2);
endlabel13522: ;
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* MV2SR.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_46e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel13523; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (2);
endlabel13523: ;
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* MV2SR.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_46e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel13524; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (4);
endlabel13524: ;
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_46f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel13525; }
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}}endlabel13525: ;
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_46f8_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel13526; }
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (4);
endlabel13526: ;
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_46f9_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel13527; }
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (6);
endlabel13527: ;
return 16 * CYCLE_UNIT / 2;
}

/* MV2SR.W (d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_46fa_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel13528; }
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}	m68k_incpci (4);
endlabel13528: ;
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.W (d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_46fb_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel13529; }
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
	regs.sr = src;
	MakeFromSR ();
}}}}}endlabel13529: ;
return 12 * CYCLE_UNIT / 2;
}

/* MV2SR.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_46fc_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel13530; }
{{	uae_s16 src = get_iword_mmu030_state (2);
	regs.sr = src;
	MakeFromSR ();
}}}	m68k_incpci (4);
endlabel13530: ;
return 8 * CYCLE_UNIT / 2;
}

/* NBCD.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4800_32)(uae_u32 opcode)
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
	SET_NFLG (((uae_s8)(newv)) < 0);
	SET_VFLG ((tmp_newv & 0x80) != 0 && (newv & 0x80) == 0);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((newv) & 0xff);
}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* LINK.L An,#<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4808_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
{	uae_s32 offs;
	offs = get_ilong_mmu030_state (2);
{	uaecptr olda;
	olda = m68k_areg (regs, 7) - 4;
	mmufixup[1].reg = 7;
	mmufixup[1].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = olda;
{	uae_s32 src = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = (m68k_areg(regs, 7));
	m68k_areg(regs, 7) += offs;
	put_long_mmu030_state (olda, src);
}}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

#endif
/* NBCD.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4810_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
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
	SET_NFLG (((uae_s8)(newv)) < 0);
	SET_VFLG ((tmp_newv & 0x80) != 0 && (newv & 0x80) == 0);
	put_byte_mmu030_state (srca, newv);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* NBCD.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4818_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
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
	SET_NFLG (((uae_s8)(newv)) < 0);
	SET_VFLG ((tmp_newv & 0x80) != 0 && (newv & 0x80) == 0);
	put_byte_mmu030_state (srca, newv);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* NBCD.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4820_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
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
	SET_NFLG (((uae_s8)(newv)) < 0);
	SET_VFLG ((tmp_newv & 0x80) != 0 && (newv & 0x80) == 0);
	put_byte_mmu030_state (srca, newv);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* NBCD.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4828_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
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
	SET_NFLG (((uae_s8)(newv)) < 0);
	SET_VFLG ((tmp_newv & 0x80) != 0 && (newv & 0x80) == 0);
	put_byte_mmu030_state (srca, newv);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NBCD.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4830_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
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
	SET_NFLG (((uae_s8)(newv)) < 0);
	SET_VFLG ((tmp_newv & 0x80) != 0 && (newv & 0x80) == 0);
	put_byte_mmu030_state (srca, newv);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* NBCD.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4838_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
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
	SET_NFLG (((uae_s8)(newv)) < 0);
	SET_VFLG ((tmp_newv & 0x80) != 0 && (newv & 0x80) == 0);
	put_byte_mmu030_state (srca, newv);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* NBCD.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4839_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
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
	SET_NFLG (((uae_s8)(newv)) < 0);
	SET_VFLG ((tmp_newv & 0x80) != 0 && (newv & 0x80) == 0);
	put_byte_mmu030_state (srca, newv);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SWAP.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4840_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_4848_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	m68k_incpci (2);
	op_illg (opcode);
}return 4 * CYCLE_UNIT / 2;
}

#endif
/* PEA.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4850_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	put_long_mmu030_state (dsta, srca);
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* PEA.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4868_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	put_long_mmu030_state (dsta, srca);
}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* PEA.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4870_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	put_long_mmu030_state (dsta, srca);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* PEA.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4878_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	put_long_mmu030_state (dsta, srca);
}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* PEA.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4879_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	put_long_mmu030_state (dsta, srca);
}}}	m68k_incpci (6);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* PEA.L (d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_487a_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	put_long_mmu030_state (dsta, srca);
}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* PEA.L (d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_487b_32)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = dsta;
	put_long_mmu030_state (dsta, srca);
}}}}	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* EXT.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4880_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_4890_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu030_state (2);
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_word_mmu030 (srca, (mmu030_data_buffer = m68k_dreg (regs, movem_index1[dmask])));
			}
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_word_mmu030 (srca, (mmu030_data_buffer = m68k_areg (regs, movem_index1[amask])));
			}
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* MVMLE.W #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_48a0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu030_state (2);
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) - 0;
{	uae_u16 amask = mask & 0xff, dmask = (mask >> 8) & 0xff;
	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	while (amask) {
		srca -= 2;
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_word_mmu030 (srca, (mmu030_data_buffer = m68k_areg (regs, movem_index2[amask])));
			}
			mmu030_state[0]++;
		}
		movem_cnt++;
		amask = movem_next[amask];
	}
	while (dmask) {
		srca -= 2;
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_word_mmu030 (srca, (mmu030_data_buffer = m68k_dreg (regs, movem_index2[dmask])));
			}
			mmu030_state[0]++;
		}
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	m68k_areg (regs, dstreg) = srca;
}}}	m68k_incpci (4);
return 10 * CYCLE_UNIT / 2;
}

/* MVMLE.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_48a8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu030_state (2);
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_word_mmu030 (srca, (mmu030_data_buffer = m68k_dreg (regs, movem_index1[dmask])));
			}
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_word_mmu030 (srca, (mmu030_data_buffer = m68k_areg (regs, movem_index1[amask])));
			}
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMLE.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_48b0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu030_state (2);
{	uaecptr srca;
	m68k_incpci (4);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_word_mmu030 (srca, (mmu030_data_buffer = m68k_dreg (regs, movem_index1[dmask])));
			}
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_word_mmu030 (srca, (mmu030_data_buffer = m68k_areg (regs, movem_index1[amask])));
			}
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MVMLE.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_48b8_32)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu030_state (2);
{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_word_mmu030 (srca, (mmu030_data_buffer = m68k_dreg (regs, movem_index1[dmask])));
			}
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_word_mmu030 (srca, (mmu030_data_buffer = m68k_areg (regs, movem_index1[amask])));
			}
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMLE.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_48b9_32)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu030_state (2);
{	uaecptr srca;
	srca = get_ilong_mmu030_state (4);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_word_mmu030 (srca, (mmu030_data_buffer = m68k_dreg (regs, movem_index1[dmask])));
			}
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_word_mmu030 (srca, (mmu030_data_buffer = m68k_areg (regs, movem_index1[amask])));
			}
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

/* EXT.L Dn */
uae_u32 REGPARAM2 CPUFUNC(op_48c0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_48d0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu030_state (2);
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_long_mmu030 (srca, (mmu030_data_buffer = m68k_dreg (regs, movem_index1[dmask])));
			}
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_long_mmu030 (srca, (mmu030_data_buffer = m68k_areg (regs, movem_index1[amask])));
			}
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* MVMLE.L #<data>.W,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_48e0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu030_state (2);
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) - 0;
{	uae_u16 amask = mask & 0xff, dmask = (mask >> 8) & 0xff;
	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	while (amask) {
		srca -= 4;
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_long_mmu030 (srca, (mmu030_data_buffer = m68k_areg (regs, movem_index2[amask])));
			}
			mmu030_state[0]++;
		}
		movem_cnt++;
		amask = movem_next[amask];
	}
	while (dmask) {
		srca -= 4;
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_long_mmu030 (srca, (mmu030_data_buffer = m68k_dreg (regs, movem_index2[dmask])));
			}
			mmu030_state[0]++;
		}
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	m68k_areg (regs, dstreg) = srca;
}}}	m68k_incpci (4);
return 10 * CYCLE_UNIT / 2;
}

/* MVMLE.L #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_48e8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu030_state (2);
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_long_mmu030 (srca, (mmu030_data_buffer = m68k_dreg (regs, movem_index1[dmask])));
			}
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_long_mmu030 (srca, (mmu030_data_buffer = m68k_areg (regs, movem_index1[amask])));
			}
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMLE.L #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_48f0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu030_state (2);
{	uaecptr srca;
	m68k_incpci (4);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_long_mmu030 (srca, (mmu030_data_buffer = m68k_dreg (regs, movem_index1[dmask])));
			}
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_long_mmu030 (srca, (mmu030_data_buffer = m68k_areg (regs, movem_index1[amask])));
			}
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MVMLE.L #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_48f8_32)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu030_state (2);
{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_long_mmu030 (srca, (mmu030_data_buffer = m68k_dreg (regs, movem_index1[dmask])));
			}
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_long_mmu030 (srca, (mmu030_data_buffer = m68k_areg (regs, movem_index1[amask])));
			}
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMLE.L #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_48f9_32)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu030_state (2);
{	uaecptr srca;
	srca = get_ilong_mmu030_state (4);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_long_mmu030 (srca, (mmu030_data_buffer = m68k_dreg (regs, movem_index1[dmask])));
			}
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
			} else {
				put_long_mmu030 (srca, (mmu030_data_buffer = m68k_areg (regs, movem_index1[amask])));
			}
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

/* EXT.B Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_49c0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_4a00_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
	optflag_testb ((uae_s8)(src));
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* TST.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a10_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* TST.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4a18_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* TST.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a20_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* TST.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a28_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* TST.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4a30_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
	optflag_testb ((uae_s8)(src));
}}}}return 12 * CYCLE_UNIT / 2;
}

/* TST.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4a38_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* TST.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4a39_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TST.B (d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4a3a_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TST.B (d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4a3b_32)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
	optflag_testb ((uae_s8)(src));
}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* TST.B #<data>.B */
uae_u32 REGPARAM2 CPUFUNC(op_4a3c_32)(uae_u32 opcode)
{
{{	uae_s8 src = get_ibyte_mmu030_state (2);
	optflag_testb ((uae_s8)(src));
}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* TST.W Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4a40_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	optflag_testw ((uae_s16)(src));
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* TST.W An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4a48_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_areg (regs, srcreg);
	optflag_testw ((uae_s16)(src));
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

#endif
/* TST.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a50_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* TST.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4a58_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* TST.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a60_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* TST.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a68_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* TST.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4a70_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
	optflag_testw ((uae_s16)(src));
}}}}return 12 * CYCLE_UNIT / 2;
}

/* TST.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4a78_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* TST.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4a79_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TST.W (d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4a7a_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
	optflag_testw ((uae_s16)(src));
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TST.W (d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4a7b_32)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
	optflag_testw ((uae_s16)(src));
}}}}return 12 * CYCLE_UNIT / 2;
}

#endif
/* TST.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_4a7c_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
	optflag_testw ((uae_s16)(src));
}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* TST.L Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4a80_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
	optflag_testl ((uae_s32)(src));
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* TST.L An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4a88_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg (regs, srcreg);
	optflag_testl ((uae_s32)(src));
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

#endif
/* TST.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4a90_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* TST.L (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4a98_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* TST.L -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4aa0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* TST.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4aa8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* TST.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4ab0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
	optflag_testl ((uae_s32)(src));
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TST.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4ab8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* TST.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4ab9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* TST.L (d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4aba_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
	optflag_testl ((uae_s32)(src));
}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

#endif
/* TST.L (d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4abb_32)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
	optflag_testl ((uae_s32)(src));
}}}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* TST.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_4abc_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
	optflag_testl ((uae_s32)(src));
}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* TAS.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_4ac0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_4ad0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_lrmw_byte_mmu030_state (srca);
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	put_lrmw_byte_mmu030_state (srca, src);
}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* TAS.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4ad8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_lrmw_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	put_lrmw_byte_mmu030_state (srca, src);
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* TAS.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ae0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_lrmw_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	put_lrmw_byte_mmu030_state (srca, src);
}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* TAS.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ae8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_lrmw_byte_mmu030_state (srca);
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	put_lrmw_byte_mmu030_state (srca, src);
}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* TAS.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4af0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_lrmw_byte_mmu030_state (srca);
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	put_lrmw_byte_mmu030_state (srca, src);
}}}}return 16 * CYCLE_UNIT / 2;
}

/* TAS.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4af8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_lrmw_byte_mmu030_state (srca);
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	put_lrmw_byte_mmu030_state (srca, src);
}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* TAS.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4af9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_lrmw_byte_mmu030_state (srca);
	optflag_testb ((uae_s8)(src));
	src |= 0x80;
	put_lrmw_byte_mmu030_state (srca, src);
}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* MULL.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c00_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	m68k_mull(opcode, dst, extra);
}}}return 8 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c10_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	m68k_incpci (4);
	m68k_mull(opcode, dst, extra);
}}}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c18_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
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
uae_u32 REGPARAM2 CPUFUNC(op_4c20_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu030_state (dsta);
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
uae_u32 REGPARAM2 CPUFUNC(op_4c28_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	m68k_incpci (6);
	m68k_mull(opcode, dst, extra);
}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c30_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	m68k_mull(opcode, dst, extra);
}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c38_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	m68k_incpci (6);
	m68k_mull(opcode, dst, extra);
}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c39_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	m68k_incpci (8);
	m68k_mull(opcode, dst, extra);
}}}}return 24 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c3a_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	m68k_incpci (6);
	m68k_mull(opcode, dst, extra);
}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c3b_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	m68k_mull(opcode, dst, extra);
}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* MULL.L #<data>.W,#<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c3c_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uae_s32 dst;
	dst = get_ilong_mmu030_state (4);
	m68k_incpci (8);
	m68k_mull(opcode, dst, extra);
}}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c40_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	m68k_incpci (4);
	m68k_divl(opcode, dst, extra);
}}}return 8 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c50_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	m68k_incpci (4);
	m68k_divl(opcode, dst, extra);
}}}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c58_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
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
uae_u32 REGPARAM2 CPUFUNC(op_4c60_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu030_state (dsta);
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
uae_u32 REGPARAM2 CPUFUNC(op_4c68_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	m68k_incpci (6);
	m68k_divl(opcode, dst, extra);
}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c70_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	m68k_divl(opcode, dst, extra);
}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c78_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	m68k_incpci (6);
	m68k_divl(opcode, dst, extra);
}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c79_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	m68k_incpci (8);
	m68k_divl(opcode, dst, extra);
}}}}return 24 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c7a_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	m68k_incpci (6);
	m68k_divl(opcode, dst, extra);
}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c7b_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	m68k_divl(opcode, dst, extra);
}}}}}return 20 * CYCLE_UNIT / 2;
}

#endif
/* DIVL.L #<data>.W,#<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4c7c_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uae_s32 dst;
	dst = get_ilong_mmu030_state (4);
	m68k_incpci (8);
	m68k_divl(opcode, dst, extra);
}}}return 16 * CYCLE_UNIT / 2;
}

#endif
/* MVMEL.W #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4c90_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu030_state (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	uae_u32 val;
	if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2)
		srca = mmu030_ad[mmu030_idx].val;
	else
		mmu030_ad[mmu030_idx].val = srca;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = (uae_s32)(uae_s16)mmu030_data_buffer;
			} else {
				val = (uae_s32)(uae_s16)get_word_mmu030 (srca);
			}
			m68k_dreg (regs, movem_index1[dmask]) = val;
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = (uae_s32)(uae_s16)mmu030_data_buffer;
			} else {
				val = (uae_s32)(uae_s16)get_word_mmu030 (srca);
			}
			m68k_areg (regs, movem_index1[amask]) = val;
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4c98_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu030_state (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	uae_u32 val;
	if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2)
		srca = mmu030_ad[mmu030_idx].val;
	else
		mmu030_ad[mmu030_idx].val = srca;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = (uae_s32)(uae_s16)mmu030_data_buffer;
			} else {
				val = (uae_s32)(uae_s16)get_word_mmu030 (srca);
			}
			m68k_dreg (regs, movem_index1[dmask]) = val;
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = (uae_s32)(uae_s16)mmu030_data_buffer;
			} else {
				val = (uae_s32)(uae_s16)get_word_mmu030 (srca);
			}
			m68k_areg (regs, movem_index1[amask]) = val;
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		amask = movem_next[amask];
	}
	m68k_areg (regs, dstreg) = srca;
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ca8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu030_state (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	uae_u32 val;
	if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2)
		srca = mmu030_ad[mmu030_idx].val;
	else
		mmu030_ad[mmu030_idx].val = srca;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = (uae_s32)(uae_s16)mmu030_data_buffer;
			} else {
				val = (uae_s32)(uae_s16)get_word_mmu030 (srca);
			}
			m68k_dreg (regs, movem_index1[dmask]) = val;
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = (uae_s32)(uae_s16)mmu030_data_buffer;
			} else {
				val = (uae_s32)(uae_s16)get_word_mmu030 (srca);
			}
			m68k_areg (regs, movem_index1[amask]) = val;
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4cb0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu030_state (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	m68k_incpci (4);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	uae_u32 val;
	if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2)
		srca = mmu030_ad[mmu030_idx].val;
	else
		mmu030_ad[mmu030_idx].val = srca;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = (uae_s32)(uae_s16)mmu030_data_buffer;
			} else {
				val = (uae_s32)(uae_s16)get_word_mmu030 (srca);
			}
			m68k_dreg (regs, movem_index1[dmask]) = val;
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = (uae_s32)(uae_s16)mmu030_data_buffer;
			} else {
				val = (uae_s32)(uae_s16)get_word_mmu030 (srca);
			}
			m68k_areg (regs, movem_index1[amask]) = val;
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4cb8_32)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu030_state (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	uae_u32 val;
	if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2)
		srca = mmu030_ad[mmu030_idx].val;
	else
		mmu030_ad[mmu030_idx].val = srca;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = (uae_s32)(uae_s16)mmu030_data_buffer;
			} else {
				val = (uae_s32)(uae_s16)get_word_mmu030 (srca);
			}
			m68k_dreg (regs, movem_index1[dmask]) = val;
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = (uae_s32)(uae_s16)mmu030_data_buffer;
			} else {
				val = (uae_s32)(uae_s16)get_word_mmu030 (srca);
			}
			m68k_areg (regs, movem_index1[amask]) = val;
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4cb9_32)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu030_state (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = get_ilong_mmu030_state (4);
{	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	uae_u32 val;
	if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2)
		srca = mmu030_ad[mmu030_idx].val;
	else
		mmu030_ad[mmu030_idx].val = srca;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = (uae_s32)(uae_s16)mmu030_data_buffer;
			} else {
				val = (uae_s32)(uae_s16)get_word_mmu030 (srca);
			}
			m68k_dreg (regs, movem_index1[dmask]) = val;
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = (uae_s32)(uae_s16)mmu030_data_buffer;
			} else {
				val = (uae_s32)(uae_s16)get_word_mmu030 (srca);
			}
			m68k_areg (regs, movem_index1[amask]) = val;
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_4cba_32)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu030_state (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = m68k_getpc () + 4;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	uae_u32 val;
	if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2)
		srca = mmu030_ad[mmu030_idx].val;
	else
		mmu030_ad[mmu030_idx].val = srca;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = (uae_s32)(uae_s16)mmu030_data_buffer;
			} else {
				val = (uae_s32)(uae_s16)get_word_mmu030 (srca);
			}
			m68k_dreg (regs, movem_index1[dmask]) = val;
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = (uae_s32)(uae_s16)mmu030_data_buffer;
			} else {
				val = (uae_s32)(uae_s16)get_word_mmu030 (srca);
			}
			m68k_areg (regs, movem_index1[amask]) = val;
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.W #<data>.W,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4cbb_32)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu030_state (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	uae_u32 val;
	if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2)
		srca = mmu030_ad[mmu030_idx].val;
	else
		mmu030_ad[mmu030_idx].val = srca;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = (uae_s32)(uae_s16)mmu030_data_buffer;
			} else {
				val = (uae_s32)(uae_s16)get_word_mmu030 (srca);
			}
			m68k_dreg (regs, movem_index1[dmask]) = val;
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = (uae_s32)(uae_s16)mmu030_data_buffer;
			} else {
				val = (uae_s32)(uae_s16)get_word_mmu030 (srca);
			}
			m68k_areg (regs, movem_index1[amask]) = val;
			mmu030_state[0]++;
		}
		srca += 2;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_4cd0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu030_state (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	uae_u32 val;
	if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2)
		srca = mmu030_ad[mmu030_idx].val;
	else
		mmu030_ad[mmu030_idx].val = srca;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = mmu030_data_buffer;
			} else {
				val = get_long_mmu030 (srca);
			}
			m68k_dreg (regs, movem_index1[dmask]) = val;
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = mmu030_data_buffer;
			} else {
				val = get_long_mmu030 (srca);
			}
			m68k_areg (regs, movem_index1[amask]) = val;
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_4cd8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu030_state (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
{	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	uae_u32 val;
	if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2)
		srca = mmu030_ad[mmu030_idx].val;
	else
		mmu030_ad[mmu030_idx].val = srca;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = mmu030_data_buffer;
			} else {
				val = get_long_mmu030 (srca);
			}
			m68k_dreg (regs, movem_index1[dmask]) = val;
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = mmu030_data_buffer;
			} else {
				val = get_long_mmu030 (srca);
			}
			m68k_areg (regs, movem_index1[amask]) = val;
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		amask = movem_next[amask];
	}
	m68k_areg (regs, dstreg) = srca;
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ce8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu030_state (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	uae_u32 val;
	if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2)
		srca = mmu030_ad[mmu030_idx].val;
	else
		mmu030_ad[mmu030_idx].val = srca;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = mmu030_data_buffer;
			} else {
				val = get_long_mmu030 (srca);
			}
			m68k_dreg (regs, movem_index1[dmask]) = val;
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = mmu030_data_buffer;
			} else {
				val = get_long_mmu030 (srca);
			}
			m68k_areg (regs, movem_index1[amask]) = val;
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4cf0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_iword_mmu030_state (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	m68k_incpci (4);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	uae_u32 val;
	if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2)
		srca = mmu030_ad[mmu030_idx].val;
	else
		mmu030_ad[mmu030_idx].val = srca;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = mmu030_data_buffer;
			} else {
				val = get_long_mmu030 (srca);
			}
			m68k_dreg (regs, movem_index1[dmask]) = val;
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = mmu030_data_buffer;
			} else {
				val = get_long_mmu030 (srca);
			}
			m68k_areg (regs, movem_index1[amask]) = val;
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}}return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4cf8_32)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu030_state (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	uae_u32 val;
	if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2)
		srca = mmu030_ad[mmu030_idx].val;
	else
		mmu030_ad[mmu030_idx].val = srca;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = mmu030_data_buffer;
			} else {
				val = get_long_mmu030 (srca);
			}
			m68k_dreg (regs, movem_index1[dmask]) = val;
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = mmu030_data_buffer;
			} else {
				val = get_long_mmu030 (srca);
			}
			m68k_areg (regs, movem_index1[amask]) = val;
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4cf9_32)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu030_state (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = get_ilong_mmu030_state (4);
{	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	uae_u32 val;
	if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2)
		srca = mmu030_ad[mmu030_idx].val;
	else
		mmu030_ad[mmu030_idx].val = srca;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = mmu030_data_buffer;
			} else {
				val = get_long_mmu030 (srca);
			}
			m68k_dreg (regs, movem_index1[dmask]) = val;
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = mmu030_data_buffer;
			} else {
				val = get_long_mmu030 (srca);
			}
			m68k_areg (regs, movem_index1[amask]) = val;
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (8);
return 16 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_4cfa_32)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu030_state (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr srca;
	srca = m68k_getpc () + 4;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (4);
{	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	uae_u32 val;
	if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2)
		srca = mmu030_ad[mmu030_idx].val;
	else
		mmu030_ad[mmu030_idx].val = srca;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = mmu030_data_buffer;
			} else {
				val = get_long_mmu030 (srca);
			}
			m68k_dreg (regs, movem_index1[dmask]) = val;
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = mmu030_data_buffer;
			} else {
				val = get_long_mmu030 (srca);
			}
			m68k_areg (regs, movem_index1[amask]) = val;
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MVMEL.L #<data>.W,(d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4cfb_32)(uae_u32 opcode)
{
{	uae_u16 mask = get_iword_mmu030_state (2);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	mmu030_state[1] |= MMU030_STATEFLAG1_MOVEM1;
	int movem_cnt = 0;
	uae_u32 val;
	if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2)
		srca = mmu030_ad[mmu030_idx].val;
	else
		mmu030_ad[mmu030_idx].val = srca;
	while (dmask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = mmu030_data_buffer;
			} else {
				val = get_long_mmu030 (srca);
			}
			m68k_dreg (regs, movem_index1[dmask]) = val;
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		dmask = movem_next[dmask];
	}
	while (amask) {
		if (mmu030_state[0] == movem_cnt) {
			if (mmu030_state[1] & MMU030_STATEFLAG1_MOVEM2) {
				mmu030_state[1] &= ~MMU030_STATEFLAG1_MOVEM2;
				val = mmu030_data_buffer;
			} else {
				val = get_long_mmu030 (srca);
			}
			m68k_areg (regs, movem_index1[amask]) = val;
			mmu030_state[0]++;
		}
		srca += 4;
		movem_cnt++;
		amask = movem_next[amask];
	}
}}}}return 12 * CYCLE_UNIT / 2;
}

/* TRAPQ.L #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_4e40_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 15);
{{	uae_u32 src = srcreg;
	m68k_incpci (2);
	Exception (src + 32);
}}return 4 * CYCLE_UNIT / 2;
}

/* LINK.W An,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_4e50_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu030_state (2);
{	uaecptr olda;
	olda = m68k_areg (regs, 7) - 4;
	mmufixup[1].reg = 7;
	mmufixup[1].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) = olda;
{	uae_s32 src = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = (m68k_areg(regs, 7));
	m68k_areg(regs, 7) += offs;
	put_long_mmu030_state (olda, src);
}}}}	m68k_incpci (4);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 18 * CYCLE_UNIT / 2;
}

/* UNLK.L An */
uae_u32 REGPARAM2 CPUFUNC(op_4e58_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg (regs, srcreg);
	uae_s32 old = get_long_mmu030_state (src);
	m68k_areg (regs, 7) = src + 4;
	m68k_areg (regs, srcreg) = old;
}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* MVR2USP.L An */
uae_u32 REGPARAM2 CPUFUNC(op_4e60_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel13648; }
{{	uae_s32 src = m68k_areg (regs, srcreg);
	regs.usp = src;
}}}	m68k_incpci (2);
endlabel13648: ;
return 4 * CYCLE_UNIT / 2;
}

/* MVUSP2R.L An */
uae_u32 REGPARAM2 CPUFUNC(op_4e68_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel13649; }
{{	m68k_areg (regs, srcreg) = (regs.usp);
}}}	m68k_incpci (2);
endlabel13649: ;
return 4 * CYCLE_UNIT / 2;
}

/* RESET.L  */
uae_u32 REGPARAM2 CPUFUNC(op_4e70_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel13650; }
{	cpureset ();
	m68k_incpci (2);
}}endlabel13650: ;
return 4 * CYCLE_UNIT / 2;
}

/* NOP.L  */
uae_u32 REGPARAM2 CPUFUNC(op_4e71_32)(uae_u32 opcode)
{
{}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* STOP.L #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_4e72_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel13652; }
{{	uae_s16 src = get_iword_mmu030_state (2);
	regs.sr = src;
	MakeFromSR ();
	m68k_setstopped ();
	m68k_incpci (4);
}}}endlabel13652: ;
return 8 * CYCLE_UNIT / 2;
}

/* RTE.L  */
uae_u32 REGPARAM2 CPUFUNC(op_4e73_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel13653; }
{	uae_u16 newsr; uae_u32 newpc;
	for (;;) {
		uaecptr a = m68k_areg (regs, 7);
		uae_u16 sr = get_word_mmu030_state (a);
		uae_u32 pc = get_long_mmu030_state (a + 2);
		uae_u16 format = get_word_mmu030_state (a + 2 + 4);
		int frame = format >> 12;
		int offset = 8;
		newsr = sr; newpc = pc;
		if (frame == 0x0) { m68k_areg (regs, 7) += offset; break; }
		else if (frame == 0x1) { m68k_areg (regs, 7) += offset; }
		else if (frame == 0x2) { m68k_areg (regs, 7) += offset + 4; break; }
		else if (frame == 0x4) { m68k_areg (regs, 7) += offset + 8; break; }
		else if (frame == 0x8) { m68k_areg (regs, 7) += offset + 50; break; }
		else if (frame == 0x7) { m68k_areg (regs, 7) += offset + 52; break; }
		else if (frame == 0x9) { m68k_areg (regs, 7) += offset + 12; break; }
		else if (frame == 0xa) { m68k_do_rte_mmu030 (a); break; }
		else if (frame == 0xb) { m68k_do_rte_mmu030 (a); break; }
		else { m68k_areg (regs, 7) += offset; Exception (14); goto endlabel13653; }
		regs.sr = newsr; MakeFromSR ();
}
	regs.sr = newsr; MakeFromSR ();
	if (newpc & 1) {
		exception3i (0x4E73, newpc);
		goto endlabel13653;
	}
	m68k_setpc_mmu (newpc);
	ipl_fetch ();
}}endlabel13653: ;
return 4 * CYCLE_UNIT / 2;
}

/* RTD.L #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_4e74_32)(uae_u32 opcode)
{
{{	uae_s16 offs = get_iword_mmu030_state (2);
{	uaecptr pca;
	pca = m68k_areg (regs, 7);
{	uae_s32 pc = get_long_mmu030_state (pca);
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) += 4;
	m68k_areg(regs, 7) += offs;
	if (pc & 1) {
		exception3i (0x4E74, pc);
		goto endlabel13654;
	}
	m68k_setpc_mmu (pc);
}}}}endlabel13654: ;
	mmufixup[0].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* RTS.L  */
uae_u32 REGPARAM2 CPUFUNC(op_4e75_32)(uae_u32 opcode)
{
{	uaecptr pc = m68k_getpc ();
	m68k_do_rts_mmu030 ();
	if (m68k_getpc () & 1) {
		uaecptr faultpc = m68k_getpc ();
	m68k_setpc_mmu (pc);
		exception3i (0x4E75, faultpc);
	}
}return 4 * CYCLE_UNIT / 2;
}

/* TRAPV.L  */
uae_u32 REGPARAM2 CPUFUNC(op_4e76_32)(uae_u32 opcode)
{
{	m68k_incpci (2);
	if (GET_VFLG ()) {
		Exception (7);
		goto endlabel13656;
	}
}endlabel13656: ;
return 4 * CYCLE_UNIT / 2;
}

/* RTR.L  */
uae_u32 REGPARAM2 CPUFUNC(op_4e77_32)(uae_u32 opcode)
{
{	uaecptr oldpc = m68k_getpc ();
	MakeSR ();
{	uaecptr sra;
	sra = m68k_areg (regs, 7);
{	uae_s16 sr = get_word_mmu030_state (sra);
	mmufixup[0].reg = 7;
	mmufixup[0].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) += 2;
{	uaecptr pca;
	pca = m68k_areg (regs, 7);
{	uae_s32 pc = get_long_mmu030_state (pca);
	mmufixup[1].reg = 7;
	mmufixup[1].value = m68k_areg (regs, 7);
	m68k_areg (regs, 7) += 4;
	regs.sr &= 0xFF00; sr &= 0xFF;
	regs.sr |= sr;
	m68k_setpc_mmu (pc);
	MakeFromSR ();
	if (m68k_getpc () & 1) {
		uaecptr faultpc = m68k_getpc ();
	m68k_setpc_mmu (oldpc);
		exception3i (0x4E77, faultpc);
	}
}}}}}	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* MOVEC2.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4e7a_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel13658; }
{{	uae_s16 src = get_iword_mmu030_state (2);
{	int regno = (src >> 12) & 15;
	uae_u32 *regp = regs.regs + regno;
	if (! m68k_movec2(src & 0xFFF, regp)) goto endlabel13658;
}}}}	m68k_incpci (4);
endlabel13658: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* MOVE2C.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_4e7b_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel13659; }
{{	uae_s16 src = get_iword_mmu030_state (2);
{	int regno = (src >> 12) & 15;
	uae_u32 *regp = regs.regs + regno;
	if (! m68k_move2c(src & 0xFFF, regp)) goto endlabel13659;
}}}}	m68k_incpci (4);
endlabel13659: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* JSR.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4e90_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uaecptr oldpc = m68k_getpc () + 2;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel13660;
	}
	put_long_mmu030_state (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}endlabel13660: ;
return 4 * CYCLE_UNIT / 2;
}

/* JSR.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ea8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uaecptr oldpc = m68k_getpc () + 4;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel13661;
	}
	put_long_mmu030_state (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}endlabel13661: ;
return 8 * CYCLE_UNIT / 2;
}

/* JSR.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4eb0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uaecptr oldpc = m68k_getpc () + 0;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel13662;
	}
	put_long_mmu030_state (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}}endlabel13662: ;
return 8 * CYCLE_UNIT / 2;
}

/* JSR.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4eb8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uaecptr oldpc = m68k_getpc () + 4;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel13663;
	}
	put_long_mmu030_state (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}endlabel13663: ;
return 8 * CYCLE_UNIT / 2;
}

/* JSR.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4eb9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uaecptr oldpc = m68k_getpc () + 6;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel13664;
	}
	put_long_mmu030_state (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}endlabel13664: ;
return 12 * CYCLE_UNIT / 2;
}

/* JSR.L (d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_4eba_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uaecptr oldpc = m68k_getpc () + 4;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel13665;
	}
	put_long_mmu030_state (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}endlabel13665: ;
return 8 * CYCLE_UNIT / 2;
}

/* JSR.L (d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4ebb_32)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uaecptr oldpc = m68k_getpc () + 0;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel13666;
	}
	put_long_mmu030_state (m68k_areg (regs, 7) - 4, oldpc);
	m68k_areg (regs, 7) -= 4;
	m68k_setpc_mmu (srca);
}}}}endlabel13666: ;
return 8 * CYCLE_UNIT / 2;
}

/* JMP.L (An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ed0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel13667;
	}
	m68k_setpc_mmu (srca);
}}endlabel13667: ;
return 4 * CYCLE_UNIT / 2;
}

/* JMP.L (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_4ee8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel13668;
	}
	m68k_setpc_mmu (srca);
}}endlabel13668: ;
return 8 * CYCLE_UNIT / 2;
}

/* JMP.L (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4ef0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel13669;
	}
	m68k_setpc_mmu (srca);
}}}endlabel13669: ;
return 8 * CYCLE_UNIT / 2;
}

/* JMP.L (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_4ef8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel13670;
	}
	m68k_setpc_mmu (srca);
}}endlabel13670: ;
return 8 * CYCLE_UNIT / 2;
}

/* JMP.L (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_4ef9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel13671;
	}
	m68k_setpc_mmu (srca);
}}endlabel13671: ;
return 12 * CYCLE_UNIT / 2;
}

/* JMP.L (d16,PC) */
uae_u32 REGPARAM2 CPUFUNC(op_4efa_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel13672;
	}
	m68k_setpc_mmu (srca);
}}endlabel13672: ;
return 8 * CYCLE_UNIT / 2;
}

/* JMP.L (d8,PC,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_4efb_32)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel13673;
	}
	m68k_setpc_mmu (srca);
}}}endlabel13673: ;
return 8 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5000_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_5010_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5018_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5020_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5028_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5030_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5038_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADDQ.B #<data>,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5039_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5040_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_5048_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_5050_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5058_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5060_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5068_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5070_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

#endif

#ifdef PART_5
/* ADDQ.W #<data>,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5078_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADDQ.W #<data>,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5079_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5080_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_5088_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_5090_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5098_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_50a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_50a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_50b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_50b8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* ADDQ.L #<data>,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_50b9_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_50c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (0) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_50c8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu030_state (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (0)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel13701;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel13701: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_50d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (0) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_50d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (0) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_50e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (0) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_50e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (0) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_50f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (0) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_50f8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (0) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_50f9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{{	int val = cctrue (0) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_50fa_32)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu030_state (2);
	if (cctrue (0)) { Exception (7); goto endlabel13709; }
}}	m68k_incpci (4);
endlabel13709: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_50fb_32)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu030_state (2);
	if (cctrue (0)) { Exception (7); goto endlabel13710; }
}}	m68k_incpci (6);
endlabel13710: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_50fc_32)(uae_u32 opcode)
{
{	if (cctrue (0)) { Exception (7); goto endlabel13711; }
}	m68k_incpci (2);
endlabel13711: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* SUBQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5100_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_5110_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5118_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5120_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5128_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5130_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5138_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUBQ.B #<data>,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5139_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5140_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_5148_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_5150_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5158_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5160_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5168_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5170_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5178_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUBQ.W #<data>,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5179_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5180_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_5188_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_5190_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5198_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_51a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_51a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_51b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_51b8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* SUBQ.L #<data>,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_51b9_32)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_51c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (1) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_51c8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu030_state (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (1)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel13739;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel13739: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_51d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (1) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_51d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (1) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_51e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (1) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_51e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (1) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_51f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (1) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_51f8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (1) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_51f9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{{	int val = cctrue (1) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_51fa_32)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu030_state (2);
	if (cctrue (1)) { Exception (7); goto endlabel13747; }
}}	m68k_incpci (4);
endlabel13747: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_51fb_32)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu030_state (2);
	if (cctrue (1)) { Exception (7); goto endlabel13748; }
}}	m68k_incpci (6);
endlabel13748: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_51fc_32)(uae_u32 opcode)
{
{	if (cctrue (1)) { Exception (7); goto endlabel13749; }
}	m68k_incpci (2);
endlabel13749: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_52c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (2) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_52c8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu030_state (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (2)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel13751;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel13751: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_52d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (2) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_52d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (2) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_52e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (2) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_52e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (2) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_52f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (2) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_52f8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (2) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_52f9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{{	int val = cctrue (2) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_52fa_32)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu030_state (2);
	if (cctrue (2)) { Exception (7); goto endlabel13759; }
}}	m68k_incpci (4);
endlabel13759: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_52fb_32)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu030_state (2);
	if (cctrue (2)) { Exception (7); goto endlabel13760; }
}}	m68k_incpci (6);
endlabel13760: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_52fc_32)(uae_u32 opcode)
{
{	if (cctrue (2)) { Exception (7); goto endlabel13761; }
}	m68k_incpci (2);
endlabel13761: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_53c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (3) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_53c8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu030_state (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (3)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel13763;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel13763: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_53d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (3) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_53d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (3) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_53e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (3) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_53e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (3) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_53f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (3) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_53f8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (3) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_53f9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{{	int val = cctrue (3) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_53fa_32)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu030_state (2);
	if (cctrue (3)) { Exception (7); goto endlabel13771; }
}}	m68k_incpci (4);
endlabel13771: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_53fb_32)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu030_state (2);
	if (cctrue (3)) { Exception (7); goto endlabel13772; }
}}	m68k_incpci (6);
endlabel13772: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_53fc_32)(uae_u32 opcode)
{
{	if (cctrue (3)) { Exception (7); goto endlabel13773; }
}	m68k_incpci (2);
endlabel13773: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_54c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (4) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_54c8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu030_state (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (4)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel13775;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel13775: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_54d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (4) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_54d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (4) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_54e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (4) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_54e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (4) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_54f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (4) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_54f8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (4) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_54f9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{{	int val = cctrue (4) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_54fa_32)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu030_state (2);
	if (cctrue (4)) { Exception (7); goto endlabel13783; }
}}	m68k_incpci (4);
endlabel13783: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_54fb_32)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu030_state (2);
	if (cctrue (4)) { Exception (7); goto endlabel13784; }
}}	m68k_incpci (6);
endlabel13784: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_54fc_32)(uae_u32 opcode)
{
{	if (cctrue (4)) { Exception (7); goto endlabel13785; }
}	m68k_incpci (2);
endlabel13785: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_55c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (5) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_55c8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu030_state (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (5)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel13787;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel13787: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_55d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (5) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_55d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (5) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_55e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (5) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_55e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (5) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_55f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (5) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_55f8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (5) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_55f9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{{	int val = cctrue (5) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_55fa_32)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu030_state (2);
	if (cctrue (5)) { Exception (7); goto endlabel13795; }
}}	m68k_incpci (4);
endlabel13795: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_55fb_32)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu030_state (2);
	if (cctrue (5)) { Exception (7); goto endlabel13796; }
}}	m68k_incpci (6);
endlabel13796: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_55fc_32)(uae_u32 opcode)
{
{	if (cctrue (5)) { Exception (7); goto endlabel13797; }
}	m68k_incpci (2);
endlabel13797: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_56c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (6) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_56c8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu030_state (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (6)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel13799;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel13799: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_56d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (6) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_56d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (6) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_56e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (6) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_56e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (6) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_56f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (6) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_56f8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (6) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_56f9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{{	int val = cctrue (6) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_56fa_32)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu030_state (2);
	if (cctrue (6)) { Exception (7); goto endlabel13807; }
}}	m68k_incpci (4);
endlabel13807: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_56fb_32)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu030_state (2);
	if (cctrue (6)) { Exception (7); goto endlabel13808; }
}}	m68k_incpci (6);
endlabel13808: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_56fc_32)(uae_u32 opcode)
{
{	if (cctrue (6)) { Exception (7); goto endlabel13809; }
}	m68k_incpci (2);
endlabel13809: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_57c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (7) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_57c8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu030_state (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (7)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel13811;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel13811: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_57d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (7) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_57d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (7) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_57e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (7) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_57e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (7) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_57f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (7) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_57f8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (7) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_57f9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{{	int val = cctrue (7) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_57fa_32)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu030_state (2);
	if (cctrue (7)) { Exception (7); goto endlabel13819; }
}}	m68k_incpci (4);
endlabel13819: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_57fb_32)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu030_state (2);
	if (cctrue (7)) { Exception (7); goto endlabel13820; }
}}	m68k_incpci (6);
endlabel13820: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_57fc_32)(uae_u32 opcode)
{
{	if (cctrue (7)) { Exception (7); goto endlabel13821; }
}	m68k_incpci (2);
endlabel13821: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_58c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (8) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_58c8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu030_state (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (8)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel13823;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel13823: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_58d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (8) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_58d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (8) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_58e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (8) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_58e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (8) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_58f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (8) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_58f8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (8) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_58f9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{{	int val = cctrue (8) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_58fa_32)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu030_state (2);
	if (cctrue (8)) { Exception (7); goto endlabel13831; }
}}	m68k_incpci (4);
endlabel13831: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_58fb_32)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu030_state (2);
	if (cctrue (8)) { Exception (7); goto endlabel13832; }
}}	m68k_incpci (6);
endlabel13832: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_58fc_32)(uae_u32 opcode)
{
{	if (cctrue (8)) { Exception (7); goto endlabel13833; }
}	m68k_incpci (2);
endlabel13833: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_59c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (9) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_59c8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu030_state (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (9)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel13835;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel13835: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_59d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (9) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_59d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (9) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_59e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (9) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_59e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (9) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_59f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (9) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_59f8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (9) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_59f9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{{	int val = cctrue (9) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_59fa_32)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu030_state (2);
	if (cctrue (9)) { Exception (7); goto endlabel13843; }
}}	m68k_incpci (4);
endlabel13843: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_59fb_32)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu030_state (2);
	if (cctrue (9)) { Exception (7); goto endlabel13844; }
}}	m68k_incpci (6);
endlabel13844: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_59fc_32)(uae_u32 opcode)
{
{	if (cctrue (9)) { Exception (7); goto endlabel13845; }
}	m68k_incpci (2);
endlabel13845: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5ac0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (10) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_5ac8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu030_state (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (10)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel13847;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel13847: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ad0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (10) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5ad8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (10) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ae0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (10) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ae8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (10) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5af0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (10) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5af8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (10) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5af9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{{	int val = cctrue (10) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5afa_32)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu030_state (2);
	if (cctrue (10)) { Exception (7); goto endlabel13855; }
}}	m68k_incpci (4);
endlabel13855: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5afb_32)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu030_state (2);
	if (cctrue (10)) { Exception (7); goto endlabel13856; }
}}	m68k_incpci (6);
endlabel13856: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5afc_32)(uae_u32 opcode)
{
{	if (cctrue (10)) { Exception (7); goto endlabel13857; }
}	m68k_incpci (2);
endlabel13857: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5bc0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (11) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_5bc8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu030_state (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (11)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel13859;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel13859: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_5bd0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (11) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5bd8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (11) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5be0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (11) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5be8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (11) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5bf0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (11) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5bf8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (11) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5bf9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{{	int val = cctrue (11) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5bfa_32)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu030_state (2);
	if (cctrue (11)) { Exception (7); goto endlabel13867; }
}}	m68k_incpci (4);
endlabel13867: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5bfb_32)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu030_state (2);
	if (cctrue (11)) { Exception (7); goto endlabel13868; }
}}	m68k_incpci (6);
endlabel13868: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5bfc_32)(uae_u32 opcode)
{
{	if (cctrue (11)) { Exception (7); goto endlabel13869; }
}	m68k_incpci (2);
endlabel13869: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5cc0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (12) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_5cc8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu030_state (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (12)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel13871;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel13871: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_5cd0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (12) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5cd8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (12) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ce0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (12) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ce8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (12) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5cf0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (12) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5cf8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (12) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5cf9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{{	int val = cctrue (12) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5cfa_32)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu030_state (2);
	if (cctrue (12)) { Exception (7); goto endlabel13879; }
}}	m68k_incpci (4);
endlabel13879: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5cfb_32)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu030_state (2);
	if (cctrue (12)) { Exception (7); goto endlabel13880; }
}}	m68k_incpci (6);
endlabel13880: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5cfc_32)(uae_u32 opcode)
{
{	if (cctrue (12)) { Exception (7); goto endlabel13881; }
}	m68k_incpci (2);
endlabel13881: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5dc0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (13) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_5dc8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu030_state (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (13)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel13883;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel13883: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_5dd0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (13) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5dd8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (13) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5de0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (13) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5de8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (13) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5df0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (13) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5df8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (13) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5df9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{{	int val = cctrue (13) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5dfa_32)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu030_state (2);
	if (cctrue (13)) { Exception (7); goto endlabel13891; }
}}	m68k_incpci (4);
endlabel13891: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5dfb_32)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu030_state (2);
	if (cctrue (13)) { Exception (7); goto endlabel13892; }
}}	m68k_incpci (6);
endlabel13892: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5dfc_32)(uae_u32 opcode)
{
{	if (cctrue (13)) { Exception (7); goto endlabel13893; }
}	m68k_incpci (2);
endlabel13893: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5ec0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (14) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_5ec8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu030_state (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (14)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel13895;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel13895: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ed0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (14) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5ed8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (14) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ee0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (14) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5ee8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (14) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5ef0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (14) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5ef8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (14) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5ef9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{{	int val = cctrue (14) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5efa_32)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu030_state (2);
	if (cctrue (14)) { Exception (7); goto endlabel13903; }
}}	m68k_incpci (4);
endlabel13903: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5efb_32)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu030_state (2);
	if (cctrue (14)) { Exception (7); goto endlabel13904; }
}}	m68k_incpci (6);
endlabel13904: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5efc_32)(uae_u32 opcode)
{
{	if (cctrue (14)) { Exception (7); goto endlabel13905; }
}	m68k_incpci (2);
endlabel13905: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Scc.B Dn */
uae_u32 REGPARAM2 CPUFUNC(op_5fc0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{{{	int val = cctrue (15) ? 0xff : 0;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* DBcc.W Dn,#<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_5fc8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = get_iword_mmu030_state (2);
	uaecptr oldpc = m68k_getpc ();
	if (!cctrue (15)) {
	m68k_incpci ((uae_s32)offs + 2);
			m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel13907;
			}
			return 12 * CYCLE_UNIT / 2;
		}
	} else {
	}
	m68k_setpc_mmu (oldpc + 4);
}}}endlabel13907: ;
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (An) */
uae_u32 REGPARAM2 CPUFUNC(op_5fd0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{{	int val = cctrue (15) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_5fd8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{{	int val = cctrue (15) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* Scc.B -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_5fe0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{{	int val = cctrue (15) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* Scc.B (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_5fe8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (15) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_5ff0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{{	int val = cctrue (15) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_5ff8_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{{	int val = cctrue (15) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* Scc.B (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_5ff9_32)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{{	int val = cctrue (15) ? 0xff : 0;
	put_byte_mmu030_state (srca, val);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* TRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5ffa_32)(uae_u32 opcode)
{
{{	uae_s16 dummy = get_iword_mmu030_state (2);
	if (cctrue (15)) { Exception (7); goto endlabel13915; }
}}	m68k_incpci (4);
endlabel13915: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5ffb_32)(uae_u32 opcode)
{
{{	uae_s32 dummy;
	dummy = get_ilong_mmu030_state (2);
	if (cctrue (15)) { Exception (7); goto endlabel13916; }
}}	m68k_incpci (6);
endlabel13916: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* TRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_5ffc_32)(uae_u32 opcode)
{
{	if (cctrue (15)) { Exception (7); goto endlabel13917; }
}	m68k_incpci (2);
endlabel13917: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6000_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
	if (!cctrue (0)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13918;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel13918: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6001_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (0)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13919;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel13919: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_60ff_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
	if (!cctrue (0)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13920;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel13920: ;
return 12 * CYCLE_UNIT / 2;
}

/* BSR.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6100_32)(uae_u32 opcode)
{
{	uae_s32 s;
{	uae_s16 src = get_iword_mmu030_state (2);
	s = (uae_s32)src + 2;
	if (src & 1) {
		exception3pc (opcode, m68k_getpc () + s, 0, 1, m68k_getpc () + s);
		goto endlabel13921;
	}
	m68k_do_bsr_mmu030 (m68k_getpc () + 4, s);
}}endlabel13921: ;
return 8 * CYCLE_UNIT / 2;
}

/* BSRQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6101_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	uae_s32 s;
{	uae_u32 src = srcreg;
	s = (uae_s32)src + 2;
	if (src & 1) {
		exception3pc (opcode, m68k_getpc () + s, 0, 1, m68k_getpc () + s);
		goto endlabel13922;
	}
	m68k_do_bsr_mmu030 (m68k_getpc () + 2, s);
}}endlabel13922: ;
return 4 * CYCLE_UNIT / 2;
}

/* BSR.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_61ff_32)(uae_u32 opcode)
{
{	uae_s32 s;
{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
	s = (uae_s32)src + 2;
	if (src & 1) {
		exception3pc (opcode, m68k_getpc () + s, 0, 1, m68k_getpc () + s);
		goto endlabel13923;
	}
	m68k_do_bsr_mmu030 (m68k_getpc () + 6, s);
}}endlabel13923: ;
return 12 * CYCLE_UNIT / 2;
}

#endif

#ifdef PART_6
/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6200_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
	if (!cctrue (2)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13924;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel13924: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6201_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (2)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13925;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel13925: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_62ff_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
	if (!cctrue (2)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13926;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel13926: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6300_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
	if (!cctrue (3)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13927;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel13927: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6301_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (3)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13928;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel13928: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_63ff_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
	if (!cctrue (3)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13929;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel13929: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6400_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
	if (!cctrue (4)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13930;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel13930: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6401_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (4)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13931;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel13931: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_64ff_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
	if (!cctrue (4)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13932;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel13932: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6500_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
	if (!cctrue (5)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13933;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel13933: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6501_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (5)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13934;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel13934: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_65ff_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
	if (!cctrue (5)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13935;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel13935: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6600_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
	if (!cctrue (6)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13936;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel13936: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6601_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (6)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13937;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel13937: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_66ff_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
	if (!cctrue (6)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13938;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel13938: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6700_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
	if (!cctrue (7)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13939;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel13939: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6701_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (7)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13940;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel13940: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_67ff_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
	if (!cctrue (7)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13941;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel13941: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6800_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
	if (!cctrue (8)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13942;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel13942: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6801_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (8)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13943;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel13943: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_68ff_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
	if (!cctrue (8)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13944;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel13944: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6900_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
	if (!cctrue (9)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13945;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel13945: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6901_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (9)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13946;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel13946: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_69ff_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
	if (!cctrue (9)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13947;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel13947: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6a00_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
	if (!cctrue (10)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13948;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel13948: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6a01_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (10)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13949;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel13949: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_6aff_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
	if (!cctrue (10)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13950;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel13950: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6b00_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
	if (!cctrue (11)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13951;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel13951: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6b01_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (11)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13952;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel13952: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_6bff_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
	if (!cctrue (11)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13953;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel13953: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6c00_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
	if (!cctrue (12)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13954;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel13954: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6c01_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (12)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13955;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel13955: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_6cff_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
	if (!cctrue (12)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13956;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel13956: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6d00_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
	if (!cctrue (13)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13957;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel13957: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6d01_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (13)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13958;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel13958: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_6dff_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
	if (!cctrue (13)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13959;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel13959: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6e00_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
	if (!cctrue (14)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13960;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel13960: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6e01_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (14)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13961;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel13961: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_6eff_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
	if (!cctrue (14)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13962;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel13962: ;
return 12 * CYCLE_UNIT / 2;
}

/* Bcc.W #<data>.W */
uae_u32 REGPARAM2 CPUFUNC(op_6f00_32)(uae_u32 opcode)
{
{{	uae_s16 src = get_iword_mmu030_state (2);
	if (!cctrue (15)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13963;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (4);
}}endlabel13963: ;
return 12 * CYCLE_UNIT / 2;
}

/* BccQ.B #<data> */
uae_u32 REGPARAM2 CPUFUNC(op_6f01_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	if (!cctrue (15)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13964;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (2);
}}endlabel13964: ;
return 8 * CYCLE_UNIT / 2;
}

/* Bcc.L #<data>.L */
uae_u32 REGPARAM2 CPUFUNC(op_6fff_32)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
	if (!cctrue (15)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel13965;
	}
	m68k_incpci ((uae_s32)src + 2);
	return 10 * CYCLE_UNIT / 2;
didnt_jump:;
	m68k_incpci (6);
}}endlabel13965: ;
return 12 * CYCLE_UNIT / 2;
}

/* MOVEQ.L #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_7000_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_8000_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_8010_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* OR.B (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8018_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* OR.B -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8020_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* OR.B (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8028_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.B (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8030_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* OR.B (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8038_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.B (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8039_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* OR.B (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_803a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.B (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_803b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* OR.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_803c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8040_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_8050_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* OR.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8058_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* OR.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8060_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* OR.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8068_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8070_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* OR.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8078_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8079_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* OR.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_807a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* OR.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_807b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* OR.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_807c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* OR.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8080_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_8090_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* OR.L (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8098_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* OR.L -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* OR.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.L (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.L (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80b8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.L (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80b9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* OR.L (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80ba_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.L (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80bb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80bc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* DIVU.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80c0_32)(uae_u32 opcode)
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
		goto endlabel14000;
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
}}}endlabel14000: ;
return 110 * CYCLE_UNIT / 2;
}

/* DIVU.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel14001;
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
}}}}endlabel14001: ;
return 114 * CYCLE_UNIT / 2;
}

/* DIVU.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel14002;
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
}}}}endlabel14002: ;
	mmufixup[0].reg = -1;
return 114 * CYCLE_UNIT / 2;
}

/* DIVU.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel14003;
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
}}}}endlabel14003: ;
	mmufixup[0].reg = -1;
return 116 * CYCLE_UNIT / 2;
}

/* DIVU.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel14004;
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
}}}}endlabel14004: ;
return 118 * CYCLE_UNIT / 2;
}

/* DIVU.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (0);
		Exception (5);
		goto endlabel14005;
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
}}}}}endlabel14005: ;
return 118 * CYCLE_UNIT / 2;
}

/* DIVU.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80f8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel14006;
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
}}}}endlabel14006: ;
return 118 * CYCLE_UNIT / 2;
}

/* DIVU.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80f9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (6);
		Exception (5);
		goto endlabel14007;
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
}}}}endlabel14007: ;
return 122 * CYCLE_UNIT / 2;
}

/* DIVU.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80fa_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel14008;
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
}}}}endlabel14008: ;
return 118 * CYCLE_UNIT / 2;
}

/* DIVU.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80fb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (0);
		Exception (5);
		goto endlabel14009;
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
}}}}}endlabel14009: ;
return 118 * CYCLE_UNIT / 2;
}

/* DIVU.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_80fc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
		divbyzero_special (0, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel14010;
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
}}}endlabel14010: ;
return 114 * CYCLE_UNIT / 2;
}

/* SBCD.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_8100_32)(uae_u32 opcode)
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
	SET_NFLG (((uae_s8)(newv)) < 0);
	SET_VFLG ((tmp_newv & 0x80) != 0 && (newv & 0x80) == 0);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* SBCD.B -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_8108_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
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
	SET_NFLG (((uae_s8)(newv)) < 0);
	SET_VFLG ((tmp_newv & 0x80) != 0 && (newv & 0x80) == 0);
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* OR.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_8110_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* OR.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_8118_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* OR.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_8120_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* OR.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_8128_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_8130_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_8138_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_8139_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* PACK.L Dn,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_8140_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uae_u16 val = m68k_dreg (regs, srcreg) + get_iword_mmu030_state (2);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & 0xffffff00) | ((val >> 4) & 0xf0) | (val & 0xf);
}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* PACK.L -(An),-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_8148_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uae_u16 val;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) -= areg_byteinc[srcreg];
	val = (uae_u16)get_byte_mmu030_state (m68k_areg (regs, srcreg));
	m68k_areg (regs, srcreg) -= areg_byteinc[srcreg];
	val = (val | ((uae_u16)get_byte_mmu030_state (m68k_areg (regs, srcreg)) << 8)) + get_iword_mmu030_state (2);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) -= areg_byteinc[dstreg];
	put_byte_mmu030_state (m68k_areg (regs, dstreg),((val >> 4) & 0xf0) | (val & 0xf));
}	m68k_incpci (4);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* OR.W Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_8150_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* OR.W Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_8158_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* OR.W Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_8160_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* OR.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_8168_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.W Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_8170_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* OR.W Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_8178_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* OR.W Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_8179_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* UNPK.L Dn,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_8180_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uae_u16 val = m68k_dreg (regs, srcreg);
	val = (((val << 4) & 0xf00) | (val & 0xf)) + get_iword_mmu030_state (2);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & 0xffff0000) | (val & 0xffff);
}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

#endif
/* UNPK.L -(An),-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_8188_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uae_u16 val;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) -= areg_byteinc[srcreg];
	val = (uae_u16)get_byte_mmu030_state (m68k_areg (regs, srcreg));
	val = (((val << 4) & 0xf00) | (val & 0xf)) + get_iword_mmu030_state (2);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) -= 2 * areg_byteinc[dstreg];
	put_byte_mmu030_state (m68k_areg (regs, dstreg) + areg_byteinc[dstreg], val);
	put_byte_mmu030_state (m68k_areg (regs, dstreg), val >> 8);
}	m68k_incpci (4);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* OR.L Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_8190_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* OR.L Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_8198_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* OR.L Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_81a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* OR.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_81a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* OR.L Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_81b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* OR.L Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_81b8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* OR.L Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_81b9_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* DIVS.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel14038;
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
}}}endlabel14038: ;
return 142 * CYCLE_UNIT / 2;
}

/* DIVS.W (An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel14039;
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
}}}}endlabel14039: ;
return 146 * CYCLE_UNIT / 2;
}

/* DIVS.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel14040;
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
}}}}endlabel14040: ;
	mmufixup[0].reg = -1;
return 146 * CYCLE_UNIT / 2;
}

/* DIVS.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (2);
		Exception (5);
		goto endlabel14041;
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
}}}}endlabel14041: ;
	mmufixup[0].reg = -1;
return 148 * CYCLE_UNIT / 2;
}

/* DIVS.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel14042;
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
}}}}endlabel14042: ;
return 150 * CYCLE_UNIT / 2;
}

/* DIVS.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (0);
		Exception (5);
		goto endlabel14043;
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
}}}}}endlabel14043: ;
return 150 * CYCLE_UNIT / 2;
}

/* DIVS.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81f8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel14044;
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
}}}}endlabel14044: ;
return 150 * CYCLE_UNIT / 2;
}

/* DIVS.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81f9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (6);
		Exception (5);
		goto endlabel14045;
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
}}}}endlabel14045: ;
return 154 * CYCLE_UNIT / 2;
}

/* DIVS.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81fa_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel14046;
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
}}}}endlabel14046: ;
return 150 * CYCLE_UNIT / 2;
}

/* DIVS.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81fb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (0);
		Exception (5);
		goto endlabel14047;
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
}}}}}endlabel14047: ;
return 150 * CYCLE_UNIT / 2;
}

/* DIVS.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_81fc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
		divbyzero_special (1, dst);
	m68k_incpci (4);
		Exception (5);
		goto endlabel14048;
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
}}}endlabel14048: ;
return 146 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9000_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_9010_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* SUB.B (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9018_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* SUB.B -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9020_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* SUB.B (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9028_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.B (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9030_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUB.B (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9038_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.B (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9039_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.B (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_903a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.B (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_903b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUB.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_903c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9040_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_9048_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_9050_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* SUB.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9058_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* SUB.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9060_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* SUB.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9068_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9070_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUB.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9078_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9079_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_907a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_907b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUB.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_907c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9080_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_9088_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_9090_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.L (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9098_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* SUB.L -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* SUB.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.L (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.L (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90b8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.L (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90b9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SUB.L (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90ba_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.L (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90bb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_90bc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* SUBA.W Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_90c0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_90c8_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_90d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* SUBA.W (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_90d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* SUBA.W -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_90e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* SUBA.W (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_90e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUBA.W (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_90f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUBA.W (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_90f8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUBA.W (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_90f9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* SUBA.W (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_90fa_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* SUBA.W (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_90fb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* SUBA.W #<data>.W,An */
uae_u32 REGPARAM2 CPUFUNC(op_90fc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* SUBX.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9100_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_9108_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	uae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	put_byte_mmu030_state (dsta, newv);
}}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9110_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_9118_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9120_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_9128_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_9130_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_9138_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_9139_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SUBX.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9140_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_9148_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	uae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	put_word_mmu030_state (dsta, newv);
}}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9150_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_9158_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9160_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_9168_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_9170_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_9178_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUB.W Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_9179_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SUBX.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_9180_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_9188_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	uae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	put_long_mmu030_state (dsta, newv);
}}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_9190_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_9198_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_91a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_91a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_91b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_91b8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* SUB.L Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_91b9_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* SUBA.L Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_91c0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_91c8_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_91d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* SUBA.L (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_91d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* SUBA.L -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_91e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* SUBA.L (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_91e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUBA.L (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_91f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUBA.L (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_91f8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUBA.L (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_91f9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* SUBA.L (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_91fa_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* SUBA.L (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_91fb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* SUBA.L #<data>.L,An */
uae_u32 REGPARAM2 CPUFUNC(op_91fc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b000_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_b010_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* CMP.B (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b018_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* CMP.B -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b020_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* CMP.B (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b028_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.B (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b030_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* CMP.B (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b038_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.B (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b039_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.B (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b03a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.B (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b03b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* CMP.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b03c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b040_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_b048_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_b050_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* CMP.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b058_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* CMP.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b060_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* CMP.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b068_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b070_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* CMP.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b078_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b079_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b07a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b07b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* CMP.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b07c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* CMP.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b080_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_b088_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_b090_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* CMP.L (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b098_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* CMP.L -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* CMP.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.L (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* CMP.L (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0b8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.L (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0b9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* CMP.L (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0ba_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMP.L (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0bb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* CMP.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b0bc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* CMPA.W Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_b0c0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_b0c8_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_b0d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* CMPA.W (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_b0d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* CMPA.W -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_b0e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* CMPA.W (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_b0e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMPA.W (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_b0f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* CMPA.W (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_b0f8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMPA.W (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_b0f9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* CMPA.W (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_b0fa_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* CMPA.W (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_b0fb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* CMPA.W #<data>.W,An */
uae_u32 REGPARAM2 CPUFUNC(op_b0fc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b100_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_b108_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_b110_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_b118_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_b120_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_b128_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_b130_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_b138_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* EOR.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_b139_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b140_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_b148_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_b150_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_b158_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_b160_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_b168_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_b170_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_b178_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* EOR.W Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_b179_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_b180_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_b188_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_b190_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_b198_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_b1a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_b1a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_b1b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_b1b8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* EOR.L Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_b1b9_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* CMPA.L Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_b1c0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_b1c8_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_b1d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* CMPA.L (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_b1d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* CMPA.L -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_b1e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* CMPA.L (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_b1e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMPA.L (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_b1f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* CMPA.L (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_b1f8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMPA.L (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_b1f9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* CMPA.L (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_b1fa_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* CMPA.L (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_b1fb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* CMPA.L #<data>.L,An */
uae_u32 REGPARAM2 CPUFUNC(op_b1fc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* AND.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c000_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_c010_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* AND.B (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c018_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* AND.B -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c020_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* AND.B (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c028_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.B (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c030_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* AND.B (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c038_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.B (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c039_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* AND.B (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c03a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.B (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c03b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* AND.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c03c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c040_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_c050_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* AND.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c058_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* AND.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c060_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* AND.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c068_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c070_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* AND.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c078_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c079_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* AND.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c07a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* AND.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c07b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}return 12 * CYCLE_UNIT / 2;
}

/* AND.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c07c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* AND.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c080_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_c090_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* AND.L (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c098_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* AND.L -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* AND.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.L (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.L (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0b8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.L (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0b9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* AND.L (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0ba_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.L (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0bb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0bc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* MULU.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0c0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_c0d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (2);
}}}}}return 62 * CYCLE_UNIT / 2;
}

/* MULU.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (2);
}}}}}	mmufixup[0].reg = -1;
return 62 * CYCLE_UNIT / 2;
}

/* MULU.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (2);
}}}}}	mmufixup[0].reg = -1;
return 64 * CYCLE_UNIT / 2;
}

/* MULU.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (4);
}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULU.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULU.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0f8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (4);
}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULU.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0f9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (6);
}}}}}return 70 * CYCLE_UNIT / 2;
}

/* MULU.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0fa_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (4);
}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULU.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0fb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULU.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c0fc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpci (4);
}}}}return 62 * CYCLE_UNIT / 2;
}

/* ABCD.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c100_32)(uae_u32 opcode)
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
	SET_NFLG (((uae_s8)(newv)) < 0);
	SET_VFLG ((tmp_newv & 0x80) == 0 && (newv & 0x80) != 0);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}	m68k_incpci (2);
return 4 * CYCLE_UNIT / 2;
}

/* ABCD.B -(An),-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c108_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
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
	SET_NFLG (((uae_s8)(newv)) < 0);
	SET_VFLG ((tmp_newv & 0x80) == 0 && (newv & 0x80) != 0);
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* AND.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c110_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* AND.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_c118_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* AND.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c120_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* AND.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_c128_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_c130_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_c138_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_c139_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	put_byte_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* EXG.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c140_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_c148_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_c150_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* AND.W Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_c158_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* AND.W Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c160_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* AND.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_c168_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.W Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_c170_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* AND.W Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_c178_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* AND.W Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_c179_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	put_word_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* EXG.L Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_c188_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_c190_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* AND.L Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_c198_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* AND.L Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_c1a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* AND.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_c1a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* AND.L Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_c1b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}}return 24 * CYCLE_UNIT / 2;
}

/* AND.L Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_c1b8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* AND.L Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_c1b9_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	put_long_mmu030_state (dsta, src);
}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* MULS.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1c0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_c1d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 62 * CYCLE_UNIT / 2;
}

/* MULS.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 62 * CYCLE_UNIT / 2;
}

/* MULS.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 64 * CYCLE_UNIT / 2;
}

/* MULS.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 66 * CYCLE_UNIT / 2;
}

/* MULS.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULS.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1f8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 66 * CYCLE_UNIT / 2;
}

/* MULS.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1f9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 70 * CYCLE_UNIT / 2;
}

/* MULS.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1fa_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 66 * CYCLE_UNIT / 2;
}

/* MULS.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1fb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}return 66 * CYCLE_UNIT / 2;
}

/* MULS.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_c1fc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(newv));
	m68k_dreg (regs, dstreg) = (newv);
}}}}	m68k_incpci (4);
return 62 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d000_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_d010_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* ADD.B (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d018_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* ADD.B -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d020_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* ADD.B (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d028_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.B (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d030_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADD.B (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d038_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.B (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d039_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.B (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d03a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.B (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d03b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s8 src = get_byte_mmu030_state (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADD.B #<data>.B,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d03c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = get_ibyte_mmu030_state (2);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d040_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_d048_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_d050_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* ADD.W (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d058_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* ADD.W -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d060_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* ADD.W (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d068_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.W (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d070_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADD.W (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d078_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.W (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d079_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.W (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d07a_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.W (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d07b_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADD.W #<data>.W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d07c_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d080_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_d088_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_d090_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.L (An)+,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d098_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ADD.L -(An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ADD.L (d16,An),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.L (d8,An,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.L (xxx).W,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0b8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.L (xxx).L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0b9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ADD.L (d16,PC),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0ba_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.L (d8,PC,Xn),Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0bb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.L #<data>.L,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d0bc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* ADDA.W Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_d0c0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_d0c8_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_d0d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 8 * CYCLE_UNIT / 2;
}

/* ADDA.W (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_d0d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 8 * CYCLE_UNIT / 2;
}

/* ADDA.W -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_d0e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 10 * CYCLE_UNIT / 2;
}

/* ADDA.W (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_d0e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADDA.W (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_d0f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADDA.W (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_d0f8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADDA.W (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_d0f9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 16 * CYCLE_UNIT / 2;
}

/* ADDA.W (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_d0fa_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 12 * CYCLE_UNIT / 2;
}

/* ADDA.W (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_d0fb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s16 src = get_word_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 12 * CYCLE_UNIT / 2;
}

/* ADDA.W #<data>.W,An */
uae_u32 REGPARAM2 CPUFUNC(op_d0fc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_iword_mmu030_state (2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (4);
return 8 * CYCLE_UNIT / 2;
}

/* ADDX.B Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d100_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_d108_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = get_byte_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	uae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	put_byte_mmu030_state (dsta, newv);
}}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d110_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_d118_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d120_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = get_byte_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_d128_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_d130_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_d138_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.B Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_d139_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s8 dst = get_byte_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	put_byte_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ADDX.W Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d140_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_d148_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
{	uae_s16 src = get_word_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	uae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	put_word_mmu030_state (dsta, newv);
}}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 16 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d150_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_d158_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 2;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d160_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
{	uae_s16 dst = get_word_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_d168_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_d170_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_d178_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADD.W Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_d179_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s16 dst = get_word_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	put_word_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ADDX.L Dn,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_d180_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_d188_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[1].reg = dstreg;
	mmufixup[1].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{	uae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	put_long_mmu030_state (dsta, newv);
}}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
	mmufixup[1].reg = -1;
return 28 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d190_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
return 20 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,(An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_d198_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += 4;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 20 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,-(An) */
uae_u32 REGPARAM2 CPUFUNC(op_d1a0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
{	uae_s32 dst = get_long_mmu030_state (dsta);
	mmufixup[0].reg = dstreg;
	mmufixup[0].value = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) = dsta;
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 22 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,(d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_d1a8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,(d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_d1b0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	m68k_incpci (2);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}}return 24 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,(xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_d1b8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (4);
return 24 * CYCLE_UNIT / 2;
}

/* ADD.L Dn,(xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_d1b9_32)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (2);
{	uae_s32 dst = get_long_mmu030_state (dsta);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	put_long_mmu030_state (dsta, newv);
}}}}}}	m68k_incpci (6);
return 28 * CYCLE_UNIT / 2;
}

/* ADDA.L Dn,An */
uae_u32 REGPARAM2 CPUFUNC(op_d1c0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_d1c8_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_d1d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ADDA.L (An)+,An */
uae_u32 REGPARAM2 CPUFUNC(op_d1d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ADDA.L -(An),An */
uae_u32 REGPARAM2 CPUFUNC(op_d1e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
{	uae_s32 src = get_long_mmu030_state (srca);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ADDA.L (d16,An),An */
uae_u32 REGPARAM2 CPUFUNC(op_d1e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADDA.L (d8,An,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_d1f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	m68k_incpci (2);
{	srca = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADDA.L (xxx).W,An */
uae_u32 REGPARAM2 CPUFUNC(op_d1f8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADDA.L (xxx).L,An */
uae_u32 REGPARAM2 CPUFUNC(op_d1f9_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_ilong_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ADDA.L (d16,PC),An */
uae_u32 REGPARAM2 CPUFUNC(op_d1fa_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ADDA.L (d8,PC,Xn),An */
uae_u32 REGPARAM2 CPUFUNC(op_d1fb_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	m68k_incpci (2);
{	tmppc = m68k_getpc ();
	srca = get_disp_ea_020_mmu030 (tmppc, 0);
{	uae_s32 src = get_long_mmu030_state (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ADDA.L #<data>.L,An */
uae_u32 REGPARAM2 CPUFUNC(op_d1fc_32)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_ilong_mmu030_state (2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpci (6);
return 12 * CYCLE_UNIT / 2;
}

/* ASRQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e000_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e008_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e010_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e018_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e020_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e028_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e030_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e038_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e040_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e048_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e050_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e058_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e060_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e068_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e070_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e078_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e080_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e088_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e090_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e098_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e0a0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e0a8_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e0b0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e0b8_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e0d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ASRW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e0d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu030_state (dataa);
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
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ASRW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e0e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_word_mmu030_state (dataa);
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
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ASRW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e0e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ASRW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e0f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ASRW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e0f8_32)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ASRW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e0f9_32)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ASLQ.B #<data>,Dn */
uae_u32 REGPARAM2 CPUFUNC(op_e100_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e108_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e110_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e118_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e120_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e128_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e130_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e138_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e140_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e148_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e150_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e158_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e160_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e168_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e170_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e178_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e180_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e188_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e190_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e198_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e1a0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e1a8_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e1b0_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e1b8_32)(uae_u32 opcode)
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
uae_u32 REGPARAM2 CPUFUNC(op_e1d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ASLW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e1d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu030_state (dataa);
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
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ASLW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e1e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_word_mmu030_state (dataa);
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
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ASLW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e1e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ASLW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e1f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	put_word_mmu030_state (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ASLW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e1f8_32)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ASLW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e1f9_32)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* LSRW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e2d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* LSRW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e2d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu030_state (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* LSRW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e2e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_word_mmu030_state (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* LSRW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e2e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* LSRW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e2f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* LSRW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e2f8_32)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* LSRW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e2f9_32)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* LSLW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e3d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* LSLW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e3d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu030_state (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* LSLW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e3e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_word_mmu030_state (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* LSLW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e3e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* LSLW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e3f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* LSLW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e3f8_32)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* LSLW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e3f9_32)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ROXRW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e4d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ROXRW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e4d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu030_state (dataa);
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
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ROXRW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e4e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_word_mmu030_state (dataa);
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
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ROXRW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e4e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ROXRW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e4f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ROXRW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e4f8_32)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ROXRW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e4f9_32)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ROXLW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e5d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ROXLW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e5d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu030_state (dataa);
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
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ROXLW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e5e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_word_mmu030_state (dataa);
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
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ROXLW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e5e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ROXLW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e5f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ROXLW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e5f8_32)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ROXLW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e5f9_32)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* RORW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e6d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* RORW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e6d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu030_state (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* RORW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e6e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_word_mmu030_state (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* RORW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e6e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* RORW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e6f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	put_word_mmu030_state (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* RORW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e6f8_32)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* RORW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e6f9_32)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* ROLW.W (An) */
uae_u32 REGPARAM2 CPUFUNC(op_e7d0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
return 12 * CYCLE_UNIT / 2;
}

/* ROLW.W (An)+ */
uae_u32 REGPARAM2 CPUFUNC(op_e7d8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
{	uae_s16 data = get_word_mmu030_state (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 2;
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 12 * CYCLE_UNIT / 2;
}

/* ROLW.W -(An) */
uae_u32 REGPARAM2 CPUFUNC(op_e7e0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
{	uae_s16 data = get_word_mmu030_state (dataa);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = dataa;
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (2);
	mmufixup[0].reg = -1;
return 14 * CYCLE_UNIT / 2;
}

/* ROLW.W (d16,An) */
uae_u32 REGPARAM2 CPUFUNC(op_e7e8_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ROLW.W (d8,An,Xn) */
uae_u32 REGPARAM2 CPUFUNC(op_e7f0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	m68k_incpci (2);
{	dataa = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	put_word_mmu030_state (dataa, val);
}}}}}return 16 * CYCLE_UNIT / 2;
}

/* ROLW.W (xxx).W */
uae_u32 REGPARAM2 CPUFUNC(op_e7f8_32)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_iword_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (4);
return 16 * CYCLE_UNIT / 2;
}

/* ROLW.W (xxx).L */
uae_u32 REGPARAM2 CPUFUNC(op_e7f9_32)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_ilong_mmu030_state (2);
{	uae_s16 data = get_word_mmu030_state (dataa);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	put_word_mmu030_state (dataa, val);
}}}}	m68k_incpci (6);
return 20 * CYCLE_UNIT / 2;
}

/* BFTST.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_e8c0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
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
uae_u32 REGPARAM2 CPUFUNC(op_e8d0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
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
uae_u32 REGPARAM2 CPUFUNC(op_e8e8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_e8f0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
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
uae_u32 REGPARAM2 CPUFUNC(op_e8f8_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_e8f9_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_e8fa_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_e8fb_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
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
uae_u32 REGPARAM2 CPUFUNC(op_e9c0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
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
uae_u32 REGPARAM2 CPUFUNC(op_e9d0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
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
uae_u32 REGPARAM2 CPUFUNC(op_e9e8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_e9f0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
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
uae_u32 REGPARAM2 CPUFUNC(op_e9f8_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_e9f9_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_e9fa_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_e9fb_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
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
uae_u32 REGPARAM2 CPUFUNC(op_eac0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
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
uae_u32 REGPARAM2 CPUFUNC(op_ead0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
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
uae_u32 REGPARAM2 CPUFUNC(op_eae8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_eaf0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
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
uae_u32 REGPARAM2 CPUFUNC(op_eaf8_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_eaf9_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_ebc0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
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
uae_u32 REGPARAM2 CPUFUNC(op_ebd0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
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
uae_u32 REGPARAM2 CPUFUNC(op_ebe8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_ebf0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
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
uae_u32 REGPARAM2 CPUFUNC(op_ebf8_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_ebf9_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_ebfa_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_ebfb_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
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
uae_u32 REGPARAM2 CPUFUNC(op_ecc0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
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
uae_u32 REGPARAM2 CPUFUNC(op_ecd0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
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
uae_u32 REGPARAM2 CPUFUNC(op_ece8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_ecf0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
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
uae_u32 REGPARAM2 CPUFUNC(op_ecf8_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_ecf9_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_edc0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
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
uae_u32 REGPARAM2 CPUFUNC(op_edd0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
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
uae_u32 REGPARAM2 CPUFUNC(op_ede8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_edf0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
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
uae_u32 REGPARAM2 CPUFUNC(op_edf8_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_edf9_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_edfa_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_edfb_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr tmppc;
	uaecptr dsta;
	m68k_incpci (4);
{	tmppc = m68k_getpc ();
	dsta = get_disp_ea_020_mmu030 (tmppc, 0);
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
uae_u32 REGPARAM2 CPUFUNC(op_eec0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
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
uae_u32 REGPARAM2 CPUFUNC(op_eed0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
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
uae_u32 REGPARAM2 CPUFUNC(op_eee8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_eef0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
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
uae_u32 REGPARAM2 CPUFUNC(op_eef8_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_eef9_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_efc0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
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
uae_u32 REGPARAM2 CPUFUNC(op_efd0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
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
uae_u32 REGPARAM2 CPUFUNC(op_efe8_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_eff0_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	m68k_incpci (4);
{	dsta = get_disp_ea_020_mmu030 (m68k_areg (regs, dstreg), 0);
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
uae_u32 REGPARAM2 CPUFUNC(op_eff8_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_iword_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_eff9_32)(uae_u32 opcode)
{
{{	uae_s16 extra = get_iword_mmu030_state (2);
{	uaecptr dsta;
	dsta = get_ilong_mmu030_state (4);
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
uae_u32 REGPARAM2 CPUFUNC(op_f000_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel14548; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	uae_u16 extraa = 0;
	mmu_op30 (pc, opcode, extra, extraa);
}}endlabel14548: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L An,#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f008_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel14549; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	uae_u16 extraa = 0;
	mmu_op30 (pc, opcode, extra, extraa);
}}endlabel14549: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L (An),#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f010_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel14550; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
{	uaecptr extraa;
	extraa = m68k_areg (regs, srcreg);
	mmu_op30 (pc, opcode, extra, extraa);
}}}endlabel14550: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L (An)+,#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f018_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel14551; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
{	uaecptr extraa;
	extraa = m68k_areg (regs, srcreg);
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) += 4;
	mmu_op30 (pc, opcode, extra, extraa);
}}}endlabel14551: ;
	mmufixup[0].reg = -1;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L -(An),#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f020_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel14552; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
{	uaecptr extraa;
	extraa = m68k_areg (regs, srcreg) - 4;
	mmufixup[0].reg = srcreg;
	mmufixup[0].value = m68k_areg (regs, srcreg);
	m68k_areg (regs, srcreg) = extraa;
	mmu_op30 (pc, opcode, extra, extraa);
}}}endlabel14552: ;
	mmufixup[0].reg = -1;
return 6 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L (d16,An),#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f028_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel14553; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
{	uaecptr extraa;
	extraa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_iword_mmu030_state (0);
	m68k_incpci (2);
	mmu_op30 (pc, opcode, extra, extraa);
}}}endlabel14553: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L (d8,An,Xn),#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f030_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel14554; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
{	uaecptr extraa;
{	extraa = get_disp_ea_020_mmu030 (m68k_areg (regs, srcreg), 0);
	mmu_op30 (pc, opcode, extra, extraa);
}}}}endlabel14554: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L (xxx).W,#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f038_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel14555; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
{	uaecptr extraa;
	extraa = (uae_s32)(uae_s16)get_iword_mmu030_state (0);
	m68k_incpci (2);
	mmu_op30 (pc, opcode, extra, extraa);
}}}endlabel14555: ;
return 8 * CYCLE_UNIT / 2;
}

#endif
/* MMUOP030.L (xxx).L,#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f039_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel14556; }
{	uaecptr pc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
{	uaecptr extraa;
	extraa = get_ilong_mmu030_state (0);
	m68k_incpci (4);
	mmu_op30 (pc, opcode, extra, extraa);
}}}endlabel14556: ;
return 12 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f200_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,An */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f208_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f210_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f218_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f220_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f228_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f230_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f238_32)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f239_32)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f23a_32)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,(d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f23b_32)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FPP.L #<data>.W,#<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f23c_32)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_arithmetic(opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f240_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FDBcc.L #<data>.W,Dn */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f248_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_dbcc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f250_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,(An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f258_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,-(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f260_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,(d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f268_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,(d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f270_32)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,(xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f278_32)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FScc.L #<data>.W,(xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f279_32)(uae_u32 opcode)
{
{
#ifdef FPUEMU
{	uae_s16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_scc (opcode, extra);
}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FTRAPcc.L #<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f27a_32)(uae_u32 opcode)
{
{
#ifdef FPUEMU
	uaecptr oldpc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu030_state (2);
{	uae_s16 dummy = get_iword_mmu030_state (4);
	m68k_incpci (6);
	fpuop_trapcc (opcode, oldpc, extra);
}
#endif
}return 12 * CYCLE_UNIT / 2;
}

#endif
/* FTRAPcc.L #<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f27b_32)(uae_u32 opcode)
{
{
#ifdef FPUEMU
	uaecptr oldpc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu030_state (2);
{	uae_s32 dummy;
	dummy = get_ilong_mmu030_state (4);
	m68k_incpci (8);
	fpuop_trapcc (opcode, oldpc, extra);
}
#endif
}return 16 * CYCLE_UNIT / 2;
}

#endif
/* FTRAPcc.L  */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f27c_32)(uae_u32 opcode)
{
{
#ifdef FPUEMU
	uaecptr oldpc = m68k_getpc ();
	uae_u16 extra = get_iword_mmu030_state (2);
	m68k_incpci (4);
	fpuop_trapcc (opcode, oldpc, extra);

#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FBccQ.L #<data>,#<data>.W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f280_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 63);
{
#ifdef FPUEMU
	m68k_incpci (2);
{	uaecptr pc = m68k_getpc ();
{	uae_s16 extra = get_iword_mmu030_state (0);
	m68k_incpci (2);
	fpuop_bcc (opcode, pc,extra);
}}
#endif
}return 8 * CYCLE_UNIT / 2;
}

#endif
/* FBccQ.L #<data>,#<data>.L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f2c0_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 63);
{
#ifdef FPUEMU
	m68k_incpci (2);
{	uaecptr pc = m68k_getpc ();
{	uae_s32 extra;
	extra = get_ilong_mmu030_state (0);
	m68k_incpci (4);
	fpuop_bcc (opcode, pc,extra);
}}
#endif
}return 12 * CYCLE_UNIT / 2;
}

#endif
/* FSAVE.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f310_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel14583; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_save (opcode);

#endif
}}endlabel14583: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FSAVE.L -(An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f320_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel14584; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_save (opcode);

#endif
}}endlabel14584: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FSAVE.L (d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f328_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel14585; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_save (opcode);

#endif
}}endlabel14585: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FSAVE.L (d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f330_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel14586; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_save (opcode);

#endif
}}endlabel14586: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FSAVE.L (xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f338_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel14587; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_save (opcode);

#endif
}}endlabel14587: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FSAVE.L (xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f339_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel14588; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_save (opcode);

#endif
}}endlabel14588: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f350_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel14589; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel14589: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (An)+ */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f358_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel14590; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel14590: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (d16,An) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f368_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel14591; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel14591: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (d8,An,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f370_32)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel14592; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel14592: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (xxx).W */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f378_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel14593; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel14593: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (xxx).L */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f379_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel14594; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel14594: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (d16,PC) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f37a_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel14595; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel14595: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
/* FRESTORE.L (d8,PC,Xn) */
#ifndef CPUEMU_68000_ONLY
uae_u32 REGPARAM2 CPUFUNC(op_f37b_32)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel14596; }
{
#ifdef FPUEMU
	m68k_incpci (2);
	fpuop_restore (opcode);

#endif
}}endlabel14596: ;
return 4 * CYCLE_UNIT / 2;
}

#endif
#endif

