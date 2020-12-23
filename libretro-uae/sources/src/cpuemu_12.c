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
void REGPARAM2 CPUFUNC(op_0000_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* OR.B #<data>.B,(An) */
void REGPARAM2 CPUFUNC(op_0010_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* OR.B #<data>.B,(An)+ */
void REGPARAM2 CPUFUNC(op_0018_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* OR.B #<data>.B,-(An) */
void REGPARAM2 CPUFUNC(op_0020_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* OR.B #<data>.B,(d16,An) */
void REGPARAM2 CPUFUNC(op_0028_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* OR.B #<data>.B,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_0030_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
{	uae_s8 dst = x_get_byte (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (6);
} /* 22 (4/1) */

/* OR.B #<data>.B,(xxx).W */
void REGPARAM2 CPUFUNC(op_0038_12)(uae_u32 opcode)
{
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* OR.B #<data>.B,(xxx).L */
void REGPARAM2 CPUFUNC(op_0039_12)(uae_u32 opcode)
{
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= get_word_ce000_prefetch (8);
{	uae_s8 dst = x_get_byte (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (8);
} /* 24 (5/1) */

/* ORSR.B #<data>.W */
void REGPARAM2 CPUFUNC(op_003c_12)(uae_u32 opcode)
{
{	MakeSR ();
{	uae_s16 src = get_word_ce000_prefetch (4);
	x_get_iword (6);
	src &= 0xFF;
	do_cycles_ce000 (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	regs.sr |= src;
	MakeFromSR ();
}}	m68k_incpc (4);
} /* 20 (3/0) */

/* OR.W #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_0040_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* OR.W #<data>.W,(An) */
void REGPARAM2 CPUFUNC(op_0050_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3745;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (4);
endlabel3745: ;
} /* 16 (3/1) */

/* OR.W #<data>.W,(An)+ */
void REGPARAM2 CPUFUNC(op_0058_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3746;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) += 2;
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (4);
endlabel3746: ;
} /* 16 (3/1) */

/* OR.W #<data>.W,-(An) */
void REGPARAM2 CPUFUNC(op_0060_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	do_cycles_ce000 (2);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3747;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (4);
endlabel3747: ;
} /* 18 (3/1) */

/* OR.W #<data>.W,(d16,An) */
void REGPARAM2 CPUFUNC(op_0068_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3748;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (6);
endlabel3748: ;
} /* 20 (4/1) */

/* OR.W #<data>.W,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_0070_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3749;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (6);
endlabel3749: ;
} /* 22 (4/1) */

/* OR.W #<data>.W,(xxx).W */
void REGPARAM2 CPUFUNC(op_0078_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3750;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (6);
endlabel3750: ;
} /* 20 (4/1) */

/* OR.W #<data>.W,(xxx).L */
void REGPARAM2 CPUFUNC(op_0079_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3751;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (8);
endlabel3751: ;
} /* 24 (5/1) */

/* ORSR.W #<data>.W */
void REGPARAM2 CPUFUNC(op_007c_12)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel3752; }
{	MakeSR ();
{	uae_s16 src = get_word_ce000_prefetch (4);
	x_get_iword (6);
	do_cycles_ce000 (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	regs.sr |= src;
	MakeFromSR ();
}}}	m68k_incpc (4);
endlabel3752: ;
} /* 20 (3/0) */

/* OR.L #<data>.L,Dn */
void REGPARAM2 CPUFUNC(op_0080_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (4);
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpc (6);
} /* 16 (3/0) */

/* OR.L #<data>.L,(An) */
void REGPARAM2 CPUFUNC(op_0090_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3754;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (6);
endlabel3754: ;
} /* 28 (5/2) */

/* OR.L #<data>.L,(An)+ */
void REGPARAM2 CPUFUNC(op_0098_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3755;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) += 4;
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (6);
endlabel3755: ;
} /* 28 (5/2) */

/* OR.L #<data>.L,-(An) */
void REGPARAM2 CPUFUNC(op_00a0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	do_cycles_ce000 (2);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3756;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (6);
endlabel3756: ;
} /* 30 (5/2) */

/* OR.L #<data>.L,(d16,An) */
void REGPARAM2 CPUFUNC(op_00a8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3757;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (8);
endlabel3757: ;
} /* 32 (6/2) */

/* OR.L #<data>.L,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_00b0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (8));
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3758;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (8);
endlabel3758: ;
} /* 34 (6/2) */

/* OR.L #<data>.L,(xxx).W */
void REGPARAM2 CPUFUNC(op_00b8_12)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3759;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (8);
endlabel3759: ;
} /* 32 (6/2) */

/* OR.L #<data>.L,(xxx).L */
void REGPARAM2 CPUFUNC(op_00b9_12)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (8) << 16;
	dsta |= get_word_ce000_prefetch (10);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3760;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (12);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (10);
endlabel3760: ;
} /* 36 (7/2) */

/* BTST.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_0100_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	src &= 31;
	do_cycles_ce000 (2);
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* MVPMR.W (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_0108_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr memp = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_u16 val = (x_get_byte (memp) << 8) + x_get_byte (memp + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}	m68k_incpc (4);
} /* 16 (4/0) */

/* BTST.B Dn,(An) */
void REGPARAM2 CPUFUNC(op_0110_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (4);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpc (2);
} /* 8 (2/0) */

/* BTST.B Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_0118_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	get_word_ce000_prefetch (4);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpc (2);
} /* 8 (2/0) */

/* BTST.B Dn,-(An) */
void REGPARAM2 CPUFUNC(op_0120_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	get_word_ce000_prefetch (4);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpc (2);
} /* 10 (2/0) */

/* BTST.B Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_0128_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* BTST.B Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_0130_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpc (4);
} /* 14 (3/0) */

/* BTST.B Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_0138_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* BTST.B Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_0139_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpc (6);
} /* 16 (4/0) */

/* BTST.B Dn,(d16,PC) */
void REGPARAM2 CPUFUNC(op_013a_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_getpc () + 2;
	dsta += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* BTST.B Dn,(d8,PC,Xn) */
void REGPARAM2 CPUFUNC(op_013b_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr tmppc;
	uaecptr dsta;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpc (4);
} /* 14 (3/0) */

/* BTST.B Dn,#<data>.B */
void REGPARAM2 CPUFUNC(op_013c_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = (uae_u8)get_word_ce000_prefetch (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* BCHG.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_0140_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	src &= 31;
	do_cycles_ce000 (2);
	if (src > 15) do_cycles_ce000 (2);
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* MVPMR.L (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_0148_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	uaecptr memp = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_u32 val = (x_get_byte (memp) << 24) + (x_get_byte (memp + 2) << 16)
              + (x_get_byte (memp + 4) << 8) + x_get_byte (memp + 6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (val);
}}	m68k_incpc (4);
} /* 24 (6/0) */

/* BCHG.B Dn,(An) */
void REGPARAM2 CPUFUNC(op_0150_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (4);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* BCHG.B Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_0158_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	get_word_ce000_prefetch (4);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* BCHG.B Dn,-(An) */
void REGPARAM2 CPUFUNC(op_0160_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	get_word_ce000_prefetch (4);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (2);
} /* 14 (2/1) */

/* BCHG.B Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_0168_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* BCHG.B Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_0170_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* BCHG.B Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_0178_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* BCHG.B Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_0179_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* BCHG.B Dn,(d16,PC) */
void REGPARAM2 CPUFUNC(op_017a_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_getpc () + 2;
	dsta += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* BCHG.B Dn,(d8,PC,Xn) */
void REGPARAM2 CPUFUNC(op_017b_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr tmppc;
	uaecptr dsta;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* BCLR.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_0180_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	src &= 31;
	do_cycles_ce000 (2);
	if (src > 15) do_cycles_ce000 (2);
	do_cycles_ce000 (2);
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpc (2);
} /* 8+ (1/0) */

/* MVPRM.W Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_0188_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	uaecptr memp = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	x_put_byte (memp, src >> 8);
	x_put_byte (memp + 2, src);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}	m68k_incpc (4);
} /* 16 (2/2) */

/* BCLR.B Dn,(An) */
void REGPARAM2 CPUFUNC(op_0190_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (4);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* BCLR.B Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_0198_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	get_word_ce000_prefetch (4);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* BCLR.B Dn,-(An) */
void REGPARAM2 CPUFUNC(op_01a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	get_word_ce000_prefetch (4);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (2);
} /* 14 (2/1) */

/* BCLR.B Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_01a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* BCLR.B Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_01b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* BCLR.B Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_01b8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* BCLR.B Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_01b9_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* BCLR.B Dn,(d16,PC) */
void REGPARAM2 CPUFUNC(op_01ba_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_getpc () + 2;
	dsta += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* BCLR.B Dn,(d8,PC,Xn) */
void REGPARAM2 CPUFUNC(op_01bb_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr tmppc;
	uaecptr dsta;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* BSET.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_01c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	src &= 31;
	do_cycles_ce000 (2);
	if (src > 15) do_cycles_ce000 (2);
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* MVPRM.L Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_01c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
	uaecptr memp = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	x_put_byte (memp, src >> 24);
	x_put_byte (memp + 2, src >> 16);
	x_put_byte (memp + 4, src >> 8);
	x_put_byte (memp + 6, src);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}	m68k_incpc (4);
} /* 24 (2/4) */

/* BSET.B Dn,(An) */
void REGPARAM2 CPUFUNC(op_01d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (4);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* BSET.B Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_01d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	get_word_ce000_prefetch (4);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* BSET.B Dn,-(An) */
void REGPARAM2 CPUFUNC(op_01e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	get_word_ce000_prefetch (4);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (2);
} /* 14 (2/1) */

/* BSET.B Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_01e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* BSET.B Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_01f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* BSET.B Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_01f8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* BSET.B Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_01f9_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* BSET.B Dn,(d16,PC) */
void REGPARAM2 CPUFUNC(op_01fa_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_getpc () + 2;
	dsta += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* BSET.B Dn,(d8,PC,Xn) */
void REGPARAM2 CPUFUNC(op_01fb_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr tmppc;
	uaecptr dsta;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* AND.B #<data>.B,Dn */
void REGPARAM2 CPUFUNC(op_0200_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* AND.B #<data>.B,(An) */
void REGPARAM2 CPUFUNC(op_0210_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* AND.B #<data>.B,(An)+ */
void REGPARAM2 CPUFUNC(op_0218_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* AND.B #<data>.B,-(An) */
void REGPARAM2 CPUFUNC(op_0220_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* AND.B #<data>.B,(d16,An) */
void REGPARAM2 CPUFUNC(op_0228_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* AND.B #<data>.B,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_0230_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
{	uae_s8 dst = x_get_byte (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (6);
} /* 22 (4/1) */

/* AND.B #<data>.B,(xxx).W */
void REGPARAM2 CPUFUNC(op_0238_12)(uae_u32 opcode)
{
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* AND.B #<data>.B,(xxx).L */
void REGPARAM2 CPUFUNC(op_0239_12)(uae_u32 opcode)
{
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= get_word_ce000_prefetch (8);
{	uae_s8 dst = x_get_byte (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (8);
} /* 24 (5/1) */

/* ANDSR.B #<data>.W */
void REGPARAM2 CPUFUNC(op_023c_12)(uae_u32 opcode)
{
{	MakeSR ();
{	uae_s16 src = get_word_ce000_prefetch (4);
	x_get_iword (6);
	src |= 0xFF00;
	do_cycles_ce000 (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	regs.sr &= src;
	MakeFromSR ();
}}	m68k_incpc (4);
} /* 20 (3/0) */

/* AND.W #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_0240_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* AND.W #<data>.W,(An) */
void REGPARAM2 CPUFUNC(op_0250_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3816;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (4);
endlabel3816: ;
} /* 16 (3/1) */

/* AND.W #<data>.W,(An)+ */
void REGPARAM2 CPUFUNC(op_0258_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3817;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) += 2;
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (4);
endlabel3817: ;
} /* 16 (3/1) */

/* AND.W #<data>.W,-(An) */
void REGPARAM2 CPUFUNC(op_0260_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	do_cycles_ce000 (2);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3818;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (4);
endlabel3818: ;
} /* 18 (3/1) */

/* AND.W #<data>.W,(d16,An) */
void REGPARAM2 CPUFUNC(op_0268_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3819;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (6);
endlabel3819: ;
} /* 20 (4/1) */

/* AND.W #<data>.W,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_0270_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3820;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (6);
endlabel3820: ;
} /* 22 (4/1) */

/* AND.W #<data>.W,(xxx).W */
void REGPARAM2 CPUFUNC(op_0278_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3821;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (6);
endlabel3821: ;
} /* 20 (4/1) */

/* AND.W #<data>.W,(xxx).L */
void REGPARAM2 CPUFUNC(op_0279_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3822;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (8);
endlabel3822: ;
} /* 24 (5/1) */

/* ANDSR.W #<data>.W */
void REGPARAM2 CPUFUNC(op_027c_12)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel3823; }
{	MakeSR ();
{	uae_s16 src = get_word_ce000_prefetch (4);
	x_get_iword (6);
	do_cycles_ce000 (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	regs.sr &= src;
	MakeFromSR ();
}}}	m68k_incpc (4);
endlabel3823: ;
} /* 20 (3/0) */

/* AND.L #<data>.L,Dn */
void REGPARAM2 CPUFUNC(op_0280_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (4);
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpc (6);
} /* 16 (3/0) */

/* AND.L #<data>.L,(An) */
void REGPARAM2 CPUFUNC(op_0290_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3825;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (6);
endlabel3825: ;
} /* 28 (5/2) */

/* AND.L #<data>.L,(An)+ */
void REGPARAM2 CPUFUNC(op_0298_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3826;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) += 4;
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (6);
endlabel3826: ;
} /* 28 (5/2) */

/* AND.L #<data>.L,-(An) */
void REGPARAM2 CPUFUNC(op_02a0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	do_cycles_ce000 (2);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3827;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (6);
endlabel3827: ;
} /* 30 (5/2) */

/* AND.L #<data>.L,(d16,An) */
void REGPARAM2 CPUFUNC(op_02a8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3828;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (8);
endlabel3828: ;
} /* 32 (6/2) */

/* AND.L #<data>.L,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_02b0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (8));
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3829;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (8);
endlabel3829: ;
} /* 34 (6/2) */

/* AND.L #<data>.L,(xxx).W */
void REGPARAM2 CPUFUNC(op_02b8_12)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3830;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (8);
endlabel3830: ;
} /* 32 (6/2) */

/* AND.L #<data>.L,(xxx).L */
void REGPARAM2 CPUFUNC(op_02b9_12)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (8) << 16;
	dsta |= get_word_ce000_prefetch (10);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3831;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (12);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (10);
endlabel3831: ;
} /* 36 (7/2) */

/* SUB.B #<data>.B,Dn */
void REGPARAM2 CPUFUNC(op_0400_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* SUB.B #<data>.B,(An) */
void REGPARAM2 CPUFUNC(op_0410_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* SUB.B #<data>.B,(An)+ */
void REGPARAM2 CPUFUNC(op_0418_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* SUB.B #<data>.B,-(An) */
void REGPARAM2 CPUFUNC(op_0420_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* SUB.B #<data>.B,(d16,An) */
void REGPARAM2 CPUFUNC(op_0428_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* SUB.B #<data>.B,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_0430_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (6);
} /* 22 (4/1) */

/* SUB.B #<data>.B,(xxx).W */
void REGPARAM2 CPUFUNC(op_0438_12)(uae_u32 opcode)
{
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* SUB.B #<data>.B,(xxx).L */
void REGPARAM2 CPUFUNC(op_0439_12)(uae_u32 opcode)
{
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= get_word_ce000_prefetch (8);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (8);
} /* 24 (5/1) */

/* SUB.W #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_0440_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* SUB.W #<data>.W,(An) */
void REGPARAM2 CPUFUNC(op_0450_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3841;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (4);
endlabel3841: ;
} /* 16 (3/1) */

/* SUB.W #<data>.W,(An)+ */
void REGPARAM2 CPUFUNC(op_0458_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3842;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (4);
endlabel3842: ;
} /* 16 (3/1) */

/* SUB.W #<data>.W,-(An) */
void REGPARAM2 CPUFUNC(op_0460_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	do_cycles_ce000 (2);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3843;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (4);
endlabel3843: ;
} /* 18 (3/1) */

/* SUB.W #<data>.W,(d16,An) */
void REGPARAM2 CPUFUNC(op_0468_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3844;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (6);
endlabel3844: ;
} /* 20 (4/1) */

/* SUB.W #<data>.W,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_0470_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3845;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (6);
endlabel3845: ;
} /* 22 (4/1) */

/* SUB.W #<data>.W,(xxx).W */
void REGPARAM2 CPUFUNC(op_0478_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3846;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (6);
endlabel3846: ;
} /* 20 (4/1) */

/* SUB.W #<data>.W,(xxx).L */
void REGPARAM2 CPUFUNC(op_0479_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3847;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (8);
endlabel3847: ;
} /* 24 (5/1) */

/* SUB.L #<data>.L,Dn */
void REGPARAM2 CPUFUNC(op_0480_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (4);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpc (6);
} /* 16 (3/0) */

/* SUB.L #<data>.L,(An) */
void REGPARAM2 CPUFUNC(op_0490_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3849;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (6);
endlabel3849: ;
} /* 28 (5/2) */

/* SUB.L #<data>.L,(An)+ */
void REGPARAM2 CPUFUNC(op_0498_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3850;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) += 4;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (6);
endlabel3850: ;
} /* 28 (5/2) */

/* SUB.L #<data>.L,-(An) */
void REGPARAM2 CPUFUNC(op_04a0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	do_cycles_ce000 (2);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3851;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (6);
endlabel3851: ;
} /* 30 (5/2) */

/* SUB.L #<data>.L,(d16,An) */
void REGPARAM2 CPUFUNC(op_04a8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3852;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (8);
endlabel3852: ;
} /* 32 (6/2) */

/* SUB.L #<data>.L,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_04b0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (8));
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3853;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (8);
endlabel3853: ;
} /* 34 (6/2) */

/* SUB.L #<data>.L,(xxx).W */
void REGPARAM2 CPUFUNC(op_04b8_12)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3854;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (8);
endlabel3854: ;
} /* 32 (6/2) */

/* SUB.L #<data>.L,(xxx).L */
void REGPARAM2 CPUFUNC(op_04b9_12)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (8) << 16;
	dsta |= get_word_ce000_prefetch (10);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3855;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (12);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (10);
endlabel3855: ;
} /* 36 (7/2) */

/* ADD.B #<data>.B,Dn */
void REGPARAM2 CPUFUNC(op_0600_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* ADD.B #<data>.B,(An) */
void REGPARAM2 CPUFUNC(op_0610_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* ADD.B #<data>.B,(An)+ */
void REGPARAM2 CPUFUNC(op_0618_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* ADD.B #<data>.B,-(An) */
void REGPARAM2 CPUFUNC(op_0620_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* ADD.B #<data>.B,(d16,An) */
void REGPARAM2 CPUFUNC(op_0628_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* ADD.B #<data>.B,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_0630_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (6);
} /* 22 (4/1) */

/* ADD.B #<data>.B,(xxx).W */
void REGPARAM2 CPUFUNC(op_0638_12)(uae_u32 opcode)
{
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* ADD.B #<data>.B,(xxx).L */
void REGPARAM2 CPUFUNC(op_0639_12)(uae_u32 opcode)
{
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= get_word_ce000_prefetch (8);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (8);
} /* 24 (5/1) */

/* ADD.W #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_0640_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* ADD.W #<data>.W,(An) */
void REGPARAM2 CPUFUNC(op_0650_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3865;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (4);
endlabel3865: ;
} /* 16 (3/1) */

/* ADD.W #<data>.W,(An)+ */
void REGPARAM2 CPUFUNC(op_0658_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3866;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (4);
endlabel3866: ;
} /* 16 (3/1) */

/* ADD.W #<data>.W,-(An) */
void REGPARAM2 CPUFUNC(op_0660_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	do_cycles_ce000 (2);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3867;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (4);
endlabel3867: ;
} /* 18 (3/1) */

/* ADD.W #<data>.W,(d16,An) */
void REGPARAM2 CPUFUNC(op_0668_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3868;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (6);
endlabel3868: ;
} /* 20 (4/1) */

/* ADD.W #<data>.W,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_0670_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3869;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (6);
endlabel3869: ;
} /* 22 (4/1) */

/* ADD.W #<data>.W,(xxx).W */
void REGPARAM2 CPUFUNC(op_0678_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3870;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (6);
endlabel3870: ;
} /* 20 (4/1) */

/* ADD.W #<data>.W,(xxx).L */
void REGPARAM2 CPUFUNC(op_0679_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3871;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (8);
endlabel3871: ;
} /* 24 (5/1) */

/* ADD.L #<data>.L,Dn */
void REGPARAM2 CPUFUNC(op_0680_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (4);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpc (6);
} /* 16 (3/0) */

/* ADD.L #<data>.L,(An) */
void REGPARAM2 CPUFUNC(op_0690_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3873;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (6);
endlabel3873: ;
} /* 28 (5/2) */

/* ADD.L #<data>.L,(An)+ */
void REGPARAM2 CPUFUNC(op_0698_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3874;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) += 4;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (6);
endlabel3874: ;
} /* 28 (5/2) */

/* ADD.L #<data>.L,-(An) */
void REGPARAM2 CPUFUNC(op_06a0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	do_cycles_ce000 (2);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3875;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (6);
endlabel3875: ;
} /* 30 (5/2) */

/* ADD.L #<data>.L,(d16,An) */
void REGPARAM2 CPUFUNC(op_06a8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3876;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (8);
endlabel3876: ;
} /* 32 (6/2) */

/* ADD.L #<data>.L,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_06b0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (8));
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3877;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (8);
endlabel3877: ;
} /* 34 (6/2) */

/* ADD.L #<data>.L,(xxx).W */
void REGPARAM2 CPUFUNC(op_06b8_12)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3878;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (8);
endlabel3878: ;
} /* 32 (6/2) */

/* ADD.L #<data>.L,(xxx).L */
void REGPARAM2 CPUFUNC(op_06b9_12)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (8) << 16;
	dsta |= get_word_ce000_prefetch (10);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3879;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (12);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (10);
endlabel3879: ;
} /* 36 (7/2) */

/* BTST.L #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_0800_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	src &= 31;
	do_cycles_ce000 (2);
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}	m68k_incpc (4);
} /* 10 (2/0) */

/* BTST.B #<data>.W,(An) */
void REGPARAM2 CPUFUNC(op_0810_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* BTST.B #<data>.W,(An)+ */
void REGPARAM2 CPUFUNC(op_0818_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* BTST.B #<data>.W,-(An) */
void REGPARAM2 CPUFUNC(op_0820_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpc (4);
} /* 14 (3/0) */

/* BTST.B #<data>.W,(d16,An) */
void REGPARAM2 CPUFUNC(op_0828_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpc (6);
} /* 16 (4/0) */

/* BTST.B #<data>.W,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_0830_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpc (6);
} /* 18 (4/0) */

/* BTST.B #<data>.W,(xxx).W */
void REGPARAM2 CPUFUNC(op_0838_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpc (6);
} /* 16 (4/0) */

/* BTST.B #<data>.W,(xxx).L */
void REGPARAM2 CPUFUNC(op_0839_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= get_word_ce000_prefetch (8);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (10);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpc (8);
} /* 20 (5/0) */

/* BTST.B #<data>.W,(d16,PC) */
void REGPARAM2 CPUFUNC(op_083a_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpc (6);
} /* 16 (4/0) */

/* BTST.B #<data>.W,(d8,PC,Xn) */
void REGPARAM2 CPUFUNC(op_083b_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr tmppc;
	uaecptr dsta;
	tmppc = m68k_getpc () + 4;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (6));
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}}	m68k_incpc (6);
} /* 18 (4/0) */

/* BTST.B #<data>.W,#<data>.B */
void REGPARAM2 CPUFUNC(op_083c_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s8 dst = (uae_u8)get_word_ce000_prefetch (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
}}}	m68k_incpc (6);
} /* 12 (3/0) */

/* BCHG.L #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_0840_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	src &= 31;
	do_cycles_ce000 (2);
	if (src > 15) do_cycles_ce000 (2);
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpc (4);
} /* 10+ (2/0) */

/* BCHG.B #<data>.W,(An) */
void REGPARAM2 CPUFUNC(op_0850_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* BCHG.B #<data>.W,(An)+ */
void REGPARAM2 CPUFUNC(op_0858_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	get_word_ce000_prefetch (6);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* BCHG.B #<data>.W,-(An) */
void REGPARAM2 CPUFUNC(op_0860_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	get_word_ce000_prefetch (6);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* BCHG.B #<data>.W,(d16,An) */
void REGPARAM2 CPUFUNC(op_0868_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* BCHG.B #<data>.W,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_0870_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (6);
} /* 22 (4/1) */

/* BCHG.B #<data>.W,(xxx).W */
void REGPARAM2 CPUFUNC(op_0878_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* BCHG.B #<data>.W,(xxx).L */
void REGPARAM2 CPUFUNC(op_0879_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= get_word_ce000_prefetch (8);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (10);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (8);
} /* 24 (5/1) */

/* BCHG.B #<data>.W,(d16,PC) */
void REGPARAM2 CPUFUNC(op_087a_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* BCHG.B #<data>.W,(d8,PC,Xn) */
void REGPARAM2 CPUFUNC(op_087b_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr tmppc;
	uaecptr dsta;
	tmppc = m68k_getpc () + 4;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (6));
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	dst ^= (1 << src);
	SET_ZFLG (((uae_u32)dst & (1 << src)) >> src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (6);
} /* 22 (4/1) */

/* BCLR.L #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_0880_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	src &= 31;
	do_cycles_ce000 (2);
	if (src > 15) do_cycles_ce000 (2);
	do_cycles_ce000 (2);
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpc (4);
} /* 12+ (2/0) */

/* BCLR.B #<data>.W,(An) */
void REGPARAM2 CPUFUNC(op_0890_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* BCLR.B #<data>.W,(An)+ */
void REGPARAM2 CPUFUNC(op_0898_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* BCLR.B #<data>.W,-(An) */
void REGPARAM2 CPUFUNC(op_08a0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* BCLR.B #<data>.W,(d16,An) */
void REGPARAM2 CPUFUNC(op_08a8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* BCLR.B #<data>.W,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_08b0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (6);
} /* 22 (4/1) */

/* BCLR.B #<data>.W,(xxx).W */
void REGPARAM2 CPUFUNC(op_08b8_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* BCLR.B #<data>.W,(xxx).L */
void REGPARAM2 CPUFUNC(op_08b9_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= get_word_ce000_prefetch (8);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (10);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (8);
} /* 24 (5/1) */

/* BCLR.B #<data>.W,(d16,PC) */
void REGPARAM2 CPUFUNC(op_08ba_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* BCLR.B #<data>.W,(d8,PC,Xn) */
void REGPARAM2 CPUFUNC(op_08bb_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr tmppc;
	uaecptr dsta;
	tmppc = m68k_getpc () + 4;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (6));
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst &= ~(1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (6);
} /* 22 (4/1) */

/* BSET.L #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_08c0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	src &= 31;
	do_cycles_ce000 (2);
	if (src > 15) do_cycles_ce000 (2);
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	m68k_dreg (regs, dstreg) = (dst);
}}}	m68k_incpc (4);
} /* 10+ (2/0) */

/* BSET.B #<data>.W,(An) */
void REGPARAM2 CPUFUNC(op_08d0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* BSET.B #<data>.W,(An)+ */
void REGPARAM2 CPUFUNC(op_08d8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* BSET.B #<data>.W,-(An) */
void REGPARAM2 CPUFUNC(op_08e0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	get_word_ce000_prefetch (6);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* BSET.B #<data>.W,(d16,An) */
void REGPARAM2 CPUFUNC(op_08e8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* BSET.B #<data>.W,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_08f0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (6);
} /* 22 (4/1) */

/* BSET.B #<data>.W,(xxx).W */
void REGPARAM2 CPUFUNC(op_08f8_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* BSET.B #<data>.W,(xxx).L */
void REGPARAM2 CPUFUNC(op_08f9_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= get_word_ce000_prefetch (8);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (10);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (8);
} /* 24 (5/1) */

/* BSET.B #<data>.W,(d16,PC) */
void REGPARAM2 CPUFUNC(op_08fa_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_getpc () + 4;
	dsta += (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* BSET.B #<data>.W,(d8,PC,Xn) */
void REGPARAM2 CPUFUNC(op_08fb_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr tmppc;
	uaecptr dsta;
	tmppc = m68k_getpc () + 4;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (6));
	regs.ir = regs.irc;
	regs.irc = 0;
	ipl_fetch ();
{	uae_s8 dst = x_get_byte (dsta);
	get_word_ce000_prefetch (8);
	src &= 7;
	SET_ZFLG (1 ^ ((dst >> src) & 1));
	dst |= (1 << src);
	x_put_byte (dsta, dst);
}}}}	m68k_incpc (6);
} /* 22 (4/1) */

/* EOR.B #<data>.B,Dn */
void REGPARAM2 CPUFUNC(op_0a00_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* EOR.B #<data>.B,(An) */
void REGPARAM2 CPUFUNC(op_0a10_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* EOR.B #<data>.B,(An)+ */
void REGPARAM2 CPUFUNC(op_0a18_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* EOR.B #<data>.B,-(An) */
void REGPARAM2 CPUFUNC(op_0a20_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* EOR.B #<data>.B,(d16,An) */
void REGPARAM2 CPUFUNC(op_0a28_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* EOR.B #<data>.B,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_0a30_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
{	uae_s8 dst = x_get_byte (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (6);
} /* 22 (4/1) */

/* EOR.B #<data>.B,(xxx).W */
void REGPARAM2 CPUFUNC(op_0a38_12)(uae_u32 opcode)
{
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* EOR.B #<data>.B,(xxx).L */
void REGPARAM2 CPUFUNC(op_0a39_12)(uae_u32 opcode)
{
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= get_word_ce000_prefetch (8);
{	uae_s8 dst = x_get_byte (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (8);
} /* 24 (5/1) */

/* EORSR.B #<data>.W */
void REGPARAM2 CPUFUNC(op_0a3c_12)(uae_u32 opcode)
{
{	MakeSR ();
{	uae_s16 src = get_word_ce000_prefetch (4);
	x_get_iword (6);
	src &= 0xFF;
	do_cycles_ce000 (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	regs.sr ^= src;
	MakeFromSR ();
}}	m68k_incpc (4);
} /* 20 (3/0) */

/* EOR.W #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_0a40_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* EOR.W #<data>.W,(An) */
void REGPARAM2 CPUFUNC(op_0a50_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3931;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (4);
endlabel3931: ;
} /* 16 (3/1) */

/* EOR.W #<data>.W,(An)+ */
void REGPARAM2 CPUFUNC(op_0a58_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3932;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) += 2;
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (4);
endlabel3932: ;
} /* 16 (3/1) */

/* EOR.W #<data>.W,-(An) */
void REGPARAM2 CPUFUNC(op_0a60_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	do_cycles_ce000 (2);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3933;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (4);
endlabel3933: ;
} /* 18 (3/1) */

/* EOR.W #<data>.W,(d16,An) */
void REGPARAM2 CPUFUNC(op_0a68_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3934;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (6);
endlabel3934: ;
} /* 20 (4/1) */

/* EOR.W #<data>.W,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_0a70_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3935;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (6);
endlabel3935: ;
} /* 22 (4/1) */

/* EOR.W #<data>.W,(xxx).W */
void REGPARAM2 CPUFUNC(op_0a78_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3936;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (6);
endlabel3936: ;
} /* 20 (4/1) */

/* EOR.W #<data>.W,(xxx).L */
void REGPARAM2 CPUFUNC(op_0a79_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3937;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (8);
endlabel3937: ;
} /* 24 (5/1) */

/* EORSR.W #<data>.W */
void REGPARAM2 CPUFUNC(op_0a7c_12)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel3938; }
{	MakeSR ();
{	uae_s16 src = get_word_ce000_prefetch (4);
	x_get_iword (6);
	do_cycles_ce000 (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	regs.sr ^= src;
	MakeFromSR ();
}}}	m68k_incpc (4);
endlabel3938: ;
} /* 20 (3/0) */

/* EOR.L #<data>.L,Dn */
void REGPARAM2 CPUFUNC(op_0a80_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (4);
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpc (6);
} /* 16 (3/0) */

#endif

#ifdef PART_2
/* EOR.L #<data>.L,(An) */
void REGPARAM2 CPUFUNC(op_0a90_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3940;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (6);
endlabel3940: ;
} /* 28 (5/2) */

/* EOR.L #<data>.L,(An)+ */
void REGPARAM2 CPUFUNC(op_0a98_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3941;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) += 4;
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (6);
endlabel3941: ;
} /* 28 (5/2) */

/* EOR.L #<data>.L,-(An) */
void REGPARAM2 CPUFUNC(op_0aa0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	do_cycles_ce000 (2);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3942;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (6);
endlabel3942: ;
} /* 30 (5/2) */

/* EOR.L #<data>.L,(d16,An) */
void REGPARAM2 CPUFUNC(op_0aa8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3943;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (8);
endlabel3943: ;
} /* 32 (6/2) */

/* EOR.L #<data>.L,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_0ab0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (8));
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3944;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (8);
endlabel3944: ;
} /* 34 (6/2) */

/* EOR.L #<data>.L,(xxx).W */
void REGPARAM2 CPUFUNC(op_0ab8_12)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3945;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (8);
endlabel3945: ;
} /* 32 (6/2) */

/* EOR.L #<data>.L,(xxx).L */
void REGPARAM2 CPUFUNC(op_0ab9_12)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (8) << 16;
	dsta |= get_word_ce000_prefetch (10);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3946;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (12);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (10);
endlabel3946: ;
} /* 36 (7/2) */

/* CMP.B #<data>.B,Dn */
void REGPARAM2 CPUFUNC(op_0c00_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* CMP.B #<data>.B,(An) */
void REGPARAM2 CPUFUNC(op_0c10_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* CMP.B #<data>.B,(An)+ */
void REGPARAM2 CPUFUNC(op_0c18_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* CMP.B #<data>.B,-(An) */
void REGPARAM2 CPUFUNC(op_0c20_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpc (4);
} /* 14 (3/0) */

/* CMP.B #<data>.B,(d16,An) */
void REGPARAM2 CPUFUNC(op_0c28_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpc (6);
} /* 16 (4/0) */

/* CMP.B #<data>.B,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_0c30_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpc (6);
} /* 18 (4/0) */

/* CMP.B #<data>.B,(xxx).W */
void REGPARAM2 CPUFUNC(op_0c38_12)(uae_u32 opcode)
{
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpc (6);
} /* 16 (4/0) */

/* CMP.B #<data>.B,(xxx).L */
void REGPARAM2 CPUFUNC(op_0c39_12)(uae_u32 opcode)
{
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= get_word_ce000_prefetch (8);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpc (8);
} /* 20 (5/0) */

/* CMP.W #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_0c40_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* CMP.W #<data>.W,(An) */
void REGPARAM2 CPUFUNC(op_0c50_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3956;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpc (4);
endlabel3956: ;
} /* 12 (3/0) */

/* CMP.W #<data>.W,(An)+ */
void REGPARAM2 CPUFUNC(op_0c58_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3957;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpc (4);
endlabel3957: ;
} /* 12 (3/0) */

/* CMP.W #<data>.W,-(An) */
void REGPARAM2 CPUFUNC(op_0c60_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	do_cycles_ce000 (2);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3958;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpc (4);
endlabel3958: ;
} /* 14 (3/0) */

/* CMP.W #<data>.W,(d16,An) */
void REGPARAM2 CPUFUNC(op_0c68_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3959;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpc (6);
endlabel3959: ;
} /* 16 (4/0) */

/* CMP.W #<data>.W,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_0c70_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3960;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpc (6);
endlabel3960: ;
} /* 18 (4/0) */

/* CMP.W #<data>.W,(xxx).W */
void REGPARAM2 CPUFUNC(op_0c78_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3961;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpc (6);
endlabel3961: ;
} /* 16 (4/0) */

/* CMP.W #<data>.W,(xxx).L */
void REGPARAM2 CPUFUNC(op_0c79_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel3962;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpc (8);
endlabel3962: ;
} /* 20 (5/0) */

/* CMP.L #<data>.L,Dn */
void REGPARAM2 CPUFUNC(op_0c80_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpc (6);
} /* 14 (3/0) */

/* CMP.L #<data>.L,(An) */
void REGPARAM2 CPUFUNC(op_0c90_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3964;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (6);
endlabel3964: ;
} /* 20 (5/0) */

/* CMP.L #<data>.L,(An)+ */
void REGPARAM2 CPUFUNC(op_0c98_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3965;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) += 4;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (6);
endlabel3965: ;
} /* 20 (5/0) */

/* CMP.L #<data>.L,-(An) */
void REGPARAM2 CPUFUNC(op_0ca0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	do_cycles_ce000 (2);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3966;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (6);
endlabel3966: ;
} /* 22 (5/0) */

/* CMP.L #<data>.L,(d16,An) */
void REGPARAM2 CPUFUNC(op_0ca8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3967;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (8);
endlabel3967: ;
} /* 24 (6/0) */

/* CMP.L #<data>.L,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_0cb0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (8));
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3968;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (8);
endlabel3968: ;
} /* 26 (6/0) */

/* CMP.L #<data>.L,(xxx).W */
void REGPARAM2 CPUFUNC(op_0cb8_12)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3969;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (8);
endlabel3969: ;
} /* 24 (6/0) */

/* CMP.L #<data>.L,(xxx).L */
void REGPARAM2 CPUFUNC(op_0cb9_12)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (8) << 16;
	dsta |= get_word_ce000_prefetch (10);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel3970;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (12);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (10);
endlabel3970: ;
} /* 28 (7/0) */

/* MOVE.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_1000_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 4 (1/0) */

/* MOVE.B (An),Dn */
void REGPARAM2 CPUFUNC(op_1010_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 8 (2/0) */

/* MOVE.B (An)+,Dn */
void REGPARAM2 CPUFUNC(op_1018_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 8 (2/0) */

/* MOVE.B -(An),Dn */
void REGPARAM2 CPUFUNC(op_1020_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 10 (2/0) */

/* MOVE.B (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_1028_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 12 (3/0) */

/* MOVE.B (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_1030_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 14 (3/0) */

/* MOVE.B (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_1038_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 12 (3/0) */

/* MOVE.B (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_1039_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 16 (4/0) */

/* MOVE.B (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_103a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 12 (3/0) */

/* MOVE.B (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_103b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 14 (3/0) */

/* MOVE.B #<data>.B,Dn */
void REGPARAM2 CPUFUNC(op_103c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	optflag_testb ((uae_s8)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 8 (2/0) */

/* MOVE.B Dn,(An) */
void REGPARAM2 CPUFUNC(op_1080_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 8 (1/1) */

/* MOVE.B (An),(An) */
void REGPARAM2 CPUFUNC(op_1090_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 12 (2/1) */

/* MOVE.B (An)+,(An) */
void REGPARAM2 CPUFUNC(op_1098_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 12 (2/1) */

/* MOVE.B -(An),(An) */
void REGPARAM2 CPUFUNC(op_10a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 14 (2/1) */

/* MOVE.B (d16,An),(An) */
void REGPARAM2 CPUFUNC(op_10a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 16 (3/1) */

/* MOVE.B (d8,An,Xn),(An) */
void REGPARAM2 CPUFUNC(op_10b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 18 (3/1) */

/* MOVE.B (xxx).W,(An) */
void REGPARAM2 CPUFUNC(op_10b8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 16 (3/1) */

/* MOVE.B (xxx).L,(An) */
void REGPARAM2 CPUFUNC(op_10b9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 20 (4/1) */

/* MOVE.B (d16,PC),(An) */
void REGPARAM2 CPUFUNC(op_10ba_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 16 (3/1) */

/* MOVE.B (d8,PC,Xn),(An) */
void REGPARAM2 CPUFUNC(op_10bb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 18 (3/1) */

/* MOVE.B #<data>.B,(An) */
void REGPARAM2 CPUFUNC(op_10bc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 12 (2/1) */

/* MOVE.B Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_10c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 8 (1/1) */

/* MOVE.B (An),(An)+ */
void REGPARAM2 CPUFUNC(op_10d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 12 (2/1) */

/* MOVE.B (An)+,(An)+ */
void REGPARAM2 CPUFUNC(op_10d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 12 (2/1) */

/* MOVE.B -(An),(An)+ */
void REGPARAM2 CPUFUNC(op_10e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 14 (2/1) */

/* MOVE.B (d16,An),(An)+ */
void REGPARAM2 CPUFUNC(op_10e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 16 (3/1) */

/* MOVE.B (d8,An,Xn),(An)+ */
void REGPARAM2 CPUFUNC(op_10f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 18 (3/1) */

/* MOVE.B (xxx).W,(An)+ */
void REGPARAM2 CPUFUNC(op_10f8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 16 (3/1) */

/* MOVE.B (xxx).L,(An)+ */
void REGPARAM2 CPUFUNC(op_10f9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 20 (4/1) */

/* MOVE.B (d16,PC),(An)+ */
void REGPARAM2 CPUFUNC(op_10fa_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 16 (3/1) */

/* MOVE.B (d8,PC,Xn),(An)+ */
void REGPARAM2 CPUFUNC(op_10fb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 18 (3/1) */

/* MOVE.B #<data>.B,(An)+ */
void REGPARAM2 CPUFUNC(op_10fc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 12 (2/1) */

/* MOVE.B Dn,-(An) */
void REGPARAM2 CPUFUNC(op_1100_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (2);
}}}} /* 8 (1/1) */

/* MOVE.B (An),-(An) */
void REGPARAM2 CPUFUNC(op_1110_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (2);
}}}}} /* 12 (2/1) */

/* MOVE.B (An)+,-(An) */
void REGPARAM2 CPUFUNC(op_1118_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (2);
}}}}} /* 12 (2/1) */

/* MOVE.B -(An),-(An) */
void REGPARAM2 CPUFUNC(op_1120_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (2);
}}}}} /* 14 (2/1) */

/* MOVE.B (d16,An),-(An) */
void REGPARAM2 CPUFUNC(op_1128_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
}}}}} /* 16 (3/1) */

/* MOVE.B (d8,An,Xn),-(An) */
void REGPARAM2 CPUFUNC(op_1130_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
}}}}} /* 18 (3/1) */

/* MOVE.B (xxx).W,-(An) */
void REGPARAM2 CPUFUNC(op_1138_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
}}}}} /* 16 (3/1) */

/* MOVE.B (xxx).L,-(An) */
void REGPARAM2 CPUFUNC(op_1139_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
}}}}} /* 20 (4/1) */

/* MOVE.B (d16,PC),-(An) */
void REGPARAM2 CPUFUNC(op_113a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
}}}}} /* 16 (3/1) */

/* MOVE.B (d8,PC,Xn),-(An) */
void REGPARAM2 CPUFUNC(op_113b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
}}}}} /* 18 (3/1) */

/* MOVE.B #<data>.B,-(An) */
void REGPARAM2 CPUFUNC(op_113c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
}}}} /* 12 (2/1) */

/* MOVE.B Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_1140_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 12 (2/1) */

/* MOVE.B (An),(d16,An) */
void REGPARAM2 CPUFUNC(op_1150_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 16 (3/1) */

/* MOVE.B (An)+,(d16,An) */
void REGPARAM2 CPUFUNC(op_1158_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 16 (3/1) */

/* MOVE.B -(An),(d16,An) */
void REGPARAM2 CPUFUNC(op_1160_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 18 (3/1) */

/* MOVE.B (d16,An),(d16,An) */
void REGPARAM2 CPUFUNC(op_1168_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 20 (4/1) */

/* MOVE.B (d8,An,Xn),(d16,An) */
void REGPARAM2 CPUFUNC(op_1170_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 22 (4/1) */

/* MOVE.B (xxx).W,(d16,An) */
void REGPARAM2 CPUFUNC(op_1178_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 20 (4/1) */

/* MOVE.B (xxx).L,(d16,An) */
void REGPARAM2 CPUFUNC(op_1179_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 24 (5/1) */

/* MOVE.B (d16,PC),(d16,An) */
void REGPARAM2 CPUFUNC(op_117a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 20 (4/1) */

/* MOVE.B (d8,PC,Xn),(d16,An) */
void REGPARAM2 CPUFUNC(op_117b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 22 (4/1) */

/* MOVE.B #<data>.B,(d16,An) */
void REGPARAM2 CPUFUNC(op_117c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 16 (3/1) */

/* MOVE.B Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_1180_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 14 (2/1) */

/* MOVE.B (An),(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_1190_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 18 (3/1) */

/* MOVE.B (An)+,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_1198_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 18 (3/1) */

/* MOVE.B -(An),(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_11a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 20 (3/1) */

/* MOVE.B (d16,An),(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_11a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 22 (4/1) */

/* MOVE.B (d8,An,Xn),(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_11b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 24 (4/1) */

/* MOVE.B (xxx).W,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_11b8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 22 (4/1) */

/* MOVE.B (xxx).L,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_11b9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (8));
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 26 (5/1) */

/* MOVE.B (d16,PC),(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_11ba_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 22 (4/1) */

/* MOVE.B (d8,PC,Xn),(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_11bb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 24 (4/1) */

/* MOVE.B #<data>.B,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_11bc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 18 (3/1) */

/* MOVE.B Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_11c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 12 (2/1) */

/* MOVE.B (An),(xxx).W */
void REGPARAM2 CPUFUNC(op_11d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 16 (3/1) */

/* MOVE.B (An)+,(xxx).W */
void REGPARAM2 CPUFUNC(op_11d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 16 (3/1) */

/* MOVE.B -(An),(xxx).W */
void REGPARAM2 CPUFUNC(op_11e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 18 (3/1) */

/* MOVE.B (d16,An),(xxx).W */
void REGPARAM2 CPUFUNC(op_11e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 20 (4/1) */

/* MOVE.B (d8,An,Xn),(xxx).W */
void REGPARAM2 CPUFUNC(op_11f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 22 (4/1) */

/* MOVE.B (xxx).W,(xxx).W */
void REGPARAM2 CPUFUNC(op_11f8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 20 (4/1) */

/* MOVE.B (xxx).L,(xxx).W */
void REGPARAM2 CPUFUNC(op_11f9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 24 (5/1) */

/* MOVE.B (d16,PC),(xxx).W */
void REGPARAM2 CPUFUNC(op_11fa_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 20 (4/1) */

/* MOVE.B (d8,PC,Xn),(xxx).W */
void REGPARAM2 CPUFUNC(op_11fb_12)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 22 (4/1) */

/* MOVE.B #<data>.B,(xxx).W */
void REGPARAM2 CPUFUNC(op_11fc_12)(uae_u32 opcode)
{
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 16 (3/1) */

/* MOVE.B Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_13c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 16 (3/1) */

/* MOVE.B (An),(xxx).L */
void REGPARAM2 CPUFUNC(op_13d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= regs.irc;
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 20 (4/1) */

/* MOVE.B (An)+,(xxx).L */
void REGPARAM2 CPUFUNC(op_13d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= regs.irc;
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 20 (4/1) */

/* MOVE.B -(An),(xxx).L */
void REGPARAM2 CPUFUNC(op_13e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= regs.irc;
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (6);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 22 (4/1) */

/* MOVE.B (d16,An),(xxx).L */
void REGPARAM2 CPUFUNC(op_13e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= regs.irc;
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (8);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 24 (5/1) */

/* MOVE.B (d8,An,Xn),(xxx).L */
void REGPARAM2 CPUFUNC(op_13f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= regs.irc;
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (8);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 26 (5/1) */

/* MOVE.B (xxx).W,(xxx).L */
void REGPARAM2 CPUFUNC(op_13f8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= regs.irc;
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (8);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 24 (5/1) */

/* MOVE.B (xxx).L,(xxx).L */
void REGPARAM2 CPUFUNC(op_13f9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (8) << 16;
	dsta |= regs.irc;
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (10);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 28 (6/1) */

/* MOVE.B (d16,PC),(xxx).L */
void REGPARAM2 CPUFUNC(op_13fa_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= regs.irc;
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (8);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 24 (5/1) */

/* MOVE.B (d8,PC,Xn),(xxx).L */
void REGPARAM2 CPUFUNC(op_13fb_12)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= regs.irc;
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (8);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}} /* 26 (5/1) */

/* MOVE.B #<data>.B,(xxx).L */
void REGPARAM2 CPUFUNC(op_13fc_12)(uae_u32 opcode)
{
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= get_word_ce000_prefetch (8);
	optflag_testb ((uae_s8)(src));
	x_put_byte (dsta, src);
	m68k_incpc (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 20 (4/1) */

/* MOVE.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_2000_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 4 (1/0) */

/* MOVE.L An,Dn */
void REGPARAM2 CPUFUNC(op_2008_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 4 (1/0) */

/* MOVE.L (An),Dn */
void REGPARAM2 CPUFUNC(op_2010_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4061;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4061: ;
} /* 12 (3/0) */

/* MOVE.L (An)+,Dn */
void REGPARAM2 CPUFUNC(op_2018_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4062;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4062: ;
} /* 12 (3/0) */

/* MOVE.L -(An),Dn */
void REGPARAM2 CPUFUNC(op_2020_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4063;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4063: ;
} /* 14 (3/0) */

/* MOVE.L (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_2028_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4064;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4064: ;
} /* 16 (4/0) */

/* MOVE.L (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_2030_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4065;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4065: ;
} /* 18 (4/0) */

/* MOVE.L (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_2038_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4066;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4066: ;
} /* 16 (4/0) */

/* MOVE.L (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_2039_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (4);
		exception3 (opcode, srca);
		goto endlabel4067;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4067: ;
} /* 20 (5/0) */

/* MOVE.L (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_203a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4068;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4068: ;
} /* 16 (4/0) */

/* MOVE.L (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_203b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4069;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4069: ;
} /* 18 (4/0) */

/* MOVE.L #<data>.L,Dn */
void REGPARAM2 CPUFUNC(op_203c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 12 (3/0) */

/* MOVEA.L Dn,An */
void REGPARAM2 CPUFUNC(op_2040_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 4 (1/0) */

/* MOVEA.L An,An */
void REGPARAM2 CPUFUNC(op_2048_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 4 (1/0) */

/* MOVEA.L (An),An */
void REGPARAM2 CPUFUNC(op_2050_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4073;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4073: ;
} /* 12 (3/0) */

/* MOVEA.L (An)+,An */
void REGPARAM2 CPUFUNC(op_2058_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4074;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4074: ;
} /* 12 (3/0) */

/* MOVEA.L -(An),An */
void REGPARAM2 CPUFUNC(op_2060_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4075;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4075: ;
} /* 14 (3/0) */

/* MOVEA.L (d16,An),An */
void REGPARAM2 CPUFUNC(op_2068_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4076;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4076: ;
} /* 16 (4/0) */

/* MOVEA.L (d8,An,Xn),An */
void REGPARAM2 CPUFUNC(op_2070_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4077;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4077: ;
} /* 18 (4/0) */

/* MOVEA.L (xxx).W,An */
void REGPARAM2 CPUFUNC(op_2078_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4078;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4078: ;
} /* 16 (4/0) */

/* MOVEA.L (xxx).L,An */
void REGPARAM2 CPUFUNC(op_2079_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (4);
		exception3 (opcode, srca);
		goto endlabel4079;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4079: ;
} /* 20 (5/0) */

/* MOVEA.L (d16,PC),An */
void REGPARAM2 CPUFUNC(op_207a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4080;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4080: ;
} /* 16 (4/0) */

/* MOVEA.L (d8,PC,Xn),An */
void REGPARAM2 CPUFUNC(op_207b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4081;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4081: ;
} /* 18 (4/0) */

/* MOVEA.L #<data>.L,An */
void REGPARAM2 CPUFUNC(op_207c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	m68k_areg (regs, dstreg) = (src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 12 (3/0) */

/* MOVE.L Dn,(An) */
void REGPARAM2 CPUFUNC(op_2080_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4083;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4083: ;
} /* 12 (1/2) */

/* MOVE.L An,(An) */
void REGPARAM2 CPUFUNC(op_2088_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4084;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4084: ;
} /* 12 (1/2) */

/* MOVE.L (An),(An) */
void REGPARAM2 CPUFUNC(op_2090_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4085;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4085;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4085: ;
} /* 20 (3/2) */

/* MOVE.L (An)+,(An) */
void REGPARAM2 CPUFUNC(op_2098_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4086;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4086;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4086: ;
} /* 20 (3/2) */

/* MOVE.L -(An),(An) */
void REGPARAM2 CPUFUNC(op_20a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4087;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4087;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4087: ;
} /* 22 (3/2) */

/* MOVE.L (d16,An),(An) */
void REGPARAM2 CPUFUNC(op_20a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4088;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4088;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4088: ;
} /* 24 (4/2) */

/* MOVE.L (d8,An,Xn),(An) */
void REGPARAM2 CPUFUNC(op_20b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4089;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4089;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4089: ;
} /* 26 (4/2) */

/* MOVE.L (xxx).W,(An) */
void REGPARAM2 CPUFUNC(op_20b8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4090;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4090;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4090: ;
} /* 24 (4/2) */

/* MOVE.L (xxx).L,(An) */
void REGPARAM2 CPUFUNC(op_20b9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (4);
		exception3 (opcode, srca);
		goto endlabel4091;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4091;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4091: ;
} /* 28 (5/2) */

/* MOVE.L (d16,PC),(An) */
void REGPARAM2 CPUFUNC(op_20ba_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4092;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4092;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4092: ;
} /* 24 (4/2) */

/* MOVE.L (d8,PC,Xn),(An) */
void REGPARAM2 CPUFUNC(op_20bb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4093;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4093;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4093: ;
} /* 26 (4/2) */

/* MOVE.L #<data>.L,(An) */
void REGPARAM2 CPUFUNC(op_20bc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4094;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4094: ;
} /* 20 (3/2) */

/* MOVE.L Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_20c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4095;
	}
{	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4095: ;
} /* 12 (1/2) */

/* MOVE.L An,(An)+ */
void REGPARAM2 CPUFUNC(op_20c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4096;
	}
{	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4096: ;
} /* 12 (1/2) */

/* MOVE.L (An),(An)+ */
void REGPARAM2 CPUFUNC(op_20d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4097;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4097;
	}
{	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4097: ;
} /* 20 (3/2) */

/* MOVE.L (An)+,(An)+ */
void REGPARAM2 CPUFUNC(op_20d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4098;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4098;
	}
{	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4098: ;
} /* 20 (3/2) */

/* MOVE.L -(An),(An)+ */
void REGPARAM2 CPUFUNC(op_20e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4099;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4099;
	}
{	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4099: ;
} /* 22 (3/2) */

/* MOVE.L (d16,An),(An)+ */
void REGPARAM2 CPUFUNC(op_20e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4100;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4100;
	}
{	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4100: ;
} /* 24 (4/2) */

/* MOVE.L (d8,An,Xn),(An)+ */
void REGPARAM2 CPUFUNC(op_20f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4101;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4101;
	}
{	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4101: ;
} /* 26 (4/2) */

/* MOVE.L (xxx).W,(An)+ */
void REGPARAM2 CPUFUNC(op_20f8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4102;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4102;
	}
{	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4102: ;
} /* 24 (4/2) */

/* MOVE.L (xxx).L,(An)+ */
void REGPARAM2 CPUFUNC(op_20f9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (4);
		exception3 (opcode, srca);
		goto endlabel4103;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4103;
	}
{	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4103: ;
} /* 28 (5/2) */

/* MOVE.L (d16,PC),(An)+ */
void REGPARAM2 CPUFUNC(op_20fa_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4104;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4104;
	}
{	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4104: ;
} /* 24 (4/2) */

/* MOVE.L (d8,PC,Xn),(An)+ */
void REGPARAM2 CPUFUNC(op_20fb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4105;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4105;
	}
{	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4105: ;
} /* 26 (4/2) */

/* MOVE.L #<data>.L,(An)+ */
void REGPARAM2 CPUFUNC(op_20fc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4106;
	}
{	m68k_areg (regs, dstreg) += 4;
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4106: ;
} /* 20 (3/2) */

/* MOVE.L Dn,-(An) */
void REGPARAM2 CPUFUNC(op_2100_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4107;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (2);
}}}}endlabel4107: ;
} /* 12 (1/2) */

/* MOVE.L An,-(An) */
void REGPARAM2 CPUFUNC(op_2108_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4108;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (2);
}}}}endlabel4108: ;
} /* 12 (1/2) */

/* MOVE.L (An),-(An) */
void REGPARAM2 CPUFUNC(op_2110_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4109;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4109;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (2);
}}}}}}endlabel4109: ;
} /* 20 (3/2) */

/* MOVE.L (An)+,-(An) */
void REGPARAM2 CPUFUNC(op_2118_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4110;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4110;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (2);
}}}}}}endlabel4110: ;
} /* 20 (3/2) */

/* MOVE.L -(An),-(An) */
void REGPARAM2 CPUFUNC(op_2120_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4111;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4111;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (2);
}}}}}}endlabel4111: ;
} /* 22 (3/2) */

/* MOVE.L (d16,An),-(An) */
void REGPARAM2 CPUFUNC(op_2128_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4112;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4112;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
}}}}}}endlabel4112: ;
} /* 24 (4/2) */

/* MOVE.L (d8,An,Xn),-(An) */
void REGPARAM2 CPUFUNC(op_2130_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4113;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4113;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
}}}}}}endlabel4113: ;
} /* 26 (4/2) */

/* MOVE.L (xxx).W,-(An) */
void REGPARAM2 CPUFUNC(op_2138_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4114;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4114;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
}}}}}}endlabel4114: ;
} /* 24 (4/2) */

/* MOVE.L (xxx).L,-(An) */
void REGPARAM2 CPUFUNC(op_2139_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (4);
		exception3 (opcode, srca);
		goto endlabel4115;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4115;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
}}}}}}endlabel4115: ;
} /* 28 (5/2) */

