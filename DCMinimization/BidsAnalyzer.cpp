#include "BidsAnalyzer.h"



BidsAnalyzer::BidsAnalyzer()
{
}


BidsAnalyzer::~BidsAnalyzer()
{
}

int BidsAnalyzer::num_marginal_bids(const Eigen::ArrayXXd & valuations, const Eigen::VectorXd & prices)
{
	Eigen::ArrayXXd t_prices = prices.transpose();
	int num_goods = prices.size();
	int num_bids = valuations.rows();
	int num_marginal_bids = 0;
	for (int i = 0; i < num_bids; ++i) {
		Eigen::ArrayXXd diff = valuations.row(i) - t_prices;
		double utility = diff.maxCoeff();
		if (utility < 0) continue;
		int num_margins = 0;
		for (int j = 0; j < num_goods; ++j) {
			if (std::abs(utility - diff(0,j)) < 0.1) ++num_margins;
		}
		if (num_margins >= 2) ++num_marginal_bids;
	}
	return num_marginal_bids;
}
