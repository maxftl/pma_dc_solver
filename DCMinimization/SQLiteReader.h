#pragma once
#include <Eigen/Dense>
#include <sqlite3.h>
#include <string>
#include <iostream>
#include <sstream>
#include "AuctionMeta.h"

class SQLiteReader
{
public:
	SQLiteReader(sqlite3* db): db(db) {}


	void open(int auction_id) {
		this->auction_id = auction_id;
		sqlite3_stmt *get_n_goods;
		if (sqlite3_prepare_v2(db, "SELECT num_goods, num_bids, num_pos_bids, num_neg_bids FROM auctions WHERE rowid = ?;", -1, &get_n_goods, NULL) != SQLITE_OK) {
			std::cerr << "Could not read num_auctions" << std::endl;
			return;
		}
		sqlite3_bind_int(get_n_goods, 1, auction_id);
		sqlite3_step(get_n_goods);
		num_goods = sqlite3_column_int(get_n_goods, 0);
		num_bids = sqlite3_column_int(get_n_goods, 1);
		num_pos_bids = sqlite3_column_int(get_n_goods, 2);
		num_neg_bids = sqlite3_column_int(get_n_goods, 3);
		sqlite3_finalize(get_n_goods);
	}

	void get_bids(Eigen::ArrayXXd &pos_valuations, Eigen::ArrayXXd &neg_valuations, Eigen::VectorXd &pos_demand, Eigen::VectorXd &neg_demand) {
		pos_valuations = Eigen::ArrayXXd::Zero(num_pos_bids, num_goods);
		neg_valuations = Eigen::ArrayXXd::Zero(num_neg_bids, num_goods);
		pos_demand = Eigen::VectorXd(num_pos_bids);
		neg_demand = Eigen::VectorXd(num_neg_bids);

		std::stringstream stmt_str; stmt_str << "SELECT quantity";
		for (int i = 0; i < num_goods; ++i) stmt_str << ",v" << i;
		stmt_str << " FROM bids WHERE auction_id=" << auction_id << ";";
		//std::cout << stmt_str.str() << std::endl;
		sqlite3_stmt *get_bids;
		if (sqlite3_prepare_v2(db, stmt_str.str().c_str(), -1, &get_bids, NULL) != SQLITE_OK) {
			std::cerr << "Could not prepare get_bids statement" << std::endl;
		}

		int pi = 0;
		int ni = 0;
		while (sqlite3_step(get_bids) == SQLITE_ROW) {
			int q = sqlite3_column_int(get_bids, 0);
			if (q > 0) {
				pos_demand(pi) = q;
				for (int j = 0; j < num_goods; ++j) pos_valuations(pi, j) = sqlite3_column_int(get_bids, j + 1);
				++pi;
			}
			else {
				neg_demand(ni) = -q;
				for (int j = 0; j < num_goods; ++j) neg_valuations(ni, j) = sqlite3_column_int(get_bids, j + 1);
				++ni;
			}
		}

		sqlite3_finalize(get_bids);
	}

	void get_supply(Eigen::VectorXd &supply) {
		supply = Eigen::VectorXd::Zero(num_goods);
		std::stringstream stmt_str; stmt_str << "SELECT s0";
		for (int j = 1; j < num_goods; ++j) stmt_str << ",s" << j;
		stmt_str << " FROM supply WHERE auction_id=" << auction_id << ";";
		//std::cout << stmt_str.str() << std::endl;
		sqlite3_stmt *get_supply;
		if (sqlite3_prepare_v2(db, stmt_str.str().c_str(), -1, &get_supply, NULL) != SQLITE_OK) {
			std::cerr << "Could not prepare supply statement" << std::endl;
			return;
		}
		sqlite3_step(get_supply);
		for (int j = 0; j < num_goods; ++j) supply(j) = sqlite3_column_int(get_supply, j);

		sqlite3_finalize(get_supply);
	}

	std::vector<AuctionMeta> get_auction_meta(const std::string &filter = "") {
		std::vector<AuctionMeta> result;
		std::stringstream get_strstream;
		get_strstream << "SELECT rowid, num_bids, num_goods, b_lb, b_ub, a_lb, a_ub, displacement_lb, displacement_ub, weight_lb, weight_ub, num_pos_bids, num_neg_bids FROM auctions " << filter << ";";
		std::string get_str = get_strstream.str();

		sqlite3_stmt *get_stmt;
		if (sqlite3_prepare_v2(db, get_str.c_str(), -1, &get_stmt, NULL) != SQLITE_OK) {
			std::cerr << "Could not prepare metadata statement" << std::endl;
			return result;
		}

		while (sqlite3_step(get_stmt) == SQLITE_ROW) {
			AuctionMeta am;
			am.id = sqlite3_column_int(get_stmt, 0);
			am.num_bids = sqlite3_column_int(get_stmt, 1);
			am.num_goods = sqlite3_column_int(get_stmt, 2);
			am.b_lb = sqlite3_column_int(get_stmt, 3);
			am.b_ub = sqlite3_column_int(get_stmt, 4);
			am.a_lb = sqlite3_column_int(get_stmt, 5);
			am.a_ub = sqlite3_column_int(get_stmt, 6);
			am.displacement_lb = sqlite3_column_int(get_stmt, 7);
			am.displacement_ub = sqlite3_column_int(get_stmt, 8);
			am.weight_lb = sqlite3_column_int(get_stmt, 9);
			am.weight_ub = sqlite3_column_int(get_stmt, 10);
			am.num_pos_bids = sqlite3_column_int(get_stmt, 11);
			am.num_neg_bids = sqlite3_column_int(get_stmt, 12);
			result.push_back(am);
		}
		return result;
	}

private:
	sqlite3 *db;
	int num_goods;
	int num_bids;
	int num_pos_bids;
	int num_neg_bids;
	int auction_id;
};

