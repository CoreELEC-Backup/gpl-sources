/*
 * nlbase.c
 *
 */

#include "nl_base.h"
#include "devices/nld_system.h"
#include "analog/nld_solver.h"
#include "pstring.h"
#include "nl_util.h"

#include <stdlib.h> // FIXME: only included for atof

const netlist_time netlist_time::zero = netlist_time::from_raw(0);

netlist_logic_family_desc_t netlist_family_TTL =
{
		0.8, // m_low_thresh_V
		2.0, // m_high_thresh_V
		0.3, // m_low_V  - these depend on sinked/sourced current. Values should be suitable for typical applications.
		3.7, // m_high_V
		1.0, // m_R_low;
		130.0, //  m_R_high;
};

//FIXME: set to proper values
netlist_logic_family_desc_t netlist_family_CD4000 =
{
		0.8, // m_low_thresh_V
		2.0, // m_high_thresh_V
		0.3, // m_low_V  - these depend on sinked/sourced current. Values should be suitable for typical applications.
		3.7, // m_high_V
		1.0, // m_R_low;
		130.0, //  m_R_high;
};

// ----------------------------------------------------------------------------------------
// netlist_queue_t
// ----------------------------------------------------------------------------------------

netlist_queue_t::netlist_queue_t(netlist_base_t &nl)
	: netlist_timed_queue<netlist_net_t *, netlist_time, 512>()
	, netlist_object_t(QUEUE, GENERIC)
	, pstate_callback_t()
	, m_qsize(0)
{
	this->init_object(nl, "QUEUE");
}

void netlist_queue_t::register_state(pstate_manager_t &manager, const pstring &module)
{
	NL_VERBOSE_OUT(("register_state\n"));
	manager.save_item(m_qsize, this, module + "." + "qsize");
	manager.save_item(m_times, this, module + "." + "times");
	manager.save_item(&(m_name[0][0]), this, module + "." + "names", sizeof(m_name));
}

void netlist_queue_t::on_pre_save()
{
	NL_VERBOSE_OUT(("on_pre_save\n"));
	m_qsize = this->count();
	NL_VERBOSE_OUT(("current time %f qsize %d\n", netlist().time().as_double(), m_qsize));
	for (int i = 0; i < m_qsize; i++ )
	{
		m_times[i] =  this->listptr()[i].exec_time().as_raw();
		const char *p = this->listptr()[i].object()->name().cstr();
		int n = MIN(63, strlen(p));
		strncpy(&(m_name[i][0]), p, n);
		m_name[i][n] = 0;
	}
}


void netlist_queue_t::on_post_load()
{
	this->clear();
	NL_VERBOSE_OUT(("current time %f qsize %d\n", netlist().time().as_double(), m_qsize));
	for (int i = 0; i < m_qsize; i++ )
	{
		netlist_net_t *n = netlist().find_net(&(m_name[i][0]));
		//NL_VERBOSE_OUT(("Got %s ==> %p\n", qtemp[i].m_name, n));
		//NL_VERBOSE_OUT(("schedule time %f (%f)\n", n->time().as_double(), qtemp[i].m_time.as_double()));
		this->push(netlist_queue_t::entry_t(netlist_time::from_raw(m_times[i]), n));
	}
}

// ----------------------------------------------------------------------------------------
// netlist_object_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_object_t::netlist_object_t(const type_t atype, const family_t afamily)
: m_objtype(atype)
, m_family(afamily)
, m_netlist(NULL)
{}

ATTR_COLD netlist_object_t::~netlist_object_t()
{
	//delete m_name;
}

ATTR_COLD void netlist_object_t::init_object(netlist_base_t &nl, const pstring &aname)
{
	m_netlist = &nl;
	m_name = aname;
	save_register();
}

ATTR_COLD const pstring &netlist_object_t::name() const
{
	if (m_name == "")
		netlist().error("object not initialized");
	return m_name;
}

// ----------------------------------------------------------------------------------------
// netlist_owned_object_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_owned_object_t::netlist_owned_object_t(const type_t atype,
		const family_t afamily)
: netlist_object_t(atype, afamily)
, m_netdev(NULL)
{
}

ATTR_COLD void netlist_owned_object_t::init_object(netlist_core_device_t &dev,
		const pstring &aname)
{
	netlist_object_t::init_object(dev.netlist(), aname);
	m_netdev = &dev;
}

