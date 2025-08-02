#include "Algorithm.h"
#include "DualLP.h"
#include "SubgradientLP.h"
#include "Result.h"
#include <iostream>
#include "RandomSignature.h"
#include <limits>
#include "SignatureIterator.h"
#include "BidsAnalyzer.h"
#include "DemandVectorIterator.h"
#include <SubmodularOracle.h>
#include <NearestPoint.h>
#include <functional>
#include "OperationCounter.h"
#include "FlowPrimalLP.h"
#include "SupplyFastSolver.h"
#include "SC2Flow.h"

using namespace std;
using namespace Eigen;

Algorithm::Algorithm(const Auction &auction):
env("Env"),
auction(auction),
verbose(true),
lyap(auction,.0001),
oracle(lyap)
{
	env.set(GRB_IntParam_OutputFlag, 0);
}

Algorithm::~Algorithm()
{
}

void Algorithm::set_verbose(bool verbose) {
	this->verbose = verbose;
}

/*Eigen::VectorXd Algorithm::find_exhaustive_descent_supply(const Eigen::VectorXd & prices, const Eigen::VectorXd & current_oversupply, SubgradientLP & supply_calculator)
{
	if (verbose) {
		BidsAnalyzer analyzer;
		std::cout << "Number marginal bids: " << analyzer.num_marginal_bids(auction.neg_valuation, prices) << endl;
	}
	//Calculate current difference
	DualLP positive_prices_calculator(env, auction.pos_valuation, auction.pos_demand, auction.supply, 0.0);
	DualLP negative_prices_calculator(env, auction.neg_valuation, auction.neg_demand, VectorXd::Zero(auction.num_goods), 0.0);
	positive_prices_calculator.set_oversupply(current_oversupply);
	negative_prices_calculator.set_oversupply(current_oversupply);
	positive_prices_calculator.optimize();
	negative_prices_calculator.optimize();
	double best_diff = positive_prices_calculator.get_objective_val() - negative_prices_calculator.get_objective_val();

	VectorXd result = numeric_limits<double>::infinity() * VectorXd::Ones(auction.num_goods);
	//SignatureIterator s_it(auction.num_goods);
	DemandVectorIterator dvi(auction.num_goods);
	int num_iterations = 0;
	while (!dvi.done()) {
		++num_iterations;
		supply_calculator.recalculate_objective(dvi.get());
		supply_calculator.optimize();
		VectorXd curr_supply = supply_calculator.get_s();

		positive_prices_calculator.set_oversupply(curr_supply);
		negative_prices_calculator.set_oversupply(curr_supply);
		positive_prices_calculator.optimize();
		negative_prices_calculator.optimize();

		double curr_diff = positive_prices_calculator.get_objective_val() - negative_prices_calculator.get_objective_val();
		if (curr_diff < best_diff) {
			result = curr_supply;
			break;
		}
		dvi.next();
	}
	if(verbose) cout << "Needed " << num_iterations << " iterations to find a descent direction." << endl;
	return result;
}*/

/*Eigen::VectorXd Algorithm::find_random_descent_supply(const VectorXd & prices, const VectorXd & current_oversupply, SubgradientLP & supply_calculator, int max_tries)
{
	//Calculate current difference
	DualLP positive_prices_calculator(env, auction.pos_valuation, auction.pos_demand, auction.supply, 0.0);
	DualLP negative_prices_calculator(env, auction.neg_valuation, auction.neg_demand, VectorXd::Zero(auction.num_goods), 0.0);
	positive_prices_calculator.set_oversupply(current_oversupply);
	negative_prices_calculator.set_oversupply(current_oversupply);
	positive_prices_calculator.optimize();
	negative_prices_calculator.optimize();
	double best_diff = positive_prices_calculator.get_objective_val() - negative_prices_calculator.get_objective_val();

	VectorXd result = numeric_limits<double>::infinity() * VectorXd::Ones(auction.num_goods);
	RandomSignature rs(auction.num_goods);
	bool success = false;
	for (int i = 0; i < max_tries && !success; ++i) {
		vector<double> signature = rs.generate();
		//supply_calculator.recalculate_objective(signature);
		supply_calculator.optimize();
		VectorXd curr_supply = supply_calculator.get_s();

		positive_prices_calculator.set_oversupply(curr_supply);
		negative_prices_calculator.set_oversupply(curr_supply);
		positive_prices_calculator.optimize();
		negative_prices_calculator.optimize();
		double current_diff = positive_prices_calculator.get_objective_val() - negative_prices_calculator.get_objective_val();
		if (current_diff < best_diff) {
			success = true;
			result = curr_supply;
		}
	}
	return result;
}*/

