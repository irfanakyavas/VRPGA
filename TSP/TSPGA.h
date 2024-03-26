#pragma once

#include "RandomUtils.h"
#include "chromosomeTSP.h"
#include "mutationTSP.h"
#include "Customer.h"
#include "pmx.h"
#include <chrono>
#include <vector>

#define minv(vec) *std::min_element(vec.begin(), vec.end())
#define maxv(vec) *std::max_element(vec.begin(), vec.end())
#define Npop_TSP 32
#include <omp.h>

std::vector<uint32_t> solveTSPforVehicle(std::vector<uint32_t> customers, uint32_t Pmut, uint32_t Ngen, double& Fstar, bool verbose = false)
{
    static std::mt19937 _gen(seed);
    std::uniform_int_distribution<uint32_t> dis(0, Npop_TSP - 1);

    uint32_t Ncustomers = customers.size();
    //uint32_t CustomerMax = maxv(customers) + 1;
    //bool* visited = (bool*)alloca(CustomerMax * sizeof(bool));
    //memset(visited, 0, CustomerMax *sizeof(bool));

    ChromosomeTSP chromosomes[Npop_TSP];
    ChromosomeTSP new_chromosomes[Npop_TSP];
    ChromosomeTSP tmp1;
    tmp1.path = std::vector<uint32_t>(Ncustomers);
    for (uint32_t i = 0; i < Ncustomers; i++)
        tmp1.path[i] = UINT32_MAX;
    for (ChromosomeTSP& c : chromosomes)
		c = ChromosomeTSP(Ncustomers);
    for (ChromosomeTSP& c : new_chromosomes)
        c = ChromosomeTSP(Ncustomers);

    //ChromosomeTSP tmp1;
    ChromosomeTSP bestChromosomeEver(Ncustomers);
    double bestFitnessEver = DBL_MAX, bestFitness = DBL_MAX;
    int i, j, i_max = 0;

    if (verbose) std::cout << "\t\t\t[TSP] Generating random initial population..." << std::endl;
    for (i = 0; i < Npop_TSP; i++)
    {
        chromosomes[i].initialiseRandomly(customers);
    }

    if(verbose) std::cout << "\t\t\t[TSP] Calculating initial population fitness..." << std::endl;
    for (i = 0; i < Npop_TSP; i++)
    {
        uint32_t currentFitness = chromosomes[i].calculateFitness();
        if (currentFitness < bestFitness)
        {
            bestFitness = currentFitness;
            i_max = i;
        }
    }
    bestChromosomeEver = chromosomes[i_max];
    bestFitnessEver = bestChromosomeEver.fitness;
    Fstar = bestChromosomeEver.fitness;
    if (verbose) std::cout << "\t\t\t[TSP] Best objective function value in the initial population is " << bestChromosomeEver.fitness << std::endl;
    if (Ncustomers <= 5)
        return bestChromosomeEver.path;

    //tmp1.path = vector<uint32_t>(Ncustomers);
    uint32_t s1, s2, s3, s4, count;
    count = 0;

    for (i = 0; i < Ngen / 2; i++)
    {
        if (verbose && i % (Ngen / 20) == 0 && i > 0)
            std::cout << "\t\t\t\t* Generation " << i * 2 << " of " << Ngen << "\t" << "Fstar = " << bestFitnessEver << std::endl;
        for (j = 0; j < Npop_TSP; j++)
        {            
            //memset(visited, 0, CustomerMax * sizeof(bool));
            //s1 = dis(_gen); s2 = dis(_gen); s3 = dis(_gen); s4 = dis(_gen);
            s1 = dis(_gen); s2 = dis(_gen); s3 = dis(_gen); s4 = dis(_gen);
            std::fill(tmp1.path.begin(), tmp1.path.end(), UINT32_MAX);
            crossoverSCX(chromosomes[s1].fitness < chromosomes[s2].fitness ? chromosomes[s1].path : chromosomes[s2].path,
                chromosomes[s3].fitness < chromosomes[s4].fitness ? chromosomes[s3].path : chromosomes[s4].path,
                tmp1.path);
            new_chromosomes[j] = tmp1;
            if (dis(_gen) == 0)
                mutate(new_chromosomes[j].path);
        }
        for (j = 0; j < Npop_TSP; j++)
        {
            if (new_chromosomes[j].calculateFitness() < bestFitnessEver)
            {
                bestFitnessEver = new_chromosomes[j].fitness;
                bestChromosomeEver = new_chromosomes[j];
            }
        }
        for (j = 0; j < Npop_TSP; j++)
        {
            //memset(visited, 0, CustomerMax * sizeof(bool));
            s1 = dis(_gen); s2 = dis(_gen); s3 = dis(_gen); s4 = dis(_gen);
            std::fill(tmp1.path.begin(), tmp1.path.end(), UINT32_MAX);
            crossoverSCX(new_chromosomes[s1].fitness < new_chromosomes[s2].fitness ? new_chromosomes[s1].path : new_chromosomes[s2].path,
                new_chromosomes[s3].fitness < new_chromosomes[s4].fitness ? new_chromosomes[s3].path : new_chromosomes[s4].path,
                tmp1.path);
            chromosomes[j] = tmp1;
            if (dis(_gen) == 0)
                mutate(chromosomes[j].path);
        }
        for (j = 0; j < Npop_TSP; j++)
        {
            if (chromosomes[j].calculateFitness() < bestFitnessEver)
            {
                bestFitnessEver = chromosomes[j].fitness;
                bestChromosomeEver = chromosomes[j];
            }
        }
    }


    if (verbose)
    {
        std::cout << "\t\t\t[TSP] TSPGA solver finished, Fstar=" << bestChromosomeEver.fitness << std::endl;
        std::cout << "\t\t\tBest path found: " << std::endl;
        std::cout << "\t\t\t" << 0 << "->";
        for (int i = 0; i < Ncustomers - 1; i++)
            std::cout << customersSortIndices[bestChromosomeEver.path[i]] << "->";
        std::cout << customersSortIndices[bestChromosomeEver.path[Ncustomers - 1]] << std::endl;
    }
    Fstar = bestChromosomeEver.fitness;
    return bestChromosomeEver.path;
}
