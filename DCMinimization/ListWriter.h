#pragma once
#include <vector>
#include <Eigen/Dense>
#include <fstream>
#include <iostream>

class ListWriter {
public:
	template<class It>
	static void write_list(It begin, It end, std::ostream &out) {
		for (; begin != end; ++begin) {
			out << *begin << std::endl;
		}
	}

	template<class It>
	static void write_list(It begin, It end, const std::string &filename) {
		std::ofstream out(filename);
		if (!out.is_open()) {
			std::cerr << "Could not open file " << filename << "for writing" << std::endl;
		}
		write_list(begin, end, out);
		out.close();
	}
};