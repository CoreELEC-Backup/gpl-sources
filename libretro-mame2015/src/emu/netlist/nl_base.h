// license:GPL-2.0+
// copyright-holders:Couriersud
/*
 * nlbase.h
 *
 *  A mixed signal circuit simulation
 *
 *  D: Device
 *  O: Rail output (output)
 *  I: Infinite impedance input (input)
 *  T: Terminal (finite impedance)
 *
 *  +---+     +---+     +---+     +---+     +---+
 *  |   |     |   |     |   |     |   |     |   |
 *  | D |     | D |     | D |     | D |     | D |
 *  |   |     |   |     |   |     |   |     |   |
 *  +-O-+     +-I-+     +-I-+     +-T-+     +-T-+
 *    |         |         |         |         |
 *  +-+---------+---------+---------+---------+-+
 *  | rail net                                  |
 *  +-------------------------------------------+
 *
 *  A rail net is a net which is driven by exactly one output with an
 *  (idealized) internal resistance of zero.
 *  Ideally, it can deliver infinite current.
 *
 *  A infinite resistance input does not source or sink current.
 *
 *  Terminals source or sink finite (but never zero) current.
 *
 *  The system differentiates between analog and logic input and outputs and
 *  analog terminals. Analog and logic devices can not be connected to the
 *  same net. Instead, proxy devices are inserted automatically:
 *
 *  +---+     +---+
 *  |   |     |   |
 *  | D1|     | D2|
 *  | A |     | L |
 *  +-O-+     +-I-+
 *    |         |
 *  +-+---------+---+
 *  | rail net      |
 *  +---------------+
 *
 *  is converted into
 *              +----------+
 *              |          |
 *  +---+     +-+-+        |   +---+
 *  |   |     | L |  A-L   |   |   |
 *  | D1|     | D | Proxy  |   | D2|
 *  | A |     | A |        |   |   |
 *  +-O-+     +-I-+        |   +-I-+
 *    |         |          |     |
 *  +-+---------+--+     +-+-----+-------+
 *  | rail net (A) |     | rail net (L)  |
 *  +--------------|     +---------------+
 *
 *  This works both analog to logic as well as logic to analog.
 *
 *  The above is an advanced implementation of the existing discrete
 *  subsystem in MAME. Instead of relying on a fixed time-step, analog devices
 *  could either connect to fixed time-step clock or use an internal clock
 *  to update them. This would however introduce macro devices for RC, diodes
 *  and transistors again.
 *
 *  ============================================================================
 *
 *  Instead, the following approach in case of a pure terminal/input network
 *  is taken:
 *
 *  +---+     +---+     +---+     +---+     +---+
 *  |   |     |   |     |   |     |   |     |   |
 *  | D |     | D |     | D |     | D |     | D |
 *  |   |     |   |     |   |     |   |     |   |
 *  +-T-+     +-I-+     +-I-+     +-T-+     +-T-+
 *    |         |         |         |         |
 *   '+'        |         |        '-'       '-'
 *  +-+---------+---------+---------+---------+-+
 *  | Calculated net                            |
 *  +-------------------------------------------+
 *
 *  SPICE uses the following basic two terminal device:
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
 *  This is a resistance in series to a voltage source and paralleled by a
 *  current source. This is suitable to model voltage sources, current sources,
 *  resistors, capacitors, inductances and diodes.
 *
 *  I(n,l) = - I(n,k) = ( V(k) - V - V(l) ) * (1/R(n)) + I(n)
 *
 *  Now, the sum of all currents for a given net must be 0:
 *
 *  Sum(n,I(n,l)) = 0 = sum(n, ( V(k) - V(n) - V(l) ) * (1/R(n)) + I(n) )
 *
 *  With G(n) = 1 / R(n) and sum(n, G(n)) = Gtot and k=k(n)
 *
 *  0 = - V(l) * Gtot + sum(n, (V(k(n)) - V(n)) * G(n) + I(n))
 *
 *  and with l=l(n) and fixed k
 *
 *  0 =  -V(k) * Gtot + sum(n, ( V(l(n) + V(n) ) * G(n) - I(n))
 *
 *  These equations represent a linear Matrix equation (with more math).
 *
 *  In the end the solution of the analog subsystem boils down to
 *
 *  (G - D) * V = I
 *
 *  with G being the conductance matrix, D a diagonal matrix with the total
 *  conductance on the diagonal elements, V the net voltage vector and I the
 *  current vector.
 *
 *  By using solely two terminal devices, we can simplify the whole calculation
 *  significantly. A BJT now is a four terminal device with two terminals being
 *  connected internally.
 *
 *  The system is solved using an iterative approach:
 *
 *  G * V - D * V = I
 *
 *  assuming V=Vn=Vo
 *
 *  Vn = D-1 * (I - G * Vo)
 *
 *  Each terminal thus has three properties:
 *
 *  a) Resistance
 *  b) Voltage source
 *  c) Current source/sink
 *
 *  Going forward, the approach can be extended e.g. to use a linear
 *  equation solver.
 *
 *  The formal representation of the circuit will stay the same, thus scales.
 *
 */

#ifndef NLBASE_H_
#define NLBASE_H_

#include "nl_config.h"
#include "nl_lists.h"
#include "nl_time.h"
#include "nl_util.h"
#include "pstring.h"
#include "pstate.h"

