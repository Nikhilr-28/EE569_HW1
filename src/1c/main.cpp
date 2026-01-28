#include <iostream>
#include <string>
#include <vector>
#include <cstdint>

#include "RawIO.h"
#include "ColorSpace.h"
#include "HistEq.h"
#include "Clahe.h"

static std::vector<uint8_t> Y_float_to_u8(const std::vector<float>& Y) {
    std::vector<uint8_t> out(Y.size(), 0);
    for (size_t i = 0; i < Y.size(); i++) out[i] = cs::clamp_u8(Y[i]);
    return out;
}

static void Y_u8_to_float(const std::vector<uint8_t>& Yu8, std::vector<float>& Y) {
    if (Y.size() != Yu8.size()) Y.resize(Yu8.size());
    for (size_t i = 0; i < Yu8.size(); i++) Y[i] = (float)Yu8[i];
}

int main() {
    const int width = 1620;
    const int height = 1080;

    const std::string in_path = "A:\\DIP\\HW_1\\EE569_HW1\\images\\towers.raw";
    const std::string out_dir = "A:\\DIP\\HW_1\\EE569_HW1\\images\\1c\\";

    const std::string out_roundtrip = out_dir + "towers_roundtrip.raw";
    const std::string out_A = out_dir + "towers_Y_HE_A.raw";
    const std::string out_B = out_dir + "towers_Y_HE_B.raw";
    const std::string out_C = out_dir + "towers_Y_CLAHE.raw";

    std::vector<uint8_t> rgb;
    if (!rawio::read_raw_u8(in_path, width, height, 3, rgb)) return 1;

    // Step 1: RGB -> YUV
    std::vector<float> Y, U, V;
    cs::rgb_to_yuv_bt601(rgb, width, height, Y, U, V);

    // ---- Roundtrip sanity test (RGB->YUV->RGB without changing Y) ----
    {
        auto rgb_rt = cs::yuv_to_rgb_bt601(Y, U, V, width, height);
        if (!rawio::write_raw_u8(out_roundtrip, rgb_rt)) return 1;
        std::cout << "Wrote: " << out_roundtrip << "\n";
    }

    // Prepare Y as 8-bit image for equalization/CLAHE
    const auto Y_u8 = Y_float_to_u8(Y);

    // ---- Step 2 + 3 using Method A on Y ----
    {
        auto Yp = he::equalize_A(Y_u8);
        std::vector<float> Y_new = Y;
        Y_u8_to_float(Yp, Y_new);

        auto rgb_out = cs::yuv_to_rgb_bt601(Y_new, U, V, width, height);
        if (!rawio::write_raw_u8(out_A, rgb_out)) return 1;
        std::cout << "Wrote: " << out_A << "\n";
    }

    // ---- Step 2 + 3 using Method B on Y ----
    {
        auto Yp = he::equalize_B(Y_u8);
        std::vector<float> Y_new = Y;
        Y_u8_to_float(Yp, Y_new);

        auto rgb_out = cs::yuv_to_rgb_bt601(Y_new, U, V, width, height);
        if (!rawio::write_raw_u8(out_B, rgb_out)) return 1;
        std::cout << "Wrote: " << out_B << "\n";
    }

    // ---- Step 2 + 3 using CLAHE on Y ----
    // Tune these two (hyperparams):
    // tiles: try 8x8, 12x8, 16x12 etc.
    // clip_limit: try 2.0, 3.0, 4.0
    {
        const int tiles_x = 8;
        const int tiles_y = 6;
        const double clip_limit = 3.5;

        auto Yp = clahe::apply(Y_u8, width, height, tiles_x, tiles_y, clip_limit);

        std::vector<float> Y_new = Y;
        Y_u8_to_float(Yp, Y_new);

        auto rgb_out = cs::yuv_to_rgb_bt601(Y_new, U, V, width, height);
        if (!rawio::write_raw_u8(out_C, rgb_out)) return 1;
        std::cout << "Wrote: " << out_C << " (tiles=" << tiles_x << "x" << tiles_y
                  << ", clip=" << clip_limit << ")\n";
    }

    std::cout << "\nDone.\n";
    std::cout << "Open outputs in ImageJ as RAW: 1620x1080, 24-bit RGB (interleaved).\n";
    return 0;
}
