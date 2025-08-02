#pragma once
#include "LemonDefs.h"
#include <Eigen/Dense>
#include <vector>
#include <memory>

class FlowDualLP
{
public:
	FlowDualLP(const Eigen::ArrayXXd &valuations, const Eigen::VectorXd &demand, const Eigen::VectorXd &supply, double penalty = 0.001);
	~FlowDualLP();

	void set_oversupply(const Eigen::ArrayXd &oversupply);

	void optimize();

	int get_objective_val();

	Eigen::VectorXd get_p();
private:
	graph_t graph;
	std::vector<std::vector<node_t>> source_nodes; //first index i: bids, second index j: goods
	std::vector<node_t> utility_nodes; //indexed by bid i
	std::vector<node_t> price_nodes; //indexed by good j
	std::vector<std::vector<arc_t>> source_to_utility; //indexed (i,j)
	std::vector<std::vector<arc_t>> source_to_price; //indexed (i,j)
	node_t price_sink; //Add extra node as sink for price node; reduces number of cost updates when supply is updated
	std::vector<arc_t> price_to_sink;
	std::unique_ptr<arcmap_t> arc_costs;
	std::unique_ptr<nodemap_t>  source_supply;

	std::unique_ptr<algorithm_t> algo;


	unsigned num_goods;
	unsigned num_bids;

	int optimal_value;

	const Eigen::ArrayXXd &valuations;
	const Eigen::VectorXd &demand;
	const Eigen::VectorXd &supply;
	double penalty = 0.001;

	void build_graph();
	void init_costs();
	void init_source_supply();
	void init_algo();

};

