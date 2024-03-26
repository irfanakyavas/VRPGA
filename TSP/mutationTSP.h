#pragma once
#include <algorithm>
#include <execution>
#include <numeric>
#include <vector>
#include "RandomUtils.h"
#include "defines.h"

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

inline void mutate(std::vector<T_CUSTOMERID> &p1)
{
    std::swap(p1[randomInteger(0, p1.size() -1)], p1[randomInteger(0, p1.size() -1)]);
}