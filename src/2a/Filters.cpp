#include "Filters.h"
#include <algorithm>
#include <cmath>

namespace filters {

uint8_t atClamp(const std::vector<uint8_t>& img, int w, int h, int x, int y) {
    x = std::clamp(x, 0, w - 1);
    y = std::clamp(y, 0, h - 1);
    return img[static_cast<size_t>(y) * static_cast<size_t>(w) + static_cast<size_t>(x)];
}

std::vector<uint8_t> convolve(
    const std::vector<uint8_t>& img, int w, int h,
    const std::vector<float>& kernel, int k
) {
    const int r = k / 2;
    std::vector<uint8_t> out(static_cast<size_t>(w) * static_cast<size_t>(h), 0);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            double acc = 0.0;
            for (int ky = -r; ky <= r; ++ky) {
                for (int kx = -r; kx <= r; ++kx) {
                    const float wgt = kernel[static_cast<size_t>(ky + r) * static_cast<size_t>(k) + static_cast<size_t>(kx + r)];
                    acc += wgt * static_cast<double>(atClamp(img, w, h, x + kx, y + ky));
                }
            }
            int v = static_cast<int>(std::lround(acc));
            v = std::clamp(v, 0, 255);
            out[static_cast<size_t>(y) * static_cast<size_t>(w) + static_cast<size_t>(x)] = static_cast<uint8_t>(v);
        }
    }
    return out;
}

std::vector<float> boxKernel(int k) {
    const int n = k * k;
    const float v = 1.0f / static_cast<float>(n);
    return std::vector<float>(static_cast<size_t>(n), v);
}

std::vector<float> gaussianKernel(int k, float sigma) {
    const int r = k / 2;
    std::vector<float> ker(static_cast<size_t>(k) * static_cast<size_t>(k), 0.0f);

    const float s2 = sigma * sigma;
    float sum = 0.0f;

    for (int y = -r; y <= r; ++y) {
        for (int x = -r; x <= r; ++x) {
            const float e = std::exp(-(x * x + y * y) / (2.0f * s2));
            ker[static_cast<size_t>(y + r) * static_cast<size_t>(k) + static_cast<size_t>(x + r)] = e;
            sum += e;
        }
    }

    // normalize
    for (auto& v : ker) v /= sum;
    return ker;
}

}
