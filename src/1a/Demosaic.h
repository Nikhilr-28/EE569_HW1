#pragma once
#include <vector>
#include <cstdint>

// Bilinear demosaicing for Bayer CFA, GRBG pattern (top-left is G, then R).
// Input: 1-channel CFA (width*height)
// Output: RGB interleaved (width*height*3) as R,G,B per pixel.
std::vector<uint8_t> demosaic_bilinear_grbg(
    const std::vector<uint8_t>& cfa,
    int width,
    int height
);
