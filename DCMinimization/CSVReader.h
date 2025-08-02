#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <boost/tokenizer.hpp>
#include <algorithm>
#include <Eigen/Dense>

class CSVReader
{
public:
	CSVReader() {}
	Eigen::ArrayXXd read_numeric(std::string filename, int header_count = 0) {
		std::ifstream file_in;
		file_in.open(filename);
		if (!file_in) {
			std::cerr << "File " << filename << " could not be opened" << std::endl;
			return Eigen::ArrayXXd();
		}
		std::string line;
		for (int i = 0; i < header_count; ++i) std::getline(file_in, line);
		size_t max_length = 0;
		size_t num_lines = 0;
		boost::char_separator<char> sep(",");
		std::vector<std::vector<double>> line_values;
		for (; getline(file_in, line);) {
			++num_lines;
			boost::tokenizer<boost::char_separator<char>> tok(line, sep);
			std::vector<double> num_line;
			for_each(tok.begin(), tok.end(), [&num_line](std::string s) {
				num_line.push_back(std::stod(s));
			});
			line_values.push_back(num_line);
			max_length = std::max(max_length, num_line.size());
		}

		Eigen::ArrayXXd result(Eigen::ArrayXXd::Zero(num_lines, max_length));
		for (int i = 0; i < num_lines; ++i) {
			auto &curr_line = line_values[i];
			size_t num_cols = curr_line.size();
			for (int j = 0; j < num_cols; ++j) {
				result(i, j) = curr_line[j];
			}
		}
		return result;
	}
	~CSVReader() {}
private:
	int header_count;
	char delimiter;
};

