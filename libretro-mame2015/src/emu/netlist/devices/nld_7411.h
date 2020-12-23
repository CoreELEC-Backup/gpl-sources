// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_7411.h
 *
 *  DM7411: Triple 3-Input AND Gates
 *
 *          +--------------+
 *       A1 |1     ++    14| VCC
 *       B1 |2           13| C1
 *       A2 |3           12| Y1
 *       B2 |4    7411   11| C3
 *       C2 |5           10| B3
 *       Y2 |6            9| A3
 *      GND |7            8| Y3
 *          +--------------+
 *
 *              Y = ABC
 *          +---+---+---++---+
 *          | A | B | C || Y |
 *          +===+===+===++===+
 *          | X | X | 0 || 0 |
 *          | X | 0 | X || 0 |
 *          | 0 | X | X || 0 |
 *          | 1 | 1 | 1 || 1 |
 *          +---+---+---++---+
 *
 *  Naming conventions follow National Semiconductor datasheet
 *
 */

#ifndef NLD_7411_H_
#define NLD_7411_H_

#include "nld_signal.h"

#define TTL_7411_AND(_name, _I1, _I2, _I3)                                         \
		NET_REGISTER_DEV(7411, _name)                                               \
		NET_CONNECT(_name, A, _I1)                                                  \
		NET_CONNECT(_name, B, _I2)                                                  \
		NET_CONNECT(_name, C, _I3)

NETLIB_SIGNAL(7411, 3, 0, 1);

#define TTL_7411_DIP(_name)                                                         \
		NET_REGISTER_DEV(7411_dip, _name)

NETLIB_DEVICE(7411_dip,

	NETLIB_NAME(7411) m_1;
	NETLIB_NAME(7411) m_2;
	NETLIB_NAME(7411) m_3;
);

#endif /* NLD_7411_H_ */
