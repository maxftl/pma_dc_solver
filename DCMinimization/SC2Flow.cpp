#include "SC2Flow.h"
#include <limits>
#include <iostream>




SC2Flow::SC2Flow(const Eigen::ArrayXXd & valuations, const Eigen::VectorXd & demand, const Eigen::VectorXd & supply, double penalty) :
	valuations(valuations),
	demand(demand),
	supply(supply),
	penalty(penalty),
	num_goods(supply.size()),
	num_bids(demand.size()),
	num_nodes(2 + num_goods + num_bids),
	num_arcs(1 + num_goods + num_goods*num_bids + num_bids),
	cs_alg(num_nodes, num_arcs),
	good_start(2),
	bid_start(good_start + num_goods),
	prices(Eigen::VectorXd::Zero(num_goods))
{
	cs_alg._comp_duals = true;
	cs_alg._cost_restart = true;
	//Initialize graph
	//Balancing arc
	cs_alg.set_arc(source_nd(), target_nd(), 0, std::numeric_limits<long>::max(), 0);
	//Source to supply
	for (int j = 0; j < num_goods; ++j) {
		cs_alg.set_arc(source_nd(), good_nd(j), 0, static_cast<long>(supply(j)), 0);
	}
	//supply to bid
	for (int i = 0; i < num_bids; ++i) {
		for (int j = 0; j < num_goods; ++j) {
			cs_alg.set_arc(good_nd(j), bid_nd(i), 0, std::numeric_limits<long>::max(), -static_cast<long long>(valuations(i, j)));
		}
	}
	//bid to target
	for (int i = 0; i < num_bids; ++i) {
		cs_alg.set_arc(bid_nd(i), target_nd(), 0, static_cast<long>(demand(i)), 0);
	}

	cs_alg.set_supply_demand_of_node(source_nd(), static_cast<long>(supply.sum()+1));
	cs_alg.set_supply_demand_of_node(target_nd(), -static_cast<long>(supply.sum()+1));

	cs_alg.pre_processing();
	cs_alg._m = 2*cs_alg._m;
	cs_alg.cs2_initialize();
}

SC2Flow::~SC2Flow()
{
}

void SC2Flow::set_oversupply(const Eigen::VectorXd & oversupply)
{
	for (int j = 0; j < num_goods; ++j) {
		cs_alg.set_arc(source_nd(), good_nd(j), 0, static_cast<long>(supply(j) + oversupply(j)), 0);
	}
	cs_alg.set_supply_demand_of_node(source_nd(), static_cast<long>(supply.sum() + oversupply.sum()+1));
	cs_alg.set_supply_demand_of_node(target_nd(), -static_cast<long>(supply.sum() + oversupply.sum()+1));
	cs_alg.cs_cost_reinit();
}

double SC2Flow::get_objective_val()
{
	return objective_val;
}

void SC2Flow::optimize()
{
	cs_alg.cs2(&objective_val);
	objective_val *= -1;
	long long alpha = cs_alg._nodes[source_nd() - cs_alg._node_min].price();
	for (int j = 0; j < num_goods; ++j) {
		prices(j) = cs_alg._nodes[good_nd(j) - cs_alg._node_min].price() - alpha;
	}
}

/*void SC2Flow::print_duals()
{
	std::cout << "CS correct: " << ((cs_alg.check_cs())?"Yes":"No") << std::endl;
	std::cout << "Goods: " << std::endl;

	long long cost = 0;
	/*long long cost = numeric_limits<long long>::max();
	for (auto i = cs_alg._nodes; i != cs_alg._sentinel_node; ++i) {
		cost = min(cost, i->price());
	}
	std::cout << "Cost calculated by me: " << cost << std::endl;*/
/*
	std::cout << "Source: " << cs_alg._nodes[source_nd()-cs_alg._node_min].price() - cost << std::endl;
	for (int j = 0; j < num_goods; ++j) {
		std::cout << "Good " << j + 1 << ": " << cs_alg._nodes[good_nd(j)-cs_alg._node_min].price()-cost << std::endl;
	}
	std::cout << "Bid nodes: " << std::endl;
	for (int i = 0; i < num_bids; ++i) {
		std::cout << "Bid " << i + 1 << ": " << cs_alg._nodes[bid_nd(i)-cs_alg._node_min].price()-cost  << std::endl;
	}
	std::cout << "Target: " << cs_alg._nodes[target_nd()-cs_alg._node_min].price()-cost  << std::endl;
	//Check dual inequalities:
	bool sat = true;
	for (int i = 0; i < num_bids; ++i) {
		for (int j = 0; j < num_goods; ++j) {
			if (-cs_alg._nodes[good_nd(j)].price() - cs_alg._nodes[bid_nd(i)].price() < valuations(i, j)) sat = false;
		}
	}
	std::cout << "Dual inqualities satisfied: " << sat << std::endl;
	cs_alg.print_solution();
}
*/