// VRPGA.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <cmath>
#ifndef _DEBUG
#include "matplot/matplot.h"
#endif
#include <iostream>
#include "Customer.h"
#include "ChromosomeBaker2003.h"
#include <chrono>
#define minv(vec) *std::min_element(vec.begin(), vec.end())
#define maxv(vec) *std::max_element(vec.begin(), vec.end())

#ifndef _DEBUG
using namespace matplot;

matplot::figure_handle fig_initial = matplot::figure(true);
matplot::figure_handle fig_currentBest = matplot::figure(true);

void plotChromosomeCartesian(matplot::figure_handle& fh, const Chromosome& c, std::string title) {
    matplot::axes_handle ax_fix;
    ax_fix = fh->current_axes();
    ax_fix->clear();
    //ax_fix->limits(minv(Customer::customerLAT) - 0.01, maxv(Customer::customerLAT) + 0.01, minv(Customer::customerLON) - 0.01, maxv(Customer::customerLON) + 0.01);
    bool first = true;
    std::vector<double> sorted_lonAll;
    std::vector<double> sorted_latAll;
    std::vector<std::string> sorted_CID;

    for (uint32_t i = 1; i <= c.Nvehicles; i++) {
        std::vector<double> sorted_lat;
        std::vector<double> sorted_lon;
        for (int j = c.customersVisitedByVehicle[i].size() - 1; j >= 0; j--) {
            sorted_lat.emplace_back(Customer::customerLAT[customersSortIndices[c.customersVisitedByVehicle[i][j]]]);
            sorted_lon.emplace_back(Customer::customerLON[customersSortIndices[c.customersVisitedByVehicle[i][j]]]);
            sorted_lonAll.emplace_back(Customer::customerLON[customersSortIndices[c.customersVisitedByVehicle[i][j]]]);
            sorted_latAll.emplace_back(Customer::customerLAT[customersSortIndices[c.customersVisitedByVehicle[i][j]]]);
            sorted_CID.emplace_back(std::to_string(c.customersVisitedByVehicle[i][j] + 1));
        }
        sorted_lat.emplace_back(Depot.LAT);
        sorted_lon.emplace_back(Depot.LON);

        ax_fix->plot(sorted_lat, sorted_lon);
        if (first) {
            ax_fix->hold(true);
            first = false;
        }
    }
    sorted_latAll.emplace_back(Depot.LAT);
    sorted_lonAll.emplace_back(Depot.LON);
    sorted_CID.push_back("0");

    ax_fix->scatter(sorted_latAll, sorted_lonAll);

    auto [lon_c, lat_c, names_c] =
        matplot::clear_overlapping_labels(sorted_lonAll, sorted_latAll, sorted_CID, 5, 5);
    ax_fix->text(lon_c, lat_c, names_c);

    ax_fix->title("Tour distance " + matplot::num2str(c.totalWorkTime));
    fh->title(title);
    ax_fix->hold(false);
    ax_fix->draw();
}

