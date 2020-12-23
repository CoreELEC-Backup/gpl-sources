/*
 * nld_74123.c
 *
 */

#include "nld_74123.h"

#define R_OFF (1E20)
#define R_ON (m_RI.Value())

NETLIB_START(74123)
{
	register_sub(m_RP, "RP");
	register_sub(m_RN, "RN");

	register_input("A", m_A);
	register_input("B", m_B);
	register_input("CLRQ", m_CLRQ);
	register_output("Q", m_Q);
	register_output("QQ", m_QQ);

	register_input("_CV", m_CV); // internal

	register_subalias("GND", m_RN.m_N);
	register_subalias("VCC", m_RP.m_P);
	register_subalias("C",   m_RN.m_N);
	register_subalias("RC",  m_RN.m_P);

	register_param("K", m_K, 0.4);
	register_param("RI", m_RI, 400.0); // around 250 for HC series, 400 on LS/TTL, estimated from datasheets

	connect(m_RN.m_P, m_RP.m_N);
	connect(m_CV, m_RN.m_P);

	save(NLNAME(m_last_trig));
	save(NLNAME(m_state));
	save(NLNAME(m_KP));

	m_KP = 1.0 / (1.0 + exp(m_K.Value()));
}

NETLIB_UPDATE(74123)
{
	const netlist_sig_t m_trig = (INPLOGIC(m_A) ^ 1) & INPLOGIC(m_B) & INPLOGIC(m_CLRQ);

	if (!INPLOGIC(m_CLRQ))
	{
		OUTLOGIC(m_Q, 0, NLTIME_FROM_NS(10));
		OUTLOGIC(m_QQ, 1, NLTIME_FROM_NS(10));
		/* quick charge until trigger */
		/* FIXME: SGS datasheet shows quick charge to 5V,
		 * though schematics indicate quick charge to Vhigh only.
		 */
		m_RP.set_R(R_ON);
		m_RN.set_R(R_OFF);
		m_state = 2; //charging (quick)
	}
	else if (!m_last_trig && m_trig)
	{
		// FIXME: Timing!
		OUTLOGIC(m_Q, 1, NLTIME_FROM_NS(10));
		OUTLOGIC(m_QQ, 0, NLTIME_FROM_NS(10));
		m_RN.set_R(R_ON);
		m_RP.set_R(R_OFF);
		m_state = 1; // discharging
	}

	m_last_trig = m_trig;

	if (m_state == 1)
	{
		const nl_double vLow = m_KP * TERMANALOG(m_RP.m_P);
		if (INPANALOG(m_CV) < vLow)
		{
			m_RN.set_R(R_OFF);
			m_state = 2; // charging
		}
	}
	else if (m_state == 2)
	{
		const nl_double vHigh = TERMANALOG(m_RP.m_P) * (1.0 - m_KP);
		if (INPANALOG(m_CV) > vHigh)
		{
			m_RP.set_R(R_OFF);
			OUTLOGIC(m_Q, 0, NLTIME_FROM_NS(10));
			OUTLOGIC(m_QQ, 1, NLTIME_FROM_NS(10));
			m_state = 0; // waiting
		}
	}
}

NETLIB_RESET(74123)
{
	m_RP.do_reset();
	m_RN.do_reset();

	m_RP.set_R(R_OFF);
	m_RN.set_R(R_OFF);

	m_last_trig = 0;
	m_state = 0;
	m_QQ.initial(1);
}

NETLIB_START(74123_dip)
{
#if 0
	register_sub(m_1, "1");
	register_sub(m_2, "2");
	register_sub(m_3, "3");

	register_subalias("1", m_1.m_i[0]);
	register_subalias("2", m_1.m_i[1]);
	register_subalias("3", m_2.m_i[0]);
	register_subalias("4", m_2.m_i[1]);
	register_subalias("5", m_2.m_i[2]);
	register_subalias("6", m_2.m_Q);

	register_subalias("8", m_3.m_Q);
	register_subalias("9", m_3.m_i[0]);
	register_subalias("10", m_3.m_i[1]);
	register_subalias("11", m_3.m_i[2]);

	register_subalias("12", m_1.m_Q);
	register_subalias("13", m_1.m_i[2]);
#endif
}

NETLIB_UPDATE(74123_dip)
{
	/* only called during startup */
	m_1.update_dev();
	m_2.update_dev();
}

NETLIB_RESET(74123_dip)
{
	m_1.do_reset();
	m_2.do_reset();
}
