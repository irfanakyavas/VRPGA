#pragma once

#include <random>
#include <vector>
#include <algorithm>
#include <chrono>
unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
static std::mt19937 gen(seed);

/// <summary>
/// Generates a random integer in the range [min, max]
/// </summary>
/// <param name="min">Minimum value to return, inclusive.</param>
/// <param name="max">Maximum value to return, inclusive.</param>
/// <returns>Uniform random integer in the range [min, max].</returns>
uint32_t randomInteger(uint32_t min, uint32_t max)
{
	std::uniform_int_distribution<uint32_t> dis(min, max);
	return dis(gen);
}
double randomDouble(double min, double max)
{
	std::uniform_real_distribution<double> dis(min, max);
	return dis(gen);
}