// ----------------------------------------------------------------------------------------
// Type definitions
// ----------------------------------------------------------------------------------------

typedef UINT8 netlist_sig_t;

class netlist_core_device_t;

#if USE_PMFDELEGATES
typedef void (*net_update_delegate)(netlist_core_device_t *);
#endif

//============================================================
//  MACROS / netlist devices
//============================================================

#define NETLIB_NAME(_chip) nld_ ## _chip

#define NETLIB_NAME_STR_S(_s) # _s
#define NETLIB_NAME_STR(_chip) NETLIB_NAME_STR_S(nld_ ## _chip)

#define NETLIB_UPDATE(_chip) ATTR_HOT ATTR_ALIGN void NETLIB_NAME(_chip) :: update(void)
#define NETLIB_START(_chip) ATTR_COLD void NETLIB_NAME(_chip) :: start(void)
//#define NETLIB_CONSTRUCTOR(_chip) ATTR_COLD _chip :: _chip (netlist_setup_t &setup, const char *name)
//          : net_device_t(setup, name)

#define NETLIB_RESET(_chip) ATTR_COLD void NETLIB_NAME(_chip) :: reset(void)

#define NETLIB_UPDATE_PARAM(_chip) ATTR_HOT ATTR_ALIGN void NETLIB_NAME(_chip) :: update_param(void)
#define NETLIB_FUNC_VOID(_chip, _name, _params) ATTR_HOT ATTR_ALIGN void NETLIB_NAME(_chip) :: _name _params

#define NETLIB_UPDATE_TERMINALS() ATTR_HOT ATTR_ALIGN inline void update_terminals(void)
#define NETLIB_UPDATEI() ATTR_HOT ATTR_ALIGN inline void update(void)

#define NETLIB_DEVICE_BASE(_name, _pclass, _extra, _priv)                       \
	class _name : public _pclass                                                \
	{                                                                           \
	public:                                                                     \
		_name()                                                                 \
		: _pclass()    { }                                                      \
	protected:                                                                  \
		_extra                                                                  \
		ATTR_HOT void update();                                                 \
		ATTR_HOT void start();                                                  \
		ATTR_HOT void reset();                                                  \
		_priv                                                                   \
	}

#define NETLIB_DEVICE_DERIVED(_name, _pclass, _priv)                            \
		NETLIB_DEVICE_BASE(NETLIB_NAME(_name), NETLIB_NAME(_pclass), , _priv)

#define NETLIB_DEVICE(_name, _priv)                                             \
		NETLIB_DEVICE_BASE(NETLIB_NAME(_name), netlist_device_t, , _priv)

#define NETLIB_SUBDEVICE(_name, _priv)                                          \
	class NETLIB_NAME(_name) : public netlist_device_t                          \
	{                                                                           \
	public:                                                                     \
		NETLIB_NAME(_name) ()                                                   \
		: netlist_device_t()                                                    \
			{ }                                                                 \
	/*protected:*/                                                              \
		ATTR_HOT void update();                                                 \
		ATTR_HOT void start();                                                  \
		ATTR_HOT void reset();                                                  \
	public:                                                                     \
		_priv                                                                   \
	}

#define NETLIB_DEVICE_WITH_PARAMS(_name, _priv)                                 \
		NETLIB_DEVICE_BASE(NETLIB_NAME(_name), netlist_device_t,                \
			ATTR_HOT void update_param();                                       \
		, _priv)

#define NETLIB_DEVICE_WITH_PARAMS_DERIVED(_name, _pclass, _priv)                \
		NETLIB_DEVICE_BASE(NETLIB_NAME(_name), NETLIB_NAME(_pclass),            \
			ATTR_HOT void update_param();                                       \
		, _priv)

#define NETLIB_LOGIC_FAMILY(_fam)                                               \
ATTR_COLD virtual const netlist_logic_family_desc_t *logic_family()             \
{                                                                               \
	return &netlist_family_ ## _fam;                                            \
}



// -----------------------------------------------------------------------------
// forward definitions
// -----------------------------------------------------------------------------

class netlist_net_t;
class netlist_analog_net_t;
class netlist_logic_net_t;
class netlist_output_t;
class netlist_param_t;
class netlist_setup_t;
class netlist_base_t;
class netlist_matrix_solver_t;
class NETLIB_NAME(gnd);
class NETLIB_NAME(solver);
class NETLIB_NAME(mainclock);
class NETLIB_NAME(base_d_to_a_proxy);

// -----------------------------------------------------------------------------
// netlist_output_family_t
// -----------------------------------------------------------------------------

struct netlist_logic_family_desc_t
{
	nl_double m_low_thresh_V;
	nl_double m_high_thresh_V;
	nl_double m_low_V;
	nl_double m_high_V;
	nl_double m_R_low;
	nl_double m_R_high;
};

/* Terminals inherit the family description from the netlist_device
 * The default is the ttl family, but any device can override the family.
 * For individual terminals, these can be overwritten as well.
 *
 * Only devices of type GENERIC should have a family description entry
 */

extern netlist_logic_family_desc_t netlist_family_TTL;
extern netlist_logic_family_desc_t netlist_family_CD4000;


// -----------------------------------------------------------------------------
// netlist_state_t
// -----------------------------------------------------------------------------

