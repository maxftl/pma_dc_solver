#pragma once

#include "AlgorithmEventHandler.h"
#include <vector>
#include <Eigen/Dense>

class TrajectoryLogger : public AlgorithmEventHandler {
public:
	void oversupply_updated(const Eigen::VectorXd &oversupply) { supply_trajectory.push_back(oversupply); }
	void price_updated(const Eigen::VectorXd &price) { price_trajectory.push_back(price); }
	std::vector<Eigen::VectorXd> price_trajectory;
	std::vector<Eigen::VectorXd> supply_trajectory;
};