/* MOVE.L (d16,PC),-(An) */
void REGPARAM2 CPUFUNC(op_213a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4116;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4116;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
}}}}}}endlabel4116: ;
} /* 24 (4/2) */

/* MOVE.L (d8,PC,Xn),-(An) */
void REGPARAM2 CPUFUNC(op_213b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4117;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4117;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
}}}}}}endlabel4117: ;
} /* 26 (4/2) */

/* MOVE.L #<data>.L,-(An) */
void REGPARAM2 CPUFUNC(op_213c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4118;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
}}}}endlabel4118: ;
} /* 20 (3/2) */

/* MOVE.L Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_2140_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4119;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4119: ;
} /* 16 (2/2) */

/* MOVE.L An,(d16,An) */
void REGPARAM2 CPUFUNC(op_2148_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4120;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4120: ;
} /* 16 (2/2) */

/* MOVE.L (An),(d16,An) */
void REGPARAM2 CPUFUNC(op_2150_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4121;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4121;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4121: ;
} /* 24 (4/2) */

/* MOVE.L (An)+,(d16,An) */
void REGPARAM2 CPUFUNC(op_2158_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4122;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4122;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4122: ;
} /* 24 (4/2) */

/* MOVE.L -(An),(d16,An) */
void REGPARAM2 CPUFUNC(op_2160_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4123;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4123;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4123: ;
} /* 26 (4/2) */

/* MOVE.L (d16,An),(d16,An) */
void REGPARAM2 CPUFUNC(op_2168_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4124;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4124;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4124: ;
} /* 28 (5/2) */

#endif

#ifdef PART_3
/* MOVE.L (d8,An,Xn),(d16,An) */
void REGPARAM2 CPUFUNC(op_2170_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4125;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4125;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4125: ;
} /* 30 (5/2) */

/* MOVE.L (xxx).W,(d16,An) */
void REGPARAM2 CPUFUNC(op_2178_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4126;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4126;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4126: ;
} /* 28 (5/2) */

/* MOVE.L (xxx).L,(d16,An) */
void REGPARAM2 CPUFUNC(op_2179_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (4);
		exception3 (opcode, srca);
		goto endlabel4127;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4127;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4127: ;
} /* 32 (6/2) */

/* MOVE.L (d16,PC),(d16,An) */
void REGPARAM2 CPUFUNC(op_217a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4128;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4128;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4128: ;
} /* 28 (5/2) */

/* MOVE.L (d8,PC,Xn),(d16,An) */
void REGPARAM2 CPUFUNC(op_217b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4129;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4129;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4129: ;
} /* 30 (5/2) */

/* MOVE.L #<data>.L,(d16,An) */
void REGPARAM2 CPUFUNC(op_217c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4130;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4130: ;
} /* 24 (4/2) */

/* MOVE.L Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_2180_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4131;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4131: ;
} /* 18 (2/2) */

/* MOVE.L An,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_2188_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4132;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4132: ;
} /* 18 (2/2) */

/* MOVE.L (An),(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_2190_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4133;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4133;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4133: ;
} /* 26 (4/2) */

/* MOVE.L (An)+,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_2198_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4134;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4134;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4134: ;
} /* 26 (4/2) */

/* MOVE.L -(An),(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_21a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4135;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4135;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4135: ;
} /* 28 (4/2) */

/* MOVE.L (d16,An),(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_21a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4136;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4136;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4136: ;
} /* 30 (5/2) */

/* MOVE.L (d8,An,Xn),(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_21b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4137;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4137;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4137: ;
} /* 32 (5/2) */

/* MOVE.L (xxx).W,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_21b8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4138;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4138;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4138: ;
} /* 30 (5/2) */

/* MOVE.L (xxx).L,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_21b9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (4);
		exception3 (opcode, srca);
		goto endlabel4139;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (8));
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4139;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4139: ;
} /* 34 (6/2) */

/* MOVE.L (d16,PC),(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_21ba_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4140;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4140;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4140: ;
} /* 30 (5/2) */

/* MOVE.L (d8,PC,Xn),(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_21bb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4141;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4141;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4141: ;
} /* 32 (5/2) */

/* MOVE.L #<data>.L,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_21bc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (8));
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4142;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4142: ;
} /* 26 (4/2) */

/* MOVE.L Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_21c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4143;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4143: ;
} /* 16 (2/2) */

/* MOVE.L An,(xxx).W */
void REGPARAM2 CPUFUNC(op_21c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4144;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4144: ;
} /* 16 (2/2) */

/* MOVE.L (An),(xxx).W */
void REGPARAM2 CPUFUNC(op_21d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4145;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4145;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4145: ;
} /* 24 (4/2) */

/* MOVE.L (An)+,(xxx).W */
void REGPARAM2 CPUFUNC(op_21d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4146;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4146;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4146: ;
} /* 24 (4/2) */

/* MOVE.L -(An),(xxx).W */
void REGPARAM2 CPUFUNC(op_21e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4147;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4147;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4147: ;
} /* 26 (4/2) */

/* MOVE.L (d16,An),(xxx).W */
void REGPARAM2 CPUFUNC(op_21e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4148;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4148;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4148: ;
} /* 28 (5/2) */

/* MOVE.L (d8,An,Xn),(xxx).W */
void REGPARAM2 CPUFUNC(op_21f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4149;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4149;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4149: ;
} /* 30 (5/2) */

/* MOVE.L (xxx).W,(xxx).W */
void REGPARAM2 CPUFUNC(op_21f8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4150;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4150;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4150: ;
} /* 28 (5/2) */

/* MOVE.L (xxx).L,(xxx).W */
void REGPARAM2 CPUFUNC(op_21f9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (4);
		exception3 (opcode, srca);
		goto endlabel4151;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4151;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4151: ;
} /* 32 (6/2) */

/* MOVE.L (d16,PC),(xxx).W */
void REGPARAM2 CPUFUNC(op_21fa_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4152;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4152;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4152: ;
} /* 28 (5/2) */

/* MOVE.L (d8,PC,Xn),(xxx).W */
void REGPARAM2 CPUFUNC(op_21fb_12)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4153;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4153;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4153: ;
} /* 30 (5/2) */

/* MOVE.L #<data>.L,(xxx).W */
void REGPARAM2 CPUFUNC(op_21fc_12)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4154;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4154: ;
} /* 24 (4/2) */

/* MOVE.L Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_23c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4155;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4155: ;
} /* 20 (3/2) */

/* MOVE.L An,(xxx).L */
void REGPARAM2 CPUFUNC(op_23c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4156;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4156: ;
} /* 20 (3/2) */

/* MOVE.L (An),(xxx).L */
void REGPARAM2 CPUFUNC(op_23d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4157;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= regs.irc;
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4157;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4157: ;
} /* 28 (5/2) */

/* MOVE.L (An)+,(xxx).L */
void REGPARAM2 CPUFUNC(op_23d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4158;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= regs.irc;
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4158;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4158: ;
} /* 28 (5/2) */

/* MOVE.L -(An),(xxx).L */
void REGPARAM2 CPUFUNC(op_23e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4159;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= regs.irc;
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4159;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (6);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4159: ;
} /* 30 (5/2) */

/* MOVE.L (d16,An),(xxx).L */
void REGPARAM2 CPUFUNC(op_23e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4160;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= regs.irc;
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4160;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (8);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4160: ;
} /* 32 (6/2) */

/* MOVE.L (d8,An,Xn),(xxx).L */
void REGPARAM2 CPUFUNC(op_23f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4161;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= regs.irc;
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4161;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (8);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4161: ;
} /* 34 (6/2) */

/* MOVE.L (xxx).W,(xxx).L */
void REGPARAM2 CPUFUNC(op_23f8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4162;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= regs.irc;
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4162;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (8);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4162: ;
} /* 32 (6/2) */

/* MOVE.L (xxx).L,(xxx).L */
void REGPARAM2 CPUFUNC(op_23f9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (4);
		exception3 (opcode, srca);
		goto endlabel4163;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (8) << 16;
	dsta |= regs.irc;
	if (dsta & 1) {
	m68k_incpc (10);
		exception3 (opcode, dsta);
		goto endlabel4163;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (10);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4163: ;
} /* 36 (7/2) */

/* MOVE.L (d16,PC),(xxx).L */
void REGPARAM2 CPUFUNC(op_23fa_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4164;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= regs.irc;
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4164;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (8);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4164: ;
} /* 32 (6/2) */

/* MOVE.L (d8,PC,Xn),(xxx).L */
void REGPARAM2 CPUFUNC(op_23fb_12)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4165;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= regs.irc;
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4165;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (8);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4165: ;
} /* 34 (6/2) */

/* MOVE.L #<data>.L,(xxx).L */
void REGPARAM2 CPUFUNC(op_23fc_12)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (8) << 16;
	dsta |= get_word_ce000_prefetch (10);
	if (dsta & 1) {
	m68k_incpc (10);
		exception3 (opcode, dsta);
		goto endlabel4166;
	}
{	optflag_testl ((uae_s32)(src));
	x_put_word (dsta, src >> 16); x_put_word (dsta + 2, src);
	m68k_incpc (10);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4166: ;
} /* 28 (5/2) */

/* MOVE.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_3000_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 4 (1/0) */

/* MOVE.W An,Dn */
void REGPARAM2 CPUFUNC(op_3008_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 4 (1/0) */

/* MOVE.W (An),Dn */
void REGPARAM2 CPUFUNC(op_3010_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4169;
	}
{{	uae_s16 src = x_get_word (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4169: ;
} /* 8 (2/0) */

/* MOVE.W (An)+,Dn */
void REGPARAM2 CPUFUNC(op_3018_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4170;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4170: ;
} /* 8 (2/0) */

/* MOVE.W -(An),Dn */
void REGPARAM2 CPUFUNC(op_3020_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4171;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4171: ;
} /* 10 (2/0) */

/* MOVE.W (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_3028_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4172;
	}
{{	uae_s16 src = x_get_word (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4172: ;
} /* 12 (3/0) */

/* MOVE.W (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_3030_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4173;
	}
{{	uae_s16 src = x_get_word (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4173: ;
} /* 14 (3/0) */

/* MOVE.W (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_3038_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4174;
	}
{{	uae_s16 src = x_get_word (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4174: ;
} /* 12 (3/0) */

/* MOVE.W (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_3039_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (4);
		exception3 (opcode, srca);
		goto endlabel4175;
	}
{{	uae_s16 src = x_get_word (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4175: ;
} /* 16 (4/0) */

/* MOVE.W (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_303a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4176;
	}
{{	uae_s16 src = x_get_word (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4176: ;
} /* 12 (3/0) */

/* MOVE.W (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_303b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4177;
	}
{{	uae_s16 src = x_get_word (srca);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4177: ;
} /* 14 (3/0) */

/* MOVE.W #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_303c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	optflag_testw ((uae_s16)(src));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 8 (2/0) */

/* MOVEA.W Dn,An */
void REGPARAM2 CPUFUNC(op_3040_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 4 (1/0) */

/* MOVEA.W An,An */
void REGPARAM2 CPUFUNC(op_3048_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 4 (1/0) */

/* MOVEA.W (An),An */
void REGPARAM2 CPUFUNC(op_3050_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4181;
	}
{{	uae_s16 src = x_get_word (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4181: ;
} /* 8 (2/0) */

/* MOVEA.W (An)+,An */
void REGPARAM2 CPUFUNC(op_3058_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4182;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4182: ;
} /* 8 (2/0) */

/* MOVEA.W -(An),An */
void REGPARAM2 CPUFUNC(op_3060_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4183;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4183: ;
} /* 10 (2/0) */

/* MOVEA.W (d16,An),An */
void REGPARAM2 CPUFUNC(op_3068_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4184;
	}
{{	uae_s16 src = x_get_word (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4184: ;
} /* 12 (3/0) */

/* MOVEA.W (d8,An,Xn),An */
void REGPARAM2 CPUFUNC(op_3070_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4185;
	}
{{	uae_s16 src = x_get_word (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4185: ;
} /* 14 (3/0) */

/* MOVEA.W (xxx).W,An */
void REGPARAM2 CPUFUNC(op_3078_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4186;
	}
{{	uae_s16 src = x_get_word (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4186: ;
} /* 12 (3/0) */

/* MOVEA.W (xxx).L,An */
void REGPARAM2 CPUFUNC(op_3079_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (4);
		exception3 (opcode, srca);
		goto endlabel4187;
	}
{{	uae_s16 src = x_get_word (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4187: ;
} /* 16 (4/0) */

/* MOVEA.W (d16,PC),An */
void REGPARAM2 CPUFUNC(op_307a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4188;
	}
{{	uae_s16 src = x_get_word (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4188: ;
} /* 12 (3/0) */

/* MOVEA.W (d8,PC,Xn),An */
void REGPARAM2 CPUFUNC(op_307b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4189;
	}
{{	uae_s16 src = x_get_word (srca);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4189: ;
} /* 14 (3/0) */

/* MOVEA.W #<data>.W,An */
void REGPARAM2 CPUFUNC(op_307c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	src = (uae_s32)(uae_s16)src;
	m68k_areg (regs, dstreg) = (uae_s32)(uae_s16)(src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 8 (2/0) */

/* MOVE.W Dn,(An) */
void REGPARAM2 CPUFUNC(op_3080_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4191;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4191: ;
} /* 8 (1/1) */

/* MOVE.W An,(An) */
void REGPARAM2 CPUFUNC(op_3088_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4192;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4192: ;
} /* 8 (1/1) */

/* MOVE.W (An),(An) */
void REGPARAM2 CPUFUNC(op_3090_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4193;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4193;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4193: ;
} /* 12 (2/1) */

/* MOVE.W (An)+,(An) */
void REGPARAM2 CPUFUNC(op_3098_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4194;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4194;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4194: ;
} /* 12 (2/1) */

/* MOVE.W -(An),(An) */
void REGPARAM2 CPUFUNC(op_30a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4195;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4195;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4195: ;
} /* 14 (2/1) */

/* MOVE.W (d16,An),(An) */
void REGPARAM2 CPUFUNC(op_30a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4196;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4196;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4196: ;
} /* 16 (3/1) */

/* MOVE.W (d8,An,Xn),(An) */
void REGPARAM2 CPUFUNC(op_30b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4197;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4197;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4197: ;
} /* 18 (3/1) */

/* MOVE.W (xxx).W,(An) */
void REGPARAM2 CPUFUNC(op_30b8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4198;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4198;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4198: ;
} /* 16 (3/1) */

/* MOVE.W (xxx).L,(An) */
void REGPARAM2 CPUFUNC(op_30b9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (4);
		exception3 (opcode, srca);
		goto endlabel4199;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4199;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4199: ;
} /* 20 (4/1) */

/* MOVE.W (d16,PC),(An) */
void REGPARAM2 CPUFUNC(op_30ba_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4200;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4200;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4200: ;
} /* 16 (3/1) */

/* MOVE.W (d8,PC,Xn),(An) */
void REGPARAM2 CPUFUNC(op_30bb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4201;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4201;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4201: ;
} /* 18 (3/1) */

/* MOVE.W #<data>.W,(An) */
void REGPARAM2 CPUFUNC(op_30bc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4202;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4202: ;
} /* 12 (2/1) */

/* MOVE.W Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_30c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4203;
	}
{	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4203: ;
} /* 8 (1/1) */

/* MOVE.W An,(An)+ */
void REGPARAM2 CPUFUNC(op_30c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4204;
	}
{	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4204: ;
} /* 8 (1/1) */

/* MOVE.W (An),(An)+ */
void REGPARAM2 CPUFUNC(op_30d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4205;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4205;
	}
{	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4205: ;
} /* 12 (2/1) */

/* MOVE.W (An)+,(An)+ */
void REGPARAM2 CPUFUNC(op_30d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4206;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4206;
	}
{	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4206: ;
} /* 12 (2/1) */

/* MOVE.W -(An),(An)+ */
void REGPARAM2 CPUFUNC(op_30e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4207;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4207;
	}
{	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4207: ;
} /* 14 (2/1) */

/* MOVE.W (d16,An),(An)+ */
void REGPARAM2 CPUFUNC(op_30e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4208;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4208;
	}
{	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4208: ;
} /* 16 (3/1) */

/* MOVE.W (d8,An,Xn),(An)+ */
void REGPARAM2 CPUFUNC(op_30f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4209;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4209;
	}
{	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4209: ;
} /* 18 (3/1) */

/* MOVE.W (xxx).W,(An)+ */
void REGPARAM2 CPUFUNC(op_30f8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4210;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4210;
	}
{	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4210: ;
} /* 16 (3/1) */

/* MOVE.W (xxx).L,(An)+ */
void REGPARAM2 CPUFUNC(op_30f9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (4);
		exception3 (opcode, srca);
		goto endlabel4211;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4211;
	}
{	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4211: ;
} /* 20 (4/1) */

/* MOVE.W (d16,PC),(An)+ */
void REGPARAM2 CPUFUNC(op_30fa_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4212;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4212;
	}
{	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4212: ;
} /* 16 (3/1) */

/* MOVE.W (d8,PC,Xn),(An)+ */
void REGPARAM2 CPUFUNC(op_30fb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4213;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4213;
	}
{	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4213: ;
} /* 18 (3/1) */

/* MOVE.W #<data>.W,(An)+ */
void REGPARAM2 CPUFUNC(op_30fc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4214;
	}
{	m68k_areg (regs, dstreg) += 2;
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4214: ;
} /* 12 (2/1) */

/* MOVE.W Dn,-(An) */
void REGPARAM2 CPUFUNC(op_3100_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4215;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (2);
}}}}endlabel4215: ;
} /* 8 (1/1) */

/* MOVE.W An,-(An) */
void REGPARAM2 CPUFUNC(op_3108_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4216;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (2);
}}}}endlabel4216: ;
} /* 8 (1/1) */

/* MOVE.W (An),-(An) */
void REGPARAM2 CPUFUNC(op_3110_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4217;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4217;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (2);
}}}}}}endlabel4217: ;
} /* 12 (2/1) */

/* MOVE.W (An)+,-(An) */
void REGPARAM2 CPUFUNC(op_3118_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4218;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4218;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (2);
}}}}}}endlabel4218: ;
} /* 12 (2/1) */

