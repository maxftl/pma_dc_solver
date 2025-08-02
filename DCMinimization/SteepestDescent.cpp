#include "SteepestDescent.h"



SteepestDescent::SteepestDescent(const GRBEnv &env, const Auction & auction):
	auction(auction), pos_lp(env,auction.pos_valuation,auction.pos_demand,auction.supply,0.0001),
	neg_lp(env,auction.neg_valuation,auction.neg_demand,Eigen::VectorXd::Zero(auction.num_goods),0.0001)
{
}

SteepestDescent::~SteepestDescent()
{
}

Eigen::VectorXd SteepestDescent::run()
{
	Eigen::VectorXd oversupply = Eigen::VectorXd::Zero(auction.num_goods);
	Eigen::VectorXd prices = Eigen::VectorXd::Zero(auction.num_goods);
	std::vector<Eigen::VectorXd> elementary_descent_steps(auction.num_goods + 1, Eigen::VectorXd::Zero(auction.num_goods)); //Contains the zero vector (index 0) and all standard vectors
	for (int i = 1; i < auction.num_goods + 1; ++i) {
		elementary_descent_steps[i](i-1) = 1;
	}
	pos_lp.optimize(); neg_lp.optimize();
	double current_value = pos_lp.get_objective_val() - neg_lp.get_objective_val();
	prices = pos_lp.get_p();
	while (true) {
		Eigen::VectorXd steepest_descent_dir = Eigen::VectorXd::Zero(auction.num_goods);
		bool change = false;
		for (int i = 0; i < auction.num_goods + 1; ++i) {
			for (int j = 0; j < auction.num_goods + 1; ++j) {
				if (i == j) continue;
				Eigen::VectorXd delta = elementary_descent_steps[i] - elementary_descent_steps[j];
				std::cout << "Checking " << delta.transpose() << "..." << std::endl;
				pos_lp.set_oversupply((oversupply + delta).cwiseMax(0));
				neg_lp.set_oversupply((oversupply + delta).cwiseMax(0));
				pos_lp.optimize(); neg_lp.optimize();
				double new_value = pos_lp.get_objective_val() - neg_lp.get_objective_val();
				if (new_value < current_value) {
					prices = pos_lp.get_p();
					current_value = new_value;
					steepest_descent_dir = delta;
					change = true;
				}
			}
		}
		std::cout << "Current value: " << current_value << std::endl;
		if (change) {
			oversupply = (oversupply + steepest_descent_dir).cwiseMax(0);
		}
		else break;
	}
	std::cout << "Oversupply: " << oversupply.transpose() << std::endl;
	return prices;
}
