#include <iostream>
#include "FlowDualLP.h"
#include <Eigen/Dense>
#include <string>
#include "CSVReader.h"
#include "Auction.h"
#include "Result.h"
#include "Algorithm.h"
#include "CSVWriter.h"
#include "SignatureIterator.h"
#include "DemandVectorIterator.h"
#include <chrono>
#include "ConsoleLogger.h"
#include "TrajectoryLogger.h"
#include "TrajectoryVisualiser.h"
#include "OperationCounter.h"
#include "ListWriter.h"
#include "FlowPrimalLP.h"
#include "FlowSupplyLP.h"
#include "SupplyFastSolver.h"
#include "SQLiteReader.h"
#include "SQLiteWriter.h"
#include "Difference_Plotter.h"
#include "SteepestDescent.h"
#include <iterator>
#include "Verifier.h"
#include "SC2Flow.h"

using namespace std;

void write_vectors(const vector<Eigen::VectorXd> &vectors, const string &filename) {
	ofstream out(filename);
	if (!out.is_open()) {
		cerr << "Could not open file " << filename << endl;
	}
	for (auto &v : vectors) {
		for (int i = 0; i < v.size(); ++i) {
			out << v(i);
			if (i != v.size() - 1) out << ",";
		}
		out << endl;
	}
	out.close();
}

vector<Result> run_multiple(const vector<AuctionMeta> &auction_meta, SQLiteReader &sqlreader, SQLiteWriter &sqlwriter) {
	for (const AuctionMeta &am: auction_meta) {
		cout << "Calculating instance " << am.id << endl;
		Auction auction;
		sqlreader.open(am.id);
		sqlreader.get_bids(auction.pos_valuation, auction.neg_valuation, auction.pos_demand, auction.neg_demand);
		sqlreader.get_supply(auction.supply);
		auction.num_pos_bids = auction.pos_valuation.rows();
		auction.num_neg_bids = auction.neg_valuation.rows();
		auction.num_goods = auction.supply.size();
		Algorithm alg(auction);
		alg.set_verbose(false);
		OperationCounter oc;
		alg.add_event_handler(&oc);
		auto start = chrono::steady_clock::now();
		Result r = alg.run_price_based();
		r.running_time = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count();
		sqlwriter.write_result(am.id, r, oc);
	}
}

/*void run_single(GRBEnv & env, string base_path, int index) {
	ConsoleLogger console_logger;
	TrajectoryLogger trajectory_logger;
	OperationCounter operation_counter;
	stringstream bids_filename;
	bids_filename << base_path << "genbid" << index << ".csv";
	stringstream supply_filename;
	supply_filename << base_path << "gensupply" << index << ".csv";
	Auction auction(bids_filename.str(), supply_filename.str());
	Algorithm alg(env, auction);
	alg.set_verbose(true);
	alg.add_event_handler(&console_logger);
	alg.add_event_handler(&trajectory_logger);
	alg.add_event_handler(&operation_counter);
	//Result r = alg.run(Algorithm::DescentStrategy::SubmodularMin);
	cout << "Starting..." << endl;
	auto start = chrono::steady_clock::now();
	Result r = alg.run_price_based();
	cout << "Elapsed time:\n" <<
		chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - start).count() << "ms" << endl;
	cout << "Final price vector: " << r.prices.transpose() << endl;
	cout << "Objective value: " << r.objective_value << endl;
	cout << "Number LPs solved: " << operation_counter.get_num_lps() << endl;
	cout << "Number submodular minimizations: " << operation_counter.get_num_sm_minimizations() << endl;
	cout << "Number of iterations: " << operation_counter.get_num_iterations() << endl;
	cout << "Number descent steps: " << operation_counter.get_num_descent_searches() << endl;

	
	stringstream trajectory_filename;
	trajectory_filename << base_path << "trajectories\\traj" << index << ".txt";
	write_vectors(trajectory_logger.price_trajectory, trajectory_filename.str());
	//TrajectoryVisualiser visualiser;
	//visualiser.price_trajectory_to_svg(auction, trajectory_logger.price_trajectory, "trajectory.svg");
}*/

