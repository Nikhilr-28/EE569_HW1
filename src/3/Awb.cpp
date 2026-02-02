#include "Awb.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>

namespace awb {

static inline uint8_t clip_u8(int v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return static_cast<uint8_t>(v);
}

Stats compute_stats(const std::vector<uint8_t>& rgb) {
    Stats s{};
    if (rgb.empty() || (rgb.size() % 3 != 0)) return s;

    const size_t N = rgb.size() / 3;
    long double sumR = 0.0L, sumG = 0.0L, sumB = 0.0L;

    for (size_t i = 0; i < rgb.size(); i += 3) {
        sumR += rgb[i + 0];
        sumG += rgb[i + 1];
        sumB += rgb[i + 2];
    }

    s.meanR = static_cast<double>(sumR / static_cast<long double>(N));
    s.meanG = static_cast<double>(sumG / static_cast<long double>(N));
    s.meanB = static_cast<double>(sumB / static_cast<long double>(N));
    s.targetMean = (s.meanR + s.meanG + s.meanB) / 3.0;

    // gains
    s.gainR = (s.meanR != 0.0) ? (s.targetMean / s.meanR) : 1.0;
    s.gainG = (s.meanG != 0.0) ? (s.targetMean / s.meanG) : 1.0;
    s.gainB = (s.meanB != 0.0) ? (s.targetMean / s.meanB) : 1.0;

    return s;
}

void compute_histograms(const std::vector<uint8_t>& rgb, Hist256& hR, Hist256& hG, Hist256& hB) {
    hR.fill(0); hG.fill(0); hB.fill(0);
    if (rgb.empty() || (rgb.size() % 3 != 0)) return;

    for (size_t i = 0; i < rgb.size(); i += 3) {
        hR[rgb[i + 0]]++;
        hG[rgb[i + 1]]++;
        hB[rgb[i + 2]]++;
    }
}

std::vector<uint8_t> grey_world_awb(const std::vector<uint8_t>& rgb, Stats& outStats) {
    outStats = compute_stats(rgb);

    std::vector<uint8_t> out(rgb.size(), 0);
    int clipLow = 0, clipHigh = 0;

    for (size_t i = 0; i < rgb.size(); i += 3) {
        const int r = static_cast<int>(std::lround(outStats.gainR * rgb[i + 0]));
        const int g = static_cast<int>(std::lround(outStats.gainG * rgb[i + 1]));
        const int b = static_cast<int>(std::lround(outStats.gainB * rgb[i + 2]));

        if (r < 0 || g < 0 || b < 0) clipLow++;
        if (r > 255 || g > 255 || b > 255) clipHigh++;

        out[i + 0] = clip_u8(r);
        out[i + 1] = clip_u8(g);
        out[i + 2] = clip_u8(b);
    }

    outStats.clippedLow = clipLow;
    outStats.clippedHigh = clipHigh;
    return out;
}

bool write_hist_csv(const std::string& path, const Hist256& hR, const Hist256& hG, const Hist256& hB) {
    std::ofstream ofs(path);
    if (!ofs) {
        std::cerr << "Error: cannot write histogram csv: " << path << "\n";
        return false;
    }
    ofs << "value,R,G,B\n";
    for (int v = 0; v <= 255; v++) {
        ofs << v << "," << hR[v] << "," << hG[v] << "," << hB[v] << "\n";
    }
    return true;
}

} // namespace awb