Eigen::VectorXd Algorithm::find_submodular_descent_supply(const Eigen::VectorXd & prices, const Eigen::VectorXd & oversupply, SubgradientLP & supply_calculator)
{
	Eigen::VectorXd penalty_signature(Eigen::VectorXd::Zero(auction.num_goods));
	double sign = 1;
	lyap.set_base_price(prices);
	lyap.set_sign(1);
	auto start_vertex = oracle(Eigen::VectorXd::Zero(auction.num_goods));
	for_each(event_handlers.begin(), event_handlers.end(), [](AlgorithmEventHandler *h) {h->submodular_minimization_started(); });
	auto nearest_point = np.get_nearest_point(start_vertex, &oracle, false);
	if (verbose) cout << "Positive nearest point: " << nearest_point.transpose() << endl;
	if (nearest_point.minCoeff() >= -1e-6) {
		sign = -1;
		lyap.set_sign(-1);
		Eigen::VectorXd mask = prices.array().min(1.0);
		oracle.set_mask(mask);
		auto start_vertex = oracle(Eigen::VectorXd::Zero(auction.num_goods));
		for_each(event_handlers.begin(), event_handlers.end(), [](AlgorithmEventHandler *h) {h->submodular_minimization_started(); });
		nearest_point = np.get_nearest_point(start_vertex, &oracle, false);
		oracle.remove_mask();
		if (verbose) cout << "Negative nearest point: " << nearest_point.transpose() << endl;
	}
	if(verbose) cout << "Nearest point: " << nearest_point.transpose() << endl;
	bool done = true;
	for (int i = 0; i < auction.num_goods; ++i) {
		if (nearest_point(i) <= -1e-6 && prices(i) + sign >= 0) {
			penalty_signature(i) = sign;
			done = false;
		}
	}
	if(verbose) cout << "Old value-New value= " << lyap(Eigen::VectorXd::Zero(auction.num_goods)) - lyap(-penalty_signature) << endl;
	for_each(event_handlers.begin(), event_handlers.end(), [&penalty_signature](AlgorithmEventHandler *h) {h->descent_direction_found(penalty_signature); });
	if (done) return numeric_limits<double>::infinity() * Eigen::VectorXd::Ones(auction.num_goods);
	supply_calculator.recalculate_objective(-penalty_signature);
	supply_calculator.optimize();
	for_each(event_handlers.begin(), event_handlers.end(), [&penalty_signature](AlgorithmEventHandler *h) {h->supply_lp_invoked(); });
	return supply_calculator.get_s();
}