template< typename X>
class netlist_state_t {
public:
	inline netlist_state_t() : m_x(static_cast<X>(0)) {}
	inline netlist_state_t(const X& x_) : m_x(x_) {}
	inline const X& get() const { return m_x; }
	inline X& ref() { return m_x; }
	inline operator const X&() const { return m_x; }
	inline operator X&() { return m_x; }
private:
	X m_x;
};

// -----------------------------------------------------------------------------
// netlist_object_t
// -----------------------------------------------------------------------------

class netlist_object_t
{
	NETLIST_PREVENT_COPYING(netlist_object_t)
public:
	enum type_t {
		TERMINAL = 0,
		INPUT    = 1,
		OUTPUT   = 2,
		PARAM    = 3,
		NET      = 4,
		DEVICE   = 5,
		NETLIST   = 6,
		QUEUE   = 7,
	};
	enum family_t {
		// Terminal families
		LOGIC,
		ANALOG,
		// Device families
		GENERIC,    // <== devices usually fall into this category
		TWOTERM,    // Generic twoterm ...
		RESISTOR,   // Resistor
		CAPACITOR,  // Capacitor
		DIODE,      // Diode
		DUMMY,      // DUMMY device without function
		FRONTIER,   // Net frontier
		BJT_EB,     // BJT(Ebers-Moll)
		BJT_SWITCH, // BJT(Switch)
		VCVS,       // Voltage controlled voltage source
		VCCS,       // Voltage controlled current source
		CCCS,       // Current controlled current source
		GND,        // GND device
	};

	ATTR_COLD netlist_object_t(const type_t atype, const family_t afamily);

	virtual ~netlist_object_t();

	ATTR_COLD void init_object(netlist_base_t &nl, const pstring &aname);
	ATTR_COLD bool isInitialized() { return (m_netlist != NULL); }

	ATTR_COLD const pstring &name() const;

	PSTATE_INTERFACE_DECL()
	template<typename C> ATTR_COLD void save(netlist_state_t<C> &state,
			const pstring &stname)
	{
		save(state.ref(), stname);
	}

	ATTR_HOT inline const type_t type() const { return m_objtype; }
	ATTR_HOT inline const family_t family() const { return m_family; }

	ATTR_HOT inline const bool isType(const type_t atype) const { return (m_objtype == atype); }
	ATTR_HOT inline const bool isFamily(const family_t afamily) const { return (m_family == afamily); }

	ATTR_HOT inline netlist_base_t & RESTRICT netlist() { return *m_netlist; }
	ATTR_HOT inline const netlist_base_t & RESTRICT netlist() const { return *m_netlist; }

	ATTR_COLD void inline do_reset()
	{
		reset();
	}

protected:

	ATTR_COLD virtual void reset() = 0;
	// must call parent save_register !
	ATTR_COLD virtual void save_register() { };

private:
	pstring m_name;
	const type_t m_objtype;
	const family_t m_family;
	netlist_base_t * RESTRICT m_netlist;
};

// -----------------------------------------------------------------------------
// netlist_owned_object_t
// -----------------------------------------------------------------------------

class netlist_owned_object_t : public netlist_object_t
{
	NETLIST_PREVENT_COPYING(netlist_owned_object_t)
public:
	ATTR_COLD netlist_owned_object_t(const type_t atype, const family_t afamily);

	ATTR_COLD void init_object(netlist_core_device_t &dev, const pstring &aname);

	ATTR_HOT inline netlist_core_device_t & RESTRICT netdev() const { return *m_netdev; }
private:
	netlist_core_device_t * RESTRICT m_netdev;
};

// -----------------------------------------------------------------------------
// netlist_core_terminal_t
// -----------------------------------------------------------------------------

class netlist_core_terminal_t : public netlist_owned_object_t, public plinkedlist_element_t<netlist_core_terminal_t>
{
	NETLIST_PREVENT_COPYING(netlist_core_terminal_t)
public:

	typedef plinearlist_t<netlist_core_terminal_t *> list_t;

	/* needed here ... */

	enum state_e {
		STATE_INP_PASSIVE = 0,
		STATE_INP_ACTIVE = 1,
		STATE_INP_HL = 2,
		STATE_INP_LH = 4,
		STATE_OUT = 128,
		STATE_NONEX = 256
	};


	ATTR_COLD netlist_core_terminal_t(const type_t atype, const family_t afamily);

	//ATTR_COLD void init_object(netlist_core_device_t &dev, const pstring &aname);

	ATTR_COLD void set_net(netlist_net_t &anet);
	ATTR_COLD inline void clear_net() { m_net = NULL; }
	ATTR_HOT inline bool has_net() const { return (m_net != NULL); }

	ATTR_HOT inline const netlist_net_t & RESTRICT net() const { return *m_net;}
	ATTR_HOT inline netlist_net_t & RESTRICT net() { return *m_net;}

	ATTR_HOT inline const bool is_state(const state_e astate) const { return (m_state == astate); }
	ATTR_HOT inline const state_e state() const { return m_state; }
	ATTR_HOT inline void set_state(const state_e astate)
	{
		nl_assert(astate != STATE_NONEX);
		m_state = astate;
	}

	const netlist_logic_family_desc_t *m_logic_family;

protected:
	ATTR_COLD virtual void save_register()
	{
		save(NLNAME(m_state));
		netlist_owned_object_t::save_register();
	}

private:
	netlist_net_t * RESTRICT m_net;
	state_e m_state;
};

