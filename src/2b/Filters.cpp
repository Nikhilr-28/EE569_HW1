#include "Filters.h"
#include <cmath>
#include <algorithm>

namespace filters {

uint8_t atClamp(const std::vector<uint8_t>& img, int w, int h, int x, int y) {
    x = std::clamp(x, 0, w - 1);
    y = std::clamp(y, 0, h - 1);
    return img[y * w + x];
}

std::vector<uint8_t> bilateralFilter(
    const std::vector<uint8_t>& img,
    int w, int h,
    int k,
    double sigma_c,
    double sigma_s
) {
    int r = k / 2;
    std::vector<uint8_t> out(w * h, 0);

    const double inv2sc2 = 1.0 / (2.0 * sigma_c * sigma_c);
    const double inv2ss2 = 1.0 / (2.0 * sigma_s * sigma_s);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {

            double norm = 0.0;
            double sum  = 0.0;
            const double center = img[y * w + x];

            for (int dy = -r; dy <= r; ++dy) {
                for (int dx = -r; dx <= r; ++dx) {

                    int nx = x + dx;
                    int ny = y + dy;
                    uint8_t val = atClamp(img, w, h, nx, ny);

                    double spatial = std::exp(-(dx*dx + dy*dy) * inv2sc2);
                    double range   = std::exp(-((val - center)*(val - center)) * inv2ss2);
                    double weight  = spatial * range;

                    sum  += weight * val;
                    norm += weight;
                }
            }

            int outVal = static_cast<int>(std::round(sum / norm));
            out[y * w + x] = static_cast<uint8_t>(std::clamp(outVal, 0, 255));
        }
    }

    return out;
}

}
