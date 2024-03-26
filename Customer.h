#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
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

std::vector<std::vector<T_DIST>> distanceMatrix;
std::vector<std::vector<T_DIST>> distanceMatrixSorted;
std::vector<T_DIST> depotDistancesSorted;
std::vector<Customer> customers;
std::vector<T_CUSTOMERID> customersSortIndices;
std::vector<T_CUSTOMERID> CIDsByAngle(std::vector<float> depotAngles);
std::vector<T_CUSTOMERID> CIDsByDepotDistances(std::vector<T_DIST> depotDistances);
Customer Depot;

inline bool operator< (const Customer& lhs, const Customer& rhs) {
    return lhs.angleWithDepot < rhs.angleWithDepot;
}

inline bool operator==(const Customer& lhs, const Customer& rhs) {
    return lhs.angleWithDepot == rhs.angleWithDepot;
}

std::vector<std::string> customerIDsSTR;

std::ostream& operator<<(std::ostream& Str, Customer const& v) {
    Str << "CID: " << v.CID << "\tLAT:" << v.LAT << "\tLON:" << v.LON << "\tANG:" << v.angleWithDepot << "\tDIS:" << v.distanceFromDepot << "\tDEMAND:" << v.demand << std::endl;
    return Str;
}

inline float angleWithDepot(float lat, float lon)
{
    return atan2(lat - Depot.LAT, lon - Depot.LON);
}

void loadCustomersFromCSV(const std::string& filename) {    
    std::ifstream file(filename);
    if (file) {
        std::string line;
        std::getline(file, line);
        bool first = true;

        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string token;
            std::getline(iss, token, ','); // Read PointID
            T_CUSTOMERID pointID = std::stoi(token);
            std::getline(iss, token, ','); // Read LAT
            float latitude = std::stod(token);
            std::getline(iss, token, ','); // Read LON
            float longitude = std::stod(token);
            float angleWithDepot = 0.0;
            if (first)
            {
                Depot = Customer{ pointID, latitude, longitude, angleWithDepot, 0, 0 };
				first = false;
                continue;
            }
            angleWithDepot = atan2(latitude - Depot.LAT, longitude - Depot.LON);
            Customer::depotAngles.push_back(angleWithDepot);
            customers.push_back({ pointID, latitude, longitude, angleWithDepot, 0, 0});
            Customer::customerLAT.push_back(latitude);
			Customer::customerLON.push_back(longitude);
            Customer::customerIDs.push_back(pointID);
			customerIDsSTR.push_back(std::to_string(pointID));
        }
    }
    else {
        std::cerr << "Error opening file: " << filename << std::endl;
    }
}

void loadDemandFromCSV(const std::string& filename, std::vector<Customer>& customers) {
    std::ifstream file(filename);
    if (file) {
        std::string line;
        std::getline(file, line);
        int i = 0;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string token;

            std::getline(iss, token, ',');
            T_CUSTOMERID pointID = std::stoi(token);
            std::getline(iss, token, ',');
            T_DEMAND demand = std::stoi(token);
            customers[i].demand = demand;
            Customer::customerDemands.push_back(demand);
			i++;
        }
    }
    else {
        std::cerr << "Error opening file: " << filename << std::endl;
    }
}

std::vector<std::vector<T_DIST>> loadDistanceMatrixFromTSV(const std::string& filename, std::vector<Customer> customers) {
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;

        // trash the customer ID line
        std::getline(file, line);

        // status variable for the
        bool currentLineIsDepotDistances = true;
        while (std::getline(file, line)) {
            std::vector<T_DIST> row;
            uint32_t value;

            // Create a stringstream from the line
            std::stringstream ss(line);

            if (currentLineIsDepotDistances)
            {
                currentLineIsDepotDistances = false;
                ss >> value;
                ss.ignore();
                ss >> value;
                ss.ignore();
                int i = 0;
                while (ss >> value)
                {
                    customers[i].distanceFromDepot = (T_DIST)(value / 60000.0);
                    Customer::depotDistances.push_back(customers[i++].distanceFromDepot);
                    ss.ignore();
                }
				continue;
            }
            else
            {
                // Read each integer value from the line
                ss >> value;
                ss.ignore();
                ss >> value;
                ss.ignore();
                while (ss >> value) {
                    row.push_back((T_DIST) (value / 60000.0));
                    // Ignore the tab character
                    ss.ignore();
                }
                distanceMatrix.push_back(row);
            }
        }
        file.close();
    }
    else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }

    return distanceMatrix;
}

void loadData(std::string caseStr, bool verbose = false)
{
    loadCustomersFromCSV("C:\\bitirme_data\\New_T\\"+caseStr+"\\points.csv");
    loadDemandFromCSV("C:\\bitirme_data\\New_T\\" + caseStr + "\\Npackages.txt", customers);
    loadDistanceMatrixFromTSV("C:\\bitirme_data\\New_T\\" + caseStr + "\\Mdist.tsv", customers);

    customersSortIndices = CIDsByAngle(Customer::depotAngles);

    for(uint32_t i = 0; i < customers.size(); i++)
    {
        depotDistancesSorted.push_back(Customer::depotDistances[customersSortIndices[i]]);
	}

    distanceMatrixSorted = std::vector<std::vector<T_DIST>>();
    for (uint32_t i = 0; i < customers.size(); i++)
    {
        std::vector<T_DIST> row(customers.size());
        for (uint32_t j = 0; j < customers.size(); j++)
            row[j] = distanceMatrix[customersSortIndices[i]][customersSortIndices[j]];
        distanceMatrixSorted.push_back(row);
    }
    if (verbose) for (const Customer& c : customers) std::cout << c << std::endl;
    std::cout << "[VRP] Data loaded for case " << caseStr << std::endl;
}