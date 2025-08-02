#pragma once
#include <string>
#include <Eigen/Dense>
#include <algorithm>
#include "CSVReader.h"


class Auction
{

public:
	Auction(std::string filename_bids, std::string filename_supply) {
		CSVReader csv_reader;
		Eigen::ArrayXXd bids_input = csv_reader.read_numeric(filename_bids, 1);
		Eigen::ArrayXXd supply_input = csv_reader.read_numeric(filename_supply, 1);
		num_goods = supply_input.size() / 2;
		size_t num_bids = bids_input.rows();
		supply.resize(num_goods);
		for (size_t i = 0; i < num_goods; ++i) supply[i] = supply_input(0, 2 * i);
		//Delete irrelevant columns
		size_t neg_start = 0;
		for (; bids_input(neg_start, 1) >= 0; ++neg_start);
		pos_valuation = bids_input.block(0, 2, neg_start, num_goods);
		neg_valuation = bids_input.block(neg_start, 2, num_bids - neg_start, num_goods);
		pos_demand = bids_input.block(0, 1, neg_start, 1);
		neg_demand = -bids_input.block(neg_start, 1, num_bids - neg_start, 1);
		num_pos_bids = pos_demand.size();
		num_neg_bids = neg_demand.size();
	}
	Auction() {}
	Eigen::ArrayXXd pos_valuation, neg_valuation;
	Eigen::VectorXd pos_demand, neg_demand;
	Eigen::VectorXd supply;
	size_t num_goods;
	size_t num_pos_bids;
	size_t num_neg_bids;
	~Auction() {}
};

