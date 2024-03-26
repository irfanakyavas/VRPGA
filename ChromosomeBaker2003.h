#pragma once
#include <vector>
#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>
#include "TSP/TSPGA.h"
#include "Customer.h"
#include "RandomUtils.h"

#define minv(vec) *std::min_element(vec.begin(), vec.end())
#define maxv(vec) *std::max_element(vec.begin(), vec.end())
#ifdef _DEBUG
#define min std::min
#endif // _DEBUG

//TODO: bu parametreler farklı bir dosyada tanımlanacak
uint32_t MAX_WORK_TIME = INT32_MAX;
uint32_t VEHICLE_CAPACITY = 30;
double minutesPerPackageDelivery = 4.0;

struct Chromosome {
	std::vector<uint32_t> genes;

	std::vector<std::vector<uint32_t>> customersVisitedByVehicle;
	std::vector<uint32_t> workTimeByVehicle;
	std::vector<uint32_t> packagesDeliveredByVehicle;

	std::vector<std::vector<double>> vehiclePoints;
	std::vector<double> vehicleAngles;

	uint32_t totalWorkTime = 0;
	uint32_t totalPackagesDelivered = 0;
	uint32_t Nvehicles = 0;
	double unfitness = 0.0;

	Chromosome(uint32_t Ncustomers, uint32_t Nvehicles) {
		this->genes = std::vector<uint32_t>(Ncustomers);
		this->customersVisitedByVehicle = std::vector<std::vector<uint32_t>>(Nvehicles + 1);
		this->workTimeByVehicle = std::vector<uint32_t>(Nvehicles+1, 0);
		this->packagesDeliveredByVehicle = std::vector<uint32_t>(Nvehicles+1, 0);
		this->Nvehicles = Nvehicles;
		this->totalWorkTime = 0;
		this->totalPackagesDelivered = 0;

		for (uint32_t i = 1; i < Nvehicles; i++)
		{
			customersVisitedByVehicle[i] = std::vector<uint32_t>();
		}
	}

	uint32_t Ncustomers() const
	{
		return this->genes.size();
	}

	uint32_t calculateFitness(bool verbose = false)
	{
		uint32_t i = 0;
		/*
		uint32_t NvehiclesReal = maxv(genes);
		if(NvehiclesReal != Nvehicles)
		{
			cout << "ERROR: NvehiclesReal != Nvehicles" << endl;
		}*/
		for (i = 0; i < Ncustomers(); i++)
		{
			/*
			if (genes[i] == 0)
			{
				cout << "WARNING: Must not assign to vehicle 0" << endl;
			}
			*/
			customersVisitedByVehicle[genes[i]].push_back(i);
			packagesDeliveredByVehicle[genes[i]] += Customer::customerDemands[customersSortIndices[i]];
			workTimeByVehicle[genes[i]] += (uint32_t)round(Customer::customerDemands[customersSortIndices[i]] * minutesPerPackageDelivery);
			totalWorkTime += (uint32_t)round(Customer::customerDemands[customersSortIndices[i]] * minutesPerPackageDelivery);
			totalPackagesDelivered += Customer::customerDemands[customersSortIndices[i]];
		}
		for (i = 1; i <= Nvehicles; i++)
		{
			if (verbose) std::cout << std::endl << "\t\t[TSP] Solving TSP for vehicle " << i << std::endl;
			customersVisitedByVehicle[i] = solveTSPforVehicle(customersVisitedByVehicle[i], 6, 128, workTimeByVehicle[i], verbose);
			totalWorkTime += workTimeByVehicle[i];
			unfitness = (packagesDeliveredByVehicle[i] > VEHICLE_CAPACITY) ? ((packagesDeliveredByVehicle[i] - VEHICLE_CAPACITY) / ((double)VEHICLE_CAPACITY)) : 0.0;
			unfitness += (workTimeByVehicle[i] > MAX_WORK_TIME) ? ((workTimeByVehicle[i] - MAX_WORK_TIME) / ((double)MAX_WORK_TIME)) : 0.0;
		}

		if (verbose) std::cout << std::endl << "\t\t[VRP] All TSP instances solved for chromosome" << std::endl;
		if (verbose) std::cout << "\t\t\tTotal Work Time=" << totalWorkTime << " Total Packages Delivered=" << totalPackagesDelivered << std::endl;
		//vehicleAngles = std::vector<double>(Nvehicles);
		//vehiclePoints = std::vector<vector<double>>(Nvehicles + 1);
		for (i = 1; i <= Nvehicles; i++)
		{
			//double sumLat = 0;
			//double sumLon = 0;
			//vehiclePoints[i] = vector<double>(2);

			if (verbose) std::cout << "\t\t\tVehicle " << i << "\tWork Time=" << workTimeByVehicle[i] << "\tPackages Delivered = " << packagesDeliveredByVehicle[i] << std::endl;
			if (verbose) std::cout << "\t\t\tRoute: 0";
			for (uint32_t j = 0; j < customersVisitedByVehicle[i].size(); j++)
			{
				if (verbose) std::cout << "->" << customersVisitedByVehicle[i][j] + 1;
				genes[customersVisitedByVehicle[i][j]] = i;
				//sumLat += Customer::customerLAT[customersSortIndices[customersVisitedByVehicle[i][j]]];
				//sumLon += Customer::customerLON[customersSortIndices[customersVisitedByVehicle[i][j]]];
			}
			//vehiclePoints[i][0] = sumLat / customersVisitedByVehicle[i].size();
			//vehiclePoints[i][1] = sumLon / customersVisitedByVehicle[i].size();
			//vehicleAngles[i-1] = angleWithDepot(vehiclePoints[i][0], vehiclePoints[i][1]);
		}
		//vector<uint32_t> vehicleIndicesByDepotAngle = CIDsByAngle(vehicleAngles);

		if (verbose) std::cout << std::endl << std::endl;

		return totalWorkTime;
	}
};