NETLIST_SAVE_TYPE(netlist_core_terminal_t::state_e, DT_INT);


class ATTR_ALIGN netlist_terminal_t : public netlist_core_terminal_t
{
	NETLIST_PREVENT_COPYING(netlist_terminal_t)
public:

	typedef plinearlist_t<netlist_terminal_t * RESTRICT> list_t;

	ATTR_COLD netlist_terminal_t();

	nl_double *m_Idr1; // drive current
	nl_double *m_go1;  // conductance for Voltage from other term
	nl_double *m_gt1;  // conductance for total conductance

	ATTR_HOT inline void set(const nl_double G)
	{
		set_ptr(m_Idr1, 0);
		set_ptr(m_go1, G);
		set_ptr(m_gt1, G);
	}

	ATTR_HOT inline void set(const nl_double GO, const nl_double GT)
	{
		set_ptr(m_Idr1, 0);
		set_ptr(m_go1, GO);
		set_ptr(m_gt1, GT);
	}

	ATTR_HOT inline void set(const nl_double GO, const nl_double GT, const nl_double I)
	{
		set_ptr(m_Idr1, I);
		set_ptr(m_go1, GO);
		set_ptr(m_gt1, GT);
	}

	ATTR_HOT void schedule_solve();
	ATTR_HOT void schedule_after(const netlist_time &after);

	netlist_terminal_t *m_otherterm;

protected:
	ATTR_COLD virtual void save_register();

	ATTR_COLD virtual void reset();
private:
	inline void set_ptr(nl_double *ptr, const nl_double val)
	{
		if (ptr != NULL)
			*ptr = val;
	}
};


// -----------------------------------------------------------------------------
// netlist_input_t
// -----------------------------------------------------------------------------

class netlist_input_t : public netlist_core_terminal_t
{
public:


	ATTR_COLD netlist_input_t(const type_t atype, const family_t afamily)
		: netlist_core_terminal_t(atype, afamily)
	{
		set_state(STATE_INP_ACTIVE);
	}

	ATTR_HOT inline void inactivate();
	ATTR_HOT inline void activate();

protected:
	ATTR_COLD virtual void reset()
	{
		//netlist_core_terminal_t::reset();
		set_state(STATE_INP_ACTIVE);
	}

private:
};

// -----------------------------------------------------------------------------
// netlist_logic_input_t
// -----------------------------------------------------------------------------

class netlist_logic_input_t : public netlist_input_t
{
public:
	ATTR_COLD netlist_logic_input_t()
		: netlist_input_t(INPUT, LOGIC)
	{
	}

	ATTR_HOT inline const netlist_sig_t Q() const;
	ATTR_HOT inline const netlist_sig_t last_Q() const;

	ATTR_HOT inline void activate_hl();
	ATTR_HOT inline void activate_lh();

};

// -----------------------------------------------------------------------------
// netlist_ttl_input_t
// -----------------------------------------------------------------------------

class netlist_ttl_input_t : public netlist_logic_input_t
{
public:
	ATTR_COLD netlist_ttl_input_t()
		: netlist_logic_input_t() { }
};

// -----------------------------------------------------------------------------
// netlist_analog_input_t
// -----------------------------------------------------------------------------

class netlist_analog_input_t : public netlist_input_t
{
public:
	ATTR_COLD netlist_analog_input_t()
		: netlist_input_t(INPUT, ANALOG) { }

	ATTR_HOT inline const nl_double Q_Analog() const;
};

//#define INPVAL(_x) (_x).Q()

// -----------------------------------------------------------------------------
// net_net_t
// -----------------------------------------------------------------------------

class netlist_net_t : public netlist_object_t
{
	NETLIST_PREVENT_COPYING(netlist_net_t)
public:

	typedef plinearlist_t<netlist_net_t *> list_t;

	ATTR_COLD netlist_net_t(const family_t afamily);
	ATTR_COLD virtual ~netlist_net_t();

	ATTR_COLD void init_object(netlist_base_t &nl, const pstring &aname);

	ATTR_COLD void register_con(netlist_core_terminal_t &terminal);
	ATTR_COLD void merge_net(netlist_net_t *othernet);
	ATTR_COLD void register_railterminal(netlist_output_t &mr);

	ATTR_HOT inline netlist_logic_net_t & RESTRICT as_logic();
	ATTR_HOT inline const netlist_logic_net_t & RESTRICT as_logic() const;

	ATTR_HOT inline netlist_analog_net_t & RESTRICT as_analog();
	ATTR_HOT inline const netlist_analog_net_t & RESTRICT as_analog() const;

	ATTR_HOT void update_devs();

	ATTR_HOT inline const netlist_time time() const { return m_time; }
	ATTR_HOT inline void set_time(const netlist_time ntime) { m_time = ntime; }

	ATTR_HOT inline bool isRailNet() const { return !(m_railterminal == NULL); }
	ATTR_HOT inline const netlist_core_terminal_t & RESTRICT  railterminal() const { return *m_railterminal; }

	ATTR_HOT inline void push_to_queue(const netlist_time delay);
	ATTR_HOT inline void reschedule_in_queue(const netlist_time delay);
	ATTR_HOT bool inline is_queued() const { return m_in_queue == 1; }

	ATTR_HOT inline int num_cons() const { return m_core_terms.count(); }

