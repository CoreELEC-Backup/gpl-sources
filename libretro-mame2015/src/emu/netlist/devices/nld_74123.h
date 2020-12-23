// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_74123.h
 *
 *  74123: Dual Retriggerable One-Shot with Clear and Complementary Outputs
 *
 *           +--------------+
 *        A1 |1     ++    16| VCC
 *        B1 |2           15| RC1
 *      CLR1 |3           14| C1
 *       Q1Q |4   74123   13| Q1
 *        Q2 |5           12| Q2Q
 *        C2 |6           11| CLR2
 *       RC2 |7           10| B2
 *       GND |8            9| A2
 *           +--------------+
 *
 *  Naming conventions follow Fairchild Semiconductor datasheet
 *
 */

#ifndef NLD_74123_H_
#define NLD_74123_H_

#include "../nl_base.h"
#include "../analog/nld_twoterm.h"

#define TTL_74123(_name)                                                        \
		NET_REGISTER_DEV(74123, _name)


NETLIB_DEVICE(74123,
public:
	NETLIB_NAME(R) m_RP;
	NETLIB_NAME(R) m_RN;

	netlist_ttl_input_t m_A;
	netlist_ttl_input_t m_B;
	netlist_ttl_input_t m_CLRQ;
	netlist_ttl_output_t m_Q;
	netlist_ttl_output_t m_QQ;

	netlist_analog_input_t m_CV;

	netlist_state_t<netlist_sig_t> m_last_trig;
	netlist_state_t<UINT8>         m_state;
	netlist_state_t<double>        m_KP;

	netlist_param_double_t m_K;
	netlist_param_double_t m_RI;

);

#define TTL_74123_DIP(_name)                                                         \
		NET_REGISTER_DEV(74123_dip, _name)

NETLIB_DEVICE(74123_dip,

	NETLIB_NAME(74123) m_1;
	NETLIB_NAME(74123) m_2;

);


#endif /* NLD_74123_H_ */
