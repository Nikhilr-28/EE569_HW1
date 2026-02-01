#include "RawIO.h"
#include "Filters.h"
#include "Metrics.h"

#include <iostream>
#include <vector>
#include <filesystem>
#include <cstdint>

int main() {
    const int W = 768;
    const int H = 512;

    const std::string baseImgDir = "A:/DIP/HW_1/EE569_HW1/images";
    const std::string origPath   = baseImgDir + "/flower_gray.raw";
    const std::string noisyPath  = baseImgDir + "/flower_gray_noisy.raw";
    const std::string outDir     = baseImgDir + "/2b";

    std::filesystem::create_directories(outDir);

    std::vector<uint8_t> orig, noisy;

    if (!rawio::readRaw8(origPath, W, H, orig)) {
        std::cerr << "Failed to read original image\n";
        return 1;
    }
    if (!rawio::readRaw8(noisyPath, W, H, noisy)) {
        std::cerr << "Failed to read noisy image\n";
        return 1;
    }

    // Baseline metrics
    double mse_base  = metrics::mse8(orig, noisy);
    double psnr_base = metrics::psnrFromMSE(mse_base);

    std::cout << "Baseline (Noisy vs Original)\n";
    std::cout << "MSE  = " << mse_base << "\n";
    std::cout << "PSNR = " << psnr_base << " dB\n\n";

    // Bilateral parameters
    const int k = 5;
    const std::vector<double> sigma_c_vals = {1.0, 2.0, 3.0};
    const std::vector<double> sigma_s_vals = {30.0, 50.0, 80.0, 120.0};

    for (double sigma_c : sigma_c_vals) {
        for (double sigma_s : sigma_s_vals) {

            auto den = filters::bilateralFilter(noisy, W, H, k, sigma_c, sigma_s);

            double mse  = metrics::mse8(orig, den);
            double psnr = metrics::psnrFromMSE(mse);

            std::string outPath =
                outDir + "/flower_bilateral_k5_sc" +
                std::to_string(sigma_c) + "_ss" +
                std::to_string((int)sigma_s) + ".raw";

            rawio::writeRaw8(outPath, den);

            std::cout << "[BILATERAL]\n";
            std::cout << "k = " << k
                      << "  sigma_c = " << sigma_c
                      << "  sigma_s = " << sigma_s << "\n";
            std::cout << "MSE  = " << mse << "\n";
            std::cout << "PSNR = " << psnr << " dB\n";
            std::cout << "Output -> " << outPath << "\n\n";
        }
    }

    std::cout << "Done.\n";
    return 0;
}
