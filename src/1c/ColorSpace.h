#pragma once
#include <vector>
#include <cstdint>

namespace cs {

// Convert interleaved RGB (uint8) to YUV (float per channel).
// YUV values are stored in float vectors length N = width*height.
void rgb_to_yuv_bt601(const std::vector<uint8_t>& rgb, int width, int height,
                      std::vector<float>& Y,
                      std::vector<float>& U,
                      std::vector<float>& V);

// Convert YUV (float) back to interleaved RGB (uint8), with clipping.
std::vector<uint8_t> yuv_to_rgb_bt601(const std::vector<float>& Y,
                                      const std::vector<float>& U,
                                      const std::vector<float>& V,
                                      int width, int height);

// Helper: clamp float to [0,255] and convert to uint8.
uint8_t clamp_u8(float x);

} // namespace cs
