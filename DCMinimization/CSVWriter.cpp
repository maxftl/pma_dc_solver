#include "CSVWriter.h"
#include <fstream>
#include <iostream>
#include "Auction.h"

using namespace std;

CSVWriter::CSVWriter()
{
}


CSVWriter::~CSVWriter()
{
}

void CSVWriter::write_results(const string & filename, const vector<Result> & results) {
	ofstream file_out;
	file_out.open(filename);
	if (!file_out) {
		cout << "File " << filename << " could not be openend." << endl;
		return;
	}
	file_out << "Prices, Objective value, Running time(ms) \n";
	for_each(results.begin(), results.end(), [&file_out](const Result &r) { file_out <<
		r.prices.transpose() << "," <<
		r.objective_value << "," <<
		r.running_time << "\n"; });
	file_out.close();
}

void CSVWriter::write_results(const std::string & filename, const std::vector<Result>& results, const std::vector<OperationCounter> & op_counters)
{
	ofstream file_out;
	file_out.open(filename);
	if (!file_out) {
		cout << "File " << filename << " could not be openend." << endl;
		return;
	}
	file_out << "Prices, Objective value, Running time (ms), Iterations, Lyaponov descent steps, LPs solved \n";
	for (int i = 0; i < results.size(); ++i) {
		auto r = results[i];
		auto o = op_counters[i];
		file_out << r.prices.transpose() 
			<< "," << r.objective_value 
			<< "," << r.running_time
			<< "," << o.get_num_iterations()
			<< "," << o.get_num_descent_searches() 
			<< "," << o.get_num_lps() << "\n";
	}
	file_out.close();
}

void CSVWriter::write_auction_data(const std::string & filename, const vector<Auction> &auctions) {
	ofstream file_out;
	file_out.open(filename);
	file_out << "Goods, Positive Bids, Negative Bids\n";
	for (auto it = auctions.begin(); it != auctions.end(); ++it) {
		file_out << it->num_goods << ","
			<< it->num_pos_bids << ","
			<< it->num_neg_bids << ","
			<< endl;
	}
	file_out.close();
}
