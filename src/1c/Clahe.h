#pragma once
#include <vector>
#include <cstdint>

namespace clahe {

// Apply CLAHE on an 8-bit grayscale image using OpenCV.
// tile_x, tile_y: number of tiles in x and y (e.g., 8x8).
// clip_limit: typical values 2.0 to 4.0 (tune).
std::vector<uint8_t> apply(const std::vector<uint8_t>& img_u8,
                           int width, int height,
                           int tile_x, int tile_y,
                           double clip_limit);

} // namespace clahe
