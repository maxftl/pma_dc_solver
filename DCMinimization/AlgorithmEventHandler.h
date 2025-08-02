#pragma once

#include <Eigen/Dense>
#include "Result.h"

class AlgorithmEventHandler
{
public:
	AlgorithmEventHandler() {};
	virtual void algorithm_started() {};
	virtual void oversupply_updated(const Eigen::VectorXd &oversupply) {}
	
	virtual void supply_lp_invoked() {}
	virtual void price_lp_invoked() {}

	virtual void price_updated(const Eigen::VectorXd &price) {}
	virtual void iteration_started() {}
	virtual void iteration_ended() {}
	virtual void descent_search_started() {}
	virtual void submodular_minimization_started() {}
	virtual void descent_direction_found(const Eigen::VectorXd& descent_direction) {}
	virtual void algorithm_terminated(const Result & result) {}
	virtual ~AlgorithmEventHandler() {};
};

