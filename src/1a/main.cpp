/*
Name: Nikhil Ravichandran
ID: 8515860149
Email: nikhilr@usc.edu
Date: February 1, 2026
*/
#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <cstdint>
#include <algorithm>

#include "RawIO.h"
#include "Demosaic.h"

static double compute_mse_u8(const std::vector<uint8_t>& a,
                             const std::vector<uint8_t>& b)
{
    if (a.size() != b.size() || a.empty()) return -1.0;

    long double sum = 0.0L;
    for (size_t i = 0; i < a.size(); i++) {
        const long double d = (long double)a[i] - (long double)b[i];
        sum += d * d;
    }
    return (double)(sum / (long double)a.size());
}

static double compute_psnr_from_mse(double mse, double max_val = 255.0)
{
    if (mse <= 0.0) return INFINITY;
    return 10.0 * std::log10((max_val * max_val) / mse);
}

static std::vector<uint8_t> abs_diff_u8(const std::vector<uint8_t>& a,
                                        const std::vector<uint8_t>& b)
{
    std::vector<uint8_t> diff;
    if (a.size() != b.size()) return diff;

    diff.resize(a.size());
    for (size_t i = 0; i < a.size(); i++) {
        int d = (int)a[i] - (int)b[i];
        if (d < 0) d = -d;
        diff[i] = (uint8_t)d;
    }
    return diff;
}

int main() {
    // 1(a) sailboats CFA is 512x768, 8-bit single channel
    const int width = 512;
    const int height = 768;

    const std::string input_cfa_path   = "A:\\DIP\\HW_1\\EE569_HW1\\images\\sailboats_cfa.raw";
    const std::string ref_rgb_path     = "A:\\DIP\\HW_1\\EE569_HW1\\images\\sailboats.raw";

    const std::string output_rgb_path  = "A:\\DIP\\HW_1\\EE569_HW1\\images\\1a\\sailboats_bilinear.raw";
    const std::string output_diff_path = "A:\\DIP\\HW_1\\EE569_HW1\\images\\1a\\sailboats_diff_abs.raw";

    // --- Step 1: Read CFA ---
    std::vector<uint8_t> cfa;
    if (!rawio::read_raw_u8(input_cfa_path, width, height, 1, cfa)) {
        return 1;
    }

    // --- Step 2: Demosaic ---
    std::vector<uint8_t> rgb = demosaic_bilinear_grbg(cfa, width, height);
    if (rgb.empty()) {
        std::cerr << "ERROR: demosaicing failed.\n";
        return 1;
    }

    if (!rawio::write_raw_u8(output_rgb_path, rgb)) {
        return 1;
    }

    std::cout << "Wrote demosaiced: " << output_rgb_path << "\n";

    // --- Step 3: Read reference RGB ---
    std::vector<uint8_t> ref;
    if (!rawio::read_raw_u8(ref_rgb_path, width, height, 3, ref)) {
        std::cerr << "ERROR: Could not read reference sailboats.raw.\n";
        return 1;
    }

    // --- Step 4: Compute error metrics ---
    // Overall MSE/PSNR across all channels
    const double mse_all = compute_mse_u8(rgb, ref);
    const double psnr_all = compute_psnr_from_mse(mse_all);

    // Per-channel MSE/PSNR
    auto channel_mse_psnr = [&](int ch) {
        long double sum = 0.0L;
        const size_t npx = (size_t)width * height;
        for (size_t p = 0; p < npx; p++) {
            const int i = (int)(p * 3 + ch);
            const long double d = (long double)rgb[i] - (long double)ref[i];
            sum += d * d;
        }
        const double mse = (double)(sum / (long double)npx);
        const double psnr = compute_psnr_from_mse(mse);
        return std::pair<double,double>(mse, psnr);
    };

    const auto [mse_r, psnr_r] = channel_mse_psnr(0);
    const auto [mse_g, psnr_g] = channel_mse_psnr(1);
    const auto [mse_b, psnr_b] = channel_mse_psnr(2);

    std::cout << "\n=== Comparison vs sailboats.raw (reference) ===\n";
    std::cout << "Overall MSE  : " << mse_all << "\n";
    std::cout << "Overall PSNR : " << psnr_all << " dB\n\n";
    std::cout << "R channel MSE  : " << mse_r << " | PSNR: " << psnr_r << " dB\n";
    std::cout << "G channel MSE  : " << mse_g << " | PSNR: " << psnr_g << " dB\n";
    std::cout << "B channel MSE  : " << mse_b << " | PSNR: " << psnr_b << " dB\n";

    // --- Step 5: Write absolute difference image (visual artifact map) ---
    std::vector<uint8_t> diff_abs = abs_diff_u8(rgb, ref);
    if (diff_abs.empty()) {
        std::cerr << "ERROR: diff computation failed.\n";
        return 1;
    }

    if (!rawio::write_raw_u8(output_diff_path, diff_abs)) {
        return 1;
    }

    std::cout << "\nWrote abs diff image: " << output_diff_path << "\n";
    std::cout << "Open diff in ImageJ as RAW (512x768, RGB, interleaved).\n";

    return 0;
}
