#pragma once
#include <Eigen/Dense>
#include <cstdlib>

class BidsAnalyzer
{
public:
	BidsAnalyzer();
	~BidsAnalyzer();
	int num_marginal_bids(const Eigen::ArrayXXd & valuations, const Eigen::VectorXd & prices);
};