Eigen::VectorXd Algorithm::find_submodular_descent_prices(const Eigen::VectorXd & prices)
{
	Eigen::VectorXd penalty_signature(Eigen::VectorXd::Zero(auction.num_goods));
	double sign = 1;
	lyap.set_base_price(prices);
	lyap.set_sign(1);
	auto start_vertex = oracle(Eigen::VectorXd::Zero(auction.num_goods));
	for_each(event_handlers.begin(), event_handlers.end(), [](AlgorithmEventHandler *h) {h->submodular_minimization_started(); });
	auto nearest_point = np.get_nearest_point(start_vertex, &oracle, false);
	if (verbose) cout << "Positive nearest point: " << nearest_point.transpose() << endl;
	if (nearest_point.minCoeff() >= -1e-6) {
		sign = -1;
		lyap.set_sign(-1);
		Eigen::VectorXd mask = prices.array().min(1.0);
		oracle.set_mask(mask);
		auto start_vertex = oracle(Eigen::VectorXd::Zero(auction.num_goods));
		for_each(event_handlers.begin(), event_handlers.end(), [](AlgorithmEventHandler *h) {h->submodular_minimization_started(); });
		nearest_point = np.get_nearest_point(start_vertex, &oracle, false);
		oracle.remove_mask();
		if (verbose) cout << "Negative nearest point: " << nearest_point.transpose() << endl;
	}
	//if (verbose) cout << "Nearest point: " << nearest_point.transpose() << endl;
	bool done = true;
	for (int i = 0; i < auction.num_goods; ++i) {
		if (nearest_point(i) <= -1e-6 && prices(i) + sign >= 0) {
			penalty_signature(i) = sign;
			done = false;
		}
	}
	int lb = 1;
	int ub = 1;
	if (!done) {
		double curr_val = lyap(Eigen::VectorXd::Zero(auction.num_goods));
		double diff = lyap(penalty_signature) - curr_val;
		while (abs(lyap(ub*penalty_signature) - curr_val - ub * diff) < 1e-1) {
			ub *= 2;
		}
		lb = ub / 2;
		while (ub - lb >= 2) {
			int c = lb + (ub - lb) / 2;
			if (abs(lyap(c*penalty_signature) - curr_val - c * diff) < 1e-1) {
				lb = c;
			}
			else {
				ub = c;
			}
		}
	}
	return prices + lb*penalty_signature;
}

void Algorithm::add_event_handler(AlgorithmEventHandler * h)
{
	event_handlers.push_back(h);
}

/*Result Algorithm::run(Algorithm::DescentStrategy strat)
{
	DualLP price_calculator(env, auction.pos_valuation, auction.pos_demand, auction.supply);
	SubgradientLP supply_calculator(env, auction.neg_valuation, auction.neg_demand, Eigen::VectorXd::Zero(auction.num_goods), SubgradientLP::PenaltyType::minimize_supply);
	VectorXd oversupply(VectorXd::Zero(auction.num_goods));
	VectorXd prices(VectorXd::Zero(auction.num_goods));
	
	bool done = false;
	while (!done) {
		for_each(event_handlers.begin(), event_handlers.end(), [](AlgorithmEventHandler *h) {h->iteration_started();});
		price_calculator.set_oversupply(oversupply);
		price_calculator.optimize();
		prices = price_calculator.get_p();
		for_each(event_handlers.begin(), event_handlers.end(), [&prices](AlgorithmEventHandler *h) {h->price_updated(prices); h->price_lp_invoked(); });
		supply_calculator.set_prices(prices);
		supply_calculator.recalculate_objective();
		supply_calculator.optimize();
		VectorXd oversupply_new = supply_calculator.get_s();
		for_each(event_handlers.begin(), event_handlers.end(), [&oversupply](AlgorithmEventHandler *h) { h->supply_lp_invoked(); });
		if (oversupply_new == oversupply)
		{
			if (strat == DescentStrategy::None) done = true;
			else {
				for_each(event_handlers.begin(), event_handlers.end(), [](AlgorithmEventHandler *h) {h->descent_search_started(); });
				VectorXd descent_supply = numeric_limits<double>::infinity() * VectorXd::Ones(auction.num_goods);
				if(strat == DescentStrategy::Random) descent_supply = find_random_descent_supply(prices, oversupply, supply_calculator);
				if (strat == DescentStrategy::Exhaustive) descent_supply = find_exhaustive_descent_supply(prices, oversupply, supply_calculator);
				if (strat == DescentStrategy::SubmodularMin) descent_supply = find_submodular_descent_supply(prices, oversupply, supply_calculator);
				
				if (descent_supply[0] == numeric_limits<double>::infinity()) done = true;
				else {
					oversupply = descent_supply;
					for_each(event_handlers.begin(), event_handlers.end(), [&oversupply](AlgorithmEventHandler *h) { h->oversupply_updated(oversupply); });
				}
			}
		}
		else {
			oversupply = oversupply_new;
			for_each(event_handlers.begin(), event_handlers.end(), [&oversupply](AlgorithmEventHandler *h) { h->oversupply_updated(oversupply); });
		}
		for_each(event_handlers.begin(), event_handlers.end(), [](AlgorithmEventHandler *h) {h->iteration_ended(); });
	}
	Result result;
	result.oversupply = oversupply;
	result.prices = prices;
	//Calculate objective value
	//DualLP neg_price_calculator(env, auction.neg_valuation, auction.neg_demand, oversupply, 0.0);
	//neg_price_calculator.optimize();
	//result.objective_value = price_calculator.get_objective_val() - neg_price_calculator.get_objective_val() - price_calculator.get_penalty()*prices.lpNorm<1>();
	for_each(event_handlers.begin(), event_handlers.end(), [&result](AlgorithmEventHandler *h) {h->algorithm_terminated(result); });
	return result;
}*/

