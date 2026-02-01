#pragma once
#include <vector>
#include <cstdint>

namespace filters {

// border: clamp-to-edge (simple and stable)
uint8_t atClamp(const std::vector<uint8_t>& img, int w, int h, int x, int y);

// generic convolution with a square kernel of size k x k
std::vector<uint8_t> convolve(
    const std::vector<uint8_t>& img, int w, int h,
    const std::vector<float>& kernel, int k);

// kernels
std::vector<float> boxKernel(int k);
std::vector<float> gaussianKernel(int k, float sigma);

}