/*void flow_primal_test() {
	string base_path = "C:\\Users\\Maximilian Fichtl\\Documents\\publications\\Max_Productmix_Auction\\Code\\BidsGenerator_New\\auctions_2_goods\\";
	Eigen::VectorXd oversupply = Eigen::VectorXd::Zero(2);
	oversupply(0) = 0;
	oversupply(1) = 0;
	int index = 0;
	stringstream bids_filename;
	bids_filename << base_path << "genbid" << index << ".csv";
	stringstream supply_filename;
	supply_filename << base_path << "gensupply" << index << ".csv";
	Auction auction(bids_filename.str(), supply_filename.str());
	cout << auction.supply << endl;
	FlowPrimalLP fpl(auction.pos_valuation, auction.pos_demand, auction.supply);
	fpl.set_oversupply(oversupply);
	auto start = chrono::steady_clock::now();
	fpl.optimize();
	auto end = chrono::steady_clock::now();
	cout << "Flow time: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << " ms" << endl;
	cout << "Flow: " << endl;
	cout << "Welfare: " << fpl.get_objective_val() << endl;
	cout << "Prices: " << fpl.get_p().transpose() << endl << endl;

	GRBEnv env;
	env.set(GRB_IntParam_OutputFlag, 0);
	cout << "Gurobi: " << endl;
	DualLP dlp(env, auction.pos_valuation, auction.pos_demand, auction.supply, 0);
	dlp.set_oversupply(oversupply);
	start = chrono::steady_clock::now();
	dlp.optimize();
	end = chrono::steady_clock::now();
	cout << "LP time: " << chrono::duration_cast<chrono::microseconds>(end - start).count() << " ms" << endl;
	cout << "Welfare: " << dlp.get_objective_val() << endl;
	cout << "Prices: " << dlp.get_p().transpose() << endl;
}*/


void write_map_to_file(const Difference_Plotter::graph_t &graph, const string &filename) {
	ofstream ostrm(filename);
	if (!ostrm.is_open()) {
		cerr << "Could not open file " << filename << endl;
	}
	for (auto it = graph.begin(); it != graph.end(); ++it) {
		ostrm << it->first.first << " " << it->first.second << " " << it->second << endl;
	}
}

int main() {	
	/* Code for running tests*/
	/*
	sqlite3 *db; sqlite3_open("C:\\Users\\Maximilian Fichtl\\Documents\\publications\\Max_Productmix_Auction\\Code\\Auctions_Dataset\\test_dataset.db", &db);
	SQLiteReader sqlreader(db);
	SQLiteWriter sqlwriter(db);
	std::vector<AuctionMeta> auction_meta = sqlreader.get_auction_meta("WHERE comment='0.01_negbids_15_goods'");
	run_multiple(auction_meta, sqlreader, sqlwriter);	
	sqlite3_close(db);
	cin.ignore();
	*/

	GRBEnv env("env");
	env.set(GRB_IntParam_OutputFlag, 0);

	sqlite3 *db; sqlite3_open("C:\\Users\\Maximilian Fichtl\\Documents\\publications\\Max_Productmix_Auction\\Code\\Auctions_Dataset\\test_dataset.db", &db);
	SQLiteReader sqlreader(db);
	SQLiteWriter sqlwriter(db);
	std::vector<AuctionMeta> auction_meta = sqlreader.get_auction_meta("WHERE comment='0.01_negbids_30_goods'");
	Auction auction;
	sqlreader.open(auction_meta[0].id);
	sqlreader.get_bids(auction.pos_valuation, auction.neg_valuation, auction.pos_demand, auction.neg_demand);
	sqlreader.get_supply(auction.supply);
	auction.num_pos_bids = auction.pos_valuation.rows();
	auction.num_neg_bids = auction.neg_valuation.rows();
	auction.num_goods = auction.supply.size();
	cout << "Supply: " << auction.supply.transpose() << endl;

	int num_perturbations = 10;
	double perturb_spread = 10;
	Eigen::ArrayXXd perturbations = (perturb_spread * Eigen::ArrayXXd::Random(auction.num_goods, num_perturbations)).floor();

	cout << "Perturbations: \n" << perturbations << "\n" << endl;


	FlowPrimalLP network_solver(auction.pos_valuation, auction.pos_demand, auction.supply);
	DualLP gurobi_solver(env,auction.pos_valuation, auction.pos_demand, auction.supply);

	for (int i = 0; i < num_perturbations; ++i) {
		auto network_start = chrono::steady_clock::now();
		network_solver.set_oversupply(perturbations.col(i).matrix());
		network_solver.optimize();
		auto network_end = chrono::steady_clock::now();
		auto gurobi_start = chrono::steady_clock::now();
		gurobi_solver.set_oversupply(perturbations.col(i).matrix());
		gurobi_solver.optimize();
		auto gurobi_end = chrono::steady_clock::now();

		auto network_time = chrono::duration_cast<chrono::milliseconds>(network_end - network_start).count();
		auto gurobi_time = chrono::duration_cast<chrono::milliseconds>(gurobi_end - gurobi_start).count();

		cout << "Network Prices: " << network_solver.get_p().transpose() << endl;
		cout << "Gurobi Prices: " << gurobi_solver.get_p().transpose() << endl;
		cout << endl;
		cout << "Network time: " << network_time << endl;
		cout << "Gurobi time: " << gurobi_time << endl;
	}
	
	cin.ignore();

	return 0;
}
