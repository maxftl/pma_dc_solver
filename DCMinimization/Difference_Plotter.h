#pragma once

#include <vector>
#include <map>
#include "Auction.h"
#include "FlowPrimalLP.h"
#include <gurobi_c++.h>

class Difference_Plotter
{
public:
	using graph_t = std::map<std::pair<int, int>, int>;

	Difference_Plotter();
	void plot_valuations(const Auction &auction, graph_t &pos_val, graph_t &neg_val);
	~Difference_Plotter();

private:
	void plot_valuation(const Eigen::ArrayXXd &valuations, const Eigen::VectorXd &demand, graph_t &val, int max_demand);
	GRBEnv env;
};

