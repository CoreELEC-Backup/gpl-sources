/*
 * nld_ms_direct1.h
 *
 */

#ifndef NLD_MS_DIRECT1_H_
#define NLD_MS_DIRECT1_H_

#include "nld_solver.h"
#include "nld_ms_direct.h"

class ATTR_ALIGNED(64) netlist_matrix_solver_direct1_t: public netlist_matrix_solver_direct_t<1,1>
{
public:

	netlist_matrix_solver_direct1_t(const netlist_solver_parameters_t &params)
		: netlist_matrix_solver_direct_t<1, 1>(params, 1)
		{}
	ATTR_HOT inline int vsolve_non_dynamic();
protected:
	ATTR_HOT virtual nl_double vsolve();
private:
};

// ----------------------------------------------------------------------------------------
// netlist_matrix_solver - Direct1
// ----------------------------------------------------------------------------------------

ATTR_HOT nl_double netlist_matrix_solver_direct1_t::vsolve()
{
	solve_base<netlist_matrix_solver_direct1_t>(this);
	return this->compute_next_timestep();
}

ATTR_HOT inline int netlist_matrix_solver_direct1_t::vsolve_non_dynamic()
{
	netlist_analog_net_t *net = m_nets[0];
	this->build_LE();
	//NL_VERBOSE_OUT(("%f %f\n", new_val, m_RHS[0] / m_A[0][0]);

	nl_double new_val =  m_RHS[0] / m_A[0][0];

	nl_double e = (new_val - net->m_cur_Analog);
	nl_double cerr = fabs(e);

	net->m_cur_Analog = new_val;

	if (is_dynamic() && (cerr  > m_params.m_accuracy))
	{
		return 2;
	}
	else
		return 1;

}



#endif /* NLD_MS_DIRECT1_H_ */
