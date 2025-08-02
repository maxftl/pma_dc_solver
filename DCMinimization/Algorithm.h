#pragma once
#include "DualLP.h"
#include "SubgradientLP.h"
#include <Eigen/Dense>
#include <gurobi_c++.h>
#include "Result.h"
#include "Auction.h"
#include <Lyaponov.h>
#include "AlgorithmEventHandler.h"
#include "NearestPoint.h"
#include <SubmodularOracle.h>


class Algorithm
{
public:
	enum DescentStrategy {Random, Exhaustive, SubmodularMin, None};
	Algorithm(const Auction &auction);
	~Algorithm();
	//Result run(DescentStrategy strat = None);
	Result run_price_based();
	void set_verbose(bool verbose);
	//Eigen::VectorXd find_exhaustive_descent_supply(const Eigen::VectorXd &prices, const Eigen::VectorXd &current_oversupply, SubgradientLP & supply_calculator);
	//Eigen::VectorXd find_random_descent_supply(const Eigen::VectorXd &prices, const Eigen::VectorXd &current_oversupply, SubgradientLP & supply_calculator, int max_tries = 200);
	Eigen::VectorXd find_submodular_descent_supply(const Eigen::VectorXd &prices, const Eigen::VectorXd &oversupply, SubgradientLP &supply_calculator);
	Eigen::VectorXd find_submodular_descent_prices(const Eigen::VectorXd &prices);
	void add_event_handler(AlgorithmEventHandler *h);
private:
	Auction auction;
	Lyaponov lyap;
	bool verbose;
	std::vector<AlgorithmEventHandler*> event_handlers;
	NearestPoint np;
	SubmodularOracle<Lyaponov> oracle;
	GRBEnv env;
};