/* MOVE.W -(An),-(An) */
void REGPARAM2 CPUFUNC(op_3120_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4219;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4219;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (2);
}}}}}}endlabel4219: ;
} /* 14 (2/1) */

/* MOVE.W (d16,An),-(An) */
void REGPARAM2 CPUFUNC(op_3128_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4220;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4220;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
}}}}}}endlabel4220: ;
} /* 16 (3/1) */

/* MOVE.W (d8,An,Xn),-(An) */
void REGPARAM2 CPUFUNC(op_3130_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4221;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4221;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
}}}}}}endlabel4221: ;
} /* 18 (3/1) */

/* MOVE.W (xxx).W,-(An) */
void REGPARAM2 CPUFUNC(op_3138_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4222;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4222;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
}}}}}}endlabel4222: ;
} /* 16 (3/1) */

/* MOVE.W (xxx).L,-(An) */
void REGPARAM2 CPUFUNC(op_3139_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (4);
		exception3 (opcode, srca);
		goto endlabel4223;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4223;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
}}}}}}endlabel4223: ;
} /* 20 (4/1) */

/* MOVE.W (d16,PC),-(An) */
void REGPARAM2 CPUFUNC(op_313a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4224;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4224;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
}}}}}}endlabel4224: ;
} /* 16 (3/1) */

/* MOVE.W (d8,PC,Xn),-(An) */
void REGPARAM2 CPUFUNC(op_313b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4225;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4225;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
}}}}}}endlabel4225: ;
} /* 18 (3/1) */

/* MOVE.W #<data>.W,-(An) */
void REGPARAM2 CPUFUNC(op_313c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4226;
	}
{	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
}}}}endlabel4226: ;
} /* 12 (2/1) */

/* MOVE.W Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_3140_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4227;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4227: ;
} /* 12 (2/1) */

/* MOVE.W An,(d16,An) */
void REGPARAM2 CPUFUNC(op_3148_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4228;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4228: ;
} /* 12 (2/1) */

/* MOVE.W (An),(d16,An) */
void REGPARAM2 CPUFUNC(op_3150_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4229;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4229;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4229: ;
} /* 16 (3/1) */

/* MOVE.W (An)+,(d16,An) */
void REGPARAM2 CPUFUNC(op_3158_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4230;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4230;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4230: ;
} /* 16 (3/1) */

/* MOVE.W -(An),(d16,An) */
void REGPARAM2 CPUFUNC(op_3160_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4231;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4231;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4231: ;
} /* 18 (3/1) */

/* MOVE.W (d16,An),(d16,An) */
void REGPARAM2 CPUFUNC(op_3168_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4232;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4232;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4232: ;
} /* 20 (4/1) */

/* MOVE.W (d8,An,Xn),(d16,An) */
void REGPARAM2 CPUFUNC(op_3170_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4233;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4233;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4233: ;
} /* 22 (4/1) */

/* MOVE.W (xxx).W,(d16,An) */
void REGPARAM2 CPUFUNC(op_3178_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4234;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4234;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4234: ;
} /* 20 (4/1) */

/* MOVE.W (xxx).L,(d16,An) */
void REGPARAM2 CPUFUNC(op_3179_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (4);
		exception3 (opcode, srca);
		goto endlabel4235;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4235;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4235: ;
} /* 24 (5/1) */

/* MOVE.W (d16,PC),(d16,An) */
void REGPARAM2 CPUFUNC(op_317a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4236;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4236;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4236: ;
} /* 20 (4/1) */

/* MOVE.W (d8,PC,Xn),(d16,An) */
void REGPARAM2 CPUFUNC(op_317b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4237;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4237;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4237: ;
} /* 22 (4/1) */

/* MOVE.W #<data>.W,(d16,An) */
void REGPARAM2 CPUFUNC(op_317c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4238;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4238: ;
} /* 16 (3/1) */

/* MOVE.W Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_3180_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4239;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4239: ;
} /* 14 (2/1) */

/* MOVE.W An,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_3188_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4240;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4240: ;
} /* 14 (2/1) */

/* MOVE.W (An),(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_3190_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4241;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4241;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4241: ;
} /* 18 (3/1) */

/* MOVE.W (An)+,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_3198_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4242;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4242;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4242: ;
} /* 18 (3/1) */

/* MOVE.W -(An),(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_31a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4243;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4243;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4243: ;
} /* 20 (3/1) */

/* MOVE.W (d16,An),(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_31a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4244;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4244;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4244: ;
} /* 22 (4/1) */

/* MOVE.W (d8,An,Xn),(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_31b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4245;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4245;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4245: ;
} /* 24 (4/1) */

/* MOVE.W (xxx).W,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_31b8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4246;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4246;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4246: ;
} /* 22 (4/1) */

/* MOVE.W (xxx).L,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_31b9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (4);
		exception3 (opcode, srca);
		goto endlabel4247;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (8));
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4247;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4247: ;
} /* 26 (5/1) */

/* MOVE.W (d16,PC),(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_31ba_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4248;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4248;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4248: ;
} /* 22 (4/1) */

/* MOVE.W (d8,PC,Xn),(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_31bb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4249;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4249;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4249: ;
} /* 24 (4/1) */

/* MOVE.W #<data>.W,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_31bc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4250;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4250: ;
} /* 18 (3/1) */

/* MOVE.W Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_31c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4251;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4251: ;
} /* 12 (2/1) */

/* MOVE.W An,(xxx).W */
void REGPARAM2 CPUFUNC(op_31c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4252;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4252: ;
} /* 12 (2/1) */

/* MOVE.W (An),(xxx).W */
void REGPARAM2 CPUFUNC(op_31d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4253;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4253;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4253: ;
} /* 16 (3/1) */

/* MOVE.W (An)+,(xxx).W */
void REGPARAM2 CPUFUNC(op_31d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4254;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4254;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4254: ;
} /* 16 (3/1) */

/* MOVE.W -(An),(xxx).W */
void REGPARAM2 CPUFUNC(op_31e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4255;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4255;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4255: ;
} /* 18 (3/1) */

/* MOVE.W (d16,An),(xxx).W */
void REGPARAM2 CPUFUNC(op_31e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4256;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4256;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4256: ;
} /* 20 (4/1) */

/* MOVE.W (d8,An,Xn),(xxx).W */
void REGPARAM2 CPUFUNC(op_31f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4257;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4257;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4257: ;
} /* 22 (4/1) */

/* MOVE.W (xxx).W,(xxx).W */
void REGPARAM2 CPUFUNC(op_31f8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4258;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4258;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4258: ;
} /* 20 (4/1) */

/* MOVE.W (xxx).L,(xxx).W */
void REGPARAM2 CPUFUNC(op_31f9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (4);
		exception3 (opcode, srca);
		goto endlabel4259;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4259;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4259: ;
} /* 24 (5/1) */

/* MOVE.W (d16,PC),(xxx).W */
void REGPARAM2 CPUFUNC(op_31fa_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4260;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4260;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4260: ;
} /* 20 (4/1) */

/* MOVE.W (d8,PC,Xn),(xxx).W */
void REGPARAM2 CPUFUNC(op_31fb_12)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4261;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4261;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4261: ;
} /* 22 (4/1) */

/* MOVE.W #<data>.W,(xxx).W */
void REGPARAM2 CPUFUNC(op_31fc_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4262;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4262: ;
} /* 16 (3/1) */

/* MOVE.W Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_33c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4263;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4263: ;
} /* 16 (3/1) */

/* MOVE.W An,(xxx).L */
void REGPARAM2 CPUFUNC(op_33c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4264;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4264: ;
} /* 16 (3/1) */

/* MOVE.W (An),(xxx).L */
void REGPARAM2 CPUFUNC(op_33d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4265;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= regs.irc;
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4265;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4265: ;
} /* 20 (4/1) */

/* MOVE.W (An)+,(xxx).L */
void REGPARAM2 CPUFUNC(op_33d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4266;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= regs.irc;
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4266;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4266: ;
} /* 20 (4/1) */

/* MOVE.W -(An),(xxx).L */
void REGPARAM2 CPUFUNC(op_33e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4267;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= regs.irc;
	if (dsta & 1) {
	m68k_incpc (6);
		exception3 (opcode, dsta);
		goto endlabel4267;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (6);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4267: ;
} /* 22 (4/1) */

/* MOVE.W (d16,An),(xxx).L */
void REGPARAM2 CPUFUNC(op_33e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4268;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= regs.irc;
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4268;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (8);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4268: ;
} /* 24 (5/1) */

/* MOVE.W (d8,An,Xn),(xxx).L */
void REGPARAM2 CPUFUNC(op_33f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4269;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= regs.irc;
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4269;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (8);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4269: ;
} /* 26 (5/1) */

/* MOVE.W (xxx).W,(xxx).L */
void REGPARAM2 CPUFUNC(op_33f8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4270;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= regs.irc;
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4270;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (8);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4270: ;
} /* 24 (5/1) */

/* MOVE.W (xxx).L,(xxx).L */
void REGPARAM2 CPUFUNC(op_33f9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (4);
		exception3 (opcode, srca);
		goto endlabel4271;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (8) << 16;
	dsta |= regs.irc;
	if (dsta & 1) {
	m68k_incpc (10);
		exception3 (opcode, dsta);
		goto endlabel4271;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (10);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4271: ;
} /* 28 (6/1) */

/* MOVE.W (d16,PC),(xxx).L */
void REGPARAM2 CPUFUNC(op_33fa_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4272;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= regs.irc;
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4272;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (8);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4272: ;
} /* 24 (5/1) */

/* MOVE.W (d8,PC,Xn),(xxx).L */
void REGPARAM2 CPUFUNC(op_33fb_12)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4273;
	}
{{	uae_s16 src = x_get_word (srca);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= regs.irc;
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4273;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (8);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}endlabel4273: ;
} /* 26 (5/1) */

/* MOVE.W #<data>.W,(xxx).L */
void REGPARAM2 CPUFUNC(op_33fc_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (6) << 16;
	dsta |= get_word_ce000_prefetch (8);
	if (dsta & 1) {
	m68k_incpc (8);
		exception3 (opcode, dsta);
		goto endlabel4274;
	}
{	optflag_testw ((uae_s16)(src));
	x_put_word (dsta, src);
	m68k_incpc (8);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}endlabel4274: ;
} /* 20 (4/1) */

/* NEGX.B Dn */
void REGPARAM2 CPUFUNC(op_4000_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
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
}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* NEGX.B (An) */
void REGPARAM2 CPUFUNC(op_4010_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	x_put_byte (srca, newv);
}}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* NEGX.B (An)+ */
void REGPARAM2 CPUFUNC(op_4018_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	x_put_byte (srca, newv);
}}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* NEGX.B -(An) */
void REGPARAM2 CPUFUNC(op_4020_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	x_put_byte (srca, newv);
}}}}}	m68k_incpc (2);
} /* 14 (2/1) */

/* NEGX.B (d16,An) */
void REGPARAM2 CPUFUNC(op_4028_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	x_put_byte (srca, newv);
}}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* NEGX.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_4030_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	x_put_byte (srca, newv);
}}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* NEGX.B (xxx).W */
void REGPARAM2 CPUFUNC(op_4038_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	x_put_byte (srca, newv);
}}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* NEGX.B (xxx).L */
void REGPARAM2 CPUFUNC(op_4039_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(0)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	x_put_byte (srca, newv);
}}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* NEGX.W Dn */
void REGPARAM2 CPUFUNC(op_4040_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
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
}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* NEGX.W (An) */
void REGPARAM2 CPUFUNC(op_4050_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4284;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	x_put_word (srca, newv);
}}}}}}	m68k_incpc (2);
endlabel4284: ;
} /* 12 (2/1) */

/* NEGX.W (An)+ */
void REGPARAM2 CPUFUNC(op_4058_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4285;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	x_put_word (srca, newv);
}}}}}}	m68k_incpc (2);
endlabel4285: ;
} /* 12 (2/1) */

/* NEGX.W -(An) */
void REGPARAM2 CPUFUNC(op_4060_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4286;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	x_put_word (srca, newv);
}}}}}}	m68k_incpc (2);
endlabel4286: ;
} /* 14 (2/1) */

/* NEGX.W (d16,An) */
void REGPARAM2 CPUFUNC(op_4068_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4287;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	x_put_word (srca, newv);
}}}}}}	m68k_incpc (4);
endlabel4287: ;
} /* 16 (3/1) */

/* NEGX.W (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_4070_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4288;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	x_put_word (srca, newv);
}}}}}}	m68k_incpc (4);
endlabel4288: ;
} /* 18 (3/1) */

/* NEGX.W (xxx).W */
void REGPARAM2 CPUFUNC(op_4078_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4289;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	x_put_word (srca, newv);
}}}}}}	m68k_incpc (4);
endlabel4289: ;
} /* 16 (3/1) */

/* NEGX.W (xxx).L */
void REGPARAM2 CPUFUNC(op_4079_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4290;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(0)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	x_put_word (srca, newv);
}}}}}}	m68k_incpc (6);
endlabel4290: ;
} /* 20 (4/1) */

/* NEGX.L Dn */
void REGPARAM2 CPUFUNC(op_4080_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
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
}}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* NEGX.L (An) */
void REGPARAM2 CPUFUNC(op_4090_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4292;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	x_put_word (srca + 2, newv); x_put_word (srca, newv >> 16);
}}}}}}	m68k_incpc (2);
endlabel4292: ;
} /* 20 (3/2) */

/* NEGX.L (An)+ */
void REGPARAM2 CPUFUNC(op_4098_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4293;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	x_put_word (srca + 2, newv); x_put_word (srca, newv >> 16);
}}}}}}	m68k_incpc (2);
endlabel4293: ;
} /* 20 (3/2) */

/* NEGX.L -(An) */
void REGPARAM2 CPUFUNC(op_40a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4294;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	x_put_word (srca + 2, newv); x_put_word (srca, newv >> 16);
}}}}}}	m68k_incpc (2);
endlabel4294: ;
} /* 22 (3/2) */

/* NEGX.L (d16,An) */
void REGPARAM2 CPUFUNC(op_40a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4295;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	x_put_word (srca + 2, newv); x_put_word (srca, newv >> 16);
}}}}}}	m68k_incpc (4);
endlabel4295: ;
} /* 24 (4/2) */

/* NEGX.L (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_40b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4296;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	x_put_word (srca + 2, newv); x_put_word (srca, newv >> 16);
}}}}}}	m68k_incpc (4);
endlabel4296: ;
} /* 26 (4/2) */

/* NEGX.L (xxx).W */
void REGPARAM2 CPUFUNC(op_40b8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4297;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	x_put_word (srca + 2, newv); x_put_word (srca, newv >> 16);
}}}}}}	m68k_incpc (4);
endlabel4297: ;
} /* 24 (4/2) */

/* NEGX.L (xxx).L */
void REGPARAM2 CPUFUNC(op_40b9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4298;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	uae_u32 newv = 0 - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(0)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	x_put_word (srca + 2, newv); x_put_word (srca, newv >> 16);
}}}}}}	m68k_incpc (6);
endlabel4298: ;
} /* 28 (5/2) */

/* MVSR2.W Dn */
void REGPARAM2 CPUFUNC(op_40c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
	MakeSR ();
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((regs.sr) & 0xffff);
}}	m68k_incpc (2);
} /* 6 (1/0) */

/* MVSR2.W (An) */
void REGPARAM2 CPUFUNC(op_40d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4300;
	}
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	MakeSR ();
	x_put_word (srca, regs.sr);
}}}	m68k_incpc (2);
endlabel4300: ;
} /* 8 (1/1) */

/* MVSR2.W (An)+ */
void REGPARAM2 CPUFUNC(op_40d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4301;
	}
{	m68k_areg (regs, srcreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	MakeSR ();
	x_put_word (srca, regs.sr);
}}}	m68k_incpc (2);
endlabel4301: ;
} /* 8 (1/1) */

/* MVSR2.W -(An) */
void REGPARAM2 CPUFUNC(op_40e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4302;
	}
{	m68k_areg (regs, srcreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	MakeSR ();
	x_put_word (srca, regs.sr);
}}}	m68k_incpc (2);
endlabel4302: ;
} /* 10 (1/1) */

/* MVSR2.W (d16,An) */
void REGPARAM2 CPUFUNC(op_40e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4303;
	}
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	MakeSR ();
	x_put_word (srca, regs.sr);
}}}	m68k_incpc (4);
endlabel4303: ;
} /* 12 (2/1) */

/* MVSR2.W (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_40f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4304;
	}
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	MakeSR ();
	x_put_word (srca, regs.sr);
}}}	m68k_incpc (4);
endlabel4304: ;
} /* 14 (2/1) */

/* MVSR2.W (xxx).W */
void REGPARAM2 CPUFUNC(op_40f8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4305;
	}
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	MakeSR ();
	x_put_word (srca, regs.sr);
}}}	m68k_incpc (4);
endlabel4305: ;
} /* 12 (2/1) */

/* MVSR2.W (xxx).L */
void REGPARAM2 CPUFUNC(op_40f9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4306;
	}
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	MakeSR ();
	x_put_word (srca, regs.sr);
}}}	m68k_incpc (6);
endlabel4306: ;
} /* 16 (3/1) */

/* CHK.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_4180_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpc (2);
	do_cycles_ce000 (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel4307;
	}
	do_cycles_ce000 (2);
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel4307;
	}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4307: ;
} /* 10 (1/0) */

/* CHK.W (An),Dn */
void REGPARAM2 CPUFUNC(op_4190_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4308;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpc (2);
	do_cycles_ce000 (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel4308;
	}
	do_cycles_ce000 (2);
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel4308;
	}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4308: ;
} /* 14 (2/0) */

/* CHK.W (An)+,Dn */
void REGPARAM2 CPUFUNC(op_4198_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4309;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpc (2);
	do_cycles_ce000 (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel4309;
	}
	do_cycles_ce000 (2);
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel4309;
	}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4309: ;
} /* 14 (2/0) */

/* CHK.W -(An),Dn */
void REGPARAM2 CPUFUNC(op_41a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4310;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpc (2);
	do_cycles_ce000 (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel4310;
	}
	do_cycles_ce000 (2);
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel4310;
	}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4310: ;
} /* 16 (2/0) */

/* CHK.W (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_41a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4311;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpc (4);
	do_cycles_ce000 (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel4311;
	}
	do_cycles_ce000 (2);
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel4311;
	}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4311: ;
} /* 18 (3/0) */

/* CHK.W (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_41b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4312;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpc (4);
	do_cycles_ce000 (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel4312;
	}
	do_cycles_ce000 (2);
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel4312;
	}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4312: ;
} /* 20 (3/0) */

/* CHK.W (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_41b8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4313;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpc (4);
	do_cycles_ce000 (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel4313;
	}
	do_cycles_ce000 (2);
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel4313;
	}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4313: ;
} /* 18 (3/0) */

/* CHK.W (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_41b9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4314;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpc (6);
	do_cycles_ce000 (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel4314;
	}
	do_cycles_ce000 (2);
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel4314;
	}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4314: ;
} /* 22 (4/0) */

/* CHK.W (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_41ba_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4315;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpc (4);
	do_cycles_ce000 (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel4315;
	}
	do_cycles_ce000 (2);
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel4315;
	}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4315: ;
} /* 18 (3/0) */

/* CHK.W (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_41bb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4316;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpc (4);
	do_cycles_ce000 (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel4316;
	}
	do_cycles_ce000 (2);
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel4316;
	}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4316: ;
} /* 20 (3/0) */

/* CHK.W #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_41bc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	m68k_incpc (4);
	do_cycles_ce000 (4);
	if (dst > src) {
		SET_NFLG (0);
		Exception (6);
		goto endlabel4317;
	}
	do_cycles_ce000 (2);
	if ((uae_s32)dst < 0) {
		SET_NFLG (1);
		Exception (6);
		goto endlabel4317;
	}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4317: ;
} /* 14 (2/0) */

/* LEA.L (An),An */
void REGPARAM2 CPUFUNC(op_41d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* LEA.L (d16,An),An */
void REGPARAM2 CPUFUNC(op_41e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* LEA.L (d8,An,Xn),An */
void REGPARAM2 CPUFUNC(op_41f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpc (4);
} /* 12 (2/0) */

/* LEA.L (xxx).W,An */
void REGPARAM2 CPUFUNC(op_41f8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* LEA.L (xxx).L,An */
void REGPARAM2 CPUFUNC(op_41f9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpc (6);
} /* 12 (3/0) */

/* LEA.L (d16,PC),An */
void REGPARAM2 CPUFUNC(op_41fa_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* LEA.L (d8,PC,Xn),An */
void REGPARAM2 CPUFUNC(op_41fb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
{	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
	m68k_areg (regs, dstreg) = (srca);
}}}	m68k_incpc (4);
} /* 12 (2/0) */

/* CLR.B Dn */
void REGPARAM2 CPUFUNC(op_4200_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	SET_CZNV (FLAGVAL_Z);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((0) & 0xff);
}}	m68k_incpc (2);
} /* 4 (1/0) */

/* CLR.B (An) */
void REGPARAM2 CPUFUNC(op_4210_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	SET_CZNV (FLAGVAL_Z);
	x_put_byte (srca, 0);
}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* CLR.B (An)+ */
void REGPARAM2 CPUFUNC(op_4218_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	SET_CZNV (FLAGVAL_Z);
	x_put_byte (srca, 0);
}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* CLR.B -(An) */
void REGPARAM2 CPUFUNC(op_4220_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	SET_CZNV (FLAGVAL_Z);
	x_put_byte (srca, 0);
}}}	m68k_incpc (2);
} /* 14 (2/1) */

/* CLR.B (d16,An) */
void REGPARAM2 CPUFUNC(op_4228_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	SET_CZNV (FLAGVAL_Z);
	x_put_byte (srca, 0);
}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* CLR.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_4230_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	SET_CZNV (FLAGVAL_Z);
	x_put_byte (srca, 0);
}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* CLR.B (xxx).W */
void REGPARAM2 CPUFUNC(op_4238_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	SET_CZNV (FLAGVAL_Z);
	x_put_byte (srca, 0);
}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* CLR.B (xxx).L */
void REGPARAM2 CPUFUNC(op_4239_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	SET_CZNV (FLAGVAL_Z);
	x_put_byte (srca, 0);
}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* CLR.W Dn */
void REGPARAM2 CPUFUNC(op_4240_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	SET_CZNV (FLAGVAL_Z);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((0) & 0xffff);
}}	m68k_incpc (2);
} /* 4 (1/0) */

/* CLR.W (An) */
void REGPARAM2 CPUFUNC(op_4250_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4334;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	SET_CZNV (FLAGVAL_Z);
	x_put_word (srca, 0);
}}}}	m68k_incpc (2);
endlabel4334: ;
} /* 12 (2/1) */

/* CLR.W (An)+ */
void REGPARAM2 CPUFUNC(op_4258_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4335;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	SET_CZNV (FLAGVAL_Z);
	x_put_word (srca, 0);
}}}}	m68k_incpc (2);
endlabel4335: ;
} /* 12 (2/1) */

/* CLR.W -(An) */
void REGPARAM2 CPUFUNC(op_4260_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4336;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	SET_CZNV (FLAGVAL_Z);
	x_put_word (srca, 0);
}}}}	m68k_incpc (2);
endlabel4336: ;
} /* 14 (2/1) */

/* CLR.W (d16,An) */
void REGPARAM2 CPUFUNC(op_4268_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4337;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	SET_CZNV (FLAGVAL_Z);
	x_put_word (srca, 0);
}}}}	m68k_incpc (4);
endlabel4337: ;
} /* 16 (3/1) */

/* CLR.W (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_4270_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4338;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	SET_CZNV (FLAGVAL_Z);
	x_put_word (srca, 0);
}}}}	m68k_incpc (4);
endlabel4338: ;
} /* 18 (3/1) */

/* CLR.W (xxx).W */
void REGPARAM2 CPUFUNC(op_4278_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4339;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	SET_CZNV (FLAGVAL_Z);
	x_put_word (srca, 0);
}}}}	m68k_incpc (4);
endlabel4339: ;
} /* 16 (3/1) */

/* CLR.W (xxx).L */
void REGPARAM2 CPUFUNC(op_4279_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4340;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	SET_CZNV (FLAGVAL_Z);
	x_put_word (srca, 0);
}}}}	m68k_incpc (6);
endlabel4340: ;
} /* 20 (4/1) */

/* CLR.L Dn */
void REGPARAM2 CPUFUNC(op_4280_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
	SET_CZNV (FLAGVAL_Z);
	m68k_dreg (regs, srcreg) = (0);
}}	m68k_incpc (2);
} /* 6 (1/0) */

/* CLR.L (An) */
void REGPARAM2 CPUFUNC(op_4290_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4342;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	SET_CZNV (FLAGVAL_Z);
	x_put_word (srca + 2, 0); x_put_word (srca, 0 >> 16);
}}}}	m68k_incpc (2);
endlabel4342: ;
} /* 20 (3/2) */

/* CLR.L (An)+ */
void REGPARAM2 CPUFUNC(op_4298_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4343;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	SET_CZNV (FLAGVAL_Z);
	x_put_word (srca + 2, 0); x_put_word (srca, 0 >> 16);
}}}}	m68k_incpc (2);
endlabel4343: ;
} /* 20 (3/2) */

/* CLR.L -(An) */
void REGPARAM2 CPUFUNC(op_42a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4344;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	SET_CZNV (FLAGVAL_Z);
	x_put_word (srca + 2, 0); x_put_word (srca, 0 >> 16);
}}}}	m68k_incpc (2);
endlabel4344: ;
} /* 22 (3/2) */

/* CLR.L (d16,An) */
void REGPARAM2 CPUFUNC(op_42a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4345;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	SET_CZNV (FLAGVAL_Z);
	x_put_word (srca + 2, 0); x_put_word (srca, 0 >> 16);
}}}}	m68k_incpc (4);
endlabel4345: ;
} /* 24 (4/2) */

/* CLR.L (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_42b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4346;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	SET_CZNV (FLAGVAL_Z);
	x_put_word (srca + 2, 0); x_put_word (srca, 0 >> 16);
}}}}	m68k_incpc (4);
endlabel4346: ;
} /* 26 (4/2) */

/* CLR.L (xxx).W */
void REGPARAM2 CPUFUNC(op_42b8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4347;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	SET_CZNV (FLAGVAL_Z);
	x_put_word (srca + 2, 0); x_put_word (srca, 0 >> 16);
}}}}	m68k_incpc (4);
endlabel4347: ;
} /* 24 (4/2) */

/* CLR.L (xxx).L */
void REGPARAM2 CPUFUNC(op_42b9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4348;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	SET_CZNV (FLAGVAL_Z);
	x_put_word (srca + 2, 0); x_put_word (srca, 0 >> 16);
}}}}	m68k_incpc (6);
endlabel4348: ;
} /* 28 (5/2) */

#endif

#ifdef PART_4
/* NEG.B Dn */
void REGPARAM2 CPUFUNC(op_4400_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((dst) & 0xff);
}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* NEG.B (An) */
void REGPARAM2 CPUFUNC(op_4410_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	x_put_byte (srca, dst);
}}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* NEG.B (An)+ */
void REGPARAM2 CPUFUNC(op_4418_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	x_put_byte (srca, dst);
}}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* NEG.B -(An) */
void REGPARAM2 CPUFUNC(op_4420_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	x_put_byte (srca, dst);
}}}}}	m68k_incpc (2);
} /* 14 (2/1) */

/* NEG.B (d16,An) */
void REGPARAM2 CPUFUNC(op_4428_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	x_put_byte (srca, dst);
}}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* NEG.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_4430_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	x_put_byte (srca, dst);
}}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* NEG.B (xxx).W */
void REGPARAM2 CPUFUNC(op_4438_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	x_put_byte (srca, dst);
}}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* NEG.B (xxx).L */
void REGPARAM2 CPUFUNC(op_4439_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 dst;
	optflag_subb (dst, (uae_s8)(src), (uae_s8)(0));
	x_put_byte (srca, dst);
}}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* NEG.W Dn */
void REGPARAM2 CPUFUNC(op_4440_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((dst) & 0xffff);
}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* NEG.W (An) */
void REGPARAM2 CPUFUNC(op_4450_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4358;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	x_put_word (srca, dst);
}}}}}}	m68k_incpc (2);
endlabel4358: ;
} /* 12 (2/1) */

/* NEG.W (An)+ */
void REGPARAM2 CPUFUNC(op_4458_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4359;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	x_put_word (srca, dst);
}}}}}}	m68k_incpc (2);
endlabel4359: ;
} /* 12 (2/1) */

/* NEG.W -(An) */
void REGPARAM2 CPUFUNC(op_4460_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4360;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	x_put_word (srca, dst);
}}}}}}	m68k_incpc (2);
endlabel4360: ;
} /* 14 (2/1) */

/* NEG.W (d16,An) */
void REGPARAM2 CPUFUNC(op_4468_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4361;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	x_put_word (srca, dst);
}}}}}}	m68k_incpc (4);
endlabel4361: ;
} /* 16 (3/1) */

/* NEG.W (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_4470_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4362;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	x_put_word (srca, dst);
}}}}}}	m68k_incpc (4);
endlabel4362: ;
} /* 18 (3/1) */

/* NEG.W (xxx).W */
void REGPARAM2 CPUFUNC(op_4478_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4363;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	x_put_word (srca, dst);
}}}}}}	m68k_incpc (4);
endlabel4363: ;
} /* 16 (3/1) */

/* NEG.W (xxx).L */
void REGPARAM2 CPUFUNC(op_4479_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4364;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 dst;
	optflag_subw (dst, (uae_s16)(src), (uae_s16)(0));
	x_put_word (srca, dst);
}}}}}}	m68k_incpc (6);
endlabel4364: ;
} /* 20 (4/1) */

/* NEG.L Dn */
void REGPARAM2 CPUFUNC(op_4480_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	m68k_dreg (regs, srcreg) = (dst);
}}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* NEG.L (An) */
void REGPARAM2 CPUFUNC(op_4490_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4366;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	x_put_word (srca + 2, dst); x_put_word (srca, dst >> 16);
}}}}}}	m68k_incpc (2);
endlabel4366: ;
} /* 20 (3/2) */

/* NEG.L (An)+ */
void REGPARAM2 CPUFUNC(op_4498_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4367;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	x_put_word (srca + 2, dst); x_put_word (srca, dst >> 16);
}}}}}}	m68k_incpc (2);
endlabel4367: ;
} /* 20 (3/2) */

/* NEG.L -(An) */
void REGPARAM2 CPUFUNC(op_44a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4368;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	x_put_word (srca + 2, dst); x_put_word (srca, dst >> 16);
}}}}}}	m68k_incpc (2);
endlabel4368: ;
} /* 22 (3/2) */

/* NEG.L (d16,An) */
void REGPARAM2 CPUFUNC(op_44a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4369;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	x_put_word (srca + 2, dst); x_put_word (srca, dst >> 16);
}}}}}}	m68k_incpc (4);
endlabel4369: ;
} /* 24 (4/2) */

/* NEG.L (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_44b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4370;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	x_put_word (srca + 2, dst); x_put_word (srca, dst >> 16);
}}}}}}	m68k_incpc (4);
endlabel4370: ;
} /* 26 (4/2) */

/* NEG.L (xxx).W */
void REGPARAM2 CPUFUNC(op_44b8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4371;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	x_put_word (srca + 2, dst); x_put_word (srca, dst >> 16);
}}}}}}	m68k_incpc (4);
endlabel4371: ;
} /* 24 (4/2) */

/* NEG.L (xxx).L */
void REGPARAM2 CPUFUNC(op_44b9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4372;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 dst;
	optflag_subl (dst, (uae_s32)(src), (uae_s32)(0));
	x_put_word (srca + 2, dst); x_put_word (srca, dst >> 16);
}}}}}}	m68k_incpc (6);
endlabel4372: ;
} /* 28 (5/2) */

/* MV2SR.B Dn */
void REGPARAM2 CPUFUNC(op_44c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	x_get_iword (4);
	do_cycles_ce000 (4);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
}}	m68k_incpc (2);
} /* 12 (2/0) */

/* MV2SR.B (An) */
void REGPARAM2 CPUFUNC(op_44d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4374;
	}
{{	uae_s16 src = x_get_word (srca);
	x_get_iword (4);
	do_cycles_ce000 (4);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
}}}}	m68k_incpc (2);
endlabel4374: ;
} /* 16 (3/0) */

/* MV2SR.B (An)+ */
void REGPARAM2 CPUFUNC(op_44d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4375;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
	x_get_iword (4);
	do_cycles_ce000 (4);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
}}}}	m68k_incpc (2);
endlabel4375: ;
} /* 16 (3/0) */

/* MV2SR.B -(An) */
void REGPARAM2 CPUFUNC(op_44e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4376;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
	x_get_iword (4);
	do_cycles_ce000 (4);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
}}}}	m68k_incpc (2);
endlabel4376: ;
} /* 18 (3/0) */

/* MV2SR.B (d16,An) */
void REGPARAM2 CPUFUNC(op_44e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4377;
	}
{{	uae_s16 src = x_get_word (srca);
	x_get_iword (6);
	do_cycles_ce000 (4);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}	m68k_incpc (4);
endlabel4377: ;
} /* 20 (4/0) */

/* MV2SR.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_44f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4378;
	}
{{	uae_s16 src = x_get_word (srca);
	x_get_iword (6);
	do_cycles_ce000 (4);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}	m68k_incpc (4);
endlabel4378: ;
} /* 22 (4/0) */

/* MV2SR.B (xxx).W */
void REGPARAM2 CPUFUNC(op_44f8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4379;
	}
{{	uae_s16 src = x_get_word (srca);
	x_get_iword (6);
	do_cycles_ce000 (4);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}	m68k_incpc (4);
endlabel4379: ;
} /* 20 (4/0) */

/* MV2SR.B (xxx).L */
void REGPARAM2 CPUFUNC(op_44f9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4380;
	}
{{	uae_s16 src = x_get_word (srca);
	x_get_iword (8);
	do_cycles_ce000 (4);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}	m68k_incpc (6);
endlabel4380: ;
} /* 24 (5/0) */

/* MV2SR.B (d16,PC) */
void REGPARAM2 CPUFUNC(op_44fa_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4381;
	}
{{	uae_s16 src = x_get_word (srca);
	x_get_iword (6);
	do_cycles_ce000 (4);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}	m68k_incpc (4);
endlabel4381: ;
} /* 20 (4/0) */

/* MV2SR.B (d8,PC,Xn) */
void REGPARAM2 CPUFUNC(op_44fb_12)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4382;
	}
{{	uae_s16 src = x_get_word (srca);
	x_get_iword (6);
	do_cycles_ce000 (4);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}	m68k_incpc (4);
endlabel4382: ;
} /* 22 (4/0) */

/* MV2SR.B #<data>.B */
void REGPARAM2 CPUFUNC(op_44fc_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
	x_get_iword (6);
	do_cycles_ce000 (4);
	MakeSR ();
	regs.sr &= 0xFF00;
	regs.sr |= src & 0xFF;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}	m68k_incpc (4);
} /* 16 (3/0) */

/* NOT.B Dn */
void REGPARAM2 CPUFUNC(op_4600_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((dst) & 0xff);
}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* NOT.B (An) */
void REGPARAM2 CPUFUNC(op_4610_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	x_put_byte (srca, dst);
}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* NOT.B (An)+ */
void REGPARAM2 CPUFUNC(op_4618_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	x_put_byte (srca, dst);
}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* NOT.B -(An) */
void REGPARAM2 CPUFUNC(op_4620_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	x_put_byte (srca, dst);
}}}}	m68k_incpc (2);
} /* 14 (2/1) */

/* NOT.B (d16,An) */
void REGPARAM2 CPUFUNC(op_4628_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	x_put_byte (srca, dst);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* NOT.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_4630_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	x_put_byte (srca, dst);
}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* NOT.B (xxx).W */
void REGPARAM2 CPUFUNC(op_4638_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	x_put_byte (srca, dst);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* NOT.B (xxx).L */
void REGPARAM2 CPUFUNC(op_4639_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	uae_u32 dst = ~src;
	optflag_testb ((uae_s8)(dst));
	x_put_byte (srca, dst);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* NOT.W Dn */
void REGPARAM2 CPUFUNC(op_4640_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((dst) & 0xffff);
}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* NOT.W (An) */
void REGPARAM2 CPUFUNC(op_4650_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4393;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	x_put_word (srca, dst);
}}}}}	m68k_incpc (2);
endlabel4393: ;
} /* 12 (2/1) */

