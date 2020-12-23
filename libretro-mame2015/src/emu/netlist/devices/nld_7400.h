// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_7400.h
 *
 *  DM7400: Quad 2-Input NAND Gates
 *
 *          +--------------+
 *       A1 |1     ++    14| VCC
 *       B1 |2           13| B4
 *       Y1 |3           12| A4
 *       A2 |4    7400   11| Y4
 *       B2 |5           10| B3
 *       Y2 |6            9| A3
 *      GND |7            8| Y3
 *          +--------------+
 *                  __
 *              Y = AB
 *          +---+---++---+
 *          | A | B || Y |
 *          +===+===++===+
 *          | 0 | 0 || 1 |
 *          | 0 | 1 || 1 |
 *          | 1 | 0 || 1 |
 *          | 1 | 1 || 0 |
 *          +---+---++---+
 *
 *  Naming conventions follow National Semiconductor datasheet
 *
 */

#ifndef NLD_7400_H_
#define NLD_7400_H_

#include "nld_signal.h"

#define TTL_7400_NAND(_name, _A, _B)                                                \
		NET_REGISTER_DEV(7400, _name)                                               \
		NET_CONNECT(_name, A, _A)                                                   \
		NET_CONNECT(_name, B, _B)

#if 1
NETLIB_SIGNAL(7400, 2, 0, 0);
#else
#include "nld_truthtable.h"
NETLIB_TRUTHTABLE(7400, 2, 1, 0);
#endif


#define TTL_7400_DIP(_name)                                                         \
		NET_REGISTER_DEV(7400_dip, _name)

NETLIB_DEVICE(7400_dip,

	NETLIB_NAME(7400) m_1;
	NETLIB_NAME(7400) m_2;
	NETLIB_NAME(7400) m_3;
	NETLIB_NAME(7400) m_4;
);

#endif /* NLD_7400_H_ */
