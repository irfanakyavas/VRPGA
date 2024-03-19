// VRPGA.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <cmath>
#ifndef _DEBUG
#include "matplot/matplot.h"
#endif
#include <iostream>
#include "Customer.h"
#include "ChromosomeBaker2003.h"

#pragma comment(linker, "/STACK:4000000000")
#pragma comment(linker, "/HEAP:400000000")

#define minv(vec) *std::min_element(vec.begin(), vec.end())
#define maxv(vec) *std::max_element(vec.begin(), vec.end())

struct VectorHash {
    size_t operator()(const std::vector<uint32_t>& v) const {
        std::hash<int> hasher;
        size_t seed = 0;
        for (int i : v) {
            seed ^= hasher(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};

#ifndef _DEBUG
using namespace matplot;

matplot::figure_handle fig_initial = matplot::figure(true);
matplot::figure_handle fig_currentBest = matplot::figure(true);

void plotChromosomeSerial(matplot::figure_handle& fh, const Chromosome& c, string title) {
    matplot::axes_handle ax_fix;
    ax_fix = fh->current_axes();
    ax_fix->clear();
    ax_fix->geolimits(minv(Customer::customerLAT) - 0.01, maxv(Customer::customerLAT) + 0.01, minv(Customer::customerLON) - 0.01, maxv(Customer::customerLON) + 0.01);
    bool first = true;
    std::vector<double> sorted_lonAll;
    std::vector<double> sorted_latAll;
    std::vector<string> sorted_CID;

    for (uint32_t i = 1; i <= c.Nvehicles; i++) {
        std::vector<double> sorted_lat;
        std::vector<double> sorted_lon;
        for (int j = c.customersVisitedByVehicle[i].size()-1; j >= 0; j--) {
            sorted_lat.emplace_back(Customer::customerLAT[customersSortIndices[c.customersVisitedByVehicle[i][j]]]);
            sorted_lon.emplace_back(Customer::customerLON[customersSortIndices[c.customersVisitedByVehicle[i][j]]]);
            sorted_lonAll.emplace_back(Customer::customerLON[customersSortIndices[c.customersVisitedByVehicle[i][j]]]);
            sorted_latAll.emplace_back(Customer::customerLAT[customersSortIndices[c.customersVisitedByVehicle[i][j]]]);
            sorted_CID.emplace_back(to_string(c.customersVisitedByVehicle[i][j]+1));
        }
        sorted_lat.emplace_back(Depot.LAT);
        sorted_lon.emplace_back(Depot.LON);
        ax_fix->geoplot(sorted_lat, sorted_lon);
        if (first) {
            ax_fix->hold(true);
            first = false;
        }
    }
    sorted_latAll.emplace_back(Depot.LAT);
    sorted_lonAll.emplace_back(Depot.LON);
    sorted_CID.push_back("0");

    ax_fix->geoscatter(sorted_latAll, sorted_lonAll);

    auto [lon_c, lat_c, names_c] =
        matplot::clear_overlapping_labels(sorted_lonAll, sorted_latAll, sorted_CID, 0.001, 0.001);
    ax_fix->text(lon_c, lat_c, names_c);

    ax_fix->title("Tour distance " + matplot::num2str(c.totalWorkTime));
    fh->title(title);
    ax_fix->hold(false);
    ax_fix->draw();
}
#endif
#include <unordered_set>

Chromosome solveVRP(uint32_t Ncustomers, uint32_t Nvehicles, uint32_t Npop, uint32_t Pmut, uint32_t Ngen, uint32_t& Fstar, bool verbose = false)
{
    if (verbose) cout << "[VRP] VRP solver initialising for " << Ncustomers << " customers and " << Nvehicles << " vehicles" << endl;
    
    std::vector<Chromosome> chromosomes(Npop, Chromosome(Ncustomers, Nvehicles));
    std::unordered_set<vector<uint32_t>, VectorHash> chromosome_genes;

    if (verbose) cout << "[VRP] Generating random initial population..." << endl;
    int n = 0;
    while(n < Npop)
    {
		chromosomes[n] = Chromosome(Ncustomers, Nvehicles);
        initialiseRandomChromosome(chromosomes[n]);
        auto r = chromosome_genes.insert(chromosomes[n].genes);
        if (r.second == true)
            n++;
	}
    Chromosome bestChromosomeEver(Ncustomers, Nvehicles);
    uint32_t bestFitnessEver = UINT_FAST32_MAX, bestFitness = UINT_FAST32_MAX;
    uint32_t i, j, i_max = 0;

    if (verbose) cout << "[VRP] Calculating initial population fitness..." << endl;
    int tmp = 1;
    for (i = 0; i < Npop; i++)
    {
        if (verbose && i % (Npop / 10) == 0 && i > 0)
        {
			cout << "\t* Fitness calculation of initial population " << tmp*10 << "%." << endl;
            tmp++;
        }
        uint32_t currentFitness = chromosomes[i].calculateFitness();
        if (currentFitness < bestFitness)
        {
            bestFitness = currentFitness;
            i_max = i;
        }
    }
    
    bestChromosomeEver = chromosomes[i_max];
    bestFitnessEver = bestChromosomeEver.totalWorkTime;
    if (verbose) cout << "[VRP] Best objective function value in the initial population is " << bestChromosomeEver.totalWorkTime << endl;

    for (int k = 0; k < 10; k++)
    {
        cout << chromosomes[k] << endl;
    }
    plotChromosomeSerial(fig_initial, bestChromosomeEver, "Initial population best");
    for (i = 0; i < Ngen; i++) {
        if (verbose && i % (Ngen / 20) == 0 && i >= 0) {
            cout << "\t* Generation " << i << " of " << Ngen << "\t" << "Fstar = " << bestFitnessEver << " (";
            for(uint32_t k = 1; k < Nvehicles; k++)
				cout << bestChromosomeEver.workTimeByVehicle[k] << ",";
			cout << bestChromosomeEver.workTimeByVehicle[Nvehicles] << ")" << endl;

            for (int k = 0; k < 10; k++)
            {
                cout << chromosomes[k] << endl;
            }
        }
        uint32_t s1, s2, s3, s4, s5, s6, s7, s8;
        for (j = 1; j < Npop; j++) {
            s1 = randomInteger(0, Npop - 1); s2 = randomInteger(0, Npop - 1); s3 = randomInteger(0, Npop - 1); s4 = randomInteger(0, Npop - 1);
            s5 = randomInteger(0, Npop - 1); s6 = randomInteger(0, Npop - 1); s7 = randomInteger(0, Npop - 1); s8 = randomInteger(0, Npop - 1);

            Chromosome tmp1(Ncustomers, Nvehicles);
            Chromosome tmp2(Ncustomers, Nvehicles);
            Chromosome tmp3(Ncustomers, Nvehicles);
            s1 = chromosomes[s1].totalWorkTime < chromosomes[s2].totalWorkTime ? s1 : s2;
            s2 = chromosomes[s3].totalWorkTime < chromosomes[s4].totalWorkTime ? s3 : s4;
            //s5 = chromosomes[s5].totalWorkTime < chromosomes[s6].totalWorkTime ? s5 : s6;
            //s6 = chromosomes[s7].totalWorkTime < chromosomes[s8].totalWorkTime ? s7 : s8;

            crossover(chromosomes[s1].genes, chromosomes[s2].genes, tmp1.genes, Nvehicles);
            //crossover(chromosomes[s5].genes, chromosomes[s6].genes, tmp2.genes, Nvehicles);
            tmp3 = tmp1;//.totalWorkTime < tmp2.totalWorkTime ? tmp1:tmp2;
            
            if (randomDouble(0,1) <= i/Ngen)
                mutate(tmp1.genes);
            
            auto r = chromosome_genes.insert(tmp1.genes);
            if (r.second == false)
            {
                j--;
                continue;
            }
            else
            {
                chromosome_genes.erase(chromosomes[j].genes);
            }
            chromosomes[j] = tmp1;

            if (chromosomes[j].calculateFitness() < bestFitnessEver) {
                bestFitnessEver = chromosomes[j].totalWorkTime;
                chromosomes[0] = chromosomes[j];
				bestChromosomeEver = chromosomes[j];
                initialiseRandomChromosome(chromosomes[j]);
                chromosome_genes.insert(chromosomes[j].genes);
#ifndef _DEBUG
                plotChromosomeSerial(fig_currentBest, bestChromosomeEver, "Current Best Solution");
#endif // !
            }
        }
        //memcpy(&chromosomes, &new_chromosomes, sizeof(chromosomes));
    }
    

    cout << "[VRP] VRP solving with GA complete, Fstar=" << bestChromosomeEver.totalWorkTime << endl;
    Fstar = bestChromosomeEver.totalWorkTime;
    return bestChromosomeEver;
}

void testTSPwithSingleChromosome() {
    Chromosome c1(customers.size(), 4);
    initialiseRandomChromosome(c1, true);

    c1.calculateFitness(true);
    //plotChromosomeSerial(fig_currentBest,c1, "");
    //matplot::show();
}

int main() {
    loadData("TCB100");
    //(std::vector<uint32_t> customers, uint32_t Nvehicles, uint32_t Npop, uint32_t Pmut, uint32_t Ngen, uint32_t & Fstar, bool verbose = false)
    uint32_t Fstar = UINT32_MAX;

    Chromosome bestChromosomeEver = solveVRP(customers.size(),12, 100, 6, 1024, Fstar, true);
#ifndef _DEBUG
    //plot(bestChromosomeEver);
#endif // !
    return 0;
}