/* NOT.W (An)+ */
void REGPARAM2 CPUFUNC(op_4658_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4394;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	x_put_word (srca, dst);
}}}}}	m68k_incpc (2);
endlabel4394: ;
} /* 12 (2/1) */

/* NOT.W -(An) */
void REGPARAM2 CPUFUNC(op_4660_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4395;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	x_put_word (srca, dst);
}}}}}	m68k_incpc (2);
endlabel4395: ;
} /* 14 (2/1) */

/* NOT.W (d16,An) */
void REGPARAM2 CPUFUNC(op_4668_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4396;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	x_put_word (srca, dst);
}}}}}	m68k_incpc (4);
endlabel4396: ;
} /* 16 (3/1) */

/* NOT.W (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_4670_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4397;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	x_put_word (srca, dst);
}}}}}	m68k_incpc (4);
endlabel4397: ;
} /* 18 (3/1) */

/* NOT.W (xxx).W */
void REGPARAM2 CPUFUNC(op_4678_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4398;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	x_put_word (srca, dst);
}}}}}	m68k_incpc (4);
endlabel4398: ;
} /* 16 (3/1) */

/* NOT.W (xxx).L */
void REGPARAM2 CPUFUNC(op_4679_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4399;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	uae_u32 dst = ~src;
	optflag_testw ((uae_s16)(dst));
	x_put_word (srca, dst);
}}}}}	m68k_incpc (6);
endlabel4399: ;
} /* 20 (4/1) */

/* NOT.L Dn */
void REGPARAM2 CPUFUNC(op_4680_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	m68k_dreg (regs, srcreg) = (dst);
}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* NOT.L (An) */
void REGPARAM2 CPUFUNC(op_4690_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4401;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	x_put_word (srca + 2, dst); x_put_word (srca, dst >> 16);
}}}}}	m68k_incpc (2);
endlabel4401: ;
} /* 20 (3/2) */

/* NOT.L (An)+ */
void REGPARAM2 CPUFUNC(op_4698_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4402;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	x_put_word (srca + 2, dst); x_put_word (srca, dst >> 16);
}}}}}	m68k_incpc (2);
endlabel4402: ;
} /* 20 (3/2) */

/* NOT.L -(An) */
void REGPARAM2 CPUFUNC(op_46a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4403;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	x_put_word (srca + 2, dst); x_put_word (srca, dst >> 16);
}}}}}	m68k_incpc (2);
endlabel4403: ;
} /* 22 (3/2) */

/* NOT.L (d16,An) */
void REGPARAM2 CPUFUNC(op_46a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4404;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	x_put_word (srca + 2, dst); x_put_word (srca, dst >> 16);
}}}}}	m68k_incpc (4);
endlabel4404: ;
} /* 24 (4/2) */

/* NOT.L (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_46b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4405;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	x_put_word (srca + 2, dst); x_put_word (srca, dst >> 16);
}}}}}	m68k_incpc (4);
endlabel4405: ;
} /* 26 (4/2) */

/* NOT.L (xxx).W */
void REGPARAM2 CPUFUNC(op_46b8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4406;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	x_put_word (srca + 2, dst); x_put_word (srca, dst >> 16);
}}}}}	m68k_incpc (4);
endlabel4406: ;
} /* 24 (4/2) */

/* NOT.L (xxx).L */
void REGPARAM2 CPUFUNC(op_46b9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4407;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	uae_u32 dst = ~src;
	optflag_testl ((uae_s32)(dst));
	x_put_word (srca + 2, dst); x_put_word (srca, dst >> 16);
}}}}}	m68k_incpc (6);
endlabel4407: ;
} /* 28 (5/2) */

/* MV2SR.W Dn */
void REGPARAM2 CPUFUNC(op_46c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel4408; }
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	x_get_iword (4);
	do_cycles_ce000 (4);
	regs.sr = src;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
}}}	m68k_incpc (2);
endlabel4408: ;
} /* 12 (2/0) */

/* MV2SR.W (An) */
void REGPARAM2 CPUFUNC(op_46d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel4409; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4409;
	}
{{	uae_s16 src = x_get_word (srca);
	x_get_iword (4);
	do_cycles_ce000 (4);
	regs.sr = src;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
}}}}}	m68k_incpc (2);
endlabel4409: ;
} /* 16 (3/0) */

/* MV2SR.W (An)+ */
void REGPARAM2 CPUFUNC(op_46d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel4410; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4410;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
	x_get_iword (4);
	do_cycles_ce000 (4);
	regs.sr = src;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
}}}}}	m68k_incpc (2);
endlabel4410: ;
} /* 16 (3/0) */

/* MV2SR.W -(An) */
void REGPARAM2 CPUFUNC(op_46e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel4411; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4411;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
	x_get_iword (4);
	do_cycles_ce000 (4);
	regs.sr = src;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
}}}}}	m68k_incpc (2);
endlabel4411: ;
} /* 18 (3/0) */

/* MV2SR.W (d16,An) */
void REGPARAM2 CPUFUNC(op_46e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel4412; }
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4412;
	}
{{	uae_s16 src = x_get_word (srca);
	x_get_iword (6);
	do_cycles_ce000 (4);
	regs.sr = src;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}}	m68k_incpc (4);
endlabel4412: ;
} /* 20 (4/0) */

/* MV2SR.W (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_46f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel4413; }
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4413;
	}
{{	uae_s16 src = x_get_word (srca);
	x_get_iword (6);
	do_cycles_ce000 (4);
	regs.sr = src;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}}	m68k_incpc (4);
endlabel4413: ;
} /* 22 (4/0) */

/* MV2SR.W (xxx).W */
void REGPARAM2 CPUFUNC(op_46f8_12)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel4414; }
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4414;
	}
{{	uae_s16 src = x_get_word (srca);
	x_get_iword (6);
	do_cycles_ce000 (4);
	regs.sr = src;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}}	m68k_incpc (4);
endlabel4414: ;
} /* 20 (4/0) */

/* MV2SR.W (xxx).L */
void REGPARAM2 CPUFUNC(op_46f9_12)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel4415; }
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4415;
	}
{{	uae_s16 src = x_get_word (srca);
	x_get_iword (8);
	do_cycles_ce000 (4);
	regs.sr = src;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}}	m68k_incpc (6);
endlabel4415: ;
} /* 24 (5/0) */

/* MV2SR.W (d16,PC) */
void REGPARAM2 CPUFUNC(op_46fa_12)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel4416; }
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4416;
	}
{{	uae_s16 src = x_get_word (srca);
	x_get_iword (6);
	do_cycles_ce000 (4);
	regs.sr = src;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}}	m68k_incpc (4);
endlabel4416: ;
} /* 20 (4/0) */

/* MV2SR.W (d8,PC,Xn) */
void REGPARAM2 CPUFUNC(op_46fb_12)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel4417; }
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4417;
	}
{{	uae_s16 src = x_get_word (srca);
	x_get_iword (6);
	do_cycles_ce000 (4);
	regs.sr = src;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}}	m68k_incpc (4);
endlabel4417: ;
} /* 22 (4/0) */

/* MV2SR.W #<data>.W */
void REGPARAM2 CPUFUNC(op_46fc_12)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel4418; }
{{	uae_s16 src = get_word_ce000_prefetch (4);
	x_get_iword (6);
	do_cycles_ce000 (4);
	regs.sr = src;
	MakeFromSR ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}	m68k_incpc (4);
endlabel4418: ;
} /* 16 (3/0) */

/* NBCD.B Dn */
void REGPARAM2 CPUFUNC(op_4800_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
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
}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* NBCD.B (An) */
void REGPARAM2 CPUFUNC(op_4810_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
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
	x_put_byte (srca, newv);
}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* NBCD.B (An)+ */
void REGPARAM2 CPUFUNC(op_4818_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
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
	x_put_byte (srca, newv);
}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* NBCD.B -(An) */
void REGPARAM2 CPUFUNC(op_4820_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
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
	x_put_byte (srca, newv);
}}}}	m68k_incpc (2);
} /* 14 (2/1) */

/* NBCD.B (d16,An) */
void REGPARAM2 CPUFUNC(op_4828_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
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
	x_put_byte (srca, newv);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* NBCD.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_4830_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
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
	x_put_byte (srca, newv);
}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* NBCD.B (xxx).W */
void REGPARAM2 CPUFUNC(op_4838_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
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
	x_put_byte (srca, newv);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* NBCD.B (xxx).L */
void REGPARAM2 CPUFUNC(op_4839_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
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
	x_put_byte (srca, newv);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* SWAP.W Dn */
void REGPARAM2 CPUFUNC(op_4840_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 dst = ((src >> 16)&0xFFFF) | ((src&0xFFFF)<<16);
	optflag_testl ((uae_s32)(dst));
	m68k_dreg (regs, srcreg) = (dst);
}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* PEA.L (An) */
void REGPARAM2 CPUFUNC(op_4850_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4428;
	}
{	m68k_areg (regs, 7) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta, srca >> 16); x_put_word (dsta + 2, srca);
}}}}	m68k_incpc (2);
endlabel4428: ;
} /* 12 (1/2) */

/* PEA.L (d16,An) */
void REGPARAM2 CPUFUNC(op_4868_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4429;
	}
{	m68k_areg (regs, 7) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, srca >> 16); x_put_word (dsta + 2, srca);
}}}}	m68k_incpc (4);
endlabel4429: ;
} /* 16 (2/2) */

/* PEA.L (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_4870_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4430;
	}
{	m68k_areg (regs, 7) = dsta;
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
	x_put_word (dsta, srca >> 16); x_put_word (dsta + 2, srca);
}}}}	m68k_incpc (4);
endlabel4430: ;
} /* 20 (2/2) */

/* PEA.L (xxx).W */
void REGPARAM2 CPUFUNC(op_4878_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4431;
	}
{	m68k_areg (regs, 7) = dsta;
	x_put_word (dsta, srca >> 16); x_put_word (dsta + 2, srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}	m68k_incpc (4);
endlabel4431: ;
} /* 16 (2/2) */

/* PEA.L (xxx).L */
void REGPARAM2 CPUFUNC(op_4879_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	if (dsta & 1) {
	m68k_incpc (4);
		exception3 (opcode, dsta);
		goto endlabel4432;
	}
{	m68k_areg (regs, 7) = dsta;
	x_put_word (dsta, srca >> 16); x_put_word (dsta + 2, srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}	m68k_incpc (6);
endlabel4432: ;
} /* 20 (3/2) */

/* PEA.L (d16,PC) */
void REGPARAM2 CPUFUNC(op_487a_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4433;
	}
{	m68k_areg (regs, 7) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, srca >> 16); x_put_word (dsta + 2, srca);
}}}}	m68k_incpc (4);
endlabel4433: ;
} /* 16 (2/2) */

/* PEA.L (d8,PC,Xn) */
void REGPARAM2 CPUFUNC(op_487b_12)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
{	uaecptr dsta;
	dsta = m68k_areg (regs, 7) - 4;
	if (dsta & 1) {
	m68k_incpc (2);
		exception3 (opcode, dsta);
		goto endlabel4434;
	}
{	m68k_areg (regs, 7) = dsta;
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
	x_put_word (dsta, srca >> 16); x_put_word (dsta + 2, srca);
}}}}	m68k_incpc (4);
endlabel4434: ;
} /* 20 (2/2) */

/* EXT.W Dn */
void REGPARAM2 CPUFUNC(op_4880_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u16 dst = (uae_s16)(uae_s8)src;
	optflag_testw ((uae_s16)(dst));
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | ((dst) & 0xffff);
}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* MVMLE.W #<data>.W,(An) */
void REGPARAM2 CPUFUNC(op_4890_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_word_ce000_prefetch (4);
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4436;
	}
{{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { x_put_word (srca, m68k_dreg (regs, movem_index1[dmask])); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { x_put_word (srca, m68k_areg (regs, movem_index1[amask])); srca += 2; amask = movem_next[amask]; }
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}	m68k_incpc (4);
endlabel4436: ;
} /* 8+ (2/0) */

/* MVMLE.W #<data>.W,-(An) */
void REGPARAM2 CPUFUNC(op_48a0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_word_ce000_prefetch (4);
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) - 0;
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4437;
	}
{{	uae_u16 amask = mask & 0xff, dmask = (mask >> 8) & 0xff;
	while (amask) { srca -= 2; x_put_word (srca, m68k_areg (regs, movem_index2[amask])); amask = movem_next[amask]; }
	while (dmask) { srca -= 2; x_put_word (srca, m68k_dreg (regs, movem_index2[dmask])); dmask = movem_next[dmask]; }
	m68k_areg (regs, dstreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}	m68k_incpc (4);
endlabel4437: ;
} /* 8+ (2/0) */

/* MVMLE.W #<data>.W,(d16,An) */
void REGPARAM2 CPUFUNC(op_48a8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_word_ce000_prefetch (4);
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4438;
	}
{{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { x_put_word (srca, m68k_dreg (regs, movem_index1[dmask])); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { x_put_word (srca, m68k_areg (regs, movem_index1[amask])); srca += 2; amask = movem_next[amask]; }
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}	m68k_incpc (6);
endlabel4438: ;
} /* 12+ (3/0) */

/* MVMLE.W #<data>.W,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_48b0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_word_ce000_prefetch (4);
{	uaecptr srca;
	srca = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4439;
	}
{	do_cycles_ce000 (2);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { x_put_word (srca, m68k_dreg (regs, movem_index1[dmask])); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { x_put_word (srca, m68k_areg (regs, movem_index1[amask])); srca += 2; amask = movem_next[amask]; }
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}	m68k_incpc (6);
endlabel4439: ;
} /* 14+ (3/0) */

/* MVMLE.W #<data>.W,(xxx).W */
void REGPARAM2 CPUFUNC(op_48b8_12)(uae_u32 opcode)
{
{	uae_u16 mask = get_word_ce000_prefetch (4);
{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4440;
	}
{{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { x_put_word (srca, m68k_dreg (regs, movem_index1[dmask])); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { x_put_word (srca, m68k_areg (regs, movem_index1[amask])); srca += 2; amask = movem_next[amask]; }
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}	m68k_incpc (6);
endlabel4440: ;
} /* 12+ (3/0) */

/* MVMLE.W #<data>.W,(xxx).L */
void REGPARAM2 CPUFUNC(op_48b9_12)(uae_u32 opcode)
{
{	uae_u16 mask = get_word_ce000_prefetch (4);
{	uaecptr srca;
	srca = get_word_ce000_prefetch (6) << 16;
	srca |= get_word_ce000_prefetch (8);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4441;
	}
{{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { x_put_word (srca, m68k_dreg (regs, movem_index1[dmask])); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { x_put_word (srca, m68k_areg (regs, movem_index1[amask])); srca += 2; amask = movem_next[amask]; }
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
}}}}	m68k_incpc (8);
endlabel4441: ;
} /* 16+ (4/0) */

/* EXT.L Dn */
void REGPARAM2 CPUFUNC(op_48c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 dst = (uae_s32)(uae_s16)src;
	optflag_testl ((uae_s32)(dst));
	m68k_dreg (regs, srcreg) = (dst);
}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* MVMLE.L #<data>.W,(An) */
void REGPARAM2 CPUFUNC(op_48d0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_word_ce000_prefetch (4);
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4443;
	}
{{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { x_put_word (srca, m68k_dreg (regs, movem_index1[dmask]) >> 16); x_put_word (srca + 2, m68k_dreg (regs, movem_index1[dmask])); srca += 4; dmask = movem_next[dmask]; }
	while (amask) { x_put_word (srca, m68k_areg (regs, movem_index1[amask]) >> 16); x_put_word (srca + 2, m68k_areg (regs, movem_index1[amask])); srca += 4; amask = movem_next[amask]; }
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}	m68k_incpc (4);
endlabel4443: ;
} /* 8+ (2/0) */

/* MVMLE.L #<data>.W,-(An) */
void REGPARAM2 CPUFUNC(op_48e0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_word_ce000_prefetch (4);
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) - 0;
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4444;
	}
{{	uae_u16 amask = mask & 0xff, dmask = (mask >> 8) & 0xff;
	while (amask) { srca -= 4; x_put_word (srca, m68k_areg (regs, movem_index2[amask]) >> 16); x_put_word (srca + 2, m68k_areg (regs, movem_index2[amask])); amask = movem_next[amask]; }
	while (dmask) { srca -= 4; x_put_word (srca, m68k_dreg (regs, movem_index2[dmask]) >> 16); x_put_word (srca + 2, m68k_dreg (regs, movem_index2[dmask])); dmask = movem_next[dmask]; }
	m68k_areg (regs, dstreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}	m68k_incpc (4);
endlabel4444: ;
} /* 8+ (2/0) */

/* MVMLE.L #<data>.W,(d16,An) */
void REGPARAM2 CPUFUNC(op_48e8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_word_ce000_prefetch (4);
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4445;
	}
{{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { x_put_word (srca, m68k_dreg (regs, movem_index1[dmask]) >> 16); x_put_word (srca + 2, m68k_dreg (regs, movem_index1[dmask])); srca += 4; dmask = movem_next[dmask]; }
	while (amask) { x_put_word (srca, m68k_areg (regs, movem_index1[amask]) >> 16); x_put_word (srca + 2, m68k_areg (regs, movem_index1[amask])); srca += 4; amask = movem_next[amask]; }
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}	m68k_incpc (6);
endlabel4445: ;
} /* 12+ (3/0) */

/* MVMLE.L #<data>.W,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_48f0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_word_ce000_prefetch (4);
{	uaecptr srca;
	srca = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4446;
	}
{	do_cycles_ce000 (2);
{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { x_put_word (srca, m68k_dreg (regs, movem_index1[dmask]) >> 16); x_put_word (srca + 2, m68k_dreg (regs, movem_index1[dmask])); srca += 4; dmask = movem_next[dmask]; }
	while (amask) { x_put_word (srca, m68k_areg (regs, movem_index1[amask]) >> 16); x_put_word (srca + 2, m68k_areg (regs, movem_index1[amask])); srca += 4; amask = movem_next[amask]; }
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}	m68k_incpc (6);
endlabel4446: ;
} /* 14+ (3/0) */

/* MVMLE.L #<data>.W,(xxx).W */
void REGPARAM2 CPUFUNC(op_48f8_12)(uae_u32 opcode)
{
{	uae_u16 mask = get_word_ce000_prefetch (4);
{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4447;
	}
{{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { x_put_word (srca, m68k_dreg (regs, movem_index1[dmask]) >> 16); x_put_word (srca + 2, m68k_dreg (regs, movem_index1[dmask])); srca += 4; dmask = movem_next[dmask]; }
	while (amask) { x_put_word (srca, m68k_areg (regs, movem_index1[amask]) >> 16); x_put_word (srca + 2, m68k_areg (regs, movem_index1[amask])); srca += 4; amask = movem_next[amask]; }
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}	m68k_incpc (6);
endlabel4447: ;
} /* 12+ (3/0) */

/* MVMLE.L #<data>.W,(xxx).L */
void REGPARAM2 CPUFUNC(op_48f9_12)(uae_u32 opcode)
{
{	uae_u16 mask = get_word_ce000_prefetch (4);
{	uaecptr srca;
	srca = get_word_ce000_prefetch (6) << 16;
	srca |= get_word_ce000_prefetch (8);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4448;
	}
{{	uae_u16 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	while (dmask) { x_put_word (srca, m68k_dreg (regs, movem_index1[dmask]) >> 16); x_put_word (srca + 2, m68k_dreg (regs, movem_index1[dmask])); srca += 4; dmask = movem_next[dmask]; }
	while (amask) { x_put_word (srca, m68k_areg (regs, movem_index1[amask]) >> 16); x_put_word (srca + 2, m68k_areg (regs, movem_index1[amask])); srca += 4; amask = movem_next[amask]; }
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
}}}}	m68k_incpc (8);
endlabel4448: ;
} /* 16+ (4/0) */

/* TST.B Dn */
void REGPARAM2 CPUFUNC(op_4a00_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testb ((uae_s8)(src));
}}	m68k_incpc (2);
} /* 4 (1/0) */

/* TST.B (An) */
void REGPARAM2 CPUFUNC(op_4a10_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpc (2);
} /* 8 (2/0) */

/* TST.B (An)+ */
void REGPARAM2 CPUFUNC(op_4a18_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpc (2);
} /* 8 (2/0) */

/* TST.B -(An) */
void REGPARAM2 CPUFUNC(op_4a20_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpc (2);
} /* 10 (2/0) */

/* TST.B (d16,An) */
void REGPARAM2 CPUFUNC(op_4a28_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* TST.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_4a30_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpc (4);
} /* 14 (3/0) */

/* TST.B (xxx).W */
void REGPARAM2 CPUFUNC(op_4a38_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* TST.B (xxx).L */
void REGPARAM2 CPUFUNC(op_4a39_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	optflag_testb ((uae_s8)(src));
}}}	m68k_incpc (6);
} /* 16 (4/0) */

/* TST.B #<data>.B */
void REGPARAM2 CPUFUNC(op_4a3c_12)(uae_u32 opcode)
{
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testb ((uae_s8)(src));
}}	m68k_incpc (4);
} /* 8 (2/0) */

/* TST.W Dn */
void REGPARAM2 CPUFUNC(op_4a40_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testw ((uae_s16)(src));
}}	m68k_incpc (2);
} /* 4 (1/0) */

/* TST.W (An) */
void REGPARAM2 CPUFUNC(op_4a50_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4459;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testw ((uae_s16)(src));
}}}}	m68k_incpc (2);
endlabel4459: ;
} /* 8 (2/0) */

/* TST.W (An)+ */
void REGPARAM2 CPUFUNC(op_4a58_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4460;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testw ((uae_s16)(src));
}}}}	m68k_incpc (2);
endlabel4460: ;
} /* 8 (2/0) */

/* TST.W -(An) */
void REGPARAM2 CPUFUNC(op_4a60_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4461;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testw ((uae_s16)(src));
}}}}	m68k_incpc (2);
endlabel4461: ;
} /* 10 (2/0) */

/* TST.W (d16,An) */
void REGPARAM2 CPUFUNC(op_4a68_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4462;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testw ((uae_s16)(src));
}}}}	m68k_incpc (4);
endlabel4462: ;
} /* 12 (3/0) */

/* TST.W (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_4a70_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4463;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testw ((uae_s16)(src));
}}}}	m68k_incpc (4);
endlabel4463: ;
} /* 14 (3/0) */

/* TST.W (xxx).W */
void REGPARAM2 CPUFUNC(op_4a78_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4464;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testw ((uae_s16)(src));
}}}}	m68k_incpc (4);
endlabel4464: ;
} /* 12 (3/0) */

/* TST.W (xxx).L */
void REGPARAM2 CPUFUNC(op_4a79_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4465;
	}
{{	uae_s16 src = x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	optflag_testw ((uae_s16)(src));
}}}}	m68k_incpc (6);
endlabel4465: ;
} /* 16 (4/0) */

/* TST.W #<data>.W */
void REGPARAM2 CPUFUNC(op_4a7c_12)(uae_u32 opcode)
{
{{	uae_s16 src = get_word_ce000_prefetch (4);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testw ((uae_s16)(src));
}}	m68k_incpc (4);
} /* 8 (2/0) */

/* TST.L Dn */
void REGPARAM2 CPUFUNC(op_4a80_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testl ((uae_s32)(src));
}}	m68k_incpc (2);
} /* 4 (1/0) */

/* TST.L (An) */
void REGPARAM2 CPUFUNC(op_4a90_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4468;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testl ((uae_s32)(src));
}}}}	m68k_incpc (2);
endlabel4468: ;
} /* 12 (3/0) */

/* TST.L (An)+ */
void REGPARAM2 CPUFUNC(op_4a98_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4469;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testl ((uae_s32)(src));
}}}}	m68k_incpc (2);
endlabel4469: ;
} /* 12 (3/0) */

/* TST.L -(An) */
void REGPARAM2 CPUFUNC(op_4aa0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4470;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	optflag_testl ((uae_s32)(src));
}}}}	m68k_incpc (2);
endlabel4470: ;
} /* 14 (3/0) */

/* TST.L (d16,An) */
void REGPARAM2 CPUFUNC(op_4aa8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4471;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testl ((uae_s32)(src));
}}}}	m68k_incpc (4);
endlabel4471: ;
} /* 16 (4/0) */

/* TST.L (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_4ab0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4472;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testl ((uae_s32)(src));
}}}}	m68k_incpc (4);
endlabel4472: ;
} /* 18 (4/0) */

/* TST.L (xxx).W */
void REGPARAM2 CPUFUNC(op_4ab8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4473;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	optflag_testl ((uae_s32)(src));
}}}}	m68k_incpc (4);
endlabel4473: ;
} /* 16 (4/0) */

/* TST.L (xxx).L */
void REGPARAM2 CPUFUNC(op_4ab9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4474;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	optflag_testl ((uae_s32)(src));
}}}}	m68k_incpc (6);
endlabel4474: ;
} /* 20 (5/0) */

/* TST.L #<data>.L */
void REGPARAM2 CPUFUNC(op_4abc_12)(uae_u32 opcode)
{
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	optflag_testl ((uae_s32)(src));
}}	m68k_incpc (6);
} /* 12 (3/0) */

/* TAS.B Dn */
void REGPARAM2 CPUFUNC(op_4ac0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	src |= 0x80;
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((src) & 0xff);
}}	m68k_incpc (2);
} /* 4 (1/0) */

/* TAS.B (An) */
void REGPARAM2 CPUFUNC(op_4ad0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	optflag_testb ((uae_s8)(src));
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	src |= 0x80;
	if (!is_cycle_ce ()) {
	x_put_byte (srca, src);
	} else {
		do_cycles_ce000 (4);
	}
}}}	m68k_incpc (2);
} /* 14 (2/1) */

/* TAS.B (An)+ */
void REGPARAM2 CPUFUNC(op_4ad8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
	optflag_testb ((uae_s8)(src));
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	src |= 0x80;
	if (!is_cycle_ce ()) {
	x_put_byte (srca, src);
	} else {
		do_cycles_ce000 (4);
	}
}}}	m68k_incpc (2);
} /* 14 (2/1) */

/* TAS.B -(An) */
void REGPARAM2 CPUFUNC(op_4ae0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
	optflag_testb ((uae_s8)(src));
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	src |= 0x80;
	if (!is_cycle_ce ()) {
	x_put_byte (srca, src);
	} else {
		do_cycles_ce000 (4);
	}
}}}	m68k_incpc (2);
} /* 16 (2/1) */

/* TAS.B (d16,An) */
void REGPARAM2 CPUFUNC(op_4ae8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
	optflag_testb ((uae_s8)(src));
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	src |= 0x80;
	if (!is_cycle_ce ()) {
	x_put_byte (srca, src);
	} else {
		do_cycles_ce000 (4);
	}
}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* TAS.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_4af0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
	optflag_testb ((uae_s8)(src));
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	src |= 0x80;
	if (!is_cycle_ce ()) {
	x_put_byte (srca, src);
	} else {
		do_cycles_ce000 (4);
	}
}}}	m68k_incpc (4);
} /* 20 (3/1) */

/* TAS.B (xxx).W */
void REGPARAM2 CPUFUNC(op_4af8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
	optflag_testb ((uae_s8)(src));
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	src |= 0x80;
	if (!is_cycle_ce ()) {
	x_put_byte (srca, src);
	} else {
		do_cycles_ce000 (4);
	}
}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* TAS.B (xxx).L */
void REGPARAM2 CPUFUNC(op_4af9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
	optflag_testb ((uae_s8)(src));
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	src |= 0x80;
	if (!is_cycle_ce ()) {
	x_put_byte (srca, src);
	} else {
		do_cycles_ce000 (4);
	}
}}}	m68k_incpc (6);
} /* 22 (4/1) */

/* MVMEL.W #<data>.W,(An) */
void REGPARAM2 CPUFUNC(op_4c90_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_word_ce000_prefetch (4);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	uae_u32 v;
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4484;
	}
{{	while (dmask) { m68k_dreg (regs, movem_index1[dmask]) = (uae_s32)(uae_s16)x_get_word (srca); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg (regs, movem_index1[amask]) = (uae_s32)(uae_s16)x_get_word (srca); srca += 2; amask = movem_next[amask]; }
	x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}	m68k_incpc (4);
endlabel4484: ;
} /* 12+ (3/0) */

/* MVMEL.W #<data>.W,(An)+ */
void REGPARAM2 CPUFUNC(op_4c98_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_word_ce000_prefetch (4);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	uae_u32 v;
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4485;
	}
{{	while (dmask) { m68k_dreg (regs, movem_index1[dmask]) = (uae_s32)(uae_s16)x_get_word (srca); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg (regs, movem_index1[amask]) = (uae_s32)(uae_s16)x_get_word (srca); srca += 2; amask = movem_next[amask]; }
	x_get_word (srca);
	m68k_areg (regs, dstreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}	m68k_incpc (4);
endlabel4485: ;
} /* 12+ (3/0) */

/* MVMEL.W #<data>.W,(d16,An) */
void REGPARAM2 CPUFUNC(op_4ca8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_word_ce000_prefetch (4);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	uae_u32 v;
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4486;
	}
{{	while (dmask) { m68k_dreg (regs, movem_index1[dmask]) = (uae_s32)(uae_s16)x_get_word (srca); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg (regs, movem_index1[amask]) = (uae_s32)(uae_s16)x_get_word (srca); srca += 2; amask = movem_next[amask]; }
	x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}	m68k_incpc (6);
endlabel4486: ;
} /* 16+ (4/0) */

/* MVMEL.W #<data>.W,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_4cb0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_word_ce000_prefetch (4);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	uae_u32 v;
{	uaecptr srca;
	srca = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4487;
	}
{	do_cycles_ce000 (2);
{	while (dmask) { m68k_dreg (regs, movem_index1[dmask]) = (uae_s32)(uae_s16)x_get_word (srca); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg (regs, movem_index1[amask]) = (uae_s32)(uae_s16)x_get_word (srca); srca += 2; amask = movem_next[amask]; }
	x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}	m68k_incpc (6);
endlabel4487: ;
} /* 18+ (4/0) */

/* MVMEL.W #<data>.W,(xxx).W */
void REGPARAM2 CPUFUNC(op_4cb8_12)(uae_u32 opcode)
{
{	uae_u16 mask = get_word_ce000_prefetch (4);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	uae_u32 v;
{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4488;
	}
{{	while (dmask) { m68k_dreg (regs, movem_index1[dmask]) = (uae_s32)(uae_s16)x_get_word (srca); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg (regs, movem_index1[amask]) = (uae_s32)(uae_s16)x_get_word (srca); srca += 2; amask = movem_next[amask]; }
	x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}	m68k_incpc (6);
endlabel4488: ;
} /* 16+ (4/0) */

/* MVMEL.W #<data>.W,(xxx).L */
void REGPARAM2 CPUFUNC(op_4cb9_12)(uae_u32 opcode)
{
{	uae_u16 mask = get_word_ce000_prefetch (4);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	uae_u32 v;
{	uaecptr srca;
	srca = get_word_ce000_prefetch (6) << 16;
	srca |= get_word_ce000_prefetch (8);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4489;
	}
{{	while (dmask) { m68k_dreg (regs, movem_index1[dmask]) = (uae_s32)(uae_s16)x_get_word (srca); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg (regs, movem_index1[amask]) = (uae_s32)(uae_s16)x_get_word (srca); srca += 2; amask = movem_next[amask]; }
	x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
}}}}	m68k_incpc (8);
endlabel4489: ;
} /* 20+ (5/0) */

/* MVMEL.W #<data>.W,(d16,PC) */
void REGPARAM2 CPUFUNC(op_4cba_12)(uae_u32 opcode)
{
{	uae_u16 mask = get_word_ce000_prefetch (4);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	uae_u32 v;
{	uaecptr srca;
	srca = m68k_getpc () + 4;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4490;
	}
{{	while (dmask) { m68k_dreg (regs, movem_index1[dmask]) = (uae_s32)(uae_s16)x_get_word (srca); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg (regs, movem_index1[amask]) = (uae_s32)(uae_s16)x_get_word (srca); srca += 2; amask = movem_next[amask]; }
	x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}	m68k_incpc (6);
endlabel4490: ;
} /* 16+ (4/0) */

/* MVMEL.W #<data>.W,(d8,PC,Xn) */
void REGPARAM2 CPUFUNC(op_4cbb_12)(uae_u32 opcode)
{
{	uae_u16 mask = get_word_ce000_prefetch (4);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	uae_u32 v;
{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 4;
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (6));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4491;
	}
{	do_cycles_ce000 (2);
{	while (dmask) { m68k_dreg (regs, movem_index1[dmask]) = (uae_s32)(uae_s16)x_get_word (srca); srca += 2; dmask = movem_next[dmask]; }
	while (amask) { m68k_areg (regs, movem_index1[amask]) = (uae_s32)(uae_s16)x_get_word (srca); srca += 2; amask = movem_next[amask]; }
	x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}	m68k_incpc (6);
endlabel4491: ;
} /* 18+ (4/0) */

/* MVMEL.L #<data>.W,(An) */
void REGPARAM2 CPUFUNC(op_4cd0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_word_ce000_prefetch (4);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	uae_u32 v;
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4492;
	}
{{	while (dmask) { v = x_get_word (srca) << 16; v |= x_get_word (srca + 2); m68k_dreg (regs, movem_index1[dmask]) = v; srca += 4; dmask = movem_next[dmask]; }
	while (amask) { v = x_get_word (srca) << 16; v |= x_get_word (srca + 2); m68k_areg (regs, movem_index1[amask]) = v; srca += 4; amask = movem_next[amask]; }
	x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}	m68k_incpc (4);
endlabel4492: ;
} /* 12+ (3/0) */

/* MVMEL.L #<data>.W,(An)+ */
void REGPARAM2 CPUFUNC(op_4cd8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_word_ce000_prefetch (4);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	uae_u32 v;
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4493;
	}
{{	while (dmask) { v = x_get_word (srca) << 16; v |= x_get_word (srca + 2); m68k_dreg (regs, movem_index1[dmask]) = v; srca += 4; dmask = movem_next[dmask]; }
	while (amask) { v = x_get_word (srca) << 16; v |= x_get_word (srca + 2); m68k_areg (regs, movem_index1[amask]) = v; srca += 4; amask = movem_next[amask]; }
	x_get_word (srca);
	m68k_areg (regs, dstreg) = srca;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}	m68k_incpc (4);
endlabel4493: ;
} /* 12+ (3/0) */

/* MVMEL.L #<data>.W,(d16,An) */
void REGPARAM2 CPUFUNC(op_4ce8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_word_ce000_prefetch (4);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	uae_u32 v;
{	uaecptr srca;
	srca = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4494;
	}
{{	while (dmask) { v = x_get_word (srca) << 16; v |= x_get_word (srca + 2); m68k_dreg (regs, movem_index1[dmask]) = v; srca += 4; dmask = movem_next[dmask]; }
	while (amask) { v = x_get_word (srca) << 16; v |= x_get_word (srca + 2); m68k_areg (regs, movem_index1[amask]) = v; srca += 4; amask = movem_next[amask]; }
	x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}	m68k_incpc (6);
endlabel4494: ;
} /* 16+ (4/0) */

/* MVMEL.L #<data>.W,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_4cf0_12)(uae_u32 opcode)
{
	uae_u32 dstreg = opcode & 7;
{	uae_u16 mask = get_word_ce000_prefetch (4);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	uae_u32 v;
{	uaecptr srca;
	srca = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (6));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4495;
	}
{	do_cycles_ce000 (2);
{	while (dmask) { v = x_get_word (srca) << 16; v |= x_get_word (srca + 2); m68k_dreg (regs, movem_index1[dmask]) = v; srca += 4; dmask = movem_next[dmask]; }
	while (amask) { v = x_get_word (srca) << 16; v |= x_get_word (srca + 2); m68k_areg (regs, movem_index1[amask]) = v; srca += 4; amask = movem_next[amask]; }
	x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}	m68k_incpc (6);
endlabel4495: ;
} /* 18+ (4/0) */

/* MVMEL.L #<data>.W,(xxx).W */
void REGPARAM2 CPUFUNC(op_4cf8_12)(uae_u32 opcode)
{
{	uae_u16 mask = get_word_ce000_prefetch (4);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	uae_u32 v;
{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4496;
	}
{{	while (dmask) { v = x_get_word (srca) << 16; v |= x_get_word (srca + 2); m68k_dreg (regs, movem_index1[dmask]) = v; srca += 4; dmask = movem_next[dmask]; }
	while (amask) { v = x_get_word (srca) << 16; v |= x_get_word (srca + 2); m68k_areg (regs, movem_index1[amask]) = v; srca += 4; amask = movem_next[amask]; }
	x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}	m68k_incpc (6);
endlabel4496: ;
} /* 16+ (4/0) */

/* MVMEL.L #<data>.W,(xxx).L */
void REGPARAM2 CPUFUNC(op_4cf9_12)(uae_u32 opcode)
{
{	uae_u16 mask = get_word_ce000_prefetch (4);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	uae_u32 v;
{	uaecptr srca;
	srca = get_word_ce000_prefetch (6) << 16;
	srca |= get_word_ce000_prefetch (8);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4497;
	}
{{	while (dmask) { v = x_get_word (srca) << 16; v |= x_get_word (srca + 2); m68k_dreg (regs, movem_index1[dmask]) = v; srca += 4; dmask = movem_next[dmask]; }
	while (amask) { v = x_get_word (srca) << 16; v |= x_get_word (srca + 2); m68k_areg (regs, movem_index1[amask]) = v; srca += 4; amask = movem_next[amask]; }
	x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (10);
}}}}	m68k_incpc (8);
endlabel4497: ;
} /* 20+ (5/0) */

/* MVMEL.L #<data>.W,(d16,PC) */
void REGPARAM2 CPUFUNC(op_4cfa_12)(uae_u32 opcode)
{
{	uae_u16 mask = get_word_ce000_prefetch (4);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	uae_u32 v;
{	uaecptr srca;
	srca = m68k_getpc () + 4;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (6);
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4498;
	}
{{	while (dmask) { v = x_get_word (srca) << 16; v |= x_get_word (srca + 2); m68k_dreg (regs, movem_index1[dmask]) = v; srca += 4; dmask = movem_next[dmask]; }
	while (amask) { v = x_get_word (srca) << 16; v |= x_get_word (srca + 2); m68k_areg (regs, movem_index1[amask]) = v; srca += 4; amask = movem_next[amask]; }
	x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}	m68k_incpc (6);
endlabel4498: ;
} /* 16+ (4/0) */

/* MVMEL.L #<data>.W,(d8,PC,Xn) */
void REGPARAM2 CPUFUNC(op_4cfb_12)(uae_u32 opcode)
{
{	uae_u16 mask = get_word_ce000_prefetch (4);
	uae_u32 dmask = mask & 0xff, amask = (mask >> 8) & 0xff;
	uae_u32 v;
{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 4;
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (6));
	if (srca & 1) {
	m68k_incpc (2);
		exception3 (opcode, srca);
		goto endlabel4499;
	}
{	do_cycles_ce000 (2);
{	while (dmask) { v = x_get_word (srca) << 16; v |= x_get_word (srca + 2); m68k_dreg (regs, movem_index1[dmask]) = v; srca += 4; dmask = movem_next[dmask]; }
	while (amask) { v = x_get_word (srca) << 16; v |= x_get_word (srca + 2); m68k_areg (regs, movem_index1[amask]) = v; srca += 4; amask = movem_next[amask]; }
	x_get_word (srca);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
}}}}	m68k_incpc (6);
endlabel4499: ;
} /* 18+ (4/0) */

/* TRAPQ.L #<data> */
void REGPARAM2 CPUFUNC(op_4e40_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 15);
{{	uae_u32 src = srcreg;
	m68k_incpc (2);
	Exception (src + 32);
}}} /* 4 (0/0) */

/* LINK.W An,#<data>.W */
void REGPARAM2 CPUFUNC(op_4e50_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr olda;
	olda = m68k_areg (regs, 7) - 4;
	if (olda & 1) {
		exception3 (opcode, olda);
		goto endlabel4501;
	}
{	m68k_areg (regs, 7) = olda;
{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s16 offs = get_word_ce000_prefetch (4);
	x_put_word (olda, src >> 16); x_put_word (olda + 2, src);
	m68k_areg (regs, srcreg) = (m68k_areg (regs, 7));
	m68k_areg (regs, 7) += offs;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
}}}}}	m68k_incpc (4);
endlabel4501: ;
} /* 16 (2/2) */

/* UNLK.L An */
void REGPARAM2 CPUFUNC(op_4e58_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s32 src = m68k_areg (regs, srcreg);
	m68k_areg (regs, 7) = src;
{	uaecptr olda;
	olda = m68k_areg (regs, 7);
	if (olda & 1) {
		exception3 (opcode, olda);
		goto endlabel4502;
	}
{{	uae_s32 old = x_get_word (olda) << 16; old |= x_get_word (olda + 2);
	m68k_areg (regs, 7) += 4;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_areg (regs, srcreg) = (old);
}}}}}	m68k_incpc (2);
endlabel4502: ;
} /* 12 (3/0) */

/* MVR2USP.L An */
void REGPARAM2 CPUFUNC(op_4e60_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel4503; }
{{	uae_s32 src = m68k_areg (regs, srcreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	regs.usp = src;
}}}	m68k_incpc (2);
endlabel4503: ;
} /* 4 (1/0) */

/* MVUSP2R.L An */
void REGPARAM2 CPUFUNC(op_4e68_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{if (!regs.s) { Exception (8); goto endlabel4504; }
{{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_areg (regs, srcreg) = (regs.usp);
}}}	m68k_incpc (2);
endlabel4504: ;
} /* 4 (1/0) */

/* RESET.L  */
void REGPARAM2 CPUFUNC(op_4e70_12)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel4505; }
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	cpureset ();
	m68k_incpc (2);
	do_cycles_ce000 (128);
	get_word_ce000_prefetch (2);
}}endlabel4505: ;
} /* 132 (1/0) */

