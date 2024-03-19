#pragma once
#include <algorithm>
#include <execution>
#include <numeric>
#include <vector>
#include "RandomUtils.h"


size_t c_gcd(size_t a, size_t b) { return b == 0 ? a : c_gcd(b, a % b); };

inline void rotate_array(std::vector<uint32_t> arr, int n, int shift) {
    int i, j, k;
    uint32_t tmp;
    if (n <= 1 || shift == 0) return;
    shift = shift % n; // make sure shift isn't >n
    int gcd = c_gcd(n, shift);

    for (i = 0; i < gcd; i++) {
        // start cycle at i
        tmp = arr[i];
        for (j = i; 1; j = k) {
            k = j + shift;
            if (k >= n) k -= n; // wrap around if we go outside array
            if (k == i) break; // end of cycle
            arr[j] = arr[k];
        }
        arr[j] = tmp;
    }
}

#ifndef USE_NEW_MUTATE
inline void mutate(std::vector<uint32_t> &p1)
{
    uint32_t Ncustomers = p1.size();
    uint32_t l_mut, r_mut, stemp;

    l_mut = randomInteger(0, Ncustomers-1);
    r_mut = randomInteger(0, Ncustomers-1);

    stemp = p1[l_mut];
    p1[l_mut] = p1[r_mut];
    p1[r_mut] = stemp;
}
#endif

#ifdef USE_NEW_MUTATE
inline void mutate(vector<uint32_t> p1)
{
    uint32_t Ncustomers = p1.size();
    uint32_t l_mut, r_mut, N_opt, stemp;
    l_mut = 0;
    r_mut = 0;

    while (l_mut == r_mut)
	{
		l_mut = randomInteger(0, Ncustomers);
		r_mut = randomInteger(0, Ncustomers);
	}

    if (l_mut > r_mut)
    {
        stemp = l_mut;
        l_mut = r_mut;
        r_mut = stemp;
    }

    N_opt = r_mut - l_mut;

    rotate_array(&p1[l_mut], N_opt, 1);
}
#endif
