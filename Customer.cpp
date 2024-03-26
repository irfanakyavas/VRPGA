#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>

struct Customer {
    uint32_t CID = UINT32_MAX;
    double LAT;
    double LON;
    double angleWithDepot = 0.0;
    uint32_t distanceFromDepot = 0;
    uint32_t demand = 0;
    bool isDepot = false;
    static std::vector<uint32_t> depotDistances;
    static std::vector<double> depotAngles;
    static std::vector<uint32_t> customerDemands;
    static std::vector<double> customerLAT;
    static std::vector<double> customerLON;
    static std::vector<uint32_t> customerIDs;
};
std::vector<uint32_t> Customer::depotDistances;
std::vector<double> Customer::depotAngles;
std::vector<uint32_t> Customer::customerDemands;
std::vector<double> Customer::customerLAT;
std::vector<double> Customer::customerLON;
std::vector<uint32_t> Customer::customerIDs;


std::vector<uint32_t> CIDsByDepotDistances(std::vector<uint32_t> depotDistances)
{
    std::multimap<uint32_t, uint32_t> mm;
    for (std::uint32_t i = 0; i != depotDistances.size(); ++i)
        mm.insert({ depotDistances[i], i });

    std::vector<uint32_t> sortedAngles;
    std::vector<std::uint32_t> indices;
    for (const auto& kv : mm) {
        sortedAngles.push_back(kv.first);
        indices.push_back(kv.second);
    }
    return indices;
}

std::vector<uint32_t> CIDsByAngle(std::vector<double> depotAngles)
{
    std::multimap<double, std::uint32_t> mm;
    for (std::uint32_t i = 0; i != depotAngles.size(); ++i)
        mm.insert({ depotAngles[i], i });

    std::vector<double> sortedAngles;
    std::vector<std::uint32_t> indices;
    for (const auto& kv : mm) {
        sortedAngles.push_back(kv.first);
        indices.push_back(kv.second);
    }
    return indices;
}