/* NOP.L  */
void REGPARAM2 CPUFUNC(op_4e71_12)(uae_u32 opcode)
{
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
}	m68k_incpc (2);
} /* 4 (1/0) */

/* STOP.L #<data>.W */
void REGPARAM2 CPUFUNC(op_4e72_12)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel4507; }
{	regs.sr = regs.irc;
	MakeFromSR ();
	m68k_setstopped ();
	m68k_incpc (4);
}}endlabel4507: ;
} /* 4 (0/0) */

/* RTE.L  */
void REGPARAM2 CPUFUNC(op_4e73_12)(uae_u32 opcode)
{
{if (!regs.s) { Exception (8); goto endlabel4508; }
{{	uaecptr sra;
	sra = m68k_areg (regs, 7);
	if (sra & 1) {
		exception3 (opcode, sra);
		goto endlabel4508;
	}
{{	uae_s16 sr = x_get_word (sra);
	m68k_areg (regs, 7) += 2;
{	uaecptr pca;
	pca = m68k_areg (regs, 7);
	if (pca & 1) {
		exception3 (opcode, pca);
		goto endlabel4508;
	}
{{	uae_s32 pc = x_get_word (pca) << 16; pc |= x_get_word (pca + 2);
	m68k_areg (regs, 7) += 4;
	regs.sr = sr;
	m68k_setpc (pc);
	MakeFromSR ();
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}}}endlabel4508: ;
} /* 20 (5/0) */

/* RTD.L #<data>.W */
void REGPARAM2 CPUFUNC(op_4e74_12)(uae_u32 opcode)
{
{{	uaecptr pca;
	pca = m68k_areg (regs, 7);
	if (pca & 1) {
		exception3 (opcode, pca);
		goto endlabel4509;
	}
{{	uae_s32 pc = x_get_word (pca) << 16; pc |= x_get_word (pca + 2);
	m68k_areg (regs, 7) += 4;
{	uae_s16 offs = get_word_ce000_prefetch (4);
	m68k_areg (regs, 7) += offs;
	if (pc & 1) {
		exception3i (0x4E74, pc);
		goto endlabel4509;
	}
	if (pc & 1) {
		exception3i (0x4E74, pc);
		goto endlabel4509;
	}
	m68k_setpc (pc);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}endlabel4509: ;
} /* 20 (5/0) */

/* RTS.L  */
void REGPARAM2 CPUFUNC(op_4e75_12)(uae_u32 opcode)
{
{	uaecptr pc = m68k_getpc ();
	m68k_do_rts_ce ();
	if (m68k_getpc () & 1) {
		uaecptr faultpc = m68k_getpc ();
	m68k_setpc (pc);
		exception3i (0x4E75, faultpc);
	}
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}} /* 16 (4/0) */

/* TRAPV.L  */
void REGPARAM2 CPUFUNC(op_4e76_12)(uae_u32 opcode)
{
{	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	if (GET_VFLG ()) {
		Exception (7);
		goto endlabel4511;
	}
}endlabel4511: ;
} /* 4 (1/0) */

/* RTR.L  */
void REGPARAM2 CPUFUNC(op_4e77_12)(uae_u32 opcode)
{
{	uaecptr oldpc = m68k_getpc ();
	MakeSR ();
{	uaecptr sra;
	sra = m68k_areg (regs, 7);
	if (sra & 1) {
		exception3 (opcode, sra);
		goto endlabel4512;
	}
{{	uae_s16 sr = x_get_word (sra);
	m68k_areg (regs, 7) += 2;
{	uaecptr pca;
	pca = m68k_areg (regs, 7);
	if (pca & 1) {
		exception3 (opcode, pca);
		goto endlabel4512;
	}
{{	uae_s32 pc = x_get_word (pca) << 16; pc |= x_get_word (pca + 2);
	m68k_areg (regs, 7) += 4;
	regs.sr &= 0xFF00; sr &= 0xFF;
	regs.sr |= sr;
	m68k_setpc (pc);
	MakeFromSR ();
	if (m68k_getpc () & 1) {
		uaecptr faultpc = m68k_getpc ();
	m68k_setpc (oldpc);
		exception3i (0x4E77, faultpc);
	}
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}}}}}endlabel4512: ;
} /* 20 (5/0) */

/* JSR.L (An) */
void REGPARAM2 CPUFUNC(op_4e90_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uaecptr oldpc = m68k_getpc () + 2;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel4513;
	}
	m68k_setpc (srca);
	get_word_ce000_prefetch (0);
	m68k_areg (regs, 7) -= 4;
	x_put_word (m68k_areg (regs, 7), oldpc >> 16);
	x_put_word (m68k_areg (regs, 7) + 2, oldpc);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4513: ;
} /* 16 (2/2) */

/* JSR.L (d16,An) */
void REGPARAM2 CPUFUNC(op_4ea8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)regs.irc;
{	uaecptr oldpc = m68k_getpc () + 4;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel4514;
	}
	do_cycles_ce000 (2);
	m68k_setpc (srca);
	get_word_ce000_prefetch (0);
	m68k_areg (regs, 7) -= 4;
	x_put_word (m68k_areg (regs, 7), oldpc >> 16);
	x_put_word (m68k_areg (regs, 7) + 2, oldpc);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4514: ;
} /* 18 (2/2) */

/* JSR.L (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_4eb0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), regs.irc);
{	uaecptr oldpc = m68k_getpc () + 4;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel4515;
	}
	m68k_setpc (srca);
	get_word_ce000_prefetch (0);
	do_cycles_ce000 (6);
	m68k_areg (regs, 7) -= 4;
	x_put_word (m68k_areg (regs, 7), oldpc >> 16);
	x_put_word (m68k_areg (regs, 7) + 2, oldpc);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4515: ;
} /* 22 (2/2) */

/* JSR.L (xxx).W */
void REGPARAM2 CPUFUNC(op_4eb8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)regs.irc;
{	uaecptr oldpc = m68k_getpc () + 4;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel4516;
	}
	do_cycles_ce000 (2);
	m68k_setpc (srca);
	get_word_ce000_prefetch (0);
	m68k_areg (regs, 7) -= 4;
	x_put_word (m68k_areg (regs, 7), oldpc >> 16);
	x_put_word (m68k_areg (regs, 7) + 2, oldpc);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4516: ;
} /* 18 (2/2) */

/* JSR.L (xxx).L */
void REGPARAM2 CPUFUNC(op_4eb9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= regs.irc;
{	uaecptr oldpc = m68k_getpc () + 6;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel4517;
	}
	m68k_setpc (srca);
	get_word_ce000_prefetch (0);
	m68k_areg (regs, 7) -= 4;
	x_put_word (m68k_areg (regs, 7), oldpc >> 16);
	x_put_word (m68k_areg (regs, 7) + 2, oldpc);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4517: ;
} /* 20 (3/2) */

/* JSR.L (d16,PC) */
void REGPARAM2 CPUFUNC(op_4eba_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)regs.irc;
{	uaecptr oldpc = m68k_getpc () + 4;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel4518;
	}
	do_cycles_ce000 (2);
	m68k_setpc (srca);
	get_word_ce000_prefetch (0);
	m68k_areg (regs, 7) -= 4;
	x_put_word (m68k_areg (regs, 7), oldpc >> 16);
	x_put_word (m68k_areg (regs, 7) + 2, oldpc);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4518: ;
} /* 18 (2/2) */

/* JSR.L (d8,PC,Xn) */
void REGPARAM2 CPUFUNC(op_4ebb_12)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	srca = get_disp_ea_000 (tmppc, regs.irc);
{	uaecptr oldpc = m68k_getpc () + 4;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel4519;
	}
	m68k_setpc (srca);
	get_word_ce000_prefetch (0);
	do_cycles_ce000 (6);
	m68k_areg (regs, 7) -= 4;
	x_put_word (m68k_areg (regs, 7), oldpc >> 16);
	x_put_word (m68k_areg (regs, 7) + 2, oldpc);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4519: ;
} /* 22 (2/2) */

/* JMP.L (An) */
void REGPARAM2 CPUFUNC(op_4ed0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel4520;
	}
	m68k_setpc (srca);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4520: ;
} /* 8 (2/0) */

/* JMP.L (d16,An) */
void REGPARAM2 CPUFUNC(op_4ee8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)regs.irc;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel4521;
	}
	do_cycles_ce000 (2);
	m68k_setpc (srca);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4521: ;
} /* 10 (2/0) */

/* JMP.L (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_4ef0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel4522;
	}
	do_cycles_ce000 (2);
	m68k_setpc (srca);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4522: ;
} /* 14 (3/0) */

/* JMP.L (xxx).W */
void REGPARAM2 CPUFUNC(op_4ef8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)regs.irc;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel4523;
	}
	do_cycles_ce000 (2);
	m68k_setpc (srca);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4523: ;
} /* 10 (2/0) */

/* JMP.L (xxx).L */
void REGPARAM2 CPUFUNC(op_4ef9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= regs.irc;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel4524;
	}
	m68k_setpc (srca);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4524: ;
} /* 12 (3/0) */

/* JMP.L (d16,PC) */
void REGPARAM2 CPUFUNC(op_4efa_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)regs.irc;
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel4525;
	}
	do_cycles_ce000 (2);
	m68k_setpc (srca);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4525: ;
} /* 10 (2/0) */

/* JMP.L (d8,PC,Xn) */
void REGPARAM2 CPUFUNC(op_4efb_12)(uae_u32 opcode)
{
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3i (opcode, srca);
		goto endlabel4526;
	}
	do_cycles_ce000 (2);
	m68k_setpc (srca);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4526: ;
} /* 14 (3/0) */

