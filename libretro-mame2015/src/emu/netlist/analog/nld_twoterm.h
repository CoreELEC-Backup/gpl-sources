// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nld_twoterm.h
 *
 * Devices with two terminals ...
 *
 *
 *       (k)
 *  +-----T-----+
 *  |     |     |
 *  |  +--+--+  |
 *  |  |     |  |
 *  |  R     |  |
 *  |  R     |  |
 *  |  R     I  |
 *  |  |     I  |  Device n
 *  |  V+    I  |
 *  |  V     |  |
 *  |  V-    |  |
 *  |  |     |  |
 *  |  +--+--+  |
 *  |     |     |
 *  +-----T-----+
 *       (l)
 *
 *  This is a resistance in series to a voltage source and paralleled by a current source.
 *  This is suitable to model voltage sources, current sources, resistors, capacitors,
 *  inductances and diodes.
 *
 */

#ifndef NLD_TWOTERM_H_
#define NLD_TWOTERM_H_

#include "../nl_base.h"

// ----------------------------------------------------------------------------------------
// Macros
// ----------------------------------------------------------------------------------------

#define RES(_name, _R)                                                         \
		NET_REGISTER_DEV(R, _name)                                                  \
		NETDEV_PARAMI(_name, R, _R)

#define POT(_name, _R)                                                       \
		NET_REGISTER_DEV(POT, _name)                                                \
		NETDEV_PARAMI(_name, R, _R)


#define CAP(_name, _C)                                                         \
		NET_REGISTER_DEV(C, _name)                                                  \
		NETDEV_PARAMI(_name, C, _C)

/* Generic Diode */
#define DIODE(_name,  _model)                                                    \
		NET_REGISTER_DEV(D, _name)                                                  \
		NETDEV_PARAMI(_name, model, _model)

// ----------------------------------------------------------------------------------------
// Generic macros
// ----------------------------------------------------------------------------------------


#ifdef RES_R
#warning "Do not include rescap.h in a netlist environment"
#endif

#define RES_R(res) ((double)(res))
#define RES_K(res) ((double)(res) * 1e3)
#define RES_M(res) ((double)(res) * 1e6)
#define CAP_U(cap) ((double)(cap) * 1e-6)
#define CAP_N(cap) ((double)(cap) * 1e-9)
#define CAP_P(cap) ((double)(cap) * 1e-12)
#define IND_U(ind) ((double)(ind) * 1e-6)
#define IND_N(ind) ((double)(ind) * 1e-9)
#define IND_P(ind) ((double)(ind) * 1e-12)

// ----------------------------------------------------------------------------------------
// Implementation
// ----------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------
// nld_twoterm
// ----------------------------------------------------------------------------------------

class NETLIB_NAME(twoterm) : public netlist_device_t
{
public:
	ATTR_COLD NETLIB_NAME(twoterm)(const family_t afamily);
	ATTR_COLD NETLIB_NAME(twoterm)();

	netlist_terminal_t m_P;
	netlist_terminal_t m_N;

	virtual NETLIB_UPDATE_TERMINALS()
	{
	}

	ATTR_HOT inline void set(const nl_double G, const nl_double V, const nl_double I)
	{
		/*      GO, GT, I                */
		m_P.set( G,  G, (  V) * G - I);
		m_N.set( G,  G, ( -V) * G + I);
	}

	ATTR_HOT inline nl_double deltaV() const
	{
		return m_P.net().as_analog().Q_Analog() - m_N.net().as_analog().Q_Analog();
	}

	ATTR_HOT void set_mat(nl_double a11, nl_double a12, nl_double a21, nl_double a22, nl_double r1, nl_double r2)
	{
		/*      GO, GT, I                */
		m_P.set(-a12, a11, -r1);
		m_N.set(-a21, a22, -r2);
	}

protected:
	ATTR_COLD virtual void start();
	ATTR_COLD virtual void reset();
	ATTR_HOT ATTR_ALIGN void update();

private:
};

// ----------------------------------------------------------------------------------------
// nld_R
// ----------------------------------------------------------------------------------------

class NETLIB_NAME(R_base) : public NETLIB_NAME(twoterm)
{
public:
	ATTR_COLD NETLIB_NAME(R_base)() : NETLIB_NAME(twoterm)(RESISTOR) { }

	inline void set_R(const nl_double R)
	{
		set(1.0 / R, 0.0, 0.0);
	}

protected:
	ATTR_COLD virtual void start();
	ATTR_COLD virtual void reset();
	ATTR_HOT ATTR_ALIGN void update();
};

NETLIB_DEVICE_WITH_PARAMS_DERIVED(R, R_base,
	netlist_param_double_t m_R;
);

// ----------------------------------------------------------------------------------------
// nld_POT
// ----------------------------------------------------------------------------------------

