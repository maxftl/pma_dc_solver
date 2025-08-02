#pragma once

#include <Eigen/Dense>
#include <sqlite3.h>
#include <string>
#include <iostream>
#include <sstream>
#include "Result.h"
#include "OperationCounter.h"

class SQLiteWriter {
public:
	SQLiteWriter(sqlite3 *db) : db(db) {}

	void write_result(int auction_id, const Result &r, const OperationCounter &oc) {
		std::stringstream insert_str; insert_str << "INSERT INTO dc_results (auction_id, optimal_value, running_time, num_iterations, num_lyapunov_steps, num_lps_solved";
		for (int i = 0; i < r.prices.size(); ++i) insert_str << ",p" << i;
		insert_str << ") VALUES (?,?,?,?,?,?";
		for (int i = 0; i < r.prices.size(); ++i) insert_str << ",?";
		insert_str << ");";

		std::cout << insert_str.str() << std::endl;
		sqlite3_stmt *insert_stmt;
		auto err = sqlite3_prepare_v2(db, insert_str.str().c_str(), -1, &insert_stmt, NULL);
		if (err != SQLITE_OK) {
			std::cerr << "Could not prepare insert statement" << err << std::endl;
			return;
		}

		sqlite3_bind_int(insert_stmt, 1, auction_id);
		sqlite3_bind_int(insert_stmt, 2, (int) r.objective_value);
		sqlite3_bind_int(insert_stmt, 3, r.running_time);
		sqlite3_bind_int(insert_stmt, 4, oc.get_num_iterations());
		sqlite3_bind_int(insert_stmt, 5, oc.get_num_descent_searches());
		sqlite3_bind_int(insert_stmt, 6, oc.get_num_lps());
		for (int i = 0; i < r.prices.size(); ++i) {
			sqlite3_bind_int(insert_stmt, 7 + i, r.prices(i));
		}

		sqlite3_step(insert_stmt);
		sqlite3_finalize(insert_stmt);
	}
private:
	sqlite3 *db;
};