	ATTR_HOT void inc_active(netlist_core_terminal_t &term);
	ATTR_HOT void dec_active(netlist_core_terminal_t &term);

	ATTR_COLD void rebuild_list();     /* rebuild m_list after a load */

	ATTR_COLD void move_connections(netlist_net_t *new_net);

	plinearlist_t<netlist_core_terminal_t *> m_core_terms; // save post-start m_list ...

protected:  //FIXME: needed by current solver code

	ATTR_COLD virtual void save_register();
	ATTR_COLD virtual void reset();

	netlist_sig_t m_new_Q;
	netlist_sig_t m_cur_Q;

private:

	netlist_core_terminal_t * RESTRICT m_railterminal;
	plinkedlist_t<netlist_core_terminal_t> m_list_active;

	netlist_time m_time;
	INT32        m_active;
	UINT8        m_in_queue;    /* 0: not in queue, 1: in queue, 2: last was taken */

public:
	// We have to have those on one object. Dividing those does lead
	// to a significant performance hit
	// FIXME: Have to fix the public at some time
	nl_double m_cur_Analog;

};

class netlist_logic_net_t : public netlist_net_t
{
	NETLIST_PREVENT_COPYING(netlist_logic_net_t)
public:

	typedef plinearlist_t<netlist_logic_net_t *> list_t;

	ATTR_COLD netlist_logic_net_t();
	ATTR_COLD virtual ~netlist_logic_net_t() { };

	ATTR_HOT inline const netlist_sig_t Q() const
	{
		return m_cur_Q;
	}

	ATTR_HOT inline const netlist_sig_t new_Q() const
	{
		return m_new_Q;
	}

	ATTR_HOT inline void set_Q(const netlist_sig_t newQ, const netlist_time delay)
	{
		if (EXPECTED(newQ !=  m_new_Q))
		{
			m_new_Q = newQ;
			push_to_queue(delay);
		}
	}

	ATTR_HOT inline void toggle_new_Q()
	{
		m_new_Q ^= 1;
	}

	ATTR_COLD void initial(const netlist_sig_t val)
	{
		m_cur_Q = val;
		m_new_Q = val;
	}

	/* internal state support
	 * FIXME: get rid of this and implement export/import in MAME
	 */
	ATTR_COLD inline netlist_sig_t &Q_state_ptr()
	{
		nl_assert(family() == LOGIC);
		return m_cur_Q;
	}

protected:  //FIXME: needed by current solver code

	ATTR_COLD virtual void save_register();
	ATTR_COLD virtual void reset();


private:

public:

};

class netlist_analog_net_t : public netlist_net_t
{
	NETLIST_PREVENT_COPYING(netlist_analog_net_t)
public:

	typedef plinearlist_t<netlist_analog_net_t *> list_t;

	ATTR_COLD netlist_analog_net_t();
	ATTR_COLD virtual ~netlist_analog_net_t() { };

	ATTR_HOT inline const nl_double Q_Analog() const
	{
		//nl_assert(object_type(SIGNAL_MASK) == SIGNAL_ANALOG);
		nl_assert(family() == ANALOG);
		return m_cur_Analog;
	}

	ATTR_COLD inline nl_double &Q_Analog_state_ptr()
	{
		//nl_assert(object_type(SIGNAL_MASK) == SIGNAL_ANALOG);
		nl_assert(family() == ANALOG);
		return m_cur_Analog;
	}

	ATTR_HOT inline netlist_matrix_solver_t *solver() { return m_solver; }

	ATTR_COLD bool already_processed(list_t *groups, int cur_group);
	ATTR_COLD void process_net(list_t *groups, int &cur_group);

protected:

	ATTR_COLD virtual void save_register();
	ATTR_COLD virtual void reset();


private:

public:
	nl_double m_DD_n_m_1;
	nl_double m_h_n_m_1;

	//FIXME: needed by current solver code
	netlist_matrix_solver_t *m_solver;
};

// -----------------------------------------------------------------------------
// net_output_t
// -----------------------------------------------------------------------------

class netlist_output_t : public netlist_core_terminal_t
{
	NETLIST_PREVENT_COPYING(netlist_output_t)
public:

	ATTR_COLD netlist_output_t(const type_t atype, const family_t afamily);
	ATTR_COLD virtual ~netlist_output_t();

	ATTR_COLD void init_object(netlist_core_device_t &dev, const pstring &aname);
	ATTR_COLD virtual void reset()
	{
		set_state(STATE_OUT);
	}

private:
};


class netlist_logic_output_t : public netlist_output_t
{
	NETLIST_PREVENT_COPYING(netlist_logic_output_t)
public:

	ATTR_COLD netlist_logic_output_t();

	ATTR_COLD void initial(const netlist_sig_t val);

	ATTR_COLD bool has_proxy() const { return (m_proxy != NULL); }
	ATTR_COLD nld_base_d_to_a_proxy *get_proxy() const  { return m_proxy; }
	ATTR_COLD void set_proxy(nld_base_d_to_a_proxy *proxy) { m_proxy = proxy; }

	ATTR_HOT inline void set_Q(const netlist_sig_t newQ, const netlist_time delay)
	{
		net().as_logic().set_Q(newQ, delay);
	}

private:
	netlist_logic_net_t m_my_net;
	nld_base_d_to_a_proxy *m_proxy;
};

