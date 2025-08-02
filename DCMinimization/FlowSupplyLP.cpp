#include "FlowSupplyLP.h"






FlowSupplyLP::FlowSupplyLP(const Eigen::ArrayXXd & valuations, const Eigen::VectorXd & demand):
	valuations(valuations), demand(demand), num_bids(valuations.rows()), num_goods(valuations.cols())
{
	build_graph();
	init_demand();
	init_costs();

	algo = std::unique_ptr<algorithm_t>(new algorithm_t(graph));
	algo->supplyMap(*demand_map);
}

FlowSupplyLP::~FlowSupplyLP()
{
}

void FlowSupplyLP::set_prices(const Eigen::VectorXd & prices)
{
	for (unsigned i = 0; i < num_bids; ++i) {
		for (unsigned j = 0; j < num_goods; ++j) {
			cost_map->set(x_to_bid[i][j], -valuations(i, j) + prices(j));
		}
	}
	algo->costMap(*cost_map);
}

void FlowSupplyLP::optimize()
{
	algo->run();
}

Eigen::VectorXd FlowSupplyLP::get_s()
{
	Eigen::VectorXd result(num_goods);
	for (unsigned i = 0; i < num_bids; ++i) {
		for (unsigned j = 0; j < num_goods; ++j) {
			result(j) += algo->flow(x_to_bid[i][j]);
		}
	}
	return result;
}

void FlowSupplyLP::build_graph()
{
	//Make space
	x_nodes.resize(num_bids, std::vector<node_t>(num_goods));
	bid_nodes.resize(num_bids);
	x_to_bid.resize(num_bids, std::vector<arc_t>(num_goods));
	//Make graph
	for (unsigned i = 0; i < num_bids; ++i) {
		bid_nodes[i] = graph.addNode();
		for (unsigned j = 0; j < num_goods; ++j) {
			x_nodes[i][j] = graph.addNode();
			x_to_bid[i][j] = graph.addArc(x_nodes[i][j], bid_nodes[i]);
		}
	}
}

void FlowSupplyLP::init_demand()
{
	demand_map = std::unique_ptr<nodemap_t>(new nodemap_t(graph));
	for (unsigned i = 0; i < num_bids; ++i) {
		demand_map->set(bid_nodes[i], -demand(i));
	}
}

void FlowSupplyLP::init_costs()
{
	cost_map = std::unique_ptr<arcmap_t>(new arcmap_t(graph));
}
