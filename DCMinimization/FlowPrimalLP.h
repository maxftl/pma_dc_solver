#pragma once
#include "LemonDefs.h"
#include <Eigen/Dense>
#include <vector>
#include <memory>

class FlowPrimalLP
{
public:
	FlowPrimalLP(const Eigen::ArrayXXd &valuations, const Eigen::VectorXd &demand, const Eigen::VectorXd &supply);
	virtual ~FlowPrimalLP();
	Eigen::VectorXd get_p();
	void set_oversupply(const Eigen::VectorXd &oversupply);
	double get_objective_val();
	void optimize();
private:
	graph_t graph;
	std::unique_ptr<algorithm_t> flow_alg;
	std::vector<std::vector<arc_t>> bid_to_good; //bid_to_good[i][j] = arc from bid i to good j; cost = v_ij; cap = demand_i
	std::vector<arc_t> bid_to_target; //bid_to_target[i] = arc from bid i to target; cost = 0; cap = demand_i
	std::vector<arc_t> good_to_target; //good_to_target[j] = arc from good j to target; cost = 0; cap = supply_j
	std::vector<node_t> good_nodes; //supply of good[j]: supply[j]-sum_i demand_i
	std::vector<node_t> bid_nodes; //supply of bid[i]: num_goods*demand_i
	node_t target_node; //supply = -sum supply_j

	std::unique_ptr<arcmap_t> cost_map;
	std::unique_ptr<nodemap_t> supply_map;
	std::unique_ptr<arcmap_t> capacity_map;

	Eigen::VectorXd p_val;

	const Eigen::ArrayXXd &valuations;
	const Eigen::VectorXd &supply;
	const Eigen::VectorXd &demand;

	const size_t num_bids;
	const size_t num_goods;

	int total_demand;

	//void optimization_done();
	void build_graph();
	void build_cost_and_capacity();
	void build_supply();

};