// ----------------------------------------------------------------------------------------
// netlist_base_t
// ----------------------------------------------------------------------------------------

netlist_base_t::netlist_base_t()
	:   netlist_object_t(NETLIST, GENERIC),
		m_time(netlist_time::zero),
		m_queue(*this),
		m_mainclock(NULL),
		m_solver(NULL),
		m_gnd(NULL),
		m_setup(NULL)
{
}

netlist_base_t::~netlist_base_t()
{
	for (int i=0; i < m_nets.count(); i++)
	{
		if (!m_nets[i]->isRailNet())
		{
			global_free(m_nets[i]);
		}
	}

	m_nets.clear();

	m_devices.clear_and_free();

	pstring::resetmem();
}

ATTR_COLD void netlist_base_t::save_register()
{
	save(static_cast<pstate_callback_t &>(m_queue), "m_queue");
	save(NLNAME(m_time));
	netlist_object_t::save_register();
}

ATTR_HOT const nl_double netlist_base_t::gmin() const
{
	return solver()->gmin();
}

ATTR_COLD void netlist_base_t::start()
{
	/* find the main clock and solver ... */

	m_mainclock = get_single_device<NETLIB_NAME(mainclock)>("mainclock");
	m_solver = get_single_device<NETLIB_NAME(solver)>("solver");
	m_gnd = get_single_device<NETLIB_NAME(gnd)>("gnd");

	/* make sure the solver is started first! */

	if (m_solver != NULL)
		m_solver->start_dev();

	NL_VERBOSE_OUT(("Initializing devices ...\n"));
	for (netlist_device_t * const * entry = m_devices.first(); entry != NULL; entry = m_devices.next(entry))
	{
		netlist_device_t *dev = *entry;
		if (dev != m_solver)
			dev->start_dev();
	}

}

ATTR_COLD netlist_net_t *netlist_base_t::find_net(const pstring &name)
{
	for (int i = 0; i < m_nets.count(); i++)
	{
		if (m_nets[i]->name() == name)
			return m_nets[i];
	}
	return NULL;
}

ATTR_COLD void netlist_base_t::rebuild_lists()
{
	for (int i = 0; i < m_nets.count(); i++)
		m_nets[i]->rebuild_list();
}


ATTR_COLD void netlist_base_t::reset()
{
	m_time = netlist_time::zero;
	m_queue.clear();
	if (m_mainclock != NULL)
		m_mainclock->m_Q.net().set_time(netlist_time::zero);
	if (m_solver != NULL)
		m_solver->do_reset();

	// Reset all nets once !
	for (int i = 0; i < m_nets.count(); i++)
		m_nets[i]->do_reset();

	// Reset all devices once !
	for (int i = 0; i < m_devices.count(); i++)
	{
		m_devices[i]->do_reset();
	}

	// Step all devices once !
	for (int i = 0; i < m_devices.count(); i++)
	{
		m_devices[i]->update_dev();
	}

	// FIXME: some const devices rely on this
	/* make sure params are set now .. */
	for (int i = 0; i < m_devices.count(); i++)
	{
		m_devices[i]->update_param();
	}
}


ATTR_HOT ATTR_ALIGN void netlist_base_t::process_queue(const netlist_time delta)
{
	m_stop = m_time + delta;

	if (m_mainclock == NULL)
	{
		while ( (m_time < m_stop) && (m_queue.is_not_empty()))
		{
			const netlist_queue_t::entry_t *e = m_queue.pop();
			m_time = e->exec_time();
			e->object()->update_devs();

			add_to_stat(m_perf_out_processed, 1);
		}
		if (m_queue.is_empty())
			m_time = m_stop;

	} else {
		netlist_logic_net_t &mc_net = m_mainclock->m_Q.net().as_logic();
		const netlist_time inc = m_mainclock->m_inc;
		netlist_time mc_time = mc_net.time();

		while (m_time < m_stop)
		{
			if (m_queue.is_not_empty())
			{
				while (m_queue.peek()->exec_time() > mc_time)
				{
					m_time = mc_time;
					mc_time += inc;
					NETLIB_NAME(mainclock)::mc_update(mc_net);
				}

				const netlist_queue_t::entry_t *e = m_queue.pop();
				m_time = e->exec_time();
				e->object()->update_devs();

			} else {
				m_time = mc_time;
				mc_time += inc;
				NETLIB_NAME(mainclock)::mc_update(mc_net);
			}

			add_to_stat(m_perf_out_processed, 1);
		}
		mc_net.set_time(mc_time);
	}
}

