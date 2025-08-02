#pragma once
#include <vector>

template<typename T>
class SubsetIterator
{
public:
	SubsetIterator(const std::vector<T> & set) : set(set), mask(0), num_subsets(1 << set.size()) {}
	~SubsetIterator() {}

	std::vector<T> operator*() {
		std::vector<T> result;
		for (size_t i = 0; i < set.size(); ++i) {
			if ((mask >> i) & 1) result.push_back(set[i]);
		}
		return result;
	}

	SubsetIterator<T> & operator++() {
		++mask;
		return *this;
	}

	bool is_last() {
		return mask > num_subsets;
	}


	bool operator==(const SubsetIterator<T> rhs) { return mask == rhs.mask; }
	bool operator!=(const SubsetIterator<T> rhs) { return mask != rhs.mask; }

private:
	const std::vector<T> & set;
	unsigned long mask;
	unsigned long num_subsets;
};

