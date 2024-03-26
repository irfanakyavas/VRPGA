#pragma once
#include <random>
#include <chrono>
#include <vector>

#define RANDPOOL_SAFETY_ON 0
#define N_RANDPOOL UINT32_MAX
#undef min
#undef max

class mt19937pooled {
    std::vector<uint32_t> _randpool;
    uint32_t count = 0;

public:
    std::mt19937 mt19937;

    static uint32_t min()
    {
        return 0;
    }
    static uint32_t max()
    {
        return UINT32_MAX;
    }

    mt19937pooled(uint32_t seed)
    {
        _randpool = std::vector<uint32_t>(N_RANDPOOL);
       mt19937 = std::mt19937(seed);

       for(uint64_t i = 0; i < N_RANDPOOL; i++)
	   {
           _randpool[i] = mt19937();
	   }
    }

    inline uint32_t operator() ()
    {
#if RANDPOOL_SAFETY_ON == 1
        if (RANDPOOL_PTR >= RANDPOOL_END)
            return mt19937();
        else
        {
            return _randpool[count++];
        }
#else
        return _randpool[count++];
#endif // RA

    }
};
