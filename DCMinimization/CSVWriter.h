#pragma once
#include <string>
#include "Result.h"
#include <vector>
#include "OperationCounter.h"
#include "Auction.h"

class CSVWriter
{
public:
	CSVWriter();
	~CSVWriter();
	void write_results(const std::string & filename, const std::vector<Result> & results);
	void write_results(const std::string &filename, const std::vector<Result> &results, const std::vector<OperationCounter> &op_counter);
	void write_auction_data(const std::string &filename, const std::vector<Auction> &auctions);
};

