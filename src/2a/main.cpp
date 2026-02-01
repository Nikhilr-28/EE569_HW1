#include "RawIO.h"
#include "Filters.h"
#include "Metrics.h"

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

int main(int argc, char** argv) {
    // Defaults (change if needed). You can also pass W H on command line.
    int W = 768;
    int H = 512;
    if (argc >= 3) {
        W = std::stoi(argv[1]);
        H = std::stoi(argv[2]);
    }

    // Paths based on your structure
    const std::string origPath  = "images/flower_gray.raw";
    const std::string noisyPath = "images/flower_gray_noisy.raw";
    const std::string outDir    = "images/2a";

    std::filesystem::create_directories(outDir);

    std::vector<uint8_t> orig, noisy;
    if (!rawio::readRaw8(origPath, W, H, orig)) {
        std::cerr << "Failed to read original: " << origPath << "\n";
        return 1;
    }
    if (!rawio::readRaw8(noisyPath, W, H, noisy)) {
        std::cerr << "Failed to read noisy: " << noisyPath << "\n";
        return 1;
    }

    // Baseline PSNR (noisy vs original)
    const double mseNoisy = metrics::mse8(orig, noisy);
    const double psnrNoisy = metrics::psnrFromMSE(mseNoisy);
    std::cout << "Baseline (noisy vs original): MSE=" << mseNoisy << " PSNR=" << psnrNoisy << " dB\n\n";

    // Window sizes to test
    const std::vector<int> ks = {3, 5, 7};

    // A simple sigma rule for Gaussian: sigma ~= k/3 (common heuristic)
    auto sigmaForK = [](int k) {
        return static_cast<float>(k) / 3.0f;
    };

    for (int k : ks) {
        // Box filter
        {
            auto ker = filters::boxKernel(k);
            auto den = filters::convolve(noisy, W, H, ker, k);

            const double mse = metrics::mse8(orig, den);
            const double psnr = metrics::psnrFromMSE(mse);

            const std::string outPath = outDir + "/flower_box_k" + std::to_string(k) + ".raw";
            rawio::writeRaw8(outPath, den);

            std::cout << "[BOX]     k=" << k << "  MSE=" << mse << "  PSNR=" << psnr << " dB  -> " << outPath << "\n";
        }

        // Gaussian filter
        {
            const float sigma = sigmaForK(k);
            auto ker = filters::gaussianKernel(k, sigma);
            auto den = filters::convolve(noisy, W, H, ker, k);

            const double mse = metrics::mse8(orig, den);
            const double psnr = metrics::psnrFromMSE(mse);

            const std::string outPath =
                outDir + "/flower_gauss_k" + std::to_string(k) + "_s" + std::to_string(sigma) + ".raw";
            rawio::writeRaw8(outPath, den);

            std::cout << "[GAUSSIAN] k=" << k << " sigma=" << sigma
                      << "  MSE=" << mse << "  PSNR=" << psnr << " dB  -> " << outPath << "\n";
        }
    }

    std::cout << "\nDone.\n";
    return 0;
}