ATTR_COLD void netlist_base_t::error(const char *format, ...) const
{
	va_list ap;
	va_start(ap, format);
	verror(NL_ERROR, format, ap);
	va_end(ap);
}

ATTR_COLD void netlist_base_t::warning(const char *format, ...) const
{
	va_list ap;
	va_start(ap, format);
	verror(NL_WARNING, format, ap);
	va_end(ap);
}

ATTR_COLD void netlist_base_t::log(const char *format, ...) const
{
	va_list ap;
	va_start(ap, format);
	verror(NL_LOG, format, ap);
	va_end(ap);
}


// ----------------------------------------------------------------------------------------
// Default netlist elements ...
// ----------------------------------------------------------------------------------------



// ----------------------------------------------------------------------------------------
// net_core_device_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_core_device_t::netlist_core_device_t(const family_t afamily)
: netlist_object_t(DEVICE, afamily)
{
}

ATTR_COLD void netlist_core_device_t::init(netlist_base_t &anetlist, const pstring &name)
{
	init_object(anetlist, name);

#if USE_PMFDELEGATES
	void (netlist_core_device_t::* pFunc)() = &netlist_core_device_t::update;
	static_update = reinterpret_cast<net_update_delegate>((this->*pFunc));
#endif

}

ATTR_COLD netlist_core_device_t::~netlist_core_device_t()
{
}

ATTR_HOT ATTR_ALIGN const netlist_sig_t netlist_core_device_t::INPLOGIC_PASSIVE(netlist_logic_input_t &inp)
{
	if (inp.state() != netlist_input_t::STATE_INP_PASSIVE)
		return inp.Q();
	else
	{
		inp.activate();
		const netlist_sig_t ret = inp.Q();
		inp.inactivate();
		return ret;
	}
}


// ----------------------------------------------------------------------------------------
// netlist_device_t
// ----------------------------------------------------------------------------------------

netlist_device_t::netlist_device_t()
	: netlist_core_device_t(GENERIC),
		m_terminals(20)
{
}

netlist_device_t::netlist_device_t(const family_t afamily)
	: netlist_core_device_t(afamily),
		m_terminals(20)
{
}

netlist_device_t::~netlist_device_t()
{
	//NL_VERBOSE_OUT(("~net_device_t\n");
}

ATTR_COLD netlist_setup_t &netlist_device_t::setup()
{
	return netlist().setup();
}

ATTR_COLD void netlist_device_t::init(netlist_base_t &anetlist, const pstring &name)
{
	netlist_core_device_t::init(anetlist, name);
}


ATTR_COLD void netlist_device_t::register_sub(netlist_device_t &dev, const pstring &name)
{
	dev.init(netlist(), this->name() + "." + name);
	dev.start_dev();
}

ATTR_COLD void netlist_device_t::register_subalias(const pstring &name, netlist_core_terminal_t &term)
{
	pstring alias = this->name() + "." + name;

	// everything already fully qualified
	setup().register_alias_nofqn(alias, term.name());

	if (term.isType(netlist_terminal_t::INPUT) || term.isType(netlist_terminal_t::TERMINAL))
		m_terminals.add(alias);
}

ATTR_COLD void netlist_device_t::register_terminal(const pstring &name, netlist_terminal_t &port)
{
	setup().register_object(*this, name, port);
	if (port.isType(netlist_terminal_t::INPUT) || port.isType(netlist_terminal_t::TERMINAL))
		m_terminals.add(port.name());
}

ATTR_COLD void netlist_device_t::register_output(const pstring &name, netlist_output_t &port)
{
	port.m_logic_family = this->logic_family();
	setup().register_object(*this, name, port);
}

ATTR_COLD void netlist_device_t::register_input(const pstring &name, netlist_input_t &inp)
{
	inp.m_logic_family = this->logic_family();
	setup().register_object(*this, name, inp);
	m_terminals.add(inp.name());
}

