#pragma once

#include "RandomUtils.h"
#include "chromosomeTSP.h"
#include "mutationTSP.h"
#include "Customer.h"
#include "pmx.h"
#include <chrono>
#include <vector>


std::vector<uint32_t> solveTSPforVehicle(std::vector<uint32_t> customers, uint32_t Npop, uint32_t Pmut, uint32_t Ngen, uint32_t& Fstar, bool verbose = false)
{
    uint32_t Ncustomers = customers.size();
    std::vector<ChromosomeTSP> chromosomes(Npop);
    std::vector<ChromosomeTSP> new_chromosomes(Npop);
    ChromosomeTSP bestChromosomeEver(Ncustomers);
    uint32_t bestFitnessEver = UINT_FAST32_MAX, bestFitness = UINT_FAST32_MAX;
    uint32_t i, j, i_max = 0;

    if (verbose) cout << "\t\t\t[TSP] Generating random initial population..." << endl;
    for (i = 0; i < Npop; i++)
    {
        chromosomes[i].initialiseRandomly(customers);
    }

    if(verbose) cout << "\t\t\t[TSP] Calculating initial population fitness..." << endl;
    for (i = 0; i < Npop; i++)
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
    if (verbose) cout << "\t\t\t[TSP] Best objective function value in the initial population is " << bestChromosomeEver.fitness << endl;
     
    ChromosomeTSP tmp1;
    uint32_t s1, s2, s3, s4;

    for (i = 0; i < Ngen / 2; i++)
    {
        if (verbose && i % (Ngen / 20) == 0 && i > 0)
            cout << "\t\t\t\t* Generation " << i * 2 << " of " << Ngen << "\t" << "Fstar = " << bestFitnessEver << endl;
        for (j = 0; j < Npop; j++)
        {
            s1 = randomInteger(0, Npop - 1); s2 = randomInteger(0, Npop - 1); s3 = randomInteger(0, Npop - 1); s4 = randomInteger(0, Npop - 1);

            crossoverPMX(chromosomes[s1].fitness < chromosomes[s2].fitness ? chromosomes[s1].path : chromosomes[s2].path,
                chromosomes[s3].fitness < chromosomes[s4].fitness ? chromosomes[s3].path : chromosomes[s4].path,
                tmp1.path);

            new_chromosomes[j] = tmp1;

            if (randomInteger(1, 100) < Pmut)
                mutate(new_chromosomes[j].path);

            if (new_chromosomes[j].calculateFitness() < bestFitnessEver)
            {
                bestFitnessEver = new_chromosomes[j].fitness;
                bestChromosomeEver = new_chromosomes[j];
            }
        }
        for (j = 0; j < Npop; j++)
        {
            s1 = randomInteger(0, Npop - 1); s2 = randomInteger(0, Npop - 1); s3 = randomInteger(0, Npop - 1); s4 = randomInteger(0, Npop - 1);

            crossoverPMX(new_chromosomes[s1].fitness < new_chromosomes[s2].fitness ? new_chromosomes[s1].path : new_chromosomes[s2].path,
                new_chromosomes[s3].fitness < new_chromosomes[s4].fitness ? new_chromosomes[s3].path : new_chromosomes[s4].path,
                tmp1.path);

            chromosomes[j] = tmp1;

            if (randomInteger(1, 100) < Pmut)
                mutate(chromosomes[j].path);

            if (chromosomes[j].calculateFitness() < bestFitnessEver)
            {
                bestFitnessEver = chromosomes[j].fitness;
                bestChromosomeEver = chromosomes[j];
            }
        }
    }


    if (verbose)
    {
        cout << "\t\t\t[TSP] TSPGA solver finished, Fstar=" << bestChromosomeEver.fitness << endl;
        cout << "\t\t\tBest path found: " << endl;
        cout << "\t\t\t" << 0 << "->";
        for (int i = 0; i < Ncustomers - 1; i++)
            cout << customersSortIndices[bestChromosomeEver.path[i]] << "->";
        cout << customersSortIndices[bestChromosomeEver.path[Ncustomers - 1]] << endl;
    }
    Fstar = bestChromosomeEver.fitness;
    return bestChromosomeEver.path;
}
