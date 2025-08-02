#pragma once
#include <Eigen/Dense>
#include <gurobi_c++.h>
#include "LinearProgram.h"

class DualLP: public LinearProgram
{
public:
	DualLP(const GRBEnv &env, const Eigen::ArrayXXd &valuations, const Eigen::VectorXd &demand, const Eigen::VectorXd &supply, double penalty = 0.001);
	virtual ~DualLP();
	Eigen::VectorXd get_p();
	Eigen::VectorXd get_pi();
	void set_oversupply(const Eigen::VectorXd &oversupply);
	double get_objective_val();
	double get_penalty() { return penalty; }
private:
	GRBVar *p;
	GRBVar *pi;
	Eigen::VectorXd p_val;
	Eigen::VectorXd pi_val;
	GRBLinExpr base_expr;
	GRBLinExpr oversupply_expr;
	double objective_val;
	double penalty;
	void optimization_done();
	void build_model();
};