ATTR_COLD void netlist_device_t::connect(netlist_core_terminal_t &t1, netlist_core_terminal_t &t2)
{
	setup().connect(t1, t2);
}


template <class C, class T>
ATTR_COLD void netlist_device_t::register_param(const pstring &sname, C &param, const T initialVal)
{
	pstring fullname = this->name() + "." + sname;
	param.init_object(*this, fullname);
	param.initial(initialVal);
	setup().register_object(*this, fullname, param);
}

template ATTR_COLD void netlist_device_t::register_param(const pstring &sname, netlist_param_double_t &param, const double initialVal);
template ATTR_COLD void netlist_device_t::register_param(const pstring &sname, netlist_param_double_t &param, const float initialVal);
template ATTR_COLD void netlist_device_t::register_param(const pstring &sname, netlist_param_int_t &param, const int initialVal);
template ATTR_COLD void netlist_device_t::register_param(const pstring &sname, netlist_param_logic_t &param, const int initialVal);
template ATTR_COLD void netlist_device_t::register_param(const pstring &sname, netlist_param_str_t &param, const char * const initialVal);
template ATTR_COLD void netlist_device_t::register_param(const pstring &sname, netlist_param_str_t &param, const pstring &initialVal);
template ATTR_COLD void netlist_device_t::register_param(const pstring &sname, netlist_param_model_t &param, const char * const initialVal);


// ----------------------------------------------------------------------------------------
// netlist_net_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_net_t::netlist_net_t(const family_t afamily)
	: netlist_object_t(NET, afamily)
	, m_new_Q(0)
	, m_cur_Q (0)
	, m_railterminal(NULL)
	, m_time(netlist_time::zero)
	, m_active(0)
	, m_in_queue(2)
	, m_cur_Analog(0.0)
{
};

ATTR_COLD netlist_net_t::~netlist_net_t()
{
	if (isInitialized())
		netlist().remove_save_items(this);
}

ATTR_COLD void netlist_net_t::init_object(netlist_base_t &nl, const pstring &aname)
{
	netlist_object_t::init_object(nl, aname);
	nl.m_nets.add(this);
}

ATTR_HOT void netlist_net_t::inc_active(netlist_core_terminal_t &term)
{
	m_active++;

	m_list_active.insert(term);

	if (USE_DEACTIVE_DEVICE)
	{
		if (m_active == 1 && m_in_queue > 0)
		{
			railterminal().netdev().inc_active();
			m_cur_Q = m_new_Q;
		}
	}

	if (m_active == 1 && m_in_queue == 0)
	{
		if (m_time > netlist().time())
		{
			m_in_queue = 1;     /* pending */
			netlist().push_to_queue(this, m_time);
		}
		else
		{
			m_cur_Q = m_new_Q;
			m_in_queue = 2;
		}
	}
}

ATTR_HOT void netlist_net_t::dec_active(netlist_core_terminal_t &term)
{
	m_active--;
	m_list_active.remove(term);

	if (USE_DEACTIVE_DEVICE)
	{
		if (m_active == 0)
			railterminal().netdev().dec_active();
	}
}

ATTR_COLD void netlist_net_t::register_railterminal(netlist_output_t &mr)
{
	nl_assert(m_railterminal == NULL);
	m_railterminal = &mr;
}

ATTR_COLD void netlist_net_t::rebuild_list()
{
	/* rebuild m_list */

	m_list_active.clear();
	for (int i=0; i < m_core_terms.count(); i++)
		if (m_core_terms[i]->state() != netlist_input_t::STATE_INP_PASSIVE)
			m_list_active.add(*m_core_terms[i]);
}

ATTR_COLD void netlist_net_t::save_register()
{
	save(NLNAME(m_time));
	save(NLNAME(m_active));
	save(NLNAME(m_in_queue));
	save(NLNAME(m_cur_Analog));
	save(NLNAME(m_cur_Q));
	save(NLNAME(m_new_Q));
	netlist_object_t::save_register();
}

ATTR_HOT ATTR_ALIGN static inline void update_dev(const netlist_core_terminal_t *inp, const UINT32 mask)
{
	if ((inp->state() & mask) != 0)
	{
		netlist_core_device_t &netdev = inp->netdev();
		begin_timing(netdev.total_time);
		inc_stat(netdev.stat_count);
		netdev.update_dev();
		end_timing(netdev().total_time);
	}
}

