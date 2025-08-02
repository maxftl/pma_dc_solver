#include "FlowPrimalLP.h"
#include <limits>



FlowPrimalLP::FlowPrimalLP(const Eigen::ArrayXXd &valuations, const Eigen::VectorXd &demand, const Eigen::VectorXd &supply):
	valuations(valuations), supply(supply), demand(demand), p_val(Eigen::VectorXd::Zero(supply.size())), num_bids(demand.size()), num_goods(supply.size())
{
	build_graph();
	cost_map = std::unique_ptr<arcmap_t>(new arcmap_t(graph));
	supply_map = std::unique_ptr<nodemap_t>(new nodemap_t(graph));
	capacity_map = std::unique_ptr<arcmap_t>(new arcmap_t(graph));
	build_cost_and_capacity();
	build_supply();
	flow_alg = std::unique_ptr<algorithm_t>(new algorithm_t(graph));
	flow_alg->costMap(*cost_map);
	flow_alg->upperMap(*capacity_map);
	flow_alg->supplyMap(*supply_map);
}


FlowPrimalLP::~FlowPrimalLP()
{
}

Eigen::VectorXd FlowPrimalLP::get_p()
{
	return p_val;
}

void FlowPrimalLP::set_oversupply(const Eigen::VectorXd & oversupply)
{
	for (int j = 0; j < oversupply.size(); ++j) {
		(*supply_map)[good_nodes[j]] = supply[j] + oversupply[j] - total_demand;
	}
	(*supply_map)[target_node] = -supply.sum() - oversupply.sum();
	flow_alg->supplyMap(*supply_map);
}

double FlowPrimalLP::get_objective_val()
{
	return -1;
}

void FlowPrimalLP::optimize()
{
	flow_alg->run();

	for (int j = 0; j < supply.size(); ++j) {
		p_val(j) = flow_alg->potential(good_nodes[j]) - flow_alg->potential(target_node);
	}
}

void FlowPrimalLP::build_graph()
{

	good_nodes.resize(num_goods);
	for (int j = 0; j < num_goods; ++j) {
		good_nodes[j] = graph.addNode();
	}

	bid_nodes.resize(num_bids);
	for (int i = 0; i < num_bids; ++i) {
		bid_nodes[i] = graph.addNode();
	}

	target_node = graph.addNode();

	bid_to_good.resize(num_bids, std::vector<arc_t>(num_goods));
	bid_to_target.resize(num_bids);
	good_to_target.resize(num_goods);
	for (int i = 0; i < num_bids; ++i) {
		for (int j = 0; j < num_goods; ++j) {
			bid_to_good[i][j] = graph.addArc(bid_nodes[i], good_nodes[j]);
		}
		bid_to_target[i] = graph.addArc(bid_nodes[i], target_node);
	}
	for (int j = 0; j < num_goods; ++j) {
		good_to_target[j] = graph.addArc(good_nodes[j], target_node);
	}
}

void FlowPrimalLP::build_cost_and_capacity()
{
	for (int i = 0; i < num_bids; ++i) {
		for (int j = 0; j < num_goods; ++j) {
			(*cost_map)[bid_to_good[i][j]] = valuations(i, j);
			(*capacity_map)[bid_to_good[i][j]] = demand(i);
		}
		(*cost_map)[bid_to_target[i]] = 0;
		(*capacity_map)[bid_to_target[i]] = demand(i);
	}

	for (int j = 0; j < num_goods; ++j) {
		(*cost_map)[good_to_target[j]] = 0;
		(*capacity_map)[good_to_target[j]] = supply[j];
	}

}

void FlowPrimalLP::build_supply() {
	total_demand = demand.sum();
	for (int i = 0; i < num_bids; ++i) {
		(*supply_map)[bid_nodes[i]] = num_goods * demand(i);
	}
	for (int j = 0; j < num_goods; ++j) {
		(*supply_map)[good_nodes[j]] = supply(j)-total_demand;
	}
	(*supply_map)[target_node] = -supply.sum();
}