#pragma once
#include <Eigen/Dense>

class Result
{
public:
	Result();
	~Result();
	Eigen::VectorXd oversupply;
	Eigen::VectorXd prices;
	double objective_value;
	int running_time;
};

