#pragma once
#include <Eigen/Dense>

class DemandVectorIterator
{
public:
	DemandVectorIterator(int num_goods); 
	Eigen::VectorXd  get() { return current_vector.block(1, 0, num_goods, 1); }
	void next() {
		if (current_vector[indices[0]] == 1) {
			current_vector[indices[0]] = -current_vector[indices[0]];
			current_vector[indices[1]] = -current_vector[indices[1]];
		}
		else {
			current_vector[indices[0]] = 0;
			current_vector[indices[1]] = 0;
			if (indices[1]++ == num_goods) {
				if (indices[0]++ == num_goods - 1) {
					is_done = true;
					indices[0] = 0;
				}
				indices[1] = indices[0] + 1;
			}
			current_vector[indices[0]] = 1;
			current_vector[indices[1]] = -1;
		}
	}
	bool done() { return is_done;  }
	~DemandVectorIterator() {}
private:
	int num_goods;
	int indices[2];
	Eigen::VectorXd current_vector;
	bool is_done;
};

