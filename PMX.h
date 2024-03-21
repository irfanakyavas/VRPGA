#pragma once
#pragma comment(linker, "/STACK:4000000000")
#pragma comment(linker, "/HEAP:400000000")

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
using namespace std;

#include <map>
#include <set>

inline void crossover(const vector<uint32_t>& p1, const vector<uint32_t>& p2, vector<uint32_t>& o1, const uint32_t& Nvehicles)
{
    static set<uint32_t> visited;
    uint32_t Ncustomers = p1.size();
    /*
    if (p2.size() != Ncustomers)
    {
        cout << "PMX: p1 and p2 have different sizes" << endl;
        exit(-1);
    }*/
    o1 = vector<uint32_t>(Ncustomers);

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
}

inline void crossoverFull(const vector<uint32_t>& p1, const vector<uint32_t>& p2, vector<uint32_t>& o1, vector<uint32_t>& o2)
{
    uint32_t Ncustomers = p1.size();
    if (p2.size() != Ncustomers)
    {
        cout << "PMX: p1 and p2 have different sizes" << endl;
        exit(-1);
    }
    o1 = vector<uint32_t>(Ncustomers);
    o2 = vector<uint32_t>(Ncustomers);

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
    vector<uint32_t> temp_section(point2 - point1 + 1);

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

#define PMX_SECTION_WIDTH p1.size()/2

 void crossoverPMX(const std::vector<uint32_t> &p1, const std::vector<uint32_t> &p2, std::vector<uint32_t> &o1, bool visited[])
{
     if(p1.size() != p2.size())
         cout << "CATASTROPHIC ERROR: p1.size() != p2.size()" << endl;
     uint32_t Ncustomers = p2.size();
    //o1 = std::vector<uint32_t>(p1.size());

    if (Ncustomers <= 3)
    {
        if (randomInteger(1, 100) < 50)
        {
            for (uint32_t i = 0; i < Ncustomers; i++)
                o1[i] = p1[i];
        }
        else
        {
            for (uint32_t i = 0; i < Ncustomers; i++)
                o1[i] = p2[i];
        }
        return;
    }


    uint32_t left = randomInteger(1,  Ncustomers - 1);
    uint32_t right = randomInteger(1, Ncustomers - 1);
    while (left == right)
        right = randomInteger(1, Ncustomers - 1);
    uint32_t k_, p, temp; 

    if (left > right)
    {
        temp = left;
        left = right;
        right = temp;
    }

    // <= right?
    for (k_ = left; k_ <= right; k_++)
    {
        o1[k_] = p1[k_];
        visited[p1[k_]] = true;
    }

    k_ = (right + 1) % p1.size();
    p = k_;

    while (k_ != left)
    {
        //cout << p2[p] << endl;
        if (!visited[p2[p]])
        {
            visited[p2[p]] = true;
            o1[k_++] = p2[p];
            //k_++ % p1.size();
            if(k_ == Ncustomers)
                k_ = 0;
            
        }
        //p = (p + 1) % p1.size();
        p++;
        if (p == Ncustomers)
            p = 0;
    }
    // Copy o1tmp to o1, optimize code
    //memset(visited, 0, sizeof(visited));
}
#endif