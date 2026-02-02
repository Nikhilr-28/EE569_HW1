#pragma once
#include <vector>
#include <cstdint>
#include <array>
#include <string>

namespace awb {

struct Stats {
    double meanR = 0.0;
    double meanG = 0.0;
    double meanB = 0.0;
    double targetMean = 0.0;
    double gainR = 1.0;
    double gainG = 1.0;
    double gainB = 1.0;
    int clippedLow = 0;
    int clippedHigh = 0;
};

using Hist256 = std::array<uint64_t, 256>;

Stats compute_stats(const std::vector<uint8_t>& rgb);
void compute_histograms(const std::vector<uint8_t>& rgb, Hist256& hR, Hist256& hG, Hist256& hB);

// Applies Grey World AWB. Returns corrected image and stats.
std::vector<uint8_t> grey_world_awb(const std::vector<uint8_t>& rgb, Stats& outStats);

// Writes a single CSV with 256 rows: value,R,G,B
bool write_hist_csv(const std::string& path, const Hist256& hR, const Hist256& hG, const Hist256& hB);

} // namespace awb
