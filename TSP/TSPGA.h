#pragma once

#include "RandomUtils.h"
#include "chromosomeTSP.h"
#include "mutationTSP.h"
#include "Customer.h"
#include "pmx.h"
#include <chrono>
#include <vector>
#include "defines.h"
#include <omp.h>

std::vector<T_CUSTOMERID> solveTSPforVehicle(const std::vector<T_CUSTOMERID> &customers, const uint32_t &Ngen, uint32_t& Fstar)
{
    T_CUSTOMERID Ncustomers = customers.size();
    static T_CUSTOMERID s1, s2, s3, s4;//, Ncustomers = customers.size();
    static std::random_device rd;
    static std::mt19937 _gen(rd());
    static std::uniform_int_distribution<T_CUSTOMERID> dis(0, Npop_TSP - 1);
    static ChromosomeTSP chromosomes[Npop_TSP];
    static ChromosomeTSP new_chromosomes[Npop_TSP];
    std::uniform_int_distribution<T_CUSTOMERID> disC(0, Ncustomers - 1);
    std::vector<T_CUSTOMERID> invalidPath(Ncustomers, MAX_CID);

    for (uint32_t i = 0; i < Npop_TSP; i++)
    {
        chromosomes[i] = ChromosomeTSP(Ncustomers);
        new_chromosomes[i] = chromosomes[i];
        chromosomes[i].initialiseRandomly(customers);
        chromosomes[0] = chromosomes[i].calculateFitness() < chromosomes[0].fitness ? chromosomes[i] : chromosomes[0];
    }

    if (chromosomes[0].path.size() <= 3)
        return chromosomes[0].path;

    new_chromosomes[0] = chromosomes[0];

    for (uint32_t i = 0; i < Ngen / 2; i++)
    {
        for (uint32_t j = 1; j < Npop_TSP; j++)
        {            
            s1 = dis(_gen); s2 = dis(_gen); s3 = dis(_gen); s4 = dis(_gen);
            new_chromosomes[j].path = invalidPath;
            crossoverSCX(chromosomes[s1].fitness < chromosomes[s2].fitness ? chromosomes[s1].path : chromosomes[s2].path,
                chromosomes[s3].fitness < chromosomes[s4].fitness ? chromosomes[s3].path : chromosomes[s4].path,
                new_chromosomes[j].path);

            if (dis(_gen) == 0)
                std::swap(new_chromosomes[j].path[randomInteger(0, Ncustomers - 1)], new_chromosomes[j].path[randomInteger(0, Ncustomers - 1)]);
            
            new_chromosomes[0] = new_chromosomes[j].calculateFitness() < new_chromosomes[0].fitness ? new_chromosomes[j] : new_chromosomes[0];
        }
        for (uint32_t j = 1; j < Npop_TSP; j++)
        {
            s1 = dis(_gen); s2 = dis(_gen); s3 = dis(_gen); s4 = dis(_gen);
            chromosomes[j].path = invalidPath;
            crossoverSCX(new_chromosomes[s1].fitness < new_chromosomes[s2].fitness ? new_chromosomes[s1].path : new_chromosomes[s2].path,
                new_chromosomes[s3].fitness < new_chromosomes[s4].fitness ? new_chromosomes[s3].path : new_chromosomes[s4].path,
                chromosomes[j].path);
            
            if (dis(_gen) == 0)
                std::swap(chromosomes[j].path[randomInteger(0, Ncustomers - 1)], chromosomes[j].path[randomInteger(0, Ncustomers - 1)]);
                //std::swap(chromosomes[j].path[disC(_gen)], chromosomes[j].path[disC(_gen)]);

            chromosomes[0] = (chromosomes[j].calculateFitness() < chromosomes[0].fitness) ? chromosomes[j] : chromosomes[0];
        }
    }

/*
    if (verbose)
    {
        std::cout << "\t\t\t[TSP] TSPGA solver finished, Fstar=" << bestChromosomeEver.fitness << std::endl;
        std::cout << "\t\t\tBest path found: " << std::endl;
        std::cout << "\t\t\t" << 0 << "->";
        for (int i = 0; i < Ncustomers - 1; i++)
            std::cout << customersSortIndices[bestChromosomeEver.path[i]] << "->";
        std::cout << customersSortIndices[bestChromosomeEver.path[Ncustomers - 1]] << std::endl;
    }*/
    Fstar = chromosomes[0].fitness;
    return chromosomes[0].path;
}
#undef Ncustomers