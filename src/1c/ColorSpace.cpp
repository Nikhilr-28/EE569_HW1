/*
Name: Nikhil Ravichandran
ID: 8515860149
Email: nikhilr@usc.edu
Date: February 1, 2026
*/
#include "ColorSpace.h"
#include <algorithm>
#include <cmath>

namespace cs {

uint8_t clamp_u8(float x) {
    if (x < 0.0f) x = 0.0f;
    if (x > 255.0f) x = 255.0f;
    return static_cast<uint8_t>(std::lround(x));
}

// RGB -> YUV (BT.601-like coefficients used in EE569 appendix)
void rgb_to_yuv_bt601(const std::vector<uint8_t>& rgb, int width, int height,
                      std::vector<float>& Y,
                      std::vector<float>& U,
                      std::vector<float>& V)
{
    const size_t N = static_cast<size_t>(width) * height;
    Y.assign(N, 0.0f);
    U.assign(N, 0.0f);
    V.assign(N, 0.0f);

    for (size_t p = 0; p < N; p++) {
        const float R = rgb[p * 3 + 0];
        const float G = rgb[p * 3 + 1];
        const float B = rgb[p * 3 + 2];

        // Given in assignment appendix:
        // Y = 0.257R + 0.504G + 0.098B + 16
        // U = -0.148R - 0.291G + 0.439B + 128
        // V = 0.439R - 0.368G - 0.071B + 128
        Y[p] = 0.257f * R + 0.504f * G + 0.098f * B + 16.0f;
        U[p] = -0.148f * R - 0.291f * G + 0.439f * B + 128.0f;
        V[p] = 0.439f * R - 0.368f * G - 0.071f * B + 128.0f;
    }
}

// YUV -> RGB (inverse equations from assignment appendix)
std::vector<uint8_t> yuv_to_rgb_bt601(const std::vector<float>& Y,
                                      const std::vector<float>& U,
                                      const std::vector<float>& V,
                                      int width, int height)
{
    const size_t N = static_cast<size_t>(width) * height;
    std::vector<uint8_t> rgb(N * 3, 0);

    for (size_t p = 0; p < N; p++) {
        const float y = Y[p];
        const float u = U[p];
        const float v = V[p];

        // Given in assignment appendix:
        // R = 1.164(Y-16) + 1.596(V-128)
        // G = 1.164(Y-16) - 0.813(V-128) - 0.391(U-128)
        // B = 1.164(Y-16) + 2.018(U-128)
        const float Yp = (y - 16.0f);
        const float Up = (u - 128.0f);
        const float Vp = (v - 128.0f);

        const float R = 1.164f * Yp + 1.596f * Vp;
        const float G = 1.164f * Yp - 0.813f * Vp - 0.391f * Up;
        const float B = 1.164f * Yp + 2.018f * Up;

        rgb[p * 3 + 0] = clamp_u8(R);
        rgb[p * 3 + 1] = clamp_u8(G);
        rgb[p * 3 + 2] = clamp_u8(B);
    }

    return rgb;
}

} // namespace cs
