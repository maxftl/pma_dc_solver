#include "SupplyFastSolver.h"






SupplyFastSolver::SupplyFastSolver(const Eigen::ArrayXXd & valuations, const Eigen::VectorXd &demand):
valuations(valuations), demand(demand),  permutation(valuations.cols()), s(Eigen::VectorXd::Zero(valuations.cols()))
{
	for (int i = 0; i < valuations.cols(); ++i) {
		permutation[i] = i;
	}
}

void SupplyFastSolver::set_permutation(const std::vector<int>& perm)
{
	permutation = perm;
}

void SupplyFastSolver::set_prices(const Eigen::VectorXd & prices)
{
	this->prices = prices;
}

void SupplyFastSolver::optimize()
{
	s = Eigen::VectorXd::Zero(valuations.cols());
	for (int i = 0; i < demand.size(); ++i) {
		int j = 0;
		int m = 0;
		for (int l : permutation) {
			if (valuations(i, l) - prices(l) > m) {
				m = valuations(i, l) - prices(l);
				j = l;
			}
		}
		if (m > 0) s(j) += demand(i);
	}
}

Eigen::VectorXd SupplyFastSolver::get_s()
{
	return s;
}

SupplyFastSolver::~SupplyFastSolver()
{
}
