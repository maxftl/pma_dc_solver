#pragma once
#include "AlgorithmEventHandler.h"
class OperationCounter :
	public AlgorithmEventHandler
{
public:
	OperationCounter() : num_supply_lps(0), num_price_lps(0), num_sm_minimizations(0), num_iterations(0), num_descent_searches(0) {}

	void supply_lp_invoked() { ++num_supply_lps; }
	void price_lp_invoked() { ++num_price_lps; }

	void iteration_started() { ++num_iterations; }

	void submodular_minimization_started() { ++num_sm_minimizations; }

	void descent_search_started() { ++num_descent_searches; }

	int get_num_lps() const { return num_supply_lps + num_price_lps; }
	int get_num_supply_lps() const { return num_supply_lps; }
	int get_num_price_lps() const { return num_price_lps; }
	int get_num_sm_minimizations() const { return num_sm_minimizations; }
	int get_num_iterations() const { return num_iterations; }
	int get_num_descent_searches() const { return num_descent_searches; }

private:
	int num_supply_lps;
	int num_price_lps;
	int num_sm_minimizations;
	int num_iterations;
	int num_descent_searches;
};

