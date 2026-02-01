#pragma once
#include <vector>
#include <cstdint>

namespace filters {

uint8_t atClamp(const std::vector<uint8_t>& img, int w, int h, int x, int y);

std::vector<uint8_t> bilateralFilter(
    const std::vector<uint8_t>& img,
    int w, int h,
    int k,
    double sigma_c,
    double sigma_s
);

}
