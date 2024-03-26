#pragma once

#include <algorithm>
#include <random>
#include <iostream>
#include <random>
#include "Customer.h"
#include "RandomUtils.h"
#include "defines.h"
#include <list>

typedef struct ChromosomeTSP
{
    std::vector<T_CUSTOMERID> path;
    T_FITNESS fitness = UINT32_MAX;
    T_CUSTOMERID Ncustomers;

    T_FITNESS calculateFitness()
    {
        static std::vector<std::vector<T_DIST>> dists = distanceMatrixSorted;
        static std::vector<T_DIST> depotDists = depotDistancesSorted;

        fitness = depotDists[path[0]];
        for (uint32_t fi = 0; fi < Ncustomers - 1; fi++)
            fitness += dists[path[fi]][path[fi + 1]];
        return fitness;
    }
    ChromosomeTSP() : Ncustomers(0)
    {
    }
    ChromosomeTSP(const T_CUSTOMERID &Ncustomers) : Ncustomers(Ncustomers)
    {
        this->path = std::vector<T_CUSTOMERID>(Ncustomers);
    }

    void initialiseRandomly(std::vector<T_CUSTOMERID> const& customerPool)
    {
        static std::random_device rd;
        static std::mt19937 _gen(rd());
        //this->path = std::vector<uint32_t>(customerPool.size());
        for (uint32_t fi = 0; fi < Ncustomers; fi++)
            this->path[fi] = customerPool[fi];
        std::shuffle(std::begin(this->path), std::end(this->path), _gen);
    }
};

std::ostream& operator<<(std::ostream& Str, ChromosomeTSP const& c) {
    Str << 0;
    for (uint32_t i = 0; i < c.Ncustomers; i++)
    {
        Str << "->" << c.path[customersSortIndices[i]];
    }
    Str << std::endl;
    return Str;
}