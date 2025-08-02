#pragma once
#include <iterator>
#include <vector>


class SignatureIterator : public std::iterator<std::forward_iterator_tag, double*>
{
public:
	SignatureIterator(size_t num_goods) : mask(0), num_goods(num_goods), num_signatures(1 << num_goods), current_signature(num_goods,-1) {}
	SignatureIterator(const SignatureIterator &si) : mask(si.mask), current_signature(si.current_signature) {}
	void next() { ++mask; update_signature(); }
	bool done() { return mask >= num_signatures; }
	void update_signature() {
		for (int i = 0; i < num_goods; ++i) {
			if ((mask >> i) & 1) current_signature[i] = 1;
			else current_signature[i] = -1;
		}
	}
	std::vector<double> get() {
		return current_signature;
	}
	~SignatureIterator() {}
private:
	unsigned long mask;
	size_t num_goods;
	unsigned long num_signatures;
	std::vector<double> current_signature;
};