/* ADDQ.B #<data>,Dn */
void REGPARAM2 CPUFUNC(op_5000_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* ADDQ.B #<data>,(An) */
void REGPARAM2 CPUFUNC(op_5010_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* ADDQ.B #<data>,(An)+ */
void REGPARAM2 CPUFUNC(op_5018_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* ADDQ.B #<data>,-(An) */
void REGPARAM2 CPUFUNC(op_5020_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (2);
} /* 14 (2/1) */

/* ADDQ.B #<data>,(d16,An) */
void REGPARAM2 CPUFUNC(op_5028_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* ADDQ.B #<data>,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_5030_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* ADDQ.B #<data>,(xxx).W */
void REGPARAM2 CPUFUNC(op_5038_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* ADDQ.B #<data>,(xxx).L */
void REGPARAM2 CPUFUNC(op_5039_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* ADDQ.W #<data>,Dn */
void REGPARAM2 CPUFUNC(op_5040_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* ADDAQ.W #<data>,An */
void REGPARAM2 CPUFUNC(op_5048_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* ADDQ.W #<data>,(An) */
void REGPARAM2 CPUFUNC(op_5050_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4537;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (2);
endlabel4537: ;
} /* 12 (2/1) */

/* ADDQ.W #<data>,(An)+ */
void REGPARAM2 CPUFUNC(op_5058_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4538;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (2);
endlabel4538: ;
} /* 12 (2/1) */

/* ADDQ.W #<data>,-(An) */
void REGPARAM2 CPUFUNC(op_5060_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	do_cycles_ce000 (2);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4539;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (2);
endlabel4539: ;
} /* 14 (2/1) */

/* ADDQ.W #<data>,(d16,An) */
void REGPARAM2 CPUFUNC(op_5068_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4540;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (4);
endlabel4540: ;
} /* 16 (3/1) */

/* ADDQ.W #<data>,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_5070_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4541;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (4);
endlabel4541: ;
} /* 18 (3/1) */

#endif

#ifdef PART_5
/* ADDQ.W #<data>,(xxx).W */
void REGPARAM2 CPUFUNC(op_5078_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4542;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (4);
endlabel4542: ;
} /* 16 (3/1) */

/* ADDQ.W #<data>,(xxx).L */
void REGPARAM2 CPUFUNC(op_5079_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4543;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (6);
endlabel4543: ;
} /* 20 (4/1) */

/* ADDQ.L #<data>,Dn */
void REGPARAM2 CPUFUNC(op_5080_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* ADDAQ.L #<data>,An */
void REGPARAM2 CPUFUNC(op_5088_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* ADDQ.L #<data>,(An) */
void REGPARAM2 CPUFUNC(op_5090_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4546;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (2);
endlabel4546: ;
} /* 20 (3/2) */

/* ADDQ.L #<data>,(An)+ */
void REGPARAM2 CPUFUNC(op_5098_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4547;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) += 4;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (2);
endlabel4547: ;
} /* 20 (3/2) */

/* ADDQ.L #<data>,-(An) */
void REGPARAM2 CPUFUNC(op_50a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	do_cycles_ce000 (2);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4548;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (2);
endlabel4548: ;
} /* 22 (3/2) */

/* ADDQ.L #<data>,(d16,An) */
void REGPARAM2 CPUFUNC(op_50a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4549;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (4);
endlabel4549: ;
} /* 24 (4/2) */

/* ADDQ.L #<data>,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_50b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4550;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (4);
endlabel4550: ;
} /* 26 (4/2) */

/* ADDQ.L #<data>,(xxx).W */
void REGPARAM2 CPUFUNC(op_50b8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4551;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (4);
endlabel4551: ;
} /* 24 (4/2) */

/* ADDQ.L #<data>,(xxx).L */
void REGPARAM2 CPUFUNC(op_50b9_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4552;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (6);
endlabel4552: ;
} /* 28 (5/2) */

/* Scc.B Dn */
void REGPARAM2 CPUFUNC(op_50c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (0) ? 0xff : 0;
	int cycles = 0;
	if (val) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 4+ (1/0) */

/* DBcc.W Dn,#<data>.W */
void REGPARAM2 CPUFUNC(op_50c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = regs.irc;
	uaecptr oldpc = m68k_getpc ();
	do_cycles_ce000 (2);
	if (!cctrue (0)) {
	m68k_incpc ((uae_s32)offs + 2);
		get_word_ce000_prefetch (0);
		m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel4554;
			}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
			return;
		}
	} else {
		do_cycles_ce000 (2);
	}
	m68k_setpc (oldpc + 4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4554: ;
} /* 20 (4/0) */

/* Scc.B (An) */
void REGPARAM2 CPUFUNC(op_50d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (0) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B (An)+ */
void REGPARAM2 CPUFUNC(op_50d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (0) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B -(An) */
void REGPARAM2 CPUFUNC(op_50e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (0) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 14+ (2/1) */

/* Scc.B (d16,An) */
void REGPARAM2 CPUFUNC(op_50e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (0) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_50f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (0) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 18+ (3/1) */

/* Scc.B (xxx).W */
void REGPARAM2 CPUFUNC(op_50f8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (0) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (xxx).L */
void REGPARAM2 CPUFUNC(op_50f9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	int val = cctrue (0) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (6);
} /* 20+ (4/1) */

/* SUBQ.B #<data>,Dn */
void REGPARAM2 CPUFUNC(op_5100_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* SUBQ.B #<data>,(An) */
void REGPARAM2 CPUFUNC(op_5110_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* SUBQ.B #<data>,(An)+ */
void REGPARAM2 CPUFUNC(op_5118_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* SUBQ.B #<data>,-(An) */
void REGPARAM2 CPUFUNC(op_5120_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (2);
} /* 14 (2/1) */

/* SUBQ.B #<data>,(d16,An) */
void REGPARAM2 CPUFUNC(op_5128_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* SUBQ.B #<data>,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_5130_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* SUBQ.B #<data>,(xxx).W */
void REGPARAM2 CPUFUNC(op_5138_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* SUBQ.B #<data>,(xxx).L */
void REGPARAM2 CPUFUNC(op_5139_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* SUBQ.W #<data>,Dn */
void REGPARAM2 CPUFUNC(op_5140_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* SUBAQ.W #<data>,An */
void REGPARAM2 CPUFUNC(op_5148_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* SUBQ.W #<data>,(An) */
void REGPARAM2 CPUFUNC(op_5150_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4572;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (2);
endlabel4572: ;
} /* 12 (2/1) */

/* SUBQ.W #<data>,(An)+ */
void REGPARAM2 CPUFUNC(op_5158_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4573;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (2);
endlabel4573: ;
} /* 12 (2/1) */

/* SUBQ.W #<data>,-(An) */
void REGPARAM2 CPUFUNC(op_5160_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	do_cycles_ce000 (2);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4574;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (2);
endlabel4574: ;
} /* 14 (2/1) */

/* SUBQ.W #<data>,(d16,An) */
void REGPARAM2 CPUFUNC(op_5168_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4575;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (4);
endlabel4575: ;
} /* 16 (3/1) */

/* SUBQ.W #<data>,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_5170_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4576;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (4);
endlabel4576: ;
} /* 18 (3/1) */

/* SUBQ.W #<data>,(xxx).W */
void REGPARAM2 CPUFUNC(op_5178_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4577;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (4);
endlabel4577: ;
} /* 16 (3/1) */

/* SUBQ.W #<data>,(xxx).L */
void REGPARAM2 CPUFUNC(op_5179_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4578;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (6);
endlabel4578: ;
} /* 20 (4/1) */

/* SUBQ.L #<data>,Dn */
void REGPARAM2 CPUFUNC(op_5180_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* SUBAQ.L #<data>,An */
void REGPARAM2 CPUFUNC(op_5188_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* SUBQ.L #<data>,(An) */
void REGPARAM2 CPUFUNC(op_5190_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4581;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (2);
endlabel4581: ;
} /* 20 (3/2) */

/* SUBQ.L #<data>,(An)+ */
void REGPARAM2 CPUFUNC(op_5198_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4582;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) += 4;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (2);
endlabel4582: ;
} /* 20 (3/2) */

/* SUBQ.L #<data>,-(An) */
void REGPARAM2 CPUFUNC(op_51a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	do_cycles_ce000 (2);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4583;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (2);
endlabel4583: ;
} /* 22 (3/2) */

/* SUBQ.L #<data>,(d16,An) */
void REGPARAM2 CPUFUNC(op_51a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4584;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (4);
endlabel4584: ;
} /* 24 (4/2) */

/* SUBQ.L #<data>,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_51b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4585;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (4);
endlabel4585: ;
} /* 26 (4/2) */

/* SUBQ.L #<data>,(xxx).W */
void REGPARAM2 CPUFUNC(op_51b8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4586;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (4);
endlabel4586: ;
} /* 24 (4/2) */

/* SUBQ.L #<data>,(xxx).L */
void REGPARAM2 CPUFUNC(op_51b9_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
{{	uae_u32 src = srcreg;
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4587;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (6);
endlabel4587: ;
} /* 28 (5/2) */

/* Scc.B Dn */
void REGPARAM2 CPUFUNC(op_51c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (1) ? 0xff : 0;
	int cycles = 0;
	if (val) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 4+ (1/0) */

/* DBcc.W Dn,#<data>.W */
void REGPARAM2 CPUFUNC(op_51c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = regs.irc;
	uaecptr oldpc = m68k_getpc ();
	do_cycles_ce000 (2);
	if (!cctrue (1)) {
	m68k_incpc ((uae_s32)offs + 2);
		get_word_ce000_prefetch (0);
		m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel4589;
			}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
			return;
		}
	} else {
		do_cycles_ce000 (2);
	}
	m68k_setpc (oldpc + 4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4589: ;
} /* 20 (4/0) */

/* Scc.B (An) */
void REGPARAM2 CPUFUNC(op_51d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (1) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B (An)+ */
void REGPARAM2 CPUFUNC(op_51d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (1) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B -(An) */
void REGPARAM2 CPUFUNC(op_51e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (1) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 14+ (2/1) */

/* Scc.B (d16,An) */
void REGPARAM2 CPUFUNC(op_51e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (1) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_51f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (1) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 18+ (3/1) */

/* Scc.B (xxx).W */
void REGPARAM2 CPUFUNC(op_51f8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (1) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (xxx).L */
void REGPARAM2 CPUFUNC(op_51f9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	int val = cctrue (1) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (6);
} /* 20+ (4/1) */

/* Scc.B Dn */
void REGPARAM2 CPUFUNC(op_52c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (2) ? 0xff : 0;
	int cycles = 0;
	if (val) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 4+ (1/0) */

/* DBcc.W Dn,#<data>.W */
void REGPARAM2 CPUFUNC(op_52c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = regs.irc;
	uaecptr oldpc = m68k_getpc ();
	do_cycles_ce000 (2);
	if (!cctrue (2)) {
	m68k_incpc ((uae_s32)offs + 2);
		get_word_ce000_prefetch (0);
		m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel4598;
			}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
			return;
		}
	} else {
		do_cycles_ce000 (2);
	}
	m68k_setpc (oldpc + 4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4598: ;
} /* 20 (4/0) */

/* Scc.B (An) */
void REGPARAM2 CPUFUNC(op_52d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (2) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B (An)+ */
void REGPARAM2 CPUFUNC(op_52d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (2) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B -(An) */
void REGPARAM2 CPUFUNC(op_52e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (2) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 14+ (2/1) */

/* Scc.B (d16,An) */
void REGPARAM2 CPUFUNC(op_52e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (2) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_52f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (2) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 18+ (3/1) */

/* Scc.B (xxx).W */
void REGPARAM2 CPUFUNC(op_52f8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (2) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (xxx).L */
void REGPARAM2 CPUFUNC(op_52f9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	int val = cctrue (2) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (6);
} /* 20+ (4/1) */

/* Scc.B Dn */
void REGPARAM2 CPUFUNC(op_53c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (3) ? 0xff : 0;
	int cycles = 0;
	if (val) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 4+ (1/0) */

/* DBcc.W Dn,#<data>.W */
void REGPARAM2 CPUFUNC(op_53c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = regs.irc;
	uaecptr oldpc = m68k_getpc ();
	do_cycles_ce000 (2);
	if (!cctrue (3)) {
	m68k_incpc ((uae_s32)offs + 2);
		get_word_ce000_prefetch (0);
		m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel4607;
			}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
			return;
		}
	} else {
		do_cycles_ce000 (2);
	}
	m68k_setpc (oldpc + 4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4607: ;
} /* 20 (4/0) */

/* Scc.B (An) */
void REGPARAM2 CPUFUNC(op_53d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (3) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B (An)+ */
void REGPARAM2 CPUFUNC(op_53d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (3) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B -(An) */
void REGPARAM2 CPUFUNC(op_53e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (3) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 14+ (2/1) */

/* Scc.B (d16,An) */
void REGPARAM2 CPUFUNC(op_53e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (3) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_53f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (3) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 18+ (3/1) */

/* Scc.B (xxx).W */
void REGPARAM2 CPUFUNC(op_53f8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (3) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (xxx).L */
void REGPARAM2 CPUFUNC(op_53f9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	int val = cctrue (3) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (6);
} /* 20+ (4/1) */

/* Scc.B Dn */
void REGPARAM2 CPUFUNC(op_54c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (4) ? 0xff : 0;
	int cycles = 0;
	if (val) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 4+ (1/0) */

/* DBcc.W Dn,#<data>.W */
void REGPARAM2 CPUFUNC(op_54c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = regs.irc;
	uaecptr oldpc = m68k_getpc ();
	do_cycles_ce000 (2);
	if (!cctrue (4)) {
	m68k_incpc ((uae_s32)offs + 2);
		get_word_ce000_prefetch (0);
		m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel4616;
			}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
			return;
		}
	} else {
		do_cycles_ce000 (2);
	}
	m68k_setpc (oldpc + 4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4616: ;
} /* 20 (4/0) */

/* Scc.B (An) */
void REGPARAM2 CPUFUNC(op_54d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (4) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B (An)+ */
void REGPARAM2 CPUFUNC(op_54d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (4) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B -(An) */
void REGPARAM2 CPUFUNC(op_54e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (4) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 14+ (2/1) */

/* Scc.B (d16,An) */
void REGPARAM2 CPUFUNC(op_54e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (4) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_54f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (4) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 18+ (3/1) */

/* Scc.B (xxx).W */
void REGPARAM2 CPUFUNC(op_54f8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (4) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (xxx).L */
void REGPARAM2 CPUFUNC(op_54f9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	int val = cctrue (4) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (6);
} /* 20+ (4/1) */

/* Scc.B Dn */
void REGPARAM2 CPUFUNC(op_55c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (5) ? 0xff : 0;
	int cycles = 0;
	if (val) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 4+ (1/0) */

/* DBcc.W Dn,#<data>.W */
void REGPARAM2 CPUFUNC(op_55c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = regs.irc;
	uaecptr oldpc = m68k_getpc ();
	do_cycles_ce000 (2);
	if (!cctrue (5)) {
	m68k_incpc ((uae_s32)offs + 2);
		get_word_ce000_prefetch (0);
		m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel4625;
			}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
			return;
		}
	} else {
		do_cycles_ce000 (2);
	}
	m68k_setpc (oldpc + 4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4625: ;
} /* 20 (4/0) */

/* Scc.B (An) */
void REGPARAM2 CPUFUNC(op_55d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (5) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B (An)+ */
void REGPARAM2 CPUFUNC(op_55d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (5) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B -(An) */
void REGPARAM2 CPUFUNC(op_55e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (5) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 14+ (2/1) */

/* Scc.B (d16,An) */
void REGPARAM2 CPUFUNC(op_55e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (5) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_55f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (5) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 18+ (3/1) */

/* Scc.B (xxx).W */
void REGPARAM2 CPUFUNC(op_55f8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (5) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (xxx).L */
void REGPARAM2 CPUFUNC(op_55f9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	int val = cctrue (5) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (6);
} /* 20+ (4/1) */

/* Scc.B Dn */
void REGPARAM2 CPUFUNC(op_56c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (6) ? 0xff : 0;
	int cycles = 0;
	if (val) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 4+ (1/0) */

/* DBcc.W Dn,#<data>.W */
void REGPARAM2 CPUFUNC(op_56c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = regs.irc;
	uaecptr oldpc = m68k_getpc ();
	do_cycles_ce000 (2);
	if (!cctrue (6)) {
	m68k_incpc ((uae_s32)offs + 2);
		get_word_ce000_prefetch (0);
		m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel4634;
			}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
			return;
		}
	} else {
		do_cycles_ce000 (2);
	}
	m68k_setpc (oldpc + 4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4634: ;
} /* 20 (4/0) */

/* Scc.B (An) */
void REGPARAM2 CPUFUNC(op_56d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (6) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B (An)+ */
void REGPARAM2 CPUFUNC(op_56d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (6) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B -(An) */
void REGPARAM2 CPUFUNC(op_56e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (6) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 14+ (2/1) */

/* Scc.B (d16,An) */
void REGPARAM2 CPUFUNC(op_56e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (6) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_56f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (6) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 18+ (3/1) */

/* Scc.B (xxx).W */
void REGPARAM2 CPUFUNC(op_56f8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (6) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (xxx).L */
void REGPARAM2 CPUFUNC(op_56f9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	int val = cctrue (6) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (6);
} /* 20+ (4/1) */

/* Scc.B Dn */
void REGPARAM2 CPUFUNC(op_57c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (7) ? 0xff : 0;
	int cycles = 0;
	if (val) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 4+ (1/0) */

/* DBcc.W Dn,#<data>.W */
void REGPARAM2 CPUFUNC(op_57c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = regs.irc;
	uaecptr oldpc = m68k_getpc ();
	do_cycles_ce000 (2);
	if (!cctrue (7)) {
	m68k_incpc ((uae_s32)offs + 2);
		get_word_ce000_prefetch (0);
		m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel4643;
			}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
			return;
		}
	} else {
		do_cycles_ce000 (2);
	}
	m68k_setpc (oldpc + 4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4643: ;
} /* 20 (4/0) */

/* Scc.B (An) */
void REGPARAM2 CPUFUNC(op_57d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (7) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B (An)+ */
void REGPARAM2 CPUFUNC(op_57d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (7) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B -(An) */
void REGPARAM2 CPUFUNC(op_57e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (7) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 14+ (2/1) */

/* Scc.B (d16,An) */
void REGPARAM2 CPUFUNC(op_57e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (7) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_57f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (7) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 18+ (3/1) */

/* Scc.B (xxx).W */
void REGPARAM2 CPUFUNC(op_57f8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (7) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (xxx).L */
void REGPARAM2 CPUFUNC(op_57f9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	int val = cctrue (7) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (6);
} /* 20+ (4/1) */

/* Scc.B Dn */
void REGPARAM2 CPUFUNC(op_58c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (8) ? 0xff : 0;
	int cycles = 0;
	if (val) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 4+ (1/0) */

/* DBcc.W Dn,#<data>.W */
void REGPARAM2 CPUFUNC(op_58c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = regs.irc;
	uaecptr oldpc = m68k_getpc ();
	do_cycles_ce000 (2);
	if (!cctrue (8)) {
	m68k_incpc ((uae_s32)offs + 2);
		get_word_ce000_prefetch (0);
		m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel4652;
			}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
			return;
		}
	} else {
		do_cycles_ce000 (2);
	}
	m68k_setpc (oldpc + 4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4652: ;
} /* 20 (4/0) */

/* Scc.B (An) */
void REGPARAM2 CPUFUNC(op_58d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (8) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B (An)+ */
void REGPARAM2 CPUFUNC(op_58d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (8) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B -(An) */
void REGPARAM2 CPUFUNC(op_58e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (8) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 14+ (2/1) */

/* Scc.B (d16,An) */
void REGPARAM2 CPUFUNC(op_58e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (8) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_58f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (8) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 18+ (3/1) */

/* Scc.B (xxx).W */
void REGPARAM2 CPUFUNC(op_58f8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (8) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (xxx).L */
void REGPARAM2 CPUFUNC(op_58f9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	int val = cctrue (8) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (6);
} /* 20+ (4/1) */

/* Scc.B Dn */
void REGPARAM2 CPUFUNC(op_59c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (9) ? 0xff : 0;
	int cycles = 0;
	if (val) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 4+ (1/0) */

/* DBcc.W Dn,#<data>.W */
void REGPARAM2 CPUFUNC(op_59c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = regs.irc;
	uaecptr oldpc = m68k_getpc ();
	do_cycles_ce000 (2);
	if (!cctrue (9)) {
	m68k_incpc ((uae_s32)offs + 2);
		get_word_ce000_prefetch (0);
		m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel4661;
			}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
			return;
		}
	} else {
		do_cycles_ce000 (2);
	}
	m68k_setpc (oldpc + 4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4661: ;
} /* 20 (4/0) */

/* Scc.B (An) */
void REGPARAM2 CPUFUNC(op_59d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (9) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B (An)+ */
void REGPARAM2 CPUFUNC(op_59d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (9) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B -(An) */
void REGPARAM2 CPUFUNC(op_59e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (9) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 14+ (2/1) */

/* Scc.B (d16,An) */
void REGPARAM2 CPUFUNC(op_59e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (9) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_59f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (9) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 18+ (3/1) */

/* Scc.B (xxx).W */
void REGPARAM2 CPUFUNC(op_59f8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (9) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (xxx).L */
void REGPARAM2 CPUFUNC(op_59f9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	int val = cctrue (9) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (6);
} /* 20+ (4/1) */

/* Scc.B Dn */
void REGPARAM2 CPUFUNC(op_5ac0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (10) ? 0xff : 0;
	int cycles = 0;
	if (val) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 4+ (1/0) */

/* DBcc.W Dn,#<data>.W */
void REGPARAM2 CPUFUNC(op_5ac8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = regs.irc;
	uaecptr oldpc = m68k_getpc ();
	do_cycles_ce000 (2);
	if (!cctrue (10)) {
	m68k_incpc ((uae_s32)offs + 2);
		get_word_ce000_prefetch (0);
		m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel4670;
			}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
			return;
		}
	} else {
		do_cycles_ce000 (2);
	}
	m68k_setpc (oldpc + 4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4670: ;
} /* 20 (4/0) */

/* Scc.B (An) */
void REGPARAM2 CPUFUNC(op_5ad0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (10) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B (An)+ */
void REGPARAM2 CPUFUNC(op_5ad8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (10) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B -(An) */
void REGPARAM2 CPUFUNC(op_5ae0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (10) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 14+ (2/1) */

/* Scc.B (d16,An) */
void REGPARAM2 CPUFUNC(op_5ae8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (10) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_5af0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (10) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 18+ (3/1) */

/* Scc.B (xxx).W */
void REGPARAM2 CPUFUNC(op_5af8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (10) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (xxx).L */
void REGPARAM2 CPUFUNC(op_5af9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	int val = cctrue (10) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (6);
} /* 20+ (4/1) */

/* Scc.B Dn */
void REGPARAM2 CPUFUNC(op_5bc0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (11) ? 0xff : 0;
	int cycles = 0;
	if (val) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 4+ (1/0) */

/* DBcc.W Dn,#<data>.W */
void REGPARAM2 CPUFUNC(op_5bc8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = regs.irc;
	uaecptr oldpc = m68k_getpc ();
	do_cycles_ce000 (2);
	if (!cctrue (11)) {
	m68k_incpc ((uae_s32)offs + 2);
		get_word_ce000_prefetch (0);
		m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel4679;
			}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
			return;
		}
	} else {
		do_cycles_ce000 (2);
	}
	m68k_setpc (oldpc + 4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4679: ;
} /* 20 (4/0) */

/* Scc.B (An) */
void REGPARAM2 CPUFUNC(op_5bd0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (11) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B (An)+ */
void REGPARAM2 CPUFUNC(op_5bd8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (11) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B -(An) */
void REGPARAM2 CPUFUNC(op_5be0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (11) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 14+ (2/1) */

/* Scc.B (d16,An) */
void REGPARAM2 CPUFUNC(op_5be8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (11) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_5bf0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (11) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 18+ (3/1) */

/* Scc.B (xxx).W */
void REGPARAM2 CPUFUNC(op_5bf8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (11) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (xxx).L */
void REGPARAM2 CPUFUNC(op_5bf9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	int val = cctrue (11) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (6);
} /* 20+ (4/1) */

/* Scc.B Dn */
void REGPARAM2 CPUFUNC(op_5cc0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (12) ? 0xff : 0;
	int cycles = 0;
	if (val) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 4+ (1/0) */

/* DBcc.W Dn,#<data>.W */
void REGPARAM2 CPUFUNC(op_5cc8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = regs.irc;
	uaecptr oldpc = m68k_getpc ();
	do_cycles_ce000 (2);
	if (!cctrue (12)) {
	m68k_incpc ((uae_s32)offs + 2);
		get_word_ce000_prefetch (0);
		m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel4688;
			}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
			return;
		}
	} else {
		do_cycles_ce000 (2);
	}
	m68k_setpc (oldpc + 4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4688: ;
} /* 20 (4/0) */

/* Scc.B (An) */
void REGPARAM2 CPUFUNC(op_5cd0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (12) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B (An)+ */
void REGPARAM2 CPUFUNC(op_5cd8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (12) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B -(An) */
void REGPARAM2 CPUFUNC(op_5ce0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (12) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 14+ (2/1) */

/* Scc.B (d16,An) */
void REGPARAM2 CPUFUNC(op_5ce8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (12) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_5cf0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (12) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 18+ (3/1) */

/* Scc.B (xxx).W */
void REGPARAM2 CPUFUNC(op_5cf8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (12) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (xxx).L */
void REGPARAM2 CPUFUNC(op_5cf9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	int val = cctrue (12) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (6);
} /* 20+ (4/1) */

/* Scc.B Dn */
void REGPARAM2 CPUFUNC(op_5dc0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (13) ? 0xff : 0;
	int cycles = 0;
	if (val) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 4+ (1/0) */

/* DBcc.W Dn,#<data>.W */
void REGPARAM2 CPUFUNC(op_5dc8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = regs.irc;
	uaecptr oldpc = m68k_getpc ();
	do_cycles_ce000 (2);
	if (!cctrue (13)) {
	m68k_incpc ((uae_s32)offs + 2);
		get_word_ce000_prefetch (0);
		m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel4697;
			}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
			return;
		}
	} else {
		do_cycles_ce000 (2);
	}
	m68k_setpc (oldpc + 4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4697: ;
} /* 20 (4/0) */

/* Scc.B (An) */
void REGPARAM2 CPUFUNC(op_5dd0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (13) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B (An)+ */
void REGPARAM2 CPUFUNC(op_5dd8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (13) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B -(An) */
void REGPARAM2 CPUFUNC(op_5de0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (13) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 14+ (2/1) */

/* Scc.B (d16,An) */
void REGPARAM2 CPUFUNC(op_5de8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (13) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_5df0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (13) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 18+ (3/1) */

/* Scc.B (xxx).W */
void REGPARAM2 CPUFUNC(op_5df8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (13) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (xxx).L */
void REGPARAM2 CPUFUNC(op_5df9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	int val = cctrue (13) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (6);
} /* 20+ (4/1) */

/* Scc.B Dn */
void REGPARAM2 CPUFUNC(op_5ec0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (14) ? 0xff : 0;
	int cycles = 0;
	if (val) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 4+ (1/0) */

/* DBcc.W Dn,#<data>.W */
void REGPARAM2 CPUFUNC(op_5ec8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = regs.irc;
	uaecptr oldpc = m68k_getpc ();
	do_cycles_ce000 (2);
	if (!cctrue (14)) {
	m68k_incpc ((uae_s32)offs + 2);
		get_word_ce000_prefetch (0);
		m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel4706;
			}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
			return;
		}
	} else {
		do_cycles_ce000 (2);
	}
	m68k_setpc (oldpc + 4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4706: ;
} /* 20 (4/0) */

/* Scc.B (An) */
void REGPARAM2 CPUFUNC(op_5ed0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (14) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B (An)+ */
void REGPARAM2 CPUFUNC(op_5ed8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (14) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B -(An) */
void REGPARAM2 CPUFUNC(op_5ee0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (14) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 14+ (2/1) */

/* Scc.B (d16,An) */
void REGPARAM2 CPUFUNC(op_5ee8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (14) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_5ef0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (14) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 18+ (3/1) */

/* Scc.B (xxx).W */
void REGPARAM2 CPUFUNC(op_5ef8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (14) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (xxx).L */
void REGPARAM2 CPUFUNC(op_5ef9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	int val = cctrue (14) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (6);
} /* 20+ (4/1) */

/* Scc.B Dn */
void REGPARAM2 CPUFUNC(op_5fc0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (15) ? 0xff : 0;
	int cycles = 0;
	if (val) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 4+ (1/0) */

/* DBcc.W Dn,#<data>.W */
void REGPARAM2 CPUFUNC(op_5fc8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 offs = regs.irc;
	uaecptr oldpc = m68k_getpc ();
	do_cycles_ce000 (2);
	if (!cctrue (15)) {
	m68k_incpc ((uae_s32)offs + 2);
		get_word_ce000_prefetch (0);
		m68k_dreg (regs, srcreg) = (m68k_dreg (regs, srcreg) & ~0xffff) | (((src - 1)) & 0xffff);
		if (src) {
			if (offs & 1) {
				exception3i (opcode, m68k_getpc () + 2 + (uae_s32)offs + 2);
				goto endlabel4715;
			}
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
			return;
		}
	} else {
		do_cycles_ce000 (2);
	}
	m68k_setpc (oldpc + 4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}endlabel4715: ;
} /* 20 (4/0) */

/* Scc.B (An) */
void REGPARAM2 CPUFUNC(op_5fd0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (15) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B (An)+ */
void REGPARAM2 CPUFUNC(op_5fd8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (15) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 12+ (2/1) */

/* Scc.B -(An) */
void REGPARAM2 CPUFUNC(op_5fe0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	int val = cctrue (15) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (2);
} /* 14+ (2/1) */

/* Scc.B (d16,An) */
void REGPARAM2 CPUFUNC(op_5fe8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (15) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_5ff0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (15) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 18+ (3/1) */

/* Scc.B (xxx).W */
void REGPARAM2 CPUFUNC(op_5ff8_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	int val = cctrue (15) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (4);
} /* 16+ (3/1) */

/* Scc.B (xxx).L */
void REGPARAM2 CPUFUNC(op_5ff9_12)(uae_u32 opcode)
{
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	int val = cctrue (15) ? 0xff : 0;
	int cycles = 0;
	if (cycles > 0) do_cycles_ce000 (cycles);
	x_put_byte (srca, val);
}}}}}	m68k_incpc (6);
} /* 20+ (4/1) */

/* Bcc.W #<data>.W */
void REGPARAM2 CPUFUNC(op_6000_12)(uae_u32 opcode)
{
{{	uae_s16 src = regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (0)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4723;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	do_cycles_ce000 (2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4723: ;
} /* 20 (4/0) */

/* BccQ.B #<data> */
void REGPARAM2 CPUFUNC(op_6001_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	do_cycles_ce000 (2);
	if (!cctrue (0)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4724;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (2);
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4724: ;
} /* 16 (3/0) */

/* Bcc.L #<data>.L */
void REGPARAM2 CPUFUNC(op_60ff_12)(uae_u32 opcode)
{
{	do_cycles_ce000 (2);
	if (cctrue (0)) {
		exception3i (opcode, m68k_getpc () + 1);
		goto endlabel4725;
	}
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	goto endlabel4725;
{	uae_s32 src;
	src = get_word_ce000_prefetch (2) << 16;
	src |= regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (0)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4725;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4725: ;
} /* 28 (6/0) */

/* BSR.W #<data>.W */
void REGPARAM2 CPUFUNC(op_6100_12)(uae_u32 opcode)
{
{	uae_s32 s;
{	uae_s16 src = regs.irc;
	s = (uae_s32)src + 2;
	if (src & 1) {
		exception3pc (opcode, m68k_getpc () + s, 0, 1, m68k_getpc () + s);
		goto endlabel4726;
	}
	do_cycles_ce000 (2);
	m68k_do_bsr_ce (m68k_getpc () + 4, s);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4726: ;
} /* 18 (2/2) */

/* BSRQ.B #<data> */
void REGPARAM2 CPUFUNC(op_6101_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{	uae_s32 s;
{	uae_u32 src = srcreg;
	s = (uae_s32)src + 2;
	if (src & 1) {
		exception3pc (opcode, m68k_getpc () + s, 0, 1, m68k_getpc () + s);
		goto endlabel4727;
	}
	do_cycles_ce000 (2);
	m68k_do_bsr_ce (m68k_getpc () + 2, s);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4727: ;
} /* 18 (2/2) */

/* BSR.L #<data>.L */
void REGPARAM2 CPUFUNC(op_61ff_12)(uae_u32 opcode)
{
{	uae_s32 s;
	uae_u32 src = 0xffffffff;
	s = (uae_s32)src + 2;
	if (src & 1) {
		exception3pc (opcode, m68k_getpc () + s, 0, 1, m68k_getpc () + s);
		goto endlabel4728;
	}
	do_cycles_ce000 (2);
	m68k_do_bsr_ce (m68k_getpc () + 2, s);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}endlabel4728: ;
} /* 18 (2/2) */

#endif

#ifdef PART_6
/* Bcc.W #<data>.W */
void REGPARAM2 CPUFUNC(op_6200_12)(uae_u32 opcode)
{
{{	uae_s16 src = regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (2)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4729;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	do_cycles_ce000 (2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4729: ;
} /* 20 (4/0) */

/* BccQ.B #<data> */
void REGPARAM2 CPUFUNC(op_6201_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	do_cycles_ce000 (2);
	if (!cctrue (2)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4730;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (2);
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4730: ;
} /* 16 (3/0) */

/* Bcc.L #<data>.L */
void REGPARAM2 CPUFUNC(op_62ff_12)(uae_u32 opcode)
{
{	do_cycles_ce000 (2);
	if (cctrue (2)) {
		exception3i (opcode, m68k_getpc () + 1);
		goto endlabel4731;
	}
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	goto endlabel4731;
{	uae_s32 src;
	src = get_word_ce000_prefetch (2) << 16;
	src |= regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (2)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4731;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4731: ;
} /* 28 (6/0) */

/* Bcc.W #<data>.W */
void REGPARAM2 CPUFUNC(op_6300_12)(uae_u32 opcode)
{
{{	uae_s16 src = regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (3)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4732;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	do_cycles_ce000 (2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4732: ;
} /* 20 (4/0) */

/* BccQ.B #<data> */
void REGPARAM2 CPUFUNC(op_6301_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	do_cycles_ce000 (2);
	if (!cctrue (3)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4733;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (2);
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4733: ;
} /* 16 (3/0) */

/* Bcc.L #<data>.L */
void REGPARAM2 CPUFUNC(op_63ff_12)(uae_u32 opcode)
{
{	do_cycles_ce000 (2);
	if (cctrue (3)) {
		exception3i (opcode, m68k_getpc () + 1);
		goto endlabel4734;
	}
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	goto endlabel4734;
{	uae_s32 src;
	src = get_word_ce000_prefetch (2) << 16;
	src |= regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (3)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4734;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4734: ;
} /* 28 (6/0) */

/* Bcc.W #<data>.W */
void REGPARAM2 CPUFUNC(op_6400_12)(uae_u32 opcode)
{
{{	uae_s16 src = regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (4)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4735;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	do_cycles_ce000 (2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4735: ;
} /* 20 (4/0) */

/* BccQ.B #<data> */
void REGPARAM2 CPUFUNC(op_6401_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	do_cycles_ce000 (2);
	if (!cctrue (4)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4736;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (2);
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4736: ;
} /* 16 (3/0) */

/* Bcc.L #<data>.L */
void REGPARAM2 CPUFUNC(op_64ff_12)(uae_u32 opcode)
{
{	do_cycles_ce000 (2);
	if (cctrue (4)) {
		exception3i (opcode, m68k_getpc () + 1);
		goto endlabel4737;
	}
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	goto endlabel4737;
{	uae_s32 src;
	src = get_word_ce000_prefetch (2) << 16;
	src |= regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (4)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4737;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4737: ;
} /* 28 (6/0) */

/* Bcc.W #<data>.W */
void REGPARAM2 CPUFUNC(op_6500_12)(uae_u32 opcode)
{
{{	uae_s16 src = regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (5)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4738;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	do_cycles_ce000 (2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4738: ;
} /* 20 (4/0) */

/* BccQ.B #<data> */
void REGPARAM2 CPUFUNC(op_6501_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	do_cycles_ce000 (2);
	if (!cctrue (5)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4739;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (2);
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4739: ;
} /* 16 (3/0) */

/* Bcc.L #<data>.L */
void REGPARAM2 CPUFUNC(op_65ff_12)(uae_u32 opcode)
{
{	do_cycles_ce000 (2);
	if (cctrue (5)) {
		exception3i (opcode, m68k_getpc () + 1);
		goto endlabel4740;
	}
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	goto endlabel4740;
{	uae_s32 src;
	src = get_word_ce000_prefetch (2) << 16;
	src |= regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (5)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4740;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4740: ;
} /* 28 (6/0) */

/* Bcc.W #<data>.W */
void REGPARAM2 CPUFUNC(op_6600_12)(uae_u32 opcode)
{
{{	uae_s16 src = regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (6)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4741;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	do_cycles_ce000 (2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4741: ;
} /* 20 (4/0) */

/* BccQ.B #<data> */
void REGPARAM2 CPUFUNC(op_6601_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	do_cycles_ce000 (2);
	if (!cctrue (6)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4742;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (2);
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4742: ;
} /* 16 (3/0) */

/* Bcc.L #<data>.L */
void REGPARAM2 CPUFUNC(op_66ff_12)(uae_u32 opcode)
{
{	do_cycles_ce000 (2);
	if (cctrue (6)) {
		exception3i (opcode, m68k_getpc () + 1);
		goto endlabel4743;
	}
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	goto endlabel4743;
{	uae_s32 src;
	src = get_word_ce000_prefetch (2) << 16;
	src |= regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (6)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4743;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4743: ;
} /* 28 (6/0) */

/* Bcc.W #<data>.W */
void REGPARAM2 CPUFUNC(op_6700_12)(uae_u32 opcode)
{
{{	uae_s16 src = regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (7)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4744;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	do_cycles_ce000 (2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4744: ;
} /* 20 (4/0) */

/* BccQ.B #<data> */
void REGPARAM2 CPUFUNC(op_6701_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	do_cycles_ce000 (2);
	if (!cctrue (7)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4745;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (2);
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4745: ;
} /* 16 (3/0) */

/* Bcc.L #<data>.L */
void REGPARAM2 CPUFUNC(op_67ff_12)(uae_u32 opcode)
{
{	do_cycles_ce000 (2);
	if (cctrue (7)) {
		exception3i (opcode, m68k_getpc () + 1);
		goto endlabel4746;
	}
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	goto endlabel4746;
{	uae_s32 src;
	src = get_word_ce000_prefetch (2) << 16;
	src |= regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (7)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4746;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4746: ;
} /* 28 (6/0) */

/* Bcc.W #<data>.W */
void REGPARAM2 CPUFUNC(op_6800_12)(uae_u32 opcode)
{
{{	uae_s16 src = regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (8)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4747;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	do_cycles_ce000 (2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4747: ;
} /* 20 (4/0) */

/* BccQ.B #<data> */
void REGPARAM2 CPUFUNC(op_6801_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	do_cycles_ce000 (2);
	if (!cctrue (8)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4748;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (2);
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4748: ;
} /* 16 (3/0) */

/* Bcc.L #<data>.L */
void REGPARAM2 CPUFUNC(op_68ff_12)(uae_u32 opcode)
{
{	do_cycles_ce000 (2);
	if (cctrue (8)) {
		exception3i (opcode, m68k_getpc () + 1);
		goto endlabel4749;
	}
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	goto endlabel4749;
{	uae_s32 src;
	src = get_word_ce000_prefetch (2) << 16;
	src |= regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (8)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4749;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4749: ;
} /* 28 (6/0) */

/* Bcc.W #<data>.W */
void REGPARAM2 CPUFUNC(op_6900_12)(uae_u32 opcode)
{
{{	uae_s16 src = regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (9)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4750;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	do_cycles_ce000 (2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4750: ;
} /* 20 (4/0) */

/* BccQ.B #<data> */
void REGPARAM2 CPUFUNC(op_6901_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	do_cycles_ce000 (2);
	if (!cctrue (9)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4751;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (2);
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4751: ;
} /* 16 (3/0) */

/* Bcc.L #<data>.L */
void REGPARAM2 CPUFUNC(op_69ff_12)(uae_u32 opcode)
{
{	do_cycles_ce000 (2);
	if (cctrue (9)) {
		exception3i (opcode, m68k_getpc () + 1);
		goto endlabel4752;
	}
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	goto endlabel4752;
{	uae_s32 src;
	src = get_word_ce000_prefetch (2) << 16;
	src |= regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (9)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4752;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4752: ;
} /* 28 (6/0) */

/* Bcc.W #<data>.W */
void REGPARAM2 CPUFUNC(op_6a00_12)(uae_u32 opcode)
{
{{	uae_s16 src = regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (10)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4753;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	do_cycles_ce000 (2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4753: ;
} /* 20 (4/0) */

/* BccQ.B #<data> */
void REGPARAM2 CPUFUNC(op_6a01_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	do_cycles_ce000 (2);
	if (!cctrue (10)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4754;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (2);
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4754: ;
} /* 16 (3/0) */

/* Bcc.L #<data>.L */
void REGPARAM2 CPUFUNC(op_6aff_12)(uae_u32 opcode)
{
{	do_cycles_ce000 (2);
	if (cctrue (10)) {
		exception3i (opcode, m68k_getpc () + 1);
		goto endlabel4755;
	}
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	goto endlabel4755;
{	uae_s32 src;
	src = get_word_ce000_prefetch (2) << 16;
	src |= regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (10)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4755;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4755: ;
} /* 28 (6/0) */

/* Bcc.W #<data>.W */
void REGPARAM2 CPUFUNC(op_6b00_12)(uae_u32 opcode)
{
{{	uae_s16 src = regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (11)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4756;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	do_cycles_ce000 (2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4756: ;
} /* 20 (4/0) */

/* BccQ.B #<data> */
void REGPARAM2 CPUFUNC(op_6b01_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	do_cycles_ce000 (2);
	if (!cctrue (11)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4757;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (2);
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4757: ;
} /* 16 (3/0) */

/* Bcc.L #<data>.L */
void REGPARAM2 CPUFUNC(op_6bff_12)(uae_u32 opcode)
{
{	do_cycles_ce000 (2);
	if (cctrue (11)) {
		exception3i (opcode, m68k_getpc () + 1);
		goto endlabel4758;
	}
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	goto endlabel4758;
{	uae_s32 src;
	src = get_word_ce000_prefetch (2) << 16;
	src |= regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (11)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4758;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4758: ;
} /* 28 (6/0) */

/* Bcc.W #<data>.W */
void REGPARAM2 CPUFUNC(op_6c00_12)(uae_u32 opcode)
{
{{	uae_s16 src = regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (12)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4759;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	do_cycles_ce000 (2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4759: ;
} /* 20 (4/0) */

/* BccQ.B #<data> */
void REGPARAM2 CPUFUNC(op_6c01_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	do_cycles_ce000 (2);
	if (!cctrue (12)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4760;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (2);
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4760: ;
} /* 16 (3/0) */

/* Bcc.L #<data>.L */
void REGPARAM2 CPUFUNC(op_6cff_12)(uae_u32 opcode)
{
{	do_cycles_ce000 (2);
	if (cctrue (12)) {
		exception3i (opcode, m68k_getpc () + 1);
		goto endlabel4761;
	}
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	goto endlabel4761;
{	uae_s32 src;
	src = get_word_ce000_prefetch (2) << 16;
	src |= regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (12)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4761;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4761: ;
} /* 28 (6/0) */

/* Bcc.W #<data>.W */
void REGPARAM2 CPUFUNC(op_6d00_12)(uae_u32 opcode)
{
{{	uae_s16 src = regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (13)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4762;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	do_cycles_ce000 (2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4762: ;
} /* 20 (4/0) */

/* BccQ.B #<data> */
void REGPARAM2 CPUFUNC(op_6d01_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	do_cycles_ce000 (2);
	if (!cctrue (13)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4763;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (2);
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4763: ;
} /* 16 (3/0) */

/* Bcc.L #<data>.L */
void REGPARAM2 CPUFUNC(op_6dff_12)(uae_u32 opcode)
{
{	do_cycles_ce000 (2);
	if (cctrue (13)) {
		exception3i (opcode, m68k_getpc () + 1);
		goto endlabel4764;
	}
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	goto endlabel4764;
{	uae_s32 src;
	src = get_word_ce000_prefetch (2) << 16;
	src |= regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (13)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4764;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4764: ;
} /* 28 (6/0) */

/* Bcc.W #<data>.W */
void REGPARAM2 CPUFUNC(op_6e00_12)(uae_u32 opcode)
{
{{	uae_s16 src = regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (14)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4765;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	do_cycles_ce000 (2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4765: ;
} /* 20 (4/0) */

/* BccQ.B #<data> */
void REGPARAM2 CPUFUNC(op_6e01_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	do_cycles_ce000 (2);
	if (!cctrue (14)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4766;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (2);
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4766: ;
} /* 16 (3/0) */

/* Bcc.L #<data>.L */
void REGPARAM2 CPUFUNC(op_6eff_12)(uae_u32 opcode)
{
{	do_cycles_ce000 (2);
	if (cctrue (14)) {
		exception3i (opcode, m68k_getpc () + 1);
		goto endlabel4767;
	}
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	goto endlabel4767;
{	uae_s32 src;
	src = get_word_ce000_prefetch (2) << 16;
	src |= regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (14)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4767;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4767: ;
} /* 28 (6/0) */

/* Bcc.W #<data>.W */
void REGPARAM2 CPUFUNC(op_6f00_12)(uae_u32 opcode)
{
{{	uae_s16 src = regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (15)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4768;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	do_cycles_ce000 (2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4768: ;
} /* 20 (4/0) */

/* BccQ.B #<data> */
void REGPARAM2 CPUFUNC(op_6f01_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
{{	uae_u32 src = srcreg;
	do_cycles_ce000 (2);
	if (!cctrue (15)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4769;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (2);
	do_cycles_ce000 (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4769: ;
} /* 16 (3/0) */

/* Bcc.L #<data>.L */
void REGPARAM2 CPUFUNC(op_6fff_12)(uae_u32 opcode)
{
{	do_cycles_ce000 (2);
	if (cctrue (15)) {
		exception3i (opcode, m68k_getpc () + 1);
		goto endlabel4770;
	}
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	goto endlabel4770;
{	uae_s32 src;
	src = get_word_ce000_prefetch (2) << 16;
	src |= regs.irc;
	do_cycles_ce000 (2);
	if (!cctrue (15)) goto didnt_jump;
	if (src & 1) {
		exception3i (opcode, m68k_getpc () + 2 + (uae_s32)src);
		goto endlabel4770;
	}
	m68k_incpc ((uae_s32)src + 2);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
	return;
didnt_jump:;
	m68k_incpc (4);
	get_word_ce000_prefetch (0);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}endlabel4770: ;
} /* 28 (6/0) */

/* MOVEQ.L #<data>,Dn */
void REGPARAM2 CPUFUNC(op_7000_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (uae_s32)(uae_s8)(opcode & 255);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_u32 src = srcreg;
{	optflag_testl ((uae_s32)(src));
	m68k_dreg (regs, dstreg) = (src);
	m68k_incpc (2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (2);
}}}} /* 4 (1/0) */

/* OR.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_8000_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* OR.B (An),Dn */
void REGPARAM2 CPUFUNC(op_8010_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpc (2);
} /* 8 (2/0) */

/* OR.B (An)+,Dn */
void REGPARAM2 CPUFUNC(op_8018_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpc (2);
} /* 8 (2/0) */

/* OR.B -(An),Dn */
void REGPARAM2 CPUFUNC(op_8020_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpc (2);
} /* 10 (2/0) */

/* OR.B (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_8028_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* OR.B (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_8030_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpc (4);
} /* 14 (3/0) */

/* OR.B (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_8038_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* OR.B (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_8039_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpc (6);
} /* 16 (4/0) */

/* OR.B (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_803a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* OR.B (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_803b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpc (4);
} /* 14 (3/0) */

/* OR.B #<data>.B,Dn */
void REGPARAM2 CPUFUNC(op_803c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* OR.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_8040_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* OR.W (An),Dn */
void REGPARAM2 CPUFUNC(op_8050_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4784;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}	m68k_incpc (2);
endlabel4784: ;
} /* 8 (2/0) */

/* OR.W (An)+,Dn */
void REGPARAM2 CPUFUNC(op_8058_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4785;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}	m68k_incpc (2);
endlabel4785: ;
} /* 8 (2/0) */

/* OR.W -(An),Dn */
void REGPARAM2 CPUFUNC(op_8060_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4786;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}	m68k_incpc (2);
endlabel4786: ;
} /* 10 (2/0) */

/* OR.W (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_8068_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4787;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}	m68k_incpc (4);
endlabel4787: ;
} /* 12 (3/0) */

/* OR.W (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_8070_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4788;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}	m68k_incpc (4);
endlabel4788: ;
} /* 14 (3/0) */

/* OR.W (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_8078_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4789;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}	m68k_incpc (4);
endlabel4789: ;
} /* 12 (3/0) */

/* OR.W (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_8079_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4790;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}	m68k_incpc (6);
endlabel4790: ;
} /* 16 (4/0) */

/* OR.W (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_807a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4791;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}	m68k_incpc (4);
endlabel4791: ;
} /* 12 (3/0) */

/* OR.W (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_807b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4792;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}	m68k_incpc (4);
endlabel4792: ;
} /* 14 (3/0) */

/* OR.W #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_807c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* OR.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_8080_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* OR.L (An),Dn */
void REGPARAM2 CPUFUNC(op_8090_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4795;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (src);
}}}}}	m68k_incpc (2);
endlabel4795: ;
} /* 14 (3/0) */

/* OR.L (An)+,Dn */
void REGPARAM2 CPUFUNC(op_8098_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4796;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (src);
}}}}}	m68k_incpc (2);
endlabel4796: ;
} /* 14 (3/0) */

/* OR.L -(An),Dn */
void REGPARAM2 CPUFUNC(op_80a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4797;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (src);
}}}}}	m68k_incpc (2);
endlabel4797: ;
} /* 16 (3/0) */

/* OR.L (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_80a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4798;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (src);
}}}}}	m68k_incpc (4);
endlabel4798: ;
} /* 18 (4/0) */

/* OR.L (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_80b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4799;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (src);
}}}}}	m68k_incpc (4);
endlabel4799: ;
} /* 20 (4/0) */

/* OR.L (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_80b8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4800;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (src);
}}}}}	m68k_incpc (4);
endlabel4800: ;
} /* 18 (4/0) */

/* OR.L (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_80b9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4801;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (src);
}}}}}	m68k_incpc (6);
endlabel4801: ;
} /* 22 (5/0) */

/* OR.L (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_80ba_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4802;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (src);
}}}}}	m68k_incpc (4);
endlabel4802: ;
} /* 18 (4/0) */

/* OR.L (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_80bb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4803;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (src);
}}}}}	m68k_incpc (4);
endlabel4803: ;
} /* 20 (4/0) */

/* OR.L #<data>.L,Dn */
void REGPARAM2 CPUFUNC(op_80bc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (4);
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpc (6);
} /* 16 (3/0) */

/* DIVU.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_80c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
	m68k_incpc (2);
		Exception (5);
		goto endlabel4805;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{		int cycles = (getDivu68kCycles((uae_u32)dst, (uae_u16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpc (2);
	}
}}}}endlabel4805: ;
} /* 4+ (1/0) */

/* DIVU.W (An),Dn */
void REGPARAM2 CPUFUNC(op_80d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4806;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
	m68k_incpc (2);
		Exception (5);
		goto endlabel4806;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{		int cycles = (getDivu68kCycles((uae_u32)dst, (uae_u16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpc (2);
	}
}}}}}}endlabel4806: ;
} /* 8+ (2/0) */

/* DIVU.W (An)+,Dn */
void REGPARAM2 CPUFUNC(op_80d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4807;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
	m68k_incpc (2);
		Exception (5);
		goto endlabel4807;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{		int cycles = (getDivu68kCycles((uae_u32)dst, (uae_u16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpc (2);
	}
}}}}}}endlabel4807: ;
} /* 8+ (2/0) */

/* DIVU.W -(An),Dn */
void REGPARAM2 CPUFUNC(op_80e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4808;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
	m68k_incpc (2);
		Exception (5);
		goto endlabel4808;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{		int cycles = (getDivu68kCycles((uae_u32)dst, (uae_u16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpc (2);
	}
}}}}}}endlabel4808: ;
} /* 10+ (2/0) */

/* DIVU.W (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_80e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4809;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
	m68k_incpc (4);
		Exception (5);
		goto endlabel4809;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{		int cycles = (getDivu68kCycles((uae_u32)dst, (uae_u16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpc (4);
	}
}}}}}}endlabel4809: ;
} /* 12+ (3/0) */

/* DIVU.W (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_80f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4810;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
	m68k_incpc (4);
		Exception (5);
		goto endlabel4810;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{		int cycles = (getDivu68kCycles((uae_u32)dst, (uae_u16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpc (4);
	}
}}}}}}endlabel4810: ;
} /* 14+ (3/0) */

/* DIVU.W (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_80f8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4811;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
	m68k_incpc (4);
		Exception (5);
		goto endlabel4811;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{		int cycles = (getDivu68kCycles((uae_u32)dst, (uae_u16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpc (4);
	}
}}}}}}endlabel4811: ;
} /* 12+ (3/0) */

/* DIVU.W (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_80f9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4812;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
	m68k_incpc (6);
		Exception (5);
		goto endlabel4812;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{		int cycles = (getDivu68kCycles((uae_u32)dst, (uae_u16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpc (6);
	}
}}}}}}endlabel4812: ;
} /* 16+ (4/0) */

/* DIVU.W (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_80fa_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4813;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
	m68k_incpc (4);
		Exception (5);
		goto endlabel4813;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{		int cycles = (getDivu68kCycles((uae_u32)dst, (uae_u16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpc (4);
	}
}}}}}}endlabel4813: ;
} /* 12+ (3/0) */

/* DIVU.W (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_80fb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4814;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
	m68k_incpc (4);
		Exception (5);
		goto endlabel4814;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{		int cycles = (getDivu68kCycles((uae_u32)dst, (uae_u16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpc (4);
	}
}}}}}}endlabel4814: ;
} /* 14+ (3/0) */

/* DIVU.W #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_80fc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	CLEAR_CZNV ();
	if (src == 0) {
	m68k_incpc (4);
		Exception (5);
		goto endlabel4815;
	} else {
		uae_u32 newv = (uae_u32)dst / (uae_u32)(uae_u16)src;
		uae_u32 rem = (uae_u32)dst % (uae_u32)(uae_u16)src;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{		int cycles = (getDivu68kCycles((uae_u32)dst, (uae_u16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
		if (newv > 0xffff) {
			SET_VFLG (1);
			SET_NFLG (1);
		} else {
			optflag_testw ((uae_s16)(newv));
			newv = (newv & 0xffff) | ((uae_u32)rem << 16);
			m68k_dreg (regs, dstreg) = (newv);
		}
	m68k_incpc (4);
	}
}}}}endlabel4815: ;
} /* 8+ (2/0) */

/* SBCD.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_8100_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
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
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* SBCD.B -(An),-(An) */
void REGPARAM2 CPUFUNC(op_8108_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	do_cycles_ce000 (2);
{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
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
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (2);
} /* 18 (3/1) */

/* OR.B Dn,(An) */
void REGPARAM2 CPUFUNC(op_8110_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* OR.B Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_8118_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* OR.B Dn,-(An) */
void REGPARAM2 CPUFUNC(op_8120_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (2);
} /* 14 (2/1) */

/* OR.B Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_8128_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 dst = x_get_byte (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* OR.B Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_8130_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
{	uae_s8 dst = x_get_byte (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* OR.B Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_8138_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 dst = x_get_byte (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* OR.B Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_8139_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	src |= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* OR.W Dn,(An) */
void REGPARAM2 CPUFUNC(op_8150_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4825;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (2);
endlabel4825: ;
} /* 12 (2/1) */

/* OR.W Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_8158_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4826;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) += 2;
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (2);
endlabel4826: ;
} /* 12 (2/1) */

/* OR.W Dn,-(An) */
void REGPARAM2 CPUFUNC(op_8160_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	do_cycles_ce000 (2);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4827;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (2);
endlabel4827: ;
} /* 14 (2/1) */

/* OR.W Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_8168_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4828;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (4);
endlabel4828: ;
} /* 16 (3/1) */

/* OR.W Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_8170_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4829;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (4);
endlabel4829: ;
} /* 18 (3/1) */

/* OR.W Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_8178_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4830;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (4);
endlabel4830: ;
} /* 16 (3/1) */

/* OR.W Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_8179_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4831;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src |= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (6);
endlabel4831: ;
} /* 20 (4/1) */

/* OR.L Dn,(An) */
void REGPARAM2 CPUFUNC(op_8190_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4832;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (2);
endlabel4832: ;
} /* 20 (3/2) */

/* OR.L Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_8198_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4833;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) += 4;
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (2);
endlabel4833: ;
} /* 20 (3/2) */

/* OR.L Dn,-(An) */
void REGPARAM2 CPUFUNC(op_81a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	do_cycles_ce000 (2);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4834;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) = dsta;
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (2);
endlabel4834: ;
} /* 22 (3/2) */

/* OR.L Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_81a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4835;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (4);
endlabel4835: ;
} /* 24 (4/2) */

/* OR.L Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_81b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4836;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (4);
endlabel4836: ;
} /* 26 (4/2) */

/* OR.L Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_81b8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4837;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (4);
endlabel4837: ;
} /* 24 (4/2) */

/* OR.L Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_81b9_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4838;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src |= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (6);
endlabel4838: ;
} /* 28 (5/2) */

/* DIVS.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_81c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
	m68k_incpc (2);
		Exception (5);
		goto endlabel4839;
	}
	CLEAR_CZNV ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{		int cycles = (getDivs68kCycles((uae_s32)dst, (uae_s16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
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
	m68k_incpc (2);
}}}}endlabel4839: ;
} /* 4+ (1/0) */

/* DIVS.W (An),Dn */
void REGPARAM2 CPUFUNC(op_81d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4840;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
	m68k_incpc (2);
		Exception (5);
		goto endlabel4840;
	}
	CLEAR_CZNV ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{		int cycles = (getDivs68kCycles((uae_s32)dst, (uae_s16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
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
	m68k_incpc (2);
}}}}}}endlabel4840: ;
} /* 8+ (2/0) */

/* DIVS.W (An)+,Dn */
void REGPARAM2 CPUFUNC(op_81d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4841;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
	m68k_incpc (2);
		Exception (5);
		goto endlabel4841;
	}
	CLEAR_CZNV ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{		int cycles = (getDivs68kCycles((uae_s32)dst, (uae_s16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
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
	m68k_incpc (2);
}}}}}}endlabel4841: ;
} /* 8+ (2/0) */

/* DIVS.W -(An),Dn */
void REGPARAM2 CPUFUNC(op_81e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4842;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
	m68k_incpc (2);
		Exception (5);
		goto endlabel4842;
	}
	CLEAR_CZNV ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{		int cycles = (getDivs68kCycles((uae_s32)dst, (uae_s16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
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
	m68k_incpc (2);
}}}}}}endlabel4842: ;
} /* 10+ (2/0) */

/* DIVS.W (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_81e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4843;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
	m68k_incpc (4);
		Exception (5);
		goto endlabel4843;
	}
	CLEAR_CZNV ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{		int cycles = (getDivs68kCycles((uae_s32)dst, (uae_s16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
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
	m68k_incpc (4);
}}}}}}endlabel4843: ;
} /* 12+ (3/0) */

/* DIVS.W (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_81f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4844;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
	m68k_incpc (4);
		Exception (5);
		goto endlabel4844;
	}
	CLEAR_CZNV ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{		int cycles = (getDivs68kCycles((uae_s32)dst, (uae_s16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
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
	m68k_incpc (4);
}}}}}}endlabel4844: ;
} /* 14+ (3/0) */

/* DIVS.W (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_81f8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4845;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
	m68k_incpc (4);
		Exception (5);
		goto endlabel4845;
	}
	CLEAR_CZNV ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{		int cycles = (getDivs68kCycles((uae_s32)dst, (uae_s16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
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
	m68k_incpc (4);
}}}}}}endlabel4845: ;
} /* 12+ (3/0) */

/* DIVS.W (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_81f9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4846;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
	m68k_incpc (6);
		Exception (5);
		goto endlabel4846;
	}
	CLEAR_CZNV ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{		int cycles = (getDivs68kCycles((uae_s32)dst, (uae_s16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
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
	m68k_incpc (6);
}}}}}}endlabel4846: ;
} /* 16+ (4/0) */

/* DIVS.W (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_81fa_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4847;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
	m68k_incpc (4);
		Exception (5);
		goto endlabel4847;
	}
	CLEAR_CZNV ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{		int cycles = (getDivs68kCycles((uae_s32)dst, (uae_s16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
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
	m68k_incpc (4);
}}}}}}endlabel4847: ;
} /* 12+ (3/0) */

/* DIVS.W (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_81fb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4848;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
	m68k_incpc (4);
		Exception (5);
		goto endlabel4848;
	}
	CLEAR_CZNV ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{		int cycles = (getDivs68kCycles((uae_s32)dst, (uae_s16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
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
	m68k_incpc (4);
}}}}}}endlabel4848: ;
} /* 14+ (3/0) */

/* DIVS.W #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_81fc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	if (src == 0) {
	m68k_incpc (4);
		Exception (5);
		goto endlabel4849;
	}
	CLEAR_CZNV ();
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{		int cycles = (getDivs68kCycles((uae_s32)dst, (uae_s16)src));
		if (cycles > 0) do_cycles_ce000 (cycles);
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
	m68k_incpc (4);
}}}}endlabel4849: ;
} /* 8+ (2/0) */

/* SUB.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_9000_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* SUB.B (An),Dn */
void REGPARAM2 CPUFUNC(op_9010_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpc (2);
} /* 8 (2/0) */

/* SUB.B (An)+,Dn */
void REGPARAM2 CPUFUNC(op_9018_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpc (2);
} /* 8 (2/0) */

/* SUB.B -(An),Dn */
void REGPARAM2 CPUFUNC(op_9020_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpc (2);
} /* 10 (2/0) */

/* SUB.B (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_9028_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* SUB.B (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_9030_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpc (4);
} /* 14 (3/0) */

/* SUB.B (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_9038_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* SUB.B (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_9039_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpc (6);
} /* 16 (4/0) */

/* SUB.B (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_903a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* SUB.B (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_903b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpc (4);
} /* 14 (3/0) */

/* SUB.B #<data>.B,Dn */
void REGPARAM2 CPUFUNC(op_903c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* SUB.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_9040_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* SUB.W An,Dn */
void REGPARAM2 CPUFUNC(op_9048_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* SUB.W (An),Dn */
void REGPARAM2 CPUFUNC(op_9050_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4863;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}	m68k_incpc (2);
endlabel4863: ;
} /* 8 (2/0) */

/* SUB.W (An)+,Dn */
void REGPARAM2 CPUFUNC(op_9058_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4864;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}	m68k_incpc (2);
endlabel4864: ;
} /* 8 (2/0) */

/* SUB.W -(An),Dn */
void REGPARAM2 CPUFUNC(op_9060_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4865;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}	m68k_incpc (2);
endlabel4865: ;
} /* 10 (2/0) */

/* SUB.W (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_9068_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4866;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}	m68k_incpc (4);
endlabel4866: ;
} /* 12 (3/0) */

/* SUB.W (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_9070_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4867;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}	m68k_incpc (4);
endlabel4867: ;
} /* 14 (3/0) */

/* SUB.W (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_9078_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4868;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}	m68k_incpc (4);
endlabel4868: ;
} /* 12 (3/0) */

/* SUB.W (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_9079_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4869;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}	m68k_incpc (6);
endlabel4869: ;
} /* 16 (4/0) */

/* SUB.W (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_907a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4870;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}	m68k_incpc (4);
endlabel4870: ;
} /* 12 (3/0) */

/* SUB.W (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_907b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4871;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}	m68k_incpc (4);
endlabel4871: ;
} /* 14 (3/0) */

/* SUB.W #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_907c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* SUB.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_9080_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* SUB.L An,Dn */
void REGPARAM2 CPUFUNC(op_9088_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* SUB.L (An),Dn */
void REGPARAM2 CPUFUNC(op_9090_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4875;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}	m68k_incpc (2);
endlabel4875: ;
} /* 14 (3/0) */

/* SUB.L (An)+,Dn */
void REGPARAM2 CPUFUNC(op_9098_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4876;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}	m68k_incpc (2);
endlabel4876: ;
} /* 14 (3/0) */

/* SUB.L -(An),Dn */
void REGPARAM2 CPUFUNC(op_90a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4877;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}	m68k_incpc (2);
endlabel4877: ;
} /* 16 (3/0) */

/* SUB.L (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_90a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4878;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}	m68k_incpc (4);
endlabel4878: ;
} /* 18 (4/0) */

/* SUB.L (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_90b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4879;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}	m68k_incpc (4);
endlabel4879: ;
} /* 20 (4/0) */

/* SUB.L (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_90b8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4880;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}	m68k_incpc (4);
endlabel4880: ;
} /* 18 (4/0) */

/* SUB.L (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_90b9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4881;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}	m68k_incpc (6);
endlabel4881: ;
} /* 22 (5/0) */

/* SUB.L (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_90ba_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4882;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}	m68k_incpc (4);
endlabel4882: ;
} /* 18 (4/0) */

/* SUB.L (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_90bb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4883;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}	m68k_incpc (4);
endlabel4883: ;
} /* 20 (4/0) */

/* SUB.L #<data>.L,Dn */
void REGPARAM2 CPUFUNC(op_90bc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (4);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpc (6);
} /* 16 (3/0) */

/* SUBA.W Dn,An */
void REGPARAM2 CPUFUNC(op_90c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* SUBA.W An,An */
void REGPARAM2 CPUFUNC(op_90c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* SUBA.W (An),An */
void REGPARAM2 CPUFUNC(op_90d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4887;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (2);
endlabel4887: ;
} /* 12 (2/0) */

/* SUBA.W (An)+,An */
void REGPARAM2 CPUFUNC(op_90d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4888;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (2);
endlabel4888: ;
} /* 12 (2/0) */

/* SUBA.W -(An),An */
void REGPARAM2 CPUFUNC(op_90e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4889;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (2);
endlabel4889: ;
} /* 14 (2/0) */

/* SUBA.W (d16,An),An */
void REGPARAM2 CPUFUNC(op_90e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4890;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel4890: ;
} /* 16 (3/0) */

/* SUBA.W (d8,An,Xn),An */
void REGPARAM2 CPUFUNC(op_90f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4891;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel4891: ;
} /* 18 (3/0) */

/* SUBA.W (xxx).W,An */
void REGPARAM2 CPUFUNC(op_90f8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4892;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel4892: ;
} /* 16 (3/0) */

/* SUBA.W (xxx).L,An */
void REGPARAM2 CPUFUNC(op_90f9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4893;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (6);
endlabel4893: ;
} /* 20 (4/0) */

/* SUBA.W (d16,PC),An */
void REGPARAM2 CPUFUNC(op_90fa_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4894;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel4894: ;
} /* 16 (3/0) */

/* SUBA.W (d8,PC,Xn),An */
void REGPARAM2 CPUFUNC(op_90fb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4895;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel4895: ;
} /* 18 (3/0) */

/* SUBA.W #<data>.W,An */
void REGPARAM2 CPUFUNC(op_90fc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpc (4);
} /* 12 (2/0) */

/* SUBX.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_9100_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
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
}}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* SUBX.B -(An),-(An) */
void REGPARAM2 CPUFUNC(op_9108_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	do_cycles_ce000 (2);
{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	x_put_byte (dsta, newv);
}}}}}}}	m68k_incpc (2);
} /* 18 (3/1) */

/* SUB.B Dn,(An) */
void REGPARAM2 CPUFUNC(op_9110_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* SUB.B Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_9118_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* SUB.B Dn,-(An) */
void REGPARAM2 CPUFUNC(op_9120_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (2);
} /* 14 (2/1) */

/* SUB.B Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_9128_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* SUB.B Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_9130_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* SUB.B Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_9138_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* SUB.B Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_9139_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_subb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* SUBX.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_9140_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
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
}}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* SUBX.W -(An),-(An) */
void REGPARAM2 CPUFUNC(op_9148_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	do_cycles_ce000 (2);
{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4907;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4907;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	x_put_word (dsta, newv);
}}}}}}}}}	m68k_incpc (2);
endlabel4907: ;
} /* 18 (3/1) */

/* SUB.W Dn,(An) */
void REGPARAM2 CPUFUNC(op_9150_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4908;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (2);
endlabel4908: ;
} /* 12 (2/1) */

/* SUB.W Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_9158_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4909;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (2);
endlabel4909: ;
} /* 12 (2/1) */

/* SUB.W Dn,-(An) */
void REGPARAM2 CPUFUNC(op_9160_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	do_cycles_ce000 (2);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4910;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (2);
endlabel4910: ;
} /* 14 (2/1) */

/* SUB.W Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_9168_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4911;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (4);
endlabel4911: ;
} /* 16 (3/1) */

/* SUB.W Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_9170_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4912;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (4);
endlabel4912: ;
} /* 18 (3/1) */

/* SUB.W Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_9178_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4913;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (4);
endlabel4913: ;
} /* 16 (3/1) */

/* SUB.W Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_9179_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4914;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_subw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (6);
endlabel4914: ;
} /* 20 (4/1) */

/* SUBX.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_9180_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
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
}}}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* SUBX.L -(An),-(An) */
void REGPARAM2 CPUFUNC(op_9188_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	do_cycles_ce000 (2);
{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4916;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4916;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = dst - src - (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgo) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgn) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	x_put_word (dsta, newv >> 16); x_put_word (dsta + 2, newv);
}}}}}}}}}	m68k_incpc (2);
endlabel4916: ;
} /* 30 (5/2) */

/* SUB.L Dn,(An) */
void REGPARAM2 CPUFUNC(op_9190_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4917;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (2);
endlabel4917: ;
} /* 20 (3/2) */

/* SUB.L Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_9198_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4918;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) += 4;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (2);
endlabel4918: ;
} /* 20 (3/2) */

/* SUB.L Dn,-(An) */
void REGPARAM2 CPUFUNC(op_91a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	do_cycles_ce000 (2);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4919;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (2);
endlabel4919: ;
} /* 22 (3/2) */

/* SUB.L Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_91a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4920;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (4);
endlabel4920: ;
} /* 24 (4/2) */

/* SUB.L Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_91b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4921;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (4);
endlabel4921: ;
} /* 26 (4/2) */

/* SUB.L Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_91b8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4922;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (4);
endlabel4922: ;
} /* 24 (4/2) */

/* SUB.L Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_91b9_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4923;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_subl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (6);
endlabel4923: ;
} /* 28 (5/2) */

/* SUBA.L Dn,An */
void REGPARAM2 CPUFUNC(op_91c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* SUBA.L An,An */
void REGPARAM2 CPUFUNC(op_91c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* SUBA.L (An),An */
void REGPARAM2 CPUFUNC(op_91d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4926;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (2);
endlabel4926: ;
} /* 14 (3/0) */

/* SUBA.L (An)+,An */
void REGPARAM2 CPUFUNC(op_91d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4927;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (2);
endlabel4927: ;
} /* 14 (3/0) */

/* SUBA.L -(An),An */
void REGPARAM2 CPUFUNC(op_91e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4928;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (2);
endlabel4928: ;
} /* 16 (3/0) */

/* SUBA.L (d16,An),An */
void REGPARAM2 CPUFUNC(op_91e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4929;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel4929: ;
} /* 18 (4/0) */

/* SUBA.L (d8,An,Xn),An */
void REGPARAM2 CPUFUNC(op_91f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4930;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel4930: ;
} /* 20 (4/0) */

/* SUBA.L (xxx).W,An */
void REGPARAM2 CPUFUNC(op_91f8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4931;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel4931: ;
} /* 18 (4/0) */

/* SUBA.L (xxx).L,An */
void REGPARAM2 CPUFUNC(op_91f9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4932;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (6);
endlabel4932: ;
} /* 22 (5/0) */

/* SUBA.L (d16,PC),An */
void REGPARAM2 CPUFUNC(op_91fa_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4933;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel4933: ;
} /* 18 (4/0) */

/* SUBA.L (d8,PC,Xn),An */
void REGPARAM2 CPUFUNC(op_91fb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4934;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel4934: ;
} /* 20 (4/0) */

/* SUBA.L #<data>.L,An */
void REGPARAM2 CPUFUNC(op_91fc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst - src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpc (6);
} /* 16 (3/0) */

/* CMP.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_b000_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* CMP.B (An),Dn */
void REGPARAM2 CPUFUNC(op_b010_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpc (2);
} /* 8 (2/0) */

/* CMP.B (An)+,Dn */
void REGPARAM2 CPUFUNC(op_b018_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpc (2);
} /* 8 (2/0) */

/* CMP.B -(An),Dn */
void REGPARAM2 CPUFUNC(op_b020_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpc (2);
} /* 10 (2/0) */

/* CMP.B (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_b028_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* CMP.B (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_b030_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpc (4);
} /* 14 (3/0) */

/* CMP.B (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_b038_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* CMP.B (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_b039_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpc (6);
} /* 16 (4/0) */

/* CMP.B (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_b03a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* CMP.B (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_b03b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}	m68k_incpc (4);
} /* 14 (3/0) */

/* CMP.B #<data>.B,Dn */
void REGPARAM2 CPUFUNC(op_b03c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* CMP.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_b040_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* CMP.W An,Dn */
void REGPARAM2 CPUFUNC(op_b048_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* CMP.W (An),Dn */
void REGPARAM2 CPUFUNC(op_b050_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4949;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpc (2);
endlabel4949: ;
} /* 8 (2/0) */

/* CMP.W (An)+,Dn */
void REGPARAM2 CPUFUNC(op_b058_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4950;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpc (2);
endlabel4950: ;
} /* 8 (2/0) */

/* CMP.W -(An),Dn */
void REGPARAM2 CPUFUNC(op_b060_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4951;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpc (2);
endlabel4951: ;
} /* 10 (2/0) */

/* CMP.W (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_b068_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4952;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpc (4);
endlabel4952: ;
} /* 12 (3/0) */

/* CMP.W (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_b070_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4953;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpc (4);
endlabel4953: ;
} /* 14 (3/0) */

/* CMP.W (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_b078_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4954;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpc (4);
endlabel4954: ;
} /* 12 (3/0) */

/* CMP.W (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_b079_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4955;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpc (6);
endlabel4955: ;
} /* 16 (4/0) */

/* CMP.W (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_b07a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4956;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpc (4);
endlabel4956: ;
} /* 12 (3/0) */

/* CMP.W (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_b07b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4957;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}	m68k_incpc (4);
endlabel4957: ;
} /* 14 (3/0) */

/* CMP.W #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_b07c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* CMP.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_b080_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* CMP.L An,Dn */
void REGPARAM2 CPUFUNC(op_b088_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpc (2);
} /* 6 (1/0) */

#endif

#ifdef PART_7
/* CMP.L (An),Dn */
void REGPARAM2 CPUFUNC(op_b090_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4961;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (2);
endlabel4961: ;
} /* 14 (3/0) */

/* CMP.L (An)+,Dn */
void REGPARAM2 CPUFUNC(op_b098_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4962;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (2);
endlabel4962: ;
} /* 14 (3/0) */

/* CMP.L -(An),Dn */
void REGPARAM2 CPUFUNC(op_b0a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4963;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (2);
endlabel4963: ;
} /* 16 (3/0) */

/* CMP.L (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_b0a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4964;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (4);
endlabel4964: ;
} /* 18 (4/0) */

/* CMP.L (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_b0b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4965;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (4);
endlabel4965: ;
} /* 20 (4/0) */

/* CMP.L (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_b0b8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4966;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (4);
endlabel4966: ;
} /* 18 (4/0) */

/* CMP.L (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_b0b9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4967;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (6);
endlabel4967: ;
} /* 22 (5/0) */

/* CMP.L (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_b0ba_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4968;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (4);
endlabel4968: ;
} /* 18 (4/0) */

/* CMP.L (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_b0bb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4969;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (4);
endlabel4969: ;
} /* 20 (4/0) */

/* CMP.L #<data>.L,Dn */
void REGPARAM2 CPUFUNC(op_b0bc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpc (6);
} /* 14 (3/0) */

/* CMPA.W Dn,An */
void REGPARAM2 CPUFUNC(op_b0c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* CMPA.W An,An */
void REGPARAM2 CPUFUNC(op_b0c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* CMPA.W (An),An */
void REGPARAM2 CPUFUNC(op_b0d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4973;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (2);
endlabel4973: ;
} /* 10 (2/0) */

/* CMPA.W (An)+,An */
void REGPARAM2 CPUFUNC(op_b0d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4974;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (2);
endlabel4974: ;
} /* 10 (2/0) */

/* CMPA.W -(An),An */
void REGPARAM2 CPUFUNC(op_b0e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4975;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (2);
endlabel4975: ;
} /* 12 (2/0) */

/* CMPA.W (d16,An),An */
void REGPARAM2 CPUFUNC(op_b0e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4976;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (4);
endlabel4976: ;
} /* 14 (3/0) */

/* CMPA.W (d8,An,Xn),An */
void REGPARAM2 CPUFUNC(op_b0f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4977;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (4);
endlabel4977: ;
} /* 16 (3/0) */

/* CMPA.W (xxx).W,An */
void REGPARAM2 CPUFUNC(op_b0f8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4978;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (4);
endlabel4978: ;
} /* 14 (3/0) */

/* CMPA.W (xxx).L,An */
void REGPARAM2 CPUFUNC(op_b0f9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4979;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (6);
endlabel4979: ;
} /* 18 (4/0) */

/* CMPA.W (d16,PC),An */
void REGPARAM2 CPUFUNC(op_b0fa_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4980;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (4);
endlabel4980: ;
} /* 14 (3/0) */

/* CMPA.W (d8,PC,Xn),An */
void REGPARAM2 CPUFUNC(op_b0fb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4981;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (4);
endlabel4981: ;
} /* 16 (3/0) */

/* CMPA.W #<data>.W,An */
void REGPARAM2 CPUFUNC(op_b0fc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpc (4);
} /* 10 (2/0) */

/* EOR.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_b100_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* CMPM.B (An)+,(An)+ */
void REGPARAM2 CPUFUNC(op_b108_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	optflag_cmpb ((uae_s8)(src), (uae_s8)(dst));
}}}}}}	m68k_incpc (2);
} /* 12 (3/0) */

/* EOR.B Dn,(An) */
void REGPARAM2 CPUFUNC(op_b110_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* EOR.B Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_b118_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* EOR.B Dn,-(An) */
void REGPARAM2 CPUFUNC(op_b120_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (2);
} /* 14 (2/1) */

/* EOR.B Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_b128_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 dst = x_get_byte (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* EOR.B Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_b130_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
{	uae_s8 dst = x_get_byte (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* EOR.B Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_b138_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 dst = x_get_byte (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* EOR.B Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_b139_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	src ^= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* EOR.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_b140_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* CMPM.W (An)+,(An)+ */
void REGPARAM2 CPUFUNC(op_b148_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel4993;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4993;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	optflag_cmpw ((uae_s16)(src), (uae_s16)(dst));
}}}}}}}}	m68k_incpc (2);
endlabel4993: ;
} /* 12 (3/0) */

/* EOR.W Dn,(An) */
void REGPARAM2 CPUFUNC(op_b150_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4994;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (2);
endlabel4994: ;
} /* 12 (2/1) */

/* EOR.W Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_b158_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4995;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) += 2;
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (2);
endlabel4995: ;
} /* 12 (2/1) */

/* EOR.W Dn,-(An) */
void REGPARAM2 CPUFUNC(op_b160_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	do_cycles_ce000 (2);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4996;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (2);
endlabel4996: ;
} /* 14 (2/1) */

/* EOR.W Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_b168_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4997;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (4);
endlabel4997: ;
} /* 16 (3/1) */

/* EOR.W Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_b170_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4998;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (4);
endlabel4998: ;
} /* 18 (3/1) */

/* EOR.W Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_b178_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel4999;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (4);
endlabel4999: ;
} /* 16 (3/1) */

/* EOR.W Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_b179_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5000;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src ^= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (6);
endlabel5000: ;
} /* 20 (4/1) */

/* EOR.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_b180_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* CMPM.L (An)+,(An)+ */
void REGPARAM2 CPUFUNC(op_b188_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5002;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5002;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) += 4;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}}}	m68k_incpc (2);
endlabel5002: ;
} /* 20 (5/0) */

/* EOR.L Dn,(An) */
void REGPARAM2 CPUFUNC(op_b190_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5003;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (2);
endlabel5003: ;
} /* 20 (3/2) */

/* EOR.L Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_b198_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5004;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) += 4;
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (2);
endlabel5004: ;
} /* 20 (3/2) */

/* EOR.L Dn,-(An) */
void REGPARAM2 CPUFUNC(op_b1a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	do_cycles_ce000 (2);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5005;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) = dsta;
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (2);
endlabel5005: ;
} /* 22 (3/2) */

/* EOR.L Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_b1a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5006;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (4);
endlabel5006: ;
} /* 24 (4/2) */

/* EOR.L Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_b1b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5007;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (4);
endlabel5007: ;
} /* 26 (4/2) */

/* EOR.L Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_b1b8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5008;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (4);
endlabel5008: ;
} /* 24 (4/2) */

/* EOR.L Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_b1b9_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5009;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src ^= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (6);
endlabel5009: ;
} /* 28 (5/2) */

/* CMPA.L Dn,An */
void REGPARAM2 CPUFUNC(op_b1c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* CMPA.L An,An */
void REGPARAM2 CPUFUNC(op_b1c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* CMPA.L (An),An */
void REGPARAM2 CPUFUNC(op_b1d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5012;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (2);
endlabel5012: ;
} /* 14 (3/0) */

/* CMPA.L (An)+,An */
void REGPARAM2 CPUFUNC(op_b1d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5013;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (2);
endlabel5013: ;
} /* 14 (3/0) */

/* CMPA.L -(An),An */
void REGPARAM2 CPUFUNC(op_b1e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5014;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (2);
endlabel5014: ;
} /* 16 (3/0) */

/* CMPA.L (d16,An),An */
void REGPARAM2 CPUFUNC(op_b1e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5015;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (4);
endlabel5015: ;
} /* 18 (4/0) */

/* CMPA.L (d8,An,Xn),An */
void REGPARAM2 CPUFUNC(op_b1f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5016;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (4);
endlabel5016: ;
} /* 20 (4/0) */

/* CMPA.L (xxx).W,An */
void REGPARAM2 CPUFUNC(op_b1f8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5017;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (4);
endlabel5017: ;
} /* 18 (4/0) */

/* CMPA.L (xxx).L,An */
void REGPARAM2 CPUFUNC(op_b1f9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5018;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (6);
endlabel5018: ;
} /* 22 (5/0) */

/* CMPA.L (d16,PC),An */
void REGPARAM2 CPUFUNC(op_b1fa_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5019;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (4);
endlabel5019: ;
} /* 18 (4/0) */

/* CMPA.L (d8,PC,Xn),An */
void REGPARAM2 CPUFUNC(op_b1fb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5020;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}}}	m68k_incpc (4);
endlabel5020: ;
} /* 20 (4/0) */

/* CMPA.L #<data>.L,An */
void REGPARAM2 CPUFUNC(op_b1fc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (2);
{	optflag_cmpl ((uae_s32)(src), (uae_s32)(dst));
}}}}	m68k_incpc (6);
} /* 14 (3/0) */

/* AND.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_c000_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* AND.B (An),Dn */
void REGPARAM2 CPUFUNC(op_c010_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpc (2);
} /* 8 (2/0) */

/* AND.B (An)+,Dn */
void REGPARAM2 CPUFUNC(op_c018_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpc (2);
} /* 8 (2/0) */

/* AND.B -(An),Dn */
void REGPARAM2 CPUFUNC(op_c020_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpc (2);
} /* 10 (2/0) */

/* AND.B (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_c028_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* AND.B (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_c030_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpc (4);
} /* 14 (3/0) */

/* AND.B (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_c038_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* AND.B (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_c039_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpc (6);
} /* 16 (4/0) */

/* AND.B (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_c03a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* AND.B (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_c03b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}}	m68k_incpc (4);
} /* 14 (3/0) */

/* AND.B #<data>.B,Dn */
void REGPARAM2 CPUFUNC(op_c03c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((src) & 0xff);
}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* AND.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_c040_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* AND.W (An),Dn */
void REGPARAM2 CPUFUNC(op_c050_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5034;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}	m68k_incpc (2);
endlabel5034: ;
} /* 8 (2/0) */

/* AND.W (An)+,Dn */
void REGPARAM2 CPUFUNC(op_c058_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5035;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}	m68k_incpc (2);
endlabel5035: ;
} /* 8 (2/0) */

/* AND.W -(An),Dn */
void REGPARAM2 CPUFUNC(op_c060_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5036;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}	m68k_incpc (2);
endlabel5036: ;
} /* 10 (2/0) */

/* AND.W (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_c068_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5037;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}	m68k_incpc (4);
endlabel5037: ;
} /* 12 (3/0) */

/* AND.W (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_c070_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5038;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}	m68k_incpc (4);
endlabel5038: ;
} /* 14 (3/0) */

/* AND.W (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_c078_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5039;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}	m68k_incpc (4);
endlabel5039: ;
} /* 12 (3/0) */

/* AND.W (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_c079_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5040;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}	m68k_incpc (6);
endlabel5040: ;
} /* 16 (4/0) */

/* AND.W (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_c07a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5041;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}	m68k_incpc (4);
endlabel5041: ;
} /* 12 (3/0) */

/* AND.W (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_c07b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5042;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}}}	m68k_incpc (4);
endlabel5042: ;
} /* 14 (3/0) */

/* AND.W #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_c07c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((src) & 0xffff);
}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* AND.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_c080_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* AND.L (An),Dn */
void REGPARAM2 CPUFUNC(op_c090_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5045;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (src);
}}}}}	m68k_incpc (2);
endlabel5045: ;
} /* 14 (3/0) */

/* AND.L (An)+,Dn */
void REGPARAM2 CPUFUNC(op_c098_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5046;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (src);
}}}}}	m68k_incpc (2);
endlabel5046: ;
} /* 14 (3/0) */

/* AND.L -(An),Dn */
void REGPARAM2 CPUFUNC(op_c0a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5047;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (src);
}}}}}	m68k_incpc (2);
endlabel5047: ;
} /* 16 (3/0) */

/* AND.L (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_c0a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5048;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (src);
}}}}}	m68k_incpc (4);
endlabel5048: ;
} /* 18 (4/0) */

/* AND.L (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_c0b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5049;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (src);
}}}}}	m68k_incpc (4);
endlabel5049: ;
} /* 20 (4/0) */

/* AND.L (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_c0b8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5050;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (src);
}}}}}	m68k_incpc (4);
endlabel5050: ;
} /* 18 (4/0) */

/* AND.L (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_c0b9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5051;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (src);
}}}}}	m68k_incpc (6);
endlabel5051: ;
} /* 22 (5/0) */

/* AND.L (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_c0ba_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5052;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (src);
}}}}}	m68k_incpc (4);
endlabel5052: ;
} /* 18 (4/0) */

/* AND.L (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_c0bb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5053;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (src);
}}}}}	m68k_incpc (4);
endlabel5053: ;
} /* 20 (4/0) */

/* AND.L #<data>.L,Dn */
void REGPARAM2 CPUFUNC(op_c0bc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (4);
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpc (6);
} /* 16 (3/0) */

/* MULU.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_c0c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	int cycles = 38 - 4, bits;
	optflag_testl ((uae_s32)(newv));
	for(bits = 0; bits < 16 && src; bits++, src >>= 1)
		if (src & 1) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpc (2);
}}}}} /* 38+ (1/0) */

/* MULU.W (An),Dn */
void REGPARAM2 CPUFUNC(op_c0d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5056;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	int cycles = 38 - 4, bits;
	optflag_testl ((uae_s32)(newv));
	for(bits = 0; bits < 16 && src; bits++, src >>= 1)
		if (src & 1) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpc (2);
}}}}}}endlabel5056: ;
} /* 42+ (2/0) */

/* MULU.W (An)+,Dn */
void REGPARAM2 CPUFUNC(op_c0d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5057;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	int cycles = 38 - 4, bits;
	optflag_testl ((uae_s32)(newv));
	for(bits = 0; bits < 16 && src; bits++, src >>= 1)
		if (src & 1) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpc (2);
}}}}}}endlabel5057: ;
} /* 42+ (2/0) */

/* MULU.W -(An),Dn */
void REGPARAM2 CPUFUNC(op_c0e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5058;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	int cycles = 38 - 4, bits;
	optflag_testl ((uae_s32)(newv));
	for(bits = 0; bits < 16 && src; bits++, src >>= 1)
		if (src & 1) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpc (2);
}}}}}}endlabel5058: ;
} /* 44+ (2/0) */

/* MULU.W (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_c0e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5059;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	int cycles = 38 - 4, bits;
	optflag_testl ((uae_s32)(newv));
	for(bits = 0; bits < 16 && src; bits++, src >>= 1)
		if (src & 1) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpc (4);
}}}}}}endlabel5059: ;
} /* 46+ (3/0) */

/* MULU.W (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_c0f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5060;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	int cycles = 38 - 4, bits;
	optflag_testl ((uae_s32)(newv));
	for(bits = 0; bits < 16 && src; bits++, src >>= 1)
		if (src & 1) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpc (4);
}}}}}}endlabel5060: ;
} /* 48+ (3/0) */

/* MULU.W (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_c0f8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5061;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	int cycles = 38 - 4, bits;
	optflag_testl ((uae_s32)(newv));
	for(bits = 0; bits < 16 && src; bits++, src >>= 1)
		if (src & 1) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpc (4);
}}}}}}endlabel5061: ;
} /* 46+ (3/0) */

/* MULU.W (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_c0f9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5062;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	int cycles = 38 - 4, bits;
	optflag_testl ((uae_s32)(newv));
	for(bits = 0; bits < 16 && src; bits++, src >>= 1)
		if (src & 1) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpc (6);
}}}}}}endlabel5062: ;
} /* 50+ (4/0) */

/* MULU.W (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_c0fa_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5063;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	int cycles = 38 - 4, bits;
	optflag_testl ((uae_s32)(newv));
	for(bits = 0; bits < 16 && src; bits++, src >>= 1)
		if (src & 1) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpc (4);
}}}}}}endlabel5063: ;
} /* 46+ (3/0) */

/* MULU.W (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_c0fb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5064;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	int cycles = 38 - 4, bits;
	optflag_testl ((uae_s32)(newv));
	for(bits = 0; bits < 16 && src; bits++, src >>= 1)
		if (src & 1) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpc (4);
}}}}}}endlabel5064: ;
} /* 48+ (3/0) */

/* MULU.W #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_c0fc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = (uae_u32)(uae_u16)dst * (uae_u32)(uae_u16)src;
	int cycles = 38 - 4, bits;
	optflag_testl ((uae_s32)(newv));
	for(bits = 0; bits < 16 && src; bits++, src >>= 1)
		if (src & 1) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
	m68k_incpc (4);
}}}}} /* 42+ (2/0) */

/* ABCD.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_c100_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
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
	do_cycles_ce000 (2);
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* ABCD.B -(An),-(An) */
void REGPARAM2 CPUFUNC(op_c108_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	do_cycles_ce000 (2);
{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
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
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (2);
} /* 18 (3/1) */

/* AND.B Dn,(An) */
void REGPARAM2 CPUFUNC(op_c110_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* AND.B Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_c118_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* AND.B Dn,-(An) */
void REGPARAM2 CPUFUNC(op_c120_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (2);
} /* 14 (2/1) */

/* AND.B Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_c128_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 dst = x_get_byte (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* AND.B Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_c130_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
{	uae_s8 dst = x_get_byte (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* AND.B Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_c138_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 dst = x_get_byte (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* AND.B Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_c139_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	src &= dst;
	optflag_testb ((uae_s8)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_byte (dsta, src);
}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* EXG.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_c140_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
	m68k_dreg (regs, srcreg) = (dst);
	m68k_dreg (regs, dstreg) = (src);
}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* EXG.L An,An */
void REGPARAM2 CPUFUNC(op_c148_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
	m68k_areg (regs, srcreg) = (dst);
	m68k_areg (regs, dstreg) = (src);
}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* AND.W Dn,(An) */
void REGPARAM2 CPUFUNC(op_c150_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5077;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (2);
endlabel5077: ;
} /* 12 (2/1) */

/* AND.W Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_c158_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5078;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) += 2;
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (2);
endlabel5078: ;
} /* 12 (2/1) */

/* AND.W Dn,-(An) */
void REGPARAM2 CPUFUNC(op_c160_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	do_cycles_ce000 (2);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5079;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (2);
endlabel5079: ;
} /* 14 (2/1) */

/* AND.W Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_c168_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5080;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (4);
endlabel5080: ;
} /* 16 (3/1) */

/* AND.W Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_c170_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5081;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (4);
endlabel5081: ;
} /* 18 (3/1) */

/* AND.W Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_c178_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5082;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (4);
endlabel5082: ;
} /* 16 (3/1) */

/* AND.W Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_c179_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5083;
	}
{{	uae_s16 dst = x_get_word (dsta);
	src &= dst;
	optflag_testw ((uae_s16)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta, src);
}}}}}	m68k_incpc (6);
endlabel5083: ;
} /* 20 (4/1) */

/* EXG.L Dn,An */
void REGPARAM2 CPUFUNC(op_c188_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
	m68k_dreg (regs, srcreg) = (dst);
	m68k_areg (regs, dstreg) = (src);
}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* AND.L Dn,(An) */
void REGPARAM2 CPUFUNC(op_c190_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5085;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (2);
endlabel5085: ;
} /* 20 (3/2) */

/* AND.L Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_c198_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5086;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) += 4;
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (2);
endlabel5086: ;
} /* 20 (3/2) */

/* AND.L Dn,-(An) */
void REGPARAM2 CPUFUNC(op_c1a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	do_cycles_ce000 (2);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5087;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) = dsta;
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (2);
endlabel5087: ;
} /* 22 (3/2) */

/* AND.L Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_c1a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5088;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (4);
endlabel5088: ;
} /* 24 (4/2) */

/* AND.L Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_c1b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5089;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (4);
endlabel5089: ;
} /* 26 (4/2) */

/* AND.L Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_c1b8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5090;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (4);
endlabel5090: ;
} /* 24 (4/2) */

/* AND.L Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_c1b9_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5091;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	src &= dst;
	optflag_testl ((uae_s32)(src));
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	x_put_word (dsta + 2, src); x_put_word (dsta, src >> 16);
}}}}}	m68k_incpc (6);
endlabel5091: ;
} /* 28 (5/2) */

/* MULS.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_c1c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	int cycles = 38 - 4, bits;
	uae_u32 usrc;
	optflag_testl ((uae_s32)(newv));
	usrc = ((uae_u32)src) << 1;
	for(bits = 0; bits < 16 && usrc; bits++, usrc >>= 1)
		if ((usrc & 3) == 1 || (usrc & 3) == 2) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
}}}}	m68k_incpc (2);
} /* 38+ (1/0) */

/* MULS.W (An),Dn */
void REGPARAM2 CPUFUNC(op_c1d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5093;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	int cycles = 38 - 4, bits;
	uae_u32 usrc;
	optflag_testl ((uae_s32)(newv));
	usrc = ((uae_u32)src) << 1;
	for(bits = 0; bits < 16 && usrc; bits++, usrc >>= 1)
		if ((usrc & 3) == 1 || (usrc & 3) == 2) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (2);
endlabel5093: ;
} /* 42+ (2/0) */

/* MULS.W (An)+,Dn */
void REGPARAM2 CPUFUNC(op_c1d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5094;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	int cycles = 38 - 4, bits;
	uae_u32 usrc;
	optflag_testl ((uae_s32)(newv));
	usrc = ((uae_u32)src) << 1;
	for(bits = 0; bits < 16 && usrc; bits++, usrc >>= 1)
		if ((usrc & 3) == 1 || (usrc & 3) == 2) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (2);
endlabel5094: ;
} /* 42+ (2/0) */

/* MULS.W -(An),Dn */
void REGPARAM2 CPUFUNC(op_c1e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5095;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	int cycles = 38 - 4, bits;
	uae_u32 usrc;
	optflag_testl ((uae_s32)(newv));
	usrc = ((uae_u32)src) << 1;
	for(bits = 0; bits < 16 && usrc; bits++, usrc >>= 1)
		if ((usrc & 3) == 1 || (usrc & 3) == 2) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (2);
endlabel5095: ;
} /* 44+ (2/0) */

/* MULS.W (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_c1e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5096;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	int cycles = 38 - 4, bits;
	uae_u32 usrc;
	optflag_testl ((uae_s32)(newv));
	usrc = ((uae_u32)src) << 1;
	for(bits = 0; bits < 16 && usrc; bits++, usrc >>= 1)
		if ((usrc & 3) == 1 || (usrc & 3) == 2) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel5096: ;
} /* 46+ (3/0) */

/* MULS.W (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_c1f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5097;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	int cycles = 38 - 4, bits;
	uae_u32 usrc;
	optflag_testl ((uae_s32)(newv));
	usrc = ((uae_u32)src) << 1;
	for(bits = 0; bits < 16 && usrc; bits++, usrc >>= 1)
		if ((usrc & 3) == 1 || (usrc & 3) == 2) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel5097: ;
} /* 48+ (3/0) */

/* MULS.W (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_c1f8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5098;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	int cycles = 38 - 4, bits;
	uae_u32 usrc;
	optflag_testl ((uae_s32)(newv));
	usrc = ((uae_u32)src) << 1;
	for(bits = 0; bits < 16 && usrc; bits++, usrc >>= 1)
		if ((usrc & 3) == 1 || (usrc & 3) == 2) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel5098: ;
} /* 46+ (3/0) */

/* MULS.W (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_c1f9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5099;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	int cycles = 38 - 4, bits;
	uae_u32 usrc;
	optflag_testl ((uae_s32)(newv));
	usrc = ((uae_u32)src) << 1;
	for(bits = 0; bits < 16 && usrc; bits++, usrc >>= 1)
		if ((usrc & 3) == 1 || (usrc & 3) == 2) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (6);
endlabel5099: ;
} /* 50+ (4/0) */

/* MULS.W (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_c1fa_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5100;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	int cycles = 38 - 4, bits;
	uae_u32 usrc;
	optflag_testl ((uae_s32)(newv));
	usrc = ((uae_u32)src) << 1;
	for(bits = 0; bits < 16 && usrc; bits++, usrc >>= 1)
		if ((usrc & 3) == 1 || (usrc & 3) == 2) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel5100: ;
} /* 46+ (3/0) */

/* MULS.W (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_c1fb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5101;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	int cycles = 38 - 4, bits;
	uae_u32 usrc;
	optflag_testl ((uae_s32)(newv));
	usrc = ((uae_u32)src) << 1;
	for(bits = 0; bits < 16 && usrc; bits++, usrc >>= 1)
		if ((usrc & 3) == 1 || (usrc & 3) == 2) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel5101: ;
} /* 48+ (3/0) */

/* MULS.W #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_c1fc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 newv = (uae_s32)(uae_s16)dst * (uae_s32)(uae_s16)src;
	int cycles = 38 - 4, bits;
	uae_u32 usrc;
	optflag_testl ((uae_s32)(newv));
	usrc = ((uae_u32)src) << 1;
	for(bits = 0; bits < 16 && usrc; bits++, usrc >>= 1)
		if ((usrc & 3) == 1 || (usrc & 3) == 2) cycles += 2;
	if (cycles > 0) do_cycles_ce000 (cycles);
	m68k_dreg (regs, dstreg) = (newv);
}}}}	m68k_incpc (4);
} /* 42+ (2/0) */

/* ADD.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_d000_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* ADD.B (An),Dn */
void REGPARAM2 CPUFUNC(op_d010_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpc (2);
} /* 8 (2/0) */

/* ADD.B (An)+,Dn */
void REGPARAM2 CPUFUNC(op_d018_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) += areg_byteinc[srcreg];
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpc (2);
} /* 8 (2/0) */

/* ADD.B -(An),Dn */
void REGPARAM2 CPUFUNC(op_d020_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
	do_cycles_ce000 (2);
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpc (2);
} /* 10 (2/0) */

/* ADD.B (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_d028_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* ADD.B (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_d030_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpc (4);
} /* 14 (3/0) */

/* ADD.B (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_d038_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* ADD.B (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_d039_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpc (6);
} /* 16 (4/0) */

/* ADD.B (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_d03a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpc (4);
} /* 12 (3/0) */

/* ADD.B (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_d03b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
{	uae_s8 src = x_get_byte (srca);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}}	m68k_incpc (4);
} /* 14 (3/0) */

/* ADD.B #<data>.B,Dn */
void REGPARAM2 CPUFUNC(op_d03c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = (uae_u8)get_word_ce000_prefetch (4);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((newv) & 0xff);
}}}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* ADD.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_d040_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* ADD.W An,Dn */
void REGPARAM2 CPUFUNC(op_d048_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* ADD.W (An),Dn */
void REGPARAM2 CPUFUNC(op_d050_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5116;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}	m68k_incpc (2);
endlabel5116: ;
} /* 8 (2/0) */

/* ADD.W (An)+,Dn */
void REGPARAM2 CPUFUNC(op_d058_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5117;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}	m68k_incpc (2);
endlabel5117: ;
} /* 8 (2/0) */

/* ADD.W -(An),Dn */
void REGPARAM2 CPUFUNC(op_d060_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5118;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}	m68k_incpc (2);
endlabel5118: ;
} /* 10 (2/0) */

/* ADD.W (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_d068_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5119;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}	m68k_incpc (4);
endlabel5119: ;
} /* 12 (3/0) */

/* ADD.W (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_d070_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5120;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}	m68k_incpc (4);
endlabel5120: ;
} /* 14 (3/0) */

/* ADD.W (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_d078_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5121;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}	m68k_incpc (4);
endlabel5121: ;
} /* 12 (3/0) */

/* ADD.W (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_d079_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5122;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}	m68k_incpc (6);
endlabel5122: ;
} /* 16 (4/0) */

/* ADD.W (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_d07a_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5123;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}	m68k_incpc (4);
endlabel5123: ;
} /* 12 (3/0) */

/* ADD.W (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_d07b_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5124;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}}}	m68k_incpc (4);
endlabel5124: ;
} /* 14 (3/0) */

/* ADD.W #<data>.W,Dn */
void REGPARAM2 CPUFUNC(op_d07c_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((newv) & 0xffff);
}}}}}	m68k_incpc (4);
} /* 8 (2/0) */

/* ADD.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_d080_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* ADD.L An,Dn */
void REGPARAM2 CPUFUNC(op_d088_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* ADD.L (An),Dn */
void REGPARAM2 CPUFUNC(op_d090_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5128;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}	m68k_incpc (2);
endlabel5128: ;
} /* 14 (3/0) */

/* ADD.L (An)+,Dn */
void REGPARAM2 CPUFUNC(op_d098_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5129;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}	m68k_incpc (2);
endlabel5129: ;
} /* 14 (3/0) */

/* ADD.L -(An),Dn */
void REGPARAM2 CPUFUNC(op_d0a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5130;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}	m68k_incpc (2);
endlabel5130: ;
} /* 16 (3/0) */

/* ADD.L (d16,An),Dn */
void REGPARAM2 CPUFUNC(op_d0a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5131;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}	m68k_incpc (4);
endlabel5131: ;
} /* 18 (4/0) */

/* ADD.L (d8,An,Xn),Dn */
void REGPARAM2 CPUFUNC(op_d0b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5132;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}	m68k_incpc (4);
endlabel5132: ;
} /* 20 (4/0) */

/* ADD.L (xxx).W,Dn */
void REGPARAM2 CPUFUNC(op_d0b8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5133;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}	m68k_incpc (4);
endlabel5133: ;
} /* 18 (4/0) */

/* ADD.L (xxx).L,Dn */
void REGPARAM2 CPUFUNC(op_d0b9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5134;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}	m68k_incpc (6);
endlabel5134: ;
} /* 22 (5/0) */

/* ADD.L (d16,PC),Dn */
void REGPARAM2 CPUFUNC(op_d0ba_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5135;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}	m68k_incpc (4);
endlabel5135: ;
} /* 18 (4/0) */

/* ADD.L (d8,PC,Xn),Dn */
void REGPARAM2 CPUFUNC(op_d0bb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5136;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}}}	m68k_incpc (4);
endlabel5136: ;
} /* 20 (4/0) */

/* ADD.L #<data>.L,Dn */
void REGPARAM2 CPUFUNC(op_d0bc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (4);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	m68k_dreg (regs, dstreg) = (newv);
}}}}}	m68k_incpc (6);
} /* 16 (3/0) */

/* ADDA.W Dn,An */
void REGPARAM2 CPUFUNC(op_d0c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* ADDA.W An,An */
void REGPARAM2 CPUFUNC(op_d0c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* ADDA.W (An),An */
void REGPARAM2 CPUFUNC(op_d0d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5140;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (2);
endlabel5140: ;
} /* 12 (2/0) */

/* ADDA.W (An)+,An */
void REGPARAM2 CPUFUNC(op_d0d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5141;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) += 2;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (2);
endlabel5141: ;
} /* 12 (2/0) */

/* ADDA.W -(An),An */
void REGPARAM2 CPUFUNC(op_d0e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5142;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (2);
endlabel5142: ;
} /* 14 (2/0) */

/* ADDA.W (d16,An),An */
void REGPARAM2 CPUFUNC(op_d0e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5143;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel5143: ;
} /* 16 (3/0) */

/* ADDA.W (d8,An,Xn),An */
void REGPARAM2 CPUFUNC(op_d0f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5144;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel5144: ;
} /* 18 (3/0) */

/* ADDA.W (xxx).W,An */
void REGPARAM2 CPUFUNC(op_d0f8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5145;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel5145: ;
} /* 16 (3/0) */

/* ADDA.W (xxx).L,An */
void REGPARAM2 CPUFUNC(op_d0f9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5146;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (6);
endlabel5146: ;
} /* 20 (4/0) */

/* ADDA.W (d16,PC),An */
void REGPARAM2 CPUFUNC(op_d0fa_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5147;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel5147: ;
} /* 16 (3/0) */

/* ADDA.W (d8,PC,Xn),An */
void REGPARAM2 CPUFUNC(op_d0fb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5148;
	}
{{	uae_s16 src = x_get_word (srca);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel5148: ;
} /* 18 (3/0) */

/* ADDA.W #<data>.W,An */
void REGPARAM2 CPUFUNC(op_d0fc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = get_word_ce000_prefetch (4);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpc (4);
} /* 12 (2/0) */

/* ADDX.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_d100_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uae_s8 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
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
}}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* ADDX.B -(An),-(An) */
void REGPARAM2 CPUFUNC(op_d108_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	do_cycles_ce000 (2);
{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - areg_byteinc[srcreg];
{	uae_s8 src = x_get_byte (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s8)(src)) < 0;
	int flgo = ((uae_s8)(dst)) < 0;
	int flgn = ((uae_s8)(newv)) < 0;
	SET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s8)(newv)) == 0));
	SET_NFLG (((uae_s8)(newv)) < 0);
	x_put_byte (dsta, newv);
}}}}}}}	m68k_incpc (2);
} /* 18 (3/1) */

/* ADD.B Dn,(An) */
void REGPARAM2 CPUFUNC(op_d110_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* ADD.B Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_d118_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) += areg_byteinc[dstreg];
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (2);
} /* 12 (2/1) */

/* ADD.B Dn,-(An) */
void REGPARAM2 CPUFUNC(op_d120_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - areg_byteinc[dstreg];
	do_cycles_ce000 (2);
{	uae_s8 dst = x_get_byte (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (2);
} /* 14 (2/1) */

/* ADD.B Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_d128_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* ADD.B Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_d130_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (4);
} /* 18 (3/1) */

/* ADD.B Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_d138_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (4);
} /* 16 (3/1) */

/* ADD.B Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_d139_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s8 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
{	uae_s8 dst = x_get_byte (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_addb (newv, (uae_s8)(src), (uae_s8)(dst));
	x_put_byte (dsta, newv);
}}}}}}	m68k_incpc (6);
} /* 20 (4/1) */

/* ADDX.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_d140_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uae_s16 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
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
}}}}}	m68k_incpc (2);
} /* 4 (1/0) */

/* ADDX.W -(An),-(An) */
void REGPARAM2 CPUFUNC(op_d148_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	do_cycles_ce000 (2);
{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 2;
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5160;
	}
{{	uae_s16 src = x_get_word (srca);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5160;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s16)(src)) < 0;
	int flgo = ((uae_s16)(dst)) < 0;
	int flgn = ((uae_s16)(newv)) < 0;
	SET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s16)(newv)) == 0));
	SET_NFLG (((uae_s16)(newv)) < 0);
	x_put_word (dsta, newv);
}}}}}}}}}	m68k_incpc (2);
endlabel5160: ;
} /* 18 (3/1) */

/* ADD.W Dn,(An) */
void REGPARAM2 CPUFUNC(op_d150_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5161;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (2);
endlabel5161: ;
} /* 12 (2/1) */

/* ADD.W Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_d158_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5162;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (2);
endlabel5162: ;
} /* 12 (2/1) */

/* ADD.W Dn,-(An) */
void REGPARAM2 CPUFUNC(op_d160_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 2;
	do_cycles_ce000 (2);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5163;
	}
{{	uae_s16 dst = x_get_word (dsta);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (2);
endlabel5163: ;
} /* 14 (2/1) */

/* ADD.W Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_d168_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5164;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (4);
endlabel5164: ;
} /* 16 (3/1) */

/* ADD.W Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_d170_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5165;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (4);
endlabel5165: ;
} /* 18 (3/1) */

/* ADD.W Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_d178_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5166;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (4);
endlabel5166: ;
} /* 16 (3/1) */

/* ADD.W Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_d179_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s16 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5167;
	}
{{	uae_s16 dst = x_get_word (dsta);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_addw (newv, (uae_s16)(src), (uae_s16)(dst));
	x_put_word (dsta, newv);
}}}}}}}	m68k_incpc (6);
endlabel5167: ;
} /* 20 (4/1) */

/* ADDX.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_d180_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
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
}}}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* ADDX.L -(An),-(An) */
void REGPARAM2 CPUFUNC(op_d188_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{	do_cycles_ce000 (2);
{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5169;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5169;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 newv = dst + src + (GET_XFLG () ? 1 : 0);
{	int flgs = ((uae_s32)(src)) < 0;
	int flgo = ((uae_s32)(dst)) < 0;
	int flgn = ((uae_s32)(newv)) < 0;
	SET_VFLG ((flgs ^ flgn) & (flgo ^ flgn));
	SET_CFLG (flgs ^ ((flgs ^ flgo) & (flgo ^ flgn)));
	COPY_CARRY ();
	SET_ZFLG (GET_ZFLG () & (((uae_s32)(newv)) == 0));
	SET_NFLG (((uae_s32)(newv)) < 0);
	x_put_word (dsta, newv >> 16); x_put_word (dsta + 2, newv);
}}}}}}}}}	m68k_incpc (2);
endlabel5169: ;
} /* 30 (5/2) */

/* ADD.L Dn,(An) */
void REGPARAM2 CPUFUNC(op_d190_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5170;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (2);
endlabel5170: ;
} /* 20 (3/2) */

/* ADD.L Dn,(An)+ */
void REGPARAM2 CPUFUNC(op_d198_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5171;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) += 4;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (2);
endlabel5171: ;
} /* 20 (3/2) */

/* ADD.L Dn,-(An) */
void REGPARAM2 CPUFUNC(op_d1a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) - 4;
	do_cycles_ce000 (2);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5172;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	m68k_areg (regs, dstreg) = dsta;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (2);
endlabel5172: ;
} /* 22 (3/2) */

/* ADD.L Dn,(d16,An) */
void REGPARAM2 CPUFUNC(op_d1a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = m68k_areg (regs, dstreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5173;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (4);
endlabel5173: ;
} /* 24 (4/2) */

/* ADD.L Dn,(d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_d1b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	do_cycles_ce000 (2);
	dsta = get_disp_ea_000 (m68k_areg (regs, dstreg), get_word_ce000_prefetch (4));
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5174;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (4);
endlabel5174: ;
} /* 26 (4/2) */

/* ADD.L Dn,(xxx).W */
void REGPARAM2 CPUFUNC(op_d1b8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5175;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (4);
endlabel5175: ;
} /* 24 (4/2) */

/* ADD.L Dn,(xxx).L */
void REGPARAM2 CPUFUNC(op_d1b9_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uaecptr dsta;
	dsta = get_word_ce000_prefetch (4) << 16;
	dsta |= get_word_ce000_prefetch (6);
	if (dsta & 1) {
		exception3 (opcode, dsta);
		goto endlabel5176;
	}
{{	uae_s32 dst = x_get_word (dsta) << 16; dst |= x_get_word (dsta + 2);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{{	uae_u32 newv;
	optflag_addl (newv, (uae_s32)(src), (uae_s32)(dst));
	x_put_word (dsta + 2, newv); x_put_word (dsta, newv >> 16);
}}}}}}}	m68k_incpc (6);
endlabel5176: ;
} /* 28 (5/2) */

/* ADDA.L Dn,An */
void REGPARAM2 CPUFUNC(op_d1c0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_dreg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpc (2);
} /* 8 (1/0) */

/* ADDA.L An,An */
void REGPARAM2 CPUFUNC(op_d1c8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src = m68k_areg (regs, srcreg);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpc (2);
} /* 6 (1/0) */

/* ADDA.L (An),An */
void REGPARAM2 CPUFUNC(op_d1d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5179;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (2);
endlabel5179: ;
} /* 14 (3/0) */

/* ADDA.L (An)+,An */
void REGPARAM2 CPUFUNC(op_d1d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5180;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) += 4;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (2);
endlabel5180: ;
} /* 14 (3/0) */

/* ADDA.L -(An),An */
void REGPARAM2 CPUFUNC(op_d1e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) - 4;
	do_cycles_ce000 (2);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5181;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
	m68k_areg (regs, srcreg) = srca;
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (2);
endlabel5181: ;
} /* 16 (3/0) */

/* ADDA.L (d16,An),An */
void REGPARAM2 CPUFUNC(op_d1e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5182;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel5182: ;
} /* 18 (4/0) */

/* ADDA.L (d8,An,Xn),An */
void REGPARAM2 CPUFUNC(op_d1f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5183;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel5183: ;
} /* 20 (4/0) */

/* ADDA.L (xxx).W,An */
void REGPARAM2 CPUFUNC(op_d1f8_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5184;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel5184: ;
} /* 18 (4/0) */

/* ADDA.L (xxx).L,An */
void REGPARAM2 CPUFUNC(op_d1f9_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = get_word_ce000_prefetch (4) << 16;
	srca |= get_word_ce000_prefetch (6);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5185;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (6);
endlabel5185: ;
} /* 22 (5/0) */

/* ADDA.L (d16,PC),An */
void REGPARAM2 CPUFUNC(op_d1fa_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr srca;
	srca = m68k_getpc () + 2;
	srca += (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5186;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel5186: ;
} /* 18 (4/0) */

/* ADDA.L (d8,PC,Xn),An */
void REGPARAM2 CPUFUNC(op_d1fb_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uaecptr tmppc;
	uaecptr srca;
	tmppc = m68k_getpc () + 2;
	do_cycles_ce000 (2);
	srca = get_disp_ea_000 (tmppc, get_word_ce000_prefetch (4));
	if (srca & 1) {
		exception3 (opcode, srca);
		goto endlabel5187;
	}
{{	uae_s32 src = x_get_word (srca) << 16; src |= x_get_word (srca + 2);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
	do_cycles_ce000 (2);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}}}	m68k_incpc (4);
endlabel5187: ;
} /* 20 (4/0) */

/* ADDA.L #<data>.L,An */
void REGPARAM2 CPUFUNC(op_d1fc_12)(uae_u32 opcode)
{
	uae_u32 dstreg = (opcode >> 9) & 7;
{{	uae_s32 src;
	src = get_word_ce000_prefetch (4) << 16;
	src |= get_word_ce000_prefetch (6);
{	uae_s32 dst = m68k_areg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
	do_cycles_ce000 (4);
{	uae_u32 newv = dst + src;
	m68k_areg (regs, dstreg) = (newv);
}}}}	m68k_incpc (6);
} /* 16 (3/0) */

/* ASRQ.B #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e000_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u8)data;
	uae_u32 sign = (0x80 & val) >> 7;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* LSRQ.B #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e008_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u8)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ROXRQ.B #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e010_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u8)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* RORQ.B #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e018_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u8)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ASR.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_e020_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u8)data;
	uae_u32 sign = (0x80 & val) >> 7;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* LSR.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_e028_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u8)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ROXR.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_e030_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u8)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

#endif

#ifdef PART_8
/* ROR.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_e038_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u8)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ASRQ.W #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e040_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = (0x8000 & val) >> 15;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* LSRQ.W #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e048_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ROXRQ.W #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e050_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* RORQ.W #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e058_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ASR.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_e060_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = (0x8000 & val) >> 15;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* LSR.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_e068_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ROXR.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_e070_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ROR.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_e078_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ASRQ.L #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e080_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = data;
	uae_u32 sign = (0x80000000 & val) >> 31;
	int ccnt = cnt & 63;
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
	{
		int cycles = 4;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpc (2);
} /* 8+ (1/0) */

/* LSRQ.L #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e088_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 4;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpc (2);
} /* 8+ (1/0) */

/* ROXRQ.L #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e090_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 4;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpc (2);
} /* 8+ (1/0) */

/* RORQ.L #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e098_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 4;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpc (2);
} /* 8+ (1/0) */

/* ASR.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_e0a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = data;
	uae_u32 sign = (0x80000000 & val) >> 31;
	int ccnt = cnt & 63;
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
	{
		int cycles = 4;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpc (2);
} /* 8+ (1/0) */

/* LSR.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_e0a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 4;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpc (2);
} /* 8+ (1/0) */

/* ROXR.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_e0b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 4;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpc (2);
} /* 8+ (1/0) */

/* ROR.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_e0b8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 4;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpc (2);
} /* 8+ (1/0) */

/* ASRW.W (An) */
void REGPARAM2 CPUFUNC(op_e0d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5213;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5213: ;
} /* 12 (2/1) */

/* ASRW.W (An)+ */
void REGPARAM2 CPUFUNC(op_e0d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5214;
	}
{{	uae_s16 data = x_get_word (dataa);
	m68k_areg (regs, srcreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5214: ;
} /* 12 (2/1) */

/* ASRW.W -(An) */
void REGPARAM2 CPUFUNC(op_e0e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5215;
	}
{{	uae_s16 data = x_get_word (dataa);
	m68k_areg (regs, srcreg) = dataa;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5215: ;
} /* 14 (2/1) */

/* ASRW.W (d16,An) */
void REGPARAM2 CPUFUNC(op_e0e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5216;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5216: ;
} /* 16 (3/1) */

/* ASRW.W (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_e0f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	do_cycles_ce000 (2);
	dataa = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5217;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5217: ;
} /* 18 (3/1) */

/* ASRW.W (xxx).W */
void REGPARAM2 CPUFUNC(op_e0f8_12)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5218;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5218: ;
} /* 16 (3/1) */

/* ASRW.W (xxx).L */
void REGPARAM2 CPUFUNC(op_e0f9_12)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_word_ce000_prefetch (4) << 16;
	dataa |= get_word_ce000_prefetch (6);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5219;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 cflg = val & 1;
	val = (val >> 1) | sign;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (cflg);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (6);
endlabel5219: ;
} /* 20 (4/1) */

/* ASLQ.B #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e100_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u8)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* LSLQ.B #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e108_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u8)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ROXLQ.B #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e110_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u8)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ROLQ.B #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e118_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s8 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u8)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ASL.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_e120_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u8)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* LSL.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_e128_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u8)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ROXL.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_e130_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u8)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ROL.B Dn,Dn */
void REGPARAM2 CPUFUNC(op_e138_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s8 cnt = m68k_dreg (regs, srcreg);
{	uae_s8 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u8)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xff) | ((val) & 0xff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ASLQ.W #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e140_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* LSLQ.W #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e148_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ROXLQ.W #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e150_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ROLQ.W #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e158_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s16 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ASL.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_e160_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* LSL.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_e168_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ROXL.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_e170_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ROL.W Dn,Dn */
void REGPARAM2 CPUFUNC(op_e178_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s16 cnt = m68k_dreg (regs, srcreg);
{	uae_s16 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 2;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (m68k_dreg (regs, dstreg) & ~0xffff) | ((val) & 0xffff);
}}}}	m68k_incpc (2);
} /* 6+ (1/0) */

/* ASLQ.L #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e180_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 4;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpc (2);
} /* 8+ (1/0) */

/* LSLQ.L #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e188_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 4;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpc (2);
} /* 8+ (1/0) */

/* ROXLQ.L #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e190_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 4;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpc (2);
} /* 8+ (1/0) */

/* ROLQ.L #<data>,Dn */
void REGPARAM2 CPUFUNC(op_e198_12)(uae_u32 opcode)
{
	uae_u32 srcreg = imm8_table[((opcode >> 9) & 7)];
	uae_u32 dstreg = opcode & 7;
{{	uae_u32 cnt = srcreg;
{	uae_s32 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 4;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpc (2);
} /* 8+ (1/0) */

/* ASL.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_e1a0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 4;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpc (2);
} /* 8+ (1/0) */

/* LSL.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_e1a8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 4;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpc (2);
} /* 8+ (1/0) */

/* ROXL.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_e1b0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 4;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpc (2);
} /* 8+ (1/0) */

/* ROL.L Dn,Dn */
void REGPARAM2 CPUFUNC(op_e1b8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = ((opcode >> 9) & 7);
	uae_u32 dstreg = opcode & 7;
{{	uae_s32 cnt = m68k_dreg (regs, srcreg);
{	uae_s32 data = m68k_dreg (regs, dstreg);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = data;
	int ccnt = cnt & 63;
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
	{
		int cycles = 4;
		cycles += 2 * ccnt;
		if (cycles > 0) do_cycles_ce000 (cycles);
	}
	m68k_dreg (regs, dstreg) = (val);
}}}}	m68k_incpc (2);
} /* 8+ (1/0) */

/* ASLW.W (An) */
void REGPARAM2 CPUFUNC(op_e1d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5244;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5244: ;
} /* 12 (2/1) */

/* ASLW.W (An)+ */
void REGPARAM2 CPUFUNC(op_e1d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5245;
	}
{{	uae_s16 data = x_get_word (dataa);
	m68k_areg (regs, srcreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5245: ;
} /* 12 (2/1) */

/* ASLW.W -(An) */
void REGPARAM2 CPUFUNC(op_e1e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5246;
	}
{{	uae_s16 data = x_get_word (dataa);
	m68k_areg (regs, srcreg) = dataa;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5246: ;
} /* 14 (2/1) */

/* ASLW.W (d16,An) */
void REGPARAM2 CPUFUNC(op_e1e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5247;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5247: ;
} /* 16 (3/1) */

/* ASLW.W (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_e1f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	do_cycles_ce000 (2);
	dataa = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5248;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5248: ;
} /* 18 (3/1) */

/* ASLW.W (xxx).W */
void REGPARAM2 CPUFUNC(op_e1f8_12)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5249;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5249: ;
} /* 16 (3/1) */

/* ASLW.W (xxx).L */
void REGPARAM2 CPUFUNC(op_e1f9_12)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_word_ce000_prefetch (4) << 16;
	dataa |= get_word_ce000_prefetch (6);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5250;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	uae_u32 val = (uae_u16)data;
	uae_u32 sign = 0x8000 & val;
	uae_u32 sign2;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	sign2 = 0x8000 & val;
	SET_CFLG (sign != 0);
	COPY_CARRY ();
	SET_VFLG (GET_VFLG () | (sign2 != sign));
	x_put_word (dataa, val);
}}}}}	m68k_incpc (6);
endlabel5250: ;
} /* 20 (4/1) */

/* LSRW.W (An) */
void REGPARAM2 CPUFUNC(op_e2d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5251;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5251: ;
} /* 12 (2/1) */

/* LSRW.W (An)+ */
void REGPARAM2 CPUFUNC(op_e2d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5252;
	}
{{	uae_s16 data = x_get_word (dataa);
	m68k_areg (regs, srcreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5252: ;
} /* 12 (2/1) */

/* LSRW.W -(An) */
void REGPARAM2 CPUFUNC(op_e2e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5253;
	}
{{	uae_s16 data = x_get_word (dataa);
	m68k_areg (regs, srcreg) = dataa;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5253: ;
} /* 14 (2/1) */

/* LSRW.W (d16,An) */
void REGPARAM2 CPUFUNC(op_e2e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5254;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5254: ;
} /* 16 (3/1) */

/* LSRW.W (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_e2f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	do_cycles_ce000 (2);
	dataa = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5255;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5255: ;
} /* 18 (3/1) */

/* LSRW.W (xxx).W */
void REGPARAM2 CPUFUNC(op_e2f8_12)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5256;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5256: ;
} /* 16 (3/1) */

/* LSRW.W (xxx).L */
void REGPARAM2 CPUFUNC(op_e2f9_12)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_word_ce000_prefetch (4) << 16;
	dataa |= get_word_ce000_prefetch (6);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5257;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	uae_u32 val = (uae_u16)data;
	uae_u32 carry = val & 1;
	val >>= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (6);
endlabel5257: ;
} /* 20 (4/1) */

/* LSLW.W (An) */
void REGPARAM2 CPUFUNC(op_e3d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5258;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5258: ;
} /* 12 (2/1) */

/* LSLW.W (An)+ */
void REGPARAM2 CPUFUNC(op_e3d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5259;
	}
{{	uae_s16 data = x_get_word (dataa);
	m68k_areg (regs, srcreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5259: ;
} /* 12 (2/1) */

/* LSLW.W -(An) */
void REGPARAM2 CPUFUNC(op_e3e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5260;
	}
{{	uae_s16 data = x_get_word (dataa);
	m68k_areg (regs, srcreg) = dataa;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5260: ;
} /* 14 (2/1) */

/* LSLW.W (d16,An) */
void REGPARAM2 CPUFUNC(op_e3e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5261;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5261: ;
} /* 16 (3/1) */

/* LSLW.W (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_e3f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	do_cycles_ce000 (2);
	dataa = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5262;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5262: ;
} /* 18 (3/1) */

/* LSLW.W (xxx).W */
void REGPARAM2 CPUFUNC(op_e3f8_12)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5263;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5263: ;
} /* 16 (3/1) */

/* LSLW.W (xxx).L */
void REGPARAM2 CPUFUNC(op_e3f9_12)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_word_ce000_prefetch (4) << 16;
	dataa |= get_word_ce000_prefetch (6);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5264;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (6);
endlabel5264: ;
} /* 20 (4/1) */

/* ROXRW.W (An) */
void REGPARAM2 CPUFUNC(op_e4d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5265;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5265: ;
} /* 12 (2/1) */

/* ROXRW.W (An)+ */
void REGPARAM2 CPUFUNC(op_e4d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5266;
	}
{{	uae_s16 data = x_get_word (dataa);
	m68k_areg (regs, srcreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5266: ;
} /* 12 (2/1) */

/* ROXRW.W -(An) */
void REGPARAM2 CPUFUNC(op_e4e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5267;
	}
{{	uae_s16 data = x_get_word (dataa);
	m68k_areg (regs, srcreg) = dataa;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5267: ;
} /* 14 (2/1) */

/* ROXRW.W (d16,An) */
void REGPARAM2 CPUFUNC(op_e4e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5268;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5268: ;
} /* 16 (3/1) */

/* ROXRW.W (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_e4f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	do_cycles_ce000 (2);
	dataa = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5269;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5269: ;
} /* 18 (3/1) */

/* ROXRW.W (xxx).W */
void REGPARAM2 CPUFUNC(op_e4f8_12)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5270;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5270: ;
} /* 16 (3/1) */

/* ROXRW.W (xxx).L */
void REGPARAM2 CPUFUNC(op_e4f9_12)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_word_ce000_prefetch (4) << 16;
	dataa |= get_word_ce000_prefetch (6);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5271;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (GET_XFLG ()) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (6);
endlabel5271: ;
} /* 20 (4/1) */

/* ROXLW.W (An) */
void REGPARAM2 CPUFUNC(op_e5d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5272;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5272: ;
} /* 12 (2/1) */

/* ROXLW.W (An)+ */
void REGPARAM2 CPUFUNC(op_e5d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5273;
	}
{{	uae_s16 data = x_get_word (dataa);
	m68k_areg (regs, srcreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5273: ;
} /* 12 (2/1) */

/* ROXLW.W -(An) */
void REGPARAM2 CPUFUNC(op_e5e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5274;
	}
{{	uae_s16 data = x_get_word (dataa);
	m68k_areg (regs, srcreg) = dataa;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5274: ;
} /* 14 (2/1) */

/* ROXLW.W (d16,An) */
void REGPARAM2 CPUFUNC(op_e5e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5275;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5275: ;
} /* 16 (3/1) */

/* ROXLW.W (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_e5f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	do_cycles_ce000 (2);
	dataa = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5276;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5276: ;
} /* 18 (3/1) */

/* ROXLW.W (xxx).W */
void REGPARAM2 CPUFUNC(op_e5f8_12)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5277;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5277: ;
} /* 16 (3/1) */

/* ROXLW.W (xxx).L */
void REGPARAM2 CPUFUNC(op_e5f9_12)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_word_ce000_prefetch (4) << 16;
	dataa |= get_word_ce000_prefetch (6);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5278;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (GET_XFLG ()) val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	COPY_CARRY ();
	x_put_word (dataa, val);
}}}}}	m68k_incpc (6);
endlabel5278: ;
} /* 20 (4/1) */

/* RORW.W (An) */
void REGPARAM2 CPUFUNC(op_e6d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5279;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5279: ;
} /* 12 (2/1) */

/* RORW.W (An)+ */
void REGPARAM2 CPUFUNC(op_e6d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5280;
	}
{{	uae_s16 data = x_get_word (dataa);
	m68k_areg (regs, srcreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5280: ;
} /* 12 (2/1) */

/* RORW.W -(An) */
void REGPARAM2 CPUFUNC(op_e6e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5281;
	}
{{	uae_s16 data = x_get_word (dataa);
	m68k_areg (regs, srcreg) = dataa;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5281: ;
} /* 14 (2/1) */

/* RORW.W (d16,An) */
void REGPARAM2 CPUFUNC(op_e6e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5282;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5282: ;
} /* 16 (3/1) */

/* RORW.W (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_e6f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	do_cycles_ce000 (2);
	dataa = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5283;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5283: ;
} /* 18 (3/1) */

/* RORW.W (xxx).W */
void REGPARAM2 CPUFUNC(op_e6f8_12)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5284;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5284: ;
} /* 16 (3/1) */

/* RORW.W (xxx).L */
void REGPARAM2 CPUFUNC(op_e6f9_12)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_word_ce000_prefetch (4) << 16;
	dataa |= get_word_ce000_prefetch (6);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5285;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	uae_u16 val = data;
	uae_u32 carry = val & 1;
	val >>= 1;
	if (carry) val |= 0x8000;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry);
	x_put_word (dataa, val);
}}}}}	m68k_incpc (6);
endlabel5285: ;
} /* 20 (4/1) */

/* ROLW.W (An) */
void REGPARAM2 CPUFUNC(op_e7d0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5286;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5286: ;
} /* 12 (2/1) */

/* ROLW.W (An)+ */
void REGPARAM2 CPUFUNC(op_e7d8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5287;
	}
{{	uae_s16 data = x_get_word (dataa);
	m68k_areg (regs, srcreg) += 2;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5287: ;
} /* 12 (2/1) */

/* ROLW.W -(An) */
void REGPARAM2 CPUFUNC(op_e7e0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) - 2;
	do_cycles_ce000 (2);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5288;
	}
{{	uae_s16 data = x_get_word (dataa);
	m68k_areg (regs, srcreg) = dataa;
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (4);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	x_put_word (dataa, val);
}}}}}	m68k_incpc (2);
endlabel5288: ;
} /* 14 (2/1) */

/* ROLW.W (d16,An) */
void REGPARAM2 CPUFUNC(op_e7e8_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	dataa = m68k_areg (regs, srcreg) + (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5289;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5289: ;
} /* 16 (3/1) */

/* ROLW.W (d8,An,Xn) */
void REGPARAM2 CPUFUNC(op_e7f0_12)(uae_u32 opcode)
{
	uae_u32 srcreg = (opcode & 7);
{{	uaecptr dataa;
	do_cycles_ce000 (2);
	dataa = get_disp_ea_000 (m68k_areg (regs, srcreg), get_word_ce000_prefetch (4));
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5290;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5290: ;
} /* 18 (3/1) */

/* ROLW.W (xxx).W */
void REGPARAM2 CPUFUNC(op_e7f8_12)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = (uae_s32)(uae_s16)get_word_ce000_prefetch (4);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5291;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (6);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	x_put_word (dataa, val);
}}}}}	m68k_incpc (4);
endlabel5291: ;
} /* 16 (3/1) */

/* ROLW.W (xxx).L */
void REGPARAM2 CPUFUNC(op_e7f9_12)(uae_u32 opcode)
{
{{	uaecptr dataa;
	dataa = get_word_ce000_prefetch (4) << 16;
	dataa |= get_word_ce000_prefetch (6);
	if (dataa & 1) {
		exception3 (opcode, dataa);
		goto endlabel5292;
	}
{{	uae_s16 data = x_get_word (dataa);
	regs.ir = regs.irc;
	ipl_fetch ();
	get_word_ce000_prefetch (8);
{	uae_u16 val = data;
	uae_u32 carry = val & 0x8000;
	val <<= 1;
	if (carry)  val |= 1;
	optflag_testw ((uae_s16)(val));
	SET_CFLG (carry >> 15);
	x_put_word (dataa, val);
}}}}}	m68k_incpc (6);
endlabel5292: ;
} /* 20 (4/1) */

#endif

