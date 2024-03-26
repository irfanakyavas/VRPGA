#pragma once
#include <cstdint>
#include <cstdbool>
#include <vector>
#include <algorithm>

#define Npop_TSP 10

#define T_LOOPVAR size_t
#define T_DIST uint32_t
#define T_VISITED_ARRAY uint32_t
#define T_CUSTOMERID uint32_t
#define T_FITNESS uint32_t
#define T_DEMAND T_CUSTOMERID

#define MAX_FITNESS UINT32_MAX
#define MAX_CID UINT32_MAX
#define MAX_DEMAND MAX_CID

#define minv(vec) *std::min_element(vec.begin(), vec.end())
#define maxv(vec) *std::max_element(vec.begin(), vec.end())
