#pragma once
#include <vector>

class RandomSignature
{
public:
	RandomSignature(size_t num_goods);
	std::vector<double> generate();
	~RandomSignature();
private:
	size_t num_goods;
};

