#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

struct Customer {
    uint32_t CID = UINT32_MAX;
    double LAT;
    double LON;
    double angleWithDepot = 0.0;
    double distanceFromDepot = 0;
    uint32_t demand = 0;
    static std::vector<double> depotDistances;
    static std::vector<double> depotAngles;
    static std::vector<uint32_t> customerDemands;
    static std::vector<double> customerLAT;
    static std::vector<double> customerLON;
	static std::vector<uint32_t> customerIDs;
};

std::vector<std::vector<double>> distanceMatrix;
std::vector<std::vector<double>> distanceMatrixSorted;
std::vector<double> depotDistancesSorted;
std::vector<Customer> customers;
std::vector<uint32_t> customersSortIndices;
std::vector<uint32_t> CIDsByAngle(std::vector<double> depotAngles);
std::vector<uint32_t> CIDsByDepotDistances(std::vector<uint32_t> depotDistances);
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

inline double angleWithDepot(double lat, double lon)
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
            uint32_t pointID = std::stoi(token);
            std::getline(iss, token, ','); // Read LAT
            double latitude = std::stod(token);
            std::getline(iss, token, ','); // Read LON
            double longitude = std::stod(token);
            double angleWithDepot = 0.0;
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
            uint32_t pointID = std::stoi(token);
            std::getline(iss, token, ',');
            uint32_t demand = std::stoi(token);
            customers[i].demand = demand;
            Customer::customerDemands.push_back(demand);
			i++;
        }
    }
    else {
        std::cerr << "Error opening file: " << filename << std::endl;
    }
}

std::vector<std::vector<double>> loadDistanceMatrixFromTSV(const std::string& filename, std::vector<Customer> customers) {
    std::ifstream file(filename);

    if (file.is_open()) {
        std::string line;

        // trash the customer ID line
        std::getline(file, line);

        // status variable for the
        bool currentLineIsDepotDistances = true;
        while (std::getline(file, line)) {
            std::vector<double> row;
            double value;

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
                    //customers[i].distanceFromDepot = value / 60000;
                    customers[i].distanceFromDepot = value;
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
                    //row.push_back(value / 60000);
                    row.push_back(value);
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
    //loadCustomersFromCSV("C:\\bitirme_data\\New_T\\"+caseStr+"\\points.csv");
    //loadDemandFromCSV("C:\\bitirme_data\\New_T\\" + caseStr + "\\Npackages.txt", customers);
    //loadDistanceMatrixFromTSV("C:\\bitirme_data\\New_T\\" + caseStr + "\\Mdist.tsv", customers);
    loadCustomersFromCSV("C:\\bitirme_data\\New_D\\" + caseStr + "\\points.csv");
    loadDemandFromCSV("C:\\bitirme_data\\New_D\\" + caseStr + "\\Npackages.txt", customers);
    loadDistanceMatrixFromTSV("C:\\bitirme_data\\New_D\\" + caseStr + "\\Mdist.tsv", customers);

    customersSortIndices = CIDsByAngle(Customer::depotAngles);

    for(uint32_t i = 0; i < customers.size(); i++)
    {
        depotDistancesSorted.push_back(Customer::depotDistances[customersSortIndices[i]]);
	}

    distanceMatrixSorted = std::vector<std::vector<double>>();
    for (uint32_t i = 0; i < customers.size(); i++)
    {
        std::vector<double> row(customers.size());
        for (uint32_t j = 0; j < customers.size(); j++)
            row[j] = distanceMatrix[customersSortIndices[i]][customersSortIndices[j]];
        distanceMatrixSorted.push_back(row);
    }
    if (verbose) for (const Customer& c : customers) std::cout << c << std::endl;
    std::cout << "[VRP] Data loaded for case " << caseStr << std::endl;
}

inline double Distance(uint32_t x, uint32_t y)
{
    return distanceMatrix[customersSortIndices[x]][customersSortIndices[y]];
}

inline double Distance(uint32_t x, uint32_t y, std::vector<uint32_t> indices, std::vector<std::vector<double>>& distanceMatrix)
{
    return distanceMatrix[indices[x]][indices[y]];
}