class netlist_ttl_output_t : public netlist_logic_output_t
{
public:

	ATTR_COLD netlist_ttl_output_t();

};

class netlist_analog_output_t : public netlist_output_t
{
	NETLIST_PREVENT_COPYING(netlist_analog_output_t)
public:

	ATTR_COLD netlist_analog_output_t();

	ATTR_COLD void initial(const nl_double val);

	ATTR_HOT inline void set_Q(const nl_double newQ);

	netlist_analog_net_t *m_proxied_net; // only for proxy nets in analog input logic

private:
	netlist_analog_net_t m_my_net;
};

// -----------------------------------------------------------------------------
// net_param_t
// -----------------------------------------------------------------------------

class netlist_param_t : public netlist_owned_object_t
{
	NETLIST_PREVENT_COPYING(netlist_param_t)
public:

	enum param_type_t {
		MODEL,
		STRING,
		DOUBLE,
		INTEGER,
		LOGIC
	};

	ATTR_COLD netlist_param_t(const param_type_t atype);

	ATTR_HOT inline const param_type_t param_type() const { return m_param_type; }

protected:

	ATTR_COLD virtual void reset() { }

private:
	const param_type_t m_param_type;
};

class netlist_param_double_t : public netlist_param_t
{
	NETLIST_PREVENT_COPYING(netlist_param_double_t)
public:
	ATTR_COLD netlist_param_double_t();

	ATTR_HOT inline void setTo(const nl_double param);
	ATTR_COLD inline void initial(const nl_double val) { m_param = val; }
	ATTR_HOT inline const nl_double Value() const        { return m_param;   }

protected:
	ATTR_COLD virtual void save_register()
	{
		save(NLNAME(m_param));
		netlist_param_t::save_register();
	}

private:
	nl_double m_param;
};

class netlist_param_int_t : public netlist_param_t
{
	NETLIST_PREVENT_COPYING(netlist_param_int_t)
public:
	ATTR_COLD netlist_param_int_t();

	ATTR_HOT inline void setTo(const int param);
	ATTR_COLD inline void initial(const int val) { m_param = val; }

	ATTR_HOT inline const int Value() const     { return m_param;     }

protected:
	ATTR_COLD virtual void save_register()
	{
		save(NLNAME(m_param));
		netlist_param_t::save_register();
	}

private:
	int m_param;
};

class netlist_param_logic_t : public netlist_param_int_t
{
	NETLIST_PREVENT_COPYING(netlist_param_logic_t)
public:
	ATTR_COLD netlist_param_logic_t();
};

class netlist_param_str_t : public netlist_param_t
{
	NETLIST_PREVENT_COPYING(netlist_param_str_t)
public:
	ATTR_COLD netlist_param_str_t();

	ATTR_HOT inline void setTo(const pstring &param);
	ATTR_COLD inline void initial(const pstring &val) { m_param = val; }

	ATTR_HOT inline const pstring &Value() const     { return m_param;     }

private:
	pstring m_param;
};

class netlist_param_model_t : public netlist_param_t
{
	NETLIST_PREVENT_COPYING(netlist_param_model_t)
public:
	ATTR_COLD netlist_param_model_t();

	ATTR_COLD inline void initial(const pstring &val) { m_param = val; }

	ATTR_HOT inline const pstring &Value() const     { return m_param;     }

	/* these should be cached! */
	ATTR_COLD nl_double model_value(const pstring &entity, const nl_double defval = 0.0) const;
	ATTR_COLD const pstring model_type() const;

private:
	pstring m_param;
};

// -----------------------------------------------------------------------------
// net_device_t
// -----------------------------------------------------------------------------

class netlist_core_device_t : public netlist_object_t
{
	NETLIST_PREVENT_COPYING(netlist_core_device_t)
public:

	typedef plinearlist_t<netlist_core_device_t *> list_t;

	ATTR_COLD netlist_core_device_t(const family_t afamily);

	ATTR_COLD virtual ~netlist_core_device_t();

	ATTR_COLD virtual void init(netlist_base_t &anetlist, const pstring &name);
	ATTR_HOT virtual void update_param() {}

	ATTR_HOT inline void update_dev()
	{
#if USE_PMFDELEGATES
		static_update(this);
#else
		update();
#endif
	}
	ATTR_HOT inline void start_dev()
	{
		start();
	}

	ATTR_HOT const netlist_sig_t INPLOGIC_PASSIVE(netlist_logic_input_t &inp);

	ATTR_HOT inline const netlist_sig_t INPLOGIC(const netlist_logic_input_t &inp) const
	{
		nl_assert(inp.state() != netlist_input_t::STATE_INP_PASSIVE);
		return inp.Q();
	}

	ATTR_HOT inline void OUTLOGIC(netlist_logic_output_t &out, const netlist_sig_t val, const netlist_time delay)
	{
		out.set_Q(val, delay);
	}

	ATTR_HOT inline const nl_double INPANALOG(const netlist_analog_input_t &inp) const { return inp.Q_Analog(); }

	ATTR_HOT inline const nl_double TERMANALOG(const netlist_terminal_t &term) const { return term.net().as_analog().Q_Analog(); }

	ATTR_HOT inline void OUTANALOG(netlist_analog_output_t &out, const nl_double val)
	{
		out.set_Q(val);
	}

	ATTR_HOT virtual void inc_active() {  }

