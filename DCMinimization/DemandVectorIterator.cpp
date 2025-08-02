#include "DemandVectorIterator.h"

DemandVectorIterator::DemandVectorIterator(int num_goods) : is_done(false),
num_goods(num_goods),
current_vector(Eigen::VectorXd::Zero(num_goods + 1)) {
	indices[0] = 0;
	indices[1] = 1;
	current_vector[indices[0]] = 1;
	current_vector[indices[1]] = -1;
}
