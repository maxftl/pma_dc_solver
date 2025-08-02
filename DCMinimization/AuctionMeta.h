#pragma once

class AuctionMeta {
public:
	int id;
	int num_bids;
	int num_goods;
	int b_lb;
	int b_ub;
	int a_lb;
	int a_ub;
	int displacement_lb;
	int displacement_ub;
	int weight_lb;
	int weight_ub;
	int num_pos_bids;
	int num_neg_bids;
};