	ATTR_HOT virtual void dec_active() {  }

	ATTR_HOT virtual void step_time(const nl_double st) { }
	ATTR_HOT virtual void update_terminals() { }



#if (NL_KEEP_STATISTICS)
	/* stats */
	osd_ticks_t total_time;
	INT32 stat_count;
#endif

#if USE_PMFDELEGATES
	net_update_delegate static_update;
#endif

protected:

	ATTR_HOT virtual void update() { }
	ATTR_COLD virtual void start() { }
	ATTR_COLD virtual const netlist_logic_family_desc_t *logic_family()
	{
		return &netlist_family_TTL;
	}

private:
};


class netlist_device_t : public netlist_core_device_t
{
	NETLIST_PREVENT_COPYING(netlist_device_t)
public:

	ATTR_COLD netlist_device_t();
	ATTR_COLD netlist_device_t(const family_t afamily);

	ATTR_COLD virtual ~netlist_device_t();

	ATTR_COLD virtual void init(netlist_base_t &anetlist, const pstring &name);

	ATTR_COLD netlist_setup_t &setup();

	ATTR_COLD void register_sub(netlist_device_t &dev, const pstring &name);
	ATTR_COLD void register_subalias(const pstring &name, netlist_core_terminal_t &term);
	ATTR_COLD void register_terminal(const pstring &name, netlist_terminal_t &port);
	ATTR_COLD void register_output(const pstring &name, netlist_output_t &out);
	ATTR_COLD void register_input(const pstring &name, netlist_input_t &in);

	ATTR_COLD void connect(netlist_core_terminal_t &t1, netlist_core_terminal_t &t2);

	plinearlist_t<pstring, 20> m_terminals;

protected:

	ATTR_HOT virtual void update() { }
	ATTR_HOT virtual void start() { }
	ATTR_HOT virtual void update_terminals() { }

	template <class C, class T>
	ATTR_COLD void register_param(const pstring &sname, C &param, const T initialVal);

private:
};


// -----------------------------------------------------------------------------
// netlist_queue_t
// -----------------------------------------------------------------------------

class netlist_queue_t : public netlist_timed_queue<netlist_net_t *, netlist_time, 512>,
						public netlist_object_t,
						public pstate_callback_t
{
public:
	netlist_queue_t(netlist_base_t &nl);

protected:

	void reset() {}

	void register_state(pstate_manager_t &manager, const pstring &module);
	void on_pre_save();
	void on_post_load();

private:
	int m_qsize;
	netlist_time::INTERNALTYPE m_times[512];
	char m_name[512][64];
};

// -----------------------------------------------------------------------------
// netlist_base_t
// -----------------------------------------------------------------------------


class netlist_base_t : public netlist_object_t, public pstate_manager_t
{
	NETLIST_PREVENT_COPYING(netlist_base_t)
public:

	netlist_base_t();
	virtual ~netlist_base_t();

	ATTR_COLD void start();

	ATTR_HOT inline const netlist_queue_t &queue() const { return m_queue; }
	ATTR_HOT inline netlist_queue_t &queue() { return m_queue; }
	ATTR_HOT inline const netlist_time time() const { return m_time; }
	ATTR_HOT inline NETLIB_NAME(solver) *solver() const { return m_solver; }
	ATTR_HOT inline NETLIB_NAME(gnd) *gnd() const { return m_gnd; }
	ATTR_HOT const nl_double gmin() const;

	ATTR_HOT inline void push_to_queue(netlist_net_t *out, const netlist_time attime)
	{
		m_queue.push(netlist_queue_t::entry_t(attime, out));
	}

	ATTR_HOT inline void remove_from_queue(netlist_net_t *out)
	{
		m_queue.remove(out);
	}

	ATTR_HOT void process_queue(const netlist_time delta);
	ATTR_HOT inline void abort_current_queue_slice() { m_stop = netlist_time::zero; }

	ATTR_COLD void rebuild_lists(); /* must be called after post_load ! */

	ATTR_COLD void set_setup(netlist_setup_t *asetup) { m_setup = asetup;  }
	ATTR_COLD netlist_setup_t &setup() { return *m_setup; }

	ATTR_COLD netlist_net_t *find_net(const pstring &name);

	ATTR_COLD void error(const char *format, ...) const ATTR_PRINTF(2,3);
	ATTR_COLD void warning(const char *format, ...) const ATTR_PRINTF(2,3);
	ATTR_COLD void log(const char *format, ...) const ATTR_PRINTF(2,3);

	template<class _C>
	plinearlist_t<_C *> get_device_list()
	{
		plinearlist_t<_C *> tmp;
		for (netlist_device_t * const *entry = m_devices.first(); entry != NULL; entry = m_devices.next(entry))
		{
			_C *dev = dynamic_cast<_C *>(*entry);
			if (dev != NULL)
				tmp.add(dev);
		}
		return tmp;
	}

	template<class _C>
	_C *get_first_device()
	{
		//FIXME:
		for (netlist_device_t * const *entry = m_devices.first(); entry != NULL; entry = m_devices.next(entry))
		{
			_C *dev = dynamic_cast<_C *>(*entry);
			if (dev != NULL)
				return dev;
		}
		return NULL;
	}