ATTR_HOT ATTR_ALIGN inline void netlist_net_t::update_devs()
{
	//assert(m_num_cons != 0);
	nl_assert(this->isRailNet());

	const UINT32 masks[4] = { 1, 5, 3, 1 };
	const UINT32 mask = masks[ (m_cur_Q  << 1) | m_new_Q ];
	netlist_core_terminal_t *p = m_list_active.first();

	m_in_queue = 2; /* mark as taken ... */
	m_cur_Q = m_new_Q;

	switch (m_active)
	{
	case 2:
		update_dev(p, mask);
		p = m_list_active.next(p);
		if (p == NULL) break;
	case 1:
		update_dev(p, mask);
		break;
	default:
		while (p != NULL)
		{
			update_dev(p, mask);
			p = m_list_active.next(p);
		}
		break;
	}

}

ATTR_COLD void netlist_net_t::reset()
{
	m_time = netlist_time::zero;
	m_active = 0;
	m_in_queue = 2;

	m_new_Q = 0;
	m_cur_Q = 0;
	m_cur_Analog = 0.0;

	/* rebuild m_list */

	m_list_active.clear();
	for (int i=0; i < m_core_terms.count(); i++)
		m_list_active.add(*m_core_terms[i]);

	for (int i=0; i < m_core_terms.count(); i++)
		m_core_terms[i]->do_reset();

	for (int i=0; i < m_core_terms.count(); i++)
		if (m_core_terms[i]->state() != netlist_input_t::STATE_INP_PASSIVE)
			m_active++;
}

ATTR_COLD void netlist_net_t::register_con(netlist_core_terminal_t &terminal)
{
	terminal.set_net(*this);

	m_core_terms.add(&terminal);

	if (terminal.state() != netlist_input_t::STATE_INP_PASSIVE)
		m_active++;
}

ATTR_COLD void netlist_net_t::move_connections(netlist_net_t *dest_net)
{
	for (int i = 0; i < m_core_terms.count(); i++)
	{
		netlist_core_terminal_t *p = m_core_terms[i];
		dest_net->register_con(*p);
	}
	m_core_terms.clear(); // FIXME: othernet needs to be free'd from memory
}

ATTR_COLD void netlist_net_t::merge_net(netlist_net_t *othernet)
{
	NL_VERBOSE_OUT(("merging nets ...\n"));
	if (othernet == NULL)
		return; // Nothing to do

	if (othernet == this)
	{
		netlist().warning("Connecting %s to itself. This may be right, though\n", this->name().cstr());
		return; // Nothing to do
	}

	if (this->isRailNet() && othernet->isRailNet())
		netlist().error("Trying to merge two rail nets: %s and %s\n", this->name().cstr(), othernet->name().cstr());

	if (othernet->isRailNet())
	{
		NL_VERBOSE_OUT(("othernet is railnet\n"));
		othernet->merge_net(this);
	}
	else
	{
		othernet->move_connections(this);
	}
}

// ----------------------------------------------------------------------------------------
// netlist_logic_net_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_logic_net_t::netlist_logic_net_t()
	: netlist_net_t(LOGIC)
{
};


ATTR_COLD void netlist_logic_net_t::reset()
{
	netlist_net_t::reset();
}

ATTR_COLD void netlist_logic_net_t::save_register()
{
	netlist_net_t::save_register();
}

// ----------------------------------------------------------------------------------------
// netlist_analog_net_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_analog_net_t::netlist_analog_net_t()
	: netlist_net_t(ANALOG)
	, m_DD_n_m_1(0.0)
	, m_h_n_m_1(1e-6)
	, m_solver(NULL)
{
};

ATTR_COLD void netlist_analog_net_t::reset()
{
	netlist_net_t::reset();
}

ATTR_COLD void netlist_analog_net_t::save_register()
{
	save(NLNAME(m_DD_n_m_1));
	save(NLNAME(m_h_n_m_1));
	netlist_net_t::save_register();
}

ATTR_COLD bool netlist_analog_net_t::already_processed(list_t *groups, int cur_group)
{
	if (isRailNet())
		return true;
	for (int i = 0; i <= cur_group; i++)
	{
		if (groups[i].contains(this))
			return true;
	}
	return false;
}

