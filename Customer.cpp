#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include "defines.h"

struct Customer {
    T_CUSTOMERID CID = MAX_CID;
    float LAT;
    float LON;
    float angleWithDepot = 0.0;
    T_DIST distanceFromDepot = 0;
    T_DEMAND demand = 0;
    static std::vector<T_DIST> depotDistances;
    static std::vector<float> depotAngles;
    static std::vector<T_DEMAND> customerDemands;
    static std::vector<float> customerLAT;
    static std::vector<float> customerLON;
    static std::vector<T_CUSTOMERID> customerIDs;
};

std::vector<T_DIST> Customer::depotDistances;
std::vector<float> Customer::depotAngles;
std::vector<T_DEMAND> Customer::customerDemands;
std::vector<float> Customer::customerLAT;
std::vector<float> Customer::customerLON;
std::vector<T_CUSTOMERID> Customer::customerIDs;


std::vector<T_CUSTOMERID> CIDsByDepotDistances(std::vector<T_DIST> depotDistances)
{
    std::multimap<T_DIST, T_CUSTOMERID> mm;
    for (std::uint32_t i = 0; i != depotDistances.size(); ++i)
        mm.insert({ depotDistances[i], i });

    std::vector<T_DIST> sortedAngles;
    std::vector<T_CUSTOMERID> indices;
    for (const auto& kv : mm) {
        sortedAngles.push_back(kv.first);
        indices.push_back(kv.second);
    }
    return indices;
}

std::vector<T_CUSTOMERID> CIDsByAngle(std::vector<float> depotAngles)
{
    std::multimap<float, T_CUSTOMERID> mm;
    for (std::uint32_t i = 0; i != depotAngles.size(); ++i)
        mm.insert({ depotAngles[i], i });

    std::vector<float> sortedAngles;
    std::vector<T_CUSTOMERID> indices;
    for (const auto& kv : mm) {
        sortedAngles.push_back(kv.first);
        indices.push_back(kv.second);
    }
    return indices;
}