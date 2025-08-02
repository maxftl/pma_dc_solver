#pragma once
#include <Eigen/Dense>
#include <vector>

class SupplyFastSolver
{
public:
	SupplyFastSolver(const Eigen::ArrayXXd &valuations, const Eigen::VectorXd &demand);

	void set_permutation(const std::vector<int> &perm);

	void set_prices(const Eigen::VectorXd &prices);

	void optimize();

	Eigen::VectorXd get_s();
	~SupplyFastSolver();
private:
	std::vector<int> permutation;

	Eigen::VectorXd prices;

	const Eigen::ArrayXXd &valuations;
	const Eigen::VectorXd &demand;

	Eigen::VectorXd s;
};

