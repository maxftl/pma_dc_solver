#pragma once
#include "mcmf.h"
#include <Eigen/Dense>
#include <vector>
#include <memory>

class SC2Flow
{
public:
	SC2Flow(const Eigen::ArrayXXd &valuations, const Eigen::VectorXd &demand, const Eigen::VectorXd &supply, double penalty = 0.001);
	virtual ~SC2Flow();
	Eigen::VectorXd get_p() {
		return prices;
	};
	void set_oversupply(const Eigen::VectorXd &oversupply);
	double get_objective_val();
	double get_penalty() { return penalty; }
	void optimize();
	//void print_duals();

private:
	double penalty;
	const Eigen::ArrayXXd &valuations;
	const Eigen::VectorXd &demand;
	const Eigen::VectorXd &supply;
	int num_goods;
	int num_bids;
	int num_nodes;
	int num_arcs;
	MCMF_CS2 cs_alg;
	//node ids
	int good_start, bid_start;
	inline int source_nd() { return 1; }
	inline int target_nd() { return num_nodes; }
	inline int good_nd(int j) { return good_start + j; } //j from 0 to n_goods-1
	inline int bid_nd(int i) { return bid_start + i; }

	double objective_val;
	Eigen::VectorXd prices;
};

