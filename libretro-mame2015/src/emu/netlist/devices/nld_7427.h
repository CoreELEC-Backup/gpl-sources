// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_7427.h
 *
 *  DM7427: Triple 3-Input NOR Gates
 *
 *          +--------------+
 *       A1 |1     ++    14| VCC
 *       B1 |2           13| C1
 *       A2 |3           12| Y1
 *       B2 |4    7427   11| C3
 *       C2 |5           10| B3
 *       Y2 |6            9| A3
 *      GND |7            8| Y3
 *          +--------------+
 *                  _____
 *              Y = A+B+C
 *          +---+---+---++---+
 *          | A | B | C || Y |
 *          +===+===+===++===+
 *          | X | X | 1 || 0 |
 *          | X | 1 | X || 0 |
 *          | 1 | X | X || 0 |
 *          | 0 | 0 | 0 || 1 |
 *          +---+---+---++---+
 *
 *  Naming conventions follow National Semiconductor datasheet
 *
 */

#ifndef NLD_7427_H_
#define NLD_7427_H_

#include "nld_signal.h"

#define TTL_7427_NOR(_name, _I1, _I2, _I3)                                          \
		NET_REGISTER_DEV(7427, _name)                                               \
		NET_CONNECT(_name, A, _I1)                                                  \
		NET_CONNECT(_name, B, _I2)                                                  \
		NET_CONNECT(_name, C, _I3)

NETLIB_SIGNAL(7427, 3, 1, 0);

#define TTL_7427_DIP(_name)                                                         \
		NET_REGISTER_DEV(7427_dip, _name)

NETLIB_DEVICE(7427_dip,

	NETLIB_NAME(7427) m_1;
	NETLIB_NAME(7427) m_2;
	NETLIB_NAME(7427) m_3;
);
#endif /* NLD_7427_H_ */
