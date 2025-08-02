#pragma once
#include <iostream>
#include "AlgorithmEventHandler.h"

class ConsoleLogger : public AlgorithmEventHandler {
public:
	ConsoleLogger() : iteration_counter(0), descent_search_counter(0) {}
	void algorithm_started() {};
	void oversupply_updated(const Eigen::VectorXd &oversupply) { std::cout << "Current oversupply: " << oversupply.transpose() << std::endl; }
	void price_updated(const Eigen::VectorXd &price) { std::cout << "Current price: " << price.transpose() << std::endl; }
	void iteration_started() { ++iteration_counter; }
	void iteration_ended() { std::cout << std::endl; }
	void descent_search_started() { ++descent_search_counter; std::cout << "Searching for descent direction" << std::endl; }
	void descent_direction_found(const Eigen::VectorXd& descent_direction) { std::cout << "Found descent direction " << descent_direction.transpose() << std::endl; }
	void algorithm_terminated(const Result &result) { std::cout << "Algorithm terminated after " << iteration_counter << " iterations." << std::endl; }
private:
	int iteration_counter;
	int descent_search_counter;
};