ATTR_COLD void netlist_analog_net_t::process_net(list_t *groups, int &cur_group)
{
	if (num_cons() == 0)
		return;
	/* add the net */
	//SOLVER_VERBOSE_OUT(("add %d - %s\n", cur_group, name().cstr()));
	groups[cur_group].add(this);
	for (int i = 0; i < m_core_terms.count(); i++)
	{
		netlist_core_terminal_t *p = m_core_terms[i];
		//SOLVER_VERBOSE_OUT(("terminal %s\n", p->name().cstr()));
		if (p->isType(netlist_terminal_t::TERMINAL))
		{
			//SOLVER_VERBOSE_OUT(("isterminal\n"));
			netlist_terminal_t *pt = static_cast<netlist_terminal_t *>(p);
			netlist_analog_net_t *other_net = &pt->m_otherterm->net().as_analog();
			if (!other_net->already_processed(groups, cur_group))
				other_net->process_net(groups, cur_group);
		}
	}
}


// ----------------------------------------------------------------------------------------
// netlist_core_terminal_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_core_terminal_t::netlist_core_terminal_t(const type_t atype, const family_t afamily)
: netlist_owned_object_t(atype, afamily)
, plinkedlist_element_t<netlist_core_terminal_t>()
, m_logic_family(NULL)
, m_net(NULL)
, m_state(STATE_NONEX)
{
}

ATTR_COLD void netlist_core_terminal_t::set_net(netlist_net_t &anet)
{
	m_net = &anet;
}

// ----------------------------------------------------------------------------------------
// netlist_terminal_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_terminal_t::netlist_terminal_t()
: netlist_core_terminal_t(TERMINAL, ANALOG)
, m_Idr1(NULL)
, m_go1(NULL)
, m_gt1(NULL)
, m_otherterm(NULL)
{
}

ATTR_HOT void netlist_terminal_t::schedule_solve()
{
	// FIXME: Remove this after we found a way to remove *ALL* twoterms connected to railnets only.
	if (net().as_analog().solver() != NULL)
		net().as_analog().solver()->update_forced();
}

ATTR_HOT void netlist_terminal_t::schedule_after(const netlist_time &after)
{
	// FIXME: Remove this after we found a way to remove *ALL* twoterms connected to railnets only.
	if (net().as_analog().solver() != NULL)
		net().as_analog().solver()->update_after(after);
}

ATTR_COLD void netlist_terminal_t::reset()
{
	//netlist_terminal_core_terminal_t::reset();
	set_state(STATE_INP_ACTIVE);
	set_ptr(m_Idr1, 0.0);
	set_ptr(m_go1, netlist().gmin());
	set_ptr(m_gt1, netlist().gmin());
}

ATTR_COLD void netlist_terminal_t::save_register()
{
	save(NLNAME(m_Idr1));
	save(NLNAME(m_go1));
	save(NLNAME(m_gt1));
	netlist_core_terminal_t::save_register();
}


// ----------------------------------------------------------------------------------------
// net_input_t
// ----------------------------------------------------------------------------------------

// ----------------------------------------------------------------------------------------
// net_output_t
// ----------------------------------------------------------------------------------------

netlist_output_t::netlist_output_t(const type_t atype, const family_t afamily)
	: netlist_core_terminal_t(atype, afamily)
{
	set_state(STATE_OUT);
}

ATTR_COLD netlist_output_t::~netlist_output_t()
{
}


ATTR_COLD void netlist_output_t::init_object(netlist_core_device_t &dev, const pstring &aname)
{
	netlist_core_terminal_t::init_object(dev, aname);
	net().init_object(dev.netlist(), aname + ".net");
	net().register_railterminal(*this);
}

// ----------------------------------------------------------------------------------------
// netlist_logic_output_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_logic_output_t::netlist_logic_output_t()
	: netlist_output_t(OUTPUT, LOGIC), m_proxy(NULL)
{
	this->set_net(m_my_net);
}

ATTR_COLD void netlist_logic_output_t::initial(const netlist_sig_t val)
{
	net().as_logic().initial(val);
}

// ----------------------------------------------------------------------------------------
// netlist_ttl_output_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_ttl_output_t::netlist_ttl_output_t()
	: netlist_logic_output_t()
{
}