	template<class _C>
	_C *get_single_device(const char *classname)
	{
		_C *ret = NULL;
		for (netlist_device_t * const *entry = m_devices.first(); entry != NULL; entry = m_devices.next(entry))
		{
			_C *dev = dynamic_cast<_C *>(*entry);
			if (dev != NULL)
			{
				if (ret != NULL)
					this->error("more than one %s device found", classname);
				else
					ret = dev;
			}
		}
		return ret;
	}

	pnamedlist_t<netlist_device_t *> m_devices;
	netlist_net_t::list_t m_nets;

protected:

	enum loglevel_e
	{
		NL_ERROR,
		NL_WARNING,
		NL_LOG,
	};

	// any derived netlist must override this ...
	ATTR_COLD virtual void verror(const loglevel_e level,
			const char *format, va_list ap) const = 0;

	/* from netlist_object */
	ATTR_COLD virtual void reset();
	ATTR_COLD virtual void save_register();

#if (NL_KEEP_STATISTICS)
	// performance
	int m_perf_out_processed;
	int m_perf_inp_processed;
	int m_perf_inp_active;
#endif

private:
	netlist_time                m_stop;     // target time for current queue processing

	netlist_time                m_time;
	netlist_queue_t             m_queue;

	NETLIB_NAME(mainclock) *    m_mainclock;
	NETLIB_NAME(solver) *       m_solver;
	NETLIB_NAME(gnd) *          m_gnd;

	netlist_setup_t *m_setup;
};

// -----------------------------------------------------------------------------
// Inline implementations
// -----------------------------------------------------------------------------

PSTATE_INTERFACE(netlist_object_t, m_netlist, name())

ATTR_HOT inline void netlist_param_str_t::setTo(const pstring &param)
{
	m_param = param;
	netdev().update_param();
}

ATTR_HOT inline void netlist_param_int_t::setTo(const int param)
{
	if (m_param != param)
	{
		m_param = param;
		netdev().update_param();
	}
}

ATTR_HOT inline void netlist_param_double_t::setTo(const nl_double param)
{
	if (m_param != param)
	{
		m_param = param;
		netdev().update_param();
	}
}

ATTR_HOT inline netlist_logic_net_t & RESTRICT netlist_net_t::as_logic()
{
	nl_assert(family() == LOGIC);
	return static_cast<netlist_logic_net_t &>(*this);
}

ATTR_HOT inline const netlist_logic_net_t & RESTRICT netlist_net_t::as_logic() const
{
	nl_assert(family() == LOGIC);
	return static_cast<const netlist_logic_net_t &>(*this);
}

ATTR_HOT inline netlist_analog_net_t & RESTRICT netlist_net_t::as_analog()
{
	nl_assert(family() == ANALOG);
	return static_cast<netlist_analog_net_t &>(*this);
}

ATTR_HOT inline const netlist_analog_net_t & RESTRICT netlist_net_t::as_analog() const
{
	nl_assert(family() == ANALOG);
	return static_cast<const netlist_analog_net_t &>(*this);
}


ATTR_HOT inline void netlist_input_t::inactivate()
{
	if (EXPECTED(!is_state(STATE_INP_PASSIVE)))
	{
		set_state(STATE_INP_PASSIVE);
		net().as_logic().dec_active(*this);
	}
}

ATTR_HOT inline void netlist_input_t::activate()
{
	if (is_state(STATE_INP_PASSIVE))
	{
		net().as_logic().inc_active(*this);
		set_state(STATE_INP_ACTIVE);
	}
}

ATTR_HOT inline void netlist_logic_input_t::activate_hl()
{
	if (is_state(STATE_INP_PASSIVE))
	{
		net().as_logic().inc_active(*this);
		set_state(STATE_INP_HL);
	}
}

ATTR_HOT inline void netlist_logic_input_t::activate_lh()
{
	if (is_state(STATE_INP_PASSIVE))
	{
		net().as_logic().inc_active(*this);
		set_state(STATE_INP_LH);
	}
}


ATTR_HOT inline void netlist_net_t::push_to_queue(const netlist_time delay)
{
	//if (UNEXPECTED(m_num_cons == 0 || is_queued()))
	if (!is_queued())
	{
		m_time = netlist().time() + delay;
		m_in_queue = (m_active > 0);     /* queued ? */
		if (EXPECTED(m_in_queue))
		{
			netlist().push_to_queue(this, m_time);
		}
	}
}

ATTR_HOT inline void netlist_net_t::reschedule_in_queue(const netlist_time delay)
{
	//if (UNEXPECTED(m_num_cons == 0 || is_queued()))
	if (is_queued())
		netlist().remove_from_queue(this);

	m_time = netlist().time() + delay;
	m_in_queue = (m_active > 0);     /* queued ? */
	if (EXPECTED(m_in_queue))
	{
		netlist().push_to_queue(this, m_time);
	}
}


ATTR_HOT inline const netlist_sig_t netlist_logic_input_t::Q() const
{
	return net().as_logic().Q();
}

ATTR_HOT inline const nl_double netlist_analog_input_t::Q_Analog() const
{
	return net().as_analog().Q_Analog();
}

ATTR_HOT inline void netlist_analog_output_t::set_Q(const nl_double newQ)
{
	if (newQ != net().as_analog().m_cur_Analog)
	{
		net().as_analog().m_cur_Analog = newQ;
		net().push_to_queue(NLTIME_FROM_NS(1));
	}
}

#endif /* NLBASE_H_ */