void plotChromosomeWorldmap(matplot::figure_handle& fh, const Chromosome& c, std::string title) {
    matplot::axes_handle ax_fix;
    ax_fix = fh->current_axes();
    ax_fix->clear();
    ax_fix->geolimits(minv(Customer::customerLAT) - 0.01, maxv(Customer::customerLAT) + 0.01, minv(Customer::customerLON) - 0.01, maxv(Customer::customerLON) + 0.01);
    bool first = true;
    std::vector<double> sorted_lonAll;
    std::vector<double> sorted_latAll;
    std::vector<std::string> sorted_CID;

    for (uint32_t i = 1; i <= c.Nvehicles; i++) {
        std::vector<double> sorted_lat;
        std::vector<double> sorted_lon;
        for (int j = c.customersVisitedByVehicle[i].size()-1; j >= 0; j--) {
            sorted_lat.emplace_back(Customer::customerLAT[customersSortIndices[c.customersVisitedByVehicle[i][j]]]);
            sorted_lon.emplace_back(Customer::customerLON[customersSortIndices[c.customersVisitedByVehicle[i][j]]]);
            sorted_lonAll.emplace_back(Customer::customerLON[customersSortIndices[c.customersVisitedByVehicle[i][j]]]);
            sorted_latAll.emplace_back(Customer::customerLAT[customersSortIndices[c.customersVisitedByVehicle[i][j]]]);
            sorted_CID.emplace_back(std::to_string(c.customersVisitedByVehicle[i][j]+1));
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
#include <format> 

Chromosome solveVRP(T_CUSTOMERID &Ncustomers, T_CUSTOMERID &Nvehicles, std::vector<Chromosome> &initialPopulation, uint32_t Npop, const uint32_t &Pmut,const uint32_t &Ngen, T_FITNESS& Fstar, bool verbose = false)
{
    uint32_t j = 1, s1, s2, s3, s4;
    Fstar = MAX_FITNESS;
    std::vector<Chromosome> chromosomes;
    if (verbose) std::cout << "[VRP] VRP solver initialising for " << Ncustomers << " customers and " << Nvehicles << " vehicles" << std::endl;
    if (initialPopulation.size() <= 0)
    {
        chromosomes = std::vector<Chromosome>(Npop, Chromosome(Ncustomers, Nvehicles));
        if (verbose) std::cout << "[VRP] Generating random initial population..." << std::endl;
        for (uint32_t i = 0; i < Npop; i++)
            initialiseRandomChromosome(chromosomes[i], false);

        if (verbose) std::cout << "[VRP] Calculating initial population fitness..." << std::endl;

        for (uint32_t i = 0; i < Npop; i++)
        {
            if (verbose && i % (Npop / 10) == 0 && i > 0)
            {
                std::cout << "\t* Fitness calculation of initial population " << j * 10 << "%." << std::endl;
                j++;
            }
            chromosomes[0] = (chromosomes[i].calculateFitness() < chromosomes[0].totalWorkTime) ? chromosomes[i] : chromosomes[0];
        }
    }
    else
    {
        Npop = (uint32_t)initialPopulation.size();
        if (verbose) std::cout << "[VRP] Using provided initial population, ignoring Npop..." << std::endl;
        chromosomes = initialPopulation;
    }
    Chromosome tmp1(Ncustomers, Nvehicles);
   
    if (verbose) std::cout << "[VRP] Initial population best   F*=" << chromosomes[0].totalWorkTime << "    U*=" << chromosomes[0].unfitness << std::endl;
    std::chrono::milliseconds ms = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
    std::chrono::milliseconds total = std::chrono::milliseconds(0);
    auto now = ms;
    auto milestoneGenCount = (Ngen / 20);
#ifndef _DEBUG
   plotChromosomeWorldmap(fig_initial, chromosomes[0], "Initial population best");
#endif // !
    for (uint32_t i = 0; i < Ngen; i++) {
        if (verbose && i % milestoneGenCount == 0 && i > 0) {
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()) - now;
            now = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch());
            total += diff;
            std::cout << std::format("\t*[{0:6d}/{1:6d}]\tF*={2:<6d} U*={3:1.4f}\t{4:2.4f} s/Gen\tT={5:6.2f} s", i, Ngen, chromosomes[0].totalWorkTime, chromosomes[0].unfitness, ((float)diff.count()) / 1000.0 / milestoneGenCount, total.count() / 1000.0) << std::endl;
            //std::cout << "\t* Generation " << i << " of " << Ngen << "\t" << "Fstar= " << chromosomes[0].totalWorkTime << " unfitness= " << chromosomes[0].unfitness << "\t" << ((double)diff.count())/1000.0/milestoneGenCount << " Gen/s " << "Total time= " << total.count()/1000.0 << "s" << std::endl;// << " (";
        }
        for (j = 1; j < Npop; j++) {
            s1 = randomInteger(0, Npop - 1); s2 = randomInteger(1, Npop - 1); s3 = randomInteger(1, Npop - 1); s4 = randomInteger(0, Npop - 1);

            crossover(chromosomes[s1].totalWorkTime < chromosomes[s2].totalWorkTime ? chromosomes[s1].genes : chromosomes[s2].genes,
                      chromosomes[s3].totalWorkTime < chromosomes[s4].totalWorkTime ? chromosomes[s3].genes : chromosomes[s4].genes, tmp1.genes, Nvehicles);

            if (randomInteger(0, Npop - 1) == 0)
                mutate(tmp1.genes);

            chromosomes[j] = tmp1;

            if (chromosomes[j].calculateFitness() < chromosomes[0].totalWorkTime) {
                chromosomes[0] = chromosomes[j];
                initialiseRandomChromosome(chromosomes[j]);
#ifndef _DEBUG
                plotChromosomeWorldmap(fig_currentBest, chromosomes[0], "Current Best Solution");
#endif // !
            }
        }
    }
    

    std::cout << "[VRP] VRP solving with GA complete, Fstar=" << chromosomes[0].totalWorkTime << std::endl;
    Fstar = chromosomes[0].totalWorkTime;
    return chromosomes[0];
}

void testTSPwithSingleChromosome() {
    //Chromosome c1(customers.size(), 4);
    //initialiseRandomChromosome(c1, true);
    //c1.calculateFitness(true);
    //plotChromosomeWorldmap(fig_currentBest,c1, "");
    //matplot::show();
}


#include <chrono>
#include "RandomUtils.h"
int main() {
    loadData("TCB100");
    T_CUSTOMERID Ncustomers = customers.size();
    T_CUSTOMERID Nvehicles = 10;
    uint32_t Npop1 = 72;
    uint32_t Npop2 = 256;
    uint32_t Pmut = 8;
    uint32_t Ngen1 = 256;
    uint32_t Ngen2 = 2048;

    T_FITNESS Fstar = 0;
    std::vector<Chromosome> initialPopulation;
    std::vector<Chromosome> _initialPopulation(Npop2, Chromosome(Ncustomers, Nvehicles));
    Chromosome bestChromosomeEver(Ncustomers, Nvehicles);
    for (uint32_t i = 0; i < Npop2; i++)
    {
        bestChromosomeEver = solveVRP(Ncustomers, Nvehicles, initialPopulation, Npop1, Pmut, Ngen1, Fstar, false);
        _initialPopulation[i] = bestChromosomeEver;
        std::cout << "Pre-solve [" << i+1 << "/" << Npop2 << "]" << std::endl;
#ifndef _DEBUG
        //plotChromosomeWorldmap(fig_currentBest, bestChromosomeEver, "Best Solution");
#endif // !
    }

    bestChromosomeEver = solveVRP(Ncustomers, Nvehicles, _initialPopulation, Npop2, Pmut, Ngen2, Fstar, true);

#ifndef _DEBUG
    plotChromosomeWorldmap(fig_currentBest, bestChromosomeEver, "Best Solution");

    matplot::show();
#endif // !
    return 0;
}