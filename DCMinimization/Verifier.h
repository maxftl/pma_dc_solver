#pragma once

#include "Auction.h"
#include <iostream>

class Verifier {
public:
	static double calculate_objective_value(const Auction &auction, const Eigen::VectorXd &prices) {
		//std::cout << auction.pos_valuation << std::endl << std::endl;
		//std::cout << prices << std::endl;
		return (auction.pos_demand.array() * (auction.pos_valuation.rowwise() - prices.transpose().array()).rowwise().maxCoeff().max(0)).sum() -
			(auction.neg_demand.array() * (auction.neg_valuation.rowwise() - prices.transpose().array()).rowwise().maxCoeff().max(0)).sum() +
			prices.transpose()*auction.supply;
	}
};