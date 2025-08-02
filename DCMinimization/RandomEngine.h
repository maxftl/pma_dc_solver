#pragma once
#include <random>
#include <chrono>

class RandomEngine {
private:
	std::mt19937 engine;
	unsigned seed;
	RandomEngine() {
		seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		engine.seed(seed);
	}
public:
	RandomEngine(const RandomEngine&) = delete;
	RandomEngine(RandomEngine&&) = delete;
	RandomEngine& operator=(const RandomEngine&) = delete;
	RandomEngine&& operator=(RandomEngine&&) = delete;

	static RandomEngine& instance() {
		static RandomEngine re;
		return re;
	}

	int get_random_int(int lb, int ub) {
		return lb + (engine() % (ub - lb + 1));
	}

	unsigned get_seed() { return seed; }

	void set_seed(unsigned s) {
		seed = s;
		engine.seed(s);
	}

	std::mt19937& get_engine() { return engine; }
};