NETLIB_DEVICE_WITH_PARAMS(POT,
	NETLIB_NAME(R_base) m_R1;
	NETLIB_NAME(R_base) m_R2;

	netlist_param_double_t m_R;
	netlist_param_double_t m_Dial;
	netlist_param_logic_t m_DialIsLog;
);


// ----------------------------------------------------------------------------------------
// nld_C
// ----------------------------------------------------------------------------------------

class NETLIB_NAME(C) : public NETLIB_NAME(twoterm)
{
public:
	ATTR_COLD NETLIB_NAME(C)() : NETLIB_NAME(twoterm)(CAPACITOR) { }

	ATTR_HOT void step_time(const nl_double st)
	{
		const nl_double G = m_C.Value() / st;
		const nl_double I = -G * deltaV();
		set(G, 0.0, I);
	}

protected:
	ATTR_COLD virtual void start();
	ATTR_COLD virtual void reset();
	ATTR_COLD virtual void update_param();
	ATTR_HOT ATTR_ALIGN void update();

	netlist_param_double_t m_C;

};


// ----------------------------------------------------------------------------------------
// A generic diode model to be used in other devices (Diode, BJT ...)
// ----------------------------------------------------------------------------------------

class netlist_generic_diode
{
public:
	ATTR_COLD netlist_generic_diode();

	ATTR_HOT inline void update_diode(const nl_double nVd)
	{
		//FIXME: Optimize cutoff case

		if (nVd < -5.0 * m_Vt)
		{
			m_Vd = nVd;
			m_G = m_gmin;
			m_Id = - m_Is;
		}
		else if (nVd < m_Vcrit)
		{
			m_Vd = nVd;

			const nl_double eVDVt = exp(m_Vd * m_VtInv);
			m_Id = m_Is * (eVDVt - 1.0);
			m_G = m_Is * m_VtInv * eVDVt + m_gmin;
		}
		else
		{
#if defined(_MSC_VER) && _MSC_VER < 1800
			m_Vd = m_Vd + log((nVd - m_Vd) * m_VtInv + 1.0) * m_Vt;
#else
			nl_double a = (nVd - m_Vd) * m_VtInv;
			if (a<1e-12 - 1.0) a = 1e-12 - 1.0;
			m_Vd = m_Vd + log1p(a) * m_Vt;
#endif
			const nl_double eVDVt = exp(m_Vd * m_VtInv);
			m_Id = m_Is * (eVDVt - 1.0);

			m_G = m_Is * m_VtInv * eVDVt + m_gmin;
		}

		//printf("nVd %f m_Vd %f Vcrit %f\n", nVd, m_Vd, m_Vcrit);
	}

	ATTR_COLD void set_param(const nl_double Is, const nl_double n, nl_double gmin);

	ATTR_HOT inline nl_double I() const { return m_Id; }
	ATTR_HOT inline nl_double G() const { return m_G; }
	ATTR_HOT inline nl_double Ieq() const { return (m_Id - m_Vd * m_G); }
	ATTR_HOT inline nl_double Vd() const { return m_Vd; }

	/* owning object must save those ... */

	ATTR_COLD void save(pstring name, netlist_object_t &parent);

private:
	nl_double m_Vd;
	nl_double m_Id;
	nl_double m_G;

	nl_double m_Vt;
	nl_double m_Is;
	nl_double m_n;
	nl_double m_gmin;

	nl_double m_VtInv;
	nl_double m_Vcrit;
};

// ----------------------------------------------------------------------------------------
// nld_D
// ----------------------------------------------------------------------------------------


// this one has an accuracy of better than 5%. That's enough for our purpose
// add c3 and it'll be better than 1%

#if 0
inline nl_double fastexp_h(const nl_double x)
{
	static const nl_double ln2r = 1.442695040888963387;
	static const nl_double ln2  = 0.693147180559945286;
	//static const nl_double c3   = 0.166666666666666667;

	const nl_double y = x * ln2r;
	const unsigned int t = y;
	const nl_double z = (x - ln2 * (double) t);
	const nl_double zz = z * z;
	//const nl_double zzz = zz * z;

	return (double)(1 << t)*(1.0 + z + 0.5 * zz); // + c3*zzz;
}

inline nl_double fastexp(const nl_double x)
{
	if (x<0)
		return 1.0 / fastexp_h(-x);
	else
		return fastexp_h(x);
}
#endif

class NETLIB_NAME(D) : public NETLIB_NAME(twoterm)
{
public:
	ATTR_COLD NETLIB_NAME(D)() : NETLIB_NAME(twoterm)(DIODE) { }

	NETLIB_UPDATE_TERMINALS()
	{
		m_D.update_diode(deltaV());
		set(m_D.G(), 0.0, m_D.Ieq());
	}

protected:
	ATTR_COLD virtual void start();
	ATTR_COLD virtual void update_param();
	ATTR_HOT ATTR_ALIGN void update();

	netlist_param_model_t m_model;

	netlist_generic_diode m_D;
};



#endif /* NLD_TWOTERM_H_ */
