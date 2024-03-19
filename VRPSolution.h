#pragma once
#include <vector>
#include "Customer.h"

struct VRPSolution
{
	std::vector<uint32_t> totalDistances;
	std::vector<uint32_t> totalPackagesDelivered;
	std::vector<uint32_t> totalWorktimes;
	std::vector<std::vector<Customer>> tours;
};

struct VRPTour
{
	uint32_t totalDistance;
	uint32_t totalPackagesDelivered;
	uint32_t totalWorktime;
	std::vector<Customer> tour;
};