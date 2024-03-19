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
    uint32_t fitness = UINT_FAST32_MAX;

    uint32_t calculateFitness()
    {
        fitness = Customer::depotDistances[customersSortIndices[path[0]]];
        for (uint32_t fi = 0; fi < Ncustomers() - 1; fi++)
        {
            fitness += Distance(path[fi], path[fi + 1]);
        }
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
        this->path = std::vector<uint32_t>(customerPool.size());
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