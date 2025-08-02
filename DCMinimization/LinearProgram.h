#pragma once
#include <Eigen/Dense>
#include <gurobi_c++.h>


class LinearProgram
{
public:
	LinearProgram(const GRBEnv &penv, const Eigen::ArrayXXd &pvaluations, const Eigen::VectorXd &pdemand, const Eigen::VectorXd &psupply);
	virtual ~LinearProgram();
	void optimize();
protected:
	Eigen::ArrayXXd valuations;
	Eigen::VectorXd demand;
	Eigen::VectorXd supply;
	const GRBEnv &env;
	GRBModel model;
	int num_bids;
	int num_goods;
	bool is_optimized;
	virtual void build_model() = 0;
	virtual void optimization_done();
};

