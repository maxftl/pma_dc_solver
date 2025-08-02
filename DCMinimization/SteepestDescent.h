#pragma once
#include "Auction.h"
#include "DualLP.h"
#include <vector>
class SteepestDescent
{
public:
	SteepestDescent(const GRBEnv &env,const Auction &auction);
	~SteepestDescent();
	Eigen::VectorXd run();
private:
	const Auction &auction;
	DualLP pos_lp, neg_lp;
};

