/*
Name: Nikhil Ravichandran
ID: 8515860149
Email: nikhilr@usc.edu
Date: February 1, 2026
*/
#include "HistEq.h"
#include <vector>
#include <cstdint>
#include <cmath>
#include <algorithm>

namespace he {

static std::vector<long long> histogram_u8(const std::vector<uint8_t>& img) {
    std::vector<long long> hist(256, 0);
    for (uint8_t v : img) hist[v]++;
    return hist;
}

static std::vector<int> mapping_A(const std::vector<long long>& hist, long long N) {
    std::vector<int> map(256, 0);
    long long running = 0;
    for (int i = 0; i < 256; i++) {
        running += hist[i];
        double cdf = (N > 0) ? (double)running / (double)N : 0.0;
        int out = (int)std::lround(255.0 * cdf);
        map[i] = std::clamp(out, 0, 255);
    }
    return map;
}

std::vector<uint8_t> equalize_A(const std::vector<uint8_t>& img_u8) {
    const long long N = (long long)img_u8.size();
    if (N <= 0) return {};

    const auto hist = histogram_u8(img_u8);
    const auto map = mapping_A(hist, N);

    std::vector<uint8_t> out(img_u8.size(), 0);
    for (size_t i = 0; i < img_u8.size(); i++) {
        out[i] = (uint8_t)map[img_u8[i]];
    }
    return out;
}

// Bucket-filling method (rank-based)
std::vector<uint8_t> equalize_B(const std::vector<uint8_t>& img_u8) {
    const long long N = (long long)img_u8.size();
    if (N <= 0) return {};

    const auto hist = histogram_u8(img_u8);

    // prefix counts: starting rank for each intensity
    std::vector<long long> prefix(256, 0);
    long long running = 0;
    for (int i = 0; i < 256; i++) {
        prefix[i] = running;
        running += hist[i];
    }

    std::vector<long long> seen(256, 0);
    std::vector<uint8_t> out(img_u8.size(), 0);

    for (size_t p = 0; p < img_u8.size(); p++) {
        const int v = img_u8[p];
        const long long k = seen[v]++;        // occurrence index
        const long long rank = prefix[v] + k; // 0..N-1

        long long outv = (256LL * rank) / N;  // 0..255
        if (outv < 0) outv = 0;
        if (outv > 255) outv = 255;
        out[p] = (uint8_t)outv;
    }
    return out;
}

} // namespace he
