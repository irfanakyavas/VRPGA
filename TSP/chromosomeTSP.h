#pragma once

#include <algorithm>
#include <random>
#include <iostream>
#include <random>
#include "Customer.h"
#include "RandomUtils.h"
#include <list>

typedef struct ChromosomeTSP
{
    std::vector<uint32_t> path;
    double fitness = DBL_MAX;

    uint32_t calculateFitness()
    {
        static std::vector<std::vector<double>> dists = distanceMatrixSorted;
        static std::vector<double> depotDists = depotDistancesSorted;

        fitness = depotDists[path[0]];
        for (uint32_t fi = 0; fi < Ncustomers() - 1; fi++)
        {
            fitness += dists[path[fi]][path[fi + 1]];
        }
        fitness += depotDists[path[Ncustomers()-1]];
        return fitness;
    }

    uint32_t Ncustomers() const
    {
		return path.size();
    }

    ChromosomeTSP()
    {
        this->path = std::vector<uint32_t>();
    }

    ChromosomeTSP(uint32_t Ncustomers)
    {
        this->path = std::vector<uint32_t>(Ncustomers);
    }
    void initialiseRandomly(std::vector<uint32_t> const& customerPool)
    {
        //this->path = std::vector<uint32_t>(customerPool.size());
        for (uint32_t fi = 0; fi < Ncustomers(); fi++)
            this->path[fi] = customerPool[fi];
        std::shuffle(std::begin(this->path), std::end(this->path), gen);
    }
};

std::ostream& operator<<(std::ostream& Str, ChromosomeTSP const& c) {
    Str << 0;
    for (uint32_t i = 0; i < c.Ncustomers(); i++)
    {
        Str << "->" << c.path[customersSortIndices[i]];
    }
    Str << std::endl;
    return Str;
}