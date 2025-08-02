#pragma once
#include "LinearProgram.h"
#include <Eigen/Dense>
#include <gurobi_c++.h>
#include <vector>


class SubgradientLP: public LinearProgram
{
public:
	enum PenaltyType { minimize_supply, minimize_supply_dist };
	void set_prices(const Eigen::VectorXd &prices);
	void recalculate_objective();
	void recalculate_objective(const Eigen::VectorXd & penalty_signature);
	void set_supply_goal(const Eigen::VectorXd &supply_goal);
	SubgradientLP(const GRBEnv & env, const Eigen::ArrayXXd & valuations, const Eigen::VectorXd & demand, const Eigen::VectorXd & supply, PenaltyType t=PenaltyType::minimize_supply);
	~SubgradientLP();
	Eigen::VectorXd get_s();
	
private:
	void build_model();
	void optimization_done();
	GRBVar *s;
	GRBVar *x;
	GRBVar *supply_dist;
	Eigen::VectorXd s_val;
	double objective_value;
	GRBLinExpr base_expr;
	GRBLinExpr price_expr;
	GRBLinExpr penalty_expr;
	Eigen::VectorXd prices;
	Eigen::VectorXd supply_goal;
	double penalty;
	bool prices_changed;
	double *std_penalty_signature;
	PenaltyType penalty_type;
	std::vector<GRBConstr> dist_contraints;
	Eigen::VectorXd standard_penalty_signature;
};

