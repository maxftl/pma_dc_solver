#include "LinearProgram.h"
#include <iostream>



LinearProgram::LinearProgram(const GRBEnv & penv, const Eigen::ArrayXXd & pvaluations, const Eigen::VectorXd & pdemand, const Eigen::VectorXd & psupply): env(penv),
valuations(pvaluations),
demand(pdemand),
supply(psupply),
model(penv),
is_optimized(false),
num_goods(supply.size()),
num_bids(demand.size())
{
}

LinearProgram::~LinearProgram()
{
}

void LinearProgram::optimize()
{
	model.optimize();
	is_optimized = true;
	optimization_done();
}

void LinearProgram::optimization_done()
{
}
