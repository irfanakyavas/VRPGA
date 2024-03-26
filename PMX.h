#pragma once
//#pragma comment(linker, "/STACK:4000000000")
//#pragma comment(linker, "/HEAP:400000000")

#include <algorithm>
#include <execution>
#include <numeric>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <limits.h>
#include <algorithm>
#include <random>
#include <iostream>
#include <random>
#include <execution>
#include <cmath>
#include <list>
#include <vector>
#include "parameters.h"
#include "RandomUtils.h"

#define USE_PMX

#ifdef USE_PMX

#include <map>
#include <set>

inline void crossover(const std::vector<uint32_t>& p1, const std::vector<uint32_t>& p2, std::vector<uint32_t>& o1, const uint32_t& Nvehicles)
{
    static std::set<uint32_t> visited;
    uint32_t Ncustomers = p1.size();
    /*
    if (p2.size() != Ncustomers)
    {
        cout << "PMX: p1 and p2 have different sizes" << endl;
        exit(-1);
    }*/
    o1 = std::vector<uint32_t>(Ncustomers);

    uint32_t point1 = randomInteger(1, Ncustomers - 1);
    uint32_t point2 = point1;
    while (point1 == point2)
    {
        point2 = randomInteger(1, Ncustomers - 1);
    }
    if (point1 > point2)
    {
        uint32_t temp = point1;
        point1 = point2;
        point2 = temp;
    }
    // 5 2 8 1 7 6
    // p2 = 4   
    // p1 = 1
    // 

    if (randomInteger(1, 100) < 50)
    {
        for (uint32_t i = 0; i < point1; i++)
        {
            o1[i] = p1[i];
            visited.insert(p1[i]);
        }
        for (uint32_t i = point1; i < point2; i++)
        {
            o1[i] = p2[i];
            visited.insert(p2[i]);
        }
        for (uint32_t i = point2; i < p1.size(); i++)
        {
            o1[i] = p1[i];
            visited.insert(p1[i]);
        }
    }
    else
    {
        for (uint32_t i = 0; i < point1; i++)
        {
            o1[i] = p2[i];
            visited.insert(p2[i]);
        }
        for (uint32_t i = point1; i < point2; i++)
        {
            o1[i] = p1[i];
            visited.insert(p1[i]);
        }
        for (uint32_t i = point2; i < p1.size(); i++)
        {
            o1[i] = p2[i];
            visited.insert(p2[i]);
        }
    }

    if (visited.size() != Nvehicles)
    {
        for (uint32_t i = 0; i < p1.size(); i++)
            if (randomInteger(1, 100) < 50)
                o1[i] = p1[i];
            else
                o1[i] = p2[i];
    }
    visited.clear();
}

inline void crossoverFull(const std::vector<uint32_t>& p1, const std::vector<uint32_t>& p2, std::vector<uint32_t>& o1, std::vector<uint32_t>& o2)
{
    uint32_t Ncustomers = p1.size();
    if (p2.size() != Ncustomers)
    {
        std::cout << "PMX: p1 and p2 have different sizes" << std::endl;
        exit(-1);
    }
    o1 = std::vector<uint32_t>(Ncustomers);
    o2 = std::vector<uint32_t>(Ncustomers);

    uint32_t point1 = randomInteger(1, Ncustomers - 3);
    uint32_t point2 = point1;
    while (point1 == point2)
    {
        point2 = randomInteger(1, Ncustomers - 2);
    }
    if (point1 > point2)
    {
        uint32_t temp = point1;
        point1 = point2;
        point2 = temp;
    }
    // 5 2 8 1 7 6
    // p2 = 4   
    // p1 = 1
    // 
    std::vector<uint32_t> temp_section(point2 - point1 + 1);

    for (uint32_t i = 0; i < point1; i++)
    {
        o1[i] = p1[i];
        for (uint32_t i = point1; i < point2; i++)
            o1[i] = p2[i];
        for (uint32_t i = point2; i < p1.size(); i++)
            o1[i] = p1[i];
        for (uint32_t i = 0; i < point1; i++)
            o2[i] = p2[i];
        for (uint32_t i = point1; i < point2; i++)
            o2[i] = p1[i];
        for (uint32_t i = point2; i < p1.size(); i++)
            o2[i] = p2[i];
    }
}
#include<unordered_set>
#define minv(vec) *std::min_element(vec.begin(), vec.end())
#define maxv(vec) *std::max_element(vec.begin(), vec.end())
#define vcontains(vec, x) std::find(vec.begin(), vec.end(), x) != vec.end()

void crossoverSCX(const std::vector<uint32_t>& p1, const std::vector<uint32_t>& p2, std::vector<uint32_t>& o1) {
    static std::vector<std::vector<double>> dists = distanceMatrixSorted;
    static std::vector<double> depotDists = depotDistancesSorted;
    uint32_t Ncustomers = p1.size();
    uint32_t i1 = 1, i2 = 1 , j = 1, opt1, opt2;
    o1[0] = p1[0];
    for (; j < Ncustomers; j++)
    {
        opt1 = p1[i1++ % Ncustomers];
        if (vcontains(o1, opt1))
        {
            for(; vcontains(o1, opt1); i1++)
				opt1 = p1[i1 % Ncustomers];
        }
        if (j == Ncustomers - 1)
        {
            opt2 = p2[i2++ % Ncustomers];
            if (vcontains(o1, opt2))
            {
                for (; vcontains(o1, opt2); i2++)
                    opt2 = p2[i2 % Ncustomers];
            }
            opt1 = dists[o1[j - 1]][opt1] < dists[o1[j - 1]][opt2] ? opt1 : opt2;
        }
        o1[j] = opt1;
    }
}
 void crossoverPMX(const std::vector<uint32_t> &p1, const std::vector<uint32_t> &p2, std::vector<uint32_t> &o1)
{
    static std::mt19937 _gen(seed);
    static constexpr uint_fast32_t sizeof_visited = 100 * sizeof(uint_fast16_t);
    uint_fast16_t* visited = (uint_fast16_t*) _alloca(sizeof_visited);
    uint32_t left, k_, p, right;
    uint32_t Ncustomers = p2.size();
    std::uniform_int_distribution<uint32_t> dis(1, Ncustomers - 1);

    memset(visited, 0, sizeof_visited);

    left = dis(_gen);
    right = dis(_gen);
    while (left == right)
        right = dis(_gen);

    if (left > right)
        std::swap(left, right);

    std::copy(p1.begin() + left, p1.begin() + right + 1, o1.begin() + left);
    for (k_ = left; k_ <= right; k_++)
        visited[p1[k_]] = 1;

    right++;
    k_ = right < Ncustomers ? right : right - Ncustomers;
    p = k_;
    /*
    for (p = 0; p < Ncustomers; p++)
    {
        if (visited[p2[p]] == 0)
        {
            visited[p2[p]] = 1;
            o1[k_++] = p2[p];
            k_ = k_ == Ncustomers ? 0 : k_;
        }
    }*/
    
    while (k_ != left)
    {
        //std::cout << p2[p] << std::endl;
        if (visited[p2[p]] == 0)
        {
            visited[p2[p]] = 1;
            o1[k_++] = p2[p];
            if(k_ == Ncustomers)
                k_ = 0;
            
        }
        p++;
        if (p == Ncustomers)
            p = 0;
    }
}
#endif