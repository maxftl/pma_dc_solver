#pragma once
#include "LemonDefs.h"
#include <vector>
#include <Eigen/Dense>

class FlowSupplyLP
{
public:
	FlowSupplyLP(const Eigen::ArrayXXd &valuations, const Eigen::VectorXd &demand);
	~FlowSupplyLP();

	void set_prices(const Eigen::VectorXd &prices);

	void optimize();

	Eigen::VectorXd get_s();
private:
	graph_t graph;
	std::vector<std::vector<node_t>> x_nodes;
	std::vector<node_t> bid_nodes;
	std::vector<std::vector<arc_t>> x_to_bid;

	std::unique_ptr<nodemap_t> demand_map;
	std::unique_ptr<arcmap_t> cost_map;

	std::unique_ptr<algorithm_t> algo;

	const Eigen::ArrayXXd &valuations;
	const Eigen::VectorXd &demand;


	int num_bids;
	int num_goods;

	void build_graph();
	void init_demand();
	void init_costs();
};

