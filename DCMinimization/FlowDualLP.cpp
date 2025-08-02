#include "FlowDualLP.h"
#include <iostream>



FlowDualLP::FlowDualLP(const Eigen::ArrayXXd &valuations, const Eigen::VectorXd &demand, const Eigen::VectorXd &supply, double penalty) :
	valuations(valuations), demand(demand), supply(supply), penalty(penalty), num_goods(supply.size()), num_bids(demand.size())
{
	build_graph();
	init_costs();
	init_source_supply();
	init_algo();
}



FlowDualLP::~FlowDualLP()
{
}

void FlowDualLP::set_oversupply(const Eigen::ArrayXd & oversupply)
{
	std::cout << "supply: " << supply.transpose() << std::endl;
	std::cout << "demand: " << demand.transpose() << std::endl;
	for (unsigned j = 0; j < num_goods; ++j) {
		arc_costs->set(price_to_sink[j], supply[j] + oversupply[j]);
	}
	algo->costMap(*arc_costs);
}

void FlowDualLP::optimize()
{
	auto pt = algo->run();
	optimal_value = algo->totalCost();
}

int FlowDualLP::get_objective_val()
{
	return optimal_value;
}

Eigen::VectorXd FlowDualLP::get_p()
{
	Eigen::VectorXd result = Eigen::VectorXd(num_goods);
	for (unsigned j = 0; j < num_goods; ++j) {
		result[j] = algo->flow(price_to_sink[j]);
	}
	return result;
}

void FlowDualLP::build_graph()
{
	/*Make space for nodes*/
	source_nodes.resize(num_bids, std::vector<node_t>(num_goods));
	utility_nodes.resize(num_bids);
	price_nodes.resize(num_goods);
	/*Add nodes*/
	for (unsigned i = 0; i < num_bids; ++i) {
		for (unsigned j = 0; j < num_goods; ++j) {
			source_nodes[i][j] = graph.addNode();
		}
	}

	for (unsigned i = 0; i < num_bids; ++i) {
		utility_nodes[i] = graph.addNode();
	}

	for (unsigned j = 0; j < num_goods; ++j) {
		price_nodes[j] = graph.addNode();
	}

	price_sink = graph.addNode();

	/*Make space for arcs*/
	source_to_utility.resize(num_bids, std::vector<arc_t>(num_goods));
	source_to_price.resize(num_bids, std::vector<arc_t>(num_goods));
	price_to_sink.resize(num_goods);
	/*Add arcs*/
	for (unsigned i = 0; i < num_bids; ++i) {
		for (unsigned j = 0; j < num_goods; ++j) {
			source_to_utility[i][j] = graph.addArc(source_nodes[i][j], utility_nodes[i]);
			source_to_price[i][j] = graph.addArc(source_nodes[i][j], price_nodes[j]);
		}
	}

	for (unsigned j = 0; j < num_goods; ++j) {
		price_to_sink[j] = graph.addArc(price_nodes[j], price_sink);
	}
}

void FlowDualLP::init_costs()
{
	arc_costs = std::unique_ptr<arcmap_t>(new arcmap_t(graph));
	//Initializes costs for source_to_utiltiy arcs; costs for price arcs change frequently because of changing oversupply
	for (unsigned i = 0; i < num_bids; ++i) {
		for (unsigned j = 0; j < num_goods; ++j) {
			arc_costs->set(source_to_utility[i][j], demand(i));
			arc_costs->set(source_to_price[i][j], 0);
		}
	}
}

void FlowDualLP::init_source_supply()
{
	source_supply = std::unique_ptr<nodemap_t>(new nodemap_t(graph));
	for (unsigned i = 0; i < num_bids; ++i) {
		for (unsigned j = 0; j < num_goods; ++j) {
			source_supply->set(source_nodes[i][j], valuations(i, j));
		}
		source_supply->set(utility_nodes[i], std::numeric_limits<int>::lowest());
	}
	for (unsigned j = 0; j < num_goods; ++j) {
		source_supply->set(price_nodes[j], 0);
	}

	source_supply->set(price_sink, std::numeric_limits<int>::lowest());
}

void FlowDualLP::init_algo()
{
	algo = std::unique_ptr<algorithm_t>(new algorithm_t(graph));
	algo->supplyMap(*source_supply);
}
