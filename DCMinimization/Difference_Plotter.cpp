#include "Difference_Plotter.h"
#include <iostream>
#include "DualLP.h"


Difference_Plotter::Difference_Plotter()
{
}

void Difference_Plotter::plot_valuations(const Auction & auction, graph_t & pos_val, graph_t & neg_val)
{
	plot_valuation(auction.pos_valuation, auction.pos_demand, pos_val, auction.pos_demand.sum());
	plot_valuation(auction.neg_valuation, auction.neg_demand, neg_val, auction.pos_demand.sum());
}


Difference_Plotter::~Difference_Plotter()
{
}

void Difference_Plotter::plot_valuation(const Eigen::ArrayXXd & valuations, const Eigen::VectorXd &demand, graph_t &val, int max_demand)
{
	//FlowPrimalLP fpl(valuations,demand,Eigen::VectorXd::Zero(2),0);
	DualLP dlp(env, valuations, demand, Eigen::VectorXd::Zero(2), 0);
	for (int s1 = 0;s1 < max_demand; ++s1) {
		std::cout << s1 << std::endl;
		for (int s2 = 0; s2 < max_demand; ++s2) {
			Eigen::VectorXd oversupply(2);
			oversupply(0) = s1;
			oversupply(1) = s2;
			dlp.set_oversupply(oversupply);
			dlp.optimize();
			int v = dlp.get_objective_val();
			val[std::pair<int, int>(s1, s2)] = v;
		}
	}
}
