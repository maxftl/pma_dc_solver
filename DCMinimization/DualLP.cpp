#include "DualLP.h"
#include <gurobi_c++.h>


DualLP::DualLP(const GRBEnv & env, const Eigen::ArrayXXd & valuations, const Eigen::VectorXd & demand, const Eigen::VectorXd & supply, double penalty): LinearProgram(env,valuations,demand,supply),
p_val(supply),
pi_val(demand),
penalty(penalty)
{
	build_model();
}


DualLP::~DualLP()
{
	delete(p);
	delete(pi);
}


Eigen::VectorXd DualLP::get_p()
{
	return p_val;
}

Eigen::VectorXd DualLP::get_pi()
{
	return pi_val;
}

void DualLP::set_oversupply(const Eigen::VectorXd & oversupply)
{
	GRBLinExpr objective_expr;
	for (int j = 0; j < num_goods; ++j) objective_expr += oversupply[j]*p[j];
	objective_expr += base_expr;
	model.setObjective(objective_expr, GRB_MINIMIZE);
}

double DualLP::get_objective_val()
{
	return objective_val;
}

void DualLP::optimization_done()
{
	for (int j = 0; j < num_goods; ++j) p_val[j] = p[j].get(GRB_DoubleAttr_X);
	for (int i = 0; i < num_bids; ++i) pi_val[i] = pi[i].get(GRB_DoubleAttr_X);
	objective_val = model.get(GRB_DoubleAttr_ObjVal);
}

void DualLP::build_model()
{
	p = model.addVars(num_goods);
	pi = model.addVars(num_bids);
	for (int i = 0; i < num_bids; ++i) {
		for (int j = 0; j < num_goods; ++j) {
			model.addConstr(pi[i] + p[j] >= valuations(i, j));
		}
	}
	base_expr = 0;
	for (int i = 0; i < num_bids; ++i) base_expr += demand[i] * pi[i];
	for (int j = 0; j < num_goods; ++j) base_expr += supply[j] * p[j];

	for (int j = 0; j < num_goods; ++j) base_expr += penalty * p[j];
	model.setObjective(base_expr, GRB_MINIMIZE);
}