// ----------------------------------------------------------------------------------------
// netlist_analog_output_t
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_analog_output_t::netlist_analog_output_t()
	: netlist_output_t(OUTPUT, ANALOG), m_proxied_net(NULL)
{
	this->set_net(m_my_net);

	net().as_analog().m_cur_Analog = 0.98;
}

ATTR_COLD void netlist_analog_output_t::initial(const nl_double val)
{
	net().as_analog().m_cur_Analog = val * 0.99;
}

// ----------------------------------------------------------------------------------------
// netlist_param_t & friends
// ----------------------------------------------------------------------------------------

ATTR_COLD netlist_param_t::netlist_param_t(const param_type_t atype)
	: netlist_owned_object_t(PARAM, ANALOG)
	, m_param_type(atype)
{
}

ATTR_COLD netlist_param_double_t::netlist_param_double_t()
	: netlist_param_t(DOUBLE)
	, m_param(0.0)
{
}

ATTR_COLD netlist_param_int_t::netlist_param_int_t()
	: netlist_param_t(INTEGER)
	, m_param(0)
{
}

ATTR_COLD netlist_param_logic_t::netlist_param_logic_t()
	: netlist_param_int_t()
{
}

ATTR_COLD netlist_param_str_t::netlist_param_str_t()
	: netlist_param_t(STRING)
	, m_param("")
{
}

ATTR_COLD netlist_param_model_t::netlist_param_model_t()
	: netlist_param_t(MODEL)
	, m_param("")
{
}

ATTR_COLD const pstring netlist_param_model_t::model_type() const
{
	pstring tmp = this->Value();
	// .model 1N914 D(Is=2.52n Rs=.568 N=1.752 Cjo=4p M=.4 tt=20n Iave=200m Vpk=75 mfg=OnSemi type=silicon)
	int p = tmp.find("(");
	int p1 = p;
	while (--p >= 0 && tmp[p] != ' ')
		;

	return tmp.substr(p+1, p1-p-1).ucase();
}


ATTR_COLD nl_double netlist_param_model_t::model_value(const pstring &entity, const nl_double defval) const
{
	pstring tmp = this->Value();
	// .model 1N914 D(Is=2.52n Rs=.568 N=1.752 Cjo=4p M=.4 tt=20n Iave=200m Vpk=75 mfg=OnSemi type=silicon)
	int p = tmp.ucase().find(entity.ucase() + "=");
	if (p>=0)
	{
		int pblank = tmp.find(" ", p);
		if (pblank < 0) pblank = tmp.len() + 1;
		tmp = tmp.substr(p, pblank - p);
		int pequal = tmp.find("=", 0);
		if (pequal < 0)
			netlist().error("parameter %s misformat in model %s temp %s\n", entity.cstr(), Value().cstr(), tmp.cstr());
		tmp = tmp.substr(pequal+1);
		nl_double factor = 1.0;
		switch (*(tmp.right(1).cstr()))
		{
			case 'm': factor = 1e-3; break;
			case 'u': factor = 1e-6; break;
			case 'n': factor = 1e-9; break;
			case 'p': factor = 1e-12; break;
			case 'f': factor = 1e-15; break;
			case 'a': factor = 1e-18; break;

		}
		if (factor != 1.0)
			tmp = tmp.left(tmp.len() - 1);
		return atof(tmp.cstr()) * factor;
	}
	else
		return defval;
}


// ----------------------------------------------------------------------------------------
// mainclock
// ----------------------------------------------------------------------------------------

ATTR_HOT inline void NETLIB_NAME(mainclock)::mc_update(netlist_logic_net_t &net)
{
	net.toggle_new_Q();
	net.update_devs();
}

NETLIB_START(mainclock)
{
	register_output("Q", m_Q);

	register_param("FREQ", m_freq, 7159000.0 * 5);
	m_inc = netlist_time::from_hz(m_freq.Value()*2);
}

NETLIB_RESET(mainclock)
{
	m_Q.net().set_time(netlist_time::zero);
}

NETLIB_UPDATE_PARAM(mainclock)
{
	m_inc = netlist_time::from_hz(m_freq.Value()*2);
}

NETLIB_UPDATE(mainclock)
{
	netlist_logic_net_t &net = m_Q.net().as_logic();
	// this is only called during setup ...
	net.toggle_new_Q();
	net.set_time(netlist().time() + m_inc);
}