Result Algorithm::run_price_based()
{
	//FlowPrimalLP price_calculator(auction.pos_valuation, auction.pos_demand, auction.supply);
	//SC2Flow price_calculator(auction.pos_valuation, auction.pos_demand, auction.supply);
	DualLP price_calculator(env,auction.pos_valuation,auction.pos_demand,auction.supply);
	SupplyFastSolver supply_calculator(auction.neg_valuation, auction.neg_demand);
	VectorXd oversupply(VectorXd::Zero(auction.num_goods));
	VectorXd prices(VectorXd::Zero(auction.num_goods));

	bool done = false;
	while (!done) {
		lyap.set_sign(1);
		lyap.set_base_price(Eigen::VectorXd::Zero(auction.num_goods));
		//cout << "Value of lyaponov function at prices " << prices.transpose() << ": " << lyap(prices) << endl;
		for_each(event_handlers.begin(), event_handlers.end(), [](AlgorithmEventHandler *h) {h->iteration_started(); });
		supply_calculator.set_prices(prices);
		supply_calculator.optimize();
		oversupply = supply_calculator.get_s();
		for_each(event_handlers.begin(), event_handlers.end(), [&oversupply](AlgorithmEventHandler *h) { h->supply_lp_invoked(); h->oversupply_updated(oversupply); });
		price_calculator.set_oversupply(oversupply);
		price_calculator.optimize();
		VectorXd prices_new = price_calculator.get_p();
		for_each(event_handlers.begin(), event_handlers.end(), [&prices](AlgorithmEventHandler *h) { h->price_lp_invoked(); });
		if (prices_new == prices) {
			for_each(event_handlers.begin(), event_handlers.end(), [](AlgorithmEventHandler *h) {h->descent_search_started(); });
			prices_new = find_submodular_descent_prices(prices);
			if (prices_new == prices) done = true;
		}
		prices = prices_new;
		for_each(event_handlers.begin(), event_handlers.end(), [&prices](AlgorithmEventHandler *h) { h->price_updated(prices); });
		lyap.set_sign(1);
		lyap.set_base_price(Eigen::VectorXd::Zero(auction.num_goods));
		if (verbose) cout << "Lyaponov value: " << lyap(prices) << endl;
		for_each(event_handlers.begin(), event_handlers.end(), [](AlgorithmEventHandler *h) {h->iteration_ended(); });
	}

	Result result;
	result.oversupply = oversupply;
	result.prices = prices;
	return result;
}
