#include "RandomSignature.h"
#include <cstdlib>



RandomSignature::RandomSignature(size_t num_goods):
	num_goods(num_goods)
{
}

std::vector<double> RandomSignature::generate()
{
	std::vector<double> signature(num_goods);
	for (size_t i = 0; i < num_goods; ++i) {
		signature[i] = 2*(rand() % 2)-1;
	}
	return signature;
}

RandomSignature::~RandomSignature()
{
}
