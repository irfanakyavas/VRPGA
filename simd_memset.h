#pragma once
#include <cstddef>

static int __memset2(unsigned char* ptr, unsigned char v, std::size_t len) {
    for (std::size_t i = 0; i < len; ++i) {
        ptr[i] = v;
    }

    return 0;
}

#include <immintrin.h>

static int memset2(void* ptr, unsigned char v, std::size_t len) {
    unsigned char* p = (unsigned char*)ptr;
    if (len < 64) {
        __memset2(p, v, len);
        return 0;
    }

    const std::size_t a = 0x1f & (0x20 - (0x1f & ((std::size_t)ptr)));
    const std::size_t b = 0x1f & (len - a);
    const std::size_t c = len - a - b;

    __memset2(&p[0], v, a);
    __memset2(&p[c + a], v, b);

    if (c == 0) {
        return 0;
    }

    __m256i v_vec = _mm256_set1_epi8(v);
    p += a;

    for (std::size_t i = 0; i < c; i += 32) {
        _mm256_store_si256((__m256i*)p, v_vec);
        p += 32;
    }

    return 0;
}