std::ostream& operator<<(std::ostream& Str, Chromosome const& c) {
	for (uint32_t i = 0; i < c.Ncustomers(); i++)
	{
		Str << c.genes[i] << " ";
	}
	Str << std::endl;
	return Str;
}

void initialiseRandomChromosome(Chromosome& c, bool verbose = false)
{
	uint32_t customersAssignedSoFar = 0;
	uint32_t numberOfCustomers = c.Ncustomers();
	uint32_t meanCustomersPerVehicle = numberOfCustomers / c.Nvehicles;
	uint32_t lastIndexAssigned = 0;
	if (verbose) std::cout << std::endl << "[VRP] Random vehicle assignment started" << std::endl;

	uint32_t numberOfCustomersAssigned = min(randomInteger(meanCustomersPerVehicle * 0.95, meanCustomersPerVehicle * 1.05), numberOfCustomers - customersAssignedSoFar);
	uint32_t k = randomInteger(numberOfCustomers - (numberOfCustomersAssigned / 2), numberOfCustomers);



	for (uint32_t i = 0; i < c.Nvehicles - 1; i++)
	{
		numberOfCustomersAssigned = min(randomInteger(meanCustomersPerVehicle * 0.95, meanCustomersPerVehicle * 1.05), numberOfCustomers - customersAssignedSoFar);
		for (uint32_t j = 0; j < numberOfCustomersAssigned; j++)
			c.genes[k++ % numberOfCustomers] = i + 1;
		customersAssignedSoFar += numberOfCustomersAssigned;
		if (verbose) std::cout << "\t[VRP] Assigned " << numberOfCustomersAssigned << " customers to vehicle " << i << "\t" << numberOfCustomers - customersAssignedSoFar << " remaining. Total customer count: " << numberOfCustomers << std::endl;

	}
	for (uint32_t i = 0; i < numberOfCustomers-customersAssignedSoFar; i++)
	{
		c.genes[k++ % numberOfCustomers] = c.Nvehicles;
	}
	if (verbose) std::cout << "\t[VRP] Assigned remaining " << numberOfCustomers - customersAssignedSoFar << " customers to last vehicle " << c.Nvehicles << std::endl;
	//std::rotate(begin(c.genes), begin(c.genes)+randomInteger(1, numberOfCustomers/2), end(c.genes));
}