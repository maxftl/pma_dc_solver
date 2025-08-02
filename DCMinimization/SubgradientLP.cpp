#include "SubgradientLP.h"
#include <vector>
#include <cmath>
#include <iostream>


void SubgradientLP::set_prices(const Eigen::VectorXd & prices)
{
	prices_changed = true;
	this->prices = prices;
	price_expr = 0;
	for (int j = 0; j < num_goods; ++j) price_expr += -prices[j] * s[j];
}

void SubgradientLP::recalculate_objective()
{
	recalculate_objective(standard_penalty_signature);
}

void SubgradientLP::recalculate_objective(const Eigen::VectorXd & penalty_signature)
{
	penalty_expr = 0;
	/*if (penalty_type == SubgradientLP::PenaltyType::minimize_supply_dist && supply_goal.size() == num_goods)
	{
		for (int j = 0; j < num_goods; ++j) {
			dist_contraints[2 * j].set(GRB_DoubleAttr_RHS, supply_goal[j]);
			dist_contraints[2 * j + 1].set(GRB_DoubleAttr_RHS, -supply_goal[j]);
		}
		penalty_expr = -supply_dist[0];
	}
	else*/
		for (int j = 0; j < num_goods; ++j) penalty_expr += penalty_signature[j]*s[j];
	//std::cout << prices << std::endl;
	model.setObjective(base_expr + price_expr /*+ penalty * penalty_expr*/, GRB_MAXIMIZE);
}

void SubgradientLP::set_supply_goal(const Eigen::VectorXd & supply_goal)
{
	this->supply_goal = supply_goal;
}

SubgradientLP::SubgradientLP(const GRBEnv & env, const Eigen::ArrayXXd & valuations, const Eigen::VectorXd & demand, const Eigen::VectorXd & supply, SubgradientLP::PenaltyType t) : LinearProgram(env, valuations, demand, supply),
prices(num_goods),
penalty(0),
prices_changed(false),
s_val(supply),
penalty_type(t),
standard_penalty_signature(Eigen::VectorXd::Zero(num_goods))
{
	for (int j = 0; j < num_goods; ++j) prices[j] = 1;
	for (int j = 0; j < num_goods; ++j) standard_penalty_signature[j] = 1.0;
	build_model();
}

Eigen::VectorXd SubgradientLP::get_s()
{
	return s_val;
}

SubgradientLP::~SubgradientLP()
{
	delete(s);
	delete(x);
}


void SubgradientLP::build_model()
{
	s = model.addVars(num_goods);
	for (int j = 0; j < num_goods; ++j) model.addConstr(s[j] <= 200000);
	x = model.addVars(num_bids*num_goods);
	//Add demand constraints
	for (int i = 0; i < num_bids; ++i) {
		GRBLinExpr expr;
		for (int j = 0; j < num_goods; ++j) expr += x[i + j * num_bids];
		model.addConstr(expr, GRB_LESS_EQUAL, demand[i] + .0001);
	}
	//Add supply constraints
	for (int j = 0; j < num_goods; ++j) {
		GRBLinExpr expr;
		for (int i = 0; i < num_bids; ++i) expr += x[i + j * num_bids];
		model.addConstr(expr, GRB_LESS_EQUAL, s[j] + supply[j]);
	}
	//Introduce additional distance variable if necessary
	/*if (penalty_type == SubgradientLP::PenaltyType::minimize_supply_dist) {
		supply_dist = model.addVars(1);
		for (int j = 0; j < num_goods; ++j) {
			GRBLinExpr expr = supply_dist[0] + s[j];
			dist_contraints.push_back(model.addConstr(expr, GRB_GREATER_EQUAL, 0));
			expr = supply_dist[0] - s[j];
			dist_contraints.push_back(model.addConstr(expr, GRB_GREATER_EQUAL, 0));
		}
	}*/
	//Define unpenalized objective
	for (int i = 0; i < num_bids; ++i) {
		for (int j = 0; j < num_goods; ++j) base_expr += valuations(i, j)*x[i + j * num_bids];
	}
}

void SubgradientLP::optimization_done()
{
	prices_changed = false;
	for (int j = 0; j < num_goods; ++j) s_val[j] = round(s[j].get(GRB_DoubleAttr_X));
}
